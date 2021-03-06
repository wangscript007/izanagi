#if !defined(__STATE_MANAGER_H__)
#define __STATE_MANAGER_H__

#include "izSceneGraph.h"

enum State {
    State_Default,
    State_RotateByFling,
    State_ChangeView,
    State_MoveToItem,
    State_RotateByDrag,

    StateNum,
};

class StateManager : public izanagi::CFixedSceneStateManager<State, StateNum> {
private:
    static StateManager s_Instance;

public:
    static StateManager& Instance();

private:
    StateManager();
    virtual ~StateManager() {}

public:
    void Create(izanagi::CVectorCamera& camera);
};

#endif    // #if !defined(__STATE_MANAGER_H__)
