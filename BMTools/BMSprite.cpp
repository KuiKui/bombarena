#include	"BMSprite.h"
#include	"BMFile.h"
#include	"BMDebug.h"
//--------------------------------
// Globals
//--------------------------------

SDWORD		BMSpriteMemory = 0;	//Allocate memory


BMLIST		*ListModel;		//list of model, SPRMODEL...
BMLIST		*ListSprite;	//list of sprite, je le vire prendrai trop de temps pour les delete de sprite, reparcours de la liste a chaque fois
UDWORD		SpriteIdFree;

//*********************************************************************
//* FUNCTION: BMSpriteStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the sprite API
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 17:09:55
//* REVISION:									
//*********************************************************************
SBYTE		BMSpriteStart		(STRING *FileName, STRING *pszPath)
{
	STRING		file [255];
	KMLFILE		*f;
	STRING		RelativeDirectory [255];
	STRING		chaine [255];

	ListModel = BMListNew ();
	if (ListModel == NULL)
		return (BMTRUE - 1);

	ListSprite = BMListNew ();
	if (ListSprite == NULL)
		return (BMTRUE - 1);

	SpriteIdFree = 1;

	//check if there's a path
	file [0] = '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(file, pszPath);
	}
	strcat (file, FileName);

	f = KMLPackOpen(NULL, file);
	if (f == NULL)
		return (BMTRUE - 1);												//Error, cannot open the file !

	//Parse the file
	strcpy(RelativeDirectory, "\\.");										//Set the directory to the current

	//Search for the beginning of the file
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}	
	}while ( strcmp(chaine, SPR_BEGIN_MODEL) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, SPR_END_MODEL) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		//------------------
		// Case of Directory
		//------------------
		if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}
		//------------------------------
		//Description of sprite models
		//------------------------------
		else if (strcmp (chaine , SPR_END_MODEL) != 0) 
		{
			if (BMSpriteAddModel (chaine, RelativeDirectory) != BMTRUE)
				return (BMTRUE - 1);		//Error, failed to load the model...
		}
	}
	
	KMLPackClose (f);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMSpriteEnd	
//*--------------------------------------------------------------------
//* DESCRIPT: End the sprite API (delete the list)
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : nothing
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    17/01/2001 11:04:02
//* REVISION:									    18/01/2001 10:45:09
//*********************************************************************
SBYTE		BMSpriteEnd			()
{
	BMNODE	*Courant,	
			*Suivant;

	//delete all the instance of sprite
	while (ListSprite->First->Next != NULL)
	{
		if (BMSpriteDelete ( (BMSPRITE *)ListSprite->First->Info) != BMTRUE)
			return (BMTRUE - 1);					//Error, can't delete the instance
	}

	BMListDelete (ListSprite);

	//delete the sprite model
	Courant = ListModel->First;

	while (Suivant = Courant->Next)
	{
		BMNODE	*lCourant,
				*lSuivant;

		if ( ((SPRMODEL *)Courant->Info)->NumberUsed != 0)
			return (BMTRUE - 1);								//Error, not all the instance have been deleted !!!

		FREE2 ( ((SPRMODEL *)Courant->Info)->FileName,BMSpriteMemory);
		
		lCourant = ((SPRMODEL *)Courant->Info)->ListAnim->First;

		while (lSuivant = lCourant->Next)
		{
			//the anim will be destroy at the end of the anim API
			BMAnimDelete ( ((BMSPRANIMS *)lCourant->Info)->Anim);
			FREE2 (lCourant->Info,BMSpriteMemory);
			BMDeleteNode (lCourant);
			lCourant = lSuivant;
		}
		BMListDelete ( ((SPRMODEL *)Courant->Info)->ListAnim);

		FREE2 ( Courant->Info,BMSpriteMemory);

		BMDeleteNode (Courant);
		Courant = Suivant;
	}

	BMListDelete (ListModel);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMSpriteAdd
//*--------------------------------------------------------------------
//* DESCRIPT: Add the sprite, (search in the BDD first)
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : pointer to sprite which is filled, filename, path (can be NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 18:29:42
//* REVISION:									    18/01/2001 10:45:04
//*********************************************************************
SBYTE		BMSpriteAdd			(BMSPRITE **pSprite, STRING *FileName, STRING *pszPath)
{
	//traverse the list of models
	BMNODE		*Courant;
	BMNODE		*Suivant;
	SPRMODEL	*NewModel;
	KMLFILE		*f;
	STRING		RelativeDirectory [255];
	STRING		chaine [255];
	STRING		file [255];
	BMSPRITE	*NewSprite;

	Courant = ListModel->First;

	while (Suivant = Courant->Next)
	{
		SPRMODEL *Model;

		Model = (SPRMODEL *)Courant->Info;
		if ( stricmp( Model->FileName, FileName) == 0)	//Object found
		{
			NewSprite = (BMSPRITE *) ALLOC2 (sizeof (BMSPRITE),BMSpriteMemory);

			NewSprite->Id = SpriteIdFree;
			SpriteIdFree++;

			NewSprite->PtrModel			= Model;												//Link to the model list
			NewSprite->PtrModel->NumberUsed++;
			NewSprite->CurrentAnim		= NULL;	
			NewSprite->PosX				= 0;
			NewSprite->PosY				= 0;
			
			*pSprite = NewSprite;

			BMListAddTail (ListSprite, NewSprite);

			return (BMTRUE);
		}
		Courant = Suivant;
	}

	//not found in the bdd so create it !
	NewModel = (SPRMODEL *) ALLOC2 (sizeof(SPRMODEL),BMSpriteMemory);
	
	NewModel->FileName	= (STRING *) ALLOC2 (strlen(FileName) + 1,BMSpriteMemory);
	NewModel->ListAnim	= BMListNew ();
	NewModel->Flag		= 0;

	BMListAddTail (ListModel, NewModel);

	strcpy (NewModel->FileName, FileName);

	//check if there's a path
	file [0] = '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(file, pszPath);
	}
	strcat (file, FileName);

	f = KMLPackOpen(NULL, file);
	if (f == NULL)
		return (BMTRUE - 1);												//Error, cannot open the file !

	//Parse the file
	strcpy(RelativeDirectory, "\\.");										//Set the directory to the current

	//Search for the beginning of the file
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}	
	}while ( strcmp(chaine, SPR_BEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, SPR_END) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		//------------------
		// Case of Directory
		//------------------
		if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}
		//--------------------------------
		// Case of all flags
		//--------------------------------
		else if ( strcmp (chaine, SPR_COLOR) == 0)
		{
			;
		}
		//------------------------------
		//Description of sprite
		//------------------------------
		else if (strcmp (chaine , SPR_END) != 0) 
		{
			BMSPRANIMS	*NewSprAnim;

			NewSprAnim = (BMSPRANIMS *) ALLOC2 (sizeof (BMSPRANIMS),BMSpriteMemory);
			
			if (BMAnimAdd (&NewSprAnim->Anim, chaine, RelativeDirectory) != BMTRUE)//if (BMAnimAdd (&id, NULL, NULL, FileName, RelativeDirectory) != BMTRUE)
				return (BMTRUE - 1);

			//Add the anim to the list of the model sprite
			BMListAddTail (NewModel->ListAnim, NewSprAnim);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
			
			NewSprAnim->KeyAnim = atoi (chaine);
		}
	}
	
	KMLPackClose (f);
	//create the sprite
	NewSprite = (BMSPRITE *) ALLOC2 (sizeof (BMSPRITE),BMSpriteMemory);

	NewSprite->Id = SpriteIdFree;
	SpriteIdFree++;

	NewSprite->PtrModel			= NewModel;													//Link to the model list
	NewSprite->PtrModel->NumberUsed = 1;
	NewSprite->CurrentAnim		= NULL;	
	NewSprite->PosX				= 0;
	NewSprite->PosY				= 0;
	
	*pSprite = NewSprite;

	BMListAddTail (ListSprite, NewSprite);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMSpriteDelete		
//*--------------------------------------------------------------------
//* DESCRIPT: Delete the specified sprite
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the sprite
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    17/01/2001 10:44:16
//* REVISION:									    18/01/2001 10:44:58
//*********************************************************************
SBYTE		BMSpriteDelete		(BMSPRITE *Sprite)
{
	BMNODE *Courant;
	BMNODE *Suivant;
	if (Sprite->PtrModel->NumberUsed == 0)
		return (BMTRUE - 1);				//Error, all the instance have been deleted !!!
	
	Sprite->PtrModel->NumberUsed--;
	
	//Search the anim in the list
	Courant = ListSprite->First;

	while (Suivant = Courant->Next)
	{
		if (Courant->Info == Sprite)
		{
			//delete also the anim from the anim liste models
			BMDeleteNode (Courant);
			if (Sprite->CurrentAnim != NULL)
			{
				if (BMAnimDelete (Sprite->CurrentAnim) != BMTRUE)
					return (BMTRUE - 1);
			}
			FREE2 (Sprite,BMSpriteMemory);

			return (BMTRUE);
		}
		Courant = Suivant;
	}

	return (BMTRUE - 1);							//Error, sprite not found in the list
}

//*********************************************************************
//* FUNCTION: BMSpriteSetAnim
//*--------------------------------------------------------------------
//* DESCRIPT: Set the anim of the sprite
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the sprite, id of the anim to set
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    17/01/2001 10:50:31
//* REVISION:									
//*********************************************************************
SBYTE		BMSpriteSetAnim		(BMSPRITE *Sprite, SDWORD AnimId)
{
	BMNODE	*Courant;
	BMNODE	*Suivant;

	Courant = Sprite->PtrModel->ListAnim->First;//->Info BMSPRANIMS
	while (Suivant = Courant->Next)
	{
		BMSPRANIMS *SprAnim;

		SprAnim = (BMSPRANIMS *) Courant->Info;
		
		if (SprAnim->KeyAnim == AnimId)
		{
			if (Sprite->CurrentAnim != NULL)
			{
				if (BMAnimDelete (Sprite->CurrentAnim) != BMTRUE)
					return (BMTRUE - 1);
			}
			if (BMAnimAdd (&Sprite->CurrentAnim, SprAnim->Anim->Model->FileName) != BMTRUE)
				return (BMTRUE - 1);
			//Sprite->CurrentAnim = SprAnim->Anim;
			BMAnimPlay (Sprite->CurrentAnim);//, &Sprite->TimeToKeepFrame);
			
			return (BMTRUE);
		}

		Courant = Suivant;
	}

	return (BMTRUE - 1);		//error, anim not found
}

//*********************************************************************
//* FUNCTION: BMSpriteDisplay	
//*--------------------------------------------------------------------
//* DESCRIPT: display the sprite
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the sprite
//*--------------------------------------------------------------------
//* OUT     : return  code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    17/01/2001 10:51:31
//* REVISION:									
//*********************************************************************
SBYTE		BMSpriteDisplay		(BMSPRITE *Sprite)
{
	if (!(Sprite->PtrModel->Flag & SPR_COLORPLAYER))
	{
		if (BMAnimUpdate (Sprite->CurrentAnim) != BMTRUE)
			BMDebugPrint("PAS OK SPRITE UPDATE");
		//BMDebugPrint("OK anim update");
		UDWORD ValueX;
		UDWORD ValueY;

		ValueX = (UDWORD) (Sprite->PosX * BMMAP_ELEMENTSIZE);
		ValueY = (UDWORD) (Sprite->PosY * BMMAP_ELEMENTSIZE);

		KMLFastBlt (	NULL,
						50 + Sprite->PosX,
						74 + Sprite->PosY,
						Sprite->CurrentAnim->Model->Image,
						((BMANIMFRAME *)Sprite->CurrentAnim->CurrentFrame->Info)->AnimFrameUCoords,
						((BMANIMFRAME *)Sprite->CurrentAnim->CurrentFrame->Info)->AnimFrameVCoords,
						((BMANIMFRAME *)Sprite->CurrentAnim->CurrentFrame->Info)->AnimFrameWidth,
						((BMANIMFRAME *)Sprite->CurrentAnim->CurrentFrame->Info)->AnimFrameHeight
					);
	}
	else
	{
		//blit palletized
		return (BMTRUE - 1);	//Error, not available for the moment
	}

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMSpriteAddModel
//*--------------------------------------------------------------------
//* DESCRIPT: add a model to the sprite BDD
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : file name, path (can be NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 10:30:44
//* REVISION:									
//*********************************************************************
SBYTE		BMSpriteAddModel	(STRING *FileName, STRING *pszPath)
{
	//traverse the list of models
	BMNODE		*Courant;
	BMNODE		*Suivant;
	SPRMODEL	*NewModel;
	KMLFILE		*f;
	STRING		RelativeDirectory [255];
	STRING		chaine [255];
	STRING		file [255];

	Courant = ListModel->First;

	while (Suivant = Courant->Next)
	{
		SPRMODEL *Model;

		Model = (SPRMODEL *)Courant->Info;
		if ( strcmp( Model->FileName, FileName) == 0)	//Object found
			return (BMTRUE - 1);										//Error, object already registered in the SpriteModel BDD

		Courant = Suivant;
	}

	//not found in the bdd so create it !
	NewModel = (SPRMODEL *) ALLOC2 (sizeof(SPRMODEL),BMSpriteMemory);
	
	NewModel->FileName		= (STRING *) ALLOC2 (strlen(FileName) + 1,BMSpriteMemory);
	NewModel->ListAnim		= BMListNew ();
	NewModel->Flag			= 0;
	NewModel->NumberUsed	= 0;

	BMListAddTail (ListModel, NewModel);

	strcpy (NewModel->FileName, FileName);

	//check if there's a path
	file [0] = '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(file, pszPath);
	}
	strcat (file, FileName);

	f = KMLPackOpen(NULL, file);
	if (f == NULL)
		return (BMTRUE - 1);												//Error, cannot open the file !

	//Parse the file
	strcpy(RelativeDirectory, "\\.");										//Set the directory to the current

	//Search for the beginning of the file
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}	
	}while ( strcmp(chaine, SPR_BEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, SPR_END) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		//------------------
		// Case of Directory
		//------------------
		if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}
		//--------------------------------
		// Case of all flags
		//--------------------------------
		else if ( strcmp (chaine, SPR_COLOR) == 0)
		{
			;
		}
		//------------------------------
		//Description of sprite
		//------------------------------
		else if (strcmp (chaine , SPR_END) != 0) 
		{
			BMSPRANIMS	*NewSprAnim;

			NewSprAnim = (BMSPRANIMS *) ALLOC2 (sizeof (BMSPRANIMS),BMSpriteMemory);
			
			if (BMAnimAdd (&NewSprAnim->Anim, chaine, RelativeDirectory) != BMTRUE)//if (BMAnimAdd (&id, NULL, NULL, FileName, RelativeDirectory) != BMTRUE)
				return (BMTRUE - 1);

			//Add the anim to the list of the model sprite
			BMListAddTail (NewModel->ListAnim, NewSprAnim);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
			
			NewSprAnim->KeyAnim = atoi (chaine);
		}
	}

	KMLPackClose (f);

	return (BMTRUE);
}