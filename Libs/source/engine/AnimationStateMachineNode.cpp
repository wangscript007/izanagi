#include"engine/AnimationStateMachineNode.h"

namespace izanagi {
namespace engine {
    AnimationStateMachineNode* AnimationStateMachineNode::create(
        IMemoryAllocator* allocator,
        const char* name)
    {
        void* buf = ALLOC(allocator, sizeof(AnimationStateMachineNode));
        VRETURN_NULL(buf);

        AnimationStateMachineNode* ret = new(buf)AnimationStateMachineNode;
        {
            ret->m_Allocator = allocator;
            ret->setName(name);

            ret->AddRef();
        }

        return ret;
    }

    AnimationStateMachineNode::AnimationStateMachineNode()
    {
        m_item.Init(this);
    }

    AnimationStateMachineNode::~AnimationStateMachineNode()
    {
        SAFE_RELEASE(m_skl);
        SAFE_RELEASE(m_anm);
    }

    const AnimationStateMachineNode::Name& AnimationStateMachineNode::getName() const
    {
        return m_name;
    }

    void AnimationStateMachineNode::setName(const char* name)
    {
        IZ_ASSERT(strlen(name) <= m_name.GetLen());
        m_name = name;
        m_key.SetValue(m_name.GetKeyValue());
    }

    void AnimationStateMachineNode::setAnimation(izanagi::IAnimation* anm)
    {
        SAFE_REPLACE(m_anm, anm);

        if (m_anm) {
            m_timeline.Init(
                m_anm->GetAnimationTime(),
                0.0f);
            m_timeline.EnableLoop(IZ_TRUE); // TODO
            m_timeline.AutoReverse(IZ_FALSE);
            m_timeline.Reset();
            m_timeline.Start();
        }
    }

    izanagi::IAnimation* AnimationStateMachineNode::getAnimation()
    {
        return m_anm;
    }

    IZ_BOOL AnimationStateMachineNode::addBehaviour(
        AnimationStateMachineBehaviour* behaviour,
        AnimationStateMachineNode* to)
    {
        auto b = getBehaviour(to);

        if (b) {
            return IZ_FALSE;
        }

        behaviour->setNode(this, to);

        auto ret = m_behaviours.AddTail(behaviour->getListItem());

        if (ret) {
            behaviour->AddRef();
        }

        return ret;
    }

    IZ_BOOL AnimationStateMachineNode::removeBehaviour(AnimationStateMachineBehaviour* behaviour)
    {
        if (behaviour->getListItem()->GetList() == &m_behaviours) {
            behaviour->getListItem()->Leave();
            return IZ_TRUE;
        }

        return IZ_FALSE;
    }

    IZ_BOOL AnimationStateMachineNode::removeBehaviour(AnimationStateMachineNode* to)
    {
        auto behaviour = getBehaviour(to);

        if (behaviour) {
            SAFE_RELEASE(behaviour);
            return IZ_TRUE;
        }

        return IZ_FALSE;
    }

    AnimationStateMachineBehaviour* AnimationStateMachineNode::getBehaviour(AnimationStateMachineNode* to)
    {
        auto item = m_behaviours.GetTop();

        while (item) {
            auto behaviour = item->GetData();

            if (behaviour->getTo() == to) {
                return behaviour;
            }
        }

        return nullptr;
    }

    StateMachineNode::State AnimationStateMachineNode::update(IZ_FLOAT delta)
    {
        if (m_state == State::None) {
            m_currentBehaviour = nullptr;

            enter();
            m_state = State::Enter;
            return State::Enter;
        }
        else if (m_state == State::Exit) {
            exit();
            m_state = State::None;
            return State::None;
        }
        else {
            // TODO
            // behaviour�̗D��x.

            auto item = m_behaviours.GetTop();

            while (item) {
                auto behaviour = item->GetData();

                auto state = behaviour->update(delta);

                if (state == State::Enter) {
                    m_currentBehaviour = behaviour;

                    exit();
                    m_state = State::None;

                    return State::Exit;
                }

                item = item->GetNext();
            }

            updateAnimation(delta);

            m_state = State::Running;
            return State::Running;
        }
    }

    StateMachineNode* AnimationStateMachineNode::next()
    {
        return m_currentBehaviour;
    }

    void AnimationStateMachineNode::enter()
    {
        StateMachineNode::enter();

        m_timeline.Reset();
        m_timeline.Start();
    }

    void AnimationStateMachineNode::exit()
    {
        StateMachineNode::exit();

        SAFE_RELEASE(m_skl);
    }

    void AnimationStateMachineNode::updateAnimation(IZ_FLOAT delta)
    {
        if (m_anm && m_skl) {
            m_timeline.Advance(delta);

            auto t = m_timeline.GetTime();

            m_skl->ApplyAnimation(t, m_anm);
        }
    }

    void AnimationStateMachineNode::setTarget(izanagi::CSkeletonInstance* skl)
    {
        SAFE_REPLACE(m_skl, skl);
    }
}
}