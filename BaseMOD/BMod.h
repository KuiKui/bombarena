#include "../BMTools/BMGlobalInclude.h"

#include "../BMTools/BMClientServer.h"
#include "../BMTools/BMGame.h"

#include "../BMTools/BMBonus.h"
#include "../BMTools/BMBombs.h"
#include "../BMTools/BMSprite.h"
#include "../BMTools/BMTimer.h"
//--------------------------------
// Define
//--------------------------------

//-- init mod
#define		BMOD_START_SPEED		150.0f
#define		BMOD_START_BOMB			1
#define		BMOD_START_FIRE			1


#define		BMOD_BLOCTYPE_FIRE		1

#define		BMOD_NUMBER_ANIMATION_NEEDED	4

//-- state du mod
#define		BMOD_CURRENT_STATE_INGAME		1
#define		BMOD_CURRENT_STATE_OUTGAME		2

//-- player animation definition don't remove, this must be the same in all mods
#define		BMOD_PLAYER_UP			1
#define		BMOD_PLAYER_DOWN		2
#define		BMOD_PLAYER_LEFT		3
#define		BMOD_PLAYER_RIGHT		4
#define		BMOD_PLAYER_UP_STOP		5
#define		BMOD_PLAYER_DOWN_STOP	6
#define		BMOD_PLAYER_LEFT_STOP	7
#define		BMOD_PLAYER_RIGHT_STOP	8

//-- sens des bombes
#define		BMOD_FIRE_UP			1
#define		BMOD_FIRE_DOWN			2
#define		BMOD_FIRE_RIGHT			3
#define		BMOD_FIRE_LEFT			4
#define		BMOD_FIRE_CENTER		5
#define		BMOD_FIRE_BOOM			6
#define		BMOD_FIRE_MASK			15

#define		BMOD_FIRE_IS_END		16

//-- player animation mod specific
//Launch bomb
//Dead #1
//...

//-- Sens
#define		BMOD_UP					1
#define		BMOD_DOWN				2
#define		BMOD_RIGHT				3
#define		BMOD_LEFT				4



//-- Bonus
#define		BMOD_BONUS_FIRE			1

//-- File Delimiter
#define BMMAPBEGIN				"#BEGIN_FILE_DESCRIPTION"
#define BMMAPEND				"#END_FILE_DESCRIPTION"
								
#define	BMSETDIRECTORY			"#SET_DIRECTORY"		//set directory "relative directory"

//--------------------------------
// Structures
//--------------------------------

typedef struct{
	float		WalkSpeed;				//Walk speed
	UBYTE		NumberOfBombs;			//Number of bombs available for this player
	UBYTE		BombLeft;				//Number of bombs left
	SBYTE		ThrowBomb;				//Can the player throwing bomb
	UBYTE		Fire;
}BMODPLAYERDATAS;

//--------------------------------
// Globales
//--------------------------------

extern	SBYTE			BModCurrentState;
extern	BMCLIENTDATAS	*BModClientDatas;
extern	BMPLAYER		*BModPlayerArray;
extern	BMENTITY		**BModEntityArray;
extern	float			BModDTime;
extern	UBYTE			BModEventBuffer		[BMPLAYERMAXNUMBER]; 

//--------------------------------
// Fonctions
//--------------------------------

// init
extern	void	BMModInitPlayer			(UBYTE pid);
extern	void	BModResetPlayer			(void);

// delete
__declspec(dllexport)	UBYTE	BMModDeletePlayer		(void *Player);

// proceed
extern	void	BModUpdateSimpleAction1	(UBYTE pid);
extern	void	BModUpdatePlayer		(UBYTE pid);
extern	void	BModUpdatePos			(UBYTE pid, UBYTE Direction);
extern	void	BModUpdateAnim			(UBYTE pid, UBYTE Direction);
extern	void	BModSetPlayerBlock		(UBYTE pid, UBYTE flag = FALSE);	// FALSE : Up & Left Pos TRUE : MiddleSpritePos
extern	void	BModUpdateInterPos		(UBYTE pid, UBYTE Direction);
extern	void	BModUpdateEntity		();

extern	void	BModRecurseBomb			(UDWORD posx, UDWORD posy);
extern	void	BModDeleteGhost			();
