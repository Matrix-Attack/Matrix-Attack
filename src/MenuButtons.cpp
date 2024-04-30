#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "dinput.h"
#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "camera3d.h"
#include "player.h"
#include "board.h"

#include "text.h"
#include "GLSurfDef.h"


extern APP			App;
extern GAME_DATA	GameData;

void strToupper( char* string );

void GLSetup3DMode();

void	ChatInput(char *Buffer, unsigned int maxChar);
int		DisplayIP(Game_Menu *pGameMenu, Menu_Button *pButton);
void	LoadPlayerNames(Game_Menu *pGameMenu);
BOOL FAR PASCAL EnumPlayersCallback (	DPID dpId,
										DWORD dwPlayerType,
										LPCDPNAME lpName,
										DWORD dwFlags,
										LPVOID lpContext
									  );

void SetSlotClipping(Menu_Button	*buttonArray)
{
	static RECT rcFoxSelect		= {0,0,64,64};
	static RECT rcFoxDeselect	= {64,0,128,64};
	static RECT rcVineSelect	= {0,64,64,128};
	static RECT rcVineDeselect	= {64,64,128,128};
	static RECT rcNinjaSelect	= {0,128,64,192};
	static RECT rcNinjaDeselect	= {64,128,128,192};
	static RECT rcBundySelect	= {0,192,64,256};
	static RECT rcBundyDeselect	= {64,192,128,256};

	for(int i = 5; i < 10; i++)
	{
		switch(buttonArray[i].GetCharacterIndex())
		{
		case (0):
			{
				buttonArray[i].SetClipSelect(rcFoxSelect);
				buttonArray[i].SetClipDeselect(rcFoxDeselect);
			}
			break;
		case (1):
			{
				buttonArray[i].SetClipSelect(rcVineSelect);
				buttonArray[i].SetClipDeselect(rcVineDeselect);
			}
			break;
		case (2):
			{
				buttonArray[i].SetClipSelect(rcNinjaSelect);
				buttonArray[i].SetClipDeselect(rcNinjaDeselect);
			}
			break;
		case (3):
			{
				buttonArray[i].SetClipSelect(rcBundySelect);
				buttonArray[i].SetClipDeselect(rcBundyDeselect);
			}
			break;
		}
	}
}

void LoadPlayerNames(Game_Menu *pGameMenu)
{
	//why the hell not!
	GameData.DBase.LoadDatabase( DATABASE_FILENAME );
	
	// i=5; i <=9
	for (u32 i=0; i <= 5; i++)
	{
		if( !GameData.DBase.GetProfile( i )->IsEmpty() )
		{
			//copy our profile name into the button slot
			strcpy( pGameMenu->buttonArray[i+5].GetTextString(), 
					GameData.DBase.GetProfile( i )->GetName() );

			strToupper( pGameMenu->buttonArray[i+5].GetTextString() );
			
			pGameMenu->buttonArray[i+5].SetCharacterIndex( 
					GameData.DBase.GetProfile( i )->GetCharacterID() );
		}
		else
		{
			strcpy(pGameMenu->buttonArray[i+5].GetTextString(), "EMPTY" );
			pGameMenu->buttonArray[i+5].SetCharacterIndex( 0 );
		}

//		fscanf(fp,"%s", pGameMenu->buttonArray[i].GetTextString());
//		fscanf(fp,"%d",&temp);
//		pGameMenu->buttonArray[i].SetCharacterIndex(temp);
	}
/*
	FILE	*fp = NULL;

	fp = fopen("spnames.txt", "rt");

	if(fp)
	{
		for (int i=5; i <= 9; i++)
		{
			int temp;

			fscanf(fp,"%s",pGameMenu->buttonArray[i].GetTextString());
			fscanf(fp,"%d",&temp);
			pGameMenu->buttonArray[i].SetCharacterIndex(temp);
		}
	fclose(fp);
	}
*/
}

void SavePlayerNames(Menu_Button *pButton)
{
	GameData.DBase.SaveDatabase( DATABASE_FILENAME );
/*
	FILE	*fp;
	
	fp = fopen("spnames.txt", "wt");
	
	if(fp)
	{
		for (int i=5; i <= 9; i++)
		{
			fprintf(fp,"%s\n",pButton[i].GetTextString());
			fprintf(fp,"%d\n",pButton[i].GetCharacterIndex());
		}
	}

	fclose(fp);
*/
}

int CheckForEmptySlot(Menu_Button	*buttonArray)
{
	CProfile* pProfile;
	if(buttonArray)
	{
		//used to be i=5; i<=9
		for(int i=0; i<=5; i++)
		{
			pProfile = GameData.DBase.GetProfile( i );
			if( pProfile && pProfile->IsEmpty() )
					return i;
		}
	}
	return 0;
}

void Menu_Button::Enable(float wait)
{
	buttonFlags	&=	~BUTTON_FLAG_ACTIVE_DISABLED;
	buttonFlags	|=  BUTTON_FLAG_ENABLED;
	
	enableDelay	= (wait * 0.01f);
	fTimeWaiting= 0;

	bUpdate		= true;
}




void Menu_Button::Disable(float wait)
{
	buttonFlags		&= ~BUTTON_FLAG_ENABLED;

	disableDelay	= (wait * 0.01f);
	fTimeWaiting	= 0;
}




void Menu_Button::Select(Game_Menu	*pGameMenu,
						Menu_Button	*pButton,
						int			direction)
{
	buttonFlags |=  BUTTON_FLAG_SELECTED;

	if(pSelect)
		pSelect(pGameMenu, pButton, direction);
}




void Menu_Button::Deselect(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	buttonFlags &= ~BUTTON_FLAG_SELECTED;

	if(pDeselect)
		pDeselect(pGameMenu, pButton, direction);
}




void Menu_Button::ClipAgainstScreenEdges()
{
	// modify clip rect
	int	buttonWidth	= clipRect.right - clipRect.left,
		buttonHeight	= clipRect.bottom - clipRect.top;

	if(  screenPos.x < 0 )
	{
		clipRect.left	-= screenPos.x; // amount of overlap
		screenPos.x		= 0;
	}
	if( (screenPos.x + buttonWidth) > 639)
		clipRect.right	-= (screenPos.x + buttonWidth) - 640; // amount of overlap

	if(  screenPos.y < 0 )
	{
		clipRect.top	-= screenPos.y; // amount of overlap
		screenPos.y		= 0;
	}
	if( (screenPos.y + buttonHeight) > 479)
		clipRect.bottom	-= (screenPos.y + buttonHeight) - 480; // amount of overlap
}





void Menu_Button::Update( float fDeltaTime )
{
	fTimeWaiting += fDeltaTime;

	if(IsEnabled() && (fTimeWaiting >= enableDelay) )
	{
		ActiveEnable();
	}
	else if(IsDisabled() && (fTimeWaiting >= disableDelay) )
	{
		ActiveDisable();
	}
	
	UpdateVeloc();
	
	UpdatePos( fDeltaTime );
	

	// get clip rect
	clipRect	= IsSelected() ? clipSelect : clipDeselect;
}





void Menu_Button::UpdatePos( float fDeltaTime )
{
	// update position
	screenPos.x	= (int)(screenX += velocX * fDeltaTime);
	screenPos.y	= (int)(screenY += velocY * fDeltaTime);

	if(IsActiveEnabled())
	{
		if(	(screenPos.x >= enabledPos.x - 1) &&
			(screenPos.x <= enabledPos.x + 1) )
			screenPos.x	= enabledPos.x;
	}

	if(IsActiveDisabled())
	{
		if(	(screenPos.x >= disabledPos.x - 1) &&
			(screenPos.x <= disabledPos.x + 1) )
		{
			screenPos.x	= disabledPos.x;
//			bUpdate = false;
		}
	}
}




void Menu_Button::UpdateVeloc()
{
	
	// update velocity
	if( IsActiveEnabled() )
	{
		velocX		= (enabledPos.x - screenX) * 7;
		velocY		= (enabledPos.y - screenY) * 7;
	}
	else if( IsActiveDisabled() )
	{
		velocX		= (disabledPos.x - screenX) * 7;
		velocY		= (disabledPos.y - screenY) * 7;
	}

}




void SelectLoadCharacterButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	int	emptyslot=0;

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		pGameMenu->UnsetBlinkingCursor();

		if(direction == CURSOR_MOVE_UP || direction == CURSOR_MOVE_DOWN)
		{
			SetSlotClipping(buttonArray);
			for(int i = 5; i < 10; i++)
				buttonArray[i].Enable(((float)i - 5) * 5);
		}

		for(int i = 5; i < 10; i++)
		{
			buttonArray[i].SetNeighbor(BUTTON_LEFT, pButton);
			buttonArray[i].SetNeighbor(BUTTON_PARENT, pButton);
		}
	}
}




void DeselectLoadCharacterButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		if(direction == CURSOR_MOVE_UP)
		{
			for(int i = 5; i < 10; i++)
				buttonArray[i].Disable(((float)i - 5) * 5);
		}
		else if(direction == CURSOR_MOVE_RIGHT)
			pGameMenu->SetBlinkingCursor();
	}
}




void SelectKillCharacterButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		pGameMenu->UnsetBlinkingCursor();

		if(direction == CURSOR_MOVE_UP)
		{
			// start at delay = 2, 4, 6, 8, 10
			for(int i = 5; i < 10; i++)
				buttonArray[i].Enable(((float)i - 4) * 5);
		}

		for(int i = 5; i < 10; i++)
		{
			buttonArray[i].SetNeighbor(BUTTON_LEFT, pButton);
			buttonArray[i].SetNeighbor(BUTTON_PARENT, pButton);
		}
	}
}




void DeselectKillCharacterButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		if(direction == CURSOR_MOVE_DOWN)
		{
			for(int i = 5; i < 10; i++)
				buttonArray[i].Disable(((float)i - 5) * 2);
		}
		else if(direction == CURSOR_MOVE_RIGHT)
			pGameMenu->SetBlinkingCursor();
	}
}




void SelectKeyConfigButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	pGameMenu->UnsetBlinkingCursor();

	buttonArray[10].Enable( (direction == CURSOR_MOVE_DOWN) ? 10.0f : 0.0f );
}




void DeselectKeyConfigButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(direction == CURSOR_MOVE_RIGHT)
		pGameMenu->SetBlinkingCursor();

	else //if(direction != CURSOR_MOVE_RIGHT)
	{
		buttonArray[10].Disable(0);
	}
}




void UpdateLoadCharacterIcon(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	POINT		screenPos	= pButton->GetScreenPos();
	POINT		textOffset	= pButton->GetTextOffset();
	if(((screenPos.x + textOffset.x) < 640) &&
	   ((screenPos.y + textOffset.y) < 480) )
		if(!App.UserSettings.RenderMode)
		{
			
			LPDD_SURF	pBackSurf	= App.pDDraw->getBackBuffer();
			App.pDDraw->DDrawText(screenPos.x + textOffset.x,
								  screenPos.y + textOffset.y,
								  TF_FONT_LARGE | TF_SPACING_3,
								  pButton->GetTextString()) ;
		}
		else//OPENGL MODE
		{
			App.pOpenGL->GLDrawText(screenPos.x + textOffset.x, screenPos.y + textOffset.y,
									TF_FONT_LARGE, pButton->GetTextString() );
		}
	
}




void DeselectLoadCharacterIcon(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(direction == CURSOR_MOVE_LEFT)
	{
		Menu_Button	*parentButton	= pButton->GetNeighbor(BUTTON_PARENT);

		parentButton->SetNeighbor(BUTTON_RIGHT, pButton);
	}
}




void SelectGamePadIcon(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	for(int i = 11; i <= 26; i++)
		buttonArray[i].Enable(0);
}




void DeselectGamePadButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(direction == CURSOR_MOVE_LEFT)
	{
		buttonArray[11].Disable(0);			// UP
		buttonArray[19].Disable(0);			// UP LIGHT BEAM

		buttonArray[12].Disable(0);			// DOWN
		buttonArray[20].Disable(0);			// DOWN LIGHT BEAM

		buttonArray[13].Disable(0);			// LEFT
		buttonArray[21].Disable(0);			// LEFT LIGHT BEAM

		buttonArray[14].Disable(0);			// RIGHT
		buttonArray[22].Disable(0);			// RIGHT LIGHT BEAM

		buttonArray[15].Disable(0);			// SMACK
		buttonArray[23].Disable(0);			// SMACK LIGHT BEAM

		buttonArray[16].Disable(0);			// FLIP
		buttonArray[24].Disable(0);			// FLIP LIGHT BEAM

		buttonArray[17].Disable(0);			// SWITCH
		buttonArray[25].Disable(0);			// SWITCH LIGHT BEAM

		buttonArray[18].Disable(0);			// PAUSE
		buttonArray[26].Disable(0);			// PAUSE LIGHT BEAM
	}
}

void UpdateLightBeamButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	static int	timer			= 0,
				frameNo			= 0;

	int			width			= pButton->GetWidth();

	Menu_Button	*pParent		= pButton->GetNeighbor(BUTTON_PARENT);

	if(pParent->IsSelected())
	{
		if(!(++timer &= 3))
			++frameNo &= 3;

		RECT	clipRect		= pButton->GetSelectRect();

		clipRect.left			+= frameNo * width;
		clipRect.right			+= frameNo * width;

		pButton->SetClipRect(clipRect);
	}
}


//*********TITLE SCREEN SPECIFIC FUNCTIONS*********
void LaunchSinglePlayerLoginButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	LoadPlayerNames(pGameMenu);

	pGameMenu->buttonArray[27].Disable(6);
//	pGameMenu->buttonArray[28].Disable(4);	// Multi player button
	pGameMenu->buttonArray[29].Disable(2);

	pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
}
void LaunchMultiPlayerLoginButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	LoadPlayerNames(pGameMenu);

	pGameMenu->buttonArray[27].Disable(6);
//	pGameMenu->buttonArray[28].Disable(4);	// Multi player button
	pGameMenu->buttonArray[29].Disable(2);


	pGameMenu->SwitchMenu(MENU_ID_MULTI_IP_LOGIN);
}
void LaunchQuitGameButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	PostQuitMessage(0);
}



//*********LOGIN SCREEN SPECIFIC FUNCTIONS*********
void LaunchNewPlayerButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	Menu_Cursor	*pCursor	= pGameMenu->GetCursor();
	Menu_Button	*pNeighbor	= pButton->GetNeighbor(BUTTON_RIGHT);

	// set the blinking cursor
	pGameMenu->SetBlinkingCursor();

	// have the cursor point to the neighbor on the right
	pCursor->SetButton(pNeighbor);

	// invoke that neighbor's select function
	
	pNeighbor->Select(pGameMenu, pButton, CURSOR_MOVE_RIGHT);
	
	// set the cursor in motion, and,
	// disable cursor movement until all keys are released
	pCursor->SetCursorFlag(CURSOR_FLAG_MOVING);
	pCursor->UnsetCursorFlag(CURSOR_FLAG_CAN_MOVE);
}

void LaunchPlayerSlotButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	
//	pButton->Deselect(pGameMenu, pButton, 0);

	buttonArray[5].Disable(8);
	buttonArray[6].Disable(6);
	buttonArray[7].Disable(4);
	buttonArray[8].Disable(2);
	buttonArray[9].Disable(0);


	//***  OK DAMIT --> WE HAVE TO FIGURE OUT THIS HARD CODED CRAP.... RIGHT NOW!!!!
	// this menu system is a sick piece of work...
	//*** Get the Character's Button Index
//	GameData.iCurrentCharacterButton = pButton->GetButtonID();
	//*** Get the Characters's Profile Slot Index
//	GameData.iCharacterProfileSlot = GameData.iCurrentCharacterButton - 44;
	
	pButton->Deselect(pGameMenu, pButton, 0);
	
//(pGameMenu->cursor.GetpButton())->pNeighbor[BUTTON_PARENT]->GetButtonID()
	switch( pButton->pNeighbor[BUTTON_PARENT]->GetButtonID() )
	{
		case 0: //***NEW PLAYER***
			
			//*** Get the Character's Button Index
			GameData.iCurrentCharacterButton = pButton->GetButtonID();
			
			//*** Get the Characters's Profile Slot Index
			GameData.iCharacterProfileSlot = GameData.iCurrentCharacterButton - 44;
			
			
			
			
			if(App.pDP)
				App.pDP->bNewPlayer = TRUE;

			//***Check to see if we have room to create another character
			if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
			{				
				if( GameData.DBase.GetNumProfiles() >= 5 )
				{
					//***FIX*** LET THE USER KnoW that we are FULL!
					pButton->Deselect(pGameMenu, pButton, 0);
					pGameMenu->InitializeCursor(0);
					return;
				}
			}
			GameData.CharPic = GameData.iCharacterProfileSlot;
			
			GameData.pGameMenu->buttonArray[ GameData.iCurrentCharacterButton ].
				SetCharacterIndex( GameData.iCharacterProfileSlot );

			pButton->SetDisabledPos(374, 60);
			pButton->Disable(0);

			//***Disable the player icons
			buttonArray[44].Disable(6);
			buttonArray[45].Disable(4);
			buttonArray[46].Disable(2);
			buttonArray[47].Disable(0);

			buttonArray[48].Enable(0);
			buttonArray[49].Enable(0);

			pGameMenu->InitializeCursor(48);
			break;

		case 1: //***LOAD PLAYER***
			
			//*** Get the Character's Button Index
			GameData.iCurrentCharacterButton = pButton->GetButtonID();
			
			//*** Get the Characters's Profile Slot Index
			GameData.iCharacterProfileSlot = GameData.iCurrentCharacterButton - 5;

			pButton->SetDisabledPos(374, 60);
			pButton->Disable(0);

			//***Disable the player login buttons
			buttonArray[0].Disable(8);
			buttonArray[1].Disable(6);
			buttonArray[2].Disable(4);
//			buttonArray[3].Disable(2);
			buttonArray[4].Disable(0);
			
			buttonArray[51].Enable(0);
			pGameMenu->InitializeCursor(51);
			
			//Copy our name from our selected slot to our real name
			if( GameData.DBase.SetActiveProfile( GameData.iCharacterProfileSlot ) )
			{
				strcpy(GameData.CharacterName, GameData.DBase.GetActiveProfile()->GetName() );
				GameData.CharPic = GameData.DBase.GetActiveProfile()->GetCharacterID();
			}
			
			break;

		case 2: //***ERASE PLAYER***
		
			//*** Get the Character's Button Index
			GameData.iCurrentCharacterButton = pButton->GetButtonID();
			
			//*** Get the Characters's Profile Slot Index
			GameData.iCharacterProfileSlot = GameData.iCurrentCharacterButton - 5;
	
			
			
			
			pButton->SetDisabledPos(374, 60);
			pButton->Disable(0);

			//***Disable the player login buttons
			buttonArray[0].Disable(8);
			buttonArray[1].Disable(6);
			buttonArray[2].Disable(4);
//			buttonArray[3].Disable(2);
			buttonArray[4].Disable(0);
	
			buttonArray[52].Enable(0);
			pGameMenu->InitializeCursor(52);
			
			//Copy our name from our selected slot to our real name
			strcpy(GameData.CharacterName, pButton->GetTextString());
//			LaunchErasePlayerButton(pGameMenu, pButton, 0);

			break;

		default:
			MessageBox(NULL, "there is no case for this intLaunchPlayerSlotButton()", "ok", MB_OK);

	}
}

void	LaunchEnterErasePasswordButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

//	long temp;
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	ResetAllCharacterButtons(pGameMenu);
	
	buttonArray[52].Disable(5);//rid the Loadpassword entry button
	
	if(pGameMenu->GetMenuID() == MENU_ID_MULTI_PLAYER_LOGIN)
	{
		strcpy(App.pDP->szPassword,pButton->GetTextString());

		//ANDY
		App.pDP->JoinSession();
		
		//create our player
		App.pDP->CreatePlayer( App.pDP->szShortName, 2 );

		pGameMenu->InitializeCursor(0);

		pGameMenu->SwitchMenu(MENU_ID_MULTI_PLAYER_LOGIN);	
		
		//clear the text screen
		pButton->GetTextString()[0]			=	0;
		buttonArray[48].Disable(0);				
		buttonArray[48].GetTextString()[0]	=	0;	

	}
	else
	{	
		//*** We are trying to delete a character's profile!!!

		//first make the profile we want to delete active..
		
		if( GameData.DBase.DeleteProfile( GameData.iCharacterProfileSlot ) )
		{
			//go back to login screen
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);

			//remove player slot data
//			strcpy( pGameMenu->buttonArray[ GameData.CurrentCharacterSlot ].GetTextString(), "EMPTY" );
//			pGameMenu->buttonArray[ GameData.CurrentCharacterSlot ].SetCharacterIndex( 0 );
			
			SavePlayerNames( buttonArray );
			LoadPlayerNames( pGameMenu );
		}
		else
		{
			//*FIX*
		//	MessageBox(NULL, "could not select the correct profile to delete");

			pGameMenu->buttonArray[68].Enable(0);

			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
		}
		
		//clear the text screen
		pButton->GetTextString()[0]			=	0;
		buttonArray[48].GetTextString()[0]	=	0;

/*
		//We are in SINGLE player
		GameData.DBase.LogProfileOn(
			&GameData.Profile,
			pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString());

		temp=GameData.DBase.CheckPassword(
			&GameData.Profile,
			pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString(),
			pButton->GetTextString());	//password

		//
		//do some password checking
		if(temp == PP_CORRECT_PASSWORD)
		{
			//valid password now delete the profile
			GameData.DBase.DeleteProfile(&GameData.Profile);

			//go back to login screen
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);

			//remove player slot data
			strcpy(pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString(),"EMPTY");
			pGameMenu->buttonArray[GameData.CurrentCharacterSlot].SetCharacterIndex(0);

			SavePlayerNames(buttonArray);

			//clear the text screen
			pButton->GetTextString()[0]		=	0;
			buttonArray[48].GetTextString()[0]	=	0;	
		}
		else
		{
			pGameMenu->buttonArray[68].Enable(0);

			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);

			//clear the text screen
			pButton->GetTextString()[0]=	0;
			buttonArray[48].GetTextString()[0]	=	0;	
		}
*/
	}

}


void LaunchLoadPlayerButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	Menu_Cursor	*pCursor	= pGameMenu->GetCursor();
	Menu_Button	*pNeighbor	= pButton->GetNeighbor(BUTTON_RIGHT);

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		// set the blinking cursor
		pGameMenu->SetBlinkingCursor();

		// have the cursor point to the neighbor on the right
		pCursor->SetButton(pNeighbor);

		// invoke that neighbor's select function
		
		pNeighbor->Select(pGameMenu, pButton, CURSOR_MOVE_RIGHT);

		// set the cursor in motion, and,
		// disable cursor movement until all keys are released
		pCursor->SetCursorFlag(CURSOR_FLAG_MOVING);
		pCursor->UnsetCursorFlag(CURSOR_FLAG_CAN_MOVE);
	}
	else
	{
		pGameMenu->InitializeCursor(48);
		
		//enable name and pw buttons
		pGameMenu->buttonArray[48].Enable(0);
		pGameMenu->buttonArray[51].Enable(5);
	}
}

void LaunchErasePlayerButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//To get here you MUST have pressed enter... 
	// so to make sure that this event doesn't carry over to the next button
	// force the enter key off. <- getto as hell - eli.
	GameData.keysDown[VK_RETURN] = 0;

	Menu_Cursor	*pCursor	= pGameMenu->GetCursor();
	Menu_Button	*pNeighbor	= pButton->GetNeighbor(BUTTON_RIGHT);

	if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
	{
		// set the blinking cursor
		pGameMenu->SetBlinkingCursor();

		// have the cursor point to the neighbor on the right
		pCursor->SetButton(pNeighbor);

		// invoke that neighbor's select function
		pNeighbor->Select(pGameMenu, pButton, CURSOR_MOVE_RIGHT);

		// set the cursor in motion, and,
		// disable cursor movement until all keys are released
		pCursor->SetCursorFlag(CURSOR_FLAG_MOVING);
		pCursor->UnsetCursorFlag(CURSOR_FLAG_CAN_MOVE);
	}
	else
	{
		pGameMenu->InitializeCursor(48);
		
		//enable name and pw buttons
		pGameMenu->buttonArray[48].Enable(0);
		pGameMenu->buttonArray[52].Enable(5);
	}

}
bool CheckBadKeys()
{
	U32*	pKeys = App.pdistruct->curKeyState;

	if	(pKeys[DIK_TAB] == 1)
		return FALSE;
	else if	(pKeys[DIK_SPACE] == 1)
		return FALSE;
	else if	(pKeys[DIK_COMMA] == 1)
		return FALSE;
	else if	(pKeys[DIK_PERIOD] == 1)
		return FALSE;
	else if	(pKeys[DIK_SLASH] == 1)	
		return FALSE;
	else if	(pKeys[DIK_MULTIPLY] == 1)	
		return FALSE;
	else if	(pKeys[DIK_DIVIDE] == 1)
		return FALSE;
	else if	(pKeys[DIK_SEMICOLON] == 1)
		return FALSE;
	else if	(pKeys[DIK_APOSTROPHE] == 1)
		return FALSE;
	else if	(pKeys[DIK_GRAVE] == 1)	
		return FALSE;
	else if	(pKeys[DIK_BACKSLASH] == 1)
		return FALSE;
	else if	(pKeys[DIK_RBRACKET] == 1)	
		return FALSE;
	else if	(pKeys[DIK_LBRACKET] == 1)
		return FALSE;
	else if	(pKeys[DIK_GRAVE] == 1)	
		return FALSE;
	else if	(pKeys[DIK_EQUALS] == 1)
		return FALSE;
	else if	(pKeys[DIK_MINUS] == 1)		
		return FALSE;
	else if	(	(pKeys[DIK_1] == 1
		||		pKeys[DIK_2] == 1
		||		pKeys[DIK_3] == 1
		||		pKeys[DIK_4] == 1
		||		pKeys[DIK_5] == 1
		||		pKeys[DIK_6] == 1
		||		pKeys[DIK_7] == 1
		||		pKeys[DIK_8] == 1
		||		pKeys[DIK_9] == 1
		||		pKeys[DIK_0] == 1) 
		&& (pKeys[DIK_LSHIFT]  || pKeys[DIK_RSHIFT] ))	
			return FALSE;
	else if	(pKeys[DIK_SUBTRACT] == 1)		
		return FALSE;
	else if	(pKeys[DIK_ADD] == 1)		
		return FALSE;
	else if	(pKeys[DIK_LBRACKET] == 1)		
		return FALSE;
	else if	(pKeys[DIK_RBRACKET] == 1)		
		return FALSE;

	return TRUE;
}

void UpdateEnterNameButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	U32*	pKeys = App.pdistruct->curKeyState;

	//if the password button is the button "the hand" is pointing to
	if( (pButton == pGameMenu->cursor.GetpButton()) )
	{
		if(CheckBadKeys())
			ChatInput(pButton->GetTextString(), 12);

		//player cannot enter "EMPTY" as a name
		if(!(strcmp("EMPTY",pButton->GetTextString())))
			pButton->GetTextString()[0]=0;

//		if((App.pdistruct->keys[DIK_RETURN] & 0x80) ||
//		   (App.pdistruct->keys[DIK_NUMPADENTER] & 0x80) ||
//		   (App.pdistruct->keys[DIK_TAB] & 0x80) )

		if( GameData.keysDown[VK_RETURN] )
		{
			GameData.keysDown[VK_RETURN] = 0;

			if(pGameMenu->GetMenuID() == MENU_ID_CREATE_CHANNEL)
			{
				//move to PW button for channel

				//Copy our name from our selected slot to our temp name slot prior to PW check
				strcpy(App.pDP->NewChannelName, pButton->GetTextString());

				//put the cursor on the Enter Password button
				pGameMenu->InitializeCursor(60);
				return;
			}
			
			//Copy our name from our selected slot to our temp name slot prior to PW check
			strcpy(pGameMenu->TempName, pButton->GetTextString());
			strcpy(GameData.CharacterName, pGameMenu->TempName);


			if(pGameMenu->GetMenuID() == MENU_ID_SINGLE_PLAYER_LOGIN)
			{
				//put the cursor on the Enter Password button
				pGameMenu->InitializeCursor(49);
			}
			else if(pGameMenu->GetMenuID() == MENU_ID_MULTI_PLAYER_LOGIN)
			{
				//Copy our name from our selected slot to our temp name slot prior to PW check
				strcpy(App.pDP->szShortName, pButton->GetTextString());

				if(pGameMenu->buttonArray[49].IsEnabled())
				{
					//we are creating a player

					//put the cursor on the Enter Password button
					pGameMenu->InitializeCursor(49);
				}
				else if(pGameMenu->buttonArray[51].IsEnabled())
				{
					//we are loading a player

					//put the cursor on the Enter Password button
					pGameMenu->InitializeCursor(51);
				}
				else if(pGameMenu->buttonArray[52].IsEnabled())
				{
					//we are loading a player

					//put the cursor on the Enter Password button
					pGameMenu->InitializeCursor(52);
				}
			}
		}
	}
}

void UpdateEnterPasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	U32*	pKeys = App.pdistruct->curKeyState;

	//if the password button is the button "the hand" is pointing to
	if( (pButton == pGameMenu->cursor.GetpButton()) )
	{
		if(!(pKeys[DIK_TAB] == 1 || pKeys[DIK_SPACE] == 1))
			ChatInput(pButton->GetTextString(), 12);

//		if((App.pdistruct->curKeyState[DIK_RETURN]		== 1) ||
//		   (App.pdistruct->curKeyState[DIK_NUMPADENTER] == 1)  )
	    if(GameData.keysDown[VK_RETURN])
		{
			GameData.keysDown[VK_RETURN] = 0;
			//Copy our name from our selected slot to our real name
			strcpy(GameData.CharacterName, pButton->GetTextString());
		}
	}

}

void LaunchEnterNewPasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
//	long						temp;
//	char						szProfile[40];

	if(pGameMenu->GetMenuID() == MENU_ID_MULTI_PLAYER_LOGIN)
	{
		
		strcpy(App.pDP->szPassword,pButton->GetTextString());

		//ANDY
		App.pDP->JoinSession();
		
		//create our player
		App.pDP->CreatePlayer(pGameMenu->TempName,TRUE);
		
		buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
		buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button

		//clear the text screen
		pButton->GetTextString()[0]=	0;
	}
	else
	{	//We are in SINGLE player login
		ResetAllCharacterButtons(pGameMenu);

		buttonArray[48].Disable(0); //rid the name entry button
		buttonArray[49].Disable(0); //rid the Newpassword entry button

		buttonArray[0].Disable(5);
		buttonArray[1].Disable(10);
		buttonArray[2].Disable(15);
//		buttonArray[3].Disable(8);
		buttonArray[4].Disable(20);

/*		switch (pGameMenu->TempCharIcon-44)
		{
			case(0):	strcpy(szProfile,"SexxyFoxWoman");	break;
			case(1):	strcpy(szProfile,"VineChick");		break;
			case(2):	strcpy(szProfile,"NinjaDude");		break;
			case(3):	strcpy(szProfile,"Bundy");			break;
		}
		
		//This characterID crap is sad.... look at the shitty code above... -44 ?
		// who the hell thinks they know what there doing around here. -eli. 
*/		
		s32 iSlot;
		
		//Create our profile
		iSlot = GameData.DBase.CreateProfile( pGameMenu->TempName, GameData.CharPic );
		
		//check to see if its all good
		if( iSlot < 0 )
		{
			//*** Error
			// could not find an open slot to create a profile with
			// or the profile created is a duplicate name.
			
			//MessageBox(NULL,"Profile name already in use","ERROR",0);
			pGameMenu->buttonArray[67].Enable(0);
			
			//go back a menu
			//clear the text screen
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
			
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
		}
		else
		{
			//*** WE are all good!!
			GameData.iCharacterProfileSlot = iSlot;
			
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
			
			//all is good save characer info to slot
//			strcpy(pGameMenu->buttonArray[ GameData.CurrentCharacterSlot ].GetTextString(), pGameMenu->TempName);
//			strcpy(GameData.CharacterName,	pGameMenu->TempName);
			
			SavePlayerNames( pGameMenu->buttonArray );
			LoadPlayerNames( pGameMenu );
			
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button

			//switch to game select screen
			pGameMenu->SwitchMenu( MENU_ID_SINGLE_GAME_SELECT );
		}

		/*
		temp=GameData.DBase.CreateProfile(
			&GameData.Profile,
			pGameMenu->TempName,
			pButton->GetTextString(),
			szProfile);
		
		if(temp != PP_USERID_ALREADY_IN_USE)
		{
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
			//all is good save characer info to slot
			strcpy(pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString(),pGameMenu->TempName);
			strcpy(
				GameData.CharacterName,
				pGameMenu->TempName);
			SavePlayerNames(pGameMenu->buttonArray);
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button

			//switch to game select screen
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);

		}
		else
		{
			//MessageBox(NULL,"Profile name already in use","ERROR",0);
			pGameMenu->buttonArray[67].Enable(0);
			//
			//go back a menu
			//clear the text screen
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
			//
			//clear the text screen
			buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
			buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
		}
		*/
	}
}

void LaunchEnterLoadPasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
//	long temp;
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	ResetAllCharacterButtons(pGameMenu);
	
	buttonArray[51].Disable(0);//rid the Loadpassword entry button
	buttonArray[48].Disable(5);//rid the Loadpassword entry button
//	buttonArray[48].Disable(5);//rid the Loadpassword entry button
	
	buttonArray[0].Disable(2);
	buttonArray[1].Disable(4);
	buttonArray[2].Disable(6);
//	buttonArray[3].Disable(8);
	buttonArray[4].Disable(10);

	if(pGameMenu->GetMenuID() == MENU_ID_MULTI_PLAYER_LOGIN)
	{
		strcpy(App.pDP->szPassword,pButton->GetTextString());

		App.pDP->bNewPlayer = FALSE;

		//ANDY
		App.pDP->JoinSession();
	
		//create our player
		App.pDP->CreatePlayer(App.pDP->szShortName, FALSE);
		
		//clear the text screen
		pButton->GetTextString()[0]=	0;
		buttonArray[48].GetTextString()[0]	=	0;	
	}
	else
	{	
		//We are in SINGLE player login
		strcpy(	pGameMenu->TempName,
				pGameMenu->buttonArray[ GameData.iCurrentCharacterButton ].GetTextString());
		
		
		//Make our profile active because we are about to play a game!!!
		if( strcmp(pGameMenu->TempName, "EMPTY") &&
			GameData.DBase.SetActiveProfile( GameData.CharacterName ) )
		{
			//*** Everything is ALL GOOD 
			
			//Get our Name And CharacterPic from the profile 
			strcpy(GameData.CharacterName, GameData.DBase.GetActiveProfile()->GetName() );
			GameData.CharPic = GameData.DBase.GetActiveProfile()->GetCharacterID();

			//clear the text screen
			pButton->GetTextString()[0]=	0;

			//pGameMenu->InitializeCursor(30);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
			buttonArray[48].GetTextString()[0]	=	0;	

			//save character icon where eli needs it

			//GameData.CharPic = pButton->GetCharacterIndex();//GameData.CurrentCharacterSlot;

		}
		else
		{
			//*** ERROR We could not set our profile active!!!

			//MessageBox(NULL,"Bad password","ERROR",0);
			pGameMenu->buttonArray[68].Enable(0);

			//
			//clear the text screen
			pButton->GetTextString()[0]=	0;

			//
			//switch men back to log on
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
			ResetAllCharacterButtons(pGameMenu);
			buttonArray[48].GetTextString()[0]	=	0;	
		}

/*		GameData.DBase.LogProfileOn(
			&GameData.Profile,
			pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString());

		temp=GameData.DBase.CheckPassword(
			&GameData.Profile,
			pGameMenu->buttonArray[GameData.CurrentCharacterSlot].GetTextString(),
			pButton->GetTextString());	//password

		//
		//do some password checking
		if(temp == PP_CORRECT_PASSWORD)
		{
			//clear the text screen
			pButton->GetTextString()[0]=	0;

			//pGameMenu->InitializeCursor(30);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
			buttonArray[48].GetTextString()[0]	=	0;	

			//save character icon where eli needs it
			
			//GameData.CharPic = pButton->GetCharacterIndex();//GameData.CurrentCharacterSlot;

		}
		else
		{
			GameData.DBase.LogProfileOff(
				&GameData.Profile);

			//MessageBox(NULL,"Bad password","ERROR",0);
			pGameMenu->buttonArray[68].Enable(0);

			//
			//clear the text screen
			pButton->GetTextString()[0]=	0;

			//
			//switch men back to log on
			pGameMenu->InitializeCursor(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
			ResetAllCharacterButtons(pGameMenu);
			buttonArray[48].GetTextString()[0]	=	0;	
		}
*/
	}
}

void DisableAllButtons( void )
{
	int iNum	= 0;
	int i		= 0;

	if(!GameData.pGameMenu)
		return;

	Menu_Button	*buttonArray	= GameData.pGameMenu->GetButtonArray();

	iNum = GameData.pGameMenu->GetNumButtons();

	for(i=0; i < iNum; i++)
	{
		buttonArray[i].Disable(0);
	}
	
	ResetAllCharacterButtons(GameData.pGameMenu);

/*	//make sure all the player slots are disabled at the correct position
	for(i = 5; i <= 9; i++)
	{
		buttonArray[i].SetDisabledPos(720, 20 + (i - 5) * 94);
		buttonArray[i].Deselect(GameData.pGameMenu, buttonArray + i, 0);
	}
*/
/*	for(i = 30; i <= 33; i++)	//GameSelect Disable
	{
		buttonArray[i].Disable(((float)i - 30) * 2);
	}
*/
}

void LaunchCancelButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{

	//**** THIS FUNCTION WORKS FOR ALL CANCEL BUTTONS -Eli
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	
	if(!buttonArray || !pGameMenu)
		return;

	int i = 0;

	switch(pGameMenu->GetMenuID())
	{
		case MENU_ID_TITLESCREEN:
			break;
		
		case MENU_ID_CREDITS_SCREEN:

			buttonArray[78].enabledPos.x	= 340 ;
			buttonArray[78].enabledPos.y	= 375 ;
			buttonArray[78].cursorPos.x		= 298 ;
			buttonArray[78].cursorPos.y		= 375 ;
			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);

/*			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);
			buttonArray[78].enabledPos.x	= 340;
			buttonArray[78].enabledPos.y	= 375;
			//cursor pos: 298 375
			pGameMenu->InitializeCursor(78);
*/
			break;

		case MENU_ID_SINGLE_PLAYER_LOGIN:
			buttonArray[0].Disable(0);
			buttonArray[1].Disable(2);
			buttonArray[2].Disable(4);
//			buttonArray[3].Disable(6);
			buttonArray[4].Disable(8);

			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);
			break;

		case MENU_ID_MULTI_IP_LOGIN:
			buttonArray[34].Disable(0);
			buttonArray[42].Disable(2);

			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);
			break;

		case MENU_ID_MULTI_PLAYER_LOGIN:
			buttonArray[0].Disable(0);
			buttonArray[1].Disable(2);
			buttonArray[2].Disable(4);
//			buttonArray[3].Disable(6);
			buttonArray[4].Disable(8);

			pGameMenu->SwitchMenu(MENU_ID_MULTI_IP_LOGIN);
			break;
		
//		case MENU_ID_SINGLE_CHAR_SELECT:
//			break;

		case MENU_ID_SINGLE_GAME_SELECT:
			//make sure all the player slots are disabled at the correct position
			for(i = 5; i <= 9; i++)
			{
				buttonArray[i].SetDisabledPos(720, 20 + (i - 5) * 94);
				buttonArray[i].Deselect(pGameMenu, buttonArray + i, 0);
			}
			for(i = 30; i <= 33; i++)	//GameSelect Disable
				buttonArray[i].Disable(((float)i - 30) * 2);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PLAYER_LOGIN);
			break;

		case MENU_ID_SINGLE_PUZZLE_SCREEN:
			buttonArray[71].Disable(0);
			buttonArray[72].Disable(2);
			buttonArray[73].Disable(4);
			buttonArray[74].Disable(6);

			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
			break;
		
		case MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY:
			buttonArray[74].Disable(0);
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_PUZZLE_SCREEN);

			break;

		case MENU_ID_MULTI_CHAT_SCREEN:
			delete App.pDP;
			App.pDP = NULL;
			buttonArray[35].Disable(0);
			buttonArray[36].Disable(2);
			buttonArray[37].Disable(4);
			buttonArray[38].Disable(6);
			buttonArray[39].Disable(8);
			buttonArray[40].Disable(10);
			buttonArray[41].Disable(12);
			buttonArray[50].Disable(12);

			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);

			break;

		case MENU_ID_MULTI_GAME_LAUNCH:

			//leaving the launch screen, disable the buttons and move back to chat
			pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
			buttonArray[53].Disable(0);
			buttonArray[54].Disable(5);

			//invisible buttons
			buttonArray[55].Disable(0);
			buttonArray[56].Disable(0);
			buttonArray[57].Disable(0);
			buttonArray[58].Disable(0);
			buttonArray[59].Disable(0);
			break;

		case MENU_ID_CREATE_CHANNEL:

			//leaving create channel screen disable buttons
			buttonArray[61].Disable(0);
			buttonArray[60].Disable(5);
			buttonArray[48].Disable(0);

			//switch back to chat
			pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
			break;
		
//		case MENU_ID_SERVER_SCORES:
//		case MENU_ID_PLAYER_SCORES:

		case MENU_ID_CHANNEL_LIST:

			//leaving join channel
			pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
			buttonArray[64].Disable(0);
			buttonArray[65].Disable(5);
			buttonArray[66].Disable(10);

			break;
		
		case MENU_ID_PLAYER_SCORES_SINGLE_PLAYER:

			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
			buttonArray[75].Disable(0);
			break;
		
		default: //We don't have a case for this menu id so go back to title screen	
//			MessageBox(NULL, "Error-> This Cancel button has no\n instructions in LaunchCancelButton()", "Fix it now! -Eli", MB_OK);

			//Disable all buttons
			for(i=0; i < pGameMenu->GetNumButtons(); i++)
			{
				buttonArray[i].Disable(0);
			}

			//Switch to title screen.
			pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
			buttonArray[78].enabledPos.x	= 340;
			buttonArray[78].enabledPos.y	= 375;
			//cursor pos: 298 375
			pGameMenu->InitializeCursor(78);
			break;
	}//end switch
}
/*
	case MENU_ID_TITLESCREEN:
	case MENU_ID_CREDITS_SCREEN:
	case MENU_ID_SINGLE_PLAYER_LOGIN:
	case MENU_ID_MULTI_IP_LOGIN:
	case MENU_ID_MULTI_PLAYER_LOGIN:
	case MENU_ID_SINGLE_CHAR_SELECT:
	case MENU_ID_SINGLE_GAME_SELECT:
	case MENU_ID_SINGLE_PUZZLE_SCREEN:
	case MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY:
	case MENU_ID_MULTI_CHAT_SCREEN:
	case MENU_ID_MULTI_GAME_LAUNCH:
	case MENU_ID_CREATE_CHANNEL:
	case MENU_ID_SERVER_SCORES:
	case MENU_ID_PLAYER_SCORES:
	case MENU_ID_CHANNEL_LIST:
	case MENU_ID_PLAYER_SCORES_SINGLE_PLAYER:
*/
void LaunchCancelButton2(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//**** THIS FUNCTION WORKS FOR ALL CANCEL BUTTONS -Eli
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	
	if(!buttonArray || pGameMenu)
		return;
	
	int i = 0;
	
	switch(pGameMenu->GetMenuID())
	{
	case MENU_ID_TITLESCREEN:
		buttonArray[27].disabledPos.x	= 640;
		buttonArray[27].disabledPos.y	= 250;
		
		// This is the multiplayer button
		buttonArray[28].disabledPos.x	= 640;
		buttonArray[28].disabledPos.y	= 314;
		


		buttonArray[27].Enable(5);
//		buttonArray[28].Enable(4);	// Multi player button
		buttonArray[78].Enable(10);
		buttonArray[79].Enable(10);
		buttonArray[29].Enable(15);
		
	

		//Enable our TitleScreen
		buttonArray[76].Enable(0);
		buttonArray[77].Enable(0);

		pGameMenu->InitializeCursor(27);
		break;
	
	case MENU_ID_CREDITS_SCREEN:

		buttonArray[78].enabledPos.x	= 0;
		buttonArray[78].enabledPos.y	= 0;

		buttonArray[76].Disable(2);
		buttonArray[77].Disable(2);
		
		pGameMenu->InitializeCursor(78);
		break;

	case MENU_ID_SINGLE_PLAYER_LOGIN:
		buttonArray[27].disabledPos.x	= 0;
		buttonArray[27].disabledPos.y	= 0;

		buttonArray[76].Disable(2);
		buttonArray[77].Disable(2);
		buttonArray[78].Disable(2);
		
		buttonArray[79].Disable(2);

		buttonArray[0].Enable(4);
		buttonArray[1].Enable(8);
		buttonArray[2].Enable(12);
//		buttonArray[3].Enable(10);
		buttonArray[4].Enable(16);
		
//		pGameMenu->cursor.pButton	= buttonArray + 0; //target pos
//		pGameMenu->cursor.pButton->Select(this, buttonArray + 0, 0);
		break;
		
	case MENU_ID_MULTI_IP_LOGIN:

		// This is the multi player button
		buttonArray[28].disabledPos.x	= 0;
		buttonArray[28].disabledPos.y	= 0;

		buttonArray[76].Disable(2);
		buttonArray[77].Disable(2);	
		buttonArray[78].Disable(2);

		buttonArray[42].Enable(6);
		buttonArray[34].Enable(6);
		
//		pGameMenu->cursor.pButton	= buttonArray + 42; //target pos
		break;
		
	case MENU_ID_MULTI_PLAYER_LOGIN:
		
		buttonArray[0].Enable(2);
		buttonArray[1].Enable(6);
		buttonArray[2].Enable(12);
//		buttonArray[3].Enable(8);	// Key config button
		buttonArray[4].Enable(18);
		
//		pGameMenu->cursor.pButton	= buttonArray + 0; //target pos
//		pGameMenu->cursor.pButton->Select(this, buttonArray + 0, 0);
		break;
		
/*	case MENU_ID_SINGLE_CHAR_SELECT:
		buttonArray[44].Enable(2);
		buttonArray[45].Enable(4);
		buttonArray[46].Enable(6);
		buttonArray[47].Enable(8);

		cursor.pButton	= buttonArray + 44; //target pos
		cursor.pButton->Select(this, buttonArray + 44, 0);
		break;
*/		
	case MENU_ID_SINGLE_GAME_SELECT:
		
		buttonArray[30].Enable(2);
		buttonArray[31].Enable(4);
		buttonArray[32].Enable(6);
		buttonArray[33].Enable(8);
		
		buttonArray[75].Disable(10);

//		pGameMenu->cursor.pButton	= buttonArray + 30; //target pos
		break;
	
	case MENU_ID_SINGLE_PUZZLE_SCREEN:
		buttonArray[71].Enable(2);
		buttonArray[72].Enable(4);
		buttonArray[73].Enable(6);

//		pGameMenu->cursor.pButton	= buttonArray + 71; //target pos
		break;

	case MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY:
		buttonArray[74].Enable(0);

//		pGameMenu->cursor.pButton	= buttonArray + 74; //target pos
		break;

	case MENU_ID_MULTI_CHAT_SCREEN:
		
		buttonArray[35].Enable(0);
		buttonArray[36].Enable(5);
		buttonArray[37].Enable(10);
		buttonArray[38].Enable(15);
		buttonArray[39].Enable(5);
		buttonArray[40].Enable(10);
		buttonArray[41].Enable(15);
		buttonArray[50].Enable(15);

		buttonArray[62].Disable(15);
		buttonArray[63].Disable(15);
		buttonArray[64].Disable(0);
		buttonArray[65].Disable(5);
		buttonArray[66].Disable(10);



//		pGameMenu->cursor.pButton	= buttonArray + 41; //target pos
		break;

	case MENU_ID_MULTI_GAME_LAUNCH:

		//going into the launch game screen, enable all the buttons
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(2);
		buttonArray[37].Disable(4);
		buttonArray[38].Disable(6);
		buttonArray[39].Disable(8);
		buttonArray[40].Disable(10);
		buttonArray[41].Disable(12);
		buttonArray[50].Disable(12);

		//enable launch screen
//		pGameMenu->cursor.pButton	= buttonArray + 54; //target pos
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
		buttonArray[36].Disable(2);
		buttonArray[37].Disable(4);
		buttonArray[38].Disable(6);
		buttonArray[39].Disable(8);
		buttonArray[40].Disable(10);
		buttonArray[41].Disable(12);
		buttonArray[50].Disable(12);

//		pGameMenu->cursor.pButton	= buttonArray + 48; //target pos
		buttonArray[61].Enable(0);
		buttonArray[60].Enable(5);
		buttonArray[48].Enable(0);
		break;

	case MENU_ID_SERVER_SCORES:
		//disable chat
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(2);
		buttonArray[37].Disable(4);
		buttonArray[38].Disable(6);
		buttonArray[39].Disable(8);
		buttonArray[40].Disable(10);
		buttonArray[41].Disable(12);
		buttonArray[50].Disable(12);

//		pGameMenu->cursor.pButton	= buttonArray + 62; //target pos
		buttonArray[62].Enable(0);
		break;

	case MENU_ID_PLAYER_SCORES:
		//disable chat
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(2);
		buttonArray[37].Disable(4);
		buttonArray[38].Disable(6);
		buttonArray[39].Disable(8);
		buttonArray[40].Disable(10);
		buttonArray[41].Disable(12);
		buttonArray[50].Disable(12);

//		pGameMenu->cursor.pButton	= buttonArray + 63; //target pos
		buttonArray[63].Enable(0);
		break;

	case MENU_ID_CHANNEL_LIST:
		buttonArray[35].Disable(0);
		buttonArray[36].Disable(2);
		buttonArray[37].Disable(4);
		buttonArray[38].Disable(6);
		buttonArray[39].Disable(8);
		buttonArray[40].Disable(10);
		buttonArray[41].Disable(12);
		buttonArray[50].Disable(12);

//		pGameMenu->cursor.pButton	= buttonArray + 65; //target pos
		buttonArray[64].Enable(0);
		buttonArray[65].Enable(5);
		buttonArray[66].Enable(10);
		break;

	case MENU_ID_PLAYER_SCORES_SINGLE_PLAYER:
		buttonArray[30].Disable(0);
		buttonArray[31].Disable(0);
		buttonArray[32].Disable(0);
		buttonArray[33].Disable(0);
//		pGameMenu->cursor.pButton	= buttonArray + 75; //target pos

		buttonArray[75].Enable(0);
		break;

	} //end switch
}

//*********GAME_SELECT SCREEN SPECIFIC FUNCTIONS*********

void LaunchKeyConfigButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Cursor	*pCursor	= pGameMenu->GetCursor();
	Menu_Button	*pNeighbor	= pButton->GetNeighbor(BUTTON_RIGHT);

	// set the blinking cursor
	pGameMenu->SetBlinkingCursor();

	// have the cursor point to the neighbor on the right
	pCursor->SetButton(pNeighbor);

	// invoke that neighbor's select function
	pNeighbor->Select(pGameMenu, pButton, CURSOR_MOVE_RIGHT);

	// set the cursor in motion, and,
	// disable cursor movement until all keys are released
	pCursor->SetCursorFlag(CURSOR_FLAG_MOVING);
	pCursor->UnsetCursorFlag(CURSOR_FLAG_CAN_MOVE);
}


//*********MULTIPLAYER LOGIN SCREEN SPECIFIC FUNCTIONS*********
void UpdateEnterIPButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{

	//***Display our IP information (eli style)
	if( (pButton == pGameMenu->cursor.GetpButton()  &&
		DisplayIP(pGameMenu, pButton)  ) )
	{
		GameData.bLoggingOntoServer = 1;
	}
}

void LaunchEnterIPButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if(GameData.bLoggingOntoServer)
	{
		//***SUCCESS
		// Andy you need to take it from here(with our IP address)
		char IPstring[15];

		//build the IPstring
		strcpy(IPstring,GameData.IPAddress[0]);
		strcat(IPstring,".");
		strcat(IPstring,GameData.IPAddress[1]);
		strcat(IPstring,".");
		strcat(IPstring,GameData.IPAddress[2]);
		strcat(IPstring,".");
		strcat(IPstring,GameData.IPAddress[3]);

		//Initialize class
		if(!App.UserSettings.RenderMode)
			App.pDP = new tagDPClass(App.pDDraw->getHwnd());
		else
			App.pDP = new tagDPClass(App.pOpenGL->getHwnd());

		//ANDY
		App.pDP->CreateTCPConnection(IPstring);//"209.84.100.192");

		//ANDY
		App.pDP->EnumerateSessions();

		GameData.bLoggingOntoServer = 0;
	}

}

void UpdateErrorIPButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if(pButton->GetScreenPos().y == pButton->GetEnabledPos().y )
	{
		pButton->Disable(150);
	}
}


void UpdateScreenBorder(Menu_Button *pButton)
{
	RECT		BltRect			= { 0, 16, 4, 56};
	POINT		screenPos		= pButton->GetScreenPos();

	if(!App.UserSettings.RenderMode)
	{
		//***Draw our Chat Box***
		//ELI
		LPDD_SURF	pSurf			= App.pDDraw->surfaceArray[5];//pButton->GetpSurf();

		//***Blt the left and right side of our chat box
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x, screenPos.y,
				pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.left = 49; BltRect.right = 53;
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x + 448, screenPos.y,
				pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		
		BltRect.left = 4; BltRect.right = 48;
		for(int i=0; i <= 10; i++)
			App.pDDraw->getBackBuffer()->BltFast(screenPos.x+4+(i*40), screenPos.y,
				pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);


		//***Draw our Lobby Chat Box (not the player's Chat box)***
		BltRect.left	= 0;
		BltRect.top		= 56;
		BltRect.right	= 26;
		BltRect.bottom	= 95;
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x-4,  44, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.top		= 66; BltRect.bottom	= 105;
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x-4, 363, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.bottom	= 95;
		for(i=0; i <= 9; i++)
		{
			App.pDDraw->getBackBuffer()->BltFast(screenPos.x-4, 83+(i*29), pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
			App.pDDraw->getBackBuffer()->BltFast(screenPos.x + 446, 83+(i*29), pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		}
		BltRect.left	= 26;	
		BltRect.top		= 56;	
		BltRect.right	= 52;	
		BltRect.bottom	= 95;
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x + 430, 44, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.top		= 66; BltRect.bottom	= 105;
		App.pDDraw->getBackBuffer()->BltFast(screenPos.x + 430, 363, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
	}
	else
	{	//***OPEN_GL MODE***

		//***Blt the left and right side of our chat box
		BltGLStyle(screenPos.x, screenPos.y, &BltRect, 
			256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.left = 49; BltRect.right = 53;
		BltGLStyle(screenPos.x + 448, screenPos.y, &BltRect, 
			256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.left = 4; BltRect.right = 48;
		for(int i=0; i <= 10; i++)
			BltGLStyle(screenPos.x+4+(i*40), screenPos.y, &BltRect, 
				256, 256, GL_SURF_MENU_BUTTONS2_TEXID);

		//***Draw our Lobby Chat Box (not the player's Chat box)***
		BltRect.left	= 0;
		BltRect.top		= 56;
		BltRect.right	= 26;
		BltRect.bottom	= 95;
		
		BltGLStyle(screenPos.x-4, 44, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.top		= 66; BltRect.bottom	= 105;
		BltGLStyle(screenPos.x-4, 363, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.bottom	= 95;
		for(i=0; i <= 9; i++)
		{
			BltGLStyle(screenPos.x-4, 83+(i*29), &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
			BltGLStyle(screenPos.x + 446, 83+(i*29), &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		}
		BltRect.left	= 26;	
		BltRect.top		= 56;	
		BltRect.right	= 52;	
		BltRect.bottom	= 95;
		BltGLStyle(screenPos.x + 430, 44, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.top		= 66; BltRect.bottom	= 105;
		BltGLStyle(screenPos.x + 430, 363, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
	}
}

void UpdatePlayerBorder(Menu_Button *pButton)
{
	//***Draw our Chat Box***
	
	RECT		BltRect			= { 0, 16, 4, 56};
	
	if(!App.UserSettings.RenderMode)
	{
		//DAMINT FIX IT
		LPDD_SURF	pSurf			= App.pDDraw->surfaceArray[5];//pButton->GetpSurf();
		//***Draw our players box)***
		BltRect.left	= 0;
		BltRect.top		= 56;
		BltRect.right	= 26;
		BltRect.bottom	= 95;
		App.pDDraw->getBackBuffer()->BltFast(5,  44, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.top		= 66; BltRect.bottom	= 105;
		App.pDDraw->getBackBuffer()->BltFast(5, 150, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.bottom	= 95;
		for(int i=0; i <= 3; i++)
		{
			App.pDDraw->getBackBuffer()->BltFast(5, 55+(i*29), pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
			App.pDDraw->getBackBuffer()->BltFast(143, 55+(i*29), pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		}
		BltRect.left	= 26;	
		BltRect.top		= 56;	
		BltRect.right	= 52;	
		BltRect.bottom	= 95;
		App.pDDraw->getBackBuffer()->BltFast(127, 44, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
		BltRect.top		= 66; BltRect.bottom	= 105;
		App.pDDraw->getBackBuffer()->BltFast(127, 150, pSurf, &BltRect, DDBLTFAST_SRCCOLORKEY);
	}
	else
	{		//***OPEN_GL MODE***
		BltRect.left	= 0;
		BltRect.top		= 56;
		BltRect.right	= 26;
		BltRect.bottom	= 95;
		
		BltGLStyle(5, 44, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.top		= 66; BltRect.bottom	= 105;
		BltGLStyle(5, 150, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.bottom	= 95;
		for(int i=0; i <= 3; i++)
		{
			BltGLStyle(5, 55+(i*29), &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
			BltGLStyle(143, 55+(i*29), &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		}
		BltRect.left	= 26;	
		BltRect.top		= 56;	
		BltRect.right	= 52;	
		BltRect.bottom	= 95;
		BltGLStyle(127, 44, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		BltRect.top		= 66; BltRect.bottom	= 105;
		BltGLStyle(127, 150, &BltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);

	}
}


//*********MULTIPLAYER CHAT SCREEN SPECIFIC FUNCTIONS*********
void UpdatePlayerChatBox(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	int error=0;

	if(pGameMenu->GetMenuID() == MENU_ID_MULTI_CHAT_SCREEN )
	{	
//		POINT		screenPos		= pButton->GetScreenPos();

		//update main screen borders
		UpdateScreenBorder(pButton);

		//update player list border
		UpdatePlayerBorder(pButton);

		//***Draw our Text***
//		POINT	textOffset		= pButton->GetTextOffset();
		char	*pChatString	= GameData.ChatString;	
		int		strLength		= strlen(pChatString);
			
	
		ChatInput(pChatString, 128);

		RECT rcText = { 168, 418, 610, 480 };

		if(!App.UserSettings.RenderMode)
		{	//***DDRAW MODE
			//This function will edit our Player Chat Box's String
			App.pDDraw->DDrawText(&rcText, 0, pChatString );

			App.pDDraw->DDrawText(325,5, TF_FONT_LARGE, App.pDP->ChannelList.GetPlayerName(App.pDP->dpidRoom));

			App.pDP->ChatList.DisplayChatList();
			App.pDP->numPlayers = 0;
			App.pDP->PlayerList.ClearList();
			App.pDP->lpDP->EnumGroupPlayers(App.pDP->dpidRoom, App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);
			App.pDP->PlayerList.DisplayPlayerList();
		}
		else//***OPENGL MODE
		{
			App.pOpenGL->GLDrawText(&rcText, 0, pChatString );

			App.pOpenGL->GLDrawText(325,5, TF_FONT_LARGE, App.pDP->ChannelList.GetPlayerName(App.pDP->dpidRoom));

			App.pDP->ChatList.DisplayChatList();
			App.pDP->numPlayers = 0;
			App.pDP->PlayerList.ClearList();
			App.pDP->lpDP->EnumGroupPlayers(App.pDP->dpidRoom, App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);
			App.pDP->PlayerList.DisplayPlayerList();
		}
	}
}


void SelectNewPlayerButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	pGameMenu->UnsetBlinkingCursor();

	for(int i = 44; i <= 47; i++)
	{
		buttonArray[i].Enable(((float)i - 39) * 3);
	}

	for(i = 44; i <= 47; i++)
	{
		buttonArray[i].SetNeighbor(BUTTON_LEFT, pButton);
		buttonArray[i].SetNeighbor(BUTTON_PARENT, pButton);
	}
}

void DeselectNewPlayerButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(direction == CURSOR_MOVE_DOWN || direction == CURSOR_MOVE_UP)
	{
		for(int i = 44; i <= 47; i++)
			buttonArray[i].Disable(((float)i - 44) * 2);
	}
	else if(direction == CURSOR_MOVE_RIGHT)
		pGameMenu->SetBlinkingCursor();
}


void ResetAllCharacterButtons(Game_Menu *pGameMenu)
{
	//***Set all our Character's disabled and enabled position back to normal.
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	//SLOTS
	buttonArray[5].SetDisabledPos(720, 20);
	buttonArray[5].SetEnabledPos( 374, 20);

	buttonArray[6].SetDisabledPos(720, 114);
	buttonArray[6].SetEnabledPos( 374, 114);

	buttonArray[7].SetDisabledPos(720, 208);
	buttonArray[7].SetEnabledPos( 374, 208);

	buttonArray[8].SetDisabledPos(720,302);
	buttonArray[8].SetEnabledPos( 374,302);

	buttonArray[9].SetDisabledPos(720, 396);
	buttonArray[9].SetEnabledPos( 374, 396);

	//ICONS
	buttonArray[44].SetDisabledPos(720, 38);
	buttonArray[44].SetEnabledPos( 374, 38);

	buttonArray[45].SetDisabledPos(720, 152);
	buttonArray[45].SetEnabledPos( 374, 152);

	buttonArray[46].SetDisabledPos(720, 266);
	buttonArray[46].SetEnabledPos( 374, 266);

	buttonArray[47].SetDisabledPos(720,380);
	buttonArray[47].SetEnabledPos( 374,380);
}

void LaunchTextEntry (Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	//***Check to see if we pressed Enter (if so TextString = NULL and send msg through dplay
//	if(App.pdistruct->curKeyState[DIK_RETURN]		== 1 ||
//	   App.pdistruct->curKeyState[DIK_NUMPADENTER]	== 1  )
	if(GameData.keysDown[VK_RETURN])
	{
		GameData.keysDown[VK_RETURN] = 0;
		//ANDY needs to send our TextString here
		char *string;

		if(strlen(GameData.ChatString))
		{
			DPCHAT dpchat;

			dpchat.dwSize	= sizeof(DPCHAT);
			dpchat.dwFlags	= 0;
			dpchat.lpszMessageA	= GameData.ChatString;

			App.pDP->lpDP->SendChatMessage(	App.pDP->dpid,
											App.pDP->dpidRoom,
											DPSEND_GUARANTEED ,
											&dpchat);

			string = (char*)malloc(strlen(GameData.ChatString)+17);
			strcpy(string,"%C2YOU SAID%C0: ");
			strcat(string,GameData.ChatString);
			//ANDY needs to send our TextString here
			App.pDP->ChatList.AddChatString(string);
			free(string);
		}
		ZeroMemory(GameData.ChatString, 128);
		//GameData.ChatString[0] = NULL;

	}
}
void	LaunchCreateGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if(App.pDP->dpidRoom != App.pDP->dpidRootRoom)
		//clear all manu buttons
		pGameMenu->SwitchMenu(MENU_ID_MULTI_GAME_LAUNCH);
}

void	UpdateCreateGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	int i=0;
	RECT		BoxBltRect			= { 0, 56, 52, 108};
	RECT		CheckBltRect		= { 60, 24, 95, 49};
	
	if(!App.UserSettings.RenderMode)
	{
		//this button has my surface with the boxes and check mark
		LPDD_SURF	pSurf			= App.pDDraw->surfaceArray[5];//pGameMenu->buttonArray[41].GetpSurf();
		//draw the boxes
		//blit left box
		App.pDDraw->getBackBuffer()->BltFast(390,  50, pSurf, &BoxBltRect, DDBLTFAST_SRCCOLORKEY);

		//blit right box
		App.pDDraw->getBackBuffer()->BltFast(582, 50, pSurf, &BoxBltRect, DDBLTFAST_SRCCOLORKEY);

		//draw our lables
		App.pDDraw->DDrawText(175,65, TF_FONT_LARGE | TF_SPACING_3, "GAME TYPE");

		App.pDDraw->DDrawText(350,20, TF_FONT_LARGE | TF_SPACING_3, "VERSUS");
		App.pDDraw->DDrawText(485,20, TF_FONT_LARGE | TF_SPACING_3, "TIME TRIAL");

		//draw our checks
		if(GameData.mpOptions.option[i])
			//option is set to on
			App.pDDraw->getBackBuffer()->BltFast(592,  60, pSurf, &CheckBltRect, DDBLTFAST_SRCCOLORKEY);
		else
			//option is set to off
			App.pDDraw->getBackBuffer()->BltFast(400, 60, pSurf, &CheckBltRect, DDBLTFAST_SRCCOLORKEY);
	}
	else
	{
		//***OPENGL MODE***
		//draw the boxes
		//blit left box
		BltGLStyle(390, 50, &BoxBltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);

		//blit right box
		BltGLStyle(582, 50, &BoxBltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);

		//draw our lables
		App.pOpenGL->GLDrawText(175,  65, TF_FONT_LARGE | TF_SPACING_3, "GAME TYPE");

		App.pOpenGL->GLDrawText(350,  20, TF_FONT_LARGE | TF_SPACING_3, "VERSUS");
		App.pOpenGL->GLDrawText(485,  20, TF_FONT_LARGE | TF_SPACING_3, "TIME TRIAL");

		//draw our checks
		if(GameData.mpOptions.option[i])
			//option is set to on
			BltGLStyle(592, 60, &CheckBltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
		else
			//option is set to off
			BltGLStyle(400, 60, &CheckBltRect, 256, 256, GL_SURF_MENU_BUTTONS2_TEXID);
	}
}

void	LaunchOption1Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.mpOptions.option[0] = (GameData.mpOptions.option[0] == TRUE)?FALSE:TRUE;
}

void	LaunchOption2Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.mpOptions.option[1] = (GameData.mpOptions.option[1] == TRUE)?FALSE:TRUE;
}

void	LaunchOption3Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.mpOptions.option[2] = (GameData.mpOptions.option[2] == TRUE)?FALSE:TRUE;
}

void	LaunchOption4Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.mpOptions.option[3] = (GameData.mpOptions.option[3] == TRUE)?FALSE:TRUE;
}

void	LaunchOption5Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	GameData.mpOptions.option[4] = (GameData.mpOptions.option[4] == TRUE)?FALSE:TRUE;
}

void LaunchCreateChannelButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	//clean up chat buttons and load create channel buttons
	pGameMenu->SwitchMenu(MENU_ID_CREATE_CHANNEL);
}

void LaunchCreateChannelPasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();

	if(pGameMenu->GetMenuID() == MENU_ID_CREATE_CHANNEL)
	{
		strcpy(App.pDP->szPassword,pButton->GetTextString());

		//create our player
		App.pDP->CreateRoom(App.pDP->NewChannelName);
		
		buttonArray[60].GetTextString()[0] = 0; //rid the name entry button
		buttonArray[48].GetTextString()[0] = 0; //rid the Newpassword entry button
	}

	//we are in a private room, enable create game button
	RECT rcSelect	= {80, 0, 160, 28};
	RECT rcDeSelect = {80, 0, 160, 28};

	//now set the clipping rects
	pGameMenu->buttonArray[38].SetClipSelect(rcSelect);
	pGameMenu->buttonArray[38].SetClipDeselect(rcDeSelect);
}

void LaunchDisplayServerStats(
	Game_Menu	*pGameMenu,
	Menu_Button	*pButton,
	int			direction)
{
//	free(GameData.Data);
//	GameData.Data=	NULL;

	//
	//now go back a screen
	pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
}

void PrintTitles(
	long	yVal)
{
	long	counter;
	long	yPos=yVal;

	char	titles[7][30]=	{
							"NAME",
							"HIGHEST   SCORE",
							"GAMES     PLAYED",
							"GAMES     WON",
							"HIGHEST   COMBO",
							"HIGHEST   LINKER",
							"TOTAL     CLUSTERS"
							};
	long	offset[7]=	{	25,		160,
							240,	320,
							400,	480,
							560};	//offsets on the screen
	RECT	rc;

	rc.top=	yVal;
	rc.bottom=rc.top + 40;

	//
	//print the titles at the top of screen
	if(!App.UserSettings.RenderMode)
	{
		for(counter=0;counter < 7;++counter)
		{
			rc.left=offset[counter];
			rc.right=rc.left + 100;

			//
			//draw title to screen
			App.pDDraw->DDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter]);
		}
	}
	else
	{	//***OPENGL MODE***
		for(counter=0;counter < 7;++counter)
		{
			rc.left=offset[counter];
			rc.right=rc.left + 100;

			App.pOpenGL->GLDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter]);
		}
	}
}

void UpdateDisplayServerStats(
	Game_Menu	*pGameMenu,
	Menu_Button	*pButton,
	int			direction)
{

/*
	static long	checking=0;
//	char	*Data=	GameData.Data;
	char	CurData[13];

	long	counter,
			curCounter=0,
			wordCounter=0;
	long	masterCounter=0;

	long	yPos=	60;//start in pixels to draw on y

	//
	//store the offsets in an array of values
	long	offset[7]=	{	25,		160,
							240,	320,
							400,	480,
							560};	//offsets on the screen
	long	done=0;

	if(!GameData.Data)
	{
		if(!checking)
		{
			//*FIX*
//			GameData.DBase.GetHighScoreDataFromServer();
			checking=1;
		}
		return;
	}
*/

/*	checking=0;

	PrintTitles(yPos);

	//
	//cycle through data string and parse out the words and numbers
	counter=0;
	yPos+=60;
	while(!done)	//or the max number of lines we have to write upon
	{
		if((GameData.Data[counter] != '\t')
			&& (GameData.Data[counter] != '\r')
			&& (GameData.Data[counter] != ' ')
			&& (GameData.Data[counter] != '\0'))
		{
			CurData[curCounter]=	GameData.Data[counter];
			curCounter++;
		}
		else
		{
			if(GameData.Data[counter] == '\0')
				done=1;

			CurData[curCounter]=	0;

			//
			//draw the text to the screen in propor position
			if(!App.UserSettings.RenderMode)
			{
				App.pDDraw->DDrawText(
					offset[wordCounter],//x start
					yPos,//y start
					TF_LENGTH_STATS,//width of screen, colour
					CurData);
			}
			else
			{
				//***OPENGL MODE***
				App.pOpenGL->GLDrawText(
					offset[wordCounter],//x start
					yPos,//y start
					TF_LENGTH_STATS,//width of screen, colour
					CurData);
			}
			curCounter=0;
			memset(CurData,0,sizeof(char)*13);
			++wordCounter;

			if(GameData.Data[counter] == '\r')
			{
				yPos+=22;
				++counter;
			}
		}

		if(wordCounter == 7)//max titles
		{
			wordCounter=0;
			masterCounter++;
		}

		if(masterCounter == 15)
			break;

		++counter;
	}
	*/
}

void LaunchDisplayPlayerStats(
	Game_Menu	*pGameMenu,
	Menu_Button	*pButton,
	int			direction)
{
//	free(GameData.Data);
//	GameData.Data=	NULL;

	//
	//go back a menu
	pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
}

void UpdateDisplayPlayerStats(
	Game_Menu	*pGameMenu,
	Menu_Button	*pButton,
	int			direction)
{
/*
	char	CurData[13];

	long	counter=0,
			curCounter=0;

	long	checking=0;
	long	wordCounter=0;

	long	yPos=	60;//start in pixels to draw on y

	//
	//store the offsets in an array of values
	long	offset[7]=	{	25,		160,
							240,	320,
							400,	480,
							560};	//offsets on the screen

	char	titles[7][30]=	{
							"NAME",
							"HIGHEST   SCORE",
							"GAMES     PLAYED",
							"GAMES     WON",
							"HIGHEST   COMBO",
							"HIGHEST   LINKER",
							"TOTAL     CLUSTERS"
							};
	long	done=0;
	RECT	rc;


	if(!GameData.Data)
	{
		if(!checking)
		{
			//*FIX*
//			GameData.DBase.GetPlayerHighScoreDataFromServer(App.pDP->szShortName);
			checking=1;
		}
		return;
	}

	checking=0;

	rc.top=		yPos;
	rc.right=	rc.top + 40;

	//
	//print the titles at the top of screen
	if(!App.UserSettings.RenderMode)
	{
		for(counter=0;counter < 7;++counter)
		{
			rc.left=	offset[counter];
			rc.right=	rc.left + 100;

			//
			//draw title to screen
			App.pDDraw->DDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter]);
		}
	}
	else
	{	//***OPENGL MODE***
		for(counter=0;counter < 7;++counter)
		{
			rc.left=	offset[counter];
			rc.right=	rc.left + 100;

			//
			//draw title to screen
			App.pOpenGL->GLDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter]);
		}
	}

	yPos+=60;

	//
	//print the titles at the top of screen
	counter=0;
	while(!done)
	{
		if((GameData.Data[counter] != ' ') &&
			(GameData.Data[counter] != '\0'))
		{
			CurData[curCounter]=	GameData.Data[counter];
			++curCounter;
		}
		else
		{
			if(GameData.Data[counter] == '\0')
				done=1;

			CurData[curCounter]=0;

			if(!App.UserSettings.RenderMode)
			{
				//
				//draw text in propor position
				App.pDDraw->DDrawText(
					offset[wordCounter],//x start
					yPos,//y start
					TF_LENGTH_STATS,//width of screen, colour
					CurData);
			}
			else
			{	//***OPENGL MODE***
				App.pOpenGL->GLDrawText(
					offset[wordCounter],//x start
					yPos,//y start
					TF_LENGTH_STATS,//width of screen, colour
					CurData);
			}

			memset(CurData,0,sizeof(char)*13);
			curCounter=0;
			++wordCounter;

			if(GameData.Data[counter] == '\n')
			{
				yPos+=32;
				++counter;
			}
		}

		++counter;
	}
	*/

}

void LaunchServerStats(
	Game_Menu *pGameMenu,
	Menu_Button *pButton,
	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_SERVER_SCORES);
}

void LaunchPlayerStats(
	Game_Menu *pGameMenu,
	Menu_Button *pButton,
	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_PLAYER_SCORES);
}

void LaunchPlayerStatsSinglePlayer(
	Game_Menu *pGameMenu,
	Menu_Button *pButton,
	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_PLAYER_SCORES_SINGLE_PLAYER);
}

void GoBack(
	Game_Menu *pGameMenu,
	Menu_Button *pButton,
	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_SINGLE_GAME_SELECT);
}

void LaunchMultiStart(
	Game_Menu *pGameMenu,
	Menu_Button *pButton,
	int direction)
{
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	//Create game button has been selected
	int		data = DP_MSG_START_GAME;

	//need to send group message to launch game
	App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&data, sizeof(data));


	App.pDP->hostflag = 1;

	buttonArray[53].Disable(0);
	buttonArray[54].Disable(0);

	//invisible buttons
	buttonArray[55].Disable(0);
	buttonArray[56].Disable(0);
	buttonArray[57].Disable(0);
	buttonArray[58].Disable(0);
	buttonArray[59].Disable(0);

	//Setup our game
	GameData.GameStateFlags |= GAME_STATE_SWITCHING;
	GameData.GameStateFlags |= GAME_STATE_VERSUS_MULTI;

}

void LaunchChannelList (Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_CHANNEL_LIST);

	App.pDP->ChannelList.ClearList();
	
	App.pDP->lpDP->EnumGroups(App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);

	App.pDP->lpDP->EnumGroupsInGroup(App.pDP->dpidRootRoom,App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);

	//	App.pDP->lpDP->EnumGroupPlayers(App.pDP->dpidRoom,App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);

	App.pDP->pCurChannel = App.pDP->ChannelList.pListHead;
}

void UpdateChannelList (Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	//display our text
	if(!App.UserSettings.RenderMode)
	{
		App.pDDraw->DDrawText(175,80, TF_FONT_LARGE | TF_SPACING_3, "ENTER CHANNEL:");
		App.pDDraw->DDrawText(175,150, TF_FONT_LARGE | TF_SPACING_3, App.pDP->pCurChannel->name);
	}
	else
	{
		App.pOpenGL->GLDrawText(175,80, TF_FONT_LARGE | TF_SPACING_3, "ENTER CHANNEL:");
		App.pOpenGL->GLDrawText(175,150, TF_FONT_LARGE | TF_SPACING_3, App.pDP->pCurChannel->name);

	}
}

void LaunchNextButton (Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	if(App.pDP->pCurChannel->pNext)
		App.pDP->pCurChannel = App.pDP->pCurChannel->pNext;
	else
		App.pDP->pCurChannel = App.pDP->ChannelList.pListHead;
}

void LaunchOkChannelButton (Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	RECT rcSelect, rcDeSelect;

	//check to see if there is room
	if(App.pDP->pCurChannel->dpid == App.pDP->dpidRootRoom)
	{
		App.pDP->lpDP->DeletePlayerFromGroup(App.pDP->dpidRoom, App.pDP->dpid);
		App.pDP->lpDP->AddPlayerToGroup(App.pDP->pCurChannel->dpid, App.pDP->dpid);
		App.pDP->dpidRoom = App.pDP->pCurChannel->dpid;

		pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
	}
	else
	{
		//enumerate players in the group
		App.pDP->numPlayers	= 0;
		App.pDP->PlayerList.ClearList();

		App.pDP->lpDP->EnumGroupPlayers(App.pDP->dpidRoom,App.pDP->SessionHead.pStart->pGuid,EnumPlayersCallback, App.pDP, 0);

		if(App.pDP->numPlayers < 4)
		{
			//ok to add me to the group
			App.pDP->lpDP->DeletePlayerFromGroup(App.pDP->dpidRoom, App.pDP->dpid);
			App.pDP->lpDP->AddPlayerToGroup(App.pDP->pCurChannel->dpid, App.pDP->dpid);
			App.pDP->dpidRoom = App.pDP->pCurChannel->dpid;

			pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
		}
		else
		{
			//no room, return to the previous room
			pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
		}
	}
	//reset clip rect for start game
	if(App.pDP->dpidRoom == App.pDP->dpidRootRoom)
	{
		//we are in the main lobby, grey out button
		rcSelect.left		= 160;
		rcSelect.top		= 56;
		rcSelect.right		= 240;
		rcSelect.bottom		= 84;
		rcDeSelect.left		= 160;
		rcDeSelect.top		= 56;
		rcDeSelect.right	= 240;
		rcDeSelect.bottom	= 84;
	}
	else
	{
		//we are in a private room, enable button
		rcSelect.left		= 80;
		rcSelect.top		= 0;
		rcSelect.right		= 160;
		rcSelect.bottom		= 28;
		rcDeSelect.left		= 80;
		rcDeSelect.top		= 0;
		rcDeSelect.right	= 160;
		rcDeSelect.bottom	= 28;
	}

	//now set the clipping rects
	pGameMenu->buttonArray[38].SetClipSelect(rcSelect);
	pGameMenu->buttonArray[38].SetClipDeselect(rcDeSelect);

}


void LaunchSinglePuzzleGameSelectButton(Game_Menu *pGameMenu, Menu_Button *pButton,	int direction)
{
	
	Menu_Button	*buttonArray	= pGameMenu->GetButtonArray();
	
	buttonArray[30].Disable(2);
	buttonArray[31].Disable(4);
	buttonArray[32].Disable(6);
	buttonArray[33].Disable(8);
	
	pGameMenu->SwitchMenu(MENU_ID_SINGLE_PUZZLE_SCREEN);
}

void LaunchSingleTimeTrialButton(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	//Prepare to Setup our game
	GameData.GameStateFlags |= GAME_STATE_SWITCHING;
	GameData.GameStateFlags |= GAME_STATE_SINGLE_TTRIAL;
}

void LaunchSingleNewPuzzleButton(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	//Setup our game
	GameData.GameStateFlags |= GAME_STATE_SWITCHING;
	GameData.GameStateFlags |= GAME_STATE_SINGLE_PUZZLE;

}


void LaunchSingleEnterPasswordPuzzleButton(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	pGameMenu->SwitchMenu(MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY);
}

void UpdateSinglePuzzlePasswordButton(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{
	U32*	pKeys = App.pdistruct->curKeyState;

	if(!(pKeys[DIK_TAB] == 1 || pKeys[DIK_SPACE] == 1))
		ChatInput(pButton->GetTextString(), 12);
	
}

void LaunchSinglePuzzlePasswordButton(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction)
{

	GameData.GameStateFlags |= GAME_STATE_SWITCHING;
	GameData.GameStateFlags |= GAME_STATE_SINGLE_PUZZLE;
}

void UpdateDisplayPlayerStatsSinglePlayer(Game_Menu*	pGameMenu, 
										  Menu_Button*	pButton, 
										  int			direction)
{
	long	counter		= 0,
			curCounter	= 0;
	long	yPos		= 60;//start in pixels to draw on y
	
	//store the offsets in an array of values
	long	offset[7] = {	25, 
							50,		140,
							240,	340,
							440,	540
						};	//offsets on the screen
	
	char	titles[7][64] = {	"#",
								"NAME",
								"TOTAL     SCORE",
								"LONGEST   STREAK",
								"TOTAL&HI  LINKER",
								"TOTAL&HI  COMBO",
								"TOTAL&HI  CLUSTER"
							};
	char	strName[256];

	RECT	rc;
	
	rc.top	  =	yPos;
	rc.bottom =	rc.top + 40;
	
	//
	//print the titles at the top of screen
	if(!App.UserSettings.RenderMode)
	{
		for(counter=0;counter < 7;++counter)
		{
			rc.left=	offset[counter];
			rc.right=	rc.left + 100;
			//
			//draw title to screen
			App.pDDraw->DDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter]);
		}
	}
	else
	{	//***OPENGL MODE***

		for(counter = 0; counter < 7; ++counter)
		{
			rc.left  = offset[counter];
			rc.right = rc.left + 100;
			//
			//draw title to screen
			App.pOpenGL->GLDrawText(
				&rc,
				TF_COLOR_LIGHT_BLUE,//width of screen, colour
				titles[counter] );
		}
	}

	yPos += 50;

	char		strBuffer[256];
	u32			color;
	CRecord*	pCurRecord;
	

	//List the Top Ten
	for(u32 i=0; i < 10; i++)
	{
		//list the records
		
		pCurRecord = GameData.DBase.GetRecord( i );

		if(pCurRecord == NULL || pCurRecord->m_iTotalScore == 0)
			continue;
		
		//	"NAME"
		//	"HIGHEST   SCORE"
		//	"AVERAGE   SCORE"
		//	"HIGHEST   COMBO"
		//	"HIGHEST   LINKER"
		//	"TOTAL     CLUSTERS"
		
		//Name can only be 8 characters on the top 10 list
		strcpy(strName, pCurRecord->m_strName);
		if(strlen(strName) > 8 )
			strName[8] = '\0';
	
		for(u32 j=0; j < 7; j++)
		{
			switch(j)
			{ 
				case 0: sprintf(strBuffer, "%d",	i+1);							break;
				case 1: sprintf(strBuffer, "%s",	strName);						break;
				case 2: sprintf(strBuffer, "%d",	pCurRecord->m_iTotalScore);		break;
				case 3: sprintf(strBuffer, "%0.02f",pCurRecord->m_fLongestStreak);	break;
				case 4: sprintf(strBuffer, "%d,%d",	pCurRecord->m_iTotalLinkers,	pCurRecord->m_iHighestLinker);	break;
				case 5: sprintf(strBuffer, "%d,%d",	pCurRecord->m_iTotalCombos,		pCurRecord->m_iHighestCombo);	break;
				case 6: sprintf(strBuffer, "%d,%d",	pCurRecord->m_iTotalClusters,	pCurRecord->m_iHighestCluster);	break;
				default:	strcpy(strBuffer, "COUNTER TOO HIGH");
			}
			
			if(j == 0)
				color = TF_COLOR_YELLOW;
			else if( i % 2)
				color = TF_COLOR_GREEN;
			else
				color = TF_COLOR_WHITE;
			
			
			//draw our buffer
			if(!App.UserSettings.RenderMode)
			{
				//draw text in propor position
				App.pDDraw->DDrawText(
					offset[j],				//x start
					yPos,					//y start
					TF_LENGTH_STATS | color,//width of screen, colour
					strBuffer);
			}
			else
			{	//***OPENGL MODE***
				App.pOpenGL->GLDrawText(
					offset[j],				//x start
					yPos,					//y start
					TF_LENGTH_STATS | color,//width of screen, colour
					strBuffer);
			}
		}
		yPos += 30;
	}

}
		
void LaunchCreditsButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	// Another hack - for some reason this launch is called when
	// you are already in the credits screen - which is bad
	if(!(pGameMenu->GetMenuID() == MENU_ID_CREDITS_SCREEN))
	{
		GameData.keysDown[VK_RETURN] = 0;
		pGameMenu->SwitchMenu(MENU_ID_CREDITS_SCREEN);
	}

	pGameMenu->buttonArray[27].Disable(6);
//	pGameMenu->buttonArray[28].Disable(4);	// Multi player button
	pGameMenu->buttonArray[29].Disable(2);

	pButton->cursorPos.x = -50;
	pButton->cursorPos.y = -50;
	
}

void UpdateCreditsButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction)
{
	if(pGameMenu->GetMenuID() == MENU_ID_CREDITS_SCREEN)
	{
		//*** Draw our Credits ***
		if(!App.UserSettings.RenderMode)
		{
			//*** DDRAW ***
			App.pDDraw->DDrawText(50,   60, TF_FONT_LARGE, "TEAM MATRIX IS");
			App.pDDraw->DDrawText(100, 120, TF_COLOR_LIGHT_BLUE, "TIM  BERRY");
			App.pDDraw->DDrawText(120, 135, TF_LENGTH_STATS, "-IT'S CRUNCH TIME AND I'M GOING SKIING!");
			App.pDDraw->DDrawText(100, 170, TF_COLOR_LIGHT_BLUE, "ELI  EMERSON");
			App.pDDraw->DDrawText(120, 185, TF_LENGTH_STATS, "-I COMMENTED OUT THE MAIN LOOP AND IT STILL WORKED.");
			App.pDDraw->DDrawText(100, 220, TF_COLOR_LIGHT_BLUE, "ANDY KAPLAN");
			App.pDDraw->DDrawText(120, 235, TF_LENGTH_STATS, "-OK.... I'M GOING OUT TO THE SHOOTING RANGE. ");
			App.pDDraw->DDrawText(100, 270, TF_COLOR_LIGHT_BLUE, "RYAN KOHLER");
			App.pDDraw->DDrawText(120, 285, TF_LENGTH_STATS, "-WELL, SEE YOU GUYS TODAY.");
			App.pDDraw->DDrawText(100, 320, TF_COLOR_LIGHT_BLUE, "GREG MORCHOWER");
			App.pDDraw->DDrawText(120, 335, TF_LENGTH_STATS, "-IT'S NOT \"VERTEXES\"");
			App.pDDraw->DDrawText(100, 370, TF_COLOR_LIGHT_BLUE, "MAX  SZLAGOR");
			App.pDDraw->DDrawText(120, 385, TF_LENGTH_STATS, "-YOU WANT SOME OF THIS?!");
			App.pDDraw->DDrawText(100, 420, TF_COLOR_BLUE, "MUSIC BY %C1DOUG QUINN");
			App.pDDraw->DDrawText(120, 434, TF_LENGTH_STATS, "-WWW.MP3.COM\\DOUGNOTRONIK");
		}
		else
		{
			//*** OPEN_GL ***
			App.pOpenGL->GLDrawText(50,   60, TF_FONT_LARGE, "TEAM%C1 MATRIX%C0 IS");
			App.pOpenGL->GLDrawText(100, 120, TF_COLOR_LIGHT_BLUE, "TIM  BERRY");
			App.pOpenGL->GLDrawText(120, 135, TF_LENGTH_STATS, "-IT'S CRUNCH TIME AND I'M GOING SKIING!");
			App.pOpenGL->GLDrawText(100, 170, TF_COLOR_LIGHT_BLUE, "ELI  EMERSON");
			App.pOpenGL->GLDrawText(120, 185, TF_LENGTH_STATS, "-I COMMENTED OUT THE MAIN LOOP AND IT STILL WORKED.");
			App.pOpenGL->GLDrawText(100, 220, TF_COLOR_LIGHT_BLUE, "ANDY KAPLAN");
			App.pOpenGL->GLDrawText(120, 235, TF_LENGTH_STATS, "-OK.... I'M GOING OUT TO THE SHOOTING RANGE. ");
			App.pOpenGL->GLDrawText(100, 270, TF_COLOR_LIGHT_BLUE, "RYAN KOHLER");
			App.pOpenGL->GLDrawText(120, 285, TF_LENGTH_STATS, "-WELL, SEE YOU GUYS TODAY.");
			App.pOpenGL->GLDrawText(100, 320, TF_COLOR_LIGHT_BLUE, "GREG MORCHOWER");
			App.pOpenGL->GLDrawText(120, 335, TF_LENGTH_STATS, "-IT'S NOT \"VERTEXES\"");
			App.pOpenGL->GLDrawText(100, 370, TF_COLOR_LIGHT_BLUE, "MAX  SZLAGOR");
			App.pOpenGL->GLDrawText(120, 385, TF_LENGTH_STATS, "-YOU WANT SOME OF THIS?!");
			App.pOpenGL->GLDrawText(100, 420, TF_COLOR_BLUE, "MUSIC BY %C1DOUG QUINN");
			App.pOpenGL->GLDrawText(120, 434, TF_LENGTH_STATS, "-WWW.MP3.COM\\DOUGNOTRONIK");


		
		}

		//if(App.pdistruct->curKeyState[DIK_RETURN] == 1)
		if(GameData.keysDown[VK_RETURN])
	//	if(App.pdistruct->keys[DIK_RETURN] & 0x80 )
		{
			// HACK - for making sure we get the enter key properly
			GameData.keysDown[VK_RETURN] = 0;
			pButton->enabledPos.x	= 340 ;
			pButton->enabledPos.y	= 375 ;
			pButton->cursorPos.x	= 298 ;
			pButton->cursorPos.y	= 375 ;

			pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);
		}
	}
}