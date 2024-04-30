#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <math.h>
#include <gl/gl.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "font.h"




U32 UpdateTextObject(P_TEXTOBJ t)
{
	
/*	if(t->TimeLeft <= 0)
	{
		if(t->Size < 20)
			t->Size += 1;
		
		t->TimeLeft = CHARSIZE_WAIT_TIME;
	}
	else
		t->TimeLeft--;
*/	
	//*** Apply Velocity
	t->XPos += t->XVel;
	t->YPos += t->YVel;

	//******* SCALING FLAGS *******
	if(t->TypeFlags & TF_USING_SCALING)
	{
		t->Size += t->SizeVel;
		if(t->Size <= 5)
		{
			//Kill The text object if we cant see it anyway
			t->pUpdate	= NULL;
			t->Health = -1;
		}
	}

	if(t->TypeFlags & TF_WARBLING)
	{

		if(t->Size >= 40)
			t->SizeVel = -0.09f;
		else if(t->Size <= 30)
			t->SizeVel = 0.09f;
	}

	if(t->TypeFlags & TF_ITALIC_WARBLING)
	{
		if(t->Italic >= 10)
			t->ItalicVel = -0.1f;
		else if(t->Italic <= -10)
			t->ItalicVel = 0.1f;

		t->Italic += t->ItalicVel;
	}

	//******* FADING FLAGS **********
	if(t->TypeFlags & TF_USING_FADING)
		if(t->TypeFlags & TF_FADING)
		{
			t->fgColor[3] -= 0.002f;
			t->gdColor[3] -= 0.002f;
		}
		else if(t->TypeFlags & TF_FADING_FAST)
		{
			t->fgColor[3] -= 0.003f;
			t->gdColor[3] -= 0.003f;
		}
		
		if(t->fgColor[3] <= 0.0f)
		{
			//Kill The text object if we cant see it anyway
			t->pUpdate	= NULL;
			t->Health = -1;
		}
	
	

/*	

  	if(t->TypeFlagsFlags & TF_WARBLING)
	{
		//t->XCenter	= (t->Size*strlen(String))*0.5; //"*0.5" is faster than "/2"
		//Size = t->XCenter;
		t->XPos += ((t->Size*strlen(String))*0.5);
	}
	else
		Size = t->Size;

	if(t->TypeFlags & TF_WARBLING)
	{
		static float ColorVel = 0.01f;

		if(t->fgColor[0] >= 1.0)
			ColorVel = -0.1f;
		else if(t->fgColor[0] <= -0.1)
			ColorVel = 0.1f;

		t->fgColor[0] += ColorVel;
	}
*/
//	t->YPos += t->YVel;
	//t->SizeVel	-= 0.01f;

	/*
	if(t->Size >= 30)
		t->SizeVel = -0.1;
	else if (t->Size <= 5)
		t->SizeVel = 0.1;

	App.pFont->SetSize(t->Size);
*/
	
	//****************BOUNCE TEXT AROUND THE SCREEN *********************8
	//*** If our text hits the wall or floor lets make em bounce in the opposite 
	// direction... cuz that looks cool!
	if(t->XPos >= 630 || t->XPos <= 10)
		t->XVel *= -0.5;
	
	if(t->YPos >= 470 || t->YPos <= 10)
		t->YVel *= -0.5;
	
	if( t->Health > 0)
		t->Health--;
	else
		//Kill thing
	{
		t->pUpdate	= NULL;
		t->Health  = -1;
	}

	return 1;
	
}//end UpdateText