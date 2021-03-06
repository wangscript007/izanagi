#include "PostEffectApp.h"
#include "StateManager.h"

CPostEffectApp::CPostEffectApp()
{
    m_Allocator = IZ_NULL;
    m_Device = IZ_NULL;
}

CPostEffectApp::~CPostEffectApp()
{
}

// キーボード押下
IZ_BOOL CPostEffectApp::OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key)
{
    if (key == izanagi::sys::E_KEYBOARD_BUTTON_SPACE)
    {
        CPostEffectSample::GetInstance()->ToggleEnablePostEffect();
        return IZ_TRUE;
    }

    return CStateManager::GetInstance().OnKeyDown(key);
}

// 初期化.
IZ_BOOL CPostEffectApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    m_Allocator = allocator;
    SAFE_REPLACE(m_Device, device);

    // カメラ
    camera.Init(
        izanagi::math::CVector4(0.0f, 0.0f, 30.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 0.0f, 0.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 1.0f, 0.0f, 1.0f),
        1.0f,
        500.0f,
        izanagi::math::CMath::Deg2Rad(60.0f),
        (IZ_FLOAT)device->GetBackBufferWidth() / device->GetBackBufferHeight());
    camera.Update();

    VRETURN(
        CSceneRenderer::GetInstance()->Init(
            allocator,
            device));

    VRETURN(
        CPostEffectSample::GetInstance()->Init(
            allocator,
            device));

    CStateManager::GetInstance().Create(
        this,
        const_cast<izanagi::SCameraParam&>(camera.GetParam()));

    return CStateManager::GetInstance().Init();
}

// 解放.
void CPostEffectApp::ReleaseInternal()
{
    CStateManager::GetInstance().Destroy();
    CSceneRenderer::GetInstance()->Release();
    CPostEffectSample::GetInstance()->Release();
    SAFE_RELEASE(m_Device);
}

// 更新.
void CPostEffectApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    CStateManager::GetInstance().Update(
        0.0f,   // Not used.
        m_Allocator, device);
}

// 描画.
void CPostEffectApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
    CStateManager::GetInstance().Render(device);
}
