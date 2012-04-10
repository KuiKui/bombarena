#include	"BMGlobalInclude.h"
#include	"BMTimer.h"

//--------------------------------
// Globales
//--------------------------------

/*LARGE_INTEGER	BMTimerFrequency;
LARGE_INTEGER	BMTimerCount;
LARGE_INTEGER	BMTimerCountPause;*/
UDWORD			BMTimerCount;
UDWORD			BMTimerCountPause;
UBYTE			BMTimerFlag;

//*********************************************************************
//* FUNCTION: BMTimerStart		
//*--------------------------------------------------------------------
//* DESCRIPT: Start the timer !
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 15:00:21
//* REVISION:									
//*********************************************************************
SBYTE		BMTimerStart		()
{
/*	if (QueryPerformanceFrequency(&BMTimerFrequency) == FALSE)
		return (BMTRUE - 1);

	BMTimerFrequency.QuadPart = BMTimerFrequency.QuadPart / 1000;	//set the frequency in count per ms

	if (QueryPerformanceCounter (&BMTimerCount) == FALSE)
		return (BMTRUE - 1);*/
	BMTimerCount = (UDWORD) GetTickCount ();

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMTimerEnd			
//*--------------------------------------------------------------------
//* DESCRIPT: nothing 
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 15:00:40
//* REVISION:									
//*********************************************************************
SBYTE		BMTimerEnd			()
{
	//nothing special
	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMTimerReset		
//*--------------------------------------------------------------------
//* DESCRIPT: reset the timer
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 15:00:55
//* REVISION:									
//*********************************************************************
SBYTE		BMTimerReset		()
{
/*	if (QueryPerformanceCounter (&BMTimerCount) == FALSE)
		return (BMTRUE - 1);*/
	BMTimerCount = (UDWORD) GetTickCount ();

	return (BMTRUE);
}

//*********************************************************************
//* FUNCTION: BMTimerGetTime	
//*--------------------------------------------------------------------
//* DESCRIPT: return the value of the time elapsed in ms
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : void
//*--------------------------------------------------------------------
//* OUT     : timer value
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    18/12/2000 15:01:13
//* REVISION:									
//*********************************************************************
UDWORD		BMTimerGetTime		(void)
{
	/*LARGE_INTEGER	temp;

	if (QueryPerformanceCounter (&temp) == FALSE)
			return (BMTRUE - 1);

	return ( (UDWORD) ( ( temp.QuadPart - BMTimerCount.QuadPart ) / BMTimerFrequency.QuadPart) ); */
	return ( (UDWORD)GetTickCount () - BMTimerCount );
}

//*********************************************************************
//* FUNCTION: BMTimerPause
//*--------------------------------------------------------------------
//* DESCRIPT: Pause the timer
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : nothing
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 15:37:00
//* REVISION:									
//*********************************************************************
SBYTE		BMTimerPause	()
{
	if (BMTimerFlag == 0)
	{
	/*	if (QueryPerformanceCounter (&BMTimerCountPause) == FALSE)
			return (BMTRUE - 1);*/

		BMTimerCountPause = (UDWORD) GetTickCount ();
		BMTimerFlag = 1;

		return (BMTRUE);
	}
	
	return (BMTRUE - 1);
}

//*********************************************************************
//* FUNCTION: BMTimerUnPause
//*--------------------------------------------------------------------
//* DESCRIPT: Unpause the timer
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : nothing
//*--------------------------------------------------------------------
//* OUT     : return code
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    25/01/2001 15:37:19
//* REVISION:									
//*********************************************************************
SBYTE		BMTimerUnPause	()
{
	//LARGE_INTEGER	temp;
	UDWORD temp;

	if (BMTimerFlag == 1)
	{
		/*if (QueryPerformanceCounter (&temp) == FALSE)
			return (BMTRUE - 1);

		BMTimerCount.QuadPart  += temp.QuadPart - BMTimerCountPause.QuadPart;*/
		temp = (UDWORD) GetTickCount ();
		BMTimerCount += temp - BMTimerCountPause;

		BMTimerFlag = 0;

		return (BMTRUE);
	}

	return (BMTRUE - 1);
}