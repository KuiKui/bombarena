#include "kmllight.h"


//*****************************************************************************GLOBALES

long			KMLPackNbFileOpen = 0;
char			KMLPackTempPath[KML_PACK_SIZEOF_FILENAME];
char			KMLPackTempPackName[KML_PACK_SIZEOF_FILENAME];
long			KMLPackScanFileIndex = 0;

//***************************************************************
//	FUNCTION : KMLPackGetPacknameFromTotalName
//---------------------------------------------------------------
//	INPUT	 : totalname + pack + file
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 24 août 2000
//***************************************************************

void KMLPackSetPacknameFromTotalName(char *total, char *pack, char *file)
{
	char	lou [2] = {KML_PACK_CAT_STRING, 0};

	if( (!total) || (!pack) || (!file) )
		return;

	// cherche le debut du nom de fichier
	strcpy(total, pack);
	strcat(total, lou);
	strcat(total, file);
}

//***************************************************************
//	FUNCTION : KMLPackGetPacknameFromTotalName
//---------------------------------------------------------------
//	INPUT	 : totalname + pack + file
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 24 août 2000
//***************************************************************

void KMLPackGetPacknameFromTotalName(char *total, char *pack, char *file)
{
	int		i;

	if( (!total) || (!pack) || (!file) )
		return;

	// cherche le debut du nom de fichier
	strcpy(pack, total);
	for(i=strlen(total); total[i]!=KML_PACK_CAT_STRING && i>=0; i--);
	strcpy(file, &total[i+1]);
	if(i<0)
		i=0;
	pack[i]=0;
}

//***************************************************************
//	FUNCTION : KMLPackOpen
//---------------------------------------------------------------
//	INPUT	 : name + file + flag
//---------------------------------------------------------------
//	OUTPUT	 : 
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

KMLFILE *KMLPackOpen(char *packname, char *filename, long flag)
{
	KMLFILE			*kmlfile=NULL;
	KMLFILE			filetmp;
	FILE			*f;
	unsigned long	i, nbfile;
	char			strtmp	[KML_PACK_SIZEOF_FILENAME];
	char			pack	[KML_PACK_SIZEOF_FILENAME];
	char			file	[KML_PACK_SIZEOF_FILENAME];
	char			name	[KML_PACK_SIZEOF_FILENAME];

	// init
	strtmp[0] = pack[0] = file[0] = NULL;

	strcpy (name, filename);
	//-- convert the path name
	i = 0;

	while (name[i] != '\0')
	{
		if (name[i] == '\\')
			name[i] = '/';
		i++;
	}

	// Manage concatenation
	KMLPackGetPacknameFromTotalName(name, pack, file);

	// test packname
	if(packname)
		strcpy(pack, packname);

	// CHECK SUR LE HD ****
	// a la racine !
	strcpy(strtmp, "./");
	strcat(strtmp, file);
	if( (kmlfile = KMLPackGetKMLFILE(strtmp)) != NULL)
		return kmlfile;
	
	// dans le chemin KMLPackTempPath
	strcpy(strtmp, KMLPackTempPath);
	strcat(strtmp, "/");
	strcat(strtmp, file);

	if( (kmlfile = KMLPackGetKMLFILE(strtmp)) != NULL)
		return kmlfile;


	// CHECK DANS LE PACK ****

	if( !pack[0] )
		return NULL;

	// ouverture du pack dans pack
	if( (f=fopen(pack, "rb")) != NULL )
	{
		// recup nb file dans pack
		fread(&nbfile, sizeof(nbfile), 1, f);

		// scan les files du pack pour trouver le bon
		for(i=0; i<nbfile; i++)
		{
			fread(&filetmp, sizeof(KMLFILE)-sizeof(filetmp.data), 1, f);

			if(strcmp(filetmp.filename, file))
			{
				// si different
				fseek(f, filetmp.sizeofdata, SEEK_CUR);
			}
			else
			{
				// on a trouve le bon file
				// allocation de la structure
				kmlfile	= (KMLFILE *)ALLOC2(sizeof(KMLFILE),BMKMLLightMemory);
				// size du buffer
				kmlfile->sizeofdata	= filetmp.sizeofdata;
				// allocation du buffer
				kmlfile->data		= (void *)ALLOC2(kmlfile->sizeofdata,BMKMLLightMemory);
				// lecture du buffer
				fread(kmlfile->data, kmlfile->sizeofdata, 1, f);
				// MAJ du nom de fichier
				strcpy(kmlfile->filename, filetmp.filename);
				// MAJ du path
				strcpy(kmlfile->path, filetmp.path);
				// reset de la position
				kmlfile->offset = 0;
				// incrementation du conteur
				KMLPackNbFileOpen++;
				// on s'barre
				fclose(f);
				return kmlfile;
			}
		}
		fclose(f);
	}

	// ouverture du pack dans KMLPackTempPackName
	if( (f=fopen(KMLPackTempPackName,"rb")) != NULL )
	{
		// recup nb file dans pack
		fread(&nbfile, sizeof(nbfile), 1, f);

		// scan les files du pack pour trouver le bon
		for(i=0; i<nbfile; i++)
		{
			fread(&filetmp, sizeof(KMLFILE)-sizeof(filetmp.data), 1, f);

			if(strcmp(filetmp.filename, file))
			{
				// si different
				fseek(f, filetmp.sizeofdata, SEEK_CUR);
			}
			else
			{
				// on a trouve le bon file
				// allocation de la structure
				kmlfile	= (KMLFILE *)ALLOC2(sizeof(KMLFILE),BMKMLLightMemory);
				// size du buffer
				kmlfile->sizeofdata	= filetmp.sizeofdata;
				// allocation du buffer
				kmlfile->data		= (void *)ALLOC2(kmlfile->sizeofdata,BMKMLLightMemory);
				// lecture du buffer
				fread(kmlfile->data, kmlfile->sizeofdata, 1, f);
				// MAJ du nom de fichier
				strcpy(kmlfile->filename, filetmp.filename);
				// MAJ du path
				strcpy(kmlfile->path, filetmp.path);
				// reset de la position
				kmlfile->offset = 0;
				// incrementation du conteur
				KMLPackNbFileOpen++;
				// on s'barre
				fclose(f);
				return kmlfile;
			}
		}
		fclose(f);
	}

	return kmlfile;
}

//***************************************************************
//	FUNCTION : KMLPackClose
//---------------------------------------------------------------
//	INPUT	 : KMLFILE
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

char KMLPackClose(KMLFILE *f)
{
	if(!f)
		return KML_ERROR_PACK;

	KMLPackNbFileOpen--;
	FREE2(f->data,BMKMLLightMemory);
	FREE2(f,BMKMLLightMemory);

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLPackRead
//---------------------------------------------------------------
//	INPUT	 : buf+size+file
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

char KMLPackRead(void *dest, unsigned long nboctet, KMLFILE *f)
{
	char eof = 0;

	if(!f)
		return KML_ERROR_PACK;

	if(nboctet > f->sizeofdata)
		nboctet = f->sizeofdata;

	if(nboctet+f->offset > f->sizeofdata)
	{
		nboctet = f->sizeofdata - f->offset;
		eof++;
	}	

	// remplissage a partir de f dans dest
	MoveMemory( dest, (void *)( (char *)(f->data) + f->offset ), nboctet);

	f->offset+=nboctet;

	if( eof )
		return KML_PACK_EOF;

	return KML_OK;
}

//*********************************************************************
// Name	: KMLPackEof
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/01/2001 15:49:02
//*********************************************************************

char KMLPackEof(KMLFILE *f)
{
	if( f->offset == f->sizeofdata )
		return TRUE;

	return FALSE;
}

//***************************************************************
//	FUNCTION : KMLPackSeek
//---------------------------------------------------------------
//	INPUT	 : file+size+flag
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

char KMLPackSeek(KMLFILE *f, unsigned long size, char flag)
{
	if(flag == KML_PACK_START)
	{
		f->offset = 0;
	}

	f->offset+=size;

	if(f->offset>f->sizeofdata)
	{
		f->offset = f->sizeofdata;
		return KML_ERROR_PACK;
	}

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLPackGetKMLFILE
//---------------------------------------------------------------
//	INPUT	 : file
//---------------------------------------------------------------
//	OUTPUT	 : KMLFILE
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 22 août 2000
//***************************************************************

KMLFILE *KMLPackGetKMLFILE(char *str)
{
	FILE			*filein;
	KMLFILE			*kmlf;
	int				i;
	char			path	[KML_PACK_SIZEOF_FILENAME];
	char			endname	[KML_PACK_SIZEOF_FILENAME];
	char			strtmp	[KML_PACK_SIZEOF_FILENAME];


	// clip
	if(!str)
		return NULL;

	// ouverture du fichier
	if( (filein=fopen(str,"rb")) == NULL )
		return NULL;

	strcpy(strtmp, str);
	strcpy(path, str);
	for(i=strlen(str); strtmp[i]!='/'; i--);
	strcpy(endname, &strtmp[i+1]);
	path[i]=0;
	
	//init
	kmlf				= (KMLFILE *)ALLOC2(sizeof(KMLFILE),BMKMLLightMemory);
	memset(kmlf, 0, sizeof(KMLFILE));
	kmlf->offset		= 0;
	strcpy(kmlf->filename,	endname);
	strcpy(kmlf->path,		path);

	// get file size
	fseek(filein, 0, SEEK_END);
	kmlf->sizeofdata	= (unsigned long)ftell(filein);

	// allocation
	kmlf->data			= (void *)ALLOC2(kmlf->sizeofdata,BMKMLLightMemory);

	//lecture et stockage
	fseek(filein, 0, SEEK_SET);
	fread(kmlf->data, kmlf->sizeofdata, 1, filein);
	
	//fermeture
	fclose(filein);

	return kmlf;
}

//***************************************************************
//	FUNCTION : KMLPackAddFile
//---------------------------------------------------------------
//	INPUT	 : packname + file
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  vendredi 4 août 2000
//***************************************************************

char KMLPackAddFile(char *packname, char *filename)
{
	FILE			*filein;
	FILE			*filepack;
	KMLFILE			kmlfileadd;
	unsigned long	nbfile;

	char			path[KML_PACK_SIZEOF_FILENAME];
	char			endname[KML_PACK_SIZEOF_FILENAME];
	char			strtmp[KML_PACK_SIZEOF_FILENAME];


	// partage du filename
	long i;

	strcpy(strtmp, filename);
	strcpy(path, filename);
	for(i=strlen(filename); strtmp[i]!='/'; i--);
	strcpy(endname, &strtmp[i+1]);
	path[i]=0;

	// ouverture du fichier
	if( (filein=fopen(filename,"rb")) == NULL )
	{
		KMLError("KMLPackAddFile ( ) :\n filename invalide :\n%d", filename);
	}

	// ouverture du pack
	if( (filepack=fopen(packname,"rb")) == NULL )
	{
		nbfile = 0;
	}
	else
	{
		fread(&nbfile, sizeof(nbfile), 1, filepack);
		fclose(filepack);
	}


	//init
	memset(&kmlfileadd, 0, sizeof(KMLFILE));
	kmlfileadd.offset		= 0;
	strcpy(kmlfileadd.filename, endname);
	strcpy(kmlfileadd.path, path);

	// get file size
	fseek(filein, 0, SEEK_END);
	kmlfileadd.sizeofdata	= (unsigned long)ftell(filein);

	// allocation
	kmlfileadd.data			= (void *)ALLOC2(kmlfileadd.sizeofdata,BMKMLLightMemory);

	//lecture et stockage
	fseek(filein, 0, SEEK_SET);
	fread(kmlfileadd.data, kmlfileadd.sizeofdata, 1, filein);

	// ouverture du pack a la fin
	if( (filepack=fopen(packname,"ab")) == NULL )
	{
		KMLError("KMLPackAddFile ( ) :\n packname invalide :\n%d", packname);
	}

	// si premiere insertion
	if(nbfile == 0)
	{
		fwrite(&nbfile, sizeof(nbfile), 1, filepack);
	}

	// insertion dans le pack -> je suis a la fin du pack
	fwrite(&kmlfileadd, sizeof(KMLFILE)-sizeof(kmlfileadd.data), 1, filepack);
	fwrite(kmlfileadd.data, kmlfileadd.sizeofdata, 1, filepack);
	fclose(filepack);	

	// insertion nouveau nb file
	if( (filepack=fopen(packname,"r+b")) == NULL )
	{
		KMLError("KMLPackAddFile ( ) :\n packname invalide :\n%d", packname);
	}
	fseek(filepack, 0, SEEK_SET);
	nbfile++;
	fwrite(&nbfile, sizeof(nbfile), 1, filepack);

	//fermeture des fichier
	fclose(filein);
	fclose(filepack);

	// free
	FREE2(kmlfileadd.data,BMKMLLightMemory);

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLPackGetFilename
//---------------------------------------------------------------
//	INPUT	 : packname + index + reponse (char *)
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 8 août 2000
//***************************************************************

char KMLPackGetFilename(char *packname, unsigned long index, char *filename, char flag)
{
	FILE			*f;
	unsigned long	nbfile, i;
	KMLFILE			filetmp;

	// ouverture du pack
	if( (f=fopen(packname,"rb")) == NULL )
	{
		KMLError("KMLPackOpen ( ) :\n packname invalide :\n%d", packname);
	}

	// recup nb file dans pack
	fread(&nbfile, sizeof(nbfile), 1, f);

	// test
	if(index >= nbfile)
	{
		strcpy(filename, "");
		fclose(f);
		return KML_ERROR_PACK;
	}

	// scan les files du pack pour trouver le bon
	for(i=0; i<index; i++)
	{
		fread(&filetmp, sizeof(KMLFILE)-sizeof(filetmp.data), 1, f);
		fseek(f, filetmp.sizeofdata, SEEK_CUR);
	}
	
	fread(&filetmp, sizeof(KMLFILE)-sizeof(filetmp.data), 1, f);
	
	if(flag)
	{
		strcpy(filename, filetmp.path);
		strcat(filename, "/");
		strcat(filename, filetmp.filename);
	}
	else
	{
		strcpy(filename, filetmp.filename);
	}

	fclose(f);

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLPackGetNbFile
//---------------------------------------------------------------
//	INPUT	 : packname
//---------------------------------------------------------------
//	OUTPUT	 : nb file
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 8 août 2000
//***************************************************************

unsigned long KMLPackGetNbFile(char *packname)
{
	FILE			*f;
	unsigned long	nbfile;

	// ouverture du pack
	if( (f=fopen(packname,"rb")) == NULL )
	{
		KMLError("KMLPackOpen ( ) :\n packname invalide :\n%d", packname);
	}

	// recup nb file dans pack
	fread(&nbfile, sizeof(nbfile), 1, f);

	return nbfile;
}

//***************************************************************
//	FUNCTION : KMLPackSetTempPath
//---------------------------------------------------------------
//	INPUT	 : temp path
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 23 août 2000
//***************************************************************

void KMLPackSetTempPath(char *path)
{
	if(!path)
		return;

	strcpy(KMLPackTempPath, path);	
}

//***************************************************************
//	FUNCTION : KMLPackGetTempPath
//---------------------------------------------------------------
//	INPUT	 : temp path vide
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 23 août 2000
//***************************************************************

void KMLPackGetTempPath(char *path)
{
	if(!path)
		return;

	strcpy(path, KMLPackTempPath);	
}

//***************************************************************
//	FUNCTION : KMLPackSetTempPackName
//---------------------------------------------------------------
//	INPUT	 : temp path
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 23 août 2000
//***************************************************************

void KMLPackSetTempPackName(char *name)
{
	if(!name)
		return;

	strcpy(KMLPackTempPackName, name);	
}

//***************************************************************
//	FUNCTION : KMLPackGetTempPackName
//---------------------------------------------------------------
//	INPUT	 : temp path vide
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 23 août 2000
//***************************************************************

void KMLPackGetTempPackName(char *name)
{
	if(!name)
		return;

	strcpy(name, KMLPackTempPackName);	
}


//**************************************************************
/** \fn     void KMLPackResetFindFile ( void )
 *
 *  \param  void 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 20/03/2001 14:42:08
 ***************************************************************/

void KMLPackResetFindFile ( void )
{
	KMLPackScanFileIndex = 0;
}

//**************************************************************
/** \fn     char KMLPackFindFile(char *filename, )
 *
 *  \param  *filename 
 *  \param            
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author KuiKui	                  \date 20/03/2001 13:53:23
 ***************************************************************/

char KMLPackFindFile(char *packname, char *filename, KMLFILEDESC *pfiledesc)
{
	long					nbfile, i;
	char					file[KML_PACK_SIZEOF_FILENAME];

	if ( (!packname) || (!filename) || (!pfiledesc) )
		return KML_PACK_NOFILE;

	nbfile = KMLPackGetNbFile ( packname );

	if ( nbfile < KMLPackScanFileIndex )
		return KML_PACK_NOFILE;

	for ( i = 0; i < nbfile; i++ )
	{
		if ( i >= KMLPackScanFileIndex )
		{
			KMLPackScanFileIndex++;
			KMLPackGetFilename( packname, i, file, FALSE);
			if ( ! strcmp ( file, filename ) )
			{
				strcpy ( pfiledesc->filename, file );
				return KML_OK;
			}
		}
	}
	KMLPackResetFindFile();
	return KML_PACK_EOF;
}


//***************************************************************
//	FUNCTION : KMLPackStartLib
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 23 août 2000
//***************************************************************

void KMLPackStartLib(void)
{
	strcpy(KMLPackTempPath, "");
	strcpy(KMLPackTempPackName, "base.kpk");
	KMLPackScanFileIndex = 0;
}