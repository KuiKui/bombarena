#include "BMod.h"

//--------------------------------
// Globales
//--------------------------------

BMCLIENTDATAS	*BModClientDatas;
BMPLAYER		*BModPlayerArray;
BMENTITY		**BModEntityArray;
float			BModDTime;
UBYTE			BModEventBuffer	[BMPLAYERMAXNUMBER];
UBYTE			BModOldEvent	[BMPLAYERMAXNUMBER];
SBYTE			BModCurrentState;

//--------------------------------
// Fonctions
//--------------------------------
SDWORD BMModUpdateGlobalEvent	( BMEVENT Event );
SDWORD BMModUpdateOutgame		( void );
SDWORD BMModUpdateIngame		( void );

//**************************************************************
/** \fn     extern "C" _declspec(dllexport) int BMModUpdate(float dTime,BMCLIENTDATAS *ClientDatas)
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:11:08
 ***************************************************************/

extern "C"  _declspec(dllexport) int BMModUpdate(float dTime,BMCLIENTDATAS *ClientDatas)
{
	BModClientDatas = ClientDatas;
	BModDTime		= dTime;


	// switch state
	switch ( BModCurrentState )
	{
	case BMOD_CURRENT_STATE_INGAME:
		BMModUpdateIngame();
		break;

	case BMOD_CURRENT_STATE_OUTGAME:
		BMModUpdateOutgame();
		break;

	default:
		return FALSE;
	}

	return TRUE;
}


//**************************************************************
/** \fn     void SwitchCurrentState ( void )
 *
 *  \param  void 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 05/04/2001 10:19:39
 ***************************************************************/

void SwitchCurrentState ( void )
{
	if ( BModCurrentState == BMOD_CURRENT_STATE_INGAME )
	{
		BModCurrentState = BMOD_CURRENT_STATE_OUTGAME;
	}
	else
	{
		BModResetPlayer();
		BMBombsResetAll();
		BMBonusResetAll();
		BModCurrentState = BMOD_CURRENT_STATE_INGAME;
	}
}

//**************************************************************
/** \fn     void BModUpdateGlobalEvent ( void )
 *
 *  \param  void 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:17:26
 ***************************************************************/

SDWORD BMModUpdateGlobalEvent ( BMEVENT	 Event )
{

	switch(Event.action)
	{

	case BM_MDEVENT_NEWPLAYER://-------------------------- new player
		BMModInitPlayer(Event.pid);
		break;

	case BM_MDEVENT_DELETEPLAYER://----------------------- delete player
		BModEventBuffer[Event.pid]	= NULL;
		BModOldEvent[Event.pid]		= NULL;
		break;

	case BM_MDEVENT_SWITCHSTATE:
		SwitchCurrentState();
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//**************************************************************
/** \fn     int BMModUpdateOutgame ( void )
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:12:14
 ***************************************************************/

SDWORD BMModUpdateOutgame ( void )
{
	BMEVENT		event;


	// Gestion Messages
	while(BMMdGetEvent(&event))
	{

		switch(event.action)
		{
		case BM_MDEVENT_PLAYERBUTTON1:
			event.action = BM_MDEVENT_SWITCHSTATE;
			BMMdSendEvent ( &event );
			break;

		default:
			return BMModUpdateGlobalEvent ( event );
		}
	}

	return TRUE;
}

//**************************************************************
/** \fn     int BMModUpdateIngame ( void )
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:11:13
 ***************************************************************/

SDWORD BMModUpdateIngame ( void )
{
	BMEVENT		event;
	SBYTE		index;
	BMPLAYER	*player;

	memcpy(BModOldEvent,BModEventBuffer, BMPLAYERMAXNUMBER);

	// Gestion Messages
	while(BMMdGetEvent(&event))
	{
		if ( ! BMPlayerIsReady ( event.pid ) )
			continue;

		switch(event.action)
		{
		case BM_MDEVENT_PLAYERLEFT://------------------------- move player
		case BM_MDEVENT_PLAYERRIGHT:
		case BM_MDEVENT_PLAYERUP:
		case BM_MDEVENT_PLAYERDOWN:
			BModEventBuffer[event.pid]	= event.action;
			break;

		case (BM_MDEVENT_PLAYERLEFT		| BM_MDEVENT_UP):
		case (BM_MDEVENT_PLAYERRIGHT	| BM_MDEVENT_UP):
		case (BM_MDEVENT_PLAYERUP		| BM_MDEVENT_UP):
		case (BM_MDEVENT_PLAYERDOWN		| BM_MDEVENT_UP):
			if( BModEventBuffer[event.pid] == (event.action&(~BM_MDEVENT_UP)))
				BModEventBuffer[event.pid]	= event.action;
			break;

		case BM_MDEVENT_PLAYERBUTTON1://---------------------- action 1
			BModUpdateSimpleAction1(event.pid);
			break;

		default:
			return BMModUpdateGlobalEvent ( event );
		}
	}

	// Update Player Pos
	for(index = 0; index < BMPLAYERMAXNUMBER; index++)
	{
		if ( ! BMPlayerIsReady ( index ) )
			continue;

		if(BModEventBuffer[index])
		{
			if( BModEventBuffer[index] != BModOldEvent[index] )
			{
				// changement d'etat : on balance l'anim
				BModUpdateAnim(index, BModEventBuffer[index]);
			}
			
			if( ! (BModEventBuffer[index]&BM_MDEVENT_UP) )
			{
				// deplacement : on update la position
				BModUpdatePos		(index, BModEventBuffer[index]);
				//BModSetPlayerBlock	(index, TRUE);
				BMPlayerGetPtr(&player, index);
				BMEntitySetBlockPosition(player->Entity, player->Entity->Pos.x, player->Entity->Pos.y, TRUE);

				BModUpdatePlayer	(index);
			}

			BModUpdateInterPos(index, BModEventBuffer[index]);
		}
	}

	//Update Entity Bombs
	BModUpdateEntity	();
	BModDeleteGhost		();

	return TRUE;
}

//**************************************************************
/** \fn     extern "C" _declspec(dllexport) int BMModInit(BMSERVERDATAS *BMServerDatas)
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:11:20
 ***************************************************************/

extern "C"  _declspec(dllexport) int BMModInit(BMSERVERDATAS *BMServerDatas)
{
	BMPlayerSetCallBack		(BMModDeletePlayer);
	BMEntitySetCallBack		(NULL);

	BModCurrentState	= BMOD_CURRENT_STATE_OUTGAME;
	BModClientDatas		= NULL;
	BModPlayerArray		= BMServerDatas->PlayerArray;
	BModEntityArray		= BMServerDatas->BMEntityArray;
	memset(BModEventBuffer, 0, BMPLAYERMAXNUMBER);
	memset(BModOldEvent, 0, BMPLAYERMAXNUMBER);


	return TRUE;
}

//**************************************************************
/** \fn     extern "C" _declspec(dllexport) void BMModCleanUp()
 *
 *  \param  void 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 04/04/2001 14:11:33
 ***************************************************************/

extern "C"  _declspec(dllexport) void BMModCleanUp()
{
	BModClientDatas = NULL;
	BModPlayerArray = NULL;
	BModCurrentState	= BMOD_CURRENT_STATE_OUTGAME;
	memset(BModEventBuffer, 0, BMPLAYERMAXNUMBER);
	memset(BModOldEvent, 0, BMPLAYERMAXNUMBER);
}
