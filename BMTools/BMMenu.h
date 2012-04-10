#ifndef	__BMMENU_H__
#define	__BMMENU_H__

#include "BMGlobalInclude.h"
#include "BMUIOS.h"
#include "BMMap.h"
#include "BMFile.h"

//--------------------------------
// Define
//--------------------------------

#define		BM_MAINMENU			1
#define		BM_CREATEGAME		2
#define		BM_JOINGAME			3
#define		BM_SETUP			4
#define		BM_OPTIONS			5
#define		BM_QUIT				6

#define	 BMMENUFILETOOLS		"./Data/Menu/windowback.bmp"
//-- File Delimiter
#define BMMENUBEGIN				"#BEGIN_MENU"
#define BMMENUEND				"#END_MENU"

#define BMMENUTEXT				"#SET_TEXT"							
#define	BMMENUTEXTS				"#SET_TEXTS"
#define BMMENUELEVATOR			"#SET_BOX"

#define BMMENUBACKGROUND		"#SET_BACKGROUND"
#define	BMMENUBACKNORMAL		"NORMAL"
#define	BMMENUBACKMOSAIC		"MOSAIC"

#define BMMENUCENTERH			"CENTERH"
#define BMMENUCENTERV			"CENTERV"
#define BMMENUALLCENTER			"CENTERHV"

typedef void (*MenuCallBack) (UDWORD MenuId);

//--------------------------------
// Structure
//--------------------------------
typedef struct
{
	KMLIMAGE	*PlayMapImage;
	STRING		pszFichierElement[256];
	STRING		pszFichierField[256];
	UBYTE		bIsMenuLoaded;
}BMMENUGLOBAL;

//----------------------------------------------------
//! Global
//----------------------------------------------------
extern	BMMENUGLOBAL	GlobalMenu;

//--------------------------------
// Fonctions
//--------------------------------

void		BMMenuSetCallBackLoad	( MenuCallBack fonction);
void		BMMenuSetCallBackManage	( MenuCallBack fonction);

extern UBYTE		BMMenuStart					();
extern UBYTE		BMMenuEnd					();
extern UBYTE		BMMenuLoad					( BMOSOBJECT *Object );
extern UBYTE		BMMenuLoadScript			( BMOSOBJECT *Object, STRING *pszFichier );
extern UBYTE		BMMenuManage				( BMOSWINDOW *CurrentWindow, UDWORD MenuId );

//----------------------------------------------------
//! Menu Behavior
//----------------------------------------------------

extern	UBYTE	BMMenuMainMenu					( BMOSWINDOW *CurrentWindow );
extern	UBYTE	BMMenuCreateGame				( BMOSWINDOW *CurrentWindow );
extern	UBYTE	BMMenuSetup						( BMOSWINDOW *CurrentWindow );


#endif