//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMUIFont_H__
#define	__BMUIFont_H__


#include	"BMGlobalInclude.h"
#include	"BMFile.h"

//--------------------------------
// Defines
//--------------------------------

#define		BMUIFONT_MAXCHAR		255
#define		BMUIFONT_CARACPERLINE	16

//--	File Delimiter
#define		FNT_BEGIN			"#BEGIN_FONT_MODEL"
#define		FNT_END				"#END_FONT_MODEL"

#define		FNT_FONT			"#SETFONT"
#define		FNT_DEGRADE			"#SETDEGRADE"

#define		FNT_UNACTIVE		"#SETUNACTIVE"

//--------------------------------
// Structures
//--------------------------------
typedef struct
{
	UBYTE		SizeX;				//size x of a character
	UBYTE		SizeY;				//size y of a character
	UBYTE		FontSpaceX;			//space between every character on x 
	UBYTE		FontSpaceY;			//space between every character on y
	STRING*		pszFileName;		//file name of the image
	STRING*		pszFontName;		//specific name of the font define by the author
	KMLIMAGE*	FontImage;			//image of the font
	UBYTE		FontModifie;		//is the font modified since loading
	SDWORD		FontCouleur1;		//first color  for degrade
	SDWORD		FontCouleur2;		//second color  for degrade
	SDWORD		FontVertical;		//is the degrade on vertical or not
}BMFONT;

//--------------------------------
// Variables
//--------------------------------
extern		SDWORD		BMFontMemory;

extern BMFONT*		BMUICurrentFont;			//current font in use
extern BMLIST*		BMUIListFont;				//list of the font, BMFONT


//--------------------------------
// Functions
//--------------------------------

extern		UBYTE	BMUIFontInit(STRING *pszFileName = NULL, STRING *pszPath = NULL);
extern		void	BMUIFontStop			(void);
EXPORT_DLL	void	BMUIFontDrawString		(STRING *str, SDWORD posx, SDWORD posy, KMLIMAGE *ima);
EXPORT_DLL	void	BMUIFontDegrade			(BMFONT *fonte, long CouleurA, long CouleurB, long IsVertical);
EXPORT_DLL	void	BMUIFontDegrade			(STRING *NameFont, long CouleurA, long CouleurB, long IsVertical);
extern		void	BMUIFontOnSwitch		(KMLIMAGE *image, void *param);
EXPORT_DLL  UBYTE	BMUIFontSelect			(STRING *pszName);
EXPORT_DLL	BMFONT*	BMUIFontCreate			(STRING *pszImage,STRING *pszNameFont, UBYTE sizex, UBYTE sizey, UBYTE spacex, UBYTE spacey);
EXPORT_DLL	UBYTE	BMUIFontDelete			(BMFONT *FontToDelete);
EXPORT_DLL	UBYTE	BMUIFontDelete			(STRING *FontName);
EXPORT_DLL	UBYTE	BMUIFontUnload			(BMFONT *FontToUnload);
EXPORT_DLL	UBYTE	BMUIFontUnload			(STRING *NameFont);
EXPORT_DLL	UBYTE	BMUIFontUnloadAllBut	(...);
EXPORT_DLL	BMFONT*	BMUIFontGetFont			(STRING *FontName);

#endif // __BMUIFont_H__