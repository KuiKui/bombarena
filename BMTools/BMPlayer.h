#ifndef __BMPLAYER_H__
#define __BMPLAYER_H__

#include	"BMGlobalInclude.h"
#include	"BMAnims.h"
#include	"BMSprite.h"
#include	"BMEntity.h"

//--------------------------------
// Define
//--------------------------------
#define		BMPLAYERMAXNUMBER			8		//Warning don't change this value by a superior !!! (only 3 bits available)

//--flag
#define		BMPLAYERSERVER				0
#define		BMPLAYERCLIENT				1

//--State of player
#define		BMPLAYERSTATEINVALID		(1 << 0)	//invalid player
#define		BMPLAYERSTATEDEAD			(1 << 1)	//the player is dead, waiting for the next begin game
#define		BMPLAYERSTATEISDYING		(1 << 2)	//the player is dying, (playing dead animation)
#define		BMPLAYERSTATESPECTATE		(1 << 3)	//the player is in spectate, (when he entered in a server for example)
#define		BMPLAYERSTATEOKAY			(1 << 4)	//the player is ok !

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	UBYTE		PlayerId;				//Player Id, the same as the index of the player array !
	STRING		*Name;					//Name of the player
	
	SDWORD		PlayerState;			//State of the Player (dead, not valid, ...)
	BMENTITY	*Entity;
	
	SBYTE		IsDead;					
				
	SDWORD		Score;					//Score
	SDWORD		Frag;					//Number of frags done

	//Colors	
	UBYTE		ColorRed;
	UBYTE		ColorGreen;
	UBYTE		ColorBlue;

		//don't forget the callback after the reload

	//Bonus

	//Disease
	
	//...

	void		*ModData;				// Mod specific datas attached to the player

}BMPLAYER;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMPlayerMemory;

//--------------------------------
// Globales
//--------------------------------

extern	UBYTE			BMPlayerOnGame;							//Number of players in the game
extern	BMPLAYER		BMPlayerArray [BMPLAYERMAXNUMBER];		//Array of the player

//--------------------------------
// Fonctions
//--------------------------------
extern	SBYTE				BMPlayerStart		();
extern	SBYTE				BMPlayerEnd			();

extern	SBYTE				BMPlayerAdd			(UBYTE PlayerId, UBYTE CRed, UBYTE CGreen,UBYTE CBlue, STRING *Name, STRING *FileName, STRING *pszPath = NULL);

extern	SBYTE				BMPlayerDelete		(UBYTE PlayerId);

EXPORT_DLL	SBYTE			BMPlayerSetCallBack (CALLBACKFUNCTION Function);

EXPORT_DLL	SBYTE			BMPlayerSetReady	(UBYTE PlayerId);
EXPORT_DLL	SBYTE			BMPlayerIsReady		(UBYTE playerId);

//extern						PlayerKill

//exrern						PlayerUpdate (UDWORD PlayerId);

extern	SBYTE				BMPlayerDisplay		(UBYTE PlayerId);

EXPORT_DLL	SBYTE			BMPlayerGetPtr		(BMPLAYER **ppPlayer, UBYTE Id);

#endif
