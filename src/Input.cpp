#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>

#include "./mmgr.h"

#include "dplay.h"
#include "dinput.h"
//#include "database.h"

#include "app.h"

extern APP App;
extern GAME_DATA GameData;

void SetKeyboardFlags(UINT message, WPARAM wParam)
{
    switch (message)
    {
	case WM_KEYUP:
		
		if(wParam == VK_RETURN)
		{
			GameData.bDrawSwitcher = false;
		}

//		if( GameData.keysDown[wParam] )
//			GameData.keysKeyUp[wParam]	= 1;
/*		if(wParam == VK_CONTROL)
		{
			GameData.keysDown[VK_RETURN] = 1;
		}
		else if(wParam == VK_SHIFT)
		{
			GameData.keysDown[VK_SPACE] = 0;
		}
*/		GameData.keysDown[wParam]	= 0;
//		App.pdistruct->keys[wParam] = 0;
		break;

	case WM_KEYDOWN:
		// quit the game if necessary
		switch(wParam)
		{

			case VK_ESCAPE: //Quit menu/game
					GameData.OnEscapeKey();
					break;
			
			case VK_F1:
					//Previous track
					GameData.OnF1Key();
					break;
			
			case VK_F2:
					//Next track
					GameData.OnF2Key();
					break;
			
			case VK_F11:
					//Camera Mode
					GameData.OnF11Key();
					break;
			
			case VK_F12:
					//Debug mode (FPS)
					GameData.OnF12Key();
					break;
			
			case VK_RETURN:
					GameData.bDrawSwitcher = true;
					break;

/*		else case VK_CONTROL)
		{
			GameData.keysDown[VK_RETURN] = 1;
		}
		else case VK_SHIFT)
		{
			GameData.keysDown[VK_SPACE] = 1;
		}
*/
		}

		GameData.keysDown[wParam]	= 1;
//		App.pdistruct->keys[wParam]	= 1;
		break;

	default:
		break;
	}
}