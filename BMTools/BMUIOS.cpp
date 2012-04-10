//--------------------------------
// Includes
//--------------------------------

#include	"BMUIOS.h"
#include	"BMImage.h"
#include	"BMTimer.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD		BMOSMemory = 0;			//Allocate Memory

KMLIMAGE	*UIToolsImage;			//Image of the tools
BMLIST		*UIListObject;			//List of the objects created, BMOSOBJECT...
BMLIST		*UIStackWindow;			//Stack of the opened window
BMLIST		*UIListImage;			//List of loaded image, BMOSIMAGE...
UDWORD		OSFreeId;

//*********************************************************************
//* FUNCTION: BMUIOSStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the OS API (initialize lists...)
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:07:39
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSStart			( STRING *pszString )
{
	UIListObject	= BMListNew ();
	UIStackWindow	= BMListNew ();
	UIListImage		= BMListNew ();

	UIToolsImage	= KMLCreateImage (0, 0, KML_BMP | KML_MEM_SYSTEM, pszString);

	if (UIToolsImage == NULL)
		return (BMFALSE);

	KMLSetBackColor ( UIToolsImage, KML_SET);

	OSFreeId = 1; 
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSEnd
//*--------------------------------------------------------------------
//* DESCRIPT: End the OS API
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void 
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:08:01
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSEnd			()
{
	while (UIStackWindow->First->Next != NULL)
	{
		if (BMUIOSWindowClose () != BMTRUE)
			return (BMTRUE - 1);
	}
	//FREE LES LISTE NON VIDE
	BMListDelete (UIStackWindow);

	BMListDelete (UIListObject);

	BMListDelete (UIListImage);

	if (UIToolsImage != NULL)
	{
		KMLDeleteImage ( UIToolsImage );
		UIToolsImage = NULL;
	}
	return (BMTRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////// BMOSobject

//*********************************************************************
//* FUNCTION: BMUIOSObjectCreate
//*--------------------------------------------------------------------
//* DESCRIPT: Create the object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the new object
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:03:47
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectCreate		(BMOSOBJECT **NewObject)
{
	*NewObject = (BMOSOBJECT *) ALLOC2 (sizeof(BMOSOBJECT), BMOSMemory);

	if (*NewObject == NULL)
	{
		return (BMTRUE - 1);
	}
	
	(*NewObject)->BMOSChildren	= BMListNew();
	(*NewObject)->BMOSInfos		= NULL;
	(*NewObject)->BMOSType		= 0;
	(*NewObject)->id			= OSFreeId++;

	//Add to the list of object
	BMListAddTail (UIListObject, *NewObject);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSObjectDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the specified object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : object to delete
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:03:25
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectDelete		(BMOSOBJECT *DeleteObject)
{
	BMNODE		*Courant,
				*Suivant;
	BMOSOBJECT	*parent;

	Courant = UIListObject->First;

	while (Suivant = Courant->Next)
	{
		if ( Courant->Info == DeleteObject)
		{
			while ( ((BMOSOBJECT *)Courant->Info)->BMOSChildren->First->Next != NULL)
			{
				if (BMUIOSObjectDelete ( (BMOSOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSChildren->First->Info) != BMTRUE)	//recursive call !!!
					return (BMTRUE - 1);
				//we delete the node in the function which call (recursively) this one, it's done in the unlink !!!
			}
			BMListDelete ( ((BMOSOBJECT *)Courant->Info)->BMOSChildren);

			//Get Parent, and unlink
			if ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->parentid != 0)
			{
				BMNODE	*fCourant,
						*fSuivant;
				SBYTE	ok;

				BMUIOSFindObject ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->parentid, &parent);
				fCourant = parent->BMOSChildren->First;

				ok = 0;
				while ( (fSuivant = fCourant->Next) && (ok == 0) )
				{
					if ( (BMOSOBJECT *)fCourant->Info == DeleteObject)
					{
						BMDeleteNode (fCourant);
						ok = 1;
					}
					fCourant = fSuivant;
				}

				if (ok == 0)
					return (BMFALSE);
			}
			//Delete the attached info...
			if (DeleteObject->BMOSType == BMOS_WINDOW)
			{
				if(	BMUIOSWindowDelete ( (BMOSWINDOW *)DeleteObject->BMOSInfos) != BMTRUE)
					return (BMFALSE);
			}
			else if (DeleteObject->BMOSType == BMOS_TEXT)
			{
				if ( BMUIOSTextDelete ( (BMOSTEXT *)DeleteObject->BMOSInfos) != BMTRUE)
					return (BMFALSE);
			}
			else if (DeleteObject->BMOSType == BMOS_SELECTIONNABLETEXT)
			{
				if (BMUIOSTextSDelete ( (BMOSTEXTSELECTIONNABLE *) DeleteObject->BMOSInfos) != BMTRUE)
					return (BMFALSE);
			}
			else if (DeleteObject->BMOSType == BMOS_ELEVATOR)
			{
				if(	BMUIOSElevatorDelete ( (BMOSELEVATOR *)DeleteObject->BMOSInfos) != BMTRUE)
					return (BMFALSE);
			}
			else
			{
				KMLWarning ("type de l'element OS inconnu ??? ");
				return (BMTRUE - 1);
			}

			FREE2 (Courant->Info,BMOSMemory);
			BMDeleteNode (Courant);
			
			return (BMTRUE);
		}

		Courant = Suivant;
	}

	return (BMFALSE);	//Error, Object not found !!!
}

//*********************************************************************
//* FUNCTION: BMUIOSObjectDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the specified object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : id of the object
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:03:25
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectDelete		(UDWORD Id)
{
//	BMNODE	*Courant,
//			*Suivant;

	return (BMTRUE - 1);//revoir avec la fonction du dessus
/*	Courant = UIListObject->First;

	while (Suivant = Courant->Next)
	{
		if ( ((BMOSOBJECT *)Courant->Info)->id == Id)
		{
			while ( ((BMOSOBJECT *)Courant->Info)->BMOSChildren->First->Next != NULL)
			{
				BMOSOBJECT *Child;

				Child = (BMOSOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSChildren->First->Info;

				if (Child->BMOSType == BMOS_TEXT)
				{
					//call the delete function
				}
				else if (Child->BMOSType == BMOS_SELECTIONNABLETEXT)
				{
					;
				}
				else if (Child->BMOSType == BMOS_ELEVATOR)
				{
					;
				}
				else
				{
					KMLWarning ("type de l'element OS inconnu ??? ");
					return (BMTRUE - 1);
				}

				FREE2 (Child, BMOSMemory);
				BMDeleteNode ( ((BMOSOBJECT *)Courant->Info)->BMOSChildren->First);
			}
			FREE2 (Courant->Info,BMOSMemory);
			BMDeleteNode (Courant);
			return (BMTRUE);
		}
	}

	return (BMTRUE - 1);	//Error, Object not found !!!*/
}

//*********************************************************************
//* FUNCTION: BMUIOSFindObject
//*--------------------------------------------------------------------
//* DESCRIPT: Search the object in the BDD of object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : id of object, double ptr which is filled with the ptr to the object
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    12/01/2001 18:34:31
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSFindObject	(UDWORD Id,	BMOSOBJECT **pObject)
{
	BMNODE	*Courant;
	BMNODE	*Suivant;

	Courant = UIListObject->First;

	while (Suivant = Courant->Next)
	{
		if ( ((BMOSOBJECT *)Courant->Info)->id == Id)
		{
			*pObject = (BMOSOBJECT *) Courant->Info;
			return (BMTRUE);
		}
		Courant = Suivant;
	}

	*pObject = NULL;
	return (BMTRUE - 1);		//Error, object not found in the list !
}

//*********************************************************************
//* FUNCTION: BMUIOSObjectInsert
//*--------------------------------------------------------------------
//* DESCRIPT: insert an object into another if possible
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : base obj, obj to ins, coord of insertion, flag
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:29:56
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectInsert	(UDWORD BaseObjectId, UDWORD	ObjectToInsertId, UDWORD CoordX, UDWORD CoordY, UDWORD Flag)
{
	BMOSOBJECT	*BaseObj,
				*ObjToIns;
	if (BMUIOSFindObject (BaseObjectId, &BaseObj) != BMTRUE)
		return (BMTRUE - 1);								//Error, base object not found !

	if (BMUIOSFindObject (ObjectToInsertId, &ObjToIns) != BMTRUE)
		return (BMTRUE - 1);								//Error, object to insert not found !

	//insert the object into the object !!!
	if (ObjToIns->BMOSType == BMOS_WINDOW)	//special case to link window...
	{
		BMListAddTail (BaseObj->BMOSChildren, ObjToIns);
		return (BMTRUE);							
	}
	
	if (BaseObj->BMOSType != BMOS_WINDOW)		//generic case, you can add object only on window
		return (BMFALSE);

/*	if (ObjToIns->BMOSType != BMOS_WINDOW)
	{
		//verifying if id is unique
		BMNODE *Courant;
		BMNODE *Suivant;

		Courant = BaseObj->BMOSChildren->First;

		while ( Suivant = Courant->Next)
		{
			if ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->menuid == ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->menuid)	
				return (BMFALSE); //menu id already used

			Courant = Suivant;
		}
	}*/

/*	if ( (BaseObj->BMOSType == BMOS_TEXT) || (BaseObj->BMOSType == BMOS_SELECTIONNABLETEXT))
		return (BMTRUE - 1);								//Error, can't add an object to a text

	//-- specific object case
	if (BaseObj->BMOSType == BMOS_ELEVATOR)
	{
		if (	(ObjToIns->BMOSType != BMOS_SELECTIONNABLETEXT) 
			 && (ObjToIns->BMOSType != BMOS_SELECTIONNABLETEXT) )
			return (BMTRUE - 1);							//Impossible to add this type if object
	}*/

	//Test for the flag...
	//Does the coords match ?
	if (Flag == BMOS_NOTHING)
	{
		if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->width + CoordX > ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->width )
			return (BMTRUE - 1);

		if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->height + CoordY > ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->height )
			return (BMTRUE - 1);

		if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->x <= 0)
			return (BMTRUE - 1);
		
		if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->y <= 0)
			return (BMTRUE - 1);
	}
	else
	{
		if (Flag & (BMOS_CENTER_H | BMOS_RIGHT | BMOS_LEFT) )
		{
			if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->width > ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->width )
				return (BMTRUE - 1);
		}
		
		if (Flag & (BMOS_CENTER_V | BMOS_TOP | BMOS_BOTTOM) )
		{
			if ( ((BMOSBASEOBJECT *)ObjToIns->BMOSInfos)->height > ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->height )
				return (BMTRUE - 1);
		}

		//Apply the flag
		if (Flag & BMOS_CENTER_H)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->x = ( ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->width - ((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->width ) /2;
		else if (Flag & BMOS_RIGHT)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->x = ( ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->width - ((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->width );
		else if (Flag & BMOS_LEFT)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->x = 0;

		if (Flag & BMOS_CENTER_V)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->y = ( ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->height - ((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->height ) /2;
		else if (Flag & BMOS_BOTTOM)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->y = ( ((BMOSBASEOBJECT *)BaseObj->BMOSInfos)->height - ((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->height);
		else if (Flag & BMOS_TOP)
			((BMOSBASEOBJECT *) ObjToIns->BMOSInfos)->y = 0;
	}

	BMListAddTail (BaseObj->BMOSChildren, ObjToIns);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSObjectCreateImage
//*--------------------------------------------------------------------
//* DESCRIPT: Create an image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : filename&path, ptr to the image created
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 11:29:45
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectCreateImage (STRING *pszFile, KMLIMAGE **NewImage)
{
	BMNODE		*Courant,
				*Suivant;
	STRING		Path[255];
	STRING		FileName[255];
	SDWORD		i;
	BMOSIMAGE	*osimage;

	//Search for path and filename
	strcpy(Path, pszFile);
	i = strlen (Path) - 1;

	while ( (Path [i] != '/') && (Path [i] != '\\') && (i != 0) )
		i--;

	if (i != 0)
	{
		strcpy( FileName, &Path [i + 1]);
		Path [i + 1] = '\0';
	}
	else
	{
		strcpy( FileName, Path);
		Path[0] = '\0';
	}
	
	//Search the image in the BDD
	Courant = UIListImage->First;
	while (Suivant = Courant->Next)
	{
		osimage = (BMOSIMAGE *)Courant->Info;
		if ( strcmp( osimage->pszFileName, FileName) == 0)
		{
			osimage->NumberUsed++;

			*NewImage = osimage->PtrImage;

			return (BMTRUE);
		}

		Courant = Suivant;
	}
	//image not loaded, create it !
	
	*NewImage = BMImageCreate(0, 0, KML_BMP, pszFile);

	if (*NewImage == NULL)		
		return (BMTRUE - 1);		//Error, cannot create the image, verify path and filename

	osimage = (BMOSIMAGE *) ALLOC2 (sizeof (BMOSIMAGE), BMOSMemory);
	
	osimage->NumberUsed		= 1;
	osimage->pszFileName	= (STRING *) ALLOC2 (strlen(FileName) + 1,BMOSMemory);
	osimage->PtrImage		= *NewImage;
	strcpy( osimage->pszFileName, FileName);

	BMListAddTail (UIListImage, osimage);
	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSObjectDeleteImage 
//*--------------------------------------------------------------------
//* DESCRIPT: delete the specified image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to image
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 11:30:26
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSObjectDeleteImage (KMLIMAGE *PtrImage)
{
	BMNODE	*Courant,
			*Suivant;

	Courant = UIListImage->First;
	while (Suivant = Courant->Next)
	{
		if ( ((BMOSIMAGE *)Courant->Info)->PtrImage == PtrImage)
		{
			((BMOSIMAGE *)Courant->Info)->NumberUsed--;

			if ( ((BMOSIMAGE *)Courant->Info)->NumberUsed == 0)		//if the image is no longer used delete it
			{
				BMImageDelete (PtrImage);

				FREE2 ( ((BMOSIMAGE *)Courant->Info)->pszFileName,BMOSMemory);
				FREE2 (Courant->Info, BMOSMemory);
				BMDeleteNode(Courant);
			}

			return (BMTRUE);
		}

		Courant = Suivant;
	}

	return (BMTRUE - 1);			//Error, image not found
}

//*********************************************************************
//* FUNCTION: BMUIOSDisplayCurrent
//*--------------------------------------------------------------------
//* DESCRIPT: Display the current window
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : nothing
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 12:32:05
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSDisplayCurrent	()
{
	//Display the current window...
	if (UIStackWindow->First->Next != NULL) 
	{
		BMOSOBJECT *Obj;

		//Obj = (BMOSOBJECT *)UIStackWindow->First->Info;07/03/2001 14:12:05
		Obj = (BMOSOBJECT *)UIStackWindow->Last->Info;//the last window is the currently open

		if (BMUIOSDisplayObject (Obj) != BMTRUE)
			return (BMTRUE - 1);

		return (BMTRUE);
	}

	return (BMTRUE - 1);
}

//**************************************************************
/** \fn     BMOSOBJECT *BMUIOSGetCurrentWindow()
 *
 *  \param  void 
 *
 *  \return  the currnent window
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 08/03/2001 15:10:02
 ***************************************************************/
BMOSOBJECT *BMUIOSGetCurrentWindow()
{
	if (UIStackWindow->First->Next != NULL) 
		return ( (BMOSOBJECT *)UIStackWindow->Last->Info);
	else
		return NULL;
}

//*********************************************************************
//* FUNCTION: BMUIOSDisplayObject
//*--------------------------------------------------------------------
//* DESCRIPT: display the object and its children
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : obj to display
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 12:32:30
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSDisplayObject (BMOSOBJECT *Obj)
{
	BMNODE	*Courant,
			*Suivant;

	//display object
	if (Obj->BMOSType == BMOS_WINDOW)
	{
		if (BMUIOSWindowDisplay ( (BMOSWINDOW *)Obj->BMOSInfos) != BMTRUE)
			return (BMFALSE);
	}
	else if (Obj->BMOSType == BMOS_TEXT)
	{
		if (BMUIOSTextDisplay ( (BMOSTEXT *)Obj->BMOSInfos) != BMTRUE)
			return (BMFALSE);
	}
	else if (Obj->BMOSType == BMOS_SELECTIONNABLETEXT)
	{
		if (BMUIOSTextSDisplay ( (BMOSTEXTSELECTIONNABLE *)Obj->BMOSInfos) != BMTRUE)
			return (BMFALSE);
	}
	else if (Obj->BMOSType == BMOS_ELEVATOR)
	{
		if (BMUIOSElevatorDisplay ( (BMOSELEVATOR *)Obj->BMOSInfos) != BMTRUE)
			return (BMFALSE);

	}

	//display child
	Courant = Obj->BMOSChildren->First;

	while (Suivant = Courant->Next)
	{
		BMOSOBJECT *Child;

		Child = (BMOSOBJECT *)Courant->Info;
		if (BMUIOSDisplayObject (Child) != BMTRUE)
			return (BMFALSE);

		Courant = Suivant;
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSObjectSetMenuId (BMOSOBJECT *NewObject, UDWORD MenuId)
 *
 *  \param  *NewObject 
 *  \param  MenuId     
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 16/03/2001 14:00:24
 ***************************************************************/
SBYTE		BMUIOSObjectSetMenuId	(BMOSOBJECT *NewObject, UDWORD MenuId)
{
	//verifying if id is unique
	BMOSOBJECT	*BaseObj;
	BMNODE		*Courant;
	BMNODE		*Suivant;

	if ( ((BMOSBASEOBJECT *)NewObject->BMOSInfos)->parentid == 0)
		return BMTRUE;;

	if (BMUIOSFindObject ( ((BMOSBASEOBJECT *)NewObject->BMOSInfos)->parentid, &BaseObj) != BMTRUE)
		return (BMFALSE);

	Courant = BaseObj->BMOSChildren->First;

	while ( Suivant = Courant->Next)
	{
		if ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->menuid != (UDWORD) -1)
		{
			if ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->menuid == MenuId)	
				return (BMFALSE); //menu id already used
		}
		Courant = Suivant;
	}

	((BMOSBASEOBJECT *)NewObject->BMOSInfos)->menuid = MenuId;

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSGetObjectByMenuId (BMOSOBJECT **Object, UDWORD MenuId )
 *
 *  \param  **Object 
 *  \param  MenuId   
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 16/03/2001 14:25:28
 ***************************************************************/
SBYTE	BMUIOSGetObjectByMenuId (BMOSOBJECT **Object, UDWORD MenuId )
{
	if (UIStackWindow->First->Next != NULL) 
	{
		BMNODE		*Courant,
					*Suivant;
		BMOSOBJECT	*Obj;

		if (MenuId == (UDWORD) -1)
			return (BMFALSE);

		Obj = (BMOSOBJECT *)UIStackWindow->Last->Info;//the last window is the currently open

		Courant = Obj->BMOSChildren->First;

		while (Suivant = Courant->Next)
		{
			if ( ((BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos)->menuid == MenuId )
			{
				*Object = (BMOSOBJECT *)Courant->Info;
				return (BMTRUE);
			}
			Courant = Suivant;
		}

		return (BMTRUE);
	}

	return (BMFALSE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSObjectManage (BMOSOBJECT *Object, KMLMOUSE mousestate)
 *
 *  \param  *Object   
 *  \param  mousestate
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 19/03/2001 13:50:44
 ***************************************************************/
SBYTE	BMUIOSObjectManage	(BMOSOBJECT *Object, KMLMOUSE mousestate)
{
	switch (Object->BMOSType)
	{
	case BMOS_TEXT:
		return (BMUIOSTextManage ( (BMOSTEXT *)Object->BMOSInfos, mousestate) );
		break;
	case BMOS_SELECTIONNABLETEXT:
		return (BMUIOSTextSManage ( (BMOSTEXTSELECTIONNABLE *)Object->BMOSInfos, mousestate) );
		break;
	case BMOS_ELEVATOR:
		return (BMUIOSElevatorManage ( (BMOSELEVATOR *)Object->BMOSInfos, mousestate) );
		break;
	default:
		exit (-1023);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////// BMOSWindow

//*********************************************************************
//* FUNCTION: BMUIOSWindowCreate
//*--------------------------------------------------------------------
//* DESCRIPT: Create a window
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the newobject, string background (can be NULL), flag background
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 18:09:03
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSWindowCreate		(BMOSOBJECT **NewObject, STRING *pszFile, UDWORD Flag)
{
	BMOSWINDOW *BMWindow;
	
	BMUIOSObjectCreate (NewObject);

	(*NewObject)->BMOSType		= BMOS_WINDOW;
	(*NewObject)->BMOSInfos		= ALLOC2 (sizeof(BMOSWINDOW),BMOSMemory);
	
	BMWindow = (BMOSWINDOW *) (*NewObject)->BMOSInfos;
	BMWindow->height	= 480;
	BMWindow->width		= 640;
	BMWindow->x			= 0;
	BMWindow->y			= 0;
	BMWindow->menuid	= -1;
	BMWindow->BackGrd	= NULL;
	BMWindow->Flag		= 0;

	if (pszFile != NULL)
	{
		if (BMUIOSObjectCreateImage (pszFile, &BMWindow->BackGrd) != BMTRUE)
			return (BMTRUE - 1);

		if ( !(Flag & (BMOS_BACKMOSAIQUE | BMOS_BACKSTRETCH)) )
			Flag |= BMOS_BACKSTRETCH;
	}

	BMWindow->Flag		= Flag;

	if (UIStackWindow->First->Next == NULL)
		BMWindow->parentid	= 0;
	else
	{
		BMWindow->parentid	= ((BMOSOBJECT *) UIStackWindow->Last->Info)->id;
		//link in the parent child object
		if (BMUIOSObjectInsert ( /*IdParent*/((BMOSOBJECT *) UIStackWindow->Last->Info)->id, (*NewObject)->id, 0, 0, 0) != BMTRUE)
			return (BMTRUE - 1);
	}

	//add to the new window to the stack
	BMListAddTail (UIStackWindow, *NewObject);
	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSWindowLoadBMP (BMOSWINDOW *Window, STRING *pszFile, UDWORD Flag = BMOS_NOTHING)
 *
 *  \param  *Window            
 *  \param  *pszFile           
 *  \param  Flag = BMOS_NOTHING
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 08/03/2001 14:09:58
 ***************************************************************/
SBYTE		BMUIOSWindowLoadBMP		(BMOSWINDOW *Window, STRING *pszFile, UDWORD Flag)
{
	if (pszFile == NULL)
		return (BMFALSE);

	if (Window->BackGrd != NULL)
	{	
		if (BMUIOSObjectDeleteImage (Window->BackGrd) != BMTRUE)
			return (BMTRUE - 1);
	}

	if (BMUIOSObjectCreateImage (pszFile, &Window->BackGrd) != BMTRUE)
		return (BMTRUE - 1);

	if ( !(Flag & (BMOS_BACKMOSAIQUE | BMOS_BACKSTRETCH)) )
		Flag |= BMOS_BACKSTRETCH;

	Window->Flag = Flag;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSWindowClose
//*--------------------------------------------------------------------
//* DESCRIPT: Close the last open window
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : nothing
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 11:59:07
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSWindowClose	()
{
	if (UIStackWindow->Last->Next != NULL)
	{
		if (BMUIOSObjectDelete ( (BMOSOBJECT *)UIStackWindow->Last->Info) != BMTRUE)
			return (BMTRUE - 1);		//Error, can't delete the specified object !!!

		BMDeleteNode (UIStackWindow->Last);
		return (BMTRUE);
	}

	return (BMTRUE - 1);		//Error, No more window !*/
}

//*********************************************************************
//* FUNCTION: BMUIOSWindowDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the specified window (call by the bmuiosobjectdelete only !!!)
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the window
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 12:01:22
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSWindowDelete	(BMOSWINDOW *objtodelete)
{
	if (objtodelete->BackGrd != NULL)
	{	
		if (BMUIOSObjectDeleteImage (objtodelete->BackGrd) != BMTRUE)
			return (BMTRUE - 1);
	}
	FREE2 (objtodelete, BMOSMemory);
	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSWindowDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: display the window
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the window
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 12:37:10
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSWindowDisplay (BMOSWINDOW *Window)
{
	if (Window->BackGrd == NULL)
		return (BMTRUE);

	if (Window->Flag & BMOS_BACKMOSAIQUE)
	{
		UDWORD	i,
				j;

		j = 0;

		while (j < Window->height)
		{
			i = 0;
			while (i < Window->width)
			{
				KMLFastBlt (NULL, i, j, Window->BackGrd, 0, 0, Window->BackGrd->Largeur, Window->BackGrd->Hauteur);
				i += Window->BackGrd->Largeur;
			}

			j+= Window->BackGrd->Hauteur;
		}

	}
	else if (Window->Flag &	BMOS_BACKSTRETCH)
	{
		KMLStretchBlt (NULL, 0, 0, 640, 480, Window->BackGrd, 0, 0, Window->BackGrd->Largeur, Window->BackGrd->Hauteur);
	}
	
	return (BMTRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////// BMOSText

//*********************************************************************
//* FUNCTION: BMUIOSTextCreate
//*--------------------------------------------------------------------
//* DESCRIPT: create a text and insert it into the parent object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : new obj, text, coordx, coordy (insert), parent id, flag, note that pszFontName can be NULL, so it takes the current font
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 17:12:15
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSTextCreate		(BMOSOBJECT	**NewObject, STRING	*pszText, STRING *pszFontName, UDWORD CoordX, UDWORD CoordY, UDWORD IdParent, UDWORD Flag)
{
	BMOSTEXT	*BMText;
	BMOSOBJECT	*parent;
	BMFONT		*font;
	
	if ((pszFontName == NULL) && (BMUICurrentFont == NULL))
		return (BMFALSE);

	if ( pszFontName == NULL)
		font = BMUICurrentFont;
	else
	{
		font = BMUIFontGetFont (pszFontName);
		if (font == NULL)
			return (BMFALSE);
	}

	if (IdParent == 0)
		return (BMFALSE);			//Error, can't add the text if the id is null, no parent...

	if (BMUICurrentFont == NULL)
		return (BMFALSE);

	if (BMUIOSFindObject (IdParent, &parent) != BMTRUE)
		return (BMFALSE);

	BMUIOSObjectCreate (NewObject);

	(*NewObject)->BMOSType		= BMOS_TEXT;
	(*NewObject)->BMOSInfos		= ALLOC2 (sizeof(BMOSTEXT),BMOSMemory);
	
	BMText = (BMOSTEXT *) (*NewObject)->BMOSInfos;
	BMText->width 		= (font->SizeX + font->FontSpaceX) * strlen(pszText);
	BMText->height 		= font->SizeY + font->FontSpaceY;
	BMText->x			= CoordX;
	BMText->y			= CoordY;
	BMText->pszText		= (STRING *)ALLOC2 (strlen(pszText) + 1,BMOSMemory);
	strcpy(BMText->pszText, pszText);
	BMText->parentid	= IdParent;
	BMText->menuid		= -1;
	BMText->pszFontName = (STRING *)ALLOC2 (strlen(font->pszFontName) + 1,BMOSMemory);
	strcpy (BMText->pszFontName, font->pszFontName);

	if (BMUIOSObjectInsert ( IdParent, (*NewObject)->id, CoordX, CoordY, Flag) != BMTRUE)
		return (BMFALSE);
	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSTextDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the text object
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : text object to delete
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 17:13:08
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSTextDelete		(BMOSTEXT	*objtodelete)
{
	FREE2 (objtodelete->pszText, BMOSMemory);
	FREE2 (objtodelete->pszFontName, BMOSMemory);
	FREE2 (objtodelete, BMOSMemory);
	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMUIOSTextDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: display the text
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : text obj
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/01/2001 17:14:04
//* REVISION:									
//*********************************************************************
SBYTE		BMUIOSTextDisplay		(BMOSTEXT	*Text)
{
	BMFONT *font;

	font = BMUICurrentFont;

	if (BMUIFontSelect (Text->pszFontName) != BMTRUE)
		return (BMFALSE);

	BMUIFontDrawString (Text->pszText, Text->x, Text->y, NULL);

	if (font != NULL)
	{
		if (BMUIFontSelect (font->pszFontName) != BMTRUE)
			return (BMFALSE);
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSTextManage (BMOSTEXT *Text, KMLMOUSE mousestate)
 *
 *  \param  *Text     
 *  \param  mousestate
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 13:41:33
 ***************************************************************/
SBYTE		BMUIOSTextManage (BMOSTEXT	*Text, KMLMOUSE mousestate)
{
	if ( KMLControlMouseButtonPress(0) )
	{
		return BMTRUE;
	}

	return (BMFALSE);
}


//*********************************************************************
//* FUNCTION: BMUIOSWhosObject
//*--------------------------------------------------------------------
//* DESCRIPT: get the object on coord x y
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : object pointed, x, y
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    
//* REVISION:									
//*********************************************************************
SBYTE	BMUIOSWhosObject		(BMOSOBJECT **ObjectPointed, UDWORD CoordX, UDWORD CoordY)
{
	//the currrent window is the last in the stack
	BMNODE			*Courant;
	BMNODE			*Suivant;
	BMOSOBJECT		*CurrentWindow;
	BMOSBASEOBJECT	*Object;
	
	CurrentWindow = (BMOSOBJECT *) UIStackWindow->Last->Info;			//Stack of the opened window

	if ( CurrentWindow->BMOSChildren == NULL)
	{
		*ObjectPointed = NULL;
		return (BMTRUE);
	}
	Courant = CurrentWindow->BMOSChildren->First;

	while (Suivant = Courant->Next)
	{
		Object = (BMOSBASEOBJECT *)((BMOSOBJECT *)Courant->Info)->BMOSInfos;

		if (	(CoordX >= Object->x)
			&&	(CoordX <= Object->x + Object->width)
			&&  (CoordY >= Object->y)
			&&	(CoordY <= Object->y + Object->height) )
		{	
			*ObjectPointed = (BMOSOBJECT *)Courant->Info;		//an object is found
			return (BMTRUE);
		}
	
		Courant = Suivant;
	}
	
	*ObjectPointed = NULL;

	return (BMTRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////// BMOSElevator

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorCreate (BMOSOBJECT **NewObject, UDWORD CoordX, UDWORD CoordY, UDWORD SizeX, UDWORD SizeY, UDWORD IdParent, UDWORD Flag)
 *
 *  \param  **NewObject 
 *  \param  CoordX      
 *  \param  CoordY      
 *  \param  SizeX       
 *  \param  SizeY       
 *  \param  IdParent    
 *  \param  Flag        
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 13/03/2001 13:11:46
 ***************************************************************/
SBYTE	BMUIOSElevatorCreate		(BMOSOBJECT	**NewObject, STRING *pszFontName,UDWORD CoordX, UDWORD CoordY, UDWORD SizeX, UDWORD SizeY, UDWORD IdParent, STRING* BackGround,UDWORD FlagBack)
{
	BMOSELEVATOR	*BMElevator;
	BMOSOBJECT		*parent;

	if (pszFontName == NULL && BMUICurrentFont == NULL)
		return (BMFALSE);

	if (pszFontName == NULL)
		strcpy( pszFontName, BMUICurrentFont->pszFontName);
	
	if (BMUIFontGetFont(pszFontName) == NULL)
		return (BMFALSE);

	if (IdParent == 0)
		return (BMFALSE);			//Error, can't add the elevator if the id is null, no parent...

	if (BMUIOSFindObject (IdParent, &parent) != BMTRUE)
		return (BMFALSE);

	BMUIOSObjectCreate (NewObject);

	(*NewObject)->BMOSType		= BMOS_ELEVATOR;
	(*NewObject)->BMOSInfos		= ALLOC2 (sizeof(BMOSELEVATOR),BMOSMemory);
	
	BMElevator = (BMOSELEVATOR *) (*NewObject)->BMOSInfos;

	BMElevator->width 			= (SizeX < BMOS_ELEVATOR_WIDHTMIN ? BMOS_ELEVATOR_WIDHTMIN : SizeX);
	BMElevator->height 			= (SizeY < BMOS_ELEVATOR_HEIGHTMIN ? BMOS_ELEVATOR_HEIGHTMIN : SizeY);
	BMElevator->x				= CoordX;
	BMElevator->y				= CoordY;
	BMElevator->parentid		= IdParent;
	BMElevator->menuid			= -1;
	BMElevator->itemslist		= BMListNew ();
	BMElevator->selectionitem	= NULL;
	BMElevator->currenttopitem	= NULL;
	BMElevator->currentchar		= 0;
	BMElevator->timeleft		= BMTimerGetTime () + BMOS_ELEVATOR_TIMER;
	BMElevator->flagback		= FlagBack;
	BMElevator->max_carac		= ( BMElevator->width - BMOS_ELEVATOR_BUTTONL_SX - (BMOS_ELEVATOR_BOXY_SX << 1) ) / BMUIFontGetFont(pszFontName)->SizeX;
	BMElevator->pszFontName		= (STRING *) ALLOC2 (strlen(BMUICurrentFont->pszFontName)+ 1, BMOSMemory);
	BMElevator->bsens			= BMFALSE;

	strcpy	(BMElevator->pszFontName, BMUICurrentFont->pszFontName);

	if (BackGround != NULL)
		BMElevator->background = KMLCreateImage (0, 0, KML_BMP, BackGround);
	else
		BMElevator->background	= NULL;


	if (BMUIOSObjectInsert ( IdParent, (*NewObject)->id, CoordX, CoordY, FlagBack) != BMTRUE)
		return (BMTRUE - 1);
	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorDelete (BMOSELEVATOR *objtodelete)
 *
 *  \param  *objtodelete
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 13/03/2001 13:19:41
 ***************************************************************/
SBYTE		BMUIOSElevatorDelete		(BMOSELEVATOR *objtodelete)
{
	BMNODE	*Courant;

	Courant = objtodelete->itemslist->First;

	while (objtodelete->itemslist->First->Next != NULL)
	{
		BMOSBOXTEXT *obj;

		obj = (BMOSBOXTEXT *) objtodelete->itemslist->First->Info;
		FREE2 (obj->pszString, BMOSMemory);
		FREE2 (obj, BMOSMemory);

		BMDeleteNode (objtodelete->itemslist->First);
	}
	BMListDelete (objtodelete->itemslist);
	FREE2 (objtodelete, BMOSMemory);
	FREE2 (objtodelete->pszFontName, BMOSMemory);
	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorDisplay (BMOSELEVATOR *Elevator)
 *
 *  \param  *Elevator
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 13/03/2001 14:20:32
 ***************************************************************/
SBYTE		BMUIOSElevatorDisplay		(BMOSELEVATOR *Elevator)
{
	SDWORD	CurrentX,
			CurrentY,
			SizeX,
			SizeY;
	UDWORD	i,
			j;
	BMNODE  *Courant,
			*Suivant;
	RECT	rect;

	//Draw the background
	if (Elevator->background != NULL)
	{
		if (Elevator->flagback & BMOS_BACKMOSAIQUE)
		{
			j = Elevator->y + BMOS_ELEVATOR_BOXY_SX;

			SizeY = Elevator->background->Hauteur;
			SizeX = Elevator->background->Largeur;
			while (j < Elevator->y + Elevator->height - ( BMOS_ELEVATOR_BOXY_SX ) )
			{
				if ( j > Elevator->y + Elevator->height - ( BMOS_ELEVATOR_BOXY_SX ) - SizeY)
					SizeY = Elevator->y + Elevator->height - ( BMOS_ELEVATOR_BOXY_SX ) - j;
				i = Elevator->x + BMOS_ELEVATOR_BOXX_SY;

				while (i < Elevator->x + Elevator->width - ( BMOS_ELEVATOR_BOXX_SY ))
				{
					if ( i > Elevator->x + Elevator->width - ( BMOS_ELEVATOR_BOXX_SY ) - SizeX)
						SizeX = Elevator->x + Elevator->width - ( BMOS_ELEVATOR_BOXX_SY ) - i;
					else 
						SizeX = Elevator->background->Largeur;
					KMLFastBlt (NULL, i, j, Elevator->background, 0, 0, SizeX, SizeY);
					i += SizeX;
				}

				j+= SizeY;
			}

		}
		else if (Elevator->flagback & BMOS_BACKSTRETCH)
		{
			KMLStretchBlt (NULL, Elevator->x + BMOS_ELEVATOR_BOXX_SY, Elevator->y + BMOS_ELEVATOR_BOXY_SX, Elevator->width - (BMOS_ELEVATOR_BOXX_SY << 1), Elevator->height - (BMOS_ELEVATOR_BOXY_SX << 1), Elevator->background, 0, 0, Elevator->background->Largeur, Elevator->background->Hauteur);
		}
	}

	//Drawning the box

	//horizontal
	for (i = 0; i < 3; i++)
	{
		CurrentX = Elevator->x + BMOS_ELEVATOR_BOXY_SX;
		if (i == 0) 
			CurrentY = Elevator->y;
		else if ( i == 1 )
			CurrentY = Elevator->y + Elevator->height - BMOS_ELEVATOR_BOXX_SY;
		else
			CurrentY = Elevator->y + Elevator->height - (BMOS_ELEVATOR_BOXX_SY << 1) - BMOS_ELEVATOR_SCROLL_SY;
		
		SizeX = Elevator->width + Elevator->x - BMOS_ELEVATOR_BOXY_SX - CurrentX;
		while ( SizeX > BMOS_ELEVATOR_BOXX_SX )
		{
			KMLFastBlt ( NULL, CurrentX, CurrentY, UIToolsImage, BMOS_ELEVATOR_BOXX_X, BMOS_ELEVATOR_BOXX_Y, BMOS_ELEVATOR_BOXX_SX, BMOS_ELEVATOR_BOXX_SY );
			CurrentX += BMOS_ELEVATOR_BOXX_SX;
			SizeX = Elevator->width + Elevator->x - BMOS_ELEVATOR_BOXY_SX - CurrentX;
		}
		
		if ( SizeX > 0 )
				KMLFastBlt ( NULL, CurrentX, CurrentY, UIToolsImage, BMOS_ELEVATOR_BOXX_X, BMOS_ELEVATOR_BOXX_Y, SizeX, BMOS_ELEVATOR_BOXX_SY );
	}

	//vertical
	for (i = 0; i < 2; i++)
	{
		if (i == 0)
			CurrentX = Elevator->x;
		else
			CurrentX = Elevator->x + Elevator->width - BMOS_ELEVATOR_BOXY_SX;
		
		CurrentY = Elevator->y + BMOS_ELEVATOR_BOXX_SY;
		
		SizeY = Elevator->height + Elevator->y - BMOS_ELEVATOR_BOXX_SY - CurrentY;
		while ( SizeY > BMOS_ELEVATOR_BOXY_SY )
		{
			KMLFastBlt ( NULL, CurrentX, CurrentY, UIToolsImage, BMOS_ELEVATOR_BOXY_X, BMOS_ELEVATOR_BOXY_Y, BMOS_ELEVATOR_BOXY_SX, BMOS_ELEVATOR_BOXY_SY );
			CurrentY += BMOS_ELEVATOR_BOXY_SY;
			SizeY = Elevator->height + Elevator->y - BMOS_ELEVATOR_BOXX_SY - CurrentY;
		}
		
		if ( Elevator->height + Elevator->y - BMOS_ELEVATOR_BOXX_SY - CurrentY > 0 )
			KMLFastBlt ( NULL, CurrentX, CurrentY, UIToolsImage, BMOS_ELEVATOR_BOXY_X, BMOS_ELEVATOR_BOXY_Y, BMOS_ELEVATOR_BOXY_SX, SizeY );
	}

	for (i = 0; i < 4; i++)
	{
		if (i & 1)
			CurrentX = Elevator->x;
		else
			CurrentX = Elevator->x + Elevator->width - BMOS_ELEVATOR_CORN_SX;

		if (i & 2)
			CurrentY = Elevator->y;
		else
			CurrentY = Elevator->y + Elevator->height - BMOS_ELEVATOR_CORN_SY;

		KMLFastBlt ( NULL, CurrentX, CurrentY, UIToolsImage, BMOS_ELEVATOR_CORN_X, BMOS_ELEVATOR_CORN_Y, BMOS_ELEVATOR_CORN_SX, BMOS_ELEVATOR_CORN_SY );
	}


	//button left
	BMUIOSElevatorGetLButton ( Elevator, &rect ); 
	//CurrentX = Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	//CurrentY = Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY;//BMOS_ELEVATOR_BOXX_SY;
	KMLFastBlt ( NULL, rect.left, rect.top, UIToolsImage, BMOS_ELEVATOR_BUTTONL_X, BMOS_ELEVATOR_BUTTONL_Y, BMOS_ELEVATOR_BUTTONL_SX, BMOS_ELEVATOR_BUTTONL_SY );

	//button right
	//	CurrentX = Elevator->x + (Elevator->width >> 1) + 5;
	// CurrentY = Elevator->y + BMOS_ELEVATOR_BOXX_SY;
	BMUIOSElevatorGetRButton ( Elevator, &rect ); 
	KMLFastBlt ( NULL, rect.left, rect.top, UIToolsImage, BMOS_ELEVATOR_BUTTONR_X, BMOS_ELEVATOR_BUTTONR_Y, BMOS_ELEVATOR_BUTTONR_SX, BMOS_ELEVATOR_BUTTONR_SY );

	//background button
	//CurrentX = Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	//CurrentY = Elevator->y + (BMOS_ELEVATOR_BOXX_SY << 1);
	BMUIOSElevatorGetBackGrButton (Elevator, &rect);

	while (rect.top < rect.bottom/*Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY*/)
	{
		UDWORD y;

		if ( rect.top + BMOS_BACK_SY < rect.bottom/*Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY*/)
			y = BMOS_BACK_SY;
		else
			y = rect.bottom - rect.top/*Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY - CurrentY*/;

		KMLFastBlt (NULL, rect.left, rect.top, UIToolsImage, BMOS_BACK_X, BMOS_BACK_Y, BMOS_BACK_SX, y);

		rect.top += BMOS_BACK_SY;
	}

	//display the item text
	Courant = Elevator->currenttopitem;
	

	if (Courant != NULL)
	{	
		BMFONT	*font;

		font = BMUICurrentFont;
		if (BMUIFontSelect (Elevator->pszFontName) != BMTRUE)
			return (BMFALSE);

		CurrentX = Elevator->x + BMOS_ELEVATOR_BOXY_SX + 5;
		CurrentY = Elevator->y + BMOS_ELEVATOR_BOXY_SX + 5;
		while ( (Suivant = Courant->Next) && ( unsigned(CurrentY + BMUICurrentFont->SizeY ) < Elevator->y + 
																						Elevator->height - 
																						(BMOS_ELEVATOR_BOXX_SY << 1)  - 
																						BMOS_ELEVATOR_SCROLL_SY	) )
		{
			UDWORD	max_carac;
			STRING	string[255];
			//we can display a text

			//calcul the max of character we can display on horizontal
			max_carac = ( Elevator->width - 10 - (BMOS_ELEVATOR_BOXY_SX << 1)) / BMUICurrentFont->SizeX;

			strncpy( string, ((BMOSBOXTEXT *)Courant->Info)->pszString, max_carac);
			string [max_carac] = '\0';
			BMUIFontDrawString (string, CurrentX, CurrentY, NULL);

			CurrentY += BMUICurrentFont->SizeY;
			Courant = Suivant;
		}

		if (font != NULL)
		{
			if (BMUIFontSelect (font->pszFontName) != BMTRUE)
				return (BMFALSE);
		}
	}

	//display the selectionned text
	if (Elevator->selectionitem != NULL)
	{
		STRING string2 [255];
		UBYTE  j;
		UBYTE  maxcaracspecific;

		BMFONT *font;

		font = BMUICurrentFont;
		if (BMUIFontSelect (Elevator->pszFontName) != BMTRUE)
			return (BMFALSE);

		CurrentX = Elevator->x + BMOS_ELEVATOR_BOXY_SX;
		CurrentY = Elevator->y + Elevator->height - BMOS_ELEVATOR_BOXX_SY - BMOS_ELEVATOR_SCROLL_SY;

		maxcaracspecific = (UBYTE) (( Elevator->width - (BMOS_ELEVATOR_BOXY_SX << 1) ) / BMUICurrentFont->SizeX);

		if (strlen (((BMOSBOXTEXT *)Elevator->selectionitem->Info)->pszString) > maxcaracspecific)
		{
			//update time
			if (BMTimerGetTime () > Elevator->timeleft)
			{
				if (Elevator->bsens == BMFALSE)
				{
					Elevator->currentchar++;
					if ( (unsigned)Elevator->currentchar > strlen( ((BMOSBOXTEXT *)Elevator->selectionitem->Info)->pszString ) - maxcaracspecific)
					{
						Elevator->currentchar = strlen (((BMOSBOXTEXT *)Elevator->selectionitem->Info)->pszString ) - maxcaracspecific
							;
						Elevator->bsens = BMTRUE;
					}
				}
				else
				{
					if (Elevator->currentchar <= 0)
					{
						Elevator->currentchar = 0;
						Elevator->bsens = BMFALSE;
					}
					else
						Elevator->currentchar--;

				}

				Elevator->timeleft = BMTimerGetTime () + BMOS_ELEVATOR_TIMER;
			}
		}

		j = 0;
		for (i = Elevator->currentchar; (i <(unsigned) (Elevator->currentchar + maxcaracspecific) ) && (((BMOSBOXTEXT *)Elevator->selectionitem->Info)->pszString [i] != '\0'); i++)
		{
			string2 [j] = ((BMOSBOXTEXT *)Elevator->selectionitem->Info)->pszString [i];
			j++;
		}
		string2[maxcaracspecific] = '\0';
		BMUIFontDrawString (string2, CurrentX, CurrentY, NULL);

		if (font != NULL)
		{
			if (BMUIFontSelect (font->pszFontName) != BMTRUE)
			return (BMFALSE);
		}
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorAddItem (BMOSELEVATOR *Elevator, BMOSBOXTEXT NewItem)
 *
 *  \param  *Elevator 
 *  \param  NewItem   
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 13/03/2001 14:20:40
 ***************************************************************/
SBYTE	BMUIOSElevatorAddItem		(BMOSELEVATOR *Elevator, BMOSBOXTEXT NewItem)
{
	BMOSBOXTEXT *newtext;
	UBYTE		i,
				j;

	newtext				= (BMOSBOXTEXT *) ALLOC2 ( sizeof(BMOSBOXTEXT), BMOSMemory);
	newtext->pszString	= (STRING *) ALLOC2 ( strlen(NewItem.pszString) + 1, BMOSMemory);
	newtext->Id			= NewItem.Id;
	strcpy (newtext->pszString, NewItem.pszString );
	
	strcpy (newtext->pszString2, "*** ");
	strcat (newtext->pszString2, newtext->pszString);
	strcat (newtext->pszString2, " ***");
	
	j = 0;
	for (i = strlen (newtext->pszString2); i < 255; i++)
	{
		newtext->pszString2[i] = newtext->pszString2[j];
		j++;
	}
	newtext->pszString2[i] = '\0';

	BMListAddTail (Elevator->itemslist, newtext);

	if (Elevator->currenttopitem == NULL)
		Elevator->currenttopitem = Elevator->itemslist->First;

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorManage (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
 *
 *  \param  *Elevator 
 *  \param  MouseState
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  call this function when a mouse button is down
 *
 *  \author MikE                       \date 13/03/2001 14:28:56
 ***************************************************************/
SBYTE	BMUIOSElevatorManage		(BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
{
	SWORD	item;
	BMNODE	*courant;

	if ( KMLControlMouseButtonPress(0) ) //button 0 is down
	{
		//what was touched ?
		if ( BMUIOSElevatorIsInLButton (Elevator, MouseState) == BMTRUE)
		{
			if (Elevator->currenttopitem != NULL)
			{
				if (Elevator->currenttopitem->Previous->Previous != NULL)
					Elevator->currenttopitem = Elevator->currenttopitem->Previous;
			}
		}
        if ( BMUIOSElevatorIsInRButton (Elevator, MouseState) == BMTRUE)
		{
			if (Elevator->currenttopitem != NULL)
			{
				if (Elevator->currenttopitem->Next->Next != NULL)
					Elevator->currenttopitem = Elevator->currenttopitem->Next;
			}
		}
		if ( (item = BMUIOSElevatorIsText (Elevator, MouseState) ) != - 1)
		{
			courant = Elevator->currenttopitem;
			while (item > 0)
			{
				if (courant->Next->Next == NULL)
					item = 0;

				courant = courant->Next;
				item--;
			}
			
			if (item != -1)
			{
				Elevator->selectionitem = courant;
				Elevator->currentchar	= 0;
				return (BMTRUE);
			}
		}
	}

	if ( KMLControlMouseButtonPress(1) ) //button 1 is down
	{
		//for future if needed
	}

	return (BMFALSE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorIsInLButton (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
 *
 *  \param  *Elevator 
 *  \param  MouseState
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 11:06:21
 ***************************************************************/
SBYTE	BMUIOSElevatorIsInLButton (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
{
	//MouseState.
	SDWORD		x,
				y,
				x2,
				y2;

	x	= Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	x2	= x + BMOS_ELEVATOR_BUTTONL_SX;

	y	= Elevator->y + BMOS_ELEVATOR_BOXY_SX;
	y2	= y + BMOS_ELEVATOR_BUTTONR_SY;

	if ( (MouseState.x >= x) && ( MouseState.x <= x2) )
	{
		if ( (MouseState.y >= y) && ( MouseState.y <= y2) )
			return (BMTRUE);
	}
	return (BMFALSE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorIsInRButton (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
 *
 *  \param  *Elevator 
 *  \param  MouseState
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 11:06:26
 ***************************************************************/
SBYTE	BMUIOSElevatorIsInRButton (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
{
	//MouseState.
	SDWORD		x,
				y,
				x2,
				y2;

	x	= Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	x2	= x + BMOS_ELEVATOR_BUTTONL_SX;

	y	= Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY;//BMOS_ELEVATOR_BOXX_SY;
	y2	= y + BMOS_ELEVATOR_BUTTONR_SY;

	if ( (MouseState.x >= x) && ( MouseState.x <= x2) )
	{
		if ( (MouseState.y >= y) && ( MouseState.y <= y2) )
			return (BMTRUE);
	}

	return (BMFALSE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorIsText (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
 *
 *  \param  *Elevator 
 *  \param  MouseState
 *
 *  \return the index of the item relatively to the currenttopitem
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 11:16:47
 ***************************************************************/
SWORD	BMUIOSElevatorIsText (BMOSELEVATOR *Elevator, KMLMOUSE MouseState)
{
	SDWORD		x,
				y,
				x2,
				y2;
	BMFONT		*font;

	font = BMUICurrentFont;
	
	if (BMUIFontSelect (Elevator->pszFontName) != BMTRUE)
		return (BMFALSE);

	x	= Elevator->x + BMOS_ELEVATOR_BOXY_SX;
	x2	= Elevator->x + Elevator->width - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;

	y	= Elevator->y + BMOS_ELEVATOR_BOXY_SX;
	y2	= Elevator->y + Elevator->height - (BMOS_ELEVATOR_BOXX_SY << 1)  - BMOS_ELEVATOR_SCROLL_SY;

	if ( (MouseState.x >= x) && ( MouseState.x <= x2) )
	{
		if ( (MouseState.y >= y) && ( MouseState.y <= y2) )
		{
			//retrieve the good item
			return ( (SWORD)((MouseState.y - y) / BMUICurrentFont->SizeY));
		}
	}

	if (font != NULL)
	{
		if (BMUIFontSelect (font->pszFontName) != BMTRUE)
			return (BMFALSE);
	}

	return (-1);
}

//**************************************************************
/** \fn     SBYTE BMUIOSLoadBackGround (BMOSELEVATOR *Elevator, STRING* BackGround, UDWORD FlagBack )
 *
 *  \param  *Elevator  
 *  \param  BackGround 
 *  \param  FlagBack   
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 12:09:10
 ***************************************************************/
SBYTE	BMUIOSLoadBackGround		(BMOSELEVATOR *Elevator, STRING* BackGround, UDWORD FlagBack )
{
	if (Elevator->background != NULL)
	{
		KMLDeleteImage (Elevator->background);
		Elevator->background = NULL;
	}

	if (BackGround != NULL)
		Elevator->background = KMLCreateImage (0, 0, KML_BMP, BackGround);
	Elevator->flagback = FlagBack;

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorGetLButton (BMOSELEVATOR *Elevator,RECT *rect)
 *
 *  \param  *Elevator 
 *  \param  *rect     
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 19/03/2001 09:52:52
 ***************************************************************/
SBYTE	BMUIOSElevatorGetLButton (BMOSELEVATOR *Elevator,RECT *rect)
{
	rect->left  = Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	rect->top   = Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY;//BMOS_ELEVATOR_BOXX_SY;

	rect->right = rect->left + BMOS_ELEVATOR_BUTTONL_SX;
	rect->bottom= rect->top  + BMOS_ELEVATOR_BUTTONL_SY;

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSElevatorGetBackGrButton (BMOSELEVATOR *Elevator,RECT *rect)
 *
 *  \param  *Elevator 
 *  \param  *rect     
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 19/03/2001 10:01:56
 ***************************************************************/
SBYTE	BMUIOSElevatorGetBackGrButton (BMOSELEVATOR *Elevator,RECT *rect)
{
	rect->left	= Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	rect->top	= Elevator->y + (BMOS_ELEVATOR_BOXX_SY << 1);
	rect->right	= BMOS_BACK_SX;
	rect->bottom = Elevator->y + Elevator->height - BMOS_ELEVATOR_SCROLL_SY - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SY - BMOS_ELEVATOR_BOXX_SY;

	return (BMTRUE);
}
//**************************************************************
/** \fn     SBYTE BMUIOSElevatorGetLButton (BMOSELEVATOR *Elevator,RECT *rect)
 *
 *  \param  *Elevator 
 *  \param  *rect     
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 19/03/2001 09:57:25
 ***************************************************************/
SBYTE	BMUIOSElevatorGetRButton (BMOSELEVATOR *Elevator,RECT *rect)
{
	rect->left  = Elevator->x + (Elevator->width) - BMOS_ELEVATOR_BOXY_SX - BMOS_ELEVATOR_BUTTONL_SX;
	rect->top   = Elevator->y + BMOS_ELEVATOR_BOXX_SY;

	rect->right = rect->left + BMOS_ELEVATOR_BUTTONL_SX;
	rect->bottom= rect->top  + BMOS_ELEVATOR_BUTTONL_SY;

	return (BMTRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////// BMOSTextSelectionnable

//**************************************************************
/** \fn     SBYTE BMUIOSTextSCreate (BMOSOBJECT **NewObject, STRING *pszText, STRING *pszFontName, STRING *pszFontNameActive, UDWORD CoordX, UDWORD CoordY, UDWORD IdParent, UDWORD Flag = BMOS_NOTHING)
 *
 *  \param  **NewObject        
 *  \param  *pszText           
 *  \param  *pszFontName       
 *  \param  CoordX             
 *  \param  CoordY             
 *  \param  IdParent           
 *  \param  Flag = BMOS_NOTHING
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 12:28:51
 ***************************************************************/
SBYTE	BMUIOSTextSCreate		(BMOSOBJECT	**NewObject, STRING	*pszText, STRING *pszFontName, STRING *pszFontNameActive,UDWORD CoordX, UDWORD CoordY, UDWORD IdParent, UDWORD Flag)
{
	BMOSTEXTSELECTIONNABLE	*BMTextS;
	BMOSOBJECT				*parent;
	
	if (IdParent == 0)
		return (BMTRUE - 1);			//Error, can't add the text if the id is null, no parent...

	if (BMUICurrentFont == NULL)
		return BMFALSE;

	if (BMUIFontGetFont (pszFontName) == NULL)
		return BMFALSE;

	if (BMUIOSFindObject (IdParent, &parent) != BMTRUE)
		return (BMTRUE - 1);

	BMUIOSObjectCreate (NewObject);

	(*NewObject)->BMOSType		= BMOS_SELECTIONNABLETEXT;
	(*NewObject)->BMOSInfos		= ALLOC2 (sizeof(BMOSTEXTSELECTIONNABLE),BMOSMemory);
	
	BMTextS = (BMOSTEXTSELECTIONNABLE *) (*NewObject)->BMOSInfos;
	BMTextS->width 				= (BMUICurrentFont->SizeX + BMUICurrentFont->FontSpaceX) * strlen(pszText);
	BMTextS->height				= BMUICurrentFont->SizeY + BMUICurrentFont->FontSpaceY;
	BMTextS->x					= CoordX;
	BMTextS->y					= CoordY;
	BMTextS->pszText			= (STRING *)ALLOC2 (strlen(pszText) + 1,BMOSMemory);
	BMTextS->parentid			= IdParent;
	BMTextS->menuid				= -1;
	BMTextS->pszFontName		= (STRING *)ALLOC2 (strlen(pszFontName) + 1,BMOSMemory);
	BMTextS->pszFontNameActive	= (STRING *) ALLOC2 (strlen(pszFontNameActive) + 1,BMOSMemory);
	BMTextS->bActive			= BMFALSE;
	BMTextS->bIsDown			= BMFALSE;
	BMTextS->TimeLeft			= 0;

	strcpy(BMTextS->pszText, pszText);
	strcpy (BMTextS->pszFontName, pszFontName);
	strcpy (BMTextS->pszFontNameActive, pszFontNameActive);

	if (BMUIOSObjectInsert ( IdParent, (*NewObject)->id, CoordX, CoordY, Flag) != BMTRUE)
		return (BMTRUE - 1);
	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSTextSDelete (BMOSTEXTSELECTIONNABLE *objtodelete)
 *
 *  \param  *objtodelete
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 12:36:48
 ***************************************************************/
SBYTE	BMUIOSTextSDelete		(BMOSTEXTSELECTIONNABLE	*objtodelete)
{
	FREE2 (objtodelete->pszText, BMOSMemory);
	FREE2 (objtodelete->pszFontName, BMOSMemory);
	FREE2 (objtodelete->pszFontNameActive, BMOSMemory);
	FREE2 (objtodelete, BMOSMemory);
	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSTextSDisplay (BMOSTEXTSELECTIONNABLE *Text)
 *
 *  \param  *Text
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 12:36:51
 ***************************************************************/
SBYTE	BMUIOSTextSDisplay		(BMOSTEXTSELECTIONNABLE	*Text)
{
	BMFONT* font;

	font = BMUICurrentFont;

	if (font == NULL)
		return (BMFALSE);

	if (Text->bActive == BMTRUE)
		BMUIFontSelect ( Text->pszFontNameActive );
	else
		BMUIFontSelect ( Text->pszFontName );

	BMUIFontDrawString (Text->pszText, Text->x, Text->y, NULL);

	if (Text->bActive == BMTRUE)
	{
		BMUIFontSelect ( font->pszFontName );
		Text->bActive = BMFALSE;
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMUIOSTextSManage (BMOSTEXTSELECTIONNABLE *Text, KMLMOUSE mousestate)
 *
 *  \param  *Text     
 *  \param  mousestate
 *
 *  \return BMTRUE if text is selectionned, bmfalse otherwise
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 12:37:52
 ***************************************************************/
SBYTE	BMUIOSTextSManage		(BMOSTEXTSELECTIONNABLE	*Text, KMLMOUSE mouestate)
{

	if ( Text->bIsDown == BMTRUE )
	{
		//update timer
		if ( Text->TimeLeft < BMTimerGetTime () )
		{
			Text->bIsDown = BMFALSE;
			Text->bActive = BMFALSE;
			return (BMTRUE);
		}

		Text->bActive = (UBYTE) ( ( (Text->TimeLeft - BMTimerGetTime () ) / BMOS_SELECTIONNABLE_TIME2 ) %2 ) ;//so BMTRUE or BMFALSE
	}
	else if ( KMLControlMouseButtonPress(0) ) //button 0 is down
	{
		Text->bIsDown	= BMTRUE;
		Text->TimeLeft	= BMTimerGetTime () + BMOS_SELECTIONNABLE_TIME;
		Text->bActive	= BMTRUE;
	}
	else if ( KMLControlMouseButtonPress(1) ) //button 1 is down
	{
		//for future if needed
	}
	else
		Text->bActive = BMTRUE;

	return (BMFALSE);
}

//#define		BMOS_SELECTIONNABLE_TIME	200
//#define		BMOS_SELECTIONNABLE_TIME2	50


/*

#define		BMOS_ELEVATOR_BOXX_X		41
#define		BMOS_ELEVATOR_BOXX_Y		25
#define		BMOS_ELEVATOR_BOXX_SX		144
#define		BMOS_ELEVATOR_BOXX_SY		6

#define		BMOS_ELEVATOR_BOXY_X		27
#define		BMOS_ELEVATOR_BOXY_Y		40
#define		BMOS_ELEVATOR_BOXY_SX		6
#define		BMOS_ELEVATOR_BOXY_SY		144

#define		BMOS_ELEVATOR_BUTTONL_X		88
#define		BMOS_ELEVATOR_BUTTONL_Y		182
#define		BMOS_ELEVATOR_BUTTONL_SX	20
#define		BMOS_ELEVATOR_BUTTONL_SY	23

#define		BMOS_ELEVATOR_BUTTONR_X		144
#define		BMOS_ELEVATOR_BUTTONR_Y		182
#define		BMOS_ELEVATOR_BUTTONR_SX	20
#define		BMOS_ELEVATOR_BUTTONR_SY	23

#define		BMOS_ELEVATOR_SCROLL_X		47
#define		BMOS_ELEVATOR_SCROLL_Y		72
#define		BMOS_ELEVATOR_SCROLL_SX		47
#define		BMOS_ELEVATOR_SCROLL_SY		19

*/
/*
//-- Elevator
EXPORT_DLL	SBYTE	BMUIOSElevatorCreate		(BMOSOBJECT	**NewObject, UDWORD CoordX, UDWORD CoordY, UDWORD SizeX, UDWORD SizeY, UDWORD IdParent, UDWORD Flag = BMOS_NOTHING);

EXPORT_DLL	SBYTE	BMUIOSElevatorDelete		(BMOSELEVATOR *objtodelete);

EXPORT_DLL	SBYTE	BMUIOSElevatorDisplay		(BMOSELEVATOR *Elevator);

EXPORT DLL	SBYTE	BMUIOSElevatorManage		(BMOSELEVATOR *Elevator, KMLMOUSE MouseState);

*/



