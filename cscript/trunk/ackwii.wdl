/* Debug script for using with Wiimote
   for short descriptions see comments in ackwii_inc.wdl
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
include 'ackwii_inc.wdl' and define 'WII_DEBUG' in your main script instead!
*/

	
//////////Debug helpers

FONT wiifont = "ackfont.pcx", 6, 9;
var wii_buf[wiimote_buffer_size];

/* panel to show values retrieved from wiimote and expansion device */
PANEL wii_panel = 
{
	pos_x	= 0;
	pos_y	= 130;

	digits(0,	0,		"Wiimote", wiifont, 1, 1);
	digits(0,	15,		"X %-+5.0f", wiifont, 1, wii_buf[accel1_x]);
	digits(60,	15,		"Y %-+5.0f", wiifont, 1, wii_buf[accel1_y]);
	digits(120,	15,		"Z %-+5.0f", wiifont, 1, wii_buf[accel1_z]);

	digits(0,	30,		"P %-+5.0f", wiifont, 1, wii_buf[angle1_pan]);
	digits(60,	30,		"T %-+5.0f", wiifont, 1, wii_buf[angle1_tilt]);
	digits(120,	30,		"R %-+5.0f", wiifont, 1, wii_buf[angle1_roll]);

	digits(0,	45,		"X1 %-+4.0f", wiifont, 1, wii_buf[ir1_x]);
	digits(50,	45,		"X2 %-+4.0f", wiifont, 1, wii_buf[ir1_y]);
	digits(100,	45,		"X3 %-+4.0f", wiifont, 1, wii_buf[ir2_x]);
	digits(150,	45,		"X4 %-+4.0f", wiifont, 1, wii_buf[ir2_y]);
	digits(0,	60,		"Y1 %-+4.0f", wiifont, 1, wii_buf[ir3_x]);
	digits(50,	60,		"Y2 %-+4.0f", wiifont, 1, wii_buf[ir3_y]);
	digits(100,	60,		"Y3 %-+4.0f", wiifont, 1, wii_buf[ir4_x]);
	digits(150,	60,		"Y4 %-+4.0f", wiifont, 1, wii_buf[ir4_y]);


	digits(0,	80,		"Nunchuk", wiifont, 1, 1);
	digits(0,	95,		"X %-+5.0f", wiifont, 1, wii_buf[accel2_x]);
	digits(60,	95,		"Y %-+5.0f", wiifont, 1, wii_buf[accel2_y]);
	digits(120,	95,		"Z %-+5.0f", wiifont, 1, wii_buf[accel2_z]);

	digits(0,	110,		"P %-+5.0f", wiifont, 1, wii_buf[angle2_pan]);
	digits(60,	110,		"T %-+5.0f", wiifont, 1, wii_buf[angle2_tilt]);
	digits(120,	110,		"R %-+5.0f", wiifont, 1, wii_buf[angle2_roll]);


	digits(0,	130,		"Analog sticks", wiifont, 1, 1);
	digits(0,	145,		"X1 %-+3.0f", wiifont, 1, wii_buf[joy1_x]);
	digits(50,	145,		"Y1 %-+3.0f", wiifont, 1, wii_buf[joy1_y]);
	digits(100,	145,		"X2 %-+3.0f", wiifont, 1, wii_buf[joy2_x]);
	digits(150,	145,		"Y2 %-+3.0f", wiifont, 1, wii_buf[joy2_y]);


	digits(0,	165,		"Direction pad", wiifont, 1, 1);
	digits(0,	180,		"UP %1.0f", wiifont, 1, wii_buf[up]);
	digits(40,	180,		"DN %1.0f", wiifont, 1, wii_buf[down]);
	digits(80,	180,		"LT %1.0f", wiifont, 1, wii_buf[left]);
	digits(120,	180,		"RT %1.0f", wiifont, 1, wii_buf[right]);


	digits(0,	200,		"Shoulder buttons", wiifont, 1, 1);
	digits(0,	215,		"LT %-3.0f", wiifont, 1, wii_buf[sl]);
	digits(40,	215,		"RT %-3.0f", wiifont, 1, wii_buf[sr]);


	digits(0,	235,		"Button states", wiifont, 1, 1);
	digits(0,	250,		"A  %1.0f", wiifont, 1, wii_buf[butA]);
	digits(40,	250,		"B  %1.0f", wiifont, 1, wii_buf[butB]);
	digits(80,	250,		"1  %1.0f", wiifont, 1, wii_buf[but1]);
	digits(120,	250,		"2  %1.0f", wiifont, 1, wii_buf[but2]);

	digits(0,	265,		"X  %1.0f", wiifont, 1, wii_buf[butX]);
	digits(40,	265,		"Y  %1.0f", wiifont, 1, wii_buf[butY]);
	digits(80,	265,		"C  %1.0f", wiifont, 1, wii_buf[butC]);
	digits(120,	265,		"Z  %1.0f", wiifont, 1, wii_buf[butZ]);

	digits(0,	280,		"L  %1.0f", wiifont, 1, wii_buf[butL]);
	digits(40,	280,		"R  %1.0f", wiifont, 1, wii_buf[butR]);
	digits(80,	280,		"ZL %1.0f", wiifont, 1, wii_buf[butZL]);
	digits(120,	280,		"ZR %1.0f", wiifont, 1, wii_buf[butZR]);

	digits(0,	295,		"Gr %1.0f", wiifont, 1, wii_buf[butGreen]);
	digits(40,	295,		"Re %1.0f", wiifont, 1, wii_buf[butRed]);
	digits(80,	295,		"Ye %1.0f", wiifont, 1, wii_buf[butYellow]);
	digits(120,	295,		"Bl %1.0f", wiifont, 1, wii_buf[butBlue]);
	digits(160,	295,		"Or %1.0f", wiifont, 1, wii_buf[butOrange]);

	digits(0,	310,		"-  %1.0f", wiifont, 1, wii_buf[butMinus]);
	digits(40,	310,		"HM %1.0f", wiifont, 1, wii_buf[butHome]);
	digits(80,	310,		"+  %1.0f", wiifont, 1, wii_buf[butPlus]);


	digits(0,	330,		"Balance Board", wiifont, 1, 1);
	digits(0,	345,		"TR %3.0f", wiifont, 1, wii_buf[topright]);
	digits(40,	345,		"BR %3.0f", wiifont, 1, wii_buf[botright]);
	digits(80,	345,		"TL %3.0f", wiifont, 1, wii_buf[topleft]);
	digits(120,	345,		"BL %3.0f", wiifont, 1, wii_buf[botleft]);
	digits(160,	345,		"W  %3.0f", wiifont, 1, wii_buf[weight]);


	digits(0,	365,		"Devices", wiifont, 1, 1);
	digits(0,	380,		"IR %1.0f", wiifont, 1, wii_buf[ir]);
	digits(40,	380,		"N  %1.0f", wiifont, 1, wii_buf[nunchuk]);
	digits(80,	380,		"CC %1.0f", wiifont, 1, wii_buf[classic]);
	digits(120,	380,		"G  %1.0f", wiifont, 1, wii_buf[guitar]);
	digits(160,	380,		"BB %1.0f", wiifont, 1, wii_buf[balanceboard]);
	digits(200,	380,		"BAT %3.0fprc", wiifont, 1, wii_buf[battery]);

	digits(0,	410,		"F12: toggle display - Keep B pressed on Wiimote to rec. motion sensors", wiifont, 1, 1);

	flags	= OVERLAY;
}

/* copy a buffer to the debug panel buffer */
function wiimote_panel(&buffer)
{
	var i;

	proc_kill(4); /* make sure only one instance of function is running */

	/* copy current Wiimote buffer to debug panel buffer */
	while(1)
	{
		i = 0;
		while(i < wiimote_buffer_size)
		{
			wii_buf[i] = buffer[i];
			i += 1;
		}
		wait (1);
	}
}

/* assign to buffer[on_bb] function pointer
will record nunchuk and wiimote motion sensors as long as B button is pressed 
log file: 'wii_list.log'
*/
function wiimote_print(handle, &buffer)
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
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_month);
	file_str_write (listhndl_n, "/");
	if (sys_day < 10)
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_day);
	file_str_write (listhndl_n, "/");
	if (sys_year < 10)
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_year);
	file_str_write (listhndl_n, "- ");
	if (sys_hours < 10)
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_hours);
	file_str_write (listhndl_n, ":");
	if (sys_minutes < 10)
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_minutes);
	file_str_write (listhndl_n, ":");
	if (sys_seconds < 10)
	{
		file_str_write (listhndl_n, "0");	
	}
	file_var_write (listhndl_n, sys_seconds);
	file_str_write (listhndl_n, "-----
");

	while (buffer[butB])
	{
		file_str_write (listhndl_n, "accel1 x");
		file_var_write (listhndl_n, buffer[accel1_x]);
		file_str_write (listhndl_n, "	y");
		file_var_write (listhndl_n, buffer[accel1_y]);
		file_str_write (listhndl_n, "	z");
		file_var_write (listhndl_n, buffer[accel1_z]);

		file_str_write (listhndl_n, "	accel2 x");
		file_var_write (listhndl_n, buffer[accel2_x]);
		file_str_write (listhndl_n, "	y");
		file_var_write (listhndl_n, buffer[accel2_y]);
		file_str_write (listhndl_n, "	z");
		file_var_write (listhndl_n, buffer[accel2_z]);

    	file_str_write (listhndl_n, "
");
		wait(1);
	}
	fps_max = tmp_fps;
	file_close (listhndl_n);
}

/* debugger startup function */
function wiimote_debug(&buffer)
{
	wiimote_panel(buffer);	/* update debug panel each tick with given buffer */
	buffer[on_bb] = wiimote_print;
	wiimote_on();
}

/* toggle visibility of debug panel using F12 */
function wiimote_on()
{
	wii_panel.VISIBLE = ON;
	on_f12 = wiimote_off;
}

function wiimote_off()
{
	on_f12 = wiimote_on;
	wii_panel.VISIBLE = OFF;
}