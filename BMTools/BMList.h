#ifndef __BMLIST_H__
#define	__BMLIST_H__

#include	"BMGlobalInclude.h"

//--------------------------------
// Structures
//--------------------------------

typedef struct _node_struct
{
	struct _node_struct *Next;		
	struct _node_struct *Previous;
	void	*Info;
}BMNODE;

typedef struct 
{
	BMNODE	*First;		
	BMNODE	*Null;
	BMNODE	*Last;
}BMLIST;

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMListMemory;

//--------------------------------
// Fonctions
//--------------------------------
extern BMLIST *		BMListNew		();
extern void			BMListDelete	(BMLIST *list);

extern void			BMListAddTail	(BMLIST *list, void *Info);
extern void			BMListAddHead	(BMLIST *list, void *Info);
extern void			BMInsertNode	(BMNODE *node, void *Info);
extern void			BMDeleteNode	(BMNODE *node);

#endif