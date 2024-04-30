#include <ddraw.h>
#include <stdio.h>
#include <stdarg.h>
#include <dplay.h>
#include <dplobby.h>


#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "Bitmap.h"
#include "text.h"
#include "GLSurfDef.h"


extern APP			App;
extern GAME_DATA	GameData;

void tagAPI_OpenGL::GLDrawText(int	startX,
							  int	startY,
							  U32	Flags,
							  char	*szMsg,
							  ...)
{
	int			letterWidth		= 8,
				letterHeight	= 10;

	va_list		ap;			// points to each unnamed arg in turn.

	char		*szVar,
				*p,
				*sval,
				buf[256];

	int			ival,
				x	= startX - letterWidth,
				y	= startY;

	unsigned int	Font		=	0;
	unsigned int	Color		=	0;
	unsigned int	TextSpacing	=	2;
	unsigned int	maxLength	=	128;
	unsigned int	CharNum		=	0;

	//**** SET THE TEXT FLAGS FOR EVERYTHIING *****
	switch(Flags & (TF_FONT) )
	{
		//default:
		case TF_FONT_SMALL:		
			letterWidth		= 8;
			letterHeight	= 10;
			Font			= 0;
			break;

		case TF_FONT_LARGE:
			letterWidth		= 12;
			letterHeight	= 20;
			TextSpacing		= 3;
			Font			= 1;
			break;
	}

	switch(Flags & (TF_COLOR) )
	{
		case TF_COLOR_RED:			Color = 4;			break;
		case TF_COLOR_GREEN:		Color = 2;			break;
		case TF_COLOR_BLUE:			Color = 3;			break;
		case TF_COLOR_YELLOW:		Color = 5;			break;
		case TF_COLOR_LIGHT_BLUE:	Color = 1;			break;
		case TF_COLOR_WHITE: 		Color = 0;			break;
	}
	switch(Flags & (TF_SPACING) )
	{
		case TF_SPACING_0:			TextSpacing = 0;	break;
		case TF_SPACING_1:			TextSpacing = 1;	break;
		case TF_SPACING_2:			TextSpacing = 2;	break;
		
	}
	switch(Flags & (TF_LENGTH) )
	{
		case TF_LENGTH_CHAT_OUTPUT:	maxLength = 44;		break;
		case TF_LENGTH_CHAT_INPUT:	maxLength = 44;		break;
		case TF_LENGTH_IP_INPUT:	maxLength = 3;		break;
		case TF_LENGTH_STATS:		maxLength = 640;	break;
	}


	va_start(ap, szMsg);	// make ap point to 1st unnamed arg
	
	if(!Font)
	{
		for(p = szMsg; *p; p++, CharNum++)
		{

			if( CharNum >= maxLength)
				return;
			
			if (*p != '%')
			{
				GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				x += TextSpacing;
				continue;
			}

			switch( *++p )
			{
			case 'd':
				//***Get Intiger***
				ival	= va_arg(ap, int);
				szVar	= itoa(ival, buf, 10);
				while(*szVar != '\0')
				{
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *szVar);
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *sval);
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 0;	//error checking
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	else //**** FONT 2 *****
	{
		for(p = szMsg; *p; p++, CharNum++)
		{

			if( CharNum >= maxLength)
				return;

			if (*p != '%')
			{
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				x += TextSpacing;
				continue;
			}

			switch( *++p )
			{
			case 'd':
				//***Get Intiger***
				ival	= va_arg(ap, int);
				szVar	= itoa(ival, buf, 10);
				while(*szVar != '\0')
				{
					GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *szVar);
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
					GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *sval);
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 1;	//error checking
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x += letterWidth, y, Color, *p);
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	
} //end GLDrawText function

void tagAPI_OpenGL::GLDrawChar0(const int TexID,
								int	x,
								int	y,
								int	Color,
								char character)
{
	const int	letterWidth		= 8,
				letterHeight	= 10;
	RECT		clipRect;
	
	
	// blit the letter
	if(character != ' ')
	{
		if(character >= '!' && character <= '@')
		{
			clipRect.left	= (character - '!') * letterWidth;
			clipRect.top	= 80 + (Color * 20);
			clipRect.right	= clipRect.left + letterWidth;
			clipRect.bottom	= clipRect.top + letterHeight;
		}
		else if(character >= 'A' && character <= '_')
		{
			clipRect.left	= (character - 'A') * letterWidth;
			clipRect.top	= 80 + letterHeight + (Color * 20);
			clipRect.right	= clipRect.left + letterWidth;
			clipRect.bottom	= clipRect.top + letterHeight;
		}
		else
			return;

		BltGLStyle(x, y, &clipRect, 256, 256, TexID);
	}
}

void  tagAPI_OpenGL::GLDrawChar1(const int TexID,
								 int	x,
								 int	y,
								 int	Color,
								 char character)
{
	const int	letterWidth		= 12,
				letterHeight	= 20;
	RECT		clipRect;
	
	
	// blit the letter
	if(character != ' ')
	{
		if(character >= 'A' && character <= 'U')
		{
			clipRect.right	= (clipRect.left = (character - 'A') * letterWidth) + letterWidth;
			clipRect.bottom	= (clipRect.top = (Color * 40)) + letterHeight;
		}
		else if(character >= 'V' && character <= 'Z')
		{
			clipRect.right	= (clipRect.left = (character - 'V') * letterWidth) + letterWidth;
			clipRect.bottom	= (clipRect.top	= letterHeight + (Color * 40)) + letterHeight;
		}
		else if(character >= '0' && character <= '9')
		{
			clipRect.right	= (clipRect.left = (character - '0') * letterWidth + 60) + letterWidth;
			clipRect.bottom	= (clipRect.top	= letterHeight + (Color * 40)) + letterHeight;
		}
		else if(character == '!')
		{	
			clipRect.left	= 180;
			clipRect.top	= 20 + (Color * 40);
			clipRect.right	= clipRect.left+ letterWidth;
			clipRect.bottom	= clipRect.top + letterHeight;
		}
		else
			return;

		//This will flip our clipping coords for opengl 
		// (so that 0,0 is the upper left hand corner of the surface/texture)

		BltGLStyle(x, y, &clipRect, 256, 256, TexID);
	}
}

void tagAPI_OpenGL::GLDrawText(RECT	*rcClip,
							   U32	Flags,
							   char	*szMsg,
							   ... )
{
	int			letterWidth		= 8,
				letterHeight	= 10;

	va_list		ap;			// points to each unnamed arg in turn.

	char		*szVar,
				*p,
				*sval,
				buf[256];

	int			ival,
				x	= rcClip->left,
				y	= rcClip->top;

	unsigned int	Font		=	0;
	unsigned int	Color		=	0;
	unsigned int	TextSpacing	=	2;

	//**** SET THE TEXT FLAGS FOR EVERYTHIING *****
	switch(Flags & (TF_FONT) )
	{
		//default:
		case TF_FONT_SMALL:		
			letterWidth		= 8;
			letterHeight	= 10;
			Font			= 0;
			break;

		case TF_FONT_LARGE:
			letterWidth		= 12;
			letterHeight	= 20;
			TextSpacing		= 3;
			Font			= 1;
			break;
	}

	switch(Flags & (TF_COLOR) )
	{
		case TF_COLOR_RED:			Color = 4;			break;
		case TF_COLOR_GREEN:		Color = 2;			break;
		case TF_COLOR_BLUE:			Color = 3;			break;
		case TF_COLOR_YELLOW:		Color = 5;			break;
		case TF_COLOR_LIGHT_BLUE:	Color = 1;			break;
		case TF_COLOR_WHITE: 		Color = 0;			break;
	}
	switch(Flags & (TF_SPACING) )
	{
		case TF_SPACING_0:			TextSpacing = 0;	break;
		case TF_SPACING_1:			TextSpacing = 1;	break;
		case TF_SPACING_2:			TextSpacing = 2;	break;
		
	}


	va_start(ap, szMsg);	// make ap point to 1st unnamed arg
	
	if(!Font)
	{
		for(p = szMsg; *p; p++)
		{

			if( x+letterWidth >= rcClip->right)
			{
				y += letterHeight + TextSpacing;
				x =	rcClip->left;
			}
			
			if (*p != '%')
			{
				GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
				x += letterWidth + TextSpacing;
				continue;
			}

			switch( *++p )
			{
			case 'd':
				//***Get Intiger***
				ival	= va_arg(ap, int);
				szVar	= itoa(ival, buf, 10);
				while(*szVar != '\0')
				{
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *sval);
					x += letterWidth + TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 0;	//error checking
					GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
					x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				GLDrawChar0(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	else //**** FONT 2 *****
	{
		for(p = szMsg; *p; p++ )
		{

			if( x+letterWidth >= rcClip->right)
			{
				y += letterHeight + TextSpacing;
				x =	rcClip->left;// - letterWidth;
			}

			if (*p != '%')
			{
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
				x += letterWidth + TextSpacing;
				continue;
			}

			switch( *++p )
			{
			case 'd':
				//***Get Intiger***
				ival	= va_arg(ap, int);
				szVar	= itoa(ival, buf, 10);
				while(*szVar != '\0')
				{
					GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *sval);
					x += TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 1;	//error checking
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				GLDrawChar1(GL_SURF_MAIN_FONT_TEXID, x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	
} //end DDrawText function