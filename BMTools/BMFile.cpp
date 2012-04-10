#include "BMFile.h"

//--------------------------------
// Globales
//--------------------------------


//*********************************************************************
//* FUNCTION: BMReadWord 
//*--------------------------------------------------------------------
//* DESCRIPT: Read a word in a text file (separators are ' ' or '\t' or '\r' or '\n')
//*			  exclude the comment (;xxxxxx), activecomment = 1 active the function of comment
//*--------------------------------------------------------------------
//* IN      : file, buffer, size of buffer, activecomment
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    02/01/2001 18:33:25
//* REVISION:									    16/01/2001 15:34:43
//*********************************************************************
SBYTE			BMFileReadWord		(KMLFILE **f, STRING *buffer, SDWORD sizeofbuffer, UBYTE activecomment)
{
	SDWORD  i;
	UBYTE	bOk,
			bComment;

	i			= 0;
	bOk			= 0;
	bComment	= 0;

	while ( (i < sizeofbuffer) && (!KMLPackEof(*f)) )
	{
		KMLPackRead ( (STRING *) &buffer[i], 1, *f);

		if (buffer[i] == ';')	//Comment
		{
			if (activecomment == 1)
			{
				if (bComment == 0)
					bComment = 1;
			}
		}

		if (bComment == 1)
		{
			if (buffer[i] == '\n')
				bComment = 0; 
		}

		if (bComment == 0)
		{
			if (bOk == 0)
			{
				if ( (buffer[i] != ' ') && (buffer[i] != '\n') && (buffer[i] != 0x0d) && (buffer[i] != '\t') )
				{
					bOk = 1;
					i++;
				}
			}
			else
			{
				if ( (buffer[i] == ' ') || (buffer[i] == '\n') || (buffer[i] == 0x0d) || (buffer[i] == '\t') )
				{
					buffer[i] = '\0';
					return (BMTRUE);
				}
				i++;
			}
		}
		
	}

	return (BMTRUE - 1);	//error !
}

//*********************************************************************
//* FUNCTION: BMFileReadLine
//*--------------------------------------------------------------------
//* DESCRIPT: replace the fgets functions
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : bufer, sizeof buffer, file pointer
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    11/01/2001 15:34:36
//* REVISION:									
//*********************************************************************
SBYTE			BMFileReadLine		(STRING *buffer, SDWORD sizeofbuffer, KMLFILE *f)
{
	SDWORD i;

	i = -1;
	if (sizeofbuffer == 0)
		return (BMTRUE - 1);//error

	if (KMLPackEof (f))
		return (BMTRUE - 1);

	do
	{
		i++;
		if (KMLPackRead (&buffer[i], 1, f) != KML_OK)
		{
			buffer[i] = '\0';
			return (BMTRUE);
		}
	}while ((buffer[i] != '\n') && (buffer[i] != '\r') && (i != sizeofbuffer));

	buffer[i] = '\0';

	return (BMTRUE);
}

//**************************************************************
/** \fn     void BMFileGetPath (STRING *pszFile, STRING *pszPath)
 *
 *  \param  *pszFile 
 *  \param  *pszPath 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author MikE                       \date 16/03/2001 09:45:21
 ***************************************************************/
void	BMFileGetPath		(STRING *pszFile, STRING *pszPath)
{
	STRING chainetemp[255];
	
	//Get the path if there is one
	chainetemp[0]= '\0';
	if (pszPath != NULL)
	{
		if ( strcmp(pszPath, "\\.") != 0)
			strcat(chainetemp, pszPath);
	}
	strcat (chainetemp, pszFile);

	strcpy (pszFile, chainetemp);
}
