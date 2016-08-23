/*
 * Copyright (C) 2003 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/trima_common/rcs/common_hw.h 1.6 2007/08/09 22:52:50Z rhecusb Exp dslausb $
 * $Log: common_hw.h $
 * Revision 1.6  2007/08/09 22:52:50Z  rhecusb
 * Fixed OVP_TEST_RESULT to match the values in the updated Interface Requirements Spec For Control1 FPGA.
 * Revision 1.1  2003/06/19 19:53:11Z  jl11312
 * Initial revision
 *
 * Defines hardware driver functions which are available to all system tasks.
 *
 */

#ifndef _COMMON_HW_INCLUDE
#define _COMMON_HW_INCLUDE

// List of switches controlled by the hardware driver
typedef enum
{
   hw_stopSwitch,
   hw_pauseSwitch
} HWSwitch;

// Bitmasks for switch status conditions
typedef enum
{
   hw_switchPressed = 0x01
} HWSwitchStatus;

typedef enum
{
   hw_cassetteRaised  = 0x01,
   hw_cassetteLowered = 0x02
} HWCassetteStatus;

typedef enum
{

   hw_controlBoard = 0x01,
   hw_safetyBoard  = 0x02
} HWBoardType;

typedef enum
{
   hw_control1Fpga = 0x01,
   hw_control2Fpga = 0x02,
   hw_safetyFpga   = 0x03
} HWFpgaType;

typedef enum
{
   hw_noFailure,
   hw_wrongA5Byte,
   hw_wrongF0Byte,
   hw_a5WriteWaitingF0,
   hw_f0WriteWaitingA5,
   hw_wdTimeOut,
   hw_powerUp,
   hw_unknown
} HWWatchDogStatus;

// Seal Safe Status (On/Off)
typedef enum
{
   hw_sealSafeStatusUnavailable,
   hw_sealSafeOn,
   hw_sealSafeOff
} HWSealSafeStatus;

// Seal Safe Over Temp (Warning/OK)
typedef enum
{
   hw_sealSafeTempUnavailable,
   hw_sealSafeOTW,
   hw_sealSafeTempOk
} HWSealSafeOTW;

// Over Voltage Protect Test Result (Pass/Fail)
typedef enum
{
   hw_sealTestResultUnavailable,
   hw_ovpTestPass,
   hw_ovpTestFail
} HWOvpTestResult;

// Over Voltage Protect Test Status (Off/On)
typedef enum
{
   hw_ovpTextUnavailable,
   hw_ovpTestOff,
   hw_ovpTestOn
} HWOvpTestStatus;

// (Enable/Disable) Power Supply Outputs
typedef enum
{
   hw_enableOutputs,
   hw_disableOutputs
} HWSupplyOutputs;

// Disable Power Supply Outputs Status (Enabled/Disabled)
typedef enum
{
   hw_supplyOutputsUnavailable,
   hw_supplyOutputsEnabled,
   hw_supplyOutputsDisabled
} HWSupplyOutputStatus;

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */


//// GENERAL HARDWARE DRIVER FUNCTIONS ////

// Initialize access to HW driver - must be called before any other driver
// functions.  Returns > 0 on success, 0 otherwise.
//
int hw_init (void);

////                                                      ////
//// NEW (MX) CONTROL/SAFETY BOARD AND FPGA REVISION INFO ////
////                                                      ////

// Returns board type (control or safety)
HWBoardType hw_getBoardType (void);

// Returns true if the New MX FPGAs are installed. False otherwise
int hw_mxFpgaInstalled (void);

// Returns FPGA Firmware Version based on type of FPGA (Control1, Control2, or Safety)
unsigned char hw_fpgaFwRevision (HWFpgaType fpga);

// Returns FPGA ID Version based on type of FPGA (Control1, Control2, or Safety)
unsigned char hw_fpgaIdRevision (HWFpgaType fpga);

// Returns FPGA Interface Version based on type of FPGA (Control1, Control2, or Safety)
unsigned char hw_fpgaIntfRevision (HWFpgaType fpga);

// Returns Control or Safety Board CCA Revision
unsigned char hw_ccaRevision (void);

// Returns the Status of the Hardware WatchDog
HWWatchDogStatus hw_watchDogStatus (void);


//// ARTESYN POWER SUPPLY SEAL SAFE RELATED FUNCTIONS ////

HWSealSafeStatus hw_sealSafeStatus (void); // On/Off
HWSealSafeOTW    hw_sealSafeOTWC (void);   // Warning/OK


//// ARTESYN POWER SUPPLY OVER-VOLTAGE PROTECT FUNCTIONS ////

HWOvpTestResult hw_OVPTest (void);       // Run the OVP Test
HWOvpTestStatus hw_OVPTestStatus (void); // OVP Test On/Off
HWOvpTestResult hw_OVPTestResult (void); // OVP Test Pass/Fail

//// ENABLE/DISABLE ARTESYN POWER SUPPLY OUTPUTS FUNCTIONS ////

HWSupplyOutputStatus hw_SupplyOutputs (HWSupplyOutputs command); // Enable/Disable
HWSupplyOutputStatus hw_SupplyOutputStatus (void);               // Enabled/Disabled


// Force machine to trigger watchdogs and generate a hardware reset
//
void hw_forceReset (void);

// Command all hardware to safe state
//
void hw_safeState (void);


////                                                    ////
//// LEGACY CONTROL/SAFETY BOARD AND FPGA REVISION INFO ////
////                                                    ////

// Return control board layout version (0-255)
//
unsigned short hw_boardVersion (void);

// Return FPGA version (0-255)
//
unsigned short hw_fpgaVersion (void);

// Return hardware version - high 8 bits give major version, low 8 bits give
// minor version (e.g. 0x0206 is returned for version 2.6)
//
unsigned short hw_hardwareVersion (void);

// Does this system have the Artesyn Power Supply?  0 if no, 1 if yes.
unsigned short hw_hasArtesynPs (void);

//// SWITCH RELATED FUNCTIONS ////

// Return status (bitwise-OR of conditions defined for HWSwitchStatus) for
// the specified switch.
//
unsigned short hw_switchGetStatus (HWSwitch select // switch for which status is to be read
                                   );

//// CASSETTE RELATED FUNCTIONS ////

// Return status (bitwise-OR of conditions defined for HWCassetteStatus) for
// the cassette
//
unsigned short hw_cassetteGetStatus (void);


#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _COMMON_HW_INCLUDE */

/* FORMAT HASH 7c9bc50a40d8d00e0a7e2d5dfc184fcf */
