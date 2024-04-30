#ifndef app_h
#define app_h


#define PI						3.141592653589793f
//#define DEBUG					0
#define APP_NAME				"Matrix Attack"
#define GAME_NAME				"Matrix Attack, version 1.23"
#define STR_VERSION				("1.23")
#define NUM_SURFACES			2
#define TIMER_MILLISECONDS		33
#define	PRIMARY					0x0000
#define	BACKBUFFER				0x0001
#define SCREEN_WIDTH			640		
#define SCREEN_HEIGHT			480		

#define SETTINGS_FILENAME	("data\\settings.dat")
#define SOUNDS_FILENAME		("data\\sounds.dat")

//#define  STANDARD_INPUT
#define DINPUT		0

//////////////////////////////
#define NUM_OF_MAIN_SURF			1	//1 main surf
#define NUM_OF_TITLE_SURF			2	//2 title surf
#define NUM_OF_MENU_SURF			8	//11 menu surf //was 9 but i got rid of the font surf -Eli
#define NUM_OF_GAME_SURF			4	//4 game surf

//GAME STATE FLAGS for tagGame_Data
#define GAME_STATE_MENU			 0x01
#define GAME_STATE_GAME			 0x02
#define GAME_STATE_VERSUS_MULTI  0x04
#define GAME_STATE_VERSUS_TTRIAL 0x08
#define GAME_STATE_SINGLE_PUZZLE 0x10
#define GAME_STATE_SINGLE_TTRIAL 0x20
#define GAME_STATE_CURSOR_IS_ON	 0x40
#define GAME_STATE_SWITCHING	 0x80


#define GAME_STATE_SINGLE_GAME		(GAME_STATE_SINGLE_PUZZLE | GAME_STATE_SINGLE_TTRIAL)
#define GAME_STATE_VERSUS_GAME		(GAME_STATE_VERSUS_MULTI | GAME_STATE_VERSUS_TTRIAL)

typedef unsigned int			U32,		*P_U32;
typedef unsigned short			U16,		*P_U16;
typedef char					S8,			*P_S8;
typedef LPDIRECTDRAW			LPDD_OBJ,	*P_LPDD_OBJ;
typedef LPDIRECTDRAWSURFACE		LPDD_SURF,	*P_LPDD_SURF;

typedef class tagApp			APP,		*P_APP;
typedef class tagAPI_DDraw		API_DDRAW,	*P_API_DDRAW;
typedef class tagAPI_OpenGL		API_OPENGL,	*P_API_OPENGL;

typedef struct tagTextObj		TEXTOBJ,	*P_TEXTOBJ;
typedef class  tagFont			FONT,		*P_FONT;
typedef struct glParticle		GL_PARTICLE,*P_GL_PARTICLE;

class Board3D;
class Game_Menu;
struct DISTRUCT;


struct tagSimplePlayer
{
	int	   msgId;	   // Used for DPLAY...THIS MUST BE FIRST

	int	   id;		   // A player id(0-3)
	int	   score;	   // Current score for this player
	int	   frontX;	   // X Position of the player on the board
	int	   frontY;	   // Y Position of the player on the board
	int	   mapX;	   // X Position of the player on the map(backside)
	int	   mapY;	   // Y Position of the player on the map(backside)
	int	   vitality;   // Current vitality for this player
	int	   RowGem;	   // X offset into gem array
	int	   ColumnGem;  // Y offset into gem array
	int    character;  // The character corresponding to this player - foxy, ninja, etc.
	int    bFlipping;  // Is this player flipping?
	int    bSwitching; // Is this player switching?
	int    bCanFlip;   // Can the player flip?
	int    bCanSwitch; // Can the player switch?
	int	   bCanMove;   // Can the player move?
	char   szName[20]; // The player name
};


typedef struct tagCallocMemRecord
{
	void * pAddress;
	char   szRef[40];
	struct tagCallocMemRecord *pNextRecord;
}CALLOC_MEMRECORD, *pCALLOC_MEMRECORD;


typedef struct tagSurfaceRecord
{
	char	szFileName[255];
	LPDIRECTDRAWSURFACE *lpdds;
	int		bVidMem;
	int		iWidth;
	int		iHeight;
	struct	tagSurfaceRecord	*pPrevRecord;
	struct	tagSurfaceRecord *pNextRecord;
	
}SURFACERECORD, *pSURFACERECORD;

typedef struct tagScreen
{
	
	U32	RGBBitCount,		//ULong: Num of bits per pixel (primary surface)
		LoRedBit,			//ULong: lo red bit
		LoGreenBit,			//ULong: lo green bit
		LoBlueBit,			//ULong: lo blue bit
		NumRedBits,			//ULong: num red bits
		NumGreenBits,		//ULong: num green bits
		NumBlueBits;		//ULong: num blue bits
	
	//***********Needed vars for locking the surface***********
	int				iWidth,			// width of the surface in pixels
					iHeight,		// height of the surface in pixels
					iRealWidth;		// width of the surface in memory (wider than pixel width)
	
	int				bGreen6Mode;	// if true, color is in 5-6-5 mode, otherwise it's 5-5-5

	unsigned short	*pScreen;		// pointer to the screen surface
	WORD			wRBitMask,		// mask used to get the red component of a pixel
					wGBitMask,		// mask used to get the green component of a pixel
					wBBitMask;		// mask used to get the blue component of a pixel

}SCREEN, *pSCREEN;

typedef struct tagMultiPlayerOptions
{
	BOOL option[5];
}MP_OPTIONS,*pMP_OPTIONS;
typedef struct tagSettings
{
	// User Settings (set from a launch program or a .cfg file)
	
	U32		RenderMode;		//0  == DDraw,   1 == OpenGL
	U32		ScreenWidth;	//640
	U32		ScreenHeight;	//480
	U32		BitDepth;		//16 == 16bit,  32 == 32bit;
	U32		DetailLevel;	//0 == lowest detail
	U32		bParticles;		//0 == no particles, 1 == particles on
	U32		bGLAntiAliasing;//0 == no AntiAliasing
	U32		bWaitOnVSynch;	//0 == no wait,  1 == wait for VSynch
	U32		bFullScreen;	//0 == windowed, 1 == fullscreen

	void	SetDefaultSettings();
	void	SetSettingsFromFile(char *pFile);
	void	Initialize();
	
}SETTINGS, *P_SETTINGS;

typedef struct ddParticle
{
	
	int			PosX, PosY;	// 0  x and y coordinates. 

	LPDD_SURF	pDDSurf;	// Texture ID
	RECT		ClipRect;	// Sprite ClipRect

	float		VelX, VelY;	// x,y Velocity
	int			Health;		//When Health == 0 the Particle will die
	int			TimeLeft;	//When TimeLeft == 0, Health--;
	
	//Pointers
	int (*pUpdate)(struct ddParticle *pParticle);
	struct ddParticle *pNext;
	struct ddParticle *pPrev;

}DD_PARTICLE, *P_DD_PARTICLE;

class tagAPI_DDraw
{
	private:
		int				iCmdShow;			//int: ??
		HINSTANCE		hInstance;			//HINSTANCE: hinstance of our app		
		int				bAllowFullScreen;	//UChar:Flag to allow alt+enter
		int				bFullScreen;		//UChar: Fullscreen flag
		HWND			Hwnd;				//HWND: handle to our window
		
		
		LPDD_OBJ		lpDD;				//LPDD: Direct Draw object
		LPDD_SURF		lpDDSPrimary;		//LPDDS Primary Surface
		LPDD_SURF		lpDDSBack;			//LPDDS Back Buffer	

		void			DumpDDMainSurfaces ();
		void			GetPixelInfo();
		unsigned long	GetLowBit(unsigned long uiBitmask);
		unsigned long	GetHiBit(unsigned long uiBitmask);
		int			LoadImage24toSurface16 (LPDIRECTDRAWSURFACE	lpdds,BYTE*	pImageBuf,
									int	iWidth,	int	iHeight);
		
		int			LoadImage24toSurface24 (LPDIRECTDRAWSURFACE lpdds,
									BYTE* pImageBuf, int iWidth,	int	iHeight);
		
		int			LoadImage24toSurface32 (LPDIRECTDRAWSURFACE lpdds, 
									BYTE* pImageBuf, int iWidth, int iHeight);
		
		HRESULT		InitializeWindowedDirectDraw (int	iWidth,	// Width of client area
											  int	iHeight);	// Height of client area
			
		HRESULT		InitializeFullScreenDirectDraw (int	iWidth,		// Width of client area
												int	iHeight,	// Height of client area
												int	iColorBits);// Number of color bits in color depth
		void		Mem_AddRef(void * ptr, char *szRefName);
		void		Mem_CheckForLeaks();
		int			DoInit ();
		int			SetupWindowedWindow (LPCSTR		szTitle,	//Title of the application
									LPCSTR		szName,		//Name of the application
									int			iWidth,		//Width of the desired client area
									int			iHeight);	//Height of the desired client area
		int			SetupFullScreenWindow (
							LPCSTR		szTitle,	//Title of the application
							LPCSTR		szName,		//Name of the application
							int			iWidth,		//Width of the desired client area
							int			iHeight		//Height of the desired client area
							);
				
		void		CheckReleasedSurfaces ();
		void		CheckGameSurfaces();
		void		LoadMainSurfaces();	

		void		FlipBuffers();

		//Text Functions
		void DDrawChar0(int x, int y, int Color, char character);
		void DDrawChar1(int x, int y, int Color, char character);
		P_DD_PARTICLE	CreateParticleNode(P_DD_PARTICLE pParticlePos);
		P_DD_PARTICLE	DeleteParticleNode(P_DD_PARTICLE pParticlePos);
		void			CleanUpParticleEngine();

	public:
		
		LPDD_SURF		*surfaceArray;		// array of pointers to surfaces
		int				numSurfaces;		//number of allocated surfaces


		int				iTotalParticles;
		P_DD_PARTICLE	pHeadParticle;
		P_DD_PARTICLE	pTailParticle;
		
		
		void			Destroy();
		int				UpdateParticleEngine();
		int				DrawParticleEngine();
		P_DD_PARTICLE	ddCreateParticle(int PosX, int PosY, float VelX, float VelY,
									     int Health, int Type);

		void		DDrawText(int startX, int startY, U32 Flags, char *szMsg, ... );
		void		DDrawText(RECT *rcClip, U32 Flags, char *szMsg, ... );

		//***SCREEN STRUCT STUFF******
		LPDD_SURF	GetSurface(int index)	{ if(surfaceArray)return surfaceArray[index];else return NULL; }
		int			GetNumSurfaces(){return numSurfaces;}
		struct		tagScreen Screen;
		int			bNotInitializedFlag;
		void		InterruptFrame();
		void		PrepFrame();
		int			InitRGB16();
		int			DetermineGreenBitFormat(DWORD dwRBitMask);
		int			ClearSurface(LPDIRECTDRAWSURFACE lpDDSurface, DWORD dwRGBvalue);
		int			TextMessage(char * s, int x, int y); //used for debug

		void		DrawRect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, COLORREF Color);
		RECT		rcWindow;			//RECT: Rectangle of our client area
		~tagAPI_DDraw();
		
		DWORD WINAPI	RedrawScreen(LPVOID Parm);
		
		int			Initialize();

		void		setiCmdShow(int a){iCmdShow=a;}
		void		sethInstance(HINSTANCE a){hInstance=a;}
		int			getiCmdShow(){return iCmdShow;}
		int			IsAllowFullScreen(){return bAllowFullScreen;}
		int			IsFullScreen(){return bFullScreen;}
		LPDD_SURF	getPrimarySurface(){return lpDDSPrimary;}
		LPDD_SURF	getBackBuffer(){return lpDDSBack;}
		HRESULT		ClearDDRegion (LPDD_SURF *lpdds, RECT rect);
		HWND		getHwnd(){return Hwnd;};
		HINSTANCE   getHInstance(){return hInstance;}
		void		setHwndApp(HWND a){Hwnd=a;}
		LPDD_OBJ	getDDObj(){return lpDD;}
		
		void *		Mem_Alloc(int size, char *szRefName);
		void		Mem_Free(void* ptr);
		void		Mem_CheckPtr(void* ptr);
		
		void		setRGBBitCount(U32 a)	{Screen.RGBBitCount		= a;}
		void		setLoRedBit(U32 a)		{Screen.LoRedBit		= a;}
		void		setLoGreenBit(U32 a)	{Screen.LoGreenBit		= a;}
		void		setLoBlueBit(U32 a)		{Screen.LoBlueBit		= a;}
		void		setNumRedBits(U32 a)	{Screen.NumRedBits		= a;}
		void		setNumGreenBits(U32 a)	{Screen.NumGreenBits	= a;}
		void		setNumBlueBits(U32 a)	{Screen.NumBlueBits		= a;}
		
		U32			getLoRedBit()		{return Screen.LoRedBit;}
		U32			getLoGreenBit()		{return Screen.LoGreenBit;}
		U32			getLoBlueBit()		{return Screen.LoBlueBit;}
		U32			getNumRedBits()		{return Screen.NumRedBits;}
		U32			getNumGreenBits()	{return Screen.NumGreenBits;}
		U32			getNumBlueBits()	{return Screen.NumBlueBits;}
		U32			getRGBBitCount()	{return Screen.RGBBitCount;}
		
		HRESULT		LoadDDBitmap (	LPDIRECTDRAWSURFACE	*lpdds,	LPCSTR	szFilename,	
									int	bVideoMemory, long iWidth, long iHeight );
		int			SetTransparency(LPDIRECTDRAWSURFACE *surface);
		void		DeleteSurface (	LPDIRECTDRAWSURFACE		*lpdds);
		void		CheckResult( HRESULT result );
		
		void		SwitchModes ();
		
};

typedef struct glParticle{
	
	float	PosX, PosY;	// 0  x and y coordinates. 
	float	PosZ;		// 8  z coord

	int		TexID;		// Texture ID
	float	VelX, VelY,
			VelZ;		// x,y and z Velocity
	float	Intensity;	//Intensity Indicates how bright/large the particle is
	int		Health;		//When Health == 0 the Particle will die
	int		TimeLeft;	//When TimeLeft == 0, Health--;
	float	Step;

	//Pointers
	int (*pUpdate)(struct glParticle *pParticle);
	struct glParticle *pNext;
	struct glParticle *pPrev;

}GL_PARTICLE, *P_GL_PARTICLE;

class tagAPI_OpenGL
{
private:
	//Repeat vars.. used to save time rather than memory
	HWND		Hwnd;			//Handle to our Window
	HGLRC		hRC;			//Rendering Context for openGL
	HDC			hDC;			//Handle to our Device Context

	int			iCmdShow;		//
	HINSTANCE	hInstance;		//HINSTANCE: hinstance of our app
	RECT		rcWindow;
	int			bFullScreen;

	//***OPENGL MEMBER FUNCTIONS***
	int				ChangeResolution(int w, int h, int bitdepth);
	void			SetDefaultSettings();
	int				SetGLPixelFormat(int BitDepth, int ZDepth);
	int				LoadMainTextures();

	//****TEXT ENGINE MEMBER FUNCTIONS****
	P_TEXTOBJ		CreateTextObjectNode(P_TEXTOBJ pTextPos);
	P_TEXTOBJ		DeleteTextObjectNode(P_TEXTOBJ pTextPos);
	P_TEXTOBJ		CreateTextObject(float PosX,	float PosY,	char *String,
								U32 TypeFlags, U32 ColorFlags);
	
	//****PARTICLE ENGINE MEMBER FUNCTIONS****
	void			glCleanUpParticleEngine();
	P_GL_PARTICLE	CreateParticle (P_GL_PARTICLE pParticlePos);
	P_GL_PARTICLE	DeleteParticle(P_GL_PARTICLE pParticlePos);
	
	//****STANDARD TEXT MEMBER FUNCTIONS****
	void			GLDrawChar0(const int	TexID, int x, int y, int Color, char character);
	void			GLDrawChar1(const int	TexID, int x, int y, int Color, char character);

public:

	~tagAPI_OpenGL();
	void			GLDrawText	( int	startX, int	startY,U32	Flags,char	*szMsg, ...);
	void			GLDrawText	( RECT *rcClip, U32 Flags, char *szMsg, ... );

	P_GL_PARTICLE	glCreateSpark(	float PosX, float PosY, float PosZ,
									float VelX, float VelY, float VelZ,
									float Intencity, int Health, int TexID);

	P_GL_PARTICLE	glCreateSpiralSpark(float PosX, float PosY, float PosZ,
									  float Intencity, int Health, int TexID);

	//***TEXT ENGINE STUFF***
	int				iTotalTextObjs;
	P_TEXTOBJ		pHeadTextObj;
	P_TEXTOBJ		pTailTextObj;
	P_FONT			pFont;			//pointer to our font struct
	int				UpdateTextObjEngine();

	//***PARTICLE ENGINE STUFF***
	int				iTotalParticles;
	P_GL_PARTICLE	pHeadParticle;
	P_GL_PARTICLE	pTailParticle;
	int				glDrawParticleEngine();
	int				glUpdateParticleEngine();
	//***LIGHT STUFF***
	//LIGHT			Light;

	// member functions
	DWORD WINAPI	RedrawScreen(LPVOID Parm);
	int				Initialize();

	int				InitFullScreen		( int Width, int Height, int BitDepth);
	int				InitWindowedScreen	( int Width, int Height);
	void			SwitchModes			( void );
					
	void			ReleaseTextures		( void );
	void			ResizeViewport		( int width, int height);
	void			SetiCmdShow			( int a)	{iCmdShow=a;}
	void			SethInstance		( HINSTANCE a){hInstance=a;}
	void			SethRC				( HGLRC i)		{hRC=i;}
	void			SethDC				( HDC i)		{hDC=i;}
	void			SetpFont			( P_FONT f)	{pFont = f;}

	// functions that retrieve values
	int				IsFullScreen()	{ return bFullScreen;}
	HWND			getHwnd()		{ return Hwnd;}
	HGLRC			GethRC()		{ return hRC;}
	HDC				GethDC()		{ return hDC;}
	HINSTANCE		GethInstance()	{ return hInstance;}
};

typedef struct tagActionState
{
	int bShoot    :1;
	int bFire     :1;
	int bLeft     :1;
	int bRight    :1;
	int bUp		  :1;
	int bDown	  :1;
} ACTION_STATE, *P_ACTION_STATE;


class tagApp
{
private:
	// windows information		
	int	bActive,
		bPaused,
		bSwitchingMode;
	
	// debug information
	DWORD			dwTimeStamp;		// time App was initilized
	DWORD			dwFrameTime;		// current time
	DWORD			dwFrameCount;		// current time compared to last (frame) time
	DWORD			dwFrames;			// frames per second
	
	float	DegToRad( float angle );

public:
	int				bDebug;				// used to determine if we're in debug mode or not

	tagApp();
	~tagApp();

	void	InitTrig();

	//ANDY
	tagDPClass		*pDP;

	//MAX
	DISTRUCT		*pdistruct;

	// input information
	U32				keyFlags;
	POINT			ptMouse;			// stored mouse coord
	
	P_API_DDRAW		pDDraw;
	P_API_OPENGL	pOpenGL;
	SETTINGS		UserSettings;
	float			Sin[360],
					Cos[360];
	
	// functions that retrieve values
	inline int		IsActive		( void )			{ return bActive; }
	inline int		IsPaused		( void )			{ return bPaused; }
	inline int		IsSwitchingMode	( void )			{ return bSwitchingMode; }
	inline U32		GetKeyFlags		( void )			{ return keyFlags; }
	inline int		GetSwitchingMode( void )			{ return bSwitchingMode; }							
	inline int		GetFrames		( void )			{ return dwFrames; }
	inline int		GetFrameTime	( void )			{ return dwFrameTime; }
	inline int		GetFrameCount	( void )			{ return dwFrameCount; }
//	void			SetKeyboardFlags(UINT message, WPARAM wParam);
	
	inline int		IncFrameCount	( void )			{ return ++dwFrameCount; }
	inline void		SetpDDraw		( P_API_DDRAW p )	{ pDDraw = p;	}
	inline void		SetpOpenGL		( P_API_OPENGL p )	{ pOpenGL = p;	}
	inline int		SetFrames		( int value )		{ return dwFrames = value; }
	inline int		SetFrameTime	( int value )		{ return dwFrameTime = value; }
	inline int		SetFrameCount	( int value )		{ return dwFrameCount = value; }
	inline int		SetActive		( int bIsActive )	{ return bActive = bIsActive; }
	inline int		SetPause		( int bIsPaused )	{ return bPaused = bIsPaused; }
	inline void		SetMousePos		( int x, int y )	{ ptMouse.x = x; ptMouse.y = y; }
	inline int		SetSwitchingMode( int value )		{ return bSwitchingMode = value; }
};			


class tagScore
{
public:
	unsigned long	dwCurrTime;
	int				minutes, seconds;	// Timer vars
	
	unsigned long	Points,				// actual number of points a player has
					CurrClusters,		// Current	number of Clusters
					TotalClusters,		// Total	number of Clusters
					HighestCluster,		// Highest	number of Clusters 

					CurrCombos,			// Current	number of Combos
					TotalCombos,		// Total	number of Combos
					HighestCombo,		// Highest	number of Combos

					CurrLinkers,		// Current	number of Linkers
					TotalLinkers,		// Total	number of Linkers
					HighestLinker;		// Highest	number of Linkers
	
	int				iStreakLevel;
	float			fStreakTime;

	int				bGameFinished;		// Used to deturmine if a game is finished or not

	void ResetAll(){Points			= 0; 
					CurrClusters	= TotalClusters	= HighestCluster = 0;
					CurrCombos		= TotalCombos	= HighestCombo	 = 0;
					CurrLinkers		= TotalLinkers	= HighestLinker	 = 0;
					bGameFinished	= 0;
					dwCurrTime		= minutes		= seconds = 0;
					iStreakLevel	= 0;
					fStreakTime		= 0.0f;	}


	tagScore()	{ ResetAll(); }
	~tagScore()	{ };
	
	//***Streak functions
	void	UpdateStreakFX		( float fDeltaTime );
	void	CreateStreakSparks	( void );

	//***Functions that Add to the score
	void	AddPoints			( unsigned long i)	{ Points += i; }
	void	AddCluster			( unsigned long numClusters);
	void	ResetCurrClusters	( void );
	void	AddCombo			( unsigned long numCombos);
	void	ResetCurrCombos		( void );
	void	AddLinker			( unsigned long numLinkers);
	void	ResetCurrLinkers	( void );

	//***Functions that return the score
	unsigned long	GetPoints()			{return Points;}
	unsigned long	GetNumClusters()	{return TotalClusters;}
	unsigned long	GetNumCombos()		{return TotalCombos;}
	unsigned long	GetNumLinkers()		{return TotalLinkers;}
	
	//***Timer Functions
	void UpdateTimer( int bCountDown );
	void DrawTimer	( int bCountDown );

};


//eli's new includes ( as of 4-11-01 )
#include "timer.h"
#include "./dataBase.h"

class Gem3D;

typedef struct tagGame_Data
{
	POINT			ptMouse;			// stored mouse coord
	U32				GameStateFlags;
	ACTION_STATE	ActionState;		// current action state
	tagScore		Score;				//Scores for 4 players
	int				bCamera;			// 1 == camera on else 0 == off

	//*** Game Input
	char			keysDown[256];		// key input
			
	// *** Multiplayer Game Input
	int				GameOver;
	int				Winner;
	int				CharPic;
	int				ChrIndex;
	int				bLoggingOntoServer;
	char			ChatString[128];
	char			IPAddress[4][4];
	
	//*** Current Character info
	int				iCurrentCharacterButton;
	char			CharacterName[16];
	char			CharacterPassword[16];
	int				iCharacterProfileSlot;
	
	
	//*** Timer
	CTimer			Timer;
	float			m_fCurrentTime;
	float			m_fDeltaTime;
	float			m_fPrevTime;
	
	//*** Database
	CDataBase		DBase;	

	INITGEMDATA		initGemData;	// This contains info about gem settings to be sent for multiplayer setup
	INITGAMEDATA	initGameData;	// This contains info about game settings to be sent for multiplayer setup
	
	
	//Game Data
	Game_Menu*		pGameMenu;
	Board3D*		pBoard;

	MP_OPTIONS		mpOptions;	//multi player options struct
	int				numplayers;

	tagSimplePlayer	simplePlayers[4];

	int				playerId;
	int				numPlayersalive;
	int				multiPlayerHost;
	
	Gem3D*			pPlayerOnGem;
	
	bool			bDrawSwitcher;

	//*** Functions
			tagGame_Data	( void )	{ Clear();	}
			~tagGame_Data	( void )	{ Destroy(); }

	void	Clear			( void );
	void	Destroy			( void );
	void	Update			( void );
	
	void	OnEscapeKey		( void );
	void	OnF1Key			( void );
	void	OnF2Key			( void );
	void	OnF11Key		( void );
	void	OnF12Key		( void );

	void	SetPlayerPosition(int whichPlayer);

} GAME_DATA, *P_GAME_DATA;

// some random functions
void	ClipToViewPort(RECT &clipRect, RECT &viewPort, int &x, int &y);
void	ResetAllGameSettings();

#endif