#if !defined(__GESTURE_LISTENER_H__)
#define __GESTURE_LISTENER_H__

#include "izSampleKit.h"
#include "izUI.h"

class AppGalleryApp;

/** Listen touch gesture.
 */
class CGestureListener : public izanagi::ui::CGestureListenerImplement
{
public:
    CGestureListener();
    virtual ~CGestureListener();

public:
    virtual void OnTapUp(const izanagi::sys::CTouchEvent& ev);

    virtual void OnFling(
        const izanagi::sys::CTouchEvent& ev,
        IZ_FLOAT velocityX, IZ_FLOAT velocityY);

    virtual void OnShowPress()
    {
        IZ_PRINTF("OnShowPress\n");
    }

    virtual void OnLongPress()
    {
        IZ_PRINTF("OnLongPress\n");
    }

    virtual void OnDrag(
        const izanagi::sys::CTouchEvent& ev,
        IZ_INT moveX, IZ_INT moveY);

    virtual void OnDragEnd(const izanagi::sys::CTouchEvent& ev);

    virtual IZ_BOOL OnDown();

public:
    void Init(
        AppGalleryApp* app,
        const izanagi::graph::SViewport& vp);

private:
    AppGalleryApp* m_App;
    izanagi::graph::SViewport m_Viewport;
};

/** Detect touch gesture.
 */
class GestureDetector
{
private:
    static GestureDetector s_Instance;

public:
    static GestureDetector& Instance();

private:
    GestureDetector();
    ~GestureDetector();

public:
    void Init(
        izanagi::IMemoryAllocator* allocator,
        AppGalleryApp* app,
        const izanagi::graph::SViewport& vp);

    void Update();

    IZ_BOOL PostTouchEvent(const izanagi::sys::CTouchEvent& ev);

private:
    izanagi::ui::CGestureDetector m_Detector;
    CGestureListener m_Listener;
};

#endif    // #if !defined(__GESTURE_LISTENER_H__)