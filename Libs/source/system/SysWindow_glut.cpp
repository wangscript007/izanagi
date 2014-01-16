extern "C" {
    #include <GL/glut.h>
}

#include "system/SysWindow.h"

namespace izanagi
{
namespace sys
{
    // ウインドのもろもろを保持しておく用
    class CWindowGLUT : public CPlacementNew {
    public:
        static CWindowGLUT* s_Instance;

    public:
        // インスタンス作成
        static CWindowGLUT* Create(
            IMemoryAllocator* allocator,
            CMessageHandler* handler);

        // インスタンス破棄
        static void Destroy(CWindowGLUT* window);

    protected:
        CWindowGLUT() {}

        ~CWindowGLUT() {}

    public:
        CMessageHandler* GetHandler() { return m_MsgHandler; }

    private:
        IMemoryAllocator* m_Allocator;
        CMessageHandler* m_MsgHandler;
    };

    CWindowGLUT* CWindowGLUT::s_Instance = IZ_NULL;

    // インスタンス作成
    CWindowGLUT* CWindowGLUT::Create(
        IMemoryAllocator* allocator,
        CMessageHandler* handler)
    {
        if (s_Instance == IZ_NULL) {
            void* buf = ALLOC(allocator, sizeof(CWindowGLUT));
            VRETURN_NULL(buf != IZ_NULL);

            CWindowGLUT* window = new(buf) CWindowGLUT();

            window->m_Allocator = allocator;
            window->m_MsgHandler = handler;

            s_Instance = window;
        }

        return s_Instance;
    }

    // インスタンス破棄
    void CWindowGLUT::Destroy(CWindowGLUT* window)
    {
        IMemoryAllocator* allocator = window->m_Allocator;

        delete window;
        FREE(allocator, window);
    }

    static void Display()
    {
        IZ_ASSERT(CWindowGLUT::s_Instance != IZ_NULL);

        CWindowGLUT::s_Instance->GetHandler()->OnIdle();
    }

    static void Idle()
    {
        glutPostRedisplay();
    }

    WindowHandle CSysWindow::Create(
        IMemoryAllocator* allocator,
        const WindowParams& param)
    {
        CWindowGLUT* window = CWindowGLUT::Create(allocator, param.handler);
        VRETURN_NULL(window != IZ_NULL);

        glutInit(
            const_cast<int*>(&param.argc), 
            param.argv);

        glutInitWindowSize(param.width, param.height);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

        glutCreateWindow(param.title);

        if (param.handler) {
            param.handler->OnInit(IZ_NULL);
        }

        glutDisplayFunc(Display);

        glutIdleFunc(Idle);

        glutMainLoop();

        return window;
    }

    // ウインドウ破棄.
    void CSysWindow::Destroy(WindowHandle handle)
    {
    }

    // ループ実行.
    void CSysWindow::RunLoop(const WindowHandle& handle)
    {
    }

    void* CSysWindow::GetNativeWindowHandle(const WindowHandle& handle)
    {
        return IZ_NULL;
    }

    void* CSysWindow::GetNativeDisplayHandle(const WindowHandle& handle)
    {
        return IZ_NULL;
    }

}   // namespace sys
}   // namespace izanagi