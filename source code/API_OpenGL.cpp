#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <string>
#include <gl/gl.h>
#include <gl/glu.h>

#include <math.h>	//for resizeViewPort
#include <stdio.h>
#include <windowsx.h> // for GetWindowStyle()

using namespace std;

#include "./mmgr.h"
#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "tga.h"
#include "font.h"
#include "Bitmap.h"
#include "Gamemenu.h" //used by SwitchModes()
#include "dinput.h"

#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"
#include "dsound.h"

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"

#include "GLSurfDef.h"


extern APP			App;
extern HWND			g_hWnd;

extern GAME_DATA	GameData;
//extern DSound		Sound;
extern CSound		cSound;


LRESULT CALLBACK WndProc_OpenGL (HWND Hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

#ifndef PI
#define PI 3.141592653589793F
#endif

void GLSetup2DMode();

int tagAPI_OpenGL::Initialize()
{
	//Get our USER settings 
	bFullScreen			= App.UserSettings.bFullScreen;
	int UserWidth		= App.UserSettings.ScreenWidth;	
	int UserHeight		= App.UserSettings.ScreenHeight;
	int UserBitDepth	= App.UserSettings.BitDepth;
	
	//*****************
	//	char	glDriver[256];
	//	OpenGLFindDriver(glDriver);
	//	OpenGLInit(glDriver);
	//	TimingInit();
	
	//Register the window class
	WNDCLASSEX  wndclass;
	ZeroMemory( &wndclass, sizeof( WNDCLASSEX ) );
	wndclass.cbSize        = sizeof (wndclass);												
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;// | CS_OWNDC;	
	wndclass.lpfnWndProc   = WndProc_OpenGL; 
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon (hInstance, "ICON");//0;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL;//(void *)COLOR_GRAYTEXT; 
	wndclass.lpszMenuName  = GAME_NAME;
	wndclass.lpszClassName = GAME_NAME;
	wndclass.hIconSm       = 0;
	if (!RegisterClassEx(&wndclass))
	{
		MessageBox(NULL, "Could not RegisterClass!", "error", MB_OK);
		return 1;
	}
	
	//if user set the fullscreen boolean then init fullscreen otherwise windowed.
	(bFullScreen) ? InitFullScreen(UserWidth, UserHeight, UserBitDepth) :
	InitWindowedScreen(UserWidth, UserHeight);
	
	//Set the pixel format for OpenGL
	SetGLPixelFormat(UserBitDepth, UserBitDepth);
	
	//Init our Default settings for OpenGL
	SetDefaultSettings();
	
	//Init our main font
	//***Init Font
	if( pFont->Initialize(GL_SURF_MAIN_SYSFONT_TEXTID) )
		return 1;
	
	//Load our main Textures
	if( LoadMainTextures() )
		return 1;

	return 0;
}

int tagAPI_OpenGL::InitFullScreen(int Width, int Height, int BitDepth)
{
	if(!bFullScreen)
		bFullScreen=1;
	
	if( ChangeResolution(Width, Height, BitDepth) )
	{
		MessageBox(Hwnd, "ChangeResolution Failed", "sorry", MB_OK);
		return 1;
	}	
	
	Hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,//|WS_EX_TOPMOST	// extended window style
		GAME_NAME,							// pointer to registered class name
		GAME_NAME,							// pointer to window name
		WS_POPUP |  WS_CLIPSIBLINGS,		// window style
		0,									// horizontal position of window
		0,									// vertical position of window
		GetSystemMetrics(SM_CXSCREEN),		// window width
		GetSystemMetrics(SM_CYSCREEN),		// window height
		NULL,								// handle to parent or owner window
		NULL,								// handle to menu, or child-window identifier
		hInstance,							// handle to application instance
		NULL);								// pointer to window-creation data
	
	// Make sure our window was created 
	if (!Hwnd)
	{	
		char buff[64];
		int i = GetLastError();
		sprintf(buff, "Could not create Hwnd, error %d", i );
		MessageBox(NULL, buff, "error", MB_OK);
		if(i == 1407)
			MessageBox(NULL, "Cannot find Window Class!!!", "error 1407", MB_OK);
		else if(i == 2)
			MessageBox(NULL, "Regesterd class name probably diffrent\n then what was passed into CreateWin", "error 2", MB_OK);
		return 1;
	}
	
	g_hWnd = Hwnd;
	
	// Show and update main window 
	ShowWindow(Hwnd, iCmdShow);
	UpdateWindow(Hwnd);
	
	return 0;
}

int tagAPI_OpenGL::InitWindowedScreen(int Width, int Height)
{
	int  winHeight;
	int  winWidth;
	
	if(bFullScreen)
		bFullScreen=0;
	
	winHeight	= GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXDLGFRAME)*2 + Height;
	winWidth	= GetSystemMetrics(SM_CXDLGFRAME)*2 + Width;
	
	//Get handle to the window from OS
	Hwnd = CreateWindowEx (
		0,//WS_EX_TOPMOST,			// extended window style
		GAME_NAME,					// pointer to registered class name
		GAME_NAME,					// pointer to window name
		WS_SYSMENU  | WS_MINIMIZEBOX,	// window style
		GetSystemMetrics (SM_CXSCREEN)/2 - 640 / 2,	// horizontal position of window
		GetSystemMetrics (SM_CYSCREEN)/2 - 480 / 2,	// vertical position of window
		0,							// window width
		0,							// window height
		NULL,						// handle to parent or owner window
		NULL,						// handle to menu, or child-window identifier
		hInstance,					// handle to application instance
		NULL);						// pointer to window-creation data


	
	// Make sure our window was created 
	if (!Hwnd)
	{	
		char buff[64];
		int i = GetLastError();
		sprintf(buff, "Could not create Hwnd, error %d", i );
		MessageBox(NULL, buff, "error", MB_OK);
		if(i == 1407)
			MessageBox(NULL, "Cannot find Window Class!!!", "error 1407", MB_OK);
		else if(i == 2)
			MessageBox(NULL, "Regesterd class name probably diffrent\n then what was passed into CreateWin", "error 2", MB_OK);
		return 1;
	}
	
	g_hWnd = Hwnd;
	
	SetRect(&rcWindow, 0, 0, Width, Height);
	
    AdjustWindowRect(&rcWindow, GetWindowStyle(Hwnd), GetMenu(Hwnd) != NULL);
	
    SetWindowPos(Hwnd, NULL, 0, 0,
		rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top,
		SWP_NOCOPYBITS | SWP_NOMOVE | SWP_NOZORDER);
	
    // Show and update main window 
	ShowWindow(Hwnd, iCmdShow);
	UpdateWindow(Hwnd);

	return 0;
}

void tagAPI_OpenGL::SwitchModes()
{

	App.SetSwitchingMode(TRUE);
	
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

	DestroyWindow (Hwnd);

	if (bFullScreen)
	{

		//Return our display settings back to normal
		ChangeDisplaySettings(NULL, 0); 

		if(App.pOpenGL->InitWindowedScreen(App.UserSettings.ScreenWidth,
			App.UserSettings.ScreenHeight) )
		{
			MessageBox(Hwnd, "Cannont switch modes into Windowed", "i wonder why..",MB_OK);
			return;
		}
		
	
		ShowWindow(Hwnd, iCmdShow);


		// Reinitialize dinput here
		App.pdistruct->~DISTRUCT();

		InitDInputStruct(App.pOpenGL->getHwnd(), App.pOpenGL->GethInstance() );
		InitDirectInput();								
		InitDirectKeyInput();
		InitDirectJoystickInput();
		
		bFullScreen = FALSE; 
	}
	else
	{
		
		if(App.pOpenGL->InitFullScreen(App.UserSettings.ScreenWidth,
			App.UserSettings.ScreenHeight, App.UserSettings.BitDepth) )
		{
			MessageBox(Hwnd, "Cannont switch modes into FullScreen", "i wonder why..",MB_OK);
			return;
		}

		//	App.pdistruct->~DISTRUCT();

		InitDInputStruct(App.pOpenGL->getHwnd(), App.pOpenGL->GethInstance() );
		InitDirectInput();								
		InitDirectKeyInput();
		InitDirectJoystickInput();
		
		bFullScreen = TRUE;
	}
	App.SetSwitchingMode(FALSE);
	
	//Set the pixel format for OpenGL
	SetGLPixelFormat(App.UserSettings.BitDepth, App.UserSettings.BitDepth);
	
	//Init our Default settings for OpenGL
	SetDefaultSettings();
	
	//Init our main font
	//***Init Font
	if( pFont->Initialize(100) )
	{
		MessageBox(Hwnd, "Font can't Initialize", "i wonder why..",MB_OK);
		return;
	}
	//Load our main Textures
	if( LoadMainTextures() )
	{
		MessageBox(Hwnd, "LoadMainTextures is a sucky function.", "i wonder why..",MB_OK);
		return;
	}

	//Always load the menu surfaces
	GameData.pGameMenu->LoadSurfaces();

	//If the game has been loaded before.. .load it again.
	if(GameData.pBoard)
	{
		GameData.pBoard->LoadSurfaces();
	}

//	if(GameData.GameStateFlags & GAME_STATE_GAME)
//	{	
//		GameData.pGameMenu->InitGameButtons();
//	}
//	else if(GameData.GameStateFlags & GAME_STATE_MENU)
//	{	
//		//if we are in the menu re-link our buttons
//		ReLinkButtonSurfaces();
//	}	
}

tagAPI_OpenGL::~tagAPI_OpenGL()
{
	//CLEAN UP ELI's CRAP!!! -Eli
	glCleanUpParticleEngine();
}


int tagAPI_OpenGL::SetGLPixelFormat(int BitDepth, int ZDepth) 
{
	PIXELFORMATDESCRIPTOR pfd;
	int                   PixelFormat;
	
	ZeroMemory( &pfd, sizeof(PIXELFORMATDESCRIPTOR) );
	
	pfd.nSize			= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion		= 1;
	pfd.dwFlags			= PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	pfd.iLayerType		= PFD_MAIN_PLANE;
	pfd.dwLayerMask		= PFD_MAIN_PLANE;
	pfd.iPixelType		= PFD_TYPE_RGBA;
	pfd.cColorBits		= BitDepth;//BITDEPTH;
	pfd.cDepthBits		= ZDepth;//ZDEPTH;
	pfd.cAccumBits		= 0;
	pfd.cStencilBits	= 0;
	
	//Get our window's DC (it's needed for initing the pixel format
	hDC	= GetDC( Hwnd );
	
	PixelFormat = ChoosePixelFormat( hDC, &pfd );
	SetPixelFormat( hDC, PixelFormat, &pfd );
	
	// create and enable the render context (RC)
	hRC	= wglCreateContext( hDC );
	wglMakeCurrent( hDC, hRC );
	
	return 0;
}

int tagAPI_OpenGL::LoadMainTextures() 
{
	//************LOAD THE NEEDED TEXTURES from targas*****************
	int error[6];
	
	//Font
	LoadGLBitmap(GL_SURF_MAIN_FONT_FILENAME, GL_SURF_MAIN_FONT_TEXID, 1, 255);

	LoadGLBitmap(GL_SURF_MAIN_TITLE00_FILENAME, GL_SURF_MAIN_TITLE00_TEXID, 1, 255);

	LoadGLBitmap(GL_SURF_MAIN_TITLE01_FILENAME, GL_SURF_MAIN_TITLE01_TEXID, 1, 255);

	//Specail OpenGL ONLY textures
	error[0]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_RED_FILENAME,
								GL_SURF_GAME_PARTICLE_RED_TEXID); //0

	error[1]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_YELLOW_FILENAME,
								GL_SURF_GAME_PARTICLE_YELLOW_TEXID);

	error[2]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_GREEN_FILENAME,
								GL_SURF_GAME_PARTICLE_GREEN_TEXID);

	error[3]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_CYAN_FILENAME, 
								GL_SURF_GAME_PARTICLE_CYAN_TEXID);

	error[4]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_PURPLE_FILENAME,
								GL_SURF_GAME_PARTICLE_PURPLE_TEXID);

	error[5]	= LoadGLTarga (GL_SURF_GAME_PARTICLE_BLUE_FILENAME, 
								GL_SURF_GAME_PARTICLE_BLUE_TEXID);
	
	//Check all the error codes
	for(int i=0; i < 6; i++)
		switch(error[i])
	{
			case 13:MessageBox(Hwnd, "TGA file was not found","loadTGA error", MB_OK);return 1;
			case 14:MessageBox(Hwnd, "color mapped image or image is not uncompressed","loadTGA error", MB_OK);return 1;
			case 15:MessageBox(Hwnd, "dimension is not a power of 2","loadTGA error", MB_OK);return 1;
			case 16:MessageBox(Hwnd, "image bits is not 8, 24 or 32","loadTGA error", MB_OK);return 1;
			case 17:MessageBox(Hwnd, "image data could not be loaded","loadTGA error", MB_OK);return 1;
	}
	//*****************************************************************
	
	return 0;
}

int IsExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;

	extensions = glGetString(GL_EXTENSIONS);
	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings,
	etc. */

	start = extensions;
	for (;;) 
	{
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\0')
				return 1;
		}
		start = terminator;
	}
	return 0;
}

#ifdef _WIN32
typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC)(GLenum pname, GLfloat param);
typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC)(GLenum pname, const GLfloat *params);
#endif

#ifdef _WIN32
PFNGLPOINTPARAMETERFEXTPROC glPointParameterfEXT;
PFNGLPOINTPARAMETERFVEXTPROC glPointParameterfvEXT;
#endif



void tagAPI_OpenGL::SetDefaultSettings() 
{
	//*********VIEWPORT CODE******* 
	//Init viewing values
	glViewport (0, 0, 640, 480);	// we can't have 640 by 640 (it messes up 2Dratio 1:1
	glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	//gluPerspective (60, 640 / 480, 1, 9999);
	gluPerspective (53, 1, 1, 9999);
	//gluPerspective(0.60, 1.33, 10, 1000 );
	glMatrixMode(GL_MODELVIEW);//pg.105
	
//	glClearColor(0.267f, 0.216f, 0.150f, 0.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	//*****Lighting Setup*****
	//glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	//enable culling of cw faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//****DEFAULT GL ENABLES***
	//glEnable(GL_LINE_SMOOTH);
	glEnable(GL_FOG);
	glEnable(GL_BLEND);

	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

	//for depth buffering.  never seems to work
	//	glEnable(GL_DEPTH_TEST);
	
	//****GL FOG SETUP****
	//	GLfloat	fogColour[4]={0.3f,0.1f,0.1f,1.0f};//same as clear screen colour
//	GLfloat	fogColour[4]={0.267f,0.216f,0.150f,0.0f};//same as clear screen colour
	GLfloat	fogColour[4]={0.0f,0.0f,0.0f,0.0f};//same as clear screen colour
	//	glFogi(GL_FOG_MODE,GL_EXP2);
	glFogi(GL_FOG_MODE,GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColour);
	glFogf(GL_FOG_DENSITY, 0.25f);
	glFogf(GL_FOG_START, 800.0f);
	glFogf(GL_FOG_END, 3000.0f);
	
	//*** Check to see if the user supports anti-alising by nvidia
//	IsExtensionSupported( NV_MULTI
	
/*	
	int hasPointParams = IsExtensionSupported("GL_EXT_point_parameters");
	#ifdef _WIN32
	if (hasPointParams) 
	{
		glPointParameterfEXT = (PFNGLPOINTPARAMETERFEXTPROC)
			wglGetProcAddress("glPointParameterfEXT");
		glPointParameterfvEXT = (PFNGLPOINTPARAMETERFVEXTPROC)
			wglGetProcAddress("glPointParameterfvEXT");
	}
	
	//glEnable(GL_SMOOTHING);
	//glShadeModel(GL_SMOOTH);

	if( IsExtensionSupported("GL_ARB_multisample") )
	{

	}
//	GL_ARB_multisample
	#endif
*/
}

int tagAPI_OpenGL::ChangeResolution(int w, int h, int bitdepth)
{
	DEVMODE devMode;
	LONG	modeExist;
	LONG	modeSwitch;
	LONG	closeMode = 0;
	LONG	i;
	char    buf[256];
	
	//Goes through each mode from the DisplaySettings list and checks 
	// for a match with the user settings w, h and bitdepth.
	for (i=0; ;i++) 
	{
		modeExist = EnumDisplaySettings(NULL, i, &devMode);
		
		// Check if we ran out of modes 
		if (!modeExist) 
		{
			// Pony up an error message 
			sprintf(buf, "No matching mode to %ldx%ldx%ld-bit color available\n", w, h, bitdepth);
			MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK);
			return 1;
		}
		
		// Check for a matching mode with user settings
		if ( ((LONG)devMode.dmBitsPerPel == bitdepth) 
			&&((LONG)devMode.dmPelsWidth == w) 
			&&((LONG)devMode.dmPelsHeight == h) )
		{
#ifndef DEBUG
			sprintf(buf, "Trying %ldx%ldx%ld-bit color (%ld hertz)\n", devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel, devMode.dmDisplayFrequency);
			MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK);
#endif
			modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN); 
			
			if (modeSwitch==DISP_CHANGE_SUCCESSFUL) 
			{
				//MessageBox(Hwnd, "SUCCESS on changine modes", "LOG_BOOT", MB_OK);
				return 0; //SUCCESS!!!!
			}
			
			//MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK);
			
			//save our last mode so we can try again without the hertz change
			if (!closeMode) 
				closeMode = i;
		}
	}
	
	
	//Might be running in Windows95, let's try without the hertz change 
	EnumDisplaySettings(NULL, closeMode, &devMode);
	
	//force the user settings onto our devMode struct
	devMode.dmBitsPerPel	= bitdepth; 
	devMode.dmPelsWidth		= w; 
	devMode.dmPelsHeight	= h; 
	devMode.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
	
#ifndef DEBUG
	sprintf(buf, "Trying %ldx%ldx%ld-bit color (Win95 Hack)\n", devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
	MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK); //Log(LOG_BOOT, buf);
#endif
	
	modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN); 
	if(modeSwitch == DISP_CHANGE_SUCCESSFUL) 
		return 0; //SUCCESS!!!!
	
	//MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK); //Log(LOG_BOOT, "Nope, didnt work\n");
	
	// try with separate changes to bitdepth and resolution
	devMode.dmFields = DM_BITSPERPEL; 
	
	sprintf(buf, "Trying %ldx%ldx%ld-bit color (Separate BPP switch)\n", devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
	MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK); //Log(LOG_BOOT, buf);
	
	modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
	if(modeSwitch == DISP_CHANGE_SUCCESSFUL) 
	{
		devMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT; 
		
#ifndef DEBUG		
		sprintf(buf, "Trying %ldx%ldx%ld-bit color (Separate resolution switch)\n", devMode.dmPelsWidth, devMode.dmPelsHeight, devMode.dmBitsPerPel);
		MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK); //Log(LOG_BOOT, buf);
#endif
		
		modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN); 
		if(modeSwitch == DISP_CHANGE_SUCCESSFUL) 
			return 0; //SUCCESS!!!!
		
		ChangeDisplaySettings(NULL, 0);
	}
	//MessageBox(Hwnd, buf, "LOG_BOOT", MB_OK); //Log(LOG_BOOT, "Nope, didnt work\n");
	
	/* Well nothing works now does it */
	if(modeSwitch == DISP_CHANGE_RESTART)
	{ 
		sprintf(buf, "You must change your video mode to %ldx%ldx%ld-bit color and then restart\n", w, h, bitdepth);
		MessageBox(Hwnd, buf, "sorry", MB_OK); //Log(LOG_BOOT, buf);
	}
	else if(modeSwitch == DISP_CHANGE_BADMODE)
	{
		sprintf(buf, "The video mode %ldx%ldx%ld-bit color is not supported\n", w, h, bitdepth);
		MessageBox(Hwnd, buf, "sorry", MB_OK); //Log(LOG_BOOT, buf);
	} 
	else if(modeSwitch == DISP_CHANGE_FAILED)
	{
		sprintf(buf, "Hardware failed to change to %ldx%ldx%ld-bit color\n", w, h, bitdepth);
		MessageBox(Hwnd, buf, "sorry", MB_OK); //Log(LOG_BOOT, buf);
	} 
	else
	{
		sprintf(buf, "Failed to change to %ldx%ldx%ld-bit color - unknown reason\n", w, h, bitdepth);
		MessageBox(Hwnd, buf, "sorry", MB_OK); //Log(LOG_BOOT, buf);
	}
	
	
	return 1; 
}

void tagAPI_OpenGL::ResizeViewport(int width, int height) 
{
	float  znear = 0.7f; // Put the screen 1 unit in front of the "eye" 
	float  zfar  = 250.0f; // Note that the ratio far/near is to be minimized 
	float  aspect;
	
	// Set a viewport to the entire screen, its all one big 3D window now 
	glViewport(0, 0, width, height);
	
	// Calculate the screen (ie a neutral) aspect ratio 
	// ie squares are squares 
	
	aspect = (GLdouble)width/(GLdouble)height;
	
	// Set the Projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// gluPerspective sets field of view for Y, but since I want a known
	// (ie 90 degree) field of view in the x direction calc our own using
	// raw frustum parameters instead.
	
#define USE_FRUSTUM 1
#if (USE_FRUSTUM)
	{
		float  left;
		float  right;
		float  bottom;
		float  top;
		
		FLOAT   fovX	= 90.0f;
		
		right = znear * tan( fovX/2.0 * PI/180.0f );
		top = right / aspect;
		bottom = -top;
		left = -right;
		glFrustum(left, right, bottom, top, znear, zfar);
	}
#else
	// Dont use GLU it wont work if you dynamic load the OGL dll 
	gluPerspective(fovX/aspect, aspect, znear, zfar); 
#endif
	glMatrixMode(GL_MODELVIEW);
}
