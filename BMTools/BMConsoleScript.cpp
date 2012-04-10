//--------------------------------
// Includes
//--------------------------------

#include	"BMConsole.h"
#include	"BMFile.h"

//--------------------------------
// Gloables
//--------------------------------


//*********************************************************************
// Name	: BMConsoleScriptLoad
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 16:47:48
//*********************************************************************

SDWORD BMConsoleScriptLoad(STRING *filename)
{
	
	KMLFILE		*	fp;
	STRING			chaine[BMCONSOLE_SCRIPT_MAX_LINE_SIZE]; 
	UDWORD			i, start = 0;
	
	// ouverture
	if((fp = KMLPackOpen(NULL, filename))==NULL)
	{
		return BMCONSOLE_SCRIPT_NOFILE;
	}

	//while( fgets(chaine, BMCONSOLE_SCRIPT_MAX_LINE_SIZE, fp) != NULL)
	while ( BMFileReadLine (chaine, BMCONSOLE_SCRIPT_MAX_LINE_SIZE, fp) == BMTRUE)
	{
		chaine[strlen(chaine)-1] = NULL;

		if( !start )
		{
			if( strcmp(chaine, BMCONSOLE_SCRIPT_HEADER) )
				return BMCONSOLE_SCRIPT_BADFILE;
			else
				start++;

		}
		else if(chaine[0] != NULL)
		{
			for(i=0; i<strlen(chaine); i++)
			{
				if( chaine[i] == BMCONSOLE_SCRIPT_COMMENT_CHAR )
					chaine[i] = NULL;
			}
			
			if(chaine[0] != NULL)
				BMConsoleCsleExterneUpdate(chaine);
		}
	}

	// fermeture
	KMLPackClose(fp);
	
	return BMCONSOLE_SCRIPT_LOADED;
}

//*********************************************************************
// Name	: BMConsoleScriptSave
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 18:57:38
//*********************************************************************

SDWORD BMConsoleScriptSave(STRING *filename)
{
	BMCONSOLEDATA		*	scandata;

	FILE				*	fp;
	STRING					chaine[BMCONSOLE_SCRIPT_MAX_LINE_SIZE], number[CONSOLE_MAX_CMDLINE_CHAR];
	UDWORD					i, start = 0;
	
	
	
	// ouverture
	if((fp = fopen(filename, "w+t"))==NULL)
	{
		return BMCONSOLE_SCRIPT_NOFILE;
	}

	strcpy	(chaine, BMCONSOLE_SCRIPT_HEADER);
	strcat	(chaine, "\n");
	fputs	(chaine, fp);

	// ecriture
	scandata =	MegaConsole->CsleDataListe;
	for(i=0; i<MegaConsole->CsleNbData; i++)
	{
		switch(scandata->DtaFlag)
		{
		case(CSLE_PARAM_FLAG_DWORD):
			sprintf(number, "%d", (*(SDWORD *)scandata->DtaData));
			break;
		case(CSLE_PARAM_FLAG_FLOAT):
			sprintf(number, "%f", (*(float *)scandata->DtaData));
			break;
		case(CSLE_PARAM_FLAG_STRING):
			sprintf(number, "\"%s\"", (STRING *)scandata->DtaData);
			break;
		}
		sprintf(chaine, "%s %s\n", scandata->DtaName, number);
		fputs(chaine, fp);

		scandata = scandata->LPNext;
	}

	fclose(fp);

	return BMCONSOLE_SCRIPT_SAVED;
}

//*********************************************************************
// Name	: BMConsoleReadFile
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 18:57:38
//			  Mike (compatility with KMLPack	11/01/2001 15:51:13
//*********************************************************************

SDWORD BMConsoleReadFile(STRING *filename)
{
	KMLFILE		*	fp;
	STRING			chaine[BMCONSOLE_SCRIPT_MAX_LINE_SIZE]; 
	
	// ouverture
	if((fp = KMLPackOpen(NULL, filename))==NULL)
	{
		return BMCONSOLE_SCRIPT_NOFILE;
	}

	//while( fgets(chaine, BMCONSOLE_SCRIPT_MAX_LINE_SIZE, fp) != NULL)
	while ( BMFileReadLine (chaine, BMCONSOLE_SCRIPT_MAX_LINE_SIZE, fp) == BMTRUE)
	{
		chaine[strlen(chaine)-1] = NULL;
		BMConsoleCsleAddOutLine(chaine);
	}

	// fermeture
	KMLPackClose(fp);
	
	return BMCONSOLE_FILE_READ;
}