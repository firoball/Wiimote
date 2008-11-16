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
define topleft,		18;
define botleft,		19;
define weight,			20;

/* 
IR sensor data (up to 4 IR dots can be detected)
range: 0 (left) to 1023 (right) 	(x)
range: 0 (top)  to 767 (bottom)	(y)
range: 0 to 15							(size) 
*/
define ir1_x,			21;
define ir1_y,			22;
define ir1_size,		23;
define ir2_x,			24;
define ir2_y,			25;
define ir2_size,		26;
define ir3_x,			27;
define ir3_y,			28;
define ir3_size,		29;
define ir4_x,			30;
define ir4_y,			31;
define ir4_size,		32;

/* 
IR pointer data
range: 0 (left) to screen width (right) 	(x)
range: 0 (top)  to screen height (bottom)	(y)
*/
define ir_x,			33;
define ir_y,			34;

/* 
shoulder button data of classic controller, whammy bar (left shoulder) of guitar
range: 0 (not pressed) to 255 (fully pressed)
limits may slightly vary due to inaccurate calibration
*/
define sl,				35;
define sr,				36;

/* 
button data of Wiimote and expansion device
Classic Controller, Guitar and Balance Board buttons are mapped to Wiimote buttons with the same caption
range: 0 (not pressed) or 1 (pressed)
*/
define butA,			37;
define butB,			38;
define butX,			39;
define butY,			40;
define butC,			41;
define butZ,			42;
define but1,			43;
define but2,			44;
define butL,			45;
define butR,			46;
define butZL,			47;
define butZR,			48;
define butPlus,		49;
define butMinus,		50;
define butHome,		51;
define butGreen,		52;
define butRed,			53;
define butYellow,		54;
define butBlue,		55;
define butOrange,		56;
	
define butAny,			57;

/* 
directional pad data of either Wiimote, Classic Controller or Guitar
range: 0 (not pressed) or 1 (pressed)
*/
define up,				58;
define down,			59;
define left,			60;
define right,			61;
	
/* 
status variables and flags of Wiimote/expansion device 
May also be recieved by using extra functions (see below), but to keep
things as easy as possible you can also use these values in your scripts
range: 0 or 1 for ir, nunchuk, classic, guitar, balanceboard, vibration
range: 0 to x for index, x being number of found devices - 1
range: 0 to 100 for battery
range: 0 to 0xC0 for batteryraw
*/
define ir,				62;
define nunchuk,		63;
define classic,		64;
define guitar,			65;
define balanceboard,	66;
define battery,		67;
define batteryraw,	68;
define vibration,		69;
define index,			70;

/* 
on_xxx events for assigning functions to Wiimote/expansion device buttons

assign user defined functions to these pointers the way it is normally
done in Acknex (on_f1, on_q, ...) e.g. x[on_ba] = myfunc;
all functions assigned to these pointers automatically recieve 
two parameters upon call:
param: 	(var) handle for wiimote device which triggered event
param:	(var&) buffer array of queried device
*/
define on_ba,			71;
define on_bb,			72;
define on_bx,			73;
define on_by,			74;
define on_bc,			75;
define on_bz,			76;
define on_b1,			77;
define on_b2,			78;
define on_bl,			79;
define on_br,			80;
define on_bzl,			81;
define on_bzr,			82;
define on_bplus,		83;
define on_bminus,		84;
define on_bhome,		85;
define on_bgreen,		86;
define on_bred,		87;
define on_byellow,	88;
define on_bblue,		89;
define on_borange,	90;
define on_bup,			91;
define on_bdown,		92;
define on_bleft,		93;
define on_bright,		94;
	
define on_bany,		95;

/* required array size for buffer - DO NOT CHANGE! */ 
define wiimote_buffer_size, 96;

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
set sensivity level for Wiimote IR camera
param: 	handle for wiimote device
param:	sensity level (0...4) (default: 2)
return:	0 - failed		1 - successful
Not supported by Balance Board
*/
dllfunction wiimote_ir_sensitivity(handle, level);


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


