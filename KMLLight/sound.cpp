#include "kmllight.h"

//*************************************************************** GLOBALES
LPDIRECTSOUND				lpDS;				//Objet Principal
LPDIRECTSOUNDBUFFER			lpBufferPrimaire;	//Buffer Primaire

unsigned short				KMLVolumeGeneral;	//VolumeGeneral pour tous les sons compris entre 0 & 1024

KMLLISTE					KMLSoundBDDListe;	//Liste des Sons de la base de données

#define NBRE_SECONDE_BUFFER_STREAM	3			//Nbre de seconde pour le buffer de streaming
#define	TAILLE_MINI_TO_READ			2048		//Taille minimal à lire dans le fichier pour lors du streaming
#define TAILLE_BLANC_BUFFER			1024		//A REVOIR POUR LE STREAMING QUI LOOP PAS

unsigned short TabLog [101]   = {    0,  14,  29,  43,  58,  74,  89, 104, 120, 136,			// Table Logarithmique pour que le volume ou le pan soit linéaire 
								   152, 168, 184, 200, 217, 234, 251, 268, 286, 304,			// 101 valeur, car volume entre 0 et 100 %
								   321, 340, 358, 377, 395, 415, 434, 454, 473, 494,
								   514, 535, 556, 577, 599, 621, 643, 666, 689, 713,
								   736, 761, 785, 810, 836, 862, 888, 915, 943, 971,
								  1000,1029,1058,1089,1120,1152,1184,1217,1251,1286,
								  1321,1358,1395,1434,1473,1514,1556,1599,1643,1689,
								  1736,1785,1836,1888,1943,2000,2058,2120,2184,2251,
								  2321,2395,2473,2556,2643,2736,2836,2943,3058,3184,
								  3321,3473,3643,3836,4058,4321,4643,5058,5643,6643, 10000};

//*************************************************************** FONCTION INTERNES
KMLSOUNDBDD *	KMLBDDAddSound			(char *Path, long flag, KMLSOUND *Sound);
KMLSOUNDBDD *	KMLBDDSearch			(char *Path, long flag);

char			KMLSoundReadWaveHeader	(FILE **f, char *Path, WAVEFORMATEX *waveformat, unsigned long *sizeofdata);
char			KMLSoundReadWaveData	(FILE **f, char **car, unsigned long size);
char			KMLSoundRestore			(KMLSOUND *PtrSound);
long			KMLGetVolume			(long Volume);
char			KMLSoundGestionStream	(KMLSOUND *PtrSound);
void			KMLSoundLireStream		(KMLSOUND *PtrSound, void **Ptr, unsigned long Size);


char KMLSoundStartLib (HINSTANCE &hInst, unsigned long SpeakerConfig, unsigned long SpeakerSubConfig)
//***************************************************************
//	FUNCTION : KMLSoundStartLib
//---------------------------------------------------------------
//	INPUT	 : HINSTANCE, Speaker Config...
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  vendredi 21 juillet 2000
//  REVISION : MikE  ->  mercredi 9 aout 2000
//***************************************************************
{
	HRESULT			result;
	DSBUFFERDESC	desc;
	
	result = DirectSoundCreate ( NULL, &lpDS, NULL);																//Creation de l'objet Direct Sound

	if (result == DS_OK)
	{
		result = lpDS->SetCooperativeLevel(hMainWnd,DSSCL_NORMAL);													//Mode coopérative normal permet d'entendre les sons des autres applications comme WinAm par exemple
		if (result == DD_OK)
		{
			ZeroMemory(&desc, sizeof(DSBUFFERDESC));		//reset les octets de desc
			desc.dwSize			= sizeof(desc);				//a ne pas oublier, donner la taille de la variable
			desc.dwFlags		= DSBCAPS_PRIMARYBUFFER;	//on va creer le buffer primaire dans lequel les sons sont mixés par la carte
			desc.dwBufferBytes	= 0;						//on met la taille du buffer à 0, dsound le creera avec une taille optimale
			desc.lpwfxFormat	= NULL;						//le mode DSSCL_NORMAL ne permet pas de changer le format du son, donc on passe NULL

			result = lpDS->CreateSoundBuffer (&desc, &lpBufferPrimaire, NULL);										//Creation du buffer primaire
			if (result == DD_OK)
			{
				KMLNewList(&KMLSoundBDDListe);				//creation de la Base de Donnees pour les sons
				KMLVolumeGeneral = 1024;					//on met le volume general au max
				
				if (SpeakerConfig != NULL)					//si l'utilisateur veut reconfigurer la config des hauts parleurs
				{
					if (SpeakerSubConfig != NULL)			//si il utilise le mode stereo, alors on peut donner leur geometrie
					{
						if (lpDS->SetSpeakerConfig(DSSPEAKER_COMBINED(SpeakerConfig,SpeakerSubConfig)) != DS_OK)	//Met a jour la configurtion des hauts parleurs
							return KML_SOUND_ERROR_SPEAKER;
					}
					else if (lpDS->SetSpeakerConfig(SpeakerConfig) != DS_OK)										//Met a jour la configurtion des hauts parleurs
						return KML_SOUND_ERROR_SPEAKER;
				}
				return KML_OK;
			}
		}
		return KML_SOUND_ERROR;
	}
	else																											//Traitement des erreurs
	{
		switch (result)
		{
			case (DSERR_NODRIVER) :																					//Pas de driver trouvé
							return KML_SOUND_NODRIVER;
			default :
							return KML_SOUND_ERROR;																	//Toutes les autres erreurs possibles
		}
	}
}

void KMLSoundQuitLib (void)
//***************************************************************
//	FUNCTION : KMLSoundQuitLib
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> vendredi 28 juillet 2000
//	REVISION : 
//***************************************************************
{
	if (lpDS != NULL)																								//Si l'objet lpDS n'est pas NULL, alors il n'a pas été libéré
	{
		KMLSOUNDBDD	*actu,*suiv;
		actu = (KMLSOUNDBDD	*)KMLSoundBDDListe.First;
		
		if(actu == NULL)
			return;

		while(suiv = (KMLSOUNDBDD *)actu->Node.Suivant)																//Parcours de la liste de la Base de Données
		{
			KMLSOUND *courant,*suivant;

			courant = actu->PtrKMLSound;
			while (courant != NULL)																					//Parcours les instances pour le son de la Basse de Données
			{
				if (courant->Flag & KML_SSTREAMING)			//si le son est streame, alors il existe une seule instance, et le champ suivant ne represente pas les instances suivantes
					suivant = NULL;							//donc on force le suivant a NULL
				else
					suivant = courant->Suivant;				//sinon le suivant est le suivant du courant
				if (KMLSoundDelete(courant) != KML_OK)		//on delete l'instance de ce son
					KMLError("Erreur dans la liberation");	
				courant = suivant;							//on passe à l'instance suivante
			}

			actu = suiv;									//on passe au prochain dans la Base de Données
		}


		lpBufferPrimaire->Release();						//on libère le buffer primaire
		lpBufferPrimaire = NULL;							
		
		lpDS->Release();									//on libère l'objet Direct Sound
		lpDS = NULL;
	}
}


KMLSOUND *KMLSoundCreate (char *Path, long flag)
//***************************************************************	
//	FUNCTION : KMLSoundCreate
//---------------------------------------------------------------
//	INPUT	 : path, flag
//---------------------------------------------------------------
//	OUTPUT	 : le son
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> jeudi 27 juillet 2000
//	REVISION : 
//***************************************************************
{
	FILE					**f;
	KMLSOUND				*NewSound;
	KMLSOUNDBDD				*PtrBDD;

	DSBUFFERDESC			dsdd;
	LPDIRECTSOUNDBUFFER		NewBuffer;	
	WAVEFORMATEX			waveformat;

	void					*car = NULL;
	unsigned long			size, size2;
	long					flagBDD;

	if (Path == NULL)											//Traitement des erreurs
		return NULL;

	if (!(flag & KML_SSTREAMING))								//On ne veut pas créer le son en Streaming donc on recherche si il existe déjà dans la Base de Données
	{
		PtrBDD = KMLBDDSearch(Path, flag);						//Recherche dans la Base, et retourne l'objet de la base ou NULL
		if (PtrBDD != NULL)										//objet trouvé
		{
			NewSound = (KMLSOUND *) ALLOC(sizeof(KMLSOUND));	//On crée une nouvelle instance de son
			if (NewSound == NULL)								//Gestion des problèmes d'allocation
				KMLError("Erreur d'allocation dans KMLSoundCreate");
																//Mise à jour des champs de l'instances
			NewSound->BDD					= PtrBDD;			//l'objet père est PtrBDD
			NewSound->Suivant				= NULL;				//pas de suivant puisque cette instance et mis en fin de listes des instances
			/*if (PtrBDD->PtrKMLSound == NULL)					inutile... puisqu'il y a forcement une instance		
				PtrBDD->PtrKMLSound			= NewSound;*/
			PtrBDD->PtrLastSound->Suivant	= NewSound;			//le dernier de la liste des instances a maintenant la nouvelle instance pour suivant
			PtrBDD->PtrLastSound			= NewSound;			//le dernier de la liste est maintenant la nouvelle instance

			if (lpDS->DuplicateSoundBuffer(PtrBDD->PtrKMLSound->SoundBuffer, &NewSound->SoundBuffer) != DS_OK)	//Duplication du buffer de son existant (puisqu'il y a deja au moins une instance du son)
			{																									//Direct Sound, permet a plusieurs instance de son d'utiliser le meme buffer de data (c'est pour ca que pour les sons streamer 
				FREE(NewSound);								//si erreur alors on desalloue l'objet			//on cree a chaque fois une nouvelle car on ne peut pas partager en streaming)
				return NULL;									//retourne valeur d'erreur
			}
																		//Creation du buffer réussi
			NewSound->BDD->User++;										//On ajoute un utilisateur pour ce son à l'objet de la Base de Données
			NewSound->SoundBuffer->GetFrequency	(&NewSound->Frequence);	//On met à jour les champs de l'instance
			KMLSoundSetVolume (NewSound, 100);							//Volume Max		dans Dsound
			NewSound->SoundBuffer->SetPan (0);							//Balance au milieu dans Dsound
			NewSound->Volume = KML_SVOLMAX;								//Volume Max pour l'instance (le volume de l'instance et du son peuvent différer puisqu'il existe un volume general, si ce dernier est à 0, et que le volume de l'instance est au max, le volume sera au final de 0)
			NewSound->Flag	 = flag & (KML_SFREQUENCE | KML_SVOLUME | KML_SPAN | KML_SALWAYSHEAR | KML_SCUTONREPLAY | KML_SLOOP);//on recupère les flags passé a la creation et relatif a l'instance
			NewSound->Flag  |= KML_SSTOP;								//Le son ne se joue pas

			return NewSound;											//on renvoie l'instance
		}
	}

																		//Le son n'existait pas dans la Base de Données
	f = (FILE **) ALLOC(sizeof(FILE *));								//On alloue un pointeur de fichier (qui sera conservé si le fichier est streamé)
	if (f == NULL)														//Gestion des problemes d'allocation
		KMLError("Erreur allocation de FILE dans KMLSoundCreate");
	
	KMLSoundReadWaveHeader(f, Path, &waveformat, &size);				//Lecture du Header du fichier Path, la fonction rempli waveformat, size (la taille des datas), le pointeur de fichier f est update puisque le fichier est maintenant ouvert en lecture

	ZeroMemory(&dsdd, sizeof(DSBUFFERDESC));	//reset des champs de la variable
	dsdd.dwSize	= sizeof(DSBUFFERDESC);			//mise à jour de la taille de la variable
//	dsdd.dwFlags = DSBCAPS_GETCURRENTPOSITION2;	//j'en ai pas vu l'utilité
	
	if (size < DSBSIZE_MIN)										//si la taille des datas est plus petite que la taille minimal pour la création du buffer (DSBSIZE_MIN constante Dsound)
		dsdd.dwBufferBytes = DSBSIZE_MIN;
	else if (size > DSBSIZE_MAX && !(flag & KML_SSTREAMING))	//si la taille des datas est trop grande et qu'on veut pas faire du streaming	(DSBSIZE_MAX constante Dsound)
	{
		KMLWarning ("Fichier trop grand (utiliser le streaming & verifier la validité du fichier)");
		fclose(*f);	//on referme le fichier ouvert
		FREE(f);	//on desalloue le pointeur de fichier
		return NULL;//retourne valeur d'erreur
	}
	else 
		dsdd.dwBufferBytes	= size;								//sinon la taille du buffer est celle lue dans le l'entête du fichier
	
	dsdd.lpwfxFormat = &waveformat;								//on remplit le champ par le format recupérer dans le fichier
//	dsdd.guid3DAlgorithm				= GUID_NULL;				pour directX 7 seulement ...

	if (waveformat.wFormatTag != WAVE_FORMAT_PCM)				//on vérifie le format du fichier
	{
		KMLWarning("Format de fichier Wave non valide");
		fclose(*f);	//on referme le fichier ouvert par readwaveheader
		FREE(f);	//on desalloue le pointeur de fichier
		return NULL;//on retourne une valeur d'erreur
	}

	if (flag & KML_SSTREAMING)																//si on veut faire du streaming
	{																						//on verifie la taille du son
		if (waveformat.nAvgBytesPerSec * NBRE_SECONDE_BUFFER_STREAM >= dsdd.dwBufferBytes)	//si elle est inférieur au buffer que l'on cree pour le streaming
			flag &= ~KML_SSTREAMING;														//on ne le streame pas
	}
												//Mise a jour des différent flags
	if (flag & KML_SFREQUENCE)					//Controle de la fréquence
		dsdd.dwFlags |= DSBCAPS_CTRLFREQUENCY;
	if (flag & KML_SVOLUME)						//Controle du volume
		dsdd.dwFlags |= DSBCAPS_CTRLVOLUME;
	if (flag & KML_SPAN)						//Controle de la balance
		dsdd.dwFlags |= DSBCAPS_CTRLPAN;
	if (flag & KML_SALWAYSHEAR)					//Son qui se joue même quand l'application n'est pas active
		dsdd.dwFlags |= DSBCAPS_GLOBALFOCUS;
	
	if (flag & (KML_SSOUNDMEMORYPREF | KML_SSOUNDMEMORY))			//on demande le son en memoire carte son
	{
		if (flag & KML_SSTREAMING)									//si streaming demandé, on force en memoire centrale
			dsdd.dwFlags |= DSBCAPS_LOCSOFTWARE;
		else
			dsdd.dwFlags |= DSBCAPS_LOCHARDWARE | DSBCAPS_STATIC;	//sinon, on met les flags pour Direct Sound
	}
	else
		dsdd.dwFlags |= DSBCAPS_LOCSOFTWARE;						//on demande en memoire centrale

	if (!(flag & KML_SSTREAMING))												//le son demande n'est pas en streaming
	{
		if (lpDS->CreateSoundBuffer(&dsdd, &NewBuffer, NULL) != DS_OK)			//on crée le buffer de son
		{
			if (flag & KML_SSOUNDMEMORYPREF)									//si ca a echoué et qu'on demandait en memoire carte son de preference, on essaie en memoire centrale
			{
				dsdd.dwFlags = DSBCAPS_LOCSOFTWARE;								//mise a jour des flags pour la memoire centrale
				if (lpDS->CreateSoundBuffer(&dsdd, &NewBuffer, NULL) != DS_OK)	//on crée le buffer de son
				{					//ca echoue
					fclose(*f);		//on ferme le fichier
					FREE(f);		//on desalloue le pointeur
					return NULL;	//on retourne une erreur
				}
			}
			else 
			{					//ca echoue
				fclose(*f);		//on ferme le fichier
				FREE(f);		//on desalloue le pointeur
				return NULL;	//on retourne une erreur
			}
		}
	
		if (NewBuffer->Lock(0, 0, &car, &size2, NULL, NULL, DSBLOCK_ENTIREBUFFER) != DS_OK)	//On lock entièrement le buffer crée
		{					//ca echoue
			fclose(*f);
			FREE(f);
			NewBuffer->Release();//on libere le buffer crée puisque ca a échoué
			return NULL;
		}
		
		if (KMLSoundReadWaveData(f,((char **) &car), size2) != KML_OK)						//On lit les datas du fichier f, on lit size2 octets, dans car (size2 et car sont renvoyé par la fonction lock)
		{					//ca echoue
			fclose(*f);
			FREE(f);
			NewBuffer->Release();//on libere le buffer crée puisque ca a échoué
			return NULL;
		}
		
		if (NewBuffer->Unlock(car, size2, NULL, NULL) != DS_OK)								//On delock le buffer
		{					//ca echoue
			fclose(*f);
			FREE(f);
			NewBuffer->Release();//on libere le buffer crée puisque ca a échoué
			return NULL;
		}
		
		fclose(*f);	//on ferme le fichier, puisqu'il n'est pas demandé en streaming, on en a plus besoin
		FREE(f);	//on desalloue le pointeur

		NewSound = (KMLSOUND *) ALLOC(sizeof(KMLSOUND));	//on alloue une instance pour le son
		if (NewSound == NULL)								//allocation echoue
			KMLError("Erreur d'allocation dans KMLSoundCreate");
		
		flagBDD = flag  & (KML_SSOUNDMEMORY | KML_SSOUNDMEMORYPREF | KML_SSTREAMING);//on enregistre dans la BDD que les flags de position memoire et de streaming (les autres flags étant dependant de l'instance du son)

		PtrBDD = KMLBDDAddSound(Path, flagBDD, NewSound);	//On ajoute un objet de son a la BDD, le son ajouté est Path, avec les flags BDD, ainsi que l'instance du son NewSound
															//On met à jour les champs de l'instance du son
		NewSound->BDD					= PtrBDD;			
		NewSound->SoundBuffer			= NewBuffer;
		NewBuffer->GetFrequency	(&NewSound->Frequence);
		KMLSoundSetVolume (NewSound, 100);
		NewSound->SoundBuffer->SetPan (0);
		NewSound->Volume				= KML_SVOLMAX;
		NewSound->Flag					= flag & (KML_SFREQUENCE | KML_SVOLUME | KML_SPAN | KML_SALWAYSHEAR | KML_SCUTONREPLAY | KML_SLOOP);
		NewSound->Flag				   |= KML_SSTOP;
		NewSound->Suivant				= NULL;
		PtrBDD->PtrLastSound			= NewSound; //comme l'objet est nouveau, la dernière instance est celle notre nouvelle instance
		PtrBDD->PtrKMLSound				= NewSound; //et la première instance aussi
	
		KMLAddTail(&KMLSoundBDDListe, PtrBDD->Node);//on ajoute l'objet dans la liste de BDD à la fin

		return NewSound;							//on retourne l'instance créee
	}
	else																					//creation du son en Streaming
	{												
		unsigned long Taille;
		KMLSTREAM *Stream;

		Taille = dsdd.dwBufferBytes;														//on enregistre la taille du buffer
		dsdd.dwBufferBytes = waveformat.nAvgBytesPerSec * NBRE_SECONDE_BUFFER_STREAM;		//on choisit une nouvelle taille de buffer

		if (lpDS->CreateSoundBuffer(&dsdd, &NewBuffer, NULL) != DS_OK)						//creation du buffer de son en memoire centrale forcément
		{				//ca echoue
			fclose(*f);
			FREE(f);
			return NULL;
		}
		
		if (NewBuffer->Lock(0, 0, &car, &size2, NULL, NULL, DSBLOCK_ENTIREBUFFER) != DS_OK)	//on lock le buffer crée
		{				//ca echoue
			fclose(*f);
			FREE(f);
			NewBuffer->Release();//comme ca echoue on efface libere le buffer
			return NULL;
		}
		
		if (KMLSoundReadWaveData(f,((char **) &car), size2) != KML_OK)						//on lit les datas du fichier de size2 octets dans car
		{				//ca echoue
			fclose(*f);
			FREE(f);
			NewBuffer->Release();//comme ca echoue on efface libere le buffer
			return NULL;
		}
		
		if (NewBuffer->Unlock(car, size2, NULL, NULL) != DS_OK)								//on delock le buffer
		{				//ca echoue
			fclose(*f);	
			FREE(f);
			NewBuffer->Release();//comme ca echoue on efface libere le buffer
			return NULL;
		}
		
		NewSound = (KMLSOUND *) ALLOC(sizeof(KMLSOUND));		//On alloue une nouvelle instance de son
		if (NewSound == NULL)									//Ca echoue
			KMLError("Erreur d'allocation dans KMLSoundCreate");

		Stream = (KMLSTREAM *) ALLOC(sizeof(KMLSTREAM));		//On alloue les infos pour le streaming enregistré dans le membre suivant de son
		if (Stream == NULL)										//Ca echoue
			KMLError("Erreur d'allocation dans KMLSoundCreate");

		Stream->writeposition	= 0;							//Position d'ecriture dans le buffer initialise a 0
		Stream->fichier			= f;							//Pointeur de fichier
		Stream->tailledata		= Taille;						//Taille totale des datas du wave
		Stream->ptrfichier		= dsdd.dwBufferBytes;			//Position du pointeur de lecture dans le fichier incrémenté de la taille du buffer puisque le buffer est chargé a l'init
		Stream->taillebuffer	= dsdd.dwBufferBytes;			//Taille du buffer
		Stream->tailleentete	= waveformat.cbSize;			//Taille de l'en tete du wave
		Stream->taillesample	= (waveformat.nChannels * waveformat.wBitsPerSample) >> 3;	//REVOIR pour le blanc
		
		fclose(*Stream->fichier);								//On ferme le fichier

		flagBDD = flag  & (KML_SSOUNDMEMORY | KML_SSOUNDMEMORYPREF | KML_SSTREAMING);//On enregistre dans la BDD que les flags de position memoire et de streaming (les autres flags étant dependant de l'instance du son)

		PtrBDD = KMLBDDAddSound(Path, flagBDD, NewSound);		//On ajoute un objet a la BDD
																//On met à jour les champs de l'instance
		NewSound->BDD					= PtrBDD;
		NewSound->SoundBuffer			= NewBuffer;
		NewBuffer->GetFrequency	(&NewSound->Frequence);
		KMLSoundSetVolume (NewSound, 100);
		NewSound->SoundBuffer->SetPan (0);
		NewSound->Volume				= KML_SVOLMAX;
		NewSound->Flag					= flag & (KML_SFREQUENCE | KML_SVOLUME | KML_SPAN | KML_SALWAYSHEAR | KML_SCUTONREPLAY | KML_SLOOP | KML_SSTREAMING);
		NewSound->Flag				   |= KML_SSTOP;
		NewSound->Suivant				= (KMLSOUND *) Stream;
		PtrBDD->PtrLastSound			= NewSound; //comme l'objet est nouveau, la dernière instance est celle notre nouvelle instance
		PtrBDD->PtrKMLSound				= NewSound; //et la première instance aussi
	
		KMLAddTail(&KMLSoundBDDListe, PtrBDD->Node);//on ajoute l'objet dans la liste de BDD à la fin

		return NewSound;							//on retourne l'instance du son
	}
}

KMLSOUNDBDD *KMLBDDAddSound (char *Path, long flag, KMLSOUND *Sound)
//***************************************************************
//	FUNCTION : KMLBDDAddSound
//---------------------------------------------------------------
//	INPUT	 : path, flag, soundbuffer
//---------------------------------------------------------------
//	OUTPUT	 : kmlsoundbdd *
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> vendredi 28 juillet 2000
//	REVISION : 
//***************************************************************
{
	KMLSOUNDBDD *bdd;

	bdd = (KMLSOUNDBDD *) ALLOC(sizeof(KMLSOUNDBDD));		//Creation d'un nouvel objet dans la BDD
	
	if (bdd == NULL)											//Gestion des erreurs d'allocation
		KMLError("Erreur d'allocation dans KMLBDDAddSound");
	
	bdd->Path = (char *) ALLOC(strlen(Path) + 1);			//Allocation du pointeur vers char de BDD pour copier le Path passé en paramètre
	if (bdd->Path == NULL)										//Gestion des erreurs d'allocation
	{
		FREE(bdd);	//on free la memoire alloue par bdd puisque ca a echoué
		KMLError("Erreur d'allocation dans KMLBDDAddSound");
	}
	strcpy(bdd->Path, Path);									//Copie de Path passé en paramètre dans bdd->Path

	bdd->PtrKMLSound	= Sound;								//Mise a jour des champ de l'objet BDD
	bdd->flag			= flag;
	bdd->PtrLastSound	= NULL;
	bdd->PtrKMLSound	= NULL;
	bdd->User			= 1;									//Par défaut un utilisateur puisque cette fonction est appelée à la création d'une nouvelle instance

	return bdd;													//On retourne l'objet alloué
}

KMLSOUNDBDD *KMLBDDSearch (char *Path, long flag)
//***************************************************************
//	FUNCTION : KMLBDDSearch
//---------------------------------------------------------------
//	INPUT	 : path, flag
//---------------------------------------------------------------
//	OUTPUT	 : kmlsoundbdd *
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> vendredi 28 juillet 2000
//	REVISION : 
//***************************************************************
{
	KMLSOUNDBDD *actu,*suiv;
	
	actu = (KMLSOUNDBDD *)KMLSoundBDDListe.First;			//On pointe au début de la liste
	
	if(actu == NULL)	//Si liste vide alors on retourne non trouvé (NULL)
		return NULL;

	while(suiv = (KMLSOUNDBDD *)actu->Node.Suivant)
	{
		if (strcmp(actu->Path, Path) == 0)		//On compare les path pour savoir si ce sont les memes sons
		{										
			if (!(actu->flag & KML_SSTREAMING))	//Si l'objet n'est en streaming alors on retourne l'instance sinon on passe au suivant car une seule instance pour les sons streamés
				return actu;
		}
		actu = suiv;
	}

	return NULL;								//Instance non trouvée
}

char KMLSoundDelete (KMLSOUND *sound)
//***************************************************************
//	FUNCTION : KMLSoundDelete
//---------------------------------------------------------------
//	INPUT	 : KMLSOUND *
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> vendredi 28 juillet 2000
//	REVISION : 
//***************************************************************
{
	KMLSOUND *precedent,*courant;

	precedent = NULL;													//Au début pas de précédent
																		//Pour effacer le son, il faut pouvoir rechainer les elements avant et après ce son, donc il faut connaitre le suivant et le precedent (pour trouver le precedent, il faut parcourir la liste)
	courant = sound->BDD->PtrKMLSound;									//On place le courant sur le premier element de la liste
	
	while (courant != NULL)
	{
		if (courant == sound)											//Si les adresses sont egales alors on a trouvé l'élément à effacer
		{
			if (courant->Flag & KML_SSTREAMING)							//Si le son a éffacer est en Streaming
			{
				if (courant->Flag & KML_SPLAY)
					fclose( *(((KMLSTREAM *)courant->Suivant)->fichier) );	//On ferme le fichier que si le son se jouait
				FREE( (((KMLSTREAM *)courant->Suivant)->fichier) );		//On désalloue le pointeur de fichier alloué par ce son
				FREE(courant->Suivant);									//On efface les infos pour le Streaming contenu dans le champs suivant du son
				sound->BDD->PtrKMLSound = NULL;								//Il n'y a plus de premier dans la liste d'objet
			}
			else if (precedent != NULL)										//Sinon le son n'est pas Streamé, si il y a un précédent
				precedent->Suivant = courant->Suivant;						//On rechaine dans l'élément précédents
			else
				sound->BDD->PtrKMLSound = courant->Suivant;					//Sinon si pas de précédent, alors c'est le premier de la liste de l'objet de BDD qu'on rechaine
			
			if (sound->BDD->PtrKMLSound == NULL)							//Si le premier de la liste pointe sur NULL, alors plus d'élément
			{
				FREE(sound->BDD->Path);								//On Free le Path qui avait été alloué
				KMLRemoveNode(sound->BDD->Node);						//On Free l'instance dans la liste de BDD
				FREE(sound->BDD);									//On Free l'instance elle meme
				sound->SoundBuffer->Release();							//On Libère le buffer de son
				FREE(sound);											//On Free le son
				return KML_OK;											//Retourne OK
			}
			
			if (sound->BDD->PtrLastSound == sound)						//Si le pointeur sur la dernière instance dans la liste de BDD est le son à effacer, alors le dernier est maintenant le précédent
				sound->BDD->PtrLastSound = precedent;
			
			sound->SoundBuffer->Release();								//On libère le buffer Direct Sound
			FREE(sound);												//On efface le son
			return KML_OK;												//Retourne OK
		}
		precedent	= courant;				//le précédent devient le courant
		courant		= courant->Suivant;		//on passe au suivant de courant
	}
	return KML_SOUND_ERROR;//Erreur son non trouvé
}


char KMLSoundSetAction (KMLSOUND *PtrSound, long flag, long time)
//***************************************************************
//	FUNCTION : KMLSoundSetAction
//---------------------------------------------------------------
//	INPUT	 : PtrSound flag
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	if (flag & KML_SFADEIN)								//Si on demande de faire un fade in
	{
		PtrSound->TimeInit		= GetTickCount();		//Le temps initial pour le fade est le temps courant (soit GetTickCount() )
		PtrSound->Time			= time;					//Le temps total du fade est time passé en paramètre
		PtrSound->TimeCourant	= PtrSound->TimeInit;	//Le temps courant est remis au temps initial
		PtrSound->Flag |= KML_SFADEIN;					//On met le flag Fade In au son ptrsound
		KMLSoundSetVolume(PtrSound,PtrSound->Volume);	//On remet le volume du son à son volume (si il y avait eu un fade avant)
	}
	else if (flag & KML_SFADEOUT)						//Si on demande de faire un fade out
	{
		PtrSound->TimeInit		= GetTickCount();		//Le temps initial pour le fade est le temps courant (soit GetTickCount() )
		PtrSound->Time			= time;					//Le temps total du fade est time passé en paramètre
		PtrSound->TimeCourant	= PtrSound->TimeInit;	//Le temps courant est remis au temps initial
		PtrSound->Flag |= KML_SFADEOUT;					//On met le flag Fade Out au son ptrsound
		KMLSoundSetVolume(PtrSound,PtrSound->Volume);	//On remet le volume du son à son volume (si il y avait eu un fade avant)
	}

	if (flag & KML_SPLAY)																				//On demande de jouer le son
	{
		if (PtrSound->Flag & KML_SPLAY)																	//Le son joue déjà
		{
			if (PtrSound->Flag & KML_SPAUSE)															//Le son est en pause
			{
				PtrSound->Flag &= ~KML_SPAUSE;															//On enlève le flag de pause
				if (PtrSound->Flag & (KML_SFADEIN|KML_SFADEOUT))										//Si le son était en fade
					PtrSound->TimeInit = GetTickCount() - (PtrSound->TimeCourant - PtrSound->TimeInit);	//On remet le temps initial au temps courant moins ce qui s'était écoulé avant qu'on mette pause
			}
			else if (!(PtrSound->Flag & KML_SCUTONREPLAY))												//Sinon si le son n'était pas en pause et que le son ne se cutte pas en plein milieu (flag KML_SCUTONREPLAY non mis)
				return KML_SOUND_PLAYINGYET;															//Alors retourne erreur le son se joue déjà
			else
			{																							//Sinon on peut couper le son en plein milieu
				if (PtrSound->Flag & KML_SSTREAMING)													//Est ce que le son était Streamé
				{
					KMLSTREAM *Stream;
					Stream = (KMLSTREAM *) PtrSound->Suivant;											//On met le pointeur Stream sur les infos du son streamé (champ Suivant)
					fseek( *Stream->fichier, - ((long) Stream->ptrfichier), SEEK_CUR);					//On déplace le pointeur du fichier streamé au début des datas du son
					Stream->ptrfichier = 0;																//On met à jour le pointeur utilisé pour savoir à quel endroit du fichier on est, 0 signifie que l'on est au début des datas et non du fichier (car il y a l'en tete du wav au début)
					KMLSoundGestionStream(PtrSound);					//On recharge les données dans le buffer
					if (flag & KML_SLOOP)																//Si on demande de looper
					{
						if (PtrSound->SoundBuffer->Stop() != DS_OK)						//On Stoppe le son
							return KML_SOUND_ERROR;
						if (PtrSound->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) //On le rejoue avec le flag LOOP
							return KML_SOUND_ERROR;
					}
					else
					{
						if (PtrSound->SoundBuffer->SetCurrentPosition(0) != DS_OK)							//On place le pointeur de lecture du son au début du buffer
							return KML_SOUND_ERROR;//erreur si on a pas pu deplacer le pointeur de lecture DirectSound
					}
					KMLSoundGestionStream(PtrSound);													//On finit de charger le buffer
				}																						//On procède en deux fois pour recharger le buffer, car une partie du buffer est utilisé par la lecture et on ne peut y écrire...
				else
				{
					if (flag & KML_SLOOP)																//Si on demande de looper
					{
						if (PtrSound->SoundBuffer->Stop() != DS_OK)						//On stoppe le son
							return KML_SOUND_ERROR;
						if (PtrSound->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) //On le rejoue avec le flag LOOP
							return KML_SOUND_ERROR;
					}
					else
					{
						if (PtrSound->SoundBuffer->SetCurrentPosition(0) != DS_OK)							//Si son non Streamé alors on replace juste le pointeur de lecture Direct Sound au début du buffer de son
							return KML_SOUND_ERROR;//gestion de l'erreur si on peut pas déplacer le pointeur DirectSound
					}
				}
				
				if (flag & KML_SFADEIN)					//si on demande aussi un fade in en meme tant que play
				{
					PtrSound->Flag &= ~KML_SFADEOUT;	//on enleve le flag fade out au cas ou il y soit
				}
				else if (flag & KML_SFADEOUT)			//sinon si on demande un fade out
				{
					PtrSound->Flag &= ~KML_SFADEIN;		//on enleve le flag fade in au cas ou il y soit
				}
				else if (PtrSound->Flag & (KML_SFADEOUT | KML_SFADEIN))	//sinon si on ne demande pas de fade mais qu'il y en avait un en cours
				{
					PtrSound->Flag &= ~(KML_SFADEOUT | KML_SFADEIN);	//on enleve le flag du fade in et out
					KMLSoundSetVolume(PtrSound,PtrSound->Volume);		//on remet le volume original du son
				}
				return KML_OK;//Retourne OK
			}
		}
		
		if (flag & KML_SLOOP)
		{
			if (PtrSound->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
				return KML_SOUND_ERROR;

			if (flag & KML_SCUTONREPLAY) 
				PtrSound->Flag |= KML_SCUTONREPLAY;

			if (flag & KML_SFADEIN)
			{
				PtrSound->Flag &= ~KML_SFADEOUT;
			}
			else if (flag & KML_SFADEOUT)
			{
				PtrSound->Flag &= ~KML_SFADEIN;
			}
			else if (PtrSound->Flag & (KML_SFADEOUT | KML_SFADEIN))
			{
				PtrSound->Flag &= ~(KML_SFADEOUT | KML_SFADEIN);
				KMLSoundSetVolume(PtrSound,PtrSound->Volume);
			}

			if (!(PtrSound->Flag & KML_SPLAY) && (PtrSound->Flag & KML_SSTREAMING))//si le son ne se jouait pas et qu'il était streamé...réouverture du fichier
			{
				KMLSTREAM *Stream;
				Stream = (KMLSTREAM *) PtrSound->Suivant;
				*Stream->fichier = fopen(PtrSound->BDD->Path,"rb");
				if (*Stream->fichier == NULL)
				{
					KMLWarning("Ne peut ouvrir le fichier %s",PtrSound->BDD->Path);
					return KML_SOUND_ERROR;
				}
				if (fseek(*Stream->fichier, Stream->tailleentete + Stream->ptrfichier, SEEK_SET) != 0)
				{
					KMLWarning("Ne peut lire dans le fichier %s",PtrSound->BDD->Path);
					return KML_SOUND_ERROR;
				}
			}

			PtrSound->Flag |= KML_SPLAY | KML_SLOOP;
		}
		else 
		{
			if (PtrSound->Flag & KML_SSTREAMING)
			{
				if (PtrSound->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
					return KML_SOUND_ERROR;
			}
			else 
			{
				if (PtrSound->SoundBuffer->Play(0, 0, 0) != DS_OK)
					return KML_SOUND_ERROR;
			}
			
			if (flag & KML_SCUTONREPLAY) 
				PtrSound->Flag |= KML_SCUTONREPLAY;

			if (PtrSound->Flag & KML_SLOOP)
				PtrSound->Flag &= ~KML_SLOOP;

			else if (!(PtrSound->Flag & (KML_SFADEOUT | KML_SFADEIN)))
			{
				PtrSound->Flag &= ~(KML_SFADEOUT | KML_SFADEIN);
				KMLSoundSetVolume(PtrSound,PtrSound->Volume);
			}


			if (!(PtrSound->Flag & KML_SPLAY) && (PtrSound->Flag & KML_SSTREAMING))//si le son ne se jouait pas et qu'il était streamé...réouverture du fichier
			{
				KMLSTREAM *Stream;

				Stream = (KMLSTREAM *) PtrSound->Suivant;
				*Stream->fichier = fopen(PtrSound->BDD->Path,"rb");
				if (*Stream->fichier == NULL)
				{
					KMLWarning("Ne peut ouvrir le fichier %s",PtrSound->BDD->Path);
					return KML_SOUND_ERROR;
				}
				if (fseek(*Stream->fichier, Stream->tailleentete + Stream->ptrfichier, SEEK_SET) != 0)
				{
					KMLWarning("Ne peut lire dans le fichier %s",PtrSound->BDD->Path);
					return KML_SOUND_ERROR;
				}
			}

			PtrSound->Flag |= KML_SPLAY;
		}
	}
	else if (flag & KML_SSTOP)
	{
		if (PtrSound->SoundBuffer->Stop() != DS_OK)
			return KML_SOUND_ERROR;
		if ((PtrSound->Flag & KML_SSTREAMING) && (PtrSound->Flag & KML_SPLAY))
		{
			KMLSTREAM *Stream;

			Stream = (KMLSTREAM *) PtrSound->Suivant;
			fseek( *Stream->fichier, - ((long) Stream->ptrfichier), SEEK_CUR);	//on revient au début du fichier
			Stream->ptrfichier = 0;												//on met a jour le pointeur du fichier
			
			if (PtrSound->SoundBuffer->SetCurrentPosition(Stream->taillebuffer - 1) != DS_OK)			//on met le pointeur de lecture Dsound a la fin
				return KML_SOUND_ERROR;
			
			Stream->writeposition = 0;											//on remet le pointeur du buffer a jour
			KMLSoundGestionStream(PtrSound);									//on charge le buffer
			if (PtrSound->SoundBuffer->SetCurrentPosition(0) != DS_OK)			//on remet le pointeur de lecture Dsound au début
				return KML_SOUND_ERROR;		
			fclose(*Stream->fichier);
		}
		else
		{
			if (PtrSound->SoundBuffer->SetCurrentPosition(0) != DS_OK)
				return KML_SOUND_ERROR;
		}
		PtrSound->Flag &= ~(KML_SPLAY|KML_SLOOP|KML_SCUTONREPLAY|KML_SFADEIN|KML_SFADEOUT);
		PtrSound->Flag |= KML_SSTOP;
	}
	else if (flag & KML_SPAUSE)
	{
		if ((PtrSound->Flag & KML_SPLAY) && (!(PtrSound->Flag & KML_SPAUSE)))
		{
			if (PtrSound->SoundBuffer->Stop() != DS_OK)
				return KML_SOUND_ERROR;
			PtrSound->Flag |= KML_SPAUSE;
			if (PtrSound->Flag & (KML_SFADEIN|KML_SFADEOUT))
				PtrSound->TimeCourant = GetTickCount();
		}
		else
			return KML_SOUND_ERROR;
	}
	return KML_OK;
}

char KMLSoundRestore (KMLSOUND *PtrSound)
//***************************************************************
//	FUNCTION : KMLSoundRestore
//---------------------------------------------------------------
//	INPUT	 : PtrSound
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	FILE			*f;
	KMLSOUND		*courant;
	unsigned long	size;
	void			*car;

	courant = PtrSound->BDD->PtrKMLSound;
	
	lpBufferPrimaire->Restore();
	
	if (courant->SoundBuffer->Restore() != DS_OK)
		return KML_SOUND_ERROR;
	
	if (courant->Flag & KML_SSTREAMING)
	{
		KMLSTREAM *Stream;
		Stream = (KMLSTREAM *) courant->Suivant;
		PtrSound->Flag &= ~KML_SSTREAMRESERVED;
		if (PtrSound->Flag & KML_SPLAY)
		{
			//rechargement du fichier
			void *car;
			unsigned long size2;

			if (Stream->ptrfichier > Stream->taillebuffer)
			{
				if (fseek(*Stream->fichier, -((long) Stream->taillebuffer), SEEK_CUR) != 0)
					return KML_SOUND_ERROR;
			}
			else if (fseek(*Stream->fichier, Stream->tailleentete, SEEK_SET) != 0)
				return KML_SOUND_ERROR;
			
			if (PtrSound->SoundBuffer->Lock(0, 0, &car, &size2, NULL, NULL, DSBLOCK_ENTIREBUFFER) != DS_OK)
			{
				fclose(*Stream->fichier);
				return KML_SOUND_ERROR;
			}

			if (KMLSoundReadWaveData(Stream->fichier,((char **) &car), size2) != KML_OK)
			{	
				fclose(*Stream->fichier);
				return KML_SOUND_ERROR;
			}
			
			if (PtrSound->SoundBuffer->Unlock(car, size2, NULL, NULL) != DS_OK)
			{
				fclose(*Stream->fichier);	
				return KML_SOUND_ERROR;
			}
			
			Stream->writeposition	= 0;					//position d'ecriture dans le buffer initialise a 0
			Stream->ptrfichier		= Stream->taillebuffer;	//position du pointeur de lecture dans le fichier incrémenté de la taille du buffer puisque le buffer est chargé a l'init

			PtrSound->Flag &= ~KML_SPLAY;
			KMLSoundSetAction(PtrSound,PtrSound->Flag | KML_SPLAY);//REVOIR CETTE PARTIE DANS RESTORE
		}
		else
		{
			void *car;
			unsigned long size2;

			if (PtrSound->SoundBuffer->Lock(0, 0, &car, &size2, NULL, NULL, DSBLOCK_ENTIREBUFFER) != DS_OK)
			{
				fclose(*Stream->fichier);
				return KML_SOUND_ERROR;
			}
			
			if (KMLSoundReadWaveData(Stream->fichier,((char **) &car), size2) != KML_OK)
			{	
				fclose(*Stream->fichier);
				return KML_SOUND_ERROR;
			}
			
			if (PtrSound->SoundBuffer->Unlock(car, size2, NULL, NULL) != DS_OK)
			{
				fclose(*Stream->fichier);	
				return KML_SOUND_ERROR;
			}
			
			Stream->writeposition	= 0;					//position d'ecriture dans le buffer initialise a 0
			Stream->ptrfichier		= Stream->taillebuffer;	//position du pointeur de lecture dans le fichier incrémenté de la taille du buffer puisque le buffer est chargé a l'init
			
			fclose(*Stream->fichier);						//on ferme le fichier

		}
		
		return KML_OK;
	}

	if (KMLSoundReadWaveHeader(&f, courant->BDD->Path, NULL, &size) != KML_OK)//on reparcourt le header du fichier
		return KML_SOUND_ERROR;

	if (courant->SoundBuffer->Lock(0, 0, &car, &size, NULL, NULL, DSBLOCK_ENTIREBUFFER) != DS_OK)//on lock le buffer du son
	{
		fclose(f);
		return NULL;
	}

	if (KMLSoundReadWaveData(&f,((char **) &car), size) != KML_OK)//on lit et ecrit les datas dans le buffer
	{	
		fclose(f);
		return NULL;
	}
	
	if (courant->SoundBuffer->Unlock(car, size, NULL, NULL) != DS_OK)//on delock le buffer
		return NULL;

	courant = courant->Suivant;
	while (courant != NULL)//on restaure toutes les surfaces
	{
		if (courant->SoundBuffer->Restore() != DS_OK)
			return KML_SOUND_ERROR;
		courant = courant->Suivant;
	}
	return KML_OK;
}

char KMLSoundReadWaveHeader(FILE **f, char *Path, WAVEFORMATEX *waveformat, unsigned long *size)
//***************************************************************
//	FUNCTION : KMLSoundReadWaveHeader
//---------------------------------------------------------------
//	INPUT	 : FILE **, Path, *waveformat, *size
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	KMLWAVEHEADER		waveheader;
	char				temp[4];

	if (Path == NULL)
	{
		KMLWarning("Nom de fichier invalide dans KMLSoundReadWaveHeader");
		return NULL;
	}

	*f = fopen (Path,"rb");

	if (*f == NULL)
	{
		KMLWarning("Nom du fichier non valide dans KMLSoundReadWaveHeader");
		return KML_SOUND_ERROR;
	}

	fread(&waveheader, sizeof(waveheader), 1, *f);
	
	if (strncmp(waveheader.RIFF, "RIFF", 4) != 0)
	{
		KMLWarning("Format du fichier non valide dans KMLSoundReadWaveHeader %s",waveheader.RIFF);
		fclose(*f);
		return KML_SOUND_ERROR;
	}
	
	if (strncmp(waveheader.WAVE, "WAVE", 4) != 0)
	{
		KMLWarning("Format du fichier non valide dans KMLSoundReadWaveHeader");
		fclose(*f);
		return KML_SOUND_ERROR;
	}

	if (strncmp(waveheader.fmt_, "fmt ", 4) != 0)
	{
		KMLWarning("Format du fichier non valide dans KMLSoundReadWaveHeader");
		fclose(*f);
		return KML_SOUND_ERROR;
	}

	if (waveheader.SizeOfDesc < sizeof(PCMWAVEFORMAT))
	{
		KMLWarning("Fichier Wave invalide dans KMLSoundReadWaveHeader");
		fclose(*f);
		return KML_SOUND_ERROR;
	}
	
	if (waveheader.SizeOfDesc > sizeof(PCMWAVEFORMAT))//si le wave a des extra bytes on les lis
	{
		fread(&(temp[0]), sizeof(temp[0]), waveheader.SizeOfDesc - sizeof(PCMWAVEFORMAT), *f);
	}

	if (waveformat != NULL)
	{
		waveformat->wFormatTag		= waveheader.FormatTag;
		waveformat->nChannels		= waveheader.Channels;
		waveformat->nSamplesPerSec	= waveheader.SampleRate;
		waveformat->nAvgBytesPerSec	= waveheader.BytesPerSec;
		waveformat->nBlockAlign		= waveheader.BlockAlign;
		waveformat->wBitsPerSample	= waveheader.BitsPerSample;
		waveformat->cbSize			= sizeof(waveheader) + (unsigned short) (waveheader.SizeOfDesc - sizeof(PCMWAVEFORMAT)) + 8;//on met ce champ pour l'offset de début des datas du wave (utile pour les fichiers streamé)
	}

	fread(temp, 4, 1, *f);
	if (strncmp(temp, "data", 4) != 0)
	{
		KMLWarning("Datas non touvees dans le fichier Wave dans KMLSoundReadWaveHeader");
		fclose(*f);
		return KML_SOUND_ERROR;
	}

	fread(size, sizeof(*size), 1, *f);

	return KML_OK;
}

char KMLSoundReadWaveData(FILE **f, char **car, unsigned long size)
//***************************************************************
//	FUNCTION : KMLSoundReadWaveData
//---------------------------------------------------------------
//	INPUT	 : FILE **, char **buffer, size
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	if (fread  ( *car, 1, size, *f) == size)
		return KML_OK;
	return KML_SOUND_ERROR;
}

char KMLSoundUpdate ()
//***************************************************************
//	FUNCTION : KMLSoundUpdate
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	KMLSOUNDBDD *actu,*suiv;
	
	actu = (KMLSOUNDBDD *)KMLSoundBDDListe.First;
	
	if(actu == NULL)
		return NULL;

	while(suiv = (KMLSOUNDBDD *)actu->Node.Suivant)
	{
		KMLSOUND *courant;

		courant = actu->PtrKMLSound;
		while (courant != NULL)
		{
			if (courant->Flag & KML_SPLAY)
			{
				if (!(courant->Flag & KML_SPAUSE))
				{				
					unsigned long param;
					courant->SoundBuffer->GetStatus(&param);
					if (param & (DSBSTATUS_BUFFERLOST))
					{
						if (KMLSoundRestore (courant) == KML_OK)
						{
							if (!(param & (DSBSTATUS_PLAYING)))//|DSBSTATUS_TERMINATED)) pour directX 7
							{
								if (courant->Flag & (KML_SFADEOUT|KML_SFADEIN))
								{
									KMLSoundSetVolume(courant,courant->Volume);
									courant->Flag &= ~(KML_SFADEOUT|KML_SFADEIN);
								}
								courant->Flag &= ~(KML_SPLAY|KML_SPAUSE);
								courant->Flag |= KML_SSTOP;
							}
						}
					}
					if (!(param & (DSBSTATUS_PLAYING)))///|DSBSTATUS_TERMINATED)) pour directX 7
					{
						if (courant->Flag & (KML_SFADEOUT|KML_SFADEIN))
						{
							KMLSoundSetVolume(courant,courant->Volume);
							courant->Flag &= ~(KML_SFADEOUT|KML_SFADEIN);
						}
						courant->Flag &= ~(KML_SPLAY|KML_SPAUSE);
						courant->Flag |= KML_SSTOP;
					}
					else
					{
						if (courant->Flag & KML_SFADEIN)
						{
							if (GetTickCount() >= courant->TimeInit + courant->Time)
							{
								courant->SoundBuffer->SetVolume(-TabLog[100 - KMLGetVolume(courant->Volume)]);
								courant->Flag &= ~KML_SFADEIN;
							}
							else
							{
								courant->SoundBuffer->SetVolume(-TabLog[100 - KMLGetVolume(courant->Volume * (GetTickCount() - courant->TimeInit)) / courant->Time]);
							}	
		
						}
						else if (courant->Flag & KML_SFADEOUT)
						{
							if (GetTickCount() >= courant->TimeInit + courant->Time)
							{
								courant->SoundBuffer->SetVolume(-TabLog[100]);
								courant->Flag &= ~KML_SFADEOUT;
							}
							else
								courant->SoundBuffer->SetVolume(-TabLog[( KMLGetVolume(courant->Volume) * (GetTickCount() - courant->TimeInit)) / courant->Time]);
						}
					}
				}
			}
			else if (courant->Flag & KML_SPAUSE)
				courant->Flag &= ~KML_SPAUSE;

			//gestion des sons streamé
			if ((courant->Flag & KML_SSTREAMING))
			{
				if  (!(courant->Flag & KML_SPAUSE))
					KMLSoundGestionStream(courant);
				courant = NULL;
			}
			else
				courant = courant->Suivant;
		}
		actu = suiv;
	}

	return KML_OK;
}

char KMLSoundIsFinished	(KMLSOUND *PtrSound)
//***************************************************************
//	FUNCTION : KMLSoundIsFinished
//---------------------------------------------------------------
//	INPUT	 : PtrSound
//---------------------------------------------------------------
//	OUTPUT	 : TRUE or FALSE
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 1 août 2000
//	REVISION : 
//***************************************************************
{
	unsigned long param;
	PtrSound->SoundBuffer->GetStatus(&param);
	if (param & DSBSTATUS_PLAYING)
		return false;
	return true;
}

char KMLSoundSetFrequency (KMLSOUND *PtrSound, unsigned long Frequence)
//***************************************************************
//	FUNCTION : KMLSoundSetFrequency
//---------------------------------------------------------------
//	INPUT	 : PtrSound, Frequence
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 2 août 2000
//	REVISION : 
//***************************************************************
{
	if (Frequence != KML_SFREQDEFAUT)
	{
		if (Frequence < DSBFREQUENCY_MIN)
			Frequence = DSBFREQUENCY_MIN;
		else if (Frequence > DSBFREQUENCY_MAX)
			Frequence = DSBFREQUENCY_MAX;
	}

	if (PtrSound->SoundBuffer->SetFrequency(Frequence) != DS_OK)
		return KML_SOUND_ERROR;
	PtrSound->Frequence = Frequence;
	return KML_OK;	
}

char KMLSoundSetVolume (KMLSOUND *PtrSound, unsigned char Volume)
//***************************************************************
//	FUNCTION : KMLSoundSetVolume
//---------------------------------------------------------------
//	INPUT	 : PtrSound, Volume
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 2 août 2000
//	REVISION : 
//***************************************************************
{
	if (Volume > 100)
		Volume = 100;
	else if (Volume < 0)
		Volume = 0;
	if (PtrSound->SoundBuffer->SetVolume(-TabLog[100 - ((Volume * KMLVolumeGeneral) >> 10 )]) != DS_OK)
		return KML_SOUND_ERROR;
	PtrSound->Volume = Volume;
	return KML_OK;
}

char KMLSoundSetPan (KMLSOUND *PtrSound, char Span)
//***************************************************************
//	FUNCTION : KMLSoundSetPan
//---------------------------------------------------------------
//	INPUT	 : PtrSound, Span
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 2 août 2000
//	REVISION : 
//***************************************************************
{
	if (Span < KML_SPANLEFT)
		Span = KML_SPANLEFT;
	else if (Span > KML_SPANRIGHT)
		Span = KML_SPANRIGHT;

	if (Span < 0)
	{
		if (PtrSound->SoundBuffer->SetPan(TabLog[-Span]) != DS_OK)
			return KML_SOUND_ERROR;
	}
	else
	{
		if (PtrSound->SoundBuffer->SetPan(-TabLog[Span]) != DS_OK)
			return KML_SOUND_ERROR;
	}
	PtrSound->Span = Span;
	return KML_OK;
}

char KMLSoundSetFlag (KMLSOUND *PtrSound, long flag)
//***************************************************************
//	FUNCTION : KMLSoundSetFlag
//---------------------------------------------------------------
//	INPUT	 : PtrSound, flag
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 2 août 2000
//	REVISION : 
//***************************************************************
{
	flag = ((flag ^ PtrSound->Flag) & flag);//on ne prend que les flags qui ne sont pas déjà setter dans kmlsound

	if (flag & KML_SCUTONREPLAY)
		PtrSound->Flag |= KML_SCUTONREPLAY;
	if (flag & KML_SLOOP)
	{
		if (!KMLSoundIsFinished	(PtrSound))
		{
			if (PtrSound->SoundBuffer->Stop() == DD_OK)
			{
				if (PtrSound->SoundBuffer->Play(0, 0, DSBPLAY_LOOPING) == DD_OK)
				{
					PtrSound->Flag |= KML_SLOOP;
					return KML_OK;
				}
			}
		}
		else
		{
			PtrSound->Flag |= KML_SLOOP;
			return KML_OK;
		}
	}
	return KML_SOUND_ERROR;
}

char KMLSoundUnsetFlag (KMLSOUND *PtrSound, long flag)
//***************************************************************
//	FUNCTION : KMLSoundUnsetFlag
//---------------------------------------------------------------
//	INPUT	 : PtrSound, flag
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 2 août 2000
//	REVISION : 
//***************************************************************
{
	flag &= PtrSound->Flag;

	if (flag & KML_SCUTONREPLAY)
		PtrSound->Flag &= ~KML_SCUTONREPLAY;
	if (flag & KML_SLOOP)
	{
		if (!KMLSoundIsFinished	(PtrSound))
		{
			if (PtrSound->SoundBuffer->Stop() == DD_OK)
			{
				if (PtrSound->SoundBuffer->Play(0, 0, 0) == DD_OK)
				{
					PtrSound->Flag &= ~KML_SLOOP;
					return KML_OK;
				}
			}
		}
		else
		{
			PtrSound->Flag &= ~KML_SLOOP;
			return KML_OK;
		}
	}
	return KML_SOUND_ERROR;
}

char KMLSoundSetGlobalAction (long flag, long time)
//***************************************************************
//	FUNCTION : KMLSoundSetGlobalAction
//---------------------------------------------------------------
//	INPUT	 : flag, time (eventuelle pour les fades)
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> jeudi 3 août 2000
//	REVISION : 
//***************************************************************
{

	KMLSOUNDBDD	*actu,*suiv;
	actu = (KMLSOUNDBDD	*)KMLSoundBDDListe.First;
	
	if(actu == NULL)
		return KML_SOUND_ERROR;

	while(suiv = (KMLSOUNDBDD *)actu->Node.Suivant)
	{
		KMLSOUND *courant;

		courant = actu->PtrKMLSound;
		while (courant != NULL)
		{
			if (flag & KML_SPAUSEALL)
			{
				KMLSoundSetAction(courant, KML_SPAUSE);
			}
			else if (flag & KML_SUNPAUSEALL)
			{
				if (courant->Flag & KML_SPAUSE)
					KMLSoundSetAction(courant, KML_SPLAY);
			}
			else if (flag & KML_SSTOPALL)
			{
				KMLSoundSetAction(courant, KML_SSTOP);
			}
			else if (flag & KML_SFADEINALL)
			{
				KMLSoundSetAction(courant, KML_SFADEIN,time);
			}
			else if (flag & KML_SFADEOUTALL)
			{
				KMLSoundSetAction(courant, KML_SFADEOUT,time);
			}

			if (courant->Flag & KML_SSTREAMING)
			{
				courant = NULL;
			}
			else courant = courant->Suivant;
		}

			actu = suiv;
	}

	return KML_OK;
}

char KMLSoundSetGlobalVolume (unsigned char Volume)//entre 0 et 1024
//***************************************************************
//	FUNCTION : KMLSoundSetGlobalVolume
//---------------------------------------------------------------
//	INPUT	 : Volume
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> jeudi 3 août 2000
//	REVISION : 
//***************************************************************
{
	if (Volume < 0 || Volume > 1024)
		return KML_SOUND_ERROR;
	KMLVolumeGeneral = Volume;
	return KML_OK;
}

long KMLGetVolume (long Volume)
//***************************************************************
//	FUNCTION : KMLGetVolume
//---------------------------------------------------------------
//	INPUT	 : Volume
//---------------------------------------------------------------
//	OUTPUT	 : le volume changé
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> jeudi 3 août 2000
//	REVISION : 
//***************************************************************
{
	return ( (Volume * KMLVolumeGeneral)  >> 10);
}

char KMLSoundGestionStream (KMLSOUND *PtrSound)
//***************************************************************
//	FUNCTION : KMLSoundGestionStream
//---------------------------------------------------------------
//	INPUT	 : PtrSound
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> vendredi 4 août 2000
//	REVISION : 
//***************************************************************
{
	unsigned long	PtrRead;
	unsigned long	EspaceLibre;
	KMLSTREAM		*StreamInfo;
	void			*Ptr1, *Ptr2;
	unsigned long	Size1, Size2 = 0;

	StreamInfo = (KMLSTREAM *) PtrSound->Suivant;
	
	if (PtrSound->SoundBuffer->GetCurrentPosition(&PtrRead, NULL) != DS_OK)
		return KML_SOUND_ERROR;
	
	if (StreamInfo->writeposition < 0)
	{
		if (PtrSound->Flag & KML_SSTREAMRESERVED)
		{
			if (PtrRead >= (unsigned long) (-StreamInfo->writeposition))
			{
				PtrSound->Flag &= ~KML_SSTREAMRESERVED;
				if (KMLSoundSetAction(PtrSound,KML_SSTOP) != KML_OK)
					return KML_SOUND_ERROR;
			}
		}
		else if (PtrRead <= (unsigned long) (-StreamInfo->writeposition))
			PtrSound->Flag |= KML_SSTREAMRESERVED;
		
		return KML_OK;
	}

	if ((long) PtrRead > StreamInfo->writeposition)
	{
		EspaceLibre = PtrRead - StreamInfo->writeposition;
	}
	else if ((long) PtrRead < StreamInfo->writeposition)
	{
		EspaceLibre = (StreamInfo->taillebuffer - StreamInfo->writeposition) + PtrRead;
	}
	else
	{
		return KML_OK;
	}

	if (EspaceLibre < TAILLE_MINI_TO_READ)
		return KML_OK;

	if (!(PtrSound->Flag & KML_SLOOP))
	{

		if (StreamInfo->tailledata - StreamInfo->ptrfichier <= EspaceLibre)
		{
			if (PtrSound->SoundBuffer->Lock(StreamInfo->writeposition, StreamInfo->tailledata - StreamInfo->ptrfichier , &Ptr1, &Size1, &Ptr2, &Size2, NULL) != DS_OK)
				return KML_SOUND_ERROR;

			KMLSoundLireStream(PtrSound, &Ptr1, Size1);
			StreamInfo->writeposition += Size1;
			if ((unsigned long) StreamInfo->writeposition >= StreamInfo->taillebuffer)
					StreamInfo->writeposition = 0;

			if (Size2 != 0)
			{
				KMLSoundLireStream(PtrSound, &Ptr2, Size2);
				StreamInfo->writeposition = Size2;
			}

			if (PtrSound->SoundBuffer->Unlock(Ptr1, Size1, Ptr2, Size2) != DS_OK)
				return KML_SOUND_ERROR;

			if (PtrRead < (unsigned long) StreamInfo->writeposition)
				PtrSound->Flag |= KML_SSTREAMRESERVED;
			StreamInfo->writeposition = -StreamInfo->writeposition;
			
			return KML_OK;
		}
		

	}
	if (PtrSound->SoundBuffer->Lock(StreamInfo->writeposition, EspaceLibre, &Ptr1, &Size1, &Ptr2, &Size2, NULL) != DS_OK)
		return KML_SOUND_ERROR;

	KMLSoundLireStream(PtrSound, &Ptr1, Size1);
	StreamInfo->writeposition += Size1;
	if ((unsigned long) StreamInfo->writeposition >= StreamInfo->taillebuffer)
		StreamInfo->writeposition = 0;
	
	if (Size2 != 0)
	{
		KMLSoundLireStream(PtrSound, &Ptr2, Size2);
		StreamInfo->writeposition = Size2;
	}

	if (PtrSound->SoundBuffer->Unlock(Ptr1, Size1, Ptr2, Size2) != DS_OK)
		return KML_SOUND_ERROR;

	return KML_OK;
}

void KMLSoundLireStream(KMLSOUND *PtrSound, void **Ptr, unsigned long Size)
//***************************************************************
//	FUNCTION : KMLSoundLireStream
//---------------------------------------------------------------
//	INPUT	 : PtrSound, PtrEcriture, Size
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> lundi 7 août 2000
//	REVISION : 
//***************************************************************
{
	KMLSTREAM *Stream;
	char *Pointeur;

	Pointeur = (char *) (*Ptr);
	Stream = (KMLSTREAM *) PtrSound->Suivant;

	if (Size > Stream->tailledata - Stream->ptrfichier)//alors si on lit "Stream->tailledata - Stream->ptrfichier" on arrive a la fin du fichier
	{
		fread(Pointeur, Stream->tailledata - Stream->ptrfichier, 1, *Stream->fichier);
		fseek(*Stream->fichier, - (long) (Stream->tailledata), SEEK_CUR);//on revient au debut du fichier
		Pointeur += Stream->tailledata - Stream->ptrfichier;
		fread(Pointeur, Size - (Stream->tailledata - Stream->ptrfichier), 1, *Stream->fichier);//on lit le reste
		Stream->ptrfichier	= Size - (Stream->tailledata - Stream->ptrfichier);
	}
	else
	{
		fread(Pointeur, Size, 1, *Stream->fichier);
		Stream->ptrfichier +=Size;
	}
}
	
void KMLSoundUpdateStream()
//***************************************************************
//	FUNCTION : KMLSoundUpdateStream
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mardi 8 août 2000
//	REVISION : 
//***************************************************************
{
	KMLSOUNDBDD *actu,*suiv;

	actu = (KMLSOUNDBDD *)KMLSoundBDDListe.First;
	
	if(actu == NULL)
		return; 

	while(suiv = (KMLSOUNDBDD *)actu->Node.Suivant)
	{
		KMLSOUND *courant;

		courant = actu->PtrKMLSound;
		if (courant->Flag & KML_SALWAYSHEAR && courant->Flag & KML_SSTREAMING)			
		{
			if  (!(courant->Flag & KML_SPAUSE))
				KMLSoundGestionStream(courant);
		}
		actu = suiv;
	}
}


/*	REVOIR LE SON STREAME QUI LOOPE PAS RAJOUTER DU BLANC...
	REVOIR LA RESTAURATION DU SON STREAME...					*/
