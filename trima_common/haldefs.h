/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/haldefs.h 1.4 2006/02/23 17:31:20Z rm70006 Exp dslausb $
 * $Log: haldefs.h $
 * Revision 1.4  2006/02/23 17:31:20Z  rm70006
 * IT 7237.  Improve logging.
 * Revision 1.3  2002/10/28 22:11:01Z  jl11312
 * - updated for vxWorks HAL implementation
 *
 */

#ifndef _HALDEFS_INCLUDE
#define _HALDEFS_INCLUDE

#include "datalog.h"


// fluid detector states
enum HAL_FLUIDDETECT
{
   HAL_ERROR_DETECTED,
   HAL_FLUID_DETECTED,
   HAL_AIR_DETECTED
};

// Possible events causing hardware update
enum HAL_EVENTS
{
   HAL_NORMAL_UPDATE   = 0,
   HAL_AC_EVENT        = 1,
   HAL_DOOR_EVENT      = 2,
   HAL_PAUSE_EVENT     = 3,
   HAL_STOP_EVENT      = 4,
   HAL_RESERVOIR_EVENT = 5
};

// State of our reservoir
enum HAL_RESERVOIRSTATE
{
   HAL_RESERVOIR_ERROR,         // error condition
   HAL_RESERVOIR_EMPTY,         // no fluid at either detector
   HAL_RESERVOIR_MIDDLE,        // fluid at low detector, no fluid high
   HAL_RESERVOIR_HIGH,          // fluid at the high detector
   HAL_RESERVOIR_UNKNOWN        // uninitialized state
};

// Valve Identification
enum HAL_VALVEID
{
   HAL_VRBC,
   HAL_VPLASMA,
   HAL_VCOLLECT
};

// Valve positions
enum HAL_VALVESTATE
{
   HAL_VALVE_UNKNOWN = 0,
   HAL_VALVE_COLLECT = 1,
   HAL_VALVE_OPEN    = 2,
   HAL_VALVE_RETURN  = 3
};

DataLog_Stream& operator << (DataLog_Stream& os, HAL_VALVESTATE valve);


enum HAL_CASSETTESTATE
{
   HAL_CASSETTE_UNKNOWN = 0,
   HAL_CASSETTE_DOWN    = 1,
   HAL_CASSETTE_UP      = 2
};

enum HAL_ERROR
{
   HAL_NO_FAULT = 0,
   HAL_FAULT    = 1
};

enum HAL_DOORSTATE
{
   HAL_DOOR_OPEN              = 0,
   HAL_DOOR_LOCKED_STATE      = 1,
   HAL_DOOR_CLOSED_STATE      = 2,
   HAL_DOOR_CLOSED_AND_LOCKED = 3
};

enum HAL_CASSETTEORDERS
{
   HAL_NOCHANGE,
   HAL_CASSETTEDOWN,
   HAL_CASSETTEUP
};

enum HAL_DOORORDERS
{
   HAL_DOORUNLOCK,
   HAL_DOORLOCK
};

enum HAL_ALARMLIGHTORDERS
{
   HAL_ALARMLIGHTOFF,
   HAL_ALARMLIGHTON
};

enum HAL_LEAKTESTORDERS
{
   HAL_LEAKTESTOFF,
   HAL_LEAKTESTON
};

enum HAL_PWR_SPLY_OUTPUT_STATUS
{
   HAL_PWR_SPLY_ENABLED,
   HAL_PWR_SPLY_DISABLED
};


#endif /* ifndef _HALDEFS_INCLUDE */

/* FORMAT HASH cccb9f43ab86e6513729c919cad4f94a */
