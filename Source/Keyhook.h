//keyhook.h

#ifndef _INC_KEYHOOK_H
#define _INC_KEYHOOK_H

//Version control (compare using GetVersion):
#define MAJOR_VERSION	0
#define MINOR_VERSION	4
#define REVISION		0


//Default message IDs for monitor call-backs:
#define WM_USER_KEYDOWN		WM_USER + 0x0100
#define WM_USER_KEYUP		WM_USER + 0x0101
#define WM_USER_KEYMATCHED	WM_USER + 0x0102


//KeyState bits for nKeyState (may be combined):
#define KEY_NORMAL		0x00
#define KEY_SHIFT		0x01
#define KEY_CTRL		0x02
#define KEY_ALT			0x04
#define KEY_EXTENDED	0x08


#ifdef __cplusplus
extern "C" {
#endif

//Exported DLL functions:

	UINT WINAPI GetDllVersion(LPBYTE majorVersion, LPBYTE minorVersion, LPBYTE revision);
	BOOL WINAPI Enable(HWND hWnd);
	BOOL WINAPI IsEnabled();
	BOOL WINAPI Disable();
	BOOL WINAPI ClearMonitors();
	BOOL WINAPI Monitor(WORD nVK, int nKeyState, BOOL bCapture);
	BOOL WINAPI OutputString(LPCSTR lpsz);
	BOOL WINAPI SetMessages(WORD nKeyDownMsg, WORD nKeyUpMsg, WORD nKeyMatchedMsg);
	BOOL WINAPI GetCapsLock();
	void WINAPI SetCapsLock(BOOL bState);
	BOOL WINAPI GetShift();

#ifdef __cplusplus
}
#endif


//DLL names (for LoadLibrary):
#ifdef _WIN32
	#define KEYHOOK_DLL		"KEYHOOK32.DLL"
#else  //16 bit
	#define KEYHOOK_DLL		"KEYHOOK.DLL"
#endif


//Exported function names (for GetProcAddress):
#define KEYHOOK_DLL_GETDLLVERSION	"GetDllVersion"
#define KEYHOOK_DLL_ENABLE			"Enable"
#define KEYHOOK_DLL_ISENABLED		"IsEnabled"
#define KEYHOOK_DLL_DISABLE			"Disable"
#define KEYHOOK_DLL_CLEARMONITORS	"ClearMonitors"
#define KEYHOOK_DLL_MONITOR			"Monitor"
#define KEYHOOK_DLL_OUTPUTSTRING	"OutputString"
#define KEYHOOK_DLL_SETMESSAGES		"SetMessages"
#define KEYHOOK_DLL_GETCAPSLOCK		"GetCapsLock"
#define KEYHOOK_DLL_SETCAPSLOCK		"SetCapsLock"
#define KEYHOOK_DLL_GETSHIFT		"GetShift"


//Exported function templates for DLL pointers:

typedef UINT (WINAPI* GETDLLVERSION)(LPBYTE majorVersion, LPBYTE minorVersion, LPBYTE revision);
typedef BOOL (WINAPI* ENABLE)(HWND hWnd);
typedef BOOL (WINAPI* ISENABLED)();
typedef BOOL (WINAPI* DISABLE)();
typedef BOOL (WINAPI* CLEARMONITORS)();
typedef BOOL (WINAPI* MONITOR)(WORD nVK, int nKeyState, BOOL bCapture);
typedef BOOL (WINAPI* OUTPUTSTRING)(LPCSTR lpsz);
typedef BOOL (WINAPI* SETMESSAGES)(WORD nKeyDownMsg, WORD nKeyUpMsg, WORD nKeyMatchedMsg);
typedef BOOL (WINAPI* GETCAPSLOCK)();
typedef void (WINAPI* SETCAPSLOCK)(BOOL bState);
typedef BOOL (WINAPI* GETSHIFT)();

#endif
