
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
 * HISTORY:    Revision 1.1  2009/04/14 20:31:33Z  jsylusb
 * HISTORY:    Initial revision
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

//#include "hw.h"   // Used for types in the HWStatus messages.
#include "chw.hpp"
#include "shw.hpp"
#include "haldefs.h"  // used for HAL status definitions


#ifdef WIN32

#pragma warning(disable : 4091)  // Added to remove Microsofts warning about "typedef struct typename"

#endif



enum TED_MessageId 
{
   TED_FIRST_MSG                  = 11000,
   TED_CONNECT_REQUEST            = 11001,
   TED_HW_PERIODIC_STATUS_REQUEST = 11002,
   TED_CONTROL_HARDWARE_CMD       = 11003,
   TED_SAFETY_HARDWARE_CMD        = 11004,
   TED_END_SERVICE_MODE           = 11005,
   TED_MOVE_CURSOR_CMD            = 11006,
   TED_SET_TIME                   = 11007,
   TED_END_CONNECTION             = 11008,
   TED_CONTROL_HARDWARE_STATUS    = 11009,
   TED_SAFETY_HARDWARE_STATUS     = 11010,
   TED_REPLY                      = 11011,
   TED_REQUEST_CURSOR_POSITION    = 11012,
   TED_CURRENT_CURSOR_POSITION    = 11013,
   TED_BROADCAST_REQUEST          = 11014,
   TED_BROADCAST                  = 11015,
   TED_REQUEST_APS_AUTOZERO_VALUE = 11016,
   TED_APS_AUTOZERO_VALUE         = 11017,
   TED_LAST_MSG                   = TED_APS_AUTOZERO_VALUE
};


enum TED_Status
{
   TED_CONNECTION_ALLOWED_OPERATIONAL      = 0,
   TED_CONNECTION_ALLOWED_SERVICE          = 1,
   TED_CONNECTION_ALLOWED_SINGLE_STEP      = 2,
   TED_CONNECTION_DENIED                   = 3,
   TED_REQUEST_ALLOWED                     = 4,
   TED_REQUEST_ALLOWED_OVERRIDE            = 5,
   TED_REQUEST_DENIED_NOT_IN_SERVICE_MODE  = 6,
   TED_REQUEST_DENIED_NOT_CLIENT           = 7,
   TED_BAD_MESSAGE_RECEIVED                = 8,
   TED_OPERATION_FAILED                    = 9,
};



//
// TED Message Header
//
typedef struct TED_Header
{
   unsigned long som;        // start of message sentinel always set to 0xCAFED00D
   unsigned long msgId;      // message id
   unsigned long length;     // total message length in bytes
   time_t        time;       // time message was sent 
   unsigned long bodyCRC;    // CRC value of the body of the message
   unsigned long headerCRC;  // CRC value of the header of the message

   inline TED_Header() : som(0), msgId(0), length(0), time(0), bodyCRC(0), headerCRC(0) {}
};



enum {TED_HEADER_SIZE = sizeof(TED_Header)};


//
// TED Reply Message
//
typedef struct TED_ReplyMsg
{
   TED_Header hdr;
   TED_Status status;
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
   TED_BoardType board;          // Status being requested from TED.
   unsigned int  period;         // Period of requested status in ms.
   unsigned int  port;           // Port Response message goes to.
};



const unsigned int TED_HwPeriodicStatusRequestMsgSize = sizeof(TED_HwPeriodicStatusRequestMsg);



//
// Control HW Status Message
//
typedef struct CHWPumpStatus
{
   unsigned char   errors;                   // CHW_PUMP_NO_FAULT,
                                             // CHW_PUMP_FAIL_TO_MAINTAIN_SPEED = 1,
                                             // CHW_PUMP_INTEGRAL_ERROR = 2,                                             // CHW_PUMP_VOLUME_ERROR = 4
// pump actual rpm
   float RPM;                                // ~-250 to ~250, for return pump                                             // 0 to 250 for all other pumps
// pump revolutions
   float Revs;                               // Delta revs since last update
// pump accumulated revs, encoder ticks
   long  Accum;                              // total revs, encoder ticks
// error terms
   long  Integral;                           // snap-shot of the integration error
   long  AcummIntegral;                      // cummulative integration error for the                                             // course of the entire run

   CHWPumpStatus & operator = (const PumpStatus &rhs);
};



typedef struct TED_ControlHwStatusMsg
{
   TED_Header          hdr;

// A/D channels, raw sensor readings, uncalibrated, 0-4095 counts
   short               accessPressure;            // access (inlet) pressure
                                                  // seal safe data not used in 2.6 HW
   short               batteryCurrent;            // seal safe battery for 2.2/2.4
   short               batteryVoltage;            // seal safe battery for 2.2/2.4
                                                  // temperature data not used in 2.2/2.4 HW
   short               emiTemperature;            // temperature of EMI box for 2.6 only
   short               centrifugeCurrent;         // centrifuge current
   short               centrifugePressure;        // centrifuge pressure sensor
   short               fiveVoltSupply;            // + 5 volt supply
   short               leakDetector;              // leak detector
   short               minusTwelvePS;             // -12 volt supply
   short               sixtyFourVoltCurrent;      // +64 volt (centrifuge) current
   short               sixtyFourVoltSupply;       // +64 volt (centrifuge) before switch
   short               sixtyFourVoltSwitched;     // +64 volt (centrifuge) after switch
   short               twelveVoltSupply;          // +12 volt supply
   short               twentyFourVoltCurrent;     // +24 volt supply current
   short               twentyFourVoltSupply;      // +24 volt before switch
   short               twentyFourVoltSwitched;    // +24 volt after switch

// cassette position
   HW_CASSETTE_STATES  cassettePosition;          // HW_CASSETTE_UNKNOWN,
                                                  // HW_CASSETTE_DOWN,
                                                  // HW_CASSETTE_UP
   
   HW_ERROR            cassetteError;             // HW_NO_FAULT, HW_FAULT

// centrifuge
   float               centrifugeRPM;             // 0 to ~3000 rpm
   unsigned char       centrifugeError;           // HW_CENT_NO_FAULT,
                                                  // HW_CENT_HARDWARE_ERROR=1,
                                                  // HW_CENT_INTEGRAL_ERROR=2,
// Misc
   HW_DOOR_STATES      doorLocks;                 // HW_DOOR_OPEN,
                                                  // HW_DOOR_LOCKED_STATE,
                                                  // HW_DOOR_CLOSED_STATE,
                                                  // HW_DOOR_CLOSED_AND_LOCKED

   HW_SWITCH_STATES    pauseSwitch;               // HW_SWITCH_DISABLED,
                                                  // HW_SWITCH_ENABLED
   HW_SWITCH_STATES    stopSwitch;             
   HW_SWITCH_STATES    pauseSwitchLatch;       
   HW_SWITCH_STATES    stopSwitchLatch;        
   HW_ERROR            vibration;                 // HW_NO_FAULT, HW_FAULT
   HW_ERROR            valveFault;                // HW_NO_FAULT, HW_FAULT
   HW_ERROR            motorFault;                // HW_NO_FAULT, HW_FAULT
   HW_ERROR            solenoidFault;             // HW_NO_FAULT, HW_FAULT

// rbc detector
   short               red;                       // raw red value, 0-~32000 counts
   short               green;                     // raw green value, 0-~32000 counts
   HW_ERROR            rbcCommError;              // HW_NO_FAULT, HW_FAULT

// pumps
   CHWPumpStatus       acPump;                    // AC
   CHWPumpStatus       inletPump;                 // Inlet
   CHWPumpStatus       plasmaPump;                // Plasma
   CHWPumpStatus       plateletPump;              // Platelet
   CHWPumpStatus       returnPump;                // Return

// ultrasonics
   CHW_AC_DETECTOR_STATES acDetector;             // CHW_AC_DETECTOR_FLUID = 1,
                                                  // CHW_AC_DETECTOR_AIR = 2

   CHW_RESERVOIR_STATES   reservoir;              // CHW_RESERVOIR_ERROR,
                                                  // CHW_RESERVOIR_EMPTY,
                                                  // CHW_RESERVOIR_MIDDLE,
                                                  // CHW_RESERVOIR_HIGH
                                             

   HW_VALVE_STATES        plasmaValve;            // HW_VALVE_UNKNOWN,
                                                  // HW_VALVE_COLLECT,
                                                  // HW_VALVE_OPEN,
                                                  // HW_VALVE_RETURN
   HW_VALVE_STATES        plateletValve;           
   HW_VALVE_STATES        rbcValve;                
                                             
// fan status bits added for 2.6 HW.  Not used in 2.4 HW.
   HW_ERROR               fanSense0;
   HW_ERROR               fanSense1;                 
   HW_ERROR               fanSense2;              // Not used in any HW
   HW_ERROR               tempError;

// power supply status bits added for 2.6 HW.  Not used in 2.4 HW.
   CHW_PS_OT_ERRORS       psTempWarning;

// sound level status bits added for 2.6 HW.  Not used in 2.4 HW.
   CHW_SOUND_LEVELS       soundLevel;

// test modes
   CHW_EVENTS             event;                  // CHW_NORMAL_UPDATE,
                                                  // CHW_AC_EVENT,
                                                  // CHW_DOOR_EVENT,
                                                  // CHW_PAUSE_EVENT,
                                                  // CHW_STOP_EVENT,
                                                  // CHW_RESERVOIR_EVENT

   long                   msgCount;               // accum message counter
   short                  lowAGC;                 // low AGC voltage
   short                  highAGC;                // high AGC voltage

};



const unsigned int TED_ControlHwStatusMsgSize = sizeof(TED_ControlHwStatusMsg);



//
// Safety HW Status Message
//
typedef struct TED_SafetyHwStatusMsg
{
   TED_Header          hdr;

// pumps
   long                inletRPM;             // 0 to ~250 rpm
   long                inletRevs;            // delta counts
   long                inletAccumRevs;       // total counts
                                           
   long                plateletRPM;          // 0 to ~250 rpm
   long                plateletRevs;         // delta counts
   long                plateletAccumRevs;    // total counts
                                           
   long                plasmaRPM;            // 0 to ~250 rpm
   long                plasmaRevs;           // delta counts
   long                plasmaAccumRevs;      // total counts
                                           
   long                acRPM;                // 0 to ~250 rpm
   long                acRevs;               // delta counts
   long                acAccumRevs;          // total counts
                                           
   long                returnRPM;            // ~-250 to ~250 rpm
   long                returnRevs;           // delta counts
   long                returnAccumRevs;      // total counts
   SHW_DIRECTION       returnDirection;      // SHW_RETURN, SHW_DRAW

// valve status    0 = HW_VALVE_UNKNOWN
//                 1 = HW_VALVE_COLLECT
//                 2 = HW_VALVE_OPEN
//                 3 = HW_VALVE_RETURN

   HW_VALVE_STATES     rbcValve;
   HW_VALVE_STATES     plasmaValve;
   HW_VALVE_STATES     plateletValve;

   unsigned char       valveLedTest;        // 1=ok, all others =error

// cassette position
   HW_CASSETTE_STATES  cassettePosition;    // HW_CASSETTE_UNKNOWN, HW_CASSETTE_DOWN,
                                             // HW_CASSETTE_UP

// ultrasonics
   SHW_RESERVOIR_STATES reservoir;           // 1=SHW_RESERVOIR_EMPTY, SHW_RESERVOIR_LOW

// centrifuge
   float               centrifugeRPM;       // RPM
   unsigned char       centrifugeError;     // HW_CENT_NO_FAULT, HW_CENT_HARDWARE_ERROR=1,
                                             // HW_CENT_INTEGRAL_ERROR=2

// misc
   HW_DOOR_STATES      doorLocks;           // HW_DOOR_OPEN, HW_DOOR_LOCKED_STATE,
                                             // HW_DOOR_CLOSED,
                                             // HW_DOOR_CLOSED_AND_LOCKED

   bool                pauseSwitch;         // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED
   bool                stopSwitch;          // HW_SWITCH_DISABLED, HW_SWITCH_ENABLED

   SHW_EVENTS          event;               // SHW_NORMAL_UPDATE, SHW_PAUSE_EVENT,     
                                             // SHW_STOP_EVENT, SHW_DOOR_EVENT,      
                                             // SHW_RESERVOIR_EVENT, SHW_RETURN_EVENT,    
                                             // SHW_AIR_EVENT

   long                msgCount;             // accum message counter

   unsigned short      returnPumpDirChgTime; // time from low level sensor
                                             // indicating air to return pump
                                             // in draw direction, milliseconds

// test modes
   long                test1;
   short               test2;

   HW_ORDERS           serviceEnableMode;    // HW_DISABLE=!servicemode,
                                             // HW_ENABLE=servicemode
   HW_ORDERS           donorConnectMode;     // HW_DISABLE=!donorconnect,
                                             // HW_ENABLE=donorconnect
};



const unsigned int TED_SafetyHwStatusMsgSize = sizeof(TED_SafetyHwStatusMsg);



//
// Control Hardware Order Message
//
typedef struct TED_ControlHwOrderMsg
{
   TED_Header          hdr;
   CHW_CASSETTE_ORDERS cassettePosition;       // CHW_CASSETTE_DOWN=1,
                                               // CHW_CASSETTE_UP=2

// centrifuge
   float               centrifugeRPM;          // 0 to 3100 RPM
                                               // -1 disables the motor controller
   float               centrifugeRampUpRate;   // RPM / sec 
   float               centrifugeRampDownRate; // RPM / sec

// misc
   HW_ORDERS           alarmLight;             // HW_DISABLE, HW_ENABLE=light on
   HW_ORDERS           doorLatches;            // HW_DISABLE, HW_ENABLE=power on
   CHW_DOOR_ORDERS     doorDirection;          // CHW_DOOR_UNLOCK, CHW_DOOR_LOCK

// pump RPM orders
   float               inletRPM;               // 0 - 250 RPM
   float               plateletRPM;            // 0 - 250 RPM
   float               plasmaRPM;              // 0 - 250 RPM
   float               acRPM;                  // 0 - 250 RPM
   float               returnRPM;              // -250 to 250 RPM

   CHW_VALVE_ORDERS    rbcValve;               // CHW_VALVE_COLLECT=1,
                                               // CHW_VALVE_OPEN=2,
                                               // CHW_VALVE_RETURN=3
   CHW_VALVE_ORDERS    plasmaValve;             
   CHW_VALVE_ORDERS    plateletValve;           

// rbc values
   unsigned char       redDrive;               // 0 to 255, typically 64
   unsigned char       greenDrive;             // 0 to 255, typically 128
 
// sound level added for 2.6 HW.  Not used in 2.4 & 2.2
   CHW_SOUND_LEVELS    soundLevel;             // CHW_SOUND_LOUD=0, CHW_SOUND_MEDIUM_LOUD,
                                               // CHW_SOUND_MEDIUM_SOFT, CHW_SOUND_SOFT

// test modes
   long                test1;                  // bit 0=1, openloop centrifuge control
                                               // bit 1=1, pump program test mode
// APS limits
   short               highAPSLimit;           // High access pressure limit
   short               lowAPSLimit;            // Low access pressure limit  
};



const unsigned int TED_ControlHwOrderMsgSize = sizeof(TED_ControlHwOrderMsg);



//
// Safety Hardware Order Message
//
typedef struct TED_SafetyHwOrderMsg
{
   TED_Header    hdr;
   HW_ORDERS     pumpPower;              // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS     centrifugePower;        // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS     valveLED;               // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS     doorSolenoidPower;      // HW_DISABLE=0, HW_ENABLE
   HW_ORDERS     alarmLight;             // HW_DISABLE=0, HW_ENABLE

// allow service mode and the safe executive to control air-to-donor
   HW_ORDERS     serviceEnableMode;      // HW_DISABLE=!servicemode,
                                         // HW_ENABLE=servicemode
   HW_ORDERS     donorConnectMode;       // HW_DISABLE=!donorconnect,
                                         // HW_ENABLE=donorconnect


// test modes
   long              test1;
   long              test2;

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
   TED_CURSOR_ORDERS order;
};



const unsigned int TED_MoveCursorMsgSize = sizeof(TED_MoveCursorMsg);



//
// Set Time Message
//
typedef struct TED_SetTimeMsg
{
   TED_Header hdr;
   time_t     time;        // Time in UTC
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
   int       x;
   int       y;
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


//
// Biometric Device List
//
#define TED_BIO_SIGNATURE_PIN  1


typedef struct TED_BroadcastMsg
{
   TED_Header   hdr;
   int          broadcastCount;                             // count of packets broadcasted
   char         trimaHWRev[TED_HW_REV_SIZE];                // Control & Safety Hdw Revisions
   char         trimaIP[TED_IP_ADDRESS_SIZE];               // 172.80.90.34
   char         trimaLogFile[TED_LOG_FILENAME_SIZE];        // T00015_0504_0033
   char         trimaSerialNumber[TED_SERIAL_NUMBER_SIZE];  // T00015
   char         trimaCRC[TED_CRC_SIZE];                     // 0xadcef73
   char         trimaRelease[TED_RELEASE_SIZE];             // Trima 3.3a
   char         trimaBuild[TED_BUILD_SIZE];                 // 1.191
   char         trimaPort[TED_PORT_NUMBER_SIZE];            // 7001
   char         sequenceNumber[TED_SEQUENCE_NUMBER_SIZE];   // 000000
   char         vipifVersionNumber[TED_VIPIF_VERSION_SIZE]; // $Revision: 1.1 $
   unsigned int biometricDeviceList;                        // Bit field containing Biometric ID's.
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
   float      rawValue;        // Raw value from cal.dat
   float      referenceValue;  // Auto zero value computed by proc
};

const unsigned int TED_ApsAutoZeroValueMsgSize = sizeof(TED_ApsAutoZeroValueMsg);



//
// TED Interface
//
class TedIf
{
public:
   TedIf();
   TedIf(TED_Header &hdr);

   virtual ~TedIf();

   // Validates a message
   TED_BUFFER_STATUS validHeader(const void *msg, unsigned long length);
   
   int findSOM(const void *msg, unsigned long length);

   // Prepares a message for sending by setting up the message header.
   void prepareMsg (void *msg, enum TED_MessageId messageId, unsigned long sizeInBytes = TED_HEADER_SIZE);

   const char * errorString() const { return _errorString; };

#ifndef WIN32
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_Header &hdr);
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_Header *hdr);
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_ControlHwStatusMsg &status);
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_BUFFER_STATUS &status);
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_Status &status);
   friend DataLog_Stream & operator << (DataLog_Stream &os, const TED_BoardType &type);
#endif

private:
   enum {TED_SOM = 0x000DECAF};

   char _errorString[128];
   TED_Header *_hdr;
};


#endif
