#if !defined(__IZANAGI_SYSTEM_KEYBOARD_H__)
#define __IZANAGI_SYSTEM_KEYBOARD_H__

#include "izStd.h"
#include "InputDeviceDefs.h"

namespace izanagi
{
namespace sys
{
	// キーボード
	enum E_KEYBOARD_BUTTON {
		E_KEYBOARD_BUTTON_ESCAPE       = DIK_ESCAPE,      
		E_KEYBOARD_BUTTON_1            = DIK_1,           
		E_KEYBOARD_BUTTON_2            = DIK_2,           
		E_KEYBOARD_BUTTON_3            = DIK_3,           
		E_KEYBOARD_BUTTON_4            = DIK_4,           
		E_KEYBOARD_BUTTON_5            = DIK_5,           
		E_KEYBOARD_BUTTON_6            = DIK_6,           
		E_KEYBOARD_BUTTON_7            = DIK_7,           
		E_KEYBOARD_BUTTON_8            = DIK_8,           
		E_KEYBOARD_BUTTON_9            = DIK_9,           
		E_KEYBOARD_BUTTON_0            = DIK_0,           
		E_KEYBOARD_BUTTON_MINUS        = DIK_MINUS,       
		E_KEYBOARD_BUTTON_EQUALS       = DIK_EQUALS,      
		E_KEYBOARD_BUTTON_BACK         = DIK_BACK,        
		E_KEYBOARD_BUTTON_TAB          = DIK_TAB,        
		E_KEYBOARD_BUTTON_Q            = DIK_Q,           
		E_KEYBOARD_BUTTON_W            = DIK_W,           
		E_KEYBOARD_BUTTON_E            = DIK_E,           
		E_KEYBOARD_BUTTON_R            = DIK_R,           
		E_KEYBOARD_BUTTON_T            = DIK_T,           
		E_KEYBOARD_BUTTON_Y            = DIK_Y,           
		E_KEYBOARD_BUTTON_U            = DIK_U,           
		E_KEYBOARD_BUTTON_I            = DIK_I,           
		E_KEYBOARD_BUTTON_O            = DIK_O,           
		E_KEYBOARD_BUTTON_P            = DIK_P,           
		E_KEYBOARD_BUTTON_LBRACKET     = DIK_LBRACKET,    
		E_KEYBOARD_BUTTON_RBRACKET     = DIK_RBRACKET,    
		E_KEYBOARD_BUTTON_RETURN       = DIK_RETURN,      
		E_KEYBOARD_BUTTON_LCONTROL     = DIK_LCONTROL,    
		E_KEYBOARD_BUTTON_A            = DIK_A,           
		E_KEYBOARD_BUTTON_S            = DIK_S,           
		E_KEYBOARD_BUTTON_D            = DIK_D,           
		E_KEYBOARD_BUTTON_F            = DIK_F,           
		E_KEYBOARD_BUTTON_G            = DIK_G,           
		E_KEYBOARD_BUTTON_H            = DIK_H,           
		E_KEYBOARD_BUTTON_J            = DIK_J,           
		E_KEYBOARD_BUTTON_K            = DIK_K,           
		E_KEYBOARD_BUTTON_L            = DIK_L,           
		E_KEYBOARD_BUTTON_SEMICOLON    = DIK_SEMICOLON,   
		E_KEYBOARD_BUTTON_APOSTROPHE   = DIK_APOSTROPHE,  
		E_KEYBOARD_BUTTON_GRAVE        = DIK_GRAVE,       
		E_KEYBOARD_BUTTON_LSHIFT       = DIK_LSHIFT,      
		E_KEYBOARD_BUTTON_BACKSLASH    = DIK_BACKSLASH,   
		E_KEYBOARD_BUTTON_Z            = DIK_Z,           
		E_KEYBOARD_BUTTON_X            = DIK_X,           
		E_KEYBOARD_BUTTON_C            = DIK_C,           
		E_KEYBOARD_BUTTON_V            = DIK_V,           
		E_KEYBOARD_BUTTON_B            = DIK_B,           
		E_KEYBOARD_BUTTON_N            = DIK_N,           
		E_KEYBOARD_BUTTON_M            = DIK_M,           
		E_KEYBOARD_BUTTON_COMMA        = DIK_COMMA,       
		E_KEYBOARD_BUTTON_PERIOD       = DIK_PERIOD,      
		E_KEYBOARD_BUTTON_SLASH        = DIK_SLASH,       
		E_KEYBOARD_BUTTON_RSHIFT       = DIK_RSHIFT,      
		E_KEYBOARD_BUTTON_MULTIPLY     = DIK_MULTIPLY,    
		E_KEYBOARD_BUTTON_LMENU        = DIK_LMENU,       
		E_KEYBOARD_BUTTON_SPACE        = DIK_SPACE,       
		E_KEYBOARD_BUTTON_CAPITAL      = DIK_CAPITAL,     
		E_KEYBOARD_BUTTON_F1           = DIK_F1,          
		E_KEYBOARD_BUTTON_F2           = DIK_F2,          
		E_KEYBOARD_BUTTON_F3           = DIK_F3,          
		E_KEYBOARD_BUTTON_F4           = DIK_F4,          
		E_KEYBOARD_BUTTON_F5           = DIK_F5,          
		E_KEYBOARD_BUTTON_F6           = DIK_F6,          
		E_KEYBOARD_BUTTON_F7           = DIK_F7,          
		E_KEYBOARD_BUTTON_F8           = DIK_F8,          
		E_KEYBOARD_BUTTON_F9           = DIK_F9,          
		E_KEYBOARD_BUTTON_F10          = DIK_F10,         
		E_KEYBOARD_BUTTON_NUMLOCK      = DIK_NUMLOCK,     
		E_KEYBOARD_BUTTON_SCROLL       = DIK_SCROLL,      
		E_KEYBOARD_BUTTON_NUMPAD7      = DIK_NUMPAD7,     
		E_KEYBOARD_BUTTON_NUMPAD8      = DIK_NUMPAD8,     
		E_KEYBOARD_BUTTON_NUMPAD9      = DIK_NUMPAD9,     
		E_KEYBOARD_BUTTON_SUBTRACT     = DIK_SUBTRACT,    
		E_KEYBOARD_BUTTON_NUMPAD4      = DIK_NUMPAD4,     
		E_KEYBOARD_BUTTON_NUMPAD5      = DIK_NUMPAD5,     
		E_KEYBOARD_BUTTON_NUMPAD6      = DIK_NUMPAD6,     
		E_KEYBOARD_BUTTON_ADD          = DIK_ADD,         
		E_KEYBOARD_BUTTON_NUMPAD1      = DIK_NUMPAD1,     
		E_KEYBOARD_BUTTON_NUMPAD2      = DIK_NUMPAD2,     
		E_KEYBOARD_BUTTON_NUMPAD3      = DIK_NUMPAD3,     
		E_KEYBOARD_BUTTON_NUMPAD0      = DIK_NUMPAD0,     
		E_KEYBOARD_BUTTON_DECIMAL      = DIK_DECIMAL,     
		E_KEYBOARD_BUTTON_OEM_102      = DIK_OEM_102,     
		E_KEYBOARD_BUTTON_F11          = DIK_F11,         
		E_KEYBOARD_BUTTON_F12          = DIK_F12,         
		E_KEYBOARD_BUTTON_F13          = DIK_F13,         
		E_KEYBOARD_BUTTON_F14          = DIK_F14,         
		E_KEYBOARD_BUTTON_F15          = DIK_F15,         
		E_KEYBOARD_BUTTON_KANA         = DIK_KANA,        
		E_KEYBOARD_BUTTON_ABNT_C1      = DIK_ABNT_C1,     
		E_KEYBOARD_BUTTON_CONVERT      = DIK_CONVERT,     
		E_KEYBOARD_BUTTON_NOCONVERT    = DIK_NOCONVERT,   
		E_KEYBOARD_BUTTON_YEN          = DIK_YEN,         
		E_KEYBOARD_BUTTON_ABNT_C2      = DIK_ABNT_C2,     
		E_KEYBOARD_BUTTON_NUMPADEQUALS = DIK_NUMPADEQUALS,
		E_KEYBOARD_BUTTON_PREVTRACK    = DIK_PREVTRACK,   
		E_KEYBOARD_BUTTON_AT           = DIK_AT,          
		E_KEYBOARD_BUTTON_COLON        = DIK_COLON,       
		E_KEYBOARD_BUTTON_UNDERLINE    = DIK_UNDERLINE,   
		E_KEYBOARD_BUTTON_KANJI        = DIK_KANJI,       
		E_KEYBOARD_BUTTON_STOP         = DIK_STOP,       
		E_KEYBOARD_BUTTON_AX           = DIK_AX,          
		E_KEYBOARD_BUTTON_UNLABELED    = DIK_UNLABELED,   
		E_KEYBOARD_BUTTON_NEXTTRACK    = DIK_NEXTTRACK,   
		E_KEYBOARD_BUTTON_NUMPADENTER  = DIK_NUMPADENTER, 
		E_KEYBOARD_BUTTON_RCONTROL     = DIK_RCONTROL,    
		E_KEYBOARD_BUTTON_MUTE         = DIK_MUTE,        
		E_KEYBOARD_BUTTON_CALCULATOR   = DIK_CALCULATOR,  
		E_KEYBOARD_BUTTON_PLAYPAUSE    = DIK_PLAYPAUSE,   
		E_KEYBOARD_BUTTON_MEDIASTOP    = DIK_MEDIASTOP,   
		E_KEYBOARD_BUTTON_VOLUMEDOWN   = DIK_VOLUMEDOWN,  
		E_KEYBOARD_BUTTON_VOLUMEUP     = DIK_VOLUMEUP,    
		E_KEYBOARD_BUTTON_WEBHOME      = DIK_WEBHOME,     
		E_KEYBOARD_BUTTON_NUMPADCOMMA  = DIK_NUMPADCOMMA, 
		E_KEYBOARD_BUTTON_DIVIDE       = DIK_DIVIDE,      
		E_KEYBOARD_BUTTON_SYSRQ        = DIK_SYSRQ,       
		E_KEYBOARD_BUTTON_RMENU        = DIK_RMENU,       
		E_KEYBOARD_BUTTON_PAUSE        = DIK_PAUSE,       
		E_KEYBOARD_BUTTON_HOME         = DIK_HOME,        
		E_KEYBOARD_BUTTON_UP           = DIK_UP,          
		E_KEYBOARD_BUTTON_PRIOR        = DIK_PRIOR,       
		E_KEYBOARD_BUTTON_LEFT         = DIK_LEFT,        
		E_KEYBOARD_BUTTON_RIGHT        = DIK_RIGHT,       
		E_KEYBOARD_BUTTON_END          = DIK_END,         
		E_KEYBOARD_BUTTON_DOWN         = DIK_DOWN,        
		E_KEYBOARD_BUTTON_NEXT         = DIK_NEXT,        
		E_KEYBOARD_BUTTON_INSERT       = DIK_INSERT,      
		E_KEYBOARD_BUTTON_DELETE       = DIK_DELETE,      
		E_KEYBOARD_BUTTON_LWIN         = DIK_LWIN,        
		E_KEYBOARD_BUTTON_RWIN         = DIK_RWIN,        
		E_KEYBOARD_BUTTON_APPS         = DIK_APPS,        
		E_KEYBOARD_BUTTON_POWER        = DIK_POWER,       
		E_KEYBOARD_BUTTON_SLEEP        = DIK_SLEEP,       
		E_KEYBOARD_BUTTON_WAKE         = DIK_WAKE,        
		E_KEYBOARD_BUTTON_WEBSEARCH    = DIK_WEBSEARCH,   
		E_KEYBOARD_BUTTON_WEBFAVORITES = DIK_WEBFAVORITES,
		E_KEYBOARD_BUTTON_WEBREFRESH   = DIK_WEBREFRESH,  
		E_KEYBOARD_BUTTON_WEBSTOP      = DIK_WEBSTOP,     
		E_KEYBOARD_BUTTON_WEBFORWARD   = DIK_WEBFORWARD,  
		E_KEYBOARD_BUTTON_WEBBACK      = DIK_WEBBACK,     
		E_KEYBOARD_BUTTON_MYCOMPUTER   = DIK_MYCOMPUTER,  
		E_KEYBOARD_BUTTON_MAIL         = DIK_MAIL,        
		E_KEYBOARD_BUTTON_MEDIASELECT  = DIK_MEDIASELECT, 

		E_KEYBOARD_BUTTON_BACKSPACE    = DIK_BACKSPACE,   
		E_KEYBOARD_BUTTON_NUMPADSTAR   = DIK_NUMPADSTAR,  
		E_KEYBOARD_BUTTON_LALT         = DIK_LALT,        
		E_KEYBOARD_BUTTON_CAPSLOCK     = DIK_CAPSLOCK,    
		E_KEYBOARD_BUTTON_NUMPADMINUS  = DIK_NUMPADMINUS, 
		E_KEYBOARD_BUTTON_NUMPADPLUS   = DIK_NUMPADPLUS,  
		E_KEYBOARD_BUTTON_NUMPADPERIOD = DIK_NUMPADPERIOD,
		E_KEYBOARD_BUTTON_NUMPADSLASH  = DIK_NUMPADSLASH, 
		E_KEYBOARD_BUTTON_RALT         = DIK_RALT,        
		E_KEYBOARD_BUTTON_UPARROW      = DIK_UPARROW,     
		E_KEYBOARD_BUTTON_PGUP         = DIK_PGUP,        
		E_KEYBOARD_BUTTON_LEFTARROW    = DIK_LEFTARROW,   
		E_KEYBOARD_BUTTON_RIGHTARROW   = DIK_RIGHTARROW,  
		E_KEYBOARD_BUTTON_DOWNARROW    = DIK_DOWNARROW,   
		E_KEYBOARD_BUTTON_PGDN         = DIK_PGDN,        
	};

	/** キーボード
	 */
	class CKeyboard : public CObject
    {
	public:
		// インスタンス作成
		static CKeyboard* CreateKeyboard(
            IMemoryAllocator* allocator,
            SInputDeviceInitParam* initParam);

	protected:
		CKeyboard();
		virtual ~CKeyboard();

		NO_COPIABLE(CKeyboard);

        IZ_DEFINE_INTERNAL_RELEASE();

	public:
		// 更新
		PURE_VIRTUAL(IZ_BOOL Update());

	public:
		// キーを押し続けているか
		IZ_BOOL IsPresshKey(E_KEYBOARD_BUTTON key)
        {
		    IZ_BOOL ret = IZ_FALSE;
		    if (m_bSucceedUpdate) {
			    ret = (m_CurState[key] > 0);
		    }
		    return ret;
	    }

		// キーを一度だけ押したかどうか
		IZ_BOOL IsPressOneShotKey(E_KEYBOARD_BUTTON key)
        {
		    IZ_BOOL ret = IZ_FALSE;
		    if (m_bSucceedUpdate) {
			    ret = ((m_CurState[key] > 0) && (m_LastState[key] == 0));
		    }
		    return ret;
	    }

        // キーが離されたかどうか
        IZ_BOOL IsReleaseKey(E_KEYBOARD_BUTTON key)
        {
            IZ_BOOL ret = IZ_FALSE;
		    if (m_bSucceedUpdate)
            {
                ret = (m_CurState[key] == 0
                        && m_LastState[key] > 0);
            }
            return ret;
        }

        const IZ_UINT8* GetCurState() const
        {
            return m_CurState;
        }

        const IZ_UINT8* GetLastState() const
        {
            return m_LastState;
        }

	protected:
		enum {
			KEY_NUM = 256,
		};

	protected:
		IMemoryAllocator* m_Allocator;

		// キーバッファ
		IZ_UINT8 m_CurState[KEY_NUM];
		IZ_UINT8 m_LastState[KEY_NUM];

		// 更新に成功したかどうか
		IZ_BOOL m_bSucceedUpdate;
	};
}   // namespace sys
}	// namespace izanagi

#endif	// #if !defined(__IZANAGI_SYSTEM_KEYBOARD_H__)
