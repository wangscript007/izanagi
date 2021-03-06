#if !defined(__SHADOW_H__)
#define __SHADOW_H__

#include "izSampleKit.h"

class CShadow : public izanagi::CObject {
public:
    static CShadow* Create(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        IZ_UINT width,
        IZ_UINT height);

public:
    CShadow();
    ~CShadow();

    IZ_DEFINE_INTERNAL_RELEASE();

public:
    IZ_BOOL BeginShadowRender(
        izanagi::graph::CGraphicsDevice* device,
        const izanagi::math::SVector4& lightPos,
        const izanagi::math::SVector4& lightDir);

    void EndShadowRender(izanagi::graph::CGraphicsDevice* device);

    izanagi::graph::CRenderTarget* GetRT() { return m_RT; }
    izanagi::graph::CRenderTarget* GetDepthRT() { return m_DepthRT; }

    const izanagi::math::SMatrix44& GetLightViewProjMtx() const;
    const izanagi::math::SMatrix44& GetShadowTexMtx() const;

private:
    izanagi::IMemoryAllocator* m_Allocator;

    izanagi::graph::CRenderTarget* m_RT;
    izanagi::graph::CRenderTarget* m_DepthRT;

    izanagi::CCamera m_Light;

    static const izanagi::graph::E_GRAPH_PIXEL_FMT fmtRT = izanagi::graph::E_GRAPH_PIXEL_FMT_RGBA8;
    static const izanagi::graph::E_GRAPH_PIXEL_FMT fmtDepth = izanagi::graph::E_GRAPH_PIXEL_FMT_D24S8;
};

#endif    // #if !defined(__SHADOW_H__)