//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			12/14/00
//	DESCRIPTION:	GSOUND.CPP		
//**********************************************************************************
#include <windows.h>
#include <string>

using namespace std;

#include "./mmgr.h"

#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"


//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Clear
//	RETURN TYPE:	NONE
//	DESCRIPTION:	Zeroes out the fields of the sound sample		
//**********************************************************************************
void CSoundSample::Clear()
{
	m_pSample		= NULL;
	m_bLooping		= 0;
	m_bStream		= 0;
	m_iChannel		= -1;
	m_b3d			= 0;
	m_vPos[0]		= 0.0f;
	m_vPos[1]		= 0.0f;
	m_vPos[2]		= 0.0f;
	m_vVel[0]		= 0.0f;
	m_vVel[1]		= 0.0f;
	m_vVel[2]		= 0.0f;
	
	m_bPlayedOnce	= false;
}



//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Load
//	RETURN TYPE:	int 
//	ARGUMENT 1:		string &rStrFilename
//	ARGUMENT 2:		bool bLooping
//	ARGUMENT 3:		bool bStream
//	ARGUMENT 4:		bool b3d
//	DESCRIPTION:	Loads the sound sample with the specified name and parameters		
//**********************************************************************************
int CSoundSample::Load(string &rStrFilename, bool bLooping, bool bStream, bool b3d, HWND hwnd)
{
	int				iIndex;
	unsigned int 	iModeFlags = 0;
	
	m_hWnd = hwnd;
	
	iIndex		  = rStrFilename.find_last_of( "." );
	string strSub = rStrFilename.substr( iIndex );
	
	// Only process .wav and .mp3 files
	if ( strSub != ".mp3" && strSub != ".wav")
		return 0;
	
	if(bStream)
	{
		iModeFlags |= FSOUND_NORMAL;
		
		if(strSub == ".wav")
			iModeFlags |=  FSOUND_2D;
	}
	else // If this isn't a streaming sample then check for 3d sound
	{
		if(b3d)
			iModeFlags |= FSOUND_HW3D;
		else
			iModeFlags |= FSOUND_2D;
	}
	
	if(bLooping)
		iModeFlags |= FSOUND_LOOP_NORMAL;
	
	if(!bStream)
		m_pSample =	(void*)FSOUND_Sample_Load(0, rStrFilename.c_str(),  iModeFlags, 0);
	else
		m_pSample = (void*)FSOUND_Stream_OpenFile(rStrFilename.c_str(), iModeFlags, 0);
	
	// Verify the sample was created properly
	if(!m_pSample)
	{
		m_iLastError = FSOUND_GetError();
		MessageBox(m_hWnd, FMOD_ErrorString(m_iLastError), "ERROR", MB_OK);
		return 0;
	}
	
	// 3d setup
	if (b3d && !bStream)
		FSOUND_Sample_SetMinMaxDistance( (FSOUND_SAMPLE*)m_pSample, 20.0f, 100000.0f );

	m_bLooping = bLooping;
	m_bStream  = bStream;
	m_b3d      = b3d;
	
	return 1;
}



//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Play
//	RETURN TYPE:	int 
//	DESCRIPTION:	Plays the sound sample		
//**********************************************************************************
int CSoundSample::Play()
{
	if(!m_pSample)
		return 0;
	
	m_bPlayedOnce = false;

	if(m_bStream)
		m_iChannel = FSOUND_Stream_Play( FSOUND_FREE, (FSOUND_STREAM*)m_pSample );
	else
	{
		m_iChannel = FSOUND_PlaySound( FSOUND_FREE, (FSOUND_SAMPLE*)m_pSample );

		if(m_b3d)
			FSOUND_Reverb_SetMix( m_iChannel, FSOUND_REVERBMIX_USEDISTANCE );
		else
			FSOUND_SetPan( m_iChannel, FSOUND_STEREOPAN );
	}

	return 1;
}



//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/24/01
//	FUNCTION NAME:	CSoundSample::Update
//	RETURN TYPE:	void 
//	DESCRIPTION:	Updates 3d sound buffers		
//**********************************************************************************
void CSoundSample::Update()
{
	if(m_b3d)
		FSOUND_3D_Update();
}


//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Free
//	RETURN TYPE:	void 
//	DESCRIPTION:	Frees the sound sample		
//**********************************************************************************
int CSoundSample::Free()
{
	if(!m_pSample)
		return 0;

	// First stop the track or sound from playing
	Stop();

	if(m_bStream)
		FSOUND_Stream_Close( (FSOUND_STREAM*)m_pSample);
	else
		FSOUND_Sample_Free( (FSOUND_SAMPLE*)m_pSample);

	// Now make the sample return to its initial state
	Clear();

	return 1;
}



//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Pause
//	RETURN TYPE:	int 
//	DESCRIPTION:	Pauses the sound sample		
//**********************************************************************************
int CSoundSample::Pause()
{
	char chRet;

	if(!m_pSample)
		return 0;

	if (m_bStream)
		chRet = FSOUND_Stream_SetPaused( (FSOUND_STREAM*)m_pSample, TRUE);
	else
		chRet = FSOUND_SetPaused(m_iChannel, TRUE);

	if(chRet == FALSE)
		return 0;

	return 1;
}


//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Unpause
//	RETURN TYPE:	int 
//	DESCRIPTION:	Unpauses the sound sample		
//**********************************************************************************
int CSoundSample::Unpause()
{
	if(!m_pSample)
		return 0;

	char chRet;

	if (m_bStream)
		chRet = FSOUND_Stream_SetPaused( (FSOUND_STREAM*)m_pSample, FALSE);
	else
		chRet = FSOUND_SetPaused(m_iChannel, FALSE);

	if(chRet == FALSE)
		return 0;

	return 1;
}

//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSoundSample::Stop
//	RETURN TYPE:	int 
//	DESCRIPTION:	Stops the sound sample		
//**********************************************************************************
int CSoundSample::Stop()
{
	m_bPlayedOnce = false;

	if(!m_pSample)
		return 0;

	if(m_bStream)
		FSOUND_Stream_Stop( (FSOUND_STREAM*)m_pSample);
	else
		FSOUND_StopSound(m_iChannel);

	m_iChannel = -1;

	return 1;
}