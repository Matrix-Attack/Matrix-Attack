#include <windows.h>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <time.h>
#include <stdio.h>


//#include "database.h"
#include "./mmgr.h"

#include "DPlay.h"
#include "app.h"
#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"

#include "vector.h"

#include "model.h"
#include "gem3d.h"
#include "defines.h"

#include "GLSurfDef.h"


extern APP App;

static U16 gemColorTable[8][6]	=
			{	0xD800, 0xE800, 0xB000, 0x6800, 0x6000, 0x9000,		// RED
				0xDEE0, 0xEFA0, 0xB5A0, 0x6B60, 0x62E0, 0x94A0,		// YELLOW
				0x06E0, 0x07A0, 0x05A0, 0x0360, 0x02E0, 0x04A0,		// GREEN
				0x06FB, 0x07BD, 0x05B6, 0x036D, 0x02EC, 0x04B2,		// CYAN
				0xA81B, 0xB81D, 0x8816, 0x500C, 0x400C, 0x7012,		// PURPLE
				0x001B, 0x001D, 0x0016, 0x000D, 0x000C, 0x0012,		// BLUE
				0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,		// WHITE
				0x738E, 0x7BCF, 0x5ACB, 0x39C7, 0x3186, 0x4A49, };	// GREY

#define GEM_COLOR_GREY 7

extern GAME_DATA GameData;
extern APP		 App;

void Gem3D::Clear()
{
	bDisplay2D			= 0;			// if true, display the gem as a sprite
	pGemSurf			= NULL;
	pBackSurf			= NULL;
	stateFlags			= 0;			// describes the current state of the gem
	m_iDetailLevel		= 0;			// specifies a lo-detail model (fewer polygons)
							// or a hi-detail model (more polygons)
	degreesFlipped		= 0;		// number of degrees the gem has rotated during
							// a flip
	frameNo				= 0;
	yoinkTimeLeft		= 0;
	linkerTimeLeft		= 0;
	clearTimeLeft		= 0;
	velocity.Clear();			// x-, y-, z-velocity of the gem
	targetPoint.Clear();		// when a gem is moving, it seeks out this point

	pNeighbor[5]		= NULL;		// pointers to neighboring gems
	pNeighbor[4]		= NULL;		// pointers to neighboring gems
	pNeighbor[3]		= NULL;		// pointers to neighboring gems
	pNeighbor[2]		= NULL;		// pointers to neighboring gems
	pNeighbor[1]		= NULL;		// pointers to neighboring gems
	pNeighbor[0]		= NULL;		// pointers to neighboring gems
	pSwitchPartner		= NULL;		// when a gem is switched, stores the address of
	pBody				= NULL;		// contains the actual 3D information
}

void Gem3D::Init( float x, float y, float z, const P_S8 filename, const P_S8 modelName)
{
	Destroy();
	
	//Set the default detail level
	m_iDetailLevel = GEM_36POLY_MESH;

	pBody = new Object3D(x, y, z, filename, modelName);
}

void Gem3D::Destroy()
{
	if(pBody)
	{
		pBody->Destroy();
		delete pBody;
	}
	Clear();
}



int Gem3D::CheckForMatches(int &numMatches, int whichSide)
{
	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_ALREADY_CHECKED);
	
	
	for(U32 i = 0; i < 6; i++)
	{
		Gem3D	*pNextGem	= pNeighbor[i];
		
		if( (pNextGem) && (pNextGem->color[whichSide] == color[whichSide]) &&
			(pNextGem->IsCheckable()) && pNextGem->color[0] != GEM_COLOR_GREY)
		{
			numMatches++;
			pNextGem->CheckForMatches(numMatches, whichSide);
		}
	}
	
	return numMatches;
}




void Gem3D::FlagTheMatches()
{
	// temp
	frameNo			= 0;
	clearTimeLeft	= 100;

	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_CLEARING);
	
	//***Create some special effects using the particle engine
	if(App.UserSettings.bParticles)
		CreateSparks(0);

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		if(GameData.playerId == associatedPlayerID)
		{
			PLAYER_GEM_CLEAR plGemClear;

			plGemClear.id			= GameData.playerId;
			plGemClear.dpMsg		= DP_MSG_GEM_CLEAR;
			plGemClear.RowGem		= row;
			plGemClear.ColumnGem	= column;
			plGemClear.hitDamage	= 5;

			App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&plGemClear,
								sizeof(PLAYER_GEM_CLEAR));
		}
	}

	for(U32 i = 0; i < 6; i++)
	{
		Gem3D	*pNextGem	= pNeighbor[i];

		if( (pNextGem) && (pNextGem->color[0] == color[0]) && !(pNextGem->IsClearing()) )
		{
			pNextGem->associatedPlayerID	= associatedPlayerID;
			pNextGem->FlagTheMatches();
		}
	}
}




// if bFront is true, set the color for the front side.
// if false, set the color for the back side.
void Gem3D::GetBaseColor(int whichSide)
{
	U16	base			= (U16)(rand() % 6);

	(whichSide == GEM_FRONT_HALF) ?	activeColor[0] = color[0] = base :
									activeColor[1] = color[1] = base ;

//	(whichSide == GEM_FRONT_HALF) ?	color[0] = base :
//									color[1] = base ;
}

void Gem3D::SetBaseColor(int whichSide, int Color)
{
	(whichSide == GEM_FRONT_HALF) ?	activeColor[0] = color[0] = Color :
									activeColor[1] = color[1] = Color ;

}

int Gem3D::GetNumMatches( int iCurNumMatches, int whichSide )
{
	bChecked = true;
	
	//Go through all neighbors and see if any match our color
	for(U32 i = 0; i < 6; i++)
	{
		Gem3D	*pNextGem	= pNeighbor[i];
		
		if( (pNextGem)										&& 
			(!pNextGem->bChecked)							&&
			(pNextGem->color[whichSide] == color[whichSide])&&
			(pNextGem->color[0] != GEM_COLOR_GREY)			&&
			(pNextGem->IsCheckable() )						)
		{
			++iCurNumMatches;
			iCurNumMatches += pNextGem->GetNumMatches(iCurNumMatches, whichSide);
		}
	}
	return iCurNumMatches;
}

void Gem3D::CorrectColor( int iNumMatchesAllowed )
{
	//*** We should test to see if this color already creates a cluster.
	// If it does we should select a new one instead.
	bool bClusterFound	= true;
	int	 iNumMatches	= 0;

	while( bClusterFound )
	{		
		//Check to see if there are any matches
		iNumMatches = 0;
		iNumMatches = GetNumMatches(iNumMatches, 0 );
		
		//If the number of matches is >= 4 then we have a cluster
		bClusterFound = (iNumMatches >= iNumMatchesAllowed) ? true : false ;

		//If cluster is found assign a new color
		if(bClusterFound)
		{
			//We found a cluster so get a new random color
			activeColor[0] = color[0] = (U16)(rand() % 6);
		}
	}
}

// sets all the faces of one half of a gem to a certain color.
// if bFront is true, set the colors for the gem's front side.
// if false, set the colors for the back side
inline void Gem3D::SetFaceColors( int whichSide )
{
	Face3D	*pFace	= pBody->FaceList;
	
	
	switch( m_iDetailLevel )
	{
		case GEM_12POLY_MESH:
			if(whichSide == GEM_FRONT_HALF)
			{
				for(int i = 0; i < 6; i++)
					pFace[i].baseColor	= gemColorTable[activeColor[0]][i];
			}

			else
			{
				for(int i = 6; i < 12; i++)
					pFace[i].baseColor	= gemColorTable[activeColor[1]][i - 6];
			}
			break;

		default:
		case GEM_36POLY_MESH:
			if(whichSide == GEM_FRONT_HALF)
			{
				for(int i = 0; i < 6; i++)
					pFace[i].baseColor	= gemColorTable[activeColor[0]][i];
			}
			else
			{
				for(int i = 6; i < 12; i++)
					pFace[i].baseColor	= gemColorTable[activeColor[1]][i - 6];
			}
			break;
	}
}


void Gem3D::Draw_DDraw()
{
	//Draw gem on primary side (left)
	pBody->Draw_DDraw();

	//Draw gem on secondary side (right)
	Draw2D(GEM_BACK_HALF);
}

void Gem3D::Draw_OpenGL()
{
	pBody->Draw_OpenGL();
}

void Gem3D::Update( float tDeltaTime )
{
	SetFaceColors(GEM_FRONT_HALF);
	SetFaceColors(GEM_BACK_HALF);
	
	if( IsLinkerOn() )
	{
		UpdateLinker( associatedPlayerID );
	}

	if( IsSwitching() )
	{
		UpdateSwitch( tDeltaTime );
	}
	else if( IsFlipping() )
	{
		UpdateFlip();
	}
	else if( IsReadyToCheck() && GameData.playerId == associatedPlayerID)
	{
		if(--yoinkTimeLeft < 0)
		{
			int	numMatches	= 0;

			if(CheckForMatches(numMatches, 0) >= 4)
			{
				GameData.Score.AddCluster(numMatches+1);
				
				if(bStartingAChain)
				{
					bStartingAChain = 0;
				}
				
				FlagTheMatches();

				if( IsLinkerEnabled() )
				{
					if( IsLinkerOn() )
						GameData.Score.AddLinker(1);

					AddStateFlags(GEM_FLAG_LINKER_ON);
					linkerTimeLeft	= 100;
				}

				// increase the player's combo count
				else if(bAutoFlipOn)
				{
					GameData.Score.AddCombo(1);
					bAutoFlipOn = 0;
				}
			}
			// the gem checked for matches with other gems,
			// but FAILED!!! HAHAHAHAHAHAAAAA!! >=D
			else
			{
				associatedPlayerID	= GEM_PLAYERID_NOBODY;
				activeColor[0]		= color[0];
				activeColor[1]		= color[1];
				UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
			}
		}
	}
	else if( IsClearing() && !IsLinkerOn() )
	{
		UpdateClear();
	}
	else if( !IsLinkerOn() )
	{
		activeColor[0]	= color[0];
		activeColor[1]	= color[1];
		SetStateFlags(0);
	}
	
	//Update the rotation of the gem
	if(App.UserSettings.RenderMode)
	{
		//OpenGL rotation
		pBody->Update_OpenGL();
	}
	else
	{
		//DirectDraw rotation
		pBody->Update_DDraw();
	}
}



// direction		this is a value 0-3, which describes one of
//					four directions to switch the gem
//
// switchSpeed		this should be some number between 0 and 1.
//					the closer this value is to 1, the faster the
//					gems will switch with each other
void Gem3D::PrepareSwitch(int playerID, int direction, float switchSpeed)
{
	Gem3D		*pSwitchGem			= pNeighbor[direction];

	Vector3D	center1				= *(GetCenterPos()),
				center2				= *(pSwitchGem->GetCenterPos()),
				velocity			= (center2 - center1) * switchSpeed;

	// prepare the original gem for switching
	SetVelocity(velocity);
	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_SWITCHING);
	SetTargetPoint(center2);
	pSwitchPartner					= pSwitchGem;
	associatedPlayerID				= playerID;

	// prepare the gem's neighbor gem for switching
	pSwitchGem->SetVelocity(-velocity);
	pSwitchGem->UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	pSwitchGem->AddStateFlags(GEM_FLAG_SWITCHING);
	pSwitchGem->SetTargetPoint(center1);
	pSwitchGem->pSwitchPartner		= this;
	pSwitchGem->associatedPlayerID	= playerID;
}




inline void Gem3D::UpdateSwitch( float tDeltaTime )
{
	Vertex3D	*vList1			= pBody->VertexList;

	for(U32 i = 0; i < pBody->GetNumVertices(); i++)
	{
		if(App.UserSettings.RenderMode)
		{
			//openGl doesn't have a problem with this
			(vList1 + i)->worldDef	+= velocity * tDeltaTime;//0.013f;	//avg time delta
			(vList1 + i)->worldCur	+= velocity * tDeltaTime;//0.013f;	//avg time delta
		}
		else
		{
			(vList1 + i)->worldDef	+= velocity * tDeltaTime;//0.013f;	//avg time delta
			(vList1 + i)->worldCur	+= velocity * tDeltaTime;//0.013f;	//avg time delta
		}
	}
	
	//						  ! <- returns the magnitute.
	float	distToTarget	= !(targetPoint - (vList1)->worldCur);
	
	
	// if the gem is within a reasonable distance from its
	// target point, complete the switch
	//
	if(GameData.GameStateFlags & GAME_STATE_SINGLE_GAME )
	{
		if(distToTarget < 4.0)
		{
			CompleteSwitch( associatedPlayerID );
			
		}
		else if(distToTarget > 35.0f) //starting distance is about 23-30
		{
			//*** FIX *** this is a patch im thowing in.
			//This is to stop a wierd bug I found. -eli
			// Where gems go flying off in diffrent directions
			CompleteSwitch( associatedPlayerID );
		}
	}
	// Multiplayer
	else
	{
		if(distToTarget < 4.0)
		{
			// crappy band-aid for non-host computers that happen to be
			// faster than the host
			if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
			{
				Vertex3D	*vList1			= pBody->VertexList,
							*vList2			= pSwitchPartner->pBody->VertexList;
				Vector3D	switchTarget	= pSwitchPartner->targetPoint;

				(vList1 + 0)->worldCur	= (vList1 + 0)->worldDef	= targetPoint;
				(vList2 + 0)->worldCur	= (vList2 + 0)->worldDef	= switchTarget;
				for(U32 i = 1; i < pBody->GetNumVertices(); i++)
				{
					Vertex3D	*v1	= vList1 + i,
								*v2	= vList2 + i;
					v1->worldCur	= v1->worldDef	= v1->objDef + targetPoint;
					v2->worldCur	= v2->worldDef	= v2->objDef + switchTarget;
				}
			}

			if(GameData.playerId == associatedPlayerID)
			{
				CompleteSwitch(associatedPlayerID);
			}
		}
	}
} 




inline void Gem3D::CompleteSwitch( int playerID )
{
	Vertex3D	*vList1			= pBody->VertexList,
				*vList2			= pSwitchPartner->pBody->VertexList;

	Vector3D	switchTarget	= pSwitchPartner->targetPoint;
	
	// once the switch is complete, return each gem to its original position.
	// (this way, we don't have to update the pointer info for neighboring gems).
	//
	(vList1 + 0)->worldCur	= (vList1 + 0)->worldDef	= switchTarget;
	(vList2 + 0)->worldCur	= (vList2 + 0)->worldDef	= targetPoint;
	
	for(U32 i = 1; i < pBody->GetNumVertices(); i++)
	{
		Vertex3D	*v1	= vList1 + i,
					*v2	= vList2 + i;

		v1->worldCur	= v1->worldDef	= v1->objDef + switchTarget;
		v2->worldCur	= v2->worldDef	= v2->objDef + targetPoint;
	}


	// now, simply swap the colors of the two gems to complete
	// the illusion of switching
	SWAP(color[0],		 pSwitchPartner->color[0]);
	SWAP(color[1],		 pSwitchPartner->color[1]);
	SWAP(activeColor[0], pSwitchPartner->activeColor[0]);
	SWAP(activeColor[1], pSwitchPartner->activeColor[1]);
	SWAP(stateFlags,	 pSwitchPartner->stateFlags);
	SWAP(linkerTimeLeft, pSwitchPartner->linkerTimeLeft);
	
	SetFaceColors(GEM_FRONT_HALF);
	SetFaceColors(GEM_BACK_HALF);

	pSwitchPartner->SetFaceColors(GEM_FRONT_HALF);
	pSwitchPartner->SetFaceColors(GEM_BACK_HALF);
	
	
	// flag the gems for color matching
	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_READY_TO_CHECK | GEM_FLAG_LINKER_ENABLED);
	
	pSwitchPartner->UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	pSwitchPartner->AddStateFlags(GEM_FLAG_READY_TO_CHECK | GEM_FLAG_LINKER_ENABLED);
	
	
	// flag the gems for color matching
	if(GameData.playerId == playerID)
	{
//		UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
//		AddStateFlags(GEM_FLAG_READY_TO_CHECK | GEM_FLAG_LINKER_ENABLED);
	
//		pSwitchPartner->UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
//		pSwitchPartner->AddStateFlags(GEM_FLAG_READY_TO_CHECK | GEM_FLAG_LINKER_ENABLED);

		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI && GameData.playerId == associatedPlayerID)
		{
			PLAYER_COMPLETE_SWITCH plCSwitch;

			plCSwitch.id		= GameData.playerId;
			plCSwitch.RowGem	= row;
			plCSwitch.ColumnGem = column;
			plCSwitch.dpMsg		= DP_MSG_COMPLETE_SWITCH;
			
			App.pDP->lpDP->Send( App.pDP->dpid,App.pDP->dpidRoom,
								 DPSEND_GUARANTEED,&plCSwitch,
							     sizeof(PLAYER_COMPLETE_SWITCH) );
		}
	}

	//switch our player's pointer to gem to the gem we are switching with.

	//*** this code is SO SHITTY......just like everything else in 
	//			this pile of crap code we call a game. -eli (5-13-01)
	switch(GameData.pBoard->player.switchdirection)
	{
		//UP (FIX)
		case 0:	GameData.pBoard->player.pGem = this;		break;
		//DOWN
		case 1:	GameData.pBoard->player.pGem = pNeighbor[1];break;
		//LEFT (FIX)
		case 2:	GameData.pBoard->player.pGem = this;		break;
		//RIGHT
		case 3:	GameData.pBoard->player.pGem = pNeighbor[3];break;
	}
	
	if(GameData.pBoard->player.pGem == NULL)
	{
		//***FIX*** There is a chance that this actually happens.
		//It happened to me once while playing... we need to do somthing about it.
		
		//This code is SO damn crappy... im not even sure where to start.
		//I guess we need a valid gem so i'll do what I can.
		int i = 0;

		while(GameData.pBoard->player.pGem != NULL )
		{	
			GameData.pBoard->player.pGem = pNeighbor[i];
			i++;

			if(i >= 6)
				return; //crappy. (worst ever)
		}
	}
}


void Gem3D::PrepareFlip(int playerID, float flipSpeed)
{
//	SetStateFlags(GEM_FLAG_FLIPPING);
	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_FLIPPING);

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		associatedPlayerID = playerID;
	}
	else
		associatedPlayerID = playerID;

	degreesFlipped	= 0;
}


void Gem3D::UpdateFlip()
{
	//This line of code is gross.. 
	// but what Im trying to achive here if weather or not the player is on this gem.
	// if so then flip the gem faster than normal.
/*	if( this == GameData.pPlayerOnGem )
	{
		degreesFlipped	+= 20.0;
	}
	else*/

		degreesFlipped	+= 15.0;

	SetRotation(0, degreesFlipped, 0);

	if(degreesFlipped > 180.0)
	{
		SetRotation(0, 180.0, 0);

		if(GameData.playerId == associatedPlayerID)
		{
			CompleteFlip(associatedPlayerID);
		}
	}
}

void Gem3D::CompleteFlip(int playerID)
{
	SWAP(color[0], color[1]);
	SWAP(activeColor[0], activeColor[1]);

	SetFaceColors(GEM_FRONT_HALF);
	SetFaceColors(GEM_BACK_HALF);

	SetRotation(0, 0, 0);

	// flag the gem for color matching
	UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
	AddStateFlags(GEM_FLAG_READY_TO_CHECK);

	// If this is the host then we need to send the complete flip message when we are done
	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI && GameData.playerId == playerID)
	{
			PLAYER_COMPLETE_FLIP plCFlip;

			plCFlip.dpMsg = DP_MSG_COMPLETE_FLIP;
			plCFlip.ColumnGem = column;
			plCFlip.RowGem = row;
			plCFlip.id = associatedPlayerID;

			App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&plCFlip,
								sizeof(PLAYER_COMPLETE_FLIP));

	}

	yoinkTimeLeft	= 2;
}

void Gem3D::UpdateLinker(int playerID)
{
	if( (++frameNo) %= 3)
	{
		activeColor[0]	= color[0];
		activeColor[1]	= color[1];
	}
	else
	{
		activeColor[0]	= 6;
		activeColor[1]	= 6;
	}
	if(--linkerTimeLeft < 0)
	{
		activeColor[0]	= color[0];
		activeColor[1]	= color[1];

		if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
			activeColor[0]	= color[0]	= GEM_COLOR_GREY;

//		UnsetStateFlags(GEM_FLAG_LINKER_FLAGS);
		UnsetStateFlags(~GEM_FLAG_SWITCHING);


		if(GameData.playerId == playerID)
		{
			if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
			{
				PLAYER_LINKER_DONE plLinkDone;

				plLinkDone.dpMsg = DP_MSG_UNSET_LINKER;
				plLinkDone.id = GameData.playerId;
				plLinkDone.RowGem = row;
				plLinkDone.ColumnGem = column;

				App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&plLinkDone,
									sizeof(PLAYER_LINKER_DONE));
			}
		}

	}
	SetFaceColors(GEM_FRONT_HALF);
	SetFaceColors(GEM_BACK_HALF);
}


#define COLOR_BRIGHTEN(cVal)													\
		{																		\
			(cVal)	=	(LIMIT( (((cVal) & 0xF800) >> 10), 0, 31) << 11)	|	\
						(LIMIT( (((cVal) & 0x7E0) >> 4), 0, 63) << 5)		|	\
						(LIMIT( (((cVal) & 0x1F) << 1), 0, 31) );											\
		}


void Gem3D::UpdateClear()
{
	if( (++frameNo) &= 1)
	{
		activeColor[0]	= color[0];
		activeColor[1]	= color[1];

		SetFaceColors(GEM_FRONT_HALF);
		SetFaceColors(GEM_BACK_HALF);
	}
	else
	{
		Face3D	*pFace	= pBody->FaceList;

		for(int i = 0; i < 12; i++)
			COLOR_BRIGHTEN(pFace[i].baseColor);
	}

	if( (--clearTimeLeft < 0) )
	{
		if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
		{
			activeColor[0]	= color[0]	= GEM_COLOR_GREY;
			bAutoFlipOn = 1;
			PrepareFlip(GameData.playerId, 0);
		}
		else if(associatedPlayerID == GameData.playerId)
		{
			activeColor[0]	= color[0]	= rand() % 6;
			bAutoFlipOn = 1;
			PrepareFlip(GameData.playerId, 0);

			if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
			{
					PLAYER_UPDATE_GEM pUGem;
		
					pUGem.dsMsgData		= DP_MSG_SEND_NEW_GEM_COLOR;
					pUGem.frontColor	= color[0];
					pUGem.backColor		= color[1];
					pUGem.RowGem		= row;
					pUGem.ColumnGem		= column;
					pUGem.id			= GameData.playerId;
					App.pDP->lpDP->Send(App.pDP->dpid, App.pDP->dpidRoom, DPSEND_GUARANTEED, &pUGem, sizeof(PLAYER_UPDATE_GEM) );
			}
		}
	//	else
	//		activeColor[0]	= color[0]	= rand() % 6;
	}
}

		


void Gem3D::Draw2D(int whichSide)
{
	RECT		clipRect	= { activeColor[whichSide - 1] * 28,
								0, clipRect.left + 28, 31},

				viewPort	= {338, 80, 504, 400};
	int			x			= 422 + (int)pBody->VertexList[0].worldCur.x - 14,
				y			= 240 - (int)pBody->VertexList[0].worldCur.y - 15;
	
	ClipToViewPort(clipRect, viewPort, x, y);
	pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[9], &clipRect, DDBLTFAST_SRCCOLORKEY);
}

void Gem3D::CreateSparks(int whichSide)
{

	if(!App.UserSettings.RenderMode)
	{
		POINT *pPoint = GetScreenCenterPos();
		switch(color[whichSide])
		{
			case 0:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 0);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 0);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 0);				
				break;

			case 1:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 1);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 1);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 1);			
				break;

			case 2:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 2);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 2);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 2);				
				break;

			case 3:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 3);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 3);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 3);				
				break;
			
			case 4:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 4);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 4);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 4);				
				break;

			case 5:
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 5);
				App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 2 - 4), 50, 5);
			//	App.pDDraw->ddCreateParticle(pPoint->x, pPoint->y, (float)(-1 + rand()% 3), (float)(rand()% 5 - 8), 50, 5);				
				break;
		}
	
	
	}
	else	//***OPENGL***
	{
		int Zmod = 1 + (2*GameData.Score.CurrLinkers) + GameData.Score.CurrClusters;


		Vector3D *pV = GetCenterPos();
		switch(color[whichSide])
		{
			case 0:// NOTE TO THOSE WHO DON'T LIKE THE WARNINGS (WE SHOULD MAKE OUR DOUBLES == FLOATS)
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_RED_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_RED_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_RED_TEXID);
				break;

			case 1:
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_YELLOW_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_YELLOW_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_YELLOW_TEXID);
				break;

			case 2:
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_GREEN_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_GREEN_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_GREEN_TEXID);
				break;

			case 3:
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_CYAN_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_CYAN_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_CYAN_TEXID);
				break;
			
			case 4:
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_PURPLE_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_PURPLE_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_PURPLE_TEXID);
				break;

			case 5:
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_BLUE_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_BLUE_TEXID);
				App.pOpenGL->glCreateSpark((float)pV->x, (float)pV->y, (float)pV->z, (float)((rand()% 3)-1), (float)(rand()% 3 ), (float)(rand()% Zmod), 20, 100, GL_SURF_GAME_PARTICLE_BLUE_TEXID);
				break;
		}

	}

}