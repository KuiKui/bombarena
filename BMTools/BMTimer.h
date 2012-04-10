#ifndef __BMTIMER_H__
#define __BMTIMER_H__

#include	"BMGlobalInclude.h"
#include	"BMWin32.h"

//--------------------------------
// Define
//--------------------------------

//--------------------------------
// Structures
//--------------------------------
//--------------------------------
// Globales
//--------------------------------

//--------------------------------
// Fonctions
//--------------------------------
extern	SBYTE		BMTimerStart			();
extern	SBYTE		BMTimerEnd				();

EXPORT_DLL	SBYTE		BMTimerReset		();
EXPORT_DLL	UDWORD		BMTimerGetTime		(void);
EXPORT_DLL	SBYTE		BMTimerUnPause		();
EXPORT_DLL	SBYTE		BMTimerPause		();

#endif
