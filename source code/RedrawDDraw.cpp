#include <ddraw.h>
#include <stdio.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <stdio.h>

#include "./mmgr.h"

///#include "database.h"
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




extern APP			App;
extern GAME_DATA	GameData;
extern DSound		Sound;
void	ChatInput(char *Buffer, unsigned int maxChar);


//DWORD WINAPI RedrawScreen_DDraw(LPVOID Parm)
DWORD WINAPI tagAPI_DDraw::RedrawScreen(LPVOID Parm)
{	
	
	
	if(GameData.GameStateFlags & GAME_STATE_MENU)
	{
		//Draw menu

		//*** FIX CHECK FOR BUGS *** -eli
		//GameData.pGameMenu->Update();
		GameData.pGameMenu->Draw_MenuMode();
	}
	else if(GameData.GameStateFlags & GAME_STATE_GAME)
	{
		//Temp for now
		ClearSurface(lpDDSBack, 0 );
		
		//***This needs to happen every frame so we have a "fresh" pointer.
		PrepFrame(); //locks the backbuffer surface
		
		//*** Draw the board
		GameData.pBoard->Draw_DDraw();
		
		//*** Draw the particels over the board
		App.pDDraw->DrawParticleEngine();
		
		//***This has to be AFTER we draw our board and particles
		
		//*** FIX CHECK FOR BUGS *** -eli
		//GameData.pGameMenu->UpdateGameButtons();
		GameData.pGameMenu->Draw_GameMode();

		//This needs to happen every frame so the Screen pointer is free.
		InterruptFrame(); //unlocks the backbuffer surface
	}

	//***Flip our buffers
	FlipBuffers();

	return 0;

}