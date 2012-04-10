#include "BMList.h"
#include "BMGlobalInclude.h"
#include "BMWin32.h"

SDWORD		BMListMemory = 0;	//Allocate memory

//*********************************************************************
//* FUNCTION: BMListNew
//*--------------------------------------------------------------------
//* DESCRIPT: create a new list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return pointer to the header of the list, or NULL
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:32:07
//* REVISION:									
//*********************************************************************
BMLIST *		BMListNew		()
{
	BMLIST *newlist;

	newlist = (BMLIST *) ALLOC2 (sizeof(BMLIST),BMListMemory);
	if (newlist == NULL)
		return NULL;

	newlist->First		= (BMNODE *) &(newlist->Null);
	newlist->Null		= NULL;
	newlist->Last		= (BMNODE *) newlist;

	return (newlist);
}

//*********************************************************************
//* FUNCTION: BMListDelete
//*--------------------------------------------------------------------
//* DESCRIPT: delete a list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : pointer to the header of the list
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:32:43
//* REVISION:									
//*********************************************************************
void			BMListDelete	(BMLIST *list)
{
	if (list->First->Next != NULL)
		KMLWarning ("Liste non totalement libérée");

	FREE2(list,BMListMemory);
}

//*********************************************************************
//* FUNCTION: BMListAddTail
//*--------------------------------------------------------------------
//* DESCRIPT: add info at the end of the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : the pointer to the list, the info to chain
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:33:11
//* REVISION:									
//*********************************************************************
void			BMListAddTail	(BMLIST *list, void *Info)
{
	BMNODE *node;

	node = (BMNODE *) ALLOC2 (sizeof(BMNODE),BMListMemory);
	node->Info = Info;

	node->Next		= list->Last->Next;
	node->Previous	= list->Last;
	list->Last->Next= node;
	list->Last		= node;
}

//*********************************************************************
//* FUNCTION: BMListAddHead
//*--------------------------------------------------------------------
//* DESCRIPT: add info at the head of the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : pointer to the list, info to chain
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:33:54
//* REVISION:									
//*********************************************************************
void			BMListAddHead	(BMLIST *list, void *Info)
{
	BMNODE *node;

	node = (BMNODE *) ALLOC2 (sizeof(BMNODE),BMListMemory);
	node->Info = Info;

	node->Previous			= list->First->Previous;
	node->Next				= list->First;
	list->First->Previous	= node;
	list->First				= node;
}

//*********************************************************************
//* FUNCTION: BMInsertNode
//*--------------------------------------------------------------------
//* DESCRIPT: insert a new node with "info" after the node
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : info and node (to insert after)
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:34:21
//* REVISION:									
//*********************************************************************
void			BMInsertNode	(BMNODE *node, void *Info)
{
	BMNODE *newnode;

	newnode = (BMNODE *) ALLOC2 (sizeof(BMNODE),BMListMemory);
	newnode->Info = Info;

	newnode->Previous		= node;
	newnode->Next			= node->Next;
	node->Next				= newnode;
	newnode->Next->Previous = newnode;
}

//*********************************************************************
//* FUNCTION: BMDeleteNode
//*--------------------------------------------------------------------
//* DESCRIPT: delete a node from the list
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : node
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    20/12/2000 15:35:14
//* REVISION:									
//*********************************************************************
void			BMDeleteNode	(BMNODE *node)
{
	node->Previous->Next = node->Next;
	node->Next->Previous = node->Previous;

	FREE2 (node,BMListMemory);
}