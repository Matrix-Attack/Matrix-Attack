#ifndef board_h
#define board_h


#define GEMDATA_FILENAME	("data\\gem.dat")

#define	FLAT_BOARD			0x1
#define	CYLINDER_BOARD		0x2
#define	MOEBIUS_BOARD		0x4
#define SPHERE_BOARD		0x8


#define NODRAW_LEFT			0x1
#define NODRAW_RIGHT		0x2
#define NODRAW_TOP			0x4
#define NODRAW_BOTTOM		0x8

#define DRAW_EMPTY			0
#define DRAW_FOXY			1
#define DRAW_VINEGIRL		2
#define DRAW_MILES			3
#define DRAW_NINJA			4

#define PLAYER_ONE			0
#define PLAYER_TWO			1
#define PLAYER_THREE		2
#define PLAYER_FOUR			3


class Board3D
{
	char		boardName[64];
	int			boardType;

	LPDD_SURF	pBackSurf;			// pointer to the back surface

public:

	Player		player;
	char		szStartMsg[64];		// Msg that you get before the game starts
	Camera3D	Camera;
	
	int			numRows;
	int			numCols;
	int			numMoves;

	Gem3D		**mGem;
	Gem3D		**gem;
	
	// functions		
			Board3D				( void )	{ Clear();	 }
			~Board3D			( void )	{ Destroy(); }
	
	void	Clear				( void );
	void	Destroy				( void );
	
	//Eli added these functions
	void	CheckForClusters	( int iNumMatchesAllowed );
	void	ClearChecked		( void );
	//


	int		LoadBoard			( const P_S8 filename, const P_S8 boardname);
	int		Initialize			( const P_S8 filename, const P_S8 boardname);

//	struct tagGameGraphics	InterfaceGameGraphics;
//	struct tagTimer			Timer;
	
	void		LoadSurfaces		( void );
	char*		GetBoardName		( void ) { return boardName; }
	Camera3D*	GetCamera			( void ) { return &Camera; }
	void		GetMeshesForGems	( int detailLevel );
	void		SetPointersBetweenGems( void );
	

//	void		Update_OpenGL		( void );
//	void		Update_DDraw		( void );
	
	void		Draw_OpenGL			( void );
	void		Draw_DDraw			( void );
	
	void		ReleaseSurfaces		( void );
//	int			GetNumSurfaces		( void )	{ return numSurfaces;}
	void		MakeRect			( int x, int y, RECT* rc, unsigned int flag);
	void		TileGameBoard		( void );
	void		DrawCharacter		( int index, int x, int y);
	void		DrawPlayerIcon		( int index, int x, int y);
	void		DrawVitality		( int x, int y, int percenthealth);
	void		DrawStreakBar		( int x, int y, float percentStreak);
	
	void		DisplayMapCursor	( int whichplayer);
	void		DrawPlayerScore		( int index, int x, int y, int score);
	void		DrawAllRects		( void );

//	void		UpdateTimer			( bool bCountdown);
//	void		DrawTimer			( void );

//	void		UpdateCameraSubZero(){Camera[0].Update();}
//	void		UpdatePlayerSubZero(){player[0].Update();}

//	void		UpdatePlayerSubZeroKeyInput(){player[0].HandleKeyInput();}
};



#endif