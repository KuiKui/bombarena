#include "BMMap.h"
#include "BMFile.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"
#include "BMGame.h"
#include "BMImage.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD			BMMapMemory = 0; //Allocate memory

BMMAPSOL		BMMapSol			[BMMAP_WIDTH * BMMAP_HEIGHT];
BMENTITY		*BMMapEntityArray	[BMMAP_WIDTH * BMMAP_HEIGHT];

BMMAPTYPE		*BMMapPtrPlayer;

BMLIST			*BMMapObject;						//list of the map object, BMMAPTYPE...

KMLIMAGE		*BMMapBackGround;					//map's background 

BMMAPARRAY		BMMapArray [256];					//Array of the element of the map (included bonus, bloc, flames...)

SBYTE			BMMapDensity;						//Density in the map
BMLIST			*BMListBonus;						//List of bonus available in this map, BMMAPBONUS...

BMMAPPOSPLAYER	BMMapPosPlayer [BMPLAYERMAXNUMBER];	//Array of the initial position of the players
SBYTE			BMMapMaxPlayer;						//Number of max player for this map

STRING			pszMapElement	[255];				//Current Map Element
STRING			pszMapField		[255];				//Current Map Field


//*********************************************************************
//* FUNCTION: BMMapStart			
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/12/2000 11:44:35
//* REVISION:									    24/01/2001 17:36:50
//*********************************************************************
SBYTE		BMMapStart			()
{
	UDWORD	i,
			j;

	BMMapBackGround = NULL;

	//reset the tables
	FillMemory (BMMapSol, BMMAP_WIDTH * BMMAP_HEIGHT * sizeof (BMMAPSOL), 0);
	FillMemory (BMMapArray, sizeof (BMMapArray), 0);

	BMMapPtrPlayer	= NULL;
	BMMapObject		= NULL;

	FillMemory (BMMapPosPlayer, BMPLAYERMAXNUMBER * sizeof (BMMAPPOSPLAYER), 0);


	for (j = 0; j < BMMAP_HEIGHT; j++)
	{
		for (i = 0; i < BMMAP_WIDTH; i++)
		{
			BMMapEntityArray [ i + j * BMMAP_WIDTH] = NULL;
			//BMMapEntityArray [ i + j * BMMAP_WIDTH] = (BMENTITY *) ALLOC2 (sizeof(BMENTITY), BMMapMemory);	
			//FillMemory (BMMapEntityArray [ i + j * BMMAP_WIDTH], sizeof(BMENTITY), 0);
		}
	}

	BMMapDensity	= 100;		//Unactive set to 100%
	BMListBonus		= NULL;		//no bonus
	BMMapMaxPlayer	= 0;		//no player position 

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapEnd 
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/12/2000 11:50:07
//* REVISION:									    24/01/2001 17:37:32
//*********************************************************************
SBYTE		BMMapEnd			()
{
	UDWORD	i;

	for (i = 0; i < 255; i++)
	{
		if (BMMapArray[i].AnimName != NULL)
			FREE2(BMMapArray[i].AnimName, BMMapMemory);
		if (BMMapArray[i].Anim != NULL)		//this had to be delete by the main list of anim
			BMAnimDelete (BMMapArray[i].Anim);
	}

	if (BMMapBackGround != NULL)
		BMImageDelete (BMMapBackGround);

	if (BMMapObject != NULL)
	{
		while (BMMapObject->First->Next != NULL)
			BMDeleteNode(BMMapObject->First);		//REVOIR FAIRE LE FREE DE L'OBJECT

		BMListDelete (BMMapObject);
	}

	if (BMListBonus != NULL)
	{
		while (BMListBonus->First->Next != NULL)
		{
			FREE2 ( (BMMAPBONUS *)BMListBonus->First->Info, BMMapMemory);
			BMDeleteNode(BMListBonus->First);		
		}

		BMListDelete (BMListBonus);
	}

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapLoadElement
//*--------------------------------------------------------------------
//* DESCRIPT: Load the map element in the file which describe the element
//* kms file and their number, then load the map blocks
//*--------------------------------------------------------------------
//* IN      : STRING *pszfichier
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/12/2000 14:43:02
//* REVISION:									
//*********************************************************************
SBYTE		BMMapLoadElement	(STRING *pszFichier)
{
	KMLFILE	*f;
	STRING	chaine [255];
	SBYTE	bAscii;

	STRING	RelativeDirectory [255];

	SBYTE	i,
			j;

	if (BMListBonus != NULL)					//if there are bonus loaded yet, delete them cause, we're loading a new map !
	{
		while (BMListBonus->First->Next != NULL)
			BMDeleteNode(BMListBonus->First);
	}

	bAscii		= 0;

	strcpy(RelativeDirectory, "\\.");			//Set the directory to the current

	

	f = KMLPackOpen (NULL, pszFichier);

	if (f == NULL)
		return (BMTRUE - 1);			//error, can't open the file

	//Search for the beginning of the description
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}
	}while ( strcmp(chaine, BMMAPBEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, BMMAPEND) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		//-----------------------
		//Case of Number or Ascii
		//-----------------------
		if (strcmp (chaine, BMMAPACTIVENUMBER) == 0)
			bAscii = 0;
		else if (strcmp(chaine, BMMAPACTIVEASCII) == 0)
			bAscii = 1;
		else if (strcmp(chaine, BMMAPACTIVEHEXA) == 0)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);
			//REVOIR RAJOUTER LE CAS HEXA ! ! !
		}
		//-------------
		//Case of Bonus
		//-------------
		else if (strcmp (chaine,	BMMAPBONUSMAP) == 0)
		{
			UBYTE	indexbonus,
					number;

			//read the index of the bonus
			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}


			indexbonus = atoi (chaine);

			if ( (indexbonus >= 0) && (indexbonus <= 255) ) 
			{
				//read the number of bonus
				if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, number not valid
				}

				number = atoi (chaine);

				if (number > 0)
				{
					//if the number is inferior to the min value do nothing !
					BMMAPBONUS *bonus;
					
					//verify if the bonus exist !
					if (BMMapArray [indexbonus].AnimName == NULL)
					{
						KMLPackClose (f);
						return (BMTRUE - 1);	//Error, the specified bonus doesn't exist or the bonus are not loaded yet !
					}

					//create the list if it's null
					if (BMListBonus == NULL)
						BMListBonus = BMListNew ();

					//add the bonus in the list
					bonus = (BMMAPBONUS *) ALLOC2 (sizeof (BMMAPBONUS), BMMapMemory);

					bonus->Identifier	= indexbonus;
					bonus->Number		= number;

					BMListAddTail (BMListBonus, bonus);
				}
				else
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, number not valid
				}
			}
			else 
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}

		//---------------
		//Case of Players
		//---------------
		else if (strcmp (chaine, BMMAPPLAYER) == 0)
		{
			UBYTE	xcoord,
					ycoord;

			//read the x coords
			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}


			xcoord = atoi (chaine);

			if ( (xcoord >= 0) && (xcoord <= BMMAP_WIDTH) ) 
			{
				//read the y coords
				if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, number not valid
				}

				ycoord = atoi (chaine);

				if ( (ycoord >= 0) && (ycoord <= BMMAP_HEIGHT) ) 
				{
					if (BMMapMaxPlayer != BMPLAYERMAXNUMBER - 1)	//if the number exceed the max value do nothing !
					{
						BMMapPosPlayer[BMMapMaxPlayer].CoordX = xcoord;
						BMMapPosPlayer[BMMapMaxPlayer].CoordY = ycoord;

						BMMapMaxPlayer++;
					}
				}
				else
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, number not valid
				}
			}
			else 
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}

		//---------------
		//Case of Density
		//---------------
		else if (strcmp (chaine, BMMAPDENSITY) == 0)
		{
			UBYTE numbertemp;

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}


			numbertemp = atoi (chaine);

			if ( (numbertemp >= 0) && ( numbertemp <= 100) )
				BMMapDensity = numbertemp;
		}
		//------------------
		// Case of Directory
		//------------------
		else if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}
		//------------------------------
		//Description of map's element !
		//------------------------------
		else if ( (strcmp (chaine , BMMAPDESTRUCTIBLE) == 0) || (strcmp (chaine, BMMAPUNDESTRUCTIBLE) == 0) )
		{
			STRING	chaine2		[255];
			UBYTE	number;

			if (strcmp( chaine, BMMAPDESTRUCTIBLE) == 0)
				number = BMMAP_DEST;
			else 
				number = BMMAP_UNDEST;

			//Read the name of the kms file
			if (BMFileReadWord (&f, chaine2, sizeof(chaine2)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			//Add the Anim
			if ( BMAnimAdd (&BMMapArray [number].Anim, chaine2, RelativeDirectory) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);
			}
			
			//Save the name of the anim in the array
			BMMapArray [number].Type		= BMMAPANIM;
			BMMapArray [number].AnimName	= (STRING *) ALLOC2 (strlen( chaine2) + 1, BMMapMemory);

			strcpy ( BMMapArray[number].AnimName, chaine2);
		}
	}

	//Read the map now
	while ( strcmp(chaine, BMMAPBEGINMAP) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}
	}

	//Read the file while not finished !
	i = 0;
	j = 0;
	
	if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
	{
		KMLPackClose (f);
		return (BMTRUE - 1);	//Error
	}

	while ( strcmp (chaine, BMMAPENDMAP) != 0)
	{
		UBYTE		number;
		BMMAPSOL	mapsol;

		if (bAscii == 0)
		{
			if (chaine[0] == '"')
				number = (SBYTE) chaine[1];
			else
				number = atoi (chaine);
		}
		else
		{
			if (chaine[0] == '"')
			{
				SDWORD k;

				k = 1;
				while (chaine[k] != '"')
					k++;
				chaine[k] = '\0';

				number = atoi(&chaine[1]);
			}
			else
				number = (UBYTE) chaine[0];
		}

	
		mapsol.SolType = 0;
		mapsol.SolInfo = 0;
		mapsol.SolAnim = NULL;
		mapsol.BlocType = number;
		mapsol.BlocInfo = 0;
		mapsol.BlocAnim = NULL;		

		if (BMMapSetBlock (i, j, mapsol) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		i++;
		if (i == BMMAP_WIDTH)
		{
			i = 0;
			j++;
		}

		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}
	}

	KMLPackClose (f);

	strcpy ( pszMapElement, pszFichier);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapLoadField
//*--------------------------------------------------------------------
//* DESCRIPT: Load the field of the map
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : name of file to load
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    05/01/2001 10:30:43
//* REVISION:									
//*********************************************************************
SBYTE		BMMapLoadField		(STRING	*pszFichier)
{
	KMLFILE *f;

	STRING	chaine [255];
	STRING	RelativeDirectory [255];
	SBYTE	bAscii;
	SBYTE	i,
			j;

	f = KMLPackOpen (NULL, pszFichier);

	if (f == NULL)
		return (BMTRUE - 1);	//Error, can't open the file !

	strcpy(RelativeDirectory, "\\.");			//Set the directory to the current

	//Search for the beginning of the file
	do
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}	
	}while ( strcmp(chaine, BMMAPBEGIN) != 0);

	//Read the file while not finished !
	while ( strcmp (chaine, BMMAPEND) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		//-------------------------------
		//Case of Number or Ascii or Hexa
		//-------------------------------
		if (strcmp (chaine, BMMAPACTIVENUMBER) == 0)
			bAscii = 0;
		else if (strcmp(chaine, BMMAPACTIVEASCII) == 0)
			bAscii = 1;
		else if (strcmp(chaine, BMMAPACTIVEHEXA) == 0)
			bAscii = 2;
		//------------------
		// Case of Directory
		//------------------
		else if ( strcmp (chaine, BMSETDIRECTORY) == 0)
		{
			if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}
		}
		//-------------------
		// Case of Background
		//-------------------
		else if (strcmp(chaine, BMMAPBACKGROUND) == 0)
		{
			STRING temp [255];

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			strcpy (temp, RelativeDirectory);
			strcat (temp, chaine);

			if (BMMapBackGround != NULL)
				BMImageDelete (BMMapBackGround);

			BMMapBackGround = BMImageCreate (0, 0, KML_BMP, temp);

			if (BMMapBackGround == NULL)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error, cannot load the image !
			}

			if ( (BMMapBackGround->Hauteur != (unsigned) GlobalResY) || (BMMapBackGround->Largeur != (unsigned) GlobalResX) )
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error, the size doesn't match with the screen !
			}
		}
		//------------------------------
		//Description of map's element !
		//------------------------------
		else if (strcmp (chaine , BMMAPEND) != 0) 
		{
			STRING	chaine2		[255];
			STRING	chaine3		[255];
			UBYTE	number;

			//Read the number of the anim
			if (BMFileReadWord (&f, chaine2, sizeof(chaine2)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			//Read if it's bmmapcolor or not
			if (BMFileReadWord (&f, chaine3, sizeof(chaine3)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			//Get the number from the chaine2
			if (chaine2[0] == '"')									//Ascii
			{
				number = (UBYTE) chaine2[1];
			}
			else if ( (chaine2[0] == '0') && ( (chaine2[1] == 'x') || (chaine2[1] == 'X') ) )	//Hexa
			{
				UBYTE i = 2;
				
				number = 0;

				while (chaine2[i] != '\0')
				{
					number *= 16;

					if ( (chaine2[i] >= '0') && (chaine2[i] <= '9') )
						number += (UBYTE) chaine2[i] - '0';
					else if ( (chaine2[i] >= 'a') && (chaine2[i] <= 'f') )
						number += (UBYTE) chaine2[i] - 'a' + 10;
					else if ( (chaine2[i] >= 'A') && (chaine2[i] <= 'F') )
						number += (UBYTE) chaine2[i] - 'A' + 10;
					else														
					{
						KMLPackClose (f);
						return (BMTRUE - 1);	//Error, bad character
					}

					i++;
				}
			}
			else													//Number
				number = atoi (chaine2);

			//Verify that the anim is valid
			if ( BMAnimAdd (&BMMapArray [number].Anim, chaine, RelativeDirectory) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error, the anim file is not valid
			}
		
			//Save the anim in the array
			BMMapArray [number].Type		= ( (atoi (chaine3) == 0) ? BMMAPANIM : BMMAPCOLOR);
			BMMapArray [number].AnimName	= (STRING *) ALLOC2 (strlen(chaine) + 1, BMMapMemory);
			strcpy( BMMapArray [number].AnimName, chaine);
		}
	}

	//Read the map now
	while ( strcmp(chaine, BMMAPBEGINMAP) != 0)
	{
		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}
	}

	//Read the file while not finished !
	i = 0;
	j = 0;
	
	if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
	{
		KMLPackClose (f);
		return (BMTRUE - 1);	//Error
	}

	while ( strcmp (chaine, BMMAPENDMAP) != 0)
	{
		UBYTE		type;
		UBYTE		info;
		BMMAPSOL	mapsol;

		if (bAscii == 0)		//number
		{
			if (chaine[0] == '"')
				type = (SBYTE) chaine[1];
			else
				type = atoi (chaine);

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			if (chaine[0] == '"')
				info = (SBYTE) chaine[1];
			else
				info = atoi (chaine);
		}
		else if (bAscii == 1)	//ascii
		{
			if (chaine[0] == '"')
			{
				SDWORD k;

				k = 1;
				while (chaine[k] != '"')
					k++;
				chaine[k] = '\0';

				type = atoi(&chaine[1]);
			}
			else
				type = (UBYTE) chaine[0];

			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMTRUE - 1);	//Error
			}

			if (chaine[0] == '"')
			{
				SDWORD k;

				k = 1;
				while (chaine[k] != '"')
					k++;
				chaine[k] = '\0';

				info = atoi(&chaine[1]);
			}
			else
				info = (UBYTE) chaine[0];


		}
		else					//hexa
		{
			UBYTE k;

			type = 0;
			info = 0;

			for (k = 0; k < 2; k++)
			{
				type *= 16;
				if ( (chaine[k] >= '0') && (chaine[k] <= '9') )
					type += (UBYTE) chaine[k] - '0';
				else if ( (chaine[k] >= 'a') && (chaine[k] <= 'f') )
					type += (UBYTE) chaine[k] - 'a' + 10;
				else if ( (chaine[k] >= 'A') && (chaine[k] <= 'F') )
					type += (UBYTE) chaine[k] - 'A' + 10;
				else														
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, bad character
				}
			}

			for (k = 2; k < 4; k++)
			{
				info *= 16;
				if ( (chaine[k] >= '0') && (chaine[k] <= '9') )
					info += (UBYTE) chaine[k] - '0';
				else if ( (chaine[k] >= 'a') && (chaine[k] <= 'f') )
					info += (UBYTE) chaine[k] - 'a' + 10;
				else if ( (chaine[k] >= 'A') && (chaine[k] <= 'F') )
					info += (UBYTE) chaine[k] - 'A' + 10;
				else														
				{
					KMLPackClose (f);
					return (BMTRUE - 1);	//Error, bad character
				}
			}


		}

		mapsol.SolType = type;
		mapsol.SolInfo = info;
		mapsol.SolAnim = NULL;//BMMapArray [info].AnimName;
		mapsol.BlocType = 0;
		mapsol.BlocInfo = 0;
		mapsol.BlocAnim = NULL;

		if (BMMapSetSol (i, j, mapsol) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}

		i++;
		if (i == BMMAP_WIDTH)
		{
			i = 0;
			j++;
		}

		if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
		{
			KMLPackClose (f);
			return (BMTRUE - 1);	//Error
		}
	}

	KMLPackClose (f);

	strcpy ( pszMapField, pszFichier);
	return (BMTRUE);
}
//*********************************************************************
//* FUNCTION: BMMapDisplay		
//*--------------------------------------------------------------------
//* DESCRIPT: Display the map in the image at the coords x, y 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : x, y
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    19/12/2000 15:34:05
//* REVISION:									
//*********************************************************************
SBYTE		BMMapDisplay		(KMLIMAGE *image, SDWORD x, SDWORD y)
{
	SDWORD  i,
			j;

	if (BMMapBackGround != NULL)
	{
		if ( image == NULL)
			KMLFastBlt (	NULL,
							0,
							0,
							BMMapBackGround,
							0,
							0,
							BMMapBackGround->Largeur,
							BMMapBackGround->Hauteur
						);
		else
			KMLStretchBlt (	image,
							0,
							0,
							image->Largeur,
							image->Hauteur,
							BMMapBackGround,
							0,
							0,
							BMMapBackGround->Largeur,
							BMMapBackGround->Hauteur
						);
	}

	for (j = 0; j < BMMAP_HEIGHT; j++)
	{
		for (i = 0; i < BMMAP_WIDTH; i++)
		{
			BMMAPSOL Value;

			Value = BMMapSol [ i + j * BMMAP_WIDTH];
			//Display the case

			//Sol Info
			if (BMMapArray [Value.SolType].Type == BMMAPANIM)
			{
				BMANIM *Anim;

				Anim = Value.SolAnim;

				if (Anim != NULL)
				{
					BMAnimUpdate (Anim);

					KMLFastBlt (	image,
									x + i * BMMAP_ELEMENTSIZE,
									y + j * BMMAP_ELEMENTSIZE,
									Anim->Model->Image,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameUCoords,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameVCoords,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameWidth,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameHeight
								);
				}
			}
			else if (BMMapArray [Value.SolType].Type == BMMAPCOLOR)
			{
				BMANIM *Anim;

				Anim = Value.BlocAnim;

				if (Anim != NULL)
				{
					BMAnimUpdate (Anim);

					//Perform the blit with the color of the player

				}
			}

			//Bloc Info
			if (BMMapArray [Value.BlocType].Type == BMMAPANIM)
			{
				BMANIM *Anim;

				Anim = Value.BlocAnim;

				if (Anim != NULL)
				{
					KMLFastBlt (	image,
									x + i * BMMAP_ELEMENTSIZE,
									y + j * BMMAP_ELEMENTSIZE,
									Anim->Model->Image,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameUCoords,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameVCoords,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameWidth,
									((BMANIMFRAME *)Anim->CurrentFrame->Info)->AnimFrameHeight
								);
				}
			}
			else if (BMMapArray [Value.BlocType].Type == BMMAPCOLOR)
			{
				BMANIM *Anim;

				Anim = Value.BlocAnim;//(BMANIM *) BMMapArray [Value.BlocType].Info;

				if (Anim != NULL)
				{
					//Perform the blit with the color of the player

				}
			}
			else if (BMMapArray [Value.BlocType].Type == 0)	//no bloc so we can blit the bonus or something else like bombs...
			{															
				if (BMMapEntityArray[i + j * BMMAP_WIDTH] != NULL)
				{
					if (BMEntityDisplay(BMMapEntityArray[i + j * BMMAP_WIDTH]) != BMTRUE)
						return (BMTRUE - 1);
				}
			}
		}
	}

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapSetCase
//*--------------------------------------------------------------------
//* DESCRIPT: Set the case of the array to the BMMAPSOL value 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : coord x, coord y, value to set
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    03/01/2001 16:39:36
//* REVISION:									
//*********************************************************************
SBYTE		BMMapSetCase		(SDWORD x, SDWORD y, BMMAPSOL Value)
{
//	BMANIM *animtemp;

	//Verify the value
	if (Value.SolType != BMMAP_NORM)								//if it is BMModNormalCase, nothing on this case !
	{
		if (BMMapArray [Value.SolType].AnimName == NULL)
			return (BMTRUE - 1);						//Error, the type is unknown
	}

	//Set the value
	BMMapSol [x + y * BMMAP_WIDTH] = Value;

	//Launch the anims
	BMAnimPlay (Value.BlocAnim);
	BMAnimPlay (Value.SolAnim);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapSetSol 
//*--------------------------------------------------------------------
//* DESCRIPT: set the sol of the map
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : coord x, coord y, value
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    04/01/2001 17:43:36
//* REVISION:									
//*********************************************************************
SBYTE		BMMapSetSol			(SDWORD x, SDWORD y, BMMAPSOL Value)
{
	if (Value.SolType != BMMAP_NORM )								//if it is BMModNormalCase, nothing on this case !
	{
		if (BMMapArray [Value.SolType].AnimName == NULL)
			return (BMTRUE - 1);						//Error, the type is unknown
	}

	//Launch the anim
	if (Value.SolAnim == NULL)
	{
		if (BMMapArray[Value.SolType].AnimName != NULL)
		{
			if (BMAnimAdd (&Value.SolAnim, BMMapArray [Value.SolType].AnimName) != BMTRUE)
				return (BMTRUE - 1);		//Error, cannot add the anim
		}
	}

	//Set the value
	BMMapSol[x + y * BMMAP_WIDTH].SolType = Value.SolType;
	BMMapSol[x + y * BMMAP_WIDTH].SolInfo = Value.SolInfo;
	BMMapSol[x + y * BMMAP_WIDTH].SolAnim = Value.SolAnim;

	if (Value.SolAnim != NULL)
		BMAnimPlay (Value.SolAnim);

  return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapSetBlock
//*--------------------------------------------------------------------
//* DESCRIPT: set the info of the block
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : coord x, coord y, value
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    04/01/2001 17:44:02
//* REVISION:									
//*********************************************************************
SBYTE		BMMapSetBlock		(SDWORD x, SDWORD y, BMMAPSOL Value)
{
//	BMANIM *animtemp;

	if (Value.BlocAnim == NULL)
	{
		if (BMMapArray[Value.BlocType].AnimName != NULL)
		{
			if (BMAnimAdd (&Value.BlocAnim, BMMapArray[Value.BlocType].AnimName) != BMTRUE)
				return (BMTRUE - 1);		//Error, cannot add the anim
		}
	}

	//Set the value
	BMMapSol[x + y * BMMAP_WIDTH].BlocType = Value.BlocType;
	BMMapSol[x + y * BMMAP_WIDTH].BlocInfo = Value.BlocInfo;
	BMMapSol[x + y * BMMAP_WIDTH].BlocAnim = Value.BlocAnim;

	//Launch the anim
	if (Value.BlocAnim != NULL)
		BMAnimPlay (Value.BlocAnim);
	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapSetObject
//*--------------------------------------------------------------------
//* DESCRIPT: Set the object to the case of the map, object can be NULL
//* so that it deletes the object                                                                    
//*--------------------------------------------------------------------
//* IN      : coord x, coord y, ojecttoset (the object in the list of objects... or NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    09/01/2001 10:41:15
//* REVISION:									
//*********************************************************************
SBYTE		BMMapSetObject		(SDWORD x, SDWORD y, BMENTITY *ObjectToSet)
{
	BMENTITY *Entity;

	Entity = BMMapEntityArray [x + y * BMMAP_WIDTH];
	//verify if there's any object on this case...
	if (ObjectToSet != NULL)							//if objecttoset is null we want to delete an object, so don't care...
	{
		if (Entity != NULL)
		{
			if (Entity != ObjectToSet)
			{
				//there's another entity on this case...

				return (BMTRUE - 1); //for the moment, see what to do later...
			}
		}
	}

	BMMapEntityArray [x + y * BMMAP_WIDTH] = ObjectToSet;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapGetMaxPlayer
//*--------------------------------------------------------------------
//* DESCRIPT: get the max number of player or client for this map
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to a number which is filled by the function
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 13:17:54
//* REVISION:									
//*********************************************************************
SBYTE		BMMapGetMaxPlayer	(UBYTE	*numberplayer)
{
	*numberplayer = BMMapMaxPlayer;
	return (BMTRUE);
}

SBYTE		BMBlitPalettised (SDWORD CoordX, SDWORD CoordY, BMANIM *AnimToBlit, UBYTE CRed, UBYTE CGreen, UBYTE CBlue)
{

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMMapGetBonusInit
//*--------------------------------------------------------------------
//* DESCRIPT: get the ptr to the list of the bonus to initialize
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to bmlist
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 17:40:38
//* REVISION:									
//*********************************************************************
SBYTE		BMMapGetBonusInit	(BMLIST	*BonusList)
{
	BonusList = BMListBonus;	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMMapReset ()
 *
 *  \param  void 
 *
 *  \return BMTRUE or BMFALSE
 *
 ***************************************************************
 *  \brief  Reset the map ( so reload it )
 *
 *  \author MikE                       \date 05/04/2001 14:13:29
 ***************************************************************/
SBYTE		BMMapReset			()
{
	if (BMMapLoadElement ( pszMapElement ) != BMTRUE)
		return ( BMFALSE );

	return ( BMMapLoadField ( pszMapField ) );
}

/*
description
  sol de la map
  image
  image de fond

  fichier de map

  bloc destructible
  indestructible

  joueur
  densite
  bonus


  */

