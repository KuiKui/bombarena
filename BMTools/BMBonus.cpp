#include "BMBonus.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD				BMBonusMemory = 0;	//Allocate memory

BMLIST				*ListBonus;			//List of the entity, BMBONUS.
CALLBACKFUNCTION	BMBonusCallBack;	//CallBack to Call to free a bonus

//*********************************************************************
//* FUNCTION: BMBonusStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the entity API
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    23/01/2001 17:25:40
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusStart		()
{
	BMBonusCallBack = NULL;
	ListBonus = BMListNew ();
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMBonusEnd
//*--------------------------------------------------------------------
//* DESCRIPT: End the entity API, delete the rest of entity 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    23/01/2001 17:25:58
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusEnd			()
{
	while (ListBonus->First->Next != NULL)
	{
		if (BMBonusDelete ( (BMBONUS *)ListBonus->First->Info) != BMTRUE)
			return (BMTRUE - 1);
		//we don't erase the node, it's the function deleteentity which does that feature !!
	}
	BMListDelete (ListBonus);

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMBonusAdd
//*--------------------------------------------------------------------
//* DESCRIPT: Add a bonus
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr of ptr bonus, type of bonus, name of sprite, path (can be NULL)
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 09:28:27
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusAdd			(BMBONUS **NewBonus, SDWORD Type, STRING *FileName, STRING *pszPath)
{
	BMENTITY *NewEnt;

	(*NewBonus) = (BMBONUS *) ALLOC2 (sizeof (BMBONUS),BMBonusMemory);

	if (BMEntityAdd (&NewEnt, *NewBonus, FileName, BMENTITY_BONUS) != BMTRUE)	//Create the entity
		return (BMTRUE - 1);													//Error, can't create the entity
	
	(*NewBonus)->Type		= Type;
	(*NewBonus)->Entity		= NewEnt;
	(*NewBonus)->ModDatas	= NULL;

	BMListAddTail (ListBonus, *NewBonus);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMBonusDelete
//*--------------------------------------------------------------------
//* DESCRIPT: Delete a bonus, and delete it from the bonus lists
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : bonus
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 09:29:10
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusDelete		(BMBONUS *Bonus)
{
	BMNODE *Courant;
	BMNODE *Suivant;

	Courant = ListBonus->First;

	while (Suivant = Courant->Next)
	{
		if ( (BMBONUS *)Courant->Info == Bonus)
		{
			if (BMEntityDelete (Bonus->Entity) != BMTRUE)
				return (BMTRUE - 1);

			//Call the callback of the mod...
			if (BMBonusCallBack)
				(*BMBonusCallBack)(Bonus);

			FREE2 (Bonus, BMBonusMemory);

			BMDeleteNode (Courant);

			return (BMTRUE);
		}
		Courant = Suivant;
	}

	return (BMTRUE - 1);
}

//*********************************************************************
//* FUNCTION: BMBonusDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: Display the specified bonus
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the bonus
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 09:29:37
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusDisplay		(BMBONUS *Bonus)
{
	if (BMEntityDisplay(Bonus->Entity) != BMTRUE)
		return (BMTRUE - 1);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMBonusSetCallBack
//*--------------------------------------------------------------------
//* DESCRIPT: Set the function callback set by the mod
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : function to set
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    24/01/2001 14:25:03
//* REVISION:									
//*********************************************************************
SBYTE		BMBonusSetCallBack (CALLBACKFUNCTION Function)
{
	BMBonusCallBack = Function;	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMBonusResetAll ()
 *
 *  \param  void 
 *
 *  \return reset all the bonus in the map
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 12/04/2001 13:42:31
 ***************************************************************/
SBYTE		BMBonusResetAll		()
{
	while (ListBonus->First->Next)
	{
		if (BMEntityDelete ( ((BMBONUS *)ListBonus->First->Info)->Entity) != BMTRUE)
			return (BMTRUE - 1);

		//Call the callback of the mod...
		if (BMBonusCallBack)
			(*BMBonusCallBack)(((BMBONUS *)ListBonus->First->Info));

		FREE2 (ListBonus->First->Info, BMBonusMemory);

		BMDeleteNode (ListBonus->First);
	}

	return (BMTRUE);
}