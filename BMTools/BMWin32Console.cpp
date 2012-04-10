//--------------------------------
// Includes
//--------------------------------

#include "BMGlobalInclude.h"
#include "BMConsole.h"

//--------------------------------
// predeclaration de fonctions
//--------------------------------

UBYTE BMWin32UpdateKeyboard (unsigned char touche,char flag);

//--------------------------------
// Define
//--------------------------------

#define			BMWIN32REPETITION		1

#define			BM_WAITFIRST			1
#define			BM_WAITSECOND			2
//--------------------------------
// Globales
//--------------------------------

unsigned long	BMWin32KeyWaitRepetition = 500;		//temps d'attente avant repetition du premier caractere
unsigned long	BMWin32KeyAddCharacter	 = 30;		//temps d'attente avant repetition des autres
unsigned char	BMWin32LastKey			 = 0;		//derniere touche enfoncée donc affichée
unsigned long	BMWin32TimeElapsed		 = 0;
unsigned long	BMWin32EtatRepet		 = BM_WAITFIRST;


//*********************************************************************
// Name	: BMWin32ConsoleTransCode
//---------------------------------------------------------------------
// In	: flag (pour savoir si utilise la repetition)
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 15:20:02
//*********************************************************************

UBYTE BMWin32ConsoleTransCode(char flag)
{
	SBYTE		Shift;

	// shift
	Shift = 0;
	if( KMLControlKeysPress(BM_RSHIFT, 1) || KMLControlKeysPress(BM_LSHIFT, 1) )
		Shift = 1;

	
	// nombre
	if(BMWin32UpdateKeyboard(BM_0, flag))
		return ( Shift ? '0' : 'à' );
	if(BMWin32UpdateKeyboard(BM_1, flag))
		return ( Shift ? '1' : '&' );
	if(BMWin32UpdateKeyboard(BM_2, flag))
		return ( Shift ? '2' : 'é' );
	if(BMWin32UpdateKeyboard(BM_3, flag))
		return ( Shift ? '3' : '"' );
	if(BMWin32UpdateKeyboard(BM_4, flag))
		return ( Shift ? '4' : '\'' );
	if(BMWin32UpdateKeyboard(BM_5, flag))
		return ( Shift ? '5' : '(' );
	if(BMWin32UpdateKeyboard(BM_6, flag))
		return ( Shift ? '6' : '-' );
	if(BMWin32UpdateKeyboard(BM_7, flag))
		return ( Shift ? '7' : 'è' );
	if(BMWin32UpdateKeyboard(BM_8, flag))
		return ( Shift ? '8' : '_' );
	if(BMWin32UpdateKeyboard(BM_9, flag))
		return ( Shift ? '9' : 'ç' );
	if(BMWin32UpdateKeyboard(BM_MINUS, flag))
		return ( Shift ? '°' : ')' );
	if(BMWin32UpdateKeyboard(BM_EQUALS, flag))
		return ( Shift ? '+' : '=' );


	// speciaux
	if(BMWin32UpdateKeyboard(BM_SPACE, flag))
		return ' ';
	if(BMWin32UpdateKeyboard(BM_PERIOD, flag))
		return ( Shift ? '/' : ':' );
	if(BMWin32UpdateKeyboard(BM_LBRACKET, flag))
		return ( Shift ? '¨' : '^' );
	if(BMWin32UpdateKeyboard(BM_RBRACKET, flag))
		return ( Shift ? '£' : '$' );
	if(BMWin32UpdateKeyboard(BM_BACKSLASH, flag))
		return ( Shift ? 'µ' : '*' );
	if(BMWin32UpdateKeyboard(BM_APOSTROPHE, flag))
		return ( Shift ? '%' : 'ù' );
	if(BMWin32UpdateKeyboard(BM_SEMICOLON, flag))
		return ( Shift ? 'M' : 'm' );
	if(BMWin32UpdateKeyboard(BM_COMMA, flag))
		return ( Shift ? '.' : ';' );
	if(BMWin32UpdateKeyboard(BM_SLASH, flag))
		return ( Shift ? '§' : '!' );
	if(BMWin32UpdateKeyboard(BM_MINUSMAXUS, flag))
		return ( Shift ? '>' : '<' );

 
	// numeric paddle
	if(BMWin32UpdateKeyboard(BM_MULTIPLY, flag))
		return '*';
	if(BMWin32UpdateKeyboard(BM_SUBTRACT, flag))
		return '-';
	if(BMWin32UpdateKeyboard(BM_ADD, flag))
		return '+';
	if(BMWin32UpdateKeyboard(BM_DIVIDE, flag))
		return '/';
	if(BMWin32UpdateKeyboard(BM_DECIMAL, flag))
		return '.'; 
	if(BMWin32UpdateKeyboard(BM_NUMPAD0, flag))
		return '0';
	if(BMWin32UpdateKeyboard(BM_NUMPAD1, flag))
		return '1';
	if(BMWin32UpdateKeyboard(BM_NUMPAD2, flag))
		return '2';
	if(BMWin32UpdateKeyboard(BM_NUMPAD3, flag))
		return '3';
	if(BMWin32UpdateKeyboard(BM_NUMPAD4, flag))
		return '4';
	if(BMWin32UpdateKeyboard(BM_NUMPAD5, flag))
		return '5';
	if(BMWin32UpdateKeyboard(BM_NUMPAD6, flag))
		return '6';
	if(BMWin32UpdateKeyboard(BM_NUMPAD7, flag))
		return '7';
	if(BMWin32UpdateKeyboard(BM_NUMPAD8, flag))
		return '8';
	if(BMWin32UpdateKeyboard(BM_NUMPAD9, flag))
		return '9';



	// lettre
	if(BMWin32UpdateKeyboard(BM_A, flag))
		return ( Shift ? 'Q' : 'q' );
	if(BMWin32UpdateKeyboard(BM_B, flag))
		return ( Shift ? 'B' : 'b' );
	if(BMWin32UpdateKeyboard(BM_C, flag))
		return ( Shift ? 'C' : 'c' );
	if(BMWin32UpdateKeyboard(BM_D, flag))
		return ( Shift ? 'D' : 'd' );
	if(BMWin32UpdateKeyboard(BM_E, flag))
		return ( Shift ? 'E' : 'e' );
	if(BMWin32UpdateKeyboard(BM_F, flag))
		return ( Shift ? 'F' : 'f' );
	if(BMWin32UpdateKeyboard(BM_G, flag))
		return ( Shift ? 'G' : 'g' );
	if(BMWin32UpdateKeyboard(BM_H, flag))
		return ( Shift ? 'H' : 'h' );
	if(BMWin32UpdateKeyboard(BM_I, flag))
		return ( Shift ? 'I' : 'i' );
	if(BMWin32UpdateKeyboard(BM_J, flag))
		return ( Shift ? 'J' : 'j' );
	if(BMWin32UpdateKeyboard(BM_K, flag))
		return ( Shift ? 'K' : 'k' );
	if(BMWin32UpdateKeyboard(BM_L, flag))
		return ( Shift ? 'L' : 'l' );
	if(BMWin32UpdateKeyboard(BM_M, flag))
		return ( Shift ? '?' : ',' );
	if(BMWin32UpdateKeyboard(BM_N, flag))
		return ( Shift ? 'N' : 'n' );
	if(BMWin32UpdateKeyboard(BM_O, flag))
		return ( Shift ? 'O' : 'o' );
	if(BMWin32UpdateKeyboard(BM_P, flag))
		return ( Shift ? 'P' : 'p' );
	if(BMWin32UpdateKeyboard(BM_Q, flag))
		return ( Shift ? 'A' : 'a' );
	if(BMWin32UpdateKeyboard(BM_R, flag))
		return ( Shift ? 'R' : 'r' );
	if(BMWin32UpdateKeyboard(BM_S, flag))
		return ( Shift ? 'S' : 's' );
	if(BMWin32UpdateKeyboard(BM_T, flag))
		return ( Shift ? 'T' : 't' );
	if(BMWin32UpdateKeyboard(BM_U, flag))
		return ( Shift ? 'U' : 'u' );
	if(BMWin32UpdateKeyboard(BM_V, flag))
		return ( Shift ? 'V' : 'v' );
	if(BMWin32UpdateKeyboard(BM_W, flag))
		return ( Shift ? 'Z' : 'z' );
	if(BMWin32UpdateKeyboard(BM_X, flag))
		return ( Shift ? 'X' : 'x' );
	if(BMWin32UpdateKeyboard(BM_Y, flag))
		return ( Shift ? 'Y' : 'y' );
	if(BMWin32UpdateKeyboard(BM_Z, flag))
		return ( Shift ? 'W' : 'w' );

	return 0;
}

//*********************************************************************
// Name	: BMWin32ConsoleUpdate
//---------------------------------------------------------------------
// In	: flagrepetition (pour savoir si utilise la repetition)
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								12/12/2000 16:03:10
//*********************************************************************

void BMWin32ConsoleUpdate(char flagrepetition)
{
	UBYTE	input;
	SDWORD	flag;

	input = BMWin32ConsoleTransCode(flagrepetition);


	// In/Out
	if( BMWin32UpdateKeyboard (BM_GRAVE, 0) )
	{
		if ( BMConsoleUIVisible == BMCONSOLEUI_INVISIBLE )
			BMConsoleUIVisible = BMCONSOLEUI_VISIBLE;
		else
			BMConsoleUIVisible = BMCONSOLEUI_INVISIBLE;
	}

	// test
	if( BMConsoleUIVisible == BMCONSOLEUI_INVISIBLE )
		return;


	// INSTRUCTION
	if( BMWin32UpdateKeyboard(BM_BACK, flagrepetition) )
		flag = BMCONSOLEUI_BACKSPACE;
	else if( BMWin32UpdateKeyboard(BM_RETURN, 0)  || BMWin32UpdateKeyboard(BM_NUMPADENTER, 0) )
		flag = BMCONSOLEUI_EXE;
	else if( BMWin32UpdateKeyboard(BM_DOWN, 0) )
		flag = BMCONSOLEUI_NEXTCMDLINE;
	else if( BMWin32UpdateKeyboard(BM_UP, 0) )
		flag = BMCONSOLEUI_PREVCMDLINE;
	else if( BMWin32UpdateKeyboard(BM_LEFT, flagrepetition) )
		flag = BMCONSOLEUI_CURS_LEFT;
	else if( BMWin32UpdateKeyboard(BM_RIGHT, flagrepetition) )
		flag = BMCONSOLEUI_CURS_RIGHT;
	else if( BMWin32UpdateKeyboard(BM_END, 0) )
		flag = BMCONSOLEUI_ENDLINE;
	else if( BMWin32UpdateKeyboard(BM_HOME, 0) )
		flag = BMCONSOLEUI_STARTLINE;
	else if( BMWin32UpdateKeyboard(BM_DELETE, 0) )
		flag = BMCONSOLEUI_SUP;
	else if( BMWin32UpdateKeyboard(BM_PRIOR, flagrepetition) )
		flag = BMCONSOLEUI_OUTLINEUP;
	else if( BMWin32UpdateKeyboard(BM_NEXT, flagrepetition) )
		flag = BMCONSOLEUI_OUTLINEDOWN;
	else if( BMWin32UpdateKeyboard(BM_TAB, 0) )
		flag = BMCONSOLEUI_TABCOMP;


	BMConsoleUIAddChar( input, flag );

	BMConsoleUIUpdate();
}

//*********************************************************************
//* FUNCTION: BMWin32UpdateKeyboard
//*--------------------------------------------------------------------
//* DESCRIPT: prend en entree la touche et dit si elle doit etre retourné
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : touche, flag
//*--------------------------------------------------------------------
//* OUT     : = TRUE si envoi, FALSE sinon
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    14/12/2000 18:36:10
//* REVISION:									
//*********************************************************************
UBYTE BMWin32UpdateKeyboard (unsigned char touche,char flag)
{
	if (flag & BMWIN32REPETITION)															//on gere la repetition
	{
		if (BMWin32LastKey == touche)														//si la derniere touche est la meme que celle qu'on teste
		{
			if (KMLControlKeysPress(touche, 1))												//alors test si enfoncée
			{
				if (BMWin32EtatRepet == BM_WAITFIRST)										//test l'etat de la repetition (en attente de la premiere repet ou ajout de carac)
				{
					if (GetTickCount () - BMWin32TimeElapsed > BMWin32KeyWaitRepetition)
					{
						BMWin32TimeElapsed += BMWin32KeyWaitRepetition;
						BMWin32EtatRepet	= BM_WAITSECOND;
						
						return (TRUE);
					}
				}
				else if (BMWin32EtatRepet == BM_WAITSECOND)
				{
					if (GetTickCount () - BMWin32TimeElapsed > BMWin32KeyAddCharacter)
					{
						BMWin32TimeElapsed += BMWin32KeyAddCharacter;

						return (TRUE);
					}
				}
			}
			else																			//si la touche n'est plus enfoncé alors on desactive la derniere enfoncée
				BMWin32LastKey = 0;
		}
		else																				
		{
			if (KMLControlKeysPress(touche))												//si ce n'est pas la meme touche que la derniere alors on teste si elle est enfoncée
			{
				BMWin32TimeElapsed	= GetTickCount ();										//on met a jour les infos pour la repetition
				BMWin32LastKey		= touche;
				BMWin32EtatRepet	= BM_WAITFIRST;

				return (TRUE);
			}

		}
		return (FALSE);
	}
	else																					//on ne gere pas les repetitions
		return (KMLControlKeysPress (touche));
}
