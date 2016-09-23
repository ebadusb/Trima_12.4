/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log$
 *
 * Defines hardware driver functions common to control and safety which are
 * available only to the ctl_drv  and safe_drv tasks.
 *
 */

#ifndef _COMMON_HW_PRIVATE_INCLUDE
#define _COMMON_HW_PRIVATE_INCLUDE

#include "common_hw.h"

// List of alarm light states used by the hardware driver
typedef enum
{
   hw_alarmOn,
   hw_alarmOff
} HWAlarmCommand;

// Bitmasks for centrifuge status conditions
typedef enum
{
   hw_centFault   = 0x01,
   hw_centReverse = 0x02
} HWCentStatus;

// Bitmasks for door status conditions
typedef enum
{
   hw_doorClosed = 0x01,
   hw_doorLocked = 0x02
} HWDoorStatus;

// Bitmasks for power fail status conditions
typedef enum
{
   hw_powerFailDC       = 0x01,
   hw_powerFailWarning  = 0x02,
   hw_powerFailOverTemp = 0x04,
   hw_powerFailLineDrop = 0x08
} HWPowerFailStatus;

// List of pumps controlled by the HW driver
typedef enum
{
   hw_acPump,
   hw_inletPump,
   hw_plasmaPump,
   hw_plateletPump,
   hw_returnPump,
   hw_allPumps
} HWPump;

// List of valves controlled by the HW driver
typedef enum
{
   hw_rbcValve,
   hw_plasmaValve,
   hw_plateletValve,
   hw_cassette,
   hw_noValve,
   hw_allValves
} HWValve;

// List of valve states used by the HW driver
typedef enum
{
   hw_valveOpen,
   hw_valveCollect,
   hw_valveReturn,
   hw_valveUnknown,
   hw_valveIllegal
} HWValvePosition;

// Number of commutator pulses per centifuge revolution
static const unsigned short centCountsPerRev = 12;

// Get wrap limit of commutation pulse counter
static const unsigned short centCountLimit = 256;


#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */


//// ERROR HANDLING RELATED FUNCTIONS ////

// Prototype for readback failed function
typedef void hw_readbackFailedFunc (
   const char* file,                    // source file name where error was detected
   int line,                            // source file line where error was detected
   const char* type,                    // type of readback (e.g. "byte", "word")
   unsigned long port,                  // I/O port address
   unsigned long value,                 // value read back from I/O port
   unsigned long expected               // value expected to be read back
   );

// Set function to be called on an I/O port readback failure
//
void hw_setReadbackFailedFunc (hw_readbackFailedFunc* func    // function to be called for readback failures
                               );


//// ALARM LIGHT RELATED FUNCTIONS ////

// Set alarm light state
//
void hw_alarmSetCommand (HWAlarmCommand command          // new state for alarm light
                         );


//// CENTIFUGE RELATED FUNCTIONS ////

//	Get current pulse count from centrifuge motor commutators
//
unsigned short hw_centGetCommutationCount (void);


// Get bitwise-or of conditions defined in HWCentStatus
//
unsigned short hw_centGetStatus (void);

//// DOOR RELATED FUNCTIONS ////

// Get bitwise-or of conditions defined in HWDoorStatus
//
unsigned short hw_doorGetStatus (void);

//// POWER FAIL RELATED FUNCTIONS ////

// Enable power fail ride-thru (available only if supported by FPGA)
//
void hw_powerFailEnableRideThru (void);

// Disable power fail ride-thru
//
void hw_powerFailDisableRideThru (void);

// Get bitwise-or of conditions defined in HWPowerFailStatus
//
unsigned short hw_powerFailGetStatus (void);

// Should be called when a loss of AC power is first detected.  The function
// will set commands to conserve power (to extend the duration of AC power loss
// the machine can tolerate without shutting down).
//
void hw_powerFailSave (void);

// Should be called when AC power is restored.  The function will restore the
// commands modified by hw_powerFailSave()
//
void hw_powerFailRestore (void);


//// VALVE RELATED FUNCTIONS ////

// Get current position of the selected valve
//
HWValvePosition hw_valveGetPosition (HWValve valve           // valve for which position is to be returned
                                     );


//// WATCHDOG RELATED FUNCTIONS ////

// Update watchdog registers to avoid a forced-reset.  Should be called at approximately
// 50 msec intervals.
//
void hw_watchdogUpdate (void);

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _COMMON_HW_PRIVATE_INCLUDE */

/* FORMAT HASH bb98976a61612149cba47098d975bde9 */
