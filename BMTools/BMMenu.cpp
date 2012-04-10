#include "BMMenu.h"

MenuCallBack	MenuCallBackLoad	= NULL;
MenuCallBack	MenuCallBackManage	= NULL;
BMMENUGLOBAL	GlobalMenu;

//**************************************************************
/** \fn     UBYTE BMMenuStart ()
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 12/03/2001 13:47:12
 ***************************************************************/
UBYTE		BMMenuStart				()
{
	GlobalMenu.PlayMapImage			= NULL;
	GlobalMenu.pszFichierElement[0]	= '\0';
	GlobalMenu.pszFichierField[0]	= '\0';
	GlobalMenu.bIsMenuLoaded		= BMFALSE;

	return (BMTRUE);
}

//**************************************************************
/** \fn     UBYTE BMMenuEnd ()
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 12/03/2001 13:53:07
 ***************************************************************/
UBYTE		BMMenuEnd				()
{
	if ( GlobalMenu.PlayMapImage != NULL )
	{
		KMLDeleteImage (GlobalMenu.PlayMapImage);
		GlobalMenu.PlayMapImage = NULL;
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     UBYTE BMMenuLoad ( BMOSOBJECT *obj )
 *
 *  \param  MenuId 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 08/03/2001 13:43:52
 ***************************************************************/
UBYTE		BMMenuLoad ( BMOSOBJECT *obj )
{
	UDWORD		MenuId;
	BMOSOBJECT	*Object;
	BMOSBOXTEXT	boxtext;
	STRING		chaine [255];

	if (obj->BMOSType != BMOS_WINDOW)
	{
		assert (false);
		exit (-1);
	}

	MenuId = ((BMOSWINDOW *)obj->BMOSInfos)->menuid;

	switch ( MenuId )
	{
	case 1:
		//we load the menu.txt
		//create window
		BMMenuLoadScript ( obj, "//data/menu/menu.txt");
		break;
	case 2:
		BMMenuLoadScript ( obj, "//data/menu/newgame.txt"); //newgame normaly !
		if (BMUIOSGetObjectByMenuId (&Object, 3) != BMTRUE)
			exit (-15);

		
		strcpy (chaine, "Cool test");
		boxtext.pszString = chaine;
		boxtext.Id = 1;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "Essai");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "Morb la salope de sa race qui fait chier et qui vomi");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "ovduf");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "sqdlgkjdsqk");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "lsfdjmlqs");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "morb la schtroumpfette");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "klui kui le lead chiant");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "morb le tigre");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);
		strcpy (chaine, "MikE le vif");
		boxtext.Id ++;
		BMUIOSElevatorAddItem ( (BMOSELEVATOR *)Object->BMOSInfos, boxtext);

		break;
	case 3:

		break;
	case 4:
		BMMenuLoadScript ( obj, "//data/setup.txt");
		break;
	case 5:

		break;
	case 6:

		break;
	default:
		if (MenuCallBackLoad != NULL)
			MenuCallBackLoad ( MenuId );
		break;
	}

	return (BMTRUE);
}



//**************************************************************
/** \fn     UBYTE BMMenuManage ( UDWORD MenuId )
 *
 *  \param  MenuId 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  manage all the menu and call the mod callback if needed
 *
 *  \author MikE                       \date 08/03/2001 14:25:56
 ***************************************************************/
UBYTE		BMMenuManage	( BMOSWINDOW *CurrentWindow, UDWORD MenuId )
{
	/*KMLMOUSE	currentmouse;
	BMOSOBJECT  *ObjectPointed,
				*ObjectWindow;*/

	switch ( MenuId )
	{
	case 1:	//the main menu
		BMMenuMainMenu		(CurrentWindow); 
		break;
	case 2:	//create a game menu															//create server
		BMMenuCreateGame	(CurrentWindow);
		break;
	case 3:

		break;
	case 4:	//setup
		BMMenuSetup			(CurrentWindow);
		break;
	case 5:

		break;
	case 6:

		break;
	default:
		if (MenuCallBackManage != NULL)
			MenuCallBackManage ( MenuId );
		break;
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     void BMMenuSetCallBackLoad ( MenuCallBack fonction)
 *
 *  \param  fonction fonction to call for specific mod menu
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 08/03/2001 13:51:53
 ***************************************************************/
void		BMMenuSetCallBackLoad		( MenuCallBack fonction)
{
	MenuCallBackLoad = fonction;
}

//**************************************************************
/** \fn     void BMMenuSetCallBackManage ( MenuCallBack fonction)
 *
 *  \param  fonction function to call for specific mod menu
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 08/03/2001 13:52:10
 ***************************************************************/
void		BMMenuSetCallBackManage	( MenuCallBack fonction)
{
	MenuCallBackManage = fonction;
}

//**************************************************************
/** \fn     UBYTE BMMenuLoadScript ( BMOSWINDOW *Window, STRING *pszFichier )
 *
 *  \param  *Window   
 *  \param  *FileName 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  load the script for a menu
 *
 *  \author MikE                       \date 08/03/2001 13:59:00
 ***************************************************************/
UBYTE		BMMenuLoadScript ( BMOSOBJECT *Object, STRING *pszFichier )
{
	KMLFILE	*f;
	STRING	chaine [255];

	STRING	RelativeDirectory [255];

	strcpy(RelativeDirectory, "\\.");			//Set the directory to the current

	f = KMLPackOpen (NULL, pszFichier);

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
	}while ( strcmp(chaine, BMMENUBEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, BMMENUEND) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMFALSE);	//Error
		}

		//-----------------------
		//Case of text
		//-----------------------
		if (strcmp (chaine, BMMENUTEXT) == 0)
		{
			STRING	name [255];
			STRING	fonte[255];
			UDWORD	menuid,
					x,
					y,
					flag;
			BMOSOBJECT *ObjText;


			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load menu Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			menuid = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load x placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			if (chaine[0] == 'X')
			{
				x = atoi( &chaine[1]);
			}
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load y placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (chaine[0] == 'Y')
			{
				y = atoi( &chaine[1]);
			}
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (BMFileReadWord (&f, fonte, sizeof(fonte)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}


			if (BMFileReadLine (name, sizeof(name), f) != BMTRUE)			//load menu Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}


			if (strcmp (chaine, BMMENUBACKNORMAL) == 0)
				flag = 0;
			else if (strcmp (chaine, BMMENUCENTERH) == 0)
				flag = BMOS_CENTER_H;
			else if (strcmp (chaine, BMMENUCENTERV) == 0)
				flag = BMOS_CENTER_V;
			else if (strcmp (chaine, BMMENUALLCENTER) == 0)
				flag = BMOS_CENTER_H|BMOS_CENTER_V;

			if( BMUIOSTextCreate (&ObjText, name, fonte, x, y, Object->id, flag) != BMTRUE)
				exit(-13);
			if (BMUIOSObjectSetMenuId	(ObjText, menuid) != BMTRUE)
				exit (-14);
		}
		//---------------------------
		//Case of Text Selectionnable
		//---------------------------
		else if (strcmp (chaine, BMMENUTEXTS) == 0)
		{
			STRING		name [255];
			STRING		fonte[255];
			STRING		fonte2[255];
			UDWORD		menuid,
						x,
						y,
						flag;
			BMOSOBJECT	*ObjText;


			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load menu Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			menuid = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load x placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			if (chaine[0] == 'X')
			{
				x = atoi( &chaine[1]);
			}
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load y placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (chaine[0] == 'Y')
			{
				y = atoi( &chaine[1]);
			}
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (BMFileReadWord (&f, fonte, sizeof(fonte)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			if (BMFileReadWord (&f, fonte2, sizeof(fonte2)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}


			if (BMFileReadLine (name, sizeof(name), f) != BMTRUE)		//load menu Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (strcmp (chaine, BMMENUBACKNORMAL) == 0)
				flag = 0;
			else if (strcmp (chaine, BMMENUCENTERH) == 0)
				flag = BMOS_CENTER_H;
			else if (strcmp (chaine, BMMENUCENTERV) == 0)
				flag = BMOS_CENTER_V;
			else if (strcmp (chaine, BMMENUALLCENTER) == 0)
				flag = BMOS_CENTER_H|BMOS_CENTER_V;

			if( BMUIOSTextSCreate (&ObjText, name, fonte, fonte2, x, y, Object->id, flag) != BMTRUE)
				exit(-13);
			if (BMUIOSObjectSetMenuId	(ObjText, menuid) != BMTRUE)
				exit (-14);

		}
		//---------------------------
		//Case of Elevator
		//---------------------------
		else if (strcmp (chaine, BMMENUELEVATOR) == 0)
		{
			STRING		name [255];
			STRING		fonte[255];
			UDWORD		menuid,
						x,
						y,
						sx,
						sy,
						flag;
			BMOSOBJECT	*ObjText;


			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load box Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			menuid = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load x placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			if (chaine[0] == 'X')
				x = atoi( &chaine[1]);
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load y placement
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (chaine[0] == 'Y')
				y = atoi( &chaine[1]);
			else
				return (BMFALSE);


			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load x size
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			
			if (chaine[0] == 'X')
				sx = atoi( &chaine[1]);
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load y size
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (chaine[0] == 'Y')
				sy = atoi( &chaine[1]);
			else
				return (BMFALSE);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)		//load flag
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			flag = atoi( chaine);

			if (BMFileReadWord (&f, fonte, sizeof(fonte)) != BMTRUE)		//load center or not
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (BMFileReadLine (name, sizeof(name), f) != BMTRUE)		//load menu Id
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			if (strcmp (chaine, BMMENUBACKNORMAL) == 0)
				flag = BMOS_BACKSTRETCH;
			else if (strcmp (chaine, BMMENUBACKMOSAIC) == 0)
				flag = BMOS_BACKMOSAIQUE;

			if( BMUIOSElevatorCreate (&ObjText, fonte, x, y, sx, sy, Object->id, BMMENUFILETOOLS, flag) != BMTRUE)
				exit(-13);
			if (BMUIOSObjectSetMenuId(ObjText, menuid) != BMTRUE)
				exit (-14);
		}
		//---------------------------
		//Case of set background
		//---------------------------
		else if (strcmp (chaine, BMMENUBACKGROUND) == 0)
		{
			STRING	name [255];
			UBYTE	flag;

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}

			strcpy (name, RelativeDirectory);
			strcat (name, chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMFALSE);			//Error
			}
			if (strcmp (chaine, BMMENUBACKMOSAIC) == 0)
				flag = BMOS_BACKMOSAIQUE;
			else
				flag = BMOS_BACKSTRETCH;

			BMUIOSWindowLoadBMP ( (BMOSWINDOW *)Object->BMOSInfos, name, flag);
		}
		//------------------
		// Case of Directory
		//------------------
		else if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMFALSE);	//Error
			}
		}
	}

/*TEST
	BMOSOBJECT *ObjElevator;
	BMOSBOXTEXT Item;
	STRING string[255];
	BMUIOSElevatorCreate (&ObjElevator, "normal", 200, 200, 180, 170, Object->id, NULL, NULL);
	BMUIOSLoadBackGround ((BMOSELEVATOR *)ObjElevator->BMOSInfos, "./data/windowback.bmp");

	Item.pszString = string;

	Item.Id = 1;
	strcpy(Item.pszString, "Okay");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);

	Item.Id = 10;
	strcpy(Item.pszString, "test sa mere");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);

	Item.Id = 16;
	strcpy(Item.pszString, "Morb la pute");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);
	
	Item.Id = 14;
	strcpy(Item.pszString, "Kui kui le lead code");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);
	
	Item.Id = 13;
	strcpy(Item.pszString, "BLah");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);

	Item.Id = 12;
	strcpy(Item.pszString, "Vive les crudites");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);

	Item.Id = 11;
	strcpy(Item.pszString, "Morb la chienne");
	BMUIOSElevatorAddItem ( (BMOSELEVATOR *)ObjElevator->BMOSInfos , Item);

//	BMOSOBJECT *objtexts;
//	BMOSTEXTSELECTIONNABLE *texts;

	BMUIOSTextSCreate (&ObjElevator, "Bonjour les connards", "normal", "selection", 200, 150, Object->id);



/*/
	KMLPackClose (f);
	return (BMTRUE);
}