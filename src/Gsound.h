//**********************************************************************************
//	AUTHOR(S):		Max Szlagor
//	DATE:			1/18/01
//	DESCRIPTION:	GSOUND.H		
//  Props to Andy Luedke for pointing me to FMOD and providing a framework for its setup
//**********************************************************************************
#ifndef GSOUND_H
#define GSOUND_H

#define SFX_SWITCH	 0
#define SFX_LINKER1  1
#define SFX_LINKER2  2
#define SFX_LINKER3  3
#define SFX_FLIP	 4
#define SFX_CLUSTER1 5
#define SFX_CLUSTER2 6
#define SFX_CLUSTER3 7

#define SFX_COMBO1	 8
#define SFX_COMBO2	 9
#define SFX_COMBO3	 10

class CSoundSample
{
public:
	friend class CSound;
			CSoundSample	( void ) {Clear();}
	void	Clear			( void );
	int		Load			(string& rStrFilename, bool bLooping, bool bStream, bool b3d, HWND hwnd);
	int		Play			( void );
	int		Free			( void );
	int		Stop			( void );
	int		Pause			( void );
	int		Unpause			( void );
	void	Update			( void );
	string  m_strFileName;		// Name of the file (used for loading)
	string  m_strSongName;
	bool	m_bPlayedOnce;	// Has this sound played once yet?

private:
	void*	m_pSample;		// Used internally within fmod
	long	m_iChannel;		// Used internally within fmod
	bool	m_bLooping;		// Does this sound loop?
	bool	m_b3d;			// Is the sound 3d?
	bool	m_bStream;		// Is this a stream?
	float	m_vPos[3];		// Position (for 3d sounds)
	float   m_vVel[3];		// Velocity (for 3d sounds)
	HWND	m_hWnd;			// Window handle (for error output)
	int		m_iLastError;	// Last error code returned from an fmod function
	unsigned int m_bPlaying; // Is the sound currently playing?
	unsigned int m_bCurNumPlaying; // How many instances of the sound are currently
	unsigned int m_bMaxNumPlaying; // Maximum number of sounds that can be played at once
};


class CSound
{
public:	
		 CSound					( void );
		 ~CSound				( void ) {Destroy();}
	int  Init					(HWND& hwnd );
	int  SetHwnd				(HWND hwnd );
	int  Destroy				( void );
	int  ParseSoundFile			( char* szFile );
	int  LoadAllStaticSounds	( HWND hwnd );
	int  LoadAllMusic			( HWND hwnd );
	int	 IsSoundInitialized		( void )	{ return m_bSoundInitialized; }
	
	//Eli added these functions
	signed char IsTrackFinished			( void );
	int			StopCurrentTrack		( void );
	int			PauseCurrentTrack		( void );
	int			UnpauseCurrentTrack		( void );
	int			PlayRandomTrack			( void );
	int			PlayNextTrack			( void );
	int			PlayPrevTrack			( void );
	int			PlayTrack				( const char* szTrackName );
	int			GetCurrentTrackIndex	( void ) { return m_iCurrentTrack; }
	
	//

	// These are dynamic arrays of sound effects/music tracks
	CSoundSample*		m_cTrackList;		
	CSoundSample*		m_cStaticSoundList;

private:
	int					m_iCurrentTrack;
	int					m_iLastError;
	int					m_iNumSongs;
	int					m_iNumStaticSounds;
	int					m_bSoundInitialized;
};

#endif