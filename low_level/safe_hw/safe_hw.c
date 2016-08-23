/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <intLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysLib.h>

#include "safe_hw_private.h"

enum { BASE = 0x0700 };     // I/O base address

// Read Only (RO) Safety FPGA Registers
enum { INP_VALVES                = BASE };        // valves
enum { INP_HALLS                 = BASE + 0x01 }; // valve & pump
enum { INPW_ULTRASONICS          = BASE + 0x02 }; // low level sensor
enum { INP_CENTRIFUGE            = BASE + 0x05 }; // centrifuge
enum { INP_SWITCHES              = BASE + 0x06 }; // switches
enum { INP_SAFE_HW_REVISION      = BASE + 0x07 }; // Legacy Safety FPGA revision register
enum { SAFETY_FPGA_CCA_REVISION  = BASE + 0x09 }; // Safety CCA Revision Register
enum { INP_BASIN_DATA            = BASE + 0x0A }; // basin microprocessor data
enum { SAFETY_FPGA_FW_REVISION   = BASE + 0x0D }; // Safety FPGA Firmware Revision Register
enum { SAFETY_FPGA_INTF_REVISION = BASE + 0x0E }; // Safety FPGA Interface Revision Register
enum { SAFETY_FPGA_ID_REVISION   = BASE + 0x0F }; // Safety FPGA ID Revision Register

// Read/Write (RW) Safety FPGA Registers
enum { IOP_POWER = BASE + 0x08 };           // power port

// Write Only (WO) Safety FPGA Registers
enum { OUTP_CLR_CENT_FAULT = BASE + 0x05 }; // clear centrifuge faults

// Write Only (WO) Registers for Petting Safety FPGA Watchdog
enum { WATCHDOG_F0_REG =   BASE + 0x00 };  // watchdog timer register 1
enum { WATCHDOG_A5_REG =   BASE + 0x01 };  // watchdog timer register 2

// Read Only (RO) Registers for Status of Safety FPGA Watchdog
enum { WATCHDOG_STATUS_REG     =     BASE + 0x0B };
enum { WATCHDOG_STATUS_CHK_REG = BASE + 0x0C };

// bit definitions for INP_VALVES
enum { NOT_CASSETTE_R_BIT = 0x00 };      // place holder
enum { NOT_CASSETTE_O_BIT = 0x80 };      // up
enum { NOT_CASSETTE_C_BIT = 0x40 };      // down
enum { NOT_RBC_O_BIT      =      0x20 }; // open
enum { NOT_RBC_C_BIT      =      0x10 }; // collect
enum { NOT_RBC_R_BIT      =      0x08 }; // return
enum { NOT_PLASMA_O_BIT   =   0x04 };    // open
enum { NOT_PLASMA_C_BIT   =   0x02 };    // collect
enum { NOT_PLASMA_R_BIT   =   0x01 };    // return

// bit definitions for INP_HALLS
enum { INLET_PUMP_HALL_BIT    =    0x80 };    // inlet tick
enum { PLATELET_PUMP_HALL_BIT = 0x40 };       // platelet tick
enum { PLASMA_PUMP_HALL_BIT   =   0x20 };     // plasma tick
enum { RETURN_PUMP_HALL_BIT   =   0x10 };     // return tick
enum { AC_PUMP_HALL_BIT       =       0x08 }; // ac tick
enum { NOT_PLATELET_O_BIT     =     0x04 };   // open
enum { NOT_PLATELET_C_BIT     =     0x02 };   // collect
enum { NOT_PLATELET_R_BIT     =     0x01 };   // return

// bit definitions for INP_SWITCHES
enum { CENT_REVERSE_BIT     =     0x80 };    // centrifuge reverse bit
enum { CENT_FAULT_BIT       =       0x40 };  // centrifuge fault bit
enum { NOT_POWER_FAIL_BIT   =   0x20 };      // power fail sense
enum { DOOR_CLOSED_BIT      =        0x10 }; // door closed bit
enum { NOT_STOP_SWITCH_BIT  =  0x08 };       // stop switch bit
enum { NOT_PAUSE_SWITCH_BIT = 0x04 };        // pause switch bit
enum { RETURN_DIR_BIT       =       0x02 };  // return pump direction
enum { DOOR_LOCKED_BIT      =        0x01 }; // door locked bit

// bit definitions for IOP_POWER
enum { ALARM_LIGHT_BIT = 0x20 };    // alarm light (off=0)
enum { DOOR_POWER_BIT  =  0x10 };   // door solenoid
enum { LED_POWER_BIT   =  0x04 };   // valve LED
enum { CENT_POWER_BIT  =  0x02 };   // 64v power
enum { PUMP_POWER_BIT  =  0x01 };   // 24v power

// bit definitions for INP_BASIN_DATA
enum { PLD_BIT               = 0x02 };    // power line dropout (only supported in newer FPGAs)
enum { BASIN_DATA_SERIAL_BIT = 0x01 };    // serial data input from basin uproc


//
//
// Safety FPGA WatchDog STATUS/STATUS_CHK Bits. The STATUS_CHK
// bits are the inverted value of the STATUS bits. This is used
// as an authenticity check to see if the system was was reset from
// a WatchDog Timeout or a Power-Fail/Power-Up condition. Any byte
// values other than those below are considered an indeterminant state
// caused by a Power-Fail/Power-Up condition.
//
//

// The WatchDog Timer has Started and No Errors in the
// Pet The WatchDog Sequence of 0xF0/0xA5 have been detected.
enum { WD_STATUS_NO_FAILURE     = 0xF0 };
enum { WD_STATUS_CHK_NO_FAILURE = 0x0F };

// The WatchDog Circuit has Timed-Out due to a byte
// other than the correct 0xF0 byte being written to
// the PET_WATCHDOG_F0 register.
enum { WD_STATUS_WRONG_F0_BYTE     = 0x33 };
enum { WD_STATUS_CHK_WRONG_F0_BYTE = 0xCC };

// The WatchDog Circuit has Timed-Out due to a byte
// other than the correct 0xA5 byte being written to
// the PET_WATCHDOG_A5 register.
enum { WD_STATUS_WRONG_A5_BYTE     = 0x55 };
enum { WD_STATUS_CHK_WRONG_A5_BYTE = 0xAA };

// The WatchDog Circuit has Timed-Out due to an incorrect
// Pet The WatchDog sequence. The PET_WATCHDOG_A5 register
// was written before the PET_WATCHDOG_F0 register.
enum { WD_STATUS_A5_WRT_WAITING_F0     = 0x17 };
enum { WD_STATUS_CHK_A5_WRT_WAITING_F0 = 0xE8 };

// The WatchDog Circuit has Timed Out due to an incorrect
// Pet The WatchDog Sequence. The PET_WATCHDOG_F0
// register was written twice.
enum { WD_STATUS_F0_WRT_WAITING_A5     = 0x69 };
enum { WD_STATUS_CHK_F0_WRT_WAITING_A5 = 0x96 };

// The correct Pet The WatchDog Sequence 0xF0/0xA5
// was not seen within ~420ms of the WatchDog being
// started.  This caused the WatchDog to Fail and reset
// the Proccessor. Processor Reset occurs 5 seconds
// after the Timeout.
enum { WD_STATUS_TIMEOUT     = 0x0F };
enum { WD_STATUS_CHK_TIMEOUT = 0xF0 };

// The FPGA ID REVISION Numbers designate whether the
// the FPGA is Control1, Control2, or Safety.
enum { SAFETY_FPGA_ID_REV = 0x06 };

// This the beginning Firmware Revision Number that is
// actually programmed into the Control1 & 2, and Safety FPGAs.
// It starts at Revision 3.
enum { SAFETY_FPGA_FIRST_FW_REV = 0x03 };

// This the beginning Interface Revision Number that is
// programmed into the Control1 & 2, and Safety FPGAs.
// It starts at Revision 3.
enum { SAFETY_FPGA_FIRST_INTF_REV = 0x03 };

enum { SAFETY_FPGA_ID_REV_INVALID = 0xFF };

static int                    initializationComplete   = 0;
static int                    initializationResult     = 0;

static unsigned short         powerFailRideThruEnabled = 0;

static unsigned short         fpgaVersion;
static unsigned short         boardVersion;
static unsigned short         hardwareVersion;

static unsigned char          currentPowerStatus = 0x20;
static unsigned short         powerStatusSave    = 0;

static hw_readbackFailedFunc* readbackFailedFunc = NULL;

// Safety FPGA Revision Numbers
static unsigned char safetyFpgaFwRevision;    // Safety FPGA Firmware Revision
static unsigned char safetyFpgaIntfRevision;  // Safety FPGA Interface Revision
static unsigned char safetyFpgaIdRevision;    // Safety FPGA ID Revision
static unsigned char safetyCcaRevision;       // Safety CCA Revision
static unsigned char safetyWatchDogStatus;    // WatchDog Status Register On Safety FPGA
static unsigned char safetyWatchDogStatusChk; // WatchDog Status Check Register On Safety FPGA

static int           mxFpgaInstalled = 0;

static void testInitialized (const char* funcName);
static void testReadbackByte (const char* file, int line, unsigned short port, unsigned short expected);
static void testReadbackWord (const char* file, int line, unsigned short port, unsigned short expected);


//// COMMON HARDWARE DRIVER FUNCTIONS (common_hw.h) ////

int hw_init (void)
{
   if ( !initializationComplete )
   {
      // Get the Safety FPGA ID Revision
      safetyFpgaIdRevision = sysInByte(SAFETY_FPGA_ID_REVISION);

      switch (safetyFpgaIdRevision)
      {
         case SAFETY_FPGA_ID_REV :

            // Get the Safety FPGA Firmware Revision Value
            safetyFpgaFwRevision = sysInByte(SAFETY_FPGA_FW_REVISION);

            // This must be REV 3 or greater
            if (safetyFpgaFwRevision >= SAFETY_FPGA_FIRST_FW_REV)
            {

               // Get the rest of Safety FPGA Revision Values
               safetyFpgaIntfRevision = sysInByte(SAFETY_FPGA_INTF_REVISION);

               safetyCcaRevision      = sysInByte(SAFETY_FPGA_CCA_REVISION);

               mxFpgaInstalled        = 1;

               initializationResult   = 1;
            }
            else
            {
               printf("Unrecognized Safety FPGA Firmware Revision %d\n", safetyFpgaFwRevision);
               abort();
            }

            break;

         default :
            fpgaVersion     = 0x0000;
            boardVersion    = 0x0000;
            hardwareVersion = 0x0000;

            sysOutByte(INP_SAFE_HW_REVISION, 0);
            sysDelay();

            fpgaVersion = sysInByte(INP_SAFE_HW_REVISION);

            switch (fpgaVersion)
            {
               case 3 : // first production release
               case 4 : // modified to hold processor reset line upon fan failure
                  hardwareVersion      = 0x0206;
                  initializationResult = 1;
                  break;

               default :
                  printf("Unrecognized FPGA version %d\n", fpgaVersion);
                  abort();
            }

            break;
      }

      initializationComplete = 1;
   }

   return initializationResult;
}

void hw_forceReset (void)
{
   // Lock in this execution thread - the system will be stuck here until
   // the watchdog generates a hardware reset
   intLock();

   // Safe state then enable the LED power for a short period (less than
   // 10 micro-seconds) to arm the hardware watchdog
   hw_safeState();
   shw_powerEnable(shw_ledPower);
   shw_powerDisable(shw_ledPower);

   // Wait for reset
   while (1)
      ;
}

void hw_safeState (void)
{
   shw_powerSet(shw_ledPower);
   hw_alarmSetCommand(hw_alarmOn);
}


////                                  ////
////                                  ////
//// NEW (MX) FPGA REVISION FUNCTIONS ////
////                                  ////
////                                  ////

int hw_mxFpgaInstalled (void)
{
   return mxFpgaInstalled;
}

HWBoardType hw_getBoardType (void)
{
   return hw_safetyBoard;
}

unsigned char hw_fpgaFwRevision (HWFpgaType fpga)
{
   unsigned char fpgaFwRevision = 0x00;

   switch (fpga)
   {
      case hw_safetyFpga :
         fpgaFwRevision = safetyFpgaFwRevision;
         break;

      default :
         break;
   }

   return fpgaFwRevision;
}

unsigned char hw_fpgaIdRevision (HWFpgaType fpga)
{

   unsigned char fpgaIdRevision = 0x00;

   switch (fpga)
   {
      case hw_safetyFpga :
         fpgaIdRevision = safetyFpgaIdRevision;
         break;

      default :
         break;
   }

   return fpgaIdRevision;
}

unsigned char hw_fpgaIntfRevision (HWFpgaType fpga)
{
   unsigned char fpgaIntfRevision = 0x00;

   switch (fpga)
   {
      case hw_safetyFpga :
         fpgaIntfRevision = safetyFpgaIntfRevision;
         break;

      default :
         break;
   }

   return fpgaIntfRevision;
}

unsigned char hw_ccaRevision (void)
{
   return safetyCcaRevision;
}


HWWatchDogStatus hw_watchDogStatus (void)
{
   HWWatchDogStatus wdStatus;

   safetyWatchDogStatus    = sysInByte(WATCHDOG_STATUS_REG);

   safetyWatchDogStatusChk = sysInByte(WATCHDOG_STATUS_CHK_REG);


   if (safetyWatchDogStatus    == WD_STATUS_NO_FAILURE &&
       safetyWatchDogStatusChk == WD_STATUS_CHK_NO_FAILURE)
   {
      wdStatus = hw_noFailure;
   }
   else if (safetyWatchDogStatus    == WD_STATUS_WRONG_F0_BYTE &&
            safetyWatchDogStatusChk == WD_STATUS_CHK_WRONG_F0_BYTE)
   {
      wdStatus = hw_wrongF0Byte;
   }
   else if (safetyWatchDogStatus    == WD_STATUS_WRONG_A5_BYTE &&
            safetyWatchDogStatusChk == WD_STATUS_CHK_WRONG_A5_BYTE)
   {
      wdStatus = hw_wrongA5Byte;
   }
   else if (safetyWatchDogStatus    == WD_STATUS_A5_WRT_WAITING_F0 &&
            safetyWatchDogStatusChk == WD_STATUS_CHK_A5_WRT_WAITING_F0)
   {
      wdStatus = hw_a5WriteWaitingF0;
   }
   else if (safetyWatchDogStatus    == WD_STATUS_F0_WRT_WAITING_A5 &&
            safetyWatchDogStatusChk == WD_STATUS_CHK_F0_WRT_WAITING_A5)
   {
      wdStatus = hw_f0WriteWaitingA5;
   }
   else if (safetyWatchDogStatus    == WD_STATUS_TIMEOUT &&
            safetyWatchDogStatusChk == WD_STATUS_CHK_TIMEOUT)
   {
      wdStatus = hw_wdTimeOut;
   }
   else
   {
      wdStatus = hw_powerUp;
   }

   return wdStatus;
}


////                                       ////
////                                       ////
//// LEGACY SAFETY FPGA REVISION FUNCTIONS ////
////                                       ////
////                                       ////

unsigned short hw_boardVersion (void)
{
   testInitialized("hw_boardVersion");
   return boardVersion;
}

unsigned short hw_fpgaVersion (void)
{
   testInitialized("hw_fpgaVersion");
   return fpgaVersion;
}

unsigned short hw_hardwareVersion (void)
{
   testInitialized("hw_hardwareVersion");
   return hardwareVersion;
}

unsigned short hw_switchGetStatus (HWSwitch select)
{
   unsigned char  value  = sysInByte(INP_SWITCHES);
   unsigned short result = 0;

   switch ( select )
   {
      case hw_stopSwitch :
         result = (value & NOT_STOP_SWITCH_BIT) ? 0 : hw_switchPressed;
         break;

      case hw_pauseSwitch :
         result = (value & NOT_PAUSE_SWITCH_BIT) ? 0 : hw_switchPressed;
         break;
   }

   return result;
}


//// COMMON/PRIVATE HARDWARE DRIVER FUNCTIONS (common_hw_private.h) ////

void hw_setReadbackFailedFunc (hw_readbackFailedFunc* func)
{
   readbackFailedFunc = func;
}

void hw_alarmSetCommand (HWAlarmCommand command)
{
   unsigned char portValue = sysInByte(IOP_POWER);

   switch ( command )
   {
      case hw_alarmOn :
         portValue &= ~ALARM_LIGHT_BIT;
         break;

      case hw_alarmOff :
         portValue |= ALARM_LIGHT_BIT;
         break;
   }

   sysOutByte(IOP_POWER, portValue);
   testReadbackByte(__FILE__, __LINE__, IOP_POWER, portValue);
}

unsigned short hw_centGetCommutationCount (void)
{
   return sysInByte(INP_CENTRIFUGE);
}

unsigned short hw_centGetStatus (void)
{
   unsigned short status    = 0;
   unsigned char  portValue = sysInByte(INP_SWITCHES);

   if (portValue & CENT_FAULT_BIT)
      status |= hw_centFault;

   if (portValue & CENT_REVERSE_BIT)
      status |= hw_centReverse;

   return status;
}

unsigned short hw_doorGetStatus (void)
{
   unsigned char  portValue = sysInByte(INP_SWITCHES);
   unsigned short result    = 0;

   if (!(portValue & DOOR_CLOSED_BIT))
      result |= hw_doorClosed;

   if (!(portValue & DOOR_LOCKED_BIT))
      result |= hw_doorLocked;

   return result;
}

void hw_powerFailEnableRideThru (void)
{
   powerFailRideThruEnabled = 1;
}

void hw_powerFailDisableRideThru (void)
{
   powerFailRideThruEnabled = 0;
}

unsigned short hw_powerFailGetStatus (void)
{
   unsigned short retVal = 0;
   retVal |= (sysInByte(INP_SWITCHES) & NOT_POWER_FAIL_BIT) ? 0 : hw_powerFailWarning;

   if ( powerFailRideThruEnabled )
   {
      retVal |= (sysInByte(INP_BASIN_DATA) | PLD_BIT) ? 0 : hw_powerFailLineDrop;
   }
   else
   {
      retVal |= hw_powerFailLineDrop;
   }

   return retVal;
}

void hw_powerFailSave (void)
{
   powerStatusSave += 1;
   sysOutByte(IOP_POWER, sysInByte(IOP_POWER) & (ALARM_LIGHT_BIT | LED_POWER_BIT));

   return;
}

void hw_powerFailRestore (void)
{
   if ( powerStatusSave > 0 )
   {
      powerStatusSave -= 1;

      if ( powerStatusSave == 0 )
      {
         unsigned char portValue = (sysInByte(IOP_POWER) & 0x20) | currentPowerStatus;
         sysOutByte(IOP_POWER, portValue);
         testReadbackByte(__FILE__, __LINE__, IOP_POWER, portValue);
      }
   }

   return;
}

HWValvePosition hw_valveGetPosition (HWValve valve)
{
   HWValvePosition result;
   unsigned char   posValue = 0;

   switch ( valve )
   {
      case hw_rbcValve :
         posValue = (sysInByte(INP_VALVES) & 0x38) >> 3;
         break;

      case hw_plateletValve :
         posValue = sysInByte(INP_HALLS) & 0x07;
         break;

      case hw_plasmaValve :
         posValue = sysInByte(INP_VALVES) & 0x07;
         break;

      case hw_cassette :
         posValue = ((sysInByte(INP_VALVES) & 0xc0) >> 5) | 0x01;
         break;

      default :
         break;
   }

   switch ( posValue )
   {
      case 0x03 :
         result = hw_valveOpen;
         break;

      case 0x05 :
         result = hw_valveCollect;
         break;

      case 0x06 :
         result = hw_valveReturn;
         break;

      case 0x07 :
         result = hw_valveUnknown;
         break;

      default :
         result = hw_valveIllegal;
         break;
   }

   return result;
}

void hw_watchdogUpdate (void)
{
   static unsigned char state = 0;

   if (state & 0x01)
   {
      sysOutByte(WATCHDOG_A5_REG, 0xA5);
   }
   else
   {
      sysOutByte(WATCHDOG_F0_REG, 0xF0);
   }

   state = ~state;
}

//// SAFETY/PRIVATE HARDWARE DRIVER FUNCTIONS (safety_hw_private.h) ////

unsigned short shw_getByteSourceData (SafeHWGetByteSource select)
{
   unsigned short result = 0;

   if ( select == shw_basinDataSource )
   {
      // this is a 16bit read to work around a memory map
      // problem in the safety FPGA.  Changing this to an
      // 8-bit read will result in unreliable ultrasonics
      // data for revision 4 of the FPGA.  This problem
      // will be fixed in revision 5.
      result = (sysInWord(INP_BASIN_DATA) & BASIN_DATA_SERIAL_BIT) ? 1 : 0;
   }

   return result;
}

void shw_centClearFault (void)
{
   sysOutByte(OUTP_CLR_CENT_FAULT, 1);
}

unsigned short shw_hallGetStatus (HWPump select)
{
   unsigned char  portValue = sysInByte(INP_HALLS);
   unsigned short result    = 0;

   switch ( select )
   {
      case hw_inletPump :
         result = (portValue & INLET_PUMP_HALL_BIT) ? 1 : 0;
         break;

      case hw_plateletPump :
         result = (portValue & PLATELET_PUMP_HALL_BIT) ? 1 : 0;
         break;

      case hw_plasmaPump :
         result = (portValue & PLASMA_PUMP_HALL_BIT) ? 1 : 0;
         break;

      case hw_returnPump :
         result = (portValue & RETURN_PUMP_HALL_BIT) ? 1 : 0;
         break;

      case hw_acPump :
         result = (portValue & AC_PUMP_HALL_BIT) ? 1 : 0;
         break;

      default :
         printf("Unrecognized pump %d\n", (int)select);
         abort();
   }

   return result;
}

unsigned short shw_ledTest (unsigned short* rawStatus)
{
   // LED sensors are in bits 0-7 for INP_VALVES, bits 0-2 for INP_HALLS and the
   // DOOR_LOCKED_BIT and DOOR_CLOSED_BIT for INP_SWITCHES
   //
   *rawStatus  = sysInByte(INP_VALVES);
   *rawStatus |= (sysInByte(INP_HALLS) & 0x07) << 8;
   *rawStatus |= (sysInByte(INP_SWITCHES) & DOOR_LOCKED_BIT) ? (1 << 11) : 0;
   *rawStatus |= (sysInByte(INP_SWITCHES) & DOOR_CLOSED_BIT) ? (1 << 12) : 0;

   return (*rawStatus == 0x1fff) ? 1 : 0;
}

unsigned short shw_powerStatus (void)
{
   unsigned short result = 0;

   result |= (currentPowerStatus & DOOR_POWER_BIT) ? shw_doorPower : 0;
   result |= (currentPowerStatus & LED_POWER_BIT) ? shw_ledPower : 0;
   result |= (currentPowerStatus & CENT_POWER_BIT) ? shw_centPower : 0;
   result |= (currentPowerStatus & PUMP_POWER_BIT) ? shw_pumpPower : 0;

   return result;
}

void shw_powerEnable (unsigned short mask)
{
   currentPowerStatus |= (mask & shw_doorPower) ? DOOR_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_ledPower) ? LED_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_centPower) ? CENT_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_pumpPower) ? PUMP_POWER_BIT : 0;

   if ( !powerStatusSave )
   {
      unsigned char portValue = (sysInByte(IOP_POWER) & ALARM_LIGHT_BIT) | currentPowerStatus;
      sysOutByte(IOP_POWER, portValue);
      testReadbackByte(__FILE__, __LINE__, IOP_POWER, portValue);
   }
}

void shw_powerDisable (unsigned short mask)
{
   currentPowerStatus &= (mask & shw_doorPower) ? ~DOOR_POWER_BIT : 0xff;
   currentPowerStatus &= (mask & shw_ledPower) ? ~LED_POWER_BIT : 0xff;
   currentPowerStatus &= (mask & shw_centPower) ? ~CENT_POWER_BIT : 0xff;
   currentPowerStatus &= (mask & shw_pumpPower) ? ~PUMP_POWER_BIT : 0xff;

   if ( !powerStatusSave )
   {
      unsigned char portValue = (sysInByte(IOP_POWER) & ALARM_LIGHT_BIT) | currentPowerStatus;
      sysOutByte(IOP_POWER, portValue);
      testReadbackByte(__FILE__, __LINE__, IOP_POWER, portValue);
   }
}

void shw_powerSet (unsigned short mask)
{
   currentPowerStatus  = 0;
   currentPowerStatus |= (mask & shw_doorPower) ? DOOR_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_ledPower) ? LED_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_centPower) ? CENT_POWER_BIT : 0;
   currentPowerStatus |= (mask & shw_pumpPower) ? PUMP_POWER_BIT : 0;

   if ( !powerStatusSave )
   {
      unsigned char portValue = (sysInByte(IOP_POWER) & ALARM_LIGHT_BIT) | currentPowerStatus;
      sysOutByte(IOP_POWER, portValue);
      testReadbackByte(__FILE__, __LINE__, IOP_POWER, portValue);
   }
}

unsigned short shw_pumpDirection (HWPump select)
{
   return (sysInByte(INP_SWITCHES) & RETURN_DIR_BIT) ? shw_pumpDraw : shw_pumpReturn;
}

unsigned short shw_ultrasonicSensorGetCounter (void)
{
   return sysInWord(INPW_ULTRASONICS);
}

static void testInitialized (const char* funcName)
{
   if ( !initializationResult )
   {
      printf("HW driver must be initialized to access function \"%s\"\n", funcName);
      abort();
   }
}

static void testReadbackByte (const char* file, int line, unsigned short port, unsigned short expected)
{
   unsigned short value = sysInByte(port);

   if ( value != expected &&
        readbackFailedFunc != NULL )
   {
      (* readbackFailedFunc)(file, line, "byte", port, value, expected);
   }
}

static void testReadbackWord (const char* file, int line, unsigned short port, unsigned short expected)
{
   unsigned short value = sysInWord(port);

   if ( value != expected &&
        readbackFailedFunc != NULL )
   {
      (* readbackFailedFunc)(file, line, "word", port, value, expected);
   }
}

/* FORMAT HASH 227f0e429fb95483610a839c8f197840 */
