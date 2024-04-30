#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <Afxwin.h>


#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <stdio.h>
#include <process.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"

#include "resource.h"



extern APP			App;
extern HINSTANCE	g_hInstance;
extern HWND			g_hWnd;

BOOL CALLBACK SettingsDlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam );

void tagSettings::SetDefaultSettings()
{
	RenderMode		= 1;	//Default 0 == DDraw, 1 == OpenGL
	BitDepth		= 16;	//Default 16 bit
	ScreenWidth		= 640;	//Default 640
	ScreenHeight	= 480;	//Default 480
	DetailLevel		= 0;	//Default 0
	bFullScreen		= 1;	//Default 1 == Fullscreen, 0 == Windowed 
	bParticles		= 1;
	bGLAntiAliasing	= 1;
	bWaitOnVSynch	= 1;	//Default Wait on VSynch
}


void tagSettings::Initialize()
{
	
	SetDefaultSettings();
	
	//Look for a settings file made previously
	FILE *fp;

	fp = fopen(SETTINGS_FILENAME,"rb");

	if(fp)
	{
		//We found a settings file! so read that bad boy in!
		fread(this,1,sizeof(SETTINGS),fp);
		//clean up
		fclose(fp);
	}
	

	//Get settings from the user
	if( DialogBox( g_hInstance, "settings", g_hWnd, SettingsDlgProc) < 0)
	{
		MessageBox(g_hWnd, "Could not load the settings dialog box!", "ERROR", MB_OK);
	}

/*	
	FILE *fp;
	if(!(fp = fopen("settings.txt","rb")))
	{
		//file does not exist so creat it with MFC window
		_spawnl(_P_WAIT,"settings.exe","settings.exe",NULL);
		//CreateProcess(NULL,"settings.exe",NULL,NULL,FALSE,0,NULL,".",NULL,NULL);
		//("settings.exe", "settings.exe",NULL);
		//system("settings.exe");
		
	
		//now open and read file
		if(!(fp = fopen("settings.txt","rb")))
		{
			//somthing is horrably wrong
			//PostQuitMessage(0);
			SetDefaultSettings();
		}
	}
	//now read-in
	fread(this,1,sizeof(*this),fp);
	fclose(fp);
*/

}

void CenterWindow( HWND hWindow );

BOOL CALLBACK SettingsDlgProc(	HWND	hDlg,
								UINT	iMsg,
								WPARAM	wParam,
								LPARAM	lParam)
{
	
	switch(iMsg)
	{
	case WM_PAINT:
		InvalidateRect( hDlg, NULL, true);
		break;

	case WM_INITDIALOG:
		{
			//*** Set the default settings ***
			//SetWindowText(hDlg, "Matrix Attack Settings");
			

			//Move our dialog box to the center of the screen
			CenterWindow( hDlg );
			

			//Windowed or Fullscreen
			if(App.UserSettings.bFullScreen)
			{
				SendDlgItemMessage(hDlg, IDC_FULLSCREEN, BM_SETCHECK, BST_CHECKED, NULL);
			}
			else
			{
				SendDlgItemMessage(hDlg, IDC_WINDOWED, BM_SETCHECK, BST_CHECKED, NULL);
			}

			//OpenGL or DirectDraw
			if(App.UserSettings.RenderMode)
			{
				SendDlgItemMessage(hDlg, IDC_OPENGL, BM_SETCHECK, BST_CHECKED, NULL);
			}
			else
			{
				SendDlgItemMessage(hDlg, IDC_DIRECTDRAW, BM_SETCHECK, BST_CHECKED, NULL);
			}

			return 1;// returning 1 tells windows to put the focus on this dialog
		}
		break;
		
		
		//************************************************************
	case WM_COMMAND:
		
		switch( LOWORD (wParam))
		{
			case IDC_FULLSCREEN:
			{
				App.UserSettings.bFullScreen = true;
				break;
			}
			
			case IDC_WINDOWED:
			{
				App.UserSettings.bFullScreen = false;
				break;
			}

			case IDC_OPENGL:
			{
				App.UserSettings.RenderMode = 1;	// 1 == OpenGL
				break;
			}
			
			case IDC_DIRECTDRAW:
			{
				App.UserSettings.RenderMode = 0;	// 0 == DirectDraw
				break;
			}
			
			case IDOK:
			{
				//Save our settings to a file for checking next time we run.
				FILE *fp;
				fp = fopen(SETTINGS_FILENAME, "wb");
				if(fp)
				{
					fwrite(&App.UserSettings,1,sizeof(SETTINGS),fp);
					fclose(fp);
				}
				else
				{
					MessageBox(hDlg, "Could not save settings to file!", "ERROR", MB_OK);
				}
				
				EndDialog(hDlg, 0);
				return 1;
			}
		}
		
		return 0;
		
		//************************************************************
	}
	return DefWindowProc(hDlg, iMsg, wParam, lParam);
}