#if !defined(__IZANAGI_ENGINE_ANIIMATION_STATE_MACHINE_NODE_H__)
#define __IZANAGI_ENGINE_ANIIMATION_STATE_MACHINE_NODE_H__

#include "engine/StateMachineNode.h"
#include "engine/AnimationStateMachineBehaviour.h"
#include "izAnimation.h"

namespace izanagi {
namespace engine {
    /**
     */
    class AnimationStateMachineNode :public StateMachineNode, public CObject {
        friend class AnimationStateMachine;
        friend class AnimationStateMachineBehaviour;

    public:
        static AnimationStateMachineNode* create(
            IMemoryAllocator* allocator,
            const char* name);

    private:
        AnimationStateMachineNode();
        virtual ~AnimationStateMachineNode();

        IZ_DEFINE_INTERNAL_RELEASE();

    public:
        using Name = izanagi::CStdString < IZ_CHAR, 15 >;

        using HookFunc = std::function<void(void)>;

        const Name& getName() const;

        void setName(const char* name);

        void setEnterFunc(HookFunc func);

        void setExitFunc(HookFunc func);

        void setAnimation(izanagi::IAnimation* anm);

        virtual izanagi::IAnimation* getAnimation();

        IZ_BOOL addBehaviour(
            AnimationStateMachineBehaviour* behaviour,
            AnimationStateMachineNode* to);

        IZ_BOOL removeBehaviour(AnimationStateMachineBehaviour* behaviour);
        IZ_BOOL removeBehaviour(AnimationStateMachineNode* to);

        AnimationStateMachineBehaviour* getBehaviour(AnimationStateMachineNode* to);

    private:
        virtual State update(IZ_FLOAT delta) override;

        virtual StateMachineNode* next() override;

        virtual void enter() override;
        virtual void exit() override;

        void updateAnimation(IZ_FLOAT delta);

        void setTarget(izanagi::CSkeletonInstance* skl);

        const animation::CTimeline getTimeline() const
        {
            return m_timeline;
        }

        izanagi::CKey& getKey()
        {
            return m_key;
        }

        izanagi::CStdList<AnimationStateMachineNode>::Item* getListItem()
        {
            return &m_item;
        }

        void clear();

    private:
        IMemoryAllocator* m_Allocator{ nullptr };

        AnimationStateMachineNode::Name m_name;
        izanagi::CKey m_key;

        HookFunc m_enterFunc{ nullptr };
        HookFunc m_exitFunc{ nullptr };

        izanagi::CSkeletonInstance* m_skl{ nullptr };
        izanagi::IAnimation* m_anm{ nullptr };

        izanagi::CStdList<AnimationStateMachineBehaviour> m_behaviours;

        izanagi::CStdList<AnimationStateMachineNode>::Item m_item;

        AnimationStateMachineBehaviour* m_currentBehaviour{ nullptr };

        animation::CTimeline m_timeline;
    };
}   // namespace engine
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_ENGINE_ANIIMATION_STATE_MACHINE_NODE_H__)
