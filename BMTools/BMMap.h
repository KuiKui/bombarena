#ifndef __BMMAP_H__
#define	__BMMAP_H__

#include	"BMGlobalInclude.h"
#include	"BMAnims.h"
#include	"BMPlayer.h"
#include	"BMEntity.h"
#include	<stdio.h>

//--------------------------------
// Define
//--------------------------------

//-- File Delimiter
#define BMMAPBEGIN				"#BEGIN_FILE_DESCRIPTION"
#define BMMAPEND				"#END_FILE_DESCRIPTION"
								
#define	BMMAPPLAYER				"#SET_PLAYER"			//set player "posx" "posy"
								
#define	BMMAPDENSITY			"#SET_DENSITY"			//set density "number"
								
#define BMMAPBONUSMAP			"#SET_BONUS"			//set bonus "# of bonus" "number of bonus"

#define	BMMAPBACKGROUND			"#SET_BACKGROUND"		//set bakcground "bitmap file"
								
#define BMMAPBEGINMAP			"#BEGIN_MAP"
#define BMMAPENDMAP				"#END_MAP"
								
#define	BMMAPACTIVENUMBER		"#ACTIVE_NUMBER"		//active the number and unactive ascii ( 1, "x")	//default is number
#define BMMAPACTIVEASCII		"#ACTIVE_ASCII"			//active the ascii and unactive number ( "1", x)	//default is number

#define	BMMAPACTIVEHEXA			"#ACTIVE_HEXA"			//active the hexadecimal value, and unactive all the other

#define	BMMAPDESTRUCTIBLE		"#DESTRUCTIBLE"			//define the destructible block
#define	BMMAPUNDESTRUCTIBLE		"#UNDESTRUCTIBLE"		//define the undestructible block
//-- Type of map array
#define BMMAPANIM				1	//Use to describe a normal anim
#define	BMMAPCOLOR				2	//Use to describe an anim which color depends on the 3 bit in info field

//-- Type for bmmaptype aray
#define	BMMAPTYPENULL			0
#define	BMMAPTYPESPR			1
								

//--------------------------------
// Structures
//--------------------------------

typedef struct
{
	UBYTE	SolType;	//type de sol  : normal, tapis roulant, teleporteur, jumper
	UBYTE	SolInfo;	//info de sol  : sens du tapis roulant, vitesse...
	BMANIM	*SolAnim;	//anim of the element
	UBYTE	BlocType;	//type de bloc : rien, bloc destructible, bloc indestructible, feu  ...
	UBYTE	BlocInfo;	//info de bloc : sens couleur du feu... // les 3 premiers bits sont reserves pour connaitre la couleur du joueur si il y a lieu, les 5 autres sont reservé pour l'indexage dans le tableau
	BMANIM	*BlocAnim;	//anim of the bloc
}BMMAPSOL;

typedef	struct
{
	UBYTE		Type;				//type de l'element (bombe, bonus) (Pas Player)
	void		*Infos;				//infos specifique au type
}BMMAPTYPE;

typedef struct 
{
	SDWORD		Type;				//type of the info (BMMAPANIM or BMMAPCOLOR)
	STRING		*AnimName;			//name of the file of anim
	BMANIM		*Anim;				//anim loaded
}BMMAPARRAY;

typedef struct
{
	SDWORD		Identifier;			//number which identifies the bonus
	SDWORD		Number;				//how much bonus available on this map
}BMMAPBONUS;

typedef struct
{
	UBYTE		CoordX;
	UBYTE		CoordY;
}BMMAPPOSPLAYER;					//use to describe on the map the first position of the player

//--------------------------------
// Globals Debug Memory
//--------------------------------
extern	SDWORD			BMMapMemory;

//--------------------------------
// Globales
//--------------------------------

extern	BMMAPSOL		BMMapSol			[BMMAP_WIDTH * BMMAP_HEIGHT];	//Describe the type of the ground and blocks
extern	BMENTITY		*BMMapEntityArray	[BMMAP_WIDTH * BMMAP_HEIGHT];	//Use by the server to know if a player or a bomb is on this case, it points to the list of objects

extern	BMMAPTYPE		*BMMapPtrPlayer;								//Pointer to the first player in the list of objects
																		
extern	BMLIST			*BMMapObject;									//list of the map object, BMMAPTYPE...
																		
extern	KMLIMAGE		*BMMapImage;									
																		
extern	SBYTE			BMMapDensity;									//Density in the map
extern	BMLIST			*BMListBonus;									//List of bonus available in this map, BMMAPBONUS...
																		
extern	BMMAPPOSPLAYER	BMMapPosPlayer [BMPLAYERMAXNUMBER];				//Array of the initial position of the players
extern	SBYTE			BMMapMaxPlayer;									//Number of max player for this map

extern	STRING			pszMapElement	[255];
extern	STRING			pszMapField		[255];
//--------------------------------										
// Fonctions															
//--------------------------------

extern	SBYTE			BMMapStart			();
extern	SBYTE			BMMapEnd			();

EXPORT_DLL	SBYTE		BMMapReset			();
EXPORT_DLL	SBYTE		BMMapLoadElement	(STRING *pszfichier);
EXPORT_DLL	SBYTE		BMMapLoadField		(STRING	*pszfichier);

extern	SBYTE			BMMapDisplay		(KMLIMAGE *image, SDWORD x, SDWORD y);

EXPORT_DLL	SBYTE		BMMapSetCase		(SDWORD x, SDWORD y, BMMAPSOL Value);
EXPORT_DLL	SBYTE		BMMapSetSol			(SDWORD x, SDWORD y, BMMAPSOL Value);
EXPORT_DLL	SBYTE		BMMapSetBlock		(SDWORD x, SDWORD y, BMMAPSOL Value);

extern	SBYTE			BMMapSetObject		(SDWORD x, SDWORD y, BMENTITY *ObjectToSet);
	
EXPORT_DLL	SBYTE		BMMapGetMaxPlayer	(UBYTE	*numberplayer);

EXPORT_DLL	SBYTE		BMMapGetBonusInit	(BMLIST	*BonusList);

#endif


