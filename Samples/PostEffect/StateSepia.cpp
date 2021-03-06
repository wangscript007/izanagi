#include "StateSepia.h"
#include "StateManager.h"
#include "SceneRenderer.h"
#include "PostEffectSample.h"

CStateSepia::CStateSepia(
    izanagi::sample::CSampleApp* app,
    izanagi::SCameraParam& camera)
: CStatePostEffect(app, camera)
{
}

CStateSepia::~CStateSepia()
{
}

// 描画.
IZ_BOOL CStateSepia::Render(izanagi::graph::CGraphicsDevice* device)
{
    CStatePostEffect::Render(device);
    RenderName(device, "Sepia");

    return IZ_TRUE;
}

// 開始
IZ_BOOL CStateSepia::Enter(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::CValue& arg)
{
    IZ_BOOL ret = CPostEffectSample::GetInstance()->Read("data/Sepia.pes");
    return ret;
}
