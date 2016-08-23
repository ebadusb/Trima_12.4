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
#include <taskLib.h>

#include "ctl_hw_private.h"


//
// CONTROL1 FPGA Register Map
//

// I/O base address for Control1 FPGA
enum { C1_BASE = 0x0B00 };

// Read/Write (RW) ports for Control1 FPGA production hardware
enum { C1_PWR_SUPPLY    =     C1_BASE + 0xB };
enum { IOP_INLET_PWM    =     C1_BASE + 0xC };
enum { IOP_PLATELET_PWM =  C1_BASE + 0xD };
enum { IOP_PLASMA_PWM   =    C1_BASE + 0xE };
enum { IOP_AC_PWM       =        C1_BASE + 0xF };
enum { IOP_RETURN_PWM   =    C1_BASE + 0x10 };
enum { IOP_SOUND_LEVEL  =   C1_BASE + 0x11 };

// Read Only (RO) Ports for Control1 FPGA production hardware
enum { INPW_INLET_ENCODER    =    C1_BASE + 0x0 };
enum { INPW_PLATELET_ENCODER = C1_BASE + 0x2 };
enum { INPW_PLASMA_ENCODER   =   C1_BASE + 0x4 };
enum { INPW_AC_ENCODER       =       C1_BASE + 0x6 };
enum { INPW_RETURN_ENCODER   =   C1_BASE + 0x8 };
enum { INPW_CTL_HW_REVISION  =  C1_BASE + 0xA };
enum { C1_FPGA_FW_REVISION   =   C1_BASE + 0x12 };
enum { C1_FPGA_INTF_REVISION = C1_BASE + 0x13 };
enum { C1_FPGA_ID_REVISION   =   C1_BASE + 0x17 };
enum { INP_BUTTONS           =           C1_BASE + 0x14 };
enum { INP_VALVES            =            C1_BASE + 0x15 };
enum { INP_POWER_STATUS      =      C1_BASE + 0x16 };


//
// CONTROL2 FPGA Register Map
//

// I/O base address for Control2 FPGA
enum { C2_BASE = 0x0F00 };

// Read/Write (RW) Ports for Control2 FPGA production hardware
enum { IOPW_CENT_SPEED =   C2_BASE + 0x8 };
enum { IOP_ADC_MUX     =       C2_BASE + 0xA };
enum { IOP_VALVE       =         C2_BASE + 0xB };
enum { IOP_ALARM       =         C2_BASE + 0xC };
enum { IOP_DOOR        =          C2_BASE + 0xD };
enum { IOP_PUMP_ENABLE =   C2_BASE + 0x0E };
enum { IOP_CENT_ENABLE =   C2_BASE + 0x0F };
enum { UNBLANK_DISPLAY =   C2_BASE + 0x12 };

// Write Only (WO) Registers for Petting Control2 FPGA Watchdog
enum { WATCHDOG_F0_REG =   C2_BASE + 0x10 };
enum { WATCHDOG_A5_REG =   C2_BASE + 0x11 };

// Read Only (RO) Registers for Status of Control2 FPGA Watchdog
enum { WATCHDOG_STATUS_REG     =     C2_BASE + 0x10 };
enum { WATCHDOG_STATUS_CHK_REG = C2_BASE + 0x11 };

// Read Only (RO) Ports for Control2 FPGA production hardware
enum { INPW_US_HIGH          =            C2_BASE + 0x0 };
enum { INPW_US_LOW           =             C2_BASE + 0x2 };
enum { INP_CENT_COMM         =           C2_BASE + 0x4 };
enum { INP_ODDS_AND_ENDS     =       C2_BASE + 0x5 };
enum { INPW_ADC_VALUE        =          C2_BASE + 0x6 };
enum { C2_FPGA_FW_REVISION   =     C2_BASE + 0x12 };
enum { C2_FPGA_INTF_REVISION =   C2_BASE + 0x13 };
enum { C2_FPGA_CCA_REVISION  =    C2_BASE + 0x16 };
enum { C2_FPGA_ID_REVISION   =     C2_BASE + 0x17 };


//
// CONTROL3 FPGA Register Map
//

// I/O base address for Control3 FPGA
enum { C3_BASE               = 0x0700 };

enum { C3_FPGA_ID_REVISION   = C3_BASE + 0x80 };
enum { C3_FPGA_INTF_REVISION = C3_BASE + 0x82 };



//
// bit definitions for C1_PWR_SUPPLY
// production hardware Control1
//
enum { OVER_VOLT_PROTECT_TEST_RESULT = 0x10 };
enum { OVER_VOLT_PROTECT_TEST        = 0x08 };
enum { DISABLE_SUPPLY_OUTPUTS        = 0x04 };
enum { SEAL_SAFE_OVER_TEMP_WARN      = 0x02 };
enum { SEAL_SAFE_CURRENT_OK          = 0x01 };

//
// value definitions for INP_BUTTONS
// production hardware
//
enum { NOT_PLASMA_O_BIT     =     0x80 };
enum { NOT_PLASMA_C_BIT     =     0x40 };
enum { NOT_PLASMA_R_BIT     =     0x20 };
enum { NOT_MOTOR_FAULT_BIT  =  0x10 };
enum { NOT_VALVE_FAULT_BIT  =  0x08 };
enum { NOT_CENT_FAULT_BIT   =   0x04 };
enum { NOT_STOP_SWITCH_BIT  =  0x02 };
enum { NOT_PAUSE_SWITCH_BIT = 0x01 };

//
// bit definitions for INP_VALVES
// production hardware (2.6)
//
enum { NOT_PLATELET_O_BIT = 0x80 };
enum { NOT_PLATELET_C_BIT = 0x40 };
enum { NOT_PLATELET_R_BIT = 0x20 };
enum { NOT_CASSETTE_O_BIT = 0x10 };     // up
enum { NOT_CASSETTE_C_BIT = 0x08 };     // down
enum { NOT_RBC_O_BIT      =      0x04 };
enum { NOT_RBC_C_BIT      =      0x02 };
enum { NOT_RBC_R_BIT      =      0x01 };
enum { NOT_CASSETTE_R_BIT = 0x00 };     // place holder


//
// bit definitions for INP_POWER_STATUS
// production hardware
//
enum { NOT_FAN_SENSE0_BIT = 0x80 };
enum { NOT_FAN_SENSE1_BIT = 0x40 };
enum { NOT_FAN_SENSE2_BIT = 0x20 };
enum { NOT_BUF_SS_OTW_BIT = 0x08 };
enum { NOT_BUF_PS_OTW_BIT = 0x04 };
enum { NOT_BUF_PFW_BIT    =    0x02 };
enum { NOT_BUF_DC_OK_BIT  =  0x01 };


//
// bit definitions for INP_ODDS_AND_ENDS
// production hardware
//
enum { NOT_SOLENOID_FAULT_BIT =  0x80 };
enum { DOOR_UNLOCKED_BIT      =       0x40 };
enum { AC_GONE_BIT            =             0x20 };
enum { NOT_DOOR_OPEN_BIT      =       0x10 };
enum { CENT_COMM_FAULT_BIT    =     0x04 };
enum { CENT_REVERSE_BIT       =        0x02 };
enum { ADC_BUSY_BIT           =            0x01 };


//
// bit definitions for IOP_ADC_MUX
// production hardware
//
enum { ADC_START_BIT = 0x80 };


//
// bit definitions for IOP_VALVE
// production hardware
//
enum { VALVE_RUN_BIT             =             0x20 };  // run valve bit
enum { VALVE_SELECT_CASSETTE_BIT = 0x10 };              // select cassette
enum { VALVE_SELECT_RBC_BIT      =      0x08 };         // select rbc valve
enum { VALVE_SELECT_PLASMA_BIT   =   0x04 };            // select plasma valve
enum { VALVE_SELECT_PLATELET_BIT = 0x02 };              // select platelet valve
enum { VALVE_CCW_BIT             =             0x01 };  // move valve CCW
enum { VALVE_CW_BIT              =              0x00 }; // move valve clockwise


//
// bit definitions for IOP_ALARM
// production hardware
//
enum { LEAK_DETECTOR_BIT   = 0x02 };
enum { NOT_ALARM_LIGHT_BIT =   0x01 };


//
// bit definitions for IOP_DOOR
// production hardware
//
enum { NOT_DOOR_SOLENOID_BIT = 0x02 };
enum { DOOR_DIRECTION_BIT    =    0x01 };


//
// bit definitions for IOP_PUMP_ENABLE
// production hardware
//
enum { PUMP_ENABLE_BIT = 0x02 };
enum { RETURN_DIR_BIT  =  0x01 };


//
// bit definitions for IOP_CENT_ENABLE
// production hardware
//
enum { CENT_CLEAR_COMM_FLAG   =   0x04 };  // write
enum { READ_DISPLAY_BLANK_BIT = 0x02 };    // read
enum { CENT_ENABLE_BIT        =        0x01 };


//
// Correct Byte Definitions that are to be written to
// PET_WATCHDOG_F0 and PET_WATCHDOG_A5 registers in the
// Pet The WatchDog Sequence of 0xF0/0xA5. production hardware.
//
enum { WATCHDOG_F0_BYTE = 0xF0 };
enum { WATCHDOG_A5_BYTE = 0xA5 };


//
//
// Control2 FPGA WatchDog STATUS/STATUS_CHK Bits. The STATUS_CHK
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
// the FPGA is Control1 or Control2
enum { CONTROL1_FPGA_ID_REV = 0x04 };
enum { CONTROL2_FPGA_ID_REV = 0x05 };

// These are the beginning Firmware Revision Numbers that are
// actually programmed into the Control1 & 2 FPGAs. They both
// start at Revision 3.
enum { CONTROL1_FPGA_FIRST_FW_REV = 0x03 };
enum { CONTROL2_FPGA_FIRST_FW_REV = 0x03 };

// This the beginning Interface Revision Number that is
// programmed into the Control1 & 2, and Safety FPGAs.
// It starts at Revision 3.
enum { CONTROL1_FPGA_FIRST_INTF_REV = 0x03 };
enum { CONTROL2_FPGA_FIRST_INTF_REV = 0x03 };

enum { CONTROL1_FPGA_ID_REV_INVALID = 0xFF };

//
// bit definitions for OUTP_BLANK_DISPLAY
// production hardware (2.6)
//
enum { WRITE_BLANK_DISPLAY_BIT = 0x02 };

static int            initializationComplete   = 0;
static int            initializationResult     = 0;

static unsigned short powerFailRideThruEnabled = 0;

static int            mxFpgaInstalled          = 0;
static unsigned short hasArtesynPs             = 0;

// For backwards compatability with old FPGA versions (2.6)
static unsigned short fpgaVersion;
static unsigned short boardVersion;
static unsigned short hardwareVersion;

// Control1 FPGA Revision Numbers
static unsigned char c1FpgaFwRevision;   // Control1 FPGA Firmware Revision
static unsigned char c1FpgaIntfRevision; // Control1 FPGA Interface Revision
static unsigned char c1FpgaIdRevision;   // Control1 FPGA ID Revision

// Control2 FPGA Revision Numbers
static unsigned char          c2FpgaFwRevision;    // Control2 FPGA Firmware Revision
static unsigned char          c2FpgaIntfRevision;  // Control2 FPGA Interface Revision
static unsigned char          c2FpgaIdRevision;    // Control2 FPGA ID Revision
static unsigned char          controlCcaRevision;  // Control CCA Revision Routed to Control2 FPGA
static unsigned char          c2WatchDogStatus;    // WatchDog Status Register On Control2 FPGA
static unsigned char          c2WatchDogStatusChk; // WatchDog Status Check Register On Control2 FPGA

static hw_readbackFailedFunc* readbackFailedFunc = NULL;

static void testInitialized (const char* funcName);
static void testReadbackByte (const char* file, int line, unsigned short port, unsigned short expected);
static void testReadbackWord (const char* file, int line, unsigned short port, unsigned short expected);


//// COMMON HARDWARE DRIVER FUNCTIONS (common_hw.h) ////

int hw_init (void)
{
   if (!initializationComplete)
   {
      hasArtesynPs = 0;

      // Get the Control1 FPGA ID Revision
      c1FpgaIdRevision = sysInByte(C1_FPGA_ID_REVISION);

      switch (c1FpgaIdRevision)
      {

         //// NEW (MX) FPGA INITIALIZATION ////

         case CONTROL1_FPGA_ID_REV :

            // Get the Control1 FPGA Firmware Revision Values
            c1FpgaFwRevision = sysInByte(C1_FPGA_FW_REVISION);

            // Get the Control2 FPGA Firmware Revision Values
            c2FpgaFwRevision = sysInByte(C2_FPGA_FW_REVISION);

            // These both must be REV 3 or greater
            if (c1FpgaFwRevision >= CONTROL1_FPGA_FIRST_FW_REV
                &&
                c2FpgaFwRevision >= CONTROL2_FPGA_FIRST_FW_REV)
            {

               // Get the rest of Control1 FPGA Revision Values
               c1FpgaIntfRevision = sysInByte(C1_FPGA_INTF_REVISION);

               // Get the rest of Control1 FPGA Revision Values
               c2FpgaIntfRevision   = sysInByte(C2_FPGA_INTF_REVISION);

               c2FpgaIdRevision     = sysInByte(C2_FPGA_ID_REVISION);

               controlCcaRevision   = sysInByte(C2_FPGA_CCA_REVISION);

               hasArtesynPs         = (sysInByte(C1_PWR_SUPPLY) != 0xF);

               mxFpgaInstalled      = 1;

               initializationResult = 1;
            }
            else
            {

               printf("Unrecognized Control1 FPGA Firmware Revision %d\n", c1FpgaFwRevision);

               printf("Unrecognized Control2 FPGA Firmware Revision %d\n", c2FpgaFwRevision);

               abort();
            }

            break;

         default :

            //// LEGACY FPGA INITIALIZATION ////

            fpgaVersion     = 0x0000;
            boardVersion    = 0x0000;
            hardwareVersion = 0x0000;

            sysOutByte(INPW_CTL_HW_REVISION, 0);

            sysDelay();

            fpgaVersion = sysInByte(INPW_CTL_HW_REVISION);

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

   // Set all commands to zero, then enable the pumps for a short period (less than
   // 10 micro-seconds) to arm the hardware watchdog
   hw_safeState();
   chw_pumpEnable();
   chw_pumpDisable();

   // Wait for reset
   while (1)
      ;
}

void hw_safeState (void)
{
   chw_pumpDisable();
   chw_pumpSetCommand(hw_allPumps, 0);
   chw_centSetCommand(0);
   chw_valveSetCommand(hw_allValves, chw_stopValve);
   chw_doorLockSetCommand(chw_doorLockOff);
   hw_alarmSetCommand(hw_alarmOn);
}


////                                  ////
////                                  ////
//// NEW (MX) FPGA REVISION FUNCTIONS ////
////                                  ////
////                                  ////


//
// Check for New MX Technology FPGAs
//
int hw_mxFpgaInstalled (void)
{

   return mxFpgaInstalled;
}

//
// Get CCA Type Control or Safety
//
HWBoardType hw_getBoardType (void)
{

   return hw_controlBoard;
}

//
// Get Firmware Revision from Control1 or 2 FPGA
//
unsigned char hw_fpgaFwRevision (HWFpgaType fpga)
{

   unsigned char fpgaFwRevision = 0x00;

   testInitialized("hw_fpgaFwRevision");

   switch (fpga)
   {

      case hw_control1Fpga :

         fpgaFwRevision = c1FpgaFwRevision;

         break;

      case hw_control2Fpga :

         fpgaFwRevision = c2FpgaFwRevision;

         break;

      default :
         break;
   }

   return fpgaFwRevision;
}

//
// Get ID Revision from Control1 or 2 FPGA
//
unsigned char hw_fpgaIdRevision (HWFpgaType fpga)
{

   unsigned char fpgaIdRevision = 0x00;

   testInitialized("hw_fpgaIdRevsion");

   switch (fpga)
   {

      case hw_control1Fpga :

         fpgaIdRevision = c1FpgaIdRevision;

         break;

      case hw_control2Fpga :

         fpgaIdRevision = c2FpgaIdRevision;

         break;

      default :
         break;
   }

   return fpgaIdRevision;
}

//
// Get Interface Revision From Control1 or 2 FPGA
//
unsigned char hw_fpgaIntfRevision (HWFpgaType fpga)
{

   unsigned char fpgaIntfRevision = 0x00;

   testInitialized("hw_fpgaIntfRevsion");

   switch (fpga)
   {

      case hw_control1Fpga :

         fpgaIntfRevision = c1FpgaIntfRevision;

         break;

      case hw_control2Fpga :

         fpgaIntfRevision = c2FpgaIntfRevision;

         break;

      default :
         break;
   }

   return fpgaIntfRevision;
}

//
// Get CCA Revision From Control2 FPGA
//
unsigned char hw_ccaRevision (void)
{

   testInitialized("hw_ccaRevision");

   return controlCcaRevision;
}

//
// Get WatchDog Status from Control2 FPGA
//
HWWatchDogStatus hw_watchDogStatus (void)
{

   HWWatchDogStatus wdStatus;

   c2WatchDogStatus    = sysInByte(WATCHDOG_STATUS_REG);

   c2WatchDogStatusChk = sysInByte(WATCHDOG_STATUS_CHK_REG);

   if (c2WatchDogStatus == WD_STATUS_NO_FAILURE
       &&
       c2WatchDogStatusChk == WD_STATUS_CHK_NO_FAILURE)
   {

      wdStatus = hw_noFailure;
   }
   else if (c2WatchDogStatus == WD_STATUS_WRONG_F0_BYTE
            &&
            c2WatchDogStatusChk == WD_STATUS_CHK_WRONG_F0_BYTE)
   {

      wdStatus = hw_wrongF0Byte;
   }
   else if (c2WatchDogStatus == WD_STATUS_WRONG_A5_BYTE
            &&
            c2WatchDogStatusChk == WD_STATUS_CHK_WRONG_A5_BYTE)
   {

      wdStatus = hw_wrongA5Byte;
   }
   else if (c2WatchDogStatus == WD_STATUS_A5_WRT_WAITING_F0
            &&
            c2WatchDogStatusChk == WD_STATUS_CHK_A5_WRT_WAITING_F0)
   {

      wdStatus = hw_a5WriteWaitingF0;
   }
   else if (c2WatchDogStatus == WD_STATUS_F0_WRT_WAITING_A5
            &&
            c2WatchDogStatusChk == WD_STATUS_CHK_F0_WRT_WAITING_A5)
   {

      wdStatus = hw_f0WriteWaitingA5;
   }
   else if (c2WatchDogStatus == WD_STATUS_TIMEOUT
            &&
            c2WatchDogStatusChk == WD_STATUS_CHK_TIMEOUT)
   {

      wdStatus = hw_wdTimeOut;
   }
   else
   {

      wdStatus = hw_powerUp;
   }

   return wdStatus;
}


//
// Seal Safe ON/OFF Check
//
HWSealSafeStatus hw_sealSafeStatus (void)
{
   return ( sysInByte(C1_PWR_SUPPLY) & SEAL_SAFE_CURRENT_OK ) ? hw_sealSafeOn : hw_sealSafeOff;
}

//
// Seal Safe Over-Temperature Warning Check
//
HWSealSafeOTW hw_sealSafeOTWC (void)
{

   return ( sysInByte(C1_PWR_SUPPLY) & SEAL_SAFE_OVER_TEMP_WARN ) ? hw_sealSafeTempOk : hw_sealSafeOTW;
}

//
// Over-Voltage Protect Test Result (Pass/Fail).
//
HWOvpTestResult hw_OVPTestResult (void)
{

   // Return OVP Test Result
   return ( sysInByte(C1_PWR_SUPPLY) & OVER_VOLT_PROTECT_TEST_RESULT ) ? hw_ovpTestFail : hw_ovpTestPass;

}

//
// Over-Voltage Protect Test Status (Off/On).
//
HWOvpTestStatus hw_OVPTestStatus (void)
{

   // Return OVP Test On or Off
   return ( sysInByte(C1_PWR_SUPPLY) & OVER_VOLT_PROTECT_TEST ) ? hw_ovpTestOn : hw_ovpTestOff;

}

//
// Over-Voltage Protect Test. The OVP Test Bit Must Be Pulsed Hi To Start the Test
//
HWOvpTestResult hw_OVPTest (void)
{

   unsigned char portValue;

   // Set Bit Hi
   portValue  = sysInByte(C1_PWR_SUPPLY);

   portValue |= OVER_VOLT_PROTECT_TEST;

   sysOutByte(C1_PWR_SUPPLY, portValue);

   testReadbackByte(__FILE__, __LINE__, C1_PWR_SUPPLY, portValue);

   if (hw_OVPTestStatus())
   {

      printf("OVP Test Started\n");
   }
   else
   {

      printf("Unable To Start OVP Test\n");
   }

   taskDelay(1);

   // Set Bit Low
   portValue  = sysInByte(C1_PWR_SUPPLY);

   portValue &= ~OVER_VOLT_PROTECT_TEST;

   sysOutByte(C1_PWR_SUPPLY, portValue);

   testReadbackByte(__FILE__, __LINE__, C1_PWR_SUPPLY, portValue);

   if (!( hw_OVPTestResult() ))
   {

      printf("OVP Test PASSED\n");

      // Return OVP Test Result
      return( hw_ovpTestPass );
   }
   else
   {

      printf("OVP Test FAILED\n");

      // Return OVP Test Result
      return( hw_ovpTestFail );
   }
}

//
// (Enable/Disable) the 12.7, 24, and 64 volt outputs of the Artesyn Power Supply
//
HWSupplyOutputStatus hw_SupplyOutputs (HWSupplyOutputs command)
{

   unsigned char portValue = sysInByte(C1_PWR_SUPPLY);

   switch (command)
   {
      case hw_disableOutputs :
         portValue |= DISABLE_SUPPLY_OUTPUTS;
         break;

      case hw_enableOutputs :
         portValue &= ~DISABLE_SUPPLY_OUTPUTS;
         break;
   }

   sysOutByte(C1_PWR_SUPPLY, portValue);

   testReadbackByte(__FILE__, __LINE__, C1_PWR_SUPPLY, portValue);

   // Return Status of the Test
   return ( sysInByte(C1_PWR_SUPPLY) & DISABLE_SUPPLY_OUTPUTS ) ? hw_supplyOutputsDisabled : hw_supplyOutputsEnabled;
}

//
// Status of the Artesyn Power Supply Outputs (Enabled/Disabled)
//
HWSupplyOutputStatus hw_SupplyOutputStatus (void)
{

   // Return Status of the Test
   return ( sysInByte(C1_PWR_SUPPLY) & DISABLE_SUPPLY_OUTPUTS ) ? hw_supplyOutputsDisabled : hw_supplyOutputsEnabled;
}


////                                ////
////                                ////
//// LEGACY FPGA REVISION FUNCTIONS ////
////                                ////
////                                ////

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

// Does this system have the Artesyn Power Supply?  0 if no, 1 if yes.
unsigned short hw_hasArtesynPs (void)
{
   testInitialized("hw_hasArtesynPs");
   return hasArtesynPs;
}

unsigned short hw_switchGetStatus (HWSwitch select)
{
   unsigned char  value  = sysInByte(INP_BUTTONS);
   unsigned short result = 0;
   switch (select)
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

unsigned short hw_cassetteGetStatus (void)
{
   unsigned short result = 0;
   unsigned char  value  = sysInByte(INP_VALVES);

   if ((value & 0x18) == 0x08) result |= hw_cassetteRaised;
   if ((value & 0x18) == 0x10) result |= hw_cassetteLowered;
   return result;
}

//// COMMON/PRIVATE HARDWARE DRIVER FUNCTIONS (common_hw_private.h) ////

void hw_setReadbackFailedFunc (hw_readbackFailedFunc* func)
{
   readbackFailedFunc = func;
}

void hw_alarmSetCommand (HWAlarmCommand command)
{
   unsigned char portValue = sysInByte(IOP_ALARM);

   switch (command)
   {
      case hw_alarmOn :
         portValue &= ~NOT_ALARM_LIGHT_BIT;
         break;

      case hw_alarmOff :
         portValue |= NOT_ALARM_LIGHT_BIT;
         break;
   }

   sysOutByte(IOP_ALARM, portValue);
   testReadbackByte(__FILE__, __LINE__, IOP_ALARM, portValue);
}

unsigned short hw_centGetCommutationCount (void)
{
   return sysInByte(INP_CENT_COMM);
}

unsigned short hw_centGetStatus (void)
{
   unsigned short status    = 0;
   unsigned char  portValue = sysInByte(INP_ODDS_AND_ENDS);

   if (portValue & CENT_COMM_FAULT_BIT) status |= hw_centFault;
   if (portValue & CENT_REVERSE_BIT) status |= hw_centReverse;

   return status;
}

unsigned short hw_doorGetStatus (void)
{
   unsigned char  portValue = sysInByte(INP_ODDS_AND_ENDS);
   unsigned short result    = 0;

   if (portValue & NOT_DOOR_OPEN_BIT) result |= hw_doorClosed;
   if (!(portValue & DOOR_UNLOCKED_BIT)) result |= hw_doorLocked;
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
   unsigned short retVal = (sysInByte(INP_POWER_STATUS) ^ 0x0f) & 0x0f;
   if (!powerFailRideThruEnabled)
   {
      retVal |= hw_powerFailLineDrop;
   }

   return retVal;
}

void hw_powerFailSave (void)
{
   // nothing to do on control side
   return;
}

void hw_powerFailRestore (void)
{
   // nothing to do on control side
   return;
}

HWValvePosition hw_valveGetPosition (HWValve valve)
{
   HWValvePosition result;
   unsigned char   posValue = 0;

   switch (valve)
   {
      case hw_rbcValve :
         posValue = sysInByte(INP_VALVES) & 0x07;
         break;

      case hw_plateletValve :
         posValue = (sysInByte(INP_VALVES) & 0xe0) >> 5;
         break;

      case hw_plasmaValve :
         posValue = (sysInByte(INP_BUTTONS) & 0xe0) >> 5;
         break;

      case hw_cassette :
         posValue = ((sysInByte(INP_VALVES) & 0x18) >> 2) | 0x01;
         break;

      default :
         break;
   }

   switch (posValue)
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

//// CONTROL/PRIVATE HARDWARE DRIVER FUNCTIONS (control_hw_private.h) ////

unsigned short chw_acDetectorGetStatus (void)
{
   return (sysInByte(INP_ODDS_AND_ENDS) & AC_GONE_BIT) ? 0 : chw_acDetectorFluid;
}

void chw_adcStart (void)
{
   sysOutByte(IOP_ADC_MUX, sysInByte(IOP_ADC_MUX) | ADC_START_BIT);
}

unsigned short chw_adcGetStatus (void)
{
   return (sysInByte(INP_ODDS_AND_ENDS) & ADC_BUSY_BIT) ? chw_adcBusy : 0;
}

unsigned short chw_adcGetValue (void)
{
   return sysInWord(INPW_ADC_VALUE);
}

void chw_adcSetMux (unsigned short mux)
{
   sysOutByte(IOP_ADC_MUX, mux & 0x7f);
}

void chw_centSetCommand (unsigned short command)
{
   sysOutWord(IOPW_CENT_SPEED, command);
   testReadbackWord(__FILE__, __LINE__, IOPW_CENT_SPEED, command);

   if (command > 0)
   {
      sysOutByte(IOP_CENT_ENABLE, sysInByte(IOP_CENT_ENABLE) | CENT_ENABLE_BIT);
   }
}

void chw_centClearCommutation (void)
{
   unsigned char value = sysInByte(IOP_CENT_ENABLE);
   value &= ~CENT_CLEAR_COMM_FLAG;

   sysOutByte(IOP_CENT_ENABLE, value | CENT_CLEAR_COMM_FLAG);
   sysOutByte(IOP_CENT_ENABLE, value);
}

void chw_centDisable (void)
{
   sysOutByte(IOP_CENT_ENABLE, sysInByte(IOP_CENT_ENABLE) & ~CENT_ENABLE_BIT);
}

void chw_centEnable (void)
{
   sysOutByte(IOP_CENT_ENABLE, sysInByte(IOP_CENT_ENABLE) | CENT_ENABLE_BIT);
}

unsigned short chw_doorLockGetStatus (void)
{
   return (sysInByte(INP_ODDS_AND_ENDS) & NOT_SOLENOID_FAULT_BIT) ? 0 : chw_doorLockFault;
}

void chw_doorLockSetCommand (CtlHWDoorLockCommand command)
{
   unsigned char commandByte = 0;

   switch (command)
   {
      case chw_doorLockOff :
         // command to current position
         commandByte = (sysInByte(INP_ODDS_AND_ENDS) & DOOR_UNLOCKED_BIT) ? 0 : DOOR_DIRECTION_BIT;
         sysOutByte(IOP_DOOR, commandByte);

         // turn off solenoid power
         commandByte |= NOT_DOOR_SOLENOID_BIT;
         sysOutByte(IOP_DOOR, commandByte);
         break;

      case chw_doorLock :
         commandByte = DOOR_DIRECTION_BIT;
         sysOutByte(IOP_DOOR, commandByte);
         break;

      case chw_doorUnlock :
         commandByte = 0;
         sysOutByte(IOP_DOOR, commandByte);
         break;

      default :
         printf("Unrecognized door lock command %d\n", (int)command);
         abort();
   }

   testReadbackByte(__FILE__, __LINE__, IOP_DOOR, commandByte);
}

unsigned short chw_fanGetStatus (void)
{
   unsigned short rawStatus = sysInByte(INP_POWER_STATUS);
   unsigned short status    = 0;
   if ((rawStatus & NOT_FAN_SENSE0_BIT)) status |= chw_fan0Fault;
   if ((rawStatus & NOT_FAN_SENSE1_BIT)) status |= chw_fan1Fault;
   if ((rawStatus & NOT_FAN_SENSE2_BIT)) status |= chw_fan2Fault;

   return status;
}

void chw_leakDetectorSetCommand (CHWLeakDetectorCommand command)
{
   unsigned char portValue = sysInByte(IOP_ALARM);

   switch (command)
   {
      case chw_leakDetectorOn :    portValue |= LEAK_DETECTOR_BIT; break;
      case chw_leakDetectorOff :   portValue &= ~LEAK_DETECTOR_BIT;    break;
   }

   sysOutByte(IOP_ALARM, portValue);
   testReadbackByte(__FILE__, __LINE__, IOP_ALARM, portValue);
}

unsigned short chw_pumpGetStatus (HWPump select)
{
   return (sysInByte(INP_BUTTONS) & NOT_MOTOR_FAULT_BIT) ? 0 : chw_pumpFault;
}

unsigned short chw_pumpGetEncoder (HWPump select)
{
   unsigned short addr = 0;
   unsigned short current;
   unsigned short reRead;

   switch (select)
   {
      case hw_acPump :
         addr = INPW_AC_ENCODER;
         break;

      case hw_inletPump :
         addr = INPW_INLET_ENCODER;
         break;

      case hw_plasmaPump :
         addr = INPW_PLASMA_ENCODER;
         break;

      case hw_plateletPump :
         addr = INPW_PLATELET_ENCODER;
         break;

      case hw_returnPump :
         addr = INPW_RETURN_ENCODER;
         break;

      default :
         printf("Unrecognized pump %d\n", (int)select);
         abort();
   }

   current = sysInWord(addr);
   reRead  = sysInWord(addr);

   //
   // The FPGA does not lock out changes to the upper encoder byte
   // during read operations, so this byte can potentially change
   // between reading the low and high byte, or during the I/O cycle
   // for reading the high byte.  If a change does occur, it will
   // require less than a full inpw I/O cycle, so it can effect only
   // one of the two inpw() calls above. Since it will not be known
   // which of the two reads was in error, we simply repeat the inpw
   // operation if a change is detected.  Even at maximum flow rates,
   // the pump encoder can not accumulate 256 counts during this
   // function call, so this repeat can be assumed to work correctly.
   //
   if ((current & 0xff00) != (reRead & 0xff00))
   {
      current = (short)sysInWord(addr);
   }

   return current;
}

void chw_pumpSetCommand (HWPump select, short command)
{
   if (select == hw_allPumps)
   {
      chw_pumpSetCommand(hw_acPump, command);
      chw_pumpSetCommand(hw_inletPump, command);
      chw_pumpSetCommand(hw_plasmaPump, command);
      chw_pumpSetCommand(hw_plateletPump, command);
      chw_pumpSetCommand(hw_returnPump, command);
   }
   else
      switch (select)
      {
         case hw_acPump :
            sysOutByte(IOP_AC_PWM, command);
            testReadbackByte(__FILE__, __LINE__, IOP_AC_PWM, command);
            break;

         case hw_inletPump :
            sysOutByte(IOP_INLET_PWM, command);
            testReadbackByte(__FILE__, __LINE__, IOP_INLET_PWM, command);
            break;

         case hw_plasmaPump :
            sysOutByte(IOP_PLASMA_PWM, command);
            testReadbackByte(__FILE__, __LINE__, IOP_PLASMA_PWM, command);
            break;

         case hw_plateletPump :
            sysOutByte(IOP_PLATELET_PWM, command);
            testReadbackByte(__FILE__, __LINE__, IOP_PLATELET_PWM, command);
            break;

         case hw_returnPump :
            if (command > 0)
            {
               unsigned char portValue = sysInByte(IOP_PUMP_ENABLE);
               if (!(portValue & RETURN_DIR_BIT))
               {
                  // Stop pump then switch direction
                  sysOutByte(IOP_RETURN_PWM, 0);
                  portValue |= RETURN_DIR_BIT;
                  sysOutByte(IOP_PUMP_ENABLE, portValue);
                  testReadbackByte(__FILE__, __LINE__, IOP_PUMP_ENABLE, portValue);
               }

               sysOutByte(IOP_RETURN_PWM, command);
               testReadbackByte(__FILE__, __LINE__, IOP_RETURN_PWM, command);
            }
            else
            {
               unsigned char portValue = sysInByte(IOP_PUMP_ENABLE);
               if (portValue & RETURN_DIR_BIT)
               {
                  // Stop pump then switch direction
                  sysOutByte(IOP_RETURN_PWM, 0);
                  portValue &= ~RETURN_DIR_BIT;
                  sysOutByte(IOP_PUMP_ENABLE, portValue);
                  testReadbackByte(__FILE__, __LINE__, IOP_PUMP_ENABLE, portValue);
               }

               sysOutByte(IOP_RETURN_PWM, -command);
               testReadbackByte(__FILE__, __LINE__, IOP_RETURN_PWM, -command);
            }

            break;

         default :
            break;
      }
}

void chw_pumpEnable (void)
{
   unsigned char portValue = sysInByte(IOP_PUMP_ENABLE) | PUMP_ENABLE_BIT;
   sysOutByte(IOP_PUMP_ENABLE, portValue);
   testReadbackByte(__FILE__, __LINE__, IOP_PUMP_ENABLE, portValue);
}

void chw_pumpDisable (void)
{
   unsigned char portValue = sysInByte(IOP_PUMP_ENABLE) & ~PUMP_ENABLE_BIT;
   sysOutByte(IOP_PUMP_ENABLE, portValue);
   testReadbackByte(__FILE__, __LINE__, IOP_PUMP_ENABLE, portValue);
}

void chw_soundSetLevel (unsigned short level)
{
   sysOutByte(IOP_SOUND_LEVEL, level & 0x03);
}

unsigned short chw_soundGetLevel (void)
{
   return sysInByte(IOP_SOUND_LEVEL) & 0x03;
}

unsigned short chw_ultrasonicSensorGetCounter (CtlHWUltrasonicSensor select)
{
   unsigned short result = 0;

   switch (select)
   {
      case chw_lowerUltrasonicSensor :
         result = sysInWord(INPW_US_LOW);
         break;

      case chw_upperUltrasonicSensor :
         result = sysInWord(INPW_US_HIGH);
         break;
   }

   return result;
}

unsigned short chw_valveGetStatus (HWValve select)
{
   return (sysInByte(INP_BUTTONS) & NOT_VALVE_FAULT_BIT) ? 0 : chw_valveFault;
}

HWValve chw_valveGetActive (void)
{
   HWValve       result    = hw_noValve;
   unsigned char portValue = sysInByte(IOP_VALVE);

   if (portValue & VALVE_RUN_BIT)
   {
      if (portValue & VALVE_SELECT_RBC_BIT)
      {
         result = hw_rbcValve;
      }
      else if (portValue & VALVE_SELECT_PLASMA_BIT)
      {
         result = hw_plasmaValve;
      }
      else if (portValue & VALVE_SELECT_PLATELET_BIT)
      {
         result = hw_plateletValve;
      }
      else if (portValue & VALVE_SELECT_CASSETTE_BIT)
      {
         result = hw_cassette;
      }
   }

   return result;
}

void chw_valveSetCommand (HWValve select, CtlHWValveCommand command)
{
   if (command == chw_stopValve)
   {
      unsigned char portValue = sysInByte(IOP_VALVE);

      portValue &= ~VALVE_RUN_BIT;
      sysOutByte(IOP_VALVE, portValue);
      testReadbackByte(__FILE__, __LINE__, IOP_VALVE, portValue);
   }
   else
   {
      unsigned char portValue = (command == chw_cwValve) ? VALVE_CW_BIT : VALVE_CCW_BIT;
      portValue |= VALVE_RUN_BIT;
      sysOutByte(IOP_VALVE, portValue);

      switch (select)
      {
         case hw_rbcValve :   portValue    |= VALVE_SELECT_RBC_BIT;              break;
         case hw_plasmaValve : portValue   |= VALVE_SELECT_PLASMA_BIT;      break;
         case hw_plateletValve : portValue |= VALVE_SELECT_PLATELET_BIT;  break;
         case hw_cassette : portValue      |= VALVE_SELECT_CASSETTE_BIT;       break;
         default :
            printf("Bad valve command %d %d\n", (int)select, (int)command);
            abort();
      }

      sysOutByte(IOP_VALVE, portValue);
      testReadbackByte(__FILE__, __LINE__, IOP_VALVE, portValue);
   }
}

static void testInitialized (const char* funcName)
{
   if (!initializationResult)
   {
      printf("HW driver must be initialized to access function \"%s\"\n", funcName);
      abort();
   }
}

static void testReadbackByte (const char* file, int line, unsigned short port, unsigned short expected)
{
   unsigned short value = sysInByte(port);

   if (value != expected &&
       readbackFailedFunc != NULL)
   {
      (* readbackFailedFunc)(file, line, "byte", port, value, expected);
   }
}

static void testReadbackWord (const char* file, int line, unsigned short port, unsigned short expected)
{
   unsigned short value = sysInWord(port);

   if (value != expected &&
       readbackFailedFunc != NULL)
   {
      (* readbackFailedFunc)(file, line, "word", port, value, expected);
   }
}

/* FORMAT HASH a2bf59f98f706500967243617d62e4d1 */
