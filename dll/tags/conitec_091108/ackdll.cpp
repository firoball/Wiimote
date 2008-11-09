/*
 *******************************************************************************
 * ackdll.cpp
 * Creation date: 2007
 * Author:        Firoball
 *
 *******************************************************************************
 * $Date$
 * $Revision$
 * $Author$
 *
 *******************************************************************************
 * Description
 * 
 * Interface module to A7 game engine
 * provide function calls for Lite-C/C-Script
 *
 *******************************************************************************
 */

#include "ackdll.h"

// this list is filled with handles in order to check handles handled over to DLL for validity
ackWiiMote* wiiPtrList[WII_HANDLES];
int wiiPtrIndex;                // next free slot index of pointer list
bool wiiPtrInitialized = false; // indication flag wether list was already initialized

void wiimote__initList()
{
	// init should only be called once
	if (!wiiPtrInitialized)
	{
		wiiPtrIndex = 0;
		wiiPtrInitialized = true;
		for (int i = 0; i < WII_HANDLES; i++)
		{
			wiiPtrList[i] = NULL;
		}
	}
}

bool wiimote__checkHandle(ackWiiMote* handle)
{
	bool retval = false;

	if (handle != NULL)
	{
		for (int i = 0; i < WII_HANDLES; i++)
		{
			// check if handle is stored in list
			if (handle == wiiPtrList[i])
			{
				retval = true;
				break;
			}
		}
	}
	return retval;
}

void wiimote__addHandle(ackWiiMote* handle)
{
	if (wiiPtrIndex < WII_HANDLES)
	{
		// store wiimote handle in handle list
		wiiPtrList[wiiPtrIndex] = handle;
		wiiPtrIndex++;
	}
}

void wiimote__removeHandle(ackWiiMote* handle)
{
	for (int i = 0; i < WII_HANDLES; i++)
	{
		// check if handle is stored in list
		if (handle == wiiPtrList[i])
		{
			// shift all following handles to avoid gaps in list
			if (i < WII_HANDLES - 1)
			{
				for (int j = i; j < WII_HANDLES - 1; j++)
				{
					// shift all handles as long as last entry is not reached yet
					if (wiiPtrList[j + 1] != NULL)
					{
						wiiPtrList[j] = wiiPtrList[j + 1];	
					}
					// last handle reached
					else
					{
						wiiPtrList[j] = NULL;
						break;	
					}	
				}
			}
			// removed handle was last one in list
			else
			{
				wiiPtrList[i] = NULL;
			}
			wiiPtrIndex--;
			break;
		}
	}
}

// DLL main entry point
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved )
{
// todo: auto disconnect upon exit... check http://msdn.microsoft.com/en-us/library/ms682583.aspx
// make sure the library is linked
	engine_bind();
    return TRUE;
}

DLLFUNC var wiimote_connect(var dev_index)
{
	ackWiiMote *wiimote = new ackWiiMote;

	wiimote__initList();
	if (wiimote->ConnectToDevice(_INT(dev_index)))
	{
		wiimote__addHandle(wiimote);
		return (var)wiimote;
	}
	else
	{
		delete wiimote;
    	//not successful - return invalid handle
		return (var)NULL;
	}
}

DLLFUNC var wiimote_connected(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 
	
	if (wiimote__checkHandle(wiimote))
		retval = wiimote->IsConnected();

	return _VAR((int)retval);
}

DLLFUNC var wiimote_nunchuk_connected(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
		retval = wiimote->nunchuk_active();

	return _VAR((int)retval);
}

DLLFUNC var wiimote_classic_connected(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
		retval = wiimote->classic_active();
		
	return _VAR((int)retval);
}

DLLFUNC var wiimote_guitar_connected(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle;
	 
	if (wiimote__checkHandle(wiimote))
		retval = wiimote->guitar_active();
		
	return _VAR((int)retval);
}

DLLFUNC var wiimote_balanceboard_connected(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
		retval = wiimote->balanceboard_active();
		
	return _VAR((int)retval);
}

DLLFUNC var wiimote_ir_active(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
		retval = wiimote->ir_active();
		
	return _VAR((int)retval);
}

DLLFUNC var wiimote_vibration_active(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
		retval = wiimote->vibration_active();

	return _VAR((int)retval);
}

DLLFUNC var wiimote_disconnect(var handle)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
	{
    	wiimote->SetVibration(false);
		wiimote->SetLEDs(false, false, false, false);
		retval = wiimote->Disconnect();
		wiimote__removeHandle(wiimote);
		delete wiimote;
	}
	
	return _VAR((int)retval);
}

DLLFUNC var wiimote_status(var handle, var buffer)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle; 

	if (wiimote__checkHandle(wiimote))
	{
		wiimote->GetStatus((WIIMOTE*)buffer);
		retval = true;
	}

	return _VAR((int)retval);
}

DLLFUNC var wiimote_led(var handle, var led)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
	{
		wiimote->SetLEDs((bool)(_INT(led) & 1), (bool)(_INT(led) & 2), (bool)(_INT(led) & 4), (bool)(_INT(led) & 8));
		retval = true;
	}
	
	return _VAR((int)retval);
}

DLLFUNC var wiimote_vibration(var handle, var vib_on)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
	{
	    wiimote->SetVibration((bool)(_INT(vib_on) & 1));
		retval = true;
	}
	
	return _VAR((int)retval);
}

DLLFUNC var wiimote_battery(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
	    return _VAR((int)wiimote->GetBatteryPercent());
	else
		return _VAR(0);
}

DLLFUNC var wiimote_battery_raw(var handle)
{
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
    	return _VAR((int)wiimote->GetRawBatteryLevel());
	else
		return _VAR(0);
}

DLLFUNC var wiimote_getdevices(void)
{
	return _VAR(cWiiMote::GetDevices());
}

DLLFUNC var wiimote_ir(var handle, var ir_on)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
	{
		if (_INT(ir_on) != 0)
   	 		wiimote->ActivateIR();
   		else
    		wiimote->DeactivateIR();
  		retval = true;  	
	}

	return _VAR((int)retval);
}

DLLFUNC var wiimote_smoothfac(var handle, var fac)
{
	bool retval = false;
	ackWiiMote *wiimote = (ackWiiMote*)handle;

	if (wiimote__checkHandle(wiimote))
	{
		wiimote->SetSmoothFac(_FLOAT(fac));
		retval = true;
	}

	return _VAR((int)retval);
}
