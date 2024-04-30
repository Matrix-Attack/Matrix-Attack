#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <dsound.h>
#include <stdio.h>
#include <string>

using namespace std;

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "dsound.h"


#include "text.h"

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


extern APP App;
extern GAME_DATA GameData;
//extern DSound Sound;
extern CSound cSound;

#ifndef GL_SURF_GAME_PARTICLE_RED_TEXID
#define GL_SURF_GAME_PARTICLE_RED_TEXID			21
#endif

#ifndef SOUND
#define SFX_STREAK1	 11
#define SFX_STREAK2	 12
#define SFX_STREAK3	 13
#define SFX_STREAK4	 14
#define SFX_STREAK5	 15
#define SFX_STREAK6	 16
#define SFX_STREAK7	 17
#endif

#include "GLSurfDef.h"

void GetRandomBackground( void );

void tagScore::CreateStreakSparks( void )
{
	//Random numbers between 0 and 1.
	//float fRand1 = ((float)rand() /(float)RAND_MAX);
	//Random number between -1 and 1.
	//float fRand2 = (((float)rand()/(float)RAND_MAX) * 2) - 1.0f;
	
	if(App.UserSettings.RenderMode)
	{
		//OPENGL		
		//						   PosX,PosY, PosZ,   VelX, VelY, VelZ, Int, Hp, Tex
		App.pOpenGL->glCreateSpark(-150, 175, 640.0f,-1.0f, -1.0f, 0.0f, 5+4*(iStreakLevel), 30+iStreakLevel, GL_SURF_GAME_PARTICLE_RED_TEXID);
		App.pOpenGL->glCreateSpark(-140, 175, 640.0f, 0.0f, -1.0f, 0.0f, 5+4*(iStreakLevel), 30+iStreakLevel, GL_SURF_GAME_PARTICLE_RED_TEXID);
		App.pOpenGL->glCreateSpark(-130, 175, 640.0f, 1.0f, -1.0f, 0.0f, 5+4*(iStreakLevel), 30+iStreakLevel, GL_SURF_GAME_PARTICLE_RED_TEXID);
		
	}
	else
	{
		//DDRAW	
		//						   PosX,PosY, PosZ,   VelX, VelY, VelZ, Int, Hp, Tex
		App.pDDraw->ddCreateParticle(80, 105,  1, -1, 50, 0);
		App.pDDraw->ddCreateParticle(80, 105,  0, -1, 50, 0);
		App.pDDraw->ddCreateParticle(80, 105, -1, -1, 50, 0);
	}
}

void tagScore::UpdateStreakFX( float fDeltaTime )
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

	s_fBeatUpdateTime += fDeltaTime;
	
	if( s_fBeatUpdateTime >= s_fBeatRate )
	{
		//Reset our beatUpdateTime counter
		s_fBeatUpdateTime = 0.0f;
		
		//Test to see if we went up a streakLevel (which increases the beat rate and plays a sound)
		switch( iStreakLevel )
		{	
			//LEVEL 0 - 8
			case 0:	if(fStreakTime >= 4.0f)  { s_fBeatRate = 1.0f;  Points += 50;	iStreakLevel++; } break;
			case 1:	if(fStreakTime >= 7.0f)  { s_fBeatRate = 0.7f;  Points += 100;	cSound.m_cStaticSoundList[SFX_STREAK1].Play(); iStreakLevel++; } break;
			case 2:	if(fStreakTime >= 12.0f) { s_fBeatRate = 0.5f;  Points += 200;	cSound.m_cStaticSoundList[SFX_STREAK2].Play(); iStreakLevel++; } break;
			case 3:	if(fStreakTime >= 18.0f) { s_fBeatRate = 0.2f;  Points += 400;	cSound.m_cStaticSoundList[SFX_STREAK3].Play(); iStreakLevel++; } break;
			case 4:	if(fStreakTime >= 24.0f) { s_fBeatRate = 0.1f;	Points += 800;	cSound.m_cStaticSoundList[SFX_STREAK4].Play(); iStreakLevel++; } break;
			case 5:	if(fStreakTime >= 28.0f) { s_fBeatRate = 0.08f; Points += 1000; cSound.m_cStaticSoundList[SFX_STREAK5].Play(); iStreakLevel++; } break;
			case 6:	if(fStreakTime >= 32.0f) { s_fBeatRate = 0.05f; Points += 1200; cSound.m_cStaticSoundList[SFX_STREAK6].Play(); iStreakLevel++; } break;
			case 7:	if(fStreakTime >= 40.0f) { s_fBeatRate = 0.02f; Points += 1400; cSound.m_cStaticSoundList[SFX_STREAK7].Play(); iStreakLevel++; } break;
			case 8:	if(fStreakTime >= 50.0f) { s_fBeatRate = 0.01f; Points += 2000; cSound.m_cStaticSoundList[SFX_STREAK7].Play(); 
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

void tagScore::AddCluster(unsigned long numClusters)
{
	if(GameData.Score.bGameFinished)
		return;

	char	Buffer[64];
	int		iNewPoints = 0;

	TotalClusters	+= numClusters;
	CurrClusters	= numClusters;
	
	if(numClusters <= 5)
		iNewPoints		+= numClusters * 10;
	else if(numClusters == 6)
		iNewPoints		+= numClusters * 20;
	else if(numClusters == 7)
		iNewPoints		+= numClusters * 40;
	else if(numClusters == 8)
		iNewPoints		+= numClusters * 50;
	else if(numClusters == 9)
		iNewPoints		+= numClusters * 60;
	else
		iNewPoints		+= numClusters * 70;
	
	//*** Add more points depending upon the streak time
	iNewPoints += (int)(fStreakTime * 2.0f);
	
	//*** See if this is our highest cluster for this game.
	if(numClusters > HighestCluster)
		HighestCluster = numClusters;
	

	//Update the database
//	GameData.DBase.UpdateScore( iNewPoints );
	GameData.DBase.UpdateCluster( numClusters );
	
	//Update score
	Points += iNewPoints;

	sprintf(Buffer, "CLUSTER X%%c1%d%%c0!", numClusters );
	
	Menu_Button	*ClusterButton	= GameData.pGameMenu->buttonArray + CLUSTER_BUTTON;

	ClusterButton->SetTextString(Buffer);
	ClusterButton->Enable(0);

	if(ClusterButton->screenPos.x > 0)
		ClusterButton->screenY = 1;
	
	if(numClusters >= 9)
		cSound.m_cStaticSoundList[SFX_CLUSTER3].Play();
	else if(numClusters >= 7)
		cSound.m_cStaticSoundList[SFX_CLUSTER2].Play();
	else if(numClusters >= 5)
		cSound.m_cStaticSoundList[SFX_CLUSTER1].Play();

	//Put a diffrent background if the player is getting allot of points! 
	// Doug's idea!
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
	{
		if( Points >= 25000 )
		{
			//little treat if you get this high!
			GetRandomBackground();
		}
		else if(Points >= 20000)
		{
			GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND2_TEXID;
		}
		else if(Points >= 15000)
		{
			GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND1_TEXID;
		}
		else if(Points >= 10000)
		{
			GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND4_TEXID;
		}
		else if(Points >= 6000)
		{
			GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND3_TEXID;
		}
		else if(Points >= 2000)
		{
			GameData.pGameMenu->iBackgroundTexID = GL_SURF_MENU_BACKGROUND2_TEXID;
		}
	}

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		GameData.simplePlayers[GameData.playerId].score = Points;

		PLAYER_UPDATE_SCORE PlayerScore;

		PlayerScore.dpMsgData	= DP_MSG_SEND_SCORE;
		PlayerScore.id			= GameData.playerId;
		PlayerScore.score		= Points;

		App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&PlayerScore, sizeof(PLAYER_UPDATE_SCORE));
	}
}


void tagScore::ResetCurrClusters()
{
	CurrClusters = 0;
}


void tagScore::AddCombo(unsigned long numCombos)
{
	if(GameData.Score.bGameFinished)
		return;

	char Buffer[64];
	int		iNewPoints = 0;

	TotalCombos		+= numCombos;
	CurrCombos		+= numCombos;
	iNewPoints		+= CurrCombos * 50;

	if(CurrCombos > HighestCombo)
		HighestCombo = CurrCombos;

	//Update the database
//	GameData.DBase.UpdateScore( iNewPoints );
	GameData.DBase.UpdateCombo( CurrCombos );
	
	//Update the score
	Points += iNewPoints;
	
	sprintf(Buffer, "COMBO X%%c1%d%%c0!!", CurrCombos );
	
	Menu_Button	*ComboButton	= GameData.pGameMenu->buttonArray  + COMBO_BUTTON;

	ComboButton->SetTextString(Buffer);
	ComboButton->Enable(0);

	if(ComboButton->screenPos.x > 0)
		ComboButton->screenY -= 5;

//	if(CurrCombos >= 13)
//		cSound.m_cStaticSoundList[SFX_COMBO3].Play();
//	else if(CurrCombos >= 8)
//		cSound.m_cStaticSoundList[SFX_COMBO3].Play();
	if(CurrCombos >= 6)
		cSound.m_cStaticSoundList[SFX_COMBO3].Play();
	else if(CurrCombos >= 3)
		cSound.m_cStaticSoundList[SFX_COMBO2].Play();
	else if(CurrCombos >= 1)
		cSound.m_cStaticSoundList[SFX_COMBO1].Play();
	
	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		GameData.simplePlayers[GameData.playerId].score = Points;

		PLAYER_UPDATE_SCORE PlayerScore;

		PlayerScore.dpMsgData	= DP_MSG_SEND_SCORE;
		PlayerScore.id			= GameData.playerId;
		PlayerScore.score		= Points;


		App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&PlayerScore, sizeof(PLAYER_UPDATE_SCORE));
	}
}

void tagScore::ResetCurrCombos()
{
	CurrCombos = 0;
}

void tagScore::AddLinker(unsigned long numLinkers)
{
	if(GameData.Score.bGameFinished)
		return;

	char	Buffer[64];
	int		iNewPoints = 0;

	TotalLinkers	+= numLinkers;
	CurrLinkers		+= numLinkers;
	
	iNewPoints		+= CurrLinkers * 250;
	
	if(CurrLinkers > HighestLinker)
		HighestLinker = CurrLinkers;
	
	//Update the database
//	GameData.DBase.UpdateScore( Points );
	GameData.DBase.UpdateLinker( CurrLinkers );
	
	//Update score
	Points += iNewPoints;

	sprintf(Buffer, "LINKER X%%c1%d%%c0!!!", CurrLinkers );
	
	Menu_Button	*LinkerButton	= GameData.pGameMenu->buttonArray  + LINKER_BUTTON;

	LinkerButton->SetTextString(Buffer);
	LinkerButton->Enable(0);

	if(LinkerButton->screenPos.x > 0)
		LinkerButton->screenY -= 5;
	
	if(CurrLinkers >= 4)
		cSound.m_cStaticSoundList[SFX_LINKER3].Play();
	else if(CurrLinkers >= 2)
		cSound.m_cStaticSoundList[SFX_LINKER2].Play();
	else if(CurrLinkers >= 1)
		cSound.m_cStaticSoundList[SFX_LINKER1].Play();


	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		GameData.simplePlayers[GameData.playerId].score = Points;

		PLAYER_UPDATE_SCORE PlayerScore;

		PlayerScore.dpMsgData = DP_MSG_SEND_SCORE;
		PlayerScore.id = GameData.playerId;
		PlayerScore.score = Points;


		App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&PlayerScore, sizeof(PLAYER_UPDATE_SCORE));
	}
		
}

void tagScore::ResetCurrLinkers()
{

	CurrLinkers = 0;

}

void tagScore::UpdateTimer( int bCountDown )
{ 
	static DWORD dwTimer	= 0;
	static int bTime		= 0;
	static int bCountdown	= 0; //0 means we are counting up

	//**************DRAW THE TIMER*********************
	dwCurrTime = timeGetTime();
	if(!bTime)
	{
		bTime	= TRUE;
		dwTimer = dwCurrTime ;
	//	Timer.minutes = 2;
	}
	else
	{
		if((timeGetTime() - dwTimer) > 1000)
		{
			dwTimer = timeGetTime();
			//***************
			if(!bCountDown)
			{
				if(seconds > 58)
				{
					minutes++;
					seconds = 0;
				}
				else
					seconds++;
			}
			else
			{
				if(minutes >= 0)
				{
					if(seconds == 0 && minutes > 0)
					{
						minutes--;
						seconds = 59;
					}
					else if(seconds > 0)
						seconds--;
				}
			}
			//****************
		}
	}

	DrawTimer( bCountDown );

	//***Update the Timer info (seconds and minutes)
	
}

void tagScore::DrawTimer( int bCountDown )
{
	char szMsg[100];

	if(seconds > 9 && minutes > 9)
		sprintf(szMsg, "%d:%d", minutes, seconds);
	else if(seconds > 9 && minutes < 10)
		sprintf(szMsg, "0%d:%d", minutes, seconds);
	else if(minutes > 9 && seconds < 10)
		sprintf(szMsg, "%d:0%d", minutes, seconds);
	else
		sprintf(szMsg, "0%d:0%d", minutes, seconds);

	if( !App.UserSettings.RenderMode )
	{
		if(bCountDown && minutes == 0 && seconds <= 10 )
		{
			//LAST 10 Seconds!!!
			App.pDDraw->DDrawText(300, 50,	TF_SPACING_0, "TIMER");// Draw the timer text
			
			if(seconds == 10)
				App.pDDraw->DDrawText(308, 61, TF_SPACING_0 | TF_FONT_LARGE, "%d", seconds);
			else
				App.pDDraw->DDrawText(308, 61, TF_SPACING_0 | TF_FONT_LARGE, "0%d", seconds);
		}
		else
		{
			App.pDDraw->DDrawText(300, 50,	TF_SPACING_0, "TIMER");// Draw the timer text
			App.pDDraw->DDrawText(300, 61,	TF_SPACING_0, szMsg);
		}
	}
	else
	{
		if(bCountDown && minutes == 0 && seconds <= 10 )
		{
			//LAST 10 Seconds!!!
			App.pOpenGL->GLDrawText(310, 50,  TF_SPACING_0, "TIMER");// Draw the timer text
			
			if(seconds == 10)
				App.pOpenGL->GLDrawText(315, 61, TF_SPACING_0 | TF_FONT_LARGE, "%d", seconds);
			else
				App.pOpenGL->GLDrawText(315, 61, TF_SPACING_0 | TF_FONT_LARGE, "0%d", seconds);
		}
		else
		{
			App.pOpenGL->GLDrawText(310, 50,  TF_SPACING_0, "TIMER");// Draw the timer text
			App.pOpenGL->GLDrawText(310, 61, TF_SPACING_0, szMsg);
		}
//		App.pOpenGL->GLDrawText(545, 9,	 TF_SPACING_0, "TIMER");// Draw the timer text
//		App.pOpenGL->GLDrawText(545, 30, TF_SPACING_0, szMsg);
	}
}
