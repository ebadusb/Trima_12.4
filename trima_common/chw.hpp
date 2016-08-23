/*
 * Copyright (c) 1995, 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      chw.hpp, Focussed System control hardware
 *             structure defines
 *
 * ABSTRACT:   Defines strucures used with control hardware driver
 *
 */

#ifndef CHW_HPP
#define CHW_HPP

#include <time.h>

#include "auxclock.h"

#include "hw.h"


// typedefs

enum CHW_AC_DETECTOR_STATES
{
   CHW_AC_DETECTOR_FLUID = 1,
   CHW_AC_DETECTOR_AIR   = 2
};

enum CHW_CASSETTE_ORDERS
{
   CHW_CASSETTE_FIRST_ORDER = 0,
   CHW_CASSETTE_DONTMOVE    = CHW_CASSETTE_FIRST_ORDER,
   CHW_CASSETTE_DOWN        = HW_CASSETTE_DOWN,
   CHW_CASSETTE_UP          = HW_CASSETTE_UP,
   CHW_CASSETTE_LAST_ORDER  = CHW_CASSETTE_UP
};

enum CHW_DOOR_ORDERS
{
   CHW_DOOR_UNLOCK = 0,
   CHW_DOOR_LOCK   = 1
};

enum CHW_VALVE_ORDERS
{
   CHW_VALVE_FIRST   = HW_VALVE_COLLECT,
   CHW_VALVE_COLLECT = CHW_VALVE_FIRST,
   CHW_VALVE_OPEN    = HW_VALVE_OPEN,
   CHW_VALVE_RETURN  = HW_VALVE_RETURN,
   CHW_VALVE_LAST    = CHW_VALVE_RETURN
};

enum CHW_EVENTS
{
   CHW_NORMAL_UPDATE   = 0,
   CHW_AC_EVENT        = 1,
   CHW_DOOR_EVENT      = 2,
   CHW_PAUSE_EVENT     = 3,
   CHW_STOP_EVENT      = 4,
   CHW_RESERVOIR_EVENT = 5
};

enum CHW_RESERVOIR_STATES
{
   CHW_RESERVOIR_ERROR  =  0,
   CHW_RESERVOIR_EMPTY  =  1,
   CHW_RESERVOIR_MIDDLE = 2,
   CHW_RESERVOIR_HIGH   =   3
};

enum CHW_SOUND_LEVELS
{
   CHW_SOUND_LEVEL_FIRST = 0,
   CHW_SOUND_LOUD        = CHW_SOUND_LEVEL_FIRST,
   CHW_SOUND_MEDIUM_LOUD = 1,
   CHW_SOUND_MEDIUM_SOFT = 2,
   CHW_SOUND_SOFT        = 3,
   CHW_SOUND_LEVEL_LAST  = CHW_SOUND_SOFT
};

// bit masks for pump errors
enum CHW_PUMP_ERRORS
{
   CHW_PUMP_NO_FAULT               = HW_NO_FAULT,
   CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
   CHW_PUMP_INTEGRAL_ERROR         = 2,
   CHW_PUMP_VOLUME_ERROR           = 4,
   CHW_MOTOR_FAULT                 = 8
};

// bit mask for power supply overtemp errors
enum CHW_PS_OT_ERRORS
{
   CHW_PS_NO_FAULT = HW_NO_FAULT,
   CHW_DC_NOT_OK   = 1,                 // DC output is not OK
   CHW_POWER_FAIL  = 2,                 // Power Fail interrupt
   CHW_PS_OTW      = 4,                 // Power supply overtemp warning
   CHW_SS_OTW      = 8                  // Seal-safe overtemp warning
};

// SEAL SAFE OVER TEMP WARNING
enum CHW_SEAL_SAFE_OPERATION
{
   CHW_SEAL_SAFE_OPERATION_NOT_AVAILABLE=0,
   CHW_SEAL_SAFE_ON                     = 1,
   CHW_SEAL_SAFE_OFF                    = 2
};

// SEAL SAFE OVER TEMP WARNING
enum CHW_SEAL_SAFE_OVER_TEMP_WARN
{
   CHW_SEAL_SAFE_OTW_NOT_AVAILABLE = 0,
   CHW_SEAL_SAFE_OTW               = 1,
   CHW_SEAL_SAFE_TEMP_OK           = 2
};


// POWER SUPPLY OVER VOLTAGE PROTECT TEST RESULT
enum CHW_PS_OVP_TEST_RESULT
{
   CHW_OVP_TEST_RESULT_NOT_AVAILABLE = 0,
   CHW_OVP_TEST_RESULT_FAIL          = 1,
   CHW_OVP_TEST_RESULT_PASS          = 2,
};

enum CHW_PS_TEST_STATUS
{
   CHW_PS_TEST_STATUS_NOT_AVAILABLE = 0,
   CHW_PS_TEST_STATUS_OFF           = 1,
   CHW_PS_TEST_STATUS_ON            = 2
};


// SPECIFICATION:    orders used with
//                      focusBufferMsg<ControlHardwareCommands>
//
// ERROR HANDLING:   none.

typedef struct
{
   CHW_CASSETTE_ORDERS cassettePosition;     // CHW_CASSETTE_DOWN=1,
                                             // CHW_CASSETTE_UP=2

// centrifuge
   float centrifugeRPM;                      // 0 to 3100 RPM
                                             // -1 disables the motor controller
   float centrifugeRampUpRate;               // RPM / sec
   float centrifugeRampDownRate;             // RPM / sec

// misc
   HW_ORDERS       alarmLight;                   // HW_DISABLE, HW_ENABLE=light on
   HW_ORDERS       doorLatches;                  // HW_DISABLE, HW_ENABLE=power on
   CHW_DOOR_ORDERS doorDirection;                // CHW_DOOR_UNLOCK, CHW_DOOR_LOCK

// pump RPM orders
   float            inletRPM;                    // 0 - 250 RPM
   float            plateletRPM;                 // 0 - 250 RPM
   float            plasmaRPM;                   // 0 - 250 RPM
   float            acRPM;                       // 0 - 250 RPM
   float            returnRPM;                   // -250 to 250 RPM

   CHW_VALVE_ORDERS rbcValve;                    // CHW_VALVE_COLLECT=1,
                                                 // CHW_VALVE_OPEN=2,
                                                 // CHW_VALVE_RETURN=3
   CHW_VALVE_ORDERS plasmaValve;
   CHW_VALVE_ORDERS plateletValve;

// rbc values
   unsigned char redDrive;                       // 0 to 255, typically 64
   unsigned char greenDrive;                     // 0 to 255, typically 128
   unsigned long sequenceNumber;                 // Keep track of the number of times this is sent


// sound level added for 2.6 HW.  Not used in 2.4 & 2.2
   CHW_SOUND_LEVELS soundLevel;                  // CHW_SOUND_LOUD=0, CHW_SOUND_MEDIUM_LOUD,
                                                 // CHW_SOUND_MEDIUM_SOFT, CHW_SOUND_SOFT

// test modes
   long openLoop;                                // 1= open loop for pump and centrifuge control

// APS limits
   short               highAPSLimit;             // High access pressure limit
   short               lowAPSLimit;              // Low access pressure limit

   HW_PS_OUTPUT_STATUS powerSupplyEnabled;
   rawTime             timeOrdersCreated;
} CHwOrders;


// SPECIFICATION:    status structure for pumps
//
// ERROR HANDLING:   none.

typedef struct
{
   unsigned char errors;                     // CHW_PUMP_NO_FAULT,
                                             // CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
                                             // CHW_PUMP_INTEGRAL_ERROR = 2,
                                             // CHW_PUMP_VOLUME_ERROR = 4
// pump actual rpm
   float RPM;                                // ~-250 to ~250, for return pump
                                             // 0 to 250 for all other pumps

// pump revolutions
   float Revs;                               // Delta revs since last update

// pump accumulated revs, encoder ticks
   long Accum;                               // total revs, encoder ticks

// error terms
   long Integral;                            // snap-shot of the integration error
   long AcummIntegral;                       // cummulative integration error for
                                             // the course of the entire run
} PumpStatus;


// SPECIFICATION:    status used with
//                   focusBufferMsg<ControlHardwareStatus>
//
// ERROR HANDLING:   none.

typedef struct
{
// A/D channels, raw sensor readings, uncalibrated, 0-4095 counts
   short accessPressure;                     // access (inlet) pressure
   short emiTemperature;                     // temperature of EMI box
   short centrifugeCurrent;                  // centrifuge current
   short centrifugePressure;                 // centrifuge pressure sensor
   short fiveVoltSupply;                     // + 5 volt supply
   short leakDetector;                       // leak detector
   short minusTwelvePS;                      // -12 volt supply
   short sixtyFourVoltCurrent;               // +64 volt supply current
   short sixtyFourVoltSupply;                // +64 volt (centrifuge) before switch
   short sixtyFourVoltSwitched;              // +64 volt (centrifuge) after switch
   short twelveVoltSupply;                   // +12 volt supply
   short twentyFourVoltCurrent;              // +24 volt supply current
   short twentyFourVoltSupply;               // +24 volt before switch
   short twentyFourVoltSwitched;             // +24 volt after switch

// cassette position
   HW_CASSETTE_STATES cassettePosition;      // HW_CASSETTE_UNKNOWN,
                                             // HW_CASSETTE_DOWN,
                                             // HW_CASSETTE_UP
   HW_ERROR cassetteError;                   // HW_NO_FAULT, HW_FAULT

// centrifuge
   float          centrifugeRPM;             // 0 to ~3000 rpm
   unsigned short centrifugeError;           // HW_CENT_NO_FAULT,
                                             // HW_CENT_HARDWARE_ERROR=1,
                                             // HW_CENT_INTEGRAL_ERROR=2,
// Misc
   HW_DOOR_STATES doorLocks;                 // HW_DOOR_OPEN,
                                             // HW_DOOR_LOCKED_STATE,
                                             // HW_DOOR_CLOSED_STATE,
                                             // HW_DOOR_CLOSED_AND_LOCKED

   HW_SWITCH_STATES pauseSwitch;             // HW_SWITCH_DISABLED,
                                             // HW_SWITCH_ENABLED
   HW_SWITCH_STATES stopSwitch;
   HW_SWITCH_STATES pauseSwitchLatch;
   HW_SWITCH_STATES stopSwitchLatch;
   HW_ERROR         vibration;               // HW_NO_FAULT, HW_FAULT
   HW_ERROR         valveFault;              // HW_NO_FAULT, HW_FAULT
   HW_ERROR         motorFault;              // HW_NO_FAULT, HW_FAULT
   HW_ERROR         solenoidFault;           // HW_NO_FAULT, HW_FAULT

// rbc detector
   short         red;                        // raw red value, 0-~32000 counts
   short         green;                      // raw green value, 0-~32000 counts
   unsigned long newReflectanceCounts;       // Track new reflectance values
   unsigned long newDriveCounts;             // Track new drive values
   HW_ERROR      rbcCommError;               // HW_NO_FAULT, HW_FAULT

// pumps
   PumpStatus acPump;                        // AC
   PumpStatus inletPump;                     // Inlet
   PumpStatus plasmaPump;                    // Plasma
   PumpStatus plateletPump;                  // Platelet
   PumpStatus returnPump;                    // Return

// ultrasonics
   CHW_AC_DETECTOR_STATES acDetector;        // CHW_AC_DETECTOR_FLUID = 1,
                                             // CHW_AC_DETECTOR_AIR = 2

   CHW_RESERVOIR_STATES reservoir;           // CHW_RESERVOIR_ERROR,
                                             // CHW_RESERVOIR_EMPTY,
                                             // CHW_RESERVOIR_MIDDLE,
                                             // CHW_RESERVOIR_HIGH


   HW_VALVE_STATES plasmaValve;              // HW_VALVE_UNKNOWN,
                                             // HW_VALVE_COLLECT,
                                             // HW_VALVE_OPEN,
                                             // HW_VALVE_RETURN
   HW_VALVE_STATES plateletValve;
   HW_VALVE_STATES rbcValve;

// fan status bits added for 2.6 HW.  Not used in 2.4 HW.
   HW_ERROR fanSense0;
   HW_ERROR fanSense1;
   HW_ERROR fanSense2;                         // Not used in any HW
   HW_ERROR tempError;

// power supply status bits added for 2.6 HW.  Not used in 2.4 HW.
   CHW_PS_OT_ERRORS psTempWarning;

// sound level status bits added for 2.6 HW.  Not used in 2.4 HW.
   CHW_SOUND_LEVELS soundLevel;

// test modes
   CHW_EVENTS event;                         // CHW_NORMAL_UPDATE,
                                             // CHW_AC_EVENT,
                                             // CHW_DOOR_EVENT,
                                             // CHW_PAUSE_EVENT,
                                             // CHW_STOP_EVENT,
                                             // CHW_RESERVOIR_EVENT

   long                         msgCount;    // accum message counter
   long                         test1;       // not currently used
   short                        lowAGC;      // low AGC voltage
   short                        highAGC;     // high AGC voltage

   CHW_SEAL_SAFE_OPERATION      sealSafeOperating;   // Only on Artysan power supplies
   CHW_SEAL_SAFE_OVER_TEMP_WARN sealSafeOvertemp;    // Only on Artysan power supplies
   CHW_PS_OVP_TEST_RESULT       ovpTestResult;       // Only on Artysan power supplies

   HW_PS_OUTPUT_STATUS          powerSupplyEnabled;  // Only on Artysan power supplies

   // Fix for IT6383: Added timestamp field to the status message to ensure synchronization between
   // control and safety computer in process hardware status messages
   struct timespec timestamp;

} CHwStates;
#endif

/* FORMAT HASH f6433ed1425426ae4dca7bd0579009a0 */
