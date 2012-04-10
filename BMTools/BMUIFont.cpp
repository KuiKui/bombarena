//--------------------------------
// Includes
//--------------------------------

#include	"BMUIFont.h"
#include	"BMImage.h"
#include	"BMList.h"
#include	"BMGlobalInclude.h"

//--------------------------------
// Globales
//--------------------------------
SDWORD				BMFontMemory = 0;	//Allocate memory

BMFONT*		BMUICurrentFont;			//current font in use
BMLIST*		BMUIListFont;				//list of the font, BMFONT


//**************************************************************
/** \fn     UBYTE BMUIFontInit(STRING *pszFileName, STRING *pszPath)
 *
 *  \param  *pszFileName 
 *  \param  *pszPath     
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  you can load a file font specifier or not
 *
 *  \author MikE                       \date 16/03/2001 09:10:52
 ***************************************************************/
UBYTE BMUIFontInit(STRING *pszFileName, STRING *pszPath)
{
	BMUIListFont	= BMListNew ();
	BMUICurrentFont = NULL;

	if (pszFileName != NULL)
	{
		STRING		file [255];
		KMLFILE		*f;
		STRING		RelativeDirectory [255];
		STRING		chaine [255];
		STRING		fontname[255];
		STRING		filename[255];
		UDWORD		color_vertic [7];
		UDWORD		i;

		file [0] = '\0';
		if (pszPath != NULL)
		{
			if ( strcmp(pszPath, "\\.") != 0)
				strcat(file, pszPath);
		}
		strcat (file, pszFileName);

		f = KMLPackOpen(NULL, file);
		if (f == NULL)
			return (BMFALSE);												//Error, cannot open the file !

		//Parse the file
		strcpy(RelativeDirectory, "\\.");										//Set the directory to the current

		//Search for the beginning of the file
		do
		{
			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMFALSE);	//Error
			}	
		}while ( strcmp(chaine, FNT_BEGIN) != 0);

		//Read the file while not finished !
		while ( strcmp (chaine, FNT_END) != 0)
		{
			if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
			{
				KMLPackClose (f);
				return (BMFALSE);	//Error
			}

			//------------------
			// Set Directory
			//------------------
			if ( strcmp (chaine, BMSETDIRECTORY) == 0)
			{
				if (BMFileReadWord (&f, RelativeDirectory, sizeof(RelativeDirectory)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}
			}
			//------------------
			// Create Font
			//------------------
			else if ( strcmp (chaine, FNT_FONT) == 0)
			{
				if (BMFileReadWord (&f, filename, sizeof(filename)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}

				BMFileGetPath (filename, RelativeDirectory);

				if (BMFileReadWord (&f, fontname, sizeof(fontname)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}
				/*if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}
*/
				for (i = 0; i < 4; i ++)
				{
					if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
					{
						KMLPackClose (f);
						return (BMFALSE);	//Error
					}
					color_vertic [i] = atoi (chaine);
				}

				BMUIFontCreate ( filename, fontname, (UBYTE) color_vertic[0], (UBYTE) color_vertic[1], (UBYTE) color_vertic[2], (UBYTE) color_vertic[3]);
				
			}
			//------------------
			// Set Font Degrade
			//------------------
			else if ( strcmp (chaine, FNT_DEGRADE) == 0)
			{
				UDWORD	Couleur1,
						Couleur2;

				if (BMFileReadWord (&f, fontname, sizeof(fontname)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}

				for (i = 0; i < 7; i ++)
				{
					if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
					{
						KMLPackClose (f);
						return (BMFALSE);	//Error
					}
					color_vertic [i] = atoi (chaine);
				}

				Couleur1 = ( ( color_vertic[0] & 0xff) << 16) + ( ( color_vertic[1] & 0xff) << 8) + (color_vertic[2] & 0xff);
				Couleur2 = ( ( color_vertic[3] & 0xff) << 16) + ( ( color_vertic[4] & 0xff) << 8) + (color_vertic[5] & 0xff);


				BMUIFontDegrade (fontname, Couleur1, Couleur2, color_vertic [6]);
			}
			//------------------
			// Set Font Unactive
			//------------------
			else if ( strcmp (chaine, FNT_UNACTIVE) == 0)
			{
				
				if (BMFileReadWord (&f, chaine, sizeof(chaine)) != BMTRUE)
				{
					KMLPackClose (f);
					return (BMFALSE);	//Error
				}

				BMUIFontUnload (chaine);
			}
		}
		
		KMLPackClose (f);
	}

	return (BMTRUE);
}

//*********************************************************************
// Name	: BMUIFontStop
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 11:55:13
//*********************************************************************

void BMUIFontStop(void)
{

	while (BMUIListFont->First->Next != NULL)
	{
		if (BMUIFontDelete ( (BMFONT *)BMUIListFont->First->Info ) != BMTRUE)
			KMLWarning ("probleme dans la desallocation de fonte");
	}

	BMListDelete ( BMUIListFont );
}

//**************************************************************
/** \fn     UBYTE BMUIFontSelect ( STRING *pszName )
 *
 *  \param  *pszName name of the fonte
 *
 *  \return  true or false
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 18:22:50
 ***************************************************************/
UBYTE	BMUIFontSelect ( STRING *pszName )
{
	//parse the list
	BMNODE	*Courant,
			*Suivant;
	BMFONT	*Fonte;

	if ( BMUICurrentFont != NULL)
	{
		if ( strcmp(BMUICurrentFont->pszFontName, pszName) == 0)		// the font is selected yet !
			return (BMTRUE);
	}

	Courant = BMUIListFont->First;

	while (Suivant = Courant->Next)
	{	
		Fonte = (BMFONT *) Courant->Info;
		
		if ( strcmp(Fonte->pszFontName, pszName) == 0)
		{
			BMUICurrentFont = Fonte;
			
			if (Fonte->FontImage == NULL)
			{
				Fonte->FontImage = KMLCreateImage (0, 0, KML_BMP, Fonte->pszFileName);	//reload the font if unloaded !
				if (Fonte->FontImage == NULL)
					return (BMFALSE);
			
				KMLSetCallback (Fonte->FontImage, BMUIFontOnSwitch, Fonte);
				
				if (Fonte->FontModifie)
					BMUIFontOnSwitch (Fonte->FontImage, Fonte);		//apply the degrade !
			}
			return (BMTRUE);
		}

		Courant = Suivant;
	}

	return (BMFALSE);
}

//**************************************************************
/** \fn     BMFONT* BMUIFontCreate (STRING *pszImage,STRING *pszNameFont, UBYTE sizex, UBYTE sizey, UBYTE spacex, UBYTE spacey)
 *
 *  \param  *pszImage    
 *  \param  *pszNameFont can't have a ' ' in the name
 *  \param  sizex        
 *  \param  sizey        
 *  \param  spacex       
 *  \param  spacey       
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 18:50:30
 ***************************************************************/
BMFONT*	BMUIFontCreate (STRING *pszImage,STRING *pszNameFont, UBYTE sizex, UBYTE sizey, UBYTE spacex, UBYTE spacey)
{
	BMFONT *font;
	UBYTE	i;

	if ( (sizey == 0) || (sizex == 0))
		return (NULL);

	i = 0;
	while (pszNameFont[i] != '\0')
	{
		if (pszNameFont[i] == ' ')		//INVALID CHARACTER
			return (NULL);

		i++;
	}
	//verifying name isn't use yet
	if (BMUIFontGetFont ( pszNameFont ) != NULL)
		return (NULL);

	font = (BMFONT *)ALLOC2 (sizeof (BMFONT), BMFontMemory);

	font->FontImage = KMLCreateImage (0, 0, KML_BMP, pszImage);
	if (font->FontImage == NULL)
	{
		FREE2 (font, BMFontMemory);
		return (NULL);
	}
	font->FontModifie = BMFALSE;
	font->FontSpaceX = spacex;
	font->FontSpaceY = spacey;
	font->FontVertical = 0;
	font->pszFileName = (STRING *) ALLOC2 ( strlen(pszImage) + 1, BMFontMemory);
	font->pszFontName = (STRING *) ALLOC2 ( strlen(pszNameFont) + 1, BMFontMemory);
	font->SizeX = sizex;
	font->SizeY = sizey;
	
	strcpy (font->pszFileName, pszImage);
	strcpy (font->pszFontName, pszNameFont);

	KMLSetCallback (font->FontImage, BMUIFontOnSwitch, font);

	BMListAddTail (BMUIListFont, font);
	return(font);
}

//**************************************************************
/** \fn     UBYTE BMUIFontDelete (BMFONT *FontToDelete)
 *
 *  \param  *FontToDelete
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 14/03/2001 18:36:23
 ***************************************************************/
UBYTE	BMUIFontDelete (BMFONT *FontToDelete)
{
	BMNODE  *Courant;
	BMNODE	*Suivant;

	Courant = BMUIListFont->First;

	//parse the list of font
	while (Suivant = Courant->Next)
	{
		if ( (BMFONT *)Courant->Info == FontToDelete)			//Font found
		{
			FREE2 (FontToDelete->pszFileName, BMFontMemory);
			FREE2 (FontToDelete->pszFontName, BMFontMemory);
			if (FontToDelete->FontImage != NULL)
			{
				KMLDeleteImage (FontToDelete->FontImage);
				FontToDelete->FontImage = NULL;
			}

			FREE2 (FontToDelete, BMFontMemory);
			BMDeleteNode (Courant);
	
			return (BMTRUE);
		}

		Courant = Suivant;
	}
	//font not found !!!
	return (BMFALSE);
}

//**************************************************************
/** \fn     UBYTE BMUIFontUnload (BMFONT *FontToUnload)
 *
 *  \param  *FontToUnload
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  the current font is null after you must select a font
 *
 *  \author MikE                       \date 14/03/2001 18:31:46
 ***************************************************************/
UBYTE	BMUIFontUnload (BMFONT *FontToUnload)
{
	if (FontToUnload->FontImage != NULL)
	{
		KMLDeleteImage (FontToUnload->FontImage);
		FontToUnload->FontImage = NULL;
	}

	BMUICurrentFont = NULL;

	return (BMTRUE);
}

//*********************************************************************
// Name	: BMUIFontDrawString
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 11:09:03
//*********************************************************************

void BMUIFontDrawString(STRING *str, SDWORD posx, SDWORD posy, KMLIMAGE *ima)
{
	UDWORD		strlenght, i, x, y;

	if( (posx >= GlobalResX) || (posx < 0 ) || (posy >= GlobalResY) || (posy < 0))
		return;

	if ( BMUICurrentFont == NULL )
		return;

	strlenght = strlen(str);

	if( strlenght > BMUIFONT_MAXCHAR )
		return;

	for( i = 0; i < strlenght; i++)
	{
		x = str[i] % BMUIFONT_CARACPERLINE;
		y = str[i] / BMUIFONT_CARACPERLINE;

		// draw
		KMLFastBlt(ima, posx + i * (BMUICurrentFont->SizeX + BMUICurrentFont->FontSpaceX), posy, BMUICurrentFont->FontImage, x * BMUICurrentFont->SizeX, y * BMUICurrentFont->SizeY, BMUICurrentFont->SizeX, BMUICurrentFont->SizeY);
	}
}

//*********************************************************************
//* FUNCTION: BMUIFontDegrade
//*--------------------------------------------------------------------
//* DESCRIPT: fait un degrade sur la fonte (16 carac par ligne et 16 carac en longueur obilgatoire !!!)
//*			les couleurs des degradés et degrade en vertical = 0 ou horizontale != 0
//*--------------------------------------------------------------------
//* IN      : image fonte, couleurA, couleurB, isvertical
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    13/12/2000 17:18:10
//* REVISION:									
//*********************************************************************
void BMUIFontDegrade (BMFONT *fonte, long CouleurA, long CouleurB, long IsVertical)
{

	unsigned char	R,
					G,
					B;
	unsigned char	R1,
					G1,
					B1;
	unsigned char	R2,
					G2,
					B2;

	unsigned char * trans;
	DDSURFACEDESC ddsd;

	if ( fonte == NULL )
	{
		fonte = BMUICurrentFont;
		if (fonte == NULL)
			return;
	}


	if (GlobalColor == 32)
	{
		R1 = (CouleurA & 0x00ff0000) >> 16;
		G1 = (CouleurA & 0x0000ff00) >> 8;
		B1 = (CouleurA & 0x000000ff);

		R2 = (CouleurB & 0x00ff0000) >> 16;
		G2 = (CouleurB & 0x0000ff00) >> 8;
		B2 = (CouleurB & 0x000000ff);
	}
	else if (GlobalColor == 16)
	{
		R1 = (CouleurA & 0x00ff0000) >> 19;
		G1 = (CouleurA & 0x0000ff00) >> 10;
		B1 = (CouleurA & 0x000000ff) >> 3;

		R2 = (CouleurB & 0x00ff0000) >> 19;
		G2 = (CouleurB & 0x0000ff00) >> 10;
		B2 = (CouleurB & 0x000000ff) >> 3;
	}

	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_PITCH | DDSD_LPSURFACE;

	while (fonte->FontImage->Surface->Lock(NULL, &ddsd, 0, NULL) == DDERR_WASSTILLDRAWING) ;

	trans = (unsigned char *) ddsd.lpSurface;

	if (GlobalColor == 32)
	{
		B = trans[0];
		G = trans[1];
		R = trans[2];
	}
	else if (GlobalColor == 16)
	{
		B = trans [0];
		G = trans [1];
	}

	for (long y = (long) fonte->FontImage->Hauteur - 1; y >= 0; y--)
	{
		for (long x = 0l; x < (long) fonte->FontImage->Largeur; x++)
		{
			unsigned char *surf;
			
			if (GlobalColor == 32)
			{
				surf = (unsigned char *) ddsd.lpSurface + x * 4 + y * ddsd.lPitch;
				if ( (surf[0] != B) || (surf[1] != G) || (surf[2] != R) )
				{
					long difference;
					long value2;
					if (IsVertical == 0)
					{
						value2		= fonte->FontImage->Hauteur / BMUIFONT_CARACPERLINE;
						difference	= y % value2;
					}
					else
					{
						value2		= fonte->FontImage->Largeur / BMUIFONT_CARACPERLINE;
						difference	= x % value2;
					}

					surf[2] = ( (R2 - R1) * difference) / value2 + R1;
					surf[1] = ( (G2 - G1) * difference) / value2 + G1;
					surf[0] = ( (B2 - B1) * difference) / value2 + B1;
				}
			}
			else if (GlobalColor == 16)
			{
				surf = (unsigned char *) ddsd.lpSurface + x * 2 + y * ddsd.lPitch;

				if ( (surf[0] != B) || (surf[1] != G) )
				{
					UWORD color;
					long difference;
					long value2;

					if (IsVertical == 0)
					{
						value2		= fonte->FontImage->Hauteur / BMUIFONT_CARACPERLINE;
						difference	= y % value2;
					}
					else
					{
						value2		= fonte->FontImage->Largeur / BMUIFONT_CARACPERLINE;
						difference	= x % value2;
					}

					color = 0;
					color |=     ( ( (B2 - B1) * difference) / value2 + B1) & 0x1F;
					color |= ( ( ( ( (G2 - G1) * difference) / value2 + G1) & 0x3F) << 5);
					color |= ( ( ( ( (R2 - R1) * difference) / value2 + R1) & 0x1F) << 11);

					
					surf[0] = color & 0xff;
					surf[1] = (color >> 8) &0xff;
				}
			}
		}
	}
	fonte->FontImage->Surface->Unlock(NULL);

	fonte->FontModifie  = BMTRUE;
	fonte->FontCouleur1 = CouleurA;
	fonte->FontCouleur2 = CouleurB;
	fonte->FontVertical = IsVertical;
}

//*********************************************************************
//* FUNCTION: BMUIFontOnSwitch
//*--------------------------------------------------------------------
//* DESCRIPT: function CALLBACK of the font, when a switch is activated
//* to reload the font and set the color correctly                                                                    
//*--------------------------------------------------------------------
//* IN      : image, param (not used)
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/01/2001 11:05:39
//* REVISION:									
//*********************************************************************
void BMUIFontOnSwitch (KMLIMAGE *image, void *param)
{
	BMFONT *fonte;

	fonte = (BMFONT *) param;

	if (fonte->FontModifie == BMTRUE)
		BMUIFontDegrade (fonte, fonte->FontCouleur1, fonte->FontCouleur2, fonte->FontVertical); 
}

//**************************************************************
/** \fn     UBYTE BMUIFontUnloadAllBut (UDWORD NumberToKeep, ...)
 *
 *  \param  NumberToKeep number of font to keep loaded
 *  \param  ... multiple parameter
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 09:40:24
 ***************************************************************/
UBYTE	BMUIFontUnloadAllBut	(UDWORD NumberToKeep, ...)
{
	va_list vlist;
	BMNODE	*Courant,
			*Suivant;
	BMFONT	*Font;
	UDWORD	i;
	UBYTE	bFound;

	Courant = BMUIListFont->First;

	while ( Suivant = Courant->Next)
	{
		//unload all font but ...

		Font = (BMFONT *) Courant->Info;
		va_start ( vlist, NumberToKeep );

		bFound = BMFALSE;
		i = 0;
		while (i < NumberToKeep)		//look if the font must be keep loaded
		{
			if (strcmp (va_arg ( vlist, STRING *), Font->pszFontName) == 0)
				bFound = BMTRUE;
			
			i++;
		}
		va_end (vlist);

		if (bFound == BMFALSE)
		{
			//so unload it
			BMUIFontUnload (Font);
		}

		Courant = Suivant;
	}

	return (BMTRUE);
}

//**************************************************************
/** \fn     BMFONT* BMUIFontGetFont ( STRING *FontName )
 *
 *  \param  *FontName 
 *
 *  \return  NULL or the specified font
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 09:11:04
 ***************************************************************/
BMFONT*	BMUIFontGetFont ( STRING *FontName )
{
	BMNODE *Courant;
	BMNODE *Suivant;

	Courant = BMUIListFont->First;

	while (Suivant = Courant->Next)
	{	
		BMFONT *Font;

		Font = (BMFONT *) Courant->Info;

		if ( strcmp (Font->pszFontName, FontName) == 0)
			return ( Font );

		Courant = Suivant;
	}

	return (NULL);
}

//**************************************************************
/** \fn     void BMUIFontDegrade (STRING *NameFont, long CouleurA, long CouleurB, long IsVertical)
 *
 *  \param  *NameFont 
 *  \param  CouleurA  
 *  \param  CouleurB  
 *  \param  IsVertical
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 10:28:18
 ***************************************************************/
void	BMUIFontDegrade			(STRING *NameFont, long CouleurA, long CouleurB, long IsVertical)
{
	BMFONT *font;
	
	font = BMUIFontGetFont (NameFont);

	if (font != NULL)
		BMUIFontDegrade (font, CouleurA, CouleurB, IsVertical);
}

//**************************************************************
/** \fn     UBYTE BMUIFontDelete (STRING *FontName)
 *
 *  \param  *FontName
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 10:28:15
 ***************************************************************/
UBYTE	BMUIFontDelete			(STRING *FontName)
{
	BMFONT *font;

	font = BMUIFontGetFont (FontName);
	if (font != NULL)
		return (BMUIFontDelete (font));

	return (BMFALSE);
}

//**************************************************************
/** \fn     UBYTE BMUIFontUnload (STRING *NameFont)
 *
 *  \param  *NameFont
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 15/03/2001 10:28:13
 ***************************************************************/
UBYTE	BMUIFontUnload			(STRING *NameFont)
{
	BMFONT *font;

	font = BMUIFontGetFont (NameFont);

	if (font != NULL)
		return (BMUIFontUnload (font));

	return (BMFALSE);
}
