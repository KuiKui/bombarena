/*
 * Couche naze pour générer des messages up/down à partir de la gestion de touches moisie de directx (rectx)
 */

#include "BMWin32.h"
#include "BMGlobalInclude.h"
#include "BMWin32Keys.h"

SDWORD			BMWin32KeysMemory = 0;	//Allocate memory


//*********************************************************************
//* FUNCTION: BMClUpdatePlayerKeys
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 14:37:53
//*********************************************************************
UBYTE	BMClUpdatePlayerKeys(void *EventKeyMap,int pid)
{
	int i,keystate;
	BMWIN32EVENTKEYMAP	*KeyMap=(BMWIN32EVENTKEYMAP *)EventKeyMap;
	BMEVENT evt;

	evt.pid=pid;

	/* Iterate through the EventKeyMap and check the corresponding keys */
	for(i=0;i<(BM_MAX_KEY_EVENT-1);i++)
	{
		/* Check the current state of the key */
		keystate=KMLControlKeysPress(KeyMap->KeyCodes[i],1);

		if(keystate!=KeyMap->KeyState[i])
		{
			/* Key state changed */
			KeyMap->KeyState[i]=keystate;

			evt.action=BM_MOD_EVENT|(keystate ? 0:BM_MDEVENT_UP)|(i+1);
			if(!BMSendEvent(&evt))
				return FALSE;
		}
	}

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMClAllocEventKeyMap
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 14:37:54
//*********************************************************************
void *BMClAllocEventKeyMap()
{
	void *keymap;

	keymap=ALLOC2(sizeof(BMWIN32EVENTKEYMAP),BMWin32KeysMemory);

	if(!keymap)
		return NULL;

	memset(keymap,0,sizeof(BMWIN32EVENTKEYMAP));
	return keymap;
}

//*********************************************************************
//* FUNCTION: BMClFreeEventKeyMap
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 14:37:56
//*********************************************************************
void BMClFreeEventKeyMap(void *EventKeyMap)
{
	FREE2(EventKeyMap, BMWin32KeysMemory);
}

//*********************************************************************
//* FUNCTION: BMClBindKey
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    11/01/2001 14:41:10
//*********************************************************************
UBYTE BMClBindKey(void *EventKeyMap,int keycode,UBYTE Action)
{
	BMWIN32EVENTKEYMAP	*KeyMap=(BMWIN32EVENTKEYMAP *)EventKeyMap;
	UBYTE eventcode;

	if((Action & BM_EVENT_DESTMASK)!=BM_MOD_EVENT)
		return FALSE;

	eventcode=Action & ~BM_EVENT_DESTMASK;
	eventcode&=~BM_MDEVENT_UP;

	if(eventcode>BM_MAX_KEY_EVENT)
		return FALSE;

	if(eventcode<1)
		return FALSE;

	KeyMap->KeyCodes[eventcode-1]=keycode;
	KeyMap->KeyState[eventcode-1]=0;

	return TRUE;
}