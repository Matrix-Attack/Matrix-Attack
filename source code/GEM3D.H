#ifndef gem_h
#define gem_h




typedef unsigned short	U16, *P_U16;




#define RED_565			0xF800
#define ORANGE_565		0xFC00
#define YEL_ORANGE_565	0xFE00
#define YELLOW_565		0xFFE0
#define GREEN_565		0x7E0
#define CYAN_565		0x7FF
#define CYAN2_565		0x61F
#define CYAN3_565		0x41F
#define	BLUE_565		0x1F
#define MAGENTA_565		0xF81F
#define LAVENDER_565	0x841F
#define PURPLE_565		0xC01F
#define WHITE_565		0xFFFF





// used to describe the mesh id
#define	GEM_FRONT_HALF					1
#define GEM_BACK_HALF					2

//Detail level
#define	GEM_12POLY_MESH					0
#define GEM_36POLY_MESH					1


#define GEM_NEIGHBOR_UP					0
#define GEM_NEIGHBOR_DOWN				1
#define GEM_NEIGHBOR_LEFT				2
#define GEM_NEIGHBOR_RIGHT				3

#define GEM_NEIGHBOR_UPRIGHT			4
#define GEM_NEIGHBOR_UPLEFT				4
#define GEM_NEIGHBOR_DOWNRIGHT			5
#define GEM_NEIGHBOR_DOWNLEFT			5


#define	GEM_FLAG_SWITCHING				0x1
#define	GEM_FLAG_FLIPPING				0x2
#define GEM_FLAG_READY_TO_CHECK			0x4
#define GEM_FLAG_ALREADY_CHECKED		0x8
#define GEM_FLAG_CLEARING				0x10
#define GEM_FLAG_DISPLAY_2D				0x20
#define GEM_FLAG_LINKER_ENABLED			0x40
#define GEM_FLAG_LINKER_ON				0x80
#define GEM_FLAG_LINKER_FLAGS			(GEM_FLAG_LINKER_ENABLED | GEM_FLAG_LINKER_ON)

#define GEM_FLAG_INC_CLUSTER_COUNT		0x100
#define GEM_FLAG_INC_LINKER_COUNT		0x200
#define GEM_FLAG_INC_COMBO_COUNT		0x400

#define GEM_PLAYERID_NOBODY				100

class Gem3D
{
	friend class Board3D;

public:
	bool		bChecked;				// each gem has a uniqueID
	
	int			bDisplay2D;			// if true, display the gem as a sprite

	LPDD_SURF	pGemSurf,
				pBackSurf;


	float		degreesFlipped;		// number of degrees the gem has rotated during
									// a flip

	int			frameNo,
				yoinkTimeLeft,
				linkerTimeLeft,
				clearTimeLeft;
	
	Vector3D	velocity,			// x-, y-, z-velocity of the gem
				targetPoint;		// when a gem is moving, it seeks out this point

	Gem3D		*pNeighbor[6],		// pointers to neighboring gems
				*pSwitchPartner;	// when a gem is switched, stores the address of
									// the gem being switched with

	Object3D	*pBody;				// contains the actual 3D information


	unsigned int stateFlags;		// describes the current state of the gem
	unsigned int m_iDetailLevel;	// specifies a lo-detail model (fewer polygons)
									// or a hi-detail model (more polygons)

	
	//These are our saviors
	int			bAutoFlipOn;
	int			bStartingAChain;


	int			row;
	int			column;
	int			associatedPlayerID;	// identifies which player this gem is associated with
									// (if value == 0, the gem is associated with nobody)
	U16			color[2],			// color on the front and back sides
				activeColor[2];		// current color to display

	//Eli added these functions
	void		CorrectColor	( int iNumMatchesAllowed );
	int			GetNumMatches	( int iCurNumMatches, int whichSide );
	
	// constructors, copy constructors, deconstructors
				Gem3D			( void ) { Clear();		}
				~Gem3D			( void ) { Destroy();	}
	void		Clear			( void );
	void		Destroy			( void );
	void		Update			( float tDeltaTime );
	void		Init			( float x, float y, float z, 
								  const P_S8 filename, const P_S8 modelName );

	// boolean functions
	int			IsSwitchable		( void )	{ return !(stateFlags) || (stateFlags & GEM_FLAG_LINKER_FLAGS); }
	int			IsFlippable			( void )	{ return !(stateFlags); }
	int			IsSwitching			( void )	{ return stateFlags & GEM_FLAG_SWITCHING; }
	int			IsFlipping			( void )	{ return stateFlags & GEM_FLAG_FLIPPING; }
	int			IsReadyToCheck		( void )	{ return stateFlags & GEM_FLAG_READY_TO_CHECK; }
	int			IsChecked			( void )	{ return stateFlags & GEM_FLAG_ALREADY_CHECKED; }
	int			IsClearing			( void )	{ return stateFlags & GEM_FLAG_CLEARING; }
	int			IsLinkerEnabled		( void )	{ return stateFlags & GEM_FLAG_LINKER_ENABLED; }
	int			IsLinkerOn			( void )	{ return stateFlags & GEM_FLAG_LINKER_ON; }
	int			IsCheckable			( void )	{ return (stateFlags == 0 || stateFlags == GEM_FLAG_READY_TO_CHECK) && !IsFlipping(); }
	int			IsBusy				( void )	{ return stateFlags; }
	int			bIncClusterCount	( void )	{ return stateFlags & GEM_FLAG_INC_CLUSTER_COUNT;}

	int			bIncLinkerCount		( void )	{ return stateFlags & GEM_FLAG_INC_LINKER_COUNT;}
	int			bIncComboCount		( void )	{ return stateFlags & GEM_FLAG_INC_COMBO_COUNT;}
				
	void		Draw2D				( int whichSide );
	void		CreateSparks		( int whichSide );
	
	// Set functions
	void		SetBaseColor		( int whichSide, int Color );
	void		SetFaceColors		( int whichSide);
	void		SetDetailLevel		( int iDetailLevel )			{ m_iDetailLevel= iDetailLevel;		}
	void		SetTargetPoint		( Vector3D &target)				{ targetPoint	= target; }
	void		SetVelocity			( Vector3D &veloc)				{ velocity		= veloc;}
	void		SetNeighbor			( int index, Gem3D *neighbor)	{ pNeighbor[index] = neighbor; }
	void		SetRotation			( float x, float y, float z)	{ pBody->rotation.x	= x; pBody->rotation.y	= y; pBody->rotation.z	= z; }
	void		SetStateFlags		( int flags)					{ stateFlags = flags;	}
	
	void		AddStateFlags		( int flags)					{ stateFlags |= flags;	}
	void		UnsetStateFlags		( int flags)					{ stateFlags &= ~flags; }
	
	// Get functions
	void		GetBaseColor		( int whichSide );
	Vector3D*	GetCenterPos		( void )	{ return &pBody->VertexList[0].worldCur; }
	POINT*		GetScreenCenterPos	( void )	{ return &pBody->VertexList[0].pt2D; }
	
	U16			GetColor			(int side)	{ return side & GEM_FRONT_HALF ? color[0] : color[1]; }
	Gem3D*		GetNeighbor			(int index)	{ return pNeighbor[index]; }
	int			GetStateFlags		( void )	{ return stateFlags; }
	Gem3D*		GetSwitchPartner	( void )	{ return pSwitchPartner; }
	
	
	void			Draw_DDraw();
	void			Draw_OpenGL();
	
	// functions that act on the actual gem
	int				CheckForMatches(int &numMatches, int whichSide );
	void			FlagTheMatches();
	
	// functions related to switching a gem
	void			PrepareSwitch	( int playerID, int direction, float switchSpeed );
	inline void		UpdateSwitch	( float tDelta );
	inline void		CompleteSwitch	( int playerID );
	
	// functions related to flipping a gem
	void			PrepareFlip		( int playerID, float flipSpeed);
	inline void		UpdateFlip		( );
	inline void		CompleteFlip	( int playerID );
	
	
	inline void		UpdateClear		( );
	inline void		UpdateLinker	( int playerID );
};




#endif