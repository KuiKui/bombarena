//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMGLOBALINIT_H__
#define	__BMGLOBALINIT_H__

#include	"BMGlobalInclude.h"
#include	"BMDebug.h"

#include	"BMConsole.h"
#include	"BMClientServer.h"
#include	"BMClient.h"
#include	"BMAnims.h"
#include	"BMWin32.h"
#include	"BMMap.h"
#include	"BMUIOS.h"
#include	"BMEntity.h"
#include	"BMBonus.h"
#include	"BMBombs.h"

//--------------------------------
// Define
//--------------------------------
#define		BM_MODE_MENU_INIT	0
#define		BM_MODE_MENU		1
#define		BM_MODE_GAME_JOIN	2
#define		BM_MODE_GAME_SEVER	3
#define		BM_MODE_INGAME		4

//-- User Config File Delimiter
#define		BMCONFIGBEGIN		"#BEGIN_CONFIG"
#define		BMCONFIGEND			"#END_CONFIG"
#define		BMCONFIGNAME1		"#DEFAULTPLAYER1"
#define		BMCONFIGNAME2		"#DEFAULTPLAYER2"
#define		BMCONFIGNAME3		"#DEFAULTPLAYER3"
#define		BMCONFIGNAME4		"#DEFAULTPLAYER4"

//----------------------------------------------------
//! Structure
//----------------------------------------------------

typedef struct 
{
	STRING		pszField	[255];		//the last field of map
	STRING		pszElement	[255];		//the last element of map

	STRING		*pszNamePlayer	[ BMCLIENT_MAX_ON_MACHIN ];		//the current name for the player
	UBYTE		CRedPlayer		[ BMCLIENT_MAX_ON_MACHIN ];		//the current red color for the player
	UBYTE		CGreenPlayer	[ BMCLIENT_MAX_ON_MACHIN ];		//the current green color for the player
	UBYTE		CBluePlayer		[ BMCLIENT_MAX_ON_MACHIN ];		//the current blue color for the player
	STRING		*pszSprite		[ BMCLIENT_MAX_ON_MACHIN ];		//the current skin for the player

	//pad
	UBYTE		Left			[ BMCLIENT_MAX_ON_MACHIN ];
	UBYTE		Right			[ BMCLIENT_MAX_ON_MACHIN ];
	UBYTE		Up				[ BMCLIENT_MAX_ON_MACHIN ];
	UBYTE		Down			[ BMCLIENT_MAX_ON_MACHIN ];
	UBYTE		Button1			[ BMCLIENT_MAX_ON_MACHIN ];
	UBYTE		Button2			[ BMCLIENT_MAX_ON_MACHIN ];
}CONFIG;

typedef struct
{
	UBYTE		bIsServer;
}BMGAMEINFOS;

//--------------------------------
// Globales
//--------------------------------
extern	UDWORD				BomberMode;

extern	UDWORD				LockFps;			//Fps Wanted
extern	UDWORD				FrameRate;
extern	CONFIG				LocalConfig;

//--------------------------------
// Fonctions
//--------------------------------

extern	UDWORD	BMGlobalStart	(HINSTANCE hInst);
extern	UDWORD	BMGlobalEnd		(void);
extern	UDWORD	BMGlobalUpdate	(void);

extern	SBYTE	BMGlobalLockFPS	(UDWORD FrameRate);

extern	SBYTE	BMWait();

extern	SBYTE	BMQuit();

extern	SBYTE	BMLoadUserConfig	();
extern	SBYTE	BMDeleteUserConfig	();

#endif // __BMGLOBALINIT_H__