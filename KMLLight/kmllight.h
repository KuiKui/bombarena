#ifndef __KML_H__
#define __KML_H__

//************************************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <windef.h>
#include <memory.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


// directx
#include <ddraw.h>

//direct input
#include <dinput.h>

// direct sound
#include <dsound.h>

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	long		BMKMLLightMemory;


//*******************************************************************************************
//*******************************************************************************************
//*********************************			DEFINE
//*******************************************************************************************
//*******************************************************************************************


#define		__USINGKMLMEMORY						 //On utilise la KMLLib de memoire
//#define		__KMLDEBUGMEMORY						 //On debug les allocations


#define		EXPORT_DLL								__declspec(dllexport)
#define		KMLLIGHT_VER							"0.1"
#define		CLOSE_KML_LIB							KMLQuitLib();\
													KMLFreeMemory();\
													KMLControlQuitLib();

EXPORT_DLL long	GetKMLLightMemory ();

//********************
//					
// DIRECTX
//					
//********************

// flag de retour
#define     DD_TRUE					0
#define		DD_ERROR				1
#define		DD_SURFACE_LOST			2
#define		DD_CANT_RESTORE			3
#define		DD_ERROR_CLEARSURFACE   4
#define		DD_ERROR_SETBACKCOLOR   5
#define     DD_ERROR_COPYIMAGET		6
#define     DD_ERROR_COPYIMAGE		7
#define		DD_ERROR_FASTBLTTRANS	8
#define		DD_ERROR_FASTBLTNOTRANS	9
#define		DD_ERROR_STRETCHTRANS	10
#define		DD_ERROR_STRETCHNOTRANS	11
#define		DD_ERROR_NO_ALPHA		12

//flag de création de surface
#define		DD_CREATE_VIDEO_PREF	32		//concordance avec KMLIMAGE
#define		DD_CREATE_VIDEO			128
#define     DD_CREATE_MEMORY	    64

//flag d'init DirectX
#define		DD_ENABLE_ALPHA			1
#define		DD_FENETRE				2

//flag pour le blitting
#define		DD_TRANSPARENCE			1
#define		DD_WITH_ALPHA			2

#define		DD_CLEAR				1


//********************
//					
// IMAGES
//					
//********************

// flags de parametre
#define		KML_TRANS				1
#define		KML_ALPHA				2
#define		KML_BMP					8
#define		KML_CLEARCOLOR			16
#define		KML_MEM_VIDEO_PREF		32
#define		KML_MEM_SYSTEM			64
#define		KML_MEM_VIDEO			128
// flags de parametre
#define		KML_SET					1
#define		KML_UNSET				0
// flags de retour
#define		KML_OK					0
#define		KML_NOALLOC				-1
#define		KML_ERROR_DD			-2
#define		KML_ERROR_READ			-3
#define		KML_NOELMT				-4
#define		KML_ALREADY				-5
#define		KML_ERROR_DI			-6
#define		KML_ERROR_PACK			-8
#define		KML_PACK_EOF			-9
#define		KML_PACK_NOFILE			-10
// flags de texte
#define		KML_TEXT_DEFAULT		0
#define		KML_TEXT_DETOURE		1
#define		KML_TEXT_3D				2


//********************
//					
// MEMORY
//					
//********************

#define		TAILLE_SEGMENT			128

extern void *   __GlobaleAlloc;

#ifdef __USINGKMLMEMORY

#ifdef __KMLDEBUGMEMORY

	#define INITMEM(x)	KMLInitMemory(x);KMLDebugInit ()
	#define FREEMEM()	KMLFreeMemory()
	#define ALLOC(x)	KMLDebugAlloc ( __FILE__, __LINE__, x, KMLAlloc(x) )
	#define FREE(x)		KMLFree(x);KMLDebugFree  (__FILE__, __LINE__, x)
	
	#define ALLOC2(x,y)	KMLAlloc(x);y+=x
	#define FREE2(x,y)	y -= KMLFree(x)
#else
	#define INITMEM(x)	KMLInitMemory(x)
	#define FREEMEM()	KMLFreeMemory()
	#define ALLOC(x)	KMLAlloc(x)
	#define FREE(x)		KMLFree(x)
	
	#define ALLOC2(x,y)	KMLAlloc(x);y+=x
	#define FREE2(x,y)	y -= KMLFree(x)

#endif

#else

#ifdef __KMLDEBUGMEMORY
	#define INITMEM(x)	;KMLDebugInit ()
	#define FREEMEM()	0
	#define	ALLOC(x)	KMLDebugAlloc ( __FILE__, __LINE__, x, malloc(x) )
	#define FREE(x)		free(x);KMLDebugFree  (__FILE__, __LINE__, x)

	#define ALLOC2(x,y)	malloc(x);y+=x
	#define FREE2(x,y)	y -= free(x)
#else
	#define INITMEM(x)	;
	#define FREEMEM()	0
	#define	ALLOC(x)	malloc(x)
	#define FREE(x)		free(x)

	#define ALLOC2(x,y)	malloc(x);y+=x
	#define FREE2(x,y)	y -= free(x)

#endif

#endif


//********************
//					
// CONTROL
//					
//********************

#define		KML_CONTROL_MOUSE		1
#define		KML_CONTROL_KEY			2
#define		KML_CONTROL_PAD			4
#define		KML_CONTROL_SWITCH		8

#define		KML_CONTROL_MOUSE_NBBUTMAX	4

#define		KML_CONTROL_NOPAD		0
#define		KML_CONTROL_NBPADMAX	4
#define		KML_CONTROL_PAD_NBBUTMAX	32

#define		KML_PAD_UP				32
#define		KML_PAD_DOWN			33
#define		KML_PAD_LEFT			34
#define		KML_PAD_RIGHT			35
#define		KML_PAD_B0				0
#define		KML_PAD_B1				1
#define		KML_PAD_B2				2
#define		KML_PAD_B3				3
#define		KML_PAD_B4				4
#define		KML_PAD_B5				5
#define		KML_PAD_B6				6
#define		KML_PAD_B7				7
#define		KML_PAD_B8				8
#define		KML_PAD_B9				9
#define		KML_PAD_B10				10
#define		KML_PAD_B11				11
#define		KML_PAD_B12				12
#define		KML_PAD_B13				13
#define		KML_PAD_B14				14
#define		KML_PAD_B15				15
#define		KML_PAD_B16				16
#define		KML_PAD_B17				17
#define		KML_PAD_B18				18
#define		KML_PAD_B19				19
#define		KML_PAD_B20				20
#define		KML_PAD_B21				21
#define		KML_PAD_B22				22
#define		KML_PAD_B23				23
#define		KML_PAD_B24				24
#define		KML_PAD_B25				25
#define		KML_PAD_B26				26
#define		KML_PAD_B27				27
#define		KML_PAD_B28				28
#define		KML_PAD_B29				29
#define		KML_PAD_B30				30
#define		KML_PAD_B31				31

#define		KML_CONTROL_SWITCH_ON	0
#define		KML_CONTROL_SWITCH_OFF	1

//********************
//					
// SOUND
//					
//********************

//flag de retour
#define		KML_OK					0
#define		KML_SOUND_NODRIVER		1
#define		KML_SOUND_ERROR			2
#define		KML_SOUND_PLAYINGYET	3
#define		KML_SOUND_ERROR_SPEAKER 4

//flag de creation a l'init
#define		KML_MONO				1
#define		KML_STEREO				2

#define		KML_11KHZ				11025
#define		KML_22KHZ				22050
#define		KML_44KHZ				44100

//flag pour la config des hauts parleurs
#define		KML_SOUND_DEFAULT		0
#define		KML_SOUND_HEADPHONE		1
#define		KML_SOUND_MONO			2
#define		KML_SOUND_STEREO		3
#define		KML_SOUND_SURROUND		4
#define		KML_SOUND_QUADRI		5

#define		KML_SOUND_MIN			0
#define		KML_SOUND_10DEG			1
#define		KML_SOUND_20DEG			2
#define		KML_SOUND_MAX			3

//flag pour la creation du son
#define		KML_SNORMAL				(0)
#define		KML_SFREQUENCE			(1 << 1)
#define		KML_SVOLUME				(1 << 2)
#define		KML_SPAN				(1 << 3)
#define		KML_SSOUNDMEMORY		(1 << 4)
#define		KML_SSOUNDMEMORYPREF	(1 << 5)
#define		KML_SSTREAMING			(1 << 6)
#define		KML_SALWAYSHEAR			(1 << 7)
#define		KML_SCUTONREPLAY		(1 << 8) 
#define		KML_SLOOP				(1 << 9)
	
//flag pour le set action
#define		KML_SPLAY				(1 << 10)
#define		KML_SSTOP				(1 << 11)
#define		KML_SPAUSE				(1 << 12)
#define		KML_SFADEIN				(1 << 13)
#define		KML_SFADEOUT			(1 << 14)

//flag pour le global action
#define		KML_SPAUSEALL			(1 << 15)
#define		KML_SUNPAUSEALL			(1 << 16)
#define		KML_SSTOPALL			(1 << 17)
#define		KML_SFADEINALL			(1 << 18)
#define		KML_SFADEOUTALL			(1 << 19)
#define		KML_SSTREAMRESERVED		(1 << 20)

#define     KML_SFREQDEFAUT			DSBFREQUENCY_ORIGINAL
#define		KML_SFREQMIN			DSBFREQUENCY_MIN
#define		KML_SFREQMAX			DSBFREQUENCY_MAX

#define		KML_SVOLMIN				0
#define		KML_SVOLMAX				100

#define		KML_SPANLEFT			-100
#define		KML_SPANRIGHT			100

//********************
//					
// PACK
//					
//********************

#define		KML_PACK_SIZEOF_FILENAME		50
#define		KML_PACK_START					0
#define		KML_PACK_CURRENT				1
#define		KML_PACK_CAT_STRING				'>'

//*******************************************************************************************
//*******************************************************************************************
//*********************************			LISTES
//*******************************************************************************************
//*******************************************************************************************


//********************
//					
// STRUCTURES
//					
//********************

typedef struct{
	void		*	Suivant;
	void		*	Precedent;
}KMLNODE;

typedef struct{
	void		*	First;
	void		*	Vide;
	void		*	Last;
}KMLLISTE;

//********************
//					
// MACROS
//					
//********************

// Initialisation d'une Liste
#define KMLNewList(l)    (l)->Vide=0;\
												(l)->First=(KMLNODE *)&((l)->Vide);\
												(l)->Last=(KMLNODE *)l;

// Insertion d'un element en fin de liste
#define KMLAddTail(l,n)  (n).Precedent=(l)->Last;\
												(n).Suivant=((KMLNODE *)((l)->Last))->Suivant; \
												((KMLNODE *)((l)->Last))->Suivant=&(n); \
												(l)->Last=&(n);

// Insertion d'un element en debut de liste
#define KMLAddHead(l,n)  (n).Suivant=(l)->First;\
												(n).Precedent=((KMLNODE *)((l)->First))->Precedent;\
												((KMLNODE *)((l)->First))->Precedent=&(n);\
												(l)->First=&(n);

#define KMLInsertNode(n,p)   (n).Precedent=&(p);\
												(n).Suivant=(p).Suivant;\
												(p).Suivant=&(n);\
												((KMLNODE *)((n).Suivant))->Suivant=&(n);

// Destruction d'une liste
#define KMLRemoveNode(n)     ((KMLNODE *)((n).Precedent))->Suivant=(n).Suivant;\
												((KMLNODE *)((n).Suivant))->Precedent=(n).Precedent;


//*******************************************************************************************
//*******************************************************************************************
//*********************************			MACROS
//*******************************************************************************************
//*******************************************************************************************

//********************
//					
// IMAGES
//					
//********************

// dispatch color
#define		KML_SET_RGB(r,g,b)			( ((DWORD)(r) << 16) | ((WORD)(g) << 8) | (BYTE)(b) )
#define		KML_SET_RGBA(a,r,g,b)		( ((DWORD)(a) << 24) | ((DWORD)(r) << 16) | ((WORD)(g) << 8) | (BYTE)(b) )
// compact color
#define		KML_GET_R(c)				(BYTE)( (c>>16) )
#define		KML_GET_G(c)				(BYTE)( (c>>8) )
#define		KML_GET_B(c)				(BYTE)( (c) )
  
//********************
//					
// CONTROL
//					
//********************

// keyboard
#define		KML_KEYISDOWN(num)				KMLControlKeys[num]&0x80
#define		KML_KEYISUP(num)				(!(KMLControlKeys[num]&0x80))


//*******************************************************************************************
//*******************************************************************************************
//*********************************			STRUCTURES
//*******************************************************************************************
//*******************************************************************************************


//********************
//					
// DLL EXPERT
//					
//********************

typedef struct{
}KMLDLLEXPORTSTRUCT;

//********************
//					
// GENERALES
//					
//********************

typedef struct{
	long			x;
	long			y;
}KMLPOINT;
typedef  KMLPOINT	*		LPKMLPOINT;

typedef struct{
	DWORD			r;
	DWORD			g;
	DWORD			b;
}KML_COLOR;

// KMLID
typedef  long				KMLID;

typedef void (*fonc)(void * lpParameter);							// definition de la fonction CALLBACK pour le switch (entre autre)
typedef void (*fon2)(struct kmlimage *image, void * lpParameter);	// definition de la fonction CALLBACK pour le reinit par images


//********************
//					
// IMAGE
//					
//********************

typedef struct kmlimage{
	KMLNODE					Node;
	LPDIRECTDRAWSURFACE		Surface;
	unsigned long			Largeur;
	unsigned long			Hauteur;
	unsigned long			Pitch;
	char				*	DataPath;
	long					ClearColor;
	unsigned char			Alpha;
	fon2					Fonction;		//fonction callback pour le reload lors de la perte de la surface
	void				*	Param;
	DWORD					Flag;
}KMLIMAGE;
typedef  KMLIMAGE	*		LPKMLIMAGE;

//********************
//					
// KMS FILE INPUT
//					
//********************

typedef struct{
	long		ID;
	long		x;
	long		y;
	long		largeur;
	long		hauteur;
	long		time;
}KMSFRAME;

typedef struct{
	long		ID;
	long		NbrFrame;
}KMSANIM;

typedef struct{
	char		PathImage[50];
	long		NbrAnim;
	long		NbrFrameTotal;
	long		Z;
}KMSHEADER;

//********************
//					
// CONTROL
//					
//********************

typedef struct{
	long		x;
	long		y;
	char		b[KML_CONTROL_MOUSE_NBBUTMAX];
	char		nbbutton;
}KMLMOUSE;

typedef struct{
	char		left;
	char		right;
	char		up;
	char		down;
	char		button[KML_CONTROL_PAD_NBBUTMAX];
	char		nbbutton;
}KMLPAD;


//********************
//					
// SOUND
//					
//********************

typedef struct 
{
	char        RIFF[4];          
	DWORD       Size;
	char        WAVE[4];          
	char        fmt_[4];          
	DWORD       SizeOfDesc;             
	WORD        FormatTag;		  
	WORD        Channels;           
	DWORD       SampleRate;          
	DWORD       BytesPerSec;      
	WORD        BlockAlign;       
	WORD        BitsPerSample;
}KMLWAVEHEADER;

typedef struct
{
	FILE			**fichier;
	long			writeposition;
	unsigned long	tailledata;
	unsigned long	taillebuffer;
	unsigned long   ptrfichier;
	unsigned long	tailleentete;
	unsigned char	taillesample;
}KMLSTREAM;

typedef struct  kml_soundbdd_struct
{
	KMLNODE					Node;
	long					User;
	char					*Path;
	long					flag;
	struct kml_sound_struct	*PtrLastSound;
	struct kml_sound_struct	*PtrKMLSound;
}KMLSOUNDBDD;

typedef struct kml_sound_struct
{
	LPDIRECTSOUNDBUFFER			SoundBuffer;
	unsigned long				Frequence;
	unsigned char				Volume;
	char						Span;
	long						Flag;
	unsigned long				Time;
	unsigned long				TimeCourant;
	unsigned long				TimeInit;
	struct  kml_sound_struct	*Suivant;
	struct  kml_soundbdd_struct	*BDD;
}KMLSOUND;


//********************
//					
// PACK
//					
//********************

typedef struct{
	char						filename[KML_PACK_SIZEOF_FILENAME];
	char						path[KML_PACK_SIZEOF_FILENAME];
	unsigned long				sizeofdata;
	unsigned long				offset;
	void						*data;
}KMLFILE;

typedef struct{
	unsigned long				nbfile;
	KMLFILE						*filelisting;
}KMLPACK;

typedef struct{
	char						filename[KML_PACK_SIZEOF_FILENAME];
}KMLFILEDESC;

//*******************************************************************************************
//*******************************************************************************************
//*********************************			VARIABLES
//*******************************************************************************************
//*******************************************************************************************


//********************
//					
// DIRECTX
//					
//********************

extern HWND					hMainWnd;
extern HINSTANCE			hInstGlobal;
extern char					NbreBitCouleur;
//********************
//					
// IMAGES
//					
//********************

extern  short				GLOBAL_RES_X;
extern	short				GLOBAL_RES_Y;
extern	long				GLOBAL_PITCH;
extern	char				GLOBAL_NB_COLOR;

//********************
//					
// CONTROL
//					
//********************

extern	unsigned char		KMLControlKeys[256];
extern	unsigned char		KMLControlLastKeys[256];
extern	KMLMOUSE			KMLControlMouse;
extern	KMLPAD				KMLControlPad[KML_CONTROL_NBPADMAX];
extern	KMLPAD				KMLControlLastPad[KML_CONTROL_NBPADMAX];
extern	long				KMLControlPadState;
extern	fonc				KMLSwitchFonction;
extern	void				*KMLSwitchParameter;


//********************
//					
// PACK
//					
//********************

extern	char				KMLPackTempPath[KML_PACK_SIZEOF_FILENAME];
extern	char				KMLPackTempPackName[KML_PACK_SIZEOF_FILENAME];


//*******************************************************************************************
//*******************************************************************************************
//*********************************			FONCTIONS
//*******************************************************************************************
//*******************************************************************************************
//********************
//					
// DLL EXPERT
//					
//********************

EXPORT_DLL void						KMLGetDllExportStruct(KMLDLLEXPORTSTRUCT *dllexport);

//********************
//					
// GENERAL
//					
//********************

EXPORT_DLL	void					KMLError		(const char *fmt,...);
EXPORT_DLL	void					KMLWarning		(const char *fmt,...);
EXPORT_DLL	void					KMLPrintf		(int x,int y,const char *fmt,...);
EXPORT_DLL	void					KMLSetTextStyle (char style = KML_TEXT_DEFAULT);
EXPORT_DLL	void					KMLSetTextColor (unsigned char r = 255, unsigned char g = 255, unsigned char b = 255);


//********************
//					
// DIRECTX
//					
//********************

// Fonctions Globales
EXPORT_DLL	char					DDInitMode		(int Largeur, int Hauteur,char flag = NULL, char ColorDepth = 32,unsigned char r = 0, unsigned char g = 0, unsigned char b = 0);
EXPORT_DLL	char					DDQuit			(void);
EXPORT_DLL	void					DDSwitch		();//basculement mode fenetre plein ecran

// Creation
EXPORT_DLL	LPDIRECTDRAWSURFACE		DDCreateSurface	(int x, int y, DWORD = DD_CREATE_VIDEO_PREF);
EXPORT_DLL	char					DDDeleteSurface	(LPDIRECTDRAWSURFACE surface);
EXPORT_DLL	char					DDLoadBMP		(LPDIRECTDRAWSURFACE surface, char *bitmap);
EXPORT_DLL	char					DDClearSurface	(LPDIRECTDRAWSURFACE surface, unsigned char r, unsigned char g, unsigned char b);
EXPORT_DLL	char					DDSaveBMP		(LPDIRECTDRAWSURFACE surface, char *pszFichier, long hauteur, long largeur);

// Blitting
EXPORT_DLL	char					DDCopyImage		(LPDIRECTDRAWSURFACE surfacedest, LPDIRECTDRAWSURFACE surfacesrc, KMLIMAGE *Image);
EXPORT_DLL	char					DDFastBlt		(LPDIRECTDRAWSURFACE surfacedest, short xdest, short ydest, LPDIRECTDRAWSURFACE surfacesrc, RECT *src, KMLIMAGE *Image);
EXPORT_DLL	char					DDStretchBlt	(LPDIRECTDRAWSURFACE surfacedest, RECT *dest, LPDIRECTDRAWSURFACE surfacesrc,RECT *src, KMLIMAGE *Image);
EXPORT_DLL	char					DDSetBackColor  (LPDIRECTDRAWSURFACE surfacedest);

EXPORT_DLL  char					DDFinalDraw		(DWORD flag=DD_CLEAR);

//Acces a la surface (à utiliser pour niquer le framerate ou pour un mode ralenti si votre pc marche trop bien)
EXPORT_DLL void *					DDLock			(LPDIRECTDRAWSURFACE surface);
EXPORT_DLL char						DDUnLock		(LPDIRECTDRAWSURFACE surface);


//Restauration de surface perdue
EXPORT_DLL  char					DDRestore		(LPDIRECTDRAWSURFACE surface);
EXPORT_DLL  char					DDReloadSurface (LPDIRECTDRAWSURFACE *surface, char *path, DWORD flag);

//utils
EXPORT_DLL	void					DDGetBackBufferDC(HDC *dc, long flag);


//********************
//					
// IMAGES
//					
//********************

// Fonction GlobalKML
EXPORT_DLL	char			KMLStartLib		(HWND hWnd, HINSTANCE hInst, short x, short y, long flag, unsigned char bitdepth = 32, unsigned char r=0, unsigned char g=0, unsigned char b=0);
EXPORT_DLL	char			KMLQuitLib		(void);
EXPORT_DLL	char			KMLFinalDraw	(char flag = DD_CLEAR);
EXPORT_DLL	char			KMLSwitch		(void);


// Creation
EXPORT_DLL	KMLIMAGE *		KMLCreateImage	(long x, long y, DWORD flag = KML_MEM_VIDEO_PREF, char *bitmap=NULL);
EXPORT_DLL	char			KMLDeleteImage	(KMLIMAGE *ima);
EXPORT_DLL	char			KMLClearImage	(KMLIMAGE *ima, unsigned char r, unsigned char g, unsigned char b);
EXPORT_DLL	char			KMLLoadBMP		(KMLIMAGE *ima, char *bitmap);
EXPORT_DLL	void			KMLSetSurfaceType(KMLIMAGE *ima, long type);

// Blitting
EXPORT_DLL	char			KMLCopyImage	(KMLIMAGE *imadest, KMLIMAGE *imasrc);
EXPORT_DLL	char			KMLFastBlt		(KMLIMAGE *imadest, long xdest, long ydest, KMLIMAGE *imasrc, long xsrc, long ysrc, long largeursrc, long hauteursrc);
EXPORT_DLL	char			KMLStretchBlt	(KMLIMAGE *imadest, long xdest, long ydest, long largeurdest, long hauteurdest, KMLIMAGE *imasrc, long xsrc, long ysrc, long largeursrc, long hauteursrc);


// Utils
EXPORT_DLL	char			KMLSetBackColor	(KMLIMAGE *ima, DWORD flag);
EXPORT_DLL	char			KMLSetAlpha		(KMLIMAGE *ima, DWORD flag, unsigned char alpha=0);
EXPORT_DLL	void		*	KMLLockSurface	(KMLIMAGE *ima);
EXPORT_DLL	char			KMLDelockSurface(KMLIMAGE *ima);

// Reload et Reinit
EXPORT_DLL	char			KMLSetCallback	(KMLIMAGE *ima, fon2 fonction, void *param);
EXPORT_DLL	char			KMLSetParam		(KMLIMAGE *ima, void *param);

//********************
//					
// MEMORY
//					
//********************

//structure debug
struct DBGALLOC
{
	KMLNODE	Node;
	char	*Filename;
	long	LineNumber;
	long	SizeOf;
	void	*Adr;
};

struct DBGFREE
{
	KMLNODE		Node;
	char		*FileName;
	long		LineNumber;
	KMLLISTE	FileFreeList;
};

struct DBGFILEFREE
{
	KMLNODE	Node;
	char	*FileName;
	long	LineNumber;
	long	NumberOfTime;
};

struct DBGUSER
{
	char	*FileNameAlloc;
	long	LineNumber;
	DBGFREE	*FreeInfos;
};

EXPORT_DLL	void			KMLInitMemory		(unsigned int size_of_total_memory=4000000, unsigned char number_of_bit_for_hashing=8);
EXPORT_DLL	long			KMLFreeMemory		(void);
EXPORT_DLL	void		*	KMLAlloc			(unsigned int size_of_alloc);
EXPORT_DLL	void		*	KMLRealloc			(void *adress_of_realloc, unsigned int size_of_realloc);
//EXPORT_DLL	void			KMLFree				(void *adress_of_alloc);
EXPORT_DLL	long			KMLFree				(void *adress_of_alloc);

EXPORT_DLL	long			KMLGetFreeSize		();
EXPORT_DLL	long			KMLGetFreeMax		();

EXPORT_DLL	void *KMLDebugAlloc ( char *FileName, long LineNumber, long SizeOfAlloc, void *Adr );
EXPORT_DLL  void KMLDebugFree  (char *FileName, long LineNumber, void *AdrToFree);
EXPORT_DLL  DBGUSER KMLDebugEnd	();
EXPORT_DLL  void KMLDebugInit	();




//********************
//					
// CONTROL
//					
//********************

EXPORT_DLL	char			KMLControlStartLib				(HINSTANCE &hInst, char flag);
EXPORT_DLL	char			KMLControlUpdate				(void);
EXPORT_DLL	void			KMLControlQuitLib				(void);
EXPORT_DLL	char			KMLControlMouseGetState			(KMLMOUSE *mouse);
EXPORT_DLL	char			KMLControlMouseButtonPress		(char button=0, char slice=0);
EXPORT_DLL	char			KMLControlPadPress				(char button, char numpad=0, char slice=0);
EXPORT_DLL	char			KMLControlKeysPress				(int key, char slice=0);
EXPORT_DLL  void			KMLControlSwitch				(char flag);
EXPORT_DLL	void			KMLControlSetCallbackSwitching	(fonc fonction, void *lpParameter);
EXPORT_DLL	void			KMLControlSetParameterSwitching (void *lpParameter);



//********************
//					
// SOUND
//					
//********************

EXPORT_DLL	char			KMLSoundStartLib		(HINSTANCE &hInst, unsigned long SpeakerConfig = NULL, unsigned long SpeakerSubConfig = NULL);
EXPORT_DLL	void			KMLSoundQuitLib			(void);

EXPORT_DLL	char			KMLSoundUpdate			();
EXPORT_DLL  void			KMLSoundUpdateStream	();

EXPORT_DLL	KMLSOUND *		KMLSoundCreate			(char *Path, long flag = KML_SFREQUENCE | KML_SVOLUME | KML_SPAN | KML_SSOUNDMEMORYPREF);
EXPORT_DLL  char			KMLSoundDelete			(KMLSOUND *PtrSound);

EXPORT_DLL	char			KMLSoundSetAction		(KMLSOUND *PtrSound, long flag, long time = 1000);
EXPORT_DLL	char			KMLSoundIsFinished		(KMLSOUND *PtrSound);

EXPORT_DLL	char			KMLSoundUnsetFlag		(KMLSOUND *PtrSound, long flag);
EXPORT_DLL	char			KMLSoundSetFlag			(KMLSOUND *PtrSound, long flag);
EXPORT_DLL	char			KMLSoundSetFrequency	(KMLSOUND *PtrSound, unsigned long Frequence);
EXPORT_DLL	char			KMLSoundSetVolume		(KMLSOUND *PtrSound, unsigned char Volume);
EXPORT_DLL	char			KMLSoundSetPan			(KMLSOUND *PtrSound, char Span);

//fonction globale
EXPORT_DLL	char			KMLSoundSetGlobalAction	(long flag, long time = 0);
EXPORT_DLL	char			KMLSoundSetGlobalVolume	(unsigned short Volume);//entre 0 et 1024


//********************
//					
// PACK
//					
//********************

EXPORT_DLL	void			KMLPackStartLib			(void);

EXPORT_DLL	char			KMLPackAddFile			(char *packname, char *filename);
EXPORT_DLL	KMLFILE *		KMLPackOpen				(char *packname, char *filename, long flag = 0);
EXPORT_DLL	char			KMLPackRead				(void *dest, unsigned long nboctet, KMLFILE *f);
EXPORT_DLL	char			KMLPackSeek				(KMLFILE *f, unsigned long size, char flag);
EXPORT_DLL	char			KMLPackClose			(KMLFILE *f);
EXPORT_DLL	char			KMLPackEof				(KMLFILE *f);


EXPORT_DLL	char			KMLPackFindFile			(char *packname, char *filename, KMLFILEDESC *pfiledesc);
EXPORT_DLL	void			KMLPackResetFindFile	( void );


EXPORT_DLL	void			KMLPackSetTempPath		(char *path);
EXPORT_DLL	void			KMLPackGetTempPath		(char *path);
EXPORT_DLL	void			KMLPackGetTempPackName	(char *name);
EXPORT_DLL	void			KMLPackSetTempPackName	(char *name);

EXPORT_DLL	KMLFILE *		KMLPackGetKMLFILE		(char *str);
EXPORT_DLL	char			KMLPackGetFilename		(char *packname, unsigned long index, char *filename, char flag);
EXPORT_DLL	unsigned long	KMLPackGetNbFile		(char *packname);
EXPORT_DLL	void			KMLPackGetPacknameFromTotalName(char *total, char *pack, char *file);
EXPORT_DLL	void			KMLPackSetPacknameFromTotalName(char *total, char *pack, char *file);


//************************************************************************************************

#endif


