#include "kmllight.h"

//*****************************************************************************GLOBALES

#define					BUFFERSIZE		5


// general

fonc					KMLSwitchFonction = NULL;
void					*KMLSwitchParameter;
char					KMLControlType;


// result
HRESULT					hr; 

// clavier
LPDIRECTINPUT			lpDI;
LPDIRECTINPUTDEVICE		lpDIDClavier;
unsigned char			KMLControlKeys[256];
unsigned char			KMLControlLastKeys[256];
unsigned char			KMLControlSwitchVar;

// mouse
LPDIRECTINPUTDEVICE		lpDIDMouse;
KMLMOUSE				KMLControlMouse;
KMLMOUSE				KMLControlLastMouse;

// pad
LPDIRECTINPUTDEVICE		lpDIDPad[KML_CONTROL_NBPADMAX];
LPDIRECTINPUTDEVICE2	lpDIDPad2[KML_CONTROL_NBPADMAX];
KMLPAD					KMLControlPad[KML_CONTROL_NBPADMAX];
KMLPAD					KMLControlLastPad[KML_CONTROL_NBPADMAX];
long					KMLControlPadState;

//*****************************************************************************FONCTIONS GLOBALES

unsigned long KMLControlGetNbButton(LPDIRECTINPUTDEVICE device);


//***************************************************************
//	FUNCTION : InitJoystickInput
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 18 juillet 2000
//***************************************************************

BOOL FAR PASCAL InitJoystickInput(LPCDIDEVICEINSTANCE pdinst, LPVOID pvRef) 
{ 
	DIPROPRANGE diprg; 

	if(lpDI->CreateDevice(pdinst->guidInstance, &lpDIDPad[KMLControlPadState], NULL) != DI_OK) 
		return DIENUM_CONTINUE;

	if(lpDIDPad[KMLControlPadState]->SetDataFormat(&c_dfDIJoystick) != DI_OK)
	{
		lpDIDPad[KMLControlPadState]->Release();
		return DIENUM_CONTINUE;
	}

	if(lpDIDPad[KMLControlPadState]->SetCooperativeLevel(hMainWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) != DI_OK) 
	{ 
		lpDIDPad[KMLControlPadState]->Release(); 
		return DIENUM_CONTINUE; 
	}

	diprg.diph.dwSize       = sizeof(diprg); 
	diprg.diph.dwHeaderSize = sizeof(diprg.diph); 
	diprg.diph.dwHow        = DIPH_BYOFFSET; 
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000; 

	diprg.diph.dwObj = DIJOFS_X; 
	if FAILED(lpDIDPad[KMLControlPadState]->SetProperty(DIPROP_RANGE, &diprg.diph) ) 
	{ 
		lpDIDPad[KMLControlPadState]->Release(); 
		return FALSE; 
	}
	
    diprg.diph.dwObj = DIJOFS_Y;
	if FAILED(lpDIDPad[KMLControlPadState]->SetProperty( DIPROP_RANGE, &diprg.diph) )
    { 
		lpDIDPad[KMLControlPadState]->Release(); 
		return FALSE; 
	}
	
    lpDIDPad[KMLControlPadState]->QueryInterface( IID_IDirectInputDevice2,(LPVOID *)&lpDIDPad2[KMLControlPadState] );
 	
	lpDIDPad[KMLControlPadState]->Acquire();

	KMLControlPadState++;

	return TRUE;
}

//***************************************************************
//	FUNCTION : ControlInit
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 10 juillet 2000
//***************************************************************

char KMLControlStartLib(HINSTANCE &hInst, char flag)
{
	HRESULT dirval;
	char	ret = KML_NOELMT, i;

	KMLSwitchFonction	= NULL;	//au depart pas de fonction CALLBACK par defaut
	KMLSwitchParameter	= NULL; //et pas de parametre

	KMLControlType		= flag;

	// dinput global
	dirval=DirectInputCreate(hInst,DIRECTINPUT_VERSION,&lpDI,NULL);
	if (SUCCEEDED(dirval))
	{
		// clavier
		if(KMLControlType&KML_CONTROL_KEY)
		{
			dirval=lpDI->CreateDevice(GUID_SysKeyboard,&lpDIDClavier,NULL);
			if (SUCCEEDED(dirval))
			{
				dirval=lpDIDClavier->SetDataFormat(&c_dfDIKeyboard);
				dirval=lpDIDClavier->SetCooperativeLevel(hMainWnd,DISCL_FOREGROUND |DISCL_NONEXCLUSIVE);
				if (SUCCEEDED(dirval))
				{
					dirval = lpDIDClavier->Acquire();
					if(SUCCEEDED(dirval))
						ret = KML_OK;
				}
			}
		}


		// souris
		if(KMLControlType&KML_CONTROL_MOUSE)
		{
			dirval=lpDI->CreateDevice(GUID_SysMouse,&lpDIDMouse,NULL);
			if (SUCCEEDED(dirval))
			{
				dirval=lpDIDMouse->SetDataFormat(&c_dfDIMouse);
				dirval=lpDIDMouse->SetCooperativeLevel(hMainWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
				if (SUCCEEDED(dirval))
				{
					DIPROPDWORD				MouseBuffer=
					{
						{
							sizeof(DIPROPDWORD),
							sizeof(DIPROPHEADER),
							0,
							DIPH_DEVICE,
						},
						BUFFERSIZE
					};

					dirval = lpDIDMouse->SetProperty(DIPROP_BUFFERSIZE, &MouseBuffer.diph);
					if(SUCCEEDED(dirval))
					{
						memset(&KMLControlMouse, 0, sizeof(KMLMOUSE));
						memset(&KMLControlLastMouse, 0, sizeof(KMLMOUSE));
						KMLControlMouse.nbbutton = (char)KMLControlGetNbButton(lpDIDMouse);
						if( KMLControlMouse.nbbutton > KML_CONTROL_MOUSE_NBBUTMAX )
							KMLControlMouse.nbbutton = KML_CONTROL_MOUSE_NBBUTMAX;
						ret = KML_OK;
					}
				}
			}
		}

		// pad
		if(KMLControlType&KML_CONTROL_PAD)
		{
			for(i=0; i<KMLControlPadState; i++)
			{
				lpDIDPad[i]	= NULL;
				lpDIDPad2[i]= NULL;
			}
			KMLControlPadState	= KML_CONTROL_NOPAD;
			ret = KML_ERROR_DI;

			dirval=lpDI->EnumDevices(DIDEVTYPE_JOYSTICK, InitJoystickInput, NULL, DIEDFL_ATTACHEDONLY);
			if(dirval == DI_OK)
			{
				for(i=0; i<KMLControlPadState; i++)
				{
					if(lpDIDPad[i])
					{
						memset(&KMLControlPad[i], 0, sizeof(KMLPAD));
						KMLControlPad[i].nbbutton	= (char)KMLControlGetNbButton(lpDIDPad[i]);
						ret = KML_OK;
					}
				}
				if(ret == KML_ERROR_DI)
					KMLControlPadState = KML_CONTROL_NOPAD;
			}
			else
				KMLControlPadState = KML_CONTROL_NOPAD;
		}
	}

	return ret;
}


//***************************************************************
//	FUNCTION : KMLControlPadInput
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 18 juillet 2000
//***************************************************************

char KMLControlPadInput(void)
{
	HRESULT					hr;
	DIJOYSTATE              js; 
	char					i,j;
	
	
	for(i=0; i<KMLControlPadState; i++)
	{
		// acquire
		lpDIDPad[i]->Acquire();
		hr = lpDIDPad2[i]->Poll();
		if ( hr != DI_OK )
		{
			memset(&KMLControlPad[i], 0, sizeof(KMLPAD)-1);
			memset(&KMLControlLastPad[i], 0, sizeof(KMLPAD)-1);
			return KML_ERROR_DI;
		}

		hr = lpDIDPad[i]->GetDeviceState( sizeof(DIJOYSTATE), &js );

		if ( hr == DIERR_INPUTLOST )
			hr = lpDIDPad[i]->Acquire();

		if (hr != DI_OK)
		{
				memset(&KMLControlPad[i], 0, sizeof(KMLPAD)-1);
				memset(&KMLControlLastPad[i], 0, sizeof(KMLPAD)-1);
				return KML_ERROR_DI;
		}

		// traitement
		memcpy(&KMLControlLastPad[i],	&KMLControlPad[i], sizeof(KMLPAD));
		memset(&KMLControlPad[i], 0, sizeof(KMLPAD)-1);

		if (js.lX < 0)	
		{ 
			KMLControlPad[i].left	= TRUE;
		}
		else if(js.lX > 0)
		{ 
			KMLControlPad[i].right = TRUE;
		}
 
		if (js.lY < 0)
		{ 
			KMLControlPad[i].up	= TRUE;
		}
		else if (js.lY > 0)
		{ 
			KMLControlPad[i].down	= TRUE;
		} 
		for(j=0; j<KMLControlPad[i].nbbutton; j++)
		{
			if (js.rgbButtons[j] & 0x80)
			{ 
				KMLControlPad[i].button[j]	= TRUE;
			} 
		} 
	}

	return KML_OK;
}

//***************************************************************
//	FUNCTION : KMLControlMouseInput
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : true or false
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 17 juillet 2000
//***************************************************************

char KMLControlMouseInput(void)
{
	static DWORD		LastClickTime;
	HRESULT				hr;
	DIDEVICEOBJECTDATA	tampon[BUFFERSIZE];
	DWORD				nbitems=BUFFERSIZE, d;
	int					j;

	if(lpDIDMouse)
	{
		if ( (hr=lpDIDMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), tampon, &nbitems, 0)) != DI_OK )
		{
			if (hr != DI_BUFFEROVERFLOW)
			{
				hr = lpDIDMouse->Acquire();
				if ( hr == DI_OK)
				{
					hr=lpDIDMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), tampon, &nbitems, 0);
				}
			}
		}
		if ( (hr != DI_OK) && (hr != DI_BUFFEROVERFLOW) ) 
		{
			if (hr == DIERR_INPUTLOST)
				return KML_ERROR_DI;
			if (hr == DIERR_NOTACQUIRED)
				return KML_ERROR_DI;
		}
		
		memcpy(&KMLControlLastMouse, &KMLControlMouse, sizeof(KMLMOUSE));
		


		for(d=0; d<nbitems; d++)
		{
			if(tampon[d].dwOfs == DIMOFS_X)
			{
					KMLControlMouse.x += tampon[d].dwData;
					if(KMLControlMouse.x<0)
						KMLControlMouse.x = 0;
					if(KMLControlMouse.x>=GLOBAL_RES_X)
						KMLControlMouse.x = GLOBAL_RES_X-1;
			}
			else if(tampon[d].dwOfs == DIMOFS_Y)
			{
					KMLControlMouse.y += tampon[d].dwData;
					if(KMLControlMouse.y<0)
						KMLControlMouse.y = 0;
					if(KMLControlMouse.y>=GLOBAL_RES_Y)
						KMLControlMouse.y = GLOBAL_RES_Y-1;
			}
			for(j=0; j<KMLControlMouse.nbbutton; j++)
			{
				// attention (DWORD)
				if( tampon[d].dwOfs == (DWORD)(FIELD_OFFSET(DIMOUSESTATE, rgbButtons) + j) )
				{
					if(tampon[d].dwData & 0x80)
					{
						KMLControlMouse.b[j] = TRUE;
					}
					else
					{
						KMLControlMouse.b[j] = FALSE;
					}
				}
			}
			
			if(tampon[d].dwOfs == DIMOFS_BUTTON2)
			{
//				exit(0);
			}
				
		}
		return KML_OK;
	}
	else
		return KML_ERROR_DI;
}

//***************************************************************
//	FUNCTION : KMLControlKeyInput
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mardi 18 juillet 2000
//***************************************************************

char KMLControlKeyInput(void)
{
	HRESULT hr;

	if(lpDIDClavier)
	{
		memcpy(KMLControlLastKeys, KMLControlKeys, 256);
		if( (hr = lpDIDClavier->GetDeviceState(256, &KMLControlKeys)) != DI_OK)
		{
			hr = lpDIDClavier->Acquire();
			if (hr == DI_OK)
				lpDIDClavier->GetDeviceState(256, &KMLControlKeys);
		}
		
		if (hr != DI_OK)
		{
			if (hr == DIERR_NOTACQUIRED)
				return KML_ERROR_DI;
		}
		

		
		if (KMLControlType & KML_CONTROL_SWITCH)
		{
			if ( (KMLControlKeysPress(DIK_LMENU, 1)) && (KMLControlKeysPress(DIK_RETURN)) )		//on teste si alt est enfonce et on verifie si entree est enfoncé 
			{//switch plein ecran																//mais sans repetition poure eviter les switch intempestifs
				KMLSwitch();
			}
		}

		return KML_OK;
	}
	else
		return KML_ERROR_DI;
}

//***************************************************************
//	FUNCTION : ControlGetInput
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 10 juillet 2000
//***************************************************************

char KMLControlUpdate(void)
{
	// clavier
	if(KMLControlType&KML_CONTROL_KEY)
		if (KMLControlKeyInput() != KML_OK)
			exit (2);
	
	// mouse
	if(KMLControlType&KML_CONTROL_MOUSE)
		KMLControlMouseInput();

	// pad
	if(KMLControlType&KML_CONTROL_PAD)
	{
		if(KMLControlPadState)
			KMLControlPadInput();
	}

	return KML_OK;
}

//***************************************************************
//	FUNCTION : ControlEnd
//---------------------------------------------------------------
//	INPUT	 : void
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  lundi 10 juillet 2000
//***************************************************************

void KMLControlQuitLib(void)
{
	char	i;

	if(lpDI)
	{
		// clavier
		if(lpDIDClavier)
		{
			lpDIDClavier->Release();
			lpDIDClavier= NULL;
		}

		// mouse
		if(lpDIDMouse)
		{
			lpDIDMouse->Release();
			lpDIDMouse= NULL;
		}

		// pad
		for(i=0; i<KMLControlPadState; i++)
		{
			if(lpDIDPad[i])
			{
				lpDIDPad[i]->Release();
				lpDIDPad[i]= NULL;
			}
		}

		lpDI->Release();
		lpDI = NULL;
	}
	KMLControlType = 0;
}


//***************************************************************
//	FUNCTION : KMLControlMouseButtonPress
//---------------------------------------------------------------
//	INPUT	 : button + slice
//---------------------------------------------------------------
//	OUTPUT	 : true or false
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  jeudi 3 août 2000
//***************************************************************

char KMLControlMouseButtonPress(char button, char slice)
{
	if(button >= KMLControlMouse.nbbutton)
		return FALSE;

	if(slice)
	{
		if(KMLControlMouse.b[button])
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if( (KMLControlMouse.b[button]) && (!(KMLControlLastMouse.b[button])) )
			return TRUE;
		else
			return FALSE;
	}
	
	return TRUE;
}

//**************************************************************
/** \fn     char KMLControlMouseGetState (KMLMOUSE *mouse)
 *
 *  \param  *mouse
 *
 *  \return  
 *
 ***************************************************************
 *  \brief  blah, for the dll
 *
 *  \author MikE                       \date 07/03/2001 14:55:03
 ***************************************************************/
char			KMLControlMouseGetState			(KMLMOUSE *mouse)
{
	memcpy( mouse, &KMLControlMouse, sizeof (KMLMOUSE));
	return (KML_OK);
}
//***************************************************************
//	FUNCTION : KMLControlKeysPress
//---------------------------------------------------------------
//	INPUT	 : button + slice + numpad
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 19 juillet 2000
//***************************************************************

char KMLControlKeysPress(int key, char slice)
{
	if(slice)
	{
		if(KMLControlKeys[key]&0x80)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		if( (KMLControlKeys[key]&0x80) && (!(KMLControlLastKeys[key]&0x80)) )
			return TRUE;
		else
			return FALSE;
	}
}

//***************************************************************
//	FUNCTION : KMLControlPadPress
//---------------------------------------------------------------
//	INPUT	 : button + slice + numpad
//---------------------------------------------------------------
//	OUTPUT	 : void
//---------------------------------------------------------------
//	AUTHOR	 : MrK  ->  mercredi 19 juillet 2000
//***************************************************************

char KMLControlPadPress(char button, char numpad, char slice)
{
	if(numpad >= KMLControlPadState)
		return FALSE;


	if( (button>=KML_PAD_B0) && (button<=KML_PAD_B31) )
	{
		if(button >= KMLControlPad[numpad].nbbutton)
			return FALSE;

		if(slice)
		{
			if(KMLControlPad[numpad].button[button])
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if( (KMLControlPad[numpad].button[button]) && !(KMLControlLastPad[numpad].button[button]) )
				return TRUE;
			else
				return FALSE;
		}
	}
	else if(button == KML_PAD_UP)
	{
		if(slice)
		{
			if(KMLControlPad[numpad].up)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if( (KMLControlPad[numpad].up) && !(KMLControlLastPad[numpad].up) )
				return TRUE;
			else
				return FALSE;
		}
	}
	else if(button == KML_PAD_DOWN)
	{
		if(slice)
		{
			if(KMLControlPad[numpad].down)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if( (KMLControlPad[numpad].down) && !(KMLControlLastPad[numpad].down) )
				return TRUE;
			else
				return FALSE;
		}
	}
	else if(button == KML_PAD_LEFT)
	{
		if(slice)
		{
			if(KMLControlPad[numpad].left)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if( (KMLControlPad[numpad].left) && !(KMLControlLastPad[numpad].left) )
				return TRUE;
			else
				return FALSE;
		}
	}
	else if(button == KML_PAD_RIGHT)
	{
		if(slice)
		{
			if(KMLControlPad[numpad].right)
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			if( (KMLControlPad[numpad].right) && !(KMLControlLastPad[numpad].right) )
				return TRUE;
			else
				return FALSE;
		}
	}

	return FALSE;
}

//***************************************************************
//	FUNCTION : KMLGetNumberOfButtons
//---------------------------------------------------------------
//	INPUT	 : Device
//---------------------------------------------------------------
//	OUTPUT	 : Nombre de bouttons
//---------------------------------------------------------------
//	AUTHOR	 : MikE ->  jeudi 20 juillet 2000
//***************************************************************

unsigned long KMLControlGetNbButton(LPDIRECTINPUTDEVICE device)
{
	DIDEVCAPS Caps;

	Caps.dwSize = sizeof (Caps);
	device->GetCapabilities(&Caps);

	return (Caps.dwButtons);
}

//*********************************************************************
//* FUNCTION: KMLControlSetCallbackSwitching
//*--------------------------------------------------------------------
//* DESCRIPT: met a jour la fonction appele lors d'un switch [ALT + ENTREE]
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : la fonction de type void fonc (void *), la parametre a passer
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    14/12/2000 14:08:33
//* REVISION:									
//*********************************************************************
void KMLControlSetCallbackSwitching (fonc fonction, void *lpParameter)
{
	KMLSwitchFonction = fonction;	
	KMLSwitchParameter = lpParameter;
}

//*********************************************************************
//* FUNCTION: KMLControlSetParameterSwitching 
//*--------------------------------------------------------------------
//* DESCRIPT: change le parametre a passer a la callback en cas de switch
//*                                                                    
//*--------------------------------------------------------------------
//* IN      : le parametre 
//*--------------------------------------------------------------------
//* OUT     : void
//*--------------------------------------------------------------------
//* AUTHOR  : MikE								    14/12/2000 14:10:48
//* REVISION:									
//*********************************************************************
void KMLControlSetParameterSwitching (void *lpParameter)
{
	KMLSwitchParameter = lpParameter;
}