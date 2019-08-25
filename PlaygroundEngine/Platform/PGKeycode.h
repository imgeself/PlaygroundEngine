#pragma once

typedef int KeyCode;

#ifdef PLATFORM_WINDOWS
#include <Windows.h>

#define PGKEY_UNKNOWN            -1

#define PGKEY_NP_0               VK_NUMPAD0
#define PGKEY_NP_1               VK_NUMPAD1
#define PGKEY_NP_2               VK_NUMPAD2
#define PGKEY_NP_3               VK_NUMPAD3
#define PGKEY_NP_4               VK_NUMPAD4
#define PGKEY_NP_5               VK_NUMPAD5
#define PGKEY_NP_6               VK_NUMPAD6
#define PGKEY_NP_7               VK_NUMPAD7
#define PGKEY_NP_8               VK_NUMPAD8
#define PGKEY_NP_9               VK_NUMPAD9

#define PGKEY_0                  48
#define PGKEY_1                  49
#define PGKEY_2                  50
#define PGKEY_3                  51
#define PGKEY_4                  52
#define PGKEY_5                  53
#define PGKEY_6                  54
#define PGKEY_7                  55
#define PGKEY_8                  56
#define PGKEY_9                  57

#define PGKEY_A                  65
#define PGKEY_B                  66
#define PGKEY_C                  67
#define PGKEY_D                  68
#define PGKEY_E                  69
#define PGKEY_F                  70
#define PGKEY_G                  71
#define PGKEY_H                  72
#define PGKEY_I                  73
#define PGKEY_J                  74
#define PGKEY_K                  75
#define PGKEY_L                  76
#define PGKEY_M                  77
#define PGKEY_N                  78
#define PGKEY_O                  79
#define PGKEY_P                  80
#define PGKEY_Q                  81
#define PGKEY_R                  82
#define PGKEY_S                  83
#define PGKEY_T                  84
#define PGKEY_U                  85
#define PGKEY_V                  86
#define PGKEY_W                  87
#define PGKEY_X                  88
#define PGKEY_Y                  89
#define PGKEY_Z                  90

#define PGKEY_SEMICOLON          VK_OEM_1       /* ; */
#define PGKEY_SLASH              VK_OEM_2       /* / */
#define PGKEY_APOSTROPHE         VK_OEM_3       /* ` */
#define PGKEY_LEFT_BRACKET       VK_OEM_4       /* [ */
#define PGKEY_BACKSLASH          VK_OEM_5       /* \ */
#define PGKEY_RIGHT_BRACKET      VK_OEM_6       /* ] */
#define PGKEY_QUOTE              VK_OEM_7       /* ' */
#define PGKEY_COMMA              VK_OEM_COMMA   /* , */
#define PGKEY_MINUS              VK_OEM_MINUS   /* - */
#define PGKEY_PERIOD             VK_OEM_PERIOD  /* . */
#define PGKEY_EQUAL              VK_OEM_PLUS    /* = */

#define PGKEY_SPACE              VK_SPACE
#define PGKEY_ESCAPE             VK_ESCAPE
#define PGKEY_ENTER              VK_RETURN
#define PGKEY_TAB                VK_TAB
#define PGKEY_BACKSPACE          VK_BACK
#define PGKEY_INSERT             VK_INSERT
#define PGKEY_DELETE             VK_DELETE
#define PGKEY_RIGHT              VK_RIGHT
#define PGKEY_LEFT               VK_LEFT
#define PGKEY_DOWN               VK_DOWN
#define PGKEY_UP                 VK_UP
#define PGKEY_PAGE_UP            VK_PRIOR
#define PGKEY_PAGE_DOWN          VK_NEXT
#define PGKEY_HOME               VK_HOME
#define PGKEY_END                VK_END
#define PGKEY_CAPS_LOCK          VK_CAPITAL
#define PGKEY_NUM_LOCK           VK_NUMLOCK
#define PGKEY_PAUSE              VK_PAUSE

#define PGKEY_F1                 VK_F1
#define PGKEY_F2                 VK_F2
#define PGKEY_F3                 VK_F3
#define PGKEY_F4                 VK_F4
#define PGKEY_F5                 VK_F5
#define PGKEY_F6                 VK_F6
#define PGKEY_F7                 VK_F7
#define PGKEY_F8                 VK_F8
#define PGKEY_F9                 VK_F9
#define PGKEY_F10                VK_F10
#define PGKEY_F11                VK_F11
#define PGKEY_F12                VK_F12
#define PGKEY_F13                VK_F13
#define PGKEY_F14                VK_F14
#define PGKEY_F15                VK_F15
#define PGKEY_F16                VK_F16
#define PGKEY_F17                VK_F17
#define PGKEY_F18                VK_F18
#define PGKEY_F19                VK_F19
#define PGKEY_F20                VK_F20
#define PGKEY_F21                VK_F21
#define PGKEY_F22                VK_F22
#define PGKEY_F23                VK_F23
#define PGKEY_F24                VK_F24

#define PGKEY_LEFT_SHIFT         VK_LSHIFT
#define PGKEY_LEFT_CONTROL       VK_LCONTROL
#define PGKEY_LEFT_ALT           VK_LMENU
#define PGKEY_LEFT_SYSTEM        VK_LWIN
#define PGKEY_RIGHT_SHIFT        VK_RSHIFT
#define PGKEY_RIGHT_CONTROL      VK_RCONTROL
#define PGKEY_RIGHT_ALT          VK_RMENU
#define PGKEY_RIGHT_SYSTEM       VK_RWIN

#endif
