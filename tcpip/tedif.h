/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      tedif.h
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 * HISTORY:    $Log: tedif.h $
 * HISTORY:    Revision 1.21  2009/07/15 17:06:19Z  dslausb
 * HISTORY:    IT 9221 - Add support for STS discovery message
 * HISTORY:    Revision 1.20  2009/07/08 17:00:44Z  dslausb
 * HISTORY:    Update TED interface for 6.0
 * HISTORY:    Revision 1.19  2009/03/24 20:54:13Z  rm70006
 * HISTORY:    IT 8143.  Rename test to openLoop.
 * HISTORY:    Revision 1.18  2008/11/17 17:52:35Z  dslausb
 * HISTORY:    Fix timing issue with TED and gooddog, add some more logging.
 * HISTORY:    Revision 1.17  2008/11/11 18:52:01Z  rm70006
 * HISTORY:    IT 8143.  Improve TED logging.
 * HISTORY:    Revision 1.16  2007/06/27 15:31:27Z  rm70006
 * HISTORY:    IT 7789.  Port over change from 5.2 that needs to be ported over.
 * HISTORY:    Revision 1.16  2006/12/06 15:08:01Z  rm70006
 * HISTORY:    IT 7808.  Fixed donor download to copy over metered options and improve the range checking.
 * HISTORY:    Revision 1.15  2006/11/08 21:45:33Z  rm70006
 * HISTORY:    IT 7232.  Added new status bits to Artesyn power supply
 * HISTORY:    Revision 1.14  2003/10/15 16:55:19Z  rm70006
 * HISTORY:    IT 6492.  Added both raw and calibrated values to interface.
 * HISTORY:    Revision 1.13  2003/10/14 22:00:29Z  rm70006
 * HISTORY:    IT 6492.  Add APS AutoZero message to interface.
 * HISTORY:    Revision 1.12  2003/10/08 18:32:06Z  rm70006
 * HISTORY:    IT 5830.  Put more fixes in TED.
 * HISTORY:    Revision 1.11  2003/09/08 20:04:58Z  rm70006
 * HISTORY:    IT 5830.  Add header to hw status message.
 * HISTORY:    Revision 1.10  2003/07/30 16:01:10Z  rm70006
 * HISTORY:    IT 6233.  Added new field to barcode message.
 * HISTORY:    Revision 1.9  2003/07/01 15:57:12Z  rm70006
 * HISTORY:    IT 5830.  Interim checkin.  Broadcast msg still not working.
 * HISTORY:    Revision 1.8  2003/06/16 21:58:39Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:    Add more functionality to TED.
 * HISTORY:    Revision 1.7  2003/04/22 17:00:36Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:
 * HISTORY:    Add win check to stop annoying compiler warning.
 * HISTORY:    Revision 1.6  2003/04/21 19:26:47Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:
 * HISTORY:    Make constant names unique to avoid name clashes with trima code.
 * HISTORY:    Revision 1.5  2003/04/07 23:12:17Z  rm70006
 * HISTORY:    IT 5841.
 * HISTORY:
 * HISTORY:    Add biometric data to broadcast.
 * HISTORY:    Revision 1.4  2003/01/31 21:09:34Z  jl11312
 * HISTORY:    - updated for new datalog stream format
 * HISTORY:    Revision 1.3  2003/01/31 21:06:08  jl11312
 * HISTORY:    - update for new datalog stream format
 * HISTORY:    Revision 1.2  2003/01/27 14:51:59  rm70006
 * HISTORY:    Change broadcast message to new format.
 * HISTORY:    Revision 1.1  2002/12/20 21:48:07Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/


#ifndef __TEDIF__
#define __TEDIF__

#ifndef WIN32    // Trima specific
#include "datalog.h"
#endif

#include "time.h"

// #include "hw.h"   // Used for types in the HWStatus messages.
#include "chw.hpp"
#include "shw.hpp"
#include "haldefs.h"  // used for HAL status definitions
#include "trima_info.h"


#ifdef WIN32

#pragma warning(disable : 4091)  // Added to remove Microsofts warning about "typedef struct typename"

#endif

const int TED_INTERFACE_REVISION_SIZE = 16;

// Update this when changes are made to match a particular TED ICD revision.
const char TED_IF_VERSION_NUMBER[] = "4.9";

enum TED_MessageId
{
   TED_FIRST_MSG                   = 11000,
   TED_CONNECT_REQUEST             = 11001,
   TED_HW_PERIODIC_STATUS_REQUEST  = 11002,
   TED_CONTROL_HARDWARE_CMD        = 11003,
   TED_SAFETY_HARDWARE_CMD         = 11004,
   TED_END_SERVICE_MODE            = 11005,
   TED_MOVE_CURSOR_CMD             = 11006,
   TED_SET_TIME                    = 11007,
   TED_END_CONNECTION              = 11008,
   TED_CONTROL_HARDWARE_STATUS     = 11009,
   TED_SAFETY_HARDWARE_STATUS      = 11010,
   TED_REPLY                       = 11011,
   TED_REQUEST_CURSOR_POSITION     = 11012,
   TED_CURRENT_CURSOR_POSITION     = 11013,
   TED_BROADCAST_REQUEST           = 11014,
   TED_BROADCAST                   = 11015,
   TED_REQUEST_APS_AUTOZERO_VALUE  = 11016,
   TED_APS_AUTOZERO_VALUE          = 11017,
   // IDs 11018-11019 reserved
   TED_SCREEN_CALIBRATION_MESSAGE  = 11020,
   // IDs 11021-11027 reserved
   TED_CONTROL_OVP_TEST_REQUEST    = 11028,
   TED_LOGGING_MESSAGE             = 11029,
   TED_HARDWARE_REQUEST            = 11031, // Request for hardware information (XML)
   TED_HARDWARE_REPLY              = 11032,
   TED_FULL_VERSION_REQUEST        = 11033,
   TED_FULL_VERSION_REPLY          = 11034,
   TED_INSTALL_REGIONALIZATION_KEY = 11035,
   TED_CONFIRMATION_CODE_REQUEST   = 11036,
   TED_CONFIRMATION_CODE_REPLY     = 11037,
   TED_LAST_MSG                    = TED_CONFIRMATION_CODE_REPLY
};


enum TED_Status
{
   TED_CONNECTION_ALLOWED_OPERATIONAL     = 0,
   TED_CONNECTION_ALLOWED_SERVICE         = 1,
   TED_CONNECTION_ALLOWED_SINGLE_STEP     = 2,
   TED_CONNECTION_DENIED                  = 3,
   TED_REQUEST_ALLOWED                    = 4,
   TED_REQUEST_ALLOWED_OVERRIDE           = 5,
   TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE = 6,
   TED_REQUEST_DENIED_NOT_CLIENT          = 7,
   TED_BAD_MESSAGE_RECEIVED               = 8,
   TED_OPERATION_FAILED                   = 9,
   TED_SAFETY_NOT_STARTED                 = 10,
   TED_UNAUTHORIZED_USER                  = 11,
   TED_UNAUTHORIZED_MACHINE               = 12,
   TED_BAD_VALUE                          = 13
};



//
// TED Message Header
//
typedef struct TED_Header
{
   unsigned long som        __attribute__ ((packed)); // start of message sentinel always set to 0x000DECAF
   unsigned long msgId      __attribute__ ((packed)); // message id
   unsigned long length     __attribute__ ((packed)); // total message length in bytes
   time_t        time       __attribute__ ((packed)); // time message was sent
   unsigned long bodyCRC    __attribute__ ((packed)); // CRC value of the body of the message
   unsigned long headerCRC  __attribute__ ((packed)); // CRC value of the header of the message

   inline TED_Header()
      : som(0), msgId(0), length(0), time(0), bodyCRC(0), headerCRC(0) {}
};



enum {TED_HEADER_SIZE = sizeof(TED_Header)};


//
// TED Reply Message
//
typedef struct TED_ReplyMsg
{
   TED_Header hdr;
   TED_Status status __attribute__ ((packed));
};



const unsigned int TED_ReplyMsgSize = sizeof(TED_ReplyMsg);



//
// TED Connect Request Message
//
typedef struct TED_ConnectRequestMsg
{
   TED_Header hdr;
};



const unsigned int TED_ConnectRequestMsgSize = sizeof(TED_ConnectRequestMsg);



//
// TED HW Periodic Status Request Message
//
enum TED_BoardType {CONTROL = 0,
                    SAFETY  = 1};


typedef struct TED_HwPeriodicStatusRequestMsg
{
   TED_Header    hdr;
   TED_BoardType board  __attribute__ ((packed));         // Status being requested from TED.
   unsigned int  period __attribute__ ((packed));         // Period of requested status in ms.
   unsigned int  port   __attribute__ ((packed));         // Port Response message goes to.
};



const unsigned int TED_HwPeriodicStatusRequestMsgSize = sizeof(TED_HwPeriodicStatusRequestMsg);



//
// Control HW Status Message
//
typedef struct CHWPumpStatus
{
   unsigned char errors;                     // CHW_PUMP_NO_FAULT,
                                             // CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
                                             // CHW_PUMP_INTEGRAL_ERROR = 2,                                             // CHW_PUMP_VOLUME_ERROR = 4
   // pump actual rpm
   float RPM            __attribute__ ((packed)); // ~-250 to ~250, for return pump                                             // 0 to 250 for all other pumps
   // pump revolutions
   float Revs           __attribute__ ((packed)); // Delta revs since last update
   // pump accumulated revs, encoder ticks
   long  Accum          __attribute__ ((packed)); // total revs, encoder ticks
   // error terms
   long  Integral       __attribute__ ((packed)); // snap-shot of the integration error
   long  AcummIntegral  __attribute__ ((packed)); // cummulative integration error for the                                             // course of the entire run

   CHWPumpStatus& operator = (const PumpStatus& rhs);
};



typedef struct TED_ControlHwStatusMsg
{
   TED_Header hdr;

// A/D channels, raw sensor readings, uncalibrated, 0-4095 counts
   short accessPressure         __attribute__ ((packed));   // access (inlet) pressure
   short emiTemperature         __attribute__ ((packed));   // temperature of EMI box
   short centrifugeCurrent      __attribute__ ((packed));   // centrifuge current
   short centrifugePressure     __attribute__ ((packed));   // centrifuge pressure sensor
   short fiveVoltSupply         __attribute__ ((packed));   // + 5 volt supply
   short leakDetector           __attribute__ ((packed));   // leak detector
   short minusTwelvePS          __attribute__ ((packed));   // -12 volt supply
   short sixtyFourVoltCurrent   __attribute__ ((packed));   // +64 volt (centrifuge) current
   short sixtyFourVoltSupply    __attribute__ ((packed));   // +64 volt (centrifuge) before switch
   short sixtyFourVoltSwitched  __attribute__ ((packed));   // +64 volt (centrifuge) after switch
   short twelveVoltSupply       __attribute__ ((packed));   // +12 volt supply
   short twentyFourVoltCurrent  __attribute__ ((packed));   // +24 volt supply current
   short twentyFourVoltSupply   __attribute__ ((packed));   // +24 volt before switch
   short twentyFourVoltSwitched __attribute__ ((packed));   // +24 volt after switch

// cassette position
   HW_CASSETTE_STATES cassettePosition __attribute__ ((packed));    // HW_CASSETTE_UNKNOWN,
                                                                    // HW_CASSETTE_DOWN,
                                                                    // HW_CASSETTE_UP

   HW_ERROR cassetteError    __attribute__ ((packed));              // HW_NO_FAULT, HW_FAULT

   // centrifuge
   float         centrifugeRPM    __attribute__ ((packed));         // 0 to ~3000 rpm
   unsigned char centrifugeError;                                   // HW_CENT_NO_FAULT,
                                                                    // HW_CENT_HARDWARE_ERROR=1,
                                                                    // HW_CENT_INTEGRAL_ERROR=2,
   // Misc
   HW_DOOR_STATES doorLocks        __attribute__ ((packed));        // HW_DOOR_OPEN,
                                                                    // HW_DOOR_LOCKED_STATE,
                                                                    // HW_DOOR_CLOSED_STATE,
                                                                    // HW_DOOR_CLOSED_AND_LOCKED

   HW_SWITCH_STATES pauseSwitch      __attribute__ ((packed));      // HW_SWITCH_DISABLED,
                                                                    // HW_SWITCH_ENABLED
   HW_SWITCH_STATES stopSwitch       __attribute__ ((packed));
   HW_SWITCH_STATES pauseSwitchLatch __attribute__ ((packed));
   HW_SWITCH_STATES stopSwitchLatch  __attribute__ ((packed));
   HW_ERROR         vibration        __attribute__ ((packed));      // HW_NO_FAULT, HW_FAULT
   HW_ERROR         valveFault       __attribute__ ((packed));      // HW_NO_FAULT, HW_FAULT
   HW_ERROR         motorFault       __attribute__ ((packed));      // HW_NO_FAULT, HW_FAULT
   HW_ERROR         solenoidFault    __attribute__ ((packed));      // HW_NO_FAULT, HW_FAULT

   // rbc detector
   short    red              __attribute__ ((packed));              // raw red value, 0-~32000 counts
   short    green            __attribute__ ((packed));              // raw green value, 0-~32000 counts
   HW_ERROR rbcCommError     __attribute__ ((packed));              // HW_NO_FAULT, HW_FAULT

   // pumps
   CHWPumpStatus acPump           __attribute__ ((packed));         // AC
   CHWPumpStatus inletPump        __attribute__ ((packed));         // Inlet
   CHWPumpStatus plasmaPump       __attribute__ ((packed));         // Plasma
   CHWPumpStatus plateletPump     __attribute__ ((packed));         // Platelet
   CHWPumpStatus returnPump       __attribute__ ((packed));         // Return

// ultrasonics
   CHW_AC_DETECTOR_STATES acDetector    __attribute__ ((packed));   // CHW_AC_DETECTOR_FLUID = 1,
                                                                    // CHW_AC_DETECTOR_AIR = 2

   CHW_RESERVOIR_STATES reservoir     __attribute__ ((packed));     // CHW_RESERVOIR_ERROR,
                                                                    // CHW_RESERVOIR_EMPTY,
                                                                    // CHW_RESERVOIR_MIDDLE,
                                                                    // CHW_RESERVOIR_HIGH


   HW_VALVE_STATES plasmaValve   __attribute__ ((packed));          // HW_VALVE_UNKNOWN,
                                                                    // HW_VALVE_COLLECT,
                                                                    // HW_VALVE_OPEN,
                                                                    // HW_VALVE_RETURN
   HW_VALVE_STATES  plateletValve __attribute__ ((packed));
   HW_VALVE_STATES  rbcValve      __attribute__ ((packed));

   HW_ERROR         fanSense0     __attribute__ ((packed));
   HW_ERROR         fanSense1     __attribute__ ((packed));
   HW_ERROR         fanSense2     __attribute__ ((packed));         // Not used in any HW
   HW_ERROR         tempError     __attribute__ ((packed));

   CHW_PS_OT_ERRORS psTempWarning __attribute__ ((packed));

   CHW_SOUND_LEVELS soundLevel    __attribute__ ((packed));

// test modes
   CHW_EVENTS event         __attribute__ ((packed));               // CHW_NORMAL_UPDATE,
                                                                    // CHW_AC_EVENT,
                                                                    // CHW_DOOR_EVENT,
                                                                    // CHW_PAUSE_EVENT,
                                                                    // CHW_STOP_EVENT,
                                                                    // CHW_RESERVOIR_EVENT

   long                         msgCount      __attribute__ ((packed)); // accum message counter
   short                        lowAGC        __attribute__ ((packed)); // low AGC voltage
   short                        highAGC       __attribute__ ((packed)); // high AGC voltage

   CHW_SEAL_SAFE_OPERATION      sealSafeOperating  __attribute__ ((packed));    // Only on Artesyn power supplies
   CHW_SEAL_SAFE_OVER_TEMP_WARN sealSafeOvertemp   __attribute__ ((packed));    // Only on Artesyn power supplies
   CHW_PS_OVP_TEST_RESULT       ovpTestResult      __attribute__ ((packed));    // Only on Artesyn power supplies
   CHW_PS_TEST_STATUS           ovpTestStatus      __attribute__ ((packed));

   short                        mxFpgaInstalled             __attribute__ ((packed)); // 0 or 1 depending on whether or not
                                                                                      // the MX FPGA is installed
   unsigned char                fpgaRevision;                                         // firmware revision for control FPGA
   unsigned char                fpgaIdRevision;                                       // hw revision for control FPGA
   unsigned char                fpgaInterfaceRevision;                                // sw interface revision for control FPGA
   unsigned char                ccaRevision;                                          // CCA revision from control FPGA
   HW_WATCHDOG_STATUS           watchDogStatus          __attribute__ ((packed));     // HW Watchdog Status
   HW_PS_OUTPUT_STATUS          powerSupplyOutputStatus __attribute__ ((packed));     // PS Output Status
};


// Extension to the Data Log streams for GRIP_OVPTestResult
inline DataLog_Stream& operator << (DataLog_Stream& os, const CHW_PS_OVP_TEST_RESULT& OVPTestResult)
{
   switch ( OVPTestResult )
   {
      case CHW_OVP_TEST_RESULT_NOT_AVAILABLE : os << "OVP_TEST_RESULT_NOT_AVAILABLE"; break;
      case CHW_OVP_TEST_RESULT_FAIL : os << "OVP_TEST_RESULT_FAIL"; break;
      case CHW_OVP_TEST_RESULT_PASS : os << "OVP_TEST_RESULT_PASS"; break;
      default : break;
   }
   os << "(" << (int)OVPTestResult << ")" ;
   return os;
}


const unsigned int TED_ControlHwStatusMsgSize = sizeof(TED_ControlHwStatusMsg);



//
// Safety HW Status Message
//
typedef struct TED_SafetyHwStatusMsg
{
   TED_Header hdr;

// pumps
   long          inletRPM             __attribute__ ((packed));         // 0 to ~250 rpm
   long          inletRevs            __attribute__ ((packed));         // delta counts
   long          inletAccumRevs       __attribute__ ((packed));         // total counts

   long          plateletRPM          __attribute__ ((packed));         // 0 to ~250 rpm
   long          plateletRevs         __attribute__ ((packed));         // delta counts
   long          plateletAccumRevs    __attribute__ ((packed));         // total counts

   long          plasmaRPM            __attribute__ ((packed));         // 0 to ~250 rpm
   long          plasmaRevs           __attribute__ ((packed));         // delta counts
   long          plasmaAccumRevs      __attribute__ ((packed));         // total counts

   long          acRPM                __attribute__ ((packed));         // 0 to ~250 rpm
   long          acRevs               __attribute__ ((packed));         // delta counts
   long          acAccumRevs          __attribute__ ((packed));         // total counts

   long          returnRPM            __attribute__ ((packed));         // ~-250 to ~250 rpm
   long          returnRevs           __attribute__ ((packed));         // delta counts
   long          returnAccumRevs      __attribute__ ((packed));         // total counts
   SHW_DIRECTION returnDirection      __attribute__ ((packed));         // SHW_RETURN, SHW_DRAW

   // valve status    0 = HW_VALVE_UNKNOWN
   //                 1 = HW_VALVE_COLLECT
   //                 2 = HW_VALVE_OPEN
   //                 3 = HW_VALVE_RETURN

   HW_VALVE_STATES rbcValve             __attribute__ ((packed));
   HW_VALVE_STATES plasmaValve          __attribute__ ((packed));
   HW_VALVE_STATES plateletValve        __attribute__ ((packed));

   unsigned char   valveLedTest;            // 1=ok, all others =error

   // cassette position
   HW_CASSETTE_STATES cassettePosition     __attribute__ ((packed));    // HW_CASSETTE_UNKNOWN, HW_CASSETTE_DOWN,
                                                                        // HW_CASSETTE_UP

   // ultrasonics
   SHW_RESERVOIR_STATES reservoir           __attribute__ ((packed));   // 1=SHW_RESERVOIR_EMPTY, SHW_RESERVOIR_LOW

   // centrifuge
   float         centrifugeRPM        __attribute__ ((packed));         // RPM
   unsigned char centrifugeError;                                       // HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR=1,
                                                                        // HW_CENT_INTEGRAL_ERROR=2

   // misc
   HW_DOOR_STATES doorLocks            __attribute__ ((packed));        // HW_DOOR_OPEN, HW_DOOR_LOCKED_STATE,
                                                                        // HW_DOOR_CLOSED,
                                                                        // HW_DOOR_CLOSED_AND_LOCKED

   bool       pauseSwitch;                                              // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED
   bool       stopSwitch;                                               // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED

   SHW_EVENTS event                __attribute__ ((packed));            // SHW_NORMAL_UPDATE,
                                                                        // SHW_PAUSE_EVENT, SHW_STOP_EVENT,
                                                                        // SHW_DOOR_EVENT,
                                                                        // SHW_RESERVOIR_EVENT,
                                                                        // SHW_RETURN_EVENT, SHW_AIR_EVENT


   long           msgCount             __attribute__ ((packed));        // accum message counter

   unsigned short returnPumpDirChgTime __attribute__ ((packed));        // time from low level sensor
                                                                        // indicating air to return
                                                                        // pump in draw direction,
                                                                        // milliseconds

   // test modes
   long      test1                __attribute__ ((packed));
   short     test2                __attribute__ ((packed));

   HW_ORDERS serviceEnableMode    __attribute__ ((packed));             // HW_DISABLE=!servicemode,
                                                                        // HW_ENABLE=servicemode
   HW_ORDERS donorConnectMode     __attribute__ ((packed));             // HW_DISABLE=!donorconnect,
                                                                        // HW_ENABLE=donorconnect
};



const unsigned int TED_SafetyHwStatusMsgSize = sizeof(TED_SafetyHwStatusMsg);



//
// Control Hardware Order Message
//
typedef struct TED_ControlHwOrderMsg
{
   TED_Header          hdr;
   CHW_CASSETTE_ORDERS cassettePosition         __attribute__ ((packed));   // CHW_CASSETTE_DOWN=1,
                                                                            // CHW_CASSETTE_UP=2
   // centrifuge
   float centrifugeRPM            __attribute__ ((packed));                 // 0 to 3100 RPM
                                                                            // -1 disables the motor controller
   float centrifugeRampUpRate     __attribute__ ((packed));                 // RPM / sec
   float centrifugeRampDownRate   __attribute__ ((packed));                 // RPM / sec

   // misc
   HW_ORDERS       alarmLight               __attribute__ ((packed));       // HW_DISABLE, HW_ENABLE=light on
   HW_ORDERS       doorLatches              __attribute__ ((packed));       // HW_DISABLE, HW_ENABLE=power on
   CHW_DOOR_ORDERS doorDirection            __attribute__ ((packed));       // CHW_DOOR_UNLOCK, CHW_DOOR_LOCK

   // pump RPM orders
   float            inletRPM                 __attribute__ ((packed));      // 0 - 250 RPM
   float            plateletRPM              __attribute__ ((packed));      // 0 - 250 RPM
   float            plasmaRPM                __attribute__ ((packed));      // 0 - 250 RPM
   float            acRPM                    __attribute__ ((packed));      // 0 - 250 RPM
   float            returnRPM                __attribute__ ((packed));      // -250 to 250 RPM

   CHW_VALVE_ORDERS rbcValve                 __attribute__ ((packed));      // CHW_VALVE_COLLECT=1,
                                                                            // CHW_VALVE_OPEN=2,
                                                                            // CHW_VALVE_RETURN=3
   CHW_VALVE_ORDERS plasmaValve              __attribute__ ((packed));
   CHW_VALVE_ORDERS plateletValve            __attribute__ ((packed));

   // rbc values
   unsigned char    redDrive;                  // 0 to 255, typically 64
   unsigned char    greenDrive;                // 0 to 255, typically 128

   CHW_SOUND_LEVELS soundLevel               __attribute__ ((packed));      // CHW_SOUND_LOUD=0, CHW_SOUND_MEDIUM_LOUD,
                                                                            // CHW_SOUND_MEDIUM_SOFT, CHW_SOUND_SOFT
   // test modes
   long openLoop                 __attribute__ ((packed));                  // 0x01 =  open loop mode for pumps and centrifuge control

   // APS limits
   short               highAPSLimit             __attribute__ ((packed));   // High access pressure limit
   short               lowAPSLimit              __attribute__ ((packed));   // Low access pressure limit

   HW_PS_OUTPUT_STATUS powerSupplyEnabled       __attribute__ ((packed));
};



const unsigned int TED_ControlHwOrderMsgSize = sizeof(TED_ControlHwOrderMsg);



//
// Safety Hardware Order Message
//
typedef struct TED_SafetyHwOrderMsg
{
   TED_Header hdr;
   HW_ORDERS  pumpPower          __attribute__ ((packed));      // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS  centrifugePower    __attribute__ ((packed));      // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS  valveLED           __attribute__ ((packed));      // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS  doorSolenoidPower  __attribute__ ((packed));      // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS  alarmLight         __attribute__ ((packed));      // HW_DISABLE=0, HW_ENABLE

   // allow service mode and the safe executive to control air-to-donor
   HW_ORDERS serviceEnableMode  __attribute__ ((packed));       // HW_DISABLE=!servicemode,
                                                                // HW_ENABLE=servicemode
   HW_ORDERS donorConnectMode   __attribute__ ((packed));       // HW_DISABLE=!donorconnect,
                                                                // HW_ENABLE=donorconnect
   // test modes
   long test1              __attribute__ ((packed));
   long test2              __attribute__ ((packed));
};



const unsigned int TED_SafetyHwOrderMsgSize = sizeof(TED_SafetyHwOrderMsg);



//
// End Service Mode Message
//
typedef struct TED_EndServiceModeMsg
{
   TED_Header hdr;
};



const unsigned int TED_EndServiceModeMsgSize = sizeof(TED_EndServiceModeMsg);



//
// Move Cursor Command
//
enum TED_CURSOR_ORDERS
{
   TED_CURSOR_LEFT  = 1,
   TED_CURSOR_RIGHT = 2,
   TED_CURSOR_UP    = 3,
   TED_CURSOR_DOWN  = 4
};


typedef struct TED_MoveCursorMsg
{
   TED_Header        hdr;
   TED_CURSOR_ORDERS order __attribute__ ((packed));
};



const unsigned int TED_MoveCursorMsgSize = sizeof(TED_MoveCursorMsg);



//
// Set Time Message
//
typedef struct TED_SetTimeMsg
{
   TED_Header hdr;
   time_t     time __attribute__ ((packed));        // Time in UTC
};



const unsigned int TED_SetTimeMsgSize = sizeof(TED_SetTimeMsg);



//
// End Connection Message
//
typedef struct TED_EndConnectionMsg
{
   TED_Header hdr;
};



const unsigned int TED_EndConnectionMsgSize = sizeof(TED_EndConnectionMsg);



//
// Request Cursor Position
//
typedef struct TED_RequestCursorPositionMsg
{
   TED_Header hdr;
};



const unsigned int TED_RequestCursorPositionMsgSize = sizeof(TED_RequestCursorPositionMsg);



//
// Current Cursor Position
//
typedef struct TED_CurrentCursorPositionMsg
{
   TED_Header hdr;
   int        x   __attribute__ ((packed));
   int        y   __attribute__ ((packed));
};



const unsigned int TED_CurrentCursorPositionMsgSize = sizeof(TED_CurrentCursorPositionMsg);



//
// Request Trima Broadcast Message
//
typedef struct TED_RequestTrimaBroadcastMsg
{
   TED_Header hdr;
};



const unsigned int TED_RequestTrimaBroadcastMsgSize = sizeof(TED_RequestTrimaBroadcastMsg);



//
// TED Broadcast Message
//
const int TED_HW_REV_SIZE          = 64;
const int TED_IP_ADDRESS_SIZE      = 32;
const int TED_LOG_FILENAME_SIZE    = 32;
const int TED_SERIAL_NUMBER_SIZE   = 16;
const int TED_CRC_SIZE             = 16;
const int TED_RELEASE_SIZE         = 16;
const int TED_BUILD_SIZE           = 16;
const int TED_PORT_NUMBER_SIZE     =  8;
const int TED_SEQUENCE_NUMBER_SIZE =  7;
const int TED_VIPIF_VERSION_SIZE   = 32;
const int TED_LOG_STRING_SIZE      = 256;

//
// Biometric Device List
//
#define TED_BIO_SIGNATURE_PIN  1


typedef struct TED_BroadcastMsg
{
   TED_Header   hdr;
   int          broadcastCount __attribute__ ((packed));         // count of packets broadcasted
   char         trimaHWRev[TED_HW_REV_SIZE];                     // Control & Safety Hdw Revisions
   char         trimaIP[TED_IP_ADDRESS_SIZE];                    // 172.80.90.34
   char         trimaLogFile[TED_LOG_FILENAME_SIZE];             // T00015_0504_0033
   char         trimaSerialNumber[TED_SERIAL_NUMBER_SIZE];       // T00015
   char         trimaCRC[TED_CRC_SIZE];                          // 0xadcef73
   char         trimaRelease[TED_RELEASE_SIZE];                  // Trima 3.3a
   char         trimaBuild[TED_BUILD_SIZE];                      // 1.191
   char         trimaPort[TED_PORT_NUMBER_SIZE];                 // 7001
   char         sequenceNumber[TED_SEQUENCE_NUMBER_SIZE];        // 000000
   char         vipifVersionNumber[TED_VIPIF_VERSION_SIZE];      // $Revision: 1.24 $
   unsigned int biometricDeviceList __attribute__ ((packed));    // Bit field containing Biometric ID's.
   char         tedifVersionNumber[TED_INTERFACE_REVISION_SIZE]; // TED_InterfaceRevision
};



const unsigned int TED_BroadcastMsgSize = sizeof(TED_BroadcastMsg);


enum TED_BUFFER_STATUS
{
   TED_BUFFER_UNDERRUN,
   TED_BUFFER_OVERRUN,
   TED_HEADER_CRC_INVALID,
   TED_BODY_CRC_INVALID,
   TED_BAD_SOM,
   TED_MESSAGE_ID_INVALID,
   TED_OK
};



//
// APS AutoZero Request Msg
//
typedef struct TED_ApsAutoZeroRequestMsg
{
   TED_Header hdr;
};

const unsigned int TED_ApsAutoZeroRequestMsgSize = sizeof(TED_ApsAutoZeroRequestMsg);


//
// APS AutoZero Value Msg
//
typedef struct TED_ApsAutoZeroValueMsg
{
   TED_Header hdr;
   float      rawValue          __attribute__ ((packed));   // Raw value from cal.dat
   float      referenceValue    __attribute__ ((packed));   // Auto zero value computed by proc
};

const unsigned int TED_ApsAutoZeroValueMsgSize = sizeof(TED_ApsAutoZeroValueMsg);



//
// TED Logging Message
//
typedef struct TED_LoggingMsg
{
   TED_Header hdr;
   char       loggingMsg[TED_LOG_STRING_SIZE];             // Trima 3.3a
};

const unsigned int TED_LoggingMsgSize = sizeof(TED_LoggingMsg);


//
// TED Screen_Calibration_Request
//
struct TED_ScreenCalibrationRequestMsg
{
   TED_Header hdr;
};

const unsigned int TED_ScreenCalibrationRequestMsgSize = sizeof(TED_ScreenCalibrationRequestMsg);


//
// TED OVP Test Request
//
struct TED_ControlOVPTestRequestMsg
{
   TED_Header hdr;
};

const unsigned int TED_ControlOVPTestRequestMsgSize = sizeof(TED_ControlOVPTestRequestMsg);

//
// TED Hardware Request
//
struct TED_HardwareRequestMsg
{
   TED_Header hdr;
};

const unsigned int TED_HardwareRequestMsgSize = sizeof(TED_HardwareRequestMsg);

//
// TED Hardware Reply
//
const int TED_DEVICE_HARDWARE_XML = 512;    // Max length of XML data to be sent back

struct TED_HardwareReplyMsg
{
   TED_Header header;

   char       deviceHardwareXML[TED_DEVICE_HARDWARE_XML]; // XML containing hardware information for STS
};

const unsigned int GRIP_TaosHardwareInformationReplyMsgSize = sizeof(TED_HardwareReplyMsg);

//
// TED Hardware Request
//
struct TED_FullVersionRequestMsg
{
   TED_Header hdr;
};

const unsigned int TED_FullVersionRequestMsgSize = sizeof(TED_FullVersionRequestMsg);

//
// TED Hardware Reply
//
const int TED_FULL_VERSION_STRING_LENGTH = 20;    // Max length of string data to be sent back

struct TED_FullVersionReplyMsg
{
   TED_Header header;

   char       trimaFullVersion[TED_FULL_VERSION_STRING_LENGTH]; // string data containing version text
};

const unsigned int TED_FullVersionReplyMsgSize = sizeof(TED_FullVersionReplyMsg);

//
// TED Install Regionalization Key message
//
const int TED_KEY_SIZE = 256;

struct TED_InstallKeyMsg
{
   TED_Header hdr;
   char       key[TED_KEY_SIZE];
};

const unsigned int TED_InstallKeyMsgSize = sizeof(TED_InstallKeyMsg);

//
// TED Confirmation Code Request
//

struct TED_ConfirmationCodeRequest
{
   TED_Header hdr;
};

const unsigned int TED_ConfirmationCodeRequestSize = sizeof(TED_ConfirmationCodeRequest);

//
// TED Confirmation Code Reply
//
const int TED_CONFIRM_CODE_SIZE = 256;
struct TED_ConfirmationCodeReply
{
   TED_Header hdr;
   char       code[TED_CONFIRM_CODE_SIZE];
};

const unsigned int TED_ConfirmationCodeReplySize = sizeof(TED_ConfirmationCodeReply);

//
// TED Interface
//
class TedIf
{
public:
   TedIf();

   virtual ~TedIf();

   // Validates a message
   TED_BUFFER_STATUS validHeader (const void* msg, unsigned long length);

   int findSOM (const void* msg, unsigned long length);

   // Prepares a message for sending by setting up the message header.
   void prepareMsg (void* msg, enum TED_MessageId messageId, unsigned long sizeInBytes = TED_HEADER_SIZE);

   const char* errorString () const { return _errorString; }

#ifndef WIN32
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_Header& hdr);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_Header* hdr);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_ControlHwStatusMsg& status);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_BUFFER_STATUS& status);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_Status& status);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const TED_BoardType& type);
   friend DataLog_Stream& operator << (DataLog_Stream& os, const CHWPumpStatus& status);
#endif

private:
   enum {TED_SOM = 0x000DECAF};

   char _errorString[128];
};


#endif

/* FORMAT HASH 276e0c3ce5fd895304ccd4ac9ffa0cfb */
