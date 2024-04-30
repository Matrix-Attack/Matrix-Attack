//
#ifndef	database_h
#define	database_h

/*
//error codes
//password checks
#define	PP_INCORRECT_PASSWORD		0
#define	PP_CORRECT_PASSWORD			1

//load and create checks
#define	PP_USERID_ALREADY_IN_USE	2
#define	PP_PLAYER_NOT_IN_DATABASE	3
#define	PP_PLAYER_LOGGED_ON			4
#define	PP_PLAYER_LOGGED_OFF		5

#define	PROFILE_DIRECTORY			"Data\\Profiles\\"
#define	PROFILE_LIST				"Profiles"

#define	CHARACTER_DIRECTORY			"Data\\CharacterProfiles\\"

#define	RANKING_DIRECTORY			"Data\\Rankings\\"
#define	HIGH_SCORE_FILE				"HighScore"

//main backup directory folders 1-5 are contained within

*/

#define DATABASE_FILENAME	("data\\database.dat")

#define	MAX_PROFILE_NAME	40
#define MAX_RECORDS			16
#define MAX_PROFILES		8

typedef int					s32;
typedef unsigned int		u32;
typedef float				f32;

class CDataBase;

struct SPuzzleStats //needs revision - eli
{
	u32		m_iStagesComplete;
	f32		m_fTotalTimePlayed;
	f32		m_AvgPuzzleTime;
	u32		m_TotalMovesMade;
};

class CRecord
{
public:
	// Profile info
	char	m_strName[MAX_PROFILE_NAME];
	
	// High Scores
	u32		m_iTotalScore;		// 
	f32		m_fLongestStreak;	// longest time board was *active*
	
	u32		m_iTotalCombos;		//
	u32		m_iHighestCombo;	// 
	u32		m_iTotalLinkers;	//
	u32		m_iHighestLinker;	// 
	u32		m_iTotalClusters;	//
	u32		m_iHighestCluster;	// 
	
			CRecord			( void ) { Clear(); }
	void	Clear			( void );
	//
	bool	operator <		( CRecord& rhs ){ return m_iTotalScore < rhs.m_iTotalScore; }
	bool	operator >		( CRecord& rhs ){ return m_iTotalScore > rhs.m_iTotalScore; }
	bool	operator ==		( CRecord& rhs ){ return m_iTotalScore == rhs.m_iTotalScore;}
	
	void	ClearScore		( void );

	// Check Functions
	bool	CheckScore		( u32 i )		{ m_iTotalScore	  += i; return true; } //if( i > m_iTotalScore   ) { m_iTotalScore  = i; return true; } else return false; }
	bool	CheckStreak		( f32 f )		{ if( f > m_fLongestStreak  ) { m_fLongestStreak = f; return true; } else return false; }
	bool	CheckCombo		( u32 i )		{ m_iTotalCombos  ++; if( i > m_iHighestCombo   ) { m_iHighestCombo  = i; return true; } else return false; }
	bool	CheckLinker		( u32 i )		{ m_iTotalLinkers ++; if( i > m_iHighestLinker  ) { m_iHighestLinker = i; return true; } else return false; }
	bool	CheckCluster	( u32 i )		{ m_iTotalClusters++; if( i > m_iHighestCluster ) { m_iHighestCluster= i; return true; } else return false; }

};

class CProfile
{
private:
	// Profile info
	char			m_strName[MAX_PROFILE_NAME];
	u32				m_iCharacterID;		//0-fox,1-vine,2-ninja,3-bundy
	bool			m_bEmpty;			// if empty we can over ride it
	
	// Profile records
	CRecord			m_CurrentRecord;
	CRecord			m_BestRecord;
	
	// TimeTrial Stats
	u32				m_iTotalGamesPlayed;
	
public:
			CProfile			( void )	{ Clear();	 }
			~CProfile			( void )	{ Destroy(); }
	
	void	Clear				( void );
	void	Init				( char* strName, u32 iCharacterID );
	void	Destroy				( void );
	
	void	ClearScore			( void );

	//Get functions
	bool	IsEmpty				( void )	{ return m_bEmpty;		 }
	char*	GetName				( void )	{ return m_strName;		 }
	u32		GetCharacterID		( void )	{ return m_iCharacterID; }
	u32		GetCurrentScore		( void )	{ return m_CurrentRecord.m_iTotalScore; }

	friend class CDataBase;
};

//CDataBase keeps track of all files of player statistics, high scores, items, etc.
class CDataBase
{
private:
	u32			m_iCurGameType;	// 0 == TimeTrial, 1 == Versus
	u32			m_iActiveSlot;
	CProfile*	m_pActiveProfile;
	
	// Record Information
	u32			m_iNumRecords;
	CRecord		m_RecordArray[MAX_RECORDS];
	
	// Profile Information
	u32			m_iNumProfiles;
	CProfile	m_ProfileArray[MAX_PROFILES];
		
public:
			CDataBase			( void )		{ Clear();	 }
			~CDataBase			( void )		{ Destroy(); }
	void	Clear				( void );
	void	Init				( void );
	void	Destroy				( void );
	s32		CreateProfile		( char* strName, u32 iCharacterID );
	bool	DeleteProfile		( u32 iProfileSlot );
	
	bool	LoadDatabase		( char* strFileName );
	bool	SaveDatabase		( char* strFileName );
	
	
	// Check Records after every game.
	bool	CheckRecords		( void );
	
	// Update Active profile functions.
	bool	UpdateScore			( u32 iNewPoints );
	bool	UpdateStreak		( f32 fStreakTime );
	bool	UpdateCombo			( u32 iComboCount );
	bool	UpdateLinker		( u32 iLinkerCount );
	bool	UpdateCluster		( u32 iClusterCount );
	
	// Set functions
	bool	SetActiveProfile	( u32 iSlot );
	bool	SetActiveProfile	( char* strName );
	void	SetCurrentGameType	( u32 iGameType )		{ m_iCurGameType = iGameType; }
	
	// Get functions
	CProfile*	GetActiveProfile( void )				{ return m_pActiveProfile;	}
	u32			GetNumProfiles	( void )				{ return m_iNumProfiles;	}
	CProfile*	GetProfile		( u32 iProfile )		{ return m_ProfileArray + iProfile; }
	
	u32			GetNumRecords	( void )				{ return m_iNumRecords; }
	CRecord*	GetRecord		( u32 iRecord )			{ return m_RecordArray + iRecord;  }
	
};

#endif