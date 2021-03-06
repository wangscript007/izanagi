#include "StateManager.h"

#include "StateDefault.h"
#include "StateRotateByFling.h"
#include "StateChangeView.h"
#include "StateMoveToItem.h"
#include "StateRotateByDrag.h"

StateManager StateManager::s_Instance;

StateManager& StateManager::Instance()
{
    return s_Instance;
}

#define _STATE(idx, clazz, camera) \
    static clazz s_##clazz(camera); \
    Register(idx, &s_##clazz)

StateManager::StateManager()
{
}

void StateManager::Create(izanagi::CVectorCamera& camera)
{
    _STATE(State_Default, StateDefault, camera);
    _STATE(State_RotateByFling, StateRotateByFling, camera);
    _STATE(State_ChangeView, StateChangeView, camera);
    _STATE(State_MoveToItem, StateMoveToItem, camera);
    _STATE(State_RotateByDrag, StateRotateByDrag, camera);
}
