#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <gl/gl.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "Bitmap.h"
#include "dinput.h"
#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "camera3d.h"
#include "player.h"
#include "board.h"

#include "text.h"
#include "GLSurfDef.h"


#define READ_FILE_STRING(pFile, string)	(fscanf((pFile), "\n%[^\n]", (string)))


void GetRandomBackground(void);

void LoopUntilString(const char *string, FILE *pFile);
void SwitchToGameFromMenu();

extern APP			App;
extern GAME_DATA	GameData;


// Constructor for the menu button class
Menu_Button::Menu_Button()
{
		buttonFlags		= 0;	
		buttonID		= 0;
		strcpy(textString, "Uninitialized");
		character		= 0;		
		textOffset.x	= 0;
		textOffset.y	= 0;
		screenPos.x		= 0;
		screenPos.y		= 0;
		enabledPos.x	= 0;
		enabledPos.y	= 0;
		disabledPos.x	= 0;
		disabledPos.y	= 0;
		cursorPos.x		= 0;
		cursorPos.y		= 0;
		screenX			= 0.0;
		screenY			= 0.0;
		velocX			= 0.0;
		velocY			= 0.0;
		enableDelay		= 0.0;
		disableDelay	= 0.0;
		SetRect(&clipRect, 0, 0, 0, 0);		
		SetRect(&clipSelect, 0, 0, 0 ,0);	
		SetRect(&clipDeselect, 0, 0, 0, 0);
		pSelect			= NULL;		
		pDeselect		= NULL;		
		pUpdate			= NULL;	
		pLaunch			= NULL;	
		pNeighbor[0]	= NULL;
		pNeighbor[1]	= NULL;
		pNeighbor[2]	= NULL;
		pNeighbor[3]	= NULL;
		pNeighbor[4]	= NULL;
		surfIndex		= 0;

		fTimeWaiting	= 0.0f;
		bUpdate			= false;
}

// Constructor for the menu corsor class
Menu_Cursor::Menu_Cursor()
{
	blinkTimer = 0;
	blinkSpeed = 0;

	screenPos.x = 0;
	screenPos.y = 0;				
	blinkPos.x  = 0;
	blinkPos.y  = 0;				

	screenX = 0.0;		
	screenY = 0.0;
	velocX  = 0.0;			
	velocY  = 0.0;

	SetRect(&clipRect, 0, 0, 0, 0);

	pButton = NULL;		
	cursorFlags = 0;
}

// Constructor for the game menu class
Game_Menu::Game_Menu()
{
	iBackgroundTexID= GL_SURF_MENU_BACKGROUND1_TEXID;

	activeSurfaces	= 0;
	menuID			= 0;
	iNumButtons		= 0;
	camPos.x		= 0;
	camPos.y		= 0;
	camScreenX		= 0.0;	
	camScreenY		= 0.0;
	camVelocX		= 0.0;	
	camVelocY		= 0.0;
	camAngle		= 0.0;	
	pBackSurf		= NULL;
	buttonArray		= NULL;
	TempCharIcon	= 0;
	strcpy(TempName, "Uninitialized");
}


Game_Menu::~Game_Menu()
{
	if(buttonArray)
	{
		delete[] buttonArray;
		buttonArray = NULL;
	}
}

void Game_Menu::DrawMenuButtons()
{
	
	// loop through all the buttons
	for(int i = 0; i < iNumButtons; i++)
	{
		Menu_Button	*pButton	= buttonArray + i;
		POINT	ButtonPos		= pButton->GetScreenPos();

		if(pButton->pUpdate && pButton->IsEnabled())
			pButton->pUpdate(this, pButton, CURSOR_MOVE_NOWHERE);
			
		// display the button
		if(!App.UserSettings.RenderMode)
		{	//**** DDRAW MODE ****
			
			pButton->ClipAgainstScreenEdges();
			
			if( App.pDDraw->surfaceArray[pButton->surfIndex] && pButton->clipRect.bottom != 0)
			{
				App.pDDraw->getBackBuffer()->BltFast(pButton->screenPos.x, pButton->screenPos.y,
					App.pDDraw->surfaceArray[pButton->surfIndex], &pButton->clipRect, DDBLTFAST_SRCCOLORKEY);
			}

			if(ButtonPos.x > 0 && ButtonPos.x < 640 && 
			   ButtonPos.y > 0 && ButtonPos.y < 480	)
			{
				//***Draw TEXT (if the button has any)
				if((pButton->buttonFlags & BUTTON_FLAG_DRAW_TEXT1))
				{
					//Large Text
					App.pDDraw->DDrawText(pButton->screenPos.x + pButton->textOffset.x, 
							pButton->screenPos.y + pButton->textOffset.y, TF_FONT_LARGE, 
							pButton->GetTextString());
				}
				else if((pButton->buttonFlags & BUTTON_FLAG_DRAW_TEXT2) )
				{
					
					//Small Text
					App.pDDraw->DDrawText(pButton->screenPos.x + pButton->textOffset.x, 
							pButton->screenPos.y + pButton->textOffset.y, TF_FONT_SMALL,
							pButton->GetTextString());
				}
			}
		
		}
		else	//**** OPENGL MODE ****
		{	
			/*
			if(ButtonPos.x > -1 && ButtonPos.x < 640 && 
			   ButtonPos.y > -1 && ButtonPos.y < 480	)
			{

			int iScaleX = abs(pButton->enabledPos.x - pButton->screenPos.x);
			int iScaleY = abs(pButton->enabledPos.y - pButton->screenPos.y);

			BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, iScaleX+iScaleY, iScaleX+iScaleY, &pButton->clipRect,
				256, 256, pButton->surfIndex );
			}
			else
				BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, &pButton->clipRect,
					256, 256, pButton->surfIndex );
			*/

			BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, &pButton->clipRect,
				256, 256, pButton->surfIndex );

			if(ButtonPos.x > 0 && ButtonPos.x < 640 && 
			   ButtonPos.y > 0 && ButtonPos.y < 480	)
			{
				//***Draw TEXT (if the button has any)
				if((pButton->buttonFlags & BUTTON_FLAG_DRAW_TEXT1) )
				{
					//Large Text
					App.pOpenGL->GLDrawText(pButton->screenPos.x + pButton->textOffset.x,
							pButton->screenPos.y + pButton->textOffset.y, TF_FONT_LARGE, 
							pButton->GetTextString() );
				}
				else if((pButton->buttonFlags & BUTTON_FLAG_DRAW_TEXT2) )
				{
					//Small Text
					App.pOpenGL->GLDrawText(pButton->screenPos.x + pButton->textOffset.x,
								pButton->screenPos.y + pButton->textOffset.y, TF_FONT_SMALL, 
								pButton->GetTextString() );
				}
			}
			//}

		}//end openGL else
	}//end for

	if(GameData.GameStateFlags & GAME_STATE_SWITCHING)
		SwitchToGameFromMenu();
}

void Game_Menu::DisplayCursor_DDraw()
{
	// display the cursor
	LPDD_SURF	pCursorSurf		= App.pDDraw->surfaceArray[3];
	
	App.pDDraw->getBackBuffer()->BltFast(cursor.screenPos.x, cursor.screenPos.y, pCursorSurf, NULL, DDBLTFAST_SRCCOLORKEY);
	
	if(cursor.IsBlinking())
	{
		if(--cursor.blinkTimer < 0)
		{
			cursor.cursorFlags	^= CURSOR_FLAG_FLASH_ON;
			cursor.blinkTimer	 = cursor.blinkSpeed;
		}
		if(cursor.cursorFlags & CURSOR_FLAG_FLASH_ON)
		{
			App.pDDraw->getBackBuffer()->BltFast(cursor.blinkPos.x, cursor.blinkPos.y, 
				pCursorSurf, NULL, DDBLTFAST_SRCCOLORKEY);
		}//end if cursorFlags 
	}//end if cursor.IsBlinking()
}

void Game_Menu::DisplayCursor_OpenGL()
{
	// display the cursor
	RECT rcSRC = {0, 0, 32, 32};
	
	BltGLStyle(cursor.screenPos.x, cursor.screenPos.y,
		&rcSRC, 32, 32, GL_SURF_MENU_CURSOR_TEXID);
	
	if(cursor.IsBlinking())
	{
		if(--cursor.blinkTimer < 0)
		{
			
			cursor.cursorFlags	^= CURSOR_FLAG_FLASH_ON;
			cursor.blinkTimer	 = cursor.blinkSpeed;
			
		}
		if(cursor.cursorFlags & CURSOR_FLAG_FLASH_ON)
		{
			
			BltGLStyle(cursor.blinkPos.x, cursor.blinkPos.y,
				&rcSRC, 32, 32, GL_SURF_MENU_CURSOR_TEXID);
			
		}//end if cursorFlags 
		
	}//end if cursor.IsBlinking()
}


void Game_Menu::DrawBackground_DDraw()
{
	RECT	clipRect1	= {0, 0, 32, 32},	// top row
		clipRect2	= {0, 0, 32, 32},	// bottom row
		clipRect3	= {0, 0, 32, 32},	// left column
		clipRect4	= {0, 0, 32, 32};	// right column
	
	LPDD_SURF	pBackground;
	
	
/*	if(GameData.GameStateFlags & GAME_STATE_MENU)
	{
		switch( iBackgroundTexID )
		{
			default:
			case GL_SURF_MENU_BACKGROUND1_TEXID: pBackground = App.pDDraw->surfaceArray[1]; break;
			case GL_SURF_MENU_BACKGROUND2_TEXID: pBackground = App.pDDraw->surfaceArray[9]; break;
			case GL_SURF_MENU_BACKGROUND3_TEXID: pBackground = App.pDDraw->surfaceArray[10]; break;
			case GL_SURF_MENU_BACKGROUND4_TEXID: pBackground = App.pDDraw->surfaceArray[11]; break;
		}
	}
	else
	{
*/		pBackground = App.pDDraw->surfaceArray[1];
//	}

	int		x, y;
	
	if(camPos.y)
	{
		clipRect1.top		= camPos.y;
		clipRect2.bottom	= camPos.y;
		for(x = 1; x < 20; x++)
		{
			App.pDDraw->getBackBuffer()->BltFast(x * 32 - camPos.x, 0, pBackground, &clipRect1, DDBLTFAST_WAIT);
			App.pDDraw->getBackBuffer()->BltFast(x * 32 - camPos.x, 480 - camPos.y, pBackground, &clipRect2, DDBLTFAST_WAIT);
		}
	}
	else
	{
		for(x = 1; x < 20; x++)
		{
			App.pDDraw->getBackBuffer()->BltFast(x * 32 - camPos.x, 0, pBackground, NULL, DDBLTFAST_WAIT);
			App.pDDraw->getBackBuffer()->BltFast(x * 32 - camPos.x, 448, pBackground, NULL, DDBLTFAST_WAIT);
		}
	}
	
	if(camPos.x)
	{
		clipRect3.left		= camPos.x;
		clipRect4.right		= camPos.x;
		for(y = 1; y < 15; y++)
		{
			App.pDDraw->getBackBuffer()->BltFast(0, y * 32 - camPos.y, pBackground, &clipRect3, DDBLTFAST_WAIT);
			App.pDDraw->getBackBuffer()->BltFast(640 - camPos.x, y * 32 - camPos.y, pBackground, &clipRect4, DDBLTFAST_WAIT);
		}
	}
	else
	{
		for(y = 1; y < 15; y++)
		{
			App.pDDraw->getBackBuffer()->BltFast(0, y * 32 - camPos.y, pBackground, NULL, DDBLTFAST_WAIT);
			App.pDDraw->getBackBuffer()->BltFast(608, y * 32 - camPos.y, pBackground, NULL, DDBLTFAST_WAIT);
		}
	}
	
	// fill in the corners
	clipRect1.left		= clipRect3.left;
	clipRect2.left		= clipRect3.left;
	
	clipRect3.top		= clipRect1.top;
	clipRect3.left		= 0;
	clipRect3.right		= clipRect4.right;
	clipRect4.bottom	= clipRect2.bottom;
	
	// upper left corner
	App.pDDraw->getBackBuffer()->BltFast(0, 0, pBackground, &clipRect1, DDBLTFAST_WAIT);
	
	// lower left corner
	App.pDDraw->getBackBuffer()->BltFast(0, 480 - camPos.y, pBackground, &clipRect2, DDBLTFAST_WAIT);
	
	// upper right corner
	App.pDDraw->getBackBuffer()->BltFast(640 - camPos.x, 0, pBackground, &clipRect3, DDBLTFAST_WAIT);
	
	// lower right corner
	App.pDDraw->getBackBuffer()->BltFast(640 - camPos.x, 480 - camPos.y, pBackground, &clipRect4, DDBLTFAST_WAIT);
	
	// fill in the body
	for(y = 1; y < 15; y++)
	{
		for(x = 1; x < 20; x++)
		{
			App.pDDraw->getBackBuffer()->BltFast(x * 32 - camPos.x, y * 32 - camPos.y, pBackground, NULL, DDBLTFAST_WAIT);
		}
	}
}

void BltGLSpeedStyle(RECT *prcSRC, RECT *prcDST )
{
	glBegin(GL_QUADS);
		
		glTexCoord2f	( 0.0f, 1.0f );
		glVertex2i		( prcDST->left, prcDST->top);
		
		glTexCoord2f	( 0.0f, 0.0f );
		glVertex2i		( prcDST->left, prcDST->bottom);
		
		glTexCoord2f	( 1.0f, 0.0f );
		glVertex2i		( prcDST->right, prcDST->bottom);
		
		glTexCoord2f	( 1.0f, 1.0f );
		glVertex2i		( prcDST->right, prcDST->top);
		
	glEnd();
}

void Game_Menu::DrawBackground_OpenGL( )
{
	RECT	rcDST, rcSRC;

	//Bind the texture once!
	glBindTexture(GL_TEXTURE_2D, iBackgroundTexID );
	
	SetRect(&rcSRC, 0, 0, 256, 256);
	
	rcDST.left		= camPos.x;
	rcDST.top		= camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);

	rcDST.left		= 256 + camPos.x;
	rcDST.top		= camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);

	rcDST.left		= camPos.x;
	rcDST.top		= 256 + camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);

	rcDST.left		= 256 + camPos.x;
	rcDST.top		= 256 + camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);

	//now to blt the far right background  (the .875 thats left)
	rcDST.left		= 512 + camPos.x;
	rcDST.top		= camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);

	rcDST.left		= 512 + camPos.x;
	rcDST.top		= 256 + camPos.y;
	rcDST.right		= rcDST.left + 256;
	rcDST.bottom	= rcDST.top  + 256;
	BltGLSpeedStyle(&rcSRC, &rcDST);
}

void Game_Menu::DrawBackground_OpenGL_InGame()
{

	RECT	rcDST, rcSRC;

	SetRect(&rcSRC, 121, 0, 256, 80);
	SetRect(&rcDST, 0, 0, 135, 80);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 121, 0, 256, 256);
	SetRect(&rcDST, 0, 80, 135, 356);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 121, 212, 256, 256);
	SetRect(&rcDST, 0, 356, 135, 400);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 121, 132, 256, 212);
	SetRect(&rcDST, 0, 400, 135, 480);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);
// middle- top and bot
	SetRect(&rcSRC, 0, 0, 256, 80);
	SetRect(&rcDST, 135, 0, 391, 80);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 0, 132, 256, 212);
	SetRect(&rcDST, 135, 400, 391, 480);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);
// right side- top and bot
	SetRect(&rcSRC, 0, 0, 256, 80);
	SetRect(&rcDST, 391, 0, 640, 80);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 0, 132, 256, 212);
	SetRect(&rcDST, 391, 400, 640, 480);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);
// middle-middle
	SetRect(&rcSRC, 170, 0, 220, 256);
	SetRect(&rcDST, 305, 80, 355, 356);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 170, 212, 220, 256);
	SetRect(&rcDST, 305, 356, 355, 400);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);
// right side-middle
	SetRect(&rcSRC, 139, 0, 256, 256);
	SetRect(&rcDST, 525, 80, 640, 356);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	SetRect(&rcSRC, 139, 212, 256, 256);
	SetRect(&rcDST, 525, 356, 640, 400);
	BltGLStyle(&rcSRC, &rcDST, 256, 256, iBackgroundTexID);

	//
	//DRAW PURPLE BOARDERS HERE!!!!!!!!
	//GAME_SURF_100.bmp
	SetRect(&rcSRC, 0, 0, 170, 160);
	SetRect(&rcDST, 131, 237, 309, 405);//bottom part of left side
	BltGLStyle(&rcSRC, &rcDST, 256, 256, GL_SURF_GAME_BOARDER_TEXID);

	SetRect(&rcSRC, 0, 94, 170, 254);
	SetRect(&rcDST, 131, 76, 309, 245);//top part of left side
	BltGLStyle(&rcSRC, &rcDST, 256, 256, GL_SURF_GAME_BOARDER_TEXID);

	SetRect(&rcSRC, 0, 0, 170, 160);
	SetRect(&rcDST, 351, 237, 529, 405);//bottom part of right side
	BltGLStyle(&rcSRC, &rcDST, 256, 256, GL_SURF_GAME_BOARDER_TEXID);

	SetRect(&rcSRC, 0, 94, 170, 254);
	SetRect(&rcDST, 351, 76, 529, 245);//top part of right side
	BltGLStyle(&rcSRC, &rcDST, 256, 256, GL_SURF_GAME_BOARDER_TEXID);

/*
	RECT	rcDST, rcSRC;
	
	glBindTexture(GL_TEXTURE_2D, iBackgroundTexID );

	SetRect(&rcSRC, 121, 0, 256, 80);
	SetRect(&rcDST, 0, 0, 135, 80);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 121, 0, 256, 256);
	SetRect(&rcDST, 0, 80, 135, 356);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 121, 212, 256, 256);
	SetRect(&rcDST, 0, 356, 135, 400);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 121, 132, 256, 212);
	SetRect(&rcDST, 0, 400, 135, 480);
	BltGLSpeedStyle(&rcSRC, &rcDST);

// middle- top and bot
	SetRect(&rcSRC, 0, 0, 256, 80);
	SetRect(&rcDST, 135, 0, 391, 80);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 0, 132, 256, 212);
	SetRect(&rcDST, 135, 400, 391, 480);
	BltGLSpeedStyle(&rcSRC, &rcDST);
// right side- top and bot
	SetRect(&rcSRC, 0, 0, 256, 80);
	SetRect(&rcDST, 391, 0, 640, 80);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 0, 132, 256, 212);
	SetRect(&rcDST, 391, 400, 640, 480);
	BltGLSpeedStyle(&rcSRC, &rcDST);
// middle-middle
	SetRect(&rcSRC, 170, 0, 220, 256);
	SetRect(&rcDST, 305, 80, 355, 356);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 170, 212, 220, 256);
	SetRect(&rcDST, 305, 356, 355, 400);
	BltGLSpeedStyle(&rcSRC, &rcDST);
// right side-middle
	SetRect(&rcSRC, 139, 0, 256, 256);
	SetRect(&rcDST, 525, 80, 640, 356);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 139, 212, 256, 256);
	SetRect(&rcDST, 525, 356, 640, 400);
	BltGLSpeedStyle(&rcSRC, &rcDST);

	//
	//DRAW PURPLE BOARDERS HERE!!!!!!!!
	//GAME_SURF_100.bmp
	glBindTexture(GL_TEXTURE_2D, GL_SURF_GAME_BOARDER_TEXID );

	SetRect(&rcSRC, 0, 0, 170, 160);
	SetRect(&rcDST, 131, 237, 309, 405);//bottom part of left side
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 0, 94, 170, 254);
	SetRect(&rcDST, 131, 76, 309, 245);//top part of left side
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 0, 0, 170, 160);
	SetRect(&rcDST, 351, 237, 529, 405);//bottom part of right side
	BltGLSpeedStyle(&rcSRC, &rcDST);

	SetRect(&rcSRC, 0, 94, 170, 254);
	SetRect(&rcDST, 351, 76, 529, 245);//top part of right side
	BltGLSpeedStyle(&rcSRC, &rcDST);
*/
}

void Game_Menu::HandleKeyInput()
{
	Menu_Button	**ppButton	= &cursor.pButton;
	
	int			direction	= CURSOR_MOVE_NOWHERE;
	int			ButtonPress	= 0;

//#ifdef DINPUT
	
	// determine what direction the cursor is moving (if any)

	direction	=	((App.pdistruct->keys[DIK_W] & 0x80) || (App.pdistruct->keys[DIK_UP] & 0x80) )		?	CURSOR_MOVE_UP		:
					((App.pdistruct->keys[DIK_S] & 0x80) || (App.pdistruct->keys[DIK_DOWN] & 0x80)	)	?	CURSOR_MOVE_DOWN	:
					((App.pdistruct->keys[DIK_A] & 0x80) || (App.pdistruct->keys[DIK_LEFT] & 0x80)	)	?	CURSOR_MOVE_LEFT	:
					((App.pdistruct->keys[DIK_D] & 0x80) || (App.pdistruct->keys[DIK_RIGHT] & 0x80)	)	?	CURSOR_MOVE_RIGHT	:
																CURSOR_MOVE_NOWHERE;


//	ButtonPress = (App.pdistruct->keys[DIK_NUMPADENTER] & 0x80 ||
//				   App.pdistruct->keys[DIK_RETURN] & 0x80  );

	ButtonPress = ( GameData.keysDown[VK_RETURN] || GameData.keysDown[VK_CONTROL] );

//	GameData.keysDown[VK_RETURN] = 0;
	
	//Check to see if we have the joystick, (if so lets get the input)
/*
	if(App.pdistruct->JoyDescHead.iNumDevices > 0)
	{
		//***Test joystick Up,Down,Left,Right
		if(direction == CURSOR_MOVE_NOWHERE)
			direction = (App.pdistruct->joystick.lY < 0)	?	CURSOR_MOVE_UP		:
		(App.pdistruct->joystick.lY > 0)	?	CURSOR_MOVE_DOWN	:
		(App.pdistruct->joystick.lX < 0)	?	CURSOR_MOVE_LEFT	:
		(App.pdistruct->joystick.lX > 0)	?	CURSOR_MOVE_RIGHT	:
		CURSOR_MOVE_NOWHERE;
		if(!ButtonPress)
			ButtonPress =	(App.pdistruct->joystick.rgbButtons[0] & 0x80);
	}	/**/
	
	// determine if we can move the cursor
	if(cursor.pButton && (direction == CURSOR_MOVE_NOWHERE))
	{
		
		cursor.cursorFlags	|= CURSOR_FLAG_CAN_MOVE;
		
		// since we're not moving the cursor, let's see if we're
		// trying to launch a button
		if(	ButtonPress &&
			(cursor.cursorFlags & CURSOR_FLAG_CAN_SELECT) &&
			((*ppButton)->pLaunch) )
		{
			(*ppButton)->pLaunch(this, (*ppButton), CURSOR_MOVE_NOWHERE);
			cursor.cursorFlags	&= ~CURSOR_FLAG_CAN_SELECT;
			return;
		}
		else if(!ButtonPress)
			cursor.cursorFlags	|= CURSOR_FLAG_CAN_SELECT;
	}
	
	// if we can, move it to the next button
	else if(cursor.pButton && 	(cursor.cursorFlags & CURSOR_FLAG_CAN_MOVE) &&
		((*ppButton)->pNeighbor[direction])	)
	{	
	
		if((*ppButton)->pNeighbor[direction]->IsEnabled())
		{
			(*ppButton)->Deselect(this, (*ppButton), direction);
			(*ppButton)	= (*ppButton)->pNeighbor[direction];
			(*ppButton)->Select(this, (*ppButton), direction);
			
			// set the cursor in motion, and,
			// disable cursor movement until all keys are released
			cursor.cursorFlags	|= CURSOR_FLAG_MOVING;
			cursor.cursorFlags	&= ~CURSOR_FLAG_CAN_MOVE;
		}
	}
	
//#endif
	
}


void Game_Menu::Initialize(int whichMenu)
{
	cursor.cursorFlags		= 0;
	
	menuID					= whichMenu;
	
	activeSurfaces			= 0x1FF;	// surface 0-8 (all 9 surfaces)
	
	if(GameData.GameStateFlags & GAME_STATE_MENU)
	{
		iNumButtons			= NUM_OF_MENU_BUTTONS;
	
		//Load surfaces will load the correct textures for DDraw or OpenGL.
		LoadSurfaces();
	}
	else if(GameData.GameStateFlags & GAME_STATE_GAME)
	{
		iNumButtons			= NUM_OF_GAME_BUTTONS;
	}
	
	// allocate memory for the menu's buttons
	if(buttonArray)
	{
		delete[] buttonArray;
	}
	buttonArray				= new Menu_Button[iNumButtons];
	
	// initialize some other stuff
	camPos.x				=  0;
	camPos.y				=  0;
	camVelocX				= DEFAULT_CAM_VELOCITY_X;
	camVelocY				= DEFAULT_CAM_VELOCITY_Y;
	camAngle				=  0;
	
	InitializeButtons();

	SwitchMenu(menuID);
	

	//Copy our version information into the right button (gay code at its best)
	char strVersion[64];
	sprintf(strVersion, "VERSION: %s", STR_VERSION);
	buttonArray[79].SetTextString(strVersion);
}

void Game_Menu::SwitchMenu(int whichMenu)
{
	menuID				= whichMenu;
	cursor.cursorFlags	|= CURSOR_FLAG_MOVING;
	cursor.cursorFlags	&= ~CURSOR_FLAG_CAN_MOVE;

	switch(whichMenu)
	{
	case MENU_ID_TITLESCREEN:
		buttonArray[27].disabledPos.x	= 640;
		buttonArray[27].disabledPos.y	= 250;
		
		// This is the multiplayer button
		buttonArray[28].disabledPos.x	= 640;
		buttonArray[28].disabledPos.y	= 314;
		
		
		
		buttonArray[27].Enable(5);
//		buttonArray[28].Enable(10);	// Multi player button
		buttonArray[78].Enable(10);

		buttonArray[79].Enable(60);	// Version button

		buttonArray[29].Enable(20);
		
	

		//Enable our TitleScreen
		buttonArray[76].Enable(0);
		buttonArray[77].Enable(0);

		InitializeCursor(27);
		//iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;

		break;
	
	case MENU_ID_CREDITS_SCREEN:

		buttonArray[78].enabledPos.x	= 0;
		buttonArray[78].enabledPos.y	= 0;

		buttonArray[76].Disable(2);
		buttonArray[77].Disable(2);
		
		buttonArray[79].Disable(10);	// Version button
		
		InitializeCursor(78);
		
		GetRandomBackground();
		
		break;

	case MENU_ID_SINGLE_PLAYER_LOGIN:
		buttonArray[27].disabledPos.x	= 0;
		buttonArray[27].disabledPos.y	= 0;

		buttonArray[76].Disable(2);
		buttonArray[77].Disable(2);
		buttonArray[78].Disable(2);
		buttonArray[79].Disable(2);

		buttonArray[0].Enable(5);
		buttonArray[1].Enable(10);
		buttonArray[2].Enable(15);
//		buttonArray[3].Enable(20);
		buttonArray[4].Enable(20);
		
		cursor.pButton	= buttonArray + 0; //target pos
		cursor.pButton->Select(this, buttonArray + 0, 0);
		break;
		
	case MENU_ID_MULTI_IP_LOGIN:

		// This is the multi player button
		buttonArray[28].disabledPos.x	= 0;
		buttonArray[28].disabledPos.y	= 0;

		buttonArray[76].Disable(5);
		buttonArray[77].Disable(5);	
		buttonArray[78].Disable(5);
		buttonArray[79].Disable(5);

		buttonArray[42].Enable(10);
		buttonArray[34].Enable(10);
		
		cursor.pButton	= buttonArray + 42; //target pos
		break;
		
	case MENU_ID_MULTI_PLAYER_LOGIN:
		
		buttonArray[0].Enable(5);
		buttonArray[1].Enable(10);
		buttonArray[2].Enable(15);
//		buttonArray[3].Enable(15);	// Key config button
		buttonArray[4].Enable(20);
		
		cursor.pButton	= buttonArray + 0; //target pos
		cursor.pButton->Select(this, buttonArray + 0, 0);
		break;
		
/*	case MENU_ID_SINGLE_CHAR_SELECT:
		buttonArray[44].Enable(5);
		buttonArray[45].Enable(10);
		buttonArray[46].Enable(15);
		buttonArray[47].Enable(20);

		cursor.pButton	= buttonArray + 44; //target pos
		cursor.pButton->Select(this, buttonArray + 44, 0);
		break;
*/		
	case MENU_ID_SINGLE_GAME_SELECT:
		
		buttonArray[30].Enable(5);
		buttonArray[31].Enable(10);
		buttonArray[32].Enable(15);
		buttonArray[33].Enable(20);
		
		buttonArray[75].Disable(20);

		cursor.pButton	= buttonArray + 30; //target pos
		
		iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
		break;
	
	case MENU_ID_SINGLE_PUZZLE_SCREEN:
		buttonArray[71].Enable(5);
		buttonArray[72].Enable(10);
		buttonArray[73].Enable(15);

		cursor.pButton	= buttonArray + 71; //target pos
		
		iBackgroundTexID = GL_SURF_MENU_BACKGROUND3_TEXID;
		break;

	case MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY:
		buttonArray[74].Enable(0);

		cursor.pButton	= buttonArray + 74; //target pos
		break;

	case MENU_ID_MULTI_CHAT_SCREEN:
		
		buttonArray[35].Enable(0);
		buttonArray[36].Enable(5);
		buttonArray[37].Enable(10);
		buttonArray[38].Enable(15);
		buttonArray[39].Enable(20);
		buttonArray[40].Enable(25);
		buttonArray[41].Enable(30);
		buttonArray[50].Enable(35);

		buttonArray[62].Disable(15);
		buttonArray[63].Disable(15);
		buttonArray[64].Disable(0);
		buttonArray[65].Disable(5);
		buttonArray[66].Disable(10);



		cursor.pButton	= buttonArray + 41; //target pos
		break;

	case MENU_ID_MULTI_GAME_LAUNCH:

		//going into the launch game screen, enable all the buttons
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(5);
		buttonArray[37].Disable(10);
		buttonArray[38].Disable(15);
		buttonArray[39].Disable(20);
		buttonArray[40].Disable(20);
		buttonArray[41].Disable(25);
		buttonArray[50].Disable(25);

		//enable launch screen
		cursor.pButton	= buttonArray + 54; //target pos
		buttonArray[53].Enable(0);
		buttonArray[54].Enable(5);
		buttonArray[55].Enable(0);
		buttonArray[56].Enable(0);
		buttonArray[57].Enable(0);
		buttonArray[58].Enable(0);
		buttonArray[59].Enable(0);
		break;

	case MENU_ID_CREATE_CHANNEL:
		//disable chat
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(5);
		buttonArray[37].Disable(10);
		buttonArray[38].Disable(15);
		buttonArray[39].Disable(20);
		buttonArray[40].Disable(20);
		buttonArray[41].Disable(25);
		buttonArray[50].Disable(25);

		cursor.pButton	= buttonArray + 48; //target pos
		buttonArray[61].Enable(0);
		buttonArray[60].Enable(5);
		buttonArray[48].Enable(0);
		break;

	case MENU_ID_SERVER_SCORES:
		//disable chat
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(5);
		buttonArray[37].Disable(10);
		buttonArray[38].Disable(15);
		buttonArray[39].Disable(20);
		buttonArray[40].Disable(20);
		buttonArray[41].Disable(25);
		buttonArray[50].Disable(25);

		cursor.pButton	= buttonArray + 62; //target pos
		buttonArray[62].Enable(0);
		break;

	case MENU_ID_PLAYER_SCORES:
		//disable chat
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(5);
		buttonArray[37].Disable(10);
		buttonArray[38].Disable(15);
		buttonArray[39].Disable(20);
		buttonArray[40].Disable(20);
		buttonArray[41].Disable(25);
		buttonArray[50].Disable(25);

		cursor.pButton	= buttonArray + 63; //target pos
		buttonArray[63].Enable(0);
		break;

	case MENU_ID_CHANNEL_LIST:
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(5);
		buttonArray[37].Disable(10);
		buttonArray[38].Disable(15);
		buttonArray[39].Disable(20);
		buttonArray[40].Disable(20);
		buttonArray[41].Disable(25);
		buttonArray[50].Disable(25);

		cursor.pButton	= buttonArray + 65; //target pos
		buttonArray[64].Enable(0);
		buttonArray[65].Enable(5);
		buttonArray[66].Enable(10);
		break;

	case MENU_ID_PLAYER_SCORES_SINGLE_PLAYER:
		buttonArray[30].Disable(0);
		buttonArray[31].Disable(0);
		buttonArray[32].Disable(0);
		buttonArray[33].Disable(0);
		cursor.pButton	= buttonArray + 75; //target pos

		buttonArray[75].Enable(0);
		iBackgroundTexID = GL_SURF_MENU_BACKGROUND2_TEXID;
		break;

	} //end switch
}


void Game_Menu::InitializeButtons()
{
	FILE			*pFile =  NULL;
	
	ButtonAction	ButtonActionArray[63]; //the maximum amount of button actions should be here
	
	//***Player Login Screen
	ButtonActionArray[0]	= SelectLoadCharacterButton;
	ButtonActionArray[1]	= DeselectLoadCharacterButton;
	ButtonActionArray[2]	= SelectKillCharacterButton;
	ButtonActionArray[3]	= DeselectKillCharacterButton;
	ButtonActionArray[4]	= DeselectLoadCharacterIcon;
	ButtonActionArray[5]	= UpdateLoadCharacterIcon;
	ButtonActionArray[6]	= SelectKeyConfigButton;
	ButtonActionArray[7]	= DeselectKeyConfigButton;
	ButtonActionArray[8]	= SelectGamePadIcon;
	ButtonActionArray[9]	= DeselectGamePadButton;
	ButtonActionArray[10]	= UpdateLightBeamButton;
	ButtonActionArray[11]	= LaunchNewPlayerButton;
	ButtonActionArray[12]	= LaunchLoadPlayerButton;
	ButtonActionArray[13]	= LaunchErasePlayerButton;
	ButtonActionArray[14]	= LaunchKeyConfigButton;
	ButtonActionArray[15]	= LaunchCancelButton;			//This will handle all cancels
	
	ButtonActionArray[16]	= LaunchPlayerSlotButton;
	
	//***Title Screen
	ButtonActionArray[17]	= LaunchSinglePlayerLoginButton;
	ButtonActionArray[18]	= LaunchMultiPlayerLoginButton;
	ButtonActionArray[19]	= LaunchQuitGameButton;
	

	
	//***Multiplayer IP Login Screen
	ButtonActionArray[20]	= UpdateEnterIPButton;
	ButtonActionArray[21]	= UpdateErrorIPButton;
	
	//***Multiplayer Chat Screen
	ButtonActionArray[22]	= UpdatePlayerChatBox;

	ButtonActionArray[23]	= SelectNewPlayerButton;
	ButtonActionArray[24]	= DeselectNewPlayerButton;

	//***Info Entry update buttons
	ButtonActionArray[25]	= UpdateEnterNameButton;
	ButtonActionArray[26]	= UpdateEnterPasswordButton;
	ButtonActionArray[27]	= LaunchEnterNewPasswordButton;
	ButtonActionArray[28]	= LaunchEnterLoadPasswordButton;

	//NEW//
	ButtonActionArray[31]	= LaunchCreateGameButton;
	ButtonActionArray[32]	= UpdateCreateGameButton;
	ButtonActionArray[33]	= LaunchEnterErasePasswordButton;
	//
	ButtonActionArray[29]	= LaunchSingleTimeTrialButton;
	ButtonActionArray[30]	= LaunchTextEntry;

	//mplayer start game option screen
	ButtonActionArray[34]	= LaunchOption1Button;
	ButtonActionArray[35]	= LaunchOption2Button;
	ButtonActionArray[36]	= LaunchOption3Button;
	ButtonActionArray[37]	= LaunchOption4Button;
	ButtonActionArray[38]	= LaunchOption5Button;
	ButtonActionArray[39]	= LaunchCreateChannelPasswordButton;
	ButtonActionArray[40]	= LaunchCreateChannelButton;

	//score updates
	ButtonActionArray[41]	= LaunchDisplayServerStats;
	ButtonActionArray[42]	= UpdateDisplayServerStats;
	ButtonActionArray[43]	= LaunchDisplayPlayerStats;
	ButtonActionArray[44]	= UpdateDisplayPlayerStats;

	ButtonActionArray[45]	= LaunchServerStats;
	ButtonActionArray[46]	= LaunchPlayerStats;
	
	ButtonActionArray[47]	= LaunchMultiStart;
	ButtonActionArray[48]	= LaunchChannelList;
	ButtonActionArray[49]	= UpdateChannelList;
	ButtonActionArray[50]	= LaunchNextButton;
 	ButtonActionArray[51]	= LaunchOkChannelButton;

 	ButtonActionArray[52]	= LaunchEnterIPButton;

	//***Game Select Screen
	ButtonActionArray[53]	= LaunchSinglePuzzleGameSelectButton;
	ButtonActionArray[54]	= LaunchSingleNewPuzzleButton;
	ButtonActionArray[55]	= LaunchSingleEnterPasswordPuzzleButton;

	ButtonActionArray[56]	= UpdateSinglePuzzlePasswordButton;
	ButtonActionArray[57]	= LaunchSinglePuzzlePasswordButton;

	ButtonActionArray[58]	= LaunchPlayerStatsSinglePlayer;
	ButtonActionArray[59]	= UpdateDisplayPlayerStatsSinglePlayer;
	ButtonActionArray[60]	= GoBack;

	ButtonActionArray[61]	= LaunchCreditsButton;
	ButtonActionArray[62]	= UpdateCreditsButton;


	pFile					= fopen("Data//Menu//MenuData.txt", "r");
	
	if(!pFile)
	{
		MessageBox(NULL,"Could not open file to read InitializeButtons","Error",0);
		return;
	}

	for(int i = 0; i < iNumButtons; i++)
	{
		int a, b, c, d, e;
		Menu_Button	*pButton	= buttonArray + i;
		ZeroMemory(pButton, sizeof(Menu_Button));
		
		// set our Button ID
		pButton->buttonID = i;

		// get the text string
		LoopUntilString("textString  : ", pFile);
		READ_FILE_STRING(pFile, pButton->textString);
		if(!strcmp(pButton->textString, "999") )
			pButton->textString[0] = NULL;
		
		// get the text offset
		LoopUntilString("textOffset  : ", pFile);
		fscanf(pFile, "%d %d", &a, &b);
		if(a != 999 && b != 999)
		{
			if(i <= 10)
				pButton->OrbuttonFlags(BUTTON_FLAG_DRAW_TEXT1);
			else
				pButton->OrbuttonFlags(BUTTON_FLAG_DRAW_TEXT2);
			pButton->textOffset.x	= a;
			pButton->textOffset.y	= b;
		}
		
		
		// get the enabled / disabled info
		LoopUntilString("enabled     : ", pFile);
		fscanf(pFile, "%d", &a);
		if(a != 999)
			pButton->Enable((float)a);
		
		// get the cursor position
		LoopUntilString("cursorPos   : ", pFile);
		fscanf(pFile, "%d %d", &pButton->cursorPos.x, &pButton->cursorPos.y);
		
		
		// get the enabled position
		LoopUntilString("enabledPos  : ", pFile);
		fscanf(pFile, "%d %d", &pButton->enabledPos.x, &pButton->enabledPos.y);
		
		
		// get the disabled position
		LoopUntilString("disabledPos : ", pFile);
		fscanf(pFile, "%d %d", &pButton->disabledPos.x, &pButton->disabledPos.y);
		
		pButton->screenX	= (float)pButton->disabledPos.x;
		pButton->screenY	= (float)pButton->disabledPos.y;
		
		
		// get the surface information
		LoopUntilString("surfaceID   : ", pFile);
		fscanf(pFile, "%d", &a);
//		pButton->pSurf		= surfaceArray + a;
		pButton->surfIndex	= a;
		
		// get the clipping coordinates (when button is selected)
		LoopUntilString("clipSelect  : ", pFile);
		fscanf(pFile, "%d %d %d %d",	&pButton->clipSelect.left,
			&pButton->clipSelect.top,
			&pButton->clipSelect.right,
			&pButton->clipSelect.bottom);
		
		
		// get the clipping coordinates (when button is de-selected)
		LoopUntilString("clipDeselect: ", pFile);
		fscanf(pFile, "%d %d %d %d",	&pButton->clipDeselect.left,
			&pButton->clipDeselect.top,
			&pButton->clipDeselect.right,
			&pButton->clipDeselect.bottom);
		
		
		// get button action info
		LoopUntilString("actionID    : ", pFile);
		fscanf(pFile, "%d %d %d %d", &a, &b, &c, &d);
		pButton->pSelect					= (a != 999) ? ButtonActionArray[a]: NULL;
		pButton->pDeselect					= (b != 999) ? ButtonActionArray[b]: NULL;
		pButton->pUpdate					= (c != 999) ? ButtonActionArray[c]: NULL;
		pButton->pLaunch					= (d != 999) ? ButtonActionArray[d]: NULL;
		
		
		// set up connections to neighbor buttons
		LoopUntilString("neighbors   : ", pFile);
		fscanf(pFile, "%d %d %d %d %d", &a, &b, &c, &d, &e);
		pButton->pNeighbor[BUTTON_UP]		= (a != 999) ? buttonArray + a: NULL;
		pButton->pNeighbor[BUTTON_DOWN]		= (b != 999) ? buttonArray + b: NULL;
		pButton->pNeighbor[BUTTON_LEFT]		= (c != 999) ? buttonArray + c: NULL;
		pButton->pNeighbor[BUTTON_RIGHT]	= (d != 999) ? buttonArray + d: NULL;
		pButton->pNeighbor[BUTTON_PARENT]	= (e != 999) ? buttonArray + e: NULL;
	}
	
	fclose(pFile);
	
}

void Game_Menu::InitializeCursor(int ButtonID)
{
	
	cursor.pButton		= buttonArray + ButtonID;
	
	cursor.cursorFlags	= 0;
	cursor.cursorFlags	|= CURSOR_FLAG_MOVING;
	cursor.cursorFlags	&= ~CURSOR_FLAG_CAN_MOVE;
	
	cursor.blinkTimer	= 0;
	cursor.blinkSpeed	= 1;
	cursor.velocX		= 0;
	cursor.velocY		= 0;
	cursor.screenX		= 0;//cursor.pButton->cursorPos.x;
	cursor.screenY		= (float)cursor.pButton->cursorPos.y;
	
}



// this function loads all of the surfaces for a particular menu.
//
// note: 
void Game_Menu::LoadSurfaces()
{
	if(!App.UserSettings.RenderMode)
	{
		char	*surfFileName[NUM_OF_MENU_SURF]	= {	
												"Data//Pics//DX_SURF_01.bmp",
												"Data//Pics//MENU_SURF_02.bmp",
												"Data//Pics//MENU_SURF_03.bmp",
												"Data//Pics//MENU_SURF_04.bmp",
												"Data//Pics//MENU_SURF_05.bmp",
												"Data//Pics//MENU_SURF_06.bmp",
												"Data//Pics//MENU_SURF_07.bmp",
												"Data//Pics//MENU_SURF_08.bmp",
												};
											//	"Data//Pics//DX_SURF_02.bmp",
											//	"Data//Pics//DX_SURF_03.bmp",
											//	"Data//Pics//DX_SURF_04.bmp"};

		int SurfaceIndex = NUM_OF_MAIN_SURF;
		int FileNameIndex = 0;

		for(int i = SurfaceIndex; i < SurfaceIndex + NUM_OF_MENU_SURF; i++, FileNameIndex++)
		{
			if(!(App.pDDraw->surfaceArray[i]) )
			{
				App.pDDraw->LoadDDBitmap(App.pDDraw->surfaceArray + i, 
					surfFileName[FileNameIndex], TRUE, 0, 0);

				App.pDDraw->SetTransparency(App.pDDraw->surfaceArray + i);
			}
		}
	}
	else if(App.UserSettings.RenderMode==1)
	{
		//Init OpenGL textures
		//There are no checks if we loaded them or not because they all get loaded
		// once at initilize time.
		LoadGLBitmap (GL_SURF_MENU_BACKGROUND1_FILENAME,	GL_SURF_MENU_BACKGROUND1_TEXID,	0, 255);

		LoadGLBitmap (GL_SURF_MENU_BACKGROUND2_FILENAME,	GL_SURF_MENU_BACKGROUND2_TEXID,	0, 255);
		LoadGLBitmap (GL_SURF_MENU_BACKGROUND3_FILENAME,	GL_SURF_MENU_BACKGROUND3_TEXID,	0, 255);
		LoadGLBitmap (GL_SURF_MENU_BACKGROUND4_FILENAME,	GL_SURF_MENU_BACKGROUND4_TEXID,	0, 255);
		
		
		LoadGLBitmap (GL_SURF_MENU_BUTTONS1_FILENAME,	GL_SURF_MENU_BUTTONS1_TEXID,	1, 255);
		LoadGLBitmap (GL_SURF_MENU_CURSOR_FILENAME,		GL_SURF_MENU_CURSOR_TEXID,		1, 255);
		LoadGLBitmap (GL_SURF_MENU_CHARACTER_FILENAME,	GL_SURF_MENU_CHARACTER_TEXID,	1, 255);
		LoadGLBitmap (GL_SURF_MENU_BUTTONS2_FILENAME,	GL_SURF_MENU_BUTTONS2_TEXID,	1, 255);
		LoadGLBitmap (GL_SURF_MENU_LIGHTS_FILENAME,		GL_SURF_MENU_LIGHTS_TEXID,		1, 255);
		LoadGLBitmap (GL_SURF_MENU_BUTTONS3_FILENAME,	GL_SURF_MENU_BUTTONS3_TEXID,	1, 255);
		LoadGLBitmap (GL_SURF_MENU_IP_FILENAME,			GL_SURF_MENU_IP_TEXID,			1, 255);

	}
}




void Game_Menu::ReleaseSurfaces()
{
//	if(!App.UserSettings.RenderMode)
//	{
//	
//		//free DDraw surfaces
//		for(int i=0; i < NUM_OF_MENU_SURF; i++)
//			if(surfaceArray[i])
//			{
//				App.pDDraw->DeleteSurface(&surfaceArray[i]);
//				surfaceArray[i] = NULL;
//			}
//	}
}
/*
		PLAYER_DETAIL	Data;

		Data.id = dpId;
		strcpy(Data.name,lpName->lpszShortNameA);
		Data.type = DP_MSG_WHAT_CHARACTER;

		dp->lpDP->Send(dp->dpid,DPID_ALLPLAYERS,DPSEND_GUARANTEED,&Data, sizeof(PLAYER_DETAIL));
		
		dp->numPlayers++;
		//add this player to the list
		dp->PlayerList.AddPlayer(dpId, lpName->lpszShortNameA, 0);
*/
void Game_Menu::ProcessMultiplayerStart()
{

	if(App.pDP && App.pDP->hostflag == 1 && !App.pDP->gotAllNames)
	{
		if(GameData.pBoard == NULL)
		{
			GameData.pBoard	= new Board3D;
			GameData.pBoard->Initialize("Data\\Boards\\MPlayer_0.txt", "<MULTI_PLAYER>");
		}
		else
			GameData.pBoard->LoadBoard("Data\\Boards\\MPlayer_0.txt", "<MULTI_PLAYER>");

		//reset buttons
		GameData.pGameMenu->InitGameButtons();

		// Now send the number of rows and columns for the game board
		INITGAMEDATA InitGameData;

		InitGameData.data = DP_MSG_SETGAME;
		InitGameData.dpIdHost = App.pDP->dpid;
		InitGameData.numRows = GameData.pBoard->numRows;
		InitGameData.numCols = GameData.pBoard->numCols;
		App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&InitGameData, sizeof(INITGAMEDATA));

		// Now set the data for each individual gem
		INITGEMDATA InitGemData;

		InitGemData.data = DP_MSG_INITGEM;
		for(int i = 0; i < GameData.pBoard->numRows; i++)
			for(int j = 0; j < GameData.pBoard->numCols; j++)
			{
				InitGemData.iOffset = i;
				InitGemData.jOffset = j;
				InitGemData.FrontColor = GameData.pBoard->gem[i][j].activeColor[0];
				InitGemData.backColor = GameData.pBoard->gem[i][j].activeColor[1];
				App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&InitGemData, sizeof(INITGEMDATA));
			}

		// Set the host parameters for the game
		GameData.numplayers = App.pDP->numPlayers;
		GameData.numPlayersalive = GameData.numplayers;
		GameData.playerId = PLAYER_ONE;
		GameData.simplePlayers[GameData.playerId].id = PLAYER_ONE;
		GameData.simplePlayers[GameData.playerId].score = 0;
		GameData.simplePlayers[GameData.playerId].vitality = 100;
		GameData.simplePlayers[GameData.playerId].RowGem = GameData.pBoard->numRows/2;
		GameData.simplePlayers[GameData.playerId].ColumnGem = GameData.pBoard->numCols/2;
//FIX get my character pic
		GameData.simplePlayers[GameData.playerId].bFlipping = 0;
		GameData.simplePlayers[GameData.playerId].bSwitching = 0;
		GameData.simplePlayers[GameData.playerId].character = GameData.CharPic;

		pPLAYER_NODE pPlayer = App.pDP->PlayerList.pListHead;
		PLAYER_ID_MSG pIdMsg;

		pIdMsg.dpMsgData = DP_MSG_SET_PLAYERID;
		pIdMsg.id = PLAYER_TWO;
		pIdMsg.numplayers = GameData.numplayers;

		char szName[4][20];
		int m = 0;

		// Send each player their unique id (PLAYER_TWO - PLAYER_FOUR)
		while(pPlayer)
		{
			strncpy(szName[m], pPlayer->name, 20);
			m++;
			if(pPlayer->dpid == App.pDP->dpid)
			{
				pPlayer = pPlayer->pNext;
				continue;
			}

			App.pDP->lpDP->Send(App.pDP->dpid, pPlayer->dpid, DPSEND_GUARANTEED, &pIdMsg, sizeof(PLAYER_ID_MSG) );

			pIdMsg.id++;
			pPlayer = pPlayer->pNext;
		}	// end switch(pIdMsg.id)

		// Now initialize the values for each player's basic stats
		for(int f = PLAYER_TWO; f < PLAYER_FOUR + 1; f++)
		{
			memset(&GameData.simplePlayers[f], 0, sizeof(struct tagSimplePlayer) );
			GameData.simplePlayers[f].msgId = DP_MSG_SET_PLAYERDATA;
			GameData.simplePlayers[f].id = f;
			GameData.simplePlayers[f].score = 0;
			GameData.simplePlayers[f].vitality = 100;
			GameData.simplePlayers[f].bFlipping = 0;
			GameData.simplePlayers[f].bSwitching = 0;
		}

		GameData.simplePlayers[PLAYER_ONE].msgId = DP_MSG_SET_PLAYERDATA;

		GameData.simplePlayers[PLAYER_TWO].ColumnGem = GameData.pBoard->numCols/2;
		GameData.simplePlayers[PLAYER_TWO].RowGem = GameData.pBoard->numRows/2;

		GameData.simplePlayers[PLAYER_THREE].ColumnGem = GameData.pBoard->numCols/2;
		GameData.simplePlayers[PLAYER_THREE].RowGem = GameData.pBoard->numRows/2;

		GameData.simplePlayers[PLAYER_FOUR].ColumnGem = GameData.pBoard->numCols/2;
		GameData.simplePlayers[PLAYER_FOUR].RowGem = GameData.pBoard->numRows/2;

		// Now all the initial game data has been set so send that message
		for(int g = 0; g < GameData.numplayers; g++)
		{
			strncpy(GameData.simplePlayers[g].szName, szName[g], 20);
		}

	}

	

	if(App.pDP && App.pDP->hostflag == 1 && !App.pDP->gotAllNames)
	{	
		
		// Now all the initial game data has been set so send that message
		for(int g = 0; g < GameData.numplayers; g++)
		{
		//	strncpy(GameData.simplePlayers[g].szName, szName[g], 20);
			App.pDP->lpDP->Send(App.pDP->dpid, App.pDP->dpidRoom, DPSEND_GUARANTEED, &GameData.simplePlayers[g], sizeof(struct tagSimplePlayer) );
		}

		for(g = 0; g < GameData.numplayers; g++)
			GameData.SetPlayerPosition(g);

		App.pDP->hostflag = 0;
		App.pDP->gotAllNames = 0;
		GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_VERSUS_MULTI);
	}
}







void Game_Menu::UpdateCamPos( float fTimeDelta )
{
//	float fSpeed = 50;
	

	if( (camScreenX += camVelocX) < 0)
		camScreenX	+= 32;
	else if(camScreenX > 31)
		camScreenX	-= 32;
	
	if( (camScreenY += camVelocY) < 0)
		camScreenY	+= 32;
	else if(camScreenY > 31)
		camScreenY	-= 32;
	
	camPos.x	= static_cast<int>(camScreenX);
	camPos.y	= static_cast<int>(camScreenY);

	if(App.UserSettings.RenderMode)
	{
		camPos.x=	-camPos.x;
		camPos.y=	-camPos.y;
	}
}




void Game_Menu::UpdateCamVeloc()
{
//	camVelocX	=  1;
//	camVelocY	= -1;
	
	//	if( (camAngle += 10) > 359)
	//		camAngle -= 360;
	
	//	camVelocX	= 4 * cos(camAngle * 3.14159 / 180.0) + 2;
	//	camVelocY	= 4 * sin(camAngle * 3.14159 / 180.0);
}




void Game_Menu::DrawCursor()
{
	//Check to see if we are leaving our Game_Menu
	if(!(GameData.GameStateFlags & GAME_STATE_MENU))
		return;
	
	(!App.UserSettings.RenderMode)? DisplayCursor_DDraw():
		DisplayCursor_OpenGL();
}




void Game_Menu::UpdateCursorPos()
{
	if(cursor.pButton)
	{
		cursor.screenPos.x	= (int)( cursor.screenX += cursor.velocX );
		cursor.screenPos.y	= (int)( cursor.screenY += cursor.velocY );
		
		if(	(cursor.screenPos.x >= cursor.pButton->cursorPos.x - 1) &&
			(cursor.screenPos.x <= cursor.pButton->cursorPos.x + 1) &&
			(cursor.screenPos.y >= cursor.pButton->cursorPos.y - 1)	&&
			(cursor.screenPos.y <= cursor.pButton->cursorPos.y + 1)	)
		{
			cursor.cursorFlags	&= ~CURSOR_FLAG_MOVING;
			cursor.screenX		 = (float)cursor.pButton->cursorPos.x;
			cursor.screenY		 = (float)cursor.pButton->cursorPos.y;
			cursor.velocX		 = 0;
			cursor.velocY		 = 0;
		}
	}
}



#define absf(x)	((x) >= 0.0f ? (x) : -(x))

void Game_Menu::UpdateCursorVeloc()
{
	if(cursor.pButton)
	{
		//This function causes a slow down affect the closer we get to our
		//cursor's destination.
		
		//				(	where we want to go			-	where we are ) / 8
		cursor.velocX	= ( cursor.pButton->cursorPos.x - cursor.screenX ) / 8;
		cursor.velocY	= ( cursor.pButton->cursorPos.y - cursor.screenY ) / 8;
		
		//Just messing around 
		// (disable this code to make you sick while in the menu ) -eli
		//
		//camVelocX = 1  + absf( cursor.velocX - cursor.velocY );
		//camVelocY = -1;

	}
}