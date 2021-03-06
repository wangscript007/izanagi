#include "izSampleKit.h"
#include "izDebugUtil.h"
#include <imgui.h>

class ImGuiApp : public izanagi::sample::CSampleApp {
public:
    ImGuiApp();
    virtual ~ImGuiApp();

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

private:
    izanagi::debugutil::ImGuiProc* m_imgui{ nullptr };

    bool show_test_window{ true };
    bool show_another_window{ false };
    ImVec4 clear_color{ ImColor(114, 144, 154) };
};

ImGuiApp::ImGuiApp()
{
}

ImGuiApp::~ImGuiApp()
{
}

// 初期化.
IZ_BOOL ImGuiApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    m_imgui = izanagi::debugutil::ImGuiProc::init(allocator, device);
    IZ_ASSERT(m_imgui);

    izanagi::sys::CSysWindow::registerExtendMsgHandler(m_imgui);

    return IZ_TRUE;
}

// 解放.
void ImGuiApp::ReleaseInternal()
{
    SAFE_RELEASE(m_imgui);
}

// 更新.
void ImGuiApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    m_imgui->beginFrame();

    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        static float f = 0.0f;
        ImGui::Text("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit3("clear color", (float*)&clear_color);
        if (ImGui::Button("Test Window")) {
            show_test_window ^= 1;
        }
        if (ImGui::Button("Another Window")) {
            show_another_window ^= 1;
        }
        auto framerate = ImGui::GetIO().Framerate * 1000.0f;
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);
    }

    // 2. Show another simple window, this time using an explicit Begin/End pair
    if (show_another_window)
    {
        ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello");
        ImGui::End();
    }

    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
        ImGui::ShowTestWindow(&show_test_window);
    }

}

// 描画.
void ImGuiApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
    ImGui::Render();
}

static const IZ_UINT BUF_SIZE = 1 * 1024 * 1024;
static IZ_UINT8 BUF[BUF_SIZE];

static const IZ_UINT GFX_BUF_SIZE = 1 * 1024 * 1024;
static IZ_UINT GFX_BUF[GFX_BUF_SIZE];

static const IZ_UINT SCREEN_WIDTH = 1280;
static const IZ_UINT SCREEN_HEIGHT = 720;

ImGuiApp app;

IzMain(SCREEN_WIDTH, SCREEN_HEIGHT)
{
    int ret = SampleMain(
        IzGetSystemDataForMainFunc(),
        &app,
        "ImGui",
        IzGetScreenWidth(), IzGetScreenHeight(),
        BUF, BUF_SIZE,
        GFX_BUF, GFX_BUF_SIZE);

    return ret;
}
