// This is the "OpenGL Particle system"
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>

#include <math.h>
#include <gl/gl.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"

//#define GL_ARB_IMAGING
#define INTENSITY_WAIT_TIME	5

int glUpdateSpark(P_GL_PARTICLE pParticle);
int glUpdateSpiralSpark(P_GL_PARTICLE pParticle);
void SpringThing(float&	w1Coeff, float&	w2Coeff, float&	tCoeff, float w1Offset, 
				 float w2Offset, float tOffset, float& t, int cycles, int width, int length);

extern APP App;

P_GL_PARTICLE tagAPI_OpenGL::glCreateSpark(float PosX, float PosY, float PosZ,
										   float VelX, float VelY, float VelZ,
										   float Intencity, int Health, int TexID)
{
	P_GL_PARTICLE  pParticle;

	//add our Particle to the list
	pParticle = CreateParticle(pTailParticle);
	
	if (pParticle == NULL)
	{
		MessageBox(NULL, "Failed to createParticle", "Damn!", MB_OK);
		return NULL;
	}
	
	pParticle->pUpdate		= glUpdateSpark;	//all sparks use UpdateSpark
	
	pParticle->PosX			= PosX;
	pParticle->PosY			= PosY;
	pParticle->PosZ			= PosZ;
	pParticle->VelX			= VelX;
	pParticle->VelY			= VelY;
	pParticle->VelZ			= VelZ;
	
	pParticle->TexID		= TexID;

	pParticle->Health		= Health;
	pParticle->Intensity	= Intencity;
	pParticle->TimeLeft		= INTENSITY_WAIT_TIME;
	
	//0xAARRGGBB
	//	pParticle->argb			= 0xFF0000FF; //255 Blue
	
	return pParticle;
	
}//end CreateSpark

P_GL_PARTICLE tagAPI_OpenGL::glCreateSpiralSpark(float PosX, float PosY, float PosZ,
												 float Intencity, int Health, int TexID)
{
	P_GL_PARTICLE  pParticle;

	//add our Particle to the list
	pParticle = CreateParticle(pTailParticle);
	
	if (pParticle == NULL)
	{
		MessageBox(NULL, "Failed to createParticle", "Damn!", MB_OK);
		return NULL;
	}
	
	pParticle->pUpdate		= glUpdateSpiralSpark;	//all sparks use UpdateSpark
	
	pParticle->PosX			= PosX;
	pParticle->PosY			= PosY;
	pParticle->PosZ			= PosZ;
	pParticle->VelX			= 0;
	pParticle->VelY			= 0;
	pParticle->VelZ			= 0;
	
	pParticle->TexID		= TexID;

	pParticle->Health		= Health;
	pParticle->Intensity	= Intencity;
	pParticle->TimeLeft		= INTENSITY_WAIT_TIME;
	
	//0xAARRGGBB
	//	pParticle->argb			= 0xFF0000FF; //255 Blue
	
	return pParticle;
	
}//end CreateSpark


int glUpdateSpark(P_GL_PARTICLE pParticle)
{
	
	if(pParticle->TimeLeft <= 0)
	{
		if(pParticle->Intensity > 1)
			pParticle->Intensity -= 1;
		
		pParticle->TimeLeft = INTENSITY_WAIT_TIME;
	}
	else
		pParticle->TimeLeft--;
	
	//*** Apply Gravity
	pParticle->VelY -= 0.01f;

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


int glUpdateSpiralSpark(P_GL_PARTICLE pParticle)
{

	
/*	if(pParticle->TimeLeft <= 0)
	{
		//if(pParticle->Intensity > 1)
		//	pParticle->Intensity -= 1;
		pParticle->TimeLeft = INTENSITY_WAIT_TIME;
	}
	else
		pParticle->TimeLeft--;
	
*/
	//*** Apply Spiral
	//Richard's gonna hook us up with some spiral code
	SpringThing(pParticle->PosZ, pParticle->PosX, pParticle->PosY,
		0, -100, 0, pParticle->Step, 10, 150, 500);
/*	
	if( pParticle->Health > 0)
		pParticle->Health--;
	else
		//Kill thing
	{
		pParticle->pUpdate	= NULL;
		pParticle->Health  = -1;
	}
*/	
	return 1;
	
}//end UpdateSpark



int tagAPI_OpenGL::glUpdateParticleEngine()
{
	//******Enable Blending and TExture mapping
	P_GL_PARTICLE	pCurrParticle, *ppStartParticle;	
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
				pCurrParticle->PosX += pCurrParticle->VelX;
				pCurrParticle->PosY += pCurrParticle->VelY;
				pCurrParticle->PosZ += pCurrParticle->VelZ;

			}//end if (we need to blit)
		}// end if (we need to update)
		
		//***If pCurrParticle->Health < 0 delete it, else go to the next Particle
		if(pCurrParticle->Health < 0)
		{
			*ppStartParticle = DeleteParticle(pCurrParticle);
		}
		else
			ppStartParticle = &pCurrParticle->pNext;
	}
	return 1;

}//end UpdateObject


int tagAPI_OpenGL::glDrawParticleEngine()
{
	//******Enable Blending and TExture mapping
	//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR );
	glEnable (GL_TEXTURE_2D);			// enable texture mapping 

	P_GL_PARTICLE	pCurrParticle, *ppStartParticle;	
	//Go through each node in the list invoking its state function
	ppStartParticle = &pHeadParticle;

	//two pointers will step through our list
	while ((pCurrParticle = *ppStartParticle) != NULL)
	{
		//if(pCurrParticle->PosX < 0 ||  pCurrParticle->PosX > 640 || 
		//	pCurrParticle->PosY < 0 || pCurrParticle->PosY > 480 )
		//	pCurrParticle->Health = -1;
		//else
		//{
	
		//**************DRAW OUR PARTICLE TEXTURE IN CCW************
		glBindTexture (GL_TEXTURE_2D, pCurrParticle->TexID);	// bind to our texture, has id of 13 
		glBegin (GL_QUADS);
	
			glTexCoord2f (0.0f,0.0f);		// lower left corner of image
			glVertex3f (pCurrParticle->PosX - pCurrParticle->Intensity,
				pCurrParticle->PosY +80 + pCurrParticle->Intensity,
				pCurrParticle->PosZ -640);
			
			glTexCoord2f (0.0f, 1.0f);		// upper left corner of image
			glVertex3f (pCurrParticle->PosX - pCurrParticle->Intensity,
				pCurrParticle->PosY +80 - pCurrParticle->Intensity,
				pCurrParticle->PosZ -640);
			
			glTexCoord2f (1.0f, 1.0f);		// upper right corner of image
			glVertex3f (pCurrParticle->PosX + pCurrParticle->Intensity,
				pCurrParticle->PosY +80 - pCurrParticle->Intensity,
				pCurrParticle->PosZ -640);
			
			glTexCoord2f (1.0f, 0.0f);		// lower right corner of image
			glVertex3f (pCurrParticle->PosX + pCurrParticle->Intensity,
				pCurrParticle->PosY +80 + pCurrParticle->Intensity,
				pCurrParticle->PosZ -640);

		glEnd ();
		//**************************************************

		ppStartParticle = &pCurrParticle->pNext;
	}
	//*******Disable the Gl settings for our particles
	glDisable (GL_TEXTURE_2D); // disable texture mapping 
	return 1;

}//end UpdateObject

void tagAPI_OpenGL::glCleanUpParticleEngine()
{
	//******Enable Blending and TExture mapping

	P_GL_PARTICLE	pCurrParticle, *ppStartParticle;	
	//Go through each node in the list invoking its state function
	ppStartParticle = &pHeadParticle;
	//two pointers will step through our list
	while ((pCurrParticle = *ppStartParticle) != NULL)
	{
		//***This will delete our current particle and return the next particle
			*ppStartParticle = DeleteParticle(pCurrParticle);
		//}
		//else
		//	ppStartParticle = &pCurrParticle->pNext;
	}


}//end UpdateObject


P_GL_PARTICLE tagAPI_OpenGL::CreateParticle (P_GL_PARTICLE pParticlePos)
{
	P_GL_PARTICLE pNewParticle;
	
	pNewParticle = (P_GL_PARTICLE) malloc( sizeof(GL_PARTICLE));
	
	if (pNewParticle == NULL)
		return NULL;
	
	ZeroMemory(pNewParticle, sizeof( GL_PARTICLE ) );

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

P_GL_PARTICLE tagAPI_OpenGL::DeleteParticle(P_GL_PARTICLE pParticlePos)
{
	//Just in case we tried to Del when there is no particles
	if(iTotalParticles <= 0)
		return 0;
	
	P_GL_PARTICLE pNext = pParticlePos->pNext;
	
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


//SpringThing(lt->location.j, lt->location.i, lt->location.k,
//			200, 0, 0, step[i], 4, 100, 80);

/*****************************( SpringThing )*****************************\

	Author:		Richard Johnson						Creation:	12/08/99

	Modifier:										Updated:	

	Parameters: float&, float&, float&, float&, float&, float&,
				float&, int, int, int

	Returns:	void

\*************************************************************************/
void SpringThing(float&			w1Coeff,
				 float&			w2Coeff,
				 float&			tCoeff,
				 float			w1Offset,
				 float			w2Offset,
				 float			tOffset,
				 float&			t,
				 int			cycles,
				 int			width,
				 int			length)
{
	float	sint = App.Sin[(int)t];
	float	r = 1 - (sint * sint);
	int		a = (int)(sint * 180 * cycles);


	while(a > 359) a %= 360;
	while(a < 0)   a += 360;

	w1Coeff = (float)(width * r * App.Cos[a] + w1Offset);
	w2Coeff = (float)(width * r * App.Sin[a] + w2Offset);
	tCoeff	= (float)(length * sint + tOffset);

	t += 0.25f;

	if(t >= 360) t = 0;

} /* SpringThing */
