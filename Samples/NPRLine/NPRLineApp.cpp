#include "NPRLineApp.h"
#include "izGraph.h"
#include "izSystem.h"
#include <imgui.h>

NPRLineApp::NPRLineApp()
{
}

NPRLineApp::~NPRLineApp()
{
}

// 開始
IZ_BOOL NPRLineApp::InitInternal(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::sample::CSampleCamera& camera)
{
    IZ_BOOL result = IZ_TRUE;

    m_scene.init(allocator, device, camera);

    auto width = device->GetBackBufferWidth();
    auto height = device->GetBackBufferHeight();

    m_shdGeometryPass.init(
        device,
        "shader/vs_geometry.glsl",
        "shader/ps_geometry.glsl");
#if 0
    m_shdFinalPass.init(
        device,
        "shader/vs_fillscreen.glsl",
        "shader/ps_final.glsl");
#endif
    
    m_gbuffer.init(allocator, device);

    m_screenFillPlane = izanagi::CDebugMeshScreenFillPlane::create(
        allocator,
        device,
        IZ_COLOR_RGBA(0xff, 0xff, 0xff, 0xff));
    IZ_ASSERT(m_screenFillPlane != IZ_NULL);

	// imgui
	{
		m_imgui = izanagi::debugutil::ImGuiProc::init(allocator, device);
		IZ_ASSERT(m_imgui);

		izanagi::sys::CSysWindow::registerExtendMsgHandler(m_imgui);
	}

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

__EXIT__:
    if (!result) {
        ReleaseInternal();
    }

    return result;
}

IZ_BOOL NPRLineApp::OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key)
{
    return IZ_TRUE;
}

// 更新.
void NPRLineApp::UpdateInternal(izanagi::graph::CGraphicsDevice* device)
{
    GetCamera().Update();

	m_imgui->beginFrame();
	{
		
	}
}

// 描画.
void NPRLineApp::RenderInternal(izanagi::graph::CGraphicsDevice* device)
{
	device->SaveRenderState();

	device->SetRenderState(
		izanagi::graph::E_GRAPH_RS_ALPHABLENDENABLE,
		IZ_FALSE);

	auto bgColor = GetBgColor();

	m_gbuffer.begin(device, bgColor);

	{
		const auto& camera = GetCamera();

		const auto& mtxW2C = camera.GetParam().mtxW2C;

		auto* shd = m_shdGeometryPass.m_program;

		device->SetShaderProgram(shd);

		auto hW2C = shd->GetHandleByName("mtxW2C");
		shd->SetMatrix(device, hW2C, mtxW2C);

		m_scene.render(device, shd);
	}

	m_gbuffer.end(device);

	device->LoadRenderState();

	device->SetTexture(0, IZ_NULL);
	m_gbuffer.drawBuffers(device);

	// imgui.
	ImGui::Render();
}

void NPRLineApp::ReleaseInternal()
{
    m_scene.release();

    m_shdGeometryPass.release();
    m_shdFinalPass.release();

    SAFE_RELEASE(m_screenFillPlane);

	SAFE_RELEASE(m_imgui);

    m_gbuffer.release();
}
