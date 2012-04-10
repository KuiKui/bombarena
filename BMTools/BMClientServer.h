/*
 * Header pas vide
 */

#ifndef __BMCLIENTSERVER_H
#define __BMCLIENTSERVER_H

#include "BMGlobalInclude.h"
#include "BMMap.h"
#include "BMServer.h"

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD			BMClientServerMemory;

/* Client and Server should have the same version in order to communicate properly */
#define BMCLIENTSERVER_VERSION 0

/* Aligné, pour faire plaisir aux névrosés atteints de compulsions obsessionelles
   qui passent leur temps à ranger leur bureau et à tailler leur crayons */

/* Note d'une vieille salope qui bordelise tout ce qu'il touche et qui est pas foutu de rester assis sans tomber*/
/* KuiKui la putasse */

typedef struct _BMclientinfo	BMCLIENTINFO;
typedef struct _BMserverinfo	BMSERVERINFO;
typedef struct _BMplayerinfo	BMPLAYERINFO;
typedef struct _BMclientdatas	BMCLIENTDATAS;
typedef struct _BMevent			BMEVENT;
typedef struct _BMeventqueue	BMEVENTQUEUE;

/* Client info, used by the server to know which clients are connected and where they are */
struct _BMclientinfo
{
	KMLNODE	node;

	UBYTE	type;
	STRING	*name;
};

/* Server info, used by the client to know the name of the server and where it is */
struct _BMserverinfo
{
	UBYTE	type;
	STRING	*name;
};

#define BMTYPE_LOCAL 0
#define BMTYPE_NET 1

/* Info sur un joueur, utilisé par le serveur pour savoir quels joueurs sont présents et sur quel client ils se trouvent */
/*
struct _BMplayerinfo
{
	KMLNODE			node;

	BMCLIENTINFO	*client;
	UBYTE			pid;	// Player id
	STRING			*name;
};*/


/* Infos sent from server (mod) to the client.
   Contains the state of all the stuff to display in the game */

/* Pour toute question concernant cette structure, demander à MIKE */
struct _BMclientdatas
{
	BMMAPSOL	*BMMapSol;								//array of the map
	BMLIST		*BMMapObejct;							//list to the object of the map (bombe, bonus, player ...)
};

/* Event structure */
struct _BMevent
{
	UBYTE	action;
	UBYTE	pid;

	SBYTE	sender;		// Who is the sender
	SBYTE	recipient;	// Who is the recipient
						// (no need to fill when sending events to server and mod)

	union
	{
		STRING string[256];
		struct
		{	
			STRING	name[255];
			UBYTE	state;
		} plystate;
		UBYTE		animid;
	} params;
};

/* Sender/Recipient ids */

/* If positive, it's a client id (0-7) */
#define	EVTID_BROADCASTCLIENT	-1	// All clients
#define EVTID_SERVER			-2
#define EVTID_MOD				-3


/* Player states */
#define PLYSTATE_SPECTATE		1
#define PLYSTATE_DEAD			2
#define PLYSTATE_TEAMA			3
#define PLYSTATE_TEAMB			4


/* Mask to check who the event is destinated to */
#define BM_EVENT_DESTMASK		0xc0


/* Events for client */
#define BM_CLIENT_EVENT			0x40

// A new player joined (param: plystate)
#define BM_CLEVENT_NEWPLAYER	(BM_CLIENT_EVENT|1)

// A player left (param: plystate)
#define BM_CLEVENT_DELETEPLAYER	(BM_CLIENT_EVENT|2)

// A player sent a message (param: string)
#define BM_CLEVENT_PLYMESSAGE	(BM_CLIENT_EVENT|3)

// A player changed of state (dead, spectator, etc.) (param: plystate)
#define BM_CLEVENT_PLYCHANGE	(BM_CLIENT_EVENT|4)

// Play anim
#define BM_CLEVENT_SETANIM		(BM_CLIENT_EVENT|5)
/*****/


/* Events for server */
#define BM_SERVER_EVENT			0x80

// A player wants to join (param: plystate)
#define BM_SVEVENT_NEWPLAYER	(BM_SERVER_EVENT|1)

// A player wants to join (param: plystate)
#define BM_SVEVENT_DELETEPLAYER	(BM_SERVER_EVENT|2)

// A player wants to send a message (param: string)
#define BM_SVEVENT_PLYMESSAGE	(BM_SERVER_EVENT|3)

// A player wants to change of state (spectate, join a team, etc.) (param: plystate)
#define BM_SVEVENT_PLYCHANGE	(BM_SERVER_EVENT|4)
/*****/


/* Events for mod */
#define BM_MOD_EVENT			0xc0

// Key events (no params)

// All key events are for key down. For key up, they must be ORed with the following:
#define BM_MDEVENT_UP				0x20

#define BM_MDEVENT_PLAYERLEFT		(BM_MOD_EVENT|1)
#define BM_MDEVENT_PLAYERRIGHT		(BM_MOD_EVENT|2)
#define BM_MDEVENT_PLAYERUP			(BM_MOD_EVENT|3)
#define BM_MDEVENT_PLAYERDOWN		(BM_MOD_EVENT|4)
#define BM_MDEVENT_PLAYERBUTTON1	(BM_MOD_EVENT|5)
#define BM_MDEVENT_PLAYERBUTTON2	(BM_MOD_EVENT|6)
#define BM_MDEVENT_SWITCHSTATE		(BM_MOD_EVENT|7)

#define BM_MAX_KEY_EVENT			BM_MDEVENT_PLAYERBUTTON2

#define BM_MDEVENT_NEWPLAYER		(BM_MOD_EVENT|10)
#define BM_MDEVENT_DELETEPLAYER		(BM_MOD_EVENT|11)

/*****/

/* Structure for queuing events in lists */
struct _BMeventqueue
{
	KMLNODE node;
	BMEVENT	event;
};

/* Functions */

/* Function pointer types for Mod functions */
typedef int		(*MODUPDATEFUNCPTR)(float dTime,BMCLIENTDATAS *ClientDatas);
typedef int		(*MODINITFUNCPTR)(BMSERVERDATAS *ServerDatas);
typedef void	(*MODCLEANUPFUNCPTR)();

/* Server */
EXPORT_DLL	int				BMSvLoadMod(char *name);
EXPORT_DLL	void			BMSvUnLoadMod();
EXPORT_DLL	int				BMSvInit();
EXPORT_DLL	void			BMSvCleanUp();
EXPORT_DLL	int				BMSvUpdate(float dTime);
EXPORT_DLL	void			BMSvDisconnectClient(BMCLIENTINFO *client);
EXPORT_DLL	int				BMSvUpdateClient(BMCLIENTINFO *client,BMCLIENTDATAS *datas);

/* Client */
EXPORT_DLL	BMCLIENTDATAS *	BMClGetDatas();

/* Events */
int							BMSendEvent(BMEVENT *event);

/* Event function to be used by the server */
EXPORT_DLL	int				BMSvSendEvent(BMEVENT *event);
EXPORT_DLL	int				BMSvGetEvent(BMEVENT *event);

/* Event functions to be used by the mod */
EXPORT_DLL	int				BMMdSendEvent(BMEVENT *event);
EXPORT_DLL	int				BMMdGetEvent(BMEVENT *event);

/* Event functions to be used by the clients */
EXPORT_DLL	int				BMClSendEvent(BMEVENT *event);
EXPORT_DLL	int				BMClGetEvent(BMEVENT *event);

void BMPurgeAllEvents();

#endif