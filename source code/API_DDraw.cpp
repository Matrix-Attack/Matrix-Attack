/***********************
DATE
PROJECT NAME
App Utilities
Andrew Kaplan
  (and a few changes made by Eli Emerson)
***********************/
//
//Standard includes
//

#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <dplay.h>
#include <dinput.h>
#include <dplobby.h>
#include <dsound.h>
#include <string>

using namespace std;

#include "./mmgr.h"


#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "gamemenu.h"
#include "vector.h"

#include "camera3d.h"
#include "model.h"
#include "gem3d.h"
#include "player.h"
#include "board.h"
#include "dinput.h"
#include "dsound.h"

// Sound includes
#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


LRESULT CALLBACK WndProc_DDraw (HWND Hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

void LoopUntilString(const char *string, FILE *pFile);

extern APP					App;
extern HWND					g_hWnd;
extern pSURFACERECORD		pSurfRecordHead;
extern pCALLOC_MEMRECORD	pMemRecordHead;
extern GAME_DATA			GameData;
//extern DSound				Sound;
extern CSound				cSound;

#define	OUTPUT_FILE "memleaks.txt"

void tagAPI_DDraw::DumpDDMainSurfaces ()
{
	if(lpDDSBack != NULL)
	{
		lpDDSBack->Release ();
		lpDDSBack = NULL;
	}

	if(lpDDSPrimary != NULL)
	{
		lpDDSPrimary->Release (); 
		lpDDSPrimary = NULL;
	}
}

HRESULT tagAPI_DDraw::ClearDDRegion (	LPDD_SURF	*lpdds,
								RECT			rect )
{
	DDBLTFX		ddbltfx;
	HRESULT		ddrval;
	char		txtval[350]="Clear Failed\n";

	ZeroMemory (&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize	=	sizeof(ddbltfx);
	ddbltfx.dwROP	=	BLACKNESS;
    
	ddrval = (*lpdds)->Blt (	&rect,
								(*lpdds),//NULL,
								NULL, 
								DDBLT_ROP| DDBLT_WAIT,
								&ddbltfx );

	return ddrval;
}

unsigned long tagAPI_DDraw::GetLowBit (unsigned long uiBitmask)
{
	UINT	uiBit = 1;
	unsigned long	i;

	//test for low bit
	for (i=0; i<32 ; i++)
	{
		if( uiBitmask & uiBit )
			return i;
		uiBit <<= 1;
	}
	return 0;
}

unsigned long tagAPI_DDraw::GetHiBit (unsigned long uiBitmask)
{
	UINT	uiBit = 1;
	unsigned long	i;

	//move to last position
	uiBit <<= 31;

	//test for high bit
	for ( i=0 ; i<32 ; i++ )
	{
		if ( uiBitmask & uiBit )
			return (31 - i);
		uiBit >>= 1;
	}
	return 0;
}

void tagAPI_DDraw::GetPixelInfo ()
{
	DDPIXELFORMAT	ddformat;
	unsigned long   HiBit;

	//set up structure
	ZeroMemory (&ddformat, sizeof(DDPIXELFORMAT));
	ddformat.dwSize = sizeof(DDPIXELFORMAT);
	
	//get pixel data
	lpDDSPrimary->GetPixelFormat (&ddformat);

	//store rgb count
	setRGBBitCount(ddformat.dwRGBBitCount);

	//store lobit and num bits/color
	setLoRedBit(GetLowBit (ddformat.dwRBitMask));
	HiBit = GetHiBit(ddformat.dwRBitMask);
	setNumRedBits((HiBit - getLoRedBit() + 1));

	setLoGreenBit(GetLowBit (ddformat.dwGBitMask));
	HiBit = GetHiBit(ddformat.dwGBitMask);
	setNumGreenBits((HiBit - getLoGreenBit() + 1));

	setLoBlueBit(GetLowBit (ddformat.dwBBitMask));
	HiBit = GetHiBit(ddformat.dwBBitMask);
	setNumBlueBits((HiBit - getLoBlueBit() + 1));
}

//////////////////////////////////////////////
//function sets up memory to be used for file mapping
//it returns a pointer to the memory location
//the handles to the file and filemap are passed as pointers
//since they can not be closed until manipulation/reading of the
//file is completed.  Thus the CloseHandle API is not called from
//this function but from where this function is called.
//////////////////////////////////////////////
PUCHAR
SetupFileMapping(HWND Hwnd,			//used to display the error boxex
				 LPCSTR FileName,		//indicates which file to be loaded
				 HANDLE *phFile,		//pointer to the file handle
				 HANDLE *phFileMap)	//pointer to the file map
{

	PUCHAR	pMemFile;
	
	*phFile = CreateFile(FileName,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
						(HANDLE)NULL);
	if(*phFile < 0)
	{
		MessageBox(Hwnd, "Couldn't open data file!", FileName, MB_OK | MB_ICONERROR);
		return NULL;
	}

	*phFileMap = CreateFileMapping(*phFile,
									NULL,
									PAGE_READONLY,
									0,
									0,
									NULL);

	if (!(*phFileMap))
	{
		MessageBox(Hwnd, "Couldn't CreateFilemap.", FileName, MB_OK | MB_ICONERROR);
		return NULL;
	}

	pMemFile = (PUCHAR) MapViewOfFile (*phFileMap,
										FILE_MAP_READ,
										0,
										0,
										0);
	//error check
	if (!pMemFile)
	{
		MessageBox(Hwnd, "Couldn't create MapViewOfFile.", FileName, MB_OK | MB_ICONERROR);
		return NULL;
	}

	return pMemFile;
}


BYTE*
BmImageLoad (HWND Hwnd, LPCSTR FilePath, long * pWidth,long * pHeight)
{
//****************************************************
//Purpose: This function will attempt to load a bitmap
//and return a handle to it if valid.  The method used
//to load the bitmap is the File Mapping technique.
//****************************************************

	HANDLE		hFile;				//used to get a handle of the file
	HANDLE		hFileMap;			//used for the file mapping
	BITMAPINFO	*pbmInfo;
	ULONG		bmBitColor;			//variable to contain the #colors info
	PUCHAR		pFile;				//used to poke around inside file :)

	pFile = SetupFileMapping(Hwnd, FilePath, &hFile, &hFileMap);

	if (!pFile)
		return NULL;
	else
	{
		pFile	+= sizeof(BITMAPFILEHEADER);	//jump past the fileheader

		pbmInfo	 = (BITMAPINFO *) pFile;		//convert info at file pointer and store
												//this accounts for variable infoheader size

		pFile	+= pbmInfo->bmiHeader.biSize;	//move file pointer to data location

		*pWidth = pbmInfo->bmiHeader.biWidth;	//return image width
		*pHeight= pbmInfo->bmiHeader.biHeight;	//return image height

		switch (pbmInfo->bmiHeader.biBitCount)
		{
			case 0:					//jpg file
				return NULL;

			case 1:					//monochrome
				bmBitColor = 2;
				break;
			case 4:					
				bmBitColor = 16;
				break;
			case 8:
				bmBitColor = 256;
				break;
			case 24:				//fall through
			default:
				bmBitColor = 0;
				break;
		}

		pFile	+= bmBitColor * sizeof(RGBQUAD);	//move file ptr to start of data

		CloseHandle(hFile);
		CloseHandle(hFileMap);
	}
	return (BYTE*)pFile;

}


/*PURPOSE
	This function initializes an application window ready for a full screen
	DirectDraw object.*/

int tagAPI_DDraw::SetupFullScreenWindow (
					LPCSTR		szTitle,	//Title of the application
					LPCSTR		szName,		//Name of the application
					int			iWidth,		//Width of the desired client area
					int			iHeight		//Height of the desired client area
				)
{

	WNDCLASS	wc;
	int rval;

	//Initial fields of the WndClass structure
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc_DDraw;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon (hInstance, "ICON");
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= szTitle;
	wc.lpszClassName	= szName;
	
	//Register class with OS
	rval = RegisterClass (&wc);

	//Get handle to the window from OS
	Hwnd = CreateWindowEx (	WS_EX_TOPMOST,
							szName,
							szTitle,
							WS_POPUP,
							0,
							0,
							iWidth,	
							iHeight,	
							NULL,
							NULL,
							hInstance,
							NULL );

	//Test to see if window was created succesfully
	if (!Hwnd)
	{
		char buff[64];
		
		int i = GetLastError();
		sprintf(buff, "Could not create Hwnd, error %d", i );
		MessageBox(NULL, buff, "error", MB_OK);
		if(i == 1407)
			MessageBox(NULL, "Cannot find Window Class!!!", "error 1407", MB_OK);
		else if(i == 2)
			MessageBox(NULL, "Regesterd class name probably diffrent\n then what was passed into CreateWin", "error 2", MB_OK);
		return 1;
	}
	
	g_hWnd = Hwnd;
	
	return 0;
}


/*
PURPOSE
	This function initializes an application window ready for a windowed
	DirectDraw object.*/

int tagAPI_DDraw::SetupWindowedWindow (
					LPCSTR		szTitle,	//Title of the application
					LPCSTR		szName,		//Name of the application
					int			iWidth,		//Width of the desired client area
					int			iHeight		//Height of the desired client area
				)
{

	WNDCLASS	wc;
	RECT		rc;

	//Initial fields of the WndClass structure
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc_DDraw;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon (hInstance, "ICON");
	wc.hCursor			= LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground	= NULL;
	wc.lpszMenuName		= szTitle;
	wc.lpszClassName	= szName;
	
	//Register class with OS
	RegisterClass (&wc);

	//Get handle to the window from OS
	Hwnd = CreateWindowEx (	0,
							szName,
							szTitle,
							WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
							0,
							0,
							0,	
							0,	
							NULL,
							NULL,
							hInstance,
							NULL);
	
	g_hWnd = Hwnd;

	//
	//Set the desired client area rectangle
	SetRect ( &rc, 0, 0, iWidth, iHeight );
	//
	//Adjust the window to include the borders
	AdjustWindowRectEx (	&rc,
							WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
							(BOOL)NULL,
							(DWORD)NULL);
	//
	//Set new window position and size
	MoveWindow (	Hwnd,
					GetSystemMetrics (SM_CXSCREEN)/2 - iWidth / 2,
					GetSystemMetrics (SM_CYSCREEN)/2 - iHeight / 2,
					rc.right - rc.left,
					rc.bottom - rc.top,
					FALSE );

	return 0;
}


/*
PURPOSE
	This function creates a Full Screen, DirectDraw Object, with a  
	primary and one back buffer surface. */


HRESULT tagAPI_DDraw::InitializeFullScreenDirectDraw (
	int					iWidth,		// Width of client area
	int					iHeight,	// Height of client area
	int					iColorBits	// Number of color bits in color depth
)
{
	
	HRESULT			ddrval;		//DirectDraw return value
	DDSURFACEDESC	ddsd;		//DirectDraw surface description
	DDSCAPS			ddscaps;	//
	DDBLTFX			ddbltfx;


	//
	//Set cooperative level to full screen exclusive.
	ddrval = lpDD->SetCooperativeLevel (Hwnd,DDSCL_EXCLUSIVE | 
												DDSCL_FULLSCREEN |
												DDSCL_ALLOWREBOOT );
	//
	//test
	/**/if ( ddrval != DD_OK )
		MessageBox (Hwnd, "SetCooperativeLevel Failed!",
		"Error Box", MB_OKCANCEL);
	//
	//Set display mode
	ddrval = lpDD->SetDisplayMode (iWidth, iHeight, iColorBits);
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "SetDisplayMode Failed!",
		"InitializeFullScreenDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Create Primary Surface
	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize		=	sizeof( ddsd );
	ddsd.dwFlags	=	DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps	=	DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;
	
	ddrval = lpDD->CreateSurface (&ddsd, &lpDDSPrimary, NULL );
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "CreateSurface Primary Failed!",
		"InitializeFullScreenDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Create off-screen surface for backbuffer
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	ddrval = lpDDSPrimary->GetAttachedSurface(&ddscaps, &lpDDSBack);
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "lpDDSPrimary->GetAttachedSurface Failed!",
		"InitializeFullScreenDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Set ROP code to blackness
	ZeroMemory (&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize	=	sizeof(ddbltfx);
	ddbltfx.dwROP	=	BLACKNESS;
	//
	//Fill back buffer
	ddrval = lpDDSBack->Blt (	NULL,
								lpDDSBack,
								NULL, 
								DDBLT_ROP | DDBLT_WAIT,
								&ddbltfx );
	//
	//test
	if ( ddrval != DD_OK )
	{
		MessageBox (Hwnd, "(*lpddsb)->Blt Failed!\nAttempt to fill backbuffer.",
					"InitializeFullScreenDirectDraw", MB_OK|MB_ICONERROR);
	}
	//
	//Fill primary surface
	ddrval = lpDDSPrimary->Blt (	NULL,
								lpDDSPrimary,//NULL,
								NULL, 
								DDBLT_ROP | DDBLT_WAIT,
								&ddbltfx );
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "(*lpddsp)->Blt Failed!\nAttempt to fill primary.",
					"InitializeFullScreenDirectDraw", MB_OK|MB_ICONERROR);

	return (ddrval);
}


/*PURPOSE
	This function creates a windowed DirectDraw Object with a  
	primary and one back buffer surface. */

HRESULT tagAPI_DDraw::InitializeWindowedDirectDraw (	
	int					iWidth,		// Width of client area
	int					iHeight		// Height of client area
)
{
	
	HRESULT				ddrval;		//DirectDraw return value
	DDSURFACEDESC		ddsd;		//DirectDraw surface description
	LPDIRECTDRAWCLIPPER	lpDDClipper;//DDraw Clipper
	//
	//Set cooperative level to normal for windowed mode
	ddrval = lpDD->SetCooperativeLevel (Hwnd, DDSCL_NORMAL);
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "SetCooperativeLevel Failed!",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Create Primary Surface
	ddsd.dwSize		=	sizeof( ddsd );
	ddsd.dwFlags	=	DDSD_CAPS;
	ddsd.ddsCaps.dwCaps	=	DDSCAPS_PRIMARYSURFACE;
	
	ddrval = lpDD->CreateSurface (&ddsd, &lpDDSPrimary, NULL );
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "CreateSurface Primary Failed!",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Create off-screen surface for backbuffer
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = iWidth;
	ddsd.dwHeight = iHeight;

	ddrval = lpDD->CreateSurface(&ddsd, &lpDDSBack, NULL);
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "CreateSurface Back Failed!",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Create clipper
	ddrval = lpDD->CreateClipper (0, &lpDDClipper, NULL);
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "CreateClipper Failed!",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Register our clipper with the o/s
	ddrval = lpDDClipper->SetHWnd(0, Hwnd );
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "SetHWnd for CreateClipper  Failed!\nDidn't register with system",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Associate our clipper with the primary surface
	//NOTE: This will advance the reference count to 2
	ddrval = lpDDSPrimary->SetClipper (lpDDClipper );
	//
	//test
	if ( ddrval != DD_OK )
		MessageBox (Hwnd, "SetClipper Failed!\nUnable to associate to primary",
		"InitializeWindowedDirectDraw", MB_OK|MB_ICONERROR);
	//
	//Release clipper
	//NOTE: This will reduce the reference count to 1
	//when the primary surface is released, the reference
	//count will be reduced to 0
	lpDDClipper->Release ();

	return ddrval;
}

BOOL tagAPI_DDraw::LoadImage24toSurface16 (	LPDIRECTDRAWSURFACE	lpdds,
										BYTE*				pImageBuf,
										int					iWidth,
										int					iHeight)
{
	DDSURFACEDESC	ddsd;
	int				iBytesReq = iWidth * 3;
	int				iBytesGiven = (iBytesReq + 3) & ~3; //assures byte alignment
	int				i, j;
	BYTE*			pImageBits;
	BYTE*			pSurfaceBits;
	float			fRed, fGreen, fBlue;
	USHORT*			pSurfPixel;
	RGBTRIPLE*		pImagePixel;
	WORD			r, g, b;
	float			fR, fG, fB;
	HRESULT			ddrval;

	//test inputs
	if( lpdds==NULL || pImageBuf==NULL )
		//bad inputs
		return FALSE;

	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	ddrval = lpdds->Lock ( 0, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );

	if ( ddrval != DD_OK)
		//surface lock failed
		return FALSE;

	//extract the pointer to the surface
	pSurfaceBits = (BYTE *) ddsd.lpSurface;

	//set pointer to ent of image bits
	pImageBits = (BYTE *) (&pImageBuf[(iHeight-1) * iBytesGiven]);

	//set adjustment factors
	fRed	= (float) 256 / (float) pow( 2, getNumRedBits() );
	fGreen	= (float) 256 / (float) pow( 2, getNumGreenBits() );
	fBlue	= (float) 256 / (float) pow( 2, getNumBlueBits() );

	//copy the image to the surface
	for ( i=0 ; i < iHeight ; i++ )
	{
		pSurfPixel = (USHORT*)pSurfaceBits;
		pImagePixel = (RGBTRIPLE*)pImageBits;
		for ( j=0 ; j < iWidth ; j++ )
		{
			fR = (float)pImagePixel->rgbtRed/fRed;
			fG = (float)pImagePixel->rgbtGreen/fGreen;
			fB = (float)pImagePixel->rgbtBlue/fBlue;

			r = (WORD) ((WORD)fR<<getLoRedBit());
			g = (WORD) ((WORD)fG<<getLoGreenBit());
			b = (WORD) ((WORD)fB<<getLoBlueBit());

			//now re-build the pixel in 16 bits
			*pSurfPixel = (WORD) (r|g|b);

			pSurfPixel++;
			pImagePixel++;
		}
		pSurfaceBits	+= ddsd.lPitch;
		pImageBits		-= iBytesGiven;	
	}
	lpdds->Unlock ( 0 );

	UnmapViewOfFile(pImageBuf);

	return TRUE;
}

BOOL tagAPI_DDraw::LoadImage24toSurface24 (	LPDIRECTDRAWSURFACE lpdds,
										BYTE*				pImageBuf,
										int					iWidth,
										int					iHeight)
{
	DDSURFACEDESC	ddsd;
	int				iBytesReq = iWidth * 3;
	int				iBytesGiven = (iBytesReq + 3) & ~3; //assures byte alignment
	int				i, j;
	BYTE*			pImageBits;
	BYTE*			pSurfaceBits;
	RGBTRIPLE*		pSurfPixel;
	RGBTRIPLE*		pImagePixel;
	DWORD			r, g, b;
	DWORD*			pData;
	HRESULT			ddrval;

	//test inputs
	if( lpdds==NULL || pImageBuf==NULL )
		//bad inputs
		return FALSE;

	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	ddrval = lpdds->Lock ( 0, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );

	if ( ddrval != DD_OK)
		//surface lock failed
		return FALSE;

	//extract the pointer to the surface
	pSurfaceBits = (BYTE *) ddsd.lpSurface;

	//set pointer to ent of image bits
	pImageBits = (BYTE *) (&pImageBuf[(iHeight-1) * iBytesGiven]);

	//copy the image to the surface
	for ( i=0 ; i < iHeight ; i++ )
	{
		pSurfPixel = (RGBTRIPLE*)pSurfaceBits;
		pImagePixel = (RGBTRIPLE*)pImageBits;
		for ( j=0 ; j < iWidth ; j++ )
		{
			r = pImagePixel->rgbtRed << getLoRedBit();
			g = pImagePixel->rgbtGreen << getLoGreenBit();
			b = pImagePixel->rgbtBlue << getLoBlueBit();
			pData = (DWORD*)pSurfPixel;

			//now re-build the pixel in 16 bits
			*pData = r|g|b;

			pSurfPixel++;
			pImagePixel++;
		}
		pSurfaceBits	+= ddsd.lPitch;
		pImageBits		-= iBytesGiven;	
	}
	lpdds->Unlock ( 0 );

	UnmapViewOfFile(pImageBuf);

	return TRUE;
}


BOOL tagAPI_DDraw::LoadImage24toSurface32 (	LPDIRECTDRAWSURFACE lpdds,
										BYTE*				pImageBuf,
										int					iWidth,
										int					iHeight)
{
	DDSURFACEDESC	ddsd;
	int				iBytesReq = iWidth * 3;
	int				iBytesGiven = (iBytesReq + 3) & ~3; //assures byte alignment
	int				i, j;
	BYTE*			pImageBits;
	BYTE*			pSurfaceBits;
	DWORD*			pSurfPixel;
	RGBTRIPLE*		pImagePixel;
	DWORD			r, g, b;
	DWORD*			pData;
	HRESULT			ddrval;

	//test inputs
	if( lpdds==NULL || pImageBuf==NULL )
		//bad inputs
		return FALSE;

	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	ddrval = lpdds->Lock ( 0, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );

	if ( ddrval != DD_OK)
		//surface lock failed
		return FALSE;

	//extract the pointer to the surface
	pSurfaceBits = (BYTE *) ddsd.lpSurface;

	//set pointer to ent of image bits
	pImageBits = (BYTE *) (&pImageBuf[(iHeight-1) * iBytesGiven]);

	//copy the image to the surface
	for ( i=0 ; i < iHeight ; i++ )
	{
		pSurfPixel = (DWORD*)pSurfaceBits;
		pImagePixel = (RGBTRIPLE*)pImageBits;
		for ( j=0 ; j < iWidth ; j++ )
		{
			r = pImagePixel->rgbtRed << getLoRedBit();
			g = pImagePixel->rgbtGreen << getLoGreenBit();
			b = pImagePixel->rgbtBlue << getLoBlueBit();
			pData = (DWORD*)pSurfPixel;

			//now re-build the pixel in 16 bits
			*pData = r|g|b;

			pSurfPixel++;
			pImagePixel++;
		}
		pSurfaceBits	+= ddsd.lPitch;
		pImageBits		-= iBytesGiven;	
	}
	lpdds->Unlock ( 0 );

	UnmapViewOfFile(pImageBuf);

	return TRUE;
}


/*
PURPOSE
	This function initializes an offscreen DirectDraw surface and loads from file
	a non-palletized bitmap onto the surface.  This function may also be used to
	create a blank offscreen surface.*/

HRESULT tagAPI_DDraw::LoadDDBitmap (	
	LPDIRECTDRAWSURFACE		*lpdds,		// Pointer to pointer to DD surface
	LPCSTR					szFilename,	// File name or NULL
	BOOL					bVideoMemory,//Load in video memory
	long					iWidth,		// Width of blank surface if NULL above
	long					iHeight		// Height of blank surface if NULL above
)
{
	HRESULT				ddrval;		//DirectDraw return value
	BYTE*				pImageBits;	//pointer to image bits
	DDSURFACEDESC		ddsd;		//DirectDraw surface description
	DDBLTFX				ddbltfx;	//DirectDraw Blt Effects structure
	pSURFACERECORD		pRecordTemp = NULL;
	char				txtval[300] = "";
	

	if (szFilename != NULL)
	{ 
		//
		//Load bitmap from file
		pImageBits = (BYTE*)BmImageLoad (Hwnd, szFilename, &iWidth, &iHeight);
		//
		//test
		if(pImageBits == NULL)
		{
			strcpy (txtval, "Load image failed for file:\n");
			strcat (txtval, szFilename);
			MessageBox (Hwnd, txtval,
			"LoadDDBitmap Error", MB_OK | MB_ICONERROR);
			return (HRESULT)DDERR_NOTFOUND;
		}
	}
	//
	//Create DirectDraw surface for this bitmap
	ZeroMemory (&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	if (bVideoMemory)
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	else
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	//set height and width
	ddsd.dwWidth	= iWidth;
	ddsd.dwHeight	= iHeight;

	ddrval = lpDD->CreateSurface(&ddsd, lpdds, NULL);
	//
	//test
	if(ddrval != DD_OK)
	{
		strcpy(txtval, "Couldn't create surface for bitmap:\n");
		if (szFilename)
			strcat(txtval, szFilename);
		MessageBox (Hwnd, txtval,
		"LoadDDBitmap Error", MB_OK | MB_ICONERROR);
		return ddrval;
	}

	if (szFilename != NULL)
	{
		if (getRGBBitCount()==16)
			//Load image 24 to surface 16
			LoadImage24toSurface16 ( *lpdds, pImageBits, iWidth, iHeight);
		else if (getRGBBitCount()==24)
			//Load image 24 to surface 24
			LoadImage24toSurface24 ( *lpdds, pImageBits, iWidth, iHeight);
		else if (getRGBBitCount()==32)
			//Load image 24 to surface 32
			LoadImage24toSurface32 ( *lpdds, pImageBits, iWidth, iHeight);

	}
	else
	{
		ZeroMemory (&ddbltfx, sizeof(ddbltfx));
		ddbltfx.dwSize	=	sizeof(ddbltfx);
		ddbltfx.dwROP	=	BLACKNESS;
		//
		//Fill surface
		ddrval = (*lpdds)->Blt (NULL,
								(*lpdds),//NULL,
								NULL, 
								DDBLT_ROP | DDBLT_WAIT,
								&ddbltfx );
		//
		//test
		if ( ddrval != DD_OK )
		{
			MessageBox (Hwnd, "lpDDSBackBuffer->Blt Failed!\nUnable to fill the surface for a no-filename option.\n",
						"LoadDDBitmap", MB_OK|MB_ICONERROR);
		}
	}


	if(pSurfRecordHead == NULL)
	{
		pSurfRecordHead = static_cast<pSURFACERECORD> (Mem_Alloc (sizeof(SURFACERECORD), "Record head DDBitmap"));

		pSurfRecordHead->bVidMem	= bVideoMemory;
		pSurfRecordHead->iHeight	= iHeight;
		pSurfRecordHead->iWidth		= iWidth;
		pSurfRecordHead->lpdds		= &(*lpdds);
		
		pSurfRecordHead->pNextRecord= NULL;
		pSurfRecordHead->pPrevRecord= NULL;

		if(szFilename != NULL)
			strcpy(pSurfRecordHead->szFileName, szFilename);
		else
			strcpy(pSurfRecordHead->szFileName, "blank");
	}
	else
	{	
		pRecordTemp = pSurfRecordHead;
		
		while(pRecordTemp->pNextRecord != NULL)
			pRecordTemp = pRecordTemp->pNextRecord;
		
		pRecordTemp->pNextRecord = static_cast<pSURFACERECORD> (Mem_Alloc (sizeof(SURFACERECORD), "New record DDBitmap"));
		
		pRecordTemp->pNextRecord->bVidMem		= bVideoMemory;
		pRecordTemp->pNextRecord->iHeight		= iHeight;
		pRecordTemp->pNextRecord->iWidth		= iWidth;
		pRecordTemp->pNextRecord->lpdds			= &(*lpdds);
		pRecordTemp->pNextRecord->pPrevRecord	= NULL;
		pRecordTemp->pNextRecord->pNextRecord	= NULL;
		if(szFilename != NULL)
			strcpy(pRecordTemp->pNextRecord->szFileName, szFilename);
		else
			strcpy(pRecordTemp->pNextRecord->szFileName, "blank");
		
		pRecordTemp->pNextRecord->pPrevRecord = pRecordTemp;
		
	}


	return (ddrval);
}

void tagAPI_DDraw::DeleteSurface (
	LPDIRECTDRAWSURFACE		*lpdds		// Pointer to pointer to DD surface
	)
{

	pSURFACERECORD pRecordTemp;

	pRecordTemp = pSurfRecordHead;

	while(pRecordTemp != NULL)
	{
		if(pRecordTemp->lpdds == &(*lpdds))
		{
			(*lpdds)->Release ();

			(*lpdds) = NULL;

			if((pRecordTemp->pPrevRecord == NULL) && (pRecordTemp->pNextRecord == NULL))
			{
				Mem_Free (pSurfRecordHead);
				pSurfRecordHead = NULL;
				return;
			}
			else if(pRecordTemp->pPrevRecord == NULL)
			{
				pSURFACERECORD pRecordTemp2;

				pRecordTemp2 = pRecordTemp;
				pSurfRecordHead = pRecordTemp->pNextRecord;
				pSurfRecordHead->pPrevRecord = NULL;
				Mem_Free (pRecordTemp2);
				return;
			}
			else if(pRecordTemp->pNextRecord == NULL)
			{
				pRecordTemp->pPrevRecord->pNextRecord = NULL;
				Mem_Free (pRecordTemp);
				return;
			}
			else
			{
				pRecordTemp->pPrevRecord->pNextRecord = pRecordTemp->pNextRecord;
				pRecordTemp->pNextRecord->pPrevRecord = pRecordTemp->pPrevRecord;
				Mem_Free (pRecordTemp);
				return;
			}
		}
		pRecordTemp = pRecordTemp->pNextRecord;
	}

	if((*lpdds) != NULL)
	{
		(*lpdds)->Release ();

		(*lpdds) = NULL;
	}
}


void tagAPI_DDraw::CheckReleasedSurfaces ()
{
	while(pSurfRecordHead != NULL)
	{
		MessageBox(
			Hwnd,
			pSurfRecordHead->szFileName,
			"Ahem!!! Surface not released!!",
			MB_OK);
		pSurfRecordHead = pSurfRecordHead->pNextRecord;
	}

}


///////////////////////////////////////////////////////////////////////////////
// This function looks at the image loaded to an offscreen surface and		 //
// sets that surfaces transparency color to the color in the upper most left //
// corner of the surface.                                                    //
//																			 //
// Randy Culley - Digipen Institute Of Technology.                           //
///////////////////////////////////////////////////////////////////////////////
/************************
borrowed from the collective.
hive mind
************************/


BOOL tagAPI_DDraw::SetTransparency(LPDIRECTDRAWSURFACE *surface)
{
	DWORD         srcColor;
	DDSURFACEDESC surfaceDesc;
	DDCOLORKEY	  transparency;

	// Get the surface Description so that we //
	// can lock it.                           //
	ZeroMemory(&surfaceDesc, sizeof(DDSURFACEDESC));
	surfaceDesc.dwSize = sizeof(DDSURFACEDESC);
	if FAILED((*surface)->GetSurfaceDesc(&surfaceDesc))
	{
		MessageBox(GetActiveWindow(),
				   "SetTransparency() - FAILED To Get Surface Description",
				   "ABORT MESSAGE",
				   MB_ICONEXCLAMATION | MB_OK);

		exit(0);
	}
	// Lock the surface. //	
	(*surface)->Lock(NULL, 
					 &surfaceDesc, 
					 DDLOCK_READONLY |
					 DDLOCK_WAIT, 
					 NULL);

	// Now to Get the color key of the //
	// first pixel on the surface.     //
	srcColor = *((DWORD *)surfaceDesc.lpSurface);

	// Actually set the color value. //
	transparency.dwColorSpaceLowValue = srcColor;
	transparency.dwColorSpaceHighValue = srcColor;

	(*surface)->SetColorKey(DDCKEY_SRCBLT, &transparency);

	// Unlock the surface. //
	(*surface)->Unlock(NULL);
	return TRUE;
}

void ReLinkButtonSurfaces()
{
	FILE *pFile = fopen("Data//Menu//MenuData.txt", "r");
	
	if(!pFile)
	{
		MessageBox(NULL,"Could not open file to read InitializeButtons","Error",0);
		return;
	}
	for(int i = 0; i < GameData.pGameMenu->GetNumButtons(); i++)
	{
//		Menu_Button	*pButton	= GameData.pGameMenu->buttonArray + i;
//		pButton->GetpSurf()		= GameData.pGameMenu->surfaceArray + pButton->surfIndex;


		Menu_Button	*pButton	= GameData.pGameMenu->buttonArray + i;
		int	a;
		// get the surface information
		LoopUntilString("surfaceID   : ", pFile);
		fscanf(pFile, "%d", &a);
//		pButton->pSurf		= GameData.pGameMenu->surfaceArray + a;
		pButton->surfIndex	= a;
	
	}
	fclose(pFile);
}

void tagAPI_DDraw::SwitchModes ()
{
	HRESULT		ddrval;
	RECT		rc = {0,0,App.UserSettings.ScreenWidth,App.UserSettings.ScreenHeight};


	//Check to see if any surfaces are not released
	CheckGameSurfaces();
	
	DumpDDMainSurfaces ();

	App.SetSwitchingMode(TRUE);

	if (bFullScreen)
	{
		DestroyWindow (Hwnd);
		if( SetupWindowedWindow (	GAME_NAME, GAME_NAME,
									rc.right, rc.bottom) )
			return;

		//
		//set up in window
		ddrval = InitializeWindowedDirectDraw (	rc.right, rc.bottom);

		ShowWindow(Hwnd, iCmdShow);

		// Reinitialize dinput here
		App.pdistruct->~DISTRUCT();

		InitDInputStruct(App.pDDraw->getHwnd(), App.pDDraw->getHInstance() );
		InitDirectInput();								
		InitDirectKeyInput();
		InitDirectJoystickInput();

		bFullScreen = FALSE; 
	}
	else
	{
		DestroyWindow (Hwnd);

		if( SetupFullScreenWindow (	GAME_NAME, GAME_NAME,
									rc.right, rc.bottom) )
			return;

		//
		//set up fullscreen
		InitializeFullScreenDirectDraw (rc.right, rc.bottom, App.UserSettings.BitDepth);

		// Reinitialize dinput here
		App.pdistruct->~DISTRUCT();

		InitDInputStruct(App.pDDraw->getHwnd(), App.pDDraw->getHInstance() );
		InitDirectInput();								
		InitDirectKeyInput();
		InitDirectJoystickInput();

		bFullScreen = TRUE;
	}
	App.SetSwitchingMode(FALSE);

	//Store surface data
	GetPixelInfo ();

	ClearDDRegion (&lpDDSBack, rc);
	ClearDDRegion (&lpDDSPrimary, rc);

	if(!surfaceArray)
	{
		numSurfaces = NUM_OF_MAIN_SURF + NUM_OF_MENU_SURF + NUM_OF_GAME_SURF + NUM_OF_TITLE_SURF;
		
		surfaceArray = new LPDD_SURF[numSurfaces];
		for(int i = 0; i < numSurfaces; i++)
			surfaceArray[i]	= NULL;
	}

	//Always load the main surfaces
	LoadMainSurfaces();
	
	//Always load the menu surfaces
	GameData.pGameMenu->LoadSurfaces();

	//If the game has been loaded before.. .load it again.
	if(GameData.pBoard)
	{
		GameData.pBoard->LoadSurfaces();
	}
}

void tagAPI_DDraw::CheckResult( HRESULT result )
{
	switch ( result )
	{
	case DDERR_ALREADYINITIALIZED:
		MessageBox(Hwnd, "DDERR_ALREADYINITIALIZED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_BLTFASTCANTCLIP:
		MessageBox(Hwnd, "DDERR_BLTFASTCANTCLIP",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANNOTATTACHSURFACE:
		MessageBox(Hwnd, "DDERR_CANNOTATTACHSURFACE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANNOTDETACHSURFACE:
		MessageBox(Hwnd, "DDERR_CANNOTDETACHSURFACE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANTCREATEDC:
		MessageBox(Hwnd, "DDERR_CANTCREATEDC",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANTDUPLICATE:
		MessageBox(Hwnd, "DDERR_CANTDUPLICATE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANTLOCKSURFACE:
		MessageBox(Hwnd, "DDERR_CANTLOCKSURFACE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANTPAGELOCK:
		MessageBox(Hwnd, "DDERR_CANTPAGELOCK",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CANTPAGEUNLOCK:
		MessageBox(Hwnd, "DDERR_CANTPAGEUNLOCK",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CLIPPERISUSINGHWND:
		MessageBox(Hwnd, "DDERR_CLIPPERISUSINGHWND",
			"DD_ERR", MB_OK);
		break;
	case DDERR_COLORKEYNOTSET:
		MessageBox(Hwnd, "DDERR_COLORKEYNOTSET",
			"DD_ERR", MB_OK);
		break;
	case DDERR_CURRENTLYNOTAVAIL:
		MessageBox(Hwnd, "DDERR_CURRENTLYNOTAVAIL",
			"DD_ERR", MB_OK);
		break;
	case DDERR_DCALREADYCREATED:
		MessageBox(Hwnd, "DDERR_DCALREADYCREATED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_DIRECTDRAWALREADYCREATED:
		MessageBox(Hwnd, "DDERR_DIRECTDRAWALREADYCREATED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_EXCEPTION:
		MessageBox(Hwnd, "DDERR_EXCEPTION",
			"DD_ERR", MB_OK);
		break;
	case DDERR_EXCLUSIVEMODEALREADYSET:
		MessageBox(Hwnd, "DDERR_EXCLUSIVEMODEALREADYSET",
			"DD_ERR", MB_OK);
		break;
	case DDERR_GENERIC:
		MessageBox(Hwnd, "DDERR_GENERIC",
			"DD_ERR", MB_OK); 
		break;
	case DDERR_HEIGHTALIGN:
		MessageBox(Hwnd, "DDERR_HEIGHTALIGN",
			"DD_ERR", MB_OK);
		break;
	case DDERR_HWNDALREADYSET:
		MessageBox(Hwnd, "DDERR_HWNDALREADYSET",
			"DD_ERR", MB_OK);
		break;
	case DDERR_HWNDSUBCLASSED:
		MessageBox(Hwnd, "DDERR_HWNDSUBCLASSED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_IMPLICITLYCREATED:
		MessageBox(Hwnd, "DDERR_IMPLICITLYCREATED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INCOMPATIBLEPRIMARY:
		MessageBox(Hwnd, "Incampatible Primary",
			"DD_ERR", MB_OK); 
		break;
	case DDERR_INVALIDCAPS:
		MessageBox(Hwnd, "DDERR_INVALIDCAPS",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDCLIPLIST:
		MessageBox(Hwnd, "DDERR_INVALIDCLIPLIST",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDDIRECTDRAWGUID:
		MessageBox(Hwnd, "DDERR_INVALIDDIRECTDRAWGUID",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDMODE:
		MessageBox(Hwnd, "DDERR_INVALIDMODE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDOBJECT:
		MessageBox(Hwnd, "DDERR_INVALIDOBJECT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDPARAMS:
		MessageBox(Hwnd, "DDERR_INVALIDPARAMS",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDPIXELFORMAT:
		MessageBox(Hwnd, "DDERR_INVALIDPIXELFORMAT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDPOSITION:
		MessageBox(Hwnd, "DDERR_INVALIDPOSITION",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDRECT:
		MessageBox(Hwnd, "DDERR_INVALIDRECT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_INVALIDSURFACETYPE:
		MessageBox(Hwnd, "DDERR_INVALIDSURFACETYPE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_LOCKEDSURFACES:
		MessageBox(Hwnd, "DDERR_LOCKEDSURFACES",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NO3D:
		MessageBox(Hwnd, "DDERR_NO3D",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOALPHAHW:
		MessageBox(Hwnd, "DDERR_NOALPHAHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOBLTHW:
		MessageBox(Hwnd, "DDERR_NOBLTHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCLIPLIST:
		MessageBox(Hwnd, "DDERR_NOCLIPLIST",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCLIPPERATTACHED:
		MessageBox(Hwnd, "DDERR_NOCLIPPERATTACHED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCOLORCONVHW:
		MessageBox(Hwnd, "DDERR_NOCOLORCONVHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCOLORKEY:
		MessageBox(Hwnd, "DDERR_NOCOLORKEY",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCOLORKEYHW:
		MessageBox(Hwnd, "no colorkey hardware",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOCOOPERATIVELEVELSET:
		MessageBox(Hwnd, "DDERR_NOCOOPERATIVELEVELSET",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NODC:
		MessageBox(Hwnd, "DDERR_NODC",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NODDROPSHW:
		MessageBox(Hwnd, "DDERR_NODDROPSHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NODIRECTDRAWHW:
		MessageBox(Hwnd, "DDERR_NODIRECTDRAWHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NODIRECTDRAWSUPPORT:
		MessageBox(Hwnd, "DDERR_NODIRECTDRAWSUPPORT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOEMULATION:
		MessageBox(Hwnd, "DDERR_NOEMULATION",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOEXCLUSIVEMODE:
		MessageBox(Hwnd, "DDERR_NOEXCLUSIVEMODE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOFLIPHW:
		MessageBox(Hwnd, "DDERR_NOFLIPHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOGDI:
		MessageBox(Hwnd, "DDERR_NOGDI",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOHWND:
		MessageBox(Hwnd, "DDERR_NOHWND",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOMIPMAPHW:
		MessageBox(Hwnd, "DDERR_NOMIPMAPHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOMIRRORHW:
		MessageBox(Hwnd, "DDERR_NOMIRRORHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOOVERLAYDEST:
		MessageBox(Hwnd, "DDERR_NOOVERLAYDEST",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOOVERLAYHW:
		MessageBox(Hwnd, "DDERR_NOOVERLAYHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOPALETTEATTACHED:
		MessageBox(Hwnd, "DDERR_NOPALETTEATTACHED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOPALETTEHW:
		MessageBox(Hwnd, "DDERR_NOPALETTEHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NORASTEROPHW:
		MessageBox(Hwnd, "DDERR_NORASTEROPHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOROTATIONHW:
		MessageBox(Hwnd, "DDERR_NOROTATIONHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOSTRETCHHW:
		MessageBox(Hwnd, "DDERR_NOSTRETCHHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOT4BITCOLOR:
		MessageBox(Hwnd, "DDERR_NOT4BITCOLOR",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOT4BITCOLORINDEX:
		MessageBox(Hwnd, "DDERR_NOT4BITCOLORINDEX",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOT8BITCOLOR:
		MessageBox(Hwnd, "DDERR_NOT8BITCOLOR",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTAOVERLAYSURFACE:
		MessageBox(Hwnd, "DDERR_NOTAOVERLAYSURFACE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTEXTUREHW:
		MessageBox(Hwnd, "DDERR_NOTEXTUREHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTFLIPPABLE:
		MessageBox(Hwnd, "DDERR_NOTFLIPPABLE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTFOUND:
		MessageBox(Hwnd, "DDERR_NOTFOUND",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTINITIALIZED:
		MessageBox(Hwnd, "DDERR_NOTINITIALIZED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTLOCKED:
		MessageBox(Hwnd, "DDERR_NOTLOCKED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTPAGELOCKED:
		MessageBox(Hwnd, "DDERR_NOTPAGELOCKED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOTPALETTIZED:
		MessageBox(Hwnd, "DDERR_NOTPALETTIZED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOVSYNCHW:
		MessageBox(Hwnd, "DDERR_NOVSYNCHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOZBUFFERHW:
		MessageBox(Hwnd, "DDERR_NOZBUFFERHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_NOZOVERLAYHW:
		MessageBox(Hwnd, "DDERR_NOZOVERLAYHW",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OUTOFCAPS:
		MessageBox(Hwnd, "DDERR_OUTOFCAPS",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OUTOFMEMORY:
		MessageBox(Hwnd, "Out of Memory",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OUTOFVIDEOMEMORY:
		MessageBox(Hwnd, "DDERR_OUTOFVIDEOMEMORY",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OVERLAYCANTCLIP:
		MessageBox(Hwnd, "DDERR_OVERLAYCANTCLIP",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
		MessageBox(Hwnd, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_OVERLAYNOTVISIBLE:
		MessageBox(Hwnd, "DDERR_OVERLAYNOTVISIBLE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_PALETTEBUSY:
		MessageBox(Hwnd, "DDERR_PALETTEBUSY",
			"DD_ERR", MB_OK);
		break;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		MessageBox(Hwnd, "DDERR_PRIMARYSURFACEALREADYEXISTS",
			"DD_ERR", MB_OK);
		break;
	case DDERR_REGIONTOOSMALL:
		MessageBox(Hwnd, "DDERR_REGIONTOOSMALL",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACEALREADYATTACHED:
		MessageBox(Hwnd, "DDERR_SURFACEALREADYATTACHED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACEALREADYDEPENDENT:
		MessageBox(Hwnd, "DDERR_SURFACEALREADYDEPENDENT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACEBUSY:
		MessageBox(Hwnd, "DDERR_SURFACEBUSY",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACEISOBSCURED:
		MessageBox(Hwnd, "DDERR_SURFACEISOBSCURED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACELOST:
		MessageBox(Hwnd, "DDERR_SURFACELOST",
			"DD_ERR", MB_OK);
		break;
	case DDERR_SURFACENOTATTACHED:
		MessageBox(Hwnd, "DDERR_SURFACENOTATTACHED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_TOOBIGHEIGHT:
		MessageBox(Hwnd, "DDERR_TOOBIGHEIGHT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_TOOBIGSIZE:
		MessageBox(Hwnd, "DDERR_TOOBIGSIZE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_TOOBIGWIDTH:
		MessageBox(Hwnd, "DDERR_TOOBIGWIDTH",
			"DD_ERR", MB_OK);
		break;
	case DDERR_UNSUPPORTED:
		MessageBox(Hwnd, "DDERR_UNSUPPORTED",
			"DD_ERR", MB_OK);
		break;
	case DDERR_UNSUPPORTEDFORMAT:
		MessageBox(Hwnd, "DDERR_UNSUPPORTEDFORMAT",
			"DD_ERR", MB_OK);
		break;
	case DDERR_UNSUPPORTEDMASK:
		MessageBox(Hwnd, "DDERR_UNSUPPORTEDMASK",
			"DD_ERR", MB_OK);
		break;
	case DDERR_UNSUPPORTEDMODE:
		MessageBox(Hwnd, "DDERR_UNSUPPORTEDMODE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_VERTICALBLANKINPROGRESS:
		MessageBox(Hwnd, "DDERR_VERTICALBLANKINPROGRESS",
			"DD_ERR", MB_OK);
		break;
	case DDERR_WASSTILLDRAWING:
		MessageBox(Hwnd, "DDERR_WASSTILLDRAWING",
			"DD_ERR", MB_OK);
		break;
	case DDERR_WRONGMODE:
		MessageBox(Hwnd, "DDERR_WRONGMODE",
			"DD_ERR", MB_OK);
		break;
	case DDERR_XALIGN:
		MessageBox(Hwnd, "DDERR_XALIGN",
			"DD_ERR", MB_OK);
		break;
	default:
		MessageBox(Hwnd, "Unknown return value",
			"DD_ERR", MB_OK);
		break;
	}
}


//
//This function adds a record to the allocation list
void tagAPI_DDraw::Mem_AddRef(void * ptr, char *szRefName)
{
	pCALLOC_MEMRECORD pTemp;

	if(pMemRecordHead == NULL)
	{
		//New list, Calloc a node for the record
		
		pMemRecordHead = (pCALLOC_MEMRECORD) malloc ( sizeof(CALLOC_MEMRECORD) );
		memset(pMemRecordHead, 0, sizeof(CALLOC_MEMRECORD) );
		
		//test
		if(!pMemRecordHead)
		{
			//Cannot set memory
			MessageBox (Hwnd, "Insufficient memory to Calloc!!!",
				"Error Box", MB_OK);
			return;
		}
	}
	else
	{
		//Not a new list, Calloc a node for the record
		pTemp = (pCALLOC_MEMRECORD) malloc ( sizeof(CALLOC_MEMRECORD));
		memset(pTemp, 0, sizeof(CALLOC_MEMRECORD) );

		//test
		if(!pTemp)
		{
			//Cannot set memory
			MessageBox (Hwnd, "Insufficient memory to Calloc!!!",
				"Error Box", MB_OK);
			return;
		}

		//Now add the node
		pTemp->pNextRecord = pMemRecordHead;
		pMemRecordHead = pTemp;
	}
	//record address of memory allocation
	pMemRecordHead->pAddress = ptr;

	//Record the reference info
	strcpy(pMemRecordHead->szRef,szRefName);
}

void * tagAPI_DDraw::Mem_Alloc(int size, char *szRefName)
{
	static short sFlag = 1;
	void * pChunk = NULL;

//used to debug bad allocation sizes when direct X full screen
/*	if (size < 0)
	{
		FILE *fp;
		fp = fopen("memLarge.txt", "wt");
		if (fp)
		{
			fprintf(fp, "%s", szRefName);
			fclose(fp);
		}
	}
*/	
	if(sFlag)
	{
		//First time through, initialize the RecordHead.
		pMemRecordHead = NULL;
		
		//Reset the flag
		sFlag = 0;
	}

	//get chunck of memory
//	pChunk = (void*) calloc (size, sizeof(char));
	pChunk = (void*) malloc ( size * sizeof(char) );
	
	//Test
	if(!pChunk)
	{
		//Cannot set memory
		MessageBox (Hwnd, "Insufficient memory to Malloc!!!", "Error Box", MB_OK);

		pChunk = NULL;
		return pChunk;
	}
	//
	//record address of memory allocation
	Mem_AddRef (pChunk, szRefName);

	return pChunk;
}


void tagAPI_DDraw::Mem_Free(void* ptr)
{
	pCALLOC_MEMRECORD pTemp, pTemp2;

	//
	//test
	if(ptr == NULL)
	{
		//Cannot find this node
		MessageBox (Hwnd, "You are trying to Free a null pointer.",
			"Mem_Free", MB_OK);

		return;
	}

	pTemp = pMemRecordHead;
	pTemp2 = pTemp;
	//
	//run the list to find the node
	while ((pTemp != NULL) && (pTemp->pAddress != ptr))
	{
		pTemp2 = pTemp;
		pTemp = pTemp->pNextRecord;
	}
	//
	//test
	if((pTemp == NULL) || (pTemp->pAddress == NULL))
	{
		//Cannot find this node
		MessageBox (Hwnd, "Cannot find this node!!!",
			"Mem_Free", MB_OK);

		return;
	}
	else if(pTemp == pMemRecordHead)
			//Freeing head node
			pMemRecordHead = pTemp->pNextRecord;
		else
			//repair list
			pTemp2->pNextRecord = pTemp->pNextRecord;
	//
	//clean up
	free(pTemp->pAddress);
	pTemp->pAddress = NULL;
	free(pTemp);
	pTemp = NULL;
}

//this function prints out any allocated memory that isn't freed to a text file.
void tagAPI_DDraw::Mem_CheckForLeaks()
{
	pCALLOC_MEMRECORD	pTemp;
	FILE	*fp;
	int		count = 0;

	//Create a time structure, get the time info
	//then format it for local time and output to the file
	struct tm *pTime;
	time_t TimeStruct;
	char DayNight[3] = "AM";

	//capture the current time info
	time(&TimeStruct);

	//convert to the computers local time
	pTime = localtime(&TimeStruct);

	//check if its day or night
	//and change from military time if so
	if( pTime->tm_hour > 12 )
	{
		strcpy( DayNight, "PM" );
		pTime->tm_hour -= 12;
	}
	//oh yeah need to also check if its midnight
	else if( pTime->tm_hour == 0 )
	{
		pTime->tm_hour = 12;
	}

	pTemp = pMemRecordHead;

	fp = fopen ("memleaks.txt" , "wt");
	if (fp)
	{
		//Output the time data to the file
		//the asctime function converts the time data to a string
		fprintf( fp, "%.19s %s\n", asctime( pTime ), DayNight );
		if (!pTemp)
		{
			fprintf(fp, "No leaks.\n");
		}

		while(pTemp != NULL)
		{
			//The list is not empty
			fprintf (fp, "%4d: %s \n", ++count, pTemp->szRef);
			pTemp = pTemp->pNextRecord;	
		}

		fclose (fp);
	}
}

void tagAPI_DDraw::Mem_CheckPtr(void* ptr)
{
	pCALLOC_MEMRECORD pTemp;

	pTemp = pMemRecordHead;
	//
	//run the list
	while ((pTemp->pAddress != ptr) && (pTemp != NULL))
		pTemp = pTemp->pNextRecord;
	//
	//test to see that we found the node
	if(pTemp == NULL)
	{
		//Cannot find this node
		MessageBox (Hwnd, "Invalid address!!!",
			"Mem_CheckPtr", MB_OK);

		return ;
	}
}

BOOL tagAPI_DDraw::DoInit ()
{
//	char	txtval[MAXFILELENGTH] = "";
	RECT	rc = {0,0,App.UserSettings.ScreenWidth,App.UserSettings.ScreenHeight};
	//

	if (!bFullScreen)
	{
		HRESULT				ddrval;
		//
		//Create DirectDraw Object
		ddrval = DirectDrawCreate ( NULL, &lpDD, NULL);
		//
		//test
		if ( ddrval != DD_OK )
			MessageBox (Hwnd, "DirectDrawCreate Failed!",
			"Error Box", MB_OKCANCEL);

		//Initialize windowed DirectDraw with a primary and one 
		//back buffer.
		InitializeWindowedDirectDraw (App.UserSettings.ScreenWidth,
									  App.UserSettings.ScreenHeight );
		//Store surface data
		GetPixelInfo();

		if (getRGBBitCount() != 16)
		{
			//Let the user know that he/she is not in 16-bit mode
			MessageBox(Hwnd, 
				"Unable to setup Direct Draw Windowed mode.\nThis is because your desktop is not in 16-bit mode.\nForcing Fullscreen mode to compensate.", 
				"STARTUP ERROR", MB_OK | MB_ICONEXCLAMATION);
			
			DumpDDMainSurfaces ();
			
			if(lpDD)
			{
				lpDD->Release();

				lpDD = NULL;
			}



			if(Hwnd)
			{
				App.SetSwitchingMode(TRUE);

				DestroyWindow (Hwnd);

				Hwnd = NULL;

				App.SetSwitchingMode(FALSE);
			}

			bFullScreen		= TRUE;
			bAllowFullScreen= FALSE;

			return FALSE;
		}

		//clean off our surfaces before we start
		ClearDDRegion (&lpDDSBack, rc);
		ClearDDRegion (&lpDDSPrimary, rc);


		if (ddrval != DD_OK)
			//
			//Something is wrong, we can't live in a window
			return FALSE;
	}
	else
	{
		HRESULT				ddrval;
		//
		//Create DirectDraw Object
		ddrval = DirectDrawCreate ( NULL, &lpDD, NULL);
		//
		//test
		if ( ddrval != DD_OK )
			MessageBox (Hwnd, "DirectDrawCreate Failed!",
			"Error Box", MB_OKCANCEL);

		//Initialize DirectDraw with a primary and one 
		//back buffer.
		InitializeFullScreenDirectDraw (App.UserSettings.ScreenWidth,
										App.UserSettings.ScreenHeight,
										App.UserSettings.BitDepth );

		//clean off our surfaces before we start
		ClearDDRegion (&lpDDSBack, rc);
		ClearDDRegion (&lpDDSPrimary, rc);

		//
		//test
		if ( ddrval != DD_OK )
		{
			MessageBox (Hwnd, "InitializeWindowedDirectDraw Failed!",
			"Error Box", MB_OKCANCEL);
			//
			//Something is wrong, the vid card must not support
			//800 X 600 X 16
			return FALSE;
		}
	}
	//Store surface data
	GetPixelInfo();

	return TRUE;
}

int tagAPI_DDraw::Initialize()
{
	BOOL bResult = TRUE;

	int UserWidth	= App.UserSettings.ScreenWidth;
	int UserHeight	= App.UserSettings.ScreenHeight;

	BOOL bWantFS	= App.UserSettings.bFullScreen;

	//
	//Let's see what we have to work with (desktop settings)
	if (!bFullScreen && (GetSystemMetrics (SM_CXSCREEN) > UserWidth ||
		GetSystemMetrics (SM_CYSCREEN) > UserHeight)	)
	{
		
		//
		//Try to set up a windowed app
		if ( SetupWindowedWindow(APP_NAME,
								GAME_NAME,
								UserWidth,
								UserHeight) )
								return 1;
		//
		//test
		if ( (Hwnd  != NULL) && DoInit () )
		{	
			//
			//WOO HOO!!!  We can live in a window.  Let's do it!
			bFullScreen		= FALSE;
			bAllowFullScreen= TRUE;

			if ( bResult == FALSE )
				MessageBox (Hwnd , "DoInit returned False for windowed mode\n setting for DDraw.",
					"IniWindow", MB_OK|MB_ICONERROR);

			if(bWantFS)
				App.pDDraw->SwitchModes();
		}
		else
		{
			//
			//Can't do window, color depth is too low.
			//Let's give full screen a shot
			if( SetupFullScreenWindow (	APP_NAME,
										GAME_NAME,
										UserWidth,
										UserHeight) )
										return 1;
			//
			//test
			if ( Hwnd  == NULL )
			MessageBox (Hwnd , "Unable to set up window or full screen!\nSetupFullScreenWindow returned NULL\nAfter attempting windowed mode",
				"IniWindow", MB_OK|MB_ICONERROR);
			//
			//Try to set up DirectDraw full screen
			bResult = DoInit ();
			bFullScreen = TRUE;
			if ( bResult == FALSE )
				MessageBox (Hwnd , "Unable to set up window or full screen!\nDoInit returned False for fullscreen DDraw\nAfter attemtped windowed mode",
					"IniWindow", MB_OK|MB_ICONERROR);
		}
	}
	else
	{
		bAllowFullScreen= TRUE;
		//Screen is to small for us to live in a window.
		//Let's give full screen a shot
		if(SetupFullScreenWindow ( APP_NAME,
									GAME_NAME,
									UserWidth,
									UserHeight) )
									return 1;
		//
		//test
		if ( Hwnd  == NULL )
			MessageBox (Hwnd , "Unable to set up window or full screen!\nSetupFullScreenWindow returned NULL",
				"IniWindow", MB_OK|MB_ICONERROR);
		//
		//Try to set up DirectDraw full screen
		bResult = DoInit ();
		bFullScreen = TRUE;
		if ( bResult == FALSE )
			MessageBox (Hwnd , "Unable to set up window or full screen!\nDoInit returned FALSE for setting DDraw FullScreen",
				"IniWindow", MB_OK|MB_ICONERROR);
	}
	
	ShowWindow(Hwnd, iCmdShow);

	//***Clear the backbuffer
	ClearSurface(lpDDSBack, 0 );

	numSurfaces = NUM_OF_MAIN_SURF + NUM_OF_MENU_SURF + NUM_OF_GAME_SURF + NUM_OF_TITLE_SURF;

	if(!surfaceArray)
	{

		surfaceArray = new LPDD_SURF[numSurfaces];
		for(int i = 0; i < numSurfaces; i++)
			surfaceArray[i]	= NULL;

		LoadMainSurfaces();
	}


	//***LOCK THE SURFACE FOR THE FIRST TIME!
	//***Init RGB16 (lock the surface to get a real pointer to the backbuffer)
	bNotInitializedFlag = 1;
	bNotInitializedFlag = InitRGB16();
	if( bNotInitializedFlag)
		MessageBox(Hwnd, "Can't Seem to InitRGB16 so we will never try again.",
			"error when we tried to lock the surface", MB_OK);

	return 0;
}

void tagAPI_DDraw::LoadMainSurfaces()
{
	char	*surfFileName[NUM_OF_MAIN_SURF]	= {	"Data//Pics//MAIN_SURF_00.bmp"};
	
	for(int i = 0; i < NUM_OF_MAIN_SURF; i++)
	{
		if( !(surfaceArray[i]) )
		{
			App.pDDraw->LoadDDBitmap(surfaceArray + i, surfFileName[i], TRUE, 0, 0);
			App.pDDraw->SetTransparency(surfaceArray + i);
		}

	}

	//***Special case for the TitleScreen
	App.pDDraw->LoadDDBitmap(&surfaceArray[13], "Data//Pics//MAIN_SURF_13.bmp", TRUE, 0, 0);
	App.pDDraw->SetTransparency(&surfaceArray[13]);

	App.pDDraw->LoadDDBitmap(&surfaceArray[14], "Data//Pics//MAIN_SURF_14.bmp", TRUE, 0, 0);
	App.pDDraw->SetTransparency(&surfaceArray[14]);
}

void tagAPI_DDraw::Destroy( void )
{
	//Check to see if any surfaces are not released
	CheckGameSurfaces();

	DumpDDMainSurfaces();
	CheckReleasedSurfaces();
	Mem_CheckForLeaks();

	if(lpDD != NULL)
	{
		lpDD->Release ();
		lpDD = NULL;
	}

	//CLEAN UP ELI's CRAP!!! -Eli
	CleanUpParticleEngine();
}

tagAPI_DDraw::~tagAPI_DDraw()
{
	Destroy();
}

void tagAPI_DDraw::CheckGameSurfaces()
{
	if(surfaceArray)
	{
		//Check main DDraw surfaces...
		for(int i=0; i < numSurfaces; i++)
		{
			if(surfaceArray[i])
				App.pDDraw->DeleteSurface(&surfaceArray[i]);
		}
		delete[] surfaceArray;
		surfaceArray = NULL;
	}

/*	//Checking GameMenu surfaces...
	if(GameData.pGameMenu != NULL)
	{
		if(GameData.pGameMenu->GetNumSurfaces())
			GameData.pGameMenu->ReleaseSurfaces();
	}
	
	//Check In-Game surfaces...
	if(GameData.pBoard != NULL)
	{
		if(GameData.pBoard->GetNumSurfaces() )
			GameData.pBoard->ReleaseSurfaces();	
	}
*/
//	if(GameData.pGameMenu->buttonArray != NULL)
//		delete[] GameData.pGameMenu->buttonArray;
	
}

void tagAPI_DDraw::FlipBuffers()
{
	if(App.UserSettings.bWaitOnVSynch)
		getDDObj()->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
	
	int ddrval;

	if (bFullScreen)
	{
		//Flip
		//ddrval = lpDDSPrimary->Flip(NULL, DDFLIP_WAIT);
		ddrval = lpDDSPrimary->Blt(NULL, lpDDSBack, 
									NULL, DDBLT_WAIT, NULL);
		//test
		if ( ddrval != DD_OK )
			MessageBox (Hwnd, "lpDDS[PRIMARY]->Flip Failed!",
							"BuildFrame", MB_OK|MB_ICONEXCLAMATION);
	}
	else
	{
		//Fake Flip
		ddrval = lpDDSPrimary->Blt(&rcWindow, lpDDSBack, 
									NULL, DDBLT_WAIT, NULL);
	}

}