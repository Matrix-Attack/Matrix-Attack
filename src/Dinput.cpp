
#define DIRECTINPUT_VERSION 0x500

#include <windows.h>
#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <dplay.h>
#include <dplobby.h>

#include <dinput.h>

#include <fstream>

#include "./mmgr.h"

using namespace std;



#include "DPlay.h"
//#include "database.h"
#include "app.h"

#include "dxerror.h"
#include "utility.h"
#include "dinput.h"


#define FC __fastcall





extern APP App;
extern GAME_DATA GameData;

#define FIRE DIK_1
#define JUMP DIK_S
#define LEFT DIK_LEFT
#define RIGHT DIK_RIGHT
#define UP	  DIK_UP
#define DOWN  DIK_DOWN

typedef struct KEYS
{
	int left;
	int right;
	int down;
	int up;
	int fire;
	int shoot;
} KEYS;

typedef struct JOYSTICK
{
	int left;
	int right;
	int down;
	int up;
	int fire;
	int shoot;
} JOYSTICK;

KEYS keys;
JOYSTICK joy;

char *game[6] = {"left", "down", "right", "up", "shoot", "jump"};


//***********************************************************************
// Function: InitDInputStruct
//
// Purpose:  Initializes the DirectInput base structure
//
// Parameters: HWND Hwnd - the window handle
//			   HINSTANCE hInstance - handle to the current window instance 
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
void FC InitDInputStruct(HWND Hwnd, HINSTANCE hInstance)
{
	keys.down = DIK_DOWN;
	joy.down = DIK_DOWN;
	keys.left = DIK_LEFT;
	joy.left = DIK_LEFT;
	keys.right = DIK_RIGHT;
	joy.right = DIK_RIGHT;
	keys.up = DIK_UP;
	joy.up = DIK_UP;
	keys.fire = DIK_W;
	joy.fire = DIK_W;
	keys.shoot = DIK_E;
	joy.shoot = DIJOFS_BUTTON0;
	App.pdistruct->hInstance = hInstance;
	App.pdistruct->Hwnd = Hwnd;
	App.pdistruct->hRet = 0;
	App.pdistruct->lpddi = NULL;
	memset(&App.pdistruct->keys, 0, 256);
	memset(&App.pdistruct->keybuffer, 0, KEYBUFFERSIZE);
	memset(&App.pdistruct->joystick, 0, sizeof(App.pdistruct->joystick) );
	App.pdistruct->dwKeyItems = KEYBUFFERSIZE;
	App.pdistruct->dwMouseItems = MOUSEBUFFERSIZE;
	App.pdistruct->DIVer[KEYBOARD_VER] = 0;
	App.pdistruct->DIVer[MOUSE_VER] = 0;
	App.pdistruct->DIVer[JOYSTICK_VER] = 0;
	App.pdistruct->JoyDescHead.iNumDevices = 0;
	App.pdistruct->JoyDescHead.pStart = NULL;
}



//***********************************************************************
// Function: DIEnumJoysticks
//
// Purpose:  Callback function for joystick enumeration
//
// Parameters: LPCDIDEVICEINSTANCE lpddi - pointer to a Direct Input
//			   device
//			   LPVOID pvRef - user defined void pointer
//			  
//
// Returns: BOOL - either DIENUM_CONTINUE to continue enumeration or 
//			false to stop
//
// Last Modified:  Date 10/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL CALLBACK DIEnumJoysticks(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	JOYDESCHEAD* pJoyHead = (JOYDESCHEAD*)pvRef;
	P_JOYDESC	   pList = pJoyHead->pStart;
	P_JOYDESC      pNew = NULL;
	static int     num = 0;

	// Allocate space for the first node in the list
	if(num == 0)
	{
//		pList = (P_JOYDESC)calloc(1, sizeof(JOYDESC) );
		pList = (P_JOYDESC) malloc ( sizeof(JOYDESC) );
		memset(pList, 0, sizeof(JOYDESC) );

		pList->guid = lpddi->guidProduct;

		strcpy(pList->szInstanceName, lpddi->tszInstanceName);
		strcpy(pList->szProductName,  lpddi->tszProductName );

		pList->pNext = NULL;	
		pJoyHead->pStart = pList;
		pJoyHead->iNumDevices++;
	}
	else
	{
		// Walk down to the latest node
		for(pList; pList->pNext != NULL; pList = pList->pNext)
			;
		
//		pNew = (P_JOYDESC)calloc(1, sizeof(JOYDESC) );
		pNew = (P_JOYDESC) malloc ( sizeof(JOYDESC) );
		memset(pNew, 0, sizeof(JOYDESC) );

		pNew->guid = lpddi->guidProduct;
		strcpy(pNew->szInstanceName, lpddi->tszInstanceName);
		strcpy(pNew->szProductName, lpddi->tszProductName);
		pNew->pNext = NULL;
		pList->pNext = pNew;
		pJoyHead->iNumDevices++;
	}

	return DIENUM_CONTINUE;
}


//***********************************************************************
// Function: FreeJoystickList
//
// Purpose:  Frees the list of joysticks enumerated
//
// Parameters: None
//			  
//
// Returns: Nothing
//			
//
// Last Modified:  Date 10/15/99      Author - Max Szlagor     
//
//***********************************************************************
void FreeJoystickList()
{
	P_JOYDESC  pCur = App.pdistruct->JoyDescHead.pStart;
	P_JOYDESC  pTemp = App.pdistruct->JoyDescHead.pStart;

	if(!pTemp)
		return;

	if(!pTemp->pNext)
	{
		free(pTemp);
	    App.pdistruct->JoyDescHead.iNumDevices = 0;
		return;
	}

	for(pTemp = pTemp->pNext; pTemp->pNext != NULL; pTemp = pCur)
	{
		pCur = pTemp->pNext;
		free(pTemp);
		pTemp = NULL;
	}

	App.pdistruct->JoyDescHead.iNumDevices  = 0;
	free(App.pdistruct->JoyDescHead.pStart);
	App.pdistruct->JoyDescHead.pStart = NULL;
}


//***********************************************************************
// Function: InitDirectInput
//
// Purpose:  Performs generic DirectInput initialization
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC InitDirectInput()
{
	// First create the Direct Input object, try grabbing the latest version of 
	// the DirectInput object, if that doesn't work then revert to DInput for DirectX 3
	App.pdistruct->hRet = (HRESULT)DirectInputCreate(App.pdistruct->hInstance, DIRECTINPUT_VERSION, &App.pdistruct->lpddi, NULL);

	if(FAILED(App.pdistruct->hRet) )
	{
		App.pdistruct->hRet = (HRESULT)DirectInputCreate(App.pdistruct->hInstance, 0x0300, &App.pdistruct->lpddi, NULL);
		if(FAILED(App.pdistruct->hRet) )
		{
			MessageBox(App.pdistruct->Hwnd, "Error creating direct input in InitDirectInput", "ERROR", MB_OK);
			DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		}
	}

	// Now go through and enumerate the available joysticks
	App.pdistruct->lpddi->EnumDevices(DIDEVTYPE_JOYSTICK, DIEnumJoysticks, &App.pdistruct->JoyDescHead, DIEDFL_ATTACHEDONLY);

	return 0;
}	/* end InitDirectInput */






//***********************************************************************
// Function: InitDirectKeyInput
//
// Purpose:  Performs DirectInput initialization for the keyboard
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC InitDirectKeyInput()
{
	// Now create the keyboard device
	App.pdistruct->hRet = App.pdistruct->lpddi->CreateDevice(GUID_SysKeyboard, &App.pdistruct->lpDIKeyboard, NULL);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error creating keyboard device in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	// Try and grab the newer interface for the keyboard
	App.pdistruct->hRet = App.pdistruct->lpDIKeyboard->QueryInterface(IID_IDirectInputDevice2, (void**)&App.pdistruct->lpDIKeyboard2 );

	if(FAILED(App.pdistruct->hRet) )
	{
		// This means we have an older version of DirectX so scale back structures
	///	MessageBox(App.pdistruct->Hwnd, "Error querying interface for DInput2 keyboard in InitDirectInput", "ERROR", MB_OK);
	//	DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		App.pdistruct->DIVer[KEYBOARD_VER] = 3;
	//	return 1;
	}
	else
	{
		// Release the old interface and set the value for a newer DInput interface
		App.pdistruct->DIVer[KEYBOARD_VER] = 5;
		App.pdistruct->lpDIKeyboard->Release();
	}

	// This code runs when DirectX 5 or later is installed
	if(App.pdistruct->DIVer[KEYBOARD_VER] == 5)
	{
	// Now set the data format for the keyboard
	App.pdistruct->hRet = App.pdistruct->lpDIKeyboard2->SetDataFormat(&c_dfDIKeyboard);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}
	
	// Now set the cooperative level for the keyboard
	App.pdistruct->hRet = App.pdistruct->lpDIKeyboard2->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_NONEXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	// Now set the buffer size, this is used for retrieving buffered key input
	App.pdistruct->keyDPropWord.diph.dwSize   = sizeof(App.pdistruct->keyDPropWord);
	App.pdistruct->keyDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->keyDPropWord.diph);
	App.pdistruct->keyDPropWord.diph.dwObj = 0;
	App.pdistruct->keyDPropWord.diph.dwHow = DIPH_DEVICE;
	App.pdistruct->keyDPropWord.dwData = KEYBUFFERSIZE;
	App.pdistruct->lpDIKeyboard2->SetProperty(DIPROP_BUFFERSIZE, &App.pdistruct->keyDPropWord.diph);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting buffer size in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	 // Now acquire the keyboard for the first time
	App.pdistruct->lpDIKeyboard2->Acquire();
	}


	// Branch the code to an earlier version of DirectX
	else if(App.pdistruct->DIVer[KEYBOARD_VER] == 3)
	{
	App.pdistruct->hRet = App.pdistruct->lpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}
	
	// Now set the cooperative level for the keyboard
	App.pdistruct->hRet = App.pdistruct->lpDIKeyboard->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_NONEXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	// Now set the buffer size, this is used for retrieving buffered key input
	App.pdistruct->keyDPropWord.diph.dwSize       = sizeof(App.pdistruct->keyDPropWord);
	App.pdistruct->keyDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->keyDPropWord.diph);
	App.pdistruct->keyDPropWord.diph.dwObj        = 0;
	App.pdistruct->keyDPropWord.diph.dwHow        = DIPH_DEVICE;
	App.pdistruct->keyDPropWord.dwData            = KEYBUFFERSIZE;
	App.pdistruct->lpDIKeyboard->SetProperty(DIPROP_BUFFERSIZE, &(App.pdistruct->keyDPropWord.diph) );

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting buffer size in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	 // Now acquire the keyboard for the first time
	while(FAILED(App.pdistruct->lpDIKeyboard->Acquire()) )
	{
	}

	}
	return 0;
}	/* end InitDirectKeyInput */




//***********************************************************************
// Function: InitDirectMouseInput
//
// Purpose:  Performs DirectInput initialization for the mouse
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC InitDirectMouseInput()
{
	// Now create the mouse device
	App.pdistruct->hRet = App.pdistruct->lpddi->CreateDevice(GUID_SysMouse, &App.pdistruct->lpDIMouse, NULL);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error creating mouse device in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}


	// Try and grab the newer interface for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIMouse->QueryInterface(IID_IDirectInputDevice2, (void**)&App.pdistruct->lpDIMouse2 );

	if(FAILED(App.pdistruct->hRet) )
	{
	//	MessageBox(App.pdistruct->Hwnd, "Error querying interface for DInput2 mouse in InitDirectInput", "ERROR", MB_OK);
	//	DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
	//	return 1;
		App.pdistruct->DIVer[MOUSE_VER] = 3;
	}
	else
	{
		// Release the old interface
		App.pdistruct->DIVer[MOUSE_VER] = 5;
		App.pdistruct->lpDIMouse->Release();
	}


	if(App.pdistruct->DIVer[MOUSE_VER] == 5)
	{
	// Now set the data format for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIMouse2->SetDataFormat(&c_dfDIMouse);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}


	// Now set the cooperative level for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIMouse2->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_EXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for mouse in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	

	// Now set the buffer size, this is used for retrieving buffered mouse input
	App.pdistruct->mouseDPropWord.diph.dwSize       = sizeof(App.pdistruct->mouseDPropWord);
	App.pdistruct->mouseDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->mouseDPropWord.diph);
	App.pdistruct->mouseDPropWord.diph.dwObj        = 0;
	App.pdistruct->mouseDPropWord.diph.dwHow        = DIPH_DEVICE;
	App.pdistruct->mouseDPropWord.dwData            = MOUSEBUFFERSIZE;
	App.pdistruct->lpDIMouse2->SetProperty(DIPROP_BUFFERSIZE, &(App.pdistruct->mouseDPropWord.diph) );

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting buffer size in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}

	// Now acquire the mouse for the first time
	App.pdistruct->lpDIMouse2->Acquire();
	}	// end if for DX 5 mouse


	// Branch the code to be compatible with DirectX 3
	else if(App.pdistruct->DIVer[MOUSE_VER] == 3)
	{
	// Now set the data format for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIMouse->SetDataFormat(&c_dfDIMouse);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}


	// Now set the cooperative level for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIMouse->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_EXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for mouse in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	

	// Now set the buffer size, this is used for retrieving buffered mouse input
	App.pdistruct->mouseDPropWord.diph.dwSize       = sizeof(App.pdistruct->mouseDPropWord);
	App.pdistruct->mouseDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->mouseDPropWord.diph);
	App.pdistruct->mouseDPropWord.diph.dwObj        = 0;
	App.pdistruct->mouseDPropWord.diph.dwHow        = DIPH_DEVICE;
	App.pdistruct->mouseDPropWord.dwData            = MOUSEBUFFERSIZE;
	App.pdistruct->lpDIMouse->SetProperty(DIPROP_BUFFERSIZE, &(App.pdistruct->mouseDPropWord.diph) );

	// Now acquire the mouse for the first time
	App.pdistruct->lpDIMouse->Acquire();
	}	// end if for DX 3 mouse

	return 0;
}	/* end InitDirectMouseInput */






//***********************************************************************
// Function: InitDirectJoystickInput
//
// Purpose:  Performs DirectInput initialization for the mouse
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC InitDirectJoystickInput()
{
	// Now create the joystick device
	if(!App.pdistruct->JoyDescHead.iNumDevices)
		return 0;

	App.pdistruct->hRet = App.pdistruct->lpddi->CreateDevice(App.pdistruct->JoyDescHead.pStart->guid, &App.pdistruct->lpDIJoystick, NULL);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error creating joystick device in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		
		return 1;
	}


	// Try and grab the newer interface for the mouse
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick->QueryInterface(IID_IDirectInputDevice2, (void**)&App.pdistruct->lpDIJoystick2 );

	if(FAILED(App.pdistruct->hRet) )
	{
	//	MessageBox(App.pdistruct->Hwnd, "Error querying interface for DInput2 mouse in InitDirectInput", "ERROR", MB_OK);
	//	DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
	//	return 1;
		App.pdistruct->DIVer[JOYSTICK_VER] = 3;
	}
	else
	{
		// Release the old interface
		App.pdistruct->DIVer[JOYSTICK_VER] = 5;
		App.pdistruct->lpDIJoystick->Release();
	}

	// Only execute the following code if we are using DirectX 5 or greater
	if(App.pdistruct->DIVer[JOYSTICK_VER] == 5)
	{
	// Now set the data format for the joystick
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick2->SetDataFormat(&c_dfDIJoystick);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}


	// Now set the cooperative level for the joystick
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick2->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_EXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for joystick in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	


	// Now set the joystick x range
	App.pdistruct->joystickDIprg.diph.dwSize       = sizeof(App.pdistruct->joystickDIprg);
	App.pdistruct->joystickDIprg.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDIprg.diph);
	App.pdistruct->joystickDIprg.diph.dwObj        = DIJOFS_X;
	App.pdistruct->joystickDIprg.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDIprg.lMin              = JOYMIN;
	App.pdistruct->joystickDIprg.lMax              = JOYMAX;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_RANGE, &(App.pdistruct->joystickDIprg.diph) );

	// Now set the joystick y range
	App.pdistruct->joystickDIprg.diph.dwSize       = sizeof(App.pdistruct->joystickDIprg);
	App.pdistruct->joystickDIprg.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDIprg.diph);
	App.pdistruct->joystickDIprg.diph.dwObj        = DIJOFS_Y;
	App.pdistruct->joystickDIprg.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDIprg.lMin              = JOYMIN;
	App.pdistruct->joystickDIprg.lMax              = JOYMAX;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_RANGE, &(App.pdistruct->joystickDIprg.diph) );

	// Now set the joystick x dead zone
	App.pdistruct->joystickDPropWord.diph.dwSize       = sizeof(App.pdistruct->joystickDPropWord);
	App.pdistruct->joystickDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDPropWord.diph);
	App.pdistruct->joystickDPropWord.diph.dwObj        = DIJOFS_X;
	App.pdistruct->joystickDPropWord.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDPropWord.dwData            = 1000;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_DEADZONE, &(App.pdistruct->joystickDPropWord.diph) );

	// Now set the joystick y dead zone
	App.pdistruct->joystickDPropWord.diph.dwSize       = sizeof(App.pdistruct->joystickDPropWord);
	App.pdistruct->joystickDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDPropWord.diph);
	App.pdistruct->joystickDPropWord.diph.dwObj        = DIJOFS_Y;
	App.pdistruct->joystickDPropWord.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDPropWord.dwData            = 1000;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_DEADZONE, &(App.pdistruct->joystickDPropWord.diph) );

	// Now set the joystick x saturation
	App.pdistruct->joystickDPropWord.diph.dwSize       = sizeof(App.pdistruct->joystickDPropWord);
	App.pdistruct->joystickDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDPropWord.diph);
	App.pdistruct->joystickDPropWord.diph.dwObj        = DIJOFS_X;
	App.pdistruct->joystickDPropWord.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDPropWord.dwData            = 1000;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_SATURATION, &(App.pdistruct->joystickDPropWord.diph) );

	// Now set the joystick y saturation
	App.pdistruct->joystickDPropWord.diph.dwSize       = sizeof(App.pdistruct->joystickDPropWord);
	App.pdistruct->joystickDPropWord.diph.dwHeaderSize = sizeof(App.pdistruct->joystickDPropWord.diph);
	App.pdistruct->joystickDPropWord.diph.dwObj        = DIJOFS_Y;
	App.pdistruct->joystickDPropWord.diph.dwHow        = DIPH_BYOFFSET;
	App.pdistruct->joystickDPropWord.dwData            = 1000;
	App.pdistruct->lpDIJoystick2->SetProperty(DIPROP_SATURATION, &(App.pdistruct->joystickDPropWord.diph) );

	DIDEVCAPS	didc;
//	char		szBut[100];

	didc.dwSize = sizeof(DIDEVCAPS);

	App.pdistruct->lpDIJoystick2->GetCapabilities(&didc);

	App.pdistruct->JoyDescHead.pStart->iNumButtons = didc.dwButtons;

	// This code prints the number of buttons available on the joystick
//	wsprintf(szBut, "%d", didc.dwButtons);
//	MessageBox(App.pdistruct->Hwnd, szBut, "WOW", MB_OK);

	// Now acquire the joystick for the first time
	App.pdistruct->lpDIJoystick->Acquire();
	}	// end if for DX 5 joystick



	else if(App.pdistruct->DIVer[JOYSTICK_VER] == 3)
	{
	// Now set the data format for the joystick
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick->SetDataFormat(&c_dfDIJoystick);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting data format for keyboard in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}


	// Now set the cooperative level for the joystick
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick->SetCooperativeLevel(App.pdistruct->Hwnd,
																	DISCL_EXCLUSIVE |
																	DISCL_FOREGROUND);

	if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error setting cooperative level for mouse in InitDirectInput", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	

	App.pdistruct->lpDIJoystick->Acquire();
	}

	return 0;
}	/* end InitDirectMouseInput */




//***********************************************************************
// Function: CheckKeyState
//
// Purpose:  Checks the state of the keyboard and notes any changes
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC CheckKeyState()
{
	static int pass = 0;

	// This is the code that runs for DirectX version 5 or higher
	if(App.pdistruct->DIVer[KEYBOARD_VER] == 5)
	{
		// This function checks the immediate state of the keyboard
		App.pdistruct->hRet = App.pdistruct->lpDIKeyboard2->GetDeviceState(sizeof(App.pdistruct->keys), App.pdistruct->keys);


		if(App.pdistruct->hRet == DIERR_INPUTLOST)
			App.pdistruct->lpDIKeyboard2->Acquire();
		else if(FAILED(App.pdistruct->hRet) )
		{
			MessageBox(App.pdistruct->Hwnd, "Error getting the state of the keyboard in CheckKeyState, shutting down game.", "ERROR", MB_OK);
//			DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
			return 1;
		}	

	}    // end else if(App.pdistruct->ver == 5)


	// This is the code that runs for DirectX version 3
	else if(App.pdistruct->DIVer[KEYBOARD_VER] == 3)
	{
		// This function checks the immediate state of the keyboard
		App.pdistruct->hRet = App.pdistruct->lpDIKeyboard->GetDeviceState(sizeof(App.pdistruct->keys), App.pdistruct->keys);


		if(App.pdistruct->hRet == DIERR_INPUTLOST)
			App.pdistruct->lpDIKeyboard->Acquire();
		else if(FAILED(App.pdistruct->hRet) )
		{
		//	MessageBox(App.pdistruct->Hwnd, "Error getting the state of the keyboard in CheckKeyState", "ERROR", MB_OK);
		//	DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
			return 1;
		}
	}	// end else if(App.pdistruct->ver == 3)


	if(App.pdistruct->keys[keys.down] & 0x80)
		GameData.ActionState.bDown = 1;
	else
		GameData.ActionState.bDown = 0;

	if(App.pdistruct->keys[keys.left] & 0x80)
		GameData.ActionState.bLeft = 1;
	else
		GameData.ActionState.bLeft = 0;

	if(App.pdistruct->keys[keys.right] & 0x80)
		GameData.ActionState.bRight = 1;
	else
		GameData.ActionState.bRight = 0;

	if(App.pdistruct->keys[keys.up] & 0x80)
		GameData.ActionState.bUp = 1;
	else
		GameData.ActionState.bUp = 0;

	if(App.pdistruct->keys[keys.shoot] & 0x80)
		GameData.ActionState.bShoot = 1;
	else
		GameData.ActionState.bShoot = 0;

	if(App.pdistruct->keys[keys.fire] & 0x80)
		GameData.ActionState.bFire = 1;
	else
		GameData.ActionState.bFire = 0;


/*
	if(App.pdistruct->keys[DIK_0] & 0x80)
		App.pdistruct->curKeyState[DIK_0]++;
	else
		App.pdistruct->curKeyState[DIK_0] = 0;

	if(App.pdistruct->keys[DIK_1] & 0x80)
		App.pdistruct->curKeyState[DIK_1]++;
	else
		App.pdistruct->curKeyState[DIK_1] = 0;

	if(App.pdistruct->keys[DIK_2] & 0x80)
		App.pdistruct->curKeyState[DIK_2]++;
	else
		App.pdistruct->curKeyState[DIK_2] = 0;

	if(App.pdistruct->keys[DIK_3] & 0x80)
		App.pdistruct->curKeyState[DIK_3]++;
	else
		App.pdistruct->curKeyState[DIK_3] = 0;

	if(App.pdistruct->keys[DIK_4] & 0x80)
		App.pdistruct->curKeyState[DIK_4]++;
	else
		App.pdistruct->curKeyState[DIK_4] = 0;

	if(App.pdistruct->keys[DIK_5] & 0x80)
		App.pdistruct->curKeyState[DIK_5]++;
	else
		App.pdistruct->curKeyState[DIK_5] = 0;

	if(App.pdistruct->keys[DIK_6] & 0x80)
		App.pdistruct->curKeyState[DIK_6]++;
	else
		App.pdistruct->curKeyState[DIK_6] = 0;

	if(App.pdistruct->keys[DIK_7] & 0x80)
		App.pdistruct->curKeyState[DIK_7]++;
	else
		App.pdistruct->curKeyState[DIK_7] = 0;

	if(App.pdistruct->keys[DIK_8] & 0x80)
		App.pdistruct->curKeyState[DIK_8]++;
	else
		App.pdistruct->curKeyState[DIK_8] = 0;

	if(App.pdistruct->keys[DIK_9] & 0x80)
		App.pdistruct->curKeyState[DIK_9]++;
	else
		App.pdistruct->curKeyState[DIK_9] = 0;

	if(App.pdistruct->keys[DIK_MINUS] & 0x80)
		App.pdistruct->curKeyState[DIK_MINUS]++;
	else
		App.pdistruct->curKeyState[DIK_MINUS] = 0;

	if(App.pdistruct->keys[DIK_EQUALS] & 0x80)
		App.pdistruct->curKeyState[DIK_EQUALS]++;
	else
		App.pdistruct->curKeyState[DIK_EQUALS] = 0;

	if(App.pdistruct->keys[DIK_BACK] & 0x80)
		App.pdistruct->curKeyState[DIK_BACK]++;
	else
		App.pdistruct->curKeyState[DIK_BACK] = 0;

	if(App.pdistruct->keys[DIK_TAB] & 0x80)
		App.pdistruct->curKeyState[DIK_TAB]++;
	else
		App.pdistruct->curKeyState[DIK_TAB] = 0;

	if(App.pdistruct->keys[DIK_Q] & 0x80)
		App.pdistruct->curKeyState[DIK_Q]++;
	else
		App.pdistruct->curKeyState[DIK_Q] = 0;

	if(App.pdistruct->keys[DIK_W] & 0x80)
		App.pdistruct->curKeyState[DIK_W]++;	
	else
		App.pdistruct->curKeyState[DIK_W] = 0;

	if(App.pdistruct->keys[DIK_E] & 0x80)
		App.pdistruct->curKeyState[DIK_E]++;
	else
		App.pdistruct->curKeyState[DIK_E] = 0;

	if(App.pdistruct->keys[DIK_R] & 0x80)
		App.pdistruct->curKeyState[DIK_R]++;
	else
		App.pdistruct->curKeyState[DIK_R] = 0;

	if(App.pdistruct->keys[DIK_T] & 0x80)
		App.pdistruct->curKeyState[DIK_T]++;
	else
		App.pdistruct->curKeyState[DIK_T] = 0;

	if(App.pdistruct->keys[DIK_Y] & 0x80)
		App.pdistruct->curKeyState[DIK_Y]++;
	else
		App.pdistruct->curKeyState[DIK_Y] = 0;

	if(App.pdistruct->keys[DIK_U] & 0x80)
		App.pdistruct->curKeyState[DIK_U]++;
	else
		App.pdistruct->curKeyState[DIK_U] = 0;

	if(App.pdistruct->keys[DIK_I] & 0x80)
		App.pdistruct->curKeyState[DIK_I]++;
	else
		App.pdistruct->curKeyState[DIK_I] = 0;

	if(App.pdistruct->keys[DIK_O] & 0x80)
		App.pdistruct->curKeyState[DIK_O]++;
	else
		App.pdistruct->curKeyState[DIK_O] = 0;

	if(App.pdistruct->keys[DIK_P] & 0x80)
		App.pdistruct->curKeyState[DIK_P]++;
	else
		App.pdistruct->curKeyState[DIK_P] = 0;

	if(App.pdistruct->keys[DIK_LBRACKET] & 0x80)
		App.pdistruct->curKeyState[DIK_LBRACKET]++;
	else
		App.pdistruct->curKeyState[DIK_LBRACKET] = 0;

	if(App.pdistruct->keys[DIK_RBRACKET] & 0x80)
		App.pdistruct->curKeyState[DIK_RBRACKET]++;
	else
		App.pdistruct->curKeyState[DIK_RBRACKET] = 0;

	if(App.pdistruct->keys[DIK_RETURN] & 0x80)
		App.pdistruct->curKeyState[DIK_RETURN]++;
	else
		App.pdistruct->curKeyState[DIK_RETURN] = 0;

	if(App.pdistruct->keys[DIK_LCONTROL] & 0x80)
		App.pdistruct->curKeyState[DIK_LCONTROL]++;
	else
		App.pdistruct->curKeyState[DIK_LCONTROL] = 0;

	if(App.pdistruct->keys[DIK_A] & 0x80)
		App.pdistruct->curKeyState[DIK_A]++;
	else
		App.pdistruct->curKeyState[DIK_A] = 0;

	if(App.pdistruct->keys[DIK_S] & 0x80)
		App.pdistruct->curKeyState[DIK_S]++;
	else
		App.pdistruct->curKeyState[DIK_S] = 0;

	if(App.pdistruct->keys[DIK_D] & 0x80)
		App.pdistruct->curKeyState[DIK_D]++;
	else
		App.pdistruct->curKeyState[DIK_D] = 0;

	if(App.pdistruct->keys[DIK_F] & 0x80)
		App.pdistruct->curKeyState[DIK_F]++;
	else
		App.pdistruct->curKeyState[DIK_F] = 0;

	if(App.pdistruct->keys[DIK_G] & 0x80)
		App.pdistruct->curKeyState[DIK_G]++;
	else
		App.pdistruct->curKeyState[DIK_G] = 0;

	if(App.pdistruct->keys[DIK_H] & 0x80)
		App.pdistruct->curKeyState[DIK_H]++;
	else
		App.pdistruct->curKeyState[DIK_H] = 0;

	if(App.pdistruct->keys[DIK_J] & 0x80)
		App.pdistruct->curKeyState[DIK_J]++;
	else
		App.pdistruct->curKeyState[DIK_J] = 0;

	if(App.pdistruct->keys[DIK_K] & 0x80)
		App.pdistruct->curKeyState[DIK_K]++;
	else
		App.pdistruct->curKeyState[DIK_K] = 0;

	if(App.pdistruct->keys[DIK_L] & 0x80)
		App.pdistruct->curKeyState[DIK_L]++;
	else
		App.pdistruct->curKeyState[DIK_L] = 0;

	if(App.pdistruct->keys[DIK_SEMICOLON] & 0x80)
		App.pdistruct->curKeyState[DIK_SEMICOLON]++;
	else
		App.pdistruct->curKeyState[DIK_SEMICOLON] = 0;

	if(App.pdistruct->keys[DIK_APOSTROPHE] & 0x80)
		App.pdistruct->curKeyState[DIK_APOSTROPHE]++;
	else
		App.pdistruct->curKeyState[DIK_APOSTROPHE] = 0;

	if(App.pdistruct->keys[DIK_GRAVE] & 0x80)
		App.pdistruct->curKeyState[DIK_GRAVE]++;
	else
		App.pdistruct->curKeyState[DIK_GRAVE] = 0;

	if(App.pdistruct->keys[DIK_LSHIFT] & 0x80)
		App.pdistruct->curKeyState[DIK_LSHIFT]++;
	else
		App.pdistruct->curKeyState[DIK_LSHIFT] = 0;

	if(App.pdistruct->keys[DIK_BACKSLASH] & 0x80)
		App.pdistruct->curKeyState[DIK_BACKSLASH]++;
	else
		App.pdistruct->curKeyState[DIK_BACKSLASH] = 0;

	if(App.pdistruct->keys[DIK_Z] & 0x80)
		App.pdistruct->curKeyState[DIK_Z]++;
	else
		App.pdistruct->curKeyState[DIK_Z] = 0;

	if(App.pdistruct->keys[DIK_X] & 0x80)
		App.pdistruct->curKeyState[DIK_X]++;
	else
		App.pdistruct->curKeyState[DIK_X] = 0;

	if(App.pdistruct->keys[DIK_C] & 0x80)
		App.pdistruct->curKeyState[DIK_C]++;
	else
		App.pdistruct->curKeyState[DIK_C] = 0;

	if(App.pdistruct->keys[DIK_V] & 0x80)
		App.pdistruct->curKeyState[DIK_V]++;
	else
		App.pdistruct->curKeyState[DIK_V] = 0;

	if(App.pdistruct->keys[DIK_B] & 0x80)
		App.pdistruct->curKeyState[DIK_B]++;
	else
		App.pdistruct->curKeyState[DIK_B] = 0;


	if(App.pdistruct->keys[DIK_N] & 0x80)
		App.pdistruct->curKeyState[DIK_N]++;
	else
		App.pdistruct->curKeyState[DIK_N] = 0;

	if(App.pdistruct->keys[DIK_M] & 0x80)
		App.pdistruct->curKeyState[DIK_M]++;
	else
		App.pdistruct->curKeyState[DIK_M] = 0;

	if(App.pdistruct->keys[DIK_COMMA] & 0x80)
		App.pdistruct->curKeyState[DIK_COMMA]++;
	else
		App.pdistruct->curKeyState[DIK_COMMA] = 0;

	if(App.pdistruct->keys[DIK_PERIOD] & 0x80)
		App.pdistruct->curKeyState[DIK_PERIOD]++;
	else
		App.pdistruct->curKeyState[DIK_PERIOD] = 0;

	if(App.pdistruct->keys[DIK_SLASH] & 0x80)
		App.pdistruct->curKeyState[DIK_SLASH]++;
	else
		App.pdistruct->curKeyState[DIK_SLASH] = 0;

	if(App.pdistruct->keys[DIK_RSHIFT] & 0x80)
		App.pdistruct->curKeyState[DIK_RSHIFT]++;
	else
		App.pdistruct->curKeyState[DIK_RSHIFT] = 0;

	if(App.pdistruct->keys[DIK_MULTIPLY] & 0x80)
		App.pdistruct->curKeyState[DIK_MULTIPLY]++;
	else
		App.pdistruct->curKeyState[DIK_MULTIPLY] = 0;

	if(App.pdistruct->keys[DIK_LMENU] & 0x80)
		App.pdistruct->curKeyState[DIK_LMENU]++;
	else
		App.pdistruct->curKeyState[DIK_LMENU] = 0;

	if(App.pdistruct->keys[DIK_SPACE] & 0x80)
		App.pdistruct->curKeyState[DIK_SPACE]++;
	else
		App.pdistruct->curKeyState[DIK_SPACE] = 0;

	if(App.pdistruct->keys[DIK_CAPITAL] & 0x80)
		App.pdistruct->curKeyState[DIK_CAPITAL]++;
	else
		App.pdistruct->curKeyState[DIK_CAPITAL] = 0;

	if(App.pdistruct->keys[DIK_F1] & 0x80)
		App.pdistruct->curKeyState[DIK_F1]++;
	else
		App.pdistruct->curKeyState[DIK_F1] = 0;

	if(App.pdistruct->keys[DIK_F2] & 0x80)
		App.pdistruct->curKeyState[DIK_F2]++;
	else
		App.pdistruct->curKeyState[DIK_F2] = 0;

	if(App.pdistruct->keys[DIK_F3] & 0x80)
		App.pdistruct->curKeyState[DIK_F3]++;
	else
		App.pdistruct->curKeyState[DIK_F3] = 0;

	if(App.pdistruct->keys[DIK_F4] & 0x80)
		App.pdistruct->curKeyState[DIK_F4]++;
	else
		App.pdistruct->curKeyState[DIK_F4] = 0;

	if(App.pdistruct->keys[DIK_F5] & 0x80)
		App.pdistruct->curKeyState[DIK_F5]++;
	else
		App.pdistruct->curKeyState[DIK_F5] = 0;

	if(App.pdistruct->keys[DIK_F6] & 0x80)
		App.pdistruct->curKeyState[DIK_F6]++;
	else
		App.pdistruct->curKeyState[DIK_F6] = 0;

	if(App.pdistruct->keys[DIK_F7] & 0x80)
		App.pdistruct->curKeyState[DIK_F7]++;
	else
		App.pdistruct->curKeyState[DIK_F7] = 0;;

	if(App.pdistruct->keys[DIK_F8] & 0x80)
		App.pdistruct->curKeyState[DIK_F8]++;
	else
		App.pdistruct->curKeyState[DIK_F8] = 0;

	if(App.pdistruct->keys[DIK_F9] & 0x80)
		App.pdistruct->curKeyState[DIK_F9]++;
	else
		App.pdistruct->curKeyState[DIK_F9] = 0;

	if(App.pdistruct->keys[DIK_F10] & 0x80)
		App.pdistruct->curKeyState[DIK_F10]++;
	else
		App.pdistruct->curKeyState[DIK_F10] = 0;

	if(App.pdistruct->keys[DIK_F11] & 0x80)
		App.pdistruct->curKeyState[DIK_F11]++;
	else
		App.pdistruct->curKeyState[DIK_F11] = 0;

	if(App.pdistruct->keys[DIK_F12] & 0x80)
		App.pdistruct->curKeyState[DIK_F12]++;
	else
		App.pdistruct->curKeyState[DIK_F12] = 0;

	if(App.pdistruct->keys[DIK_NUMLOCK] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMLOCK]++;
	else
		App.pdistruct->curKeyState[DIK_NUMLOCK] = 0;

	if(App.pdistruct->keys[DIK_SCROLL] & 0x80)
		App.pdistruct->curKeyState[DIK_SCROLL]++;
	else
		App.pdistruct->curKeyState[DIK_SCROLL] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD0] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD0]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD0] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD1] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD1]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD1] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD2] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD2]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD2] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD3] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD3]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD3] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD4] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD4]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD4] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD5] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD5]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD5] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD6] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD6]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD6] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD7] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD7]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD7] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD8] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD8]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD8] = 0;

	if(App.pdistruct->keys[DIK_NUMPAD9] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPAD9]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPAD9] = 0;

	if(App.pdistruct->keys[DIK_SUBTRACT] & 0x80)
		App.pdistruct->curKeyState[DIK_SUBTRACT]++;
	else
		App.pdistruct->curKeyState[DIK_SUBTRACT] = 0;

	if(App.pdistruct->keys[DIK_ADD] & 0x80)
		App.pdistruct->curKeyState[DIK_ADD]++;
	else
		App.pdistruct->curKeyState[DIK_ADD] = 0;

	if(App.pdistruct->keys[DIK_DECIMAL] & 0x80)
		App.pdistruct->curKeyState[DIK_DECIMAL]++;
	else
		App.pdistruct->curKeyState[DIK_DECIMAL] = 0;

	if(App.pdistruct->keys[DIK_NUMPADENTER] & 0x80)
		App.pdistruct->curKeyState[DIK_NUMPADENTER]++;
	else
		App.pdistruct->curKeyState[DIK_NUMPADENTER] = 0;

	if(App.pdistruct->keys[DIK_RCONTROL] & 0x80)
		App.pdistruct->curKeyState[DIK_RCONTROL]++;
	else
		App.pdistruct->curKeyState[DIK_RCONTROL] = 0;

	if(App.pdistruct->keys[DIK_DIVIDE] & 0x80)
		App.pdistruct->curKeyState[DIK_DIVIDE]++;
	else
		App.pdistruct->curKeyState[DIK_DIVIDE] = 0;

	if(App.pdistruct->keys[DIK_RMENU] & 0x80)
		App.pdistruct->curKeyState[DIK_RMENU]++;
	else
		App.pdistruct->curKeyState[DIK_RMENU] = 0;

	if(App.pdistruct->keys[DIK_UP] & 0x80)
		App.pdistruct->curKeyState[DIK_UP]++;
	else
		App.pdistruct->curKeyState[DIK_UP] = 0;

	if(App.pdistruct->keys[DIK_PRIOR] & 0x80)
		App.pdistruct->curKeyState[DIK_PRIOR]++;
	else
		App.pdistruct->curKeyState[DIK_PRIOR] = 0;

	if(App.pdistruct->keys[DIK_LEFT] & 0x80)
		App.pdistruct->curKeyState[DIK_LEFT]++;
	else
		App.pdistruct->curKeyState[DIK_LEFT] = 0;

	if(App.pdistruct->keys[DIK_RIGHT] & 0x80)
		App.pdistruct->curKeyState[DIK_RIGHT]++;
	else
		App.pdistruct->curKeyState[DIK_RIGHT] = 0;

	if(App.pdistruct->keys[DIK_END] & 0x80)
		App.pdistruct->curKeyState[DIK_END]++;
	else
		App.pdistruct->curKeyState[DIK_END] = 0;

	if(App.pdistruct->keys[DIK_DOWN] & 0x80)
		App.pdistruct->curKeyState[DIK_DOWN]++;
	else
		App.pdistruct->curKeyState[DIK_DOWN] = 0;

	if(App.pdistruct->keys[DIK_NEXT] & 0x80)
		App.pdistruct->curKeyState[DIK_NEXT]++;
	else
		App.pdistruct->curKeyState[DIK_NEXT] = 0;

	if(App.pdistruct->keys[DIK_INSERT] & 0x80)
		App.pdistruct->curKeyState[DIK_INSERT]++;
	else
		App.pdistruct->curKeyState[DIK_INSERT] = 0;

	if(App.pdistruct->keys[DIK_DELETE] & 0x80)
		App.pdistruct->curKeyState[DIK_DELETE]++;
	else
		App.pdistruct->curKeyState[DIK_DELETE] = 0;

	if(App.pdistruct->keys[DIK_LWIN] & 0x80)
		App.pdistruct->curKeyState[DIK_LWIN]++;
	else
		App.pdistruct->curKeyState[DIK_LWIN] = 0;

	if(App.pdistruct->keys[DIK_RWIN] & 0x80)
		App.pdistruct->curKeyState[DIK_RWIN]++;
	else
		App.pdistruct->curKeyState[DIK_RWIN] = 0;

	if(App.pdistruct->keys[DIK_DOWN] & 0x80)
		App.pdistruct->curKeyState[DIK_DOWN]++;
	else
		App.pdistruct->curKeyState[DIK_DOWN] = 0;

	if(App.pdistruct->keys[DIK_NEXT] & 0x80)
		App.pdistruct->curKeyState[DIK_NEXT]++;
	else
		App.pdistruct->curKeyState[DIK_NEXT] = 0;

	if(App.pdistruct->keys[DIK_HOME] & 0x80)
		App.pdistruct->curKeyState[DIK_HOME]++;
	else
		App.pdistruct->curKeyState[DIK_HOME] = 0;
*/

	return 0;
}	/* end CheckKeyState */





//***********************************************************************
// Function: CheckJoystickState
//
// Purpose:  Checks the state of the joystick and notes any changes
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC CheckJoystickState()
{
	static int pass = 0;

	if(App.pdistruct->DIVer[JOYSTICK_VER] == 5)
	{
	App.pdistruct->lpDIJoystick2->Poll();

	// This function checks the immediate state of the joystick
	App.pdistruct->hRet = App.pdistruct->lpDIJoystick2->GetDeviceState(sizeof(DIJOYSTATE), &App.pdistruct->joystick);

	if(App.pdistruct->hRet == DIERR_INPUTLOST)
		App.pdistruct->lpDIJoystick2->Acquire();
	else if(FAILED(App.pdistruct->hRet) )
	{
#ifndef DEBUG
		MessageBox(App.pdistruct->Hwnd, "Error getting the state of the joystick in CheckKeyState. Please restart game.", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);		
#endif
	}


	
	for(unsigned int i = 0; i < App.pdistruct->JoyDescHead.pStart->iNumButtons; i++)
	{
		if(App.pdistruct->joystick.rgbButtons[0] & 0x80);
		//	wsprintf(test, "%s", "shoot");
		else;
		//	wsprintf(test, "%s", "noshoot");
	}
	


	if(App.pdistruct->joystick.lX < JOYMAX + JOYMIN);
	//	wsprintf(test, "%s", "left");
	else if(App.pdistruct->joystick.lX > JOYMAX + JOYMIN);
	//	wsprintf(test, "%s", "right");
	else;
	//	wsprintf(test, "%s", "noleft");
	}  // end if for Joystick DX 5 or higher
	
	


	return 0;
} /* end CheckJoystickState */







//***********************************************************************
// Function: CheckMouseState
//
// Purpose:  Checks the state of the mouse and notes any changes
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
int FC CheckMouseState(char *test)
{
	if(App.pdistruct->DIVer[MOUSE_VER] == 5)
	{
	// This function checks the immediate state of the keyboard
//	App.pdistruct->hRet = App.pdistruct->lpDIMouse2->GetDeviceState(sizeof(App.pdistruct->keys), App.pdistruct->keys);

	App.pdistruct->lpDIMouse2->Poll();

	App.pdistruct->hRet = App.pdistruct->lpDIMouse2->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
															  App.pdistruct->mousebuffer,
															  &App.pdistruct->dwMouseItems,
															  0);

	if(App.pdistruct->hRet == DIERR_INPUTLOST)
		App.pdistruct->lpDIMouse2->Acquire();
	else if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error getting the state of the mouse in CheckMouseState", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	

	}	// end if for DirectX 5 mouse

	else if(App.pdistruct->DIVer[MOUSE_VER] == 3)
	{
	// This function checks the immediate state of the keyboard
//	App.pdistruct->hRet = App.pdistruct->lpDIMouse2->GetDeviceState(sizeof(App.pdistruct->keys), App.pdistruct->keys);

	App.pdistruct->hRet = App.pdistruct->lpDIMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
															  App.pdistruct->mousebuffer,
															  &App.pdistruct->dwMouseItems,
															  0);

	if(App.pdistruct->hRet == DIERR_INPUTLOST)
		App.pdistruct->lpDIMouse->Acquire();
	else if(FAILED(App.pdistruct->hRet) )
	{
		MessageBox(App.pdistruct->Hwnd, "Error getting the state of the mouse in CheckMouseState", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	}	
    if(App.pdistruct->hRet == DIERR_NOTACQUIRED)
	{
		MessageBox(App.pdistruct->Hwnd, "Mouse not acquired", "ERROR", MB_OK);
		DisplayDInputError(App.pdistruct->Hwnd, App.pdistruct->hRet);
		return 1;
	} 

	}	// end if for DX 3 mouse


	// Loop test for buffered mouse input
	for(unsigned int i = 0;i < App.pdistruct->dwMouseItems; i++)
	{
		if(App.pdistruct->mousebuffer[i].dwOfs == DIMOFS_BUTTON0)
		{
			if(App.pdistruct->mousebuffer[i].dwData & 0x80)
			{
				strcpy(test, "mouse 0 down");
				MessageBox(App.pdistruct->Hwnd, "Mouse", "Whoa", MB_OK);
			}
		}
	}
															  
	return 0;
}



//***********************************************************************
// Function: ReleaseDIObjects
//
// Purpose:  Releases the DirectInput object and any devices which may
//			 have been used.
//
// Parameters: None
//			  
//
// Returns: int - 0 for success, 1 for error
//
// Last Modified:  Date 9/24/99      Author - Max Szlagor     
//
//***********************************************************************
void FC ReleaseDIObjects()
{

    if (App.pdistruct->lpddi) 
    { 
		// First check the version of the keyboard to release
		if(App.pdistruct->DIVer[KEYBOARD_VER] == 3)
		{
        if (App.pdistruct->lpDIKeyboard) 
        { 
             App.pdistruct->lpDIKeyboard->Unacquire(); 
             App.pdistruct->lpDIKeyboard->Release();
             App.pdistruct->lpDIKeyboard = NULL; 
        } 
		}
		else if(App.pdistruct->DIVer[KEYBOARD_VER] == 5)
		{
        if (App.pdistruct->lpDIKeyboard2) 
        { 
             App.pdistruct->lpDIKeyboard2->Unacquire(); 
             App.pdistruct->lpDIKeyboard2->Release();
             App.pdistruct->lpDIKeyboard2 = NULL; 
        } 
		}

		// Now check the version of the mouse to release
		if(App.pdistruct->DIVer[MOUSE_VER] == 3)
		{
        if (App.pdistruct->lpDIMouse) 
        { 
             App.pdistruct->lpDIMouse->Unacquire(); 
             App.pdistruct->lpDIMouse->Release();
             App.pdistruct->lpDIMouse = NULL; 
        } 
		}
		else if(App.pdistruct->DIVer[MOUSE_VER] == 5)
		{
        if (App.pdistruct->lpDIMouse2) 
        { 
             App.pdistruct->lpDIMouse2->Unacquire(); 
             App.pdistruct->lpDIMouse2->Release();
             App.pdistruct->lpDIMouse2 = NULL; 
        } 
		}

		// Now check the version of the joystick to release
		if(App.pdistruct->DIVer[JOYSTICK_VER] == 3)
		{
        if (App.pdistruct->lpDIJoystick) 
        { 
             App.pdistruct->lpDIJoystick->Unacquire(); 
             App.pdistruct->lpDIJoystick->Release();
             App.pdistruct->lpDIJoystick = NULL; 
        } 
		}
		else if(App.pdistruct->DIVer[JOYSTICK_VER] == 5)
		{
        if (App.pdistruct->lpDIJoystick2) 
        { 
             App.pdistruct->lpDIJoystick2->Unacquire(); 
             App.pdistruct->lpDIJoystick2->Release();
             App.pdistruct->lpDIJoystick2 = NULL; 
        } 
		}

		// Now release the DirectInput object
        App.pdistruct->lpddi->Release();
        App.pdistruct->lpddi = NULL; 
		FreeJoystickList();
    } 
}





#undef FC
