/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * Defines safety specific hardware driver functions which are available
 * only to the safe_drv task.
 *
 */

#ifndef _SAFE_HW_PRIVATE_INCLUDE
#define _SAFE_HW_PRIVATE_INCLUDE

#include "common_hw_private.h"

// Data sources for getbyte class
typedef enum
{
   shw_basinDataSource
} SafeHWGetByteSource;

// Hall sensor status
typedef enum
{
   shw_hallActive = 0x01
} SafeHWHallStatus;

// Power switches
typedef enum
{
   shw_doorPower = 0x01,
   shw_ledPower  = 0x02,
   shw_centPower = 0x04,
   shw_pumpPower = 0x08,
   shw_allPower  = 0x0f
} SafeHWPower;

// Pump direction
typedef enum
{
   shw_pumpDraw,
   shw_pumpReturn
} SafeHWPumpDirection;


#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

unsigned short shw_getByteSourceData (SafeHWGetByteSource select);

void shw_centClearFault (void);

//// HALL SENSOR RELATED FUNCTIONS ////

unsigned short shw_hallGetStatus (HWPump select);


//// LED SENSOR RELATED FUNCTIONS ////

unsigned short shw_ledTest (unsigned short* rawStatus);


//// POWER RELATED FUNCTIONS ////

unsigned short shw_powerStatus (void);
void           shw_powerEnable (unsigned short mask);
void           shw_powerDisable (unsigned short mask);
void           shw_powerSet (unsigned short mask);


//// PUMP RELATED FUNCTIONS ////

unsigned short shw_pumpDirection (HWPump select);


//// ULTRASONIC SENSOR RELATED FUNCTIONS ////

unsigned short shw_ultrasonicSensorGetCounter (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */


#endif /* ifndef _SAFE_HW_PRIVATE_INCLUDE */

/* FORMAT HASH 9706bfdcf502111050d41a8fb056a8f0 */
