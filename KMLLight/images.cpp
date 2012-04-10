#include "kmllight.h"

long BMKMLLightMemory = 0; //Allocate memory

long	GetKMLLightMemory()
{
	return (BMKMLLightMemory);
}
	 

//*****************************************************************************GLOBALES

KMLLISTE			KMLListeImage;
short				GLOBAL_RES_X;
short				GLOBAL_RES_Y;
long				GLOBAL_PITCH;
char				GLOBAL_NB_COLOR;

unsigned char		TextR, TextG, TextB;
char				TextStyle;

//***************************************************************
//	FUNCTION : KMLStartLib
//---------------------------------------------------------------
//	INPUT	 : resolution
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLStartLib(HWND hWnd, HINSTANCE hInst, short x, short y, long flag, unsigned char bitcouleur, unsigned char r, unsigned char g, unsigned char b)
{
	// attributions globales
	GLOBAL_NB_COLOR	= bitcouleur;
	GLOBAL_RES_X	= x;
	GLOBAL_RES_Y	= y;
	GLOBAL_PITCH	= GLOBAL_NB_COLOR>>3 * GLOBAL_RES_X;
	hInstGlobal		= hInst;
	hMainWnd		= hWnd;

	TextR	= 255;
	TextG	= 255;
	TextB	= 255;

	TextStyle = KML_TEXT_DEFAULT;
	// reset des listes
	KMLNewList(&KMLListeImage);

	// active directx
	if(DDInitMode(x, y, (char)flag, GLOBAL_NB_COLOR, r, g, b)!= KML_OK)
		return KML_ERROR_DD;

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLQuitLib
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************
char KMLQuitLib(void)
{
	// delete image residuelles
	KMLIMAGE		*imaactu,*imasuiv;
	imaactu = (KMLIMAGE *)KMLListeImage.First;
	
	if(imaactu == NULL)
		return KML_NOELMT;

	while(imasuiv = (KMLIMAGE *)imaactu->Node.Suivant)
	{
		KMLDeleteImage(imaactu);
		imaactu = NULL;
		imaactu=imasuiv;
	}

	// fin directx
	DDQuit();

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLFinalDraw
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//	REVISION : MikE ->  mercredi 26 juillet 2000
//***************************************************************

char KMLFinalDraw(char flag)
{
	//  Affichage final
	long result;

	result = DDFinalDraw(flag);


	if(result != KML_OK)
	{
		if (result == DD_SURFACE_LOST)
		{
			KMLIMAGE		*imaactu,*imasuiv;

			imaactu = (KMLIMAGE *)KMLListeImage.First;

			if(imaactu == NULL)
				return KML_NOELMT;

			while(imasuiv = (KMLIMAGE *)imaactu->Node.Suivant)							//on fait peut etre cette boucle dans le cas d'un switch puisque taille de primary plus petite certaine surface qu'on voulait creer en mem video et qu'on avait pas pu le peuvent peut etre maintenant so reload surface , donc delete et create
			{
				DDReloadSurface(&imaactu->Surface, imaactu->DataPath, (imaactu->Flag & (KML_MEM_VIDEO_PREF | KML_MEM_SYSTEM | KML_MEM_VIDEO)) );
				if(imaactu->Flag & KML_CLEARCOLOR)
					KMLClearImage(imaactu, (BYTE)((imaactu->ClearColor)>>16), (BYTE)((imaactu->ClearColor)>>8), (BYTE)imaactu->ClearColor);

				if (imaactu->Fonction != NULL)
					imaactu->Fonction (imaactu, imaactu->Param);

				imaactu=imasuiv;
			}
	
			while(imasuiv = (KMLIMAGE *)imaactu->Node.Suivant)
			{
				if (DDRestore(imaactu->Surface) != KML_OK)
						return KML_ERROR_DD;


				if(imaactu->Flag & KML_CLEARCOLOR)
					KMLClearImage(imaactu, (BYTE)((imaactu->ClearColor)>>16), (BYTE)((imaactu->ClearColor)>>8), (BYTE)imaactu->ClearColor);
				if (imaactu->DataPath)
					if (DDLoadBMP(imaactu->Surface, imaactu->DataPath) != KML_OK)
						return KML_ERROR_DD;
				
				if (imaactu->Fonction != NULL)
					imaactu->Fonction (imaactu, imaactu->Param);
				imaactu=imasuiv;
			}
			
			return KML_OK;
		}
		return KML_ERROR_DD;
	}
	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLCreateImage
//---------------------------------------------------------------
//	INPUT	 : taille image
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

KMLIMAGE *	KMLCreateImage(long x, long y, DWORD flag, char *bitmap)
{
	KMLIMAGE				*i;
	KMLFILE					*f;
	char					car[18];


	// allocation et reset
	i = (KMLIMAGE *)ALLOC2(sizeof(KMLIMAGE),BMKMLLightMemory);
	if (i == NULL)
		return NULL;

	memset(i, 0, sizeof(KMLIMAGE));
	
	// set les proprietes
	i->Largeur	= x;
	i->Hauteur	= y;
	i->Pitch	= i->Largeur * (GLOBAL_NB_COLOR>>3);
	i->DataPath	= NULL;
	i->Flag		= flag;
	i->Fonction = NULL;

	// gere la bitmap
	if((bitmap) && (flag & KML_BMP))
	{
		f = KMLPackOpen(NULL, bitmap, NULL);
		if (f == NULL)
		{
			KMLWarning("KMLCreateImage ( )\nParam : char *bitmap defaillant :%s\n", bitmap);
			return NULL;
		}

		KMLPackRead(car, 18 * sizeof(char), f);
		KMLPackRead(&i->Largeur, sizeof(DWORD), f);
		KMLPackRead(&i->Hauteur, sizeof(DWORD), f);
		KMLPackClose(f);
	}

	//creation de l'image
	if (flag & KML_MEM_VIDEO_PREF)
		i->Surface	= DDCreateSurface(i->Largeur, i->Hauteur, DD_CREATE_VIDEO_PREF);
	else if (flag & KML_MEM_SYSTEM)
		i->Surface	= DDCreateSurface(i->Largeur, i->Hauteur, DD_CREATE_MEMORY);
	else 
		i->Surface	= DDCreateSurface(i->Largeur, i->Hauteur, DD_CREATE_VIDEO);

	if (i->Surface == NULL)
	{
		FREE2(i,BMKMLLightMemory);
		return NULL;
	}

	//set back color
	DDSetBackColor(i->Surface);
	KMLSetBackColor(i, KML_SET);

	// Load Bitmap
	if((bitmap)&&(flag&KML_BMP))
		KMLLoadBMP(i, bitmap);

	// Ajoue a liste principale
	KMLAddTail(&KMLListeImage, i->Node);

	return i;
}

//***************************************************************
//	FUNCTION : KMLDeleteImage
//---------------------------------------------------------------
//	INPUT	 : pointeur sur image
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLDeleteImage(KMLIMAGE *ima)
{
	if(ima==NULL)
		return KML_NOALLOC;

	// delete DD
	if(DDDeleteSurface(ima->Surface) != KML_OK)
		return KML_ERROR_DD;

	// delete image
	FREE2(ima->DataPath,BMKMLLightMemory);
	KMLRemoveNode(ima->Node);
	FREE2(ima,BMKMLLightMemory);

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLClearImage
//---------------------------------------------------------------
//	INPUT	 : pointeur image + couleur rgb
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLClearImage(KMLIMAGE *ima, unsigned char r, unsigned char g, unsigned char b)
{
	if(ima==NULL)
		return KML_NOALLOC;

	// set couleur
	if(DDClearSurface(ima->Surface, r, g, b) != KML_OK)
		return KML_ERROR_DD;
	
	KMLSetSurfaceType(ima, KML_CLEARCOLOR);
	ima->ClearColor = (r)<<16 | (g)<<8 | b;

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLLoadBMP
//---------------------------------------------------------------
//	INPUT	 : pointeur image + nom bitmap
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLLoadBMP(KMLIMAGE *ima, char *bitmap)
{
	KMLFILE					*f;
	char					car[18];
	unsigned long			x,y;

	if(ima == NULL)
		return KML_NOALLOC;

	if(bitmap)
	{
		if ((f=KMLPackOpen(NULL, bitmap, NULL))==NULL)
		{
			KMLWarning("KMLLoadBitmap ( )\nParam : char *bitmap defaillant :\n%s", bitmap);
			return KML_ERROR_DD;
		}

		KMLPackRead(car, 18*sizeof(char), f);
		KMLPackRead(&x, sizeof(DWORD), f);
		KMLPackRead(&y, sizeof(DWORD), f);
		KMLPackClose(f);

		if( (x!=ima->Largeur) || (y!=ima->Hauteur) )
		{
			KMLDeleteImage (ima);
			ima = KMLCreateImage( (short)x, (short)y, NULL, NULL);
			KMLLoadBMP( ima, bitmap);
			return KML_OK;
		}
	}
	else
	{
		KMLWarning("KMLLoadBitmap ( )\nParam : char *bitmap inexistant.\n");
		return KML_ERROR_DD;
	}


	if(DDLoadBMP(ima->Surface, bitmap) != KML_OK)
		return KML_ERROR_DD;

	if(ima->DataPath)
		FREE2(ima->DataPath,BMKMLLightMemory);
	ima->DataPath = (char *)ALLOC2(strlen(bitmap)+1,BMKMLLightMemory);
	strcpy(ima->DataPath, bitmap);
	KMLSetSurfaceType(ima, KML_BMP);

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLCopyImage
//---------------------------------------------------------------
//	INPUT	 : ima src + ima dest + couleur trans
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLCopyImage(KMLIMAGE *imadest, KMLIMAGE *imasrc)
{
	if( (imadest==NULL)||(imasrc==NULL) )
		return KML_NOALLOC;

	if(DDCopyImage(imadest->Surface, imasrc->Surface, imasrc) != KML_OK)
		return KML_ERROR_DD;

	if(imadest->DataPath)
		FREE2(imadest->DataPath,BMKMLLightMemory);
	imadest->DataPath = (char *)ALLOC2(strlen(imasrc->DataPath)+1,BMKMLLightMemory);
	strcpy(imadest->DataPath, imasrc->DataPath);

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLFastBlt
//---------------------------------------------------------------
//	INPUT	 : plein de trucs
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLFastBlt(KMLIMAGE *imadest, long xdest, long ydest, KMLIMAGE *imasrc, long xsrc, long ysrc, long largeursrc, long hauteursrc)
{
	RECT		clipping;
	KMLIMAGE	idest,isrc;
	
	if (imadest==NULL)
		idest.Surface=NULL;
	else idest=*imadest;

	if (imasrc==NULL)
		isrc.Surface=NULL;
	else isrc=*imasrc;
	
	/*if( (imadest==NULL)||(imasrc==NULL) )
		return KML_NOALLOC;*/
	
	clipping.left	= xsrc;
	clipping.right	= largeursrc;
	clipping.top	= ysrc;
	clipping.bottom	= hauteursrc;

	if( DDFastBlt(idest.Surface, (short)xdest, (short)ydest, isrc.Surface, &clipping, &isrc) != KML_OK)
		return KML_ERROR_DD;

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLStretchBlt
//---------------------------------------------------------------
//	INPUT	 : plein de trucs encore
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLStretchBlt(KMLIMAGE *imadest, long xdest, long ydest, long largeurdest, long hauteurdest, KMLIMAGE *imasrc, long xsrc, long ysrc, long largeursrc, long hauteursrc)
{
	RECT		  clipping;
	RECT		  stretch;

	if(imasrc==NULL)
		return KML_NOALLOC;

	

	stretch.left	= xdest;
	stretch.right	= largeurdest;
	stretch.top		= ydest;
	stretch.bottom	= hauteurdest;

	clipping.left	= xsrc;
	clipping.right	= largeursrc;
	clipping.top	= ysrc;
	clipping.bottom	= hauteursrc;

	if(imadest)
	{
		if( DDStretchBlt(imadest->Surface, &stretch, imasrc->Surface, &clipping, imasrc) != KML_OK )
			return KML_ERROR_DD;
	}
	else
	{
		if( DDStretchBlt(NULL, &stretch, imasrc->Surface, &clipping, imasrc) != KML_OK)
			return KML_ERROR_DD;
	}

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLSetBackColor
//---------------------------------------------------------------
//	INPUT	 : ima + coul
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 11 mai 2000
//***************************************************************

char KMLSetBackColor(KMLIMAGE *ima, DWORD flag)
{
	if(ima == NULL)
		return KML_NOALLOC;


	if(flag == KML_SET)
	{
		ima->Flag |= KML_TRANS;
	}
	else if(flag == KML_UNSET)
	{
		ima->Flag &= ~KML_TRANS;
	}
	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLSetAlpha
//---------------------------------------------------------------
//	INPUT	 : ima + flag + alpha
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 26 juin 2000
//***************************************************************

char KMLSetAlpha(KMLIMAGE *ima, DWORD flag, unsigned char alpha)
{
	if(ima==NULL)
		return KML_NOALLOC;

	if(flag == KML_SET)
	{
		ima->Flag |= KML_ALPHA;
		ima->Alpha	= alpha;
	}
	else if(flag == KML_UNSET)
	{
		ima->Flag &= ~KML_ALPHA;
	}
	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLSwitch
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 24 juillet 2000
//***************************************************************

char KMLSwitch(void)
{
	// switch video
	DDSwitch();

	// reload des surfaces
	KMLIMAGE		*imaactu,*imasuiv;
	
	imaactu = (KMLIMAGE *)KMLListeImage.First;
	
	if(imaactu == NULL)
		return KML_NOELMT;

	while(imasuiv = (KMLIMAGE *)imaactu->Node.Suivant)
	{
		DDReloadSurface(&imaactu->Surface, imaactu->DataPath, (imaactu->Flag & (KML_MEM_VIDEO_PREF | KML_MEM_SYSTEM | KML_MEM_VIDEO)) );
		if(imaactu->Flag & KML_CLEARCOLOR)
			KMLClearImage(imaactu, (BYTE)((imaactu->ClearColor)>>16), (BYTE)((imaactu->ClearColor)>>8), (BYTE)imaactu->ClearColor);

		if (imaactu->Fonction != NULL)
			imaactu->Fonction (imaactu, imaactu->Param);
		imaactu=imasuiv;
	}

	if (KMLSwitchFonction != NULL)				//on appelle la fonction callback si elle existe
		KMLSwitchFonction (KMLSwitchParameter);

	return KML_OK;
}


//***************************************************************
//	FUNCTION : KMLSetSurfaceType
//---------------------------------------------------------------
//	INPUT	 : image + type
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 24 juillet 2000
//***************************************************************

void KMLSetSurfaceType(KMLIMAGE *ima, long type)
{
	if(ima==NULL)
		return;

	switch(type)
	{
	case KML_BMP:
		ima->Flag &= ~KML_CLEARCOLOR;
		ima->Flag |= KML_BMP;
		break;

	case KML_CLEARCOLOR:
		ima->Flag &= ~KML_BMP;
		ima->Flag |= KML_CLEARCOLOR;
		break;
	}
}


//***************************************************************
//	FUNCTION : KMLLockSurface
//---------------------------------------------------------------
//	INPUT	 : image
//---------------------------------------------------------------
//	OUTPUT	 : long *
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 25 juillet 2000
//***************************************************************

void *KMLLockSurface(KMLIMAGE *ima)
{
	if(!ima)
		return NULL;

	return DDLock(ima->Surface);
}

//***************************************************************
//	FUNCTION : KMLDelockSurface
//---------------------------------------------------------------
//	INPUT	 : ima
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 25 juillet 2000
//***************************************************************

char KMLDelockSurface(KMLIMAGE *ima)
{
	if(!ima)
		return KML_NOALLOC;

	return DDUnLock(ima->Surface);
}

//***************************************************************
//	FUNCTION : KMLError
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 25 juillet 2000 (<- tu veux rire c'est moi l'auteur toi t'as changé le nom de la fonction)
//***************************************************************

void KMLError(const char *fmt,...)
{
	va_list		vlst;
	char		str[512];

	va_start(vlst,fmt);
	vsprintf(str,fmt,vlst);
	strcat(str, "\n\n Arret du programme !");
	
	MessageBox( hMainWnd, str, "KML Error", MB_OK | MB_ICONERROR);
	CLOSE_KML_LIB
	va_end(vlst);
	exit(-1);
}

//***************************************************************
//	FUNCTION : KMLWarning
//---------------------------------------------------------------
//	INPUT	 : chaine
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

void KMLWarning(const char *fmt,...)
{
	va_list		vlst;
	char		str[512];
	long		result;

	va_start(vlst,fmt);
	vsprintf(str,fmt,vlst);
	strcat(str, "\n\n Voulez vous continuer ?\n\nCancel pour Debugger");
	
	result = MessageBox( hMainWnd, str, "KML Warning", MB_YESNOCANCEL | MB_ICONWARNING);

	if (result == IDNO)
	{
		CLOSE_KML_LIB
		va_end(vlst);
		exit(-1);
	}
	else if(result == IDCANCEL)
	{
		_asm {int 3};
	}
	va_end(vlst);
}

//***************************************************************
//	FUNCTION : KMLSetTextColor
//---------------------------------------------------------------
//	INPUT	 : R, G, B
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mercredi 26 juillet 2000
//***************************************************************
void KMLSetTextColor (unsigned char r, unsigned char g, unsigned char b)
{
	TextR = r;
	TextG = g;
	TextB = b;
}

//***************************************************************
//	FUNCTION : KMLSetTextColor
//---------------------------------------------------------------
//	INPUT	 : R, G, B
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mercredi 26 juillet 2000
//***************************************************************
void KMLSetTextStyle (char style)
{
	TextStyle = style;
}


//***************************************************************
//	FUNCTION : KMLPrintf
//---------------------------------------------------------------
//	INPUT	 : pos + chaine
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 18 mai 2000
//	REVISION : MikE	->  mercredi 26 juillet 2000
//***************************************************************

void KMLPrintf(int x,int y,const char *fmt,...)
{
	va_list vlst;
	char	str[512];
	HDC		dc;
	
	DDGetBackBufferDC(&dc, KML_SET);


	va_start(vlst,fmt);
	vsprintf(str,fmt,vlst);

	SetBkMode( dc, TRANSPARENT );

	if (TextStyle == KML_TEXT_DETOURE)
	{
		if ((TextR < 127) && (TextB < 127) && (TextG < 127))
			SetTextColor(dc,RGB(255,255,255));
		else SetTextColor(dc,RGB(0,0,0));			
		TextOut(dc,x+1,y+1,str,strlen(str));
		TextOut(dc,x-1,y-1,str,strlen(str));
		TextOut(dc,x-1,y+1,str,strlen(str));
		TextOut(dc,x+1,y-1,str,strlen(str));

		TextOut(dc,x+1,y,str,strlen(str));
		TextOut(dc,x-1,y,str,strlen(str));
		TextOut(dc,x,y+1,str,strlen(str));
		TextOut(dc,x,y-1,str,strlen(str));
	}
	else if (TextStyle == KML_TEXT_3D)
	{
		unsigned char r1,g1,b1;
		if (TextR > 127)
			r1 = TextR / 2;
		else r1 = TextR + (255 - TextR) / 2;

		if (TextG > 127)
			g1 = TextG / 2;
		else g1 = TextG + (255 - TextG) / 2;

		if (TextB > 127)
			b1 = TextB / 2;
		else b1 = TextB + (255 - TextB) / 2;

		SetTextColor(dc,RGB(r1,g1,b1));
		TextOut(dc,x+1,y+1,str,strlen(str));
		TextOut(dc,x+2,y+2,str,strlen(str));
	}

	
	SetTextColor(dc,RGB(TextR,TextG,TextB));
	TextOut(dc,x,y,str,strlen(str));
	va_end(vlst);

	DDGetBackBufferDC(&dc, KML_UNSET);
}

//*********************************************************************
//* FUNCTION: KMLSetCallback
//*--------------------------------------------------------------------
//* DESCRIPT: set la callback pour l'image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : l'image, la fonction, le param associé
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    14/12/2000 14:56:40
//* REVISION:									
//*********************************************************************
char KMLSetCallback	(KMLIMAGE *ima, fon2 fonction, void *param)
{
	ima->Fonction	= fonction;
	ima->Param		= param;

	return KML_OK;
}

//*********************************************************************
//* FUNCTION: KMLSetParam
//*--------------------------------------------------------------------
//* DESCRIPT: set le param de l'image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : l'image, le param associé
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    14/12/2000 14:56:40
//* REVISION:									
//*********************************************************************
char KMLSetParam		(KMLIMAGE *ima, void *param)
{
	ima->Param		= param;

	return KML_OK;
}

//*********************************************************************
// Name	: KMLGetDllExportStruct
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								10/01/2001 12:32:00
//*********************************************************************

void KMLGetDllExportStruct(KMLDLLEXPORTSTRUCT *dllexport)
{
	if(!dllexport)
		return;
}
