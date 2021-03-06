#include "izSampleKit.h"
#include "StateManager.h"
#include "StateBase.h"

static const IZ_UINT BUF_SIZE = 8 * 1024 * 1024;
static IZ_UINT8 BUF[BUF_SIZE];

static const IZ_UINT GFX_BUF_SIZE = 1 * 1024 * 1024;
static IZ_UINT GFX_BUF[GFX_BUF_SIZE];

static const IZ_UINT SCREEN_WIDTH = 1280;
static const IZ_UINT SCREEN_HEIGHT = 720;

class CShaderApp : public izanagi::sample::CSampleApp {
public:
    CShaderApp();
    virtual ~CShaderApp();

public:
    // キーボード押下
    virtual IZ_BOOL OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key);

protected:
    // 初期化.
    virtual IZ_BOOL InitInternal(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        izanagi::sample::CSampleCamera& camera);

    // 解放.
    virtual void ReleaseInternal();

    // 更新.
    virtual void UpdateInternal(izanagi::graph::CGraphicsDevice* device);

    // 描画.
    virtual void RenderInternal(izanagi::graph::CGraphicsDevice* device);

    // 背景色取得.
    virtual IZ_COLOR GetBgColor() const;

private:
    izanagi::IMemoryAllocator* m_Allocator;
    izanagi::graph::CGraphicsDevice* m_Device;
};

CShaderApp::CShaderApp()
{
    m_Allocator = IZ_NULL;
    m_Device = IZ_NULL;
}

CShaderApp::~CShaderApp()
{
}

// キーボード押下
IZ_BOOL CShaderApp::OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key)
{
    return CStateManager::GetInstance().OnKeyDown(key);
}

// 初期化.
IZ_BOOL CShaderApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    m_Allocator = allocator;
    SAFE_REPLACE(m_Device, device);

    // カメラ
    camera.Init(
        izanagi::math::CVector4(0.0f, 10.0f, 30.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 0.0f, 0.0f, 1.0f),
        izanagi::math::CVector4(0.0f, 1.0f, 0.0f, 1.0f),
        1.0f,
        500.0f,
        izanagi::math::CMath::Deg2Rad(60.0f),
        (IZ_FLOAT)device->GetBackBufferWidth() / device->GetBackBufferHeight());
    camera.Update();

    CStateManager::GetInstance().Create(
        this,
        const_cast<izanagi::SCameraParam&>(camera.GetParam()));

    return CStateManager::GetInstance().Init();
}

// 解放.
void CShaderApp::ReleaseInternal()
{
    CStateManager::GetInstance().Destroy();
    SAFE_RELEASE(m_Device);
}

// 更新.
void CShaderApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    CStateManager::GetInstance().Update(
        0.0f,   // Not used.
        m_Allocator, m_Device);
}

// 描画.
void CShaderApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
    CStateManager::GetInstance().Render(device);
}

// 背景色取得.
IZ_COLOR CShaderApp::GetBgColor() const
{
    izanagi::CSceneStateBase* state = CStateManager::GetInstance().GetState(
        CStateManager::GetInstance().GetCurrentState());

    IZ_BOOL enableBgColor = (reinterpret_cast<CStateBase*>(state))->EnableBgColor();
    if (enableBgColor)
    {
        return (reinterpret_cast<CStateBase*>(state))->GetBgColor();
    }

    return CSampleApp::GetBgColor();
}

CShaderApp app;

IzMain(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    int ret = SampleMain(
        IzGetSystemDataForMainFunc(),
        &app,
        "Shader",
        IzGetScreenWidth(), IzGetScreenHeight(),
        BUF, BUF_SIZE,
        GFX_BUF, GFX_BUF_SIZE);

    return ret;
}
