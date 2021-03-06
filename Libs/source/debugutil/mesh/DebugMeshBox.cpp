#include "debugutil/mesh/DebugMeshAxis.h"
#include "debugutil/mesh/DebugMeshBox.h"

using namespace izanagi;

namespace {

    // NOTE
    // 0---2 4---6    |+
    // |   | |   |  --+-->x
    // 1---3 5---7    |-
    //                z

    // 位置
    const IZ_FLOAT POS[][3] = {
#ifdef IZ_COORD_LEFT_HAND
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},

        { 0.5f,  0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
#else
        {-0.5f,  0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},

        { 0.5f,  0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
#endif
    };

    // 法線
    const IZ_FLOAT NML[][3] = {
#ifdef IZ_COORD_LEFT_HAND
        { 0.0f,  0.0f, -1.0f},
        { 1.0f,  0.0f,  0.0f},
        { 0.0f,  0.0f,  1.0f},
        {-1.0f,  0.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
#else
        { 0.0f,  0.0f,  1.0f},
        { 1.0f,  0.0f,  0.0f},
        { 0.0f,  0.0f, -1.0f},
        {-1.0f,  0.0f,  0.0f},
        { 0.0f,  1.0f,  0.0f},
        { 0.0f, -1.0f,  0.0f},
#endif
    };

    // UV
    const IZ_FLOAT UV[][2] = {
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
    };

    enum {
        PRIM_NUM = 6,
        VTX_NUM_PER_FACE = 4,
        IDX_NUM_PER_FACE = 6,
    };

    // 面
    const IZ_UINT16 PRIM[PRIM_NUM][IDX_NUM_PER_FACE] = {
        {0, 1, 2, 3},
        {3, 2, 5, 4},
        {4, 5, 6, 7},
        {7, 6, 1, 0},
        {7, 0, 3, 4},
        {1, 6, 5, 2},
    };
}   // namespace

// インスタンス作成
CDebugMeshBox* CDebugMeshBox::CreateDebugMeshBox(
    IMemoryAllocator* pAllocator,
    graph::CGraphicsDevice* pDevice,
    IZ_UINT flag,
    IZ_COLOR nColor,
    IZ_FLOAT fWidth,
    IZ_FLOAT fHeight,
    IZ_FLOAT fDepth)
{
    flag |= E_DEBUG_MESH_VTX_FORM_POS;

    void* pBuf = ALLOC_ZERO(pAllocator, sizeof(CDebugMeshBox));
    VRETURN_VAL(pBuf != IZ_NULL, IZ_NULL);

    CDebugMeshBox* pInstance = IZ_NULL;
    IZ_BOOL result = IZ_FALSE;

    // インスタンス作成
    pInstance = CreateMesh<CDebugMeshBox>(pBuf, pAllocator, pDevice, flag);
    VGOTO(result != (pInstance != IZ_NULL), __EXIT__);

    {
        pInstance->m_PrimType = graph::E_GRAPH_PRIM_TYPE_TRIANGLELIST;
        pInstance->m_nPrimCnt = PRIM_NUM * 2;
    }

    result = pInstance->Init(pDevice, flag);
    VGOTO(result, __EXIT__);

    // 頂点データセット
    result = pInstance->SetData(
                pDevice,
                flag,
                nColor,
                fWidth,
                fHeight,
                fDepth);
    VGOTO(result, __EXIT__);

    // インデックスデータセット
    result = pInstance->SetIdx();
    VGOTO(result, __EXIT__);

    // データをVB、IBにコピーする
    result = pInstance->CopyDataToBuffer(pDevice, flag);
    VGOTO(result, __EXIT__);

__EXIT__:
    if (!result) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

// 初期化
IZ_BOOL CDebugMeshBox::Init(
    graph::CGraphicsDevice* device,
    IZ_UINT flag)
{
    IZ_UINT nVtxNum = PRIM_NUM * VTX_NUM_PER_FACE;
    IZ_UINT nIdxNum = PRIM_NUM * IDX_NUM_PER_FACE;

    VRETURN(
        CreateVB(
            device,
            flag, 
            nVtxNum));

    VRETURN(
        CreateIB(
            device,
            nIdxNum, 
            graph::E_GRAPH_INDEX_BUFFER_FMT_INDEX32));

    VRETURN(CreateVD(device, flag));

    m_PrimType = graph::E_GRAPH_PRIM_TYPE_TRIANGLELIST;
    m_nPrimCnt = nIdxNum / 3;

    VRETURN(
        CreateDataBuffer(
            nVtxNum,
            nIdxNum));

    VRETURN(
        CreateDebugAxis(
            device,
            nVtxNum,
            flag));

    return IZ_TRUE;
}

// データセット
IZ_BOOL CDebugMeshBox::SetData(
    graph::CGraphicsDevice* pDevice,
    IZ_UINT flag,
    IZ_COLOR nColor,
    IZ_FLOAT fWidth,
    IZ_FLOAT fHeight,
    IZ_FLOAT fDepth)
{
    SMeshVtx* pVtx = GetVtx();

    for (IZ_UINT i = 0; i < PRIM_NUM; ++i) {
        for (IZ_UINT n = 0; n < VTX_NUM_PER_FACE; ++n) {
            // 位置
            if (CDebugMeshUtil::IsPos(flag)) {
                IZ_UINT idx = PRIM[i][n];

                pVtx->pos.v[0] = fWidth * POS[idx][0];
                pVtx->pos.v[1] = fHeight * POS[idx][1];
                pVtx->pos.v[2] = fDepth * POS[idx][2];
                pVtx->pos.v[3] = 1.0f;
            }

            // 法線
            if (CDebugMeshUtil::IsNormal(flag)) {
                pVtx->nml.v[0] = NML[i][0];
                pVtx->nml.v[1] = NML[i][1];
                pVtx->nml.v[2] = NML[i][2];
                pVtx->nml.v[3] = 0.0f;
            }

            // カラー
            if (CDebugMeshUtil::IsColor(flag)) {
                pVtx->clr = nColor;
            }
            
            // UV
            if (CDebugMeshUtil::IsUV(flag)) {
                pVtx->uv[0] = UV[n][0];
                pVtx->uv[1] = UV[n][1];
            }

            ++pVtx;
        }
    }

    IZ_ASSERT(CStdUtil::GetPtrAdvanced(GetVtx(), pVtx) == GetVtxNum());

    return IZ_TRUE;
}

// インデックスデータセット
IZ_BOOL CDebugMeshBox::SetIdx()
{
    SMeshFace* pFace = GetFace();

    IZ_UINT32 nCurIdx = 0;

    for (IZ_UINT i = 0; i < PRIM_NUM; ++i) {
        {
            pFace->idx[0] = nCurIdx + 0;
            pFace->idx[1] = nCurIdx + 1;
            pFace->idx[2] = nCurIdx + 2;

            BindFaceToVtx(pFace);
            ++pFace;
        }
        {
            pFace->idx[0] = nCurIdx + 0;
            pFace->idx[1] = nCurIdx + 2;
            pFace->idx[2] = nCurIdx + 3;

            BindFaceToVtx(pFace);
            ++pFace;
        }

        nCurIdx += 4;
    }

    IZ_ASSERT(CStdUtil::GetPtrAdvanced(GetFace(), pFace) == m_nPrimCnt);

    return IZ_TRUE;
}
