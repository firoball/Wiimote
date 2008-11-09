/* Debug script for using with Wiimote
   for short descriptions see comments in ackwii.h
   10/25/08 - Firoball
*/

/////////////////////////////////////////////////////////////////////////


/*
The following sample script reads the button and sensor states of Wiimote
and expansion device
define WII_DEBUG to enable debug mode
start/stop by pressing F12
for recording values keep B button of Wiimote pressed

IMPORTANT NOTE: never include this file directly!
include 'ackwii.h' and define 'WII_DEBUG' in your main script instead!
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


	digits(0,	80,		"Nunchuk", wiifont, 1, 1);
	digits(0,	95,		"X %-+5.0f", wiifont, 1, 1);
	digits(60,	95,		"Y %-+5.0f", wiifont, 1, 1);
	digits(120,	95,		"Z %-+5.0f", wiifont, 1, 1);

	digits(0,	110,		"P %-+5.0f", wiifont, 1, 1);
	digits(60,	110,		"T %-+5.0f", wiifont, 1, 1);
	digits(120,	110,		"R %-+5.0f", wiifont, 1, 1);


	digits(0,	130,		"Analog sticks", wiifont, 1, 1);
	digits(0,	145,		"X1 %-+3.0f", wiifont, 1, 1);
	digits(50,	145,		"Y1 %-+3.0f", wiifont, 1, 1);
	digits(100,	145,		"X2 %-+3.0f", wiifont, 1, 1);
	digits(150,	145,		"Y2 %-+3.0f", wiifont, 1, 1);


	digits(0,	165,		"Direction pad", wiifont, 1, 1);
	digits(0,	180,		"UP %1.0f", wiifont, 1, 1);
	digits(40,	180,		"DN %1.0f", wiifont, 1, 1);
	digits(80,	180,		"LT %1.0f", wiifont, 1, 1);
	digits(120,	180,		"RT %1.0f", wiifont, 1, 1);


	digits(0,	200,		"Shoulder buttons", wiifont, 1, 1);
	digits(0,	215,		"LT %-3.0f", wiifont, 1, 1);
	digits(40,	215,		"RT %-3.0f", wiifont, 1, 1);


	digits(0,	235,		"Button states", wiifont, 1, 1);
	digits(0,	250,		"A  %1.0f", wiifont, 1, 1);
	digits(40,	250,		"B  %1.0f", wiifont, 1, 1);
	digits(80,	250,		"1  %1.0f", wiifont, 1, 1);
	digits(120,	250,		"2  %1.0f", wiifont, 1, 1);

	digits(0,	265,		"X  %1.0f", wiifont, 1, 1);
	digits(40,	265,		"Y  %1.0f", wiifont, 1, 1);
	digits(80,	265,		"C  %1.0f", wiifont, 1, 1);
	digits(120,	265,		"Z  %1.0f", wiifont, 1, 1);

	digits(0,	280,		"L  %1.0f", wiifont, 1, 1);
	digits(40,	280,		"R  %1.0f", wiifont, 1, 1);
	digits(80,	280,		"ZL %1.0f", wiifont, 1, 1);
	digits(120,	280,		"ZR %1.0f", wiifont, 1, 1);

	digits(0,	295,		"Gr %1.0f", wiifont, 1, 1);
	digits(40,	295,		"Re %1.0f", wiifont, 1, 1);
	digits(80,	295,		"Ye %1.0f", wiifont, 1, 1);
	digits(120,	295,		"Bl %1.0f", wiifont, 1, 1);
	digits(160,	295,		"Or %1.0f", wiifont, 1, 1);

	digits(0,	310,		"-  %1.0f", wiifont, 1, 1);
	digits(40,	310,		"HM %1.0f", wiifont, 1, 1);
	digits(80,	310,		"+  %1.0f", wiifont, 1, 1);


	digits(0,	330,		"Balance Board", wiifont, 1, 1);
	digits(0,	345,		"TR %3.0f", wiifont, 1, 1);
	digits(40,	345,		"BR %3.0f", wiifont, 1, 1);
	digits(80,	345,		"TL %3.0f", wiifont, 1, 1);
	digits(120,	345,		"BL %3.0f", wiifont, 1, 1);
	digits(160,	345,		"W  %3.0f", wiifont, 1, 1);


	digits(0,	365,		"Devices", wiifont, 1, 1);
	digits(0,	380,		"IR %1.0f", wiifont, 1, 1);
	digits(40,	380,		"N  %1.0f", wiifont, 1, 1);
	digits(80,	380,		"CC %1.0f", wiifont, 1, 1);
	digits(120,	380,		"G  %1.0f", wiifont, 1, 1);
	digits(160,	380,		"BB %1.0f", wiifont, 1, 1);
	digits(200,	380,		"BAT %3.0fprc", wiifont, 1, 1);

	digits(0,	410,		"F12: toggle display - Keep B pressed on Wiimote to rec. motion sensors", wiifont, 1, 1);

	flags	|= OVERLAY;
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
	}

	pan_setvar(wii_panel, 1, 17, &(buffer->accel2.x));
	pan_setvar(wii_panel, 1, 18, &(buffer->accel2.y));
	pan_setvar(wii_panel, 1, 19, &(buffer->accel2.z));

	pan_setvar(wii_panel, 1, 20, &(buffer->angle2.pan));
	pan_setvar(wii_panel, 1, 21, &(buffer->angle2.tilt));
	pan_setvar(wii_panel, 1, 22, &(buffer->angle2.roll));

	pan_setvar(wii_panel, 1, 24, &(buffer->stick1.joy_x));
	pan_setvar(wii_panel, 1, 25, &(buffer->stick1.joy_y));
	pan_setvar(wii_panel, 1, 26, &(buffer->stick2.joy_x));
	pan_setvar(wii_panel, 1, 27, &(buffer->stick2.joy_y));

	pan_setvar(wii_panel, 1, 29, &(buffer->dpad.up));
	pan_setvar(wii_panel, 1, 30, &(buffer->dpad.down));
	pan_setvar(wii_panel, 1, 31, &(buffer->dpad.left));
	pan_setvar(wii_panel, 1, 32, &(buffer->dpad.right));

	pan_setvar(wii_panel, 1, 34, &(buffer->shoulder.l));
	pan_setvar(wii_panel, 1, 35, &(buffer->shoulder.r));
	
	pan_setvar(wii_panel, 1, 37, &(buffer->buttons.butA));
	pan_setvar(wii_panel, 1, 38, &(buffer->buttons.butB));
	pan_setvar(wii_panel, 1, 39, &(buffer->buttons.but1));
	pan_setvar(wii_panel, 1, 40, &(buffer->buttons.but2));

	pan_setvar(wii_panel, 1, 41, &(buffer->buttons.butX));
	pan_setvar(wii_panel, 1, 42, &(buffer->buttons.butY));
	pan_setvar(wii_panel, 1, 43, &(buffer->buttons.butC));
	pan_setvar(wii_panel, 1, 44, &(buffer->buttons.butZ));

	pan_setvar(wii_panel, 1, 45, &(buffer->buttons.butL));
	pan_setvar(wii_panel, 1, 46, &(buffer->buttons.butR));
	pan_setvar(wii_panel, 1, 47, &(buffer->buttons.butZL));
	pan_setvar(wii_panel, 1, 48, &(buffer->buttons.butZR));

	pan_setvar(wii_panel, 1, 49, &(buffer->buttons.butGreen));
	pan_setvar(wii_panel, 1, 50, &(buffer->buttons.butRed));
	pan_setvar(wii_panel, 1, 51, &(buffer->buttons.butYellow));
	pan_setvar(wii_panel, 1, 52, &(buffer->buttons.butBlue));
	pan_setvar(wii_panel, 1, 53, &(buffer->buttons.butOrange));

	pan_setvar(wii_panel, 1, 54, &(buffer->buttons.butMinus));
	pan_setvar(wii_panel, 1, 55, &(buffer->buttons.butHome));
	pan_setvar(wii_panel, 1, 56, &(buffer->buttons.butPlus));

	pan_setvar(wii_panel, 1, 58, &(buffer->board.topright));
	pan_setvar(wii_panel, 1, 59, &(buffer->board.botright));
	pan_setvar(wii_panel, 1, 60, &(buffer->board.topleft));
	pan_setvar(wii_panel, 1, 61, &(buffer->board.botleft));
	pan_setvar(wii_panel, 1, 62, &(buffer->board.weight));

	pan_setvar(wii_panel, 1, 64, &(buffer->status.ir));
	pan_setvar(wii_panel, 1, 65, &(buffer->status.nunchuk));
	pan_setvar(wii_panel, 1, 66, &(buffer->status.classic));
	pan_setvar(wii_panel, 1, 67, &(buffer->status.guitar));
	pan_setvar(wii_panel, 1, 68, &(buffer->status.balanceboard));
	pan_setvar(wii_panel, 1, 69, &(buffer->status.battery));
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
void wiimote_debug(WIIMOTE * buffer)
{
	wiimote_panel(buffer);
	buffer->event.on_b = wiimote_print;
	wiimote_on();
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