//--------------------------------
// Includes
//--------------------------------

#include	"BMConsole.h"

//--------------------------------
// DEFINE
//--------------------------------

#define		BASICSCOMMAND_TOOMANYPARAM	"Trop de parametres"
#define		BASICSCOMMAND_TOOFEWPARAM	"Pas assez de parametres"
#define		BASICSCOMMAND_BADPARAM		"Mauvais parametres"

#define		BASICSCOMMAND_COPYRIGHT		"??? (c) 2000"

#define		BASICSCOMMAND_BADFILE		"Mauvais type de fichier"
#define		BASICSCOMMAND_NOFILE		"Pas de fichier"

#define		BASICSCOMMAND_SCRIPTLOADED	"Script charge"
#define		BASICSCOMMAND_SCRIPTSAVED	"Script sauve"

//*********************************************************************
//* FUNCTION: BMConsoleIsDigit
//*--------------------------------------------------------------------
//* DESCRIPT: verifie si la string passé en paramètre est un nombre
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : string
//*--------------------------------------------------------------------
//* OUT     : true or false
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    15/12/2000 14:28:45
//* REVISION:									
//*********************************************************************

SDWORD BMConsoleIsDigit(STRING *string)
{
	while (*string != '\0')
	{
		if ( ((*string < '0') || (*string > '9')) && (*string != '.') && (*string != '-') )
			return (0);
		string++;
	}

	return (1);
}

//*********************************************************************
// Name	: BasicCommandsSetValue
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 16:41:43
//*********************************************************************

UDWORD BasicCommandsSetValue(BMCONSOLE *console)
{
	STRING						VarName[CONSOLE_MAX_CHAR];
	BMCONSOLEDATA			*	Dta;


	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);
	Dta = BMConsoleCsleFindData(VarName);

	if(!Dta)
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_BADPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() > 3 )			// erreur
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )			// erreur
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() == 3 )			// affectation
	{
		BMConsoleCsleGetParam(2, VarName, CSLE_PARAM_FLAG_STRING);

		if( ( Dta->DtaFlag == CSLE_PARAM_FLAG_FLOAT || Dta->DtaFlag == CSLE_PARAM_FLAG_DWORD) && !BMConsoleIsDigit(VarName) )
		{
			BMConsoleCsleWriteString(BASICSCOMMAND_BADPARAM);
			return CONSOLE_ERROR_NULL;
		}
		
		BMConsoleCsleGetParam(2, Dta->DtaData, Dta->DtaFlag);
	}

	
	switch(Dta->DtaFlag)
	{
	case(CSLE_PARAM_FLAG_DWORD):
		sprintf(VarName, "%s = %d", Dta->DtaName, *((SDWORD *)(Dta->DtaData)));
		break;
	case(CSLE_PARAM_FLAG_FLOAT):
		sprintf(VarName, "%s = %f", Dta->DtaName, *((float *)(Dta->DtaData)));
		break;
	case(CSLE_PARAM_FLAG_STRING):
		sprintf(VarName, "%s = \"%s\"", Dta->DtaName, ((STRING *)(Dta->DtaData)));
		break;
	}

	BMConsoleCsleWriteString(VarName);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicCommandsClearConsole
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 16:41:50
//*********************************************************************

UDWORD BasicCommandsClearConsole(BMCONSOLE *console)
{
	BMConsoleCsleResetAllOutLine();
	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicCommandsVer
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 10:39:44
//*********************************************************************

UDWORD BasicCommandsVer(BMCONSOLE *console)
{
	STRING						VarName[CONSOLE_MAX_CHAR];

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	if(!strcmp(VarName, "kml"))
	{
		strcpy(VarName, "KML ver : ");
		strcat(VarName, KMLLIGHT_VER);
	}
	else if(!strcmp(VarName, "console"))
	{
		strcpy(VarName, "BMConsole ver : ");
		strcat(VarName, BMCONSOLE_VER);
	}
	else
		strcpy(VarName, BASICSCOMMAND_BADPARAM);

	BMConsoleCsleWriteString(VarName);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandCopyright
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandCopyright(BMCONSOLE *console)
{
	BMConsoleCsleWriteString(BASICSCOMMAND_COPYRIGHT);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandInterrogation
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandInterrogation(BMCONSOLE *console)
{
	BMCONSOLEDATA			*scandata;
	BMCONSOLECOMMAND		*scancmd;
	UDWORD					i;
	STRING					VarName[CONSOLE_MAX_CHAR];

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	if(!strcmp(VarName, "data"))
	{

		scandata =	MegaConsole->CsleDataListe;
		for(i=0; i<MegaConsole->CsleNbData; i++)
		{
			BMConsoleCsleWriteString(scandata->DtaName);
			scandata = scandata->LPNext;
		}
	}
	else if(!strcmp(VarName, "command"))
	{
		scancmd	=	MegaConsole->CsleCommandListe;
		for(i=0; i<MegaConsole->CsleNbCommand; i++)
		{
			BMConsoleCsleWriteString(scancmd->CmdName);
			scancmd	= scancmd->LPNext;
		}
	}
	else
	{
		strcpy(VarName, BASICSCOMMAND_BADPARAM);
		BMConsoleCsleWriteString(VarName);
	}


	return CONSOLE_OK;
}


//*********************************************************************
// Name	: BasicsCommandEcho
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandEcho(BMCONSOLE *console)
{
	STRING						VarName[CONSOLE_MAX_CHAR];

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	if(!strcmp(VarName, "off"))
	{
		MegaConsole->CsleEcho	= BMCONSOLEUI_ECHOOFF;
	}
	else if(!strcmp(VarName, "on"))
	{
		MegaConsole->CsleEcho	= BMCONSOLEUI_ECHOON;
	}
	else
	{
		BMConsoleCsleWriteString(VarName);
	}



	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandLoadScript
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandLoadScript(BMCONSOLE *console)
{
	STRING				VarName[CONSOLE_MAX_CHAR];
	SDWORD				ReturnLoad;

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	ReturnLoad = BMConsoleScriptLoad(VarName);

	if( ReturnLoad == BMCONSOLE_SCRIPT_NOFILE )
		strcpy(VarName, BASICSCOMMAND_NOFILE);
	if( ReturnLoad == BMCONSOLE_SCRIPT_BADFILE )
		strcpy(VarName, BASICSCOMMAND_BADFILE);
	if( ReturnLoad == BMCONSOLE_SCRIPT_LOADED )
		strcpy(VarName, BASICSCOMMAND_SCRIPTLOADED);

	BMConsoleCsleWriteString(VarName);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandSaveScript
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandSaveScript(BMCONSOLE *console)
{
	STRING				VarName[CONSOLE_MAX_CHAR];
	SDWORD				ReturnLoad;

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	ReturnLoad = BMConsoleScriptSave(VarName);

	if( ReturnLoad == BMCONSOLE_SCRIPT_NOFILE )
		strcpy(VarName, BASICSCOMMAND_NOFILE);
	if( ReturnLoad == BMCONSOLE_SCRIPT_SAVED )
		strcpy(VarName, BASICSCOMMAND_SCRIPTSAVED);

	BMConsoleCsleWriteString(VarName);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandReadFile
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandReadFile(BMCONSOLE *console)
{
	STRING				VarName[CONSOLE_MAX_CHAR];
	SDWORD				ReturnLoad;

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);

	ReturnLoad = BMConsoleReadFile(VarName);

	if( ReturnLoad == BMCONSOLE_SCRIPT_NOFILE )
		BMConsoleCsleWriteString(BASICSCOMMAND_NOFILE);

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandChangeCursor
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandChangeCursor(BMCONSOLE *console)
{
	STRING				VarName[CONSOLE_MAX_CHAR];

	if( BMConsoleCsleGetNbParam() > 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 2 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	BMConsoleCsleGetParam(1, VarName, CSLE_PARAM_FLAG_STRING);
	
	if( BMConsoleIsDigit(VarName) )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_BADPARAM);
		return CONSOLE_ERROR_NULL;
	}

	MegaConsole->CsleCursorChar = VarName[0];

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BasicsCommandChangeFontColor
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								13/12/2000 12:13:36
//*********************************************************************

UDWORD BasicsCommandChangeFontColor(BMCONSOLE *console)
{
	STRING				VarName[CONSOLE_MAX_CHAR];
	SDWORD				Color[7], ColorLong[2];
	SDWORD				i;


	if( BMConsoleCsleGetNbParam() > 8 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOMANYPARAM);
		return CONSOLE_ERROR_NULL;
	}

	if( BMConsoleCsleGetNbParam() < 8 )
	{
		BMConsoleCsleWriteString(BASICSCOMMAND_TOOFEWPARAM);
		return CONSOLE_ERROR_NULL;
	}

	for(i=0; i<7; i++)
	{
		BMConsoleCsleGetParam(1+i, VarName, CSLE_PARAM_FLAG_STRING);
		if( ! BMConsoleIsDigit(VarName) )
		{
			BMConsoleCsleWriteString(BASICSCOMMAND_BADPARAM);
			return CONSOLE_ERROR_NULL;
		}
		BMConsoleCsleGetParam(1+i, &Color[i], CSLE_PARAM_FLAG_DWORD);
	}

	ColorLong[0] = 0 | (Color[0]<<16) | (Color[1]<<8) | (Color[2]) ;
	ColorLong[1] = 0 | (Color[3]<<16) | (Color[4]<<8) | (Color[5]) ;
	
	BMUIFontDegrade( (BMFONT *)NULL, ColorLong[0], ColorLong[1],  Color[6]);
	
	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BMConsoleRegisterBasicCommands
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:25:42
//*********************************************************************

void BMConsoleRegisterBasicCommands(void)
{
	BMConsoleCsleAddCommandToConsole("set",				BasicCommandsSetValue);
	BMConsoleCsleAddCommandToConsole("clear",			BasicCommandsClearConsole);
	BMConsoleCsleAddCommandToConsole("ver",				BasicCommandsVer);
	BMConsoleCsleAddCommandToConsole("copyright",		BasicsCommandCopyright);
	BMConsoleCsleAddCommandToConsole("?",				BasicsCommandInterrogation);
	BMConsoleCsleAddCommandToConsole("echo",			BasicsCommandEcho);

	BMConsoleCsleAddCommandToConsole("script_loadscript",	BasicsCommandLoadScript);
	BMConsoleCsleAddCommandToConsole("script_savescript",	BasicsCommandSaveScript);
	BMConsoleCsleAddCommandToConsole("script_readfile",		BasicsCommandReadFile);

	BMConsoleCsleAddCommandToConsole("console_changecursor",BasicsCommandChangeCursor);

	BMConsoleCsleAddCommandToConsole("font_changecolor",BasicsCommandChangeFontColor);
}