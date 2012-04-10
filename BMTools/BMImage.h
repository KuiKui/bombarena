#ifndef __BMIMAGE_H__
#define	__BMIMAGE_H__

#include	"BMGlobalInclude.h"
#include	"../kmllight/KmlLight.h"

//--------------------------------
// Defines
//--------------------------------

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	UDWORD		NumberUsed;				//Number of time the picture is used
	STRING		*pszName;				//Name of the file
	KMLIMAGE	*PtrImage;				//Ptr to a Kmlimage
}BMIMAGE;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMImageMemory;

//--------------------------------
// Globales
//--------------------------------
extern	BMLIST		*BMImageList;		//List of pictures loaded, 


//--------------------------------
// Fonctions
//--------------------------------

extern		SBYTE		BMImageStart		();
extern		SBYTE		BMImageEnd			();

EXPORT_DLL	KMLIMAGE *	BMImageCreate		(UDWORD x, UDWORD y, UDWORD Flag, STRING *FileName);
EXPORT_DLL	SBYTE		BMImageDelete		(KMLIMAGE *ImageToDelete);

#endif
