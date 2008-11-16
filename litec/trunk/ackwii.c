/*
 *******************************************************************************
 * ackwii.c
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
 * Debug script for using with Wiimote
 *
 * Comments
 * 
 * for short descriptions see comments in ackwii.h
 *
 * The following sample script reads the button and sensor states of Wiimote
 * and expansion device
 * define WII_DEBUG to enable debug mode
 * start/stop by pressing F12
 * for recording values keep B button of Wiimote pressed
 * 
 * IMPORTANT NOTE: never include this file directly!
 * include 'ackwii.h' and define 'WII_DEBUG' in your main script instead!
 * 
 *******************************************************************************
 */

	
//////////Debug helpers

FONT* wiifont = "ackfont.pcx";

/* panel to show values retrieved from wiimote and expansion device */
PANEL* wii_panel = 
{
	pos_x	= 0;
	pos_y	= 130;

	digits(0,	0,		"Wiimote", wiifont, 1, 1);
	digits(0,	15,		"X %-+5.0f", wiifont, 1, 1);
	digits(60,	15,		"Y %-+5.0f", wiifont, 1, 1);
	digits(120,	15,		"Z %-+5.0f", wiifont, 1, 1);

	digits(0,	30,		"P %-+5.0f", wiifont, 1, 1);
	digits(60,	30,		"T %-+5.0f", wiifont, 1, 1);
	digits(120,	30,		"R %-+5.0f", wiifont, 1, 1);

	digits(0,	45,		"X1 %-+4.0f", wiifont, 1, 1);
	digits(50,	45,		"X2 %-+4.0f", wiifont, 1, 1);
	digits(100,	45,		"X3 %-+4.0f", wiifont, 1, 1);
	digits(150,	45,		"X4 %-+4.0f", wiifont, 1, 1);
	digits(0,	60,		"Y1 %-+4.0f", wiifont, 1, 1);
	digits(50,	60,		"Y2 %-+4.0f", wiifont, 1, 1);
	digits(100,	60,		"Y3 %-+4.0f", wiifont, 1, 1);
	digits(150,	60,		"Y4 %-+4.0f", wiifont, 1, 1);
	digits(0,	75,		"S1 %-+4.0f", wiifont, 1, 1);
	digits(50,	75,		"S2 %-+4.0f", wiifont, 1, 1);
	digits(100,	75,		"S3 %-+4.0f", wiifont, 1, 1);
	digits(150,	75,		"S4 %-+4.0f", wiifont, 1, 1);


	digits(0,	95,		"Nunchuk", wiifont, 1, 1);
	digits(0,	110,		"X %-+5.0f", wiifont, 1, 1);
	digits(60,	110,		"Y %-+5.0f", wiifont, 1, 1);
	digits(120,	110,		"Z %-+5.0f", wiifont, 1, 1);

	digits(0,	125,		"P %-+5.0f", wiifont, 1, 1);
	digits(60,	125,		"T %-+5.0f", wiifont, 1, 1);
	digits(120,	125,		"R %-+5.0f", wiifont, 1, 1);


	digits(0,	145,		"Analog sticks", wiifont, 1, 1);
	digits(0,	160,		"X1 %-+3.0f", wiifont, 1, 1);
	digits(50,	160,		"Y1 %-+3.0f", wiifont, 1, 1);
	digits(100,	160,		"X2 %-+3.0f", wiifont, 1, 1);
	digits(150,	160,		"Y2 %-+3.0f", wiifont, 1, 1);


	digits(0,	180,		"Direction pad", wiifont, 1, 1);
	digits(0,	195,		"UP %1.0f", wiifont, 1, 1);
	digits(40,	195,		"DN %1.0f", wiifont, 1, 1);
	digits(80,	195,		"LT %1.0f", wiifont, 1, 1);
	digits(120,	195,		"RT %1.0f", wiifont, 1, 1);


	digits(0,	215,		"Shoulder buttons", wiifont, 1, 1);
	digits(0,	230,		"LT %-3.0f", wiifont, 1, 1);
	digits(40,	230,		"RT %-3.0f", wiifont, 1, 1);


	digits(0,	250,		"Button states", wiifont, 1, 1);
	digits(0,	265,		"A  %1.0f", wiifont, 1, 1);
	digits(40,	265,		"B  %1.0f", wiifont, 1, 1);
	digits(80,	265,		"1  %1.0f", wiifont, 1, 1);
	digits(120,	265,		"2  %1.0f", wiifont, 1, 1);

	digits(0,	280,		"X  %1.0f", wiifont, 1, 1);
	digits(40,	280,		"Y  %1.0f", wiifont, 1, 1);
	digits(80,	280,		"C  %1.0f", wiifont, 1, 1);
	digits(120,	280,		"Z  %1.0f", wiifont, 1, 1);

	digits(0,	295,		"L  %1.0f", wiifont, 1, 1);
	digits(40,	295,		"R  %1.0f", wiifont, 1, 1);
	digits(80,	295,		"ZL %1.0f", wiifont, 1, 1);
	digits(120,	295,		"ZR %1.0f", wiifont, 1, 1);

	digits(0,	310,		"Gr %1.0f", wiifont, 1, 1);
	digits(40,	310,		"Re %1.0f", wiifont, 1, 1);
	digits(80,	310,		"Ye %1.0f", wiifont, 1, 1);
	digits(120,	310,		"Bl %1.0f", wiifont, 1, 1);
	digits(160,	310,		"Or %1.0f", wiifont, 1, 1);

	digits(0,	325,		"-  %1.0f", wiifont, 1, 1);
	digits(40,	325,		"HM %1.0f", wiifont, 1, 1);
	digits(80,	325,		"+  %1.0f", wiifont, 1, 1);


	digits(0,	345,		"Balance Board", wiifont, 1, 1);
	digits(0,	360,		"TR %3.0f", wiifont, 1, 1);
	digits(40,	360,		"BR %3.0f", wiifont, 1, 1);
	digits(80,	360,		"TL %3.0f", wiifont, 1, 1);
	digits(120,	360,		"BL %3.0f", wiifont, 1, 1);
	digits(160,	360,		"W  %3.0f", wiifont, 1, 1);


	digits(0,	380,		"Devices", wiifont, 1, 1);
	digits(0,	395,		"IR %1.0f", wiifont, 1, 1);
	digits(40,	395,		"N  %1.0f", wiifont, 1, 1);
	digits(80,	395,		"CC %1.0f", wiifont, 1, 1);
	digits(120,	395,		"G  %1.0f", wiifont, 1, 1);
	digits(160,	395,		"BB %1.0f", wiifont, 1, 1);
	digits(200,	395,		"BAT %3.0fprc", wiifont, 1, 1);

	digits(0,	425,		"F12: toggle display - Keep B pressed on Wiimote to rec. motion sensors", wiifont, 1, 1);

	flags	|= OVERLAY;
}

BMAP* ir_map = "ir_dot.tga";
PANEL* ir_panel = 
{
	pos_x = 0;
	pos_y = 0;
	bmap = ir_map;
}

/* assign a buffer to the debug panel */
void wiimote_panel(WIIMOTE *buffer)
{
	int i;
	
	pan_setvar(wii_panel, 1, 2, &(buffer->accel1.x));
	pan_setvar(wii_panel, 1, 3, &(buffer->accel1.y));
	pan_setvar(wii_panel, 1, 4, &(buffer->accel1.z));

	pan_setvar(wii_panel, 1, 5, &(buffer->angle1.pan));
	pan_setvar(wii_panel, 1, 6, &(buffer->angle1.tilt));
	pan_setvar(wii_panel, 1, 7, &(buffer->angle1.roll));

	for (i = 0; i< 4; i++)
	{
		pan_setvar(wii_panel, 1, 8+i, &(buffer->ir[i].ir_x));
		pan_setvar(wii_panel, 1, 12+i, &(buffer->ir[i].ir_y));
		pan_setvar(wii_panel, 1, 16+i, &(buffer->ir[i].size));
	}

	pan_setvar(wii_panel, 1, 21, &(buffer->accel2.x));
	pan_setvar(wii_panel, 1, 22, &(buffer->accel2.y));
	pan_setvar(wii_panel, 1, 23, &(buffer->accel2.z));

	pan_setvar(wii_panel, 1, 24, &(buffer->angle2.pan));
	pan_setvar(wii_panel, 1, 25, &(buffer->angle2.tilt));
	pan_setvar(wii_panel, 1, 26, &(buffer->angle2.roll));

	pan_setvar(wii_panel, 1, 28, &(buffer->stick1.joy_x));
	pan_setvar(wii_panel, 1, 29, &(buffer->stick1.joy_y));
	pan_setvar(wii_panel, 1, 30, &(buffer->stick2.joy_x));
	pan_setvar(wii_panel, 1, 31, &(buffer->stick2.joy_y));

	pan_setvar(wii_panel, 1, 33, &(buffer->dpad.up));
	pan_setvar(wii_panel, 1, 34, &(buffer->dpad.down));
	pan_setvar(wii_panel, 1, 35, &(buffer->dpad.left));
	pan_setvar(wii_panel, 1, 36, &(buffer->dpad.right));

	pan_setvar(wii_panel, 1, 38, &(buffer->shoulder.l));
	pan_setvar(wii_panel, 1, 39, &(buffer->shoulder.r));
	
	pan_setvar(wii_panel, 1, 41, &(buffer->buttons.butA));
	pan_setvar(wii_panel, 1, 42, &(buffer->buttons.butB));
	pan_setvar(wii_panel, 1, 43, &(buffer->buttons.but1));
	pan_setvar(wii_panel, 1, 44, &(buffer->buttons.but2));

	pan_setvar(wii_panel, 1, 45, &(buffer->buttons.butX));
	pan_setvar(wii_panel, 1, 46, &(buffer->buttons.butY));
	pan_setvar(wii_panel, 1, 47, &(buffer->buttons.butC));
	pan_setvar(wii_panel, 1, 48, &(buffer->buttons.butZ));

	pan_setvar(wii_panel, 1, 49, &(buffer->buttons.butL));
	pan_setvar(wii_panel, 1, 50, &(buffer->buttons.butR));
	pan_setvar(wii_panel, 1, 51, &(buffer->buttons.butZL));
	pan_setvar(wii_panel, 1, 52, &(buffer->buttons.butZR));

	pan_setvar(wii_panel, 1, 53, &(buffer->buttons.butGreen));
	pan_setvar(wii_panel, 1, 54, &(buffer->buttons.butRed));
	pan_setvar(wii_panel, 1, 55, &(buffer->buttons.butYellow));
	pan_setvar(wii_panel, 1, 56, &(buffer->buttons.butBlue));
	pan_setvar(wii_panel, 1, 57, &(buffer->buttons.butOrange));

	pan_setvar(wii_panel, 1, 58, &(buffer->buttons.butMinus));
	pan_setvar(wii_panel, 1, 59, &(buffer->buttons.butHome));
	pan_setvar(wii_panel, 1, 60, &(buffer->buttons.butPlus));

	pan_setvar(wii_panel, 1, 62, &(buffer->board.topright));
	pan_setvar(wii_panel, 1, 63, &(buffer->board.botright));
	pan_setvar(wii_panel, 1, 64, &(buffer->board.topleft));
	pan_setvar(wii_panel, 1, 65, &(buffer->board.botleft));
	pan_setvar(wii_panel, 1, 66, &(buffer->board.weight));

	pan_setvar(wii_panel, 1, 68, &(buffer->status.ir));
	pan_setvar(wii_panel, 1, 69, &(buffer->status.nunchuk));
	pan_setvar(wii_panel, 1, 70, &(buffer->status.classic));
	pan_setvar(wii_panel, 1, 71, &(buffer->status.guitar));
	pan_setvar(wii_panel, 1, 72, &(buffer->status.balanceboard));
	pan_setvar(wii_panel, 1, 73, &(buffer->status.battery));
}

/* assign to event.on_b function pointer
will record nunchuk and wiimote motion sensors as long as B button is pressed 
log file: 'wii_list.log'
*/
void wiimote_print(var handle, WIIMOTE* buffer)
{
	var listhndl_n;
	var tmp_fps;

	listhndl_n = file_open_append ("wii_list.log");
	tmp_fps = fps_max;
	fps_max = 30;
	wait (1);

	/* write timestamp */
	file_str_write (listhndl_n, "
----- ");
	if (sys_month < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_month);
	file_str_write (listhndl_n, "/");
	if (sys_day < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_day);
	file_str_write (listhndl_n, "/");
	if (sys_year < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_year);
	file_str_write (listhndl_n, "- ");
	if (sys_hours < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_hours);
	file_str_write (listhndl_n, ":");
	if (sys_minutes < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_minutes);
	file_str_write (listhndl_n, ":");
	if (sys_seconds < 10)
		file_str_write (listhndl_n, "0");	
	file_var_write (listhndl_n, sys_seconds);
	file_str_write (listhndl_n, "-----
");

	while (buffer->buttons.butB)
	{
		file_str_write (listhndl_n, "accel1 x");
		file_var_write (listhndl_n, buffer->accel1.x);
		file_str_write (listhndl_n, "	y");
		file_var_write (listhndl_n, buffer->accel1.y);
		file_str_write (listhndl_n, "	z");
		file_var_write (listhndl_n, buffer->accel1.z);

		file_str_write (listhndl_n, "	accel2 x");
		file_var_write (listhndl_n, buffer->accel2.x);
		file_str_write (listhndl_n, "	y");
		file_var_write (listhndl_n, buffer->accel2.y);
		file_str_write (listhndl_n, "	z");
		file_var_write (listhndl_n, buffer->accel2.z);

    	file_str_write (listhndl_n, "
");
		wait(1);
	}
	fps_max = tmp_fps;
	file_close (listhndl_n);
}

/* debugger startup function */
void wiimote_debug(WIIMOTE* buffer)
{
	wiimote_panel(buffer);
	buffer->event.on_b = wiimote_print;
	wiimote_move_irdot(buffer);
	wiimote_on();
}

/* move IR dot if IR is active */
void wiimote_move_irdot(WIIMOTE* buffer)
{
	proc_kill(4); /* make sure only one instance of function is running */
	while(1)
	{
		if (buffer->status.ir)
		{
			ir_panel->pos_x = buffer->pointer.ir_x;
			ir_panel->pos_y = buffer->pointer.ir_y;
			ir_panel->flags |= VISIBLE;
		}
		else
		{
			ir_panel->flags &= ~VISIBLE;
		}
		wait (1);
	}
}

/* toggle visibility of debug panel using F12 */
void wiimote_on()
{
	wii_panel->flags |= VISIBLE;
	on_f12 = wiimote_off;
}

void wiimote_off()
{
	on_f12 = wiimote_on;
	wii_panel->flags &= ~VISIBLE;
}