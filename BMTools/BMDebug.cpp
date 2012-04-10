#include "BMDebug.h"

//--------------------------------
// Globales
//--------------------------------
STRING		DebugStringArray [50][255];
SBYTE		DebugFirstLine;
SBYTE		DebugEndLine;


//*********************************************************************
//* FUNCTION: BMDebugStart
//*--------------------------------------------------------------------
//* DESCRIPT: Start the debug lib
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 14:21:25
//* REVISION:									
//*********************************************************************
SBYTE		BMDebugStart		()
{
	DebugFirstLine	= 0;
	DebugEndLine	= 0;

	FillMemory (DebugStringArray, sizeof(DebugStringArray), 0);

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMDebugEnd
//*--------------------------------------------------------------------
//* DESCRIPT: nothing special
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 14:21:44
//* REVISION:									
//*********************************************************************
SBYTE		BMDebugEnd			()
{

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMDebugPrint 
//*--------------------------------------------------------------------
//* DESCRIPT: print a string in the array of debug
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : string to print
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 14:22:00
//* REVISION:									
//*********************************************************************
SBYTE		BMDebugPrint		(STRING *Line)
{
#ifdef	BM_DEBUG
	strcpy (DebugStringArray [DebugEndLine], Line);
	DebugEndLine ++;
	
	if (DebugEndLine == 25)//TODO REVOIR AVEC BMDEBUGNUMBERLINE
		DebugEndLine = 0;

	if (DebugEndLine == DebugFirstLine)
	{
		DebugFirstLine ++;
		if (DebugFirstLine == 25)//TODO REVOIR PAREIL...
			DebugFirstLine = 0;
	}
#endif

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMDebugPrintArray
//*--------------------------------------------------------------------
//* DESCRIPT: display the debug values
//*                                                                  
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/01/2001 14:22:29
//* REVISION:									
//*********************************************************************
SBYTE		BMDebugPrintArray	()
{
#ifdef BM_DEBUG
	SBYTE	i;
	DWORD	j;

	i = DebugFirstLine;
	j = 0;

	if (BMUICurrentFont == NULL)
		return (BMFALSE);

	while ( i != DebugEndLine)
	{
		BMUIFontDrawString	(DebugStringArray [i], 0, j, NULL);

		i++;
		j += BMUICurrentFont->SizeY + 1;
		if (i == 25)//TODO REPLACE WITH NUMBEROFLINEWE CAN DISPLAY ON A SCREEN
			i = 0;
	}
#endif

	return (BMTRUE);
}

