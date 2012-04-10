#ifndef __BMENTITY_H__
#define	__BMENTITY_H__

#include	"BMGlobalInclude.h"
#include	"BMSprite.h"

//--------------------------------
// Defines
//--------------------------------
#define		BMENTITY_ONMAP			(1 << 0)
#define		BMENTITY_PICKEDUP		(1 << 1)
#define		BMENTITY_ISDEAD			(1 << 2)

//-- Predefined type don't use for another purpose !
#define		BMENTITY_PLAYER			(1 << 0)
#define		BMENTITY_BOMBS			(1 << 1)
#define		BMENTITY_BONUS			(1 << 2)

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	SDWORD		Type;					//Type of entity, Bonus, Bombs, Player
	UDWORD		Flag;					//Flag, ONMAP or not
	BMFPOINT	Pos;
	BMLPOINT	BlockPos;
	void		*EntityInfo;			//ptr to the entity type (bonus *...)
	BMSPRITE	*Sprite;				//ptr to the sprite
	void		*ModData;				//specific mod data
}BMENTITY;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMEntityMemory;

//--------------------------------
// Globales
//--------------------------------


//--------------------------------
// Fonctions
//--------------------------------

extern		SBYTE		BMEntityStart		();
extern		SBYTE		BMEntityEnd			();

extern		SBYTE		BMEntityAdd			(BMENTITY **NewEntity, void *ObjectToLink, STRING *FileName, SDWORD Type);
EXPORT_DLL	SBYTE		BMEntityDelete		(BMENTITY *Entity);

EXPORT_DLL	SBYTE		BMEntitySetBlockPosition(BMENTITY *Entity, float PosX, float PosY, UBYTE flag=TRUE);		
EXPORT_DLL	SBYTE		BMEntitySetRealPosition	(BMENTITY *Entity, float fX, float fY);			

EXPORT_DLL	SBYTE		BMEntityAddFlag		(BMENTITY *Entity,UDWORD Flag);
EXPORT_DLL	SBYTE		BMEntitySubFlag		(BMENTITY *Entity,UDWORD Flag);

extern		SBYTE		BMEntityDisplay		(BMENTITY *Entity);

EXPORT_DLL	SBYTE		BMEntitySetCallBack (CALLBACKFUNCTION Function);

EXPORT_DLL	SBYTE		BMEntityGetBonusPtr (BMNODE **PtrBonus);
EXPORT_DLL	SBYTE		BMEntityGetPlayerPtr(BMNODE **PtrPlayer);
EXPORT_DLL	SBYTE		BMEntityGetBombsPtr (BMNODE **PtrBombs);

#endif
