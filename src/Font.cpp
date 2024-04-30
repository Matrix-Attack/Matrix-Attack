#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "font.h"
#include "tga.h"
#include <math.h>	//for pow ()
//#include <tchar.h>	
#include <stdio.h>	//for _vsntprintf()
#include <gl/gl.h>


#include "GLSurfDef.h"
#include "./mmgr.h"

int tagFont::Initialize(int glTexID)
{
	//**** LOAD FONT DEFAULTS ****
	//Default Cursor Position
	XPos		= 0;
	YPos		= 0;

	//Resets all variables that can change.
	Reset();

	blockRow	= FONT_BLOCK_ROW;	// characters per row    										
	blockCol	= FONT_BLOCK_COL;	// characters por col    										
	texID     	= glTexID;			// texture id

	//*** get current viewport
    glGetIntegerv (GL_VIEWPORT, ViewPort);

	//*****Load our Default font****
	tIncX = (float)pow (blockCol, -1);
	tIncY = (float)pow (blockRow, -1);

	float x, y;
	int i=0;
	for (y = 1 - tIncY; y >= 0; y -= tIncY)
		for (x=0; x <= 1 - tIncX; x += tIncX, i++)
		{
			tPoints[i][0] = x;
			tPoints[i][1] = y;
		}

	i = LoadGLTarga(GL_SURF_MAIN_SYSFONT_FILENAME, texID);

	if(i)
	{	MessageBox(NULL, "Cannot load default font texture", "font init error", MB_OK);
		if(i == 13) 
			MessageBox(NULL, " \"font.tga\" is missing.", "font init error", MB_OK);
		return 1;
	}
	
	return 0;
}

int tagFont::Initialize(char *fileName, U32 row, U32 col, U32 glTexID)
{
	//**** LOAD FONT DEFAULTS ****
	//Default Cursor Position
	XPos		= 0;
	YPos		= 0;

	//Resets all variables that can change.
	Reset();
		
	blockRow	= row;			// characters per row    										
	blockCol	= col;			// characters por col    										
	texID     	= glTexID;		// texture id
	
	//*** get current viewport
    glGetIntegerv (GL_VIEWPORT, ViewPort);

	//*****Load our Custom font****
	tIncX = (float)pow (blockCol, -1);
	tIncY = (float)pow (blockRow, -1);

	float x, y;
	int i=0;
	for (y = 1 - tIncY; y >= 0; y -= tIncY)
		for (x=0; x <= 1 - tIncX; x += tIncX, i++)
		{
			tPoints[i][0] = x;
			tPoints[i][1] = y;
		}
	
	if (LoadGLTarga(fileName, texID) )
	{
		MessageBox(NULL, "Cannot load default font texture", "font init error", MB_OK);
		return 1;
	}

	return 0;
}
void tagFont::DrawString (float x, float y, char *s, ...)
{
	XPos = x;
	YPos = y;
	va_list	msg;	//var list from "..."
	
    char CharBuffer[FONT_MAX_LEN] = {'\0'};		//FONT_MAX_LEN == 1024
	
	va_start(msg, s);
	
	//finds any key '%d' characters and fills them in with msg
	//_vsntprintf (CharBuffer, FONT_MAX_LEN - 1, s, msg);	
	_vsnprintf(CharBuffer, FONT_MAX_LEN - 1, s, msg);

	va_end(msg);

    //*** save the opengl setup 
	SetOpenGLModes(FONT_GET_MODES);
    
	//*** Set up openGL so that we can draw text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

    glOrtho(0, ViewPort[2], 0, ViewPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, texID);

    //***draw the string
	WalkString(CharBuffer); //vPort[0] + vPort[2] == xMax

	//get rid of our openGL settings
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//*** save the opengl setup 
	SetOpenGLModes (FONT_RESTORE_MODES);
    //Reset ();
}

void tagFont::DrawStaticString(float x, float y, char *s)
{
	XPos = x;
	YPos = y;

    //*** save the opengl setup 
	SetOpenGLModes (FONT_GET_MODES);
    
	//*** Set up openGL so that we can draw text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

    glOrtho(0, ViewPort[2], 0, ViewPort[3], -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, texID);

    // draw the string

    //***Draw the text.
	for (XPos=x; *s; *s++, XPos+=Size)
	{
		if (XPos > (ViewPort[0] + ViewPort[2]) )
			break;
		DrawChar(*s);
	}

	//get rid of our openGL settings
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();
	glMatrixMode (GL_MODELVIEW);
	glPopMatrix ();

	//*** save the opengl setup 
	SetOpenGLModes (FONT_RESTORE_MODES);
}

//Draws a character that is 'size' pixels in w and h.  
void tagFont::DrawChar(char c)
{

    if (!bGradient && !bShadow)
        glColor4fv (fgColor);
//    else if (!bGradient && bShadow)
//        glColor4fv (bgColor);
    else if (bGradient && !bShadow)
        glColor4fv (gdColor);

	//***This is the Actual Drawing of our single Char 'c'
    glBegin (GL_QUADS);
		
		glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1]);
		glVertex2f (XPos, YPos);

    	glTexCoord2f (tPoints[(int)c][0] + tIncX, tPoints[(int)c][1]);
		glVertex2f (XPos + Size, YPos);
 
        glColor4fv (fgColor);

        glTexCoord2f (tPoints[(int)c][0] + tIncX, tPoints[(int)c][1] + tIncY);
		glVertex2f (XPos + Size + Italic, YPos + Size);

        glTexCoord2f (tPoints[(int)c][0], tPoints[(int)c][1] + tIncY);
		glVertex2f (XPos + Italic, YPos + Size);

	glEnd ();
}

//Does the actual rendering of our string.  
void tagFont::WalkString(char *s)
{
    //***Draw the text.
	for (; *s; *s++, XPos += Size)
	{
		if (XPos > (ViewPort[0] + ViewPort[2]) )//ViewPort[0] + ViewPort[2] == xMax
			break;
 
		switch (*s)
		{
			case '\n':
				YPos -= Size;
                XPos = XPos - Size;
				continue; 
			break;

			case '\t':
				XPos += (Size * FONT_TAB_SPACE);
				continue; 
			break;

            default:
				DrawChar(*s);
            break;
		}
	}
}

//Resets the   Only resets variables that could possible change.
void tagFont::Reset ()
{
    Size		= 12;
    Italic		= 0;
	bShadow		= 0;
    bGradient	= 0;
    bBold		= 0;

	//Foreground Color (default white)
	fgColor[0]	= 1.0;	// RED   white text
	fgColor[1]	= 1.0;	// GREEN white text
	fgColor[2]	= 1.0;	// BLUE  white text
	fgColor[3]	= 1.0;	// ALPHA white text
	
	//Gradient Color (default grey)
	gdColor[0]	= 0.5;	// RED   gray gradient
	gdColor[1]	= 0.5;	// GREEN gray gradient
	gdColor[2]	= 0.5;	// BLUE  gray gradient
	gdColor[3]	= 1.0;	// ALPHA gray gradient
	
	//BackGround Color (default grey)
//	bgColor[0]	= 0.5;	// RED   gray shadow
//	bgColor[1]	= 0.5;	// GREEN gray shadow
//	bgColor[2]	= 0.5;	// BLUE  gray shadow
//	bgColor[3]	= 1.0;	// ALPHA gray shadow

}

void tagFont::DrawStringFromTextObj (P_TEXTOBJ t)
{
	Size = t->Size;
	if(t->TypeFlags & TF_USING_ITALICS)
		Italic = t->Italic;
	else
		Italic = 0;

	fgColor[0]	= t->fgColor[0];
	fgColor[1]	= t->fgColor[1];
	fgColor[2]	= t->fgColor[2];
	fgColor[3]  = t->fgColor[3];

	if(t->TypeFlags & TF_GRADIENT)
	{
		bGradient	= 1;
		gdColor[0]	= t->gdColor[0];
		gdColor[1]	= t->gdColor[1];
		gdColor[2]	= t->gdColor[2];
		gdColor[3]  = t->gdColor[3];
	}
	else
		bGradient = 0;
	
	char *s = t->String;


	int i = glIsEnabled(GL_ALPHA_TEST);



	//If the text is changing its size then center its x scaling correctly
	if(t->TypeFlags & TF_USING_SCALING)
	{	
		//CENTER DRAWING
		//DrawStaticString(t->XPos - ( (t->Size*strlen(s))*0.5f),
		//t->YPos, s);

		XPos = t->XPos - ( (t->Size*strlen(s))*0.5f);
		YPos = t->YPos;
		//***run through the string and Draw the text.
		for (; *s; *s++, XPos+=Size)
		{
			if (XPos > (ViewPort[0] + ViewPort[2]) )
				break;
			DrawChar(*s);
		}
	}
	else
	{	//NORMAL DRAWING
		XPos = t->XPos;
		YPos = t->YPos;
		//***run through the string and Draw the text.
		for (; *s; *s++, XPos+=Size)
		{
			if (XPos > (ViewPort[0] + ViewPort[2]) )
				break;
			DrawChar(*s);
		}
	}

}

//Sets or restores OpenGL modes that we need to use.  Here to prevent the drawing
//of text from messing up other stuff.  
void SetOpenGLModes (U32 state)
{
	static int matrixMode;
	static int polyMode[2];
	static int lightingOn;
	static int blendOn;
	static int depthOn;
	static int textureOn;
	static int scissorOn;
	static int blendSrc;
	static int blendDst;

	// grab the modes that we might need to change
	if (state == FONT_GET_MODES)
	{
		glGetIntegerv(GL_POLYGON_MODE, polyMode);

		if (polyMode[0] != GL_FILL)
			glPolygonMode (GL_FRONT, GL_FILL);
		if (polyMode[1] != GL_FILL)
			glPolygonMode (GL_BACK, GL_FILL);
	
		textureOn = glIsEnabled (GL_TEXTURE_2D);
		if (!textureOn)
			glEnable (GL_TEXTURE_2D);

		depthOn = glIsEnabled (GL_DEPTH_TEST);

		if (depthOn)
			glDisable (GL_DEPTH_TEST);

		lightingOn= glIsEnabled (GL_LIGHTING);        
	
		if (lightingOn) 
			glDisable(GL_LIGHTING);

		scissorOn= glIsEnabled (GL_SCISSOR_TEST);        
	
		if (!scissorOn) 
			glEnable (GL_SCISSOR_TEST);

		glGetIntegerv(GL_MATRIX_MODE, &matrixMode); 
	
		// i don't know if this is correct
		blendOn= glIsEnabled (GL_BLEND);        
        glGetIntegerv (GL_BLEND_SRC, &blendSrc);
	    glGetIntegerv (GL_BLEND_DST, &blendDst);
		if (!blendOn)
			glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} 
	else if (state == FONT_RESTORE_MODES)
	{
		// put everything back where it was before
		if (polyMode[0] != GL_FILL)
			glPolygonMode (GL_FRONT, polyMode[0]);
		if (polyMode[1] != GL_FILL)
			glPolygonMode (GL_BACK, polyMode[1]);
	
		if (lightingOn)
			glEnable(GL_LIGHTING);
	
		// i don't know if this is correct
		if (!blendOn)
        {
			glDisable (GL_BLEND);
            glBlendFunc (blendSrc, blendDst);
        } else glBlendFunc (blendSrc, blendDst);

		if (depthOn)
			glEnable (GL_DEPTH_TEST);

		if (!textureOn)
			glDisable (GL_TEXTURE_2D);
		
		if (!scissorOn) 
			glDisable (GL_SCISSOR_TEST);
	
		glMatrixMode (matrixMode);
	}
}
