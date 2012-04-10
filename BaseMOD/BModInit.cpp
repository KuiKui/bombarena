#include "BMod.h"

//--------------------------------
// Globales
//--------------------------------

//*********************************************************************
// Name	: BMModInitPlayer
//---------------------------------------------------------------------
// In	: 
//---------------------------------------------------------------------
// Out	: 
//---------------------------------------------------------------------
// Author	: Mrk								11/01/2001 13:23:05
//*********************************************************************

void BMModInitPlayer(UBYTE pid)
{
	BMODPLAYERDATAS		*data;
	BMPLAYER			*player;

	BMPlayerGetPtr(&player, pid);

	if (player == NULL)				//test for the player
		return;

	data = (BMODPLAYERDATAS *)ALLOC(sizeof(BMODPLAYERDATAS));

	data->WalkSpeed		= BMOD_START_SPEED;
	data->NumberOfBombs	= BMOD_START_BOMB;
	data->BombLeft		= BMOD_START_BOMB;
	data->ThrowBomb		= TRUE;
	data->Fire			= BMOD_START_FIRE;

	player->ModData = (void *)data;
}


//**************************************************************
/** \fn     void BModResetPlayer(void)
 *
 *  \param  void 
 *
 *  \return void 
 *
 ***************************************************************
 *  \brief  
 *
 *  \author Mr Zziizz                  \date 05/04/2001 10:07:49
 ***************************************************************/

void BModResetPlayer(void)
{
	UBYTE			i;
	BMPLAYER		*player;

	for ( i = 0; i < BMPLAYERMAXNUMBER; i++ )
	{
		BMPlayerGetPtr(&player, i);
		if (player == NULL)
			continue;

		if ( ! player->ModData )
			continue;

		((BMODPLAYERDATAS *)(player->ModData))->WalkSpeed		= BMOD_START_SPEED;
		((BMODPLAYERDATAS *)(player->ModData))->NumberOfBombs	= BMOD_START_BOMB;
		((BMODPLAYERDATAS *)(player->ModData))->BombLeft		= BMOD_START_BOMB;
		((BMODPLAYERDATAS *)(player->ModData))->ThrowBomb		= TRUE;
		((BMODPLAYERDATAS *)(player->ModData))->Fire			= BMOD_START_FIRE;

		BMPlayerSetReady(i);
	}
}