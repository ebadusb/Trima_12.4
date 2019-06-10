/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      shw.hpp, Focussed System safety hardware
 *             structure defines
 *
 * ABSTRACT:   Defines strucures used with safety hardware driver
 *
 */

#ifndef SHW_HPP
#define SHW_HPP

#include "hw.h"


enum SHW_RESERVOIR_STATES
{
   SHW_RESERVOIR_EMPTY = 1,
   SHW_RESERVOIR_LOW   = 2
};

enum SHW_EVENTS
{
   SHW_NORMAL_UPDATE             = 0,
   SHW_PAUSE_EVENT               =1,
   SHW_STOP_EVENT                =2,
   SHW_DOOR_EVENT                =3,
   SHW_RESERVOIR_EVENT           =4,
   SHW_RETURN_EVENT              =5,
   SHW_AIR_EVENT                 =6,
   SHW_RETURN_PUMP_TOO_FAST_EVENT=7,
   SHW_AC_PUMP_TOO_FAST_EVENT    =8,
   SHW_SERVICE_MODE_VIOLATION    =9,
   SHW_AIR_WAIT_EVENT            =10,
   SHW_AIR_WAIT_RESTART_EVENT    =11,
   SHW_AIR_WAIT_RESTART_EVENT2   =12,
   SHW_AIR_EVENT_AFTER_RESTART   =13,
};

enum SHW_DIRECTION
{
   SHW_RETURN = 0,
   SHW_DRAW   = 1
};

// SPECIFICATION:    orders used with
//                   focusBufferMsg<SafetyHardwareCommands>
//
// ERROR HANDLING:   none.

typedef struct
{
   HW_ORDERS pumpPower;                  // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS centrifugePower;            // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS valveLED;                   // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS doorSolenoidPower;          // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS alarmLight;                 // HW_DISABLE=0, HW_ENABLE

// allow service mode and the safe executive to control air-to-donor
   HW_ORDERS serviceEnableMode;          // HW_DISABLE=!servicemode,
                                         // HW_ENABLE=servicemode
   HW_ORDERS donorConnectMode;           // HW_DISABLE=!donorconnect,
                                         // HW_ENABLE=donorconnect


// test modes
   long          test1;
   unsigned long sequenceNumber;
   
   bool          donorDisconnectState;

} SHwOrders;


// SPECIFICATION:    status used with
//                   focusBufferMsg<SafetyHardwareStatus>
//
// ERROR HANDLING:   none.

typedef struct
{

// pumps
   long          inletRPM;                   // 0 to ~250 rpm
   long          inletRevs;                  // delta counts
   long          inletAccumRevs;             // total counts

   long          plateletRPM;                // 0 to ~250 rpm
   long          plateletRevs;               // delta counts
   long          plateletAccumRevs;          // total counts

   long          plasmaRPM;                  // 0 to ~250 rpm
   long          plasmaRevs;                 // delta counts
   long          plasmaAccumRevs;            // total counts

   long          acRPM;                      // 0 to ~250 rpm
   long          acRevs;                     // delta counts
   long          acAccumRevs;                // total counts

   long          returnRPM;                  // ~-250 to ~250 rpm
   long          returnRevs;                 // delta counts
   long          returnAccumRevs;            // total counts
   SHW_DIRECTION returnDirection;            // SHW_RETURN, SHW_DRAW

// valve status    0 = HW_VALVE_UNKNOWN
//                 1 = HW_VALVE_COLLECT
//                 2 = HW_VALVE_OPEN
//                 3 = HW_VALVE_RETURN

   HW_VALVE_STATES rbcValve;
   HW_VALVE_STATES plasmaValve;
   HW_VALVE_STATES plateletValve;

   unsigned char   valveLedTest;             // 1=ok, all others =error

// cassette position
   HW_CASSETTE_STATES cassettePosition;      // HW_CASSETTE_UNKNOWN, HW_CASSETTE_DOWN,
                                             // HW_CASSETTE_UP

// ultrasonics
   SHW_RESERVOIR_STATES reservoir;           // 1=SHW_RESERVOIR_EMPTY, SHW_RESERVOIR_LOW

// centrifuge
   float          centrifugeRPM;             // RPM
   unsigned short centrifugeError;           // HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR=1,
                                             // HW_CENT_INTEGRAL_ERROR=2

// misc
   HW_DOOR_STATES doorLocks;                 // HW_DOOR_OPEN, HW_DOOR_LOCKED_STATE,
                                             // HW_DOOR_CLOSED,
                                             // HW_DOOR_CLOSED_AND_LOCKED

   HW_SWITCH_STATES pauseSwitch;             // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED
   HW_SWITCH_STATES stopSwitch;              // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED

   SHW_EVENTS       event;                   // SHW_NORMAL_UPDATE, SHW_PAUSE_EVENT,
                                             // SHW_STOP_EVENT, SHW_DOOR_EVENT,
                                             // SHW_RESERVOIR_EVENT, SHW_RETURN_EVENT,
                                             // SHW_AIR_EVENT

   long           msgCount;                  // accum message counter

   unsigned short returnPumpDirChgTime;      // time from low level sensor
                                             // indicating air to return pump
                                             // in draw direction, milliseconds

// test modes
   long      test1;
   short     test2;

   HW_ORDERS serviceEnableMode;              // HW_DISABLE=!servicemode,
                                             // HW_ENABLE=servicemode
   HW_ORDERS donorConnectMode;               // HW_DISABLE=!donorconnect,
                                             // HW_ENABLE=donorconnect
} SHwStates;
#endif

/* FORMAT HASH b2c8445c4293df9f329f94695acc8d5f */
