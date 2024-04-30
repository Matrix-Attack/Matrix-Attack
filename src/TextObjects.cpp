// This is the "OpenGL TEXT system"
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



//***Method Function pointers ***
U32 UpdateTextObject(P_TEXTOBJ t);

P_TEXTOBJ tagAPI_OpenGL::CreateTextObject(float PosX,		//X pixel position
										  float PosY,		//Y pixel position
										  char *String,		//Character String
										  U32 TypeFlags,	//Type Flags
										  U32 ColorFlags)	//Color Flags
{
	P_TEXTOBJ  t;
	
	//add our Text to the list
	t = CreateTextObjectNode(pTailTextObj);
	
	if (t == NULL)
	{
		MessageBox(NULL, "Failed to createText", "www.FixIt.com", MB_OK);
		return NULL;
	}
	
	strcpy(t->String, String );
	if (strlen(String) >= 64)
	{
		MessageBox(NULL, "String larger than 64 characters.", "www.FixIt.com", MB_OK);
		return NULL;
	}

	t->XPos		= PosX;
	t->YPos		= PosY;

	//************************* COLOR FLAGS *******************************

	//*** Set the Solid Color (TOP) ****
	if(ColorFlags & CF_RED)
		t->fgColor[0] = 1.0;
	else if(ColorFlags & CF_HALF_RED)
		t->fgColor[0] = 0.5;

	if(ColorFlags & CF_GREEN)
		t->fgColor[1] = 1.0;
	else if(ColorFlags & CF_HALF_GREEN)
		t->fgColor[1] = 0.5;

	if(ColorFlags & CF_BLUE)
		t->fgColor[2] = 1.0;
	else if(ColorFlags & CF_HALF_BLUE)
		t->fgColor[2] = 0.5;
	
	t->fgColor[3] = FONT_DEFAULT_FALPHA;//Alpha Componet is default to 1.0

	//**** Set the Gradient Color (BOTTOM) ****
	if(ColorFlags & CF_BOT_RED)
		t->gdColor[0] = 1.0;
	else if(ColorFlags & CF_BOT_HALF_RED)
		t->gdColor[0] = 0.5;

	if(ColorFlags & CF_BOT_GREEN)
		t->gdColor[1] = 1.0;
	else if(ColorFlags & CF_BOT_HALF_GREEN)
		t->gdColor[1] = 0.5;

	if(ColorFlags & CF_BOT_BLUE)
		t->gdColor[2] = 1.0;
	else if(ColorFlags & CF_BOT_HALF_BLUE)
		t->gdColor[2] = 0.5;

	t->gdColor[3] = FONT_DEFAULT_GALPHA;//Alpha Componet is default to 1.0


	//************************* TYPE FLAGS *******************************

	t->TypeFlags	= TypeFlags;

	//***** STARTING SIZE FLAGS *****
	if(TypeFlags & TF_LARGE )
		t->Size	= 50;
	else if(TypeFlags & TF_SMALL )
		t->Size	= 10;
	else // if not defined use DEFAULT SIZE
		t->Size	= 20;

	//***** ITALICS FLAGS ******
	if(TypeFlags & TF_ITALICIZED_RIGHT)
		t->Italic		= FONT_DEFAULT_ITALIC;
	else if(TypeFlags & TF_ITALICIZED_LEFT)
		t->Italic		= -FONT_DEFAULT_ITALIC;
	else if(TypeFlags & TF_ITALIC_WARBLING)
		t->ItalicVel	= 0.1f;

	//***** VELOCITY FLAGS ******
	if(TypeFlags & TF_SLIDING_LEFT )	
		if(TypeFlags & TF_FAST)
			t->XVel	= -5;
		else if(TypeFlags & TF_SLOW)
			t->XVel	= -1;
		else
			t->XVel	= -FONT_DEFAULT_SPEED;

	else if(TypeFlags & TF_SLIDING_RIGHT)	
		if(TypeFlags & TF_FAST)
			t->XVel	= 5;
		else if(TypeFlags & TF_SLOW)
			t->XVel	= 1;
		else
			t->XVel	= FONT_DEFAULT_SPEED;
	
	if(TypeFlags & TF_SLIDING_UP )	
		if(TypeFlags & TF_FAST)
			t->YVel	= -5;
		else if(TypeFlags & TF_SLOW)
			t->YVel	= -1;
		else
			t->YVel	= -FONT_DEFAULT_SPEED;
	else if(TypeFlags & TF_SLIDING_DOWN)	
		if(TypeFlags & TF_FAST)
			t->YVel	= 5;
		else if(TypeFlags & TF_SLOW)
			t->YVel	= 1;
		else
			t->YVel	= FONT_DEFAULT_SPEED;

	//***** SCALING FLAGS *****
	if(TypeFlags & TF_WARBLING)	
		if(TypeFlags & TF_FAST)
			t->SizeVel = 0.2f;
		else if(TypeFlags & TF_SLOW)
			t->SizeVel = 0.05f;
		else
			t->SizeVel = 0.1f;
	else if(TypeFlags & TF_GROWING)
		if(TypeFlags & TF_FAST)
			t->SizeVel = 0.2f;
		else if(TypeFlags & TF_SLOW)
			t->SizeVel = 0.05f;
		else
			t->SizeVel = 0.1f;
	else if(TypeFlags & TF_SHRINKING)
	{
		t->Size += 10; //adding 10 so we can see the shrinkage
		if(TypeFlags & TF_FAST)
			t->SizeVel = -0.2f;
		else if(TypeFlags & TF_SLOW)
			t->SizeVel = -0.05f;
		else
			t->SizeVel = -0.1f;
	}

	//***** MISC SETTINGS *****

	//This is a special case that we check for so we can pass the TF_USING_BOT_COLOR flag
	//through our t->TypeFlags var.
	if(ColorFlags & CF_USING_BOT_COLOR)
		t->TypeFlags |= TF_GRADIENT;

	t->Health		= FONT_DEFAULT_HEALTH;
//	t->TimeLeft		= FONT_DEFAULT_TIMELEFT;
	t->pUpdate		= UpdateTextObject;	//all sparks use UpdateText
		
	return t;
	
}//end CreateText

int tagAPI_OpenGL::UpdateTextObjEngine()
{
	P_TEXTOBJ	pCurrText, *ppStartText;	
	//Go through each node in the list invoking its state function
	ppStartText = &pHeadTextObj;

	//*********SETUP OPENGL FOR TEXT OBJECTS *************
		//*** save the opengl setup 
		SetOpenGLModes (FONT_GET_MODES);
    
		//*** Set up openGL so that we can draw text
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		//glOrtho(0, ViewPort[2], 0, ViewPort[3], -1, 1);
		glOrtho(0, 640, 0, 480, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glBindTexture(GL_TEXTURE_2D, pFont->texID);
	//*****************************************************
	
	

	//two pointers will step through our list
	while ((pCurrText = *ppStartText) != NULL)
	{
		if(pCurrText->pUpdate) //if true we need to update
		{
			//if Text is non static(or moving and changing) (triggers are static)
			if( pCurrText->pUpdate(pCurrText) ) 
			{
				pFont->DrawStringFromTextObj(pCurrText);
				
			}//end if (we need to blit)

		}// end if (we need to update)
		
		//***If pCurrText->Health < 0 delete it, else go to the next Text
		if(pCurrText->Health < 0)
		{
			*ppStartText = DeleteTextObjectNode(pCurrText);
		}
		else
			ppStartText = &pCurrText->pNext;
	}

	//Put font back to normal
	pFont->Reset();

	//**********PUT OPENGL BACK TO WHAT IT WAS **************
		//get rid of our openGL settings
		glMatrixMode (GL_PROJECTION);
		glPopMatrix ();
		glMatrixMode (GL_MODELVIEW);
		glPopMatrix ();
		//*** save the opengl setup 
		SetOpenGLModes (FONT_RESTORE_MODES);

	//*******************************************************
	
	return 1;

}//end UpdateObject

P_TEXTOBJ tagAPI_OpenGL::CreateTextObjectNode(P_TEXTOBJ pTextPos)
{
	P_TEXTOBJ pNewTextObject;
	
	pNewTextObject = (P_TEXTOBJ) malloc( sizeof(TEXTOBJ));
	
	if (pNewTextObject == NULL)
		return NULL;
	
	ZeroMemory(pNewTextObject, sizeof( TEXTOBJ ) );

	//*** If there is a Text passed, create this Text as the Text
	//	  after the passed Text(node)
	if (pTextPos)
	{
		pNewTextObject->pPrev = pTextPos;
		pNewTextObject->pNext = pTextPos->pNext;
		
		if (pTextPos->pNext && pTextPos != pTailTextObj)
			pTextPos->pNext->pPrev = pNewTextObject;
		else 
			pHeadTextObj = pNewTextObject;
		
		pTextPos->pNext = pNewTextObject;
	}
	else if (!pHeadTextObj)  // if this is the first Text created pHeadTextObj will == NULL
	{
		pNewTextObject->pNext = NULL;
		pNewTextObject->pPrev = NULL;
		
		pHeadTextObj = pNewTextObject;
		pHeadTextObj = pNewTextObject;
	}
	else // noText passed, create this Text at the beginning of the list
	{
		pNewTextObject->pNext			= pHeadTextObj;
		pNewTextObject->pPrev			= NULL;
		pHeadTextObj->pPrev	= pNewTextObject;
		pHeadTextObj			= pNewTextObject;
	}
	
	iTotalTextObjs++;
	
	return pNewTextObject;
}

P_TEXTOBJ tagAPI_OpenGL::DeleteTextObjectNode(P_TEXTOBJ pTextPos)
{
	//Just in case we tried to Del when there is no text
	if(iTotalTextObjs <= 0)
		return 0;
	
	P_TEXTOBJ pNext = pTextPos->pNext;
	
	//*** If the TextPos's Next is a valid Text, 
	//  make the next Text's prev = the deleted Texts' prev 
	//	(hence cutting out the middle "Text")
	if (pTextPos->pNext)
	{
		pNext = pTextPos->pNext;
		pTextPos->pNext->pPrev = pTextPos->pPrev;
	}
	else
	{
		pNext = NULL;
		pHeadTextObj = pTextPos->pPrev;
	}
	
	// *** same Text as above just in the oposite direction in the linked list
	if (pTextPos->pPrev)
		pTextPos->pPrev->pNext = pTextPos->pNext;
	else
		pHeadTextObj = pTextPos->pNext;
	
	
	//***If the Text we deleted was the tail return NULL for our update loop
	free(pTextPos);
	iTotalTextObjs--;
	return pNext;
}
