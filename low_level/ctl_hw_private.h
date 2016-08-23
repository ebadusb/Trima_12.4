/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/rcs/ctl_hw_private.h 1.1 2003/06/19 19:47:27Z jl11312 Exp jl11312 $
 * $Log: ctl_hw_private.h $
 * Revision 1.1  2003/06/19 19:47:27Z  jl11312
 * Initial revision
 *
 * Defines control specific hardware driver functions which are available
 * only to the ctl_drv task.
 *
 */

#ifndef _CTL_HW_PRIVATE_INCLUDE
#define _CTL_HW_PRIVATE_INCLUDE

#include "common_hw_private.h"

// Bitmasks for AC detector status conditions
typedef enum
{
   chw_acDetectorFluid = 0x01
} CtlHWAcDetectorStatus;

// Bitmasks for A/D converter status conditions
typedef enum
{
   chw_adcBusy = 0x01
} CtlHWADCStatus;

// Commands for door lock
typedef enum
{
   chw_doorLock,
   chw_doorUnlock,
   chw_doorLockOff
} CtlHWDoorLockCommand;

// Bitmasks for door lock status conditions
typedef enum
{
   chw_doorLockFault = 0x01
} CtlHWDoorLockStatus;

// Bitmasks for fan status conditions
typedef enum
{
   chw_fan0Fault = 0x01,
   chw_fan1Fault = 0x02,
   chw_fan2Fault = 0x04
} CtlHWFanStatus;

// Leak detector commands
typedef enum
{
   chw_leakDetectorOn,
   chw_leakDetectorOff
} CHWLeakDetectorCommand;

// Bitmasks for pump status conditions
typedef enum
{
   chw_pumpFault = 0x01
} CtlHWPumpStatus;

// List of ultrasonic sensors controlled by hardware driver
typedef enum
{
   chw_lowerUltrasonicSensor,
   chw_upperUltrasonicSensor
} CtlHWUltrasonicSensor;

// Commmands for valves
typedef enum
{
   chw_stopValve,
   chw_cwValve,
   chw_ccwValve
} CtlHWValveCommand;

// Bitmasks for valve status conditions
typedef enum
{
   chw_valveFault = 0x01
} CtlHWValveStatus;


#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */


//// AC DETECTOR RELATED FUNCTIONS ////

unsigned short chw_acDetectorGetStatus (void);


//// A/D CONVERTER RELATED FUNCTIONS ////

void           chw_adcStart (void);
unsigned short chw_adcGetStatus (void);
unsigned short chw_adcGetValue (void);
void           chw_adcSetMux (unsigned short mux);


//// CENTIFUGE RELATED FUNCTIONS ////

void chw_centSetCommand (unsigned short command);
void chw_centClearCommutation (void);
void chw_centDisable (void);
void chw_centEnable (void);

//// DOOR/DOOR LOCK RELATED FUNCTIONS ////

unsigned short chw_doorLockGetStatus (void);
void           chw_doorLockSetCommand (CtlHWDoorLockCommand command);


//// FAN RELATED FUNCTIONS ////

unsigned short chw_fanGetStatus (void);


//// LEAK DETECTOR RELATED FUNCTIONS ////

void chw_leakDetectorSetCommand (CHWLeakDetectorCommand command);


//// PUMP RELATED FUNCTIONS ////

unsigned short chw_pumpGetStatus (HWPump select);
unsigned short chw_pumpGetEncoder (HWPump select);
void           chw_pumpSetCommand (HWPump select, short command);
void           chw_pumpEnable (void);
void           chw_pumpDisable (void);


//// SOUND RELATED FUNCTIONS ////

void           chw_soundSetLevel (unsigned short level);
unsigned short chw_soundGetLevel (void);


//// ULTRASONIC SENSOR RELATED FUNCTIONS ////

unsigned short chw_ultrasonicSensorGetCounter (CtlHWUltrasonicSensor select);


//// VALVE RELATED FUNCTIONS ////

unsigned short chw_valveGetStatus (HWValve select);
HWValve        chw_valveGetActive (void);
void           chw_valveSetCommand (HWValve select, CtlHWValveCommand command);


#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _CTL_HW_PRIVATE_INCLUDE */

/* FORMAT HASH 70ff679ebd612a0555e4182842d38ba9 */
