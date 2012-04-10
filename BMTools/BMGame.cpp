#include	"BMGame.h"
#include	"BMDebug.h"
#include	"BMFile.h"
//--------------------------------
// Globales
//--------------------------------

BMANIM			**BMGamePlayerAnimation;	//Array of the player animation


//*********************************************************************
//* FUNCTION: BMGameStart	
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void 
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 12:10:21
//* REVISION:									
//*********************************************************************
SBYTE			BMGameStart			()
{
	BMGamePlayerAnimation = NULL;
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMGameEnd
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 12:10:37
//* REVISION:									
//*********************************************************************
SBYTE			BMGameEnd			()
{
	if (BMGamePlayerAnimation != NULL)
		FREE (BMGamePlayerAnimation);

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMGameLoadSkin
//*--------------------------------------------------------------------
//* DESCRIPT: load the skin for players in the specified file, the file 
//* must have at least numberofanim animations to be loaded                                                                   
//*--------------------------------------------------------------------
//* IN      : Name of file, number min of anim
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 12:10:57
//* REVISION:									
//*********************************************************************
SBYTE			BMGameLoadSkin		(STRING *pszFile, UDWORD numberofanim)
{
	KMLFILE	*f;
	UDWORD	i;
	STRING	chaine [255];
	STRING	RelativeDirectory [255];
	BMLIST	*listanim;							//list of animation loaded in the file

	f = KMLPackOpen (NULL, pszFile);

	if (f == NULL)
		return (BMTRUE - 1);					//error, can't open the file

	strcpy(RelativeDirectory, "\\.");			//Set the directory to the current

	//Search for the beginning of the description
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);				//Error
		}
	}while ( strcmp(chaine, BMMAPBEGIN) != 0);

	i = 1;										//index of animation

	listanim = BMListNew ();
	//Read the file while not finished !
	while ( strcmp (chaine, BMMAPEND) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);				//Error
		}

		//------------------
		// Case of Directory
		//------------------
		if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);			//Error
			}		
		}
        //--------------------------
        // Case of animation to load
        //--------------------------
		else if ( strcmp (chaine, BMMAPEND) != 0)
		{
			//load the animations and verify that all animation are defined...
			BMANIM	*anim;
//			UDWORD	id;
			STRING	name [255];

			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);			//Error
			}

			strcpy (name, "PLAYER");
			strcat (name, chaine);

			if (BMAnimAdd (&anim, chaine, RelativeDirectory) != BMTRUE)//if (BMAnimAdd (&id, name, NULL, chaine, RelativeDirectory) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);			//Error, cannot create the anim
			}
;
			BMListAddTail (listanim, anim);
			
			i++;
		}

	}

	KMLPackClose(f);

	if (i - 1 >= numberofanim)					//i - 1 because the index is set to 1 at first
	{
		//create the array and delete the list after copying its content....	

		if (BMGamePlayerAnimation != NULL)		//if another array was loaded, delete it !
			FREE (BMGamePlayerAnimation);

		BMGamePlayerAnimation = (BMANIM **) ALLOC (sizeof (BMANIM *) * i);

		if (listanim != NULL)					
		{
			UDWORD	j;							//index of the array of animation
												
			j = 1;								//it is set to 1, cause the first animation is never used...
			while (listanim->First->Next != NULL)
			{
				BMGamePlayerAnimation [j] = (BMANIM *) listanim->First->Info;
				j++;
				BMDeleteNode(listanim->First);
			}
		}

		BMListDelete (listanim);
	}
	else
	{
		BMListDelete (listanim);
		return (BMTRUE - 1);					//error, not enough animation found in the file
	}

	return (BMTRUE);
}