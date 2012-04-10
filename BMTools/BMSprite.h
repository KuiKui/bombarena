#ifndef __BMSPRITE_H__
#define	__BMSPRITE_H__

#include	"BMGlobalInclude.h"
#include	"BMAnims.h"

//--------------------------------
// Defines
//--------------------------------
#define		SPR_ONMAP			(1 << 0)
#define		SPR_COLORPLAYER		(1 << 1)
#define		SPR_VALID			(1 << 2)

//--	File Delimiter
#define		SPR_BEGIN			"#BEGIN_SPRITE"
#define		SPR_END				"#END_SPRITE"

#define		SPR_BEGIN_MODEL		"#BEGIN_SPRITE_MODEL"
#define		SPR_END_MODEL		"#END_SPRITE_MODEL"

#define		SPR_COLOR			"#SET_FLAGCOLOR"


//--------------------------------
// Structures
//--------------------------------

typedef struct
{
	STRING	*FileName;		//Name of the file model
	BMLIST	*ListAnim;		//List of anim of the sprite, BMSPRANIMS...
	UDWORD	Flag;			//Flag of the sprite, (COLOR_DEPEND_PLAYER...)
	UDWORD	NumberUsed;		//Number of instance used
}SPRMODEL;

typedef	struct
{
	SDWORD	KeyAnim;		//identifier for the anim of the sprite (local to the model of the sprite, it's not unique for every sprite)
	BMANIM	*Anim;			//anim	
}BMSPRANIMS;

typedef struct
{
	UDWORD		Id;				//Id
	UDWORD		PosX;			//Pos x in pixel
	UDWORD		PosY;			//Pos y in pixel
	SPRMODEL*	PtrModel;		//Pointer to the model, which is the same for every instance
	BMANIM		*CurrentAnim;	//Current Anim
//	UDWORD		TimeToKeepFrame;//Time To Keep the current frame of anim, Reserved
}BMSPRITE;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMSpriteMemory;

//--------------------------------
// Globales
//--------------------------------


//--------------------------------
// Fonctions
//--------------------------------

extern	SBYTE		BMSpriteStart		(STRING *FileName, STRING *pszPath = NULL);
extern	SBYTE		BMSpriteEnd			();

extern	SBYTE		BMSpriteAddModel	(STRING *FileName, STRING *pszPath = NULL);

extern	SBYTE		BMSpriteAdd			(BMSPRITE **pSprite, STRING *FileName, STRING *pszPath = NULL);
extern	SBYTE		BMSpriteDelete		(BMSPRITE *Sprite);

EXPORT_DLL	SBYTE	BMSpriteSetAnim		(BMSPRITE *Sprite, SDWORD AnimId);

extern	SBYTE		BMSpriteDisplay		(BMSPRITE *Sprite);

#endif
