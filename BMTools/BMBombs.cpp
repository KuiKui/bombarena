#include "BMBombs.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD				BMBombsMemory = 0;	//Allocate memory

BMLIST				*ListBombs;			//List of the entity, BMBOMBS.
CALLBACKFUNCTION	BMBombsCallBack;	//CallBack to Call to free a bonus

//*********************************************************************
//* FUNCTION: BMBombsStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the bombs API, create the lists ..
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 09:47:36
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsStart		()
{
	BMBombsCallBack = NULL;
	ListBombs		= BMListNew ();

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMBonusEnd
//*--------------------------------------------------------------------
//* DESCRIPT: End the bonus API, clean the lists
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:39:47
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsEnd			()
{
	while (ListBombs->First->Next != NULL)
	{
		if (BMBombsDelete ( (BMBOMBS *)ListBombs->First->Info) != BMTRUE)
			return (BMTRUE - 1);
		//we don't erase the node, it's the function deleteentity which does that feature !!
	}
	BMListDelete (ListBombs);

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMBombsAdd
//*--------------------------------------------------------------------
//* DESCRIPT: Add a bomb
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr to the new bombs, type of the bomb, time when it does explode, filename, path (can be NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:40:11
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsAdd			(BMBOMBS **NewBombs, SDWORD Type, UDWORD TimeExplode, STRING *FileName, STRING *pszPath)
{
	BMENTITY *NewEnt;

	(*NewBombs) = (BMBOMBS *) ALLOC2 (sizeof (BMBOMBS),BMBombsMemory);

	if (BMEntityAdd (&NewEnt, *NewBombs, FileName, BMENTITY_BOMBS) != BMTRUE)	//Create the entity
		return (BMTRUE - 1);													//Error, can't create the entity
	
	(*NewBombs)->Type			= Type;
	(*NewBombs)->Entity			= NewEnt;
	(*NewBombs)->ModDatas		= NULL;
	(*NewBombs)->TimeExplode	= BMTimerGetTime () + TimeExplode;

	BMListAddTail (ListBombs, *NewBombs);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMBombsDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the bombs, and remove it from the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the bombs
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:41:17
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsDelete		(BMBOMBS *Bombs)
{
	BMNODE *Courant;
	BMNODE *Suivant;

	Courant = ListBombs->First;

	while (Suivant = Courant->Next)
	{
		if ( (BMBOMBS *)Courant->Info == Bombs)
		{
			if (BMEntityDelete (Bombs->Entity) != BMTRUE)
				return (BMTRUE - 1);

			//Call the callback of the mod...
			if (BMBombsCallBack)
				(*BMBombsCallBack)(Bombs);

			FREE2 (Bombs, BMBombsMemory);

			BMDeleteNode (Courant);

			return (BMTRUE);
		}
		Courant = Suivant;
	}


	return (BMTRUE - 1);
}

//*********************************************************************
//* FUNCTION: BMBombsDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: display the specified bombs
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the bombs to display
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:41:51
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsDisplay		(BMBOMBS *Bombs)
{
	if (BMEntityDisplay(Bombs->Entity) != BMTRUE)
		return (BMTRUE - 1);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMBombsSetCallBack
//*--------------------------------------------------------------------
//* DESCRIPT: set the callback for cleaning dynamic allocation (of moddatas)
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the funcion
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:42:23
//* REVISION:									
//*********************************************************************
SBYTE		BMBombsSetCallBack (CALLBACKFUNCTION Function)
{
	BMBombsCallBack = Function;	
	return (BMTRUE);
}


//**************************************************************
/** \fn     SBYTE BMBombsResetAll ()
 *
 *  \param  void 
 *
 *  \return BMTRUE or BMFALSE
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 12/04/2001 13:47:10
 ***************************************************************/
SBYTE		BMBombsResetAll		()
{
	while (ListBombs->First->Next)
	{
		if (BMEntityDelete ( ((BMBOMBS *)ListBombs->First->Info)->Entity) != BMTRUE)
			return (BMTRUE - 1);

		//Call the callback of the mod...
		if (BMBombsCallBack)
			(*BMBombsCallBack)(((BMBOMBS *)ListBombs->First->Info));

		FREE2 (ListBombs->First->Info, BMBombsMemory);

		BMDeleteNode (ListBombs->First);

	}

	return (BMTRUE);
}


