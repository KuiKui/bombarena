#include "BMod.h"

//--------------------------------
// Globales
//--------------------------------



//*********************************************************************
// Name	: BModHaveEntity
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								25/01/2001 14:23:17
//*********************************************************************

SBYTE BModHaveEntity(SDWORD blockx, SDWORD blocky,SDWORD flag)
{
	BMENTITY	*entity;

	entity = BModEntityArray[blocky * BMMAP_WIDTH + blockx];

	if(!entity)
		return FALSE;

	if(entity->Type != flag)
		return FALSE;

	return TRUE;
}

//*********************************************************************
// Name	: BModGetBlockPos
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								09/01/2001 08:18:14
//*********************************************************************

SBYTE BModGetBlockPos(float x, float y, BMLPOINT *point)
{
	// clip
	if( x<0 )
	{
		point->x = -1;
		point->y = (SDWORD)(y / BMMAP_ELEMENTSIZE);
	}

	if( y<0 )
	{
		point->y = -1;
		point->x = (SDWORD)(x / BMMAP_ELEMENTSIZE);
	}

	if(x > BMMAP_WIDTH * BMMAP_ELEMENTSIZE)
	{
		point->x = BMMAP_WIDTH;
		point->y = (SDWORD)(y / BMMAP_ELEMENTSIZE);
	}
		
	if(y > BMMAP_HEIGHT * BMMAP_ELEMENTSIZE)
	{
		point->y = BMMAP_HEIGHT;
		point->x = (SDWORD)(x / BMMAP_ELEMENTSIZE);
	}

	// exit
	if( (x<0) || (y<0) || (x > BMMAP_WIDTH * BMMAP_ELEMENTSIZE) || (y > BMMAP_HEIGHT * BMMAP_ELEMENTSIZE) )
		return FALSE;

	// else
	point->x = (SDWORD)(x / BMMAP_ELEMENTSIZE);
	point->y = (SDWORD)(y / BMMAP_ELEMENTSIZE);

	return TRUE;
}

//*********************************************************************
// Name	: BModSetPlayerBlock
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								08/01/2001 15:51:06
//*********************************************************************

void BModSetPlayerBlock(UBYTE pid, UBYTE flag)
{
	BMPLAYER		*player;

	BMPlayerGetPtr(&player, pid);

	if(flag)
	{
		player->Entity->BlockPos.x = (SDWORD)( (player->Entity->Pos.x + (BMMAP_ELEMENTSIZE>>1)) / BMMAP_ELEMENTSIZE);
		player->Entity->BlockPos.y = (SDWORD)( (player->Entity->Pos.y + (BMMAP_ELEMENTSIZE>>1)) / BMMAP_ELEMENTSIZE);
	}
	else
	{
		player->Entity->BlockPos.x = (SDWORD)(player->Entity->Pos.x / BMMAP_ELEMENTSIZE);
		player->Entity->BlockPos.y = (SDWORD)(player->Entity->Pos.y / BMMAP_ELEMENTSIZE);
	}
}

//*********************************************************************
// Name	: BModUpdateAnim
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/01/2001 14:47:36
//*********************************************************************

void BModUpdateAnim(UBYTE pid, UBYTE Direction)
{
	BMEVENT		event;

	// send anim
	event.pid				= pid;
	event.action			= BM_CLEVENT_SETANIM;
	event.params.animid		= Direction;
	BMMdSendEvent(&event);
}


//*********************************************************************
// Name	: BModPositionIsOK
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								14/01/2001 10:37:16
//*********************************************************************

UBYTE BModPositionIsOK(float posx, float posy)
{
	BMLPOINT		block;
	
	if(!BModGetBlockPos(posx, posy, &block))
		return FALSE;

	if(BModClientDatas->BMMapSol[block.y * BMMAP_WIDTH + block.x].BlocType != BMMAP_NORM)
		return FALSE;

	if(BModHaveEntity(block.x, block.y, BMENTITY_BOMBS))
		return FALSE;

	return TRUE;
}

//*********************************************************************
// Name	: BModUpdateInterPos
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								14/01/2001 12:52:59
//*********************************************************************

void BModUpdateInterPos(UBYTE pid, UBYTE Direction)
{
	BMFPOINT	pos;
	BMLPOINT	nextblock;
	BMPLAYER	*player = NULL;
	float		currentspeed;
	float		currentdelta;


	// get player
	BMPlayerGetPtr(&player, pid);
	pos.x		= player->Entity->Pos.x;
	pos.y		= player->Entity->Pos.y;

	currentspeed	= ((BMODPLAYERDATAS *)(player->ModData))->WalkSpeed;
	currentdelta	= currentspeed * BModDTime;		// BUG
	currentdelta	= 1.f;							// BUG


	switch(Direction) 
	{
	case BM_MDEVENT_PLAYERUP:
		BModGetBlockPos(pos.x+(BMMAP_ELEMENTSIZE>>1), pos.y+(BMMAP_ELEMENTSIZE>>1), &nextblock);

		if( BModPositionIsOK((float)(nextblock.x*BMMAP_ELEMENTSIZE), (float)((nextblock.y-1)*BMMAP_ELEMENTSIZE)) )
		{
			if( (SDWORD)pos.x+(BMMAP_ELEMENTSIZE>>1) < (nextblock.x*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.x += currentdelta;
			else if( (SDWORD)pos.x+(BMMAP_ELEMENTSIZE>>1) > (nextblock.x*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.x -= currentdelta;

			player->Entity->Pos.x = pos.x;
		}

		return;

	case BM_MDEVENT_PLAYERDOWN:
		BModGetBlockPos( pos.x+(BMMAP_ELEMENTSIZE>>1), pos.y+(BMMAP_ELEMENTSIZE>>1) + BMMAP_ELEMENTSIZE, &nextblock );

		if( BModPositionIsOK((float)(nextblock.x*BMMAP_ELEMENTSIZE), (float)(nextblock.y*BMMAP_ELEMENTSIZE)) )
		{
			if( (SDWORD)pos.x+(BMMAP_ELEMENTSIZE>>1) < (nextblock.x*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.x += currentdelta;
			else if( (SDWORD)pos.x+(BMMAP_ELEMENTSIZE>>1) > (nextblock.x*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.x -= currentdelta;

			player->Entity->Pos.x = pos.x;
		}

		return;

	case BM_MDEVENT_PLAYERRIGHT:
		BModGetBlockPos(pos.x + BMMAP_ELEMENTSIZE +(BMMAP_ELEMENTSIZE>>1), pos.y+(BMMAP_ELEMENTSIZE>>1), &nextblock);
 
		if( BModPositionIsOK((float)(nextblock.x*BMMAP_ELEMENTSIZE), (float)(nextblock.y*BMMAP_ELEMENTSIZE)) )
		{
			if( (SDWORD)pos.y+(BMMAP_ELEMENTSIZE>>1) < (nextblock.y*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.y += currentdelta;
			else if( (SDWORD)pos.y+(BMMAP_ELEMENTSIZE>>1) > (nextblock.y*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.y -= currentdelta;

			player->Entity->Pos.y = pos.y;
		}

		return;

	case BM_MDEVENT_PLAYERLEFT:
		BModGetBlockPos(pos.x+(BMMAP_ELEMENTSIZE>>1), pos.y+(BMMAP_ELEMENTSIZE>>1), &nextblock );

		if( BModPositionIsOK((float)((nextblock.x-1)*BMMAP_ELEMENTSIZE), (float)(nextblock.y*BMMAP_ELEMENTSIZE)) )
		{
			if( (SDWORD)pos.y+(BMMAP_ELEMENTSIZE>>1) < (nextblock.y*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.y += currentdelta;
			else if( (SDWORD)pos.y+(BMMAP_ELEMENTSIZE>>1) > (nextblock.y*BMMAP_ELEMENTSIZE)+(BMMAP_ELEMENTSIZE>>1) )
				pos.y -= currentdelta;

			player->Entity->Pos.y = pos.y;
		}

		return;
	}
	
}

//*********************************************************************
// Name	: BModUpdatePos
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								08/01/2001 15:37:49
//*********************************************************************

void BModUpdatePos(UBYTE pid, UBYTE Direction)
{
	BMFPOINT	nextpos;
	BMLPOINT	nextblock;
	BMPLAYER	*player;
	float		currentspeed;
	BMFPOINT	currentpos;
	BMLPOINT	currentblock;

	UBYTE		isongoodplace;

	// get player
	BMPlayerGetPtr(&player, pid);
	currentspeed	= ((BMODPLAYERDATAS *)(player->ModData))->WalkSpeed;
	currentpos		= player->Entity->Pos;
	nextpos			= currentpos;
	currentblock	= player->Entity->BlockPos;

	isongoodplace = BModPositionIsOK(currentpos.x + (BMMAP_ELEMENTSIZE>>1) , currentpos.y + (BMMAP_ELEMENTSIZE>>1));

	// update pos
	switch(Direction)
	{
	case BM_MDEVENT_PLAYERUP:
		nextpos.y -= currentspeed * BModDTime;
		BModGetBlockPos(nextpos.x, nextpos.y, &nextblock);

		if( (BModPositionIsOK(nextpos.x, nextpos.y)) && (BModPositionIsOK((nextpos.x + (BMMAP_ELEMENTSIZE-1)), nextpos.y)) )
		{
			player->Entity->Pos.y = nextpos.y;
		}
		else
		{
			if(isongoodplace)
				player->Entity->Pos.y = (float)( (nextblock.y+1) * BMMAP_ELEMENTSIZE);
			else
			{
				 if(nextblock.y < currentblock.y)
					player->Entity->Pos.y = (float)( (nextblock.y+1) * BMMAP_ELEMENTSIZE);
				 else
					player->Entity->Pos.y = nextpos.y;
			}
		}
		return;
		break;

	case BM_MDEVENT_PLAYERDOWN:
		nextpos.y += currentspeed * BModDTime;
		BModGetBlockPos(nextpos.x, nextpos.y + BMMAP_ELEMENTSIZE, &nextblock );

		if( (BModPositionIsOK(nextpos.x, nextpos.y + BMMAP_ELEMENTSIZE)) && (BModPositionIsOK((nextpos.x + (BMMAP_ELEMENTSIZE-1)), (nextpos.y + BMMAP_ELEMENTSIZE))) )
		{
			player->Entity->Pos.y = nextpos.y;
		}
		else
		{
			if(isongoodplace)
				player->Entity->Pos.y = (float)( (nextblock.y-1) * BMMAP_ELEMENTSIZE);
			else
			{
				 if(nextblock.y > currentblock.y)
					player->Entity->Pos.y = (float)( (nextblock.y-1) * BMMAP_ELEMENTSIZE);
				 else
					player->Entity->Pos.y = nextpos.y;
			}
		}

		return;
		break;

	case BM_MDEVENT_PLAYERRIGHT:
		nextpos.x += currentspeed * BModDTime;
		BModGetBlockPos(nextpos.x  + BMMAP_ELEMENTSIZE, nextpos.y, &nextblock);

		if( (BModPositionIsOK(nextpos.x + BMMAP_ELEMENTSIZE, nextpos.y)) && (BModPositionIsOK((nextpos.x + BMMAP_ELEMENTSIZE), nextpos.y + (BMMAP_ELEMENTSIZE-1))) )
		{
			player->Entity->Pos.x = nextpos.x;
		}
		else
		{
			if(isongoodplace)
				player->Entity->Pos.x = (float)( (nextblock.x-1) * BMMAP_ELEMENTSIZE);
			else
			{
				 if(nextblock.x > currentblock.x)
					player->Entity->Pos.x = (float)( (nextblock.x-1) * BMMAP_ELEMENTSIZE);
				 else
					player->Entity->Pos.x = nextpos.x;
			}
		}
		return;
		break;

	case BM_MDEVENT_PLAYERLEFT:
		nextpos.x -= currentspeed * BModDTime;
		BModGetBlockPos(nextpos.x, nextpos.y, &nextblock );

		if( (BModPositionIsOK(nextpos.x, nextpos.y)) && (BModPositionIsOK(nextpos.x , nextpos.y + (BMMAP_ELEMENTSIZE-1))) )
		{
			player->Entity->Pos.x = nextpos.x;
		}
		else
		{
			if(isongoodplace)
				player->Entity->Pos.x = (float)( (nextblock.x+1) * BMMAP_ELEMENTSIZE);
			else
			{
				 if(nextblock.x < currentblock.x)
					player->Entity->Pos.x = (float)( (nextblock.x+1) * BMMAP_ELEMENTSIZE);
				 else
					player->Entity->Pos.x = nextpos.x;
			}

		}
		return;
		break;

	default:
		return;
	}
}


//*********************************************************************
// Name	: BModUpdatePlayer
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								09/01/2001 09:32:17
//*********************************************************************

void BModUpdatePlayer(UBYTE pid)
{
	BMLPOINT	blockpos;
	BMPLAYER	*player;
	BMBONUS		*bonus;
	BMENTITY	*entity;


	BMPlayerGetPtr(&player, pid);

	blockpos.x = player->Entity->BlockPos.x;
	blockpos.y = player->Entity->BlockPos.y;


	// First, we check if there is fire
	if(BModClientDatas->BMMapSol[blockpos.y * BMMAP_WIDTH + blockpos.x].BlocType == BMOD_BLOCTYPE_FIRE)
	{
		//BModKillPlayer(pid);
		return;
	}

	entity = BModEntityArray[blockpos.y * BMMAP_WIDTH + blockpos.x];

	if(!BModHaveEntity(blockpos.x, blockpos.y, BMENTITY_BONUS))
		return;

	bonus = (BMBONUS *)(entity->EntityInfo);

	// We look if there's an object lying here
	switch(bonus->Type)
	{
		case BMOD_BONUS_FIRE:
			((BMODPLAYERDATAS *)player->ModData)->Fire++;
			BMEntitySubFlag(entity, BMENTITY_ONMAP);
			break;

		default:
			break;
	}

}

//*********************************************************************
// Name	: BModUpdateSimpleAction1
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								09/01/2001 09:26:03
//*********************************************************************

void BModUpdateSimpleAction1(UBYTE pid)
{
	BMLPOINT	blockpos;
	BMPLAYER	*player;
	BMBOMBS		*bomb;

	BMPlayerGetPtr(&player, pid);
	blockpos.x	= player->Entity->BlockPos.x;
	blockpos.y	= player->Entity->BlockPos.y;

	if( !BModPositionIsOK( player->Entity->Pos.x + (BMMAP_ELEMENTSIZE>>1), player->Entity->Pos.y + (BMMAP_ELEMENTSIZE>>1) ) )
		return;

	if (BMBombsAdd(&bomb, BMBOMBS_NORMAL, 3000, "bomb.spr") != BMTRUE)
		return;
	if (BMSpriteSetAnim(bomb->Entity->Sprite, 1) != BMTRUE)
		return;

	BMEntityAddFlag(bomb->Entity, BMENTITY_ONMAP);

	bomb->Entity->Pos.x = (float)blockpos.x * BMMAP_ELEMENTSIZE;
	bomb->Entity->Pos.y = (float)blockpos.y * BMMAP_ELEMENTSIZE;
	if (BMEntitySetBlockPosition (bomb->Entity, (float)(blockpos.x * BMMAP_ELEMENTSIZE), (float)(blockpos.y *BMMAP_ELEMENTSIZE)) != BMTRUE)
		return;
	if (BMEntitySetRealPosition (bomb->Entity, (float)(blockpos.x * BMMAP_ELEMENTSIZE), (float)(blockpos.y *BMMAP_ELEMENTSIZE)) != BMTRUE)
			return;

}


//**************************************************************
/** \fn     void BModAddFire( SDWORD posx, SDWORD posy )
 *
 *  \param  posx 
 *  \param  posy 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 06/03/2001 17:15:32
 ***************************************************************/
SDWORD BModAddFire( SDWORD posx, SDWORD posy, SDWORD flag )
{
	BMBOMBS		*bomb;
	SDWORD		numanim = 0;


	BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType = BMOD_BLOCTYPE_FIRE;


	if ( flag & BMOD_FIRE_IS_END )
	{
		if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_UP )
		{
			numanim = 2;
		}
		else if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_DOWN )
		{
			numanim = 3;
		}
		else if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_LEFT )
		{
			numanim = 4;
		}
		else if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_RIGHT )
		{
			numanim = 5;
		}
	}
	else
	{
		if ( ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_UP )
		||   ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_DOWN ) )
		{
			numanim = 7;
		}
		else if ( ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_RIGHT )
		||      ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_LEFT ) )
		{
			numanim = 8;
		}
		else if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_CENTER )
		{
			numanim = 6;
		}
		else if ( ( flag & BMOD_FIRE_MASK ) == BMOD_FIRE_BOOM )
		{
			numanim = 1;
		}
	}

	if ( ! numanim )
		return FALSE;


	if (BMBombsAdd(&bomb, BMBOMBS_FLAMES, 750, "fire.spr") != BMTRUE)
		return FALSE;
	if (BMSpriteSetAnim(bomb->Entity->Sprite, numanim) != BMTRUE)
		return FALSE;

	BMEntityAddFlag(bomb->Entity, BMENTITY_ONMAP);

	bomb->Entity->Pos.x = (float)posx * BMMAP_ELEMENTSIZE;
	bomb->Entity->Pos.y = (float)posy * BMMAP_ELEMENTSIZE;
	if (BMEntitySetBlockPosition (bomb->Entity, (float)(posx * BMMAP_ELEMENTSIZE), (float)(posy *BMMAP_ELEMENTSIZE)) != BMTRUE)
		return FALSE;
	if (BMEntitySetRealPosition (bomb->Entity, (float)(posx * BMMAP_ELEMENTSIZE), (float)(posy *BMMAP_ELEMENTSIZE)) != BMTRUE)
		return FALSE;

	return TRUE;
}

//*********************************************************************
// Name	: BModRecurseFireBullet 
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: MrK								09/01/2001 09:26:03
//*********************************************************************

void BModRecurseBomb( UDWORD posx, UDWORD posy, BMENTITY *entity);	// TRUC de PUTE pour le recurse !!!!

void BModRecurseFireBullet( SDWORD posx, SDWORD posy, SDWORD firesize, SDWORD sens)
{
	BMENTITY	*entity;


	// sens ?
	switch( sens )
	{
	case BMOD_UP:
		posy--;
		break;

	case BMOD_DOWN:
		posy++;
		break;

	case BMOD_RIGHT:
		posx++;
		break;

	case BMOD_LEFT:
		posx--;
		break;
	}

	// trop petit
	if ( ( posx < 0 ) || ( posy < 0 ) )
		return;

	// trop grand
	if ( ( posx > BMMAP_WIDTH ) || ( posy > BMMAP_HEIGHT ) )
		return;

	// testfiersize
	if ( firesize == 0 )
		return;
	else
		firesize--;


	// traitement
	entity = BModEntityArray[posy * BMMAP_WIDTH + posx];
	
	if ( entity )	// bloc ou objet detruit
	{
		switch ( entity->Type )
		{
		case BMENTITY_BONUS:
			entity->Flag |= BMENTITY_ISDEAD;
			return;
			break;

		case BMENTITY_BOMBS:
			if ( ((BMBOMBS *)entity->EntityInfo)->Type == BMBOMBS_FLAMES )
			{
				return;
			}
			
			if( entity->Flag & BMENTITY_ISDEAD )
			{
				return;
			}

			BModRecurseBomb ( posx, posy, entity );
			return;
			break;
		}
	}
	else // case du field
	{
		if ( BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType == BMOD_BLOCTYPE_FIRE )
		{
			return;
		}

		if ( BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType == BMMAP_DEST )
		{
			BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType = BMOD_BLOCTYPE_FIRE;
			BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocInfo = BMOD_FIRE_BOOM;
			return;
		}

		if ( BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType == BMMAP_UNDEST )
		{
			return;
		}

		if ( BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType == BMMAP_NORM )
		{
			BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType = BMOD_BLOCTYPE_FIRE;
			BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocInfo = (UBYTE)sens;
			if ( !firesize )
			{
				BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocInfo |= BMOD_FIRE_IS_END;
			}
		}
	}
	
	// recurse
	BModRecurseFireBullet ( posx, posy, firesize, sens);
}

//*********************************************************************
// Name	: BModRecurseBomb 
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: MrK								09/01/2001 09:26:03
//*********************************************************************

void BModRecurseBomb( UDWORD posx, UDWORD posy , BMENTITY *entity)
{
	SDWORD			firesize;


	// trop petit
	if ( ( posx < 0 ) || ( posy < 0 ) )
		return;

	// trop grand
	if ( ( posx > BMMAP_WIDTH ) || ( posy > BMMAP_HEIGHT ) )
		return;

	// init
	firesize		= 2;
	entity->Flag   |= BMENTITY_ISDEAD;

	BModRecurseFireBullet( posx, posy, firesize, BMOD_UP );
	BModRecurseFireBullet( posx, posy, firesize, BMOD_RIGHT);
	BModRecurseFireBullet( posx, posy, firesize, BMOD_DOWN);
	BModRecurseFireBullet( posx, posy, firesize, BMOD_LEFT);

	// enflamme la case de la bombe...
	BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocType = BMOD_BLOCTYPE_FIRE;
	// ...au centre
	BModClientDatas->BMMapSol[posy * BMMAP_WIDTH + posx].BlocInfo = BMOD_FIRE_CENTER;

}

//*********************************************************************
// Name	: BModDeleteGhost
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: MrK								09/01/2001 09:26:03
//*********************************************************************

void BModDeleteGhost ()
{
	SDWORD			i, j;
	BMENTITY		*entity;

	for ( j = 0; j < BMMAP_HEIGHT; j++ )
		for ( i = 0; i < BMMAP_WIDTH; i++ )
		{
			entity = BModEntityArray[ j * BMMAP_WIDTH + i ];

			if ( entity )
			{
				if ( entity->Flag & BMENTITY_ISDEAD )
				{
					switch ( entity->Type )
					{
					case  BMENTITY_BOMBS:
						BMBombsDelete( (BMBOMBS *) entity->EntityInfo );
						break;

					case  BMENTITY_BONUS:
						BMBonusDelete( (BMBONUS *) entity->EntityInfo );
						break;
					}
					// BMEntityDelete ( entity );
				}
			}

			// Test Flamming !!!!
			if ( BModClientDatas->BMMapSol[ j * BMMAP_WIDTH + i ].BlocType == BMOD_BLOCTYPE_FIRE )
				BModAddFire( i, j, (SDWORD)(BModClientDatas->BMMapSol[j * BMMAP_WIDTH + i].BlocInfo) );

		}
}

//*********************************************************************
// Name	: BModUpdateEntity 
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: MikE								09/01/2001 09:26:03
//*********************************************************************

void BModUpdateEntity ()
{
	BMNODE *Current;

	BMEntityGetBombsPtr (&Current);

	if (Current == NULL)
		return;

	while (Current->Next)
	{
		BMENTITY *Entity;

		Entity = (BMENTITY *)Current->Info;

		if (Entity->Type == BMENTITY_BOMBS)
		{
			if (BMTimerGetTime () > ((BMBOMBS *)Entity->EntityInfo)->TimeExplode)
			{
				if ( ((BMBOMBS *)Entity->EntityInfo)->Type == BMBOMBS_FLAMES)
				{
					BModClientDatas->BMMapSol[Entity->BlockPos.y* BMMAP_WIDTH + Entity->BlockPos.x].BlocType = BMMAP_NORM;
					BMBombsDelete ( (BMBOMBS *) Entity->EntityInfo);		//we clear the flames
					

					BMEntityGetBombsPtr (&Current);
					if (Current == NULL)
						return;
				}
				else
				{
					BModRecurseBomb ( Entity->BlockPos.x, Entity->BlockPos.y, Entity);
					BMBombsDelete ( (BMBOMBS *) Entity->EntityInfo);
					BMEntityGetBombsPtr (&Current);
					if (Current == NULL)
						return;
				}
			}
			else
				Current = Current->Next;
		}
		else
			return;

//		BMEntityGetBombsPtr (&Current);
//		if (Current == NULL)
//			return;
	}
}