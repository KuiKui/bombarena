#ifndef __BMBOMBS_H__
#define	__BMBOMBS_H__

#include	"BMGlobalInclude.h"
#include	"BMSprite.h"
#include	"BMEntity.h"
#include	"BMTimer.h"

//--------------------------------
// Defines
//--------------------------------

#define		BMBOMBS_NORMAL			1
#define		BMBOMBS_TR				2
#define		BMBOMBS_GOUTTE			3
#define		BMBOMBS_JE_T_EMMERDE_KUIKUI	4
#define		BMBOMBS_FLAMES			5

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	SDWORD		Type;					//Type of bombs
	UDWORD		TimeExplode;			//Time when it does explode
	BMENTITY	*Entity;				//Ptr to the entity of this bombs				
	void		*ModDatas;				//Mod specific
}BMBOMBS;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMBombsMemory;

//--------------------------------
// Globales
//--------------------------------


//--------------------------------
// Fonctions
//--------------------------------

extern	SBYTE			BMBombsStart		();
extern	SBYTE			BMBombsEnd			();

EXPORT_DLL	SBYTE		BMBombsAdd			(BMBOMBS **NewBombs, SDWORD Type, UDWORD TimeExplode, STRING *FileName, STRING *pszPath = NULL);
EXPORT_DLL	SBYTE		BMBombsDelete		(BMBOMBS *Bombs);

extern	SBYTE			BMBombsDisplay		(BMBOMBS *Bombs);

EXPORT_DLL	SBYTE		BMBombsSetCallBack	(CALLBACKFUNCTION Function);

EXPORT_DLL	SBYTE		BMBombsResetAll		();
#endif
