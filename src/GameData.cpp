#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <dplay.h>
#include <dplobby.h>

#include <string>

using namespace std;

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"

#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"

#include "camera3d.h"
#include "board.h"

// Sound includes

#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"
extern CSound cSound;



extern APP App;
//extern GAME_DATA GameData;

void LaunchTrackTitle(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);


void GAME_DATA::Clear( void )
{
	pGameMenu			= NULL;
	pBoard				= NULL;
	GameStateFlags		= GAME_STATE_MENU;
	ChatString[0]		= NULL;
	ChrIndex			= 0;
	bLoggingOntoServer	= 0;
	IPAddress[0][0]		= NULL;
	IPAddress[1][0]		= NULL;
	IPAddress[2][0]		= NULL;
	IPAddress[3][0]		= NULL;

	bCamera				= 0;
	mpOptions.option[0] = 0;
	mpOptions.option[1] = 0;
	mpOptions.option[2] = 0;
	mpOptions.option[3] = 0;
	mpOptions.option[4] = 0;

	iCurrentCharacterButton = 0;
	CharacterName[0]		= NULL;
	CharacterPassword[0]	= NULL;
	iCharacterProfileSlot	= 0;
	
	//*** Timer
	CTimer			Timer;
	float			m_fCurrentTime;
	float			m_fDeltaTime;
	float			m_fPrevTime;


	m_fCurrentTime	 	= 0.0f;
	m_fDeltaTime		= 0.0f;
	m_fPrevTime			= 0.0f;
	
	Score.ResetAll();

	bDrawSwitcher = false;
}

void GAME_DATA::Destroy( void )
{
	if( pGameMenu )
	{
		delete pGameMenu;
		pGameMenu = NULL;
	}

	if(pBoard)
	{
		delete pBoard;
		pBoard = NULL;
	}

	Clear();
}

void GAME_DATA::Update( void )
{
	
	//*** Update Time
	m_fCurrentTime	= Timer.GetTheTime();
	m_fDeltaTime	= m_fCurrentTime - m_fPrevTime;
	m_fPrevTime		= m_fCurrentTime;
	
	//Maximum threshold if the computer hicupps
	if(m_fDeltaTime > 0.05f)
	{
		m_fDeltaTime = 0.05f;
	}

	// ********************************
	// *** GAME BUTTON UPDATE TIMER ***
	// ********************************
	pGameMenu->Update();
	
	// *** MENU ***
	if(GameStateFlags & GAME_STATE_MENU)
	{
		int	numButtons;	
		
		if(!pGameMenu)
			return;
		
		numButtons = pGameMenu->GetNumButtons();
		
		//HandleKeyInput for menus
		pGameMenu->HandleKeyInput();
		
		static f32 fMenuUpdateTime = 0;
		fMenuUpdateTime += m_fDeltaTime;

		//updates for the board
		if( fMenuUpdateTime >= 0.0115f )
		{
			fMenuUpdateTime = 0.0f;
			
			pGameMenu->UpdateCamVeloc();
			pGameMenu->UpdateCamPos( m_fDeltaTime );
			
			Menu_Button* pButtonArray = pGameMenu->GetButtonArray();

			for(int i=0; i < numButtons; ++i )
			{
//				if(pButtonArray[i].bUpdate)
				{
					pButtonArray[i].Update( m_fDeltaTime );
				}
				//pButton = pGameMenu->GetButtonArray() + counter;
				//button updates
				//pButton->Update();
			}
			if(pGameMenu->cursor.cursorFlags & CURSOR_FLAG_MOVING)
				pGameMenu->UpdateCursorVeloc();

			pGameMenu->UpdateCursorPos();
		}
	}
	// *** GAME ***
	else if(GameStateFlags & GAME_STATE_GAME)
	{
//		Menu_Button*	pButton;
		Gem3D*			pCurGem;
		int				numButtons;
		

		//Check to see if we are finished playing a song
		if( cSound.IsTrackFinished() )
		{
			//Play the next song
			cSound.PlayNextTrack();
			if(pGameMenu && pGameMenu->buttonArray && (GameStateFlags & GAME_STATE_GAME) )
			{
				LaunchTrackTitle( pGameMenu, pGameMenu->buttonArray + 21, 0 );
			}
		}

		// **************************
		// *** BOARD UPDATE TIMER ***
		// **************************
		static f32 fBoardUpdateTime = 0;
		fBoardUpdateTime += m_fDeltaTime;
		
		//updates for the board
		if( fBoardUpdateTime >= 0.015f )
		{
			fBoardUpdateTime = 0.0f;
			
			// update the camera
			if(bCamera && !GameOver && !Score.bGameFinished)
			{
				// update the camera
				pBoard->player.pCamera->Update();
				pBoard->player.pCamera->HandleKeyInput();
			}
			
			//updates for the game buttons
			pGameMenu->UpdateCamVeloc();
			pGameMenu->UpdateCamPos( m_fDeltaTime );

			//*** Update the buttons
			Menu_Button* pButtonArray = pGameMenu->GetButtonArray();
			numButtons = pGameMenu->GetNumButtons();
			for(int i=0; i < numButtons; ++i )
			{
//				if(pButtonArray[i].bUpdate)
				{
					pButtonArray[i].Update( m_fDeltaTime );
				}
//				pButton = pGameMenu->GetButtonArray() + i;
//				pButton->Update( m_fDeltaTime );//button updates
			}
			
		}

			//*** Update Player Input
			//board->player[0].HandleKeyInput();
			if(GameStateFlags & GAME_STATE_VERSUS_MULTI)
			{
				//MULTIPLAYER
				if(simplePlayers[playerId].vitality > 0)
				{
					pBoard->player.Update();
					pBoard->player.HandleKeyInput( m_fDeltaTime );
				}
			}
			else if(!Score.bGameFinished)
			{
				//SINGLE PLAYER 
				pBoard->player.Update();
				pBoard->player.HandleKeyInput( m_fDeltaTime );
			}
			else
			{
				//WE ARE IN THE MENU!!!
				//Standard player input
				pGameMenu->HandleKeyInput();
			}

			//*** Update the cursor
			if(GameStateFlags & GAME_STATE_CURSOR_IS_ON)
			{
				//This means that we are in a menu while in the game.

				if(pGameMenu->cursor.cursorFlags & CURSOR_FLAG_MOVING)
					pGameMenu->UpdateCursorVeloc();
				
				pGameMenu->UpdateCursorPos();
			}

		// ************************
		// *** GEM UPDATE TIMER ***
		// ************************
		static f32 fGemUpdateTime = 0;
		fGemUpdateTime += m_fDeltaTime;

		if( fGemUpdateTime >= 0.02f )
		{
			fGemUpdateTime = 0.0f;
			
			int bBusyBoard = 0;
			//Update all gems on board
			for(int i=0; i < pBoard->numRows; i++)
			{
				for(int j=0; j < pBoard->numCols; j++)
				{
					pCurGem	= pBoard->gem[i] + j;
					pCurGem->Update( m_fDeltaTime );
					
					if(!bBusyBoard)// we should only set bBusyBoard once
						bBusyBoard = pCurGem->IsBusy();
					
					//set the flag that we checked this gem
					pCurGem->stateFlags &= ~GEM_FLAG_ALREADY_CHECKED;
				}
			}
			

			//Don't stop our combo count untill all our gems are done switching and flipping
			if( bBusyBoard )
			{
				Score.fStreakTime += m_fDeltaTime;

				//Check our streak time to see if we set a new record
				DBase.UpdateStreak( Score.fStreakTime );	
			}
			else
			{
				//Board isn't busy anymore!! DOH!!!!
				float fStreakTime = Score.fStreakTime;
				
				//Check to see how HI we went
				if( (GameStateFlags & GAME_STATE_SINGLE_TTRIAL) &&
					fStreakTime >= 2.9f							)
				{
					//GOOD JOB!
					char strBuffer[256];
					sprintf(strBuffer, "(%.02f)", fStreakTime);
					
					if( fStreakTime <= 3.0f )		strcat(strBuffer, " - BABY STEPS...");
					else if( fStreakTime <= 4.0f )	strcat(strBuffer, " - KEEP THE STREAK GOING!");
					else if( fStreakTime <= 5.0f )	strcat(strBuffer, " - YOU NEED TO DO BETTER THAN THAT!");
					else if( fStreakTime <= 6.0f )	strcat(strBuffer, " - GOTTA HANG ON LONGER NEXT TIME!");
					else if( fStreakTime <= 7.0f )	strcat(strBuffer, " - KEEP THAT BOARD ACTIVE!");
					else if( fStreakTime <= 8.0f )	strcat(strBuffer, " - JUST A LITTLE MORE CLUSTER LOVIN'");
					else if( fStreakTime <= 9.0f )	strcat(strBuffer, " - THERE YOU GO!");
					else if( fStreakTime <= 10.0f )	strcat(strBuffer, " - YOUR ON YOUR WAY!");					
					else if( fStreakTime <= 12.9f )	strcat(strBuffer, " - ALMOST AT YOUR TEENS!");
					else if( fStreakTime <= 14.0f )	strcat(strBuffer, " - THERE YA GO!");
					else if( fStreakTime <= 16.0f )	strcat(strBuffer, " - KEEP IT COMING!");
					else if( fStreakTime <= 18.0f )	strcat(strBuffer, " - GOOD JOB!");
					else if( fStreakTime <= 20.0f ) strcat(strBuffer, " - HOLY COW!");
					else if( fStreakTime <= 22.0f ) strcat(strBuffer, " - HEADING FOR THE PROMISE LAND?");
					else if( fStreakTime <= 24.0f )	strcat(strBuffer, " - YOU ARE GOOD AT MATRIX ATTACK!");
					else if( fStreakTime <= 28.0f )	strcat(strBuffer, " - YOU ARE AWESOME AT MATRIX ATTACK!");
					else if( fStreakTime <= 30.0f )	strcat(strBuffer, " - YOU ARE ARE A GEM SWITCHING MAD MAN!");
					else if( fStreakTime <= 34.0f )	strcat(strBuffer, " - YOU ARE A GEM FLIPPING FREAK!");
					else if( fStreakTime <= 38.0f )	strcat(strBuffer, " - YOU ARE A CAN OPENER!");
					else if( fStreakTime <= 42.0f ) strcat(strBuffer, " - KAAAA MAEEE HAAAAA MAEE HAAAAAAAAAA!!!");
					else if( fStreakTime <= 44.0f ) strcat(strBuffer, " - YOU MUST BE CHEATING!");
					else							strcat(strBuffer, " - IS THIS ELI PLAYING?!? >=)");
					
					pGameMenu->buttonArray[19].SetTextString( strBuffer );
					pGameMenu->buttonArray[19].Enable(0);
				}

				Score.fStreakTime  = 0.0f;
				Score.iStreakLevel = 0;
				
				//Disable score buttons
				if(GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
					pGameMenu->buttonArray[8].Enable(0);

				// we need code that figures out what player is assigned to what combo
				Score.ResetCurrCombos();	//Reset our current combo count
				Score.ResetCurrLinkers();
			}

		}
		
		// **********************************
		// *** STREAK SOUND AND FX UPDATE ***
		// **********************************
//		if(GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
			Score.UpdateStreakFX( m_fDeltaTime );
		
		
		// *****************************
		// *** PARTICLE UPDATE TIMER ***
		// *****************************
		static f32 fParticleUpdateTime = 0;
		fParticleUpdateTime += m_fDeltaTime;
		
		if( fParticleUpdateTime >= 0.01f )
		{
			fParticleUpdateTime = 0.0f;
					
			//***Update our particle engine (its numbers)
			(!App.UserSettings.RenderMode)?	App.pDDraw->UpdateParticleEngine():
											App.pOpenGL->glUpdateParticleEngine();
		}
		
	} //END GAME MODE UPDATE

}


void GAME_DATA::OnF1Key()
{
	cSound.PlayPrevTrack();
	if(pGameMenu && pGameMenu->buttonArray && (GameStateFlags & GAME_STATE_GAME) )
	{
		LaunchTrackTitle( pGameMenu, pGameMenu->buttonArray + 21, 0 );
	}
}

void GAME_DATA::OnF2Key()
{
	cSound.PlayNextTrack();
	if(pGameMenu && pGameMenu->buttonArray && (GameStateFlags & GAME_STATE_GAME) )
	{
		LaunchTrackTitle( pGameMenu, pGameMenu->buttonArray + 21, 0 );
	}
}


void GAME_DATA::OnF11Key()
{
	if(pGameMenu && (GameStateFlags & GAME_STATE_GAME) )
	{
		if(bCamera)
		{
			bCamera = 0;
			pBoard->Camera.ResetPosition();
		}
		else
		{
			bCamera = 1;
		}
	}
}

void GAME_DATA::OnF12Key()
{	
	if(pGameMenu && (GameStateFlags & GAME_STATE_GAME) )
	{
		//DEBUG MODE (show the FPS counter)
		App.bDebug = !App.bDebug;

		pGameMenu->FPSButton( App.bDebug );
	}

}

void DisableAllButtons( void );
void GAME_DATA::OnEscapeKey()
{
	//If we are in the middle of a game quit out of it.
	if(!pGameMenu )
		return;

	//Check to see if we are in the menu
	if(GameStateFlags & GAME_STATE_MENU)
	{
		//Check to see if we are in the title screen.
		if(pGameMenu->GetMenuID() == MENU_ID_TITLESCREEN )
		{
			//Quit the game because we are already at the title screen
			(App.UserSettings.RenderMode)?
				SendMessage(App.pOpenGL->getHwnd(), WM_DESTROY, NULL, NULL):
				SendMessage(App.pDDraw->getHwnd(), WM_DESTROY, NULL, NULL);
		}
		else
		{
			DisableAllButtons();

			//We are not at the title screen so just execute the cancle button
			LaunchCancelButton(pGameMenu, NULL, 0);
		}
	}
	//Check to see if we are in the game
	else if(GameStateFlags & GAME_STATE_GAME)
	{
		//We must be in the game so see if we are currently playing
		if( !Score.bGameFinished )
		{
			pGameMenu->FinishGame( true );
		}
	}
}

/*
void GAME_DATA::UpdateStreakFX( )
{
	//( LEVEL 0 ) LevelUp when >= 8.0f, say nothing
	//( LEVEL 1 ) LevelUp when >= 4  seconds ) -> say "STREAK!"
	//( LEVEL 2 ) LevelUp when >= 8  seconds ) -> say "SUPER STREAK!"
	//( LEVEL 3 ) LevelUp when >= 16 seconds ) -> say "NEVER ENDING STREAK!"
	//( LEVEL 4 ) LevelUp when >= 24 seconds ) -> say "HOLY CRAP!"
	//( LEVEL 5 ) LevelUp when >= 32 seconds ) -> say "HOLY CRAP!"
	
	//*** ELI's SPECIAL CODE
	static float s_fBeatUpdateTime = 0.0f;
	static float s_fBeatRate	   = 0.1f;

	s_fBeatUpdateTime += m_fDeltaTime;
	
	if( s_fBeatUpdateTime >= s_fBeatRate )
	{
		//Reset our beatUpdateTime counter
		s_fBeatUpdateTime = 0.0f;
		
		//Test to see if we went up a streakLevel (which increases the beat rate and plays a sound)
		switch( iStreakLevel )
		{	
			//LEVEL 0 - 8
			case 0:	if(fStreakTime >= 4.0f)  { s_fBeatRate = 1.0f;  iStreakLevel++; } break;
			case 1:	if(fStreakTime >= 7.0f)  { s_fBeatRate = 0.7f;  cSound.m_cStaticSoundList[SFX_STREAK1].Play(); iStreakLevel++; } break;
			case 2:	if(fStreakTime >= 12.0f) { s_fBeatRate = 0.5f;  cSound.m_cStaticSoundList[SFX_STREAK2].Play(); iStreakLevel++; } break;
			case 3:	if(fStreakTime >= 18.0f) { s_fBeatRate = 0.2f;  cSound.m_cStaticSoundList[SFX_STREAK3].Play(); iStreakLevel++; } break;
			case 4:	if(fStreakTime >= 24.0f) { s_fBeatRate = 0.1f;	cSound.m_cStaticSoundList[SFX_STREAK4].Play(); iStreakLevel++; } break;
			case 5:	if(fStreakTime >= 28.0f) { s_fBeatRate = 0.08f; cSound.m_cStaticSoundList[SFX_STREAK5].Play(); iStreakLevel++; } break;
			case 6:	if(fStreakTime >= 32.0f) { s_fBeatRate = 0.05f; cSound.m_cStaticSoundList[SFX_STREAK6].Play(); iStreakLevel++; } break;
			case 7:	if(fStreakTime >= 40.0f) { s_fBeatRate = 0.02f; cSound.m_cStaticSoundList[SFX_STREAK7].Play(); iStreakLevel++; } break;
			case 8:	if(fStreakTime >= 50.0f) { s_fBeatRate = 0.01f; cSound.m_cStaticSoundList[SFX_STREAK7].Play(); 
																	cSound.m_cStaticSoundList[SFX_STREAK7].Play(); iStreakLevel++; } break;
			default:
				break;
		}
		
		//Create Sparks acording to the beat rate
		if(iStreakLevel > 0 )
			CreateStreakSparks();
		else if(fStreakTime <= 0.001f)
		{
			s_fBeatRate	   = 0.1f;
		}
	}

}
*/
void GAME_DATA::SetPlayerPosition(int whichPlayer)
{
	//Why do we do this? -eli (5-13-01)
	if(GameStateFlags & GAME_STATE_SINGLE_GAME)
		return;

	//This is grossest line of code i have EVER seen. At least we can minimise the suck to one line. -Eli
	Gem3D	*pPlayerOnGem	= &pBoard->gem[simplePlayers[whichPlayer].RowGem][simplePlayers[whichPlayer].ColumnGem];

	 simplePlayers[whichPlayer].mapX =  410 + (int)pPlayerOnGem->GetCenterPos()->x;
//	 (int)pBoard->gem[simplePlayers[whichPlayer].RowGem][simplePlayers[whichPlayer].ColumnGem].GetCenterPos()->x;
	 simplePlayers[whichPlayer].mapY =  240 - (int)pPlayerOnGem->GetCenterPos()->y - 17;
	 
	 switch( whichPlayer )
	 {
	 	case PLAYER_ONE:
			simplePlayers[whichPlayer].frontX = (int)pPlayerOnGem->GetScreenCenterPos()->x - 10;
			simplePlayers[whichPlayer].frontY = (int)pPlayerOnGem->GetScreenCenterPos()->y - 17;
			break;

	    case PLAYER_TWO:
			simplePlayers[whichPlayer].mapX += 8;
			simplePlayers[whichPlayer].frontX = (int)pPlayerOnGem->GetScreenCenterPos()->x - 10;
			simplePlayers[whichPlayer].frontY = (int)pPlayerOnGem->GetScreenCenterPos()->y - 17;
			break;

	    case PLAYER_THREE:
			simplePlayers[whichPlayer].mapY += 17;
			simplePlayers[whichPlayer].frontX = (int)pPlayerOnGem->GetScreenCenterPos()->x - 10;
			simplePlayers[whichPlayer].frontY = (int)pPlayerOnGem->GetScreenCenterPos()->y - 17;
			break;

	    case PLAYER_FOUR:
			simplePlayers[whichPlayer].mapY += 17;
			simplePlayers[whichPlayer].mapX += 8;
			simplePlayers[whichPlayer].frontX = (int)pPlayerOnGem->GetScreenCenterPos()->x - 10;
			simplePlayers[whichPlayer].frontY = (int)pPlayerOnGem->GetScreenCenterPos()->y - 17;
			break;
	} // end switch(whichPlayer) 
}