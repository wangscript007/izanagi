#include "GestureListener.h"
#include "Configure.h"
#include "PhotoItemManager.h"
#include "StateManager.h"

CGestureListener::CGestureListener()
{
}

CGestureListener::~CGestureListener()
{
}

void CGestureListener::OnTapUp(const izanagi::sys::CTouchEvent& ev)
{
    if (PhotoItemManager::Instance().IsRotateAnimating()) {
        StateManager::Instance().ChangeState(State_Default);
        return;
    }
}

void CGestureListener::OnFling(
    const izanagi::sys::CTouchEvent& ev,
    IZ_FLOAT velocityX, IZ_FLOAT velocityY)
{
    IZ_PRINTF("OnFling [%f] [%f]\n", velocityX, velocityY);

    if (izanagi::math::CMath::Absf(velocityY) > izanagi::math::CMath::Absf(velocityX)) {
        // Ignore fling.
        return;
    }

    // Compute angle rate.
    IZ_FLOAT angle = Configure::MaxAngleRate * velocityX;

    PhotoItemManager::Instance().SetAngleRate(angle);

    StateManager::Instance().ChangeState(State_RotateByFling);
}