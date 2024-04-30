/*
#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
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
#include "./mmgr.h"
extern GAME_DATA	GameData;
extern APP	App;

void UpdateMenu(
	long	TimeIncrement)
{
	static long	incrementer=0;

	long		counter;
	long		numButtons;
	Menu_Button	*pButton;

	if(!GameData.pGameMenu)
		return;

	numButtons=	GameData.pGameMenu->GetNumButtons();

	GameData.pGameMenu->HandleKeyInput();

	if(incrementer == 4)
	{
		GameData.pGameMenu->UpdateCamVeloc();
		GameData.pGameMenu->UpdateCamPos();

		for(counter=0;counter < numButtons;++counter)
		{
			pButton=	GameData.pGameMenu->GetButtonArray() + counter;

			//button updates
			pButton->Update();
		}

		if(GameData.pGameMenu->cursor.cursorFlags & CURSOR_FLAG_MOVING)
			GameData.pGameMenu->UpdateCursorVeloc();

		GameData.pGameMenu->UpdateCursorPos();

		incrementer=0;
	}

	++incrementer;
}

void UpdateGame(
	long	TimeIncrement)
{
	static long	incrementer=0,
				incrementer2=0,
				incrementer3=0;

	long		counterx,
				countery;
	Gem3D		*pCurGem;
	long		counter,
				numButtons;
	Menu_Button	*pButton;


	if(incrementer == 4)
	{
		//
		//updates for the board
	//	GameData.pBoard->Camera[0].Update();
		//if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI && !GameData.GameOver)

		// update the camera
		if(GameData.bCamera && !GameData.GameOver && !GameData.Score.bGameFinished)
		// update the camera
			GameData.pBoard->player.pCamera->Update();

		//***Update our particle engine (its numbers)
		(!App.UserSettings.RenderMode)?
			App.pDDraw->UpdateParticleEngine():
			App.pOpenGL->glUpdateParticleEngine();

		//
		//update the player keyboard input
		//board->player[0].HandleKeyInput();
		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			if(GameData.simplePlayers[GameData.playerId].vitality > 0)
			{
				GameData.pBoard->player.Update();
				GameData.pBoard->player.HandleKeyInput();
			}
		}
		else if(!GameData.Score.bGameFinished)
		{
			GameData.pBoard->player.Update();
			GameData.pBoard->player.HandleKeyInput();
		}
		else
			GameData.pGameMenu->HandleKeyInput();
		//
		//updates for the game buttons
		GameData.pGameMenu->UpdateCamVeloc();
		GameData.pGameMenu->UpdateCamPos();
		
		numButtons=	GameData.pGameMenu->GetNumButtons();
		for(counter=0;counter < numButtons; ++counter)
		{
			pButton = GameData.pGameMenu->GetButtonArray() + counter;

			//button updates
			pButton->Update();
		}

		if(GameData.GameStateFlags & GAME_STATE_CURSOR_IS_ON)
		{
			if(GameData.pGameMenu->cursor.cursorFlags & CURSOR_FLAG_MOVING)
				GameData.pGameMenu->UpdateCursorVeloc();

			GameData.pGameMenu->UpdateCursorPos();
			

		}

		incrementer=0;
	}

	if(incrementer2 == 6)
	{
////
		for(counterx=0;counterx < GameData.pBoard->numRows;counterx++)
		{
			for(countery=0;countery < GameData.pBoard->numCols;countery++)
			{
				pCurGem	= GameData.pBoard->gem[counterx] + countery;

				pCurGem->Update();
			}
		}
////
		incrementer2=0;
	}

	++incrementer;
	++incrementer2;	
	++incrementer3;	
	
}


*/
