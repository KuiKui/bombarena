#ifndef __BMUIOS_H__
#define	__BMUIOS_H__


#include	"BMGlobalInclude.h"
#include	"BMUIFont.h"

//--------------------------------
// Defines
//--------------------------------
#define		BMOS_WINDOW					1
#define		BMOS_TEXT					2
#define		BMOS_SELECTIONNABLETEXT		3
#define		BMOS_ELEVATOR				4

//--	Insert Flag
#define		BMOS_NOTHING				(0)
#define		BMOS_CENTER_H				(1 << 0)
#define		BMOS_CENTER_V				(1 << 1)
#define		BMOS_RIGHT					(1 << 2)
#define		BMOS_LEFT					(1 << 3)
#define		BMOS_TOP					(1 << 4) 
#define		BMOS_BOTTOM					(1 << 5)

//--	BackGround Image
#define		BMOS_BACKMOSAIQUE			(1 << 6)
#define		BMOS_BACKSTRETCH			(1 << 7)

//--	Elevator
#define		BMOS_ELEVATOR_WIDHTMIN		50
#define		BMOS_ELEVATOR_HEIGHTMIN		50

#define		BMOS_ELEVATOR_TIMER			200		//timer to scroll the character

#define		BMOS_ELEVATOR_BOXX_X		16			//horizontal
#define		BMOS_ELEVATOR_BOXX_Y		0
#define		BMOS_ELEVATOR_BOXX_SX		64
#define		BMOS_ELEVATOR_BOXX_SY		16

#define		BMOS_ELEVATOR_BOXY_X		0			//vertical
#define		BMOS_ELEVATOR_BOXY_Y		16
#define		BMOS_ELEVATOR_BOXY_SX		16
#define		BMOS_ELEVATOR_BOXY_SY		64

#define		BMOS_ELEVATOR_CORN_X		0
#define		BMOS_ELEVATOR_CORN_Y		0
#define		BMOS_ELEVATOR_CORN_SX		16
#define		BMOS_ELEVATOR_CORN_SY		16

#define		BMOS_BACK_X					80
#define		BMOS_BACK_Y					0
#define		BMOS_BACK_SX				16
#define		BMOS_BACK_SY				16

#define		BMOS_ELEVATOR_BUTTONL_X		0
#define		BMOS_ELEVATOR_BUTTONL_Y		80
#define		BMOS_ELEVATOR_BUTTONL_SX	16
#define		BMOS_ELEVATOR_BUTTONL_SY	16

#define		BMOS_ELEVATOR_BUTTONR_X		80
#define		BMOS_ELEVATOR_BUTTONR_Y		80
#define		BMOS_ELEVATOR_BUTTONR_SX	16
#define		BMOS_ELEVATOR_BUTTONR_SY	16

#define		BMOS_ELEVATOR_SCROLL_SY		18

//--	Selectionnable
#define		BMOS_SELECTIONNABLE_TIME	100
#define		BMOS_SELECTIONNABLE_TIME2	50

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD		BMOSMemory;

//--------------------------------
// Variables
//--------------------------------
typedef struct
{
	STRING		*pszString;
	STRING		pszString2 [255];
	UDWORD		Id;
}BMOSBOXTEXT;

typedef struct
{
	UDWORD		NumberUsed;		//number of image in current use
	KMLIMAGE	*PtrImage;		//ptr to the image
	STRING		*pszFileName;	//name of the file
}BMOSIMAGE;

typedef struct
{
	UDWORD	id;					//id ot the object
								
	BMLIST	*BMOSChildren;		//list of the object in this object, BMOSOBJECT... (like text in the window)
								
	UDWORD	BMOSType;			//Type of the Object (Window, Text, Selectionnable Text...)
	void	*BMOSInfos;			//Specific Infos ... see BMOSType
}BMOSOBJECT;

//Structure compatible with all type of object, any object must begin by this member !!!

typedef struct
{
	UDWORD	parentid;			//id of the parent object
	UDWORD	menuid;				//to manage the menu
	UDWORD	x;					//x coord of the window
	UDWORD	y;					//y coord of the window
	UDWORD	width;				//width of the window
	UDWORD	height;				//height of the window
}BMOSBASEOBJECT;				
								
typedef struct					
{								
	UDWORD		parentid;		//id of the parent object
	UDWORD		menuid;			//to manage the menu
	UDWORD		x;				//x coord of the window
	UDWORD		y;				//y coord of the window
	UDWORD		width;			//width of the window
	UDWORD		height;			//height of the window
	KMLIMAGE	*BackGrd;		//image of the background
	UDWORD		Flag;			//for the background, ...
}BMOSWINDOW;					
								
typedef struct
{
	UDWORD	parentid;			//id of the parent object
	UDWORD	menuid;				//to manage the menu
	UDWORD	x;					//x coord of the window
	UDWORD	y;					//y coord of the window
	UDWORD	width;				//width of the window
	UDWORD	height;				//height of the window
	STRING	*pszText;			//Text
	STRING	*pszFontName;		//current font display
}BMOSTEXT;						
								
typedef struct					
{								
	UDWORD	parentid;			//id of the parent object
	UDWORD	menuid;				//to manage the menu
	UDWORD	x;					//x coord of the window
	UDWORD	y;					//y coord of the window
	UDWORD	width;				//width of the window
	UDWORD	height;				//height of the window
	STRING	*pszText;			//Text
	//Sound when the mouse pointer when activate
	//color to be set when the mouse pointer is on the text
	STRING	*pszFontName;		//font to activate when menu activated
	STRING  *pszFontNameActive;	//current font display
	UBYTE	bActive;
	UBYTE	bIsDown;
	UDWORD	TimeLeft;
}BMOSTEXTSELECTIONNABLE;

typedef struct
{
	UDWORD		parentid;		//id of the parent object
	UDWORD		menuid;			//to manage the menu
	UDWORD		x;				//x coord of the window
	UDWORD		y;				//y coord of the window
	UDWORD		width;			//width of the window
	UDWORD		height;			//height of the window
	BMLIST*		itemslist;		//item of the elevator
	BMNODE*		selectionitem;	//item selectionned
	BMNODE*		currenttopitem;	//current top item 
	SDWORD		currentchar;	//the current char displayed in selection
	UDWORD		timeleft;		//the time before the current char passed to the other
	KMLIMAGE*	background;		//the background
	UDWORD		flagback;		//flag of the background
	STRING*		pszFontName;	//current font display
	UDWORD		max_carac;		//max caractere you can display 
	UBYTE		bsens;			//sensdeplacement
}BMOSELEVATOR;

//--------------------------------
// Functions
//--------------------------------

extern	SBYTE		BMUIOSStart				( STRING *pszString );
extern	SBYTE		BMUIOSEnd				();

//-- Object
extern	SBYTE		BMUIOSObjectCreate		(BMOSOBJECT **NewObject);
extern	SBYTE		BMUIOSObjectDelete		(BMOSOBJECT *DeleteObject);
extern	SBYTE		BMUIOSObjectDelete		(UDWORD Id);

extern	SBYTE		BMUIOSObjectSetMenuId	(BMOSOBJECT *NewObject, UDWORD MenuId);
extern	SBYTE		BMUIOSGetObjectByMenuId (BMOSOBJECT **Object, UDWORD MenuId );


extern	SBYTE		BMUIOSObjectInsert		(UDWORD BaseObejctId, UDWORD ObjectToInsertId, UDWORD CoordX, UDWORD YCoord, UDWORD Flag = BMOS_NOTHING); 
extern	SBYTE		BMUIOSFindObject		(UDWORD Id,	BMOSOBJECT **pObject);

extern	SBYTE		BMUIOSObjectCreateImage (STRING *pszFile, KMLIMAGE **NewImage);	//do not use
extern	SBYTE		BMUIOSObjectDeleteImage (KMLIMAGE *PtrImage);	//do not use

extern	SBYTE		BMUIOSDisplayObject		(BMOSOBJECT *Obj);	//do not use

extern	SBYTE		BMUIOSDisplayCurrent	();

extern	SBYTE		BMUIOSObjectManage		(BMOSOBJECT *Object, KMLMOUSE mousestate);
//-- Window
EXPORT_DLL	SBYTE	BMUIOSWindowCreate		(BMOSOBJECT **NewObject, STRING *pszFile, UDWORD Flag = BMOS_NOTHING);
EXPORT_DLL	SBYTE	BMUIOSWindowLoadBMP		(BMOSWINDOW *Window, STRING *pszFile, UDWORD Flag = BMOS_NOTHING);
EXPORT_DLL	SBYTE	BMUIOSWindowClose		();
EXPORT_DLL	BMOSOBJECT *BMUIOSGetCurrentWindow();

extern	SBYTE		BMUIOSWindowDelete		(BMOSWINDOW *objtodelete);	//do not use

extern	SBYTE		BMUIOSWindowDisplay		(BMOSWINDOW *Window);

//-- Text
EXPORT_DLL	SBYTE	BMUIOSTextCreate		(BMOSOBJECT	**NewObject, STRING	*pszText, STRING *pszFontName, UDWORD CoordX, UDWORD CoordY, UDWORD IdParent, UDWORD Flag = BMOS_NOTHING);

EXPORT_DLL	SBYTE	BMUIOSTextDelete		(BMOSTEXT	*objtodelete);

EXPORT_DLL	SBYTE	BMUIOSTextDisplay		(BMOSTEXT	*Text);

EXPORT_DLL	SBYTE	BMUIOSTextManage		(BMOSTEXT	*Text, KMLMOUSE mousestate);

//-- Text Selectionnable
EXPORT_DLL	SBYTE	BMUIOSTextSCreate		(BMOSOBJECT	**NewObject, STRING	*pszText, STRING *pszFontName, STRING *pszFontNameActive,UDWORD CoordX, UDWORD CoordY, UDWORD IdParent, UDWORD Flag = BMOS_NOTHING);
EXPORT_DLL	SBYTE	BMUIOSTextSDelete		(BMOSTEXTSELECTIONNABLE	*objtodelete);

EXPORT_DLL	SBYTE	BMUIOSTextSDisplay		(BMOSTEXTSELECTIONNABLE	*Text);

EXPORT_DLL	SBYTE	BMUIOSTextSManage		(BMOSTEXTSELECTIONNABLE	*Text, KMLMOUSE mouestate);

//-- Elevator
EXPORT_DLL	SBYTE	BMUIOSElevatorCreate		(BMOSOBJECT	**NewObject, STRING *pszFontName, UDWORD CoordX, UDWORD CoordY, UDWORD SizeX, UDWORD SizeY, UDWORD IdParent, STRING* BackGround,UDWORD FlagBack = BMOS_BACKMOSAIQUE);

EXPORT_DLL	SBYTE	BMUIOSElevatorDelete		(BMOSELEVATOR *objtodelete);

EXPORT_DLL	SBYTE	BMUIOSElevatorDisplay		(BMOSELEVATOR *Elevator);

EXPORT_DLL	SBYTE	BMUIOSElevatorManage		(BMOSELEVATOR *Elevator, KMLMOUSE MouseState);

EXPORT_DLL	SBYTE	BMUIOSElevatorAddItem		(BMOSELEVATOR *Elevator, BMOSBOXTEXT NewItem);

EXPORT_DLL	SBYTE	BMUIOSElevatorIsInLButton	(BMOSELEVATOR *Elevator, KMLMOUSE MouseState);

EXPORT_DLL	SBYTE	BMUIOSElevatorIsInRButton	(BMOSELEVATOR *Elevator, KMLMOUSE MouseState);

EXPORT_DLL	SWORD	BMUIOSElevatorIsText		(BMOSELEVATOR *Elevator, KMLMOUSE MouseState);

EXPORT_DLL	SBYTE	BMUIOSLoadBackGround		(BMOSELEVATOR *Elevator, STRING* BackGround, UDWORD FlagBack = BMOS_BACKMOSAIQUE);

EXPORT_DLL	SBYTE	BMUIOSElevatorGetLButton	(BMOSELEVATOR *Elevator,RECT *rect);

EXPORT_DLL	SBYTE	BMUIOSElevatorGetBackGrButton (BMOSELEVATOR *Elevator,RECT *rect);

EXPORT_DLL	SBYTE	BMUIOSElevatorGetRButton	(BMOSELEVATOR *Elevator,RECT *rect);

//button, font, elevator...

EXPORT_DLL	SBYTE	BMUIOSWhosObject		(BMOSOBJECT **ObjectPointed, UDWORD CoordX, UDWORD CoordY);

#endif // __BMUIOS_H__