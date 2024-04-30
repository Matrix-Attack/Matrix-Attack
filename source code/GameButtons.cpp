#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <string>



using namespace std;

#include "./mmgr.h"

#include "DPlay.h"

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


#include "text.h"
#include "Blt2DopenGL.h"
#include "GLSurfDef.h"

#include "ModeSettings.h" //ALL THE PASSWORDS AND THE TIME TRIAL MINUTES

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


#define READ_FILE_STRING(pFile, string)	(fscanf((pFile), "\n%[^\n]", (string)))

void GetRandomBackground( void );

void LoopUntilString(const char *string, FILE *pFile);
void ClipToScreenEdges(RECT &clipRect, int &x, int &y);
void LoadPlayerNames(Game_Menu *pGameMenu);

void UpdateComboButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdatePlayerTabButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdateFPSButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdateMovesLeftButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdateNextPuzzlePasswordButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdateBoardStartMessageButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void LaunchContinueButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void LaunchTryAgainButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void LaunchQuitInGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void LaunchChooseRandomTrack		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void UpdateComments					(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void LaunchTrackTitle				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

int GetPuzzleLevel(char *pBoardName);
void SwitchToMenuFromGame();
int LoadPuzzleBoard();

extern APP			App;
extern GAME_DATA	GameData;
//extern 	DSound		Sound;
extern  CSound cSound;

void Game_Menu::FPSButton( int bOn )
{
	if(bOn)
	{
		buttonArray[7].Enable(0);
	}
	else
	{
		buttonArray[7].Disable(20);
	}
}

void Game_Menu::FinishGame( bool bLooser )
{
	if(bLooser)
	{
		cursor.cursorFlags			|= CURSOR_FLAG_MOVING;
		cursor.cursorFlags			&= ~CURSOR_FLAG_CAN_MOVE;
		GameData.GameStateFlags		|= GAME_STATE_CURSOR_IS_ON;
		
		buttonArray[18].Enable(10);	//Loser Random Song ?
		buttonArray[16].Enable(15);	//Loser Try again
		buttonArray[17].Enable(20);	//Loser Quit

		cursor.pButton = buttonArray + 16;
	}
	else
	{
		//*** WINNER ***

	}
	GameData.Score.bGameFinished = 1;
}

void Game_Menu::UpdateGameButtons()
{


	//	TileGameBoard();
//	App.pOpenGL->GLDrawText(545, 130, TF_SPACING_0, "STAGE"); // Draw the stage text
//	App.pOpenGL->GLDrawText(545, 251, TF_SPACING_0, "COUNT"); // Draw the count text
//	App.pOpenGL->GLDrawText(545, 372, TF_SPACING_0, "SCORE"); // Draw the score text

	//*************Debug code detection*********************************
	
/*	if(App.pdistruct->curKeyState[DIK_F1] >= 1)
	{
		if(App.bDebug)
		{
			App.bDebug = 0;
			buttonArray[7].Enable(0);
		}
		else
		{
			App.bDebug = 1;
			buttonArray[7].Enable(0);
		}
	}

	if(App.pdistruct->curKeyState[DIK_C] == 1)
	{
		if(GameData.bCamera)
		{
			GameData.bCamera = 0;
			GameData.pBoard->Camera.ResetPosition();
		}
		else
			GameData.bCamera = 1;
	}
*/
	//******************MULTI PLAYER VERSUS****************************
	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI )
	{
		//GameData.Score.UpdateTimer(0);

		if(GameData.GameOver)
		{
			char a[100];
			sprintf(a, "GAME OVER - %d IS THE WINNER", GameData.Winner);
			App.pDDraw->DDrawText(100, 100, 0, a);
		}
	}
	//******************TIME TRIAL MODE CODE*****************************
	else if((GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL) &&
		!GameData.Score.bGameFinished)
	{
		//*****UPDATE AND DRAW THE TIMER
		//GameData.Score.UpdateTimer(1);

		if(GameData.Score.minutes == 0 &&
		   GameData.Score.seconds == 0  )
		{
			cursor.cursorFlags			|= CURSOR_FLAG_MOVING;
			cursor.cursorFlags			&= ~CURSOR_FLAG_CAN_MOVE;
			GameData.GameStateFlags		|= GAME_STATE_CURSOR_IS_ON;
			
			buttonArray[18].Enable(10);	//Loser Random Song ?
			buttonArray[16].Enable(15);	//Loser Try again
			buttonArray[17].Enable(20);	//Loser Quit

			cursor.pButton = buttonArray + 16;
			
			GameData.Score.bGameFinished = 1;
			
			//Copy our points over to our Database
			if( GameData.DBase.GetActiveProfile() )
			{
				if( GameData.DBase.GetActiveProfile()->GetCurrentScore() > 0 )
				{
					int i = 0;
				}

				GameData.DBase.UpdateScore( GameData.Score.Points );

				//Game ended so lets update our profile
				if(false == GameData.DBase.CheckRecords() )
				{
					MessageBox(NULL, "Could not save records!", "FILE I/O ERROR", MB_OK);
				}
			}
			else
			{
				MessageBox(NULL, "We do not have an Active Profile to save records to!", "DBASE ERROR", MB_OK);
			}

		}
		
	}
	//******************PUZZLE MODE CODE*****************************
	else if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE) &&
		!GameData.Score.bGameFinished)
	{
		//*** PUZZLE MODE CODE ****
		bLoser = 0;
		
		int bBusyBoard	= 0,
			numRows		= GameData.pBoard->numRows,
			numCols		= GameData.pBoard->numCols;
			
		//******UPDATE AND DRAW THE TIMER
		//GameData.Score.UpdateTimer(0);

		//***If the board is busy with something (flipping/switchin) then we won't check it
		for(int i = 0; i < numRows; i++)
		{
			for(int j = 0; j < numCols; j++)
			{
				Gem3D	*pCurGem	= GameData.pBoard->gem[i] + j;
		
				if(!bBusyBoard)
					bBusyBoard = pCurGem->IsBusy();
			}
		}

		//break out of the loop if we are still flashing
		if(!bBusyBoard)
		{	
			//***See if we are out of moves
			if(!GameData.pBoard->numMoves)
				GameData.GameStateFlags |= GAME_STATE_CURSOR_IS_ON;

			//****Our board is not busy so lets see if we need to finish our game
			for(int i = 0; i < numRows; i++)
				for(int j = 0; j < numCols; j++)
				{
					Gem3D	*pCurGem	= GameData.pBoard->gem[i] + j;
					if(!pCurGem->IsClearing() && 
						(pCurGem->color[0] != 7 || pCurGem->color[1] != 7) )
					{
						bLoser = 1;
					}
				}

			if(!bLoser)
			{
				//***PUZZLE WINNER
				
				buttonArray[10].Enable(0);	//Winner button
				buttonArray[11].Enable(0);	//Next puzzle password
				
				cursor.cursorFlags		|= CURSOR_FLAG_MOVING;
				cursor.cursorFlags		&= ~CURSOR_FLAG_CAN_MOVE;
				GameData.GameStateFlags |= GAME_STATE_CURSOR_IS_ON;
				
				buttonArray[20].Enable(0);	//Winner New Song
				buttonArray[13].Enable(0);	//Winner Continue
				buttonArray[14].Enable(0);	//Winner Try again
				buttonArray[15].Enable(0);	//Winner Quit
				cursor.pButton = buttonArray + 13;
				
				GameData.Score.bGameFinished = 1;
			}
			else if(!GameData.pBoard->numMoves && bLoser)
			{
				//***PUZZLE LOSER
				buttonArray[9].Enable(0);	//Game Over button
				buttonArray[11].Enable(0);	//Puzzle password
				
				cursor.cursorFlags		|= CURSOR_FLAG_MOVING;
				cursor.cursorFlags		&= ~CURSOR_FLAG_CAN_MOVE;
				GameData.GameStateFlags |= GAME_STATE_CURSOR_IS_ON;
				
				buttonArray[16].Enable(0);	//Loser Try again
				buttonArray[17].Enable(0);	//Loser Quit
				buttonArray[18].Enable(0);	//Loser Random Song ?
				cursor.pButton = buttonArray + 16;
				
				GameData.Score.bGameFinished = 1;
			}
		}//end else (not busy)
	}//end puzzle mode code

/*
	//***************GAME CURSOR MODE****************************
	if(GameData.GameStateFlags & GAME_STATE_CURSOR_IS_ON)
	{
		RECT rcClip = {104, 182, 136, 206};

		//***Draw our Cursor
		if(!App.UserSettings.RenderMode)
		{
			App.pDDraw->getBackBuffer()->BltFast(
				cursor.screenPos.x, cursor.screenPos.y,
				App.pDDraw->surfaceArray[10],
				&rcClip, DDBLTFAST_SRCCOLORKEY);
		}
		else
		{
			BltGLStyle(cursor.screenPos.x, cursor.screenPos.y, &rcClip,
			256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		}	
	}

	//******DRAW ALL BUTTONS******
	DrawGameButtons();

	//******DRAW ALL CHARACTER FACES AND STATS*****
	DrawPlayerStats();

	//******DRAW ALL CURSORS THAT ARE NEEDED*******
	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		for(int i = 0; i < GameData.numplayers; i++)
			GameData.pBoard->DisplayMapCursor(i);
	}
	else
		GameData.pBoard->DisplayMapCursor(PLAYER_ONE);
*/

	//***Check to see if we are switching into menu mode
	if(GameData.GameStateFlags & GAME_STATE_SWITCHING)
		SwitchToMenuFromGame();
}

void Game_Menu::Update()
{

	//Update the Game menu
//	if( !App.UserSettings.RenderMode )
//		DrawBackground_DDraw();
//	else
//		DrawBackground_OpenGL(); 
	
	//*** Update Buttons
	UpdateGameButtons();

	//*** Update Cursor
//	DrawCursor();
	
	//Grossest code EVER! WHO DID THIS?  - eli.
	if(App.pDP && (App.pDP->flag & (SETUP_1) ) )
	{
		//Setup our game
		if(!GameData.pBoard)
			GameData.pBoard	= new Board3D;
		GameData.GameStateFlags |= GAME_STATE_SWITCHING;
		GameData.GameStateFlags |= GAME_STATE_VERSUS_MULTI;
		App.pDP->flag &= ~SETUP_1;
	}

}

void Game_Menu::Draw_GameMode( void )
{
	//*************** DRAW GAME CURSOR ****************************
	if(GameData.GameStateFlags & GAME_STATE_CURSOR_IS_ON)
	{
		RECT rcClip = {104, 182, 136, 206};

		//***Draw our Cursor
		if(!App.UserSettings.RenderMode)
		{
			App.pDDraw->getBackBuffer()->BltFast(
				cursor.screenPos.x, cursor.screenPos.y,
				App.pDDraw->surfaceArray[10],
				&rcClip, DDBLTFAST_SRCCOLORKEY);
		}
		else
		{
			BltGLStyle(cursor.screenPos.x, cursor.screenPos.y, &rcClip,
			256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		}	
	}
	
	
	//*** Draw the Timer
	if(!GameData.Score.bGameFinished)
	{
		if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
		{
			GameData.Score.UpdateTimer( true );
		}
		else
		{
			GameData.Score.UpdateTimer( false );
		}
	}
	
	
	//****** DRAW ALL BUTTONS******
	DrawGameButtons();

	//****** DRAW ALL CHARACTER FACES AND STATS *****
	DrawPlayerStats();

	//****** DRAW ALL CURSORS THAT ARE NEEDED *******
	
	if(!GameData.Score.bGameFinished)
	{
		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			for(int i = 0; i < GameData.numplayers; i++)
				GameData.pBoard->DisplayMapCursor(i);
		}
		else
		{
			GameData.pBoard->DisplayMapCursor(PLAYER_ONE);
		}
	}

	//**** Draw Active Side Passive Side captions
	if(!App.UserSettings.RenderMode)
	{
		App.pDDraw->DDrawText(200, 420,	TF_SPACING_1, "ACTIVE");// Draw the timer text
		App.pDDraw->DDrawText(420, 420,	TF_SPACING_1, "PASSIVE");
	}
	else
	{
		App.pOpenGL->GLDrawText(200, 420,	TF_SPACING_1, "ACTIVE");// Draw the timer text
		App.pOpenGL->GLDrawText(420, 420,	TF_SPACING_1, "PASSIVE");
	}

}


/*
void Game_Menu::Draw()
{
	//Draw the Game menu
	if( !App.UserSettings.RenderMode )
		DrawBackground_DDraw();
	else
		DrawBackground_OpenGL(); 
	
	//***Draw and Update Buttons
	DrawMenuButtons();
	
	//***Draw and Update Cursor
	DrawCursor();
	
	//Grossest code EVER! WHO DID THIS?  - eli.
	if(App.pDP && (App.pDP->flag & (SETUP_1) ) )
	{
		//Setup our game
		if(!GameData.pBoard)
			GameData.pBoard	= new Board3D;
		GameData.GameStateFlags |= GAME_STATE_SWITCHING;
		GameData.GameStateFlags |= GAME_STATE_VERSUS_MULTI;
		App.pDP->flag &= ~SETUP_1;
	}
}
*/

void Game_Menu::Draw_MenuMode( void )
{
	//Draw the Game menu
	if( !App.UserSettings.RenderMode )
	{
		DrawBackground_DDraw();
	}
	else
	{
		DrawBackground_OpenGL();
	}
	
	//***Draw and Update Buttons
	DrawMenuButtons();
	
	//***Draw and Update Cursor
	DrawCursor();
}


void Game_Menu::DrawGameButtons()
{

	// loop through all the buttons
	for(int i = 0; i < iNumButtons; i++)
	{
		Menu_Button	*pButton	= buttonArray + i;
		POINT	ButtonPos		= pButton->GetScreenPos();

		if(pButton->pUpdate && pButton->IsEnabled())
		{
			pButton->pUpdate(this, pButton, CURSOR_MOVE_NOWHERE);
		}
		else
		{
			//if(!pButton->bUpdate)
			//	continue;

			//***We are disabled
			if(!App.UserSettings.RenderMode)
			{
				pButton->ClipAgainstScreenEdges();

				//Blit the button
				App.pDDraw->getBackBuffer()->BltFast(pButton->screenPos.x, pButton->screenPos.y,
					App.pDDraw->surfaceArray[pButton->surfIndex], &pButton->clipRect, DDBLTFAST_SRCCOLORKEY);
			}
			else
			{
				//*** OpenGL ***
				//Blit the button
				BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, &pButton->clipRect,
					256, 256, pButton->surfIndex);
				

//				if(ButtonPos.x > -1 && ButtonPos.x < 640 && 
//				   ButtonPos.y > -1 && ButtonPos.y < 480	)
//				{
//					int iScaleX = abs(pButton->enabledPos.x - pButton->screenPos.x);
//					int iScaleY = abs(pButton->enabledPos.y - pButton->screenPos.y);
//
//					BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, iScaleX+iScaleY, iScaleX+iScaleY, &pButton->clipRect,
//						256, 256, pButton->surfIndex );
//				}

				

			}
		}

		//blit the text
		if(!App.UserSettings.RenderMode)
		{	//**** DDRAW MODE ****
			if(ButtonPos.x > 0 && ButtonPos.x < 640 && 
			   ButtonPos.y > 0 && ButtonPos.y < 480	)//if onscreen
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
			if(ButtonPos.x > 0 && ButtonPos.x < 640 && 
			   ButtonPos.y > 0 && ButtonPos.y < 480	)//if onscreen
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
			}//end if onscreen
		}//end openGL else
		
		
	}//end for
	

}

void Game_Menu::InitGameButtons()
{
	cursor.cursorFlags		= 0;
	iNumButtons				= NUM_OF_GAME_BUTTONS;
	
	// allocate memory for the menu's buttons
	if(buttonArray)
	{
		delete[] buttonArray;
		buttonArray = NULL;
	}
	buttonArray				= new Menu_Button[iNumButtons];
	
	// initialize some other stuff
	camPos.x				=  0;
	camPos.y				=  0;
	camVelocX				=  1;
	camVelocY				= -1;
	camAngle				=  0;
	
	LoadFromFileGameButtons();

	//Always enable our PLAYER 1 button
	buttonArray[3].Enable(20);
	
	//Disable the Moves Left counter
	buttonArray[8].Disable(40);

	if(!(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME))
	{
		//4 is the button for the 2nd player
		for(int i = 4, y = 40; i < (3 + App.pDP->numPlayers); i++, y += 20)
		{
			buttonArray[i].Enable( static_cast<float>(y) );	// PLAYER 2 button
		}
		//Enable the other player buttons (THIS NEEDS TO DETECT HOW MANY PLAYERS THERE ARE)
	//	buttonArray[4].Enable(40);	// PLAYER 2 button
	//	buttonArray[5].Enable(60);  // PLAYER 3 button
	//	buttonArray[6].Enable(80);  // PLAYER 4 button
	}
	else if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
	{
		//Enable the Moves Left Counter
		buttonArray[8].Enable(20);
	}

	//This inits our GAME cursor to Try Again? (for puzzle mode and others)
	InitializeCursor(14);

}

void Game_Menu::LoadFromFileGameButtons()
{

	
	FILE			*pFile;
	
//	ButtonAction	ButtonActionArray[10]; //the maximum amount of button actions should be here
	
	//***Player Login Screen
	ButtonActionArray[0]	= UpdateComboButton,
	ButtonActionArray[1]	= UpdatePlayerTabButton;
	ButtonActionArray[2]	= UpdateFPSButton;
	ButtonActionArray[3]	= UpdateMovesLeftButton;
	ButtonActionArray[4]	= UpdateNextPuzzlePasswordButton;
	ButtonActionArray[5]	= UpdateBoardStartMessageButton;
	ButtonActionArray[6]	= LaunchContinueButton;
	ButtonActionArray[7]	= LaunchTryAgainButton;
	ButtonActionArray[8]	= LaunchQuitInGameButton;
	
	ButtonActionArray[9]	= LaunchChooseRandomTrack;
	ButtonActionArray[10]	= UpdateComments;
	ButtonActionArray[11]	= LaunchTrackTitle;


	pFile = fopen("Data//Menu//GameData.txt", "rt");
	
	
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
		{
			pButton->textString[0]	= NULL;
		}

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
		
		pButton->screenX	= static_cast<float>(pButton->disabledPos.x);
		pButton->screenY	= static_cast<float>(pButton->disabledPos.y);
		
		
		// get the surface information
		LoopUntilString("surfaceID   : ", pFile);
		fscanf(pFile, "%d", &a);
		//pButton->pSurf		= surfaceArray + a;
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


void Game_Menu::DrawPlayerStats()
{

	int				NumOfPlayers;	//THIS NEEDS TO BE SET

	RECT			rcPIcon;
	POINT			pt;
	Menu_Button		*pButton;
	
	//***We need to figure out what face to blit here. (and set the rcPIcon)
	rcPIcon.left	= GameData.CharPic*64;	// #*64 (where # is the face we want to blit)
	rcPIcon.top		= 1;
	rcPIcon.right	= rcPIcon.left + 64;
	rcPIcon.bottom	= 65;
	
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
	{
		NumOfPlayers = 1;
	}
	else
	{
		NumOfPlayers = App.pDP->numPlayers;		//***FIX*** some sort of way to find out how many players there are needs to go here
	}

	for(int i=3; i <= 2+NumOfPlayers; i++)
	{
		if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
		{
			//***We need to figure out what face to blit here. (and set the rcPIcon)
			rcPIcon.left	= GameData.CharPic*64;	// #*64 (where # is the face we want to blit)
			rcPIcon.top		= 1;
			rcPIcon.right	= rcPIcon.left + 64;
			rcPIcon.bottom	= 65;
		}
		else
		{
			//***We need to figure out what face to blit here. (and set the rcPIcon)
			// #*64 (where # is the face we want to blit)
			rcPIcon.left	= GameData.simplePlayers[i - 3].character*64;	
			rcPIcon.top		= 1;
			rcPIcon.right	= rcPIcon.left + 64;
			rcPIcon.bottom	= 65;
		}
		pButton = buttonArray + i;
		
		if(!App.UserSettings.RenderMode)
		{

			//**** DIRECT DRAW
			if(pButton->enabledPos.x == pButton->screenPos.x)
			{	
				//*****Our player icon is not moving****** (So we will have faster code)
				//Blit the Grey Tab
				pt = pButton->GetScreenPos();
				//*** DDraw Mode ***
				//Draw Character Icon 
				App.pDDraw->getBackBuffer()->BltFast(pt.x+6, pt.y+16, 
					App.pDDraw->surfaceArray[10],
					&rcPIcon, DDBLTFAST_SRCCOLORKEY);
			
				char Score[64];
				if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
				{
					//***Draw Character's NAME
					App.pDDraw->DDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.CharacterName);		// Draw the first player icon identifier display
					
					sprintf(Score, "%d", GameData.Score.GetPoints() );

					//***Draw Streak Time
					char strStreak[64];
					sprintf(strStreak, "STREAK %0.2f", GameData.Score.fStreakTime );
					App.pDDraw->DDrawText(pt.x+6, pt.y+93, TF_SPACING_0, strStreak );
					

				}
				else
				{
					App.pDDraw->DDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.simplePlayers[i-3].szName);		// Draw the first player icon identifier display
					App.pDDraw->DDrawText(pt.x+6, pt.y+94, TF_SPACING_1, "VITAL");
					GameData.pBoard->DrawVitality(pt.x+52, pt.y+94, GameData.simplePlayers[i-3].vitality);
					// Draw the first player icon identifier display
						//***Draw Score
					sprintf(Score, "%d", GameData.simplePlayers[i-3].score );
				
				}
				App.pDDraw->DDrawText(pt.x+86-(8*(strlen(Score)-1)), pt.y+82, TF_SPACING_0, Score);
				//***Draw # of Player (ie P1)
				App.pDDraw->DDrawText(pt.x+76, pt.y+38, TF_SPACING_0, "%dP", i-2);		// Draw the first player icon identifier display
				
				//***Draw Vitality
				//DrawVitality(47, 95,  PLAYER_ONE,	vit);				// Draw player 1 health
				
			}
			else	//Our player Icon is moving
			{
				//Our player icon is moving!!!
				pButton->ClipAgainstScreenEdges();
			
				pt = pButton->GetScreenPos();

				//Blit the Grey Tab
				App.pDDraw->getBackBuffer()->BltFast(pt.x+6, pt.y+16, 
					App.pDDraw->surfaceArray[10],
					&rcPIcon, DDBLTFAST_SRCCOLORKEY);
			
				//*** DDraw Mode ***
				//Draw Character Icon 
				int x = (int)pButton->screenX+6;
				int y = (int)pButton->screenY+16;

				ClipToScreenEdges(rcPIcon, x, y);

				//***Draw the Characters Icon
				App.pDDraw->getBackBuffer()->BltFast(x, y, 
					App.pDDraw->surfaceArray[10],
					&rcPIcon, DDBLTFAST_SRCCOLORKEY);

				x = (int)pButton->screenX;
				y = (int)pButton->screenY;

				char Score[64];
				//***Draw Character's NAME
				if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
				{
					App.pDDraw->DDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.CharacterName);
					sprintf(Score, "%d", GameData.Score.GetPoints() );		// Draw the first player icon identifier display
				
					//***Draw Streak Time
					char strStreak[64];
					sprintf(strStreak, "STREAK %0.2f", GameData.Score.fStreakTime );
					App.pDDraw->DDrawText(pt.x+6, pt.y+93, TF_SPACING_0, strStreak );
					
				}
				else
				{
					App.pDDraw->DDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.simplePlayers[i-3].szName);		// Draw the first player icon identifier display
					App.pDDraw->DDrawText(pt.x+6, pt.y+94, TF_SPACING_1, "VITAL");
					GameData.pBoard->DrawVitality(pt.x+52, pt.y+94, GameData.simplePlayers[i-3].vitality);
					sprintf(Score, "%d", GameData.simplePlayers[i-3].score );		// Draw the first player icon identifier display
				}

				//***Draw # of Player (ie P1)
				App.pDDraw->DDrawText(x+76, y+38, TF_SPACING_0, "%dP", i-2);		// Draw the first player icon identifier display
		
				//***Draw Vitality
				//DrawVitality(47, 95,  PLAYER_ONE,	vit);				// Draw player 1 health
				
				//***Draw Score
				App.pDDraw->DDrawText(x+86-(8*(strlen(Score)-1)), y+82, TF_SPACING_0, Score );

			}
		}
		else
		{	//***OPEN GL MODE****
			//*****Our player icon is not moving****** (So we will have faster code)
			//Blit the Grey Tab
			pt = pButton->GetScreenPos();

						
			BltGLStyle(pt.x, pt.y, &pButton->clipRect, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);

			char Score[64];
			//***Draw Character's NAME
			if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
			{
				App.pOpenGL->GLDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.CharacterName);		// Draw the first player icon identifier display
				sprintf(Score, "%d", GameData.Score.GetPoints() );
				
				//***Draw Streak Time
				char strStreak[64];
				sprintf(strStreak, "STREAK %0.2f", GameData.Score.fStreakTime );
				App.pOpenGL->GLDrawText(pt.x+6, pt.y+93, TF_SPACING_0, strStreak );
		
			}
			else
			{
				App.pOpenGL->GLDrawText(pt.x+5, pt.y+3, TF_SPACING_1, GameData.simplePlayers[i-3].szName);		// Draw the first player icon identifier display
				
				//***Draw Vitality
				App.pOpenGL->GLDrawText(pt.x+6, pt.y+94, TF_SPACING_1, "VITAL");
				
				GameData.pBoard->DrawVitality(pt.x+52, pt.y+94, GameData.simplePlayers[i-3].vitality);

				sprintf(Score, "%d", GameData.simplePlayers[i-3].score );
			}

			//***Draw the Characters Icon
			BltGLStyle(pt.x+6, pt.y+16,  &rcPIcon, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);

			//***Draw # of Player (ie P1)
			App.pOpenGL->GLDrawText(pt.x+76, pt.y+16, TF_SPACING_0, "%dP", i-2);
					
			//***Draw Score
			App.pOpenGL->GLDrawText(pt.x+86-(8*(strlen(Score)-1)), pt.y+82, TF_SPACING_0, Score);
		}
	}//end for
}


void UpdatePlayerTabButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	
}

void UpdateComboButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{

	if(pButton->GetScreenPos().x == pButton->GetEnabledPos().x)
	{
		//time that the combo button will be on the screen
		pButton->Disable(100);
	}

}

void UpdateBoardStartMessageButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if((pButton->GetScreenPos().y >= pButton->GetEnabledPos().y - 1) &&
		(pButton->GetScreenPos().y <= pButton->GetEnabledPos().y + 1) )
	{
		//Time that the message will be on the screen
		pButton->Disable(50); //message will be up for 1 second
		//pButton->disabledPos.x = ;
		//pButton->disabledPos.y = 10;
	}
}


void LaunchContinueButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	int  LevelToLoad;
	
	LevelToLoad = GetPuzzleLevel(GameData.pBoard->GetBoardName() );
	

	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	char	Buffer1[64],
			Buffer2[64];

	if( (LevelToLoad + 1) >= 10)
	{
		sprintf(Buffer1, "Data\\Boards\\Puzzle_%d.txt", LevelToLoad+1);
		sprintf(Buffer2, "<PUZZLE_%d>", LevelToLoad+1);
	}
	else
	{
		sprintf(Buffer1, "Data\\Boards\\Puzzle_0%d.txt", LevelToLoad+1);
		sprintf(Buffer2, "<PUZZLE_%d>", LevelToLoad+1);
	}

	
	//Disable all menu crap and lode the puzzle
	GameData.GameStateFlags &= ~GAME_STATE_CURSOR_IS_ON;

	//disable all buttons
	buttonArray[10].Disable(0);		//Winner button
	buttonArray[11].Disable(8);		//Next puzzle password
	
	buttonArray[20].Disable(8);		//Winner New song
	buttonArray[13].Disable(10);	//Winner Continue
	buttonArray[14].Disable(10);	//Winner Try again
	buttonArray[15].Disable(12);	//Winner Quit

	buttonArray[9].Disable(0);		//Game Over button		
	buttonArray[18].Disable(8);		//Loser New Song ?
	buttonArray[16].Disable(10);	//Loser Try again
	buttonArray[17].Disable(12);	//Loser Quit

	//***Load GAME BOARD HERE
	GameData.pBoard->LoadBoard(Buffer1, Buffer2);

	//Reset the game
	ResetAllGameSettings();

	//Get random color
	GetRandomBackground();
}


void LaunchTryAgainButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	int  LevelToLoad;
	char	Buffer1[64];
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	//disable all buttons
	buttonArray[10].Disable(0);		//Winner button
	buttonArray[11].Disable(8);		//Next puzzle password
	buttonArray[20].Disable(10);	//New song?

	buttonArray[13].Disable(10);	//Winner Continue
	buttonArray[14].Disable(10);	//Winner Try again
	buttonArray[15].Disable(12);	//Winner Quit

	buttonArray[9].Disable(0);		//Game Over button		
	buttonArray[16].Disable(10);	//Loser Try again
	buttonArray[17].Disable(12);	//Loser Quit
	buttonArray[18].Disable(14);	//Loser Random Song ?

	//Figure out what mode we are in and then load that board
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
	{
		LevelToLoad = GetPuzzleLevel(GameData.pBoard->GetBoardName() );
		if( LevelToLoad >= 10)
			sprintf(Buffer1, "Data\\Boards\\Puzzle_%d.txt", LevelToLoad);
		else
			sprintf(Buffer1, "Data\\Boards\\Puzzle_0%d.txt", LevelToLoad);
	}
	else if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
	{
		sprintf(Buffer1, "Data\\Boards\\TTrial_0.txt");
		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
	}

	GameData.pBoard->LoadBoard(Buffer1, GameData.pBoard->GetBoardName());

	//Reset the camera
	ResetAllGameSettings();
}


void LaunchQuitInGameButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.GameStateFlags |= GAME_STATE_SWITCHING;
}

void StrToUpper( char *str )	
{ int len = strlen(str); while(len--)str[len] = toupper(str[len]); }



void LaunchTrackTitle(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	char strBuffer[512];
	char strTrackName[256];
	int	 iTrackIndex, iLen;
	
	iTrackIndex = cSound.GetCurrentTrackIndex();
	strcpy(strTrackName, cSound.m_cTrackList[iTrackIndex].m_strSongName.c_str() );
	
	iLen = strlen(strTrackName);
	if( iLen > 4 )
	{
		StrToUpper(strTrackName);
		strTrackName[iLen-4] = '\0'; //Get rid of the ".mp3" tag

		sprintf( strBuffer, "SONG %d: \"%s\"", iTrackIndex, strTrackName );	
		pButton->SetTextString( strBuffer );
		pButton->Enable(0);
	}
}


void LaunchChooseRandomTrack(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	char strBuffer[512];
	char strTrackName[256];
	int	 iTrackIndex, iLen;

	cSound.PlayNextTrack();
	
	if(pGameMenu && pGameMenu->buttonArray )
	{
		LaunchTrackTitle( pGameMenu, pGameMenu->buttonArray + 21, 0 );
	}

	iTrackIndex = cSound.GetCurrentTrackIndex();
	strcpy(strTrackName, cSound.m_cTrackList[iTrackIndex].m_strSongName.c_str() );
	
	iLen = strlen(strTrackName);
	if( iLen > 4 )
	{
		StrToUpper(strTrackName);
		strTrackName[iLen-4] = '\0'; //Get rid of the ".mp3" tag

		sprintf( strBuffer, "NEW SONG? %d: \"%s\"", iTrackIndex, strTrackName );	
		pButton->SetTextString( strBuffer );
	}

/*	if(pGameMenu && pGameMenu->buttonArray && (GameData.GameStateFlags & GAME_STATE_GAME) )
	{
		LaunchTrackTitle( pGameMenu, pGameMenu->buttonArray + 21, 0 );
	}
*/

}

void UpdateComments(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if((pButton->GetScreenPos().y >= pButton->GetEnabledPos().y - 1) &&
		(pButton->GetScreenPos().y <= pButton->GetEnabledPos().y + 1) )
	{
		//Time that the message will be on the screen
		pButton->Disable(300);// 1 second
	}
}

void UpdateNextPuzzlePasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{

	//***This function will display the next level's password to the user (if he won the last puzzle)
	char Buffer[64];
	int  PuzzleLevel;
	
	PuzzleLevel = GetPuzzleLevel(GameData.pBoard->GetBoardName() );
	
	if(PuzzleLevel > 0 && pGameMenu->bLoser)
		PuzzleLevel--;
	
	switch(PuzzleLevel)
	{
		case 0:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL1);		break;
		case 1:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL2);		break;
		case 2:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL3);		break;
		case 3:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL4);		break;
		case 4:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL5);		break;
		case 5:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL6);		break;
		case 6:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL7);		break;
		case 7:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL8);		break;
		case 8:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL9);		break;
		case 9:		strcpy(Buffer, PUZZLE_PASSWORD_LEVEL10);	break;

		case 10:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL11);	break;
		case 11:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL12);	break;
		case 12:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL13);	break;
		case 13:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL14);	break;
		case 14:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL15);	break;
		case 15:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL16);	break;
		case 16:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL17);	break;
		case 17:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL18);	break;
		case 18:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL19);	break;
		case 19:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL20);	break;

		case 20:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL21);	break;
		case 21:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL22);	break;
		case 22:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL23);	break;
		case 23:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL24);	break;
		case 24:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL25);	break;
		case 25:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL26);	break;
		case 26:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL27);	break;
		case 27:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL28);	break;
		case 28:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL29);	break;
		case 29:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL30);	break;

		case 30:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL31);	break;
		case 31:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL32);	break;
		case 32:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL33);	break;
		case 33:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL34);	break;
		case 34:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL35);	break;
		case 35:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL36);	break;
		case 36:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL37);	break;
		case 37:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL38);	break;
/*		case 38:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL39);	break;
		case 39:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL40);	break;

		case 40:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL41);	break;
		case 41:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL42);	break;
		case 42:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL43);	break;
		case 43:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL44);	break;
		case 44:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL45);	break;
		case 45:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL46);	break;
		case 46:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL47);	break;
		case 47:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL48);	break;
		case 48:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL49);	break;
		case 49:	strcpy(Buffer, PUZZLE_PASSWORD_LEVEL50);	break;
*/
		default:
			if(!App.UserSettings.RenderMode)
				App.pDDraw->DDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, "NO PASSWORD");
			else
				App.pOpenGL->GLDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, "NO PASSWORD");
			return;
	}
	
	int x = pButton->screenPos.x;
	int y = pButton->screenPos.y;

	if(!App.UserSettings.RenderMode)
	{
		//App.pDDraw->DDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, Buffer);
		
		App.pDDraw->DDrawText(x,	 y, 0, "PASSWORD:");
		App.pDDraw->DDrawText(x+110, y, TF_COLOR_LIGHT_BLUE, Buffer);
	}
	else
	{
		App.pOpenGL->GLDrawText(x,		y, 0, "PASSWORD:");
		App.pOpenGL->GLDrawText(x+110,	y, TF_COLOR_LIGHT_BLUE, Buffer);
		//App.pOpenGL->GLDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, Buffer);
	}

}


void UpdateMovesLeftButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if(!App.UserSettings.RenderMode)
	{
		pButton->ClipAgainstScreenEdges();

		//Blit the button
		App.pDDraw->getBackBuffer()->BltFast(pButton->screenPos.x, pButton->screenPos.y,
			App.pDDraw->surfaceArray[pButton->surfIndex], &pButton->clipRect, DDBLTFAST_SRCCOLORKEY);
		//Blit the Moves left
		App.pDDraw->DDrawText(pButton->screenPos.x+70, pButton->screenPos.y+28, 
			TF_FONT_LARGE | TF_COLOR_LIGHT_BLUE,	"%d", GameData.pBoard->numMoves);
	}
	else
	{
		//*** OpenGL ***
		//Blit the button
		BltGLStyle(pButton->screenPos.x, pButton->screenPos.y, &pButton->clipRect,
			256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		//Blit the Moves left
		App.pOpenGL->GLDrawText(pButton->screenPos.x+70, pButton->screenPos.y+28, 
			TF_FONT_LARGE | TF_COLOR_LIGHT_BLUE, "%d", GameData.pBoard->numMoves);
	}
}


void UpdateFPSButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	
	//***********///DRAW OUR OPENGL FRAME RATE\\\***********
	DWORD dwCurrentTime;	
	App.IncFrameCount();
	dwCurrentTime = GetTickCount() - App.GetFrameTime();
	if (dwCurrentTime > 1000)
	{	App.SetFrames( (App.GetFrameCount() * 1000) / dwCurrentTime );
		App.SetFrameTime( GetTickCount() );
		App.SetFrameCount(0);
	}
	
	char buff[64];
	sprintf( buff, "FPS:%d", App.GetFrames() );
	
	if(App.UserSettings.RenderMode)
		App.pOpenGL->GLDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, buff);
	else
		App.pDDraw->DDrawText(pButton->screenPos.x, pButton->screenPos.y, 0, buff);
	//***********\\\DRAW OUR OPENGL FRAME RATE///************
	
}


	//***Check to see if we are switching modes
void SwitchToMenuFromGame()
{
	GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;

	//Get rid of our Game_state flag for when we init our gameMenu
	if(GameData.GameStateFlags & GAME_STATE_GAME)
		GameData.GameStateFlags &= ~GAME_STATE_GAME;
	GameData.GameStateFlags |= GAME_STATE_MENU;

	//***Set the correct menu to initialize to...
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
	{
		GameData.pGameMenu->Initialize(MENU_ID_SINGLE_PUZZLE_SCREEN);
		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND3_TEXID;
	}
	else if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
	{
		GameData.pGameMenu->Initialize(MENU_ID_SINGLE_GAME_SELECT);
	}
	else if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
	{
		GameData.pGameMenu->Initialize(MENU_ID_MULTI_CHAT_SCREEN);
	}
	else if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		GameData.pGameMenu->Initialize(MENU_ID_MULTI_CHAT_SCREEN);
	}

	//***Make sure that at the top of the screen we see the SINGLE/MULTI PLAYER sign
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
	{
		//Enable our Single player button (sign)
		GameData.pGameMenu->buttonArray[27].disabledPos.x	= 0;
		GameData.pGameMenu->buttonArray[27].disabledPos.y	= 0;
		GameData.pGameMenu->buttonArray[27].Disable(0);
	}
	else
	{
		//Enable our Multi player button (sign)
		GameData.pGameMenu->buttonArray[28].disabledPos.x	= 0;
		GameData.pGameMenu->buttonArray[28].disabledPos.y	= 0;
//		GameData.pGameMenu->buttonArray[28].Disable(0);
	}

	//Now that our flags have all been checked lets make sure everything is set correctly
	GameData.GameStateFlags = GAME_STATE_MENU;
	LoadPlayerNames(GameData.pGameMenu);
	
	// Stop the music
	//cSound.m_cTrackList[0].Stop();
	
	//Start playing lobby music if you can
	if( 0 == cSound.PlayTrack( "Flange.mp3" ) )
	{
		//If you can't play lobby music then stop all music
		cSound.PlayRandomTrack();
	}
	
}

void GetRandomBackground( void )
{
	int i = rand() % 4; 

	switch(i)
	{
		default:
		case 0:		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID; break;
		case 1:		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND2_TEXID; break;
		case 2:		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND3_TEXID; break;
		case 3:		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND4_TEXID; break;
	}
}

void SwitchToGameFromMenu()
{
//*****************************************************************
//************** INIT GAME MODE CODE HERE *************************
//*****************************************************************

	if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
	{
		//*************PUZZLE MODE INIT************************
		//***Init GAME BOARD HERE
		if ( LoadPuzzleBoard() )
		{
			GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_SINGLE_PUZZLE);
			
			//***These functions are standard for all game inits
			GameData.pGameMenu->InitGameButtons();
			ResetAllGameSettings();

			// Stop the music  <- ( you mean start? -eli )
			//cSound.m_cTrackList[0].Play();
			cSound.PlayRandomTrack();
			if(GameData.pGameMenu && GameData.pGameMenu->buttonArray  )
			{
				LaunchTrackTitle( GameData.pGameMenu, GameData.pGameMenu->buttonArray + 21, 0 );
			}
			GetRandomBackground();
		}
		else
		{
			//Puzzle didn't work...
			GameData.GameStateFlags = (GAME_STATE_MENU);
		}
	
	}
	else if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
	{
		//*************TIME TRIAL MODE INIT************************
		if(GameData.pBoard == NULL)
		{
			//More gross code
			GameData.pBoard	= new Board3D;
			GameData.pBoard->Initialize("Data\\Boards\\TTrial_0.txt", "<TIME_TRIAL>");
		}
		else
		{
			GameData.pBoard->LoadBoard("Data\\Boards\\TTrial_0.txt", "<TIME_TRIAL>");
		}

		GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_SINGLE_TTRIAL);
		//***These functions are standard for all game inits
		GameData.pGameMenu->InitGameButtons();
		ResetAllGameSettings();

		// Play the music
		//cSound.m_cTrackList[0].Play();
		cSound.PlayRandomTrack();
		if(GameData.pGameMenu && GameData.pGameMenu->buttonArray  )
		{
			LaunchTrackTitle( GameData.pGameMenu, GameData.pGameMenu->buttonArray + 21, 0 );
		}
		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
	}
	else if(GameData.GameStateFlags & GAME_STATE_VERSUS_TTRIAL)
	{
		//*************VERSUS TIME TRIAL MODE INIT************************
		GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_VERSUS_TTRIAL);
		//***These functions are standard for all game inits
		GameData.pGameMenu->InitGameButtons();
		ResetAllGameSettings();

		// Play the music
		//cSound.m_cTrackList[0].Play();
		cSound.PlayRandomTrack();
		if(GameData.pGameMenu && GameData.pGameMenu->buttonArray )
		{
			LaunchTrackTitle( GameData.pGameMenu, GameData.pGameMenu->buttonArray + 21, 0 );
		}
		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
	}
	else if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		//*************VERSUS MULTI MODE INIT************************
		GameData.pGameMenu->ProcessMultiplayerStart();

		if(App.pDP && (App.pDP->flag & (SETUP_3)))
		{
			Menu_Button	*buttonArray	= GameData.pGameMenu->GetButtonArray();

			buttonArray[53].Disable(0);
			buttonArray[54].Disable(0);

			//invisible buttons
			buttonArray[55].Disable(0);
			buttonArray[56].Disable(0);
			buttonArray[57].Disable(0);
			buttonArray[58].Disable(0);
			buttonArray[59].Disable(0);
			

			GameData.pBoard->Initialize("Data\\Boards\\MPlayer_0.txt", "<MULTI_PLAYER>");
			//***These functions are standard for all game inits
			GameData.pGameMenu->InitGameButtons();
			ResetAllGameSettings();
			App.pDP->flag = 0;
			GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_VERSUS_MULTI);

			// Play the music
			//cSound.m_cTrackList[0].Play();
			cSound.PlayRandomTrack();
			if(GameData.pGameMenu && GameData.pGameMenu->buttonArray )
			{
				LaunchTrackTitle( GameData.pGameMenu, GameData.pGameMenu->buttonArray + 21, 0 );
			}
		}
		GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
	}
	else
		MessageBox(NULL, "We dont have a case for this game mode init", "SwitchToGameFromMenu()", MB_OK);

}


void ResetAllGameSettings()
{
	//***Reset the Cursor if it is on
	if(GameData.GameStateFlags & GAME_STATE_CURSOR_IS_ON)
		GameData.GameStateFlags &= ~GAME_STATE_CURSOR_IS_ON;

	//***Reset the Camera
	GameData.pBoard->Camera.ResetPosition();

	//***Enable the Start Message for the player to see
	GameData.pGameMenu->buttonArray[12].Enable(0);
	strcpy(GameData.pGameMenu->buttonArray[12].GetTextString(), GameData.pBoard->szStartMsg);
	GameData.pGameMenu->buttonArray[12].enabledPos.x = 320-((strlen(GameData.pBoard->szStartMsg)*10)/2);
	
//	GameData.CharPic = GameData.CurrentCharacterSlot - 44;

	//***Reset the Score and Timer
	GameData.Score.ResetAll();
	
	//*** Set the active profile!!!!
	if( false == GameData.DBase.SetActiveProfile( GameData.CharacterName ) )
	{
		char strBuffer[256];
		sprintf(strBuffer, "Could not SetActiveProfile for \"%s\"!!!", GameData.CharacterName);
		MessageBox(NULL, strBuffer, "ResetAllGameSettings()", MB_OK);
	}
	else
	{
		//We successfully set an active profile so clear its score!
		GameData.DBase.GetActiveProfile()->ClearScore();
	}
	
	//***If we are in TimeTrial Mode we need to set our minutes up. (they can't be Zero)
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME)
	{
		GameData.playerId = 0;
		if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
		{
			GameData.Score.minutes = TTRIAL_MINUTES_ALLOTED;
		}

	}
//	else if(GameData.GameStateFlags & GAME_STATE_VERSUS_GAME)
//	{
//		
//	}
}


int LoadPuzzleBoard()
{
	char strName[256];
	int iLen		= 0;
	int LevelToLoad = 0; //0 means the password didn't work
	
	//See if the password has the word puzzle as the first word
	char strTemp[128];
	
	
	strcpy(strName, GameData.pGameMenu->buttonArray[74].GetTextString() );
	iLen = strlen(strName);

	if( iLen )
	{
		
		int i = 0;
		while(i < 6) 
		{ 
			strTemp[i] = strName[i]; 
			i++; 
		}
		strTemp[i] = '\0';
		
		//See if we wish to skip to a puzzle using the cheat password
		if( !strcmp(strTemp, "PUZZLE") && iLen <= 8)
		{
			strTemp[0] = strName[6];
			strTemp[1] = strName[7];
			strTemp[2] = 0;

			//We are probably warping to a puzzle
			LevelToLoad = atoi( strTemp );
		}
		
		//***Check to see if we have a valid password
		//*** I KNOW YOU GUYS WANT A FOR LOOP HERE BUT IT WON'T WORK WITH #DEFINES (cuz i dont wanna read from a file)
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL1) )	LevelToLoad = 1;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL2) )	LevelToLoad = 2;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL3) )	LevelToLoad = 3;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL4) )	LevelToLoad = 4;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL5) )	LevelToLoad = 5;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL6) )	LevelToLoad = 6;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL7) )	LevelToLoad = 7;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL8) )	LevelToLoad = 8;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL9) )	LevelToLoad = 9;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL10) )	LevelToLoad = 10;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL11) )	LevelToLoad = 11;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL12) )	LevelToLoad = 12;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL13) )	LevelToLoad = 13;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL14) )	LevelToLoad = 14;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL15) )	LevelToLoad = 15;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL16) )	LevelToLoad = 16;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL17) )	LevelToLoad = 17;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL18) )	LevelToLoad = 18;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL19) )	LevelToLoad = 19;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL20) )	LevelToLoad = 20;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL21) )	LevelToLoad = 21;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL22) )	LevelToLoad = 22;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL23) )	LevelToLoad = 23;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL24) )	LevelToLoad = 24;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL25) )	LevelToLoad = 25;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL26) )	LevelToLoad = 26;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL27) )	LevelToLoad = 27;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL28) )	LevelToLoad = 28;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL29) )	LevelToLoad = 29;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL30) )	LevelToLoad = 30;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL31) )	LevelToLoad = 31;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL32) )	LevelToLoad = 32;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL33) )	LevelToLoad = 33;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL34) )	LevelToLoad = 34;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL35) )	LevelToLoad = 35;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL36) )	LevelToLoad = 36;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL37) )	LevelToLoad = 37;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL38) )	LevelToLoad = 38;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL39) )	LevelToLoad = 39;
		else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL40) )	LevelToLoad = 40;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL41) )	LevelToLoad = 41;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL42) )	LevelToLoad = 42;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL43) )	LevelToLoad = 43;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL44) )	LevelToLoad = 44;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL45) )	LevelToLoad = 45;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL46) )	LevelToLoad = 46;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL47) )	LevelToLoad = 47;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL48) )	LevelToLoad = 48;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL49) )	LevelToLoad = 49;
		//else if(!strcmp(strName, PUZZLE_PASSWORD_LEVEL50) )	LevelToLoad = 50;
	}
	else
	{
		if(GameData.pBoard == NULL)
		{
			GameData.pBoard	= new Board3D;
			GameData.pBoard->Initialize("Data\\Boards\\Puzzle_01.txt", "<PUZZLE_1>");
		}
		else
			GameData.pBoard->LoadBoard("Data\\Boards\\Puzzle_01.txt", "<PUZZLE_1>");
		return 1;
	}

	if(LevelToLoad >= 1 && LevelToLoad <= 40)
	{
		char	Buffer1[64],
				Buffer2[64];

		if( LevelToLoad >= 10)
		{
			sprintf(Buffer1, "Data\\Boards\\Puzzle_%d.txt", LevelToLoad);
			sprintf(Buffer2, "<PUZZLE_%d>", LevelToLoad);
		}
		else
		{
			sprintf(Buffer1, "Data\\Boards\\Puzzle_0%d.txt", LevelToLoad);
			sprintf(Buffer2, "<PUZZLE_%d>", LevelToLoad);
		}
		
		GameData.GameStateFlags = (GAME_STATE_GAME | GAME_STATE_SINGLE_PUZZLE);
		
		//Disable all menu crap and lode the puzzle
		//***Init GAME BOARD HERE
		if(GameData.pBoard == NULL)
		{
			GameData.pBoard	= new Board3D;
			GameData.pBoard->Initialize(Buffer1, Buffer2);
		
		}
		else
		{
			if( GameData.pBoard->LoadBoard(Buffer1, Buffer2) )
				return 0;
		}
		
		return 1;
	}//end if

	return 0;
}

int GetPuzzleLevel(char *pBoardName)
{
	
	if(		!strcmp(pBoardName, "<PUZZLE_1>") )		return 1;
	else if(!strcmp(pBoardName, "<PUZZLE_2>") )		return 2;
	else if(!strcmp(pBoardName, "<PUZZLE_3>") )		return 3;
	else if(!strcmp(pBoardName, "<PUZZLE_4>") )		return 4;
	else if(!strcmp(pBoardName, "<PUZZLE_5>") )		return 5;
	else if(!strcmp(pBoardName, "<PUZZLE_6>") )		return 6;
	else if(!strcmp(pBoardName, "<PUZZLE_7>") )		return 7;
	else if(!strcmp(pBoardName, "<PUZZLE_8>") )		return 8;
	else if(!strcmp(pBoardName, "<PUZZLE_9>") )		return 9;
	else if(!strcmp(pBoardName, "<PUZZLE_10>") )	return 10;
	else if(!strcmp(pBoardName, "<PUZZLE_11>") )	return 11;
	else if(!strcmp(pBoardName, "<PUZZLE_12>") )	return 12;
	else if(!strcmp(pBoardName, "<PUZZLE_13>") )	return 13;
	else if(!strcmp(pBoardName, "<PUZZLE_14>") )	return 14;
	else if(!strcmp(pBoardName, "<PUZZLE_15>") )	return 15;
	else if(!strcmp(pBoardName, "<PUZZLE_16>") )	return 16;
	else if(!strcmp(pBoardName, "<PUZZLE_17>") )	return 17;
	else if(!strcmp(pBoardName, "<PUZZLE_18>") )	return 18;
	else if(!strcmp(pBoardName, "<PUZZLE_19>") )	return 19;
	else if(!strcmp(pBoardName, "<PUZZLE_20>") )	return 20;
	else if(!strcmp(pBoardName, "<PUZZLE_21>") )	return 21;
	else if(!strcmp(pBoardName, "<PUZZLE_22>") )	return 22;
	else if(!strcmp(pBoardName, "<PUZZLE_23>") )	return 23;
	else if(!strcmp(pBoardName, "<PUZZLE_24>") )	return 24;
	else if(!strcmp(pBoardName, "<PUZZLE_25>") )	return 25;
	else if(!strcmp(pBoardName, "<PUZZLE_26>") )	return 26;
	else if(!strcmp(pBoardName, "<PUZZLE_27>") )	return 27;
	else if(!strcmp(pBoardName, "<PUZZLE_28>") )	return 28;
	else if(!strcmp(pBoardName, "<PUZZLE_29>") )	return 29;
	else if(!strcmp(pBoardName, "<PUZZLE_30>") )	return 30;
	else if(!strcmp(pBoardName, "<PUZZLE_31>") )	return 31;
	else if(!strcmp(pBoardName, "<PUZZLE_32>") )	return 32;
	else if(!strcmp(pBoardName, "<PUZZLE_33>") )	return 33;
	else if(!strcmp(pBoardName, "<PUZZLE_34>") )	return 34;
	else if(!strcmp(pBoardName, "<PUZZLE_35>") )	return 35;
	else if(!strcmp(pBoardName, "<PUZZLE_36>") )	return 36;
	else if(!strcmp(pBoardName, "<PUZZLE_37>") )	return 37;
	else if(!strcmp(pBoardName, "<PUZZLE_38>") )	return 38;
	else if(!strcmp(pBoardName, "<PUZZLE_39>") )	return 39;
	else if(!strcmp(pBoardName, "<PUZZLE_40>") )	return 40;
	else if(!strcmp(pBoardName, "<PUZZLE_41>") )	return 41;
	else if(!strcmp(pBoardName, "<PUZZLE_42>") )	return 42;
	else if(!strcmp(pBoardName, "<PUZZLE_43>") )	return 43;
	else if(!strcmp(pBoardName, "<PUZZLE_44>") )	return 44;
	else if(!strcmp(pBoardName, "<PUZZLE_45>") )	return 45;
	else if(!strcmp(pBoardName, "<PUZZLE_46>") )	return 46;
	else if(!strcmp(pBoardName, "<PUZZLE_47>") )	return 47;
	else if(!strcmp(pBoardName, "<PUZZLE_48>") )	return 48;
	else if(!strcmp(pBoardName, "<PUZZLE_49>") )	return 49;
	else if(!strcmp(pBoardName, "<PUZZLE_50>") )	return 50;
	else
		return 0;//special error code
}