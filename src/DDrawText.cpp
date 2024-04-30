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

#include "text.h"


#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "Bitmap.h"


#define DDRAW_SMALL_FONT_SURFACE	surfaceArray[0]
#define DDRAW_LARGE_FONT_SURFACE	surfaceArray[0]



extern APP			App;
extern GAME_DATA	GameData;

void ClipAgainstScreenEdges(RECT &clipRect, int &x, int &y);

void tagAPI_DDraw::DDrawText(int	startX,
							 int	startY,
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
				x	= startX,// - letterWidth,
				y	= startY;

	unsigned int	Font		=	0;
	unsigned int	Color		=	0;
	unsigned int	TextSpacing	=	2;

	unsigned int	maxLength	=	44;
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
				DDrawChar0(x, y, Color, *p);
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
					DDrawChar0(x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					DDrawChar0(x, y, Color, *sval);
					x += letterWidth + TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) 
					Color = 0;	//error checking
				DDrawChar0(x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				DDrawChar0(x, y, Color, *p);
				x += letterWidth + TextSpacing;
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
				DDrawChar1(x, y, Color, *p);
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
					DDrawChar1(x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					DDrawChar1(x, y, Color, *sval);
					x += letterWidth + TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) 
					Color = 1;	//error checking
				DDrawChar1(x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				DDrawChar1(x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	
} //end DDrawText function


void tagAPI_DDraw::DDrawChar0(int	x,
							  int	y,
							  int	Color,
							  char	character)
{
	const int	letterWidth		= 8,
				letterHeight	= 10;
	RECT		clipRect;
	
	
	// blit the SMALL FONT letter
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

		ClipAgainstScreenEdges(clipRect, x, y);

		lpDDSBack->BltFast(x, y, DDRAW_SMALL_FONT_SURFACE, &clipRect,
								DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	}
}

void tagAPI_DDraw::DDrawChar1(int	x,
							 int	y,
							 int	Color,
							 char	character)
{
	const int	letterWidth		= 12,
				letterHeight	= 20;
	RECT		clipRect;
	
	
	// blit the LARGE FONT letter
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

		ClipAgainstScreenEdges(clipRect, x, y);

		lpDDSBack->BltFast(x, y, DDRAW_LARGE_FONT_SURFACE, &clipRect,
								DDBLTFAST_WAIT | DDBLTFAST_SRCCOLORKEY);
	}
}

void ClipAgainstScreenEdges(RECT &clipRect, int &x, int &y)
{
	// modify clip rect
	int	width	= clipRect.right - clipRect.left,
		height	= clipRect.bottom - clipRect.top;

	if(x < 0)
	{
		clipRect.left	-= x; // amount of overlap
		x				= 0;
	}
	if((x + width) > 639)
		clipRect.right	-= (x + width) - 640; // amount of overlap

	if(y < 0)
	{
		clipRect.top	-= y; // amount of overlap
		y				= 0;
	}
	if((y + height) > 479)
		clipRect.bottom	-= (y + height) - 480; // amount of overlap
}


// this function searches a text file for the first occurrence of
// <string> starting from the current position of the file pointer.
void LoopUntilString(const char *string, FILE *pFile)
{
	int length = strlen(string);

						// the "length + 1" is to make space for a null character
	char *tempstring	= (char *)malloc((length + 1) * sizeof(char));
	tempstring[length]	= '\0';

	// read <length> characters at a time, then do a strcmp
	for(int i = 0; i < length; i++)
		tempstring[i] = getc(pFile);

	// this loop will only execute if the strings didn't match the first time
	while(strcmp(tempstring, string))
	{
		// move all the letters in tempstring back by one space,
		// then read a new character
		for(i = 0; i < length - 1; i++)
			tempstring[i]		= tempstring[i + 1];

		tempstring[length - 1]	= getc(pFile);
	}

	free(tempstring);
}

void ClipToViewPort(RECT &clipRect, RECT &viewPort, int &x, int &y)
{
	int	width	= clipRect.right - clipRect.left,
		height	= clipRect.bottom - clipRect.top;

	if(x < viewPort.left)
	{
		clipRect.left	+= viewPort.left - x;
		x				= viewPort.left;
	}

	if((x + width) > viewPort.right)
		clipRect.right	-= (x + width) - (viewPort.right + 1);

	if(y < viewPort.top)
	{
		clipRect.top	+= viewPort.top - y;
		y				= viewPort.top;
	}

	if((y + height) > viewPort.bottom)
		clipRect.bottom	-= (y + height) - (viewPort.bottom + 1);
}

void ClipToScreenEdges(RECT &clipRect, int &x, int &y)
{
	int	buttonWidth		= clipRect.right - clipRect.left,
		buttonHeight	= clipRect.bottom - clipRect.top;

	if(  x < 0 )
	{
		clipRect.left	-= x; // amount of overlap
		x		= 0;
	}
	if( (x + buttonWidth) > 639)
		clipRect.right	-= (x + buttonWidth) - 640; // amount of overlap

	if(  y < 0 )
	{
		clipRect.top	-= y; // amount of overlap
		y		= 0;
	}
	if( (y + buttonHeight) > 479)
		clipRect.bottom	-= (y + buttonHeight) - 480; // amount of overlap
}


void tagAPI_DDraw::DDrawText(RECT	*rcClip,
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
				DDrawChar0(x, y, Color, *p);
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
					DDrawChar0(x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					DDrawChar0(x, y, Color, *sval);
					x += letterWidth + TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 0;	//error checking
					DDrawChar0(x, y, Color, *p);
					x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				DDrawChar0(x, y, Color, *p);
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
				DDrawChar1(x, y, Color, *p);
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
					DDrawChar1(x, y, Color, *szVar);
					x += letterWidth + TextSpacing;
					szVar++;
				}
				break;

			case 's':
				//***Get String***
				for (sval = va_arg(ap, char *); *sval; sval++)
				{
					DDrawChar1(x, y, Color, *sval);
					x += TextSpacing;
				}
				break;
			
			case 'C':
			case 'c':
				//***Get Color from string ***
				Color	= atoi((++p));		//get the number from the char pointer
				++p;						//move the char pointer past the color #
				if(Color >= 6) Color = 1;	//error checking
				DDrawChar1(x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;
			
			case '\n':
				//***Special Chat NULL case***
				
				break;

			default://Draws everything else (including a %)
				DDrawChar1(x, y, Color, *p);
				x += letterWidth + TextSpacing;
				break;

			} //end switch
		
			
		} //end for
		va_end(ap);	//clean up when done
	}
	
} //end DDrawText function