#ifndef __BMDEBUG_H__
#define	__BMDEBUG_H__

#include "BMGlobalInclude.h"
#include "BMWin32.h"
#include "BMUIFont.h"


//--------------------------------
// Define
//--------------------------------
//#define		DEBUGNUMBERLINES		((UBYTE) (480 / (BMUIFONT_FONTSIZE_Y + 1))) TODO revoir

//-- here, we use the debugger
#define BM_DEBUG

//--------------------------------
// Structures
//--------------------------------


//--------------------------------
// Globales
//--------------------------------

//--------------------------------										
// Fonctions															
//--------------------------------

extern	SBYTE		BMDebugStart		();
extern	SBYTE		BMDebugEnd			();

extern	SBYTE		BMDebugPrintArray	();
extern	SBYTE		BMDebugPrint		(STRING *Line);


#endif


