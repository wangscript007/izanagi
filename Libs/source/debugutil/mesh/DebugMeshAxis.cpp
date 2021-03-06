#include "debugutil/mesh/DebugMeshAxis.h"
#include "math/MathVector4.h"

using namespace izanagi;

// インスタンス作成
CDebugMeshAxis* CDebugMeshAxis::CreateDebugMeshAxis(
    IMemoryAllocator* pAllocator,
    graph::CGraphicsDevice* device,
    IZ_UINT flag,
    IZ_UINT nPointNum/*= 0*/)
{
    // 必ず１つはあるようにする・・・
    if (!(CDebugMeshUtil::IsAxisX(flag)
        || CDebugMeshUtil::IsAxisY(flag)
        || CDebugMeshUtil::IsAxisZ(flag)))
    {
        flag |= E_DEBUG_MESH_AXIS_X;
    }

    void* pBuf = ALLOC_ZERO(pAllocator, sizeof(CDebugMeshAxis));
    VRETURN_VAL(pBuf != IZ_NULL, IZ_NULL);

    CDebugMeshAxis* pInstance = IZ_NULL;
    IZ_BOOL result = IZ_FALSE;

    // インスタンス作成
    pInstance = CreateMesh<CDebugMeshAxis>(pBuf, pAllocator, device, flag);
    VGOTO(result != (pInstance != IZ_NULL), __EXIT__);

    {
        pInstance->m_PrimType = graph::E_GRAPH_PRIM_TYPE_LINELIST;

        if (CDebugMeshUtil::IsAxisX(flag)) {
            ++(pInstance->m_nPrimCnt);
        }
        if (CDebugMeshUtil::IsAxisY(flag)) {
            ++(pInstance->m_nPrimCnt);
        }
        if (CDebugMeshUtil::IsAxisZ(flag)) {
            ++(pInstance->m_nPrimCnt);
        }

        if (nPointNum > 0) {
            pInstance->m_nPrimCnt *= nPointNum; 
        }

        pInstance->m_nAxisFlag = flag;
    }

    result = pInstance->Init(
                device,
                AXIS_VTX_FLAG);
    VGOTO(result, __EXIT__);

    // TODO

__EXIT__:
    if (!result) {
        SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

CDebugMeshAxis* CDebugMeshAxis::CreateDebugMeshAxisDefault(
    IMemoryAllocator* pAllocator,
    graph::CGraphicsDevice* device)
{
    CDebugMeshAxis* pInstance = CreateDebugMeshAxis(
                                    pAllocator,
                                    device,
                                    izanagi::E_DEBUG_MESH_AXIS_X | izanagi::E_DEBUG_MESH_AXIS_Y | izanagi::E_DEBUG_MESH_AXIS_Z);
    VRETURN(pInstance != IZ_NULL);

    pInstance->BeginRegister(device);
    {
        pInstance->SetVtx(
            izanagi::E_DEBUG_MESH_AXIS_X,
            math::CVector4(0.0f, 0.0f, 0.0f),
            math::CVector4(10.0f, 0.0f, 0.0f));
        pInstance->SetVtx(
            izanagi::E_DEBUG_MESH_AXIS_Y,
            math::CVector4(0.0f, 0.0f, 0.0f),
            math::CVector4(0.0f, 10.0f, 0.0f));
        pInstance->SetVtx(
            izanagi::E_DEBUG_MESH_AXIS_Z,
            math::CVector4(0.0f, 0.0f, 0.0f),
            math::CVector4(0.0f, 0.0f, 10.0f));
    }
    pInstance->EndRegister(device);

    return pInstance;
}

CDebugMeshAxis::~CDebugMeshAxis()
{
#if 0
    if (m_pVtxDst != IZ_NULL) {
        IZ_ASSERT(IZ_FALSE);
        EndRegister();
    }
#else
    IZ_ASSERT(m_pVtxDst == IZ_NULL)
#endif
}

// 初期化
IZ_BOOL CDebugMeshAxis::Init(
    graph::CGraphicsDevice* device,
    IZ_UINT flag)
{
    VRETURN(
        CreateVB(
            device,
            flag, 
            m_nPrimCnt * 2));   // １ライン２頂点

    VRETURN(CreateVD(device, flag));

    return IZ_TRUE;
}

// 描画
IZ_BOOL CDebugMeshAxis::Draw(graph::CGraphicsDevice* device)
{
    IZ_BOOL ret = (m_nSetPrimCnt == 0);

    if (!ret && (m_nSetPrimCnt == m_nPrimCnt)) {
        ret = CDebugMeshTmpl::Draw(device);
    }

    IZ_ASSERT(ret);
    return ret;
}

IZ_BOOL CDebugMeshAxis::BeginRegister(graph::CGraphicsDevice* device)
{
    VRETURN(m_pVtxDst == IZ_NULL);
    VRETURN(m_pVB->Lock(device, 0, 0, (void**)&m_pVtxDst, IZ_FALSE));
    m_nSetPrimCnt = 0;
    return IZ_TRUE;
}

IZ_BOOL CDebugMeshAxis::EndRegister(graph::CGraphicsDevice* device)
{
    VRETURN(m_pVtxDst != IZ_NULL);
    VRETURN(m_pVB->Unlock(device));
    m_pVtxDst = IZ_NULL;
    return IZ_TRUE;
}

IZ_BOOL CDebugMeshAxis::SetVtx(
    IZ_UINT type,
    const math::SVector4& start,
    const math::SVector4& end)
{
    VRETURN(m_pVtxDst != IZ_NULL);

    // 設定されてない軸
    if ((m_nAxisFlag & type) == 0) {
        return IZ_TRUE;
    }

    // 頂点数オーバー
    if (m_nSetPrimCnt >= m_nPrimCnt) {
        IZ_ASSERT(IZ_FALSE);
        return IZ_FALSE;
    }

    ++m_nSetPrimCnt;

    IZ_UINT nColor = 0;
    switch (type) {
    case E_DEBUG_MESH_AXIS_X:
        nColor = IZ_COLOR_RGBA(0xff, 0, 0, 0xff);
        break;
    case E_DEBUG_MESH_AXIS_Y:
        nColor = IZ_COLOR_RGBA(0, 0xff, 0, 0xff);
        break;
    case E_DEBUG_MESH_AXIS_Z:
        nColor = IZ_COLOR_RGBA(0, 0, 0xff, 0xff);
        break;
    default:
        nColor = IZ_COLOR_RGBA(0xff, 0, 0, 0xff);
        break;
    }

    SMeshVtx SMeshVtx[2];

    // Position
    {
        SMeshVtx[0].pos.v[0] = start.v[0];
        SMeshVtx[0].pos.v[1] = start.v[1];
        SMeshVtx[0].pos.v[2] = start.v[2];
        SMeshVtx[0].pos.v[3] = 1.0f;
    }
    {
        SMeshVtx[1].pos.v[0] = start.v[0] + end.v[0];
        SMeshVtx[1].pos.v[1] = start.v[1] + end.v[1];
        SMeshVtx[1].pos.v[2] = start.v[2] + end.v[2];
        SMeshVtx[1].pos.v[3] = 1.0f;
    }

    // Color
    SMeshVtx[0].clr = nColor;
    SMeshVtx[1].clr = nColor;

    m_pVtxDst = SetVtxData(
                    SMeshVtx[0],
                    AXIS_VTX_FLAG,
                    m_pVtxDst);
    m_pVtxDst = SetVtxData(
                    SMeshVtx[1],
                    AXIS_VTX_FLAG,
                    m_pVtxDst);

    return IZ_TRUE;
}
