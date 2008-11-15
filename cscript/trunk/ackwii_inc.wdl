/*
 *******************************************************************************
 * ackwii_inc.wdl
 * Creation date: 2008-10-25
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

ifndef ACKWII_INC;
define ACKWII_INC;

//////////LED Defines
/* 
use those defines when calling wiimote_led(...).
By default the lit diode represents the device number
*/
define LED1,	1;
define LED2,	2;
define LED3,	4;
define LED4,	8;


//////////Buffer Defines

/*
Those index defines can be used for easier access of the buffer data.
The buffer data is sorted in a var-array of size 'wiimote_buffer_size'

Examples of usage:
var buffer[wiimote_buffer_size]; -- defines a new buffer
buffer[up]                       -- check whether 'up' key is pressed
buffer[butA]                     -- check whether 'A' button is pressed
buffer[accel1_x]                 -- read acceleration in x direction of Wiimote
buffer[tilt2]                    -- read tilt of Nunchuk (if connected)
buffer[battery]                  -- read battery charge in percent (0-100)
buffer[on_b1] = foo;             -- assign function foo() to '1' button
*/

/* 
estimated orientation of Wiimote (can be inaccurate) 
range: -180° to +180°
*/
define angle1_pan, 		0;
define angle1_tilt, 	1;
define angle1_roll, 	2;

/* 
estimated orientation of Nunchuk (can be inaccurate) 
range: -180° to +180°
*/
define angle2_pan, 		3;
define angle2_tilt, 	4;
define angle2_roll, 	5;
	
/* acceleration of Wiimote */
define accel1_x, 		6;
define accel1_y, 		7;
define accel1_z, 		8;

/* acceleration of Nunchuk */
define accel2_x,		9;
define accel2_y,		10;
define accel2_z,		11;
	
/* 
either Nunchuk or Guitar analog stick or Classic Controller left analog stick
range: -255 (left/down) to 255 (right/up)
limits may slightly vary due to inaccurate calibration
*/
define joy1_x,			12;
define joy1_y,			13;
	
/* 
classic controller right analog stick
range: -255 (left/down) to 255 (right/up)
limits may slightly vary due to inaccurate calibration
*/
define joy2_x,			14;
define joy2_y,			15;
	
/* balance board data
range: 0kg (no pressure) to ~150kg (full pressure)
weight retrieves the total weight (sum of weight detdected by all 4 pressure sensors)
*/
define topright,		16;
define botright,		17;
define topleft,			18;
define botleft,			19;
define weight,			20;

/* 
IR sensor data (up to 4 IR dots can be detected)
range: 0 (left) to 1023 (right) 	(x)
range: 0 (top) to 767 (bottom)	(y)
*/
define ir1_x,			21;
define ir1_y,			22;
define ir2_x,			23;
define ir2_y,			24;
define ir3_x,			25;
define ir3_y,			26;
define ir4_x,			27;
define ir4_y,			28;

/* 
shoulder button data of classic controller, whammy bar (left shoulder) of guitar
range: 0 (not pressed) to 255 (fully pressed)
limits may slightly vary due to inaccurate calibration
*/
define sl,				29;
define sr,				30;

/* 
button data of Wiimote and expansion device
Classic Controller, Guitar and Balance Board buttons are mapped to Wiimote buttons with the same caption
range: 0 (not pressed) or 1 (pressed)
*/
define butA,			31;
define butB,			32;
define butX,			33;
define butY,			34;
define butC,			35;
define butZ,			36;
define but1,			37;
define but2,			38;
define butL,			39;
define butR,			40;
define butZL,			41;
define butZR,			42;
define butPlus,			43;
define butMinus,		44;
define butHome,			45;
define butGreen,		46;
define butRed,			47;
define butYellow,		48;
define butBlue,			49;
define butOrange,		50;
	
define butAny,			51;

/* 
directional pad data of either Wiimote, Classic Controller or Guitar
range: 0 (not pressed) or 1 (pressed)
*/
define up,				52;
define down,			53;
define left,			54;
define right,			55;
	
/* 
status variables and flags of Wiimote/expansion device 
May also be recieved by using extra functions (see below), but to keep
things as easy as possible you can also use these values in your scripts
range: 0 or 1 for ir, nunchuk, classic, guitar, balanceboard, vibration
range: 0 to x for index, x being number of found devices - 1
range: 0 to 100 for battery
range: 0 to 0xC0 for batteryraw
*/
define ir,				56;
define nunchuk,			57;
define classic,			58;
define guitar,			59;
define balanceboard,	60;
define battery,			61;
define batteryraw,		62;
define vibration,		63;
define index,			64;

/* 
on_xxx events for assigning functions to Wiimote/expansion device buttons

assign user defined functions to these pointers the way it is normally
done in Acknex (on_f1, on_q, ...) e.g. x[on_ba] = myfunc;
all functions assigned to these pointers automatically recieve 
two parameters upon call:
param: 	(var) handle for wiimote device which triggered event
param:	(var&) buffer array of queried device
*/
define on_ba,			65;
define on_bb,			66;
define on_bx,			67;
define on_by,			68;
define on_bc,			69;
define on_bz,			70;
define on_b1,			71;
define on_b2,			72;
define on_bl,			73;
define on_br,			74;
define on_bzl,			75;
define on_bzr,			76;
define on_bplus,		77;
define on_bminus,		78;
define on_bhome,		79;
define on_bgreen,		80;
define on_bred,			81;
define on_byellow,		82;
define on_bblue,		83;
define on_borange,		84;
define on_bup,			85;
define on_bdown,		86;
define on_bleft,		87;
define on_bright,		88;
	
define on_bany,			89;

/* required array size for buffer - DO NOT CHANGE! */ 
define wiimote_buffer_size, 90;

//////////DLL Prototypes
/*
Get number of connected devices
!Note!: Bluesoleil driver always detects all listed remotes
      even if they are not connected anymore!
param:	-
return:	number of connected wiimote devices 
*/
dllfunction wiimote_getdevices();


/* 
establish connection for device with given index 
param:	index of device, first device beginning with 0
return: 	handle for wiimote device or NULL 
*/
dllfunction wiimote_connect(dev_index);


/* 
close connection - must be called before exit 
param: 	handle for wiimote device
return:	0 - failed		1 - successful
*/
dllfunction wiimote_disconnect(handle);


/* 
turn off/on LEDs of Wiimote 
param: 	handle for wiimote device
param: 	4 bits (0-15) for LED1-4 (use defines: LED1...4)
return:	0 - failed		1 - successful
Balance Board only supports LED1
*/
dllfunction wiimote_led(handle, led);


/* 
turn off(0)/on(1) Vibration 
param: 	handle for wiimote device
param:	1 - on		0 - off
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
dllfunction wiimote_vibration(handle, vib_on);
 

/* 
read motion sensors, IR sensor, analog stick and button states 
from Wiimote/Nunchuk/Classic Controller/Balance Board
param: 	handle for wiimote device
param:	address of buffer of type WIIMOTE or array var[wiimote_buffer_size] 
return:	0 - failed		1 - successful
*/
dllfunction wiimote_status(handle, &buffer);


/*
enable/disable infrared camera
param: 	handle for wiimote device
param:	0 - off (default)		1 - on
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
dllfunction wiimote_ir(handle, ir_on);


/*
set factor for smoothing of Wiimote angle data
param: 	handle for wiimote device
param:	smooth factor (0...1)
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
dllfunction wiimote_smoothfac(handle, fac);


/* 
check whether a certain device is connected/active 
use these functions if you don't want to read those values from your buffer
param: 	handle for wiimote device
return: 	0 - inactive/failed		1 - active 
*/
dllfunction wiimote_connected(handle);
dllfunction wiimote_nunchuk_connected(handle);
dllfunction wiimote_classic_connected(handle);
dllfunction wiimote_guitar_connected(handle);
dllfunction wiimote_balanceboard_connected(handle);
dllfunction wiimote_ir_active(handle);
dllfunction wiimote_vibration_active(handle);


/* get battery charge  
param: 	handle for wiimote device
return:	battery charge in percent (0 - 100)
*/
dllfunction wiimote_battery(handle);


/* get battery charge  
param: 	handle for wiimote device
return:	battery charge raw value (0 - 0xC0 (192))
*/
dllfunction wiimote_battery_raw(handle);



ifdef WII_DEBUG;
/* debug functions
define WII_DEBUG to enable Wiimote debugging
debug mode on/off via F12 - is activated automatically
*/

function wiimote_panel(&buffer);
function wiimote_print(handle, &buffer);
function wiimote_debug(&buffer);
function wiimote_on();
function wiimote_off();

include "ackwii.wdl";
endif;

endif;


