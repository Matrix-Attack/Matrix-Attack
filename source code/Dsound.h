#ifndef DSOUND_H
#define DSOUND_H


#define SOUND
/*
#define TRACK_1				"data//sound//track1.wav"
#define TRACK_2				"data//sound//track2.wav"
#define SWITCH_SOUND		"data//sound//switch.wav"
#define LINKER_SOUND1		"data//sound//Linker1.wav"
#define LINKER_SOUND2		"data//sound//Linker2.wav"
#define LINKER_SOUND3		"data//sound//Linker3.wav"
#define FLIP_SOUND			"data//sound//flip.wav"
#define CLUSTER_SOUND1		"data//sound//Cluster1.wav"
#define CLUSTER_SOUND2		"data//sound//Cluster2.wav"
#define CLUSTER_SOUND3		"data//sound//Cluster3.wav"

#define COMBO_SOUND1		"data//sound//Combo1.wav"
#define COMBO_SOUND2		"data//sound//Combo2.wav"
#define COMBO_SOUND3		"data//sound//Combo3.wav"
*/

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
#define SFX_STREAK1	 11
#define SFX_STREAK2	 12
#define SFX_STREAK3	 13
#define SFX_STREAK4	 14
#define SFX_STREAK5	 15
#define SFX_STREAK6	 16
#define SFX_STREAK7	 17

#define SOUND_MIN	 1
#define SOUND_NORMAL 2
#define SOUND_MAX	 3

#define CUR_SELECTED_TRACK_1	 1
#define CUR_SELECTED_TRACK_2	 2

#define STREAM_NOT_PLAYING  0
#define STREAM_FADING		1
#define STREAM_PLAYING_FULL	2

typedef struct __STATICSOUNDBUFFER
{
  LPDIRECTSOUNDBUFFER lpDsb[3];
  int				  curIndex;
} STATICSOUNDBUFFER, *P_STATICSOUNDBUFFER;

typedef struct __SOUNDBUFFER
{
	char				szFileName[100];
	LPDIRECTSOUNDBUFFER lpDsb;
	bool				bLoaded;
} SOUNDBUFFER, *P_SOUNDBUFFER;

typedef struct __SOUNDBUFFERHEAD
{
	int numbuffers;
	P_SOUNDBUFFER *pStart;
} SOUNDBUFFERHEAD, *P_SOUNDBUFFERHEAD;

typedef struct __SOUNDDESC 
{
	LPGUID lpGuid;							// The guid of a given sound device
	char   cName[50];						// The name of the device
	struct __SOUNDDESC *pNext;				// Pointer to next device in the list
	struct __SOUNDDESC *pPrev;				// Pointer to previous device in the list
} SOUNDDESC, *P_SOUNDDESC;


typedef struct __DSDESC_HEAD
{
	UINT	iNumberDesc;					// The total number of devices
	P_SOUNDDESC	pStart;						// The first device in the list					
	UINT	iChosenDesc;					// The chosen device for sound playback 
} DSDESC_HEAD, *P_DSDESC_HEAD, **PP_DSDESC_HEAD;


typedef struct __STREAMINGWAVDESCRIPTOR
{
	HMMIO		hmmio;
	MMCKINFO	mmckinfo;
	MMCKINFO    mmckinfoParent;
	DWORD		dwMidBuffer;
} STREAMINGWAVDESCRIPTOR, *P_STREAMINGWAVDESCRIPTOR;


class DSound
{
	public:
		DSDESC_HEAD SoundDevicesHead;
		bool		bSoundEnabled;
		bool		bPlayStream;
		bool		bPaused;
		int			iStreamState;
		HWND		hwnd;
		LPDIRECTSOUND		   lpds;						// The direct sound object
		LPDIRECTSOUNDBUFFER    lpDsbPrimary;				// The primary sound buffer
		LPDIRECTSOUNDBUFFER    lpDSStreamBuffer;			// The current streaming buffer
		LPDIRECTSOUNDBUFFER    lpDSStreamBuffer2;			// The current streaming buffer


		STATICSOUNDBUFFER	   lpDSSStaticLinker1;
		STATICSOUNDBUFFER	   lpDSSStaticLinker2;
		STATICSOUNDBUFFER	   lpDSSStaticLinker3;
		STATICSOUNDBUFFER	   lpDSSStaticCluster1;
		STATICSOUNDBUFFER	   lpDSSStaticCluster2;
		STATICSOUNDBUFFER	   lpDSSStaticCluster3;
		STATICSOUNDBUFFER	   lpDSSStaticCombo1;
		STATICSOUNDBUFFER	   lpDSSStaticCombo2;
		STATICSOUNDBUFFER	   lpDSSStaticCombo3;

		STATICSOUNDBUFFER	   lpDSSStaticSwitch;
		STATICSOUNDBUFFER	   lpDSSStaticFlip;
		SOUNDBUFFERHEAD		   StaticSoundList;
		STREAMINGWAVDESCRIPTOR streamWaveDesc;		     	// Description of the streaming sound
		DWORD				   dwStreamCurrentPosition;		// The current position in the streaming buffer

		LPGUID				lpSoundGuid;				// The guid of the user selected sound device
		char				szSoundDevice[125];			// Description of the chosen sound device
		HRESULT				hRet;
		int					iCurStreamingBuffer;		// The currently loaded streaming buffer
		int					Selection;

		DSound();
		void SoundInit(HWND Hwnd);
		void InitDSDeviceHead();
		void GrabSoundGuid();
		void DestroyDSDeviceList();
		BOOL InitDSound(LPGUID *lpGuid);
		BOOL LoadStaticSoundBuffer(LPDIRECTSOUNDBUFFER *lpDsb, char *szFile);
		BOOL DuplicateStaticSoundBuffer(LPDIRECTSOUNDBUFFER *lpDsbSrc, LPDIRECTSOUNDBUFFER *lpDsbCopy);
		BOOL PlayStaticSoundBuffer(LPDIRECTSOUNDBUFFER *lpDsb, char *szFile);
		BOOL LoadStreamingSoundBuffer(char *szFileName);
		BOOL PlayStreamingSoundBuffer();
		BOOL FillBufferWithSilence(LPDIRECTSOUNDBUFFER lpDsb);
		void PanSwitch(LPDIRECTSOUNDBUFFER lpDsb,  int PanShift);
		int  VolumeSwitch(LPDIRECTSOUNDBUFFER lpDsb, int VolShift);
		void UnloadSoundBuffers();
		void LoadSoundBuffers();
		~DSound();

		void StopStreamingBuffer(); 
		BOOL FillBufferWithSilence2( LPDIRECTSOUNDBUFFER lpDsb );
		BOOL SetupStreamBuffer( LPSTR lpzFileName );
		BOOL PlayBuffer( void );
		void Cleanup( void );
		void SetCurrentStreamBuffer(int whichBuffer);
		void PlayStaticSound(int whichSound);
};



BOOL CALLBACK MyDSoundEnumCallback(LPGUID lpGuid, LPCSTR lpstrDescription,
								   LPCSTR lpstrModule, LPVOID lpContext);

/*
	LPDIRECTSOUNDBUFFER lpDsbRifle[1];
	LPDIRECTSOUNDBUFFER lpDsbLaser[4];
	LPDIRECTSOUNDBUFFER lpDsbPlasma[4];
	LPDIRECTSOUNDBUFFER lpDsbFlame[4];
	LPDIRECTSOUNDBUFFER lpDsbGrenade[4];

	LPDIRECTSOUNDBUFFER lpDsbMenu;					// Direct sound buffer for menu sound
	LPDIRECTSOUNDBUFFER lpDsbGetItem;				// Direct sound buffer for menu sound

	LPDIRECTSOUND		lpds;						// The direct sound object
	LPDIRECTSOUNDBUFFER lpDsbPrimary;				// The primary sound buffer
	*/

//#define SOUND_ITEM_FILE			"Data\\Sounds\\misc\\Getitem.wav"

#endif