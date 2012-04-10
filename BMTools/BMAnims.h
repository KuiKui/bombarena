#ifndef __BMANIM_H__
#define	__BMANIM_H__

#include	"BMGlobalInclude.h"

//--------------------------------
// Structures
//--------------------------------

typedef	struct
{
	STRING		*FileName;				//name of the KMS
	BMLIST		*ListFrames;			//list of the frames of the anim, BMANIMFRAME...
	KMLIMAGE	*Image;					//pointer to image
	UDWORD		NumberUsed;				//Number of instance of this model
}ANIMMODEL;

typedef struct
{
	UDWORD		AnimFrameUCoords;		//coord u from uv
	UDWORD		AnimFrameVCoords;		//coord v from uv
	UDWORD		AnimFrameWidth;			//width of animation
	UDWORD		AnimFrameHeight;		//height of animation
	UDWORD		AnimFrameTimeDuration;	//time to keep this frame in ms

}BMANIMFRAME;

typedef struct
{
	UBYTE		IsPlaying;				//1 if the anim is playing
	UDWORD		AnimTimeFrame;			//Time to keep the current frame active
	BMNODE		*CurrentFrame;			//The current node which point to the frame to display (BMANIMFRAME..)
	ANIMMODEL	*Model;					//Model of the anim
}BMANIM;

typedef struct
{
	UDWORD		NumberUsed;				//Number of time the image is used
	KMLIMAGE	*Image;					//ptr to the image
}BMANIMIMAGE;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMAnimMemory;

//--------------------------------
// Globales
//--------------------------------

extern	BMLIST		*AnimsListe;			//list of create anims
extern	BMLIST		*AnimModel;				//list of anim model

//--------------------------------
// Fonctions
//--------------------------------

extern	SBYTE		BMAnimStart			();
extern	SBYTE		BMAnimEnd			();

extern	SBYTE		BMAnimAddModel		(STRING *pszFichier, STRING *pszPath = NULL);

extern	SBYTE		BMAnimAdd			(BMANIM **Anim, STRING *pszFichier, STRING *pszPath = NULL);
extern	SBYTE		BMAnimDelete		(BMANIM	*AnimToDelete);

extern	SBYTE		BMAnimPlay			(BMANIM *AnimToPlay);
//extern	SBYTE		BMAnimPlay			(BMANIM *AnimToPlay, UDWORD	*TimeToKeepFrame);

//extern	SBYTE		BMAnimUpdate		(BMANIM *Anim, UDWORD *TimeToKeepFrame);
extern	SBYTE		BMAnimUpdate		(BMANIM *Anim);

extern	SBYTE		BMAnimDisplay		(BMANIM *Anim, UWORD CoordX, UWORD CoordY);

#endif
