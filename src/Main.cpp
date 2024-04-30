#include <windows.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <time.h>
#include <stdio.h>
#include <dinput.h>
#include <dsound.h>
#include <string>

using namespace std;

#include "./mmgr.h"

#include "dplay.h"


//#include "database.h"
#include "app.h"
#include "font.h"
#include "gamemenu.h"
#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "camera3d.h"
#include "player.h"
#include "board.h"
#include "dinput.h"
#include "dsound.h"
#include "timerupdate.h"

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


APP					App;
pSURFACERECORD		pSurfRecordHead;
pCALLOC_MEMRECORD	pMemRecordHead;
GAME_DATA			GameData;
//DSound				Sound;
CSound				cSound;

HINSTANCE			g_hInstance;
HWND				g_hWnd;

// FUNCTION PROTOTYPE(S)
int	WINAPI	 MainLoop_DDraw(HINSTANCE hInst, int iCmdShow);
int	WINAPI   MainLoop_OpenGL(HINSTANCE hInst, int iCmdShow);




void Input_Init()
{
	//** Init DirectInput here
	InitDInputStruct(g_hWnd, g_hInstance );
	InitDirectInput();								
    InitDirectKeyInput();
	InitDirectJoystickInput();
}

void Input_Destroy()
{
	//Destroy input
	ReleaseDIObjects();
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int iCmdShow)
{
	int	ReturnVal = 0;
	
	g_hInstance = hInst;

	//Seed the rand
	srand (time(NULL));

	ZeroMemory(&App, sizeof(APP) );
	
	App.UserSettings.Initialize();

	App.InitTrig();
	
	App.pdistruct = NULL;
//	App.pdistruct = (DISTRUCT *)calloc(1, sizeof(DISTRUCT) );
	App.pdistruct = (DISTRUCT *)malloc( sizeof( DISTRUCT ) );
	ZeroMemory(App.pdistruct, sizeof( DISTRUCT ) );
	
	//Init our database
	GameData.DBase.Init();
	
	//***Loop depending upon our rendering mode 
	( App.UserSettings.RenderMode ) ?
		ReturnVal = MainLoop_OpenGL(hInst, iCmdShow):
		ReturnVal = MainLoop_DDraw (hInst, iCmdShow);
	
	//Save our database
	GameData.DBase.SaveDatabase( DATABASE_FILENAME );
	
	//Destroy sound
#ifdef SOUND
	cSound.Destroy();
#endif
	
	//Destroy input
	ReleaseDIObjects();
	
	//Destroy our GameData
	GameData.Destroy();
	
	if(App.pdistruct)
		free(App.pdistruct);
	
	//Reset the screen back to normal
	ChangeDisplaySettings(NULL, 0);
	
	return ReturnVal;
}


int WINAPI MainLoop_DDraw(HINSTANCE hInst, int iCmdShow)
{
	int				bIsMessage	= FALSE;
    MSG				msg;
	API_DDRAW		API_DDraw;
	
	ZeroMemory(&API_DDraw, sizeof(API_DDRAW) );
	
	App.SetActive(1);
	
	//***Init our API pointers in App
	App.SetpDDraw(&API_DDraw);
	App.SetpOpenGL(NULL);
	App.pDDraw->sethInstance(hInst);
	App.pDDraw->setiCmdShow(iCmdShow);
	
	//***Init DirectDraw
	if( App.pDDraw->Initialize() )
	{
		MessageBox(App.pDDraw->getHwnd(), "We can't init DDraw", "damn", MB_OK);
		return 0;
	}
	
	//** Init DirectInput here
	InitDInputStruct(App.pDDraw->getHwnd(), hInst );
	InitDirectInput();								
    InitDirectKeyInput();
	InitDirectJoystickInput();

	//** All Sound initialization goes here
	HWND tempHwnd;
	tempHwnd = App.pDDraw->getHwnd();

#ifdef SOUND

	// Get all data from the sound file
	if(!cSound.ParseSoundFile( SOUNDS_FILENAME ) )
		MessageBox(App.pDDraw->getHwnd(), "Error Parsing Sound File", "ERROR", MB_OK);


	if(cSound.Init(tempHwnd) )
	{
		// Load in all the music and sound effects
		cSound.LoadAllMusic(App.pDDraw->getHwnd());
		cSound.LoadAllStaticSounds(App.pDDraw->getHwnd());
		
		//Start playing lobby music if you can
		if( 0 == cSound.PlayTrack( "Flange.mp3" ) )
		{
			cSound.PlayRandomTrack();
		}
	}
	else
	{
		MessageBox(App.pDDraw->getHwnd(), "Error initializing sound", "ERROR", MB_OK);
	}
#endif

	//***Init MAIN MENU HERE
	GameData.pGameMenu	= new Game_Menu;
	GameData.pGameMenu->Initialize(MENU_ID_TITLESCREEN);
	
	//*** Init our Timer
	GameData.Timer.Init();

	SetWindowText(tempHwnd, GAME_NAME );

	//*************************
	//**** MAIN WHILE LOOP ****
	//*************************
	while(1)
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)	break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if( App.IsActive() )//we are in focus (app is active)
		{

#ifdef DINPUT
			//GET INPUT
			if( CheckKeyState() )
				PostQuitMessage( 0 );
			
			//Checking for the joystick state causes our game to hickup... ouch!
			//CheckJoystickState();
#endif

			if( !(App.IsPaused()) )
			{
				//UPDATE
				GameData.Update();
			}
			
			//DRAW
			GameData.Timer.PreDraw();
			if(App.pDDraw->RedrawScreen((LPVOID)&App))//pass void pointer for thread friendly lovin
				return 0;
			GameData.Timer.PostDraw();

		}
		else//we are not in focus (app is not active)
		{	
			WaitMessage();	//so we don't suck up cpu time when the appication is not running
		}
	}
    return(0);
}

int WINAPI MainLoop_OpenGL(HINSTANCE hInst, int iCmdShow)
{
	int				bIsMessage	= FALSE;
    MSG				msg;
	API_OPENGL		API_OpenGL;
	FONT			Font;

	ZeroMemory(&API_OpenGL, sizeof (API_OPENGL) );
	
	App.SetActive(1);

	//***Init our API pointers in App
	App.SetpOpenGL(&API_OpenGL);
	App.SetpDDraw(NULL);
	App.pOpenGL->SethInstance(hInst);
	App.pOpenGL->SetiCmdShow(iCmdShow);
	App.pOpenGL->SetpFont(&Font);

	//***Init OpenGL
	if( App.pOpenGL->Initialize() )
		return 0;

	//** Init DirectInput here
	InitDInputStruct(App.pOpenGL->getHwnd(), hInst );
	InitDirectInput();								
    InitDirectKeyInput();
	InitDirectJoystickInput();

	//** All Sound initialization goes here
	HWND hwndTemp;
	hwndTemp = App.pOpenGL->getHwnd();

#ifdef SOUND

	// Get all data from the sound file
	if(!cSound.ParseSoundFile( SOUNDS_FILENAME ) )
		MessageBox(App.pOpenGL->getHwnd(), "Error Parsing Sound File", "ERROR", MB_OK);


	if(cSound.Init(hwndTemp) )
	{
		// Load in all the music and sound effects
		cSound.LoadAllMusic(App.pOpenGL->getHwnd());
		cSound.LoadAllStaticSounds(App.pOpenGL->getHwnd());	

		//Start playing lobby music if you can
		if(0 == cSound.PlayTrack( "Flange.mp3" ))
		{
			cSound.PlayRandomTrack();
		}
	}
	else
	{
		MessageBox(App.pOpenGL->getHwnd(), "Error initializing sound", "ERROR", MB_OK);
	}
#endif

	//***Init MAIN MENU HERE
	GameData.pGameMenu	= new Game_Menu;
	GameData.pGameMenu->Initialize(MENU_ID_TITLESCREEN);

	//*** Init our Timer
	GameData.Timer.Init();
	
	SetWindowText(hwndTemp, GAME_NAME );

	//*************************
	//**** MAIN WHILE LOOP ****
	//*************************
	while(1)
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT)	break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if( App.IsActive() )//we are in focus (app is active)
		{

#ifdef DINPUT

			//GET INPUT
			if( CheckKeyState() )
				PostQuitMessage( 0 );
			//Checking for the joystick state causes our game to hickup... ouch!
			//CheckJoystickState();
#endif

			if( !(App.IsPaused()) )
			{
				//UPDATE
				GameData.Update();
			}
			
			//DRAW
			GameData.Timer.PreDraw();
			if(App.pOpenGL->RedrawScreen((LPVOID)&App) )//pass void pointer for thread friendly lovin
				return 0;
			GameData.Timer.PostDraw();

		}
		else//we are not in focus (app is not active)
		{	
			WaitMessage();	//so we don't suck up cpu time when the appication is not running
		}
	}

    return(0);
}
