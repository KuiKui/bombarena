#include "BMClient.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"
#include "BMClientServer.h"
#include "BMPlayer.h"
#include "..\BaseMOD\BMod.h"

//--------------------------------
// Structures
//--------------------------------

//--------------------------------
// Globales
//--------------------------------
static	BMCLPLAYERINFO	BMClPlayers[8];

_BMClPlayerInfo			BMClientLocalPlayer [ BMCLIENT_MAX_ON_MACHIN ];

//--------------------------------
// Fonctions
//--------------------------------

//*********************************************************************
//* FUNCTION: BMClInitPlayers
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 15:33:58
//*********************************************************************
UBYTE	BMClInitPlayers()
{
	UBYTE i;
	//BMEVENT Event;


	for (i = 0; i < BMCLIENT_MAX_ON_MACHIN; i ++)
	{
		//lire tout ca dans un fichier de config...
		
		strcpy (BMClientLocalPlayer [i].Name, LocalConfig.pszNamePlayer[i]);
		BMClientLocalPlayer [i].EventKeyMap = BMClAllocEventKeyMap();

		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Left    [i],BM_MDEVENT_PLAYERLEFT);
		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Right   [i],BM_MDEVENT_PLAYERRIGHT);
		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Up      [i],BM_MDEVENT_PLAYERUP);
		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Down    [i],BM_MDEVENT_PLAYERDOWN);

		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Button1 [i],BM_MDEVENT_PLAYERBUTTON1);
		BMClBindKey(BMClientLocalPlayer[i].EventKeyMap,LocalConfig.Button2 [i],BM_MDEVENT_PLAYERBUTTON2);
		
		BMClientLocalPlayer [i].pid = -1; //set to invalid

	}

	// Tout en dur pour le moment
	// On crée un joueur, le pid est 0
	// (jusqu'à ce que la partie management des joueurs coté serveur soit faite),
	// et on fixe les touches (flèches pour bouger, a pour action1, q pour action 2)
/*	strcpy(BMClPlayers[0].Name,"MORB la salope");
	BMClPlayers[0].pid=0;

	if(!(BMClPlayers[0].EventKeyMap=BMClAllocEventKeyMap()))
		return FALSE;

	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_LEFT,BM_MDEVENT_PLAYERLEFT);
	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_RIGHT,BM_MDEVENT_PLAYERRIGHT);
	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_UP,BM_MDEVENT_PLAYERUP);
	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_DOWN,BM_MDEVENT_PLAYERDOWN);

	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_A,BM_MDEVENT_PLAYERBUTTON1);
	BMClBindKey(BMClPlayers[0].EventKeyMap,DIK_Q,BM_MDEVENT_PLAYERBUTTON2);*/

/* REVOIR CA a LA CONNEXION
	Event.action=BM_MDEVENT_NEWPLAYER;
	strcpy(Event.params.plystate.name,BMClPlayers[0].Name);
	Event.pid=0;
	Event.params.plystate.state=0;
	BMSendEvent(&Event);
*/
	return TRUE;
}

//**************************************************************
/** \fn     SBYTE BMClAddPlayer ( UBYTE number )
 *
 *  \param  number # of the player to add
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 02/04/2001 13:33:20
 ***************************************************************/
SBYTE	BMClAddPlayer			( UBYTE number )
{
	BMEVENT Event;

	Event.action=BM_MDEVENT_NEWPLAYER;
	strcpy(Event.params.plystate.name,BMClientLocalPlayer[number].Name);
	Event.pid=number;	
	Event.params.plystate.state=0;
	BMSendEvent(&Event);

	return (BMTRUE);
}
//*********************************************************************
//* FUNCTION: BMClCleanUpPlayers
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 15:34:00
//*********************************************************************
void	BMClCleanUpPlayers()
{
	UBYTE i;

	for (i = 0; i < BMCLIENT_MAX_ON_MACHIN; i ++)
	{
		if(BMClientLocalPlayer[i].EventKeyMap != NULL)
			BMClFreeEventKeyMap(BMClientLocalPlayer[i].EventKeyMap);
	}
}

//*********************************************************************
//* FUNCTION: BMClUpdatePlayersKeys
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 15:34:02
//*********************************************************************
UBYTE	BMClUpdatePlayersKeys()
{
	if(BMClientLocalPlayer[0].EventKeyMap)
		BMClUpdatePlayerKeys(BMClientLocalPlayer[0].EventKeyMap,0);

	if(BMClientLocalPlayer[1].EventKeyMap)
		BMClUpdatePlayerKeys(BMClientLocalPlayer[1].EventKeyMap,1);

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMClUpdate
//*--------------------------------------------------------------------
//* DESCRIPT: read the message in the mailbox of the client and perform them
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    12/01/2001 13:48:15
//* REVISION:									
//*********************************************************************
SBYTE	BMClUpdate	()
{
	BMEVENT	Event;

	while (BMClGetEvent (&Event))
	{
		if (Event.action == BM_CLEVENT_NEWPLAYER)
		{// A new player joined (param: plystate)
			//if (BMPlayerAdd (Event.pid, 255, 255, 255, Event.params.plystate.name) != BMTRUE)
			//	return (BMTRUE - 1);
		}
		else if (Event.action == BM_CLEVENT_DELETEPLAYER)
		{// A player quit (param: plystate)
			if (BMPlayerDelete (Event.pid) != BMTRUE)
				return (BMTRUE - 1);
		}
		else if (Event.action == BM_CLEVENT_PLYMESSAGE)
		{// A player sent a message (param: string)


		}
		else if (Event.action == BM_CLEVENT_PLYCHANGE)
		{// A player changed of state (dead, spectator, etc.) (param: plystate)


		}
		else if (Event.action == BM_CLEVENT_SETANIM)		//Set New Anim
		{
			SDWORD	AnimId;
			UBYTE	MsgAnimId;
			UBYTE	MsgKeyDown;

			MsgAnimId = Event.params.animid;

			if (MsgAnimId & BM_MDEVENT_UP)
				MsgKeyDown = 0;
			else
				MsgKeyDown = 1;

			MsgAnimId &= ~BM_MDEVENT_UP;
			if (MsgAnimId == BM_MDEVENT_PLAYERLEFT)			//Left
			{
				if (MsgKeyDown == 1)
					AnimId = BMOD_PLAYER_LEFT;
				else
					AnimId = BMOD_PLAYER_LEFT_STOP;
			}
			else if (MsgAnimId == BM_MDEVENT_PLAYERRIGHT)	//Right
			{
				if (MsgKeyDown == 1)
					AnimId = BMOD_PLAYER_RIGHT;
				else
					AnimId = BMOD_PLAYER_RIGHT_STOP;
			}
			else if (MsgAnimId == BM_MDEVENT_PLAYERUP)		//Up
			{
				if (MsgKeyDown == 1)
					AnimId = BMOD_PLAYER_UP;
				else
					AnimId = BMOD_PLAYER_UP_STOP;
			}
			else if (MsgAnimId == BM_MDEVENT_PLAYERDOWN)	//Down
			{
				if (MsgKeyDown == 1)
					AnimId = BMOD_PLAYER_DOWN;
				else
					AnimId = BMOD_PLAYER_DOWN_STOP;
			}
			else
			{
				return (BMTRUE - 1);
			}

			//Set the Anim
			if (BMSpriteSetAnim (BMPlayerArray[Event.pid].Entity->Sprite, AnimId) != BMTRUE)
				return (BMTRUE - 1);
		}
	}

	return (BMTRUE);
}

/*
struct _BMevent
{
	UBYTE	action;
	UBYTE	pid;

	union
	{
		STRING string[256];
		struct
		{	
			STRING	name[255];
			UBYTE	state;
		} plystate;
		UBYTE	animid;
	} params;
};

// Player states 
#define PLYSTATE_SPECTATE		1
#define PLYSTATE_DEAD			2
#define PLYSTATE_TEAMA			3
#define PLYSTATE_TEAMB			4


// Mask to check who the event is destinated to 
#define BM_EVENT_DESTMASK		0xc0


// Events for client 
#define BM_CLIENT_EVENT			0x40

// A new player joined (param: plystate)
#define BM_CLEVENT_NEWPLAYER	(BM_CLIENT_EVENT|1)

// A new player joined (param: plystate)
#define BM_CLEVENT_DELETEPLAYER	(BM_CLIENT_EVENT|2)

// A player sent a message (param: string)
#define BM_CLEVENT_PLYMESSAGE	(BM_CLIENT_EVENT|3)

// A player changed of state (dead, spectator, etc.) (param: plystate)
#define BM_CLEVENT_PLYCHANGE	(BM_CLIENT_EVENT|4)

// Play anim
#define BM_CLEVENT_SETANIM		(BM_CLIENT_EVENT|5)
/*****/