#include "BMEntity.h"
#include	"BMMap.h"
#include	"BMBombs.h"
#include	"BMBonus.h"


//--------------------------------
// Globales
//--------------------------------
SDWORD				BMEntityMemory = 0;	//Allocate memory

BMLIST				*ListEntity;			//List of the entity, BMENTITY...
CALLBACKFUNCTION	BMEntityCallBack;

BMNODE				*EntityBombsFirst;
BMNODE				*EntityBonusFirst;
BMNODE				*EntityPlayerFirst;

//*********************************************************************
//* FUNCTION: BMEntityStart
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
SBYTE		BMEntityStart		()
{
	BMEntityCallBack	= NULL;
	ListEntity			= BMListNew ();
	EntityBombsFirst	= ListEntity->Null;
	EntityBonusFirst	= ListEntity->Null;
	EntityPlayerFirst	= ListEntity->Null;
	
	;return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityEnd
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
SBYTE		BMEntityEnd			()
{
	while (ListEntity->First->Next != NULL)
	{
		if (BMEntityDelete ( (BMENTITY *)ListEntity->First->Info) != BMTRUE)
			return (BMTRUE - 1);
		//we don't erase the node, it's the function deleteentity which does that feature !!
	}
	BMListDelete (ListEntity);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityAdd	
//*--------------------------------------------------------------------
//* DESCRIPT: Add an entity of type Type, we load the entity infos in the
//* file pszFileName
//*--------------------------------------------------------------------
//* IN      : ptr to new entity, ptr to the object to link, name of the sprite, type
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    23/01/2001 14:55:20
//* REVISION:									
//*********************************************************************
SBYTE		BMEntityAdd			(BMENTITY **NewEntity, void *ObjectToLink, STRING *NameSprite, SDWORD Type)
{
	*NewEntity = (BMENTITY *) ALLOC2 (sizeof(BMENTITY), BMEntityMemory);
	
	if ( BMSpriteAdd ( &(*NewEntity)->Sprite, NameSprite, NULL) != BMTRUE)
		return (BMTRUE - 1);				//Error, can't add the sprite, verify that it's already load in models...

	(*NewEntity)->Type			= Type;
	(*NewEntity)->ModData		= NULL;
	(*NewEntity)->BlockPos.x	= 0xffffffff;
	(*NewEntity)->BlockPos.y	= 0xffffffff;
	(*NewEntity)->Flag			= 0;
	(*NewEntity)->Pos.x			= -1.0f;
	(*NewEntity)->Pos.y			= -1.0f;
	(*NewEntity)->EntityInfo	= ObjectToLink;

	if (Type == BMENTITY_PLAYER)									//insert a player
	{
		if (EntityPlayerFirst != ListEntity->Null)
		{
			BMInsertNode (EntityPlayerFirst->Previous, *NewEntity);	//insert the player at the first pos on the list player
			EntityPlayerFirst = EntityPlayerFirst->Previous;		//update ptr to the first player
		}
		else
		{
			BMListAddHead (ListEntity, *NewEntity);
			EntityPlayerFirst = ListEntity->First;
		}
	}
	else if (Type == BMENTITY_BONUS)								//insert a bonus
	{
		if (EntityBonusFirst != ListEntity->Null)
		{
			BMInsertNode (EntityBonusFirst->Previous, *NewEntity);
			EntityBonusFirst = EntityBonusFirst->Previous;
		}
		else
		{
			BMListAddHead (ListEntity, *NewEntity);
			EntityBonusFirst = ListEntity->First;
		}
	}
	else if (Type == BMENTITY_BOMBS)								//insert a bomb
	{
		if (EntityBombsFirst != ListEntity->Null)
		{
			BMBOMBS	*Bombs;
			BMNODE	*Courant;
			BMNODE	*Suivant;
			BMNODE	*PosInsert;
			UBYTE	Ok;
			
			Bombs		= (BMBOMBS *) ((*NewEntity)->EntityInfo);		//the bomb to insert

			//parse the list to find where to insert (with the time...)
			Courant		= EntityBombsFirst;
			PosInsert	= Courant;
			Ok = 0;

			while ((Suivant = Courant->Next) && (Ok == 0))
			{
				if ( ((BMENTITY *)Courant->Info)->Type == BMENTITY_BOMBS)
				{
					if (Bombs->TimeExplode <= ((BMBOMBS *)((BMENTITY *)Courant->Info)->EntityInfo)->TimeExplode)
						Ok = 1;
				}
				else
					Ok = 1;

				PosInsert	= Courant;
				Courant		= Suivant;
			}

			if (PosInsert == EntityBombsFirst)
			{
				BMInsertNode (PosInsert->Previous, *NewEntity);
				EntityBombsFirst  = PosInsert->Previous;
			}
			else
				BMInsertNode (PosInsert->Previous, *NewEntity);
		}
		else
		{
			BMListAddHead (ListEntity, *NewEntity);
			EntityBombsFirst = ListEntity->First;
		}
	}
	else															//unknow or mod type
		BMListAddTail (ListEntity, *NewEntity);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityDelete
//*--------------------------------------------------------------------
//* DESCRIPT: Delete the specified entity
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the entity
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    23/01/2001 15:29:41
//* REVISION:									
//*********************************************************************
SBYTE		BMEntityDelete		(BMENTITY *Entity)
{
	BMNODE  *Courant;
	BMNODE	*Suivant;

	Courant = ListEntity->First;

	//parse the list of entity
	while (Suivant = Courant->Next)
	{
		if ( (BMENTITY *)Courant->Info == Entity)			//Entity found
		{
			//Update entity ptr
			if (Entity->Type == BMENTITY_BOMBS)
			{
				if (EntityBombsFirst == Courant)
				{
					if ( ((BMENTITY *)Courant->Next->Info)->Type == BMENTITY_BOMBS)
						EntityBombsFirst = Courant->Next;
					else
						EntityBombsFirst = ListEntity->Null;
				}
			}
			else if (Entity->Type == BMENTITY_BONUS)
			{
				if (EntityBonusFirst == Courant)
				{
					if ( ((BMENTITY *)Courant->Next->Info)->Type == BMENTITY_BONUS)
						EntityBonusFirst = Courant->Next;
					else
						EntityBonusFirst = ListEntity->Null;
				}
			}
			else if (Entity->Type == BMENTITY_PLAYER)
			{
				if (EntityPlayerFirst == Courant)
				{
					if ( ((BMENTITY *)Courant->Next->Info)->Type == BMENTITY_PLAYER)
						EntityPlayerFirst = Courant->Next;
					else
						EntityPlayerFirst = ListEntity->Null;
				}
			}
			//End of Update entity ptr

			if (Entity->Flag & BMENTITY_ONMAP)				//test if the object is on map
			{
				if (BMMapEntityArray [Entity->BlockPos.x + Entity->BlockPos.y * BMMAP_WIDTH] != Entity)		//if the object isn't in the array, problem...
				{
					//see what to do...
					//return (BMTRUE - 1);
				}
				else
				{
					if (BMMapSetObject (Entity->BlockPos.x, Entity->BlockPos.y, NULL) != BMTRUE)
						return (BMTRUE - 1);
				}
			}

			if (BMSpriteDelete (Entity->Sprite) != BMTRUE)
				return (BMTRUE - 1);

			//Call the mod function if any
			if (BMEntityCallBack)
				(*BMEntityCallBack)(Entity);

			FREE2 (Entity, BMEntityMemory);
			BMDeleteNode (Courant);
	
			return (BMTRUE);
		}

		Courant = Suivant;
	}
	//Entity not found !!!
	return (BMTRUE - 1);
}

//*********************************************************************
// Name	: BMEntitySetBlockPosition
//---------------------------------------------------------------------
// In	: entity, pos of the point of the sprite
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								24/01/2001 11:27:42
//*********************************************************************
SBYTE BMEntitySetBlockPosition(BMENTITY *Entity, float PosX, float PosY, UBYTE flag)
{
	BMLPOINT LastPos;

	LastPos.x = Entity->BlockPos.x;
	LastPos.y = Entity->BlockPos.y;

	if(flag)
	{
		Entity->BlockPos.x = (SDWORD)( (PosX + (BMMAP_ELEMENTSIZE>>1) ) / BMMAP_ELEMENTSIZE );
		Entity->BlockPos.y = (SDWORD)( (PosY + (BMMAP_ELEMENTSIZE>>1) ) / BMMAP_ELEMENTSIZE );
	}
	else
	{
		Entity->BlockPos.x = (SDWORD)(PosX / BMMAP_ELEMENTSIZE);
		Entity->BlockPos.y = (SDWORD)(PosY / BMMAP_ELEMENTSIZE);
	}

	if (Entity->Flag & BMENTITY_ONMAP)		//the entity is on the map, so update its position
	{
		if ( (LastPos.x != Entity->BlockPos.x) || (LastPos.y != Entity->BlockPos.y) )	//if the position has changed
		{
			if (BMMapEntityArray [LastPos.x + LastPos.y * BMMAP_WIDTH] == Entity)
				BMMapSetObject (LastPos.x, LastPos.y, NULL);					//we remove the object from the last pos
			BMMapSetObject (Entity->BlockPos.x, Entity->BlockPos.y, Entity);//we put it on its new pos
		}
	}

	return (BMTRUE);
}

//*********************************************************************
// Name	: BMEntitySetRealPosition
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								24/01/2001 11:27:35
//*********************************************************************
SBYTE BMEntitySetRealPosition(BMENTITY *Entity, float fX, float fY)
{

	Entity->Pos.x = fX;
	Entity->Pos.y = fY;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityAddFlag
//*--------------------------------------------------------------------
//* DESCRIPT: add flag to the entity
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : entity, flag to set
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:13:12
//* REVISION:									
//*********************************************************************
SBYTE		BMEntityAddFlag		(BMENTITY *Entity,UDWORD Flag)
{
	if (Flag & BMENTITY_ONMAP)
	{
		if (BMMapSetObject (Entity->BlockPos.x, Entity->BlockPos.y, NULL) != BMTRUE)
			return (BMTRUE - 1);
	}
	Entity->Flag |= Flag;

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntitySubFlag
//*--------------------------------------------------------------------
//* DESCRIPT: Sub a flag and update the map array if needed...
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : entity, flag to unset
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 10:13:14
//* REVISION:									
//*********************************************************************
SBYTE		BMEntitySubFlag		(BMENTITY *Entity,UDWORD Flag)
{
	if (Flag & BMENTITY_ONMAP)											//if we delete the flag on map, delete the object in the map object
	{
		if (BMMapSetObject (Entity->BlockPos.x, Entity->BlockPos.y, NULL) != BMTRUE)
			return (BMTRUE - 1);
	}

	Entity->Flag &= ~Flag;

	return (BMTRUE);
}


//*********************************************************************
//* FUNCTION: BMEntityDisplay
//*--------------------------------------------------------------------
//* DESCRIPT: display the sprite
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the entity
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    23/01/2001 15:32:32
//* REVISION:									
//*********************************************************************
SBYTE		BMEntityDisplay		(BMENTITY *Entity)
{
	Entity->Sprite->PosX = (UDWORD) Entity->Pos.x;
	Entity->Sprite->PosY = (UDWORD) Entity->Pos.y;
	
	if (BMSpriteDisplay (Entity->Sprite) != BMTRUE)
		return (BMTRUE - 1);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntitySetCallBack 
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
SBYTE				BMEntitySetCallBack (CALLBACKFUNCTION Function)
{
	BMEntityCallBack = Function;	
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityGetBombsPtr 
//*--------------------------------------------------------------------
//* DESCRIPT: Get the ptr of the first bombs in the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr ptr entity
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    29/01/2001 14:46:09
//* REVISION:									
//*********************************************************************
SBYTE				BMEntityGetBombsPtr (BMNODE **PtrBombs)
{
	*PtrBombs = EntityBombsFirst;
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityGetPlayerPtr 
//*--------------------------------------------------------------------
//* DESCRIPT: Get the ptr of the fist player in the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr ptr entity
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    29/01/2001 14:46:46
//* REVISION:									
//*********************************************************************
SBYTE				BMEntityGetPlayerPtr (BMNODE **PtrPlayer)
{
	*PtrPlayer = EntityPlayerFirst;
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMEntityGetBonusPtr 
//*--------------------------------------------------------------------
//* DESCRIPT: Get the ptr of the first bonus in the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ptr ptr entity
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    29/01/2001 14:47:15
//* REVISION:									
//*********************************************************************
SBYTE				BMEntityGetBonusPtr (BMNODE **PtrBonus)
{
	*PtrBonus = EntityBonusFirst;
	return (BMTRUE);
}


/*

typedef struct
{
	SDWORD		Type;					//Type of entity, Bonus, Bombs, Player
	UDWORD		Flag;					//Flag, ONMAP or not
	BMFPOINT	Pos;
	BMFPOINT	BlockPos;
	void		*EntityInfo;			//ptr to the entity type (bonus *...)
	BMSPRITE	*Sprite;				//ptr to the sprite
	void		*ModData;				//specific mod data
}BMENTITY;

*/
