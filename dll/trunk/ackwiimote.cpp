/*
 *******************************************************************************
 * ackwiimote.cpp
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
 * Acknex game engine specific extensions for wiimote library
 *
 *******************************************************************************
 */

#include "ackwiimote.h"

ackWiiMote::ackWiiMote():cWiiMote()
{
	shutdown = false;
	//battery count needs some time to initialize, set timer initial value slightly below maximum to reach limit very early
	BatTimer = _VAR(((float)BAT_PERIOD - 0.4)) * 16;	
	memset(&locked, 0, sizeof(locked));
	dev_index = -1;
	batRaw = bat = 0;
	
	/* IR pointer variables */
	posX = _VAR(0);
	posY = _VAR(0);
	lastPosX = _VAR(0);
	lastPosY = _VAR(0);
	point_distX = _VAR(0); 
	point_distY = _VAR(0);
	lastIRX1 = _VAR(0);
	lastIRX2 = _VAR(0);
	lastIRY1 = _VAR(0);
	lastIRY2 = _VAR(0);

}

ackWiiMote::~ackWiiMote()
{
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	ackWiiMote* wiimote = (ackWiiMote*)lpParam;
	while(!wiimote->ShutdownRequested())
    {
		wiimote->HeartBeat();
    }
	return 0;
}

bool ackWiiMote::ConnectToDevice(int index)
{
	if (cWiiMote::ConnectToDevice(index) && RequestExpansionReport())
    {
		dev_index = index;
        ThreadHandle = CreateThread(NULL,0,ThreadProc,(void*)this,0,&ThreadID);
		SetLEDs((bool)index==0, (bool)index==1, (bool)index==2, (bool)index==3);
   		return true;
    }
    else
		return false;
}
	
bool ackWiiMote::Disconnect()	
{
	shutdown = true;
	WaitForSingleObject(ThreadHandle, INFINITE);
	CloseHandle(ThreadHandle);
	return cWiiMote::Disconnect();
}

void ackWiiMote::GetStatus(WIIMOTE * buffer)
{
	/* temp variables */
	static float pan, tilt, roll;
	static float w, x, y, z;
	static int i;
	static int BatTimerLimit = BAT_PERIOD * 16;
	
	/* only for prerelease */
	//draw_text("Prerelease version - only use for testing!", _VAR(5), _VAR(5), vector(_VAR(0),_VAR(0),_VAR(200)));
	
	/* to make things easier for the developer, analog sticks and buttons with same caption of different devices
	are mapped to the same variables inside the buffer struct. Theoretically it is possible to distinguish
	between e.g. a Nunchuk and a Classic Controller analog stick, but it's easier for the developer if he can
	just access the first analog stick unrelated to which device is currently connected. 
	See comment for each specific device for further info.
	*/
	
	/* Wiimote
	- acceleration is mapped to 'accel1'
	- orientation is mapped to 'angle1'
	*/
	GetOrientation(pan, tilt, roll);
	buffer->angle1.pan  = _VAR((int)(pan * 180 / PI));
	buffer->angle1.tilt = _VAR((int)(tilt * 180 / PI));
	buffer->angle1.roll = _VAR((int)(roll * 180 / PI));

	GetCalibratedAcceleration(x, y, z);
	buffer->accel1.x = _VAR((int)(x * 255));
	buffer->accel1.y = _VAR((int)(y * 255));
	buffer->accel1.z = _VAR((int)(z * 255));
	
	buffer->buttons.but1 = _VAR(mLastButtonStatus.m1);
	buffer->buttons.but2 = _VAR(mLastButtonStatus.m2);

	for (i = 0; i < 4; i++)
	{
		GetIRP(x, y, z, i);
		buffer->ir[i].ir_x = _VAR((int)(x * 1023));
		/* invert x direction */
		if (buffer->ir[i].ir_x != _VAR(0))
			buffer->ir[i].ir_x = _VAR(1023) - buffer->ir[i].ir_x;
		buffer->ir[i].ir_y = _VAR((int)(y * 767));
		buffer->ir[i].size = _VAR((int)z);
	}	
	/* calculate IR pointer data based on buffer data of Wiimote 
	write back resulting positions directly to buffer */
	SetPointer(buffer);	

	/* Nunchuk 
	- stick is mapped to 'stick1'
	- acceleration is mapped to 'accel2'
	- orientation is mapped to 'angle2'
	*/	
	GetChukOrientation(pan, tilt, roll);
	buffer->angle2.pan  = _VAR((int)(pan * 180 / PI));
	buffer->angle2.tilt = _VAR((int)(tilt * 180 / PI));
	buffer->angle2.roll = _VAR((int)(roll * 180 / PI));

	GetCalibratedChuckAcceleration(x, y, z);
	buffer->accel2.x = _VAR((int)(x * 255));
	buffer->accel2.y = _VAR((int)(y * 255));
	buffer->accel2.z = _VAR((int)(z * 255));

	GetCalibratedChuckStick(x,y);
	buffer->stick1.joy_x = _VAR((int)(x * 255));
	buffer->stick1.joy_y = _VAR((int)(y * 255));

	buffer->buttons.butC = _VAR(mLastChuckReport.mButtonC);
	buffer->buttons.butZ = _VAR(mLastChuckReport.mButtonZ);
	
	/* Classic Controller
	- left stick is mapped to 'stick1'
	- right stick is mapped to 'stick2'
	- buttons A, B, -, +, home are mapped over Wiimote buttons
	- dpad is mapped over Wiimote dpad
	*/
	//left stick is shared with nunchuk (or-ed!!)
	GetCalibratedLeftClassicStick(x, y);
	buffer->stick1.joy_x |= _VAR((int)(x * 255));
	buffer->stick1.joy_y |= _VAR((int)(y * 255));

	GetCalibratedRightClassicStick(x, y);
	buffer->stick2.joy_x = _VAR((int)(x * 255));
	buffer->stick2.joy_y = _VAR((int)(y * 255));
		
	GetCalibratedLeftClassicShoulder(x);
	buffer->shoulder.l = _VAR((int)(x * 255));
	GetCalibratedRightClassicShoulder(x);
	buffer->shoulder.r = _VAR((int)(x * 255));
		
	/* Guitar Controller 
	- stick is mapped to 'stick1'
	- whammy bar is mapped to 'shoulder.l'
	*/
	//stick is shared with Nunchuk and Classic Controller (or-ed!!)
	GetCalibratedGuitarStick(x, y);
	buffer->stick1.joy_x |= _VAR((int)(x * 255));
	buffer->stick1.joy_y |= _VAR((int)(y * 255));
	
	//whammy bar is shared with Classic Controller left shoulder button (or-ed!!)
	GetCalibratedGuitarAnalog(x);
	buffer->shoulder.l |= _VAR((int)(x * 255));

	/* Balance Board
	- button data as received the same way as Wiimote - Balance Board uses Button 'A'
	*/
	GetCalibratedBalanceBoardSensors(w, x, y, z);
	buffer->board.topright = _VAR(w);
	buffer->board.botright = _VAR(x);
	buffer->board.topleft  = _VAR(y);
	buffer->board.botleft  = _VAR(z);
	buffer->board.weight   = _VAR((w + x + y + z));

	//Dpad is shared by Classic Controller, Wiimote and Guitar
	buffer->dpad.up    = _VAR(mLastClassicReport.mUp|mLastButtonStatus.mUp|mLastGuitarReport.mUp);
	buffer->dpad.down  = _VAR(mLastClassicReport.mDown|mLastButtonStatus.mDown|mLastGuitarReport.mDown);
	buffer->dpad.left  = _VAR(mLastClassicReport.mLeft|mLastButtonStatus.mLeft);
	buffer->dpad.right = _VAR(mLastClassicReport.mRight|mLastButtonStatus.mRight);

	//buttons are partly shared by Classic Controller, Wiimote and Guitar
	buffer->buttons.butA      = _VAR(mLastClassicReport.mA|mLastButtonStatus.mA);
	buffer->buttons.butB      = _VAR(mLastClassicReport.mB|mLastButtonStatus.mB);
	buffer->buttons.butX      = _VAR(mLastClassicReport.mX);
	buffer->buttons.butY      = _VAR(mLastClassicReport.mY);
	buffer->buttons.butPlus   = _VAR(mLastClassicReport.mPlus|mLastButtonStatus.mPlus|mLastGuitarReport.mPlus);
	buffer->buttons.butMinus  = _VAR(mLastClassicReport.mMinus|mLastButtonStatus.mMinus|mLastGuitarReport.mMinus);
	buffer->buttons.butHome   = _VAR(mLastClassicReport.mHome|mLastButtonStatus.mHome);
	buffer->buttons.butL      = _VAR(mLastClassicReport.mL);
	buffer->buttons.butR      = _VAR(mLastClassicReport.mR);
	buffer->buttons.butZL     = _VAR(mLastClassicReport.mZL);
	buffer->buttons.butZR     = _VAR(mLastClassicReport.mZR);
	buffer->buttons.butGreen  = _VAR(mLastGuitarReport.mG);
	buffer->buttons.butRed    = _VAR(mLastGuitarReport.mR);
	buffer->buttons.butYellow = _VAR(mLastGuitarReport.mY);
	buffer->buttons.butBlue   = _VAR(mLastGuitarReport.mB);
	buffer->buttons.butOrange = _VAR(mLastGuitarReport.mO);
		
	buffer->buttons.butAny = _VAR((INT)(mLastClassicReport.mUp|mLastButtonStatus.mUp|mLastGuitarReport.mUp
								|mLastClassicReport.mDown|mLastButtonStatus.mDown|mLastGuitarReport.mDown
								|mLastClassicReport.mLeft|mLastButtonStatus.mLeft
								|mLastClassicReport.mRight|mLastButtonStatus.mRight
								|mLastClassicReport.mA|mLastButtonStatus.mA
								|mLastClassicReport.mB|mLastButtonStatus.mB
								|mLastClassicReport.mX|mLastClassicReport.mY
								|mLastClassicReport.mPlus|mLastButtonStatus.mPlus|mLastGuitarReport.mPlus
								|mLastClassicReport.mMinus|mLastButtonStatus.mMinus|mLastGuitarReport.mMinus
								|mLastClassicReport.mHome|mLastButtonStatus.mHome
								|mLastClassicReport.mL|mLastClassicReport.mR
								|mLastClassicReport.mZL|mLastClassicReport.mZR
								|mLastChuckReport.mButtonC|mLastChuckReport.mButtonZ
								|mLastButtonStatus.m1|mLastButtonStatus.m2
								|mLastGuitarReport.mG|mLastGuitarReport.mR
								|mLastGuitarReport.mY|mLastGuitarReport.mB
								|mLastGuitarReport.mO
								));

	//additionally report status of connected devices
	buffer->status.ir           = _VAR((int)ir_active());
	buffer->status.nunchuk      = _VAR((int)nunchuk_active());
	buffer->status.classic      = _VAR((int)classic_active());
	buffer->status.guitar       = _VAR((int)guitar_active());
	buffer->status.balanceboard = _VAR((int)balanceboard_active());
	buffer->status.vibration    = _VAR((int)vibration_active());
	buffer->status.index        = _VAR(dev_index);
	
	//only periodically update battery status
	if (BatTimer < _VAR(BatTimerLimit))
	{
		BatTimer += v(time_step);
	}
	else
	{
		BatTimer -= _VAR(BatTimerLimit);

		//don't do this too often in short time frames - very slow
		bat = (int)GetBatteryPercent();
		batRaw = (int)GetRawBatteryLevel();
	}
	buffer->status.battery = _VAR(bat);
	buffer->status.batteryraw = _VAR(batRaw);

	//if any function is assigned to a button, call it
	CallPointers(buffer);
}

void ackWiiMote::CallPointers(WIIMOTE * buffer)
{
	//this function calls Lite-C/C-Script functions depending on button presses
	//"locked" stores whether a function was already called upon button press
	//this prevents functions from being called repeatedly as long as button is pressed

	if (buffer->buttons.butA)
	{
		if (!locked.buttons.butA)
			engine_callscript((long)buffer->event.on_a, (long)this, (long)buffer, 0, 0);
		locked.buttons.butA = _VAR(1);
	}
	else
		locked.buttons.butA = _VAR(0);


	if (buffer->buttons.butB)
	{
		if (!locked.buttons.butB)
			engine_callscript((long)buffer->event.on_b, (long)this, (long)buffer, 0, 0);
		locked.buttons.butB = _VAR(1);
	}
	else
		locked.buttons.butB = _VAR(0);


	if (buffer->buttons.butX)
	{
		if (!locked.buttons.butX)
			engine_callscript((long)buffer->event.on_x, (long)this, (long)buffer, 0, 0);
		locked.buttons.butX = _VAR(1);
	}
	else
		locked.buttons.butX = _VAR(0);


	if (buffer->buttons.butY)
	{
		if (!locked.buttons.butY)
			engine_callscript((long)buffer->event.on_y, (long)this, (long)buffer, 0, 0);
		locked.buttons.butY = _VAR(1);
	}
	else
		locked.buttons.butY = _VAR(0);


	if (buffer->buttons.butC)
	{
		if (!locked.buttons.butC)
			engine_callscript((long)buffer->event.on_c, (long)this, (long)buffer, 0, 0);
		locked.buttons.butC = _VAR(1);
	}
	else
		locked.buttons.butC = _VAR(0);


	if (buffer->buttons.butZ)
	{
		if (!locked.buttons.butZ)
			engine_callscript((long)buffer->event.on_z, (long)this, (long)buffer, 0, 0);
		locked.buttons.butZ = _VAR(1);
	}
	else
		locked.buttons.butZ = _VAR(0);


	if (buffer->buttons.but1)
	{
		if (!locked.buttons.but1)
			engine_callscript((long)buffer->event.on_1, (long)this, (long)buffer, 0, 0);
		locked.buttons.but1 = _VAR(1);
	}
	else
		locked.buttons.but1 = _VAR(0);


	if (buffer->buttons.but2)
	{
		if (!locked.buttons.but2)
			engine_callscript((long)buffer->event.on_2, (long)this, (long)buffer, 0, 0);
		locked.buttons.but2 = _VAR(1);
	}
	else
		locked.buttons.but2 = _VAR(0);


	if (buffer->buttons.butL)
	{
		if (!locked.buttons.butL)
			engine_callscript((long)buffer->event.on_l, (long)this, (long)buffer, 0, 0);
		locked.buttons.butL = _VAR(1);
	}
	else
		locked.buttons.butL = _VAR(0);


	if (buffer->buttons.butR)
	{
		if (!locked.buttons.butR)
			engine_callscript((long)buffer->event.on_r, (long)this, (long)buffer, 0, 0);
		locked.buttons.butR = _VAR(1);
	}
	else
		locked.buttons.butR = _VAR(0);


	if (buffer->buttons.butZL)
	{
		if (!locked.buttons.butZL)
			engine_callscript((long)buffer->event.on_zl, (long)this, (long)buffer, 0, 0);
		locked.buttons.butZL = _VAR(1);
	}
	else
		locked.buttons.butZL = _VAR(0);


	if (buffer->buttons.butZR)
	{
		if (!locked.buttons.butZR)
			engine_callscript((long)buffer->event.on_zr, (long)this, (long)buffer, 0, 0);
		locked.buttons.butZR = _VAR(1);
	}
	else
		locked.buttons.butZR = _VAR(0);


	if (buffer->buttons.butPlus)
	{
		if (!locked.buttons.butPlus)
			engine_callscript((long)buffer->event.on_plus, (long)this, (long)buffer, 0, 0);
		locked.buttons.butPlus = _VAR(1);
	}
	else
		locked.buttons.butPlus = _VAR(0);


	if (buffer->buttons.butMinus)
	{
		if (!locked.buttons.butMinus)
			engine_callscript((long)buffer->event.on_minus, (long)this, (long)buffer, 0, 0);
		locked.buttons.butMinus = _VAR(1);
	}
	else
		locked.buttons.butMinus = _VAR(0);


	if (buffer->buttons.butHome)
	{
		if (!locked.buttons.butHome)
			engine_callscript((long)buffer->event.on_home, (long)this, (long)buffer, 0, 0);
		locked.buttons.butHome = _VAR(1);
	}
	else
		locked.buttons.butHome = _VAR(0);

	//guitar buttons
	if (buffer->buttons.butGreen)
	{
		if (!locked.buttons.butGreen)
			engine_callscript((long)buffer->event.on_green, (long)this, (long)buffer, 0, 0);
		locked.buttons.butGreen = _VAR(1);
	}
	else
		locked.buttons.butGreen = _VAR(0);

	if (buffer->buttons.butRed)
	{
		if (!locked.buttons.butRed)
			engine_callscript((long)buffer->event.on_red, (long)this, (long)buffer, 0, 0);
		locked.buttons.butRed = _VAR(1);
	}
	else
		locked.buttons.butRed = _VAR(0);

	if (buffer->buttons.butYellow)
	{
		if (!locked.buttons.butYellow)
			engine_callscript((long)buffer->event.on_yellow, (long)this, (long)buffer, 0, 0);
		locked.buttons.butYellow = _VAR(1);
	}
	else
		locked.buttons.butYellow = _VAR(0);

	if (buffer->buttons.butBlue)
	{
		if (!locked.buttons.butBlue)
			engine_callscript((long)buffer->event.on_blue, (long)this, (long)buffer, 0, 0);
		locked.buttons.butBlue = _VAR(1);
	}
	else
		locked.buttons.butBlue = _VAR(0);

	if (buffer->buttons.butOrange)
	{
		if (!locked.buttons.butOrange)
			engine_callscript((long)buffer->event.on_orange, (long)this, (long)buffer, 0, 0);
		locked.buttons.butOrange = _VAR(1);
	}
	else
		locked.buttons.butOrange = _VAR(0);

	//directional pad
	if (buffer->dpad.up)
	{
		if (!locked.dpad.up)
			engine_callscript((long)buffer->event.on_up, (long)this, (long)buffer, 0, 0);
		locked.dpad.up = _VAR(1);
	}
	else
		locked.dpad.up = _VAR(0);


	if (buffer->dpad.down)
	{
		if (!locked.dpad.down)
			engine_callscript((long)buffer->event.on_down, (long)this, (long)buffer, 0, 0);
		locked.dpad.down = _VAR(1);
	}
	else
		locked.dpad.down = _VAR(0);


	if (buffer->dpad.left)
	{
		if (!locked.dpad.left)
			engine_callscript((long)buffer->event.on_left, (long)this, (long)buffer, 0, 0);
		locked.dpad.left = _VAR(1);
	}
	else
		locked.dpad.left = _VAR(0);


	if (buffer->dpad.right)
	{
		if (!locked.dpad.right)
			engine_callscript((long)buffer->event.on_right, (long)this, (long)buffer, 0, 0);
		locked.dpad.right = _VAR(1);
	}
	else
		locked.dpad.right = _VAR(0);


	if (buffer->buttons.butAny)
	{
		if (!locked.buttons.butAny)
			engine_callscript((long)buffer->event.on_any, (long)this, (long)buffer, 0, 0);
		locked.buttons.butAny = _VAR(1);
	}
	else
		locked.buttons.butAny = _VAR(0);
}

void ackWiiMote::SetPointer(WIIMOTE* buffer)
{	
	static var limitX, limitY;
	
	/* IR active */
	if (buffer->status.ir != 0)
	{
		/* second IR dot = 0/0 and first IR dot != 0/0 */
		if (buffer->ir[1].ir_x == _VAR(0) && buffer->ir[1].ir_y == _VAR(0) &&
			(buffer->ir[0].ir_x != _VAR(0) || buffer->ir[0].ir_y != _VAR(0)))
		{
			/* single IR dot found */

			/* check to which of the old dots the current dot is closer */
			posX = buffer->ir[0].ir_x;
			posY = buffer->ir[0].ir_y;

			/* --- x direction  --- */
			/* "first" dot still active */
			if (absv(lastIRX1 - posX) < absv(lastIRX2 - posX))
			{
				lastIRX2 = _VAR(999999); /* sth big */
				posX -= point_distX / 2;
			}

			else
			/* "second" dot still active */
			{
				lastIRX1 = _VAR(999999); /* sth big */
				posX += point_distX / 2;
			}

			/* --- y direction  --- */
			/* "first" dot still active */
			if (absv(lastIRY1 - posY) < absv(lastIRY2 - posY))
			{
				lastIRY2 = _VAR(999999); /* sth big to avoid false detection */
				posY -= point_distY / 2;
			}

			else
			/* "second" dot still active */
			{
				lastIRY1 = _VAR(999999); /* sth big to avoid false detection */
				posY += point_distY / 2;
			}

			/* before updating position data, check last value */
			posX = CLAMP(posX, _VAR(0), _VAR(1023));
			posY = CLAMP(posY, _VAR(0), _VAR(767));

			/* store last position values */
			lastPosX = posX;
			lastPosY = posY;
		}
		/* second IR dot != 0/0 and first IR dot != 0/0 */
		else 
		{
			/* calculate x distance of dots */
			if (buffer->ir[0].ir_x != _VAR(0) && buffer->ir[1].ir_x != _VAR(0))
			{
				/* negative value: dot 2 > dot 1, positive value: dot 1 > dot 2 */
				point_distX = buffer->ir[0].ir_x - buffer->ir[1].ir_x;
			}
		
			/* calculate y distance of dots */
			if (buffer->ir[0].ir_y != _VAR(0) && buffer->ir[1].ir_y != _VAR(0))
			{
				/* negative value: dot 2 > dot 1, positive value: dot 1 > dot 2 */
				point_distY = buffer->ir[0].ir_y - buffer->ir[1].ir_y;
			}

			/* two IR dots found */
			if (buffer->ir[0].ir_x != _VAR(0) || buffer->ir[0].ir_y != _VAR(0))
			{
				posX = integer((buffer->ir[0].ir_x + buffer->ir[1].ir_x) / 2);
				posY = integer((buffer->ir[0].ir_y + buffer->ir[1].ir_y) / 2);

				/* store last position values */
				lastPosX = posX;
				lastPosY = posY;

				/* store last IR values */
				lastIRX1 = 	buffer->ir[0].ir_x;	
				lastIRX2 = 	buffer->ir[1].ir_x;	
				lastIRY1 = 	buffer->ir[0].ir_y;	
				lastIRY2 = 	buffer->ir[1].ir_y;	
			}
			/* second IR dot = 0/0 and first IR dot = 0/0 */
			else
			{
				/* no IR dot found - keep last values */
				posX = lastPosX;
				posY = lastPosY; 
			}
		}

	
		/* correct influence of roll angle of Wiimote */
/* needs fixing
		posX -= _VAR(1023) / 2;
		posY -= _VAR(767) / 2;
		posX = (var)(cosv(buffer->angle1.roll) * _INT(posX) - sinv(buffer->angle1.roll) * _INT(posY));
		posY = (var)(sinv(buffer->angle1.roll) * _INT(posX) + cosv(buffer->angle1.roll) * _INT(posY));
		posX += _VAR(1023) / 2;
		posY += _VAR(767) / 2;
*/

		/* scale to screen resolution */
		posX = (var)(_FLOAT(v(screen_size).x) * posX / 1023.0f);	
		posY = (var)(_FLOAT(v(screen_size).y) * posY / 767.0f);	

	}
	/* IR not active */
	else
	{
		posX        = _VAR(0);
		posY        = _VAR(0);
		lastIRX1    = _VAR(0);
		lastIRX2    = _VAR(0);
		lastIRY1    = _VAR(0);
		lastIRX2    = _VAR(0);
		point_distX = _VAR(0);
		point_distY = _VAR(0);
	}

	/* write to buffer */
	buffer->pointer.ir_x = integer(posX);
	buffer->pointer.ir_y = integer(posY);
}
