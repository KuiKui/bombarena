#include "BMMenu.h"

//----------------------------------------------------
//! Global
//----------------------------------------------------



//**************************************************************
/** \fn     UBYTE MainMenu ( BMOSWINDOW *CurrentWindow )
 *
 *  \param  *CurrentWindow 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  the Main Menu
 *
 *  \author MikE                       \date 15/03/2001 14:14:43
 ***************************************************************/
UBYTE	BMMenuMainMenu ( BMOSWINDOW *CurrentWindow )
{
	KMLMOUSE	currentmouse;
	BMOSOBJECT  *ObjectPointed,
				*ObjectWindow;

	KMLControlMouseGetState	(&currentmouse);
	BMUIOSWhosObject (&ObjectPointed, currentmouse.x, currentmouse.y);
	
	if (ObjectPointed != NULL)
	{
		switch ( ((BMOSBASEOBJECT *)ObjectPointed->BMOSInfos)->menuid )
		{
			case 1:
				//create 
				//BomberMode = BM_MODE_GAME_SEVER;
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
				{
					if (BMUIOSWindowCreate (&ObjectWindow, NULL, 0) != BMTRUE)
						exit (-12);

					((BMOSWINDOW *)ObjectWindow->BMOSInfos)->menuid = BM_CREATEGAME;
					BMMenuLoad (ObjectWindow);
				}
				break;
			case 2:
				//join
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
					BomberMode = BM_MODE_GAME_JOIN;
				break;
			case 3:
				//setup
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
				{
					if (BMUIOSWindowCreate (&ObjectWindow, NULL, 0) != BMTRUE)
						exit (-12);

					((BMOSWINDOW *)ObjectWindow->BMOSInfos)->menuid = BM_SETUP;
					BMMenuLoad (ObjectWindow);
				}
				//mberMode = BM_MODE_SETUP;
				break;
			case 4:
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
				{
				}

				//options
				break;
			case 5:
				//credits
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
				{
				}
				break;
			case 6:
				//quit
				if (BMUIOSObjectManage (ObjectPointed, currentmouse) == BMTRUE)
				{
					BMQuit ();
				}
				break;
		}
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     UBYTE BMMenuCreateGame ( BMOSWINDOW *CurrentWindow )
 *
 *  \param  *CurrentWindow 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 14:17:52
 ***************************************************************/
UBYTE	BMMenuCreateGame ( BMOSWINDOW *CurrentWindow )
{
	KMLMOUSE	currentmouse;
	BMOSOBJECT  *ObjectPointed;
	
	if (GlobalMenu.bIsMenuLoaded == BMFALSE)
	{
		KMLIMAGE *image;
		
		//load current map
		if (BMMapLoadElement ("Data/Map/Map description.txt") != BMTRUE)
			exit (1);

		if (BMMapLoadField ("Data/Map/Bomberman.fie") != BMTRUE)
			exit(2);

		//load current image
		if (GlobalMenu.PlayMapImage != NULL)
		{
			KMLDeleteImage ( GlobalMenu.PlayMapImage );
			GlobalMenu.PlayMapImage = NULL;
		}
		
		GlobalMenu.PlayMapImage = KMLCreateImage ( 640 / 3, (480 - 60)/ 3); //revoir pour la callback en cas de ALT TAB

		image = KMLCreateImage ( 640, 480, NULL, NULL);
		BMMapDisplay (image, 50, 74);
		KMLStretchBlt (GlobalMenu.PlayMapImage, 0, 0, 640 / 3, (480-60) / 3, image, 0, 60, 640, 480 - 60);
		KMLDeleteImage (image);
		image = NULL;
		GlobalMenu.bIsMenuLoaded = BMTRUE;
	}

	KMLControlMouseGetState	(&currentmouse);
	BMUIOSWhosObject (&ObjectPointed, currentmouse.x, currentmouse.y);

	KMLFastBlt (NULL, 100, 200, GlobalMenu.PlayMapImage, 0, 0, GlobalMenu.PlayMapImage->Largeur, GlobalMenu.PlayMapImage->Hauteur);
	
	if (ObjectPointed != NULL)
	{
		switch ( ObjectPointed->BMOSType )
		{
		case BMOS_SELECTIONNABLETEXT:
			if (BMUIOSTextSManage ( (BMOSTEXTSELECTIONNABLE *)ObjectPointed->BMOSInfos, currentmouse) == BMTRUE)
			{
				//do something;
			}
			break;
		case BMOS_TEXT:
			if (BMUIOSTextManage ( (BMOSTEXT *) ObjectPointed, currentmouse) == BMTRUE)
			{
				switch ( ((BMOSTEXT *)ObjectPointed->BMOSInfos)->menuid)
				{
				case 1:	//play						
					GlobalMenu.bIsMenuLoaded = BMFALSE;
					if (GlobalMenu.PlayMapImage != NULL)
					{
						KMLDeleteImage ( GlobalMenu.PlayMapImage );
						GlobalMenu.PlayMapImage = NULL;
					}

					BMMapEnd ();		//end to reset map allocation !
					BMMapStart ();
					BomberMode = BM_MODE_GAME_SEVER;
					
					break;
				case 2:	//back
					GlobalMenu.bIsMenuLoaded = BMFALSE;
					if (GlobalMenu.PlayMapImage != NULL)
					{
						KMLDeleteImage ( GlobalMenu.PlayMapImage );
						GlobalMenu.PlayMapImage = NULL;
					}

					BMMapEnd ();		//end to reset map allocation !
					BMMapStart ();
					BMUIOSWindowClose ();
					break;
				}			
			}
			break;

		case BMOS_ELEVATOR:
			if (BMUIOSElevatorManage ( (BMOSELEVATOR *)ObjectPointed->BMOSInfos, currentmouse) == BMTRUE)
			{
				//new selection
				//do something
			}
			break;
		}
	}
			
	return (BMTRUE);
}

//**************************************************************
/** \fn     UBYTE BMMenuSetup ( BMOSWINDOW *CurrentWindow )
 *
 *  \param  *CurrentWindow 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 14:21:47
 ***************************************************************/
UBYTE BMMenuSetup ( BMOSWINDOW *CurrentWindow )
{
	KMLMOUSE	currentmouse;
	BMOSOBJECT  *ObjectPointed;
				

	KMLControlMouseGetState	(&currentmouse);
	BMUIOSWhosObject (&ObjectPointed, currentmouse.x, currentmouse.y);
		
	if (ObjectPointed != NULL)
	{
		switch ( ObjectPointed->BMOSType )
		{
		case BMOS_SELECTIONNABLETEXT:
			if (BMUIOSTextSManage ( (BMOSTEXTSELECTIONNABLE *)ObjectPointed->BMOSInfos, currentmouse) == BMTRUE)
			{
				//do something;
			}
			break;
		case BMOS_TEXT:
			if (BMUIOSTextManage ( (BMOSTEXT *) ObjectPointed, currentmouse) == BMTRUE)
			{
				switch ( ((BMOSTEXT *)ObjectPointed->BMOSInfos)->menuid)
				{
				case 1:
					//Player						
					
					break;
				case 2:
					//Control
					break;
				case 3:
					//Sound
					break;
				case 4:
					//Back
					BMUIOSWindowClose ();
					//BomberMode = BM_MODE_MENU;
					break;
				}
			}
			break;

		case BMOS_ELEVATOR:
			if (BMUIOSElevatorManage ( (BMOSELEVATOR *)ObjectPointed->BMOSInfos, currentmouse) == BMTRUE)
			{
				//new selection
			}
			break;
		}
	}

	return (BMTRUE);
}