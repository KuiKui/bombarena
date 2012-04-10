#ifndef __BMCLIENT_H__
#define __BMCLIENT_H__

#include	"BMGlobalInclude.h"

//--------------------------------
// Structures
//--------------------------------
typedef	struct _BMClPlayerInfo	BMCLPLAYERINFO;

/* Client-side player structure:
   used by the client to know the players it hosts,
   and the corresponding key assignations */
struct _BMClPlayerInfo
{
	char	Name[64];

	// Player id given by the server
	int		pid;

	// The Event Key mapping structure is platform specific 
	// and should be allocated with BMClAllocEventKeyMap, 
	// and filled with the proper functions
	void	*EventKeyMap;
};

//--------------------------------
// Globals
//--------------------------------

extern	_BMClPlayerInfo		BMClientLocalPlayer [ BMCLIENT_MAX_ON_MACHIN ];

//--------------------------------
// Functions
//--------------------------------
UBYTE	BMClUpdatePlayersKeys	();
void	BMClCleanUpPlayers		();
UBYTE	BMClInitPlayers			();
SBYTE	BMClUpdate				();

SBYTE	BMClAddPlayer			( UBYTE number );

//REVOIR
// au moment ou on fait join game , on envoie un message au serveur, et lui renvoie un message create player, tout en definissant aussi les autres player de jeu..;
//revoir faire fonction bmclcreateplayer 
#endif