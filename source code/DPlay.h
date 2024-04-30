#ifndef DPLAY_H
#define DPLAY_H

typedef class tagDPClass DP_CLASS, *pDP_CLASS;

#define DP_MSG_SEND_PLAYER_DETAIL	9
#define DP_MSG_ADD_PLAYER			10
#define DP_MSG_BAD_PROFILE			11
#define DP_MSG_BAD_PW				12
#define DP_MSG_CREATE_PLAYER		13
#define DP_MSG_PLAYER_NAME_USED		14
#define	DP_MSG_DELETE_PROFILE		15
#define	DP_MSG_PROFILE_DELETED		16
#define	DP_MSG_GET_SERVER_STATS		17
#define	DP_MSG_GET_SCORE_DATA		18
#define	DP_MSG_GET_SCORE_DATA_SINGLE_PLAYER		19
#define DP_MSG_START_GAME			20
#define DP_MSG_SETGAME				21
#define DP_MSG_INITGEM				22   
#define DP_MSG_SET_PLAYERDATA		23
#define DP_MSG_SET_PLAYERID			24
#define DP_MSG_SEND_SCORE			25
#define DP_MSG_SEND_NEW_POSITION	26
#define DP_MSG_SEND_NEW_GEM_COLOR	27
#define DP_MSG_SEND_FLIP			28
#define DP_MSG_SEND_SWITCH			29
#define DP_MSG_COMPLETE_SWITCH		30
#define DP_MSG_COMPLETE_FLIP		31
#define DP_MSG_GEM_CLEAR			32
#define DP_MSG_UNSET_LINKER			33
#define DP_MSG_SMACK_PLAYER			34
#define DP_MSG_SEND_VITALITY		35
#define DP_MSG_OH_MY_GOD_HE_TOOK_MY_COMBO_AND_NOW_HES_USING_IT_ON_ME 36
#define DP_MSG_SEND_WINNER			37
#define DP_MSG_REQUEST_FLIP			38
#define DP_MSG_REQUEST_MOVE			39
#define DP_MSG_REQUEST_SWITCH		40
#define DP_MSG_GET_PLAYER_PIC		41
#define DP_MSG_BOARD_UPDATE			42
#define DP_MSG_SWITCH_CONFIRM		43
#define DP_MSG_SWITCH_FINISH		44

#define DP_MSG_WHAT_CHARACTER		100
#define DP_MSG_CHAR_IS				101


#define CHARACTERS_PER_CHATLINE		44
#define CHAT_WINDOW_LINES			30
#define PLAYER_LIST_WINDOW_LINES	30

#define	MAX_CHAT_LINES				25
#define	MAX_STRING_PER_LIST			50
#define MAX_PLAYER_LINES			10

#define SETUP_1						0x1
#define SETUP_2						0x2
#define SETUP_3						0x4
#define SETUP_4						0x8


typedef struct SWITCH_FINISH
{
	int dpMsg;
	int playerId;
	int row1;
	int row2;
	int column1;
	int column2;
	int gemFlags1;
	int gemFlags2;
	int color1[2];
	int color2[2];
	float x1;
	float x2;
	float y1;
	float y2;
} SWITCH_FINISH, *P_SWITCH_FINISH;

typedef struct CONFIRM_SWITCH
{
	int dpMsg;
	int playerId;
	int row1;
	int row2;
	int column1;
	int column2;
	int gemFlags1;
	int gemFlags2;
} CONFIRM_SWITCH, *P_CONFIRM_SWITCH;


typedef class PLAYER_INFO
{
public:
	int				dpMsg;
	int				playerId;
	int				rowGem;
	int				columnGem;
	int				direction;
	int				hitDamage;
	unsigned int	score;
	unsigned int	vitality;
	int				frontColor;
	int				backColor;

	// constructor
	PLAYER_INFO() :	dpMsg(0), playerId(0), rowGem(0), columnGem(0), direction(0),
					hitDamage(0), score(0), vitality(0), frontColor(0), backColor(0)	{}

	// member functions
	inline void	ZeroOut()
	{
		dpMsg		= 0;
		playerId	= 0;
		rowGem		= 0;
		columnGem	= 0;
		direction	= 0;
		hitDamage	= 0;
		score		= 0;
		vitality	= 0;
		frontColor	= 0;
		backColor	= 0;
	}

} PLAYER_INFO, *P_PLAYER_INFO;




typedef struct PLAYER_PIC
{
	int dpMsg;
	int playerId;
	int charId;
} PLAYER_PIC, *P_PLAYER_PIC;

typedef struct tagInitGameData
{
	int  data;
	int  hostId;
	int  numRows;
	int  numCols;
	DPID dpIdHost;
} INITGAMEDATA, *P_INITGAMEDATA;

typedef struct tagInitGemData
{
	int data;
	int iOffset;
	int jOffset;
	unsigned short backColor;
	unsigned short FrontColor;
} INITGEMDATA, *P_INITGEMDATA;

typedef struct PLAYER_WIN
{
	int dpMsg;
	int id;
}PLAYER_WIN, *P_PLAYER_WIN;

typedef struct PLAYER_COMPLETE_SWITCH
{
	int dpMsg;
	int id;
	int RowGem;
	int ColumnGem;	
}PLAYER_COMPLETE_SWITCH, *P_PLAYER_COMPLETE_SWITCH,
 PLAYER_COMPLETE_FLIP, *P_PLAYER_COMPLETE_FLIP,
 PLAYER_LINKER_DONE, *P_PLAYER_LINKER_DONE,
 PLAYER_REQUEST_FLIP, *P_PLAYER_REQUEST_FLIP;

typedef struct PLAYER_REQUEST_SWITCH
{
	int dpMsg;
	int id;
	int RowGem;
	int ColumnGem;	
	int direction;
}PLAYER_REQUEST_SWITCH, *P_PLAYER_REQUEST_SWITCH;

typedef struct PLAYER_GEM_CLEAR
{
   int dpMsg;
   int id;
   int RowGem;
   int ColumnGem;
   int hitDamage;
}  PLAYER_GEM_CLEAR, *P_PLAYER_GEM_CLEAR;


typedef struct PLAYER_SWITCHING
{
	int dpMsgData;
	int id;
	int RowGem;
	int ColumnGem;
	int direction;
}PLAYER_SWITCHING, *P_PLAYER_SWITCHING;

typedef struct PLAYER_FLIP
{
	int dpMsgData;
	int id;
	int RowGem;
	int ColumnGem;
}PLAYER_FLIP, *P_PLAYER_FLIP;

typedef struct PLAYER_ID_MSG
{
	int dpMsgData;
	int id;
	int numplayers;
}PLAYER_ID_MSG, *P_PLAYER_ID_MSG;


typedef struct PLAYER_UPDATE_SCORE
{
	int dpMsgData;
	int id;
	unsigned int score;
}PLAYER_UPDATE_SCORE, *P_PLAYER_UPDATE_SCORE;


typedef struct PLAYER_UPDATE_VITALITY
{
	int dpMsgData;
	int id;
	unsigned int vitality;
}PLAYER_UPDATE_VITALITY, *P_PLAYER_UPDATE_VITALITY;


typedef struct PLAYER_UPDATE_POSITION
{
	int dpMsgData;
	int pId;
	int RowGem;
	int ColumnGem;
}PLAYER_UPDATE_POSITION, *P_PLAYER_UPDATE_POSITION;


typedef struct PLAYER_UPDATE_GEM
{
	int dsMsgData;
	int id;
	int frontColor;
	int backColor;
	int	RowGem;
	int ColumnGem;
} PLAYER_UPDATE_GEM, *P_PLAYER_UPDATE_GEM;

typedef struct CONNECTION_NODE
{
	CONNECTION_NODE *pNext;
	LPDPNAME     pDpName;
	LPVOID		 lpConnection;
	char		 szName[200];
} CONNECTION_NODE, *P_CONNECTION_NODE;



typedef struct CONNECTIONS_HEAD
{
	HWND hwnd;
	P_CONNECTION_NODE pStart;
	int numnodes;
} CONNECTION_HEAD, *P_CONNECTION_HEAD;

typedef struct SESSION_NODE
{
	LPGUID			pGuid;
	SESSION_NODE	*pNext;
	char			szSessionName[40];
} SESSION_NODE, *P_SESSION_NODE;


typedef struct SESSION_HEAD
{
	int				numnodes;
	P_SESSION_NODE  pStart;
} SESSION_HEAD, *P_SESSION_HEAD;

typedef class tagPlayerListNode
{
public:
	tagPlayerListNode(){pNext=NULL;}
	class tagPlayerListNode*	pNext;
	DPID			dpid;
	int				charIcon;
	char			name[40];
	DWORD			flags;
}PLAYER_NODE, *pPLAYER_NODE;

typedef class tagPlayerList
{
public:
	pPLAYER_NODE	pListHead;
	tagPlayerList(){pListHead=NULL;}
	~tagPlayerList(){ClearList();}
	char *GetPlayerName(DPID);
	void AddPlayer(DPID,const char*,DWORD);
	void RemovePlayer(DPID);
	void ClearList();
	void SetCharIcon(DPID a, int b);
	void DisplayPlayerList();
}PLAYER_LIST, *pPLAYER_LIST;

typedef class tagChannelListNode
{
public:
	tagChannelListNode(){pNext=NULL;}
	class tagChannelListNode*	pNext;
	DPID			dpid;
	char			name[40];
	DWORD			flags;
}CHANNEL_NODE, *pCHANNEL_NODE;

typedef class tagChannelList
{
public:
	pCHANNEL_NODE	pListHead;
	tagChannelList(){pListHead=NULL;}
	~tagChannelList(){ClearList();}
	char *GetPlayerName(DPID);
	void AddPlayer(DPID,const char*,DWORD);
	void RemovePlayer(DPID);
	void ClearList();
	void DisplayPlayerList();
}CHANNEL_LIST, *pCHANNEL_LIST;

typedef class tagChatNode
{
public:
	tagChatNode(){pNext=NULL;}
	class tagChatNode*	pNext;
	int		numlines;
	char*	string;
}CHAT_NODE, *pCHAT_NODE;

typedef class tagChatList
{
	pCHAT_NODE	pListHead;
	int			listlength;
public:
	tagChatList(){pListHead=NULL;listlength=0;}
	~tagChatList(){ClearList();}
	void AddChatString(char *);
	void RemoveString();
	void ClearList();
	void DisplayChatList();
}CHAT_LIST, *pCHAT_LIST;

typedef struct tagPlayerDetail
{
	int		type;
	DPID	id;
	int		character;
	char	name[40];
	char	pw[40];
}PLAYER_DETAIL, *pPLAYER_DETAIL;

typedef struct tagString{
	long	id;
	char	data[1000];
}DATA_STRUCT;

class tagDPClass
{
public:
	int		flag;
	int		hostflag;
	int     gotAllNames;
	char	NewChannelName[40];
	HANDLE	hPlayerEvent;
	HANDLE	hKillThread;
	HANDLE	hMessageThread;
	DWORD	idReceiveThread;

	tagDPClass(HWND Hwnd);
	~tagDPClass();
	int EnumerateSessions();
	void ClearSessionList();
	int CreateTCPConnection(char *IP_Address);
	SESSION_HEAD	 SessionHead;
	void CreateRoom(char* szName);

	DPID	dpid;			//THIS IS MY DPlay ID
	DPID	dpidRoom;		//THIS IS THE ID of THE GROUP CHAT OR GAME (USE THIS FOR GAME PLAY)
	int		numPlayers;		//number of players in the current room
	DPID	dpidRootRoom;	//THIS IS THE ID OF THE MAIN CHAT LOBBY
	DPID	LobbyHostID;	//THIS IS THE ID OF THE LOBBY HOST SESSION
	DPID	dpIdGameHost;	// THIS IS THE ID OF THE HOST FOR THE GAME

	char	szPassword[40];
	char	szShortName[40];
	BOOL	bIsHost;
	HWND GetHwnd(){return hwnd;}
	void	Receive_Msg();
	int		JoinSession();
	int		CreatePlayer(char* szName,BOOL);
	void	Handle_Game_Message (	HWND hwnd, 
									LPDPMSG_GENERIC lpMsg, 
									DWORD MsgSize,
									DPID idFrom, DPID idTo);
	void	Handle_System_Message(	HWND hwnd,
									LPDPMSG_GENERIC lpMsg, 
									DWORD dwMsgSize,
									DPID idFrom, DPID idTo);

	LPDIRECTPLAY3A		lpDP;
	PLAYER_LIST			PlayerList;
	CHAT_LIST			ChatList;
	CHANNEL_LIST		ChannelList;
	pCHANNEL_NODE		pCurChannel;
	BOOL				bNewPlayer;
	int					deleteflag;

private:
	HWND				hwnd;
	LPDIRECTPLAYLOBBY2A	lpDPLobby;
	GUID				guid;
	HRESULT				hRet;
	BOOL				gameServer;
	char				session_name[20];
	char				tcp_address[15];
//	DP_Player_List		*dp_player_list;
	HRESULT				InitializeThread();
};


#endif