#ifndef DINPUT_H
#define DINPUT_H

#define FC __fastcall

#define KEYBUFFERSIZE 32
#define MOUSEBUFFERSIZE 16
#define KEYBOARD_VER	0
#define MOUSE_VER		1
#define JOYSTICK_VER	2
#define JOYMIN			-1000
#define JOYMAX			 1000
//#define DIRECTINPUT_VERSION	0x0300



// This is the structure describing each joystick
typedef struct JOYDESC
{
	JOYDESC			*pNext;
	GUID			guid;
	char			szInstanceName[MAX_PATH];
	char			szProductName[MAX_PATH];
	unsigned int	iNumButtons;
} JOYDESC, *P_JOYDESC;

// This is the structure that holds the start of the
// joystick list and the number of joysticks attached
typedef struct PPJOYDESCHEAD
{
	JOYDESC		*pStart;
	UINT		iNumDevices;
} JOYDESCHEAD, *P_JOYDESCHEAD;


void FC InitDInputStruct(HWND Hwnd, HINSTANCE hInstance);
int  FC InitDirectInput();								// Generic DirectInput initialization
int  FC InitDirectKeyInput();							// DirectInput keyboard initialization
int  FC InitDirectMouseInput();							// DirectInput mouse initialization
int  FC InitDirectJoystickInput();						// Direct Input joystick initialization
int  FC CheckKeyState();								// Keyboard input loop
int  FC CheckMouseState();								// Mouse input loop
int  FC CheckJoystickState();							// Joystick input loop
void FC ReleaseDIObjects();								// Clean up the Direct Input objects
void FreeJoystickList();								// Frees the list of joystick devices
void FC DOInput();										// Processes input
BOOL CALLBACK DIEnumJoysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);	


typedef struct DISTRUCT
{
	HRESULT					hRet;
	LPDIRECTINPUT			lpddi;
	LPDIRECTINPUTDEVICE     lpDIKeyboard;
	LPDIRECTINPUTDEVICE2    lpDIKeyboard2;
	LPDIRECTINPUTDEVICE     lpDIMouse;
	LPDIRECTINPUTDEVICE2    lpDIMouse2;
	LPDIRECTINPUTDEVICE     lpDIJoystick;
	LPDIRECTINPUTDEVICE2    lpDIJoystick2;
	HWND					Hwnd;
	HINSTANCE				hInstance;
	char					keys[256];
	DIJOYSTATE				joystick;
	DIPROPRANGE				joystickDIprg;
	DIPROPDWORD				joystickDPropWord;
	DIDEVICEOBJECTDATA		keybuffer[KEYBUFFERSIZE];
	unsigned int			curKeyState[256];
	DIPROPDWORD				keyDPropWord;
	DIDEVICEOBJECTDATA		mousebuffer[KEYBUFFERSIZE];
	DIPROPDWORD				mouseDPropWord;
	DWORD					dwMouseItems;
	DWORD					dwKeyItems;
	unsigned int			DIVer[3];
	JOYDESCHEAD				JoyDescHead;

		// Clean up DirectInput stuff
    ~DISTRUCT(){ReleaseDIObjects();}
} DISTRUCT, *P_DISTRUCT;


#undef FC
#endif