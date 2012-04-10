//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMGLOBALINCLUDE_H__
#define	__BMGLOBALINCLUDE_H__

#define __BOMBERMAN_PC__				//version definition !!!


#include	<assert.h>
#include	"../kmllight/kmllight.h"
#include	<string.h>

#include	"BMKeys.h"
#include	"BMWin32Type.h"
#include	"BMList.h"

//--------------------------------
// Define
//--------------------------------

#define	BMTRUE					1
#define	BMFALSE					0

//-- Size of the map			
#define BMMAP_WIDTH				15	//Size of horizontal map's coord (in case)
#define	BMMAP_HEIGHT			11	//Size of vertical map's coord (in case)

//-- Size of element of the map
#define	BMMAP_ELEMENTSIZE		36	//Size in pixel of an element

//-- The CallBack function of the mod...
typedef	UBYTE (*CALLBACKFUNCTION)(void *Object);

#define	BMCLIENT_MAX_ON_MACHIN	4	//number max on client on one machine

//--------------------------------
// Variables
//--------------------------------

extern	HWND				hWnd;
extern	HINSTANCE			hInstance;
extern	SWORD				GlobalResX;
extern	SWORD				GlobalResY;
extern	UBYTE				GlobalColor;




#endif // __BMGLOBALINCLUDE_H__