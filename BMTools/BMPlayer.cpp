#include "BMPlayer.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"
#include "BMFile.h"				//for the BMFileReadWord function...
#include "BMAnims.h"
#include "BMDebug.h"
#include "BMSprite.h"
#include "..\BaseMod\BMod.h"
//--------------------------------
// Globales
//--------------------------------

SDWORD				BMPlayerMemory = 0;	//Allocate memory
					
BMPLAYER			BMPlayerArray[BMPLAYERMAXNUMBER];		//Array of the player
UBYTE				BMPlayerOnGame;							//Number of players in the game
UBYTE				BMPlayerCurrentId;						//Current player id free
CALLBACKFUNCTION	BMPlayerCallBack;

//*********************************************************************
//* FUNCTION: BMPlayerStart
//*--------------------------------------------------------------------
//* DESCRIPT: start the player functions
//* 
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    09/01/2001 16:46:33
//* REVISION:									    24/01/2001 13:48:56
//*********************************************************************
SBYTE				BMPlayerStart		()
{
	UDWORD i;

	BMPlayerCallBack = NULL;

	FillMemory (BMPlayerArray, sizeof(BMPLAYER) * BMPLAYERMAXNUMBER, 0);

	//set all the player to invalid state
	for (i = 0; i < BMPLAYERMAXNUMBER; i++)
		BMPlayerArray[i].PlayerState |= BMPLAYERSTATEINVALID;

	BMPlayerOnGame = 0;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMPlayerEnd
//*--------------------------------------------------------------------
//* DESCRIPT: End the Player API
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 13:15:07
//* REVISION:									
//*********************************************************************
SBYTE				BMPlayerEnd			()
{
	UBYTE i;
	
	for (i = 0; i < BMPLAYERMAXNUMBER; i++)
	{
		if (!(BMPlayerArray[i].PlayerState & BMPLAYERSTATEINVALID))
		{
			if (BMPlayerDelete (i) != BMTRUE)
				return (BMTRUE - 1);
		}
	}

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMPlayerAdd
//*--------------------------------------------------------------------
//* DESCRIPT: Add the player to the current game
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the id, color red, green, blue, name of the player
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 13:35:16
//* REVISION:									
//*********************************************************************
SBYTE				BMPlayerAdd			(UBYTE PlayerId, UBYTE CRed, UBYTE CGreen,UBYTE CBlue, STRING *Name, STRING *FileName, STRING *pszPath)
{
	if (BMPlayerArray [PlayerId].PlayerState & BMPLAYERSTATEINVALID)
	{
		BMENTITY *NewEnt;

		if (BMPlayerOnGame == BMPLAYERMAXNUMBER)
			return (BMTRUE - 1);					//can't add the player, the server is full
		
		if (BMEntityAdd (&NewEnt, &BMPlayerArray[PlayerId], FileName, BMENTITY_PLAYER) != BMTRUE)			//Create the entity
		{
			return (BMTRUE - 1);		//error, can't create the entity
		}


		BMPlayerArray [PlayerId].PlayerState &= ~BMPLAYERSTATEINVALID;
		BMPlayerArray [PlayerId].PlayerState |= BMPLAYERSTATESPECTATE;
		
		BMPlayerArray [PlayerId].ColorBlue	= CBlue;
		BMPlayerArray [PlayerId].ColorGreen	= CGreen;
		BMPlayerArray [PlayerId].ColorRed	= CRed;
		BMPlayerArray [PlayerId].Frag		= 0;
		BMPlayerArray [PlayerId].IsDead		= 1;
		BMPlayerArray [PlayerId].ModData	= NULL;			
		BMPlayerArray [PlayerId].Name		= (STRING *) ALLOC2 (strlen(Name) + 1, BMPlayerMemory);
		strcpy( BMPlayerArray [PlayerId].Name, Name);
		BMPlayerArray [PlayerId].PlayerId	= BMPlayerCurrentId;
		BMPlayerArray [PlayerId].Score		= 0;
		BMPlayerArray [PlayerId].Entity		= NewEnt;											//Link to the entity

		BMPlayerArray [PlayerId].PlayerId	= PlayerId;

		if ( BMSpriteSetAnim (BMPlayerArray [PlayerId].Entity->Sprite, BMOD_PLAYER_DOWN_STOP) != BMTRUE)
			return (BMTRUE - 1);		//Error, can't set the anim

		BMPlayerOnGame++;
	}
	else
		return (BMTRUE - 1);		//Player already in use !

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMPlayerDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete the player of the id PlayerId
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : player id
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 14:09:52
//* REVISION:									    24/01/2001 13:49:04
//*********************************************************************
SBYTE				BMPlayerDelete		(UBYTE PlayerId)
{
	//test for validity
	if (BMPlayerArray[PlayerId].PlayerState & BMPLAYERSTATEINVALID)
		return (BMTRUE - 1);		//Error, player invalid, cannot delete it !

	//Call the mod function if any
	if (BMPlayerCallBack)
	{
		(BMPlayerCallBack)(&BMPlayerArray[PlayerId]);;//test = &BMPlayerArray[PlayerId];
	}

	FREE2(BMPlayerArray[PlayerId].Name, BMPlayerMemory);
	if (BMEntityDelete ( BMPlayerArray[PlayerId].Entity) != BMTRUE)
		return (BMTRUE - 1);

	BMPlayerArray [PlayerId].PlayerState |= BMPLAYERSTATEINVALID;
	
	BMPlayerOnGame--;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMPlayerDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: Display the player of id playerId
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : player id
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 14:13:23
//* REVISION:									
//*********************************************************************
SBYTE				BMPlayerDisplay		(UBYTE PlayerId)
{
	if (BMPlayerArray[PlayerId].PlayerState & BMPLAYERSTATEINVALID)
		return (BMTRUE - 1);

	
	if (BMPlayerArray[PlayerId].PlayerState & (BMPLAYERSTATESPECTATE | BMPLAYERSTATEDEAD) )
		return (BMTRUE);

	if ( ! (BMPlayerArray[PlayerId].PlayerState & BMPLAYERSTATEOKAY ) )
		return (BMTRUE);

	return (BMEntityDisplay (BMPlayerArray[PlayerId].Entity));
}

//*********************************************************************
//* FUNCTION: BMPlayerGetPtr
//*--------------------------------------------------------------------
//* DESCRIPT: get the ptr to a player of id Id
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : **BmPlayer, id of the player to find
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    10/01/2001 14:37:45
//* REVISION:									
//*********************************************************************
SBYTE				BMPlayerGetPtr		(BMPLAYER **ppPlayer, UBYTE	Id)
{
	if (BMPlayerArray[Id].PlayerState & BMPLAYERSTATEINVALID)
	{
		*ppPlayer = NULL;
		return (BMTRUE - 1);
	}

	*ppPlayer = &BMPlayerArray[Id];

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMPlayerSetCallBack
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
SBYTE				BMPlayerSetCallBack (CALLBACKFUNCTION Function)
{
	BMPlayerCallBack = Function;	
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMPlayerSetReady (UBYTE PlayerId)
 *
 *  \param  PlayerId id of the player
 *
 *  \return BMTRUE or BMFALSE
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 12/04/2001 10:20:45
 ***************************************************************/
SBYTE				BMPlayerSetReady	(UBYTE PlayerId)
{
	if (PlayerId >BMPLAYERMAXNUMBER)
		return (BMFALSE);

	BMPlayerArray [ PlayerId ].PlayerState &= ~	( BMPLAYERSTATEDEAD	| BMPLAYERSTATEISDYING | BMPLAYERSTATESPECTATE );
	BMPlayerArray [ PlayerId ].PlayerState |= BMPLAYERSTATEOKAY;
	return (BMTRUE);
}

//**************************************************************
/** \fn     SBYTE BMPlayerIsReady (UBYTE playerId)
 *
 *  \param  playerId
 *
 *  \return  BMTRUE or BMFALSE
 *
 ***************************************************************
 *  \brief  check if the player is ready
 *
 *  \author MikE                       \date 12/04/2001 13:39:49
 ***************************************************************/
SBYTE			BMPlayerIsReady		(UBYTE PlayerId)
{
	if (PlayerId >BMPLAYERMAXNUMBER)
		return (BMFALSE);
	if (BMPlayerArray [ PlayerId ].PlayerState & BMPLAYERSTATEINVALID)
		return (BMFALSE);

	if (BMPlayerArray [ PlayerId ].PlayerState & BMPLAYERSTATEOKAY)
		return (BMTRUE);

	return (BMFALSE);
}

