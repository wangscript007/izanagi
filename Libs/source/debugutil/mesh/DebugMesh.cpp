#include "debugutil/mesh/DebugMesh.h"
#include "debugutil/mesh/DebugMeshAxis.h"
#include "math/MathVector4.h"

using namespace izanagi;

CDebugMesh::SetElemFunc CDebugMesh::SetElemFuncTbl[] = {
    &CDebugMesh::SetVtxElementPos,
    &CDebugMesh::SetVtxElementNormal,
    &CDebugMesh::SetVtxElementColor,
    &CDebugMesh::SetVtxElementUV,
    &CDebugMesh::SetVtxElementTangent,
};

CDebugMesh::SetVtxFunc CDebugMesh::SetVtxFuncTbl[] = {
    &CDebugMesh::SetVtxPos,
    &CDebugMesh::SetVtxNormal,
    &CDebugMesh::SetVtxColor,
    &CDebugMesh::SetVtxUV,
    &CDebugMesh::SetVtxTangent,
};

CDebugMesh::GetElemSizeFunc CDebugMesh::GetElemSizeFuncTbl[] = {
    &CDebugMesh::GetPosSize,
    &CDebugMesh::GetColorSize,
    &CDebugMesh::GetNormalSize,
    &CDebugMesh::GetUVSize,
    &CDebugMesh::GetTangentSize,
};

/**
* コンストラクタ
*/
CDebugMesh::CDebugMesh()
{
    m_Allocator = IZ_NULL;
    
    m_pVB = IZ_NULL;
    m_pIB = IZ_NULL;
    m_pVD = IZ_NULL;

    m_PrimType = graph::E_GRAPH_PRIM_TYPE_TRIANGLESTRIP;
    m_nPrimCnt = 0;

    m_pVtx = IZ_NULL;
    m_pFace = IZ_NULL;

    m_nVtxFormFlag = 0;

    m_pDebugAxis = IZ_NULL;
}

/**
* デストラクタ
*/
CDebugMesh::~CDebugMesh()
{
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pIB);
    SAFE_RELEASE(m_pVD);

    ClearDataBuffer();

    SAFE_RELEASE(m_pDebugAxis);
}

/**
* 描画
*/
IZ_BOOL CDebugMesh::Draw(
    graph::CGraphicsDevice* device,
    IZ_BOOL bEnableDrawDebugAxis/*= IZ_FALSE*/)
{
    IZ_ASSERT(device != IZ_NULL);
    IZ_ASSERT(m_pVB != IZ_NULL);
    IZ_ASSERT(m_pVD != IZ_NULL);

    // 頂点バッファ
    device->SetVertexBuffer(
        0,
        0,
        m_pVB->GetStride(),
        m_pVB);

    // 頂点宣言
    device->SetVertexDeclaration(m_pVD);

    // インデックスバッファ
    if (m_pIB != IZ_NULL) {
        device->SetIndexBuffer(m_pIB);
    }

    // 描画
    if (m_pIB != IZ_NULL) {
        device->DrawIndexedPrimitive(
            m_PrimType,
            0,
            m_pVB->GetVtxNum(),
            0,
            m_nPrimCnt);
    }
    else {
        device->DrawPrimitive(
            m_PrimType,
            0,
            m_nPrimCnt);
    }

    if (bEnableDrawDebugAxis) {
        if (m_pDebugAxis != IZ_NULL) {
            m_pDebugAxis->Draw(device);
        }
    }

    return IZ_TRUE;
}

graph::CVertexBuffer* CDebugMesh::GetVB()
{
    return m_pVB;
}

graph::CVertexDeclaration* CDebugMesh::GetVD()
{
    return m_pVD;
}

graph::CIndexBuffer* CDebugMesh::GetIB()
{
    return m_pIB;
}

IZ_UINT CDebugMesh::GetPrimitiveCount()
{
    return m_nPrimCnt;
}

graph::E_GRAPH_PRIM_TYPE CDebugMesh::GetPrimitiveType()
{
    return m_PrimType;
}

// 頂点バッファ作成
IZ_BOOL CDebugMesh::CreateVB(
    graph::CGraphicsDevice* device,
    IZ_UINT flag,
    IZ_UINT nVtxNum)
{
    IZ_C_ASSERT(COUNTOF(GetElemSizeFuncTbl) == E_DEBUG_MESH_VTX_FORM_NUM);

    IZ_UINT nStride = 0;
    for (IZ_UINT i = 0; i < COUNTOF(GetElemSizeFuncTbl); ++i) {
        nStride += (*GetElemSizeFuncTbl[i])(flag);
    }

    m_pVB = device->CreateVertexBuffer(
                nStride,
                nVtxNum,
                graph::E_GRAPH_RSC_USAGE_STATIC);

    return (m_pVB != IZ_NULL);
}

// インデックスバッファ作成
IZ_BOOL CDebugMesh::CreateIB(
    graph::CGraphicsDevice* device,
    IZ_UINT nIdxNum,
    graph::E_GRAPH_INDEX_BUFFER_FMT fmt)
{
    m_pIB = device->CreateIndexBuffer(
                nIdxNum,
                fmt,
                graph::E_GRAPH_RSC_USAGE_STATIC);

    return (m_pIB != IZ_NULL);
}

// 頂点宣言作成
IZ_BOOL CDebugMesh::CreateVD(
    graph::CGraphicsDevice* device,
    IZ_UINT flag)
{
    IZ_C_ASSERT(COUNTOF(SetElemFuncTbl) == E_DEBUG_MESH_VTX_FORM_NUM);

    // +1はENDの分
    graph::SVertexElement VtxElement[E_DEBUG_MESH_VTX_FORM_NUM + 1];
    FILL_ZERO(VtxElement, sizeof(VtxElement));

    IZ_WORD nOffset = 0;
    IZ_UINT pos = 0;

    for (IZ_UINT i = 0; i < COUNTOF(SetElemFuncTbl); ++i) {
        pos = (*SetElemFuncTbl[i])(flag, VtxElement, pos, &nOffset);
    }

    m_pVD = device->CreateVertexDeclaration(VtxElement, pos);

    return (m_pVD != IZ_NULL);
}

// データバッファ作成
IZ_BOOL CDebugMesh::CreateDataBuffer(
    IZ_UINT nVtxNum,
    IZ_UINT nIdxNum)
{
    IZ_ASSERT(m_nPrimCnt > 0);

    m_pVtx = (SMeshVtx*)ALLOC_ZERO(m_Allocator, sizeof(SMeshVtx) * nVtxNum);
    VRETURN_NULL(m_pVtx);

    m_pFace = (SMeshFace*)ALLOC_ZERO(m_Allocator, sizeof(SMeshFace) * m_nPrimCnt);
    VRETURN_NULL(m_pFace);

    return IZ_TRUE;
}

// デバッグ用軸メッシュ作成
IZ_BOOL CDebugMesh::CreateDebugAxis(
    graph::CGraphicsDevice* device,
    IZ_UINT nVtxNum,
    IZ_UINT flag)
{
    IZ_ASSERT(nVtxNum > 0);

    IZ_UINT nAxisFlag = 0;
    nAxisFlag |= (IsNormal(flag) ? E_DEBUG_MESH_AXIS_Z : 0);
    nAxisFlag |= (IsTangent(flag) ? E_DEBUG_MESH_AXIS_X | E_DEBUG_MESH_AXIS_Y : 0);

    m_pDebugAxis = CDebugMeshAxis::CreateDebugMeshAxis(
                    m_Allocator,
                    device,
                    nAxisFlag,
                    nVtxNum);
    IZ_ASSERT(m_pDebugAxis != IZ_NULL);

    return (m_pDebugAxis != IZ_NULL);
}

// データバッファクリア
void CDebugMesh::ClearDataBuffer()
{
    FREE(m_Allocator, m_pVtx);
    FREE(m_Allocator, m_pFace);
}

// データをVB、IBにコピーする
IZ_BOOL CDebugMesh::CopyDataToBuffer(IZ_UINT flag)
{
    IZ_ASSERT(m_pVtx != IZ_NULL);
    IZ_ASSERT(m_pFace != IZ_NULL);

    IZ_BOOL ret = IZ_TRUE;

    IZ_UINT8* pVtxData = IZ_NULL;
    IZ_UINT32* pIdxData = IZ_NULL;

    // インデックス
    VGOTO(ret = LockIB((void**)&pIdxData), __EXIT__);
    {
        for (IZ_UINT i = 0; i < m_nPrimCnt; ++i) {
#ifdef IZ_COORD_LEFT_HAND
            *pIdxData++ = m_pFace[i].idx[0];
            *pIdxData++ = m_pFace[i].idx[2];
            *pIdxData++ = m_pFace[i].idx[1];
#else
            *pIdxData++ = m_pFace[i].idx[0];
            *pIdxData++ = m_pFace[i].idx[1];
            *pIdxData++ = m_pFace[i].idx[2];
#endif

            // 面ごとの計算
            ComputeFace(&m_pFace[i], flag);
        }
    }
    VGOTO(ret = UnlockIB(), __EXIT__);

    // 頂点
    VGOTO(ret = LockVB((void**)&pVtxData), __EXIT__);
    {
        IZ_UINT nVtxNum = GetVtxNum();

        for (IZ_UINT i = 0; i < nVtxNum; ++i) {
            ApplyOverlap(
                &m_pVtx[i],
                flag);

            pVtxData = SetVtxData(
                        m_pVtx[i],
                        flag,
                        pVtxData);
        }
    }
    VGOTO(ret = UnlockVB(), __EXIT__);

__EXIT__:
    // もういらない
    ClearDataBuffer();

    return ret;
}

IZ_BOOL CDebugMesh::LockVB(void** p)
{
    IZ_ASSERT(p != IZ_NULL);

    VRETURN(m_pVB->Lock(0, 0, p, IZ_FALSE));

    if (m_pDebugAxis != IZ_NULL) {
        VRETURN(m_pDebugAxis->BeginRegister());
    }

    return IZ_TRUE;
}

IZ_BOOL CDebugMesh::UnlockVB()
{
    VRETURN(m_pVB->Unlock());
    
    if (m_pDebugAxis != IZ_NULL) {
        VRETURN(m_pDebugAxis->EndRegister());
    }

    return IZ_TRUE;
}

IZ_BOOL CDebugMesh::LockIB(void** p)
{
    IZ_ASSERT(p != IZ_NULL);

    VRETURN(m_pIB->Lock(0, 0, p, IZ_FALSE));
    return IZ_TRUE;
}

IZ_BOOL CDebugMesh::UnlockIB()
{
    return m_pIB->Unlock();
}

// 頂点データセット
IZ_UINT8* CDebugMesh::SetVtxData(
    const SMeshFace& sFace,
    IZ_UINT flag,
    IZ_UINT8* pData)
{
    for (IZ_UINT i = 0; i < SMeshFace::VTX_NUM; ++i) {
        SMeshVtx* pVtx = GetVtx(sFace.idx[i]);

        pData = SetVtxData(
                    *pVtx,
                    flag,
                    pData);
    }

    return pData;
}

// 頂点データセット
IZ_UINT8* CDebugMesh::SetVtxData(
    const SMeshVtx& sVtx,
    IZ_UINT flag,
    IZ_UINT8* pData)
{
    IZ_C_ASSERT(COUNTOF(SetVtxFuncTbl) == E_DEBUG_MESH_VTX_FORM_NUM);

    for (IZ_UINT i = 0; i < COUNTOF(SetVtxFuncTbl); ++i) {
        pData = (*SetVtxFuncTbl[i])(flag, sVtx, pData);
    }

    if (m_pDebugAxis != IZ_NULL) {
        if (IsNormal(flag)) {
            m_pDebugAxis->SetVtx(
                E_DEBUG_MESH_AXIS_Z,
                sVtx.pos,
                sVtx.nml);
        }

        if (IsTangent(flag)) {
            // Tangent
            m_pDebugAxis->SetVtx(
                E_DEBUG_MESH_AXIS_X,
                sVtx.pos,
                sVtx.tangent);

            // BiNormal
            m_pDebugAxis->SetVtx(
                E_DEBUG_MESH_AXIS_Y,
                sVtx.pos,
                sVtx.binml);
        }
    }
    
    return pData;
}

// 頂点と面を関連付ける
IZ_BOOL CDebugMesh::BindFaceToVtx(SMeshFace* pFace)
{
    for (IZ_UINT i = 0; i < SMeshFace::VTX_NUM; ++i) {
        SMeshVtx* pVtx = GetVtx(pFace->idx[i]);
        VRETURN_NULL(pVtx);

        VRETURN(pVtx->AddFace(pFace));
    }

    return IZ_TRUE;
}

// 面ごとの計算
void CDebugMesh::ComputeFace(
    SMeshFace* pFace,
    IZ_UINT flag)
{
    if (IsTangent(flag)) {
#ifdef IZ_COORD_LEFT_HAND
        SMeshVtx* vtx0 = GetVtx(pFace->idx[0]);
        SMeshVtx* vtx1 = GetVtx(pFace->idx[2]);
        SMeshVtx* vtx2 = GetVtx(pFace->idx[1]);
#else
        SMeshVtx* vtx0 = GetVtx(pFace->idx[0]);
        SMeshVtx* vtx1 = GetVtx(pFace->idx[1]);
        SMeshVtx* vtx2 = GetVtx(pFace->idx[2]);
#endif

        // Tangent
        ComputeTangent(vtx0, *vtx1, *vtx2);
        ComputeTangent(vtx1, *vtx2, *vtx0);
        ComputeTangent(vtx2, *vtx0, *vtx1);
    }
}

void CDebugMesh::ComputeTangent(
    SMeshVtx* vtx0,
    const SMeshVtx& vtx1,
    const SMeshVtx& vtx2)
{
    // NOTE
    // DirectXのuv座標系で考えると
    //
    // +---> u(x)
    // |
    // v(y)
    //
    // 手前がzとなるので、これは左手座標となる

    math::SVector4 vP;
    math::SVector4::Sub(vP, vtx1.pos, vtx0->pos);

    math::SVector4 vQ;
    math::SVector4::Sub(vQ, vtx2.pos, vtx0->pos);
    
    IZ_FLOAT s1 = vtx1.uv[0] - vtx0->uv[0];
    IZ_FLOAT t1 = vtx1.uv[1] - vtx0->uv[1];

    IZ_FLOAT s2 = vtx2.uv[0] - vtx0->uv[0];
    IZ_FLOAT t2 = vtx2.uv[1] - vtx0->uv[1];

    // 行列式の逆数
    IZ_FLOAT fInvDeterminant = 1.0f / (s1 * t2 - s2 * t1);

    // BiNormal
    math::SVector4 vB;
    {
        vB.x = fInvDeterminant * (-s2 * vP.x + s1 * vQ.x);
        vB.y = fInvDeterminant * (-s2 * vP.y + s1 * vQ.y);
        vB.z = fInvDeterminant * (-s2 * vP.z + s1 * vQ.z);
        math::SVector4::Normalize(vB, vB);
    }

    // Tangent
    math::SVector4 vT;
    {
#if 0
        vT.x = fInvDeterminant * (t2 * vP.x - t1 * vQ.x);
        vT.y = fInvDeterminant * (t2 * vP.y - t1 * vQ.y);
        vT.z = fInvDeterminant * (t2 * vP.z - t1 * vQ.z);
        math::SVector4::Normalize(vT, vT);
#else
#ifdef IZ_COORD_LEFT_HAND
        // X = Y x Z
        izanagi::math::SVector4::Cross(vT, vB, vtx0->nml);

        // Y = Z x X
        izanagi::math::SVector4::Cross(vB, vtx0->nml, vT);
#else
        // NOTE
        // ここでは、Tangent空間を左手座標にしたい
        // 左手座標にするために、あえて反対に計算する

        // X = Y x Z
        izanagi::math::SVector4::Cross(vT, vtx0->nml, vB);

        // Y = Z x X
        izanagi::math::SVector4::Cross(vB, vT, vtx0->nml);
#endif
#endif
    }

    math::SMatrix mtx;
    mtx.v[0].Set(vT.x, vB.x, vtx0->nml.x, 0.0f);
    mtx.v[1].Set(vT.y, vB.y, vtx0->nml.y, 0.0f);
    mtx.v[2].Set(vT.z, vB.z, vtx0->nml.z, 0.0f);
    mtx.v[3].Set(0.0f, 0.0f,        0.0f, 1.0f);

    IZ_FLOAT determinant = math::SMatrix::Determinant(mtx);

    vT.w = determinant;

    //math::SVector4::Scale(vB, vB, determinant);

    math::SVector4::Copy(vtx0->tangent, vT);
    math::SVector4::Copy(vtx0->binml, vB);
}

void CDebugMesh::SetOverlapVtx(
    IZ_UINT idx_0,
    IZ_UINT idx_1)
{
    SMeshVtx* pVtx_0 = GetVtx(idx_0);
    SMeshVtx* pVtx_1 = GetVtx(idx_1);

    pVtx_0->SetOverlapIdx(idx_1);
    pVtx_1->SetOverlapIdx(idx_0);
}

void CDebugMesh::ApplyOverlap(
    SMeshVtx* pVtx,
    IZ_UINT flag)
{
#if 0
    if (pVtx->HasOverlapIdx()) {
        IZ_UINT idx = pVtx->GetOverlapIdx();
        SMeshVtx* pOverlap = GetVtx(idx);

        if (IsTangent(flag)) {
            // Tangent
            math::SVector4::Add(pVtx->tangent, pVtx->tangent, pOverlap->tangent);
            math::SVector4::Scale(pVtx->tangent, pVtx->tangent, 0.5f);
            math::SVector4::Normalize(pVtx->tangent, pVtx->tangent);

            math::SVector4::Copy(pOverlap->tangent, pVtx->tangent);

            // BiNormal
            math::SVector4::Add(pVtx->binml, pVtx->binml, pOverlap->binml);
            math::SVector4::Scale(pVtx->binml, pVtx->binml, 0.5f);
            math::SVector4::Normalize(pVtx->binml, pVtx->binml);

            math::SVector4::Copy(pOverlap->binml, pVtx->binml);
        }
    }
#endif
}

//////////////////////////////////////////////////////////////

// 位置
IZ_UINT CDebugMesh::SetVtxElementPos(IZ_UINT flag, graph::SVertexElement* pElem, IZ_UINT nPos, IZ_WORD* pOffset)
{
    if (IsPos(flag)) {
        pElem[nPos].Offset = *pOffset;
        pElem[nPos].Type = graph::E_GRAPH_VTX_DECL_TYPE_FLOAT4;
        pElem[nPos].Usage = graph::E_GRAPH_VTX_DECL_USAGE_POSITION;
        *pOffset += GetPosSize(flag);
        return nPos + 1;
    }
    return nPos;
}

// 法線
IZ_UINT CDebugMesh::SetVtxElementNormal(IZ_UINT flag, graph::SVertexElement* pElem, IZ_UINT nPos, IZ_WORD* pOffset)
{
    if (IsNormal(flag)) {
        pElem[nPos].Offset = *pOffset;
        pElem[nPos].Type = graph::E_GRAPH_VTX_DECL_TYPE_FLOAT3;
        pElem[nPos].Usage = graph::E_GRAPH_VTX_DECL_USAGE_NORMAL;
        *pOffset += GetNormalSize(flag);
        return nPos + 1;
    }
    return nPos;
}

// 頂点カラー
IZ_UINT CDebugMesh::SetVtxElementColor(IZ_UINT flag, graph::SVertexElement* pElem, IZ_UINT nPos, IZ_WORD* pOffset)
{
    if (IsColor(flag)) {
        pElem[nPos].Offset = *pOffset;
        pElem[nPos].Type = graph::E_GRAPH_VTX_DECL_TYPE_COLOR;
        pElem[nPos].Usage = graph::E_GRAPH_VTX_DECL_USAGE_COLOR;
        *pOffset += GetColorSize(flag);
        return nPos + 1;
    }
    return nPos;
}

// UV座標
IZ_UINT CDebugMesh::SetVtxElementUV(IZ_UINT flag, graph::SVertexElement* pElem, IZ_UINT nPos, IZ_WORD* pOffset)
{
    if (IsUV(flag)) {
        pElem[nPos].Offset = *pOffset;
        pElem[nPos].Type = graph::E_GRAPH_VTX_DECL_TYPE_FLOAT2;
        pElem[nPos].Usage = graph::E_GRAPH_VTX_DECL_USAGE_TEXCOORD;
        *pOffset += GetUVSize(flag);
        return nPos + 1;
    }
    return nPos;
}

// 接ベクトル
IZ_UINT CDebugMesh::SetVtxElementTangent(IZ_UINT flag, graph::SVertexElement* pElem, IZ_UINT nPos, IZ_WORD* pOffset)
{
    if (IsTangent(flag)) {
        pElem[nPos].Offset = *pOffset;
        pElem[nPos].Type = graph::E_GRAPH_VTX_DECL_TYPE_FLOAT4;
        pElem[nPos].Usage = graph::E_GRAPH_VTX_DECL_USAGE_TANGENT;
        *pOffset += GetTangentSize(flag);
        return nPos + 1;
    }
    return nPos;
}

// 位置
IZ_UINT8* CDebugMesh::SetVtxPos(IZ_UINT flag, const SMeshVtx& sVtx, IZ_UINT8* pVtx)
{
    if (IsPos(flag)) {
        IZ_FLOAT* pos = (IZ_FLOAT*)pVtx;
        pos[0] = sVtx.pos.v[0];
        pos[1] = sVtx.pos.v[1];
        pos[2] = sVtx.pos.v[2];
        pos[3] = 1.0f;

        pVtx += GetPosSize(flag);
    }
    return pVtx;
}

// 法線
IZ_UINT8* CDebugMesh::SetVtxNormal(IZ_UINT flag, const SMeshVtx& sVtx, IZ_UINT8* pVtx)
{
    if (IsNormal(flag)) {
        IZ_FLOAT* nml = (IZ_FLOAT*)pVtx;
        nml[0] = sVtx.nml.v[0];
        nml[1] = sVtx.nml.v[1];
        nml[2] = sVtx.nml.v[2];

        pVtx += GetNormalSize(flag);
    }
    return pVtx;
}

// 頂点カラー
IZ_UINT8* CDebugMesh::SetVtxColor(IZ_UINT flag, const SMeshVtx& sVtx, IZ_UINT8* pVtx)
{
    if (IsColor(flag)) {
        *(IZ_COLOR*)pVtx = sVtx.clr;
        pVtx += GetColorSize(flag);
    }
    return pVtx;
}

// UV座標
IZ_UINT8* CDebugMesh::SetVtxUV(IZ_UINT flag, const SMeshVtx& sVtx, IZ_UINT8* pVtx)
{
    if (IsUV(flag)) {
        IZ_FLOAT* uv = (IZ_FLOAT*)pVtx;
        uv[0] = sVtx.uv[0];
        uv[1] = sVtx.uv[1];

        pVtx += GetUVSize(flag);
    }
    return pVtx;
}

// 接ベクトル
IZ_UINT8* CDebugMesh::SetVtxTangent(IZ_UINT flag, const SMeshVtx& sVtx, IZ_UINT8* pVtx)
{
    if (IsTangent(flag)) {
        IZ_FLOAT* tangent = (IZ_FLOAT*)pVtx;
        tangent[0] = sVtx.tangent.v[0];
        tangent[1] = sVtx.tangent.v[1];
        tangent[2] = sVtx.tangent.v[2];
        tangent[3] = sVtx.tangent.v[3];

        pVtx += GetTangentSize(flag);
    }
    return pVtx;
}
