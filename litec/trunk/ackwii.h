/*
 *******************************************************************************
 * ackwii.h
 * Creation date: 2008
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
 * Definition script demonstrating usage of Wiimote
 *
 * Comments
 * 
 * for short descriptions see comments in this file
 *
 *******************************************************************************
 */

#ifndef ACKWII_H
#define ACKWII_H


//////////LED Defines
/* 
use those defines when calling wiimote_led(...).
By default the lit diode represents the device number
*/
#define LED1	1
#define LED2	2
#define LED3	4
#define LED4	8

//////////Structures

/*
struct containing the analog stick data.
range: -255 (left/down) to 255 (right/up)
limits may slightly vary due to inaccurate calibration
*/

typedef struct
{
	var joy_x;
	var joy_y;
}STICK;


/*
struct containing the IR sensor data.
range: 0 (left) to 1023 (right) 	(x)
range: 0 (top) to 767 (bottom)	(y)
*/

typedef struct
{
	var ir_x;
	var ir_y;
	var size;
}IR;


/*
struct containing the IR pointer data.
range: 0 (left) to screen width (right) 	(x)
range: 0 (top)  to screen size (bottom)	(y)
*/

typedef struct
{
	var ir_x;
	var ir_y;
}IRPTR;


/*
struct containing the shoulder button data.
range: 0 (not pressed) to 255 (fully pressed)
limits may slightly vary due to inaccurate calibration
*/

typedef struct
{
	var l;
	var r;
}SHOULDER;


/*
struct containing the balance board data.
range: 0kg (no pressure) to ~150kg (full pressure)
weight retrieves the total weight (sum of weight detdected by all 4 pressure sensors)
*/
typedef struct
{
	var topright;
	var botright;
	var topleft;
	var botleft;
	var weight;
}BOARD;

/*
struct containing the button data.
range: 0 (not pressed) or 1 (pressed)
*/

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


/*
struct containing the directional pad data.
range: 0 (not pressed) or 1 (pressed)
*/

typedef struct
{
	var up;
	var down;
	var left;
	var right;
}DPAD;


/*
this struct contains some extra information about the Wiimote status.
May also be recieved with extra functions (see below), but to keep
things as easy as possible you can also use these values in your scripts
range: 0 or 1 for ir, nunchuk, classic, vibration
range: 0 to x for index, x being number of found devices - 1
range: 0 to 100 for battery
range: 0 to 0xC0 for batteryraw
*/

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


/* 
assign user defined functions to these pointers the way it is normally
done in Acknex (on_f1, on_q, ...) e.g. x.event.on_a = myfunc;
all functions assigned to these pointers automatically recieve 
two parameters upon call:
param: 	(var) handle for wiimote device which triggered event
param:	(WIIMOTE*) pointer to buffer which was queried
*/

typedef struct
{
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
	void* on_green;
	void* on_red;
	void* on_yellow;
	void* on_blue;
	void* on_orange;
	void* on_up;
	void* on_down;
	void* on_left;
	void* on_right;
	
	void* on_any;
}EVT;

/*
The main structure. It contains all structures above
Since it stores all data coming from Wiimote it's usually called
'buffer' throughout the ackwii files.

Examples of usage:
WIIMOTE buffer;			-- defines a new buffer
buffer.dpad.up				-- check whether 'up' key is pressed
buffer.buttons.butA		-- check whether 'A' button is pressed
buffer.accel1.x			-- read acceleration in x direction of Wiimote
buffer.angle2.tilt		-- read tilt of Nunchuk (if connected)
buffer.status.battery	-- read battery charge in percent (0-100)
buffer.event.on_1 = foo;-- assign function foo() to '1' button
*/

typedef struct
{
	/* 
	estimated orientation of Wiimote (can be inaccurate) 
	range: -180° to +180°
	*/
	ANGLE angle1;

	/* 
	estimated orientation of Nunchuk (can be inaccurate) 
	range: -180° to +180°
	*/
	ANGLE angle2;
	
	/* acceleration of Wiimote */
	VECTOR accel1;

	/* acceleration of Nunchuk */
	VECTOR accel2;
	
	/* either Nunchuk or Guitar analog stick or Classic Controller left analog stick */ 
	STICK stick1;
	
	/* classic controller right analog stick */
	STICK stick2;
	
	/* balance board data */
	BOARD board;

	/* IR sensor data (up to 4 IR dots can be detected) */
	IR ir[4];

	/* IR pointer data */
	IRPTR pointer;
	
	/* shoulder button data of classic controller, whammy bar (left shoulder) of guitar */
	SHOULDER shoulder;

	/* 
	button data of Wiimote and expansion device
	Classic Controller, Guitar and Balance Board buttons are mapped to Wiimote buttons with the same caption
	*/
	BUTTONS buttons;

	/* directional pad data of either Wiimote, Classic Controller or Guitar */
	DPAD dpad;
	
	/* status variables and flags of Wiimote/expansion device */
	STATUS status;

	/* on_xxx events for assigning functions to Wiimote/expansion device buttons */
	EVT event;
}WIIMOTE;



//////////DLL Prototypes
/*
Get number of connected devices
!Note!: Bluesoleil driver always detects all listed remotes
      even if they are not connected anymore!
param:	-
return:	number of connected wiimote devices 
*/
var wiimote_getdevices();


/* 
establish connection for device with given index 
param:	index of device, first device beginning with 0
return: 	handle for wiimote device or NULL 
*/
var wiimote_connect(var dev_index);


/* 
close connection - must be called before exit 
param: 	handle for wiimote device
return:	0 - failed		1 - successful
*/
var wiimote_disconnect(var handle);


/* 
turn off/on LEDs of Wiimote 
param: 	handle for wiimote device
param: 	4 bits (0 - 15) for LED1-4 (use defines: LED1...4)
return:	0 - failed		1 - successful
Balance Board only supports LED1
*/
var wiimote_led(var handle, var led);


/* 
turn off(0)/on(1) Vibration 
param: 	handle for wiimote device
param:	1 - on		0 - off
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
var wiimote_vibration(var handle, var vib_on);
 

/* 
read motion sensors, IR sensor, analog stick and button states 
from Wiimote/Nunchuk/Classic Controller/Guitar/Balance Board
param: 	handle for wiimote device
param:	address of buffer of type WIIMOTE or array var[96] 
return:	0 - failed		1 - successful
*/
var wiimote_status(var handle, void* buffer);


/*
enable/disable infrared camera
param: 	handle for wiimote device
param:	0 - off (default)		1 - on
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
var wiimote_ir(var handle, var ir_on);


/*
set factor for smoothing of Wiimote angle data
param: 	handle for wiimote device
param:	smooth factor (0...1)
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
var wiimote_smoothfac(var handle, var fac);


/*
set sensivity level for Wiimote IR camera
param: 	handle for wiimote device
param:	sensity level (0...4) (default: 2)
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
var wiimote_ir_sensitivity(var handle, var level);


/* 
check whether a certain device is connected/active 
use these functions if you don't want to read those values from your buffer
param: 	handle for wiimote device
return: 	0 - inactive/failed		1 - active 
*/
var wiimote_connected(var handle);
var wiimote_nunchuk_connected(var handle);
var wiimote_classic_connected(var handle);
var wiimote_guitar_connected(var handle);
var wiimote_balanceboard_connected(var handle);
var wiimote_ir_active(var handle);
var wiimote_vibration_active(var handle);


/* get battery charge  
param: 	handle for wiimote device
return:	battery charge in percent (0 - 100)
*/
var wiimote_battery(var handle);


/* get battery charge  
param: 	handle for wiimote device
return:	battery charge raw value (0 - 0xC0 (192))
*/
var wiimote_battery_raw(var handle);



#ifdef WII_DEBUG
/* debug functions
define WII_DEBUG to enable Wiimote debugging
debug mode on/off via F12 - is activated automatically
*/

void wiimote_print(var handle, WIIMOTE* buffer);
void wiimote_debug(WIIMOTE* buffer);
void wiimote_move_irdot(WIIMOTE* buffer);
void wiimote_on();
void wiimote_off();

#include "ackwii.c"
#endif

#endif

