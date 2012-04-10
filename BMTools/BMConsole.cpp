//--------------------------------
// Include
//--------------------------------
#include	"BMConsole.h"
#include	"BMImage.h"

//--------------------------------
// Macros
//--------------------------------


//--------------------------------
// Variables Globales !
//--------------------------------

BMCONSOLE		*	MegaConsole;
SDWORD				MegaTest;
char				MegaTestString[CONSOLE_MAX_CHAR];

//--------------------------------
// Debug
//--------------------------------

SDWORD				BMConsoleMemory = 0;	//Allocate memory


//*********************************************************************
//
// Commands
//
//*********************************************************************

//*********************************************************************
// Name	: BMCONSOLECmdSetAllParam
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:00:19
//*********************************************************************

UDWORD BMConsoleCmdSetAllParam(BMCONSOLECOMMAND *cmd, STRING *_CmdName, CONSOLELPFONC _CmdLpFunc)
{
	// assert
	if( (cmd == NULL) || (_CmdName == NULL) || (_CmdLpFunc == NULL) )
		return CONSOLE_ERROR_NULL;

	// cut string
	if( strlen(_CmdName) > CONSOLE_MAX_CHAR-1 )
		_CmdName[CONSOLE_MAX_CHAR] = NULL;

	// affectation
	strcpy(cmd->CmdName, _CmdName);
	cmd->CmdLpFunc	= _CmdLpFunc;

	return CONSOLE_OK;
}



//*********************************************************************
//
// Datas
//
//*********************************************************************

//*********************************************************************
// Name	: BMConsoleDtaSetAllParam
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:01:08
//*********************************************************************

UDWORD BMConsoleDtaSetAllParam(BMCONSOLEDATA *dta, STRING *_DtaName, void *_DtaData, UDWORD _DtaFlag)
{
	// assert
	if( (dta == NULL) || (_DtaName == NULL) || (_DtaData == NULL) || (_DtaFlag == NULL) )
		return CONSOLE_ERROR_NULL;

	// cut string
	if( strlen(_DtaName) > CONSOLE_MAX_CHAR-1 )
		_DtaName[CONSOLE_MAX_CHAR] = NULL;

	// affectation
	strcpy(dta->DtaName, _DtaName);
	dta->DtaData	= _DtaData;
	dta->DtaFlag	= _DtaFlag;

	return CONSOLE_OK;
}

//*********************************************************************
//
// Console
//
//*********************************************************************

//*********************************************************************
// Name	: BMConsoleStart
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:45:48
//*********************************************************************

void BMConsoleStart(STRING *bitmap)
{
	if( MegaConsole != NULL )
		return;

	MegaConsole = (BMCONSOLE *)ALLOC2( sizeof(BMCONSOLE), BMConsoleMemory );
	memset( MegaConsole, 0, sizeof(BMCONSOLE) );	
	
	MegaConsole->CsleCurrentCmdLine	= -1;

	MegaConsole->CsleBackGroundImage	= BMImageCreate (NULL, NULL, KML_BMP, bitmap);
	MegaConsole->CsleBackGroundOffsetX  = 0.0f;
	MegaConsole->CsleBackGroundSpeedX   = 1.0f;
	MegaConsole->CsleCursorSpeed		= 0.02f;
	MegaConsole->CsleEcho				= BMCONSOLEUI_ECHOON;
	MegaConsole->CsleNbVisibleLine		= BMCONSOLEUI_NBLINEVISIBLE;
	MegaConsole->CsleCursorChar			= BMCONSOLEUI_CURSORCHAR;

	// init UI
	BMConsoleUIInit();

	BMConsoleRegisterBasicCommands();
	
	// Data
	BMConsoleCsleAddDataToConsole("cs_backspeed",	&MegaConsole->CsleBackGroundSpeedX, CSLE_PARAM_FLAG_FLOAT);
	BMConsoleCsleAddDataToConsole("cs_curspeed",	&MegaConsole->CsleCursorSpeed, CSLE_PARAM_FLAG_FLOAT);
	BMConsoleCsleAddDataToConsole("cs_nbline",		&MegaConsole->CsleNbVisibleLine, CSLE_PARAM_FLAG_DWORD);
}

//*********************************************************************
// Name	: BMConsoleEnd
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:46:03
//*********************************************************************

void BMConsoleEnd(void)
{
	BMConsoleCsleResetAllCommand	();
	BMConsoleCsleResetAllData		();
	BMConsoleCsleResetAllParam		();
	BMConsoleCsleResetAllCmdLine	();
	BMConsoleCsleResetAllOutLine	();

	FREE2(MegaConsole, BMConsoleMemory);
	MegaConsole = NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleAddCommandToConsole
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:02:49
//*********************************************************************

UDWORD BMConsoleCsleAddCommandToConsole(STRING *_CmdName, CONSOLELPFONC _CmdLpFunc)
{
	BMCONSOLECOMMAND		*scan,*newcmd;
	UDWORD					i;

	scan	=	MegaConsole->CsleCommandListe;

	for(i=0; i<MegaConsole->CsleNbCommand; i++)
	{
		if(strcmp(scan->CmdName, _CmdName) == 0 )
			return CONSOLE_ERROR_CMD_ALREADY_EXISTS;

		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}

	// chainage
	newcmd				= (BMCONSOLECOMMAND *)ALLOC2( sizeof(BMCONSOLECOMMAND), BMConsoleMemory );
	memset( newcmd, 0, sizeof(BMCONSOLECOMMAND) );

	if(scan)
		scan->LPNext	= newcmd;
	else
		MegaConsole->CsleCommandListe= newcmd;

	newcmd->LPPrevious	= scan;
	MegaConsole->CsleNbCommand++;

	// affectation
	BMConsoleCmdSetAllParam(newcmd, _CmdName, _CmdLpFunc);
	
	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BMConsoleCsleRemoveCommandFromConsole
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:03:21
//*********************************************************************

UDWORD BMConsoleCsleRemoveCommandFromConsole(STRING *_CmdName)
{
	BMCONSOLECOMMAND		*scan;
	UDWORD					i;

	scan	=	MegaConsole->CsleCommandListe;

	for(i=0; i<MegaConsole->CsleNbCommand; i++)
	{
		if(strcmp(scan->CmdName, _CmdName) == 0 )
		{
			if(scan->LPPrevious)
				scan->LPPrevious->LPNext= scan->LPNext;
			else
				MegaConsole->CsleCommandListe		= scan->LPNext;

			if(scan->LPNext)
				scan->LPNext->LPPrevious = scan->LPPrevious;

			FREE2(scan, BMConsoleMemory);
			MegaConsole->CsleNbCommand--;
			return CONSOLE_OK;
		}
		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}

	return CONSOLE_ERROR_NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleResetAllCommand
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:03:41
//*********************************************************************

void BMConsoleCsleResetAllCommand(void)
{
	// command
	BMCONSOLECOMMAND		*scan, *second;
	UDWORD					i;

	scan	=	MegaConsole->CsleCommandListe;
	for(i=0; i<MegaConsole->CsleNbCommand; i++)
	{
		second	= scan->LPNext;
		FREE2(scan, BMConsoleMemory);
		scan	= second;
	}

}

//*********************************************************************
// Name	: BMConsoleCsleFindCommand
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:16:44
//*********************************************************************

BMCONSOLECOMMAND *BMConsoleCsleFindCommand(STRING *_CmdName)
{
	BMCONSOLECOMMAND		*scan;
	UDWORD					i;

	scan	=	MegaConsole->CsleCommandListe;

	for(i=0; i<MegaConsole->CsleNbCommand; i++)
	{
		if(strcmp(scan->CmdName, _CmdName) == 0 )
		{
			return scan;
		}
		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}
	return NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleAddDataToConsole
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:17:26
//*********************************************************************

UDWORD BMConsoleCsleAddDataToConsole(STRING *_DtaName, void *_DtaData, UDWORD _DtaFlag)
{
	BMCONSOLEDATA			*scan,*newdata;
	UDWORD					i;

	scan	=	MegaConsole->CsleDataListe;

	for(i=0; i<MegaConsole->CsleNbData; i++)
	{
		if(strcmp(scan->DtaName, _DtaName) == 0 )
			return CONSOLE_ERROR_CMD_ALREADY_EXISTS;

		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}

	// chainage
	newdata				= ( BMCONSOLEDATA *)ALLOC2( sizeof(BMCONSOLEDATA), BMConsoleMemory );
	memset( newdata, 0, sizeof(BMCONSOLEDATA) );

	if(scan)
		scan->LPNext	= newdata;
	else
		MegaConsole->CsleDataListe		= newdata;
	newdata->LPPrevious	= scan;
	MegaConsole->CsleNbData++;

	// affectation
	BMConsoleDtaSetAllParam( newdata, _DtaName, _DtaData, _DtaFlag);
	
	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BMConsoleCsleRemoveDataFromConsole
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:17:59
//*********************************************************************

UDWORD BMConsoleCsleRemoveDataFromConsole(STRING *_DtaName)
{
	BMCONSOLEDATA			*scan;
	UDWORD					i;

	scan	=	MegaConsole->CsleDataListe;

	for(i=0; i<MegaConsole->CsleNbData; i++)
	{
		if(strcmp(scan->DtaName, _DtaName) == 0 )
		{
			if(scan->LPPrevious)
				scan->LPPrevious->LPNext= scan->LPNext;
			else
				MegaConsole->CsleDataListe	= scan->LPNext;
				
			if(scan->LPNext)
				scan->LPNext->LPPrevious = scan->LPPrevious;
			
			delete(scan);
			MegaConsole->CsleNbData--;
			return CONSOLE_OK;
		}
		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}

	return CONSOLE_ERROR_NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleResetAllData
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:18:36
//*********************************************************************

void BMConsoleCsleResetAllData(void)
{
	// command
	BMCONSOLEDATA			*scan, *second;
	UDWORD					i;

	scan	=	MegaConsole->CsleDataListe;
	for(i=0; i<MegaConsole->CsleNbData; i++)
	{
		second	= scan->LPNext;
		FREE2(scan, BMConsoleMemory);
		scan	= second;
	}

}

//*********************************************************************
// Name	: BMConsoleCsleFindData
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:19:04
//*********************************************************************

BMCONSOLEDATA *BMConsoleCsleFindData(STRING *_DtaName)
{
	BMCONSOLEDATA			*scan;
	UDWORD					i;

	scan	=	MegaConsole->CsleDataListe;

	for(i=0; i<MegaConsole->CsleNbData; i++)
	{
		if(strcmp(scan->DtaName, _DtaName) == 0 )
		{
			return scan;
		}
		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}
	return NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleSetAllParam
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:20:34
//*********************************************************************

void BMConsoleCsleSetAllParam(STRING *cmdline)
{
	SDWORD		startarg, endarg, sizeofarg, stringbool;

	// init
	startarg	= -1;
	endarg		= 0;
	stringbool	= false;
	
	if(!cmdline)
		return;

	BMConsoleCsleResetAllParam();

	// parse
	while( endarg <= (SDWORD)strlen(cmdline) )
	{
		if( (cmdline[endarg] != ' ') && (startarg == -1) && (cmdline[endarg] != NULL) )
		{
			if(MegaConsole->CsleNbArg<CONSOLE_MAX_ARG)
			{
				startarg = endarg;
				if(cmdline[endarg] == '"')
				{
					startarg++;
					stringbool = true;
				}
			}
		}
			
		if( ( (cmdline[endarg] == ' ') || (endarg == (SDWORD)strlen(cmdline)) ) && (startarg != -1) )
		{
			if( (stringbool) && (cmdline[endarg-1])=='"' )
				endarg--;

			if( !((stringbool) && (cmdline[endarg])!='"') )
			{
				sizeofarg				= endarg-startarg;
				if(sizeofarg>CONSOLE_MAX_CHAR-1)
					sizeofarg = CONSOLE_MAX_CHAR-1;

				MegaConsole->CsleTabArg[MegaConsole->CsleNbArg]	= (STRING *)ALLOC2(CONSOLE_MAX_CHAR,BMConsoleMemory);
				memcpy(MegaConsole->CsleTabArg[MegaConsole->CsleNbArg], &cmdline[startarg], sizeofarg);
				MegaConsole->CsleTabArg[MegaConsole->CsleNbArg][sizeofarg] = NULL;
				MegaConsole->CsleNbArg++;

				//re-init
				startarg	= -1;
				if(stringbool)
				{
					stringbool	= false;
					endarg++;
				}
			}
		}
		endarg++;
	}
}

//*********************************************************************
// Name	: BMConsoleCsleResetAllParam
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:21:03
//*********************************************************************

void BMConsoleCsleResetAllParam(void)
{
	UDWORD		i;

	for(i=0; i<MegaConsole->CsleNbArg; i++)
		FREE2(MegaConsole->CsleTabArg[i], BMConsoleMemory);

	MegaConsole->CsleNbArg = 0;
}

//*********************************************************************
// Name	: BMConsoleCsleGetParam
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:21:38
//*********************************************************************

UDWORD BMConsoleCsleGetParam(UDWORD ParamIndex, void *ParamReturn, UDWORD ParamFlag)
{
	// assert
	if( MegaConsole->CsleNbArg == 0 )
		return CONSOLE_ERROR_NULL;

	if( ( ParamIndex > MegaConsole->CsleNbArg-1 ) || (!ParamReturn) || (!ParamFlag))
		return CONSOLE_ERROR_NULL;

	// parse
	switch(ParamFlag)
	{
	case(CSLE_PARAM_FLAG_DWORD):
		(*((SDWORD *)ParamReturn)) = atol(MegaConsole->CsleTabArg[ParamIndex]);
		break;

	case(CSLE_PARAM_FLAG_FLOAT):
		(*((float *)ParamReturn)) = (float)atof(MegaConsole->CsleTabArg[ParamIndex]);
		break;

	case(CSLE_PARAM_FLAG_STRING):
		strcpy((STRING *)ParamReturn, MegaConsole->CsleTabArg[ParamIndex]);
		break;
	}

	return CONSOLE_OK;
}

//*********************************************************************
// Name	: BMConsoleCsleUpdate
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:22:03
//*********************************************************************

BOOL BMConsoleCsleUpdate(void)
{
	STRING					CmdName[CONSOLE_MAX_CHAR*2];
	STRING					SecArg[CONSOLE_MAX_CHAR];
	BMCONSOLECOMMAND		*	Cmd;
	BMCONSOLEDATA			*	Dta;
	
	if( MegaConsole->CsleNbArg < 1 )
		return false;

	BMConsoleCsleGetParam(0, CmdName, CSLE_PARAM_FLAG_STRING);
	Cmd	= BMConsoleCsleFindCommand(CmdName);
	if(Cmd)
	{
		Cmd->CmdLpFunc(MegaConsole);
		//BMConsoleCsleResetAllParam(); // debug
		return true;
	}

	// pas de fonction
	Dta = BMConsoleCsleFindData(CmdName);
	if(Dta)
	{
		if(MegaConsole->CsleNbArg > 1)
		{
			BMConsoleCsleGetParam(1, SecArg, CSLE_PARAM_FLAG_STRING);
			strcpy(CmdName, "set ");
			strcat(CmdName, Dta->DtaName);
			strcat(CmdName, " ");
			if(Dta->DtaFlag == CSLE_PARAM_FLAG_STRING)
				strcat(CmdName, "\"");
			strcat(CmdName, SecArg);
			if(Dta->DtaFlag == CSLE_PARAM_FLAG_STRING)
				strcat(CmdName, "\"");
			BMConsoleCsleSetAllParam(CmdName);
			BMConsoleCsleUpdate();
			return true;
		}
		if(MegaConsole->CsleNbArg == 1)
		{
			BMConsoleCsleGetParam(0, SecArg, CSLE_PARAM_FLAG_STRING);
			strcpy(CmdName, "set ");
			strcat(CmdName, Dta->DtaName);
			BMConsoleCsleSetAllParam(CmdName);
			BMConsoleCsleUpdate();
			return true;
		}
	}
	return false;
}

//--------------------------------
// CmdLine
//--------------------------------

//*********************************************************************
// Name	: BMConsoleCsleAddCmdLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:22:48
//*********************************************************************

void BMConsoleCsleAddCmdLine(STRING *cmdline)
{
	UDWORD		i;
	STRING	*	c;

	if( MegaConsole->CsleNbCmdLine == CONSOLE_MAX_CMDLINE )
	{
		FREE2(MegaConsole->CsleTabCmdLine[MegaConsole->CsleNbCmdLine-1], BMConsoleMemory);
		MegaConsole->CsleNbCmdLine--;
	}
	
	// allocation
	c = (char *)ALLOC2(strlen(cmdline)+1, BMConsoleMemory);

	if(MegaConsole->CsleNbCmdLine>0)
	{
		for(i = MegaConsole->CsleNbCmdLine; i > 0; i--)
		{
			MegaConsole->CsleTabCmdLine[i] = MegaConsole->CsleTabCmdLine[i-1];
		}
	}

	// new cmdline en tete !
	MegaConsole->CsleTabCmdLine[0] = c;
	strcpy(MegaConsole->CsleTabCmdLine[0], cmdline);

	// maj
	MegaConsole->CsleNbCmdLine++;
	MegaConsole->CsleCurrentCmdLine = -1;
}

//*********************************************************************
// Name	: BMConsoleCsleSetCmdLineIndex
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:23:23
//*********************************************************************

void BMConsoleCsleSetCmdLineIndex(UDWORD Flag)
{
	if(Flag == CSLE_CMD_LINE_NEXT)
	{
		if(MegaConsole->CsleCurrentCmdLine > 0)
			MegaConsole->CsleCurrentCmdLine--;
	}
	else if(Flag == CSLE_CMD_LINE_PREVIOUS)
	{
		if( MegaConsole->CsleCurrentCmdLine < (SDWORD)MegaConsole->CsleNbCmdLine-1 )
			MegaConsole->CsleCurrentCmdLine++;
	}
}

//*********************************************************************
// Name	: BMConsoleCsleGetCurrentCmdLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:24:01
//*********************************************************************

STRING* BMConsoleCsleGetCurrentCmdLine(SDWORD index)
{
	if( (index != -1) && (index < (SDWORD)BMConsoleCsleGetNbCmdLine()) )
		return MegaConsole->CsleTabCmdLine[index];

	if( MegaConsole->CsleCurrentCmdLine == -1 )
		return NULL;

	return MegaConsole->CsleTabCmdLine[MegaConsole->CsleCurrentCmdLine];
}

//*********************************************************************
// Name	: BMConsoleCsleResetAllCmdLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:24:19
//*********************************************************************

void BMConsoleCsleResetAllCmdLine(void)
{
	UDWORD		i;

	for(i=0; i<MegaConsole->CsleNbCmdLine; i++)
		FREE2(MegaConsole->CsleTabCmdLine[i], BMConsoleMemory);

	MegaConsole->CsleNbCmdLine		= 0;
	MegaConsole->CsleCurrentCmdLine	= -1;
}

//--------------------------------
// OutLine
//--------------------------------

//*********************************************************************
// Name	: BMConsoleCsleAddOutLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:22:48
//*********************************************************************

void BMConsoleCsleAddOutLine(STRING *outline)
{
	UDWORD		i;
	STRING	*	c;

	if( MegaConsole->CsleEcho == BMCONSOLEUI_ECHOOFF )
		return;

	if( MegaConsole->CsleNbOutLine == CONSOLE_MAX_OUTLINE )
	{
		FREE2(MegaConsole->CsleTabOutLine[MegaConsole->CsleNbOutLine-1], BMConsoleMemory);
		MegaConsole->CsleNbOutLine--;
	}
	
	// allocation
	c = (char *)ALLOC2(strlen(outline)+1, BMConsoleMemory);

	if(MegaConsole->CsleNbOutLine>0)
	{
		for(i = MegaConsole->CsleNbOutLine; i > 0; i--)
		{
			MegaConsole->CsleTabOutLine[i] = MegaConsole->CsleTabOutLine[i-1];
		}
	}

	// new outline en tete !
	MegaConsole->CsleTabOutLine[0] = c;
	strcpy(MegaConsole->CsleTabOutLine[0], outline);

	// maj
	MegaConsole->CsleNbOutLine++;
}

//*********************************************************************
// Name	: BMConsoleCsleGetCurrentOutLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:24:01
//*********************************************************************

STRING* BMConsoleCsleGetCurrentOutLine(SDWORD index)
{
	if( (index != -1) && (index < (SDWORD)BMConsoleCsleGetNbOutLine()) )
		return MegaConsole->CsleTabOutLine[index];

	return NULL;
}

//*********************************************************************
// Name	: BMConsoleCsleResetAllOutLine
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:24:19
//*********************************************************************

void BMConsoleCsleResetAllOutLine(void)
{
	UDWORD		i;

	for(i=0; i<MegaConsole->CsleNbOutLine; i++)
		FREE2(MegaConsole->CsleTabOutLine[i], BMConsoleMemory);

	MegaConsole->CsleNbOutLine		= 0;
}


//*********************************************************************
// Name	: BMConsoleCsleGetSameString
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								14/12/2000 15:48:15
//*********************************************************************

SDWORD BMConsoleCsleGetSameString(STRING *out, STRING *in1, STRING *in2)
{
	SDWORD		size, i;

	if( !in1 || !in2 )
		return -1;

	if( out )
		memset(out, 0, CONSOLE_MAX_CHAR );
	
	if( strlen(in1) > strlen(in2) )
		size = strlen(in2);
	else
		size = strlen(in1);


	for(i=0; i<size; i++)
	{
		if( in1[i] != in2[i] )
		{
			if ( out )
				out[i] = NULL;
			return i;
		}
		if( out )
			out[i] = in1[i];
	}

	return i;
}

//*********************************************************************
// Name	: BMConsoleCsleTabComp
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								14/12/2000 14:35:53
//*********************************************************************

void BMConsoleCsleTabComp(STRING *currentline)
{
	BMCONSOLECOMMAND	*	scan;
	BMCONSOLEDATA		*	scandata;
	SDWORD					i, result, rnew, rold = 0, cpt = 0;
	STRING					cmd				[CONSOLE_MAX_CHAR];
	STRING					prev			[CONSOLE_MAX_CHAR];
	STRING				*	firstcmda3balles;

	if( !currentline )
		return;




	memset(prev, 0, CONSOLE_MAX_CHAR);

	// scan les commandes
	scan	=	MegaConsole->CsleCommandListe;
	for(i=0; i<(SDWORD)MegaConsole->CsleNbCommand; i++)
	{
		result = BMConsoleCsleGetSameString( cmd, scan->CmdName, currentline);

		if( (result == (SDWORD)strlen(currentline)) && (result < (SDWORD)strlen(scan->CmdName)+1) )
		{
			// init a 3 balles !
			if(cpt == 0)
			{
				strcpy( prev, scan->CmdName);
				rold = strlen(scan->CmdName);
				firstcmda3balles = scan->CmdName;
			}


			// on cherche le mots le plus long
			rnew = BMConsoleCsleGetSameString(NULL, prev, scan->CmdName);
			strcpy( prev, scan->CmdName);
			if(rnew<rold)
				rold = rnew;

			if(cpt==1)
				BMConsoleCsleAddOutLine(firstcmda3balles);

			if(cpt)
				BMConsoleCsleAddOutLine(scan->CmdName);

			cpt++;
		}

		// scan
		if(scan->LPNext != NULL)
			scan = scan->LPNext;
	}

	// scan les data
	scandata	=	MegaConsole->CsleDataListe;
	for(i=0; i<(SDWORD)MegaConsole->CsleNbData; i++)
	{
		result = BMConsoleCsleGetSameString( cmd, scandata->DtaName, currentline);

		if( (result == (SDWORD)strlen(currentline)) && (result < (SDWORD)strlen(scandata->DtaName)+1) )
		{
			// init a 3 balles !
			if(cpt == 0)
			{
				strcpy( prev, scandata->DtaName);
				rold = strlen(scandata->DtaName);
				firstcmda3balles = scandata->DtaName;
			}
			

			// on cherche le mots le plus long
			rnew = BMConsoleCsleGetSameString(NULL, prev, scandata->DtaName);
			strcpy( prev, scandata->DtaName);
			if(rnew<rold)
				rold = rnew;

			if(cpt==1)
				BMConsoleCsleAddOutLine(firstcmda3balles);

			if(cpt)
				BMConsoleCsleAddOutLine(scandata->DtaName);

			cpt++;
		}

		// scan
		if(scandata->LPNext != NULL)
			scandata = scandata->LPNext;
	}




	// affectation
	if( cpt == 1 )
	{
		strcpy( currentline, firstcmda3balles);
		strcat( currentline, " ");
	}
	else if( cpt > 1 )
	{
		strcpy( currentline, prev);
		for(i=rold; i<(SDWORD)strlen(currentline); i++)
			currentline[i] = NULL;
	}
}


//--------------------------------
// Extern User
//--------------------------------


//*********************************************************************
// Name	: BMConsoleCsleExterneUpdate
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:24:54
//*********************************************************************

void BMConsoleCsleExterneUpdate(STRING *_CmdLine)
{
	BMConsoleCsleWriteString(_CmdLine);
	BMConsoleCsleSetAllParam(_CmdLine);
	if(BMConsoleCsleUpdate())
		BMConsoleCsleAddCmdLine	(_CmdLine);
	else
		BMConsoleCsleWriteString(BMCONSOLE_BADCOMMAND);

}
