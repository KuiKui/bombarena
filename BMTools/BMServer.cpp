#include "BMClientServer.h"
#include "BMServer.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"

SDWORD			BMServerMemory = 0; //Allocate memory

/* Datas du serveur */
static BMSERVERDATAS BMServerDatas;

/* Le mod */
static HMODULE BMModDll=NULL;
static MODUPDATEFUNCPTR BMModUpdateFunc=NULL;
static MODINITFUNCPTR BMModInitFunc=NULL;
static MODCLEANUPFUNCPTR BMModCleanUpFunc=NULL;

/* Client list */
static KMLLISTE BMClientList;

extern KMLLISTE BMSvEventList;
extern KMLLISTE BMClEventList;
extern KMLLISTE BMMdEventList;
extern BMCLIENTDATAS BMLocalClientDatas;


//*********************************************************************
//* FUNCTION: BMSvLoadMod
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:28
//*********************************************************************
int BMSvLoadMod(char *name)
{
	BMModInitFunc=NULL;
	BMModCleanUpFunc=NULL;
	BMModUpdateFunc=NULL;

	if(!(BMModDll=LoadLibrary(name)))
		return FALSE;

	if(!(BMModUpdateFunc=(MODUPDATEFUNCPTR)GetProcAddress(BMModDll,"BMModUpdate")))
	{
		FreeLibrary(BMModDll);
		return FALSE;
	}

	if(!(BMModInitFunc=(MODINITFUNCPTR)GetProcAddress(BMModDll,"BMModInit")))
	{
		FreeLibrary(BMModDll);
		return FALSE;
	}

	if(!(BMModCleanUpFunc=(MODCLEANUPFUNCPTR)GetProcAddress(BMModDll,"BMModCleanUp")))
	{
		FreeLibrary(BMModDll);
		return FALSE;
	}

	if(!BMModInitFunc(&BMServerDatas))
	{
		FreeLibrary(BMModDll);
		return FALSE;
	}

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMSvUnLoadMod
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:33
//*********************************************************************
void BMSvUnLoadMod()
{
	if(BMModCleanUpFunc)
		BMModCleanUpFunc();

	if(BMModDll)
		FreeLibrary(BMModDll);

	BMModDll=NULL;
	BMModUpdateFunc=NULL;
}

//*********************************************************************
//* FUNCTION: BMSvInit
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:39
//*********************************************************************
int BMSvInit()
{
	BMCLIENTINFO *localclient;
	KMLNewList(&BMClientList);
	KMLNewList(&BMSvEventList);
	KMLNewList(&BMClEventList);
	KMLNewList(&BMMdEventList);

	/* On remplit la structure qui va bien (mais faut bien penser le truc) */
	BMServerDatas.PlayerArray	=	BMPlayerArray;
	BMServerDatas.BMEntityArray	=	BMMapEntityArray;

	BMLocalClientDatas.BMMapSol		= BMMapSol;
	//BMLocalClientDatas.BMMapObejct	=

	/* En dur pour le moment */
	if(!(BMSvLoadMod("BaseMod.dll")))
		return FALSE;

	/* On déclare le client local */
	localclient=(BMCLIENTINFO *) ALLOC2 (sizeof (BMCLIENTINFO),BMServerMemory);
	if(!localclient )
	{
		BMSvCleanUp();
		return FALSE;
	}

	KMLAddTail(&BMClientList,localclient->node);

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMSvCleanUp
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 11:38:39
//*********************************************************************
void BMSvCleanUp()
{
	BMCLIENTINFO *ccl,*ncl;

	/* On vire les clients */
	ccl=(BMCLIENTINFO *)BMClientList.First;

	while(ncl=(BMCLIENTINFO *)ccl->node.Suivant)
	{
		BMSvDisconnectClient(ccl);
		ccl=ncl;
	}

	BMSvUnLoadMod();

	/* Clear remaining events in case some people haven't finished what is in there plate
	   (some people don't eat the eyes) */
	BMPurgeAllEvents();
}

//*********************************************************************
//* FUNCTION: BMSvUpdate
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:48
//*********************************************************************
int BMSvUpdate(float dTime)
{
	BMCLIENTINFO *ccl,*ncl;

	/* On appelle l'update du mod */
	if(!BMModUpdateFunc)
		return FALSE;

	if(!BMModUpdateFunc(dTime,&BMLocalClientDatas))
		return FALSE;

	/* On update les clients */
	ccl=(BMCLIENTINFO *)BMClientList.First;

	while(ncl=(BMCLIENTINFO *)ccl->node.Suivant)
	{
		if(!BMSvUpdateClient(ccl,&BMLocalClientDatas))
			return FALSE;

		ccl=ncl;
	}

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMSvDisconnectClient
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 14:50:18
//*********************************************************************
void BMSvDisconnectClient(BMCLIENTINFO *client)
{
	if(!client) return;

	/* Plus tard: ajouter plein de trucs */

	KMLRemoveNode(client->node);
	FREE2(client,BMServerMemory);
}

//*********************************************************************
//* FUNCTION: BMSvUpdateClient
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:54
//*********************************************************************
int BMSvUpdateClient(BMCLIENTINFO *client,BMCLIENTDATAS *datas)
{
	switch(client->type)
	{
		case BMTYPE_LOCAL:
	//		BMLocalClientDatas=datas;
			break;

		case BMTYPE_NET:
			break;
	}

	return TRUE;
}

//**************************************************************
/** \fn     void BMSvCleanUpMod ()
 *
 *  \param  void 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  call the clean mod function if it exists
 *
 *  \author MikE                       \date 12/04/2001 14:38:48
 ***************************************************************/
void BMSvCleanUpMod ()
{
	if ( BMModCleanUpFunc )
		BMModCleanUpFunc ();
}