#include <stdlib.h> 
#include <string.h> 
#include "kmllight.h"

long DDGetColor		(unsigned char r, unsigned char g, unsigned char b);	
char LoadPixel		(DDSURFACEDESC ddsd, KMLFILE **f, long x, long y, char BitImage, bool bIsMask, unsigned long RedMask,unsigned long  GreenMask, unsigned long BlueMask);
char WritePixel		(DDSURFACEDESC ddsd, HANDLE hFile, long x, long y, char BitImage);
char DDBPPManager	(void);
char DDRestoreBPP	(void);
//*************************************************************** GLOBALES

LPDIRECTDRAW			lpDD;
LPDIRECTDRAWSURFACE		lpDDSPrimaire;
LPDIRECTDRAWSURFACE		lpDDSBackBuffer;
LPDIRECTDRAWSURFACE		lpDDSColor;

long					DD_Largeur;
long					DD_Hauteur;

char					NbreBitCouleur;

HWND					hMainWnd;
HINSTANCE				hInstGlobal;
RECT					RectDefaut;
char					IsAlpha;

long					LongueurSrc;
long					LongueurDest;
long					HauteurSrc;
long					HauteurDest;

bool					fenetre = false;

DWORD					KMLBackColor;

unsigned long			DDCurrentBPP;



//***************************************************************
//	FUNCTION : DDInitMode
//---------------------------------------------------------------
//	INPUT	 : largeur hauteur bit_depth flag
//---------------------------------------------------------------
//	OUTPUT	 : TRUE or FALSE
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  vendredi 5 mai 2000
//***************************************************************

char DDInitMode(int Largeur, int Hauteur, char flag, char ColorDepth, unsigned char r, unsigned char g, unsigned char b)
{
	HRESULT			ddrval;
	DDSURFACEDESC	ddsd;
	char			buf[256];//Traitement des erreurs;
	
	DWORD			FlagFen;


	SetRect(&RectDefaut, 0, 0, Largeur, Hauteur);
	
	NbreBitCouleur = ColorDepth;



	//Mise en place du flag pour l'alpha
	if (flag & DD_ENABLE_ALPHA)
		 IsAlpha = 1;
	else IsAlpha = 0;

	if (flag & DD_FENETRE)
	{
		fenetre = true;
		FlagFen = DDSCL_NORMAL;
		if (DDCurrentBPP != (unsigned) ColorDepth )
		{
			DDBPPManager();
		}

	}
	else
	{
		FlagFen = DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;
	}
	
	//Creation de l'objet DirectDraw
	ddrval = DirectDrawCreate(NULL, &lpDD, NULL);
	if (ddrval == DD_OK)
	{
		//Mise en place des proprietes d'objet Direct Draw

		ddrval = lpDD->SetCooperativeLevel(hMainWnd, FlagFen);
		if (ddrval == DD_OK)
		{
			DD_Largeur = Largeur;
			DD_Hauteur = Hauteur;

			//Mise en place du mode video
			if (!fenetre)
			{
				ddrval = lpDD->SetDisplayMode (Largeur, Hauteur, NbreBitCouleur);//allocation du mode video
				if (ddrval == DD_OK)
				{
					//Mise en place des proprietes du mode
					memset(&ddsd, 0, sizeof(ddsd));
					ddsd.dwSize			= sizeof(ddsd);
					ddsd.dwFlags		= DDSD_CAPS | DDSD_PITCH;
					ddsd.ddsCaps.dwCaps	= DDSCAPS_PRIMARYSURFACE;
					
					//Creation de la surface principale
					ddrval=lpDD->CreateSurface(&ddsd, &lpDDSPrimaire, NULL);
					if (ddrval == DD_OK) 
					{
						lpDDSBackBuffer = DDCreateSurface(Largeur, Hauteur, DD_CREATE_VIDEO);
						if (lpDDSBackBuffer != NULL)
						{
							lpDDSColor	= DDCreateSurface(4, 4, DD_CREATE_MEMORY);
							if (lpDDSColor != NULL)
							{
								KMLBackColor = DDGetColor(r, g, b);
								return DD_TRUE;
							}
						}
					}
				}
			}
			else
			{//mode fenetre
				
				memset(&ddsd, 0, sizeof(ddsd));
				ddsd.dwSize			= sizeof(ddsd);
				ddsd.dwFlags		= DDSD_CAPS | DDSD_PITCH;
				ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
				
				//Creation de la surface principale
				ddrval = lpDD->CreateSurface(&ddsd, &lpDDSPrimaire, NULL);
				if (ddrval == DD_OK) 
				{
					/*DDSURFACEDESC ddsd2;
					ddsd2.dwSize = sizeof(ddsd2);
					lpDDSPrimaire->GetSurfaceDesc(&ddsd2);*/
					
					lpDDSBackBuffer = DDCreateSurface(Largeur, Hauteur, DD_CREATE_VIDEO);
					if (lpDDSBackBuffer != NULL) 
					{
						lpDDSColor	= DDCreateSurface(4, 4, DD_CREATE_MEMORY);
						if (lpDDSColor != NULL)
						{
							KMLBackColor = DDGetColor(r, g, b);
							return DD_TRUE;
						}
					}
				}
			}
		}
    }
	
	wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
	MessageBox( hMainWnd, buf, "ERROR", MB_OK );
	
	return DD_ERROR;
}

//***************************************************************
//	FUNCTION : DDQuit
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : TRUE or FALSE
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 11 mai 2000
//***************************************************************
char DDQuit (void)
{
	DDRestoreBPP();
	if( lpDD != NULL )
    {
		if (lpDDSBackBuffer != NULL)
		{
		   lpDDSBackBuffer->Release();
           lpDDSBackBuffer = NULL;
        }
        
		if (lpDDSPrimaire != NULL)
        {
            lpDDSPrimaire->Release();
            lpDDSPrimaire = NULL;
        }
        lpDD->RestoreDisplayMode ();
		lpDD->Release();
        lpDD = NULL;
		return DD_TRUE;
	}
	return DD_ERROR;
}

//***************************************************************
//	FUNCTION : DDSwitch
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  lundi 24 juillet 2000
//***************************************************************
void DDSwitch ()
{
	DDSURFACEDESC ddsd;

	if (fenetre)
	{
		lpDD->SetCooperativeLevel(hMainWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		lpDD->SetDisplayMode (DD_Largeur, DD_Hauteur, NbreBitCouleur);
		fenetre = false;
	}
	else 
	{

		lpDD->SetCooperativeLevel(hMainWnd, DDSCL_NORMAL);
		lpDD->RestoreDisplayMode();
		
		DDSURFACEDESC ddsd2;
		ddsd2.dwSize = sizeof(ddsd2);
		lpDDSPrimaire->GetSurfaceDesc(&ddsd2);
		fenetre = true;
		if (ddsd2.ddpfPixelFormat.dwRGBBitCount != (unsigned long) NbreBitCouleur)
			DDBPPManager();
	}

	DDDeleteSurface(lpDDSBackBuffer);
	DDDeleteSurface(lpDDSPrimaire);

	memset(&ddsd,0,sizeof(ddsd));
	ddsd.dwSize			= sizeof(ddsd);
	ddsd.dwFlags		= DDSD_CAPS | DDSD_PITCH;
	ddsd.ddsCaps.dwCaps	= DDSCAPS_PRIMARYSURFACE;

	lpDD->CreateSurface(&ddsd, &lpDDSPrimaire, NULL);
	lpDDSBackBuffer = DDCreateSurface(DD_Largeur, DD_Hauteur, DD_CREATE_VIDEO);
}


//***************************************************************
//	FUNCTION : DDCreateSurface
//---------------------------------------------------------------
//	INPUT	 : Largeur Hauteur Flags
//---------------------------------------------------------------
//	OUTPUT	 : Pointeur sur la surface
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 11 mai 2000
//***************************************************************
LPDIRECTDRAWSURFACE	DDCreateSurface	(int x, int y, DWORD flags)
{
	DDSURFACEDESC ddsd; 
	HRESULT ddrval;
	LPDIRECTDRAWSURFACE	NewSurface;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH;
	//rajout du flag alpha si besoin
	if (flags & DD_WITH_ALPHA) ddsd.dwFlags |= /*DDSD_ALPHABITDEPTH |*/ DDSD_PIXELFORMAT;

	if (flags & (DD_CREATE_VIDEO | DD_CREATE_VIDEO_PREF)) 
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	
	if (flags & (DD_CREATE_MEMORY))						  
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	
	ddsd.dwWidth  = x;						   
	ddsd.dwHeight = y;

//*************************************************
// Ne marche pas pour l'instant
//*************************************************

	//revoir si des trucs a rajouter dans ddreloadsurface
	if (flags & DD_WITH_ALPHA)
	{
//		ddsd.dwAlphaBitDepth = 8;//8 bit pour l'alpha par défaut
/*		ddsd.ddpfPixelFormat.dwFlags = DDPF_ALPHAPIXELS | DDPF_RGB;
		
		ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;
		ddsd.ddpfPixelFormat.dwRBitMask	 	   = 0x00ff0000;
		ddsd.ddpfPixelFormat.dwGBitMask		   = 0x0000ff00;
		ddsd.ddpfPixelFormat.dwBBitMask		   = 0x000000ff;*/
	}
//*************************************************
	ddrval = lpDD->CreateSurface(&ddsd, &NewSurface, NULL);
	if (ddrval != DD_OK)
	{
		if (flags & DD_CREATE_VIDEO_PREF) 
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			ddrval = lpDD->CreateSurface(&ddsd, &NewSurface, NULL);
			if (ddrval != DD_OK)
				return NULL;
		}
		else return NULL;
	}
	return (NewSurface);
}

//***************************************************************
//	FUNCTION : DDDeleteSurface
//---------------------------------------------------------------
//	INPUT	 : LPDIRECTDRAWSURFACE 
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 11 mai 2000
//***************************************************************

char DDDeleteSurface (LPDIRECTDRAWSURFACE surface)
{
	if (surface != NULL)
	{
	   surface->Release();
       surface = NULL;
	   return (DD_TRUE);
	}
	return DD_ERROR;
}

//***************************************************************
//	FUNCTION : DDLoadBMP
//---------------------------------------------------------------
//	INPUT	 : Surface où charger fichier Bitmap
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 11 mai 2000
//  REVISION : MikE  ->  mardi 22 aout 2000
//***************************************************************

char DDLoadBMP (LPDIRECTDRAWSURFACE surface, char *bitmap)
{
	KMLFILE				*f;

	BITMAPFILEHEADER	BitmapFileHeader;
	BITMAPINFOHEADER	BitmapInfoHeader;
	
	DDSURFACEDESC		ddsd;
	DWORD				RedMask, GreenMask, BlueMask;
	bool				bIsMask;
	long				x, y;
	char				reste, temp[3];
	
	f = KMLPackOpen(NULL, bitmap, NULL);

	if (KMLPackRead( &BitmapFileHeader, sizeof(BitmapFileHeader), f) == KML_ERROR_PACK)
	{
		KMLWarning("Problème de lecture du fichier %s",bitmap);
		return DD_ERROR;
	}
	if (KMLPackRead( &BitmapInfoHeader, sizeof(BitmapInfoHeader),  f) == KML_ERROR_PACK)
	{
		KMLWarning("Problème de lecture du fichier %s",bitmap);
		return DD_ERROR;
	}

	if (BitmapFileHeader.bfType != 'B' + 'M' * 256)
	{
		KMLWarning("Fichier bitmap %s non valide",bitmap);
		return DD_ERROR;
	}

	if (BitmapInfoHeader.biBitCount < 16 || BitmapInfoHeader.biBitCount > 32)
	{
		KMLWarning("Fichier bitmap %s non valide (seuls les modes 16 24 et 32 bits sont gérés",bitmap);
		return DD_ERROR;
	}

	if (BitmapInfoHeader.biCompression & (BI_RLE8 | BI_RLE4 ))
	{
		KMLWarning("Fichier bitmap %s compressé non géré",bitmap);
		return DD_ERROR;
	}


	bIsMask = false;

	if (BitmapInfoHeader.biCompression & (BI_BITFIELDS))
	{
		KMLPackRead(   &RedMask,	sizeof(RedMask)	 ,  f);
		KMLPackRead( &GreenMask,	sizeof(GreenMask),  f);
		KMLPackRead(  &BlueMask,	sizeof(BlueMask) ,  f);
		bIsMask = true;

		//se positionner sur les datas
		KMLPackSeek( f,  BitmapFileHeader.bfOffBits, KML_PACK_START);
	}
	else
	{
		//se positionner sur les datas
		KMLPackSeek( f,  BitmapFileHeader.bfOffBits, KML_PACK_START);

		if (BitmapInfoHeader.biBitCount == 16)
		{
			RedMask		= 0x7c00;
			GreenMask	= 0x03e0;
			BlueMask	= 0x001f;
			bIsMask = true;
		}
		else if (BitmapInfoHeader.biBitCount == 32)
		{
			RedMask		= 0x00ff0000;
			GreenMask	= 0x0000ff00;
			BlueMask	= 0x000000ff;
			bIsMask = true;
		}
	}

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PITCH | DDSD_LPSURFACE;
	long compt = 0;
	while (surface->Lock(NULL, &ddsd, DDLOCK_WRITEONLY, NULL) == DDERR_WASSTILLDRAWING) ;

	reste = BitmapInfoHeader.biWidth % 4;

	if (BitmapInfoHeader.biHeight < 0)
	{
		for (y = 0; y < -BitmapInfoHeader.biHeight; y++)
		{
			for (x = 0; x < BitmapInfoHeader.biWidth; x++)
			{
				LoadPixel(ddsd, &f, x, y, (char) BitmapInfoHeader.biBitCount, bIsMask, RedMask, GreenMask, BlueMask);
			}

			if (reste != 0)
				KMLPackRead (&temp, reste, f);
		}
	}
	else
	{
		for (y = BitmapInfoHeader.biHeight - 1; y >= 0; y--)
		{
			for (x = 0; x < BitmapInfoHeader.biWidth; x++)
			{
				LoadPixel(ddsd, &f, x, y, (char) BitmapInfoHeader.biBitCount, bIsMask, RedMask, GreenMask, BlueMask);
			}

			if (reste != 0)
				KMLPackRead (&temp, reste, f);
		}
	}

	surface->Unlock(NULL);
	KMLPackClose(f);
	return DD_TRUE;
}

//*********************************************************************
//* FUNCTION: DDSaveBMP
//*--------------------------------------------------------------------
//* DESCRIPT: sauve en bitmap la surface specifié
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : la surface, le nom de fichier, la hauteur, la longueur
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    12/12/2000 17:09:24
//* REVISION:									
//*********************************************************************
char DDSaveBMP (LPDIRECTDRAWSURFACE surface, char *pszFichier, long hauteur, long largeur)
{
	HANDLE				hFile;
	BITMAPFILEHEADER	bitmapheader;
	BITMAPINFOHEADER	bif;
	DDSURFACEDESC		ddsd;
	long				reste;
	char				temp[3];
	long				x,
						y;
	unsigned long		nbr;

	bitmapheader.bfOffBits	= sizeof(bitmapheader) + sizeof(bif);
	bitmapheader.bfSize		= sizeof(bitmapheader) + sizeof(bif) + ((largeur * 3) +  ((largeur * 3) %4)) * hauteur;
	bitmapheader.bfType		= (unsigned short)('M' << 8) + 'B';

	bif.biBitCount			= 24;
	bif.biClrImportant		= 0;
	bif.biClrUsed			= 0;
	bif.biCompression		= BI_RGB;
	bif.biHeight			= hauteur;
	bif.biPlanes			= 1;
	bif.biSize				= sizeof(bif);
	bif.biSizeImage			= 0;
	bif.biWidth				= largeur;
	bif.biXPelsPerMeter		= 0;
	bif.biYPelsPerMeter		= 0;

	hFile = CreateFile(pszFichier, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	WriteFile (hFile, &bitmapheader, sizeof(bitmapheader), &nbr, NULL);
	WriteFile (hFile, &bif, sizeof(bif), &nbr, NULL);
	//attention on doit ecrire la bitmap de bas en haut
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PITCH | DDSD_LPSURFACE;

	while (surface->Lock(NULL, &ddsd, DDLOCK_WRITEONLY, NULL) == DDERR_WASSTILLDRAWING) ;

	reste = bif.biWidth % 4;
	ZeroMemory (&temp, 3);

	for (y = bif.biHeight - 1; y >= 0; y--)
	{
		for (x = 0; x < bif.biWidth; x++)
		{
			WritePixel(ddsd, hFile, x, y, (char) bif.biBitCount);
		}

		if (reste != 0)
		{
			unsigned long nbre;
			WriteFile (hFile, &temp, reste, &nbre, NULL);
		}
	}
	surface->Unlock(NULL);

	CloseHandle(hFile);

	return DD_OK;
}

//***************************************************************
//	FUNCTION : DDClearSurface
//---------------------------------------------------------------
//	INPUT	 : LPDIRECTDRAWSURFACE,	R,G,B
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 11 mai 2000
//***************************************************************
char DDClearSurface	(LPDIRECTDRAWSURFACE surface, unsigned char r, unsigned char g, unsigned char b)
{
	DDBLTFX	ddbltfx;
	
	ddbltfx.dwSize		= sizeof(ddbltfx);
	if (NbreBitCouleur > 16 && NbreBitCouleur <= 32)
		ddbltfx.dwFillColor = DWORD (r << 16) + DWORD (g << 8) + b;//Attention dépendant de BitDepth (ici 24 ou 32)
	else
	{//alors nbre bit couleur = 16
		ddbltfx.dwFillColor = unsigned short ( ((r >>3) << 11) + ((g >> 2) << 5) + (b >> 3));
	}
	
	if (surface == NULL)
	{
		if (lpDDSBackBuffer->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx) == DD_OK) 
			return DD_TRUE;
	}
	else
	{
		if (surface->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx) == DD_OK) 
			return DD_TRUE;
	}
	return DD_ERROR_CLEARSURFACE;
}

//***************************************************************
//	FUNCTION : DDSetBackColor
//---------------------------------------------------------------
//	INPUT	 : LPDIRECTDRAWSURFACE
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  lundi 15 mai 2000
//***************************************************************
char DDSetBackColor (LPDIRECTDRAWSURFACE surface)
{
	DDCOLORKEY  ddck;

	ddck.dwColorSpaceLowValue  = KMLBackColor;
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    if (surface->SetColorKey(DDCKEY_SRCBLT, &ddck) == DD_OK) 
		return DD_TRUE;
	return DD_ERROR_SETBACKCOLOR;
}


//***************************************************************
//	FUNCTION : DDCopyImage
//---------------------------------------------------------------
//	INPUT	 : Surface dest, Surface src, PtrImage
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  lundi 15 mai 2000
//***************************************************************
char DDCopyImage (LPDIRECTDRAWSURFACE surfacedest, LPDIRECTDRAWSURFACE surfacesrc, KMLIMAGE *PtrImage)
{
	if (surfacedest == NULL) 
		surfacedest = lpDDSBackBuffer;

	if (surfacesrc == NULL) 
		surfacesrc = lpDDSBackBuffer;

	if (PtrImage->Flag & DD_TRANSPARENCE)
	{
		if (surfacedest->Blt(NULL, surfacesrc, NULL, DDBLT_KEYSRC|DDBLT_WAIT, NULL) == DD_OK) 
			return DD_TRUE;
		else return DD_ERROR_COPYIMAGET;
	}
	else 
	{
		if (surfacedest->Blt(NULL, surfacesrc, NULL, DDBLT_WAIT, NULL) == DD_OK) 
			return DD_TRUE;
		else return DD_ERROR_COPYIMAGE;
	}
//***************************************************
// Implementer l'Alpha des que ca marche ? Oui ou Non
//***************************************************

}
	
//***************************************************************
//	FUNCTION : DDFastBlt
//---------------------------------------------------------------
//	INPUT	 : surfacedest, xdest, ydest, surfacesrc, *src, PtrImage
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mardi 16 mai 2000
//***************************************************************
char DDFastBlt (LPDIRECTDRAWSURFACE surfacedest, short xdest, short ydest, LPDIRECTDRAWSURFACE surfacesrc, RECT *src, KMLIMAGE *PtrImage)
{
	if (surfacesrc == NULL) 
		surfacesrc = lpDDSBackBuffer;

	if (surfacedest == NULL) 
		surfacedest = lpDDSBackBuffer;

	if (xdest + src->right > RectDefaut.right)
	{
		src->right -= xdest + src->right - RectDefaut.right;
	}
	if (xdest < 0) 
	{
		src->left  -= xdest;
		src->right += xdest;
		xdest = 0;
	}
	
	if (ydest + src->bottom > RectDefaut.bottom)
	{
		src->bottom -= ydest + src->bottom - RectDefaut.bottom;
	}
	if (ydest < 0)
	{
		src->top    -= ydest;
		src->bottom += ydest;
		ydest = 0;
	}
	
//*************************************************
// Ne marche pas pour l'instant
//*************************************************
	if (PtrImage->Flag & DD_WITH_ALPHA)
	{
		DDSURFACEDESC ddsd;
		long *PtrSurfaceTemp;
		long *PtrSurface;
		long longueur,hauteur;
		long difference;

		memset((void *)&ddsd, 0, sizeof(ddsd));

		
		PtrSurfaceTemp = (long *) ALLOC2 (src->bottom * src->right * sizeof(long),BMKMLLightMemory);
		
		longueur = src->right;
		hauteur  = src->bottom;

		src->right  += src->left; 
		src->bottom += src->top;  
	 
 	    ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags |= DDSD_LPSURFACE | DDSD_PITCH;

		if (surfacesrc->Lock(src, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT, NULL) != DD_OK)
			exit(-1);
		
		surfacesrc->GetSurfaceDesc(&ddsd);

		PtrSurface = (long *)ddsd.lpSurface;

		//copie de la surface dans la surface temp
		for (int i = 0; i < hauteur; i++)
		{
			memcpy(&PtrSurfaceTemp[i * longueur], PtrSurface, longueur);
			PtrSurface += ddsd.lPitch;
			//PtrSurfaceTemp[i] = PtrSurface[i];
		}

		surfacesrc->Unlock(src);

		difference = src->left - xdest;

		src->left  -= difference;
		src->right -= difference;

		difference = src->top - ydest;

		src->top    -= difference;
		src->bottom -= difference;

		memset((void *)&ddsd, 0, sizeof(ddsd));

		ddsd.dwSize		 = sizeof(ddsd);
		ddsd.dwFlags	|= DDSD_LPSURFACE | DDSD_PITCH ;

		if (surfacedest->Lock(src, &ddsd, DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT ,NULL) != DD_OK)
			exit (-1);

		surfacedest->GetSurfaceDesc(&ddsd);

		PtrSurface = (long *)ddsd.lpSurface;
		//blit//REVOIR ICI
		for (i = 0; i < longueur * hauteur; i++)
		{
			unsigned long CouleurSrc, CouleurDest;
			unsigned char RedSrc  , RedDest,
						  BlueSrc , BlueDest,
						  GreenSrc, GreenDest;


			CouleurSrc  = PtrSurfaceTemp[i];
			CouleurDest = PtrSurface[i];

			if (CouleurSrc != KMLBackColor)
			{
				if (CouleurDest != KMLBackColor)
				{
					//Decomposition des couleurs
					
					
					BlueSrc  = (unsigned char) (CouleurSrc % 256);
					CouleurSrc >>= 8;
					GreenSrc = (unsigned char) (CouleurSrc % 256);
					CouleurSrc >>= 8;
					RedSrc   = (unsigned char) (CouleurSrc % 256);

					BlueDest  = (unsigned char) (CouleurDest % 256);
					CouleurSrc >>= 8;
					GreenDest = (unsigned char) (CouleurDest % 256);
					CouleurSrc >>= 8;
					RedDest   = (unsigned char) (CouleurDest % 256);
					
					CouleurDest = 0;
					
					CouleurDest &= (RedDest + (((RedSrc - RedDest) * PtrImage->Alpha) >> 8) );
					CouleurDest <<= 8;
					CouleurDest &= (GreenDest + (((GreenSrc - GreenDest) * PtrImage->Alpha) >> 8) );
					CouleurDest <<= 8;
					CouleurDest &= (BlueDest + (((BlueSrc - BlueDest) * PtrImage->Alpha) >> 8) );

					PtrSurface[i] = CouleurDest;

				}
				else
				{
					PtrSurface[i] = PtrSurfaceTemp[i];
				}

			}
		}
		surfacesrc->Unlock(&src);
		FREE2(PtrSurfaceTemp,BMKMLLightMemory);
		return DD_TRUE;
	}
//*************************************************
	src->right  += src->left;
	src->bottom += src->top;


	if (PtrImage->Flag & DD_TRANSPARENCE)
	{
		if (surfacedest->BltFast(xdest, ydest, surfacesrc, src, DDBLTFAST_WAIT|DDBLTFAST_SRCCOLORKEY) == DD_OK) 
			return DD_TRUE;
		return DD_ERROR_FASTBLTTRANS;
	}
	if (surfacedest->BltFast(xdest, ydest, surfacesrc, src, DDBLTFAST_WAIT|DDBLTFAST_NOCOLORKEY) == DD_OK)
		return DD_TRUE; 
	return DD_ERROR_FASTBLTNOTRANS;
}

//***************************************************************
//	FUNCTION : DDStretchBlt
//---------------------------------------------------------------
//	INPUT	 : surfacedest, *dest, surfacesrc, *src, PtrImage
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mardi 16 mai 2000
//***************************************************************
char DDStretchBlt (LPDIRECTDRAWSURFACE surfacedest, RECT *dest, LPDIRECTDRAWSURFACE surfacesrc, RECT *src, KMLIMAGE *PtrImage)
{
	if (surfacedest == NULL) 
		surfacedest = lpDDSBackBuffer;
	
	if (surfacesrc  == NULL) 
		surfacesrc  = lpDDSBackBuffer;

	LongueurSrc = src->right;
	HauteurSrc  = src->bottom;
	src->right  += src->left;
	src->bottom += src->top;
	
	if (dest != NULL)
	{
		LongueurDest = dest->right;
		HauteurDest	 = dest->bottom;

		dest->right  += dest->left;
		dest->bottom += dest->top;

		if (dest->left < 0)
		{
			src->left   -= dest->left * LongueurSrc / LongueurDest;
			dest->left  = 0;
		}
		
		if (dest->right > RectDefaut.right)
		{
			src->right -= (dest->right - RectDefaut.right) * LongueurSrc / LongueurDest;
			dest->right = RectDefaut.right;
		}
		
		
		if (dest->top < 0)
		{
			src->top   -= dest->top * HauteurSrc / HauteurDest;
			dest->top  = 0;
		}

		if (dest->bottom > RectDefaut.bottom)
		{
			src->bottom -= (dest->bottom - RectDefaut.bottom) * HauteurSrc / (HauteurDest);
			dest->bottom = RectDefaut.bottom;
		}
		
	}


	if (PtrImage->Flag & DD_TRANSPARENCE)
	{
		if (surfacedest->Blt(dest, surfacesrc, src, DDBLT_KEYSRC|DDBLT_WAIT,NULL) == DD_OK)
			return DD_TRUE;
		return DD_ERROR_STRETCHTRANS;
	}

	if (surfacedest->Blt(dest, surfacesrc, src, DDBLT_WAIT, NULL) == DD_OK) 
		return DD_TRUE;
	return DD_ERROR_STRETCHNOTRANS;

//*************************************************
// Implementer l'Alpha des que ca marche
//*************************************************

}

//***************************************************************
//	FUNCTION : DDFinalDraw
//---------------------------------------------------------------
//	INPUT	 : flag=DD_CLEAR efface le back buffer apres le blit
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mardi 16 mai 2000
//***************************************************************
char DDFinalDraw (DWORD flag)
{
	if (fenetre)
	{
		RECT rect;
		POINT point;
		point.x = 0;
		point.y = 0;
		GetClientRect(hMainWnd,&rect);
		ClientToScreen(hMainWnd,&point);
		rect.top    += point.y;
		rect.bottom += point.y;
		rect.right	+= point.x;
		rect.left	+= point.x;
		if (lpDDSPrimaire->Blt(&rect, lpDDSBackBuffer, NULL, DDBLT_WAIT, NULL) == DDERR_SURFACELOST)
		{
			if (DDRestore(lpDDSPrimaire) == DD_ERROR) 
				return DD_CANT_RESTORE;
			if (DDRestore(lpDDSBackBuffer) == DD_ERROR) 
				return DD_CANT_RESTORE;
			return DD_SURFACE_LOST;
		}
		return (DDClearSurface(NULL, 0, 0, 0));
	}
	else
	{
		if (lpDDSPrimaire->Blt(NULL, lpDDSBackBuffer, NULL, DDBLT_WAIT, NULL) == DDERR_SURFACELOST)
		{
			if (DDRestore(lpDDSPrimaire) == DD_ERROR)
				return DD_CANT_RESTORE;
			if (DDRestore(lpDDSBackBuffer) == DD_ERROR) 
				return DD_CANT_RESTORE;
			
			return DD_SURFACE_LOST;
		}

		if (flag && DD_CLEAR)//pour l'instant que le bltfast ne marche pas... A revoir plus tard, le bltfast a l'air de fonctionner maintenant
			return (DDClearSurface(NULL, 0, 0, 0));
	}
	return DD_TRUE;
}

//***************************************************************
//	FUNCTION : DDLock
//---------------------------------------------------------------
//	INPUT	 : Surface
//---------------------------------------------------------------
//	OUTPUT	 : void *
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  mardi 25 juillet 2000
//***************************************************************
void *DDLock(LPDIRECTDRAWSURFACE surface)
{
	DDSURFACEDESC	ddsd;
	HRESULT			result;

	ddsd.dwSize = sizeof(ddsd);
	do
	{
		result = surface->Lock(NULL, &ddsd, 0, NULL);
	}
	while (result == DDERR_WASSTILLDRAWING);

	if (result != DD_OK)
		return NULL;
	
	return (ddsd.lpSurface);
}

//***************************************************************
//	FUNCTION : DDUnLock
//---------------------------------------------------------------
//	INPUT	 : Surface
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 25 juillet 2000
//*************************************************************** 
char DDUnLock (LPDIRECTDRAWSURFACE surface)
{
	HRESULT result;
  
	result = surface->Unlock(NULL);

	if (result == DD_OK)
	{
		return DD_TRUE;
	}
	return DD_ERROR;
}

//***************************************************************
//	FUNCTION : DDRestore
//---------------------------------------------------------------
//	INPUT	 : LPDirectDrawSurface
//---------------------------------------------------------------
//	OUTPUT	 : TRUE or FALSE
//---------------------------------------------------------------
//	AUTHOR	 : MikE  ->  jeudi 25 mai 2000
//***************************************************************
char DDRestore(LPDIRECTDRAWSURFACE surface)
{
	if (surface->Restore() == DD_OK) 
		return DD_TRUE;
	return DD_ERROR;
}

//***************************************************************
//	FUNCTION : DDReloadSurface
//---------------------------------------------------------------
//	INPUT	 : surface, chemin de l'image
//---------------------------------------------------------------
//	OUTPUT	 : return code
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 24 juillet 2000
//***************************************************************
char DDReloadSurface (LPDIRECTDRAWSURFACE *surface, char *path, DWORD flag)
{
	DDSURFACEDESC	ddsd;
	
	ddsd.dwSize = sizeof(ddsd);

	(*surface)->GetSurfaceDesc (&ddsd);

	DDDeleteSurface(*surface);

	//re set les flag de la surface
	if (flag & (DD_CREATE_VIDEO | DD_CREATE_VIDEO_PREF)) 
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	
	if (flag & (DD_CREATE_MEMORY))						  
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	//essaie de creer la surface
	if (lpDD->CreateSurface(&ddsd, surface, NULL) != DD_OK)
	{
		if (flag & DD_CREATE_VIDEO_PREF) 
		{
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (lpDD->CreateSurface(&ddsd, surface, NULL) != DD_OK)
				return (DD_ERROR);
		}
		else return (DD_ERROR);
	}
	
	//load l'image si besoin
	if (path != NULL)
	{
		if (DDLoadBMP(*surface, path) != DD_TRUE)
			return DD_ERROR;
	}
	
	return DD_TRUE;
}

long DDGetColor(unsigned char r, unsigned char g, unsigned char b)
//***************************************************************
//	FUNCTION : DDGetColor
//---------------------------------------------------------------
//	INPUT	 : R, G, B
//---------------------------------------------------------------
//	OUTPUT	 : couleur
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> jeudi 17 août 2000
//	REVISION : 
//***************************************************************
{
	if (NbreBitCouleur == 32 || NbreBitCouleur == 24)
		return ((r << 16) + (g << 8) + b);

	else if (NbreBitCouleur == 16)
		return (((r >> 3)  << 11) + ((g >> 2) << 5) + (b >> 3));
	
	return 0;//impossible en théorie
}

//***************************************************************
//	FUNCTION : DDGetBackBufferDC
//---------------------------------------------------------------
//	INPUT	 : dc *
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 25 juillet 2000
//***************************************************************

void DDGetBackBufferDC(HDC *dc, long flag)
{
	if(flag == KML_SET)
	{
		lpDDSBackBuffer->GetDC(dc);
	}
	else if(flag == KML_UNSET)
		lpDDSBackBuffer->ReleaseDC(*dc);
}	

//***************************************************************
//	FUNCTION : DDBPPManager
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 2 août 2000
//***************************************************************

char DDBPPManager(void)
{
	DEVMODE			CurrentSettings;
	long			retour;

	DDCurrentBPP = NbreBitCouleur;

	if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &CurrentSettings))
		return KML_ERROR_DD;

	DDCurrentBPP = CurrentSettings.dmBitsPerPel;

	if(CurrentSettings.dmBitsPerPel != (unsigned) NbreBitCouleur)
	{
		CurrentSettings.dmBitsPerPel= NbreBitCouleur;
		CurrentSettings.dmFields	= DM_BITSPERPEL;
		retour = ChangeDisplaySettings(&CurrentSettings, CDS_UPDATEREGISTRY);
		
		if( (retour == DISP_CHANGE_FAILED) || (retour == DISP_CHANGE_BADMODE) || (retour == DISP_CHANGE_NOTUPDATED) )
		{
			MessageBox(NULL, "Il faut changer de carte video.", "KML DDBPPManager", NULL);
			CLOSE_KML_LIB
			exit(-1);
		}

		if(retour == DISP_CHANGE_RESTART)
		{
			MessageBox(NULL, "Il faut redemarrer le PC.", "KML DDBPPManager", NULL);
			CLOSE_KML_LIB
			exit(-1);
		}

		if(retour != DISP_CHANGE_SUCCESSFUL)
		{
			MessageBox(NULL, "Drole d'erreur !!!", "KML DDBPPManager", NULL);
			CLOSE_KML_LIB
			exit(-1);
		}
	}
	return KML_OK;
}

//***************************************************************
//	FUNCTION : DDRestoreBPP
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 2 août 2000
//***************************************************************

char DDRestoreBPP(void)
{
	DEVMODE			CurrentSettings;
	long			retour;


	if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &CurrentSettings))
		return KML_ERROR_DD;

	if(CurrentSettings.dmBitsPerPel == DDCurrentBPP)
		return KML_OK;
	
	CurrentSettings.dmBitsPerPel = DDCurrentBPP;
	CurrentSettings.dmFields	 = DM_BITSPERPEL;

	retour = ChangeDisplaySettings(&CurrentSettings, CDS_UPDATEREGISTRY);
	
	if(retour != DISP_CHANGE_SUCCESSFUL)
	{
		MessageBox(NULL, "Restoration du BPP impossible.", "KML DDBPPManager", NULL);
		CLOSE_KML_LIB
		exit(-1);
	}

	return KML_OK;
}

char LoadPixel (DDSURFACEDESC ddsd, KMLFILE **f, long x, long y, char BitImage, bool bIsMask, DWORD RedMask,DWORD GreenMask, DWORD BlueMask)
//***************************************************************
//	FUNCTION : LoadPixel
//---------------------------------------------------------------
//	INPUT	 : ddsd, file ** x y bitimage, ismask, rgbmask
//---------------------------------------------------------------
//	OUTPUT	 : TRUE or FALSE
//---------------------------------------------------------------
//	AUTHOR	 : MikE -> mercredi 23 août 2000
//	REVISION : 
//***************************************************************
{
	unsigned char r, g, b;
	DWORD couleur = 0;


	if (BitImage == 16)
	{
		unsigned short value;

		KMLPackRead( &value, sizeof(value), *f);
		r = (unsigned char) (((value & RedMask) << 3) >> 16);

		if (GreenMask == 0x03fe)
			g = (unsigned char) (((value & GreenMask) << 2) >> 8);
		else g = (unsigned char) (((value & GreenMask) << 3) >> 8);

		b = (unsigned char) ((value & BlueMask) << 3);

		couleur = (((DWORD) r) << 16) + (((DWORD) g) << 8) + b;
	}
	else if (BitImage == 24)
	{
		KMLPackRead( &b, 1,  *f);
		KMLPackRead( &g, 1,  *f);
		KMLPackRead( &r, 1,  *f);
		
		couleur = (r << 16) + (g << 8) + b;
	}
	else if (BitImage == 32)
	{
		KMLPackRead( &b, 1,  *f);
		KMLPackRead( &g, 1,  *f);
		KMLPackRead( &r, 1,  *f);
		KMLPackRead( &couleur, 1,  *f);//on lit l'octet d'alpha

		couleur = (r << 16) + (g << 8) + b;
	}
	
	if (NbreBitCouleur == 32)
	{
		unsigned char *Surf;
		unsigned long *Surfl;

		Surf = (unsigned char *) ddsd.lpSurface;
		Surf = Surf + (x << 2) + y * ddsd.lPitch;
		Surfl = (unsigned long *) (Surf);
		*Surfl = couleur;

	}
	else if (NbreBitCouleur == 24)
	{
		unsigned char *Surf = (unsigned char *) ddsd.lpSurface;
		Surf = Surf + x * 3 + y * ddsd.lPitch;
		*(unsigned long *)Surf = couleur;
	}
	else if (NbreBitCouleur == 16)
	{
		unsigned short *Surf = (unsigned short *) ddsd.lpSurface;
		Surf = Surf + x + y * ddsd.lPitch / 2;
		r >>= 3;
		g >>= 2;
		b >>= 3;

		*Surf = b + (g << 5) + (r << 11);
	}

	return DD_TRUE;
}

//*********************************************************************
//* FUNCTION: WritePixel
//*--------------------------------------------------------------------
//* DESCRIPT: ecris dans un fichier la valeur du pixel x,y de l'image
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : ddsd, handle de fichier, x, y, nbrebitimage desirée
//*--------------------------------------------------------------------
//* OUT     : return value
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    12/12/2000 17:08:13
//* REVISION:									
//*********************************************************************
char WritePixel(DDSURFACEDESC ddsd, HANDLE hFile, long x, long y, char BitImage)
{
	unsigned char	r,
					g,
					b;


	if (NbreBitCouleur == 32)
	{
		unsigned char *Surf;

		Surf = (unsigned char *) ddsd.lpSurface;
		
		Surf = Surf + (x << 2) + y * ddsd.lPitch;

		b = *Surf;
		Surf++;
		g = *Surf;
		Surf++;
		r = *Surf;
	}
	else if (NbreBitCouleur == 24)
	{
		unsigned char *Surf = (unsigned char *) ddsd.lpSurface;
		Surf = Surf + x * 3 + y * ddsd.lPitch;
		
		b = *Surf;
		Surf++;
		g = *Surf;
		Surf++;
		r = *Surf;
	}
	else if (NbreBitCouleur == 16)
	{
		unsigned short *Surf = (unsigned short *) ddsd.lpSurface;
		Surf = Surf + x + y * ddsd.lPitch / 2;

		b = (((*Surf) >> 11)); 
		g = ((((*Surf) << 6) >> 11));
		r = ((((*Surf) << 11) >> 11));
	}

	if (BitImage == 24)
	{
		char temp[3];
		unsigned long nbre;

		temp[0] = b;
		temp[1] = g;
		temp[2] = r;
		WriteFile (hFile, temp, 3, &nbre, NULL);
	}
	return DD_OK;
}

