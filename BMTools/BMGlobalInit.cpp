#include "BMGlobalInit.h"
#include "BMImage.h"
#include "BMMenu.h"

//--------------------------------
// Globales
//--------------------------------
UDWORD		BomberMode;			//to know the current mode
UDWORD		BomberGameMode;		//to know when we are in game the mode in use (menu... in game)
UBYTE		bIsServer;			//to know if we are the server

CONFIG		LocalConfig;		//the local config to this machine

KMLIMAGE	*CursorImage;		//image of the cursor

UDWORD		LockFps;			//Fps Wanted
UDWORD		FrameRate;
UDWORD		DisplayFps;			// = 1 display fps, 0 don't display

UDWORD		LastLock;			//Used by BMWait
UDWORD		LastTime;			//Used by BMWait
UDWORD		TimeToWait;			//Used by BMWait

SWORD		GlobalResX;
SWORD		GlobalResY;
UBYTE		GlobalColor;

//*********************************************************************
//* FUNCTION: BMGlobalInitConsoleCommand 
//*--------------------------------------------------------------------
//* DESCRIPT: Regoster the console command to modifie the fps parameter
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    05/01/2001 16:41:21
//* REVISION:									
//*********************************************************************
void	BMGlobalInitConsoleCommand ()
{
	BMConsoleCsleAddDataToConsole("g_lockfps", &LockFps, CSLE_PARAM_FLAG_DWORD);
	BMConsoleCsleAddDataToConsole("g_showfps", &DisplayFps, CSLE_PARAM_FLAG_DWORD);
}

//*********************************************************************
// Name	: BMGlobalStart
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								05/01/2001 15:32:23
//*********************************************************************

UDWORD BMGlobalStart(HINSTANCE hInst)
{
	bIsServer		= BMFALSE;
/*	pszField		= NULL;
	pszElement		= NULL;
	pszNamePlayer	= NULL;
	CRedPlayer		= 0;
	CGreenPlayer	= 0;
	CBluePlayer		= 0;
	pszSprite		= NULL;*/

	// KML
	INITMEM (4000000);

//TEST MEMORY DEBUG
/*DBGUSER test;

  KMLDebugInit ();

for (int i = 0; i < 10;i ++)
{
	KMLDebugAlloc (__FILE__, __LINE__, 100, (void *)(0x010 +i) );
}

//KMLDebugFree (__FILE__, __LINE__, (void *)0x010);

test = KMLDebugEnd ();
while (test.LineNumber != -1)
{
	KMLWarning ("Problem %s line %d", test.FileNameAlloc , test.LineNumber );
	test = KMLDebugEnd ();
}

exit (-1);*/
//TEST MEMORY DEBUG

	GlobalColor = 16;
	if (KMLStartLib(hWnd, hInstance, GlobalResX, GlobalResY, DD_FENETRE, GlobalColor, 255, 0, 255) != KML_OK)
		return NULL;
	KMLControlStartLib(hInstance, KML_CONTROL_PAD | KML_CONTROL_KEY | KML_CONTROL_MOUSE | KML_CONTROL_SWITCH); 

	// Image BDD
	if (BMImageStart () != BMTRUE)
		return NULL;

	// Console
	if (BMUIFontInit	("fontmodel.txt", "./Data/Font/") != BMTRUE)
	{
		KMLWarning("can't load the fonts");
	}


	BMUIFontSelect	("normal");

	BMConsoleStart	("Data/Console/backconsole.bmp");
	BMGlobalInitConsoleCommand ();

	// Timer
	if (BMTimerStart() != BMTRUE)
		return NULL;


	// Animation
	if (BMAnimStart ()  != BMTRUE)
		return NULL;

	//Map
	if (BMMapStart () != BMTRUE)
		return NULL;

	// Player
	if (BMPlayerStart () != BMTRUE)
		return NULL;

	// Sprite
	if (BMSpriteStart ("SpriteModel.txt","./Data/") != BMTRUE)
		return NULL;

	//	Debug
	if (BMDebugStart () != BMTRUE)
		return NULL;

	//	OS
	if (BMUIOSStart ("./Data/Menu/tools.bmp") != BMTRUE)
		return NULL;

	if (BMMenuStart () != BMTRUE)
		return NULL;

	//Entity
	if (BMEntityStart () != BMTRUE)
		return NULL;

	if (BMBonusStart () != BMTRUE)
		return NULL;

	if (BMBombsStart () != BMTRUE)
		return NULL;

	//Load Config
	BMLoadUserConfig ();

	//--Always at the end !!! because the init of tools set the all the callback to null so that the mod can't set them if it isn't after them
	// Server
	BMSvInit();

	// Client players
	if(!(BMClInitPlayers()))
		return NULL;


	// Load the console command

	LockFps		= 120;
	FrameRate	= 0;
	DisplayFps	= 0;

	// ok
	return 1;
}

//*********************************************************************
// Name	: BMGlobalEnd
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								05/01/2001 15:45:01
//*********************************************************************

UDWORD BMGlobalEnd(void)
{

	// Delete debug
	if (BMDebugEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMDebug");

	// Delete User Config
	if (BMDeleteUserConfig () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de Local config");

	// Bombs
	if (BMBombsEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMBombs");

	// Bonus
	if (BMBonusEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMBonus");

	// Player
	if (BMPlayerEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMPlayer");

	//Entity
	if (BMEntityEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMEntity");

	//Sprite
	if (BMSpriteEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMSprite");

	//Delete Map
	if (BMMapEnd () != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMMap");

	// Delete Animation
	if (BMAnimEnd() != BMTRUE)
		KMLWarning ("Probleme dans la liberation de BMAnim");

	// Delete UI
	BMUIFontStop();

	//Delete Console
	BMConsoleEnd();

	//Delete OS
	BMMenuEnd ();

	BMUIOSEnd ();

	//Delete Image
	BMImageEnd ();

	//Always after the tools !!! because we must call the callback before unload dll
	// Delete players
	BMClCleanUpPlayers();

	// Delete Server
	BMSvCleanUp();


	// Delete KML
	KMLQuitLib();
	KMLControlQuitLib();
	
	SDWORD value;
	value = FREEMEM();
	if (value != 0)
	{
		KMLWarning ("Memoire non libérée %d", value);
	}

	// EndTimer
	BMTimerEnd();

	if (BMListMemory != 0)
	{
		KMLWarning ("BMList non libere %d", BMListMemory);
	}

	if (BMSpriteMemory != 0)
	{
		KMLWarning ("BMSprite non libere %d", BMSpriteMemory);
	}
	
	if (BMAnimMemory != 0)
	{
		KMLWarning ("BMAnim non libere %d", BMAnimMemory);
	}

	if (BMMapMemory != 0)
	{
		KMLWarning ("BMMap non libere %d", BMMapMemory);
	}

	if (BMPlayerMemory != 0)
	{
		KMLWarning ("BMPlayer non libere %d", BMPlayerMemory);
	}

	if (BMWin32KeysMemory != 0)
	{
		KMLWarning ("BMWin32Keys non libere %d", BMWin32KeysMemory);
	}

	if (BMServerMemory != 0)
	{
		KMLWarning ("BMServer non libere %d", BMServerMemory);
	}
	
	if (BMClientServerMemory != 0)
	{
		KMLWarning ("BMClientServer non libere %d", BMClientServerMemory);
	}

	if (GetKMLLightMemory() != 0)
	{
		KMLWarning ("BMKMLLight non libere %d", GetKMLLightMemory());
	}

	if (BMOSMemory != 0)
	{
		KMLWarning ("BMOS non libere %d", BMOSMemory);
	}

	if (BMConsoleMemory != 0)
	{
		KMLWarning ("CONSOLE non libere %d", BMConsoleMemory);
	}

	if (BMEntityMemory != 0)
	{
		KMLWarning ("Entity non libere %d", BMEntityMemory);
	}

	if (BMBonusMemory != 0)
	{
		KMLWarning ("Bonus non libere %d", BMBonusMemory);
	}

	if (BMBombsMemory != 0)
	{
		KMLWarning ("Bombs non libere %d", BMBombsMemory);
	}

	if (BMImageMemory != 0)
	{
		KMLWarning ("Image non libere %d", BMImageMemory);
	}

	return 1;
}

//*********************************************************************
// Name	: BMGlobalUpdate
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								05/01/2001 15:49:51
//*********************************************************************

UDWORD BMGlobalUpdate(void)
{
	static UDWORD	LastTime=0;
	UDWORD			Time;
	UBYTE			i;
	BMOSOBJECT *ObjectPrinc;

	Time = BMTimerGetTime();

	if (!LastTime) 
		LastTime = Time;

	KMLControlUpdate();		//update controls
	if (BomberMode == BM_MODE_MENU_INIT)
	{
		if (BMUIOSWindowCreate (&ObjectPrinc, NULL, 0) != BMTRUE)
			exit (-12);

		((BMOSWINDOW *)ObjectPrinc->BMOSInfos)->menuid = BM_MAINMENU;
		BMMenuLoad (ObjectPrinc);

		//cursor
		CursorImage = KMLCreateImage (0, 0, KML_BMP, "/Data/Menu/cursor2.bmp");
		if (CursorImage == NULL)
			exit( 223);

		BomberMode = BM_MODE_MENU;
	}
	else if (BomberMode == BM_MODE_MENU)
	{
		KMLMOUSE	currentmouse;

		KMLControlMouseGetState (&currentmouse);
		BMUIOSDisplayCurrent();

		BMMenuManage ( (BMOSWINDOW *)(BMUIOSGetCurrentWindow()->BMOSInfos), ((BMOSWINDOW *) BMUIOSGetCurrentWindow()->BMOSInfos)->menuid);

		KMLFastBlt (NULL, currentmouse.x, currentmouse.y, CursorImage, 0, 0, CursorImage->Largeur, CursorImage->Hauteur);

		BMWin32ConsoleUpdate(1);

	}
	else if (BomberMode == BM_MODE_GAME_JOIN)
	{
		bIsServer = BMFALSE;
	}
	else if (BomberMode == BM_MODE_GAME_SEVER)
	{
		bIsServer = BMTRUE;

		//close all the window
		while ( BMUIOSWindowClose () == BMTRUE)
			;

		//add the player for server
		BMClAddPlayer (0);
		BMClAddPlayer (1);

		//choose the map ...
		if (BMMapLoadElement ("Data/Map/Map description.txt") != BMTRUE)
			exit (1);

		if (BMMapLoadField ("Data/Map/Bomberman.fie") != BMTRUE)
			exit(2);

		if (BMPlayerAdd (0, 255, 255, 255, LocalConfig.pszNamePlayer[0], LocalConfig.pszSprite[0], "./Data/") != BMTRUE)			//avirer
			exit (-6);																				//avirer
		if (BMPlayerAdd (1, 100, 255, 255, LocalConfig.pszNamePlayer[1], LocalConfig.pszSprite[1], "./Data/") != BMTRUE)			//avirer
			exit (-6);																				//avirer
		BomberMode = BM_MODE_INGAME;
		LastTime = Time;
	}
	else if (BomberMode == BM_MODE_INGAME)
	{
		BMClUpdatePlayersKeys();
		if (BMClUpdate() != BMTRUE)
		{
			return (0);
		}
		
		BMMapDisplay (NULL, 50, 74);
	
		for (i = 0; i < BMPLAYERMAXNUMBER; i++)
			BMPlayerDisplay (i);


		BMSvUpdate((float)(Time-LastTime)*0.001f);

		BMWin32ConsoleUpdate(1);

		BMDebugPrintArray ();	//Display the debug array

		if ( KMLControlKeysPress ( DIK_ESCAPE, 0 ) )
		{
			//get it the sub menu game

			BMMapEnd ();		//end to reset map allocation !
			BMMapStart ();
			BMPlayerEnd ();
			BMPlayerStart ();
	
			BMSvCleanUpMod();

			BomberMode = BM_MODE_MENU_INIT;

		}
		//don't forget to do when we come back on a menu BomberMode = BM_MODE_MENU_INIT
	}

	LastTime=Time;

	if (DisplayFps == 1)	//Display the fps if needed
	{
		STRING chaine[255];

		sprintf (chaine, "Fps : %d",FrameRate);
		BMUIFontDrawString (chaine, 10, 10, NULL);
	}
	else if (DisplayFps != 0)
		DisplayFps = 0;


	KMLFinalDraw ();//Don't Clear the screen with parameter 0
	BMWait ();

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMWait
//*--------------------------------------------------------------------
//* DESCRIPT: Wait the time to have the good fps
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    05/01/2001 16:22:09
//* REVISION:									
//*********************************************************************
SBYTE	BMWait()
{
	UDWORD		CurrentTime;

	if (LastLock != LockFps)
	{
		if (LockFps != 0)
		{
			LastLock = LockFps;

			TimeToWait = 1000 / LockFps;
		}
	}
	
	CurrentTime = BMTimerGetTime ();
	while ( (CurrentTime - LastTime) < TimeToWait)
		CurrentTime = BMTimerGetTime ();

	if (CurrentTime - LastTime != 0)
		FrameRate = 1000 / (CurrentTime - LastTime);

	LastTime = CurrentTime;

	return (BMTRUE);
}


//**************************************************************
/** \fn     SBYTE BMQuit()
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  to quit bomberman
 *
 *  \author MikE                       \date 20/03/2001 13:01:22
 ***************************************************************/
SBYTE	BMQuit()
{
#ifdef	__BOMBERMAN_PC__
	PostQuitMessage (0);
#endif

#ifdef	__BOMBERMAN_AMIGA__
	//specific quit message
#endif

#ifdef __BOMBERMAN_DREAMCAST__
	//specific quit message
#endif

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMLoadUserConfig ()
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  load the local config
 *
 *  \author MikE                       \date 20/03/2001 13:57:29
 ***************************************************************/
SBYTE BMLoadUserConfig ()
{
	KMLFILE	*f;
	STRING	chaine [255];
	UBYTE	i;
	UDWORD	len;

	//set a default config
	for (i = 0; i < BMCLIENT_MAX_ON_MACHIN; i++)
	{
		LocalConfig.pszNamePlayer[i]	= (STRING *) ALLOC (strlen("Unnamed Playerx") + 1);
		LocalConfig.pszSprite[i]		= (STRING *) ALLOC (strlen("perso.spr") + 1);
		LocalConfig.CRedPlayer[i]		= 255;
		LocalConfig.CGreenPlayer[i] 	= 000;
		LocalConfig.CBluePlayer[i]		= i * 50;

		strcpy( LocalConfig.pszNamePlayer[i], "Unnamed Player");
		len = strlen (LocalConfig.pszNamePlayer[i]);
		LocalConfig.pszNamePlayer[i] [ len ] = i + '1';
		LocalConfig.pszNamePlayer[i] [ len + 1] = '\0';

		strcpy( LocalConfig.pszSprite [i], "perso.spr");
	}

	strcpy ( LocalConfig.pszField, "Data/Map/Bomberman.fie");
	strcpy ( LocalConfig.pszElement, "Data/Map/Map description.txt");

	//set default key
	LocalConfig.Left			[0] = BM_LEFT;
	LocalConfig.Right			[0] = BM_RIGHT;
	LocalConfig.Up				[0]	= BM_UP;
	LocalConfig.Down			[0] = BM_DOWN;
	LocalConfig.Button1			[0] = BM_P;
	LocalConfig.Button2			[0] = BM_M;

	LocalConfig.Left			[1] = BM_NUMPAD4;
	LocalConfig.Right			[1] = BM_NUMPAD6;
	LocalConfig.Up				[1]	= BM_NUMPAD8;
	LocalConfig.Down			[1] = BM_NUMPAD2;
	LocalConfig.Button1			[1] = BM_ADD;
	LocalConfig.Button2			[1] = BM_NUMPADENTER;

	LocalConfig.Left			[2] = BM_Q;
	LocalConfig.Right			[2] = BM_E;
	LocalConfig.Up				[2]	= BM_W;
	LocalConfig.Down			[2] = BM_S;
	LocalConfig.Button1			[2] = BM_R;
	LocalConfig.Button2			[2] = BM_F;

	LocalConfig.Left			[3] = BM_H;
	LocalConfig.Right			[3] = BM_K;
	LocalConfig.Up				[3]	= BM_U;
	LocalConfig.Down			[3] = BM_J;
	LocalConfig.Button1			[3] = BM_8;
	LocalConfig.Button2			[3] = BM_9;

	f = KMLPackOpen (NULL, "./Data/UserConfig.cfg");

	if (f == NULL)
		return (BMFALSE);			//error, can't open the file

	//Search for the beginning of the description
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMFALSE);	//Error
		}
	}while ( strcmp(chaine, BMCONFIGBEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, BMCONFIGEND) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMFALSE);	//Error
		}

		//--------------
		//Case of player
		//--------------
		if (	(strcmp (chaine, BMCONFIGNAME1) == 0)
			||	(strcmp (chaine, BMCONFIGNAME2) == 0)
			||	(strcmp (chaine, BMCONFIGNAME3) == 0)
			||	(strcmp (chaine, BMCONFIGNAME4) == 0)
			)
		{
			if (strcmp (chaine, BMCONFIGNAME1) == 0)
				i = 0;
			if (strcmp (chaine, BMCONFIGNAME2) == 0)
				i = 1;
			if (strcmp (chaine, BMCONFIGNAME3) == 0)
				i = 2;
			if (strcmp (chaine, BMCONFIGNAME4) == 0)
				i = 3;


			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load red color
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			LocalConfig.CRedPlayer [i] = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load green color
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			LocalConfig.CGreenPlayer [i] = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load blue color
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			LocalConfig.CBluePlayer [i] = atoi( chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load sprite
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if ( LocalConfig.pszSprite [i] != NULL)
				FREE (LocalConfig.pszSprite [i]);

			LocalConfig.pszSprite [i] = (STRING *) ALLOC ( strlen(chaine) + 1);
			strcpy (LocalConfig.pszSprite [i], chaine);


			if (BMFileReadLine (chaine, sizeof(chaine), f) != BMTRUE)		//load name
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (LocalConfig.pszNamePlayer [i] != NULL)
				FREE (LocalConfig.pszNamePlayer [i] );

			LocalConfig.pszNamePlayer [i] = (STRING *) ALLOC ( strlen(chaine) + 1);
			strcpy (LocalConfig.pszNamePlayer [i] , chaine);
		}
		//--------------
		//Case of keys
		//--------------
		//else
		//{
		//}
	}

	KMLPackClose (f);
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMDeleteUserConfig ()
 *
 *  \param  void 
 *
 *  \return BM_TRUE 
 *
 ***************************************************************
 *  \brief delete the allocation for local config
 *
 *  \author MikE                       \date 30/03/2001 13:27:43
 ***************************************************************/
SBYTE	BMDeleteUserConfig ()
{
	UBYTE i;

	for (i = 0; i < BMCLIENT_MAX_ON_MACHIN; i++)
	{
		if ( LocalConfig.pszNamePlayer[i] != NULL)
			FREE ( LocalConfig.pszNamePlayer [i] );

		if ( LocalConfig.pszSprite[i] != NULL )
			FREE ( LocalConfig.pszSprite [i] );
	}

	return (BMTRUE);
}