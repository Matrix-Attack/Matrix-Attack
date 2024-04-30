
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>

#include "./mmgr.h"

#include "DPlay.h"
//#include "database.h"
#include "App.h"

extern APP App;


void tagAPI_DDraw::InterruptFrame()
{
	if (bNotInitializedFlag == TRUE)
		return;
	lpDDSBack->Unlock(NULL);
	Screen.pScreen = NULL;

}//end InterruptFrame

void tagAPI_DDraw::PrepFrame()
{
	if(bNotInitializedFlag == TRUE)
	{	Screen.pScreen	= NULL;	bNotInitializedFlag = 1; }

	//***Lets get a pointer to our directDraw backbuffer (so we can mess with it!)
	DDSURFACEDESC		ddsdBack;
	
	ZeroMemory(&ddsdBack, sizeof (DDSURFACEDESC) );
	ddsdBack.dwSize = sizeof(DDSURFACEDESC);
	
	int ret;
	do{
		ret = lpDDSBack->Lock(NULL, &ddsdBack, DDLOCK_SURFACEMEMORYPTR, NULL);
	}while( ret != DD_OK);

	//lpDDSBack->Unlock(NULL);
	lpDDSBack->Unlock(ddsdBack.lpSurface);
	
	if( FAILED(ret) ) 
	{	Screen.pScreen	= NULL;	bNotInitializedFlag = 1; }

	//*******SET OUR SCREEN STRUCT************
	Screen.iWidth		= ddsdBack.dwWidth;
	Screen.iHeight		= ddsdBack.dwHeight;
	Screen.iRealWidth	= (ddsdBack.lPitch>>1);
	Screen.pScreen		= (unsigned short *)ddsdBack.lpSurface;
	bNotInitializedFlag = 0;

}//end PrepFrame

int tagAPI_DDraw::DetermineGreenBitFormat( DWORD dwRBitMask)
{
	//add the return values to the amount shifted in the Red transparency
	if((dwRBitMask >> 10) & 1)
		return 0;
	else
		return 1;
}

int tagAPI_DDraw::InitRGB16()
{
	//***This code will obtain the correct RGB masks for each computer
	DDPIXELFORMAT		ddpf;
	int ret;
	
	ZeroMemory(&ddpf, sizeof(DDPIXELFORMAT));
	ddpf.dwSize = sizeof(DDPIXELFORMAT);
	
	ret = lpDDSBack->GetPixelFormat(&ddpf);
	
	if(ret)
	{
		switch (ret)
		{
			case DDERR_INVALIDOBJECT:
				MessageBox(Hwnd,
					"GetPixelFormat: Invalid ddOBJECT ", "oh oh", MB_OK);
				break;

			case DDERR_INVALIDPARAMS:
				MessageBox(Hwnd,
					"GetPixelFormat: Invalid PARAMS ", "oh oh", MB_OK);
				break;

			case DDERR_INVALIDSURFACETYPE:
				MessageBox(Hwnd,
					"GetPixelFormat: Invalid SURFACETYPE ", "oh oh", MB_OK);
				break;

			default:
				MessageBox(Hwnd,
					"GetPixelFormat: ??? error ", "oh oh", MB_OK);
				break;

		}
		Screen.pScreen		= NULL;
		bNotInitializedFlag	= 1;
		return 1;
	}

	/*
	if(ddpf.dwRGBBitCount != 16)
	{
		Screen.pScreen		= NULL;
		bNotInitializedFlag	= 1;

		return 1; //must be in 16 bit mode
	}
	*/

	Screen.bGreen6Mode	= DetermineGreenBitFormat(ddpf.dwRBitMask);
	Screen.wRBitMask		= (WORD)ddpf.dwRBitMask;
	Screen.wGBitMask		= (WORD)ddpf.dwGBitMask;
	Screen.wBBitMask		= (WORD)ddpf.dwBBitMask;

	return 0;
}


int tagAPI_DDraw::ClearSurface(LPDIRECTDRAWSURFACE lpDDSurface, DWORD dwRGBvalue)
{
	DDBLTFX				ddbltfx;

	// clear the back buffer to black
	ddbltfx.dwSize		= sizeof( ddbltfx );
	ddbltfx.dwFillColor	= dwRGBvalue;	// Set the color to Black
//	ddbltfx.dwFillColor	= 0xFFFF;		// Set the color to white (just a test)
	
	RECT	clearRect	= {135, 80, 302, 401};

	if ( FAILED( lpDDSurface->Blt( &clearRect, NULL, NULL,
				DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx ) ) )
	{
		MessageBox(NULL, "Could not clear BackBuffer!", "Error:", MB_OK);
		return FALSE;
	}
	return TRUE;
}


void tagAPI_DDraw::DrawRect(unsigned int x1, unsigned int y1,
							unsigned int x2, unsigned int y2, COLORREF Color)
{
	HDC		dc;
	HBRUSH	hBrush;
	
    if ( FAILED(lpDDSBack->GetDC(&dc)) )
        return;
    else
    {
        // SetBkColor(dc, RGB(0, 0, 0));
		hBrush = CreateSolidBrush( Color);
		SelectObject(dc, hBrush);
		
		Rectangle(dc, x1, y1, x2, y2);
		
		DeleteObject(hBrush);
        lpDDSBack->ReleaseDC(dc);
    }

}

int tagAPI_DDraw::TextMessage(char * s, int x, int y)
{
    HDC dc;
    if ( FAILED(lpDDSBack->GetDC(&dc)) )
        return(TRUE);
    else
    {
        SetBkColor(dc, RGB(0, 0, 0));
        SetTextColor(dc, RGB(255, 255, 255));
        SetTextAlign(dc, TA_CENTER);
        TextOut(dc, x, y, s, lstrlen(s));

        lpDDSBack->ReleaseDC(dc);

    }

    return(FALSE);
}