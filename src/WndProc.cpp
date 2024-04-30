#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <time.h>
#include <string>
using namespace std;

#include "./mmgr.h"

#include "dplay.h"

//#include "database.h"
#include "app.h"
#include "timerupdate.h"
#include "gamemenu.h"

#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"

extern APP			App;
extern GAME_DATA	GameData;

extern CSound		cSound;
extern GAME_DATA	GameData;

void SetKeyboardFlags(UINT message, WPARAM wParam);

void Input_Init();
void Input_Destroy();

//////////////////////////////////////////////////////////////////////////////
//WndProc Function
LRESULT CALLBACK WndProc_DDraw (HWND Hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if( App.pDDraw && App.pDDraw->IsFullScreen() )
			ShowCursor(0);
		break;

    case WM_SYSKEYUP:
        switch( wParam )
        {
			
            // handle ALT+ENTER ( fullscreen/windowed switch )
			case VK_RETURN:

				if (App.pDDraw->IsAllowFullScreen())
				{
					//*** Shut down dinput
					Input_Destroy();

					App.pDDraw->SwitchModes ();
					if(App.pDDraw->IsFullScreen())
						ShowCursor(0);
					else
						ShowCursor(1);

					//** Init DirectInput here
					Input_Init();
				}

				break;

			case VK_TAB:
				if(App.pDDraw->IsFullScreen())
				{	
					if (App.pDDraw->IsAllowFullScreen())
					{
						App.pDDraw->SwitchModes ();
						if(App.pDDraw->IsFullScreen())
							ShowCursor(0);
					}
				}

				App.SetActive(FALSE);//PostQuitMessage(0);
				
				break;

			case VK_F1:	//ALT+F1
				break;
        }
        return 0;
	
	case WM_SETFOCUS:
		App.SetActive(TRUE);
		//GameData.Timer.StartTime();
		return 0;

	case WM_KILLFOCUS:
		App.SetActive(FALSE);
		//GameData.Timer.StopTime();
		return 0;

// The WM_ACTIVATEAPP message is sent when a window belonging to a different 
// application than the active window is about to be activated. The message is 
// sent to the application whose window is being activated and to the application whose window is being deactivated. 
	case WM_ACTIVATEAPP:
		App.SetActive( wParam );

		//If we are not ACTIVE then pause our game
		if(wParam)
		{
			GameData.Timer.StartTime();
			
			if( cSound.IsSoundInitialized() )
				cSound.UnpauseCurrentTrack();
		}
		else
		{
			GameData.Timer.StopTime();
			
			if( cSound.IsSoundInitialized() )
				cSound.PauseCurrentTrack();
		}
		break;

	case WM_MOVE:

		// 
		// Get the rectangle for our client window
		GetClientRect( App.pDDraw->getHwnd(), &App.pDDraw->rcWindow );
		// 
		// Convert rectangle to match our client area
		ClientToScreen( Hwnd, ( LPPOINT )&App.pDDraw->rcWindow );
		ClientToScreen( Hwnd, ( LPPOINT )&App.pDDraw->rcWindow + 1 );
		
		return 0;

	case WM_MOUSEMOVE:
		//Save the location of the cursor		
		App.SetMousePos(LOWORD(lParam), HIWORD(lParam));

		return 0;
	

	case WM_SETCURSOR:
		break;

	case WM_KEYUP:
	case WM_KEYDOWN:

		SetKeyboardFlags(uMsg, wParam);
		break;

	case WM_LBUTTONDOWN:
		return 0;

	case WM_LBUTTONUP:
		return 0;

	case WM_RBUTTONDOWN:
		return 0;

	case WM_RBUTTONUP:
		return 0;

	case WM_DESTROY:
		if(App.pDDraw->IsFullScreen())
			ShowCursor(1);

		if(!App.IsSwitchingMode())
		{
			//PQM
			PostQuitMessage (0);
		}
		return 0;
	}
	return DefWindowProc (Hwnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK WndProc_OpenGL (HWND Hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	

	switch (uMsg) 
	{
	case WM_CREATE:
		if(App.pOpenGL && App.pOpenGL->IsFullScreen())
		{
			ShowCursor(0);
		}
		break;

	case WM_SETFOCUS:
		App.SetActive(TRUE);
		break;

	case WM_KILLFOCUS:
		App.SetActive(FALSE);
		break;

	case WM_SYSKEYUP:
		switch( wParam )
		{
			// handle ALT+ENTER ( fullscreen/windowed switch )
			case VK_RETURN:
				{
					//*** Shut down dinput
					Input_Destroy();

					//if (App.pOpenGL->IsAllowFullScreen())
						App.pOpenGL->SwitchModes ();
					

					if(App.pOpenGL->IsFullScreen())
					{
						ShowCursor(0);
					}
					else
					{
						ShowCursor(1);
					}
					//*** Init dinput
					Input_Init();

				}
				break;

			case VK_TAB:
					
					if(App.pOpenGL->IsFullScreen())
					{
						//*** Shut down dinput
						Input_Destroy();

						App.pOpenGL->SwitchModes ();
						
						if(App.pOpenGL->IsFullScreen())
							ShowCursor(0);	

						//*** Init dinput
						Input_Init();
					}

					App.SetActive(FALSE);//PostQuitMessage(0);
					
				break;

			case VK_F1:	//ALT+F1
				break;
		}
        break;


	case WM_MOUSEMOVE:
		//Save the location of the cursor		
		App.SetMousePos(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_SETCURSOR:
		break;

	case WM_KEYUP:
	case WM_KEYDOWN:

		SetKeyboardFlags(uMsg, wParam);
		break;

// The WM_ACTIVATEAPP message is sent when a window belonging to a different 
// application than the active window is about to be activated. The message is 
// sent to the application whose window is being activated and to the application whose window is being deactivated. 
	case WM_ACTIVATEAPP:
		App.SetActive( wParam );

		//If we are not ACTIVE then pause our game
		if(wParam)
		{
			GameData.Timer.StartTime();
			
			if( cSound.IsSoundInitialized() )
				cSound.UnpauseCurrentTrack();
		}
		else
		{
			GameData.Timer.StopTime();

			if( cSound.IsSoundInitialized() )
				cSound.PauseCurrentTrack();
		}

		break;

	case WM_DESTROY:
		if(App.pOpenGL->IsFullScreen())
			ShowCursor(1);

		if(!App.IsSwitchingMode())
		{
			//OpenGLMakeCurrent(NULL, NULL) ;
			wglMakeCurrent(NULL, NULL);

			if (App.pOpenGL->GethRC() ) 
			{ 
				//OpenGLDeleteContext(App.pOpenGL->GethRC() );
				wglDeleteContext(App.pOpenGL->GethRC() );
				App.pOpenGL->SethRC(0);
			}
			if (App.pOpenGL->GethDC() )
			{ 
				ReleaseDC(App.pOpenGL->getHwnd(), App.pOpenGL->GethDC() );
				App.pOpenGL->SethDC(0); 
			}

			PostQuitMessage (0);
		}
		break;
	}
	
	return DefWindowProc (Hwnd, uMsg, wParam, lParam);;
}