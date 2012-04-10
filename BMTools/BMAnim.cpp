#include "BMAnims.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"
#include "BMTimer.h"
#include "BMImage.h"

#include "BMDebug.h"
//--------------------------------
// Globales
//--------------------------------
SDWORD		BMAnimMemory = 0; //Allocate memory

BMLIST		*AnimsListe;			//list of create anims
BMLIST		*AnimModel;				//list of the anims model

//*********************************************************************
//* FUNCTION: BMAnimStart
//*--------------------------------------------------------------------
//* DESCRIPT: Initialise les globales d'anims
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/12/2000 17:13:22
//* REVISION:									
//*********************************************************************
SBYTE		BMAnimStart			()
{
	AnimsListe		= BMListNew();
	AnimModel		= BMListNew();

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMAnimEnd
//*--------------------------------------------------------------------
//* DESCRIPT: Termine les anims et vire la liste
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/12/2000 17:20:06
//* REVISION:									    12/01/2001 11:27:47
//*********************************************************************
SBYTE		BMAnimEnd				()
{
	BMNODE *Courant;
	BMNODE *Suivant;

	//Delete the Anim first, cause it update the model BDD
	while (AnimsListe->First->Next)
	{
		if (BMAnimDelete ( (BMANIM *)AnimsListe->First->Info) != BMTRUE)
			return (BMTRUE - 1);							//Error, can't delete the instance
	}

	BMListDelete (AnimsListe);

	//Delete the model BDD
	Courant = AnimModel->First;

	while (Suivant = Courant->Next)
	{
		BMNODE *Courantf,
			   *Suivantf;

		if ( ((ANIMMODEL *) Courant->Info)->NumberUsed != 0)
			return (BMTRUE - 1);									//Error, all of the instance haven't been deleted !!!
		
		BMImageDelete ( ((ANIMMODEL *) Courant->Info)->Image);
		FREE2 ( ((ANIMMODEL *) Courant->Info)->FileName,BMAnimMemory);
		
		Courantf = ((ANIMMODEL *)Courant->Info)->ListFrames->First;

		while (Suivantf = Courantf->Next)
		{
			FREE2( Courantf->Info,BMAnimMemory);
			BMDeleteNode (Courantf);
			Courantf = Suivantf;
		}
		BMListDelete ( ((ANIMMODEL *)Courant->Info)->ListFrames);

		FREE2 ( Courant->Info,BMAnimMemory);
		BMDeleteNode(Courant);
		
		Courant = Suivant;
	}

	BMListDelete (AnimModel);

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMAnimAdd
//*--------------------------------------------------------------------
//* DESCRIPT: add an anim which is in the file pszfichier
//* it returns a ptr to the anim, it reads the anims in pszfichier                                                                    
//*--------------------------------------------------------------------
//* IN      : pszfichier, pszpath = NULL par defaut, the pszname can be NULL
//*--------------------------------------------------------------------
//* OUT     : return code, anim
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 11:27:27
//* REVISION:										16/01/2001 11:17:51
//*********************************************************************
SBYTE		BMAnimAdd				(BMANIM **Anim, STRING *pszFichier, STRING *pszPath)
{
	KMLFILE			*f;
	KMSHEADER		Header;
	KMSANIM			HeaderAnim;
	SDWORD			i;
	ANIMMODEL		*Model;
	BMANIM			*NewAnim;
	BMANIMFRAME		*bmanimframe;
	STRING			chainetemp [255];
	BMNODE			*Courant;
	BMNODE			*Suivant;

	//Search if already loaded
	Courant = AnimModel->First;

	while (Suivant = Courant->Next)
	{
		Model = (ANIMMODEL *) Courant->Info;

		if (strcmp (Model->FileName, pszFichier) == 0)
		{//found
			NewAnim = (BMANIM *) ALLOC2 (sizeof (BMANIM),BMAnimMemory);
			
			NewAnim->Model			= Model;
			NewAnim->AnimTimeFrame	= 0;
			NewAnim->IsPlaying		= 0;
			NewAnim->CurrentFrame	= Model->ListFrames->First;
							
			BMListAddTail (AnimsListe, NewAnim);

			*Anim = NewAnim;

			Model->NumberUsed++; //add an instance to the BDD model
			return (BMTRUE);
		}

		Courant = Suivant;
	}

	//Animation not found in the BDD
	
	//Get the path if there is one
	chainetemp[0]= '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(chainetemp, pszPath);
	}
	strcat (chainetemp, pszFichier);

	f = KMLPackOpen (NULL, chainetemp);

	if (f == NULL)
		return (KML_ERROR_READ);

	KMLPackRead ((KMSHEADER *) &Header, sizeof(Header), f);

	if (Header.NbrAnim > 1)
		return (KML_ERROR_READ);

	//Get the path for the KMS file image
	chainetemp[0]= '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(chainetemp, pszPath);
	}
	strcat( chainetemp, Header.PathImage);

	//Create the model
	Model				= (ANIMMODEL *) ALLOC2 (sizeof (ANIMMODEL),BMAnimMemory);
	Model->FileName		= (STRING *) ALLOC2 (strlen (pszFichier) + 1,BMAnimMemory);
	strcpy(Model->FileName, pszFichier);
	Model->Image		= BMImageCreate (0, 0, KML_BMP, chainetemp);
	Model->ListFrames	= BMListNew ();	

	BMListAddTail (AnimModel, Model);

	KMLPackRead ( (KMSANIM *) &HeaderAnim, sizeof (HeaderAnim), f);

	for (i = 0; i < HeaderAnim.NbrFrame; i++)
	{
		KMSFRAME	HeaderFrame;

		KMLPackRead ( (KMSFRAME *) &HeaderFrame, sizeof (HeaderFrame), f);

		bmanimframe = (BMANIMFRAME *) ALLOC2 (sizeof(BMANIMFRAME),BMAnimMemory);

		bmanimframe->AnimFrameUCoords		= HeaderFrame.x;
		bmanimframe->AnimFrameVCoords		= HeaderFrame.y;
		bmanimframe->AnimFrameWidth			= HeaderFrame.largeur;
		bmanimframe->AnimFrameHeight		= HeaderFrame.hauteur;
		bmanimframe->AnimFrameTimeDuration	= HeaderFrame.time;

		if (HeaderFrame.time <= 0)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error, cannot have an frame of anim with a time to 0
		}

		//chain to list of frame
		BMListAddTail (Model->ListFrames, bmanimframe);
	}

	KMLPackClose (f);

	//Create the anim
	NewAnim = (BMANIM *) ALLOC2 (sizeof (BMANIM),BMAnimMemory);
	
	//NewAnim->AnimId			= AnimFreeId;
	NewAnim->Model			= Model;
	NewAnim->AnimTimeFrame	= 0;
	NewAnim->IsPlaying		= 0;
	NewAnim->CurrentFrame	= Model->ListFrames->First;
					
	BMListAddTail (AnimsListe, NewAnim);

	*Anim = NewAnim;

	Model->NumberUsed = 1;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMAnimDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the anim and uninstanstiate it in the BDD
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : anim to delete
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 10:19:38
//* REVISION:									
//*********************************************************************
SBYTE		BMAnimDelete		(BMANIM	*AnimToDelete)
{
	BMNODE *Courant;
	BMNODE *Suivant;

	if (AnimToDelete->Model->NumberUsed == 0)			//Error, no instance registered in the BDD
		return (BMTRUE - 1);
	
	AnimToDelete->Model->NumberUsed--;
	
	//Search the anim in the list
	Courant = AnimsListe->First;

	while (Suivant = Courant->Next)
	{
		if (Courant->Info == AnimToDelete)
		{
			BMDeleteNode (Courant);
			FREE2 (AnimToDelete,BMAnimMemory);

			return (BMTRUE);

		}
		Courant = Suivant;
	}

	return (BMTRUE - 1);							//Error, anim not found in the list
}

//*********************************************************************
//* FUNCTION: BMAnimPlay			
//*--------------------------------------------------------------------
//* DESCRIPT: play the anim
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : Id, CurrentAnim (which is filled with the current ptr to the anim)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 12:09:35
//* REVISION:									
//*********************************************************************
SBYTE		BMAnimPlay			(BMANIM *AnimToPlay)
{
	if ( AnimToPlay->IsPlaying == 1)
		return (BMTRUE);

	AnimToPlay->IsPlaying		= 1;
	AnimToPlay->CurrentFrame	= AnimToPlay->Model->ListFrames->First;

	AnimToPlay->AnimTimeFrame	= ((BMANIMFRAME *)AnimToPlay->Model->ListFrames->First->Info)->AnimFrameTimeDuration + BMTimerGetTime ();

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMAnimUpdate			
//*--------------------------------------------------------------------
//* DESCRIPT: update the animation frame if the time exceeds the value
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : anim
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 15:42:25
//* REVISION:										11/01/2001 14:35:01 (updating by while)
//*													18/01/2001 15:50:17 (updating by animtimeframe = xxx + bmtimergettime) prevent timer depassement
//*********************************************************************
SBYTE		BMAnimUpdate			(BMANIM *Anim)
{
	if (Anim->IsPlaying == 0)
		return (BMTRUE);

	while (BMTimerGetTime () >= Anim->AnimTimeFrame)
	{
		Anim->CurrentFrame = Anim->CurrentFrame->Next;
		if (Anim->CurrentFrame->Next == NULL)								//We get the next node in the list of frame
			Anim->CurrentFrame = Anim->Model->ListFrames->First;
		
		Anim->AnimTimeFrame += ((BMANIMFRAME *)Anim->Model->ListFrames->First->Info)->AnimFrameTimeDuration /*+ BMTimerGetTime ()*/;//We update the time of the frame
	}

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMAnimDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: display l'anim de pointeur Anim a la pos selectionnée
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : *Anim, X, Y
//*--------------------------------------------------------------------
//* OUT     : return  code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    02/01/2001 16:15:54
//* REVISION:									
//*********************************************************************
SBYTE		BMAnimDisplay			(BMANIM *Anim, UWORD CoordX, UWORD CoordY)
{
	KMLFastBlt	(	NULL, 
					CoordX, 
					CoordY, 
					Anim->Model->Image, 
					((BMANIMFRAME *) Anim->CurrentFrame->Info)->AnimFrameUCoords, 
					((BMANIMFRAME *) Anim->CurrentFrame->Info)->AnimFrameVCoords, 
					((BMANIMFRAME *) Anim->CurrentFrame->Info)->AnimFrameWidth, 
					((BMANIMFRAME *) Anim->CurrentFrame->Info)->AnimFrameHeight
				);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMAnimAddModel
//*--------------------------------------------------------------------
//* DESCRIPT: Add a model to the BDD
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : filename of the model, path (can be NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 10:13:56
//* REVISION:									
//*********************************************************************
SBYTE		BMAnimAddModel		(STRING *pszFichier, STRING *pszPath)
{
	KMLFILE			*f;
	KMSHEADER		Header;
	KMSANIM			HeaderAnim;
	SDWORD			i;
	ANIMMODEL		*Model;
	BMANIMFRAME		*bmanimframe;
	STRING			chainetemp [255];
	BMNODE			*Courant;
	BMNODE			*Suivant;

	//Search if already loaded
	Courant = AnimModel->First;

	while (Suivant = Courant->Next)
	{
		Model = (ANIMMODEL *) Courant->Info;

		if (strcmp (Model->FileName, pszFichier) == 0)
		{//found
			return (BMTRUE - 1);						//Error, model, already exists, (warning the path of the instance are not take to compare)
		}

		Courant = Suivant;
	}
	//Model not found
	
	//Get the path if there is one
	chainetemp[0]= '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(chainetemp, pszPath);
	}
	strcat (chainetemp, pszFichier);

	f = KMLPackOpen (NULL, chainetemp);

	if (f == NULL)
		return (KML_ERROR_READ);

	KMLPackRead ((KMSHEADER *) &Header, sizeof(Header), f);

	if (Header.NbrAnim > 1)		//Only one anim by kms
	{
		KMLPackClose (f);
		return (KML_ERROR_READ);
	}

	//Get the path for the KMS file image
	chainetemp[0]= '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(chainetemp, pszPath);
	}
	strcat( chainetemp, Header.PathImage);

	//Create the model
	Model				= (ANIMMODEL *) ALLOC2 (sizeof (ANIMMODEL),BMAnimMemory);
	Model->FileName		= (STRING *) ALLOC2 (strlen (pszFichier) + 1,BMAnimMemory);
	strcpy(Model->FileName, pszFichier);
	Model->Image		= BMImageCreate (0, 0, KML_BMP, chainetemp);
	Model->ListFrames	= BMListNew ();	
	Model->NumberUsed	= 0;	

	BMListAddTail (AnimModel, Model);

	KMLPackRead ( (KMSANIM *) &HeaderAnim, sizeof (HeaderAnim), f);

	for (i = 0; i < HeaderAnim.NbrFrame; i++)
	{
		KMSFRAME	HeaderFrame;

		KMLPackRead ( (KMSFRAME *) &HeaderFrame, sizeof (HeaderFrame), f);

		bmanimframe = (BMANIMFRAME *) ALLOC2 (sizeof(BMANIMFRAME),BMAnimMemory);

		bmanimframe->AnimFrameUCoords		= HeaderFrame.x;
		bmanimframe->AnimFrameVCoords		= HeaderFrame.y;
		bmanimframe->AnimFrameWidth			= HeaderFrame.largeur;
		bmanimframe->AnimFrameHeight		= HeaderFrame.hauteur;
		bmanimframe->AnimFrameTimeDuration	= HeaderFrame.time;

		if (HeaderFrame.time <= 0)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error, cannot have a frame of anim with a time to 0
		}

		//chain to list of frame
		BMListAddTail (Model->ListFrames, bmanimframe);
	}

	KMLPackClose (f);
	
	return (BMTRUE);
}






