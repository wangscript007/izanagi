#include "FreeTypeApp.h"
#include "HarfBuzzApp.h"
#include "SimpleTextApp.h"

static const IZ_UINT SCREEN_WIDTH = 1280;
static const IZ_UINT SCREEN_HEIGHT = 720;

static const IZ_UINT BUF_SIZE = 10 * 1024 * 1024;
static IZ_UINT8 BUF[BUF_SIZE];

static const IZ_UINT GFX_BUF_SIZE = 1 * 1024 * 1024;
static IZ_UINT GFX_BUF[GFX_BUF_SIZE];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //CFreeTypeApp app;
    CSimpleTextApp app;

    int ret = SampleMain(
        hInstance,
        &app,
        "FreeType",
        SCREEN_WIDTH, SCREEN_HEIGHT,
        BUF, BUF_SIZE,
        GFX_BUF, GFX_BUF_SIZE);

    return ret;
}
