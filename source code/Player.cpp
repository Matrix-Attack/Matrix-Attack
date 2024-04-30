#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <stdio.h>
#include <gl/gl.h>
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
#include "camera3d.h"
#include "player.h"
#include "board.h"
#include "dinput.h"
#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "Bitmap.h"
#include "dsound.h"

#include "GLSurfDef.h"

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


extern APP App;
extern GAME_DATA GameData;
//extern DSound Sound;
extern CSound cSound;


Player::Player()
{
	pBackSurf		= NULL;
	pGemSurf		= NULL;
	playerID		= 0;
	cursorDirection	= 0;			// current direction of movement
	keyFlags		= NULL;			// determines which keys are being held down
	clusterCount	= 0;
	comboCount		= 0;
	linkerCount		= 0;
	numDiffCombos	= 0;			// number of different combos going on at the same time
	bCanMove		= 0;			// if true, the player can move the cursor
	bCanSwitch		= 0;			// if true, the player can switch two gems
	bCanFlip		= 0;			// if true, the player can flip the current gem
	bSwitching		= 0;			// if true, the player is switching two gems
	bFlipping		= 0;			// if true, the player is flipping a gem
	bRepeatOn		= 0;			// if true, the player is holding a movement key down
	bFlashOn		= 0;			// if true, display the gem's highlight color									// if false, display the gem's regular color
	fMoveTimer		= 0.0f;			// amount of time before the next movement can occur
	fRepeatStall	= 0;			// when a movement key is held down, this is the amount
	fRepeatTime		= 0;			// once repeat movement kicks in, this is the amount
	highlightTimer	= 0;			// amount of time remaining until current gem flashes
	flashSpeed		= 0;				// speed at which the gem flashes
	pGem			= NULL;				// pointer to the gem where the player is located
	pCamera			= NULL;

	firstMsgIndex = 0;
}

inline void	Player::BeginSwitch()
{
	switchdirection			= cursorDirection;
	pGem->bStartingAChain	= 1;
	bSwitching				= 1;
	pGem->PrepareSwitch(GameData.playerId, cursorDirection, 8.0f);

	if((GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI) )
	{
		PLAYER_SWITCHING plSwitch;

		plSwitch.dpMsgData	= DP_MSG_SEND_SWITCH;
		plSwitch.RowGem		= GameData.simplePlayers[GameData.playerId].RowGem;
		plSwitch.ColumnGem	= GameData.simplePlayers[GameData.playerId].ColumnGem;
		plSwitch.direction  = switchdirection;
		plSwitch.id			= GameData.playerId;

		App.pDP->lpDP->Send(App.pDP->dpid, App.pDP->dpidRoom, DPSEND_GUARANTEED, &plSwitch, sizeof(PLAYER_SWITCHING) );

	}
	
}



inline void Player::BeginFlip()
{ 
	bFlipping				= 1;
	pGem->bStartingAChain	= 1;
	pGem->PrepareFlip(GameData.playerId, 10.0);

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		PLAYER_FLIP plFlip;

		plFlip.dpMsgData	= DP_MSG_SEND_FLIP;
		plFlip.RowGem		= GameData.simplePlayers[GameData.playerId].RowGem;
		plFlip.ColumnGem	= GameData.simplePlayers[GameData.playerId].ColumnGem;
		plFlip.id			= GameData.playerId;

		App.pDP->lpDP->Send(App.pDP->dpid, App.pDP->dpidRoom, DPSEND_GUARANTEED, &plFlip, sizeof(PLAYER_FLIP) );
	}
}


inline int Player::GetCursorDirection()
{		
	if(App.pdistruct->JoyDescHead.iNumDevices > 0)
	{								
		return cursorDirection	=	((App.pdistruct->keys[DIK_W] & 0x80) || (App.pdistruct->keys[DIK_UP]	& 0x80) || (App.pdistruct->joystick.lY < 0) )?	PLAYER_MOVE_UP		:
									((App.pdistruct->keys[DIK_S] & 0x80) || (App.pdistruct->keys[DIK_DOWN]	& 0x80)	|| (App.pdistruct->joystick.lY > 0) )?	PLAYER_MOVE_DOWN	:
									((App.pdistruct->keys[DIK_A] & 0x80) || (App.pdistruct->keys[DIK_LEFT]	& 0x80)	|| (App.pdistruct->joystick.lX < 0)	)?	PLAYER_MOVE_LEFT	:
									((App.pdistruct->keys[DIK_D] & 0x80) || (App.pdistruct->keys[DIK_RIGHT] & 0x80)	|| (App.pdistruct->joystick.lX > 0)	)?	PLAYER_MOVE_RIGHT	:
									(App.pdistruct->keys[DIK_UP] & 0x80)	?	PLAYER_MOVE_IN		:
									(App.pdistruct->keys[DIK_DOWN] & 0x80)	?	PLAYER_MOVE_OUT		:
									PLAYER_MOVE_NOWHERE;
	}
	else
	{
		return cursorDirection	=	((App.pdistruct->keys[DIK_W] & 0x80) || (App.pdistruct->keys[DIK_UP]	& 0x80) )     ?	PLAYER_MOVE_UP		:
									((App.pdistruct->keys[DIK_S] & 0x80) || (App.pdistruct->keys[DIK_DOWN]	& 0x80) )	    ?	PLAYER_MOVE_DOWN	:
									((App.pdistruct->keys[DIK_A] & 0x80) || (App.pdistruct->keys[DIK_LEFT]	& 0x80) )		?	PLAYER_MOVE_LEFT	:
									((App.pdistruct->keys[DIK_D] & 0x80) || (App.pdistruct->keys[DIK_RIGHT] & 0x80) )		?	PLAYER_MOVE_RIGHT	:
									(App.pdistruct->keys[DIK_UP] & 0x80)	?	PLAYER_MOVE_IN		:
									(App.pdistruct->keys[DIK_DOWN] & 0x80)	?	PLAYER_MOVE_OUT		:
									PLAYER_MOVE_NOWHERE;
	}
}




// determines if the player can move the cursor
// from one gem to a neighboring gem
//
// the player can move the cursor if:
//		1. a movement key must be pressed
//		2. if repeat movement is active, has enough time
//				elapsed since the last movement?
//		3. a gem exists in the direction of movement
//		4. the player is NOT switching
//		5. the player is NOT flipping
inline int Player::CanPlayerMove( float fDeltaTime )
{
	if(	(cursorDirection == PLAYER_MOVE_NOWHERE)	||

		((bRepeatOn) && (fMoveTimer > 0.0f))		||

		(bSwitching)								||

		(bFlipping)	)
	{
		return bCanMove = 0;
	}

	if(!( (cursorDirection == PLAYER_MOVE_IN) || (cursorDirection == PLAYER_MOVE_OUT) )
		&& (!(pGem->GetNeighbor(cursorDirection))) )
	{
		return bCanMove = 0;
	}

	return bCanMove = 1;
}


// determines if the player can switch two gems
//
// the player can switch two gems together if:
//		1. the enter key must be pressed
//		2. repeat movement must not be active
//		3. the player is NOT switching
//		4. the player is NOT flipping
//		5. both gems must be perfectly still

inline int Player::CanPlayerSwitch()
{
	if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE) &&
		!GameData.pBoard->numMoves)
	{
		return 0;
	}
	else
	{
		if(App.pdistruct->JoyDescHead.iNumDevices > 0)
		{	
			if( !(App.pdistruct->keys[DIK_RETURN] & 0x80)		||
				!(App.pdistruct->joystick.rgbButtons[0] & 0x80) ||
				(bRepeatOn)										||
				(bSwitching)									||
				(bFlipping)										||
				!(pGem->IsSwitchable() ) 						||
				!(pGem->GetNeighbor(cursorDirection)->IsSwitchable() ) )
				{
					return 0;
				}
		}
		else
		{
			if( !(App.pdistruct->keys[DIK_RETURN] & 0x80)	||
			(bRepeatOn)										||
			(bSwitching)									||
			(bFlipping)										||
			!(pGem->IsSwitchable() ) 						||
			!(pGem->GetNeighbor(cursorDirection)->IsSwitchable() ) )
			{
				return 0;
			}
		}

		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			// crappy band-aid for switching
			for(int i = 0; i < GameData.numplayers; i++)
			{
				if(GameData.simplePlayers[i].id == GameData.playerId)
					continue;

				// check the actual gem
				if(GameData.simplePlayers[i].RowGem			== pGem->row 
				   && GameData.simplePlayers[i].ColumnGem	== pGem->column)
				   return 0;

				// check the gem's neighbors
				for(int j = 0; j < 6; j++)
				{
					if(	(pGem->GetNeighbor(j) )													&&
						(GameData.simplePlayers[i].RowGem		== pGem->GetNeighbor(j)->row)	&&
						(GameData.simplePlayers[i].ColumnGem	== pGem->GetNeighbor(j)->column) )
					   return 0;
				}
				
				Gem3D	*pSwitchGem	= pGem->GetNeighbor(cursorDirection);

				if(pSwitchGem)
				{
					// check the actual switch gem
					if(GameData.simplePlayers[i].RowGem			== pSwitchGem->row 
					   && GameData.simplePlayers[i].ColumnGem	== pSwitchGem->column)
					   return 0;

					// check the switch gem's neighbors
					for(int j = 0; j < 6; j++)
					{
						if(	(pSwitchGem->GetNeighbor(j) )												&&
							(GameData.simplePlayers[i].RowGem		== pSwitchGem->GetNeighbor(j)->row)	&&
							(GameData.simplePlayers[i].ColumnGem	== pSwitchGem->GetNeighbor(j)->column) )
						   return 0;
					}
				}
				
			}
		}


	}

	return 1;
}



// determines if the player can flip a gem
//
// the player can flip a gem if:
//		1. the space bar must be pressed
//		2. the player is NOT switching
//		3. the player is NOT (already) flipping
//
// note: this function should only be called once
//			it's known that the player isn't moving
inline int Player::CanPlayerFlip()
{

	if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE) &&
		!GameData.pBoard->numMoves)
		return 0;


	if(App.pdistruct->JoyDescHead.iNumDevices > 0)
	{	
		if( !(App.pdistruct->keys[DIK_SPACE] & 0x80)			||
			!(App.pdistruct->joystick.rgbButtons[1] & 0x80)		||
			(bSwitching)										||
			(bFlipping)											||
			!(pGem->IsFlippable() ) )
			return 0;
	}
	else
	{
		if( !(App.pdistruct->keys[DIK_SPACE] & 0x80)			||
			(bSwitching)										||
			(bFlipping)											||
			!(pGem->IsFlippable() ) )
			return 0;
	}

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		for(int i = 0; i < GameData.numplayers; i++)
		{
			if(GameData.simplePlayers[i].id == GameData.playerId)
				continue;

			if(GameData.simplePlayers[i].RowGem == pGem->row 
			   && GameData.simplePlayers[i].ColumnGem == pGem->column)
			   return 0;
		}
	}

	return 1;
}


inline void Player::ResetMoveTimer()
{
	fMoveTimer	= ((bRepeatOn) ? fRepeatTime : fRepeatStall);
}




void Player::MoveCursor()
{
	// move to the next gem
	if(!( (cursorDirection == PLAYER_MOVE_IN) || (cursorDirection == PLAYER_MOVE_OUT) ) )
	{
		pGem	= pGem->GetNeighbor(cursorDirection);

		switch(cursorDirection)
		{
			case PLAYER_MOVE_UP:
				GameData.simplePlayers[GameData.playerId].RowGem--;
				break;
	
			case PLAYER_MOVE_DOWN:
				GameData.simplePlayers[GameData.playerId].RowGem++;
				break;
	
			case PLAYER_MOVE_LEFT:
				GameData.simplePlayers[GameData.playerId].ColumnGem--;
				break;
	
			case PLAYER_MOVE_RIGHT:
				GameData.simplePlayers[GameData.playerId].ColumnGem++;
				break;
		}

		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			PLAYER_UPDATE_POSITION PlayerPos;

			PlayerPos.dpMsgData = DP_MSG_SEND_NEW_POSITION;
			PlayerPos.pId = GameData.playerId;
			PlayerPos.ColumnGem = GameData.simplePlayers[GameData.playerId].ColumnGem;
			PlayerPos.RowGem = GameData.simplePlayers[GameData.playerId].RowGem;

			App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&PlayerPos, sizeof(PLAYER_UPDATE_POSITION));
		}

		GameData.SetPlayerPosition(GameData.playerId);
	}
}


void Player::HandleKeyInput( float fDeltaTime )
{
//	keyFlags	= GameData.keysDown;

	//******MAX's DEBUG INFO*****************************
/*		char a[100];


	//*** DEBUG CODE ***
	if(0)//(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI) && GameData.playerId == GameData.multiPlayerHost)
	{
		
		sprintf(a, "flip %d switch %d is flipable %d is switchable %d", GameData.simplePlayers[1].bFlipping, 
						 GameData.simplePlayers[1].bSwitching,
						 GameData.pBoard->gem[GameData.simplePlayers[1].RowGem][GameData.simplePlayers[1].ColumnGem].IsFlippable(),
						 GameData.pBoard->gem[GameData.simplePlayers[1].RowGem][GameData.simplePlayers[1].ColumnGem].IsSwitchable()
						 );
						 
	//	sprintf(a, "gemR %d gemC %d", GameData.simplePlayers[GameData.playerId].RowGem,
	//								  GameData.simplePlayers[GameData.playerId].ColumnGem);
//		if(!App.UserSettings.RenderMode)
//			SetWindowText(App.pDDraw->getHwnd(), a);
//		else
//			SetWindowText(App.pOpenGL->getHwnd(), a);
	}
	else
	{
		sprintf(a, "1p pflip %d pswitch %d gFlipping %d gSwitching %d gFlipable %d gSwitchable %d asId %d clearing %d",
						 bFlipping, 
						 bSwitching,
						 this->pGem->IsFlipping(),
						 this->pGem->IsSwitching(),
						 this->pGem->IsFlippable(),
						 this->pGem->IsSwitchable(),
						 this->pGem->associatedPlayerID,
						 this->pGem->IsClearing()
						 );
	//	if(!App.UserSettings.RenderMode)
	//		SetWindowText(App.pDDraw->getHwnd(), a);
//		else
	//		SetWindowText(App.pOpenGL->getHwnd(), a);
	}
	//****************************************************
*/


	//This is some CRAK-ASS code.
	if((bRepeatOn) && (fMoveTimer > 0))
		fMoveTimer -= fDeltaTime;


	// determine if the player is trying to move
	// Query DINPUT for key movement
	GetCursorDirection();


	// if the player is trying to move, determine
	// if that movement is possible
	if( CanPlayerMove( 0 ) )
	{

		// if the player can move, determine if the
		// player is trying to switch two gems
		if(CanPlayerSwitch())
		{
				pCamera->SetMovementDirection(cursorDirection);

				cSound.m_cStaticSoundList[SFX_SWITCH].Play();
				BeginSwitch();
		}

		// if the player isn't switching gems,
		// just move the cursor without switching
		else if(!(App.pdistruct->keys[DIK_RETURN] & 0x80) )
		{
			pCamera->SetMovementDirection(cursorDirection);

			MoveCursor();
			ResetMoveTimer();
		}
	}
	
	// the player isn't trying to move, but check if
	// they're trying to flip a gem
	else if(CanPlayerFlip())
	{
     	BeginFlip();

		cSound.m_cStaticSoundList[SFX_FLIP].Play();
	}

	bRepeatOn	= (cursorDirection != PLAYER_MOVE_NOWHERE);
}



void Player::DisplayCursor()
{
	int		i;
	RECT	clipRect	= {0, 31, 32, 66},
			viewPort	= {135, 80, 301, 400};
			
	if(pGem == NULL)
	{
		return;
	}

	if(!App.UserSettings.RenderMode )
	{
		int		x	= pGem->GetScreenCenterPos()->x - 16,
				y	= pGem->GetScreenCenterPos()->y - 17;
		
		ClipToViewPort(clipRect, viewPort, x, y);
		pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[9], &clipRect, DDBLTFAST_SRCCOLORKEY);

		//*** DRAW SWITCHER
		if(GameData.bDrawSwitcher)
		{
			SetRect(&clipRect, 32, 31, 64, 66);

			for(i=0; i < 4; i++)
			{
				if( pGem->pNeighbor[i] )
				{
					x	= pGem->pNeighbor[i]->GetScreenCenterPos()->x - 16,
					y	= pGem->pNeighbor[i]->GetScreenCenterPos()->y - 17;

					ClipToViewPort(clipRect, viewPort, x, y);
					pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[9], &clipRect, DDBLTFAST_SRCCOLORKEY);
					
				}
			}
		}
	}
	else
	{

		Vector3D *pV = pGem->GetCenterPos();
		
		RECT rcSRC;
			
		//This will flip our clipping coords for opengl 
		// (so that 0,0 is the upper left hand corner of the surface/texture)
		rcSRC.left		= clipRect.left;
		rcSRC.top		= 256 - clipRect.bottom;
		rcSRC.right		= clipRect.right;
		rcSRC.bottom	= rcSRC.top + (clipRect.bottom - clipRect.top);

		glDisable(GL_BLEND);
		glEnable (GL_ALPHA_TEST);
		glAlphaFunc(GL_NOTEQUAL, 0.0f);
		glEnable (GL_TEXTURE_2D);							// enable texture mapping 
		
		//*** DRAW CURSOR
		glBindTexture (GL_TEXTURE_2D, GL_SURF_GAME_CURSOR_TEXID);// bind to our texture, has id of 13 
		glBegin (GL_QUADS);
			glTexCoord2f (0.0f, 0.0f);						// lower left corner of image
			glVertex3d (pV->x -16, pV->y +16, pV->z -640);
			glTexCoord2f (0.0f, 1.0f);						// upper left corner of image
			glVertex3d (pV->x -16, pV->y -16, pV->z -640);
			glTexCoord2f (1.0f, 1.0f);						// upper right corner of image
			glVertex3d (pV->x +16, pV->y -16, pV->z -640);
			glTexCoord2f (1.0f, 0.0f);						// lower right corner of image
			glVertex3d (pV->x +16, pV->y +16, pV->z -640);
		glEnd ();
		
		//*** DRAW SWITCHER
		if(GameData.bDrawSwitcher)
		{
			//Go through all the neighbors and highlight the gems we can switch with
			glBindTexture (GL_TEXTURE_2D, GL_SURF_GAME_SWITCHER_TEXID);// bind to our texture, has id of 13 
			for(i=0; i < 4; i++)
			{
				if( pGem->pNeighbor[i] )
				{
					pV = pGem->pNeighbor[i]->GetCenterPos();
					
					glBegin(GL_QUADS);
						glTexCoord2f (0.0f, 0.0f);						// lower left corner of image
						glVertex3d (pV->x -16, pV->y +16, pV->z -640);
						glTexCoord2f (0.0f, 1.0f);						// upper left corner of image
						glVertex3d (pV->x -16, pV->y -16, pV->z -640);
						glTexCoord2f (1.0f, 1.0f);						// upper right corner of image
						glVertex3d (pV->x +16, pV->y -16, pV->z -640);
						glTexCoord2f (1.0f, 0.0f);						// lower right corner of image
						glVertex3d (pV->x +16, pV->y +16, pV->z -640);
					glEnd();
				}
			}
		}
		glDisable(GL_TEXTURE_2D); // disable texture mapping 
		glDisable(GL_ALPHA_TEST);
	}
}



void Player::Initialize(Gem3D *startingPos)
{
	memset(this, 0, sizeof(Player));

	flashSpeed		= 4;
	
	fRepeatStall	= 0.20f;
	fRepeatTime		= 0.10f;

	pGem			= startingPos;
}




void Player::Update()
{
	// if the player was switching gems during the previous frame,
	// determine if the switch has completed for this frame.
	//
	// if so, perform the necessary updates to complete the switch
	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		if(IsDoneSwitching())
			CompleteSwitch();

		else if(IsDoneFlipping())
			CompleteFlip();
	}
	else
	{
		if(IsDoneSwitching())
			CompleteSwitch();

		else if(IsDoneFlipping())
			CompleteFlip();
	}

//	HandleKeyInput();
}


void Player::CompleteFlip()
{
	if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE) &&
		GameData.pBoard->numMoves > 0)
	{
		GameData.pBoard->numMoves--;
		GameData.pGameMenu->buttonArray[8].screenY -= 20;
	}
	bFlipping = 0;
}



void Player::CompleteSwitch()	
{ 
		bSwitching = 0;
//		pGem = pGem->GetSwitchPartner(); 

		switch(switchdirection)
		{
			case PLAYER_MOVE_UP:
				GameData.simplePlayers[GameData.playerId].RowGem--;
				break;
		
			case PLAYER_MOVE_DOWN:
				GameData.simplePlayers[GameData.playerId].RowGem++;
				break;
		
			case PLAYER_MOVE_LEFT:
				GameData.simplePlayers[GameData.playerId].ColumnGem--;
				break;
		
			case PLAYER_MOVE_RIGHT:
				GameData.simplePlayers[GameData.playerId].ColumnGem++;
				break;
		}

	if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE) &&
		GameData.pBoard->numMoves > 0)
	{
		GameData.pBoard->numMoves--;
		GameData.pGameMenu->buttonArray[8].screenY -= 20;
	}
	
	else if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		PLAYER_UPDATE_POSITION PlayerPos;

		PlayerPos.dpMsgData = DP_MSG_SEND_NEW_POSITION;
		PlayerPos.pId		= GameData.playerId;
		PlayerPos.ColumnGem = GameData.simplePlayers[GameData.playerId].ColumnGem;
		PlayerPos.RowGem	= GameData.simplePlayers[GameData.playerId].RowGem;

		App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&PlayerPos, sizeof(PLAYER_UPDATE_POSITION));
	}
	
	//If your in multiplayer mode this crappy function right here makes sure
	//your position is never close to another player's position.
	GameData.SetPlayerPosition(GameData.playerId);

}


