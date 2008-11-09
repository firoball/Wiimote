/*
 *******************************************************************************
 * wiimote.c
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
 * Sample script demonstrating usage of Wiimote
 *
 * Comments
 * 
 * for short descriptions see comments in ackwii.h
 *
 *******************************************************************************
 */

#include <acknex.h>
#include <default.c>

#define WII_DEBUG	//define before including ackwii.h to enable debugging
#include "ackwii.h"



 
/* assigned to '1' button */
void toggleIR (var handle, WIIMOTE* buffer)
{
	/* note: IR sensor requires a sensor bar or adequate replacement */
	wiimote_ir(handle, 1 - buffer->status.ir);		
}

/* assigned to '2' button */
void toggleVibration (var handle, WIIMOTE* buffer)
{
	wiimote_vibration(handle, 1 - buffer->status.vibration);		
}

WIIMOTE buffer;	//needs to be declared globally

void main ()
{
	video_mode = 7; 
	screen_color.blue = 150;
	fps_max = 100;
	
	var wii_handle;
	/* clear buffer before using!! */
	zero(buffer);

	/* assign functions to Wiimote buttons */
	buffer.event.on_1 = toggleIR;
	buffer.event.on_2 = toggleVibration;
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
#ifdef WII_DEBUG
			wiimote_debug(&buffer);
#endif
	
			while (key_esc != 1)
			{
				/* update buffer with data coming from Wiimote */
				wiimote_status(wii_handle, &buffer);
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
