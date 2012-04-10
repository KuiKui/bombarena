/*
 * Couche naze pour générer des messages up/down à partir de la gestion de touches moisie de directx (rectx)
 * KuiKui la pute
 * Mike la salope
 * DirectX la bouse
 *
 *
 * Ca va un peu mieux maintenant...
 */

#ifndef __BMWIN32KEYS__
#define __BMWIN32KEYS__

#include "BMGlobalInclude.h"
#include "BMClientServer.h"

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMWin32KeysMemory;

typedef struct _BMWin32EventKeyMap	BMWIN32EVENTKEYMAP;

struct _BMWin32EventKeyMap
{
	// The keycode corresponding to each key event
	int	KeyCodes[BM_MAX_KEY_EVENT-1];

	// The previous state of each key
	int	KeyState[BM_MAX_KEY_EVENT-1];
};

UBYTE	BMClUpdatePlayerKeys(void *EventKeyMap,int pid);
void	*BMClAllocEventKeyMap();
void	BMClFreeEventKeyMap(void *EventKeyMap);
UBYTE	BMClBindKey(void *EventKeyMap,int keycode,UBYTE Action);

#endif