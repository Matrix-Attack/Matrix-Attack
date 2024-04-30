//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			12/14/00
//	DESCRIPTION:	GSOUND.CPP		
//**********************************************************************************
#include <windows.h>
#include <string>

using namespace std;

#include "fmod.h"
#include "fmod_errors.h"
#include "gsound.h"
//#include "./mmgr.h"


//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSound::CSound
//	RETURN TYPE:	NONE
//	DESCRIPTION:	Constructor for the sound class			
//**********************************************************************************
CSound::CSound()
{
	m_iLastError		 = 0;
	m_cTrackList		 = NULL;
	m_cStaticSoundList   = NULL;
	m_iNumSongs			 = 0;
	m_iNumStaticSounds   = 0;

}

//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSound::SetHwnd
//	RETURN TYPE:	void 
//	DESCRIPTION:	Sets the hwnd for FMOD's internal use		
//**********************************************************************************
int CSound::SetHwnd(HWND hwnd)
{
	// Set the window handle for DirectSound so we know when we are in focus
	if(!FSOUND_SetHWND(hwnd) )
	{
		m_iLastError = FSOUND_GetError();
		MessageBox(hwnd, FMOD_ErrorString(m_iLastError), "ERROR", MB_OK);
		return 0;
	}

	return 1;
}

//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSound::Init
//	RETURN TYPE:	int 
//	DESCRIPTION:	Initializes the sound class		
//**********************************************************************************
int CSound::Init(HWND& hwnd)
{
	// Sound initialization code here
	if ( FSOUND_GetVersion() < FMOD_VERSION )
	{
		MessageBox(hwnd, "You are using the wrong FMod DLL version!", "Error", MB_OK );
		return 0;
	}

	if(!FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND) )
	{
		m_iLastError = FSOUND_GetError();
		MessageBox(hwnd, FMOD_ErrorString(m_iLastError), "ERROR", MB_OK);
		return 0;
	}

	FSOUND_SetDriver( 0 );		// Default Driver (Maybe provide to user?)
	FSOUND_SetMixer( FSOUND_MIXER_QUALITY_AUTODETECT );						// Set Mixer
	
	FSOUND_SetSFXMasterVolume( 300 );
	
//	FMUSIC_SetMasterVolume(

	// Set the window handle for DirectSound so we know when we are in focus
	if(!FSOUND_SetHWND(hwnd) )
	{
		m_iLastError = FSOUND_GetError();
		MessageBox(hwnd, FMOD_ErrorString(m_iLastError), "ERROR", MB_OK);
		return 0;
	}
	

	if(!FSOUND_Init(48000, 64, 0) )
	{
		m_iLastError = FSOUND_GetError();
		MessageBox(hwnd, FMOD_ErrorString(m_iLastError), "ERROR", MB_OK);
		return 0;
	}
	
	m_bSoundInitialized = 1;
	
	return 1;
}


//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			5/9/01
//	FUNCTION NAME:	CSound::LoadAllStaticSounds
//	RETURN TYPE:	int 
//	DESCRIPTION:	Loads in all the sound effects 
//					These are all uniformly loaded in to be non streaming, non 3d
//					and non looping		
//**********************************************************************************
int CSound::LoadAllStaticSounds(HWND hwnd)
{
	for(int i = 0; i < m_iNumStaticSounds; i++)
	{
		m_cStaticSoundList[i].Load(m_cStaticSoundList[i].m_strFileName, 0, 0, 0, hwnd);
	}

	return 1;
}


signed char StreamCallBack(FSOUND_STREAM *stream, void *buff, int len, int param)
{
	((CSoundSample*)param)->m_bPlayedOnce = true;
	return 0;
}

//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			5/9/01
//	FUNCTION NAME:	CSound::LoadAllMusic
//	RETURN TYPE:	int 
//	DESCRIPTION:	Loads in all the music 
//					These are all uniformly loaded in to be non 3d looping streams	
//**********************************************************************************
int CSound::LoadAllMusic(HWND hwnd)
{
	string strPath;

	for(int i = 0; i < m_iNumSongs; i++)
	{
		strPath = m_cTrackList[i].m_strFileName;
		
		//***FIX*** Music is in the path
		if( 0 == m_cTrackList[i].Load(strPath, false, true, false, hwnd) )
		{
			MessageBox(hwnd, "Could not load all music!", "STARTUP ERROR", MB_OK);
			return 0;
		}
		
		//Set the callback function for when the streaming sound ends
		FSOUND_Stream_SetEndCallback(
			(FSOUND_STREAM*)m_cTrackList[i].m_pSample,	// FSOUND_STREAM *stream,
			StreamCallBack,								// FSOUND_STREAMCALLBACK callback,
			(int)(m_cTrackList + i) );										// int userdata
		
//		m_cTrackList[i].m_strSongName = m_cTrackList[i].m_strSongName;

		//we loaded the song just fine, so get its title/name
		//m_cTrackList[i].m_strSongName = FMUSIC_GetName((FMUSIC_MODULE*)m_cTrackList[i].m_pSample);
	}
	
	return 1;
}


//**********************************************************************************
//	FUNCTION NAME:	PauseCurrentTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/24/2001
//	
//	RETURN TYPE:	int 	- 0 == success, 1 == fail
//	DESCRIPTION:	pauses current track
//**********************************************************************************
int CSound::PauseCurrentTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0 )
		return 0; // error code
	
	//Stop current track
	m_cTrackList[m_iCurrentTrack].Pause();

	return 1; //success code
}

//**********************************************************************************
//	FUNCTION NAME:	UnpauseCurrentTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/24/2001
//	
//	RETURN TYPE:	int 	- 0 == success, 1 == fail
//	DESCRIPTION:	unPauses the current track
//**********************************************************************************
int CSound::UnpauseCurrentTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0 )
		return 0; // error code
	
	//Stop current track
	m_cTrackList[m_iCurrentTrack].Unpause();

	return 1; //success code
}

//**********************************************************************************
//	FUNCTION NAME:	CSound::StopCurrentTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/24/2001
//	
//	DESCRIPTION:	stops playing the current song
//**********************************************************************************
int CSound::StopCurrentTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0 )
		return 0; // error code
	
	//Stop current track
	m_cTrackList[m_iCurrentTrack].Stop();

	return 1; //success code
}

//**********************************************************************************
//	FUNCTION NAME:	CSound::PlayRandomTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/24/2001
//	
//	RETURN TYPE:	int 	- 1 == success, 0 == fail
//	DESCRIPTION:	Picks a random song out of the List of tracks
//**********************************************************************************
int CSound::PlayRandomTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0)
		return 0; //error code
	
	//Stop the current track
	m_cTrackList[m_iCurrentTrack].Stop();

	//Choose a random track
	m_iCurrentTrack = rand() % m_iNumSongs;
	
	//Play the new track
	m_cTrackList[m_iCurrentTrack].Play();
	
	return 1; //Success code
}

//**********************************************************************************
//	FUNCTION NAME:	CSound::PlayNextTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/25/2001
//	
//	ARGUMENT 1:		void	- 
//	RETURN TYPE:	int 	- 
//	DESCRIPTION:	
//**********************************************************************************
int CSound::PlayNextTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0)
		return 0; //error code
	
	//Stop the current track
	m_cTrackList[m_iCurrentTrack].Stop();

	//Choose the next track
	++m_iCurrentTrack;

	//Bounds checking
	if(m_iCurrentTrack >= m_iNumSongs)
		m_iCurrentTrack = 0;
	
	//Play the new track
	m_cTrackList[m_iCurrentTrack].Play();

	return 1; //Success code
}

//**********************************************************************************
//	FUNCTION NAME:	CSound::PlayNextTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/25/2001
//	
//	ARGUMENT 1:		void	- 
//	RETURN TYPE:	int 	- 
//	DESCRIPTION:	
//**********************************************************************************
int CSound::PlayPrevTrack( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0)
		return 0; //error code
	
	//Stop the current track
	m_cTrackList[m_iCurrentTrack].Stop();

	//Choose the next track
	--m_iCurrentTrack;
	
	//Bounds checking
	if(m_iCurrentTrack < 0)
		m_iCurrentTrack = m_iNumSongs-1;
	
	//Play the new track
	m_cTrackList[m_iCurrentTrack].Play();

	return 1; //Success code
}

//**********************************************************************************
//	FUNCTION NAME:	CSound::IsTrackFinished
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/29/2001
//	
//	RETURN TYPE:	signed char 	- true if Track is finished, false otherwise
//	DESCRIPTION:	Checks to see if the track is finished or not.
//**********************************************************************************
signed char CSound::IsTrackFinished( void )
{
	if( m_cTrackList == NULL || m_iNumSongs == 0 )
		return false; //return failure
	
	return m_cTrackList[m_iCurrentTrack].m_bPlayedOnce;

}

//**********************************************************************************
//	FUNCTION NAME:	CSound::PlayTrack
//	AUTHOR(S):		Elijah Emerson
//	CREATION DATE:	7/29/2001
//	
//	ARGUMENT 1:		const char* szTrackName	- 
//	RETURN TYPE:		- 
//	DESCRIPTION:	
//**********************************************************************************
int	CSound::PlayTrack( const char* szTrackName )
{
	bool bFound		= false;
	int	 iNewTrack	= 0;

	if( szTrackName == NULL  || szTrackName[0] == NULL ||
		m_cTrackList == NULL || m_iNumSongs == 0 )
		return 0; //return failure

	//Look for szTrackName
	for(int i=0; i < m_iNumSongs; i++)
	{
		if( !stricmp(szTrackName, m_cTrackList[i].m_strSongName.c_str() ) )
		{
			bFound		= true;
			iNewTrack	= i;
		}
	}

	if( bFound )
	{
		//Stop current track
		m_cTrackList[m_iCurrentTrack].Stop();

		//Start playing the new track
		m_cTrackList[iNewTrack].Play();
		
		//Set the current track as the new track
		m_iCurrentTrack = iNewTrack;
		
		return 1;//success
	}
	
	return 0;//failure
}

//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/23/01
//	FUNCTION NAME:	CSound::Destroy
//	RETURN TYPE:	int 
//	DESCRIPTION:	Shuts down the sound system		
//**********************************************************************************
int CSound::Destroy()
{
	int i;

	if(m_cTrackList)
	{
		for(i = 0; i < m_iNumSongs; i++)
		{
			m_cTrackList[i].Free();
		}
		m_iNumSongs = 0;
	
		delete[] m_cTrackList;
		m_cTrackList = 0;
	}

	if(m_cStaticSoundList)
	{
		for(i = 0; i < m_iNumStaticSounds; i++)
		{
			m_cStaticSoundList[i].Free();
		}
		m_iNumStaticSounds = 0;

		delete[] m_cStaticSoundList;
		m_cStaticSoundList = 0;
	}

	FSOUND_Close();

	return 1;
}






