#include "SceneRenderer.h"
#include "izSystem.h"

CSceneRenderer CSceneRenderer::sInstance;

CSceneRenderer::CSceneRenderer()
{
    m_Torus = IZ_NULL;
    m_Shader = IZ_NULL;

    m_EnvBox = IZ_NULL;
    m_EnvImg = IZ_NULL;
    m_EnvShader = IZ_NULL;
}

CSceneRenderer::~CSceneRenderer()
{
    Release();
}

namespace {
    inline void _SetShaderParam(
        izanagi::shader::CShaderBasic* shader,
        const char* name,
        const void* value,
        IZ_UINT bytes)
    {
        izanagi::shader::IZ_SHADER_HANDLE handle = shader->GetParameterByName(name);
        if (handle > 0) {
            shader->SetParamValue(
                handle,
                value,
                bytes);
        }
    }
}

IZ_BOOL CSceneRenderer::Init(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device)
{
    IZ_BOOL result = InitTorus(allocator, device);
    VGOTO(result, __EXIT__);

    result = InitEnvBox(allocator, device);
    VGOTO(result, __EXIT__);

__EXIT__:
    if (!result) {
        Release();
    }

    return result;
}

IZ_BOOL CSceneRenderer::InitTorus(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device)
{
    IZ_BOOL result = IZ_TRUE;

    // シェーダ
    {
        izanagi::CFileInputStream in;
        VRETURN(in.Open("data/PhongShader.shd"));

        m_Shader = izanagi::shader::CShaderBasic::CreateShader<izanagi::shader::CShaderBasic>(
                    allocator,
                    device,
                    &in);
        VGOTO(result = (m_Shader != IZ_NULL), __EXIT__);
    }

    // トーラス
    {
        IZ_UINT flag = izanagi::E_DEBUG_MESH_VTX_FORM_POS
                        | izanagi::E_DEBUG_MESH_VTX_FORM_NORMAL
                        | izanagi::E_DEBUG_MESH_VTX_FORM_COLOR;

        m_Torus = izanagi::CDebugMeshTorus::CreateDebugMeshTorus(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            2.0f, 5.0f,
            10, 10);
        VGOTO(result = (m_Torus != IZ_NULL), __EXIT__);
    }

__EXIT__:
    return result;
}

IZ_BOOL CSceneRenderer::InitEnvBox(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device)
{
    IZ_BOOL result = IZ_TRUE;

    IZ_UINT flag = izanagi::E_DEBUG_MESH_VTX_FORM_POS
                    | izanagi::E_DEBUG_MESH_VTX_FORM_COLOR;

    // Cube
    {
        m_EnvBox = izanagi::sample::CSampleEnvBox::CreateSampleEnvBox(
            allocator,
            device);
        VGOTO(result = (m_EnvBox != IZ_NULL), __EXIT__);
    }

    // テクスチャ
    {
        izanagi::CFileInputStream in;
        VGOTO(result = in.Open("data/EnvMap.img"), __EXIT__);

        m_EnvImg = izanagi::CImage::CreateImage(
            allocator,
            device,
            &in);
        VGOTO(result = (m_EnvImg != IZ_NULL), __EXIT__);
    }

    // シェーダ
    {
        izanagi::CFileInputStream in;
        VGOTO(result = in.Open("data/CubeMapBoxShader.shd"), __EXIT__);

        m_EnvShader = izanagi::shader::CShaderBasic::CreateShader<izanagi::shader::CShaderBasic>(
            allocator,
            device,
            &in);
        VGOTO(result = (m_EnvShader != IZ_NULL), __EXIT__);
    }

__EXIT__:
    return result;
}

void CSceneRenderer::Render(
    const izanagi::SCameraParam& camera,
    izanagi::graph::CGraphicsDevice* device)
{
    RenderEnvBox(camera, device);

    // ライトパラメータ
    {
        // Ambient Light Color
        izanagi::SAmbientLightParam ambient;
        ambient.color.Set(0.0f, 0.0f, 0.0f);

        // Parallel Light Color
        m_ParallelLight.color.Set(1.0f, 1.0f, 1.0f);

        // Parallel Light Direction
        m_ParallelLight.vDir.Set(-1.0f, -1.0f, -1.0f);
        izanagi::math::SVector4::Normalize(m_ParallelLight.vDir, m_ParallelLight.vDir);

        // マテリアル
        izanagi::SMaterialParam mtrl;
        {
            mtrl.vDiffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
            mtrl.vAmbient.Set(1.0f, 1.0f, 1.0f, 1.0f);
            mtrl.vSpecular.Set(1.0f, 1.0f, 1.0f, 20.0f);
        }

        _SetShaderParam(m_Shader, "g_vLitParallelColor", &m_ParallelLight.color, sizeof(m_ParallelLight.color));
        _SetShaderParam(m_Shader, "g_vLitAmbientColor", &ambient.color, sizeof(ambient.color));

        _SetShaderParam(m_Shader, "g_vMtrlDiffuse", &mtrl.vDiffuse, sizeof(mtrl.vDiffuse));
        _SetShaderParam(m_Shader, "g_vMtrlAmbient", &mtrl.vAmbient, sizeof(mtrl.vAmbient));
        _SetShaderParam(m_Shader, "g_vMtrlSpecular", &mtrl.vSpecular, sizeof(mtrl.vSpecular));
    }

    RenderTorus(camera, device);
}

void CSceneRenderer::RenderTorus(
    const izanagi::SCameraParam& camera,
    izanagi::graph::CGraphicsDevice* device)
{
    izanagi::math::SMatrix44::SetUnit(m_L2W);

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(0)) {
            // パラメータ設定
            _SetShaderParam(
                m_Shader,
                "g_mL2W",
                (void*)&m_L2W,
                sizeof(m_L2W));

            _SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&camera.mtxW2C,
                sizeof(camera.mtxW2C));

            _SetShaderParam(
                m_Shader,
                "g_vEye",
                (void*)&camera.pos,
                sizeof(camera.pos));

            {
                // ライトの方向をローカル座標に変換する

                // ライトの方向はワールド座標なので World -> Localマトリクスを計算する
                izanagi::math::SMatrix44 mtxW2L;
                izanagi::math::SMatrix44::Inverse(mtxW2L, m_L2W);

                // World -> Local
                izanagi::math::SVector4 parallelLightLocalDir;
                izanagi::math::SMatrix44::ApplyXYZ(
                    parallelLightLocalDir,
                    m_ParallelLight.vDir,
                    mtxW2L);

                _SetShaderParam(
                    m_Shader,
                    "g_vLitParallelDir",
                    (void*)&parallelLightLocalDir,
                    sizeof(parallelLightLocalDir));
            }

            m_Shader->CommitChanges(device);

            m_Torus->Draw(device);
        }
    }
    m_Shader->End(device);
}

void CSceneRenderer::RenderEnvBox(
    const izanagi::SCameraParam& camera,
    izanagi::graph::CGraphicsDevice* device)
{
    izanagi::math::SMatrix44::SetScale(m_L2W, 100.0f, 100.0f, 100.0f);

    // カメラの位置にあわせて移動する
    izanagi::math::SMatrix44::Trans(
        m_L2W,
        m_L2W,
        camera.pos);

    device->SetTexture(0, m_EnvImg->GetTexture(2));

    // テクスチャあり
    m_EnvShader->Begin(device, 0, IZ_FALSE);
    {
        if (m_EnvShader->BeginPass(0)) {
            // パラメータ設定
            _SetShaderParam(
                m_EnvShader,
                "g_mW2C",
                (void*)&camera.mtxW2C,
                sizeof(izanagi::math::SMatrix44));

            _SetShaderParam(
                m_EnvShader,
                "g_mL2W",
                (void*)&m_L2W,
                sizeof(m_L2W));

            m_EnvShader->CommitChanges(device);

            m_EnvBox->Render(device);

            m_EnvShader->EndPass();
        }
    }
    m_EnvShader->End(device);
}

void CSceneRenderer::Release()
{
    SAFE_RELEASE(m_Torus);
    SAFE_RELEASE(m_Shader);

    SAFE_RELEASE(m_EnvBox);
    SAFE_RELEASE(m_EnvImg);
    SAFE_RELEASE(m_EnvShader);
}
