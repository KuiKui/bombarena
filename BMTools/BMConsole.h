//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMConsole_H__
#define	__BMConsole_H__


#include	"BMGlobalInclude.h"
#include	"BMUIFont.h"
#include	"BMWin32.h"

//--------------------------------
// Defines
//--------------------------------

#define				BMCONSOLE_VER						"0.1"


#define				CONSOLE_MAX_CHAR					64
#define				CONSOLE_MAX_ARG						8
#define				CONSOLE_MAX_CMDLINE					16
#define				CONSOLE_MAX_CMDLINE_CHAR			128
#define				CONSOLE_MAX_OUTLINE					32
#define				CONSOLE_MARGIN						5

#define				CONSOLE_OK							0
#define				CONSOLE_ERROR_NULL					1
#define				CONSOLE_ERROR_CMD_ALREADY_EXISTS	2
#define				CONSOLE_ERROR_DTA_ALREADY_EXISTS	3

#define				CSLE_PARAM_FLAG_DWORD				1
#define				CSLE_PARAM_FLAG_FLOAT				2
#define				CSLE_PARAM_FLAG_STRING				3

#define				CSLE_CMD_LINE_PREVIOUS				1
#define				CSLE_CMD_LINE_NEXT					2

#define				BMCONSOLE_BADCOMMAND				"Mauvaise commande"


// UI
#define				BMCONSOLEUI_INVISIBLE				0
#define				BMCONSOLEUI_VISIBLE					1
#define				BMCONSOLEUI_ECHOOFF					0
#define				BMCONSOLEUI_ECHOON					1
#define				BMCONSOLEUI_NBLINEVISIBLE			15

#define				BMCONSOLEUI_BACKSPACE				1
#define				BMCONSOLEUI_DELETELINE				2
#define				BMCONSOLEUI_EXE						3
#define				BMCONSOLEUI_NEXTCMDLINE				4
#define				BMCONSOLEUI_PREVCMDLINE				5
#define				BMCONSOLEUI_CURS_LEFT				6
#define				BMCONSOLEUI_CURS_RIGHT				7
#define				BMCONSOLEUI_ENDLINE					8
#define				BMCONSOLEUI_STARTLINE				9
#define				BMCONSOLEUI_SUP						10
#define				BMCONSOLEUI_OUTLINEUP				11
#define				BMCONSOLEUI_OUTLINEDOWN				12
#define				BMCONSOLEUI_TABCOMP					13
#define				BMCONSOLEUI_CURSORCHAR				'>'

// SCRIPT
#define				BMCONSOLE_SCRIPT_MAX_LINE_SIZE		128
#define				BMCONSOLE_SCRIPT_NOFILE				0
#define				BMCONSOLE_SCRIPT_LOADED				1
#define				BMCONSOLE_SCRIPT_SAVED				2
#define				BMCONSOLE_FILE_READ					3
#define				BMCONSOLE_SCRIPT_BADFILE			4
#define				BMCONSOLE_SCRIPT_COMMENT_CHAR		';'
#define				BMCONSOLE_SCRIPT_HEADER				"#BMConsole Script"
//--------------------------------
// Typedef !
//--------------------------------

typedef			struct _BMCONSOLECOMMAND_TAG	BMCONSOLECOMMAND;
typedef			struct _BMCONSOLEDATA_TAG		BMCONSOLEDATA;
typedef			struct _BMCONSOLE_TAG			BMCONSOLE;

extern	BMCONSOLE		*	MegaConsole;


//--------------------------------
// Command Pointer
//--------------------------------

typedef			UDWORD					(*CONSOLELPFONC)	(BMCONSOLE *console);

//--------------------------------
// Console Command
//--------------------------------

struct _BMCONSOLECOMMAND_TAG{
	CONSOLELPFONC		CmdLpFunc;
	STRING				CmdName[CONSOLE_MAX_CHAR];
	BMCONSOLECOMMAND*	LPNext;
	BMCONSOLECOMMAND*	LPPrevious;
};

UDWORD			BMConsoleCmdSetAllParam		(STRING *_CmdName, CONSOLELPFONC _CmdLpFunc);



//--------------------------------
// ConsoleData
//--------------------------------

struct _BMCONSOLEDATA_TAG{
	void			*	DtaData;
	STRING				DtaName[CONSOLE_MAX_CHAR];
	UDWORD				DtaFlag;
	BMCONSOLEDATA	*	LPNext;
	BMCONSOLEDATA	*	LPPrevious;
};

UDWORD			BMConsoleDtaSetAllParam		(STRING * _DtaName, void *_DtaData, UDWORD _DtaFlag);

//--------------------------------
// Console
//--------------------------------

struct _BMCONSOLE_TAG{
	//BOOL				CsleVisib;
	BMCONSOLEDATA	*	CsleDataListe;
	UDWORD				CsleNbData;
	BMCONSOLECOMMAND*	CsleCommandListe;
	UDWORD				CsleNbCommand;
	STRING			*	CsleTabArg		[CONSOLE_MAX_ARG];
	UDWORD				CsleNbArg;
	STRING			*	CsleTabCmdLine	[CONSOLE_MAX_CMDLINE];
	UDWORD				CsleNbCmdLine;
	SDWORD				CsleCurrentCmdLine;
	STRING			*	CsleTabOutLine	[CONSOLE_MAX_OUTLINE];
	UDWORD				CsleNbOutLine;
	KMLIMAGE		*	CsleBackGroundImage;
	float				CsleBackGroundOffsetX;
	float				CsleBackGroundSpeedX;
	float				CsleCursorSpeed;
	UBYTE				CsleCursorChar;
	SDWORD				CsleEcho;
	SDWORD				CsleNbVisibleLine;
};


//--------------------------------
// Intern commands
//--------------------------------

//start / end
void				BMConsoleStart						(STRING *bitmap);
void				BMConsoleEnd						(void);
// param
void				BMConsoleCsleSetAllParam			(STRING *cmdline);
void				BMConsoleCsleResetAllParam			(void);
// cmdline
void				BMConsoleCsleAddCmdLine				(STRING *cmdline);
void				BMConsoleCsleResetAllCmdLine		(void);
// update
BOOL				BMConsoleCsleUpdate					(void);
// add
UDWORD				BMConsoleCsleAddCommandToConsole	(STRING *_CmdName, CONSOLELPFONC _CmdLpFunc);
UDWORD				BMConsoleCsleAddDataToConsole		(STRING *_DtaName, void *_DtaData, UDWORD _DtaFlag);
// remove
UDWORD				BMConsoleCsleRemoveCommandFromConsole(STRING *_CmdName);
UDWORD				BMConsoleCsleRemoveDataFromConsole	(STRING *_DtaName);
// reset
void				BMConsoleCsleResetAllCommand		(void);
void				BMConsoleCsleResetAllData			(void);
// find
BMCONSOLECOMMAND*	BMConsoleCsleFindCommand			(STRING *_CmdName);
BMCONSOLEDATA	*	BMConsoleCsleFindData				(STRING *_DtaName);
// cmdline
STRING			*	BMConsoleCsleGetCurrentCmdLine		(SDWORD index=-1);
static inline \
UDWORD				BMConsoleCsleGetNbCmdLine			(void)					{ return MegaConsole->CsleNbCmdLine;}
void				BMConsoleCsleSetCmdLineIndex		(UDWORD Flag);
void				BMConsoleCsleResetAllCmdLine		(void);
// outline
void				BMConsoleCsleAddOutLine				(STRING *outline);
STRING			*	BMConsoleCsleGetCurrentOutLine		(SDWORD index=-1);
static inline \
UDWORD				BMConsoleCsleGetNbOutLine			(void)					{ return MegaConsole->CsleNbOutLine;}
void				BMConsoleCsleResetAllOutLine		(void);


//--------------------------------
// Utils
//--------------------------------

void				BMConsoleCsleWriteString			(const char *fmt,...);
UDWORD				BMConsoleCsleGetParam				(UDWORD ParamIndex, void *ParamReturn, UDWORD ParamFlag);
static inline \
UDWORD				BMConsoleCsleGetNbParam				(void)					{return MegaConsole->CsleNbArg;}
void				BMConsoleCsleExterneUpdate			(STRING *_CmdLine);
void				BMConsoleCsleTabComp				(STRING *currentline);

//--------------------------------
// Script
//--------------------------------

SDWORD				BMConsoleScriptLoad					(STRING *filename);
SDWORD				BMConsoleScriptSave					(STRING *filename);
SDWORD				BMConsoleReadFile					(STRING *filename);


//--------------------------------
// Extern Commands
//--------------------------------

void				BMConsoleRegisterBasicCommands		(void);

//--------------------------------
// UI
//--------------------------------

extern	void		BMConsoleUIInit		(void);
extern	void		BMConsoleUIShow		(UBYTE _flag);
extern	void		BMConsoleUIAddChar	(UBYTE c, SDWORD flag);
extern	void		BMConsoleUIUpdate	(void);


extern	UDWORD		BMConsoleUIVisible;
extern	SDWORD		BMConsoleUIEcho;


//--------------------------------
// Debug
//--------------------------------

extern	SDWORD		BMConsoleMemory;

#endif	//__BMConsole_H__