#ifndef __BMGAME_H__
#define __BMGAME_H__

#include	"BMGlobalInclude.h"
#include	"BMPlayer.h"
#include	"BMAnims.h"
#include	"BMMap.h"
#include	"BMWin32.h"

//--------------------------------
// Define
//--------------------------------
#define		BMMAP_DESTRUCTIBLE			"."
#define		BMMAP_UNDESTRUCTIBLE		"O"
#define		BMMAP_NORMAL				"-"

#define		BMMAP_DEST					'.'
#define		BMMAP_UNDEST				'O'
#define		BMMAP_NORM					'-'
//--------------------------------
// Structures
//--------------------------------

//--------------------------------
// Globales
//--------------------------------
extern	BMANIM			**BMGamePlayerAnimation;	//Array of the player animation

//--------------------------------
// Functions
//--------------------------------
extern	SBYTE			BMGameStart			();
extern	SBYTE			BMGameEnd			();


extern	SBYTE			BMGameLoadSkin		(STRING *pszFile, UDWORD numberofanim);


#endif
