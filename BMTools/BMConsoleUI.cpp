//--------------------------------
// Includes
//--------------------------------

#include	"BMConsole.h"

//--------------------------------
// Gloables
//--------------------------------

UDWORD		BMConsoleUIVisible;
STRING		BMConsoleUICommand[CONSOLE_MAX_CMDLINE_CHAR];
SDWORD		BMConsoleUICurrentChar;
SDWORD		BMConsoleUIStartOutLine;

//*********************************************************************
// Name	: BMConsoleUIInit
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 10:50:58
//*********************************************************************

void BMConsoleUIInit(void)
{
	BMConsoleUIVisible		= BMCONSOLEUI_INVISIBLE;
	memset(BMConsoleUICommand, 0, CONSOLE_MAX_CMDLINE_CHAR);
	BMConsoleUICurrentChar	= 0;
	BMConsoleUIStartOutLine = 0;
}


//*********************************************************************
// Name	: BMConsoleUIShow
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 10:47:46
//*********************************************************************

void BMConsoleUIShow(UBYTE _flag)
{
	if(_flag == BMCONSOLEUI_INVISIBLE)
	{
		BMConsoleUIVisible = BMCONSOLEUI_INVISIBLE;
	}
	else if(_flag == BMCONSOLEUI_VISIBLE)
	{
		BMConsoleUIVisible = BMCONSOLEUI_VISIBLE;
	}
}

//*********************************************************************
// Name	: BMConsoleUIAddChar
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 10:54:01
//*********************************************************************

void BMConsoleUIAddChar(UBYTE c, SDWORD flag)
{
	SDWORD				i;
	STRING				param[CONSOLE_MAX_CHAR];

	if( BMUICurrentFont == NULL)
		return;

	if( BMConsoleUIVisible == BMCONSOLEUI_INVISIBLE )
		return;

	if( (flag == BMCONSOLEUI_EXE) && (BMConsoleUICurrentChar>0) )
	{
		BMConsoleCsleExterneUpdate(BMConsoleUICommand);
		BMConsoleUIAddChar(NULL, BMCONSOLEUI_DELETELINE);
		return;
	}

	if( (flag == BMCONSOLEUI_BACKSPACE) && (BMConsoleUICurrentChar>0) )
	{
		BMConsoleUICurrentChar--;
		for ( i=BMConsoleUICurrentChar; i<(SDWORD)strlen(BMConsoleUICommand); i++)
		{
			BMConsoleUICommand[i] = BMConsoleUICommand[i+1];

		}

		return;
	}

	if ( flag == BMCONSOLEUI_SUP )
	{
		for ( i=BMConsoleUICurrentChar; i<(SDWORD)strlen(BMConsoleUICommand); i++)
		{
			BMConsoleUICommand[i] = BMConsoleUICommand[i+1];
		}
		return;
	}

	if( (flag == BMCONSOLEUI_DELETELINE) && (BMConsoleUICurrentChar>0) )
	{
		for(i=0; i<BMConsoleUICurrentChar; i++)
			BMConsoleUICommand[i] = NULL;
		BMConsoleUICurrentChar = 0;
		return;
	}

	if ( flag == BMCONSOLEUI_PREVCMDLINE )
	{
		BMConsoleCsleSetCmdLineIndex(CSLE_CMD_LINE_PREVIOUS);
		if(MegaConsole->CsleCurrentCmdLine != -1)
			strcpy(BMConsoleUICommand, BMConsoleCsleGetCurrentCmdLine());
		BMConsoleUICurrentChar = strlen(BMConsoleUICommand);
		return;
	}

	if ( flag == BMCONSOLEUI_NEXTCMDLINE )
	{
		BMConsoleCsleSetCmdLineIndex(CSLE_CMD_LINE_NEXT);
		if(MegaConsole->CsleCurrentCmdLine != -1)
			strcpy(BMConsoleUICommand, BMConsoleCsleGetCurrentCmdLine());
		BMConsoleUICurrentChar = strlen(BMConsoleUICommand);
		return;
	}

	if ( flag == BMCONSOLEUI_CURS_LEFT )
	{
		if(BMConsoleUICurrentChar>0)
			BMConsoleUICurrentChar--;
		return;
	}

	if ( flag == BMCONSOLEUI_CURS_RIGHT )
	{
		if(BMConsoleUICurrentChar<(SDWORD)strlen(BMConsoleUICommand))
			BMConsoleUICurrentChar++;
		return;
	}

	if ( flag == BMCONSOLEUI_ENDLINE )
	{
		BMConsoleUICurrentChar = strlen(BMConsoleUICommand);
		return;
	}

	if ( flag == BMCONSOLEUI_STARTLINE )
	{
		BMConsoleUICurrentChar = 0;
		return;
	}

	if ( flag == BMCONSOLEUI_OUTLINEUP )
	{
		if ( BMConsoleUIStartOutLine < (SDWORD)BMConsoleCsleGetNbOutLine()-1)
			BMConsoleUIStartOutLine++;
		return;
	}
	
	if ( flag == BMCONSOLEUI_OUTLINEDOWN )
	{
		if ( BMConsoleUIStartOutLine > 0 )
			BMConsoleUIStartOutLine--;
		return;
	}

	if ( flag == BMCONSOLEUI_TABCOMP )
	{	
		BMConsoleCsleSetAllParam(BMConsoleUICommand);
		strcpy(BMConsoleUICommand, "");

		for(i=0; i<(SDWORD)BMConsoleCsleGetNbParam()-1; i++)
		{
			BMConsoleCsleGetParam(i, param, CSLE_PARAM_FLAG_STRING);
			strcat(BMConsoleUICommand, param);
			strcat(BMConsoleUICommand, " ");
		}

		if( BMConsoleCsleGetNbParam()>0 )
			BMConsoleCsleGetParam(i, param, CSLE_PARAM_FLAG_STRING);
		else
			strcpy(param, "");

		BMConsoleCsleTabComp(param);
		strcat(BMConsoleUICommand, param);

		BMConsoleUICurrentChar = strlen(BMConsoleUICommand);
		return;
	}

	
	
	if( c == NULL )
		return;

	if(strlen(BMConsoleUICommand) >= (UDWORD)GlobalResX/BMUICurrentFont->SizeX)
		return;

	BMConsoleUICommand[BMConsoleUICurrentChar] = c;
	BMConsoleUICurrentChar++;
}

//*********************************************************************
// Name	: BMConsoleUIUpdate
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 11:22:53
//*********************************************************************

void BMConsoleUIUpdate(void)
{
	UDWORD			i, y=0, x, tailley;
	SDWORD			dstart, dend;
	float			offsetx;
	UBYTE			car;
	static float	curvis=0;

	if (BMUICurrentFont == NULL)
		return;

	// cut
	if( MegaConsole->CsleNbVisibleLine<2 )
		MegaConsole->CsleNbVisibleLine = 2;
	
	// draw background
	tailley = (MegaConsole->CsleNbVisibleLine)*(BMUICurrentFont->SizeY + BMUICurrentFont->FontSpaceY)+2;

	MegaConsole->CsleBackGroundOffsetX += MegaConsole->CsleBackGroundSpeedX;
	if(MegaConsole->CsleBackGroundOffsetX>=GlobalResX)
		MegaConsole->CsleBackGroundOffsetX = 0.0f;
	if(MegaConsole->CsleBackGroundOffsetX<0)
		MegaConsole->CsleBackGroundOffsetX = GlobalResX;

	x = (UDWORD)MegaConsole->CsleBackGroundOffsetX;

	offsetx = (float)( (float)(GlobalResX-x) * (float)((float)MegaConsole->CsleBackGroundImage->Largeur / (float)GlobalResX) );

	if(offsetx<1)
		offsetx = 1;

	KMLStretchBlt(	NULL, x, 0, GlobalResX-x, tailley, MegaConsole->CsleBackGroundImage, 0, 0, (UDWORD)offsetx, MegaConsole->CsleBackGroundImage->Hauteur );
	KMLStretchBlt(	NULL, 0, 0, x, tailley, MegaConsole->CsleBackGroundImage, (UDWORD)offsetx, 0, MegaConsole->CsleBackGroundImage->Largeur - (UDWORD)offsetx, MegaConsole->CsleBackGroundImage->Hauteur );


	// draw all strings
	if(BMConsoleUIStartOutLine == 0)
	{
		dstart	= 0;
		dend	= 0;
	}
	else if(BMConsoleUIStartOutLine > 0)
	{
		dstart	= 1;
		dend	= -1;
	}

	for(i=BMConsoleUIStartOutLine; i< (UDWORD)(MegaConsole->CsleNbVisibleLine-1+BMConsoleUIStartOutLine+dend); i++)
	{
		if(i<BMConsoleCsleGetNbOutLine())
		{
			y = ((MegaConsole->CsleNbVisibleLine-2) - i + BMConsoleUIStartOutLine - dstart )  * (BMUICurrentFont->SizeY + BMUICurrentFont->FontSpaceY );
			BMUIFontDrawString( BMConsoleCsleGetCurrentOutLine( i ), CONSOLE_MARGIN, y, NULL);
		}
		if(BMConsoleUIStartOutLine > 0)
		{
			y = (MegaConsole->CsleNbVisibleLine-2)  * (BMUICurrentFont->SizeY + BMUICurrentFont->FontSpaceY);
			BMUIFontDrawString( "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^", CONSOLE_MARGIN, y, NULL);
		}
	}
	
	// clignot
	curvis += MegaConsole->CsleCursorSpeed;


	y = (MegaConsole->CsleNbVisibleLine-1) * (BMUICurrentFont->SizeY + BMUICurrentFont->FontSpaceY);
	
	if((UDWORD)curvis & 1)
	{
		car = BMConsoleUICommand[BMConsoleUICurrentChar];
		BMConsoleUICommand[BMConsoleUICurrentChar] = MegaConsole->CsleCursorChar;
		if(car == NULL)
		{
			for(i=BMConsoleUICurrentChar+1; i<CONSOLE_MAX_CMDLINE_CHAR; i++)
				BMConsoleUICommand[i] = NULL;
		}
	}

	BMUIFontDrawString( BMConsoleUICommand, CONSOLE_MARGIN, y, NULL);

	if((UDWORD)curvis & 1)
	{
		BMConsoleUICommand[BMConsoleUICurrentChar] = car;
	}
}

//*********************************************************************
// Name	: BMConsoleCsleWriteString
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/12/2000 14:25:15
//*********************************************************************

void BMConsoleCsleWriteString(const char *fmt,...)
{
	va_list		vlst;
	char		str[512];

	if (BMUICurrentFont == NULL)
		return;

	va_start(vlst,fmt);
	vsprintf(str,fmt,vlst);

	// cast
	str[(UDWORD)GlobalResX/BMUICurrentFont->SizeX] = NULL;

	// affichage
	BMConsoleCsleAddOutLine	(str);

	va_end(vlst);
}