#ifndef __BMBONUS_H__
#define	__BMBONUS_H__

#include	"BMGlobalInclude.h"
#include	"BMSprite.h"
#include	"BMEntity.h"

//--------------------------------
// Defines
//--------------------------------

//-- Predefined type don't use for another purpose !

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	SDWORD		Type;					//Type of bonus
	BMENTITY	*Entity;				
	void		*ModDatas;

}BMBONUS;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMBonusMemory;

//--------------------------------
// Globales
//--------------------------------


//--------------------------------
// Fonctions
//--------------------------------

extern	SBYTE			BMBonusStart		();
extern	SBYTE			BMBonusEnd			();
EXPORT_DLL	SBYTE		BMBonusResetAll		();

EXPORT_DLL	SBYTE		BMBonusAdd			(BMBONUS **NewBonus, SDWORD Type, STRING *FileName, STRING *pszPath = NULL);

EXPORT_DLL	SBYTE		BMBonusDelete		(BMBONUS *Bonus);

extern	SBYTE			BMBonusDisplay		(BMBONUS *Bonus);

EXPORT_DLL	SBYTE		BMBonusSetCallBack	(CALLBACKFUNCTION Function);

#endif

