#if !defined(__PLAYER_CHARACTER_H__)
#define __PLAYER_CHARACTER_H__

#include "izSampleKit.h"
#include "izEngine.h"
#include "izAnimation.h"

class PlayerCharacter
{
public:
    PlayerCharacter();
    ~PlayerCharacter();

public:
    // 初期化.
    IZ_BOOL init(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        izanagi::engine::TargetFollowCamera& camera);

    // 解放.
    void release();

    // 更新.
    void update(
        izanagi::graph::CGraphicsDevice* device,
        const izanagi::CCamera& camera,
        IZ_FLOAT elapsed);

    void prepareToRender(
        const izanagi::CCamera& camera,
        izanagi::CRenderGraph& renderGraph);

    // 描画.
    void render(
        izanagi::graph::CGraphicsDevice* device,
        const izanagi::CCamera& camera,
        izanagi::CRenderGraph* renderGraph,
        izanagi::CSceneRenderer* renderer);

    IZ_BOOL OnKeyDown(izanagi::sys::E_KEYBOARD_BUTTON key);
    void OnKeyUp(izanagi::sys::E_KEYBOARD_BUTTON key);

    const izanagi::math::CVector4& position() const
    {
        return m_ctrl->position();
    }

private:
    void updateDirection(IZ_FLOAT value);

    IZ_FLOAT GetTargetDirection(
        IZ_FLOAT forward, IZ_FLOAT right,
        izanagi::math::CVector3& dir);

    void prepareRotate(
        const izanagi::math::CVector3& dir,
        IZ_FLOAT value);

    void updateRotate(
        IZ_FLOAT delta,
        IZ_FLOAT value);

    void MoveForward(
        izanagi::math::CVector3& dir,
        IZ_FLOAT value);

protected:
    IZ_BOOL initModel(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device);

    IZ_BOOL createMaterial(
        izanagi::IMemoryAllocator* allocator,
        izanagi::graph::CGraphicsDevice* device,
        izanagi::CImage* img,
        izanagi::shader::CShaderBasic* shd);

    void initAnimation(izanagi::IMemoryAllocator* allocator);

private:
    izanagi::math::CQuat QuatToRotateRight;

    izanagi::CCamera* m_camera{ nullptr };

    izanagi::engine::AnimationStateMachine* m_anm{ nullptr };

    IZ_FLOAT m_speed{ 0.0f };

    izanagi::engine::Mesh* m_mesh{ nullptr };
    izanagi::engine::CharacterController* m_ctrl{ nullptr };

    enum State {
        Idle = 1 << 0,
        Move = 1 << 1,
        Rotate = 1 << 2,
    };

    IZ_UINT m_state{ State::Idle };
    IZ_FLOAT m_slerp{ 0.0f };

    // Character direction.
    izanagi::math::CVector3 m_dir;

    izanagi::math::CQuat m_toQuat;
    izanagi::math::CQuat m_fromQuat;

    IZ_FLOAT m_forward{ 0.0f };
    IZ_FLOAT m_right{ 0.0f };

    class CameraTarget : public izanagi::engine::ICameraTarget {
    public:
        CameraTarget() {}
        virtual ~CameraTarget() {}

        virtual izanagi::math::CVector3 getPos() const override
        {
            const auto& pos = m_target->m_ctrl->position();
            izanagi::math::CVector3 ret(pos.x, pos.y, pos.z);
            return std::move(ret);
        }
        virtual izanagi::math::CQuat getDir() const override
        {
            return std::move(m_target->m_ctrl->rotation());
        }

        PlayerCharacter* m_target;
    };
    CameraTarget m_camTarget;
};

#endif    // #if !defined(__PLAYER_CHARACTER_H__)
