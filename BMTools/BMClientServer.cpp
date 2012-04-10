/*
 * Bien beau source
 */

#include "BMClientServer.h"
#include "BMServer.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"

SDWORD			BMClientServerMemory = 0; //Allocate memory

/* Events for server */
KMLLISTE BMSvEventList;

/* Events for mod */
KMLLISTE BMMdEventList;

/* Events for client */
KMLLISTE BMClEventList;

BMCLIENTDATAS BMLocalClientDatas;

static BMSERVERINFO BMServer;

//*********************************************************************
//* FUNCTION: BMSvGetEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:57
//*********************************************************************
int BMSvGetEvent(BMEVENT *event)
{
	BMEVENTQUEUE *evq;

	evq=(BMEVENTQUEUE *)BMSvEventList.First;

	if(!evq->node.Suivant) return FALSE;

	memcpy(event,&evq->event,sizeof(BMEVENT));
	KMLRemoveNode(evq->node);
	FREE2(evq,BMClientServerMemory);

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMMdGetEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:57
//*********************************************************************
int BMMdGetEvent(BMEVENT *event)
{
	BMEVENTQUEUE *evq;

	evq=(BMEVENTQUEUE *)BMMdEventList.First;

	if(!evq->node.Suivant) return FALSE;

	memcpy(event,&evq->event,sizeof(BMEVENT));
	KMLRemoveNode(evq->node);
	FREE2(evq,BMClientServerMemory);

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMClGetEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:41:57
//*********************************************************************
int BMClGetEvent(BMEVENT *event)
{
	BMEVENTQUEUE *evq;

	evq=(BMEVENTQUEUE *)BMClEventList.First;

	if(!evq->node.Suivant) return FALSE;

	memcpy(event,&evq->event,sizeof(BMEVENT));
	KMLRemoveNode(evq->node);
	FREE2(evq,BMClientServerMemory);

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMSvSendEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    12/01/2001 15:05:27
//*********************************************************************
int BMSvSendEvent(BMEVENT *event)
{
	event->sender=EVTID_SERVER;
	return BMSendEvent(event);
}

//*********************************************************************
//* FUNCTION: BMMdSendEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    12/01/2001 15:05:27
//*********************************************************************
int BMMdSendEvent(BMEVENT *event)
{
	event->sender=EVTID_MOD;
	return BMSendEvent(event);
}

//*********************************************************************
//* FUNCTION: BMClSendEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    12/01/2001 15:05:27
//*********************************************************************
int BMClSendEvent(BMEVENT *event)
{
	event->sender=0;	// TODO: replace this with the proper client id
	return BMSendEvent(event);
}

//*********************************************************************
//* FUNCTION: BMSendEvent
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:42:17
//*********************************************************************
int BMSendEvent(BMEVENT *event)
{
	BMEVENTQUEUE *evq;
	evq=(BMEVENTQUEUE *)ALLOC2(sizeof(BMEVENTQUEUE),BMClientServerMemory);
	if(!evq)
		return FALSE;

	memcpy(&evq->event,event,sizeof(BMEVENT));

	/* Dispatch the event to the right list */
	switch(evq->event.action & BM_EVENT_DESTMASK)
	{
		case BM_CLIENT_EVENT:
			// A client can't send an event directly to another client
			if(evq->event.sender>0)
			{
				FREE2(evq,BMClientServerMemory);
				return FALSE;
			}

			KMLAddTail(&BMClEventList,evq->node);
			break;

		case BM_SERVER_EVENT:
			evq->event.recipient=EVTID_SERVER;
			KMLAddTail(&BMSvEventList,evq->node);
			break;

		case BM_MOD_EVENT:
			evq->event.recipient=EVTID_MOD;
			KMLAddTail(&BMMdEventList,evq->node);
			break;

		default:
			FREE2(evq,BMClientServerMemory);
			return FALSE;
	}

	return TRUE;
}

//*********************************************************************
//* FUNCTION: BMPurgeAllEvents
//*--------------------------------------------------------------------
//* DESCRIPT: For pigs out there (namely, MikE) who don't empty their
//* event queue
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    17/01/2001 16:05:26
//*********************************************************************
void BMPurgeAllEvents()
{
	BMEVENTQUEUE *cev,*nev;

	cev=(BMEVENTQUEUE *)BMMdEventList.First;

	while(nev=(BMEVENTQUEUE *)cev->node.Suivant)
	{
		KMLRemoveNode(cev->node);
		FREE2(cev,BMClientServerMemory);
		cev=nev;
	}

	cev=(BMEVENTQUEUE *)BMSvEventList.First;

	while(nev=(BMEVENTQUEUE *)cev->node.Suivant)
	{
		KMLRemoveNode(cev->node);
		FREE2(cev,BMClientServerMemory);
		cev=nev;
	}


	cev=(BMEVENTQUEUE *)BMClEventList.First;

	while(nev=(BMEVENTQUEUE *)cev->node.Suivant)
	{
		KMLRemoveNode(cev->node);
		FREE2(cev,BMClientServerMemory);
		cev=nev;
	}
}

//*********************************************************************
//* FUNCTION: BMClGetDatas
//*--------------------------------------------------------------------
//* DESCRIPT: 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : 
//*--------------------------------------------------------------------
//* OUT     : 
//*--------------------------------------------------------------------
//* AUTHOR  : MORB								    04/01/2001 10:42:13
//*********************************************************************
BMCLIENTDATAS *BMClGetDatas()
{
	switch(BMServer.type)
	{
		case BMTYPE_LOCAL:
			return &BMLocalClientDatas;

		case BMTYPE_NET:
			return NULL;
	}

	return NULL;
}