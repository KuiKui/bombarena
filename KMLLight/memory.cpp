#include "kmllight.h"

struct KMLMemoryIndex
{
	int   size;
	void  *adr;
	KMLMemoryIndex *precedent;
	KMLMemoryIndex *suivant;
};

struct KMLMemoryPile
{
	KMLMemoryIndex	Tableau [TAILLE_SEGMENT];
	KMLMemoryPile	*Suivant;
};

	//Le Debug

//__declspec(dllimport) unsigned long MemAlloue;

//unsigned long MemAlloue;
__declspec( dllexport ) unsigned long MemAlloue;


	//La Mémoire
unsigned char *KMLMemoire;
unsigned int  KMLMemoryTailleMemoire;
unsigned long KMLHashingNumber;
unsigned long KMLHashingMask;

	//La pile d'allocation
KMLMemoryPile	**PtrPile; 
short			*IndexPile;//[HASHING_NUMBER];


	//Les Blocs Libres
KMLMemoryPile  *PtrBlocLibre;
short IndexBloc;

	//Les globales qui accélèrent le mouvement de Alloc et Free
KMLMemoryPile	*PtrBlocCourant;
KMLMemoryIndex	*BlocCourantIndex;
KMLMemoryIndex	*Bloc;
short			index;
long			IndexMask;
bool			avant,
				apres;
	
void KMLInitMemory(unsigned int size,unsigned char byte)
//***************************************************************
//	FUNCTION : KMLInitMemory
//---------------------------------------------------------------
//	INPUT	 : size, byte hashing
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  juin 2000
//***************************************************************
{
	if (size==0) exit (-1);
	KMLMemoryTailleMemoire = size;
	KMLMemoire = (unsigned char *) malloc(size);
	
	KMLMemoryPile *pile;

	if (byte == 0 || byte>16) exit(-1);

	KMLHashingNumber   = 1;
	KMLHashingNumber <<= byte;
	KMLHashingMask	   = KMLHashingNumber - 1; 
	
	PtrPile   = (KMLMemoryPile **) malloc(KMLHashingNumber*sizeof(KMLMemoryPile *));
	IndexPile = (short *)	   malloc(KMLHashingNumber*sizeof(short));
	
	for (unsigned int j=0;j<KMLHashingNumber;j++)
	{

		pile		  = (KMLMemoryPile *) malloc (sizeof(KMLMemoryPile));
		pile->Suivant = NULL;

		PtrPile  [j]  = pile;
		IndexPile[j]  = -1;

		for (int i=0;i<TAILLE_SEGMENT;i++)
		{
			PtrPile  [j]->Tableau[i].size	   = 0;
			PtrPile  [j]->Tableau[i].adr	   = NULL;
			PtrPile  [j]->Tableau[i].suivant   = NULL;
			PtrPile  [j]->Tableau[i].precedent = NULL;
		}
	}

	pile		  = (KMLMemoryPile *) malloc (sizeof(KMLMemoryPile));
	PtrBlocLibre  = pile;
	
	IndexBloc = 0;
	PtrBlocLibre->Tableau[IndexBloc].adr	   = &KMLMemoire[0];
	PtrBlocLibre->Tableau[IndexBloc].size	   = size;
	PtrBlocLibre->Tableau[IndexBloc].suivant   = NULL;
	PtrBlocLibre->Tableau[IndexBloc].precedent = NULL;
	PtrBlocLibre->Suivant = NULL;	
	
	for (int i=1;i<TAILLE_SEGMENT;i++)
	{
		PtrBlocLibre->Tableau[i].size		= 0;
		PtrBlocLibre->Tableau[i].adr		= NULL;
		PtrBlocLibre->Tableau[i].suivant	= NULL;
		PtrBlocLibre->Tableau[i].precedent	= NULL;
	}
	MemAlloue = 0;
}

long KMLFreeMemory(void)
//***************************************************************
//	FUNCTION : KMLFreeMemory
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  juin 2000
//***************************************************************
{
	for (unsigned int i=0;i<KMLHashingNumber;i++)
		free(PtrPile[i]);
	
	free (IndexPile);
	free (PtrPile);
	free (PtrBlocLibre);
	free (KMLMemoire);

	return MemAlloue;
	
}

void *KMLAlloc(unsigned int size)
//***************************************************************
//	FUNCTION : KMLInitMemory
//---------------------------------------------------------------
//	INPUT	 : size
//---------------------------------------------------------------
//	OUTPUT	 : void *
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  juin 2000
//***************************************************************
{

 	if (size ==0 || size > KMLMemoryTailleMemoire) return NULL;

	PtrBlocCourant = PtrBlocLibre;	

	index = IndexBloc;
	while (PtrBlocCourant->Tableau[index].size < (int) size)
	{
		index--;
		if (index == -1)
		{
			if (PtrBlocCourant->Suivant != NULL)
			{
				index = TAILLE_SEGMENT - 1;
				PtrBlocCourant = PtrBlocCourant->Suivant;
			}
			else return NULL;//Pas de bloc de taille suffisante trouvé
		}
	}
	//Ecrire dans index des alloués

	BlocCourantIndex = &PtrBlocCourant->Tableau[index];

	IndexMask = ((int) BlocCourantIndex->adr) & KMLHashingMask;
	IndexPile[IndexMask]++;
	if (IndexPile[IndexMask] == TAILLE_SEGMENT)
	{
		KMLMemoryPile *pile;
		pile				 = (KMLMemoryPile *) malloc (sizeof(KMLMemoryPile));
		pile->Suivant		 = PtrPile[IndexMask];
		PtrPile[IndexMask]	 = pile;
		IndexPile[IndexMask] = 0;
	}

	Bloc = &PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]];
	
	Bloc->size		= -((int) size);
	Bloc->adr		= BlocCourantIndex->adr;
	Bloc->precedent	= BlocCourantIndex->precedent;//chainage des elements

	if (Bloc->precedent != NULL)
		Bloc->precedent->suivant = Bloc;
	
	Bloc->suivant = BlocCourantIndex;
	BlocCourantIndex->precedent = Bloc;

	if ((int) size == BlocCourantIndex->size)
	{
		Bloc->suivant	= BlocCourantIndex->suivant;
		
		if (BlocCourantIndex->suivant != NULL)
			BlocCourantIndex->suivant->precedent = Bloc;

		if (BlocCourantIndex != &PtrBlocLibre->Tableau[IndexBloc])
		{
			*BlocCourantIndex = PtrBlocLibre->Tableau[IndexBloc];
			
			if (BlocCourantIndex->precedent != NULL)
				BlocCourantIndex->precedent->suivant = BlocCourantIndex;
		
			if (BlocCourantIndex->suivant != NULL)
				BlocCourantIndex->suivant->precedent = BlocCourantIndex;
		}
		IndexBloc--;
		if (IndexBloc == -1)
		{
			if (PtrBlocLibre->Suivant != NULL)
			{
				KMLMemoryPile *pile;
				pile			= PtrBlocLibre;
				IndexBloc		= TAILLE_SEGMENT - 1;
				PtrBlocLibre	= PtrBlocLibre->Suivant;
				free(pile);
			}
		}
	}
	else
	{
		BlocCourantIndex->size -= size;
		BlocCourantIndex->adr   = (unsigned char *) BlocCourantIndex->adr + size;
	}

	MemAlloue += size;

	return Bloc->adr;
}

//void KMLFree(void *Adresse)
long KMLFree(void *Adresse) //modified on 18/01/2001 11:55:13 to get the size of the free bloc
//***************************************************************
//	FUNCTION : KMLInitMemory
//---------------------------------------------------------------
//	INPUT	 : void *
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  juin 2000
//***************************************************************
{
	long sizeoffree;//modified on 18/01/2001 11:55:41

	if (Adresse == NULL) return (0);//modified on 18/01/2001 11:56:51 (add (0))

	IndexMask = ((int)Adresse) & KMLHashingMask;

	PtrBlocCourant	= PtrPile[IndexMask];
	index			= IndexPile[IndexMask];
	
	//rechercher dans blocs des alloués l'adr correspondante au pointeur Adresse
	while (Adresse != PtrBlocCourant->Tableau[index].adr)
	{
		index--;
		if (index == -1)
		{
			if (PtrBlocCourant->Suivant != NULL)
			{
				PtrBlocCourant = PtrBlocCourant->Suivant;
				index = TAILLE_SEGMENT - 1;
			}
			else return (0);//modified on 18/01/2001 11:57:00 (add (0))
		}
	}

	BlocCourantIndex = &PtrBlocCourant->Tableau[index];

	sizeoffree = -BlocCourantIndex->size;//modified on 18/01/2001 11:56:15
	MemAlloue += BlocCourantIndex->size;
	//retassage des blocs contigüs
	if (BlocCourantIndex->precedent == NULL)
		avant = false;
	else if (BlocCourantIndex->precedent->size < 0)
		avant = false;
	else avant = true;
	if (BlocCourantIndex->suivant == NULL) 
		apres = false;
	else if (BlocCourantIndex->suivant->size < 0)
		apres = false;
	else apres = true;

	if (!avant && !apres)
	{
		//nouveau bloc libre
		IndexBloc++;
		if (IndexBloc == TAILLE_SEGMENT)
		{
			KMLMemoryPile *pile;
			pile		  = (KMLMemoryPile *) malloc (sizeof(KMLMemoryPile));
			pile->Suivant = PtrBlocLibre;
			PtrBlocLibre  = pile;
			IndexBloc     = 0;
		}
		Bloc = &PtrBlocLibre->Tableau[IndexBloc];

		*Bloc = *BlocCourantIndex;
		Bloc->size = -Bloc->size;
		
		if (Bloc->precedent != NULL)
			Bloc->precedent->suivant = Bloc;	
		
		if (Bloc->suivant != NULL)
			Bloc->suivant->precedent = Bloc;	
	}
	else if (avant && !apres)
	{
		BlocCourantIndex->precedent->size -= BlocCourantIndex->size;
		BlocCourantIndex->precedent->suivant = BlocCourantIndex->suivant;
		
		if (BlocCourantIndex->suivant != NULL)
			BlocCourantIndex->suivant->precedent = BlocCourantIndex->precedent;
	}
	else if (!avant && apres)
	{
		BlocCourantIndex->suivant->adr = BlocCourantIndex->adr;
		BlocCourantIndex->suivant->size -= BlocCourantIndex->size;
		BlocCourantIndex->suivant->precedent = BlocCourantIndex->precedent;
		
		if (BlocCourantIndex->precedent != NULL)
			BlocCourantIndex->precedent->suivant = BlocCourantIndex->suivant;
	}
	else if (avant && apres)
	{
		BlocCourantIndex->precedent->size += BlocCourantIndex->suivant->size - BlocCourantIndex->size;
		BlocCourantIndex->precedent->suivant = BlocCourantIndex->suivant->suivant;

		if (BlocCourantIndex->suivant->suivant != NULL)
			BlocCourantIndex->suivant->suivant->precedent = BlocCourantIndex->precedent;

		if (BlocCourantIndex->suivant != &PtrBlocLibre->Tableau[IndexBloc])
		{
			*(BlocCourantIndex->suivant) = PtrBlocLibre->Tableau[IndexBloc];
		
			if (PtrBlocLibre->Tableau[IndexBloc].precedent != NULL)
				PtrBlocLibre->Tableau[IndexBloc].precedent->suivant = BlocCourantIndex->suivant;

			if (PtrBlocLibre->Tableau[IndexBloc].suivant != NULL)
				PtrBlocLibre->Tableau[IndexBloc].suivant->precedent = BlocCourantIndex->suivant;
		}

		IndexBloc--;
		if (IndexBloc == -1)
		{
			if (PtrBlocLibre->Suivant != NULL)
			{
				KMLMemoryPile *pile;
				pile			= PtrBlocLibre;
				IndexBloc		= TAILLE_SEGMENT - 1;
				PtrBlocLibre	= PtrBlocLibre->Suivant;
				free(pile);
			}
		}

	}

	//quand trouvé retasser blocs des alloués
	if (BlocCourantIndex != &PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]])
	{
		*BlocCourantIndex = PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]];

		if (BlocCourantIndex->precedent != NULL)
			BlocCourantIndex->precedent->suivant = BlocCourantIndex;

		if (BlocCourantIndex->suivant != NULL)
			BlocCourantIndex->suivant->precedent = BlocCourantIndex;
	}

	
	PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]].adr = NULL;
	PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]].size = 0;
	PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]].precedent = NULL;
	PtrPile[IndexMask]->Tableau[IndexPile[IndexMask]].suivant = NULL;

	IndexPile[IndexMask]--;

	if (IndexPile[IndexMask] == -1 && PtrPile[IndexMask]->Suivant != NULL)
	{
		KMLMemoryPile *pile;
		pile = PtrPile[IndexMask];
		PtrPile[IndexMask] = PtrPile[IndexMask]->Suivant;
		free(pile);
		IndexPile[IndexMask] = TAILLE_SEGMENT - 1;
	}

	return (sizeoffree);
}	

long KMLGetFreeSize ()
//***************************************************************
//	FUNCTION : KMLGetFreeSize
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : long
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mercredi 5 juillet 2000
//***************************************************************
{
	long FreeSize = 0;

	PtrBlocCourant = PtrBlocLibre;	

	index = IndexBloc;
	while (1)
	{
		FreeSize += PtrBlocCourant->Tableau[index].size;
		index--;
		if (index == -1)
		{
			if (PtrBlocCourant->Suivant != NULL)
			{
				index = TAILLE_SEGMENT - 1;
				PtrBlocCourant = PtrBlocCourant->Suivant;
			}
			else return FreeSize;
		}
	}

}

long KMLGetFreeMax ()
//***************************************************************
//	FUNCTION : KMLGetFreeMax
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : long
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mercredi 5 juillet 2000
//***************************************************************
{
	long MaxSize = 0;

	PtrBlocCourant = PtrBlocLibre;	

	index = IndexBloc;
	while (1)
	{
		if (MaxSize < PtrBlocCourant->Tableau[index].size)
			MaxSize = PtrBlocCourant->Tableau[index].size;
		index--;
		if (index == -1)
		{
			if (PtrBlocCourant->Suivant != NULL)
			{
				index = TAILLE_SEGMENT - 1;
				PtrBlocCourant = PtrBlocCourant->Suivant;
			}
			else return MaxSize;
		}
	}
}

//***************************************************************
//	FUNCTION : KMLAddalloc
//---------------------------------------------------------------
//	INPUT	 : adr + size
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  vendredi 4 août 2000
//***************************************************************

void *KMLAddalloc(void *adress_of_realloc, long size_of_realloc)
{
	if(!adress_of_realloc)
		return NULL;

	IndexMask = ((int)adress_of_realloc) & KMLHashingMask;

	PtrBlocCourant	= PtrPile[IndexMask];
	index			= IndexPile[IndexMask];
	
	//rechercher dans blocs des alloués l'adr correspondante au pointeur Adresse
	while (adress_of_realloc != PtrBlocCourant->Tableau[index].adr)
	{
		index--;
		if (index == -1)
		{
			if (PtrBlocCourant->Suivant != NULL)
			{
				PtrBlocCourant = PtrBlocCourant->Suivant;
				index = TAILLE_SEGMENT - 1;
			}
			else return NULL;
		}
	}

	BlocCourantIndex = &PtrBlocCourant->Tableau[index];//size negative ?

	
	//void *KMLAlloc(unsigned int size)
    return NULL;
}

//***************************************************************
//	FUNCTION : KMLRealloc
//---------------------------------------------------------------
//	INPUT	 : adr + size
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  vendredi 4 août 2000
//***************************************************************
void *KMLRealloc (void *Adresse, unsigned long size)
{
	void *NewAdr;

	NewAdr = KMLAlloc(size);

	if (NewAdr == NULL)
		return NULL;

	memcpy(NewAdr, Adresse, size);
	KMLFree(Adresse);
	
	return NewAdr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void * __GlobaleAlloc;

KMLLISTE	DebugAlloc;
KMLLISTE	DebugFree;

DBGFILEFREE * DebugSearchIntance ( KMLLISTE list, long	line);
DBGFREE * DebugSearchFile ( KMLLISTE list, char *filealloc, long linealloc);

void KMLDebugInit	()
{
	KMLNewList(&DebugAlloc);
	KMLNewList(&DebugFree);
}

//**************************************************************
/** \fn     bool KMLDebugEnd ()
 *
 *  \param  void 
 *
 *  \return true si on doit savoir des trucs false sinon
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 05/03/2001 14:27:13
 ***************************************************************/
DBGUSER KMLDebugEnd	()
{
	static int i = 0;
	DBGALLOC *Courant;
	DBGUSER	debug;
	
	Courant = (DBGALLOC *)DebugAlloc.First;

	if (i == 1)
	{
		free (Courant->Filename);
		KMLRemoveNode (Courant->Node);
		free (Courant);

		Courant = (DBGALLOC *)DebugAlloc.First;
	}

	if ( Courant->Node.Suivant == NULL )
	{
		//virer les infos de FREE
		DBGFREE *CourantFree;
		CourantFree = (DBGFREE *)DebugFree.First;

		while (CourantFree->Node.Suivant != NULL)
		{
			DBGFILEFREE *CourantFreeList;

			free ( CourantFree->FileName );

			CourantFreeList = (DBGFILEFREE *) CourantFree->FileFreeList.First;
			
			while (CourantFreeList->Node.Suivant != NULL)
			{
				free(CourantFreeList->FileName);
				KMLRemoveNode ( CourantFreeList->Node);
				free (CourantFreeList);
				
				CourantFreeList = (DBGFILEFREE *)CourantFree->FileFreeList.First;
			}

			KMLRemoveNode (CourantFree->Node);
			free (CourantFree);
			CourantFree = (DBGFREE *)DebugFree.First;
		}

		debug.FileNameAlloc= NULL;
		debug.FreeInfos = NULL;
		debug.LineNumber = -1;
		return debug;
	}

	debug.FileNameAlloc = Courant->Filename;
	debug.LineNumber = Courant->LineNumber;
	debug.FreeInfos = DebugSearchFile ( DebugFree, Courant->Filename, Courant->LineNumber);

	if ( i == 0)
		i = 1;
	return debug;
}

void* KMLDebugAlloc ( char *FileName, long LineNumber, long SizeOfAlloc, void *Adr )
{
	DBGALLOC *alloc;

	alloc = (DBGALLOC *) malloc ( sizeof ( DBGALLOC) );

	alloc->Adr = Adr;
	alloc->LineNumber = LineNumber;
	alloc->SizeOf = SizeOfAlloc;
	alloc->Filename = (char *) malloc (strlen (FileName) + 1);
	strcpy( alloc->Filename, FileName);

	KMLAddTail (&DebugAlloc, alloc->Node);

	return Adr;
}

void KMLDebugFree  (char *FileName, long LineNumber, void *AdrToFree)
{
	//look for the adr in the entry
	DBGALLOC *Courant;

	Courant = (DBGALLOC *)DebugAlloc.First;

	while (Courant->Node.Suivant != NULL)
	{
		if (Courant->Adr == AdrToFree)
		{
			char	FileAlloc [255];
			long	LineAlloc;
			DBGFREE *pFileFree;
			DBGFILEFREE *pInfos;
			
			strcpy( FileAlloc, Courant->Filename);
			LineAlloc = Courant->LineNumber;
			//create an instance of free
			
			//look for the file in the list
			pFileFree = DebugSearchFile ( DebugFree, FileAlloc, LineAlloc );
			if (pFileFree)
			{
				pInfos = DebugSearchIntance ( pFileFree->FileFreeList, LineNumber);

				if ( pInfos )
				{
					pInfos->NumberOfTime ++;
				}
				else
				{
					pInfos = (DBGFILEFREE *) malloc (sizeof (DBGFILEFREE));

					pInfos->FileName = (char *)malloc ( strlen (FileName) + 1);
					pInfos->NumberOfTime = 1;
					pInfos->LineNumber = LineNumber;
					strcpy (pInfos->FileName, FileName);
					KMLAddTail ( &pFileFree->FileFreeList, pInfos->Node);
				}
			}
			else
			{
				pFileFree = (DBGFREE *) malloc (sizeof (DBGFREE));

				KMLNewList (&pFileFree->FileFreeList);
				pFileFree->FileName = (char *) malloc ( strlen (FileAlloc) + 1);
				strcpy( pFileFree->FileName, FileAlloc);
				pFileFree->LineNumber = LineAlloc;
				KMLAddTail ( &DebugFree, pFileFree->Node);

				pInfos = (DBGFILEFREE *) malloc (sizeof (DBGFILEFREE));

				pInfos->FileName = (char *) malloc ( strlen (FileName) + 1);
				pInfos->NumberOfTime = 1;
				pInfos->LineNumber = LineNumber;
				strcpy (pInfos->FileName, FileName);
				KMLAddTail ( &pFileFree->FileFreeList, pInfos->Node);
			}
			

			//free this entry in alloc table
			
			KMLRemoveNode ( Courant->Node);
			free (Courant->Filename);
			free (Courant);

			return;
		}
		Courant = (DBGALLOC *)Courant->Node.Suivant;
	}

	//erreur not found
}

DBGFREE * DebugSearchFile ( KMLLISTE list, char *filealloc, long linealloc)
{
	DBGFREE *Courant;

	Courant = (DBGFREE *) list.First;

	while (Courant->Node.Suivant != NULL)
	{
		if ( linealloc == Courant->LineNumber )
		{
			if ( strcmp( filealloc, Courant->FileName) == 0)
			{
				return ( Courant );
			}
		}

		Courant = (DBGFREE *) Courant->Node.Suivant;
	}

	return NULL;
}

DBGFILEFREE * DebugSearchIntance ( KMLLISTE list, long line)
{
	DBGFILEFREE *Courant;

	Courant = (DBGFILEFREE *) list.First;

	while (Courant->Node.Suivant != NULL)
	{
		if (Courant->LineNumber == line)
			return (Courant);

		Courant = (DBGFILEFREE *)Courant->Node.Suivant;
	}

	return (NULL);
}