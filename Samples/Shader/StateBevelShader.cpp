#include "StateBevelShader.h"
#include "izGraph.h"
#include "izSystem.h"
#include "StateManager.h"

CStateBevelShader::CStateBevelShader(
    izanagi::sample::CSampleApp* app,
    izanagi::SCameraParam& camera)
: CStateBase(app, camera)
{
    m_Img = IZ_NULL;
    m_Shader = IZ_NULL;
    m_Mesh = IZ_NULL;
}

CStateBevelShader::~CStateBevelShader()
{
    Destroy();
}

// 描画.
IZ_BOOL CStateBevelShader::Render(izanagi::graph::CGraphicsDevice* device)
{
    izanagi::math::SMatrix44 mtxL2W;
    izanagi::math::SMatrix44::SetUnit(mtxL2W);

    device->SetTexture(0, m_Img->GetTexture(0));

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(0)) {
            // パラメータ設定
            SetShaderParam(
                m_Shader,
                "g_mL2W",
                (void*)&mtxL2W,
                sizeof(mtxL2W));

            SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&m_Camera.mtxW2C,
                sizeof(m_Camera.mtxW2C));

            SetShaderParam(
                m_Shader,
                "g_mW2V",
                (void*)&m_Camera.mtxW2V,
                sizeof(m_Camera.mtxW2V));

            m_Shader->CommitChanges(device);

            m_Mesh->Draw(device);
        }
    }
    m_Shader->End(device);

    RenderName(device, "MirrorMap");

    return IZ_TRUE;
}

// 開始
IZ_BOOL CStateBevelShader::Enter(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::CValue& arg)
{
    IZ_BOOL result = IZ_TRUE;

    // テクスチャ
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open("data/EnvMap.img"));

        m_Img = izanagi::CImage::CreateImage(
                    allocator,
                    device,
                    &in);
        VGOTO(result = (m_Img != IZ_NULL), __EXIT__);
    }

    // シェーダ
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open("data/MirrorMapShader.shd"));

        m_Shader = izanagi::shader::CShaderBasic::CreateShader<izanagi::shader::CShaderBasic>(
                    allocator,
                    device,
                    &in);
        VGOTO(result = (m_Shader != IZ_NULL), __EXIT__);
    }

    // 球
    {
        IZ_UINT flag = izanagi::E_DEBUG_MESH_VTX_FORM_POS
                        | izanagi::E_DEBUG_MESH_VTX_FORM_NORMAL;

        m_Mesh = BevelShaderMesh::Create(
                        allocator,
                        device,
                        flag,
                        5.0f, 5.0f, 5.0f);
        VGOTO(result = (m_Mesh != IZ_NULL), __EXIT__);
    }

__EXIT__:
    if (!result) {
        Leave();
    }

    return result;
}

// ステートから抜ける（終了）.
IZ_BOOL CStateBevelShader::Leave()
{
    SAFE_RELEASE(m_Img);
    SAFE_RELEASE(m_Shader);
    SAFE_RELEASE(m_Mesh);

    return IZ_TRUE;
}