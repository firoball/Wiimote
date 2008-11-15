
/* Sample script demonstrating usage of Wiimote
   for short descriptions see comments in ackwii_inc.wdl
   10/25/08 - Firoball
*/

define WII_DEBUG;	/* define before including ackwii_inc.wdl to enable debugging */
plugindir ".";
include "ackwii_inc.wdl";


/* assigned to '1' button */
function toggleIR (handle, &buffer)
{
	/* note: IR sensor requires a sensor bar or adequate replacement */
	wiimote_ir(handle, 1 - buffer[ir]);		
}

/* assigned to '2' button */
function toggleVibration (handle, &buffer)
{
	wiimote_vibration(handle, 1 - buffer[vibration]);		
}

var buffer[wiimote_buffer_size];	/* needs to be declared globally */
var video_mode = 7;

function main ()
{
	screen_color.blue = 150;
	fps_max = 100;
	
	var wii_handle;

	/* assign functions to Wiimote buttons */
	buffer[on_b1] = toggleIR;
	buffer[on_b2] = toggleVibration;
	on_esc = NULL;
	
	/* check if any Wiimote is found */
	if (wiimote_getdevices() > 0)
	{
		/* connect first device */
		wii_handle = wiimote_connect(0);
		if(wii_handle != NULL)
		{
			/*
			debug mode: show buffer in panel and assign log function to B button
			of Wiimote. Use F12 to toggle visibility of panel
			*/
ifdef WII_DEBUG;
			wiimote_debug(buffer);
endif;
	
			while (key_esc != 1)
			{
				/* update buffer with data coming from Wiimote */
				wiimote_status(wii_handle, buffer);
				wait(1);
			}
			/*
			IMPORTANT: this function must be called before closing the engine window.
			Otherwise engine will crash! Also wait a few ticks to ensure that DLL is
			not running anymore.
			*/
			wiimote_disconnect(wii_handle);
			wait(2);
		}
	}	
	sys_exit("");
}
