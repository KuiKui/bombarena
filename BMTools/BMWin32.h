//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMWin32_H__
#define	__BMWin32_H__

#include "BMGlobalInclude.h"
#include "BMClientServer.h"
#include "BMGlobalInit.h"
#include "BMMap.h"
#include "BMUIOS.h"

//--------------------------------
// Variables
//--------------------------------


//--------------------------------
// Functions Console
//--------------------------------

extern	void	BMWin32ConsoleUpdate	(char flagrepetition = 0);

//--------------------------------
// Fonctions Timer
//--------------------------------

#include "BmTimer.h"

//--------------------------------
// Keyboard functions
//--------------------------------
#include "BMWin32Keys.h"

//--------------------------------
// Dll export
//--------------------------------
#define EXPORT_DLL __declspec(dllexport)

EXPORT_DLL int BomberManMain (HINSTANCE hInst,HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow);


#endif // __BMWin32_H__