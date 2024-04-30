#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <dinput.h>
#include <stdio.h>			//for def of FILE 

#include <gl/gl.h>

#include "./mmgr.h"

#include "DPlay.h"

#include "app.h"
#include "vector.h"
#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"
#include "model.h"
#include "dinput.h"
#include "text.h"
#include "Blt2DopenGL.h"	//for blitting openGL 2D style
#include "Bitmap.h"

#include "GLSurfDef.h"

#include "defines.h"

#include "gamemenu.h"




void			LoopUntilString	( const char *string, FILE *pFile);
unsigned int	ConvertToInt	( int numchars, FILE *pFile);
unsigned int	BinaryToInt		( int numchars, FILE *pFile);
float			ConvertToDouble	( FILE *pFile);


extern APP App;
extern GAME_DATA GameData;	//for the score


#define READ_FILE_STRING(pFile, string)	(fscanf((pFile), "\n%[^\n]", (string)))

void Board3D::Clear( void )
{
	boardName[0]	= NULL;
	boardType		= 0;
	pBackSurf		= NULL;			// pointer to the back surface
	szStartMsg[0]	= NULL;			// Msg that you get before the game starts
	numRows			= 0;
	numCols			= 0;
	numMoves		= 0;
	mGem			= NULL;
	gem				= NULL;
}

int Board3D::Initialize(const P_S8 filename, const P_S8 boardname)
{

	if( LoadBoard( filename, boardname ) )
	{
		//return error
		Destroy();
		return 1;
	}

	LoadSurfaces();

	return 0;// return ok
}

void Board3D::Destroy( void )
{
//	switch(boardType)
//	{
//	case FLAT_BOARD:
//	}
	
	if(gem)
	{
		for(int i = 0; i < numRows; i++)
			delete[] gem[i];
		delete[] gem;

		gem = NULL;
	}
	
	if(mGem)
	{
		for(int i = 0; i < numRows; i++)
			delete[] mGem[i];
		delete[] mGem;
		mGem = NULL;
	}

	Clear();
	
}

int GetGemColor(char Char)
{
	switch(Char)
	{
		case 'R':	return 0;
		case 'Y':	return 1;
		case 'G':	return 2;
		case 'C':	return 3;
		case 'P':	return 4;
		case 'B':	return 5;
		case '0':	return 7;
		default:	return 0;
	}
}




int Board3D::LoadBoard(const P_S8 filename, const P_S8 boardname)
{
	//if we are loading when there was a board previously then we need to delete the old
	if(gem)
	{
		for(int i = 0; i < numRows; i++)
		{
			delete[] gem[i];
		}
		delete[] gem;
		gem = NULL;
	}

	FILE	*pFile	= fopen(filename, "r");

	if(pFile == NULL)
	{
		char Buffer[128];
		sprintf(Buffer, "File %s, Board %s", filename, boardname);
		if(!App.UserSettings.RenderMode)
			MessageBox(App.pDDraw->getHwnd(), Buffer, "Board DataFile Not found",  MB_OK);
		else
			MessageBox(App.pOpenGL->getHwnd(), Buffer, "Board DataFile Not found",  MB_OK);
		return 1;
	}
	
	// find the right board
	LoopUntilString(boardname, pFile);
	strcpy(boardName, boardname);

	// get the type
	LoopUntilString("<type: ", pFile);
	boardType	= ConvertToInt(4, pFile);

	switch(boardType)
	{
	case FLAT_BOARD:
		// get number of rows
		if(!(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI))
		{
			LoopUntilString("<rows: ", pFile);
			numRows	= ConvertToInt(4, pFile);

			LoopUntilString("<cols: ", pFile);
			numCols	= ConvertToInt(4, pFile);

		}
		else if(App.pDP->hostflag == 1)
		{
			LoopUntilString("<rows: ", pFile);
			numRows	= ConvertToInt(4, pFile);

			LoopUntilString("<cols: ", pFile);
			numCols	= ConvertToInt(4, pFile);
			
		}
		if((GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE))
		{
			LoopUntilString("<nmov: ", pFile);
			numMoves	= ConvertToInt(4, pFile);

		}

		LoopUntilString("<smsg: ", pFile);
		READ_FILE_STRING(pFile, szStartMsg);
		
		// allocate memory for all the gems
		gem	= new Gem3D*[numRows];
		
		for(int i = 0; i < numRows; i++)
		{
			gem[i]	= new Gem3D[numCols];
		}

		(App.UserSettings.DetailLevel) ?	GetMeshesForGems(GEM_36POLY_MESH):
											GetMeshesForGems(GEM_12POLY_MESH);
		
		Vector3D	veloc	= {0, 0, 0};

		LoopUntilString("<gems:", pFile);
		char Buffer[512];
		char *p;

		for(i = 0; i < numRows; i++)
		{
			fgetc(pFile); //get past the return character
			READ_FILE_STRING(pFile, Buffer);
			p = Buffer;
			for(int j = 0; j < numCols; j++)
			{
				//ID this gem
				gem[i][j].bChecked = false;

				if((GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI) && App.pDP->hostflag == 0)
				{
					gem[i][j].activeColor[0] = gem[i][j].color[0]  = mGem[i][j].activeColor[0];
					gem[i][j].activeColor[1] = gem[i][j].color[1]  = mGem[i][j].activeColor[1];
				}
				else if(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE)
				{
					//***This code will read in the Gem Color LETTER and apply that color to the gem
					gem[i][j].SetBaseColor( GEM_FRONT_HALF, GetGemColor(*p) );
					//go to the next character in our file
					gem[i][j].SetBaseColor( GEM_BACK_HALF,	GetGemColor(*(++p)) );
				}
				else if(GameData.GameStateFlags & GAME_STATE_SINGLE_TTRIAL)
				{
					//***This code will fill the gem with random colors.
					gem[i][j].GetBaseColor(GEM_FRONT_HALF);
					gem[i][j].GetBaseColor(GEM_BACK_HALF);
				}
				else
				{
					gem[i][j].GetBaseColor(GEM_FRONT_HALF);
					gem[i][j].GetBaseColor(GEM_BACK_HALF);
				}
		
				gem[i][j].SetVelocity(veloc);
				gem[i][j].SetStateFlags(0);
				gem[i][j].associatedPlayerID	= GEM_PLAYERID_NOBODY;
				gem[i][j].bAutoFlipOn			= 0;
				gem[i][j].bStartingAChain		= 0;
				gem[i][j].row = i;
				gem[i][j].column = j;
				p++;// go past the space between character chunks
				p++;
			}
			
		}

		break;
	}

	fclose(pFile);

	SetPointersBetweenGems();

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
	{
		player.Initialize(gem[(numRows / 2)] + (numCols / 2) );
		player.pCamera	= &Camera;	
	}
	else
	{
		player.Initialize(gem[(numRows / 2)] + (numCols / 2));
		player.pCamera	= &Camera;	
	}
	
	//Check for Clusters if found correct the gems that match
	if(!(GameData.GameStateFlags & GAME_STATE_SINGLE_PUZZLE))
	{
		CheckForClusters( 5 ); // 5 matches allowed
	}
	
	//If our board is larger than 6 rows then turn on our camera otherwise turn it off
	if(numRows >= 13 || numCols >= 7)
	{
		GameData.bCamera = 1;
	}
	else
	{
		//turn it off
		GameData.bCamera = 0;
		Camera.ResetPosition();
	}

	return 0;
}

void Board3D::ClearChecked( void )
{
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numCols; j++)
		{
			gem[i][j].bChecked = false;
		}
	}
}

void Board3D::CheckForClusters( int iNumMatchesAllowed )
{
	//*** NOW go through all the gems on the FRONT side and make sure they
	// don't already cause clusters
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numCols; j++)
		{
			//Clear any gems that were checked in the correctColor function
			ClearChecked();
			//If a cluster is found correct the color
			gem[i][j].CorrectColor( iNumMatchesAllowed );
		}
	}
}

void Board3D::ReleaseSurfaces()
{
	
//	if(surfaceArray)
//	{
//		//free DDraw surfaces
//		for(int i=0; i < numSurfaces; i++)
//			if( surfaceArray[i])
//			{
//				App.pDDraw->DeleteSurface(&surfaceArray[i]);
//				surfaceArray[i] = NULL;
//			}
//
//		delete[] surfaceArray;
//		surfaceArray = NULL;
//	}
	
}


inline void Board3D::GetMeshesForGems(int detailLevel)
{
	float	xStart	= (numCols / 2.0f) * -30 + 8,
			yStart	= (numRows / 2.0f) *  26 - 12,
			x		= xStart,
			y		= yStart;
	int		i, j;
	P_S8	modelName;

	switch(detailLevel)
	{
	case GEM_12POLY_MESH:
		modelName	= "12_POLY_GEM";
		break;

	case GEM_36POLY_MESH:
		modelName	= "36_POLY_GEM";
		break;
	}

	// get all the meshes for a board of numRows rows by numCols columns,
	// and have the board centered on the screen
	for(i = 0; i < numRows; i++)
	{
		x	= i & 1 ?	xStart + 15: xStart;
		for(j = 0; j < numCols; j++)
		{
			gem[i][j].Init( x, y, 640, GEMDATA_FILENAME, modelName );
			gem[i][j].SetDetailLevel( detailLevel );
			
			x	+= 30;
		}
		y	-= 26;
	}
}




void Board3D::SetPointersBetweenGems()
{
	int firstrow	= 0,
		lastrow		= numRows - 1,
		firstcol	= 0,
		lastcol		= numCols - 1,
		i, j;

	switch(boardType)
	{
	case FLAT_BOARD:
		// set up connections between gems
		for(i = 0; i < numRows; i++)
		{
			for(j = 0; j < numCols; j++)
			{
				gem[i][j].SetNeighbor(GEM_NEIGHBOR_UP,		gem[i - 1] + j);
				gem[i][j].SetNeighbor(GEM_NEIGHBOR_DOWN,	gem[i + 1] + j);
				gem[i][j].SetNeighbor(GEM_NEIGHBOR_LEFT,	gem[i] + j - 1);
				gem[i][j].SetNeighbor(GEM_NEIGHBOR_RIGHT,	gem[i] + j + 1);

				if(!(i & 1)) // row is even (0, 2, 4, etc...)
				{
					gem[i][j].SetNeighbor(GEM_NEIGHBOR_UPLEFT,		gem[i - 1] + j - 1);
					gem[i][j].SetNeighbor(GEM_NEIGHBOR_DOWNLEFT,	gem[i + 1] + j - 1);
				}
				else // row is odd (1, 3, 5, etc...)
				{
					gem[i][j].SetNeighbor(GEM_NEIGHBOR_UPRIGHT,		gem[i - 1] + j + 1);
					gem[i][j].SetNeighbor(GEM_NEIGHBOR_DOWNRIGHT,	gem[i + 1] + j + 1);
				}
			}
		}
		// set the top and bottom rows to point to NULL
		for(j = 0; j < numCols; j++)
		{
			gem[firstrow][j].SetNeighbor(GEM_NEIGHBOR_UPRIGHT,	NULL);
			gem[firstrow][j].SetNeighbor(GEM_NEIGHBOR_UPLEFT,	NULL);
			gem[firstrow][j].SetNeighbor(GEM_NEIGHBOR_UP,		NULL);
			gem[lastrow][j].SetNeighbor(GEM_NEIGHBOR_DOWNLEFT,	NULL);
			gem[lastrow][j].SetNeighbor(GEM_NEIGHBOR_DOWNRIGHT,	NULL);
			gem[lastrow][j].SetNeighbor(GEM_NEIGHBOR_DOWN,		NULL);
		}
		// set the left and right walls to point to NULL
		for(i = 0; i < numRows; i++)
		{
			gem[i][firstcol].SetNeighbor(GEM_NEIGHBOR_LEFT,		NULL);
			gem[i][lastcol].SetNeighbor(GEM_NEIGHBOR_RIGHT,		NULL);
			
			if(!(i & 1))
			{
				gem[i][firstcol].SetNeighbor(GEM_NEIGHBOR_UPLEFT,	NULL);
				gem[i][firstcol].SetNeighbor(GEM_NEIGHBOR_DOWNLEFT,	NULL);
			}
			else
			{
				gem[i][lastcol].SetNeighbor(GEM_NEIGHBOR_UPRIGHT,	NULL);
				gem[i][lastcol].SetNeighbor(GEM_NEIGHBOR_DOWNRIGHT,	NULL);
			}
		}
		break;
	}
}

//***********************************************************************
// Function: Board3D::TileGameBoard
//
// Purpose:  Draws the checkered background for the main game screen
//
// Parameters: None
//		
//			  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::TileGameBoard()
{
	int x, y;
	RECT src = {0, 66, 32, 98};

	for(y = 0; y < 15; y++)
		for(x = 0; x < 20; x++)
			pBackSurf->BltFast(x * 32, y * 32, App.pDDraw->surfaceArray[9], &src, 0);
}



//***********************************************************************
// Function: Board3D::MakeRect
//
// Purpose:  int x - The x coordinate of the upper left corner
//			 int y - The y coordinate of the upper left corner
//           RECT* rc - The dimensions of the rect to be drawn
//			 unsigned int flag - Flag that describes whether or not the
//			 left or right side of the rect should be drawn
//
// Parameters: None
//		
//			  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::MakeRect(int x, int y, RECT* rc, unsigned int flag)
{
	int nHsegs, nVsegs;
	int	cHSeg, cVSeg;
	int curx, cury;
	RECT vSeg = {32, 66, 36, 70};
	RECT hSeg = {32, 70, 36, 74};
	RECT rcTopLeft = {40, 70, 44, 74};
	RECT rcBotLeft = {41, 66, 45, 70};
	RECT rcTopRight = {36, 70, 40, 74};
	RECT rcBotRight = {37, 66, 41, 70};
	RECT view       = {0, 0, 639, 479};

	// Compute the number of segments needed to draw the rectangle
	nHsegs = (rc->right - rc->left)/3 - 2;
	nVsegs = (rc->bottom - rc->top)/3 - 2;

	// Draw the left edge if the user wants it
	if(!(flag & NODRAW_LEFT) )
	{
		for(cVSeg = 0; cVSeg < nVsegs; cVSeg++)
		{
			cury = y + 3 + cVSeg * 3;
			ClipToViewPort(vSeg, view, x, cury); 
			pBackSurf->BltFast(x, cury, App.pDDraw->surfaceArray[9], &vSeg, 0);
		}

		// Draw the top left border
		ClipToViewPort(rcTopLeft, view, x, y); 
		pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[9], &rcTopLeft, 0);

		// Draw the bottom left border
		cury = y + (nVsegs * 3) + 3;
		ClipToViewPort(rcBotLeft, view, x, cury);
		pBackSurf->BltFast(x, cury, App.pDDraw->surfaceArray[9], &rcBotLeft, 0);
	}

	// Draw the right edge if the user wants it
	if(!(flag & NODRAW_RIGHT) )
	{
		for(cVSeg = 0; cVSeg < nVsegs; cVSeg++)
		{
			curx = x + 6 + (nHsegs * 3);
			cury = y + 3 + cVSeg * 3;
			ClipToViewPort(vSeg, view, curx, cury);
			pBackSurf->BltFast(curx, cury, App.pDDraw->surfaceArray[9], &vSeg, 0);	
		}

		curx = x + (nHsegs * 3) + 6;
		ClipToViewPort(rcTopRight, view, curx, y);
		pBackSurf->BltFast(curx, y, App.pDDraw->surfaceArray[9], &rcTopRight, 0);

		curx = x + (nHsegs * 3) + 6;
		cury = y + (nVsegs * 3) + 3;
		ClipToViewPort(rcBotRight, view, curx, cury);
		pBackSurf->BltFast(curx, cury, App.pDDraw->surfaceArray[9], &rcBotRight, 0);
	}

	// Now draw the horizontal pieces
	for(cHSeg = 0; cHSeg < nHsegs + 1; cHSeg++)
	{
		curx = x + 3 + (cHSeg * 3);
		ClipToViewPort(rcBotRight, view, curx, y);
		pBackSurf->BltFast(curx, y, App.pDDraw->surfaceArray[9], &hSeg, 0);	
	}

	for(cHSeg = 0; cHSeg < nHsegs + 1; cHSeg++)
	{
		curx = x + 3 + (cHSeg * 3);
		cury = y + 3 * nVsegs + 3;
		ClipToViewPort(rcBotRight, view, curx, cury);
		pBackSurf->BltFast(curx, cury, App.pDDraw->surfaceArray[9], &hSeg, 0);	
	}
}



//***********************************************************************
// Function: Board3D::DrawCharacter
//
// Purpose:  Draws a portrait of the game character given the character id
//
// Parameters: int index - the player to be drawn
//			   int x	 - the x coordinate to draw to
//			   int y     - the y coordinate to draw to			  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::DrawCharacter(int index, int x, int y)
{
	RECT rcDST;

	switch(index)
	{
		case DRAW_EMPTY:	SetRect(&rcDST, 128,   0, 192,  64);	break;
		case DRAW_FOXY:		SetRect(&rcDST,   0,   0,  64,  64);	break;
		case DRAW_VINEGIRL:	SetRect(&rcDST,   0,  64,  64, 128);	break;
		case DRAW_MILES:	SetRect(&rcDST,   0, 192,  64, 256);	break;
		case DRAW_NINJA:	SetRect(&rcDST,   0, 128,  64, 192);	break;
	}

	if(!App.UserSettings.RenderMode)
		pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[1], &rcDST, DDBLTFAST_SRCCOLORKEY);
	else
	{		//***OPEN_GL MODE***
		BltGLStyle(x, y, &rcDST, 256, 256, GL_SURF_MENU_CHARACTER_TEXID);
	}
}


//***********************************************************************
// Function: Board3D::DrawPlayerIcon
//
// Purpose:  Draws the colored arrow for the specified player
//
// Parameters: int index - the player number 
//			   int x - the x coordinate to draw to
//			   int y - the y coordinate to draw to
//					  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::DrawPlayerIcon(int index, int x, int y)
{
	RECT rcDST;

	switch(index)
	{
		case PLAYER_ONE:	SetRect(&rcDST, 0,  238, 17, 247);	break;
		case PLAYER_TWO:	SetRect(&rcDST, 17, 238, 33, 247);	break;
		case PLAYER_THREE:	SetRect(&rcDST, 33, 238, 49, 247);	break;
		case PLAYER_FOUR:	SetRect(&rcDST, 49, 238, 65, 247);	break;
	}

	if(!App.UserSettings.RenderMode)
		pBackSurf->BltFast(x, y, App.pDDraw->surfaceArray[10], &rcDST, DDBLTFAST_SRCCOLORKEY);
	else
	{		//***OPEN_GL MODE***
		BltGLStyle(x, y, &rcDST, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
	}
}


//***********************************************************************
// Function: Board3D::DrawVitality
//
// Purpose:  Draws the life bar for the specified player
//
// Parameters: int index - the player number 
//			   int x - the x coordinate to draw to
//			   int y - the y coordinate to draw to
//			   int percenthealth - the amount of health the player has (0 - 100)
//					  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::DrawVitality(int x, int y, int percenthealth)
{

	int numbars = (int)(percenthealth/100.0f * 12.0f);
	float extra = (percenthealth/100.0f * 12.0f) - numbars;
	int i;

	if(!App.UserSettings.RenderMode)
	{
		RECT rcHealth	= {32, 74, 36, 84};	// full bar
		RECT rcHealth2	= {40, 77, 44, 84};	// 2/3 of a bar
		RECT rcHealth3	= {44, 80, 48, 84};	// 1/3 of a bar

		for(i = 0; i < numbars; i++)
			pBackSurf->BltFast(x + (i * 4), y, App.pDDraw->surfaceArray[9], &rcHealth, DDBLTFAST_SRCCOLORKEY);

		if(extra > .66)
			pBackSurf->BltFast(x + (i * 4), y + 3, App.pDDraw->surfaceArray[9], &rcHealth2, DDBLTFAST_SRCCOLORKEY);
		else if(extra > .33)
			pBackSurf->BltFast(x + (i * 4), y + 6, App.pDDraw->surfaceArray[9], &rcHealth3, DDBLTFAST_SRCCOLORKEY);
	}
	else //***OPENGL_MODE***
	{
		RECT rcHealth	= {65, 238, 69, 248};	// full bar
		RECT rcHealth2	= {73, 238, 77, 248};	// 2/3 of a bar
		RECT rcHealth3	= {77, 238, 81, 248};	// 1/3 of a bar

		for(i = 0; i < numbars; i++)
			BltGLStyle(x + (i * 4), y, &rcHealth, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);

		if(extra > .66)
			BltGLStyle(x + (i * 4), y, &rcHealth2, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		else if(extra > .33)
			BltGLStyle(x + (i * 4), y, &rcHealth3, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
	}
}

//***********************************************************************
// Function: Board3D::DrawStreakBar
//
// Purpose:  Draws the life bar for the specified player
//
// Parameters: int index - the player number 
//			   int x - the x coordinate to draw to
//			   int y - the y coordinate to draw to
//			   int percenthealth - the amount of health the player has (0 - 100)
//					  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Eli Emerson
//
//***********************************************************************
void Board3D::DrawStreakBar(int x, int y, float percentStreak)
{

	int numbars = (int)(percentStreak/100.0f * 12.0f);
	float extra = (percentStreak/100.0f * 12.0f) - numbars;
	int i;

	if(!App.UserSettings.RenderMode)
	{
		RECT rcHealth	= {32, 74, 36, 84};	// full bar
		RECT rcHealth2	= {40, 77, 44, 84};	// 2/3 of a bar
		RECT rcHealth3	= {44, 80, 48, 84};	// 1/3 of a bar
		
		for(i = 0; i < numbars; i++)
			pBackSurf->BltFast(x + (i * 4), y, App.pDDraw->surfaceArray[9], &rcHealth, DDBLTFAST_SRCCOLORKEY);
		
		if(extra > .66)
			pBackSurf->BltFast(x + (i * 4), y + 3, App.pDDraw->surfaceArray[9], &rcHealth2, DDBLTFAST_SRCCOLORKEY);
		else if(extra > .33)
			pBackSurf->BltFast(x + (i * 4), y + 6, App.pDDraw->surfaceArray[9], &rcHealth3, DDBLTFAST_SRCCOLORKEY);
	}
	else //***OPENGL_MODE***
	{
		RECT rcHealth	= { 238, 65, 248, 69 };	// full bar
		RECT rcHealth2	= { 238, 73, 248, 77 };	// 2/3 of a bar
		RECT rcHealth3	= { 238, 77, 248, 81 };	// 1/3 of a bar
		
		for(i = 0; i < numbars; i++)
			BltGLStyle(x + (i * 4), y, &rcHealth, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		
		if(extra > .66)
			BltGLStyle(x + (i * 4), y, &rcHealth2, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
		else if(extra > .33)
			BltGLStyle(x + (i * 4), y, &rcHealth3, 256, 256, GL_SURF_GAME_CHARACTER_TEXID);
	}
}

//***********************************************************************
// Function: Board3D::DisplayMapCursor
//
// Purpose:  Draws the colored arrow and id for the player on the game board
//
// Parameters: none
//					  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::DisplayMapCursor(int whichPlayer)
{

	RECT	viewPort	= {338, 80, 504, 400};
//
	int					x			= 430 + (int)player.pGem->GetCenterPos()->x - 16;
	int					y			= 240 - (int)player.pGem->GetCenterPos()->y - 17;
//	int	x			= 320
//		+ (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetCenterPos()->x - 16 - 102;
//	int	y			= 240 -
//		  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetCenterPos()->y - 17;
	int					cury		= 0;
	static int			ycount		= 0;
	static int			flag		= 0;
	static unsigned int	count		= 0;
	bool				bounce		= TRUE;

//	GameData.SetPlayerPosition(whichPlayer);

	if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		cury = GameData.simplePlayers[whichPlayer].mapY;
	else
		cury = y;

	cury += ycount;

	if(bounce)
	{
		if((count % 10 == 0) && flag == 0)
		{
			if(count % 30 == 0)
				flag = 1;

			ycount++;
		}
		else if ((count % 10 == 0) && flag == 1)
		{
			if(count % 30 == 0)
				flag = 0;

			ycount--;
		}
	}

//	char test[100];
//	sprintf(test, "Row %d Column %d", GameData.simplePlayers[GameData.playerId].RowGem,
//									  GameData.simplePlayers[GameData.playerId].ColumnGem);
//	SetWindowText(App.pDDraw->getHwnd(), test);

	if(App.UserSettings.RenderMode)
	{
		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			switch(whichPlayer)
			{
			case PLAYER_ONE:
					//Back
					DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "1P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
						//Front
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "1P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_TWO:
					DrawPlayerIcon(PLAYER_TWO, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "2P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_TWO, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "2P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_THREE:
					DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "3P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "3P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_FOUR:
					DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "4P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_FOUR, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "4P");		// Draw the first player icon identifier display
					}
				break;
			}
		}
		else
		{
				//DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
				//App.pOpenGL->GLDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "1P");	// Draw the first player icon identifier display
				DrawPlayerIcon(PLAYER_ONE, x+17, y+5);			    // Draw player 1 icon
				App.pOpenGL->GLDrawText(x+17, y-5,  0, "1P");		// Draw the first player icon identifier display	
		}
	}
	else
	{
		if(GameData.GameStateFlags & GAME_STATE_VERSUS_MULTI)
		{
			switch(whichPlayer)
			{
			case PLAYER_ONE:
					DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "1P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "1P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_TWO:
					DrawPlayerIcon(PLAYER_TWO, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "2P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_TWO, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "2P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_THREE:
					DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "3P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "3P");		// Draw the first player icon identifier display
					}
				break;

			case PLAYER_FOUR:
					DrawPlayerIcon(PLAYER_THREE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
					App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "4P");	// Draw the first player icon identifier display
					if(GameData.playerId != whichPlayer)
					{
					//	x = (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->x - 10;
					//	y =  (int)GameData.pBoard->gem[GameData.simplePlayers[whichPlayer].RowGem][GameData.simplePlayers[whichPlayer].ColumnGem].GetScreenCenterPos()->y - 17;
						DrawPlayerIcon(PLAYER_FOUR, GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY);			    // Draw player 1 icon
						App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].frontX, GameData.simplePlayers[whichPlayer].frontY - 10,  0, "4P");		// Draw the first player icon identifier display
					}
				break;
			}
		}
		else
		{
				//DrawPlayerIcon(PLAYER_ONE, GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY);			    // Draw player 1 icon
				//App.pDDraw->DDrawText(GameData.simplePlayers[whichPlayer].mapX, GameData.simplePlayers[whichPlayer].mapY - 10, 0, "1P");	// Draw the first player icon identifier display
				DrawPlayerIcon(PLAYER_ONE, x, y);			    // Draw player 1 icon
				App.pDDraw->DDrawText(x, y - 10,  0, "1P");		// Draw the first player icon identifier display	
		}
	}
	/*
	sprintf(test, "frontX %d frontY %d mapX %d map Y %d id %d", GameData.simplePlayers[GameData.playerId].frontX,
												          GameData.simplePlayers[GameData.playerId].frontY,
											 			  GameData.simplePlayers[GameData.playerId].mapX,
														  GameData.simplePlayers[GameData.playerId].mapY,
														  GameData.playerId);
*/
//	SetWindowText(App.pDDraw->getHwnd(), test);

	count++;
}


//***********************************************************************
// Function: Board3D::DrawPlayerScore
//
// Purpose:  Draws the score for the specified player
//
// Parameters: int index - the player number 
//			   int x - the x coordinate to draw to
//			   int y - the y coordinate to draw to
//			   int score - the player score to be drawn
//					  
//
// Returns: void
//
// Last Modified:  Date 11/27/99      Author - Max Szlagor     
//
//***********************************************************************
void Board3D::DrawPlayerScore(int index, int x, int y, int score)
{
	char szMsg[100];
	int xScoreDisplay, xScore, yCur;

	sprintf(szMsg, "%d", GameData.simplePlayers[index].score);

	switch(index)
	{
	case PLAYER_ONE:
		xScoreDisplay = 1;
		xScore		  = 47;
		yCur		  = 85;
		break;

	case PLAYER_TWO:
		xScoreDisplay = 1;
		xScore		  = 47;
		yCur		  = 206;
		break;

	case PLAYER_THREE:
		xScoreDisplay = 1;
		xScore		  = 47;
		yCur		  = 327;
		break;

	case PLAYER_FOUR: 
		xScoreDisplay = 1;
		xScore		  = 47;
		yCur		  = 448;
		break;
	}

	// This code is for right justifiying the score
	if(score < 10)
		xScore += 40;
	else if(score < 100)
		xScore += 32;
	else if(score < 1000)
		xScore += 24;
	else if(score < 10000)
		xScore += 16;
	else if(score < 100000)
		xScore += 8;

	if(!App.UserSettings.RenderMode)
	{
		App.pDDraw->DDrawText(xScoreDisplay,  yCur, TF_SPACING_0, "SCORE");		// Draw the fourth player score display
		App.pDDraw->DDrawText(xScore, yCur, TF_SPACING_0, szMsg);
	}
	else
	{	//***OpenGL mode***
		App.pOpenGL->GLDrawText(xScoreDisplay,  yCur, TF_SPACING_0, "SCORE");		// Draw the fourth player score display
		App.pOpenGL->GLDrawText(xScore, yCur, TF_SPACING_0, szMsg);
	}

}
void Board3D::DrawAllRects()
{
	RECT rc = {0, 0, 172, 329};								// This is the playfield rect
//	RECT rcPlayer1 = {0, 0, 100, 110};						// This is the rect used for updating player data

	MakeRect(131, 76, &rc, 0);								// Make the rect for the left side of the game board
	MakeRect(334, 76, &rc, 0);								// Make the rect for the right side of the game board
//	MakeRect(-3, 3, &rcPlayer1, NODRAW_LEFT);				// Make the rect for the first player
//	MakeRect(-3, 124, &rcPlayer1, NODRAW_LEFT);				// Make the rect for the second player
//	MakeRect(-3, 245, &rcPlayer1, NODRAW_LEFT);				// Make the rect for the third player
//	MakeRect(-3, 366, &rcPlayer1, NODRAW_LEFT);				// Make the rect for the fourth player
//	MakeRect(540, 3, &rcPlayer1, NODRAW_RIGHT);				// Make the rect for the timer box
//	MakeRect(540, 124, &rcPlayer1, NODRAW_RIGHT);			// Make the rect for the stage box
//	MakeRect(540, 245, &rcPlayer1, NODRAW_RIGHT);			// Make the rect for the count box
//	MakeRect(540, 366, &rcPlayer1, NODRAW_RIGHT);			// Make the rect for the score box
}


void Board3D::Draw_DDraw()
{
	int	 i, j;
	RECT rc			= {0, 0, 172, 329};		// This is the playfield rect
	RECT rcPlayer1	= {0, 0, 100, 110};		// This is the rect used for updating player data

	pBackSurf	= App.pDDraw->getBackBuffer();
	
	TileGameBoard();
	DrawAllRects();

//	DrawVitality(47, 95,  PLAYER_ONE, vit);					// Draw player 1 health
//	DrawVitality(47, 216, PLAYER_TWO, 75);					// Draw player 2 health
//	DrawVitality(47, 337, PLAYER_THREE, 50);				// Draw player 3 health
//	DrawVitality(47, 458, PLAYER_FOUR, 25);					// Draw player 4 health
//	App.pDDraw->DDrawText(545, 9, TF_SPACING_0, "TIMER");	// Draw the timer text
//	App.pDDraw->DDrawText(545, 130, TF_SPACING_0, "STAGE"); // Draw the stage text
//	App.pDDraw->DDrawText(545, 251, TF_SPACING_0, "COUNT"); // Draw the count text
//	App.pDDraw->DDrawText(545, 372, TF_SPACING_0, "SCORE"); // Draw the score text

	//***Draw our Gems
	for(i = 0; i < numRows; i++)
	{
		for(j = 0; j < numCols; j++)
		{
			Gem3D	*pCurGem	= gem[i] + j;
			pCurGem->pBackSurf	= pBackSurf;
			pCurGem->pGemSurf	= App.pDDraw->surfaceArray[0];
			pCurGem->Draw_DDraw();
		}
	}

	//*** show the player's cursor
	if(!GameData.Score.bGameFinished)
	{
		player.pBackSurf		= pBackSurf;
		player.pGemSurf			= App.pDDraw->surfaceArray[0];
		player.DisplayCursor();
	}
}


void GLSetup3DMode();
void GLSetup2DMode();
void Board3D::Draw_OpenGL()
{
	int				i, j;
	Vector3D		*pCam = &(GameData.pBoard->GetCamera())->worldPos;

	GLSetup3DMode();
	glViewport (0, 0, 640, 640);
	
	//**************
	//this will need to be modified depending on what side of the board we are updating
/*	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glEnable(GL_CLIP_PLANE2);
	glEnable(GL_CLIP_PLANE3);
*/	glDisable(GL_BLEND);
	glEnable(GL_CLIP_PLANE0);
	glTranslated(-100 + pCam->z*.135,0,0);
	
//	glEnable(GL_POLYGON_SMOOTH);
//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST ); 

	//***Draw our Gems on the Primary side (left side)
	for(i = 0; i < numRows; i++)
	{
		for(j = 0; j < numCols; j++)
		{
			Gem3D *pCurGem = gem[i] + j;
			pCurGem->Draw_OpenGL();
		}
	}
	glDisable(GL_CLIP_PLANE0);

	//*** show the player's cursor
	if(!GameData.Score.bGameFinished)
	{
		// show the player's cursor
		player.DisplayCursor();
	}


/*	GLdouble	clip0[4]= {0.0	,-1.0	,0.0	,160.0 - pCam->y},
				clip1[4]= {0.0	,1.0	,0.0	,320.0 + pCam->y};
	//clip planes
	glClipPlane(GL_CLIP_PLANE0,clip0);
	glClipPlane(GL_CLIP_PLANE1,clip1);

	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glEnable(GL_CLIP_PLANE4);
	glEnable(GL_CLIP_PLANE5);
*/
	//Turns off the Zoom for the right hand side
	glTranslated(-(pCam->z*.135),0,0);

	glEnable(GL_CLIP_PLANE1);
	Vector3D	*pV;
	//***Draw our Gems on the Secondary side (right side)
	for(i = 0; i < numRows; i++)
	{
		for(j = 0; j < numCols; j++)
		{
			Gem3D	*pCurGem	= gem[i] + j;
			pV=					pCurGem->GetCenterPos();

			glPushMatrix();
			glTranslated(220 - pCam->x,0 - pCam->y,0 - pCam->z);
			glTranslated(pV->x,pV->y,0);
			glRotated(180,0.0,180.0,0.0);
			glTranslated(-pV->x,-pV->y,0);

			pCurGem->Draw_OpenGL();

			glPopMatrix();
		}
	}
	glDisable(GL_CLIP_PLANE1);
	
//	glDisable(GL_POLYGON_SMOOTH);
//	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST ); 
	
	glEnable(GL_BLEND);
/*	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE4);
	glDisable(GL_CLIP_PLANE5);
*/

}




void Board3D::LoadSurfaces()
{

	if(!App.UserSettings.RenderMode)
	{
		//*** DDRAW MODE ***
		//surfaceArray	= new LPDD_SURF[NUM_OF_GAME_SURF];

		//for(int z = 0; z < NUM_OF_GAME_SURF; z++)
		//	surfaceArray[z]	= NULL;

		char	*surfFileName[NUM_OF_GAME_SURF]	= {	"Data//Pics//GAME_SURF_09.BMP",
													"Data//Pics//GAME_SURF_10.BMP",
													"Data//Pics//GAME_SURF_12.BMP",
													"Data//Pics//GAME_SURF_12.BMP"};
		
		int SurfaceIndex	= NUM_OF_MAIN_SURF + NUM_OF_MENU_SURF;
		int FileNameIndex	= 0;

		if(!App.UserSettings.RenderMode)
		{
			for(int i = SurfaceIndex; i < SurfaceIndex+NUM_OF_GAME_SURF; i++, FileNameIndex++)
			{
				if( !(App.pDDraw->surfaceArray[i]) )
				{
					App.pDDraw->LoadDDBitmap(App.pDDraw->surfaceArray + i, surfFileName[FileNameIndex], TRUE, 0, 0);
					App.pDDraw->SetTransparency(App.pDDraw->surfaceArray + i);
				}
			}
		}

	}
	else	//***OPEN_GL MODE
	{
		LoadGLBitmap (GL_SURF_GAME_CURSOR_FILENAME,		GL_SURF_GAME_CURSOR_TEXID,		1, 255);
		LoadGLBitmap (GL_SURF_GAME_SWITCHER_FILENAME,	GL_SURF_GAME_SWITCHER_TEXID,	1, 255);
		
		LoadGLBitmap (GL_SURF_GAME_CHARACTER_FILENAME,	GL_SURF_GAME_CHARACTER_TEXID,	1, 255);
		LoadGLBitmap (GL_SURF_GAME_YOUWIN_FILENAME,		GL_SURF_GAME_YOUWIN_TEXID,		1, 200);

		LoadGLBitmap (GL_SURF_GAME_BOARDER_FILENAME,	GL_SURF_GAME_BOARDER_TEXID,		1, 255);
	}

	//***LAME CODE that sets up local pointers inside of our gems
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numCols; j++)
		{
			gem[i][j].bDisplay2D	= 1;
			if(!App.UserSettings.RenderMode)
			{
				gem[i][j].pBackSurf		= App.pDDraw->getBackBuffer();
				gem[i][j].pGemSurf		= App.pDDraw->surfaceArray[0];
			}
			else
			{
				gem[i][j].pBackSurf		= NULL;
				gem[i][j].pGemSurf		= NULL;
			}
		}
	}
}
