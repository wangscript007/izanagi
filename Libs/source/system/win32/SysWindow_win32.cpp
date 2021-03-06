#include "system/SysWindow.h"
#include <zmouse.h>

namespace izanagi
{
namespace sys
{
    //////////////////////////////////////////

    class CMsgHandlerManager : public CStdHash<IZ_UINT64, CMessageHandler, 4> {
    public:
        CMsgHandlerManager() {}
        ~CMsgHandlerManager() {}

    public:
        IZ_BOOL Register(
            CMessageHandler* handler,
            HWND hWnd);
    };

    IZ_BOOL CMsgHandlerManager::Register(
        CMessageHandler* handler,
        HWND hWnd)
    {
        IZ_ASSERT(handler != IZ_NULL);
        //IZ_ASSERT(hWnd != IZ_NULL);

        IZ_UINT64 id = (IZ_UINT64)hWnd;
        handler->mHashItem.Init(id, handler);
    
        IZ_BOOL ret = Add(&handler->mHashItem);
        return ret;
    }

    static CMsgHandlerManager sMsgHandlerMgr;

    //////////////////////////////////////////

    class InputManager {
    public:
        InputManager() {}
        ~InputManager() {}

    public:
        IZ_BOOL Register(
            HWND hWnd,
            CKeyboard* keyboard);

        IZ_BOOL Register(
            HWND hWnd,
            CMouse* mouse);

        void Remove(IZ_UINT64 hWnd);

        CKeyboard* FindKeyboard(IZ_UINT64 hWnd);
        CMouse* FindMouse(IZ_UINT64 hWnd);

    private:
        CStdHash<IZ_UINT64, CKeyboard, 4> m_hashKeyboard;
        CStdHash<IZ_UINT64, CMouse, 4> m_hashMouse;
    };

    IZ_BOOL InputManager::Register(
        HWND hWnd,
        CKeyboard* keyboard)
    {
        IZ_ASSERT(keyboard != IZ_NULL);
        IZ_ASSERT(hWnd != IZ_NULL);

        IZ_UINT64 id = (IZ_UINT64)hWnd;
        keyboard->GetHashItem()->Init(id, keyboard);

        IZ_BOOL ret = m_hashKeyboard.Add(keyboard->GetHashItem());

        return ret;
    }

    IZ_BOOL InputManager::Register(
        HWND hWnd,
        CMouse* mouse)
    {
        IZ_ASSERT(mouse != IZ_NULL);
        IZ_ASSERT(hWnd != IZ_NULL);

        IZ_UINT64 id = (IZ_UINT64)hWnd;
        mouse->GetHashItem()->Init(id, mouse);

        IZ_BOOL ret = m_hashMouse.Add(mouse->GetHashItem());

        return ret;
    }

    void InputManager::Remove(IZ_UINT64 hWnd)
    {
        auto keyboard = m_hashKeyboard.FindData(hWnd);
        if (keyboard) {
            keyboard->GetHashItem()->Leave();
        }

        auto mouse = m_hashMouse.FindData(hWnd);
        if (mouse) {
            mouse->GetHashItem()->Leave();
        }
    }

    CKeyboard* InputManager::FindKeyboard(IZ_UINT64 hWnd)
    {
        auto keyboard = m_hashKeyboard.FindData(hWnd);
        return keyboard;
    }

    CMouse* InputManager::FindMouse(IZ_UINT64 hWnd)
    {
        auto mouse = m_hashMouse.FindData(hWnd);
        return mouse;
    }

    static InputManager sInputMgr;

    //////////////////////////////////////////

    // ウインドのもろもろを保持しておく用
    class CWindow : public CPlacementNew {
    public:
        // インスタンス作成
        static CWindow* Create(
            IMemoryAllocator* allocator,
            HINSTANCE hInst,
            HWND hWnd,
            HDC hDC);

        // インスタンス破棄
        static void Destroy(CWindow* window);

    protected:
        CWindow(HINSTANCE hInst, HWND hWnd, HDC hDC)
        {
            m_hInst = hInst;
            m_hWnd = hWnd;
            m_hDC = hDC;
        }

        ~CWindow() {}

    public:
        HINSTANCE GetHINSTANCE() { return m_hInst; }
        HWND GetHWND() { return m_hWnd; }
        HDC GetHDC() { return m_hDC; }

        CKeyboard* GetKeyboard()
        {
            return &m_keyboard;
        }

        CMouse* GetMouse()
        {
            return &m_mouse;
        }

    protected:
        IMemoryAllocator* m_Allocator;
        HINSTANCE m_hInst;
        HWND m_hWnd;
        HDC m_hDC;

        CKeyboard m_keyboard;
        CMouse m_mouse;
    };

    // インスタンス作成
    CWindow* CWindow::Create(
        IMemoryAllocator* allocator,
        HINSTANCE hInst,
        HWND hWnd,
        HDC hDC)
    {
        void* buf = ALLOC_ZERO(allocator, sizeof(CWindow));
        VRETURN_NULL(buf != IZ_NULL);

        CWindow* window = new(buf) CWindow(hInst, hWnd, hDC);
        window->m_Allocator = allocator;

        return window;
    }

    // インスタンス破棄
    void CWindow::Destroy(CWindow* window)
    {
        IMemoryAllocator* allocator = window->m_Allocator;

        delete window;
        FREE(allocator, window);
    }

    //////////////////////////////////////////

    static CWindow* s_window = nullptr;

    LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        CMessageHandler* msgHandler = sMsgHandlerMgr.FindData((IZ_UINT64)hWnd);
        CMessageHandler* msgHandlerEx = sMsgHandlerMgr.FindData((IZ_UINT64)0);

        auto keyboard = sInputMgr.FindKeyboard((IZ_UINT64)hWnd);
        auto mouse = sInputMgr.FindMouse((IZ_UINT64)hWnd);

        switch (uMsg) {
        case WM_DESTROY:
        case WM_CLOSE:
            ::PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            if (msgHandler) {
                msgHandler->OnPaint();
                return 0;
            }
            break;

        case WM_ERASEBKGND:
            if (msgHandler) {
                msgHandler->OnPaint();
                return 0;
            }
            break;

        case WM_KEYDOWN:
        {
            E_KEYBOARD_BUTTON key = CSysWindow::GetKeyMap((IZ_UINT)wParam);
            if (msgHandler) {
                msgHandler->OnKeyDown(key);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnKeyDown(key);
            }
            if (keyboard) {
                keyboard->PressKey(key);
            }
        }
            break;

        case WM_KEYUP:
        {
            E_KEYBOARD_BUTTON key = CSysWindow::GetKeyMap((IZ_UINT)wParam);
            if (msgHandler) {
                msgHandler->OnKeyUp(key);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnKeyUp(key);
            }
            if (keyboard) {
                keyboard->ReleaseKey(key);
            }
        }
            break;

        case WM_LBUTTONDOWN:
        {
            CIntPoint point(
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam)));

            ::SetCapture(hWnd);

            if (msgHandler) {
                msgHandler->OnMouseLBtnDown(point);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseLBtnDown(point);
            }
        }
            break;

        case WM_LBUTTONUP:
        {
            CIntPoint point(
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam)));

            ::ReleaseCapture();

            if (msgHandler) {
                msgHandler->OnMouseLBtnUp(point);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseLBtnUp(point);
            }
        }
            break;

        case WM_RBUTTONDOWN:
        {
            CIntPoint point(
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam)));

            ::SetCapture(hWnd);

            if (msgHandler) {
                msgHandler->OnMouseRBtnDown(point);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseLBtnUp(point);
            }
        }
            break;

        case WM_RBUTTONUP:
        {
            CIntPoint point(
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam)));

            ::ReleaseCapture();

            if (msgHandler) {
                msgHandler->OnMouseRBtnUp(point);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseRBtnUp(point);
            }
        }
            break;

        case WM_MOUSEMOVE:
        {
            CIntPoint point(
                static_cast<short>(LOWORD(lParam)),
                static_cast<short>(HIWORD(lParam)));

            if (msgHandler) {
                msgHandler->OnMouseMove(point);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseMove(point);
            }
        }
            break;

        case WM_MOUSEWHEEL:
        {
            IZ_INT zDelta = static_cast<short>(HIWORD(wParam));

            if (msgHandler) {
                msgHandler->OnMouseWheel(zDelta);
            }
            if (msgHandlerEx) {
                msgHandlerEx->OnMouseWheel(zDelta);
            }
        }
            break;
        }

        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    // コンソールイベントを処理
    BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
    {
        switch (dwCtrlType) {
        case CTRL_C_EVENT:
            // コンソールのみを閉じる
            FreeConsole();
            return TRUE;
        }

        return FALSE;
    }

    static const char* registerName = "izanagi";

    static inline void _Destroy(
        HDC hDC,
        HWND hWnd,
        HINSTANCE hInst)
    {
        if (hDC) {
            ::ReleaseDC(hWnd, hDC);
        }

        if (hWnd) {
            DestroyWindow(hWnd);
        }

        if (hInst) {
            UnregisterClass(registerName, hInst);
        }
    }

    // ウインドウ破棄.
    void CSysWindow::Destroy(WindowHandle handle)
    {
        IZ_ASSERT(handle != IZ_NULL);

        CWindow* window = (CWindow*)handle;

        HDC hDC = NULL;
        HWND hWnd = NULL;
        HINSTANCE hInst = NULL;

        if (window != IZ_NULL) {
            hDC = window->GetHDC();
            hWnd = window->GetHWND();
            hInst = window->GetHINSTANCE();

            sInputMgr.Remove((IZ_INT64)hWnd);
        }

        _Destroy(hDC, hWnd, hInst);

        CWindow::Destroy(window);

        s_window = nullptr;
    }

    // ウインドウ作成.
    WindowHandle CSysWindow::Create(
        IMemoryAllocator* allocator,
        const WindowParams& param)
    {
        IZ_ASSERT(allocator != IZ_NULL);

        //HINSTANCE hInst = (HINSTANCE)param.platformParam;
        HINSTANCE hInst = GetModuleHandleA(NULL);
        IZ_ASSERT(hInst != IZ_NULL);

        HDC hDC = NULL;
        HWND hWnd = NULL;

        IZ_BOOL result = IZ_TRUE;
        CWindow* window = IZ_NULL;

        WNDCLASSEX wcex;
        {
            wcex.cbSize         = sizeof(WNDCLASSEX);
            wcex.style          = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc    = WndProc;
            wcex.cbClsExtra     = 0;
            wcex.cbWndExtra     = 0;
            wcex.hInstance      = hInst;
            wcex.hIcon          = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
            wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
            //wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1); // システムカラー（白色）
            wcex.hbrBackground  = (HBRUSH)::GetStockObject(BLACK_BRUSH);
            wcex.lpszMenuName   = NULL;
            wcex.lpszClassName  = (param.title != IZ_NULL ? param.title : registerName);
            wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

            VRETURN_NULL(RegisterClassEx(&wcex));
        }

        DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        DWORD exStyle = 0;

        RECT rect = {
            0, 0,
            param.width,
            param.height
        };

        if (param.isWindowed) {
            style |= WS_OVERLAPPEDWINDOW;

            // TODO
            // 最大化、最小化ボタンは無し
            //style &= ~WS_MINIMIZEBOX;
            style &= ~WS_MAXIMIZEBOX;

            // TODO
            // サイズ変更無効化
            style &= ~WS_THICKFRAME;

            exStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

            // ウインドウモード
            result = ::AdjustWindowRect(
                &rect,
                style,
                FALSE);
            VGOTO(result, __EXIT__);

            rect.right = rect.right - rect.left;
            rect.bottom = rect.bottom - rect.top;
            rect.left = CW_USEDEFAULT;
            rect.top = CW_USEDEFAULT;
        }
        else {
            // フルスクリーンモード
            style |= WS_POPUP;
            exStyle = WS_EX_APPWINDOW;
        }

        // ウインドウ作成
        hWnd = ::CreateWindowExA(
            exStyle,
            wcex.lpszClassName,
            param.title,
            style,
            rect.left, rect.top,
            rect.right, rect.bottom,
            NULL,
            NULL,
            hInst,
            NULL);
        VGOTO(result = (hWnd != NULL), __EXIT__);

        // ウインドウ表示
        ::ShowWindow(hWnd, SW_SHOWNORMAL);
        ::SetFocus(hWnd);
        ::SetForegroundWindow(hWnd);

        ::UpdateWindow(hWnd);

        hDC = ::GetDC(hWnd);
        VGOTO(result = (hDC != NULL), __EXIT__);

        window = CWindow::Create(
            allocator,
            hInst,
            hWnd,
            hDC);
        VGOTO(result = (window != IZ_NULL), __EXIT__);

        // メッセージハンドラを保持
        if (param.handler) {
            sMsgHandlerMgr.Register(param.handler, hWnd);

            if (window) {
                param.handler->OnInit(window);
            }
        }

        // コンソールウインドウの閉じるを無効にする
        HWND hConsoleWnd = ::GetConsoleWindow();
        if (hConsoleWnd != NULL) {
            HMENU hMenu = ::GetSystemMenu(hConsoleWnd, FALSE);
		    ::RemoveMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        }

        // コンソールウインドウ用のハンドラを設定
        ::SetConsoleCtrlHandler(HandlerRoutine, TRUE);

__EXIT__:
        if (result) {
            sInputMgr.Register(hWnd, window->GetKeyboard());
            sInputMgr.Register(hWnd, window->GetMouse());

            s_window = window;
        }
        else {
            _Destroy(hDC, hWnd, hInst);
        }

        return window;
    }

    static IZ_BOOL ProcMsg()
    {
        MSG msg;

        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return IZ_FALSE;
            }
            else {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }
        }
    
        return IZ_TRUE;
    }

    // ループ実行.
    void CSysWindow::RunLoop(WindowHandle handle)
    {
        IZ_ASSERT(handle != IZ_NULL);
    
        CWindow* window = (CWindow*)handle;
        HWND hWnd = window->GetHWND();

        CMessageHandler* msgHandler = sMsgHandlerMgr.FindData((IZ_UINT64)hWnd);
        CMessageHandler* msgHandlerEx = sMsgHandlerMgr.FindData((IZ_UINT64)0);

        while (ProcMsg()) {
            if (msgHandler) {
                msgHandler->OnIdle();
            }
        }

        if (msgHandler) {
            msgHandler->OnTerminate();
        }
        if (msgHandlerEx) {
            msgHandlerEx->OnTerminate();
        }

        Destroy(handle);

        if (msgHandler) {
            msgHandler->OnDestroy();
        }
        if (msgHandlerEx) {
            msgHandlerEx->OnDestroy();
        }
    }

    void* CSysWindow::GetNativeWindowHandle(const WindowHandle& handle)
    {
        IZ_ASSERT(handle != IZ_NULL);
    
        CWindow* window = (CWindow*)handle;
        return window->GetHWND();
    }

    void* CSysWindow::GetNativeDisplayHandle(const WindowHandle& handle)
    {
        CWindow* window = (CWindow*)handle;
        return window->GetHDC();
    }

    E_KEYBOARD_BUTTON CSysWindow::GetKeyMap(IZ_UINT key)
    {
        if ('0' <= key && key <= '9') {
            key = key - '0';
            return (E_KEYBOARD_BUTTON)(E_KEYBOARD_BUTTON_0 + key);
        }
        else if ('A' <= key && key <= 'Z') {
            key = key - 'A';
            return (E_KEYBOARD_BUTTON)(E_KEYBOARD_BUTTON_A + key);
        }
        else if (VK_F1 <= key && key <= VK_F12) {
            key = key - VK_F1;
            return (E_KEYBOARD_BUTTON)(E_KEYBOARD_BUTTON_F1 + key);
        }
        else {
            switch (key) {
            case VK_UP:
                return E_KEYBOARD_BUTTON_UP;
            case VK_LEFT:
                return E_KEYBOARD_BUTTON_LEFT;
            case VK_DOWN:
                return E_KEYBOARD_BUTTON_DOWN;
            case VK_RIGHT:
                return E_KEYBOARD_BUTTON_RIGHT;
            case VK_CONTROL:
                return E_KEYBOARD_BUTTON_CONTROL;
            case VK_SHIFT:
                return E_KEYBOARD_BUTTON_SHIFT;
            case VK_RETURN:
                return E_KEYBOARD_BUTTON_RETURN;
            case VK_SPACE:
                return E_KEYBOARD_BUTTON_SPACE;
            case VK_BACK:
                return E_KEYBOARD_BUTTON_BACK;
            case VK_DELETE:
                return E_KEYBOARD_BUTTON_DELETE;
            default:
                break;
            }
        }

        //IZ_ASSERT(IZ_FALSE);
        return E_KEYBOARD_BUTTON_UNDEFINED;
    }

    IZ_INT CSysWindow::GetOriginalKey(E_KEYBOARD_BUTTON key)
    {
        if (E_KEYBOARD_BUTTON_0 <= key && key <= E_KEYBOARD_BUTTON_9) {
            auto c = key - E_KEYBOARD_BUTTON_0 + '0';
            return c;
        }
        else if (E_KEYBOARD_BUTTON_A <= key && key <= E_KEYBOARD_BUTTON_Z) {
            auto c = key - E_KEYBOARD_BUTTON_A + 'A';
            return c;
        }
        else if (E_KEYBOARD_BUTTON_F1 <= key && key <= E_KEYBOARD_BUTTON_F12) {
            auto c = key - E_KEYBOARD_BUTTON_F1 + VK_F1;
            return c;
        }
        else {
            switch (key) {
            case E_KEYBOARD_BUTTON_UP:
                return VK_UP;
            case E_KEYBOARD_BUTTON_LEFT:
                return VK_LEFT;
            case E_KEYBOARD_BUTTON_DOWN:
                return VK_DOWN;
            case E_KEYBOARD_BUTTON_RIGHT:
                return VK_RIGHT;
            case E_KEYBOARD_BUTTON_CONTROL:
                return VK_CONTROL;
            case E_KEYBOARD_BUTTON_SHIFT:
                return VK_SHIFT;
            case E_KEYBOARD_BUTTON_RETURN:
                return VK_RETURN;
            case E_KEYBOARD_BUTTON_SPACE:
                return VK_SPACE;
            case E_KEYBOARD_BUTTON_BACK:
                return VK_BACK;
            case E_KEYBOARD_BUTTON_DELETE:
                return VK_DELETE;
            case E_KEYBOARD_BUTTON_TAB:
                return VK_TAB;
            case E_KEYBOARD_BUTTON_PRIOR:
                return VK_PRIOR;
            case E_KEYBOARD_BUTTON_NEXT:
                return VK_NEXT;
            case E_KEYBOARD_BUTTON_HOME:
                return VK_HOME;
            case E_KEYBOARD_BUTTON_END:
                return VK_END;
            case E_KEYBOARD_BUTTON_ESCAPE:
                return VK_ESCAPE;
            default:
                break;
            }
        }

        return -1;
    }

    void CSysWindow::HideCursor()
    {
        ::SetCursor(NULL);
    }

    CIntPoint CSysWindow::getDisplaySize()
    {
        IZ_ASSERT(s_window);

        auto hWnd = s_window->GetHWND();

        RECT rect;
        GetClientRect(hWnd, &rect);

        CIntPoint ret(
            rect.right - rect.left,
            rect.bottom - rect.top);

        return std::move(ret);
    }

    IZ_BOOL CSysWindow::registerExtendMsgHandler(CMessageHandler* handler)
    {
        auto h = sMsgHandlerMgr.Find(0);

        if (h) {
            return IZ_FALSE;
        }
        else {
            sMsgHandlerMgr.Register(handler, 0);
            return IZ_TRUE;
        }
    }

    CKeyboard* CSysWindow::GetKeyboard(const WindowHandle& handle)
    {
        CWindow* window = (CWindow*)handle;
        auto hWnd = window->GetHWND();
        return sInputMgr.FindKeyboard((IZ_INT64)hWnd);
    }

    CMouse* CSysWindow::GetMouse(const WindowHandle& handle)
    {
        CWindow* window = (CWindow*)handle;
        auto hWnd = window->GetHWND();
        return sInputMgr.FindMouse((IZ_INT64)hWnd);
    }
}   // namespace sys
}   // namespace izanagi
