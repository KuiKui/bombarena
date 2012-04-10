#ifndef BMSERVER_H
#define BMSERVER_H

typedef struct _BMserverdatas BMSERVERDATAS;

struct _BMserverdatas
{
	BMPLAYER	*PlayerArray;	// Array of the players
	BMENTITY	**BMEntityArray;
};

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD			BMServerMemory;
extern	void			BMSvCleanUpMod ();
#endif