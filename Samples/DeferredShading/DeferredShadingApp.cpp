#include "DeferredShadingApp.h"

// NOTE
// http://learnopengl.com/#!Advanced-Lighting/Deferred-Shading
// http://enginetrouble.net/2014/10/classic-deferred-shading-2014.html
// http://ogldev.atspace.co.uk/www/tutorial35/tutorial35.html
// http://ogldev.atspace.co.uk/www/tutorial36/tutorial36.html
// http://ogldev.atspace.co.uk/www/tutorial37/tutorial37.html

DeferredShadingApp::DeferredShadingApp()
{
}

DeferredShadingApp::~DeferredShadingApp()
{
}

// 初期化.
IZ_BOOL DeferredShadingApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    IZ_BOOL result = IZ_TRUE;

    result = initMeshes(allocator, device);
    VGOTO(result, __EXIT__);

    result = initLights();
    VGOTO(result, __EXIT__);

    // シェーダ
    {
        izanagi::CFileInputStream in;
        VGOTO(result = in.Open("data/DeferredShader.shd"), __EXIT__);

        m_Shader = izanagi::shader::CShaderBasic::CreateShader<izanagi::shader::CShaderBasic>(
                    allocator,
                    device,
                    &in);
        VGOTO(result = (m_Shader != IZ_NULL), __EXIT__);
    }

    {
        izanagi::CFileInputStream in;
        VGOTO(result = in.Open("data/earth.img"), __EXIT__);

        m_Img = izanagi::CImage::CreateImage(allocator, device, &in);
        VGOTO(result = (m_Img != IZ_NULL), __EXIT__);
    }

    // GBuffer.
    result = m_gbuffer.initialize(allocator, device);
    VGOTO(result, __EXIT__);

    IZ_FLOAT fNear = 1.0f;
    IZ_FLOAT fFar = 500.0f;
    IZ_FLOAT verticalFOV = izanagi::math::CMath::Deg2Rad(60.0f);
    IZ_FLOAT aspect = (IZ_FLOAT)device->GetBackBufferWidth() / device->GetBackBufferHeight();

    // カメラ
    camera.Init(
        izanagi::math::CVector4(0.0f, 0.0f, 30.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 0.0f, 0.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 1.0f, 0.0f, 1.0f),
        fNear,
        fFar,
        verticalFOV,
        aspect);
    camera.Update();

__EXIT__:
    if (!result) {
        ReleaseInternal();
    }

    return IZ_TRUE;
}

IZ_BOOL DeferredShadingApp::initMeshes(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device)
{
    IZ_UINT flag = izanagi::E_DEBUG_MESH_VTX_FORM_POS
        | izanagi::E_DEBUG_MESH_VTX_FORM_COLOR
        | izanagi::E_DEBUG_MESH_VTX_FORM_NORMAL
        | izanagi::E_DEBUG_MESH_VTX_FORM_UV;

    // 球
    {
        m_meshes[0].mesh = izanagi::CDebugMeshSphere::CreateDebugMeshSphere(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            5.0f,
            20, 20);
        VRETURN(m_meshes[0].mesh != IZ_NULL);

        m_meshes[0].mtxL2W.SetTrans(10.0f, 5.0f, 10.0f);
    }
    {
        m_meshes[1].mesh = izanagi::CDebugMeshSphere::CreateDebugMeshSphere(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            5.0f,
            20, 20);
        VRETURN(m_meshes[1].mesh != IZ_NULL);

        m_meshes[1].mtxL2W.SetTrans(10.0f, 5.0f, -10.0f);
    }

    // キューブ
    {
        m_meshes[2].mesh = izanagi::CDebugMeshBox::CreateDebugMeshBox(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            10.0f, 10.0f, 10.0f);
        VRETURN(m_meshes[2].mesh != IZ_NULL);

        m_meshes[2].mtxL2W.SetTrans(-10.0f, 5.0f, -10.0f);
    }
    {
        m_meshes[3].mesh = izanagi::CDebugMeshBox::CreateDebugMeshBox(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            10.0f, 10.0f, 10.0f);
        VRETURN(m_meshes[3].mesh != IZ_NULL);

        m_meshes[3].mtxL2W.SetTrans(-10.0f, 5.0f, 10.0f);
    }

    // 地面
    {
        m_meshes[4].mesh = izanagi::CDebugMeshRectangle::CreateDebugMeshRectangle(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0x00, 0x00, 0x00, 0xff),
            10, 10,
            50.0f, 50.0f);
        VRETURN(m_meshes[4].mesh != IZ_NULL);
    }

    // For PointLight.
    {
        flag = izanagi::E_DEBUG_MESH_VTX_FORM_POS;

        m_pointLitSphere = izanagi::CDebugMeshSphere::CreateDebugMeshSphere(
            allocator,
            device,
            flag,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff),
            1.0f,
            20, 20);
        VRETURN(m_pointLitSphere != IZ_NULL);
    }

    // For FinalPass.
    {
        m_screenFillPlane = izanagi::CDebugMeshScreenFillPlane::create(
            allocator,
            device,
            IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff));
        VRETURN(m_screenFillPlane != IZ_NULL);
    }

    return IZ_TRUE;
}

IZ_BOOL DeferredShadingApp::initLights()
{
    m_pointLights[0].vPos.Set(0.0f, 5.0f, 0.0f);
    m_pointLights[0].color.Set(1.0f, 0.0f, 0.0f, 1.0f);
    m_pointLights[0].attn.Set(0.0f, 0.0f, 0.01f, 0.0f);

    m_pointLights[1].vPos.Set(10.0f, 5.0f, 0.0f);
    m_pointLights[1].color.Set(0.0f, 1.0f, 0.0f, 1.0f);
    m_pointLights[1].attn.Set(0.0f, 0.0f, 0.05f, 0.0f);

    return IZ_TRUE;
}

// 解放.
void DeferredShadingApp::ReleaseInternal()
{
    for (IZ_UINT i = 0; i < MESH_NUM; i++)
    {
        SAFE_RELEASE(m_meshes[i].mesh);
    }

    SAFE_RELEASE(m_Shader);
    SAFE_RELEASE(m_Img);

    SAFE_RELEASE(m_pointLitSphere);
    SAFE_RELEASE(m_screenFillPlane);

    m_gbuffer.release();
}

// 更新.
void DeferredShadingApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    GetCamera().Update();
}

namespace {
    inline void _SetShaderParam(
        izanagi::shader::CShaderBasic* shader,
        const char* name,
        const void* value,
        IZ_UINT bytes)
    {
        izanagi::shader::IZ_SHADER_HANDLE handle = shader->GetParameterByName(name);
        //IZ_ASSERT(handle != 0);

        if (handle > 0) {
            shader->SetParamValue(
                handle,
                value,
                bytes);
        }
    }
}

// 描画.
void DeferredShadingApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
    renderGeometryPass(device);

    renderStencilPass(device);

    {
        device->SetRenderState(
            izanagi::graph::E_GRAPH_RS_STENCIL_ENABLE,
            IZ_TRUE);

        device->SetStencilFunc(
            izanagi::graph::E_GRAPH_CMP_FUNC_NOTEQUAL,
            0,
            0xffffffff);

        device->SetStencilOp(
            izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
            izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
            izanagi::graph::E_GRAPH_STENCIL_OP_KEEP);
    }

    renderLightPass(device);

    {
        device->SetRenderState(
            izanagi::graph::E_GRAPH_RS_STENCIL_ENABLE,
            IZ_FALSE);
    }

    renderFinalPass(device);

    device->SetTexture(0, IZ_NULL);

    // Test if depth is blitted from fbo to default buffer.
#if 0
    izanagi::sample::CSampleCamera& camera = GetCamera();

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(0)) {
            m_Shader->SetTexture("tex", m_Img->GetTexture(0));

            _SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&camera.GetParam().mtxW2C,
                sizeof(camera.GetParam().mtxW2C));

            izanagi::math::CMatrix44 mtxL2W;

            _SetShaderParam(
                m_Shader,
                "g_mL2W",
                (void*)&mtxL2W,
                sizeof(izanagi::math::SMatrix44));

            m_Shader->CommitChanges(device);

            m_meshes[0].mesh->Draw(device);

            m_Shader->EndPass();
        }
    }
    m_Shader->End(device);
#endif

    m_gbuffer.drawBuffers(device);
}

void DeferredShadingApp::renderGeometryPass(izanagi::graph::CGraphicsDevice* device)
{
    izanagi::sample::CSampleCamera& camera = GetCamera();

    m_gbuffer.beginGeometryPass(device);

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(1)) {
            m_Shader->SetTexture("tex", m_Img->GetTexture(0));

            _SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&camera.GetParam().mtxW2C,
                sizeof(camera.GetParam().mtxW2C));
            _SetShaderParam(
                m_Shader,
                "g_mW2V",
                (void*)&camera.GetParam().mtxW2V,
                sizeof(camera.GetParam().mtxW2V));
            _SetShaderParam(
                m_Shader,
                "g_farClip",
                (void*)&camera.GetParam().cameraFar,
                sizeof(camera.GetParam().cameraFar));

            for (IZ_UINT i = 0; i < MESH_NUM; i++)
            {
                _SetShaderParam(
                    m_Shader,
                    "g_mL2W",
                    (void*)&m_meshes[i].mtxL2W,
                    sizeof(izanagi::math::SMatrix44));

                m_Shader->CommitChanges(device);

                m_meshes[i].mesh->Draw(device);
            }

            m_Shader->EndPass();
        }
    }
    m_Shader->End(device);

    m_gbuffer.endGeometryPass(device);
}

namespace {
    IZ_FLOAT _ComputePointLightScale(const izanagi::SPointLightParam& pointlit)
    {
        IZ_FLOAT cmax = IZ_MAX(IZ_MAX(pointlit.color.r, pointlit.color.g), pointlit.color.b);  // color

        IZ_FLOAT c = pointlit.constant; // constant
        IZ_FLOAT l = pointlit.linear;   // linear
        IZ_FLOAT e = pointlit.exp;      // exp

        IZ_FLOAT i = 1.0f;  // intensity

        IZ_FLOAT d = (-l + sqrtf(l * l - 4.0f * e * (c - 256.0f * cmax * i))) / (2.0f * e);

        return d;
    }
}

void DeferredShadingApp::renderStencilPass(izanagi::graph::CGraphicsDevice* device)
{
    izanagi::sample::CSampleCamera& camera = GetCamera();

    // カラー・デプスバッファに書き込みしない.
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_ZENABLE,
        IZ_TRUE);
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_ZWRITEENABLE,
        IZ_FALSE);
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_RGB,
        IZ_FALSE);
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_A,
        IZ_FALSE);

    device->SaveRenderState();

    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_CULLMODE,
        izanagi::graph::E_GRAPH_CULL_NONE);

    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_STENCIL_ENABLE,
        IZ_TRUE);

    device->SetStencilFunc(
        izanagi::graph::E_GRAPH_CMP_FUNC_ALWAYS,
        0,
        0);

    device->SetStencilOp(
        izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
        izanagi::graph::E_GRAPH_STENCIL_OP_INCR,
        izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
        IZ_FALSE);
    device->SetStencilOp(
        izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
        izanagi::graph::E_GRAPH_STENCIL_OP_DECR,
        izanagi::graph::E_GRAPH_STENCIL_OP_KEEP,
        IZ_TRUE);

    // カラー・デプスバッファに書き込みしない.
    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_ZENABLE,
        IZ_TRUE);
    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_ZWRITEENABLE,
        IZ_FALSE);
    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_RGB,
        IZ_FALSE);
    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_A,
        IZ_FALSE);

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(0)) {
            _SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&camera.GetParam().mtxW2C,
                sizeof(camera.GetParam().mtxW2C));

            for (IZ_UINT i = 0; i < POINT_LIGHT_NUM; i++) {
                auto s = _ComputePointLightScale(m_pointLights[i]);

                izanagi::math::CMatrix44 mtxL2W;
                mtxL2W.SetScale(s, s, s);
                mtxL2W.Trans(
                    m_pointLights[i].vPos.x,
                    m_pointLights[i].vPos.y,
                    m_pointLights[i].vPos.z);

                _SetShaderParam(
                    m_Shader,
                    "g_mL2W",
                    (void*)&mtxL2W,
                    sizeof(izanagi::math::SMatrix44));

                m_Shader->CommitChanges(device);

                m_pointLitSphere->Draw(device);
            }

            m_Shader->EndPass();
        }
    }
    m_Shader->End(device);

    // 元に戻す.
    device->LoadRenderState();

    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_ZWRITEENABLE,
        IZ_TRUE);
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_RGB,
        IZ_TRUE);
    m_Shader->EnableToUpdateRenderState(
        izanagi::graph::E_GRAPH_RS_COLORWRITEENABLE_A,
        IZ_TRUE);
}

void DeferredShadingApp::renderLightPass(izanagi::graph::CGraphicsDevice* device)
{
    // TODO
    // ポイントライトの球の表面にピッタリ？にカメラが位置するときに、球が描画されずにライトが消えた状態になってしまう？.
    // どうやって回避する....
    // 1. ある程度のバッファを持たせた半径で計算する.
    // 2. ある程度のバッファを持たせた半径で計算し、ある程度の大きさとのときは全画面板で描画する.

    izanagi::sample::CSampleCamera& camera = GetCamera();

    izanagi::math::SMatrix44 mtxC2V;
    izanagi::math::SMatrix44::Inverse(mtxC2V, camera.GetParam().mtxV2C);

    izanagi::math::SMatrix44 mtxV2W;
    izanagi::math::SMatrix44::Inverse(mtxV2W, camera.GetParam().mtxW2V);

    m_gbuffer.beginLightPass(device, m_Shader);

    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_CULLMODE,
        izanagi::graph::E_GRAPH_CULL_CW);

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        if (m_Shader->BeginPass(2)) {
            m_Shader->SetTexture("texDepth", m_gbuffer.getBuffer(GBuffer::Type::Depth));

            _SetShaderParam(
                m_Shader,
                "g_mW2C",
                (void*)&camera.GetParam().mtxW2C,
                sizeof(camera.GetParam().mtxW2C));
            _SetShaderParam(
                m_Shader,
                "g_farClip",
                (void*)&camera.GetParam().cameraFar,
                sizeof(camera.GetParam().cameraFar));

            _SetShaderParam(
                m_Shader,
                "g_mtxC2V",
                (void*)&mtxC2V,
                sizeof(mtxC2V));
            _SetShaderParam(
                m_Shader,
                "g_mtxV2W",
                (void*)&mtxV2W,
                sizeof(mtxV2W));

            for (IZ_UINT i = 0; i < POINT_LIGHT_NUM; i++) {
                auto s = _ComputePointLightScale(m_pointLights[i]);

                izanagi::math::CMatrix44 mtxL2W;
                mtxL2W.SetScale(s, s, s);
                mtxL2W.Trans(
                    m_pointLights[i].vPos.x,
                    m_pointLights[i].vPos.y,
                    m_pointLights[i].vPos.z);

                auto dist = izanagi::math::SVector4::Length2(
                    camera.GetPos(),
                    m_pointLights[i].vPos);

                _SetShaderParam(
                    m_Shader,
                    "g_mL2W",
                    (void*)&mtxL2W,
                    sizeof(izanagi::math::SMatrix44));

                _SetShaderParam(
                    m_Shader,
                    "g_PointLightPos",
                    (void*)&m_pointLights[i].vPos,
                    sizeof(m_pointLights[i].vPos));
                _SetShaderParam(
                    m_Shader,
                    "g_PointLightAttn",
                    (void*)&m_pointLights[i].attn,
                    sizeof(m_pointLights[i].attn));
                _SetShaderParam(
                    m_Shader,
                    "g_PointLightColor",
                    (void*)&m_pointLights[i].color,
                    sizeof(m_pointLights[i].color));

                m_Shader->CommitChanges(device);

                m_pointLitSphere->Draw(device);
            }

            m_Shader->EndPass();
        }
    }
    m_Shader->End(device);

    // 元に戻す.
    device->SetRenderState(
        izanagi::graph::E_GRAPH_RS_CULLMODE,
        izanagi::graph::E_GRAPH_CULL_DEFAULT);

    m_gbuffer.endLightPass(device, m_Shader);
}

void DeferredShadingApp::renderFinalPass(izanagi::graph::CGraphicsDevice* device)
{
    izanagi::math::CVector4 invScreenSize(
        1.0f / device->GetBackBufferWidth(),
        1.0f / device->GetBackBufferHeight(),
        0.0f);

    m_Shader->Begin(device, 0, IZ_FALSE);
    {
        m_Shader->SetTexture("texAlbedo", m_gbuffer.getBuffer(GBuffer::Type::Albedo));
        m_Shader->SetTexture("texLight", m_gbuffer.getLightBuffer());

        if (m_Shader->BeginPass(3)) {
            _SetShaderParam(
                m_Shader,
                "g_vInvScreen",
                (void*)&invScreenSize,
                sizeof(invScreenSize));

            m_Shader->CommitChanges(device);

            m_screenFillPlane->Draw(device);

            m_Shader->EndPass();
        }
    }
    m_Shader->End(device);
}