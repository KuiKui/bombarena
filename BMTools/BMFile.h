#ifndef __BMFILE_H__
#define	__BMFILE_H__

#include	"BMGlobalInclude.h"
#include	"BMWin32.h"

//--------------------------------
// Defines
//--------------------------------

//-- File Delimiter
#define	BMSETDIRECTORY			"#SET_DIRECTORY"		//set directory "relative directory"
								

//--------------------------------
// Structures
//--------------------------------


//--------------------------------
// Globales
//--------------------------------


//--------------------------------
// Fonctions
//--------------------------------
extern	SBYTE			BMFileReadWord		(KMLFILE **f, STRING *buffer, SDWORD sizeofbuffer, UBYTE activecomment = 1);
extern	SBYTE			BMFileReadLine		(STRING *buffer, SDWORD sizeofbuffer, KMLFILE *f);
extern	void			BMFileGetPath		(STRING *pszFile, STRING *pszPath);
#endif
