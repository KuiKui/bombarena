//--------------------------------
// Include Guard
//--------------------------------
#ifndef __BMWin32Type_H__
#define	__BMWin32Type_H__


typedef			unsigned long			UDWORD;
typedef			signed long				SDWORD;
										
typedef			unsigned short			UWORD;
typedef			signed short			SWORD;
										
typedef			unsigned char			UBYTE;
typedef			signed char				SBYTE;
										
typedef			char					STRING;

typedef			struct _BMLPOINT		BMLPOINT;
typedef			struct _BMFPOINT		BMFPOINT;


struct _BMLPOINT
{
	SDWORD			x;
	SDWORD			y;
};

struct _BMFPOINT
{
	float			x;
	float			y;
};

#endif // __BMWin32Type_H__