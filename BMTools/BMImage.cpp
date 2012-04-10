#include "BMImage.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD				BMImageMemory = 0;	//Allocate memory

BMLIST				*BMImageList;		//List of the entity, BMIMAGE...

//*********************************************************************
//* FUNCTION: BMImageStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the image API, initialize the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    26/01/2001 11:15:46
//* REVISION:									
//*********************************************************************
SBYTE		BMImageStart		()
{
	BMImageList = BMListNew ();
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMImageEnd
//*--------------------------------------------------------------------
//* DESCRIPT: End the image API; clean the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void 
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    26/01/2001 11:16:16
//* REVISION:									
//*********************************************************************
SBYTE		BMImageEnd			()
{
	while (BMImageList->First->Next)
	{
		if (BMImageDelete ( ((BMIMAGE *)BMImageList->First->Info)->PtrImage) != BMTRUE)
			return (BMTRUE - 1);
		//we don't delete the node here, it's the bmimagedelete function which does this feature !
	}
	BMListDelete (BMImageList);

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMImageCreate
//*--------------------------------------------------------------------
//* DESCRIPT: Create the specified image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : width, heigth, flag, filename
//*--------------------------------------------------------------------
//* OUT     : ptr to image created or NULL if error
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    26/01/2001 11:16:41
//* REVISION:									
//*********************************************************************
KMLIMAGE	*	BMImageCreate		(UDWORD x, UDWORD y, UDWORD Flag, STRING *FileName)
{
	KMLIMAGE	*NewImage;
	BMIMAGE		*NewImageModel;
	STRING		*ptrString;
	BMNODE		*Courant;
	BMNODE		*Suivant;
	UDWORD		i;

	i = strlen(FileName) - 1;

	while ( (FileName[i] != '\\') && (FileName[i] != '/') && (i != 0) )
		i--;

	if (i != 0)
		ptrString = &FileName[i + 1];
	else 
		ptrString = FileName;

	Courant = BMImageList->First;

	while (Suivant = Courant->Next)
	{
		if ( strcmp( ((BMIMAGE *)Courant->Info)->pszName, ptrString) == 0)
		{//image already loaded
			((BMIMAGE *)Courant->Info)->NumberUsed++;		//add a user

			return ( ((BMIMAGE *)Courant->Info)->PtrImage );
		}
		Courant = Suivant;
	}
	//image not loaded
	Flag &= ~(KML_MEM_VIDEO_PREF | KML_MEM_SYSTEM);
	Flag |= KML_MEM_VIDEO;
	NewImage = KMLCreateImage (x, y, Flag, FileName);

	if (NewImage == NULL)
		return (NULL);														//error, the image cannot be create

	NewImageModel = (BMIMAGE *) ALLOC2 (sizeof(BMIMAGE), BMImageMemory);

	NewImageModel->NumberUsed	= 1;
	NewImageModel->pszName		= (STRING *) ALLOC2 (strlen(ptrString) + 1, BMImageMemory);
	NewImageModel->PtrImage		= NewImage;

	strcpy (NewImageModel->pszName, ptrString);

	BMListAddTail (BMImageList, NewImageModel);

	return (NewImageModel->PtrImage);
}

//*********************************************************************
//* FUNCTION: BMImageDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the specified instance in the BDD
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : image to delete
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    26/01/2001 11:18:53
//* REVISION:									
//*********************************************************************
SBYTE		BMImageDelete		(KMLIMAGE *ImageToDelete)
{
	BMNODE	*Courant;
	BMNODE	*Suivant;

	Courant = BMImageList->First;

	while (Suivant = Courant->Next)
	{
		if ( ((BMIMAGE *)Courant->Info)->PtrImage == ImageToDelete)
		{//image found
			BMIMAGE *image;

			image = (BMIMAGE *)Courant->Info;

			image->NumberUsed--;
			if (image->NumberUsed == 0)
			{
				KMLDeleteImage (image->PtrImage);
				image->PtrImage = NULL;

				FREE2 (image->pszName, BMImageMemory);
				FREE2 (image, BMImageMemory);
				BMDeleteNode (Courant);
			}

			return (BMTRUE);
		}
		Courant = Suivant;
	}

	//image not found, error
	return (BMTRUE - 1);
}
