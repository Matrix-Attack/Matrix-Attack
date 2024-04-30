#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <stdio.h>	//for sprintf
#include <gl/gl.h>
#include <gl/glu.h>


#include "./mmgr.h"

//#include "database.h"
#include "DPlay.h"
#include "app.h"
#include "gamemenu.h"
#include "dinput.h"


#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"
#include "dsound.h"



#include "GLSurfDef.h"
#include "font.h"


extern APP App;
extern GAME_DATA GameData;
extern DSound		Sound;

void GLSetup2DMode();
void GLSetup3DMode();


DWORD WINAPI tagAPI_OpenGL::RedrawScreen(LPVOID Parm)
{
	//we used to do this stuff when we didn't have globals
	//P_APP pApp = ( P_APP )Parm;
	//if(pApp == NULL) return 1;
	Vector3D *pCam = &(GameData.pBoard->GetCamera())->worldPos;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//***** UPDATE OUR MAIN BOARD *****
	
	if(GameData.GameStateFlags & GAME_STATE_GAME)
	{
		//
		//now create some clipping planes
		GLdouble	clip0[4]={-1.0	,0.0	,0.0	,-15.0	+ .023*pCam->z};
		GLdouble	clip1[4]={1.0	,0.0	,0.0	,15.0	+ .023*pCam->z};

/*		GLdouble	clip0[4]= {0.0	,-1.0	,0.0	,80.0	+ -.125*pCam->z},	//80/640
					clip1[4]= {0.0	,1.0	,0.0	,240.0	+ -.375*pCam->z},	//240/640
					clip2[4]= {1.0	,0.0	,0.0	,185.0	+ -.289*pCam->z},	//185/640
					clip3[4]= {-1.0	,0.0	,0.0	,-15.0	+ .023*pCam->z},	//-15/640
					clip4[4]= {1.0	,0.0	,0.0	,-30.0	},	//-30/640
					clip5[4]= {-1.0	,0.0	,0.0	,230.0	};	//230/640

		//clip planes
		glClipPlane(GL_CLIP_PLANE0,clip0);
		glClipPlane(GL_CLIP_PLANE1,clip1);
		glClipPlane(GL_CLIP_PLANE2,clip2);
		glClipPlane(GL_CLIP_PLANE3,clip3);
		glClipPlane(GL_CLIP_PLANE4,clip4);
		glClipPlane(GL_CLIP_PLANE5,clip5);
*/
		glClipPlane(GL_CLIP_PLANE0,clip0);
		glClipPlane(GL_CLIP_PLANE1,clip1);
		//*** Update GAME BOARD
		//static float CamX=0,CamY=-80,CamZ=-640;//y was -80 z was -640

		glTranslated (pCam->x, pCam->y-80, pCam->z-640); // camera eye position
		//glViewport (0, 0, 640, 640);
		GameData.pBoard->Draw_OpenGL();

		GLSetup2DMode();
		glViewport (0, 0, 640, 480);
		
		//Update our background for the game
		GameData.pGameMenu->DrawBackground_OpenGL_InGame();
		
		//*** CHECK FOR BUGS *** -eli
		//GameData.pGameMenu->UpdateGameButtons();
		GameData.pGameMenu->Draw_GameMode();
		
		GLSetup3DMode();
		
		//*** UPDATE OUR GL_PARTICLE ENGINE ******
		
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR );
		
		glDrawParticleEngine();

		//*** UPDATE OUR TEXT ENGINE **********
		glDisable(GL_ALPHA_TEST);
		UpdateTextObjEngine();
	
	}
	else if(GameData.GameStateFlags & GAME_STATE_MENU)
	{
		GLSetup2DMode();
		
		//*** FIX CHECK FOR BUGS *** -eli
		//GameData.pGameMenu->Update();
		GameData.pGameMenu->Draw_MenuMode();
		
		//GLSetup3DMode(); // I now set this when we leave the menu
	}
	//*********END OF 2D OPENGL CODE******************


	//***********///DRAW OUR OPENGL FRAME RATE\\\***********

	if(App.bDebug)
	{

		
		glDisable(GL_LIGHTING);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR );
		
		glDrawParticleEngine();

		//*** UPDATE OUR TEXT ENGINE **********
		glDisable(GL_ALPHA_TEST);
		UpdateTextObjEngine();

		char buff[512];	
		sprintf( buff, "FPS: %0.2f %f", 
						GameData.Timer.m_fFPS,
						GameData.m_fDeltaTime);
		
		pFont->DrawStaticString( 280, 480, buff);

/*		if(GameData.bCamera)
			pFont->DrawStaticString( 300, 450, "CAMERA:ON");
		else
			pFont->DrawStaticString( 300, 450, "CAMERA:OFF");
*/
	}
	//***********\\\DRAW OUR OPENGL FRAME RATE///************
	
	
	//Swap the buffers
	glFinish();
	SwapBuffers( hDC );

	return 0;
}

void GLSetup2DMode()
{
	//*********SETUP OPENGL FOR 2D *************
	//*** save the opengl setup 
	SetOpenGLModes (FONT_GET_MODES);
	//*** Set up openGL so that we can draw text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	//glOrtho(0, 640, 0, 640, -1, 1);
	glOrtho(0, 640, 480, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_ALPHA_TEST);
	//if fragment's alpha is not equal to 1.0 don't blt
	glAlphaFunc(GL_NOTEQUAL, 0.0f);
	//*****************************************************

/*	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	glDisable(GL_CLIP_PLANE5);
*/
}

void GLSetup3DMode()
{
	//**********PUT OPENGL BACK TO WHAT IT WAS **************
	//get rid of our openGL settings
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();
	//*** save the opengl setup 
	SetOpenGLModes (FONT_RESTORE_MODES);

	glDisable(GL_ALPHA_TEST);
	//*******************************************************
}