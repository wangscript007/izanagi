#if !defined(__STATE_SPOT_LIGHT_H__)
#define __STATE_SPOT_LIGHT_H__

#include "izSceneGraph.h"
#include "izDebugUtil.h"
#include "StateBase.h"

class CStateSpotLight : public CStateBase {
public:
    CStateSpotLight(
        izanagi::sample::CSampleApp* app,
        izanagi::SCameraParam& camera);
    virtual ~CStateSpotLight();

public:
    // 描画.
    virtual IZ_BOOL Render(izanagi::graph::CGraphicsDevice* device);

    // 開始
    virtual IZ_BOOL Enter(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        izanagi::CValue& arg);

    // ステートから抜ける（終了）.
    virtual IZ_BOOL Leave();

    virtual IZ_BOOL EnableBgColor() const { return IZ_TRUE; }
    virtual IZ_COLOR GetBgColor() const { return IZ_COLOR_RGBA(0x00, 0x00, 0x00, 0xff); }

private:
    void RenderScene(
        izanagi::graph::CGraphicsDevice* device,
        izanagi::CDebugMesh* mesh,
        const izanagi::math::SVector4& position);

protected:
    izanagi::shader::CShaderBasic* m_Shader;

    izanagi::SSpotLightParam m_SpotLight;

    izanagi::CDebugMesh* m_Light;

    izanagi::CDebugMesh* m_Sphere;
    izanagi::CDebugMesh* m_Cube;
    izanagi::CDebugMesh* m_Plane;
};

#endif    // #if !defined(__STATE_SPOT_LIGHT_H__)
