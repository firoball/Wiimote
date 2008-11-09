/*
 *******************************************************************************
 * ackdll.h
 * Creation date: 2008-11-09
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
 * Interface module to A7 game engine
 * provide function calls for Lite-C/C-Script
 * Header file for internal pointer validation functions
 *
 *******************************************************************************
 */

#ifndef ACKDLL_H
#define ACKDLL_H

// standard includes
#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdio.h>

// engine specific header file
#define DLL_USE	// always define before including adll.h
#include "adll.h"	

// project includes
#include "ackwiimote.h"

// Definitions for pointer list
#define WII_HANDLES 20 // should do...

// Function prototypes
void wiimote__initList();
bool wiimote__checkHandle(ackWiiMote* handle);
void wiimote__addHandle(ackWiiMote* handle);
void wiimote__removeHandle(ackWiiMote* handle);

#endif
