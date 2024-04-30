// This is the "DDraw Particle system"
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <math.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"

#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"





//#define GL_ARB_IMAGING
#define INTENSITY_WAIT_TIME	10

int ddUpdateSpark(P_DD_PARTICLE pParticle);

extern GAME_DATA GameData;
extern APP	App;

P_DD_PARTICLE tagAPI_DDraw::ddCreateParticle(int PosX, int PosY, float VelX, float VelY,
											 int Health, int Type)
{
	P_DD_PARTICLE  pParticle;

	//add our Particle to the list
	pParticle = CreateParticleNode(pTailParticle);
	
	if (pParticle == NULL)
	{
		MessageBox(NULL, "Failed to createParticle", "Damn!", MB_OK);
		return NULL;
	}
	
	pParticle->pUpdate		= ddUpdateSpark;	//all sparks use UpdateSpark
	
	pParticle->PosX			= PosX;
	pParticle->PosY			= PosY;

	pParticle->VelX			= VelX;
	pParticle->VelY			= VelY;

	

	//pParticle->pDDSurf		= pDDSurf;

	SetRect(&pParticle->ClipRect, 14*Type, 118, 14+(14*Type), 134);


	pParticle->Health		= Health;
	//pParticle->Intensity	= Intencity;
	pParticle->TimeLeft		= INTENSITY_WAIT_TIME;
	
	return pParticle;
	
}//end CreateSpark

int ddUpdateSpark(P_DD_PARTICLE pParticle)
{
	
//	if(pParticle->TimeLeft <= 0)
//	{
//		if(pParticle->Intensity > 1)
//			pParticle->Intensity -= .5;
//		
//		pParticle->TimeLeft = INTENSITY_WAIT_TIME;
//	}
//	else
//		pParticle->TimeLeft--;
	
	//*** Apply Gravity
	pParticle->VelY += 0.1f;
	
	
	//*** If our particle hits the wall or floor lets make em bounce in the opposite 
	// direction... cuz that looks cool!
	
	
	//if(pParticle->PosX >= 630 || pParticle->PosX <= 10)
	//	pParticle->VelX *= -1;
	
	//if(pParticle->PosY >= 470 || pParticle->PosY <= 10)
	//	pParticle->VelY *= -1;
	
	if( pParticle->Health > 0)
		pParticle->Health--;
	else
		//Kill thing
	{
		pParticle->pUpdate	= NULL;
		pParticle->Health  = -1;
	}
	
	return 1;
	
}//end UpdateSpark

int tagAPI_DDraw::UpdateParticleEngine()
{

	P_DD_PARTICLE	pCurrParticle, *ppStartParticle;	
	//Go through each node in the list invoking its state function
	ppStartParticle = &pHeadParticle;

	//two pointers will step through our list
	while ((pCurrParticle = *ppStartParticle) != NULL)
	{
		if(pCurrParticle->pUpdate) //if true we need to update
		{
			//if Particle is non static(or moving and changing) (triggers are static)
			if( pCurrParticle->pUpdate(pCurrParticle) ) 
			{
				pCurrParticle->PosX += (int)pCurrParticle->VelX;
				pCurrParticle->PosY += (int)pCurrParticle->VelY;
			}//end if (we need to blit)
		}// end if (we need to update)
		
		//***If pCurrParticle->Health < 0 delete it, else go to the next Particle
		if(pCurrParticle->Health < 0)
		{
			*ppStartParticle = DeleteParticleNode(pCurrParticle);
		}
		else
			ppStartParticle = &pCurrParticle->pNext;
	}
	return 1;

}//end UpdateObject

int tagAPI_DDraw::DrawParticleEngine()
{

	P_DD_PARTICLE	pCurrParticle, *ppStartParticle;	
	//Go through each node in the list invoking its state function
	ppStartParticle = &pHeadParticle;

	//two pointers will step through our list
	while ((pCurrParticle = *ppStartParticle) != NULL)
	{
		//**************DRAW OUR PARTICLE TEXTURE IN CCW************
		lpDDSBack->BltFast(pCurrParticle->PosX, pCurrParticle->PosY,
			App.pDDraw->surfaceArray[9], &pCurrParticle->ClipRect,
			DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
		//**************************************************

		ppStartParticle = &pCurrParticle->pNext;
	}
	return 1;

}//end UpdateObject

void tagAPI_DDraw::CleanUpParticleEngine()
{
	//******Enable Blending and TExture mapping

	P_DD_PARTICLE	pCurrParticle, *ppStartParticle;	
	//Go through each node in the list invoking its state function
	ppStartParticle = &pHeadParticle;
	//two pointers will step through our list
	while ((pCurrParticle = *ppStartParticle) != NULL)
	{
		//***This will delete our current particle and return the next particle
			*ppStartParticle = DeleteParticleNode(pCurrParticle);
		//}
		//else
		//	ppStartParticle = &pCurrParticle->pNext;
	}


}//end UpdateObject

P_DD_PARTICLE tagAPI_DDraw::CreateParticleNode (P_DD_PARTICLE pParticlePos)
{
	P_DD_PARTICLE pNewParticle;
	
	pNewParticle = (P_DD_PARTICLE) malloc( sizeof(DD_PARTICLE));
	
	if (pNewParticle == NULL)
		return NULL;
	
	ZeroMemory(pNewParticle, sizeof( DD_PARTICLE ) );

	//*** If there is a Particle passed, create this Particle as the Particle
	//	  after the passed Particle(node)
	if (pParticlePos)
	{
		pNewParticle->pPrev = pParticlePos;
		pNewParticle->pNext = pParticlePos->pNext;
		
		if (pParticlePos->pNext && pParticlePos != pTailParticle)
			pParticlePos->pNext->pPrev = pNewParticle;
		else 
			pTailParticle = pNewParticle;
		
		pParticlePos->pNext = pNewParticle;
	}
	else if (!pHeadParticle)  // if this is the first Particle created pHeadParticle will == NULL
	{
		pNewParticle->pNext = NULL;
		pNewParticle->pPrev = NULL;
		
		pHeadParticle = pNewParticle;
		pTailParticle = pNewParticle;
	}
	else // noParticle passed, create this Particle at the beginning of the list
	{
		pNewParticle->pNext			= pHeadParticle;
		pNewParticle->pPrev			= NULL;
		pHeadParticle->pPrev	= pNewParticle;
		pHeadParticle			= pNewParticle;
	}
	
	iTotalParticles++;
	
	return pNewParticle;
}

P_DD_PARTICLE tagAPI_DDraw::DeleteParticleNode(P_DD_PARTICLE pParticlePos)
{
	//Just in case we tried to Del when there is no particles
	if(iTotalParticles <= 0)
		return 0;
	
	P_DD_PARTICLE pNext = pParticlePos->pNext;
	
	//*** If the ParticlePos's Next is a valid Particle, 
	//  make the next Particle's prev = the deleted Particles' prev 
	//	(hence cutting out the middle "Particle")
	if (pParticlePos->pNext)
	{
		pNext = pParticlePos->pNext;
		pParticlePos->pNext->pPrev = pParticlePos->pPrev;
	}
	else
	{
		pNext = NULL;
		pTailParticle = pParticlePos->pPrev;
	}
	
	// *** same Particle as above just in the oposite direction in the linked list
	if (pParticlePos->pPrev)
		pParticlePos->pPrev->pNext = pParticlePos->pNext;
	else 
		pHeadParticle = pParticlePos->pNext;
	
	
	//***If the Particle we deleted was the tail return NULL for our update loop
	free(pParticlePos);
	iTotalParticles--;
	return pNext;
}