#if !defined(__STATE_PHONG_SHADER_H__)
#define __STATE_PHONG_SHADER_H__

#include "izSceneGraph.h"
#include "izDebugUtil.h"
#include "StateBase.h"

class CStatePhongShader : public CStateBase {
public:
    CStatePhongShader(
        izanagi::sample::CSampleApp* app,
        izanagi::SCameraParam& camera);
    virtual ~CStatePhongShader();

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

protected:
    izanagi::shader::CShaderBasic* m_Shader;

    izanagi::CDebugMeshAxis* m_Axis;
    izanagi::CDebugMeshSphere* m_Sphere;

    izanagi::math::SMatrix44 m_L2W;
    izanagi::SParallelLightParam m_ParallelLight;
};

#endif    // #if !defined(__STATE_PHONG_SHADER_H__)
