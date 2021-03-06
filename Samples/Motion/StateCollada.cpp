#include "StateCollada.h"
#include "izGraph.h"
#include "izSystem.h"
#include "StateManager.h"

CStateCollada::CStateCollada(izanagi::sample::CSampleApp* app)
: CStateBase(app)
{
}

CStateCollada::~CStateCollada()
{
    Destroy();
}

// 開始
IZ_BOOL CStateCollada::Enter(
    izanagi::IMemoryAllocator* allocator,
    izanagi::graph::CGraphicsDevice* device,
    izanagi::CValue& arg)
{
    IZ_BOOL result = InitObject(
        allocator,
        device,
        30.0f,
        "data/ModelImage.img",
        "data/Seymour.msh",
        "data/Seymour.skl",
        "data/Seymour.anm",
        "data/SkinShader.shd");

    return result;
}

// ステートから抜ける（終了）.
IZ_BOOL CStateCollada::Leave()
{
    ReleaseObject();
    return IZ_TRUE;
}

IZ_BOOL CStateCollada::Render(izanagi::graph::CGraphicsDevice* device)
{
    IZ_BOOL ret = CStateBase::Render(device);

    RenderName(device, "Collada");

    return ret;
}
