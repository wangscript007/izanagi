#if !defined(__IZANAGI_SYSTEM_KEYBOARD_DINPUT_H__)
#define __IZANAGI_SYSTEM_KEYBOARD_DINPUT_H__

#include "izDInput.h"
#include "izStd.h"
#include "InputDeviceDefs.h"
#include "Keyboard.h"

namespace izanagi
{
namespace sys
{
    /**
    * キーボード
    */
    class CKeyboardDInput : public CKeyboard
    {
        friend class CKeyboard;

    private:
        // インスタンス作成
        static CKeyboard* CreateKeyboard(
            IMemoryAllocator* allocator,
            SInputDeviceInitParam* initParam);

    protected:
        CKeyboardDInput();
        virtual ~CKeyboardDInput();

    public:
        // 初期化
        IZ_BOOL Init(SInputDeviceInitParam* initParam);

        // 更新
        virtual IZ_BOOL Update();

    protected:
        D_INPUT_DEVICE* m_pKeyDevice;
        HWND m_hWnd;
    };
}   // namespace sys
}   // namespace izanagi

#endif  // #if !defined(__IZANAGI_SYSTEM_KEYBOARD_DINPUT_H__)
