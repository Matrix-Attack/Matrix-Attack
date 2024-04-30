#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <dplay.h>
#include <dplobby.h>
#include <dsound.h>
#include <crtdbg.h>
#include <fstream>

using namespace std;

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "wave.h"
#include "dsound.h"
#include "dxerror.h"
#include "utility.h"



extern APP			App;
extern GAME_DATA	GameData;


#include <dsound.h>
#include <stdio.h>
#include "wave.h"

WAVEFORMATEX                *pwfx;
HMMIO                       hmmio;
MMCKINFO                    mmckinfo, mmckinfoParent;
DWORD                       dwMidBuffer;


BOOL PlayBuffer(void);
BOOL SetupStreamBuffer(LPSTR);
BOOL FillBufferWithSilence( LPDIRECTSOUNDBUFFER lpDsb );
void  Cleanup(void);



DSound::DSound()
{
	bSoundEnabled = 0;
	lpDSStreamBuffer = NULL;
	lpDSStreamBuffer2 = NULL;

	lpDSSStaticCluster1.lpDsb[0] = NULL;
	lpDSSStaticCluster1.lpDsb[1] = NULL;
	lpDSSStaticCluster1.lpDsb[2] = NULL;
	lpDSSStaticCluster1.curIndex = 0;

	lpDSSStaticCluster2.lpDsb[0] = NULL;
	lpDSSStaticCluster2.lpDsb[1] = NULL;
	lpDSSStaticCluster2.lpDsb[2] = NULL;
	lpDSSStaticCluster2.curIndex = 0;

	lpDSSStaticCluster3.lpDsb[0] = NULL;
	lpDSSStaticCluster3.lpDsb[1] = NULL;
	lpDSSStaticCluster3.lpDsb[2] = NULL;
	lpDSSStaticCluster3.curIndex = 0;

	lpDSSStaticFlip.lpDsb[0] = NULL;
	lpDSSStaticFlip.lpDsb[1] = NULL;
	lpDSSStaticFlip.lpDsb[2] = NULL;
	lpDSSStaticFlip.curIndex = 0;

	lpDSSStaticLinker1.lpDsb[0] = NULL;
	lpDSSStaticLinker1.lpDsb[1] = NULL;
	lpDSSStaticLinker1.lpDsb[2] = NULL;
	lpDSSStaticLinker1.curIndex = 0;

	lpDSSStaticLinker2.lpDsb[0] = NULL;
	lpDSSStaticLinker2.lpDsb[1] = NULL;
	lpDSSStaticLinker2.lpDsb[2] = NULL;
	lpDSSStaticLinker2.curIndex = 0;

	lpDSSStaticLinker3.lpDsb[0] = NULL;
	lpDSSStaticLinker3.lpDsb[1] = NULL;
	lpDSSStaticLinker3.lpDsb[2] = NULL;
	lpDSSStaticLinker3.curIndex = 0;

	lpDSSStaticCombo1.lpDsb[0] = NULL;
	lpDSSStaticCombo1.lpDsb[1] = NULL;
	lpDSSStaticCombo1.lpDsb[2] = NULL;
	lpDSSStaticCombo1.curIndex = 0;

	lpDSSStaticCombo2.lpDsb[0] = NULL;
	lpDSSStaticCombo2.lpDsb[1] = NULL;
	lpDSSStaticCombo2.lpDsb[2] = NULL;
	lpDSSStaticCombo2.curIndex = 0;

	lpDSSStaticCombo3.lpDsb[0] = NULL;
	lpDSSStaticCombo3.lpDsb[1] = NULL;
	lpDSSStaticCombo3.lpDsb[2] = NULL;
	lpDSSStaticCombo3.curIndex = 0;

	lpDSSStaticSwitch.lpDsb[0] = NULL;
	lpDSSStaticSwitch.lpDsb[1] = NULL;
	lpDSSStaticSwitch.lpDsb[2] = NULL;
	lpDSSStaticSwitch.curIndex = 0;

	iCurStreamingBuffer = 0;
	Selection = 0;
}



void DSound::PlayStaticSound(int whichSound)
{
	MessageBox(NULL, "DO WE EVER GET HERE?", "I DONT THINK SO", MB_OK);

	switch(whichSound)
	{
		case SFX_SWITCH:
			PlayStaticSoundBuffer(&lpDSSStaticSwitch.lpDsb[lpDSSStaticSwitch.curIndex], SWITCH_SOUND);
			if(lpDSSStaticSwitch.curIndex < 2)
				lpDSSStaticSwitch.curIndex++;
			else
				lpDSSStaticSwitch.curIndex = 0;
			break;


		case SFX_FLIP:
			PlayStaticSoundBuffer(&lpDSSStaticFlip.lpDsb[lpDSSStaticFlip.curIndex], FLIP_SOUND);
			if(lpDSSStaticFlip.curIndex < 2)
				lpDSSStaticFlip.curIndex++;
			else
				lpDSSStaticFlip.curIndex = 0;
			break;

		case SFX_CLUSTER1:
			PlayStaticSoundBuffer(&lpDSSStaticCluster1.lpDsb[lpDSSStaticCluster1.curIndex], CLUSTER_SOUND1);
			if(lpDSSStaticCluster1.curIndex < 2)
				lpDSSStaticCluster1.curIndex++;
			else
				lpDSSStaticCluster1.curIndex = 0;
			break;

		case SFX_CLUSTER2:
			PlayStaticSoundBuffer(&lpDSSStaticCluster2.lpDsb[lpDSSStaticCluster2.curIndex], CLUSTER_SOUND2);
			if(lpDSSStaticCluster2.curIndex < 2)
				lpDSSStaticCluster2.curIndex++;
			else
				lpDSSStaticCluster2.curIndex = 0;
			break;

		case SFX_CLUSTER3:
			PlayStaticSoundBuffer(&lpDSSStaticCluster3.lpDsb[lpDSSStaticCluster3.curIndex], CLUSTER_SOUND3);
			if(lpDSSStaticCluster3.curIndex < 2)
				lpDSSStaticCluster3.curIndex++;
			else
				lpDSSStaticCluster3.curIndex = 0;
			break;

		case SFX_LINKER1:
			PlayStaticSoundBuffer(&lpDSSStaticLinker1.lpDsb[lpDSSStaticLinker1.curIndex], LINKER_SOUND1);
			if(lpDSSStaticLinker1.curIndex < 2)
				lpDSSStaticLinker1.curIndex++;
			else
				lpDSSStaticLinker1.curIndex = 0;
			break;

		case SFX_LINKER2:
			PlayStaticSoundBuffer(&lpDSSStaticLinker2.lpDsb[lpDSSStaticLinker2.curIndex], LINKER_SOUND2);
			if(lpDSSStaticLinker2.curIndex < 2)
				lpDSSStaticLinker2.curIndex++;
			else
				lpDSSStaticLinker2.curIndex = 0;
			break;

		case SFX_LINKER3:
			PlayStaticSoundBuffer(&lpDSSStaticLinker3.lpDsb[lpDSSStaticLinker3.curIndex], LINKER_SOUND3);
			if(lpDSSStaticLinker3.curIndex < 2)
				lpDSSStaticLinker3.curIndex++;
			else
				lpDSSStaticLinker3.curIndex = 0;
			break;

		case SFX_COMBO1:
			PlayStaticSoundBuffer(&lpDSSStaticCombo1.lpDsb[lpDSSStaticCombo1.curIndex], COMBO_SOUND1);
			if(lpDSSStaticCombo1.curIndex < 2)
				lpDSSStaticCombo1.curIndex++;
			else
				lpDSSStaticCombo1.curIndex = 0;
			break;

		case SFX_COMBO2:
			PlayStaticSoundBuffer(&lpDSSStaticCombo2.lpDsb[lpDSSStaticCombo2.curIndex], COMBO_SOUND2);
			if(lpDSSStaticCombo2.curIndex < 2)
				lpDSSStaticCombo2.curIndex++;
			else
				lpDSSStaticCombo2.curIndex = 0;
			break;

		case SFX_COMBO3:
			PlayStaticSoundBuffer(&lpDSSStaticCombo3.lpDsb[lpDSSStaticCombo3.curIndex], COMBO_SOUND3);
			if(lpDSSStaticCombo3.curIndex < 2)
				lpDSSStaticCombo3.curIndex++;
			else
				lpDSSStaticCombo3.curIndex = 0;
			break;

	}
}

void DSound::UnloadSoundBuffers()
{
	int i;

	if(GameData.GameStateFlags & GAME_STATE_MENU)
		;
	else if(GameData.GameStateFlags & GAME_STATE_GAME)
	{
		if(lpDSStreamBuffer)
			lpDSStreamBuffer->Stop(); 

		if(this->lpDSStreamBuffer2)
			lpDSStreamBuffer2->Stop(); 
	
	    // Close any open file and free the buffer.
		WaveCloseReadFile( &hmmio, &pwfx );

		if ( lpDSStreamBuffer != NULL )
		{
		   lpDSStreamBuffer->Release();
		   lpDSStreamBuffer = NULL;
		}

		if ( lpDSStreamBuffer2 != NULL )
		{
		   lpDSStreamBuffer2->Release();
		   lpDSStreamBuffer2 = NULL;
		}

		iStreamState = STREAM_NOT_PLAYING;
	

		// free all the static and duplicate buffers
		for(i = 2; i >= 0; i--)
		{
			if(lpDSSStaticCluster1.lpDsb[i])
			{
				lpDSSStaticCluster1.lpDsb[i]->Release();
				lpDSSStaticCluster1.lpDsb[i] = NULL;
				lpDSSStaticCluster1.curIndex = 0;
			}

			if(lpDSSStaticCluster2.lpDsb[i])
			{
				lpDSSStaticCluster2.lpDsb[i]->Release();
				lpDSSStaticCluster2.lpDsb[i] = NULL;
				lpDSSStaticCluster2.curIndex = 0;
			}

			if(lpDSSStaticCluster3.lpDsb[i])
			{
				lpDSSStaticCluster3.lpDsb[i]->Release();
				lpDSSStaticCluster3.lpDsb[i] = NULL;
				lpDSSStaticCluster3.curIndex = 0;
			}

			if(lpDSSStaticFlip.lpDsb[i])
			{
				lpDSSStaticFlip.lpDsb[i]->Release();
				lpDSSStaticFlip.lpDsb[i] = NULL;
				lpDSSStaticFlip.curIndex = 0;
			}

			if(lpDSSStaticLinker1.lpDsb[i])
			{
				lpDSSStaticLinker1.lpDsb[i]->Release();
				lpDSSStaticLinker1.lpDsb[i] = NULL;
				lpDSSStaticLinker1.curIndex = 0;
			}

			if(lpDSSStaticLinker2.lpDsb[i])
			{
				lpDSSStaticLinker2.lpDsb[i]->Release();
				lpDSSStaticLinker2.lpDsb[i] = NULL;
				lpDSSStaticLinker2.curIndex = 0;
			}

			if(lpDSSStaticLinker3.lpDsb[i])
			{
				lpDSSStaticLinker3.lpDsb[i]->Release();
				lpDSSStaticLinker3.lpDsb[i] = NULL;
				lpDSSStaticLinker3.curIndex = 0;
			}

			if(lpDSSStaticCombo1.lpDsb[i])
			{
				lpDSSStaticCombo1.lpDsb[i]->Release();
				lpDSSStaticCombo1.lpDsb[i] = NULL;
				lpDSSStaticCombo1.curIndex = 0;
			}

			if(lpDSSStaticCombo2.lpDsb[i])
			{
				lpDSSStaticCombo2.lpDsb[i]->Release();
				lpDSSStaticCombo2.lpDsb[i] = NULL;
				lpDSSStaticCombo2.curIndex = 0;
			}

			if(lpDSSStaticCombo3.lpDsb[i])
			{
				lpDSSStaticCombo3.lpDsb[i]->Release();
				lpDSSStaticCombo3.lpDsb[i] = NULL;
				lpDSSStaticCombo3.curIndex = 0;
			}

			if(lpDSSStaticSwitch.lpDsb[i])
			{
				lpDSSStaticSwitch.lpDsb[i]->Release();
				lpDSSStaticSwitch.lpDsb[i] = NULL;
				lpDSSStaticSwitch.curIndex = 0;
			}
		}	// end for

	}

}


void DSound::SetCurrentStreamBuffer(int whichBuffer)
{
	if(iCurStreamingBuffer && iStreamState == STREAM_PLAYING_FULL)
	{
		iStreamState = STREAM_FADING;
	}
	else if(iStreamState == STREAM_NOT_PLAYING)
	{
		iCurStreamingBuffer = whichBuffer;
		switch(whichBuffer)
		{
			case CUR_SELECTED_TRACK_1:
				SetupStreamBuffer(TRACK_1);
				break;

			case CUR_SELECTED_TRACK_2:
				SetupStreamBuffer(TRACK_2);
				break;
		}
		if(whichBuffer)
			iStreamState = STREAM_PLAYING_FULL;
	}
}


void DSound::LoadSoundBuffers()
{
//	if(GameData.GameStateFlags & GAME_STATE_MENU)
//		;
//	else if(GameData.GameStateFlags & GAME_STATE_GAME)
	{
		if(!LoadStaticSoundBuffer(&lpDSSStaticSwitch.lpDsb[0], SWITCH_SOUND) )
		{
			MessageBox(hwnd, "Error setting up static sound for switch", "ERROR", MB_OK);
		}
		lpDSSStaticSwitch.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticSwitch.lpDsb[0], &lpDSSStaticSwitch.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticSwitch.lpDsb[0], &lpDSSStaticSwitch.lpDsb[2]);


		if(!LoadStaticSoundBuffer(&lpDSSStaticCluster1.lpDsb[0], CLUSTER_SOUND1) )
		{
			MessageBox(hwnd, "Error setting up static sound for cluster", "ERROR", MB_OK);
		}
		lpDSSStaticCluster1.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster1.lpDsb[0], &lpDSSStaticCluster1.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster1.lpDsb[0], &lpDSSStaticCluster1.lpDsb[2]);


		if(!LoadStaticSoundBuffer(&lpDSSStaticCluster2.lpDsb[0], CLUSTER_SOUND2) )
		{
			MessageBox(hwnd, "Error setting up static sound for cluster", "ERROR", MB_OK);
		}
		lpDSSStaticCluster2.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster2.lpDsb[0], &lpDSSStaticCluster2.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster2.lpDsb[0], &lpDSSStaticCluster2.lpDsb[2]);


		if(!LoadStaticSoundBuffer(&lpDSSStaticCluster3.lpDsb[0], CLUSTER_SOUND3) )
		{
			MessageBox(hwnd, "Error setting up static sound for cluster", "ERROR", MB_OK);
		}
		lpDSSStaticCluster3.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster3.lpDsb[0], &lpDSSStaticCluster3.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCluster3.lpDsb[0], &lpDSSStaticCluster3.lpDsb[2]);



		if(!LoadStaticSoundBuffer(&lpDSSStaticLinker1.lpDsb[0], LINKER_SOUND1) )
		{
			MessageBox(hwnd, "Error setting up static sound for linker", "ERROR", MB_OK);
		}
		lpDSSStaticLinker1.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker1.lpDsb[0], &lpDSSStaticLinker1.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker1.lpDsb[0], &lpDSSStaticLinker1.lpDsb[2]);

		if(!LoadStaticSoundBuffer(&lpDSSStaticLinker2.lpDsb[0], LINKER_SOUND2) )
		{
			MessageBox(hwnd, "Error setting up static sound for linker", "ERROR", MB_OK);
		}
		lpDSSStaticLinker2.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker2.lpDsb[0], &lpDSSStaticLinker2.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker2.lpDsb[0], &lpDSSStaticLinker2.lpDsb[2]);

		if(!LoadStaticSoundBuffer(&lpDSSStaticLinker3.lpDsb[0], LINKER_SOUND3) )
		{
			MessageBox(hwnd, "Error setting up static sound for linker", "ERROR", MB_OK);
		}
		lpDSSStaticLinker3.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker3.lpDsb[0], &lpDSSStaticLinker3.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticLinker3.lpDsb[0], &lpDSSStaticLinker3.lpDsb[2]);


		if(!LoadStaticSoundBuffer(&lpDSSStaticCombo1.lpDsb[0], COMBO_SOUND1) )
		{
			MessageBox(hwnd, "Error setting up static sound for combo", "ERROR", MB_OK);
		}
		lpDSSStaticCombo1.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo1.lpDsb[0], &lpDSSStaticCombo1.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo1.lpDsb[0], &lpDSSStaticCombo1.lpDsb[2]);

		if(!LoadStaticSoundBuffer(&lpDSSStaticCombo2.lpDsb[0], COMBO_SOUND2) )
		{
			MessageBox(hwnd, "Error setting up static sound for combo", "ERROR", MB_OK);
		}
		lpDSSStaticCombo2.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo2.lpDsb[0], &lpDSSStaticCombo2.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo2.lpDsb[0], &lpDSSStaticCombo2.lpDsb[2]);

		if(!LoadStaticSoundBuffer(&lpDSSStaticCombo3.lpDsb[0], COMBO_SOUND3) )
		{
			MessageBox(hwnd, "Error setting up static sound for combo", "ERROR", MB_OK);
		}
		lpDSSStaticCombo3.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo3.lpDsb[0], &lpDSSStaticCombo3.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticCombo3.lpDsb[0], &lpDSSStaticCombo3.lpDsb[2]);


		if(!LoadStaticSoundBuffer(&lpDSSStaticFlip.lpDsb[0], FLIP_SOUND) )
		{
			MessageBox(hwnd, "Error setting up static sound for flip", "ERROR", MB_OK);
		}
		lpDSSStaticFlip.curIndex = 0;
		DuplicateStaticSoundBuffer(&lpDSSStaticFlip.lpDsb[0], &lpDSSStaticFlip.lpDsb[1]);
		DuplicateStaticSoundBuffer(&lpDSSStaticFlip.lpDsb[0], &lpDSSStaticFlip.lpDsb[2]);

//		if(!LoadStreamingSoundBuffer("track1.wav") )
//			MessageBox(hwnd, "Error setting up streaming sound", "ERROR", MB_OK);
	}
}



/* --------------------------------------------------------

   FillBufferWithSilence()
   Write silence to entire buffer

   -------------------------------------------------------- */

BOOL DSound::FillBufferWithSilence2( LPDIRECTSOUNDBUFFER lpDsb )
{
    WAVEFORMATEX    wfx;
    DWORD           dwSizeWritten;

    PBYTE   pb1;
    DWORD   cb1;

    if ( FAILED( lpDsb->GetFormat( &wfx, sizeof( WAVEFORMATEX ), &dwSizeWritten ) ) )
        return FALSE;

    if ( SUCCEEDED( lpDsb->Lock( 0, wfx.nAvgBytesPerSec * 200, 
                         ( LPVOID * )&pb1, &cb1, 
                         NULL, NULL, 0 ) ) )
    {
        FillMemory( pb1, cb1, ( wfx.wBitsPerSample == 8 ) ? 128 : 0 );

        lpDsb->Unlock( pb1, cb1, NULL, 0 );
        return TRUE;
    }

    return FALSE;
}  // FillBufferWithSilence


/* --------------------------------------------------------

   SetupStreamBuffer()
   Create a streaming buffer in same format as wave data

   -------------------------------------------------------- */

BOOL DSound::SetupStreamBuffer( LPSTR lpzFileName )
{
    DSBUFFERDESC    dsbdesc;
    HRESULT         hr;

    // Close any open file and free the buffer.
    WaveCloseReadFile( &hmmio, &pwfx );
    if ( lpDSStreamBuffer != NULL )
    {
        lpDSStreamBuffer->Release();
        lpDSStreamBuffer = NULL;
    }

    // Open the file, get wave format, and descend to the data chunk.
    if ( WaveOpenFile( lpzFileName, &hmmio, &pwfx, &mmckinfoParent ) != 0 )
        return FALSE;
    if ( WaveStartDataRead( &hmmio, &mmckinfo, &mmckinfoParent ) != 0 )
        return FALSE;

    // Create secondary buffer able to hold 2 seconds of data.
    memset( &dsbdesc, 0, sizeof( DSBUFFERDESC ) ); 
    dsbdesc.dwSize = sizeof( DSBUFFERDESC ); 
    dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 
                    | DSBCAPS_GLOBALFOCUS
                    | DSBCAPS_CTRLVOLUME;
    dsbdesc.dwBufferBytes = pwfx->nAvgBytesPerSec * 2;  
    dsbdesc.lpwfxFormat = pwfx; 
 
    if ( FAILED( hr = lpds->CreateSoundBuffer( &dsbdesc, &lpDSStreamBuffer, NULL ) ) )
    {
        WaveCloseReadFile( &hmmio, &pwfx );
        return FALSE; 
    }

    FillBufferWithSilence( lpDSStreamBuffer );
    hr = lpDSStreamBuffer->Play( 0, 0, DSBPLAY_LOOPING );

    dwMidBuffer = dsbdesc.dwBufferBytes / 2;

    return TRUE;
} // SetupStreamBuffer




/* --------------------------------------------------------

   PlayBuffer()
   Stream data to the buffer every time the current play position
   has covered half the distance.

   -------------------------------------------------------- */
BOOL DSound::PlayBuffer( void )
{
    HRESULT         hr;
    DWORD           dwPlay;
	DWORD			dwWrite;
    DWORD           dwStartOfs;
    static DWORD    dwLastPlayPos;
    VOID            *lpvData;
    DWORD           dwBytesLocked;
    UINT            cbBytesRead;

    if ( lpDSStreamBuffer == NULL ) return FALSE;

    if ( FAILED( lpDSStreamBuffer->GetCurrentPosition( &dwPlay, &dwWrite ) ) ) 
		return FALSE;

    // If the play cursor has just reached the first or second half of the
    // buffer, it's time to stream data to the other half.
    if ( ( ( dwPlay >= dwMidBuffer ) && ( dwLastPlayPos < dwMidBuffer ) )
        || ( dwPlay < dwLastPlayPos ) )
    {
        dwStartOfs = ( dwPlay >= dwMidBuffer ) ? 0 : dwMidBuffer;

        hr = lpDSStreamBuffer->Lock( dwStartOfs, // Offset of lock start.
                    dwMidBuffer,      // Number of bytes to lock.
                    &lpvData,         // Address of lock start.
                    &dwBytesLocked,   // Number of bytes locked.
                    NULL,             // Address of wraparound lock.
                    NULL,             // Number of wraparound bytes.
                    0 );              // Flags.
  
        WaveReadFile( hmmio,             // File handle.
                      dwBytesLocked,     // Number of bytes to read.
                      ( BYTE * )lpvData, // Destination.
                      &mmckinfo,         // File chunk info.
                      &cbBytesRead );    // Number of bytes read.

        if ( cbBytesRead < dwBytesLocked )  // Reached end of file.
        {
            if ( WaveStartDataRead( &hmmio, &mmckinfo, &mmckinfoParent ) 
				 != 0 )
            {
                OutputDebugString( "Can't reset file.\n" );
            }
            else
            {
                WaveReadFile( hmmio,          
                              dwBytesLocked - cbBytesRead,
                              ( BYTE * )lpvData + cbBytesRead, 
                              &mmckinfo,      
                              &cbBytesRead );    
            }
        }

        lpDSStreamBuffer->Unlock( lpvData, dwBytesLocked, NULL, 0 );
    }

    dwLastPlayPos = dwPlay;
    return TRUE;
} // PlayBuffer





/* --------------------------------------------------------

   Cleanup()
   Cleans up DirectSound objects and closes any open wave file

   -------------------------------------------------------- */

void DSound::Cleanup( void )
{
    WaveCloseReadFile( &hmmio, &pwfx );
    if ( lpds ) lpds->Release();  // This releases buffers as well.
}




//***********************************************************************
// Function: DSound::DSound
//
// Purpose:  Initializes the sound class
//			 
//
// Parameters: HWND Hwnd - the window application handle
//										 
//			  
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
void DSound::SoundInit(HWND Hwnd)
{
	hwnd = Hwnd;
	bPlayStream = TRUE;
	bPaused = FALSE;
	iStreamState = STREAM_NOT_PLAYING;

	InitDSDeviceHead();

	// Initialize DirectSound with the primary sound device
	if((InitDSound(NULL)))
		bSoundEnabled = 1;
	else
		bSoundEnabled = 0;
}




//***********************************************************************
// Function: DSound::~DSound
//
// Purpose:  Performs cleanup for the sound class
//			 
//
// Parameters: None
//										 
//			  
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
DSound::~DSound()
{
	int i;

	DestroyDSDeviceList();

    // Close any open file and free the buffer.
    WaveCloseReadFile( &hmmio, &pwfx );

	if(lpDSStreamBuffer)
	{
		lpDSStreamBuffer->Release();
		lpDSStreamBuffer = NULL;
	}

	// free all the static and duplicate buffers
	for(i = 2; i >= 0; i--)
	{
		if(lpDSSStaticCluster1.lpDsb[i])
		{
			lpDSSStaticCluster1.lpDsb[i]->Release();
			lpDSSStaticCluster1.lpDsb[i] = NULL;
			lpDSSStaticCluster1.curIndex = 0;
		}

		if(lpDSSStaticCluster2.lpDsb[i])
		{
			lpDSSStaticCluster2.lpDsb[i]->Release();
			lpDSSStaticCluster2.lpDsb[i] = NULL;
			lpDSSStaticCluster2.curIndex = 0;
		}

		if(lpDSSStaticCluster3.lpDsb[i])
		{
			lpDSSStaticCluster3.lpDsb[i]->Release();
			lpDSSStaticCluster3.lpDsb[i] = NULL;
			lpDSSStaticCluster3.curIndex = 0;
		}

		if(lpDSSStaticFlip.lpDsb[i])
		{
			lpDSSStaticFlip.lpDsb[i]->Release();
			lpDSSStaticFlip.lpDsb[i] = NULL;
			lpDSSStaticFlip.curIndex = 0;
		}

		if(lpDSSStaticLinker1.lpDsb[i])
		{
			lpDSSStaticLinker1.lpDsb[i]->Release();
			lpDSSStaticLinker1.lpDsb[i] = NULL;
			lpDSSStaticLinker1.curIndex = 0;
		}

		if(lpDSSStaticLinker2.lpDsb[i])
		{
			lpDSSStaticLinker2.lpDsb[i]->Release();
			lpDSSStaticLinker2.lpDsb[i] = NULL;
			lpDSSStaticLinker2.curIndex = 0;
		}

		if(lpDSSStaticLinker3.lpDsb[i])
		{
			lpDSSStaticLinker3.lpDsb[i]->Release();
			lpDSSStaticLinker3.lpDsb[i] = NULL;
			lpDSSStaticLinker3.curIndex = 0;
		}

		if(lpDSSStaticCombo1.lpDsb[i])
		{
			lpDSSStaticCombo1.lpDsb[i]->Release();
			lpDSSStaticCombo1.lpDsb[i] = NULL;
			lpDSSStaticCombo1.curIndex = 0;
		}

		if(lpDSSStaticCombo2.lpDsb[i])
		{
			lpDSSStaticCombo2.lpDsb[i]->Release();
			lpDSSStaticCombo2.lpDsb[i] = NULL;
			lpDSSStaticCombo2.curIndex = 0;
		}

		if(lpDSSStaticCombo3.lpDsb[i])
		{
			lpDSSStaticCombo3.lpDsb[i]->Release();
			lpDSSStaticCombo3.lpDsb[i] = NULL;
			lpDSSStaticCombo3.curIndex = 0;
		}

		if(lpDSSStaticSwitch.lpDsb[i])
		{
			lpDSSStaticSwitch.lpDsb[i]->Release();
			lpDSSStaticSwitch.lpDsb[i] = NULL;
			lpDSSStaticSwitch.curIndex = 0;
		}
	}	// end for

	if(lpds)
	{
		lpds->Release();
		lpds = NULL;
	}
}


//***********************************************************************
// Function: InitDSDeviceHead
//
// Purpose:  Initializes the head of the enumerated device list
//			 
//
// Parameters: None
//										
//			  
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
void DSound::InitDSDeviceHead()
{
	SoundDevicesHead.iNumberDesc = 0;
	SoundDevicesHead.iChosenDesc = 0;
	SoundDevicesHead.pStart = (P_SOUNDDESC)calloc(1, sizeof(SOUNDDESC) );
	strcpy(SoundDevicesHead.pStart->cName, "Primary Sound Driver");
}	/* end InitDSDeviceHead */




//***********************************************************************
// Function: MyDSoundEnumCallback
//
// Purpose:  Sound device enumeration callback function.
//			 
//
// Parameters: LPGUID lpGuid - pointer to a global unique indentifier
//			   LPCSTR lpstrDescription - the name of the device
//			   LPCSTR lpstrModule - the friendly or module name
//			   LPVOID lpContext - 32 bit value passed in and typecast for
//								  whatever is needed  
//			  
//
// Returns: BOOL - TRUE if successful, false otherwise
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL CALLBACK MyDSoundEnumCallback(LPGUID lpGuid, LPCSTR lpstrDescription,
								   LPCSTR lpstrModule, LPVOID lpContext)
{
	P_DSDESC_HEAD pDSHead = (P_DSDESC_HEAD)lpContext;
	P_SOUNDDESC pDevDesc;
	P_SOUNDDESC pDSTemp = pDSHead->pStart;
	P_SOUNDDESC pPrev = pDSTemp;
	//	char		szMsg[30];
	
	if(lpGuid != NULL)
	{
		pDevDesc = (P_SOUNDDESC)calloc(1, sizeof(SOUNDDESC) );
		
		if(pDevDesc == NULL)
			return FALSE;
		
		pDevDesc->lpGuid = (LPGUID)malloc(sizeof(GUID) );
		memcpy(pDevDesc->lpGuid, lpGuid, sizeof(GUID) );
		memcpy(pDevDesc->cName, lpstrDescription, 50);
		
		
		//		if((*ppDSHead)->iNumberDesc > 1)
		//		{
		while(pDSTemp->pNext) 
		{
			pPrev = pDSTemp;
			pDSTemp = pDSTemp->pNext;
		}
		
		pDevDesc->pPrev = pPrev;
		pDSTemp->pNext = pDevDesc;
		//		}
		//		else
		//		{
		//			memcpy(pDSTemp, pDevDesc, sizeof(SOUNDDESC) );
		//			free(pDevDesc);
		//		}
		//		wsprintf(szMsg, "malloc %d", (*ppDSHead)->iNumberDesc);
		//		MessageBox( (*ppDSHead)->hwnd, szMsg, "AA", MB_OK);
	}
	pDSHead->iNumberDesc++;
	
	return TRUE;
}	/* end MyDSoundEnumCallback */



//***********************************************************************
// Function: GrabSoundGuid
//
// Purpose:  Stores the guid of the chosen sound device
//			 		  
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Doug Quinn, Max Szlagor    
//
//***********************************************************************
void DSound::GrabSoundGuid()
{
	UINT i;
	P_SOUNDDESC pSoundNode = SoundDevicesHead.pStart;
	
	
	
	if(bSoundEnabled == FALSE)
		return;
	
	for( i = 0; i < SoundDevicesHead.iChosenDesc; i++)
		pSoundNode = pSoundNode->pNext;
	
	if(pSoundNode->lpGuid != NULL)
		lpSoundGuid = (LPGUID)calloc(1, sizeof(GUID));
	else
		lpSoundGuid = NULL;
	
	if(lpSoundGuid != NULL)
		memcpy(lpSoundGuid, pSoundNode->lpGuid, sizeof(GUID));
	strncpy(szSoundDevice, pSoundNode->cName, 120);
}




//***********************************************************************
// Function: DestroyDSDeviceList
//
// Purpose:  Destroys the list of enumerated devices
//			 
//
// Parameters: PP_DSDESC_HEAD ppDSHead - Pointer to pointer to head of
//										 sound device list
//
//			   HWND hwnd -				 Window handle 
//			  
//
// Returns: void
//
// Last Modified:  Date 11/15/99     Author - Max Szlagor     
//
//***********************************************************************
void DSound::DestroyDSDeviceList()
{
	P_SOUNDDESC pCurDesc = SoundDevicesHead.pStart;
	P_SOUNDDESC pTemp;
	int count = 0;
	
	while(pCurDesc)
	{
		pTemp = pCurDesc;
		pCurDesc = pCurDesc->pNext;
		free(pTemp->lpGuid);
		free(pTemp);
		count++;
	}
	
	SoundDevicesHead.pStart = NULL;

}	/* end DestroyDSDeviceList */




//***********************************************************************
// Function:  DSound::InitDSound
//
// Purpose:  Initializes the direct sound object and the primary buffer
//			 
//
// Parameters: None
//			  
//
// Returns: BOOL - TRUE if successful, FALSE otherwise
//
// Last Modified:  Date 11/15/99     Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::InitDSound(LPGUID *lpGuid)
{
	DSBUFFERDESC		dsbdesc;
	WAVEFORMATEX		wfm;
	
	if(lpGuid == NULL)
	{
		hRet = DirectSoundCreate(NULL, &lpds, NULL );
		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			return FALSE;
		}
	}
	else
	{
		// Create the Direct Sound object
		hRet = DirectSoundCreate(*lpGuid, &lpds, NULL );
		if(FAILED(hRet) )
		{	
			if(hRet == DSERR_ALLOCATED)
			{
				MessageBox(hwnd, 
					"You are currently using the sound card for somthing else. If you wish to have sound, please disable the application using the sound card and restart the game.",
					"Oh oh, Sound card is in use already!", MB_OK);
				return FALSE;
			}
			DisplayDSoundError(hwnd, hRet);
			return FALSE;
		}
	}
	
	// Set the cooperative level
	hRet = lpds->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Coop level in InitDSound", "ERROR", MB_OK);
		DisplayDSoundError(hwnd, hRet);
		return FALSE;
	}
	
	
	// Set up the buffer description for the primary buffer
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC) );
	dsbdesc.dwSize = sizeof(DSBUFFERDESC);
	dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsbdesc.dwBufferBytes = 0;
	dsbdesc.lpwfxFormat = NULL;
	
	
	// Set up the waveformat struct for optimal sound playback
	memset(&wfm, 0, sizeof(WAVEFORMATEX) );
	wfm.wFormatTag = WAVE_FORMAT_PCM;
	wfm.nChannels = 2;
	wfm.nSamplesPerSec = 44100;
	wfm.wBitsPerSample = 16;
	wfm.nBlockAlign = wfm.wBitsPerSample / 8 * wfm.nChannels;
	wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nBlockAlign;
	
	// Gain access to the primary buffer and try to set the desired format.
	// If we cannot customize this format we will go with the defaults.
	hRet = lpds->CreateSoundBuffer(&dsbdesc, &lpDsbPrimary, NULL);
	if(SUCCEEDED(hRet) )
	{
		hRet = lpDsbPrimary->SetFormat(&wfm);
	}
	else
	{
		MessageBox(hwnd, "Create Primary Sound buffer in InitDSound", "ERROR", MB_OK);
		DisplayDSoundError(hwnd, hRet);
		return FALSE;
	}
	
	//	pApp->lpDsbPrimary->Play(0, 0, DSBPLAY_LOOPING);
	
	return TRUE;
}	/* end InitDSound */




//***********************************************************************
// Function: DSound::LoadStaticSoundBuffer
//
// Purpose:  Loads a static sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to sound
//			   buffer to be loaded
//
//			   char *szFile - The name of the wav file to load into the 
//			   the sound buffer		  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::LoadStaticSoundBuffer(LPDIRECTSOUNDBUFFER *lpDsb, char *szFile)
{
	WAVEFORMATEX	*pWfx;			// Wave format info
	HMMIO			hmmio;			// File handle
	MMCKINFO		mmckinfo;		// Chunk info
	MMCKINFO		mmckinfoParent; // Parent chunk info
	DSBUFFERDESC    dsbdesc;		// Buffer description
	LPVOID			lpvAudio1;		// Address of lock start
	DWORD			dwBytes1;		// Number of bytes locked
	UINT			cbBytesRead;	// Number of bytes read from wave file
	
	// Attempt to open and read the wave file
	if(WaveOpenFile(szFile, &hmmio, &pWfx, &mmckinfoParent) != 0)
	{
		MessageBox(hwnd, "WaveOpenFile1 in LoadSoundBuffer", "ERROR", MB_OK);
		return FALSE;
	}
	
	if(WaveStartDataRead(&hmmio, &mmckinfo, &mmckinfoParent) != 0)
	{
		MessageBox(hwnd, "WaveStartDataRead in LoadSoundBuffer", "ERROR", MB_OK);
		return FALSE;
	}
	
	// If the sound buffer pointer is still null, we need to initialize it
	if(*lpDsb == NULL)
	{
		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC) );
		dsbdesc.dwSize			= sizeof(DSBUFFERDESC);
		dsbdesc.dwFlags			= DSBCAPS_STATIC ;//| DSBCAPS_LOCSOFTWARE;
		dsbdesc.dwBufferBytes	= mmckinfo.cksize;
		dsbdesc.lpwfxFormat		= pWfx;
		
		hRet = lpds->CreateSoundBuffer(&dsbdesc,
			lpDsb,
			NULL);

		if(FAILED(hRet) )
		{
			MessageBox(hwnd, "CreateSound Buffer in LoadSoundBuffer", "ERROR", MB_OK);
			WaveCloseReadFile(&hmmio, &pWfx);
			DisplayDSoundError(hwnd, hRet);
			return FALSE;
		}
	}
	
	// Now we lock the buffer and descend through the wave file
	hRet = (*lpDsb)->Lock(0,					// Address of lock start
					   dsbdesc.dwBufferBytes,	// Size of lock, ignored for static
					   &lpvAudio1,				// Address of lock start
					   &dwBytes1,				// Number of bytes locked
					   NULL,					// Address of wraparound start
					   NULL,		  			// Number of wraparound bytes
					   0);	// Locking flag
	
	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "lock in LoadSoundBuffer", "ERROR", MB_OK);
		MessageBox(hwnd, szFile, "NAME OF FILE", MB_OK);
		DisplayDSoundError(hwnd, hRet);
		WaveCloseReadFile( &hmmio, &pWfx );
		return FALSE;
	}
	
	if(WaveReadFile(hmmio,	// File handle
		dwBytes1,			// Number of bytes to read
		(BYTE*)lpvAudio1,	// Destination
		&mmckinfo,			// File chunk info
		&cbBytesRead) )		// Actual number of bytes read
	{
		MessageBox(hwnd, "Error reading wave file", "WAVE ERROR", MB_OK);
		
		hRet = (*lpDsb)->Unlock(lpvAudio1, dwBytes1, NULL, 0);
		if(FAILED(hRet) )
		{
			MessageBox(hwnd, "Unlock in LoadSoundBuffer", "ERROR", MB_OK);
			DisplayDSoundError(hwnd, hRet);
			WaveCloseReadFile( &hmmio, &pWfx );
			return FALSE;
		}
		return FALSE;
	}
	
	// Now unlock the sound buffer
	hRet = (*lpDsb)->Unlock(lpvAudio1, dwBytes1, NULL, 0);
	
	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Unlock 2 in LoadSoundBuffer", "ERROR", MB_OK);
		DisplayDSoundError(hwnd, hRet);
		return FALSE;
	}
	
	// Close the wave file
	WaveCloseReadFile( &hmmio, &pWfx );
	
	return TRUE;
}




//***********************************************************************
// Function: DSound::FillBufferWithSilence
//
// Purpose:  Loads a streaming sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to sound
//			   buffer to be loaded
//	  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::FillBufferWithSilence(LPDIRECTSOUNDBUFFER lpDsb)
{
    WAVEFORMATEX    wfx;
    DWORD           dwSizeWritten;

    PBYTE   pb1;
    DWORD   cb1;
	DWORD   bytes;

//	char a[100];

    hRet = lpDsb->GetFormat( &wfx, sizeof( WAVEFORMATEX ), &dwSizeWritten );

	if (FAILED(hRet) )
	{
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error getting the format of a sound buffer", "FillBufferWithSilence", MB_OK);
		LogErrorToFile("Error getting the format of a sound buffer", "FillBufferWithSilence");
        return FALSE;
	}

	bytes = wfx.nAvgBytesPerSec * 200;

    hRet = ( lpDsb->Lock( 0, 1, 
                         ( LPVOID * )&pb1, &cb1, 
                         NULL, NULL,  0) );

	if(FAILED(hRet) )
	{
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error locking sound buffer", "FillBufferWithSilence", MB_OK);
		LogErrorToFile("Error locking sound buffer", "FillBufferWithSilence");
        return FALSE;
	}

    FillMemory( pb1, cb1, ( wfx.wBitsPerSample == 8 ) ? 128 : 0 );

    hRet = lpDsb->Unlock( pb1, cb1, NULL, 0 );

	if(FAILED(hRet) )
	{
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error unlocking sound buffer", "FillBufferWithSilence", MB_OK);
		LogErrorToFile("Error unlocking sound buffer", "FillBufferWithSilence");
        return FALSE;
	}

	return TRUE;
}




//***********************************************************************
// Function: DSound::LoadStreamingSoundBuffer
//
// Purpose:  Loads a streaming sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to sound
//			   buffer to be loaded
//
//			   char *szFile - The name of the wav file to load into the 
//			   the sound buffer		  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::LoadStreamingSoundBuffer(char *szFileName)
{
	WAVEFORMATEX *pwfx;
	DSBUFFERDESC dsbdesc;

	// Close any open wave file and free up the sound buffer
	WaveCloseReadFile(&streamWaveDesc.hmmio, &pwfx);

	if(lpDSStreamBuffer != NULL)
	{
		lpDSStreamBuffer->Release();
		lpDSStreamBuffer = NULL;
	}

	// Open wave file, get the format, descend into data chunk
	if(WaveOpenFile(szFileName, &streamWaveDesc.hmmio, &pwfx, &streamWaveDesc.mmckinfoParent) != 0)
		return FALSE;
	if(WaveStartDataRead(&streamWaveDesc.hmmio, &streamWaveDesc.mmckinfo, &streamWaveDesc.mmckinfoParent) != 0)
		return FALSE;

	// Create a secondary sound buffer which will hold 2 seconds of data
	memset(&dsbdesc, 0, sizeof(DSBUFFERDESC ) );
	dsbdesc.dwSize = sizeof(dsbdesc);
	dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2
		              | DSBCAPS_GLOBALFOCUS
                      | DSBCAPS_CTRLPAN; 
	dsbdesc.dwBufferBytes = pwfx->nAvgBytesPerSec * 2;
	dsbdesc.lpwfxFormat = pwfx;

	hRet = lpds->CreateSoundBuffer(&dsbdesc, &lpDSStreamBuffer, NULL);

	if(FAILED(hRet) )
	{
		WaveCloseReadFile(&streamWaveDesc.hmmio, &pwfx);
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error creating streaming buffer", "ERROR", MB_OK);
		LogErrorToFile("Error creating streaming buffer");
		return FALSE;
	}

	FillBufferWithSilence2(lpDSStreamBuffer);
	hRet = lpDSStreamBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if(FAILED(hRet) )
	{
		WaveCloseReadFile(&streamWaveDesc.hmmio, &pwfx);
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error playing streaming buffer", "ERROR", MB_OK);
		LogErrorToFile("Error playing streaming buffer");
		return FALSE;
	}

	streamWaveDesc.dwMidBuffer = dsbdesc.dwBufferBytes/2;

	return TRUE;
}




//***********************************************************************
// Function: DSound::PlayStreamingSoundBuffer
//
// Purpose:  Loads a streaming sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to sound
//			   buffer to be loaded
//
//			   char *szFile - The name of the wav file to load into the 
//			   the sound buffer		  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::PlayStreamingSoundBuffer()
{
	DWORD	dwPlay;
	DWORD   dwStartOfs;
	static  DWORD dwLastPlayPos;
	VOID    *lpvData;
	DWORD   dwBytesLocked;
	UINT    cbBytesRead;

	if(lpDSStreamBuffer == NULL)
		return FALSE;

	hRet = lpDSStreamBuffer->GetCurrentPosition(&dwPlay, NULL);

	if(FAILED(hRet) )
	{
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error getting current play position", "PLAYSTREAMINGBUFFER", MB_OK);
		LogErrorToFile("Error getting current play position", "PLAYSTREAMINGBUFFER");
		return FALSE;
	}

	// If the play position has reached the first or second half of the buffer,
	// it's time to stream data to the other half
	if( ( (dwPlay >= streamWaveDesc.dwMidBuffer) &&
		(dwLastPlayPos < streamWaveDesc.dwMidBuffer) ) ||
		(dwPlay < dwLastPlayPos) )
	{
		dwStartOfs = (dwPlay >= streamWaveDesc.dwMidBuffer) ? 0 : streamWaveDesc.dwMidBuffer;

		hRet = lpDSStreamBuffer->Lock(dwStartOfs,
									  streamWaveDesc.dwMidBuffer,
									  &lpvData,
									  &dwBytesLocked,
									  NULL,
									  NULL,
									  0);

	if(FAILED(hRet) )
	{
		DisplayDSoundError(hwnd, hRet);
		MessageBox(hwnd, "Error locking streaming sound buffer", "PLAYSTREAMINGBUFFER", MB_OK);
		LogErrorToFile("Error locking streaming sound buffer", "PLAYSTREAMINGBUFFER");
		return FALSE;
	}

 
		WaveReadFile(streamWaveDesc.hmmio,
					 dwBytesLocked - cbBytesRead,
					 (BYTE*)lpvData + cbBytesRead,
					 &streamWaveDesc.mmckinfo,
					 &cbBytesRead);

		// Test for end of file
		if(cbBytesRead < dwBytesLocked)
		{
			if(WaveStartDataRead(&streamWaveDesc.hmmio, &streamWaveDesc.mmckinfo,
				                 &streamWaveDesc.mmckinfoParent) != 0)
			{
				MessageBox(hwnd, "Can't reset file in PlayStreamingSoundBuffer",
   					       "ERROR", MB_OK);
				LogErrorToFile("Can't reset file in PlayStreamingSoundBuffer");
			}
			else
			{
				WaveReadFile(streamWaveDesc.hmmio, dwBytesLocked - cbBytesRead,
					         (BYTE*)lpvData + cbBytesRead,
							 &streamWaveDesc.mmckinfo,
							 &cbBytesRead);
			}
		}

		hRet = lpDSStreamBuffer->Unlock(lpvData, dwBytesLocked, NULL, 0 );

		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			MessageBox(hwnd, "Error unlocking streaming sound buffer", "PLAYSTREAMINGBUFFER", MB_OK);
			LogErrorToFile("Error unlocking streaming sound buffer", "PLAYSTREAMINGBUFFER");
			return FALSE;
		}
	}


	dwLastPlayPos = dwPlay;
	return TRUE;
}




//***********************************************************************
// Function: DSound::DuplicateBuffers
//
// Purpose:  Duplicates a secondary sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to sound 
//			   buffer to be duplicated
//
//			   LPDIRECTSOUNDBUFFER *lpDsbCopy - Pointer to pointer to
//			   duplicate sound buffer	
//			  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::DuplicateStaticSoundBuffer(
					 LPDIRECTSOUNDBUFFER *lpDsbSrc,
					 LPDIRECTSOUNDBUFFER *lpDsbCopy)
{	
	hRet = lpds->DuplicateSoundBuffer( (*lpDsbSrc), lpDsbCopy );
	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Dupliacte in DuplicateBuffers", "ERROR", MB_OK);
		DisplayDSoundError(hwnd, hRet);
		return FALSE;
	}
	
	return TRUE;
}	/* end DuplicateBuffers */




//***********************************************************************
// Function: DSound::PlaySoundBuffer
//
// Purpose:  Plays a static sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to the 
//			   sound buffer to be played
//
//			   char *szFile - the name opf the wav file to be reloaded if
//			   needed	
//			  
//
// Returns: BOOL - TRUE for success, FALSE for failure
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
BOOL DSound::PlayStaticSoundBuffer(LPDIRECTSOUNDBUFFER *lpDsb, char *szFile)
{
	if(*lpDsb == NULL)
		return FALSE;
	
	// Set the buffer to the start position
	(*lpDsb)->SetCurrentPosition(0);
	
	// Now attempt to play the file, if the buffer has been lost then we play
	// the file after a call to restore
	hRet = (*lpDsb)->Play(0, 0, 0);
	
	if(hRet == DSERR_BUFFERLOST)
	{
		hRet = (*lpDsb)->Restore();
		
		if(SUCCEEDED(hRet) )
		{
			if(LoadStaticSoundBuffer(lpDsb, szFile) )
				(*lpDsb)->Play(0, 0, 0);
		}
		else
		{
			DisplayDSoundError(hwnd, hRet);
			MessageBox(hwnd, "PlaySoundBuffer", "ERROR", MB_OK);
			return FALSE;
		}
	}
	else
	{
		DisplayDSoundError(hwnd, hRet);
		return FALSE;
	}
	
	return TRUE;
}



//***********************************************************************
// Function: DSound::StopStreamingBuffer
//
// Purpose:  Stops the current streaming buffer from playing if it exists
//			 
// Parameters: none		  
//
// Returns: void
//
// Last Modified:  Date 11/21/99      Author - Max Szlagor     
//
//***********************************************************************
void DSound::StopStreamingBuffer() 
{
    if(lpDSStreamBuffer)
        lpDSStreamBuffer->Stop();  
	
    // Close any open file and free the buffer.
	
    WaveCloseReadFile( &hmmio, &pwfx );
    if ( lpDSStreamBuffer != NULL )
    {
        lpDSStreamBuffer->Release();
        lpDSStreamBuffer = NULL;
    }

	iStreamState = STREAM_NOT_PLAYING;
}



//***********************************************************************
// Function: DSound::PanSwitch
//
// Purpose:  Changes the pan of a sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to the 
//			   sound buffer which will be changning pan
//
//			   int Panshift - the amount by which to change pan
//			  
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
void DSound::PanSwitch(LPDIRECTSOUNDBUFFER lpDsb,  int PanShift)
{
	LONG lCurrent = 0;
	
	if(lpDsb != NULL)
	{
		// Get the current value of the pan
		hRet = lpDsb->GetPan(&lCurrent);
		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			return;
		}
		
		// Now increment the pan by the specified amount
		lCurrent += PanShift;
		if(lCurrent > DSBPAN_RIGHT)
			lCurrent = DSBPAN_RIGHT;
		if(lCurrent < DSBPAN_LEFT)
			lCurrent = DSBPAN_LEFT;
		
		// Now set the pan to the newly calculated value
		hRet = lpDsb->SetPan(lCurrent);
		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			return;
		}
	}
}




//***********************************************************************
// Function: DSound::VolumeSwitch
//
// Purpose:  Changes the volume of a sound buffer
//			 
//
// Parameters: 
//			   LPDIRECTSOUNDBUFFER *lpDsb - Pointer to pointer to the 
//			   sound buffer which will be changning volume
//
//			   int state - whether the sound buffer has reached its lowest volume
//
// Returns: void
//
// Last Modified:  Date 11/15/99      Author - Max Szlagor     
//
//***********************************************************************
int DSound::VolumeSwitch(LPDIRECTSOUNDBUFFER lpDsb, int VolShift)
{
	LONG lCurrent = 0;
	
	if( lpDsb != NULL)
	{
		// Get the current volume of the buffer
		hRet = lpDsb->GetVolume(&lCurrent);
		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			return -1;
		}
		
		// Increment the volume by the specified amount
		lCurrent += VolShift;
		if(lCurrent > DSBVOLUME_MAX)
			lCurrent = DSBVOLUME_MAX;
		if(lCurrent < DSBVOLUME_MIN)
			lCurrent = DSBVOLUME_MIN;
		
		// Now set the new volume
		hRet = lpDsb->SetVolume(lCurrent);
		if(FAILED(hRet) )
		{
			DisplayDSoundError(hwnd, hRet);
			return -1;
		}
	}

	if(lCurrent == DSBVOLUME_MIN)
		return SOUND_MIN;
	if(lCurrent >= DSBVOLUME_MAX/2)
		return SOUND_NORMAL;
	else if(lCurrent == DSBVOLUME_MAX)
		return SOUND_MAX;
	else
		return 100;
}


