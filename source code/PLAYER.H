#ifndef player_h
#define player_h




// player actions
#define PLAYER_MOVE_UP				0
#define PLAYER_MOVE_DOWN			1
#define PLAYER_MOVE_LEFT			2
#define PLAYER_MOVE_RIGHT			3
#define PLAYER_MOVE_IN				4
#define PLAYER_MOVE_OUT				5

#define PLAYER_MOVE_NOWHERE			6

#define PLAYER_SWITCH				0x40

#define MSG_QUEUE_SIZE				64


class Camera3D;
class Gem3D;

class Player
{
	friend class Board3D;

public:
	LPDD_SURF	pBackSurf,
				pGemSurf;

	int			playerID;
	int			cursorDirection;	// current direction of movement

	char		*keyFlags;			// determines which keys are being held down

	int			clusterCount,
				comboCount,
				linkerCount,
				numDiffCombos;		// number of different combos going on at the same time

	int			bCanMove;			// if true, the player can move the cursor
	int			bCanSwitch;			// if true, the player can switch two gems
	int			bCanFlip;			// if true, the player can flip the current gem

	int			bSwitching;			// if true, the player is switching two gems
	int			bFlipping;			// if true, the player is flipping a gem

	int			bRepeatOn;			// if true, the player is holding a movement key down


	int			bFlashOn;			// if true, display the gem's highlight color
									// if false, display the gem's regular color

	float		fMoveTimer;			// amount of time before the next movement can occur

	float		fRepeatStall;		// when a movement key is held down, this is the amount
									// of time before the cursor starts to move repeatedly

	float		fRepeatTime;			// once repeat movement kicks in, this is the amount
									// of time between repeated cursor movements

	int			highlightTimer;		// amount of time remaining until current gem flashes
	int			flashSpeed;			// speed at which the gem flashes

	Gem3D*		pGem;				// pointer to the gem where the player is located

	PLAYER_INFO	messages[64];		// This is the host message queue array
	int			firstMsgIndex;

	Player();

	Camera3D*	pCamera;

	int			playerPic;
	int			switchdirection;

	inline int	GetCursorDirection();

	void		DisplayCursor();

	// functions related to switching gems
	inline int	CanPlayerSwitch();
	inline void	BeginSwitch();
	int			IsDoneSwitching()	{ return bSwitching && !pGem->IsSwitching(); }
	void		CompleteSwitch();


	// functions related to flipping gems
	inline int	CanPlayerFlip();
	inline void	BeginFlip();
	int			IsDoneFlipping()	{ return bFlipping && !pGem->IsFlipping(); }
	void		CompleteFlip();		

	inline int	CanPlayerMove		( float fDeltaTime ) ;
	void		MoveCursor();
	inline void	ResetMoveTimer();


	void		HandleKeyInput(float fDeltaTime);
	void		Initialize(Gem3D *startingPos);
	void		Update();
	inline void	Switch(int direction);

};




#endif