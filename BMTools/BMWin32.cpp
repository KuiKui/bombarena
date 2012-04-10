//#include "BMConsole.h"
//#include "BMUIFont.h"
#include "BMWin32.h"
//#include "BMAnims.h"

#define WIN32_LEAN_AND_MEAN

extern DWORD KMLBackColor;

//***************************************************************
//	Variables globales
//***************************************************************
MSG			Msg;
BOOL		bactive;
UDWORD		KMLIsEnoughMemory;

HWND		hWnd;
HINSTANCE	hInstance;

//***************************************************************

LRESULT CALLBACK WndProc (HWND,UINT,WPARAM,LPARAM);
bool InitWindow(HWND &,HINSTANCE,int,int,int,bool = true);

int BomberManMain (HINSTANCE hInst,HINSTANCE hPrevInst, LPSTR lpszCmdLine, int nCmdShow)
{
	//***************************************************************
	//	Initialisation de la fenêtre Windows
	//***************************************************************
	if (!InitWindow(hWnd, hInst, nCmdShow, 640, 480, false)) //Initialisation d'une fenetre Windows la variable hMainWnd est déclaré dans la lib et doit être passé à cette fonction
		return 0;

	//***************************************************************
	//	Initialisations generales
	//***************************************************************
	if( !BMGlobalStart(hInst))
		return -1;

	BomberMode = BM_MODE_MENU_INIT;

	//***************************************************************
	//	Corps principal du programme
	//***************************************************************
	while (1)
	{
		while (PeekMessage(&Msg,NULL,0,0,PM_NOREMOVE))	//Gestion des messages envoyés à la fenêtre
		{
			if(!GetMessage(&Msg,NULL,0,0))				//Si GetMessage renvoie false, c'est que la fenetre doit être détruite
			{
                //--------------------------------
                // BMGlobalEnd
                //--------------------------------
				BMGlobalEnd();

				return Msg.wParam;
			}
			TranslateMessage(&Msg);						//Sinon on envoie le message à la fonction CALLBACK de gestion des messages WndProc
			DispatchMessage(&Msg);
		}

		if (bactive)									//Si bactive = true, alors l'application est active
		{
            //--------------------------------
            // BMGlobalUpdate
            //--------------------------------
			/*if (BMUIOSDisplayCurrent() != BMTRUE)
				return (BMTRUE - 1);*/
			
			if (BMGlobalUpdate() != 1)
				exit (-11);
			
		}
		else
		{		//Application non active
			BMTimerPause ();
		}
	}
	
}

//**************************************************************
/** \fn     bool InitWindow(HWND &hMainWnd,HINSTANCE hInst,int nCmdShow,int Largeur = 0,int Hauteur = 0,bool full)
 *
 *  \param  &hMainWnd   
 *  \param  hInst       
 *  \param  nCmdShow    
 *  \param  Largeur = 0 
 *  \param  Hauteur = 0 
 *  \param  full        
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  creation of the window...
 *
 *  \author MikE                       \date 02/04/2001 13:55:06
 ***************************************************************/
bool InitWindow(HWND &hMainWnd,HINSTANCE hInst,int nCmdShow,int Largeur = 0,int Hauteur = 0,bool full)
{
   WNDCLASS wc;
   
   wc.lpszClassName="KMLClass";
   wc.lpfnWndProc =WndProc;
   wc.cbClsExtra=0;
   wc.cbWndExtra=0;
   wc.hInstance=hInst;
   wc.hbrBackground=NULL;
   wc.lpszMenuName=NULL;
   wc.style= CS_VREDRAW|CS_HREDRAW;
   wc.hIcon=LoadIcon (NULL,MAKEINTRESOURCE (IDI_APPLICATION));
   wc.hCursor=LoadCursor (NULL,IDC_ARROW);
   if (!RegisterClass (&wc))
	   return false;
   if (!full)
   {
		hMainWnd=CreateWindow("KMLClass","BOMBERMAN'2001",WS_POPUP
			                ,0,0,Largeur,Hauteur
				            ,NULL,NULL,hInst,NULL);
   }
   else
   {
		hMainWnd=CreateWindow("KMLClass","BOMBERMAN'2001",WS_POPUP
			                ,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN)
				            ,NULL,NULL,hInst,NULL);

   }

   if (!hMainWnd)
	   return false;
   ShowWindow(hMainWnd,nCmdShow);
   UpdateWindow (hMainWnd);

	hInstance = hInst;
	GlobalResX= Largeur;
	GlobalResY= Hauteur;

   return true;
}


//**************************************************************
/** \fn     LRESULT CALLBACK WndProc (HWND hWnd,UINT msg,WPARAM wparam,LPARAM lparam)
 *
 *  \param  hWnd   
 *  \param  msg    
 *  \param  wparam 
 *  \param  lparam 
 *
 *  \return  
 *
 ***************************************************************
 *  \brief callback for messages...
 *
 *  \author MikE                       \date 02/04/2001 13:55:19
 ***************************************************************/
LRESULT CALLBACK WndProc (HWND hWnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	switch(msg)
	{
		case WM_KEYDOWN :
						  /*if (wparam == VK_ESCAPE) 
							  PostQuitMessage(0);
						  return TRUE;*/
			break;
		case WM_ACTIVATE:
						  bactive = LOWORD(wparam); // actived flag
						  if (bactive)
						  {
								BMTimerUnPause ();
						  }
						  break;
		case WM_DESTROY:
						  PostQuitMessage(0);
						  break;

		default:
		   return DefWindowProc(hWnd,msg,wparam,lparam);
	}

	return 0;
}

