#include <windows.h>
#include <cassert>
#include <ddraw.h>
#include <dplay.h>
#include <dplobby.h>
#include <fstream>

#include "./mmgr.h"

using namespace std;

#include "DPlay.h"
//#include "database.h"
#include "app.h"
#include "dxerror.h"
#include "utility.h"
#include "gamemenu.h"

#include "vector.h"
#include "model.h"
#include "gem3d.h"
#include "camera3d.h"
#include "player.h"
#include "board.h"

#include "text.h"


// Need the following for using later versions
#define  IDIRECTPLAY2_OR_GREATER

extern APP			App;
extern GAME_DATA	GameData;

//***********************************************************************
// Function: EnumSessionsCallback
//
// Purpose:  DirectPlay session enumeration callback
//
// Parameters: HLPCDPSESSIONDESC2 lpThisSD - Description of session being enumerated
//			   LPDWORD lpdwTimeOut - Timeout value for enumeration
//			   DWORD dwFlags	   - DPDESC_TIMEDOUT if enumeration timed out, 0 otherwise
//			   LPVOID lpContext    - User passed in pointer to data
//
// Returns: WINAPI, FALSE to stop enumeration, TRUE to continue
//
// Last Modified:  Date 10/29/99      Author - Max Szlagor / Andy Kaplan    
//
//***********************************************************************
BOOL WINAPI EnumSessionsCallback(LPCDPSESSIONDESC2 lpThisSD, LPDWORD lpdwTimeOut,
					 DWORD dwFlags, LPVOID lpContext)
{
	P_SESSION_HEAD   pSessionHead = (P_SESSION_HEAD)lpContext;
	P_SESSION_NODE	 pNode = NULL;

	if(dwFlags & DPESC_TIMEDOUT)
	{
		delete App.pDP;
		App.pDP = NULL;

		for(int i=0 ; i<NUM_OF_MENU_BUTTONS ; i++)
		{
			GameData.pGameMenu->buttonArray[i].Disable(0);
		}

		GameData.pGameMenu->buttonArray[69].Enable(0);

		GameData.pGameMenu->SwitchMenu(MENU_ID_TITLESCREEN);

		//clean up ip info
		GameData.ChrIndex			= 0;
		GameData.IPAddress[0][0]	= NULL;
		GameData.IPAddress[1][0]	= NULL;
		GameData.IPAddress[2][0]	= NULL;
		GameData.IPAddress[3][0]	= NULL;
		return FALSE;
	}

	// First allocate space for the node
//	pNode = (P_SESSION_NODE)calloc(1, sizeof(SESSION_NODE) );
	pNode = (P_SESSION_NODE) malloc ( sizeof(SESSION_NODE) );
	memset ( pNode, 0, sizeof( SESSION_NODE ) );
	
	// Now allocate space for the DPLAY name struct
	pNode->pGuid = (GUID*) malloc (sizeof(GUID) );
	
	memcpy(pNode->pGuid, &lpThisSD->guidInstance, sizeof(GUID) );
	
	if(lpThisSD->lpszSessionNameA)
		strcpy(pNode->szSessionName, lpThisSD->lpszSessionNameA);
	
	//set next pointer
	pNode->pNext = pSessionHead->pStart;

	//reset session head pointer
	pSessionHead->pStart = pNode;

	//increment the session count
	pSessionHead->numnodes++;

	GameData.pGameMenu->buttonArray[42].Disable(0);
	GameData.pGameMenu->buttonArray[43].Disable(2);
	GameData.pGameMenu->buttonArray[34].Disable(4);
	GameData.pGameMenu->SwitchMenu(MENU_ID_MULTI_PLAYER_LOGIN);


	//clean up ip info
	//GameData.ChrIndex			= 0;
	//GameData.IPAddress[0][0]	= NULL;
	//GameData.IPAddress[1][0]	= NULL;
	//GameData.IPAddress[2][0]	= NULL;
	//GameData.IPAddress[3][0]	= NULL;

	return FALSE;
}


//***********************************************************************
// Function: tagDPClass::ClearSessionList
//
// Purpose:  Clean up for session list
//
// Parameters: None
//
// Returns: No Return
//
// Last Modified:  Date 10/29/99      Author - Andy Kaplan    
//
//***********************************************************************
void tagDPClass::ClearSessionList()
{
	P_SESSION_NODE   pTemp;

	while(SessionHead.pStart)
	{
		pTemp = SessionHead.pStart;

		SessionHead.pStart = SessionHead.pStart->pNext;

		if(pTemp)
		{
			if(pTemp->pGuid)
			{
				free(pTemp->pGuid);
				pTemp->pGuid = NULL;
			}
			free(pTemp);
			pTemp = NULL;
		}
	}
}

	
//***********************************************************************
// Function: tagDPClass::~tagDPClass()
//
// Purpose:  Destructor for the DirectPlay
//
// Parameters: none
//
// Returns: none
//
// Last Modified:  Date 10/29/99      Author - Andy Kaplan   
//
//***********************************************************************
tagDPClass::~tagDPClass()
{
	if (hMessageThread)
	{
		// wake up receive thread and wait for it to quit
		SetEvent(hKillThread);
		WaitForSingleObject(hMessageThread, INFINITE);

		CloseHandle(hMessageThread);
		hMessageThread = NULL;
	}

	if (hKillThread)
	{
		CloseHandle(hKillThread);
		hKillThread = NULL;
	}

	ClearSessionList();

	//clean up player
	if(dpid)
	{
		lpDP->DestroyPlayer(dpid);
		lpDP->Close();
		dpid=0;
	}

	//clean up lobby
	if(lpDPLobby)
	{
		lpDPLobby->Release();
		lpDPLobby	= NULL;
	}

	//clean up dp object
	if(lpDP)
	{
		lpDP->Release();
		lpDP		= NULL;
	}

	if (hPlayerEvent)
	{
		CloseHandle(hPlayerEvent);
		hPlayerEvent = NULL;
	}

	CoUninitialize();
}

//***********************************************************************
// Function: tagDPClass::tagDPClass(HWND Hwnd)
//
// Purpose:  Constructor for the DirectPlay class - initializes the fields
//			 of the class
//
// Parameters: HWND hwnd - handle to the application window
//
// Returns: void
//
// Last Modified:  Date 10/29/99      Author - Max Szlagor / Andy Kaplan     
//
//***********************************************************************
tagDPClass::tagDPClass(HWND Hwnd)
{
	// Initialize the HWND and the guid for this application
	hwnd = Hwnd;
	guid.Data1 = 0x8f9c3d00;
	guid.Data2 = 0x7dda;
	guid.Data3 = 0x11d3;
	guid.Data4[0] = 0xa3;
	guid.Data4[1] = 0x84;
	guid.Data4[2] =	0x0;
	guid.Data4[3] =	0x10;
	guid.Data4[4] =	0x5a;
	guid.Data4[5] =	0xa0;
	guid.Data4[6] =	0xbf;
	guid.Data4[7] =	0x91;

	// Initialize the lists for the connections and sessions
	SessionHead.numnodes = 0;
	SessionHead.pStart = NULL;

	//set player ID
	dpid	= 0;
	dpidRoom= 0;
	dpidRootRoom = 0;
	bIsHost = FALSE;
	szPassword[0]	= 0;
	szShortName[0]	= 0;
	deleteflag = 0;
	flag = 0;
	hostflag=0;
	bNewPlayer = FALSE;
	numPlayers=0;
	gotAllNames = 0;

	//initialize handles
	hPlayerEvent	= NULL;
	hKillThread		= NULL;
	hMessageThread	= NULL;
	idReceiveThread	= NULL;
	NewChannelName[0]	= NULL;
	pCurChannel = NULL;

	//set pointers to NULL
	lpDP = NULL;
	lpDPLobby = NULL;


	//start message thread
	InitializeThread();
}

//***********************************************************************
// Function: tagDPClass::EnumerateSessions
//
// Purpose:  Enumerates sessions for the DPlay object 
//		     
// Parameters: none
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
int tagDPClass::EnumerateSessions()
{
	DPSESSIONDESC2	 dpSessDesc;

	ZeroMemory(&dpSessDesc, sizeof(DPSESSIONDESC2));
	dpSessDesc.dwSize = sizeof(DPSESSIONDESC2);
	dpSessDesc.guidApplication = guid;

	// Now enumerate all the service provider connections
	hRet = lpDP->EnumSessions(	&dpSessDesc, 0, 
								(LPDPENUMSESSIONSCALLBACK2)(EnumSessionsCallback), 
								&SessionHead, 0);

	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Error enumerating sessions.",
			       "Enumeration error", MB_OK);
		DisplayDPlayError(hwnd, hRet);
		LogErrorToFile("Error enumerating sessionns.");
		return 1;
	}

	return 0;
}

//***********************************************************************
// Function: tagDPClass::JoinSession
//
// Purpose: Joins an existing  
//		     
// Parameters: None
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan    
//
//***********************************************************************
int tagDPClass::JoinSession()
{
	if(SessionHead.pStart)
	{
		DPSESSIONDESC2 dpSes;

		ZeroMemory( &dpSes, sizeof( dpSes ) );
		dpSes.dwSize = sizeof(dpSes);
		dpSes.guidInstance = *(SessionHead.pStart->pGuid);
		dpSes.guidApplication = guid;

		// Now try and join the session with the specified structure
		hRet = lpDP->Open(&dpSes, DPOPEN_JOIN);

		if(FAILED(hRet) )
		{
			DisplayDPlayError(hwnd, hRet);
			LogErrorToFile("Error creating session");
			return 1;
		}
		return 0;
	}
	else
	{
		//couldn not connect to host
		//MessageBox(hwnd, "Could not connect to Lobby", "ERROR", MB_OK);
		GameData.pGameMenu->buttonArray[69].Enable(0);
		return 1;
	}
} 

//***********************************************************************
// Function: tagDPClass::CreateTCPConnection
//
// Purpose:  Creates a TCP/IP Connection or sets up app as host
//		     
//
// Parameters: IP_Address, char*, either "" for host 
//				or "xxx.xxx.xxx.xxx" for client
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
int tagDPClass::CreateTCPConnection(char *IP_Address)
{
	LPDIRECTPLAYLOBBYA			lpdpLobby_old = NULL;
	DPCOMPOUNDADDRESSELEMENT	dpAddress[2];
	DWORD						AddressSize=0;
	LPVOID						lpConnection = NULL;
	
	//initialize COM object
	CoInitialize(NULL);

	//create dplay object
	if(CoCreateInstance(CLSID_DirectPlay, NULL,
						CLSCTX_INPROC_SERVER, IID_IDirectPlay3A,
						(LPVOID*)&lpDP)!=S_OK)
	{
		//cocreateinstance failed
		CoUninitialize();
		return 0;
	}

	//create lobby
	DirectPlayLobbyCreate(NULL, &lpdpLobby_old, NULL, NULL, 0);

	//query interface
	lpdpLobby_old->QueryInterface(	IID_IDirectPlayLobby2A, 
									(LPVOID*)&lpDPLobby);

	//dump old interface
	lpdpLobby_old->Release();

	//fill in address info
	dpAddress[0].guidDataType	= DPAID_ServiceProvider;
	dpAddress[0].dwDataSize		= sizeof(GUID);
	dpAddress[0].lpData			= (LPVOID)&DPSPGUID_TCPIP;

	dpAddress[1].guidDataType	= DPAID_INet;
	dpAddress[1].dwDataSize		= strlen(IP_Address)+1;
	dpAddress[1].lpData			= IP_Address;

	//get size of address
	lpDPLobby->CreateCompoundAddress(dpAddress, 2, NULL, &AddressSize);

	//make room for the address
	lpConnection = malloc(AddressSize);

	//create the address
	lpDPLobby->CreateCompoundAddress(dpAddress, 2, lpConnection, &AddressSize);

	//initialize the connection
	hRet = lpDP->InitializeConnection(lpConnection, 0);

	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Error Initializing Connection", "DPLAY ERROR", MB_OK);
		DisplayDPlayError(hwnd, hRet);
		LogErrorToFile("Error Initializing Connection");
		return 1;
	}
	//free memory
	free(lpConnection);

	return 1;
}

//***********************************************************************
// Function: tagDPClass::CreatePlayer
//
// Purpose:  Creates a DirectPlay Player instance
//		     
// Parameters: szName player's frinedly name
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
int tagDPClass::CreatePlayer(char* szName, int type)
{
	DPNAME	name;
	DPID	id;
	
	ZeroMemory(&name, sizeof(DPNAME));
	name.dwSize = sizeof(DPNAME);
	name.lpszShortNameA = szName;
	name.lpszLongNameA = NULL;

//	szShortName = (char*)malloc(sizeof(szName)+1);
	strcpy(szShortName, szName);

	//set new player flag
	if(type==0)
		bNewPlayer = FALSE;
	else if(type==TRUE)
		bNewPlayer = TRUE;
	else if(type==2)
		deleteflag = TRUE;

	hRet = lpDP->CreatePlayer(&id, &name, hPlayerEvent, NULL, 0, 0);

	if(FAILED(hRet) )
	{
		MessageBox(hwnd, "Error CreatePlayer", "DPLAY ERROR", MB_OK);
		DisplayDPlayError(hwnd, hRet);
		LogErrorToFile("Error CreatePlayer");
		return 1;
	}

	//save local player ID
	dpid = id;

	return 1;
}

//***********************************************************************
// Function: tagDPClass::CreateRoom
//
// Purpose:  Creates a lobby room
//		     
// Parameters: szName player's frinedly name
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagDPClass::CreateRoom(char* szName)
{
	DPNAME	dpName;
	DWORD	dwFlags = 0;
	DPID	temp;

	// build a dpname structure
	ZeroMemory(&dpName, sizeof(DPNAME));
	dpName.dwSize = sizeof(DPNAME);
	dpName.lpszShortNameA = szName;
	dpName.lpszLongNameA = NULL;

	//clear the player list and rebuild
	numPlayers=0;
	PlayerList.ClearList();
	ChatList.ClearList();

	//create our new room
	lpDP->CreateGroupInGroup(dpidRootRoom, &temp, &dpName, NULL, 0, dwFlags);
	lpDP->DeletePlayerFromGroup(dpidRoom, dpid);
	lpDP->AddPlayerToGroup(temp, dpid);
	dpidRoom = temp;
}

//***********************************************************************
// Function: tagDPClass::Receive_Msg 
//
// Purpose:  Handles receipt and dispatching of DPlay messages
//		     
// Parameters: hwnd, handle to the window
//
// Returns: int - 0 if successful, 1 otherwise
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagDPClass::Receive_Msg ()
{
	DPID	idFrom, idTo;
	LPVOID	lpMsgBuff=NULL;
	DWORD	MsgBuffSize=0;
	DWORD	count;

	//get the number of messages in the queue
	lpDP->GetMessageCount(dpid, &count);

	if(count==0)
		return;

	do
	{
		do
		{
			idFrom = 0;
			idTo = 0;

			//get size of buffer needed
			hRet = lpDP->Receive(&idFrom, &idTo, 0, lpMsgBuff, &MsgBuffSize);

			if(hRet == DPERR_BUFFERTOOSMALL)
			{
				//dump ond buffer
				if(lpMsgBuff)
					free(lpMsgBuff);

				lpMsgBuff = malloc(MsgBuffSize);
			}
		}while(hRet == DPERR_BUFFERTOOSMALL);

		//message is received in buffer
		if(SUCCEEDED(hRet) && (MsgBuffSize >= sizeof(DPMSG_GENERIC)))
		{
			if(idFrom == DPID_SYSMSG)
			{
				//deal with system messages here
				Handle_System_Message(	GetHwnd(),
										(LPDPMSG_GENERIC)lpMsgBuff, 
										MsgBuffSize,
										idFrom, idTo);
			}
			else
				//deal with game messages here
				Handle_Game_Message(	GetHwnd(),
										(LPDPMSG_GENERIC)lpMsgBuff, 
										MsgBuffSize, 
										idFrom, idTo);
		}
	}while (SUCCEEDED(hRet));

	//clean up
	if(lpMsgBuff)
		free(lpMsgBuff);
}

//***********************************************************************
// Function: tagDPClass::Handle_Game_Message
//
// Purpose:  Handles game specific messages
//		     
// Parameters: hwnd, lpMsg, MsgSize, idFrom, and idTo
//
// Returns: void
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagDPClass::Handle_Game_Message (HWND hwnd, 
									 LPDPMSG_GENERIC lpMsg, 
									 DWORD MsgSize,
									 DPID idFrom, DPID idTo)
{
	switch(lpMsg->dwType)
	{

	case DP_MSG_START_GAME:
		{
			GameData.GameStateFlags |= GAME_STATE_VERSUS_MULTI;
			App.pDP->flag |= SETUP_1;
			GameData.GameOver = 0;
			GameData.Winner = 100;
		}
		break;


	case DP_MSG_SETGAME:
		{
			INITGAMEDATA InitGameData;

			memcpy(&InitGameData, lpMsg, sizeof(INITGAMEDATA) );

			GameData.pBoard->numCols = InitGameData.numCols;
			GameData.pBoard->numRows = InitGameData.numRows;

			// allocate memory for all the gems
			if(GameData.pBoard->mGem == NULL)
			{
				GameData.pBoard->mGem	= new Gem3D*[InitGameData.numRows];
				for(int i = 0; i < InitGameData.numRows; i++)
					GameData.pBoard->mGem[i]	= new Gem3D[InitGameData.numCols];
			}
			else
			{
				for(int i = 0; i < InitGameData.numRows; i++)
					delete[] GameData.pBoard->mGem[i];
				delete[] GameData.pBoard->mGem;
				
				GameData.pBoard->mGem	= new Gem3D*[InitGameData.numRows];
				for(i = 0; i < InitGameData.numRows; i++)
					GameData.pBoard->mGem[i]	= new Gem3D[InitGameData.numCols];
			}

			App.pDP->dpIdGameHost = InitGameData.dpIdHost;
			GameData.multiPlayerHost = InitGameData.hostId;

			App.pDP->flag |= SETUP_2;
		}
		break;

	case DP_MSG_GET_PLAYER_PIC:
		{
			PLAYER_PIC plPic;
			static int numinitialized = 0;

			memcpy(&plPic, lpMsg, sizeof(PLAYER_ID_MSG) );

			GameData.simplePlayers[plPic.playerId].character = plPic.charId;

			if( numinitialized == (GameData.numplayers - 1) )
			{
				App.pDP->gotAllNames = 1;
			}
		}
		break;


	case DP_MSG_INITGEM:
		{
			INITGEMDATA InitGemData;
			static int numinitialized = 0;

			memcpy(&InitGemData, lpMsg, sizeof(INITGEMDATA) );

			GameData.pBoard->mGem[InitGemData.iOffset][InitGemData.jOffset].activeColor[0] = InitGemData.FrontColor;
			GameData.pBoard->mGem[InitGemData.iOffset][InitGemData.jOffset].activeColor[1] = InitGemData.backColor;

			numinitialized++;
			if((numinitialized == (GameData.pBoard->numRows * GameData.pBoard->numCols)) && App.pDP->flag & SETUP_2)
			{
				App.pDP->flag |= SETUP_3;
			}
		}
		break;


	// This message sets a player id of 1 through 3 for the current player,
	// the host is id 0
	case DP_MSG_SET_PLAYERID:
		{
			PLAYER_ID_MSG Id;

			memcpy(&Id, lpMsg, sizeof(PLAYER_ID_MSG) );
			GameData.playerId = Id.id;
			GameData.numplayers = Id.numplayers;
			GameData.numPlayersalive = GameData.numplayers;

			// Notify the host of our player picture	
			PLAYER_PIC plPic;

			plPic.charId = GameData.CharPic;
			plPic.dpMsg = DP_MSG_GET_PLAYER_PIC;
			plPic.playerId = GameData.playerId;

			App.pDP->lpDP->Send(App.pDP->dpid, App.pDP->dpIdGameHost,DPSEND_GUARANTEED,&plPic,
								sizeof(PLAYER_PIC));
		}
		break;


	case DP_MSG_SET_PLAYERDATA:
		{
			struct tagSimplePlayer spData;

			memcpy(&spData, lpMsg, sizeof(struct tagSimplePlayer) );

			GameData.simplePlayers[spData.id].RowGem = spData.RowGem;
			GameData.simplePlayers[spData.id].ColumnGem = spData.ColumnGem;
			GameData.simplePlayers[spData.id].vitality = spData.vitality;
			GameData.simplePlayers[spData.id].score = spData.score;
			GameData.simplePlayers[spData.id].id = spData.id;
			if(spData.id != GameData.playerId)
				GameData.simplePlayers[spData.id].character = spData.character;
			else
				GameData.simplePlayers[GameData.playerId].character = GameData.CharPic;
			strncpy(GameData.simplePlayers[spData.id].szName,
				    spData.szName, 20);
		}
		break;

	case DP_MSG_SEND_SCORE:
		{
			PLAYER_UPDATE_SCORE plScore;

			memcpy(&plScore, lpMsg, sizeof(PLAYER_UPDATE_SCORE) );

			GameData.simplePlayers[plScore.id].score = plScore.score;
		}	
		break;

	case DP_MSG_SEND_NEW_POSITION:
		{
			PLAYER_UPDATE_POSITION PlayerPos;

			memcpy(&PlayerPos, lpMsg, sizeof(PLAYER_UPDATE_POSITION) );

			GameData.simplePlayers[PlayerPos.pId].ColumnGem = PlayerPos.ColumnGem;
			GameData.simplePlayers[PlayerPos.pId].RowGem = PlayerPos.RowGem;
			
			GameData.SetPlayerPosition(PlayerPos.pId);
		}
		break;



	case DP_MSG_SEND_FLIP:
		{
			PLAYER_FLIP plFlip;

			memcpy(&plFlip, lpMsg, sizeof(PLAYER_FLIP) );
		
			GameData.pBoard->gem[plFlip.RowGem][plFlip.ColumnGem].PrepareFlip(plFlip.id, 0);
		}
		break;


	case DP_MSG_COMPLETE_FLIP:
		{
			PLAYER_COMPLETE_FLIP plCFlip;

			memcpy(&plCFlip, lpMsg, sizeof(PLAYER_COMPLETE_FLIP) );

			GameData.pBoard->gem[plCFlip.RowGem][plCFlip.ColumnGem].CompleteFlip(plCFlip.id);	

		}break;


	case DP_MSG_SEND_NEW_GEM_COLOR:
		{
			PLAYER_UPDATE_GEM pUGem;

			memcpy(&pUGem, lpMsg, sizeof(PLAYER_UPDATE_GEM) );

			Gem3D	*pGem = GameData.pBoard->gem[pUGem.RowGem] + pUGem.ColumnGem;
			if(pGem)
			{
				pGem->activeColor[0] = pGem->color[0] = pUGem.frontColor;
				pGem->activeColor[1] = pGem->color[1] = pUGem.backColor;
				pGem->PrepareFlip(pUGem.id, 0);
			}

		
		}
		break;
	
	
	
	case DP_MSG_SEND_SWITCH:
		{
			PLAYER_SWITCHING plSwitch;
			
			memcpy(&plSwitch, lpMsg, sizeof(PLAYER_SWITCHING) );
			
			GameData.pBoard->gem[plSwitch.RowGem][plSwitch.ColumnGem].PrepareSwitch(plSwitch.id, plSwitch.direction, 8.0f);
		}
		break;
		


		
	case DP_MSG_COMPLETE_SWITCH:
		{
			PLAYER_COMPLETE_SWITCH plCSwitch;

			memcpy(&plCSwitch, lpMsg, sizeof(PLAYER_COMPLETE_SWITCH) );

			GameData.pBoard->gem[plCSwitch.RowGem][plCSwitch.ColumnGem].CompleteSwitch(plCSwitch.id);
		}
		break;
		



	case DP_MSG_GEM_CLEAR:
		{
			PLAYER_GEM_CLEAR plGClear;

			memcpy(&plGClear, lpMsg, sizeof(PLAYER_GEM_CLEAR) );

			GameData.pBoard->gem[plGClear.RowGem][plGClear.ColumnGem].UnsetStateFlags(~GEM_FLAG_LINKER_FLAGS);
			GameData.pBoard->gem[plGClear.RowGem][plGClear.ColumnGem].AddStateFlags(GEM_FLAG_CLEARING);

			if(   (GameData.simplePlayers[GameData.playerId].ColumnGem ==  plGClear.ColumnGem)
			   && (GameData.simplePlayers[GameData.playerId].RowGem	   ==  plGClear.RowGem) )
			{
				GameData.simplePlayers[GameData.playerId].vitality -= plGClear.hitDamage;
				
				PLAYER_UPDATE_VITALITY plUVit;

				plUVit.dpMsgData = DP_MSG_SEND_VITALITY;
				plUVit.id = GameData.playerId;
				plUVit.vitality = GameData.simplePlayers[GameData.playerId].vitality;

				App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&plUVit,
								    sizeof(PLAYER_UPDATE_VITALITY));
			}
		}
		break;

	case DP_MSG_SEND_VITALITY:
		{
			PLAYER_UPDATE_VITALITY plUVit;

			memcpy(&plUVit, lpMsg, sizeof(PLAYER_UPDATE_VITALITY) );

			GameData.simplePlayers[plUVit.id].vitality = plUVit.vitality;

			if(GameData.simplePlayers[plUVit.id].vitality <= 0)
			{
				// Another one bites the dust
				GameData.numPlayersalive--;
				
				if(GameData.simplePlayers[GameData.playerId].vitality > 0 && GameData.numPlayersalive == 1)
				{
					PLAYER_WIN plWin;
					plWin.dpMsg = DP_MSG_SEND_WINNER;
					plWin.id = GameData.playerId;

					App.pDP->lpDP->Send(App.pDP->dpid,App.pDP->dpidRoom,DPSEND_GUARANTEED,&plWin,
								    sizeof(PLAYER_WIN));

					GameData.GameOver = 1;
					GameData.Winner = plWin.id;
				}
			}
		}
		break;

		
	case DP_MSG_SEND_WINNER:
		{
			PLAYER_WIN plWin;

			memcpy(&plWin, lpMsg, sizeof(PLAYER_WIN) );

			GameData.GameOver = 1;
			GameData.Winner = plWin.id;
		}
		break;
		

	case DP_MSG_UNSET_LINKER:
		{
			PLAYER_LINKER_DONE plLinkDone;

			memcpy(&plLinkDone, lpMsg, sizeof(PLAYER_LINKER_DONE) );

			GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].activeColor[0] = GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].color[0];
			GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].activeColor[1]	= GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].color[1];
	//		UnsetStateFlags(GEM_FLAG_LINKER_FLAGS);
			GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].UnsetStateFlags(~GEM_FLAG_SWITCHING);
			GameData.Score.ResetCurrLinkers();
			GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].SetFaceColors(GEM_FRONT_HALF);
			GameData.pBoard->gem[plLinkDone.RowGem][plLinkDone.ColumnGem].SetFaceColors(GEM_BACK_HALF);	
		}
		break;


	case DP_MSG_BOARD_UPDATE:
		{
			unsigned int *data = (unsigned int*)lpMsg;

			for(int i = 0; i < GameData.pBoard->numRows ; i++)
				for(int j = 0; j < GameData.pBoard->numCols; j++)
				{
					GameData.pBoard->gem[i][j].color[0] = (*(data+i*GameData.pBoard->numCols+j+1) & 0x7);
					GameData.pBoard->gem[i][j].color[1] = (*(data+i*GameData.pBoard->numCols+j+1) >> 3) & 0x7;
					GameData.pBoard->gem[i][j].stateFlags =  (*(data+i*GameData.pBoard->numCols+j+1) >> 6);
				}
		}
		break;


	case DP_MSG_GET_SCORE_DATA:
		{
//			DATA_STRUCT	data;

			//*FIX*
//			memcpy(&data,lpMsg,sizeof(DATA_STRUCT));
//			GameData.Data=(char*)malloc(1000);
//			strcpy( GameData.Data, data.data );
		}
		break;

	case DP_MSG_PROFILE_DELETED:
		{
			//clean up multiplayer name list
			lpDP->DestroyPlayer(dpid);
			lpDP->Close();
			dpid=0;
		}
		break;
	case DP_MSG_SEND_PLAYER_DETAIL:
		//send a message type DP_MSG_ADD_PLAYER and include a 
		//pPLAYER_DETAIL structure
		PLAYER_DETAIL Temp;

		//initialize variables
		if(bNewPlayer)
		{
			Temp.type	= DP_MSG_CREATE_PLAYER;
			Temp.character = GameData.CharPic;
		}
		else
			Temp.type	= DP_MSG_ADD_PLAYER;
		
		if(deleteflag)
		{
			Temp.type	= DP_MSG_DELETE_PROFILE;
			deleteflag = 0;
		}

		Temp.id		= dpid;

		if(szShortName[0])
			strcpy(Temp.name,App.pDP->szShortName);

		if(szPassword[0])
			strcpy(Temp.pw,szPassword);
		else
			strcpy(Temp.pw,"PASSWORD");

		//send packet
		lpDP->Send(dpid, idFrom, DPSEND_GUARANTEED, &Temp, sizeof(PLAYER_DETAIL));

		//reset flag
		bNewPlayer = FALSE;

		break;
	case DP_MSG_BAD_PROFILE:
		//destroy player
		lpDP->DestroyPlayer(dpid);
		//dump session
		lpDP->Close();
		//profile does not exist try again or create a new one
		//MessageBox(hwnd, "Profile does not exist.", "ERROR", MB_OK);
		GameData.pGameMenu->buttonArray[70].Enable(0);
		//go back a menu
		GameData.pGameMenu->SwitchMenu(MENU_ID_MULTI_PLAYER_LOGIN);
		//clear the text screen
		GameData.pGameMenu->InitializeCursor(0);
		GameData.pGameMenu->buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
		GameData.pGameMenu->buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
		GameData.pGameMenu->buttonArray[48].Disable(0);
		GameData.pGameMenu->buttonArray[49].Disable(0);

		//reset character icons
		ResetAllCharacterButtons(GameData.pGameMenu);
		break;
	case DP_MSG_BAD_PW:
		//destroy player
		lpDP->DestroyPlayer(dpid);
		//dump session
		lpDP->Close();
		//password is incorrect try again
		//MessageBox(hwnd, "Bad User Password", "ERROR", MB_OK);
		GameData.pGameMenu->buttonArray[68].Enable(0);
		//go back a menu
		GameData.pGameMenu->SwitchMenu(MENU_ID_MULTI_PLAYER_LOGIN);
		//clear the text screen
		GameData.pGameMenu->InitializeCursor(0);
		GameData.pGameMenu->buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
		GameData.pGameMenu->buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
		GameData.pGameMenu->buttonArray[48].Disable(0);
		GameData.pGameMenu->buttonArray[49].Disable(0);

		//reset character icons
		ResetAllCharacterButtons(GameData.pGameMenu);
		break;
	case DP_MSG_PLAYER_NAME_USED:
		//destroy player
		lpDP->DestroyPlayer(dpid);
		//dump session
		lpDP->Close();
		//player name is being used, we need to pick a new one
		//MessageBox(hwnd, "Could not create profile,\n name already used", "ERROR", MB_OK);
		GameData.pGameMenu->buttonArray[67].Enable(0);
		//go back a menu
		GameData.pGameMenu->SwitchMenu(MENU_ID_MULTI_PLAYER_LOGIN);
		//clear the text screen
		GameData.pGameMenu->InitializeCursor(0);
		GameData.pGameMenu->buttonArray[48].GetTextString()[0] = 0; //rid the name entry button
		GameData.pGameMenu->buttonArray[49].GetTextString()[0] = 0; //rid the Newpassword entry button
		GameData.pGameMenu->buttonArray[48].Disable(0);
		GameData.pGameMenu->buttonArray[49].Disable(0);

		//reset character icons
		ResetAllCharacterButtons(GameData.pGameMenu);
		break;

	case DP_MSG_CHAR_IS:
		{
		pPLAYER_DETAIL pDetail = (pPLAYER_DETAIL)lpMsg;

		GameData.CharPic = pDetail->character;
		}

		break;
	}
}

BOOL FAR PASCAL EnumPlayersCallback (	DPID dpId,
										DWORD dwPlayerType,
										LPCDPNAME lpName,
										DWORD dwFlags,
										LPVOID lpContext
									  )
{
	class tagDPClass *dp = (class tagDPClass *) lpContext;
	
	if(dwPlayerType == DPPLAYERTYPE_PLAYER)
	{
		dp->numPlayers++;
		//add this player to the list
		dp->PlayerList.AddPlayer(dpId, lpName->lpszShortNameA, 0);

	}
	else if(dwPlayerType == DPPLAYERTYPE_GROUP)
		//add the group to the list
		dp->ChannelList.AddPlayer(dpId, lpName->lpszShortNameA, 0);

	return TRUE;
}

//***********************************************************************
// Function: tagDPClass::Handle_System_Message
//
// Purpose:  Handles game specific messages
//		     
// Parameters: hwnd, lpMsg, MsgSize, idFrom, and idTo
//
// Returns: void
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagDPClass::Handle_System_Message(	HWND hwnd,
										LPDPMSG_GENERIC lpMsg, 
										DWORD dwMsgSize,
										DPID idFrom, DPID idTo)
{
	LPSTR		lpszStr = NULL;

	//switch on message type
	switch (lpMsg->dwType)
	{
	case DPSYS_STARTSESSION:
		{
			LPDPMSG_STARTSESSION	lp = (LPDPMSG_STARTSESSION) lpMsg;
		}
		break;

	case DPSYS_CREATEPLAYERORGROUP:
        {
            LPDPMSG_CREATEPLAYERORGROUP		lp = (LPDPMSG_CREATEPLAYERORGROUP) lpMsg;
            
        }
		break;

	case DPSYS_DESTROYPLAYERORGROUP:
        {
            LPDPMSG_DESTROYPLAYERORGROUP	lp = (LPDPMSG_DESTROYPLAYERORGROUP)lpMsg;
        }
		break;

	case DPSYS_ADDPLAYERTOGROUP:
	case DPSYS_DELETEPLAYERFROMGROUP:
        {
            LPDPMSG_ADDPLAYERTOGROUP		lpAddMsg = (LPDPMSG_ADDPLAYERTOGROUP)lpMsg;
            LPDPMSG_DELETEPLAYERFROMGROUP	lpDeleteMsg = (LPDPMSG_DELETEPLAYERFROMGROUP)lpMsg;
			DPID							dpidPlayer, dpidGroup;
			LPVOID							lpPlayerName = NULL, lpGroupName = NULL;
			ULONG							size;

			if (lpMsg->dwType == DPSYS_ADDPLAYERTOGROUP)
			{
				dpidPlayer = lpAddMsg->dpIdPlayer;
				dpidGroup = lpAddMsg->dpIdGroup;

				if(dpidPlayer == dpid)
				{
					//we are being added to a group
					if(dpidRootRoom)
						dpidRoom = dpidGroup;
					else
						dpidRoom = dpidRootRoom = dpidGroup;

					//get a list of the players in the group
					hRet = lpDP->EnumGroupPlayers(dpidRoom,SessionHead.pStart->pGuid,EnumPlayersCallback, this, 0);

					hRet = lpDP->EnumGroups(SessionHead.pStart->pGuid,EnumPlayersCallback, this, 0);

					hRet = lpDP->EnumGroupsInGroup(dpidRootRoom,SessionHead.pStart->pGuid,EnumPlayersCallback, this, 0);

					//head to chat screen
					ResetAllCharacterButtons(GameData.pGameMenu);

					GameData.pGameMenu->buttonArray[48].Disable(0); //rid the name entry button
					GameData.pGameMenu->buttonArray[49].Disable(0); //rid the Newpassword entry button
					GameData.pGameMenu->buttonArray[61].Disable(2);
					GameData.pGameMenu->buttonArray[60].Disable(4);

					GameData.pGameMenu->buttonArray[0].Disable(2);
					GameData.pGameMenu->buttonArray[1].Disable(4);
					GameData.pGameMenu->buttonArray[2].Disable(6);
					GameData.pGameMenu->buttonArray[3].Disable(8);
					GameData.pGameMenu->buttonArray[4].Disable(10);

					GameData.pGameMenu->SwitchMenu(MENU_ID_MULTI_CHAT_SCREEN);
					GameData.pGameMenu->InitializeCursor(41);

					PLAYER_DETAIL	Data;

					Data.id = dpid;
					strcpy(Data.name,szShortName);
					Data.type = DP_MSG_WHAT_CHARACTER;

					lpDP->Send(dpid,DPID_ALLPLAYERS,DPSEND_GUARANTEED,&Data, sizeof(PLAYER_DETAIL));
					
				}
				else
				{
					//only add ppl in our group
					if(lpAddMsg->dpIdGroup == dpidRoom)
					{
						//someone is being added to the group
						
						// get size of player name
						hRet = lpDP->GetPlayerName(dpidPlayer, lpPlayerName, &size);

						if(hRet == DPERR_BUFFERTOOSMALL)
						{
							//set buffer
							lpPlayerName = malloc(size);

							//now get name
							hRet = lpDP->GetPlayerName(dpidPlayer, lpPlayerName, &size);
						}

						if FAILED(hRet)
						{
							// A failure may mean that the player has been deleted
							// since we began processing this message
							if(lpPlayerName)
							{
								free(lpPlayerName);
								lpPlayerName = NULL;
							}
						}

						// get size of group name
						hRet = lpDP->GetGroupName(dpidGroup, lpGroupName, &size);

						if(hRet == DPERR_BUFFERTOOSMALL)
						{
							//set buffer
							lpGroupName = malloc(size);

							//now get name
							hRet = lpDP->GetGroupName(dpidGroup, lpGroupName, &size);
						}

						if FAILED(hRet)
						{
							// A failure may mean that the player has been deleted
							// since we began processing this message
							if(lpGroupName)
							{
								free(lpGroupName);
								lpGroupName = NULL;
							}
						}
						else
						{
							PlayerList.AddPlayer(dpidPlayer, (((DPNAME*)lpPlayerName)->lpszShortNameA), 0);
							//MessageBox(hwnd, (const char *)	(((DPNAME*)lpPlayerName)->lpszShortNameA), (const char *)	(((DPNAME*)lpGroupName)->lpszShortNameA), MB_OK);
						}
						if(lpPlayerName)
						{
							free(lpPlayerName);
							lpPlayerName = NULL;
						}
						if(lpGroupName)
						{
							free(lpGroupName);
							lpGroupName = NULL;
						}
					}
				}
			}
			else
			{
				//deal with deleting a player here
				dpidPlayer = lpDeleteMsg->dpIdPlayer;
				dpidGroup = lpDeleteMsg->dpIdGroup;

				if(dpidPlayer != dpid)
					//player has left the group
					PlayerList.RemovePlayer(dpidPlayer);
				else
					dpidRoom = 0;
			}
        }
		break;

	case DPSYS_ADDGROUPTOGROUP:
	case DPSYS_DELETEGROUPFROMGROUP:
        {
            LPDPMSG_ADDGROUPTOGROUP		lpAddMsg = (LPDPMSG_ADDGROUPTOGROUP)lpMsg;
            LPDPMSG_DELETEGROUPFROMGROUP	lpDeleteMsg = (LPDPMSG_DELETEGROUPFROMGROUP)lpMsg;
        }
		break;

	case DPSYS_SESSIONLOST:
        {
            LPDPMSG_SESSIONLOST lp = (LPDPMSG_SESSIONLOST)lpMsg;
        }
		break;

	case DPSYS_HOST:
        {
            LPDPMSG_HOST	lp = (LPDPMSG_HOST)lpMsg;
        }
		break;

	case DPSYS_SETPLAYERORGROUPDATA:
        {
            LPDPMSG_SETPLAYERORGROUPDATA lp = (LPDPMSG_SETPLAYERORGROUPDATA)lpMsg;
        }
		break;

	case DPSYS_SETPLAYERORGROUPNAME:
        {
            LPDPMSG_SETPLAYERORGROUPNAME lp = (LPDPMSG_SETPLAYERORGROUPNAME)lpMsg;
        }
		break;

	case DPSYS_CHAT:
        {
            LPDPMSG_CHAT	lp		=	(LPDPMSG_CHAT)lpMsg;
			DWORD			dwSize	=	lstrlen( lp->lpChat->lpszMessageA ) + 
										lstrlen(PlayerList.GetPlayerName(lp->idFromPlayer)) + 7;
							//Allow extra room for player name and : 

			// allocate space for string
			lpszStr = (LPSTR) malloc( dwSize );

			if (lpszStr == NULL)
				break;
			lstrcpy( lpszStr, "%C3");
			lstrcat( lpszStr, PlayerList.GetPlayerName(lp->idFromPlayer));
			lstrcat( lpszStr, "%C0: " );
			lstrcat( lpszStr, lp->lpChat->lpszMessageA );
			CharUpper( lpszStr);

			ChatList.AddChatString(lpszStr);
		}
		break;
	}
}

//***********************************************************************
// Function: ReceiveThread
//
// Purpose:  Handles game messages from the message string
//		     
// Parameters: LPVOID lpThreadParameter
//
// Returns: DWORD WINAPI
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
DWORD WINAPI ReceiveThread(LPVOID lpThreadParameter)
{
	HANDLE		eventHandles[2];
	tagDPClass *pDPClass = (tagDPClass *)lpThreadParameter;


	eventHandles[0] = pDPClass->hPlayerEvent;
	eventHandles[1] = pDPClass->hKillThread;

	// loop waiting for player events. If the kill event is signaled
	// the thread will exit
	while (WaitForMultipleObjects(2, eventHandles, FALSE, INFINITE) == WAIT_OBJECT_0)
	{
		// receive any messages in the queue
		pDPClass->Receive_Msg();
	}

	ExitThread(0);

	return (0);
}

//***********************************************************************
// Function: tagDPClass::InitializeThread
//
// Purpose:  Initializes the message thread
//		     
// Parameters: None
//
// Returns: HRESULT
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
HRESULT tagDPClass::InitializeThread()
{
	// create event used by DirectPlay to signal a message has arrived
	hPlayerEvent = CreateEvent(	NULL,		// no security
								FALSE,		// auto reset
								FALSE,		// initial event reset
								NULL);		// no name
	if (hPlayerEvent == NULL)
		hRet = DPERR_NOMEMORY;

	// create event used to signal that the receive thread should exit
	hKillThread = CreateEvent(	NULL,		// no security
								FALSE,		// auto reset
								FALSE,		// initial event reset
								NULL);		// no name
	if (hKillThread == NULL)
		hRet = DPERR_NOMEMORY;

	// create thread to receive player messages
	hMessageThread = CreateThread(	NULL,				// default security
									0,				// default stack size
									ReceiveThread,	// pointer to thread routine
									this,			// argument for thread
									0,				// start it right away
									&idReceiveThread);
	if (hMessageThread == NULL)
		hRet = DPERR_NOMEMORY;

	return (hRet);
}

//***********************************************************************
// Function: tagPlayerList::DisplayPlayerList
//
// Purpose:  Displays the list of players in the room to the screen
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagPlayerList::DisplayPlayerList()
{
	pPLAYER_NODE pHead;
	POINT		pt;
	int			i=0;
	long		counter=0;

	if(pListHead)
	{
		pHead	= pListHead;
		pt.x	= 19;
		pt.y	= 57;

		if(!App.UserSettings.RenderMode)
		{
			while((counter < MAX_PLAYER_LINES) && pHead)
			{
				//
				//x,y - start, 44 #num chars to draw, strings first char
				App.pDDraw->DDrawText(
					pt.x, pt.y+(i*13),
					TF_LENGTH_CHAT_INPUT | TF_COLOR_RED,
					pHead->name);
				i++;
				counter++;
				pHead = pHead->pNext;
			}
		}
		else
		{
			while((counter < MAX_PLAYER_LINES) && pHead)
			{
				//
				//x,y - start, 44 #num chars to draw, strings first char
				App.pOpenGL->GLDrawText(
					pt.x, pt.y+(i*13),
					TF_LENGTH_CHAT_INPUT | TF_COLOR_RED,
					pHead->name);
				i++;
				counter++;
				pHead = pHead->pNext;
			}

		}
	}
}

//***********************************************************************
// Function: tagPlayerList::AddPlayer
//
// Purpose:  Adds a new player to the list of players in the room
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagPlayerList::AddPlayer(DPID id,const char* name,DWORD flags)
{
	//if this is not the lobby host
	if(strcmp(name,"Host"))
	{
		pPLAYER_NODE pNewNode = new (PLAYER_NODE);

		//set values
		pNewNode->dpid	= id;
		pNewNode->flags	= flags;

		//set name
		if(strcmp(name,""))
		{
			//pNewNode->name	= new char[strlen(name)+1];
			strcpy(pNewNode->name,name);
		}
		else
		{
			//player has no name
			//pNewNode->name = "NO_NAME";
			strcpy(pNewNode->name,"NO_NAME");
		}

		//link to list
		pNewNode->pNext = pListHead;
		pListHead		= pNewNode;
	}
	else
		//saver server id
		App.pDP->LobbyHostID = id;
}

//***********************************************************************
// Function: tagPlayerList::RemovePlayer
//
// Purpose:  Removes a player from the list of players in the room
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagPlayerList::RemovePlayer(DPID id)
{
	pPLAYER_NODE pPrevNode	= NULL;
	pPLAYER_NODE pCurNode	= pListHead;

	while(pCurNode)
	{
		if(pCurNode->dpid = id)
		{
			//we found the node to remove

			//fix the list
			if(pPrevNode)
			{
				//we are not at head
				pPrevNode->pNext = pCurNode->pNext;
			}
			else
			{
				//we are at the head
				pListHead = pCurNode->pNext;
			}

			//now clean

			//clean up pointer in structure
			//delete [] pCurNode->name;

			//clean up node
			delete pCurNode;

			pCurNode = NULL;
		}
		else
		{
			//save where we are and increment
			pPrevNode = pCurNode;
			pCurNode = pCurNode->pNext;
		}
	}
}

//***********************************************************************
// Function: tagPlayerList::ClearList
//
// Purpose:  Cleans up the list of players
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagPlayerList::ClearList()
{
	while(pListHead)
	{
		//take a copy of the head
		pPLAYER_NODE pTemp = pListHead;

		//now increment the pointer
		pListHead = pListHead->pNext;

		//clean up pointer in structure
		//delete [] pTemp->name;

		//clean up node
		delete pTemp;
	}
}

//***********************************************************************
// Function: tagPlayerList::GetPlayerName
//
// Purpose:  Cleans up the list of players
//		     
// Parameters: DPID id
//
// Returns: char * to the player's name or NULL if not found
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
char * tagPlayerList::GetPlayerName(DPID id)
{
	pPLAYER_NODE	pList = pListHead;

	while(pList)
	{
		if(pList->dpid == id)
			return pList->name;
		pList = pList->pNext;
	}
	return NULL;
}

//***********************************************************************
// Function: tagChatList::AddChatString
//
// Purpose:  Adds a chat string to the list
//		     
// Parameters: char * lpstr
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChatList::AddChatString(char *lpstr)
{
	pCHAT_NODE pNewNode = new (CHAT_NODE);
	int			length	= lstrlen(lpstr);
	pCHAT_NODE	pTemp	=this->pListHead;

	if(length)
	{
		if(this->listlength > MAX_STRING_PER_LIST)
		{
			while(pTemp->pNext)
				pTemp=pTemp->pNext;

			this->listlength-=	pTemp->numlines;

			//
			//delete tail and carry on with adding
			this->RemoveString();
		}

		//set values
		if(length%CHARACTERS_PER_CHATLINE)
		{
			pNewNode->numlines	= length/CHARACTERS_PER_CHATLINE +1;
		}
		else
		{
			pNewNode->numlines	= length/CHARACTERS_PER_CHATLINE;
		}

		//set string pointer
		pNewNode->string = (char*)malloc(length+1);
		strcpy(pNewNode->string,lpstr);

		//increment list length
		listlength += pNewNode->numlines;

		//link to list
		pNewNode->pNext = pListHead;
		pListHead		= pNewNode;
	}
}

//***********************************************************************
// Function: tagChatList::RemoveString
//
// Purpose:  Removes the oldest chat string from the list
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChatList::RemoveString()
{
	pCHAT_NODE pTemp,pTemp2;

	//assign initial pointers
	pTemp = pTemp2= pListHead;

	if(!pTemp)
		return;

	while(pTemp->pNext)
	{
		//save this node
		pTemp2 = pTemp;

		//incrimnet pointer
		pTemp = pTemp->pNext;
	}

	if(pTemp2)
	{
		pTemp=	pTemp2->pNext;

		//free the chat string
		if(pTemp->string)
		{
			free(pTemp->string);
			
			pTemp->string = NULL;
		}

		delete pTemp;

		pTemp2->pNext=	NULL;
	}
}

//***********************************************************************
// Function: tagChatList::ClearList
//
// Purpose:  Cleans up the chat string linked list
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChatList::ClearList()
{
	pCHAT_NODE pTemp;

	while(pListHead)
	{
		//save this node
		pTemp = pListHead->pNext;

		//free the chat string
		if(pListHead->string)
		{
			free(pListHead->string);
			
			pListHead->string = NULL;
		}

		delete pListHead;

		//incrimnet pointer
		pListHead = pTemp;
	}
}

//***********************************************************************
// Function: tagChatList::DisplayChatList
//
// Purpose:  Displays the list of chat strings to the list
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChatList::DisplayChatList()
{
	pCHAT_NODE	pHead;
	int			i=0;
	long		counter=0;

	if(pListHead)
	{
		pHead	= pListHead;

		while(pHead && (counter < (MAX_CHAT_LINES - pHead->numlines)) )
		{
			RECT	rcText;
			
			rcText.left		= 170;
			rcText.top		= 389 - (pHead->numlines*13) - (counter*13);
			rcText.right	= 610;
			rcText.bottom	= 480;//rcText.top + (pHead->numlines*13);

			if(!App.UserSettings.RenderMode)
			{
				App.pDDraw->DDrawText(&rcText, 0, pHead->string);
			}
			else
			{
				//*** OPEN_GL MODE
				App.pOpenGL->GLDrawText(&rcText, 0, pHead->string);
			}

			counter += pHead->numlines;
			pHead = pHead->pNext;
		}
	}

}
//***********************************************************************
// Function: tagChannelList::DisplayPlayerList
//
// Purpose:  Displays the list of players in the room to the screen
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChannelList::DisplayPlayerList()
{
	pCHANNEL_NODE pHead;
	POINT		pt;
	int			i=0;
	long		counter=0;

	if(pListHead)
	{
		pHead	= pListHead;
		pt.x	= 19;
		pt.y	= 57;

		if(!App.UserSettings.RenderMode)
		{
			while((counter < MAX_PLAYER_LINES) && pHead)
			{
				//
				//x,y - start, 44 #num chars to draw, strings first char
				App.pDDraw->DDrawText(
					pt.x, pt.y+(i*13),
					TF_LENGTH_CHAT_INPUT | TF_COLOR_RED,
					pHead->name);
				i++;
				counter++;
				pHead = pHead->pNext;
			}
		}
		else
		{
			while((counter < MAX_PLAYER_LINES) && pHead)
			{
				//
				//x,y - start, 44 #num chars to draw, strings first char
				App.pOpenGL->GLDrawText(
					pt.x, pt.y+(i*13),
					TF_LENGTH_CHAT_INPUT | TF_COLOR_RED,
					pHead->name);
				i++;
				counter++;
				pHead = pHead->pNext;
			}
		}
	}
}

//***********************************************************************
// Function: tagChannelList::AddPlayer
//
// Purpose:  Adds a new player to the list of players in the room
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChannelList::AddPlayer(DPID id,const char* name,DWORD flags)
{
	//if this is not the lobby host
	if(strcmp(name,"Host"))
	{
		pCHANNEL_NODE pNewNode = new (CHANNEL_NODE);

		//set values
		pNewNode->dpid	= id;
		pNewNode->flags	= flags;

		//set name
		if(strcmp(name,""))
		{
			//pNewNode->name	= new char[strlen(name)+1];
			strcpy(pNewNode->name,name);
		}
		else
		{
			//player has no name
			//pNewNode->name = "Genreic Player";
			strcpy(pNewNode->name,"NO_NAME");
		}

		//link to list
		pNewNode->pNext = pListHead;
		pListHead		= pNewNode;
	}
	else
		//saver server id
		App.pDP->LobbyHostID = id;
}

//***********************************************************************
// Function: tagChannelList::RemovePlayer
//
// Purpose:  Removes a player from the list of players in the room
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChannelList::RemovePlayer(DPID id)
{
	pCHANNEL_NODE pPrevNode	= NULL;
	pCHANNEL_NODE pCurNode	= pListHead;

	while(pCurNode)
	{
		if(pCurNode->dpid = id)
		{
			//we found the node to remove

			//fix the list
			if(pPrevNode)
			{
				//we are not at head
				pPrevNode->pNext = pCurNode->pNext;
			}
			else
			{
				//we are at the head
				pListHead = pCurNode->pNext;
			}

			//now clean

			//clean up pointer in structure
			//delete [] pCurNode->name;

			//clean up node
			delete pCurNode;

			pCurNode = NULL;
		}
		else
		{
			//save where we are and increment
			pPrevNode = pCurNode;
			pCurNode = pCurNode->pNext;
		}
	}
}

//***********************************************************************
// Function: tagPlayerList::ClearList
//
// Purpose:  Cleans up the list of players
//		     
// Parameters: None
//
// Returns: None
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
void tagChannelList::ClearList()
{
	while(pListHead)
	{
		//take a copy of the head
		pCHANNEL_NODE pTemp = pListHead;

		//now increment the pointer
		pListHead = pListHead->pNext;

		//clean up pointer in structure
		//delete [] pTemp->name;

		//clean up node
		delete pTemp;
	}
}

//***********************************************************************
// Function: tagPlayerList::GetPlayerName
//
// Purpose:  Cleans up the list of players
//		     
// Parameters: DPID id
//
// Returns: char * to the player's name or NULL if not found
//
// Last Modified:  Date 11/9/99      Author - Andy Kaplan     
//
//***********************************************************************
char * tagChannelList::GetPlayerName(DPID id)
{
	pCHANNEL_NODE	pList = pListHead;

	while(pList)
	{
		if(pList->dpid == id)
			return pList->name;
		pList = pList->pNext;
	}
	return NULL;
}

void tagPlayerList::SetCharIcon(DPID a, int b)
{
	pPLAYER_NODE	pTemp = pListHead;

	while(pTemp)
	{
		if(pTemp->dpid == a)
			pTemp->charIcon = b;

		pTemp = pTemp->pNext;
	}
}
