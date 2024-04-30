#ifndef gamemenu_h
#define gamemenu_h


class Game_Menu;


//Move the background so that it is sliding downward and left.

#define DEFAULT_CAM_VELOCITY_X			1		//number of pixels per second
#define DEFAULT_CAM_VELOCITY_Y			-1		//number of pixels per second

// these represent indices of an array of neighbors
// to the left, right, above, and below a particular button
#define BUTTON_UP						0
#define BUTTON_DOWN						1
#define BUTTON_LEFT						2
#define BUTTON_RIGHT					3
#define BUTTON_PARENT					4

//Amount of Menu surfaces and Game Surfaces are now in the App.h file
//#define NUM_OF_MENU_SURF				9
#define NUM_OF_MENU_BUTTONS				80

//Amount of Game Buttons (ie. COMBO!!!!! x2)
#define NUM_OF_GAME_BUTTONS				22

#define CLUSTER_BUTTON					0
#define COMBO_BUTTON					1
#define LINKER_BUTTON					2

// these are menu IDs for different types of menus
#define MENU_ID_TITLESCREEN						0
#define MENU_ID_SINGLE_PLAYER_LOGIN				1
#define MENU_ID_SINGLE_GAME_SELECT				2
#define MENU_ID_MULTI_IP_LOGIN					3
#define MENU_ID_MULTI_PLAYER_LOGIN				4
#define MENU_ID_MULTI_CHAT_SCREEN				5
#define MENU_ID_SINGLE_CHAR_SELECT				6
#define MENU_ID_MULTI_GAME_LAUNCH				7
#define MENU_ID_CREATE_CHANNEL					8
												
#define	MENU_ID_SERVER_SCORES					9
#define	MENU_ID_PLAYER_SCORES					10
#define MENU_ID_CHANNEL_LIST					11
												
#define MENU_ID_SINGLE_PUZZLE_SCREEN			12
#define MENU_ID_SINGLE_PUZZLE_PASSWORD_ENTRY	13
#define	MENU_ID_PLAYER_SCORES_SINGLE_PLAYER		14

#define MENU_ID_CREDITS_SCREEN					15

// these are button IDs for the single player login menu
#define	BUTTON_ID_NEW_PLAYER			0
#define	BUTTON_ID_LOAD_PLAYER			1
#define	BUTTON_ID_DELETE_PLAYER			2
#define	BUTTON_ID_KEY_CONFIG			3
#define	BUTTON_ID_CANCEL				4
#define BUTTON_ID_SLOT1					5
#define BUTTON_ID_SLOT2					6
#define BUTTON_ID_SLOT3					7
#define BUTTON_ID_SLOT4					8
#define BUTTON_ID_SLOT5					9
#define BUTTON_ID_GAMEPAD				10
#define BUTTON_ID_MOVE_UP				11
#define BUTTON_ID_MOVE_DOWN				12
#define BUTTON_ID_MOVE_LEFT				13
#define BUTTON_ID_MOVE_RIGHT			14
#define BUTTON_ID_SMACK					15
#define BUTTON_ID_FLIP					16
#define BUTTON_ID_SWITCH				17
#define BUTTON_ID_PAUSE					18
#define BUTTON_ID_UP_LIGHTBEAM			19
#define BUTTON_ID_DOWN_LIGHTBEAM		20
#define BUTTON_ID_LEFT_LIGHTBEAM		21
#define BUTTON_ID_RIGHT_LIGHTBEAM		22
#define BUTTON_ID_SMACK_LIGHTBEAM		23
#define BUTTON_ID_FLIP_LIGHTBEAM		24
#define BUTTON_ID_SWITCH_LIGHTBEAM		25
#define BUTTON_ID_PAUSE_LIGHTBEAM		26
#define BUTTON_ID_SINGLE_PLAYER			27
#define BUTTON_ID_MULTI_PLAYER			28
#define BUTTON_ID_QUIT					29
#define BUTTON_ID_TIME_TRIAL			30
#define BUTTON_ID_PUZZLE				31
#define BUTTON_ID_STATS					32
#define BUTTON_ID_CANCEL_GAME_SELECT	33
#define BUTTON_ID_CANCEL_INPUT_IP		34
#define BUTTON_ID_CHANNEL_LIST			35
#define BUTTON_ID_CREATE_CHANNEL		36
#define BUTTON_ID_GAMES_LIST			37
#define BUTTON_ID_CREATE_GAME			38
#define BUTTON_ID_SERVER_STATS			39
#define BUTTON_ID_PLAYER_STATS			40
#define BUTTON_ID_PLAYER_INPUT			41
#define BUTTON_ID_IP_INPUT				42
#define BUTTON_ID_IP_ERROR				43
#define BUTTON_ID_SELECT_FOX			44
#define BUTTON_ID_SELECT_VINE			45
#define BUTTON_ID_SELECT_NINJA			46
#define BUTTON_ID_SELECT_MILES			47
#define BUTTON_ID_NAME_ENTRY			48
#define BUTTON_ID_NEW_PLAYER_PW_ENTRY	49
#define BUTTON_ID_LEAVE_CHAT			50
#define BUTTON_ID_LOAD_PLAYER_PW_ENTRY	51


// this is a function pointer to some action that a button can perform
// (these actions are found in button.cpp)
class Menu_Button;
typedef void (*ButtonAction)(Game_Menu	*pGameMenu, Menu_Button *pButton, int direction);


// these are flags representing possible button states
#define BUTTON_FLAG_ENABLED					0x01
#define BUTTON_FLAG_SELECTED				0x02
#define BUTTON_FLAG_ACTIVE_ENABLED			0x04
#define BUTTON_FLAG_ACTIVE_DISABLED			0x08
#define BUTTON_FLAG_DRAW_TEXT1				0x10
#define BUTTON_FLAG_DRAW_TEXT2				0x20

#define FOX_CHICK		0
#define VINE_CHICK		1
#define NINJA_DUDE		2
#define BUNDY			3		


class Menu_Button
{
	friend class	Game_Menu;
	

	float			fTimeWaiting;

	U32				buttonFlags;	
	int				buttonID;
	char			textString[256];
	int				character;		//0-4 for character
public:
	bool			bUpdate;
	POINT			textOffset,		// position of the text (offset from button position)
					screenPos,		// current pixel coordinates
					enabledPos,		// location of the button when enabled
					disabledPos,	// location of the button when disabled
					cursorPos;		// this is where we display the cursor when this
									// button is selected

	float			screenX,		// actual screen coordinates
					screenY,
					velocX,			// button's velocity
					velocY,
					enableDelay,	// amount of time until enabling/disabling occurs
					disableDelay;	// amount of time until enabling/disabling occurs

	RECT			clipRect,		// current clipping rectangle
					clipSelect,		// clipping rectangle when button is selected
					clipDeselect;	// clipping rectangle when button is not selected



	ButtonAction	pSelect,		// pointer to an action when button is first selected
					pDeselect,		// pointer to an action when button is deselected
					pUpdate,		// pointer to an action while button is updating
					pLaunch;		// pointer to an action when button


	Menu_Button		*pNeighbor[5];

//	LPDD_SURF		*pSurf;			// surface to clip from
	int				surfIndex;		// surface Index for either the DDraw surface or the GL texture id.
	// member functions
	Menu_Button();

	void		Enable	( float wait );
	void		Disable	( float wait );
	void		Update	( float fDeltaTime );

	void		Select(Game_Menu	*pGameMenu, Menu_Button *pButton, int direction);
	void		Deselect(Game_Menu	*pGameMenu, Menu_Button *pButton, int direction);
	void		ClipAgainstScreenEdges();
	
	void		UpdatePos	( float fDeltaTime );
	void		UpdateVeloc	( void );

	// functions that get values
	RECT		GetClipRect()			{ return clipRect; }
	RECT		GetSelectRect()			{ return clipSelect; }
	Menu_Button* GetNeighbor(int index)	{ return pNeighbor[index]; }
	POINT		GetScreenPos()			{ return screenPos; }
	POINT		GetTextOffset()			{ return textOffset; }
	POINT		GetEnabledPos()			{ return enabledPos; }
	int			GetCharacterIndex()		{ return character; }
	void		SetCharacterIndex(int a){ character = a; }
	char*		GetTextString()			{ return textString; }
	int			GetWidth()				{ return clipSelect.right - clipSelect.left; }
	int			GetButtonID()			{ return buttonID;}
//	LPDD_SURF	GetpSurf()				{ return *pSurf; }
	void		SetClipSelect(RECT rc)	{ clipSelect = rc; }		// clipping rectangle when button is selected
	void		SetClipDeselect(RECT rc){clipDeselect = rc; }	// clipping rectangle when button is not selected

	// functions that set values
	void	ActiveEnable()
				{	buttonFlags |=  BUTTON_FLAG_ACTIVE_ENABLED | BUTTON_FLAG_ENABLED;
					buttonFlags &= ~BUTTON_FLAG_ACTIVE_DISABLED; }

	void	ActiveDisable()
				{	buttonFlags |=  BUTTON_FLAG_ACTIVE_DISABLED;
					buttonFlags &= ~(BUTTON_FLAG_ENABLED | BUTTON_FLAG_ACTIVE_ENABLED); }

	void SetNeighbor(int index, Menu_Button *pButton)
				{ pNeighbor[index] = pButton; }

	int		IsEnabled()					{ return buttonFlags & BUTTON_FLAG_ENABLED; }
	int		IsDisabled()				{ return !(buttonFlags & BUTTON_FLAG_ENABLED); }
	int		IsActiveEnabled()			{ return buttonFlags & BUTTON_FLAG_ACTIVE_ENABLED; }
	int		IsActiveDisabled()			{ return buttonFlags & BUTTON_FLAG_ACTIVE_DISABLED; }
	int		IsSelected()				{ return buttonFlags & BUTTON_FLAG_SELECTED; }
	float	GetDisableDelay()			{ return disableDelay; }

	void	SetScreenPos(POINT &pos)	{ screenPos = pos; }
	void	SetCursorPos(POINT &pos)	{ cursorPos = pos; }
	void	SetClipRect(RECT &rect)		{ clipRect = rect; }
	void	SetVelocity(float x, float y)		{ velocX = x; velocY = y; }
	void	SetTextString(const char *string)	{ strcpy(textString, string); }
	void	SetDisabledPos(int x, int y) {disabledPos.x = x; disabledPos.y = y;}
	void	SetEnabledPos(int x, int y) {enabledPos.x = x; enabledPos.y = y;}
	void	SetDisableDelay(float d)	{disableDelay = d;}
	
	void	OrbuttonFlags(U32 f){ buttonFlags |= f;}
};

// button actions 
//***Title Screen functions
void	LaunchSinglePlayerLoginButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchMultiPlayerLoginButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchQuitGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchCreditsButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateCreditsButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

//***Single Player Login Screen
void	SelectNewPlayerButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectNewPlayerButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

//***Single Player Login Screen
void	SelectLoadCharacterButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectLoadCharacterButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	SelectKillCharacterButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectKillCharacterButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	SelectKeyConfigButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectKeyConfigButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateLoadCharacterIcon			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectLoadCharacterIcon		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	SelectGamePadIcon				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	DeselectGamePadButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateLightBeamButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchNewPlayerButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchLoadPlayerButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchErasePlayerButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchKeyConfigButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchPlayerSlotButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

//***GameSelect Screen functions
void	LaunchGameSelectCancelButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchSinglePuzzleGameSelectButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchSingleNewPuzzleButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void    LaunchSingleEnterPasswordPuzzleButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchSinglePuzzlePasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateSinglePuzzlePasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchSingleTimeTrialButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

//***Multiplayer IP Login Screen
void	UpdateEnterIPButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateErrorIPButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchEnterIPButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdatePlayerChatBox				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

//***Functions that multiple Buttons use
void	LaunchCancelButton				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void	UpdateEnterNameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateEnterPasswordButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchEnterNewPasswordButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchEnterLoadPasswordButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void	LaunchTextEntry					(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchEnterErasePasswordButton	(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchCreateGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateCreateGameButton			(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void	LaunchOption1Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchOption2Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchOption3Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchOption4Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchOption5Button				(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchCreateChannelPasswordButton(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchCreateChannelButton		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void	LaunchDisplayServerStats		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateDisplayServerStats		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	LaunchDisplayPlayerStats		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);
void	UpdateDisplayPlayerStats		(Game_Menu *pGameMenu, Menu_Button *pButton, int direction);

void	UpdateDisplayPlayerStatsSinglePlayer(	Game_Menu	*pGameMenu,	Menu_Button	*pButton,	int			direction);
void	GoBack(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);

void	LaunchPlayerStatsSinglePlayer(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);


void	LaunchServerStats				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);

void	LaunchPlayerStats				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);

void	LaunchMultiStart				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);
void	LaunchChannelList				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);
void	UpdateChannelList				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);
void	LaunchNextButton				(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);
void	LaunchOkChannelButton			(Game_Menu *pGameMenu,	Menu_Button *pButton,	int direction);
void	ResetAllCharacterButtons		(Game_Menu *pGameMenu);
void	SavePlayerNames					(Menu_Button *pButton);
#define CURSOR_MOVE_UP			0
#define CURSOR_MOVE_DOWN		1
#define CURSOR_MOVE_LEFT		2
#define CURSOR_MOVE_RIGHT		3
#define CURSOR_MOVE_NOWHERE		4

#define CURSOR_FLAG_CAN_MOVE	0x1
#define CURSOR_FLAG_MOVING		0x2
#define CURSOR_FLAG_BLINKING	0x4
#define CURSOR_FLAG_FLASH_ON	0x8
#define CURSOR_FLAG_CAN_SELECT	0x10

class Menu_Cursor
{
	friend class Game_Menu;


	int				blinkTimer,
					blinkSpeed;

	POINT			screenPos,		// pixel coordinates
					blinkPos;		// pixel coordinates of the blinking cursor

	float			screenX,		// actual screen coordinates
					screenY,
					velocX,			// cursor's velocity
					velocY;

	RECT			clipRect;

	Menu_Button		*pButton;		// pointer to the currently selected button

public:
	U32				cursorFlags;

	Menu_Cursor();
	int	IsBlinking()				{ return cursorFlags & CURSOR_FLAG_BLINKING; }


	// functions that set values
	void	SetScreenPos(POINT &pos)				{ screenPos = pos; }
	void	SetClipRect(RECT rect)					{ clipRect = rect; }
	void	SetCursorFlag(int flag)					{ cursorFlags |= flag; }
	void	UnsetCursorFlag(int flag)				{ cursorFlags &= ~flag; }
	void	SetButton(Menu_Button *pMenuButton)	{ pButton = pMenuButton; }
	Menu_Button	*GetpButton()						{ return pButton; }

};

// delete these flags
//#define LAST_MENU_TITLE			0x1
//#define LAST_MENU_SINGLE_LOGIN	0x2
//#define LAST_MENU_MULTI_LOGIN	0x4

class Game_Menu
{

	U32				activeSurfaces;
	int				menuID,
	//				numSurfaces,
					iNumButtons;

	POINT			camPos;			// position of the camera (so we can scroll the menu)

	float			camScreenX,		// actual screen coordinates
					camScreenY,
					camVelocX,		// camera's velocity
					camVelocY,
					camAngle;		// angle the camera is traveling

	LPDD_SURF		pBackSurf;		// array of pointers to surfaces
	
	ButtonAction	ButtonActionArray[16];	//Maximum of 16 Actions
	
public:
	int				iBackgroundTexID;
	int				bLoser;			//save the puzzle win/loser state

	Menu_Cursor		cursor;
	Menu_Button		*buttonArray;	// array of buttons

	Game_Menu();
	~Game_Menu();
	void ProcessMultiplayerStart();
//	LPDD_SURF		*surfaceArray;	// array of pointers to surfaces

	// member functions
	void	FPSButton		( int bOn );
	
	void	DrawGameButtons();
	void	DrawPlayerStats();

	void	FinishGame		( bool bLooser );

	void	Update();

	void	Draw_GameMode();
	void	Draw_MenuMode();

	void	DrawMenuButtons();

	void	DisplayCursor_DDraw();
	void	DisplayCursor_OpenGL();
	void	DrawBackground_DDraw();
	void	DrawBackground_OpenGL();
	void	HandleKeyInput();
	void    DoInput();		
	void	Initialize(int whichMenu);
	void	InitializeButtons();
	void	InitializeCursor(int ButtonID);
	void	SwitchMenu(int whichMenu);
	void	LoadSurfaces();
	void	ReleaseSurfaces();
	
	void	UpdateCamPos	( float fTimeDelta );
	void	UpdateCamVeloc();
	
	void	DrawCursor();
	
	void	UpdateCursorPos();
	void	UpdateCursorVeloc();
//	int		GetNumSurfaces(){return numSurfaces;}
	int		GetMenuID(){return menuID;}
	int		GetNumButtons(){return iNumButtons;}

	char	TempName[128];
	int		TempCharIcon;

	void	InitGameButtons();
	void	UpdateGameButtons();
	void	LoadFromFileGameButtons();

	// member functions
	void SetBlinkingCursor()
				{	cursor.cursorFlags |= CURSOR_FLAG_BLINKING;
					cursor.blinkTimer	= cursor.blinkSpeed;
					cursor.blinkPos		= cursor.pButton->cursorPos; }
	void UnsetBlinkingCursor()
				{	cursor.cursorFlags &= ~CURSOR_FLAG_BLINKING; }


	// functions that get values
	Menu_Button*	GetButtonArray()		{ return buttonArray; }
	Menu_Cursor*	GetCursor()				{ return &cursor; }
	LPDD_SURF		GetBackSurface()		{ return pBackSurf; }
//	LPDD_SURF		GetSurface(int index)	{ if(surfaceArray)return surfaceArray[index];else return NULL; }

	void DrawBackground_OpenGL_InGame();
};


#endif