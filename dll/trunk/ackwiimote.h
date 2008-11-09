/*
 *******************************************************************************
 * ackwiimote.h
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
 * Acknex game engine specific extensions for wiimote library - header
 *
 * Comments
 * 
 * IMPORTANT: Changes to this structure have to be copied to ackwii.h of
 * Wiimote Lite-C Project.
 * Array defines of Wiimote C-Script project have to be adapted as well.
 *
 *******************************************************************************
 */

#ifndef ACKWIIMOTE_H
#define ACKWIIMOTE_H

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

// engine specific header file
#define DLL_USE	// always define before including adll.h
#include "adll.h"	


#include "wiimote.h"

//battery update every 5 minutes
#define BAT_PERIOD 300	/* 300 seconds */

typedef struct
{
	var joy_x;
	var joy_y;
}STICK;

typedef struct
{
	var ir_x;
	var ir_y;
}IR;

typedef struct
{
	var l;
	var r;
}SHOULDER;

typedef struct
{
	var topright;
	var botright;
	var topleft;
	var botleft;
	var weight;
}BOARD;

typedef struct
{
	var butA;
	var butB;
	var butX;
	var butY;
	var butC;
	var butZ;
	var but1;
	var but2;
	var butL;
	var butR;
	var butZL;
	var butZR;
	var butPlus;
	var butMinus;
	var butHome;
	var butGreen;
	var butRed;
	var butYellow;
	var butBlue;
	var butOrange;
		
	var butAny;
}BUTTONS;

typedef struct
{
	var up;
	var down;
	var left;
	var right;
}DPAD;

typedef struct
{
	var ir;
	var nunchuk;
	var classic;
	var guitar;
	var balanceboard;
	var battery;
	var batteryraw;
	var vibration;
	var index;
}STATUS;

typedef struct
{
	//wiimote, Nunchuk, Classic Controller buttons
	void* on_a;
	void* on_b;
	void* on_x;
	void* on_y;
	void* on_c;
	void* on_z;
	void* on_1;
	void* on_2;
	void* on_l;
	void* on_r;
	void* on_zl;
	void* on_zr;
	void* on_plus;
	void* on_minus;
	void* on_home;
	//guitar buttons
	void* on_green;
	void* on_red;
	void* on_yellow;
	void* on_blue;
	void* on_orange;
	//directional pad
	void* on_up;
	void* on_down;
	void* on_left;
	void* on_right;
	
	void* on_any;
}EVT;

typedef struct
{
	ANGLE angle1;
	ANGLE angle2;
	
	VECTOR accel1;
	VECTOR accel2;
	
	STICK stick1;
	STICK stick2;
	
	BOARD board;
	
	IR ir[4];
	SHOULDER shoulder;
	BUTTONS buttons;
	DPAD dpad;
	
	STATUS status;
	EVT event;
}WIIMOTE;

typedef struct
{
	BUTTONS buttons;
	DPAD dpad;
}LOCKER;

class ackWiiMote:public cWiiMote
{
public:
	ackWiiMote();
	~ackWiiMote();

	bool ConnectToDevice(int index = 0);
	bool Disconnect();	
	void GetStatus(WIIMOTE * buffer);
	bool ShutdownRequested() const {return shutdown;}	
	void CallPointers(WIIMOTE * buffer);
	
private:
	bool shutdown;
	HANDLE ThreadHandle;
    DWORD ThreadID;
    var BatTimer;
    LOCKER locked;
    int dev_index;
    int batRaw, bat;
};

#endif
