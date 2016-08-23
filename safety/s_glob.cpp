/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.60  2008/08/26 18:38:51Z  spriusb
 * Added code to send proc state down to safety driver layer through test1 field to better facilitate unit testing.
 * Revision 1.59  2006/03/18 00:07:06Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.58  2003/12/16 22:00:28Z  ms10234
 * 6642 - Command power off on every alarm repeater message
 * Revision 1.57  2003/08/20 19:04:10Z  jl11312
 * - changes for IT 6373
 * Revision 1.56  2003/08/15 20:52:48Z  jl11312
 * - implemented new APS sensor auto-zero functions (IT 6253)
 * Revision 1.55  2003/07/21 22:09:13Z  td07711
 * IT6157 - don't alarm when going fatal
 * Revision 1.54  2003/07/15 21:31:49Z  ms10234
 * 6157 - log level changes from debug to info
 * Revision 1.53  2003/07/07 21:04:42Z  jl11312
 * - implemented APS auto-zero
 * Revision 1.52  2003/07/01 22:34:46Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.51  2003/06/03 14:18:03Z  rm70006
 * IT 6096.
 *
 * Change code to use new alarm interface.
 * Revision 1.50  2003/04/04 00:51:58Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.49  2003/01/27 21:06:36Z  pn02526
 * Remove "datalog_delete" comments.  Improve some logging.
 * Remove CalculateAPSParms(), expand its functionality inline in
 * calc_APS_mmhg_3_pt(), and revise the logic to store the values
 * in static variables so that repeated re-reads of cal.dat are not needed.
 * Remove obsolete calc_APS_mmhg() function.
 * Revision 1.48  2002/10/11 10:52:18  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.47  2002/08/07 17:06:06  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Use the new buffered message base class definition to send the SHwOrders message, rather than deriving class aSafetyHardwareCommandsExec from it.
 * Revision 1.46  2002/07/23 16:52:27  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.45  2002/04/09 18:09:44  pn02526
 * "Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one."
 * Revision 1.44  2000/11/02 22:34:46Z  bs04481
 * If multiple values in the cal.dat file are bad, only alarm once for
 * the whole file instead of alarming for each value.
 * Revision 1.43  2000/08/01 20:48:39  pn02526
 * This change effects the solution to IT3633.
 * When Proc notifies saf_exec of an A2 ,
 * power is shut off on the 24 and 64 V lines,
 * and a message is logged.
 * Revision 1.42  2000/07/06 01:18:52  bs04481
 * Variable name change
 * Revision 1.41  2000/06/08 21:35:42  bs04481
 * Get rid of the separate alarm sender and just use the repeater
 * Revision 1.40  2000/05/26 17:07:24  BS04481
 * Added alarm repeater.  Changed messages to NO_BOUNCE.
 * Revision 1.39  2000/03/15 18:19:06  BS04481
 * Make hardware command object into the message handler and
 * remove unnecessary handler; remove unnecessary defines
 * Revision 1.38  2000/02/28 21:48:41  SB07663
 * included dat_file.h vs. dat_file.cpp for proc renaming
 * Revision 1.37  1999/10/14 18:11:25  BS04481
 * At transition to Disposable Test, AC Connect and AC Prime, set
 * donor mode to Disabled.
 * Revision 1.36  1999/10/08 15:37:21  BS04481
 * Restore air-to-donor monitoring in the safety driver following a
 * PF if the procedure is not recoverable and the cassette is down.
 * Revision 1.35  1998/09/28 22:19:40  bs04481
 * Add blockpower flag to SetBothPower function
 * Revision 1.34  1998/08/25 17:01:10  bs04481
 * Add the blockpumppower flag to the function which request power
 * enabled to both 24 and 64 switched supplies.
 * Revision 1.33  1998/07/15 15:38:50  bs04481
 * Fixed the pressure calibration that I messed up when I moved
 * this code in rev 1.31
 * Revision 1.32  1998/07/14 21:23:18  bs04481
 * Temporarily move this file back to previous revision
 * Revision 1.31  1998/07/14 18:00:34  bs04481
 * Move read of the cal.dat file to after the dispatcher is initialized.
 * Revision 1.30  1998/02/24 04:36:59  bs04481
 * When coming back from a PF, if the donor mode is enabled and
 * the cassette is in the up position, don't stick the donor mode.  This
 * can happen if the power fails after the donor disconnect test is
 * passed and the cassette raised but before they push the button
 * to transition to Post Run.
 * Revision 1.29  1997/12/12 17:12:51  bs04481
 * Change BlockPumpPower function to also turn off 24v if it is
 * already enabled.
 * Revision 1.28  1997/12/05 21:09:09  bs04481
 * Change to disable the pump power timer if the timer has already
 * gone off because no one pushed the button but now we have
 * decided to go to donor disconnect.  Previously, the power was
 * off and it was necessary to recycle before the pumps could run
 * the disconnect test.
 * Revision 1.27  1997/12/02 15:59:49  bs04481
 * Changes to implement power fail recovery
 * Revision 1.26  1997/07/30 15:53:10  SM02805
 * Modified text search implementations for state names and
 * alarm names that are used for safety trace logging to be consistent
 * with enumeration changes to states ( /proc/states.h ) and
 * alarms ( /proc/alarms.h ). These files and enums are common
 * between the domains.
 * Revision 1.25  1997/05/20 15:22:18  bs04481
 * Change to 3pt calibration method.  Raw ADC reference reading was not initialized.  This effected the inlet pump stroke volume and hence, the inlet pump volume which then effected the AC ratio calculations.
 * Revision 1.24  1997/05/15 00:43:22  SM02805
 * Implemented three point APS calibration.
 * Revision 1.23  1997/04/18 15:30:18  SM02805
 * Moved range test/clipping of pressure ( required for inlet pump stroke
 * volume correction ) from the pressure calculation routine in s_glob.cpp
 * to the stroke volume correction routine in s_pump.cpp.
 *
 * This change results in the pressure calculation routine always returing
 * the calculated pressure.  If the pressure is outside allowable limits,
 * it is clipped to allowable limits before the inlet pump sv is calculated.
 *
 * This change is required to correct a bug in the donor d/c test where
 * if the starting pressure was -150 mmHg or less, safety would never
 * conclude the test was passed because the pressure would never appear
 * to get to 100 mmHG below the starting value due to clipping.
 * Revision 1.22  1997/03/16 21:52:03  bs04481
 * Corrected logging to show correct old & new state for power
 * requests
 * Revision 1.21  1997/03/13 23:21:04  bs04481
 * Changes to set the new donorMode in the safety command data
 * Revision 1.20  1997/02/11 18:00:39  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.19  1996/12/13 18:17:11  bs04481
 * Added range testing of calculated APS : limited to +400 : -250 mmHg.
 * Revision 1.18  1996/11/28 00:08:12  SM02805
 * Revision 1.17  1996/09/15 21:50:44  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.16  1996/09/15 21:50:44  SM02805
 * Fixes for safety bug that was not keeping power disabled following
 * alarm detection :
 * 1. eliminate inclusion of safact.hpp in all files and remove all conditional
 * safety ( no code remains to allow continuation from safety alarms).
 * 2. Implement _cAlarmState flag in aSafetyHardwareCommandsExec to
 * prevent pump/centrifuge power enabling post alarm generation.
 * _cAlarmState is initialized to SAFETY_ALARM_ASSERTED in
 * constructor, cleared by s_exec.cpp (NO_SAFETY_ALARM) if
 * positive response to query safety alive msg is sent to proc.
 * 3. Gate door power messages on change of door power
 * cmd from last cmd to reduce msg loading to safety driver.
 * Revision 1.15  1996/09/04 19:34:42  SM02805
 * Fixes for safety bug related to re-enabling power following alarm assertion:
 * - eliminate inclusion of safact.hpp, elminate all references to conditional
 * safety.
 * - add _cAlarmState flag to aSafetyHardwareCommandsExec : when
 * _cAlarmState is asserted to SAFETY_ALARM_ASSERTED, enabling
 * messages for safety power are not sent to the safety driver ( methods
 * SetPumpPower,SetCentrifugePower,SetCentrifugePower )
 * - i/fs for SetAlarmState and GetAlarmState
 * - init _cAlarmState in constructor to SAFETY_ALARM_ASSERTED
 * - clear _cAlarmState in query safety alive when turn off alarm light
 * - assert _cAlarmState in respond_to_alarms following alarm message to
 * procedure
 * - at 2Hz after an alarm assertion, send command msg to safety driver
 * with power disable both switches
 * - gate door power messages in the SetDoorPower method based on
 * door power state to reduce message load.
 * -added more logging for alarm generation in respond_to_alarm
 * Revision 1.14  1996/08/20 21:21:20  tm02109
 * Jerry lint fix, array limit.
 * Revision 1.13  1996/08/19 22:07:17  SM02805
 * Respond to lint problems identified in 08/19/96 listing relating
 * to array size and index for alarm_action_table.
 * Revision 1.12  1996/07/31 22:11:36  SM02805
 * Revision 1.11  1996/07/31 22:11:36  SM02805
 * Added pressure calibration fix - read cal.dat for gain and offset for
 * APS.
 * Revision 1.10  1996/07/31 02:16:58  SM02805
 * GOt cal constants fro Cramer from cal.dat and h/w'ed into
 * APS pressure calc funtion. Improved problem with inlet volume
 * disagreement with procedure.
 * Revision 1.9  1996/07/29 11:27:14  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.8  1996/07/24 13:57:02  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 * Revision 1.7  1996/07/22 17:14:17  SM02805
 * Update documentation for code review.
 *
 * Corrections to pump monitoring : but for platelet pump due to two magnet rotor.
 *
 * stest.mak target changed to be  saf_exec.
 *
 * TITLE:      s_glob.cpp
 *             Safety global functions.
 *
 *
 * ABSTRACT:
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#include <stdio.h>

#include "trima_datalog.h"
#include "alarms.h"
#include "s_glob.hpp"
#include "s_pfr.hpp"
#include "filenames.h"

#include "dat_fnof.hpp" // use local reader.  can't use the trima_common version
                        // because it redefines bool

#define APS_GAIN       0.408105732f
#define APS_OFFSET     -1053.312287f

/********
  Global Declarations
**************/

// globally accessible safe state actuator.
aSafetyHardwareCommandsExec* paSafetyHardwareCommandsExec = NULL;

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

// debug array to make safety alarm positings one-shots.
bool acAlarmActive[NUM_SAFETY_ALARMS];


/**************************
 Global Functions
*************************/

// Eventually, the following APS routines should be encapsulated in class(es).  PCN 1/22/03
// SPECIFICATION:    calculate access pressure
//                   Parameter :
//                   raw_adc - raw adc value for access pressure xducr from
//                   control status.
//
//
// ERROR HANDLING:   Software Fault Alarm.

//
// calibration vbls for 3 point
//
static float fAPS_gain_POS             = 0.f; // gain for positive pressures incl 0 mmHG
static float fAPS_gain_NEG             = 0.f; // gain for negative pressures
static float fAPS_raw_REF              = 0.f; // current adc raw value for 0 mmHg
static float fAPS_raw_REF_from_file    = 0.f; // adc raw value for 0 mmHg (as read from cal.dat)

static bool  bAPS_cal_data_initialized = false;

void initialize_APS_cal (void)
{
   // Init calibration vbls for 3 point, if necessary.
   if (bAPS_cal_data_initialized)
   {
      return;
   }
   // 05/14/97 3 point APS cal init routine
   // take input from cal.dat calculate gains for
   // piecewise linear ( 2 curve ) fit
   float aps_mmHg_pos, aps_mmHg_ref, aps_mmHg_neg;
   float aps_raw_pos, aps_raw_ref, aps_raw_neg;
   bool  result = true;

   // 07/31/96 implement pressure calibration data read
   aDat_File_Reader_no_fault config (PNAME_CALDAT);

   if ( config.get_float("APS", "aps_raw_pos", -2000.0f, 5000.0f, &aps_raw_pos) == false ||
        config.get_float("APS", "aps_mmHg_pos", -500.0f, 500.0f, &aps_mmHg_pos) == false ||
        config.get_float("APS", "aps_raw_ref", -2000.0f, 5000.0f, &aps_raw_ref) == false ||
        config.get_float("APS", "aps_mmHg_ref", -500.0f, 500.0f, &aps_mmHg_ref) == false ||
        config.get_float("APS", "aps_raw_neg", -2000.0f, 5000.0f, &aps_raw_neg) == false ||
        config.get_float("APS", "aps_mmHg_neg", -500.0f, 500.0f, &aps_mmHg_neg) == false )
   {
      result = false;
      DataLog(log_level_safety_alarm_detail) << "Error reading \"" << PNAME_CALDAT << "\"" << endmsg;
   }
   else
   {
      bAPS_cal_data_initialized = true;

      // NOMINAL, Non-Error Operation logging
      // 05/14/97 mod for 3 point APS cal
      DataLog(log_level_safe_exec_info) << "aps_raw_pos = " << aps_raw_pos
                                        << " aps_mmHg_pos = " << aps_mmHg_pos << endmsg;

      DataLog(log_level_safe_exec_info) << "aps_raw_ref = " << aps_raw_ref
                                        << " aps_mmHg_ref = " << aps_mmHg_ref << endmsg;

      DataLog(log_level_safe_exec_info) << "aps_raw_neg = " << aps_raw_neg
                                        << " aps_mmHg_neg = " << aps_mmHg_neg << endmsg;

      fAPS_gain_POS = ( aps_mmHg_pos - aps_mmHg_ref ) / ( aps_raw_pos - aps_raw_ref );
      fAPS_gain_NEG = ( aps_mmHg_ref - aps_mmHg_neg ) / ( aps_raw_ref - aps_raw_neg );
      fAPS_raw_REF  = fAPS_raw_REF_from_file = aps_raw_ref;

      DataLog(log_level_safe_exec_info) << "fAPS_gain_POS = " << fAPS_gain_POS
                                        << " fAPS_gain_NEG = " << fAPS_gain_NEG << endmsg;
   }
}

// 05/14/97 3 point APS cal conversion routine
// take raw adc, return mmHg to caller
// positive cal curve for raw adc >= ref value
// negative for less than
float calc_APS_mmhg_3_pt (short raw_adc)
{
   float fABSTemp = 0.f;

   if ( !bAPS_cal_data_initialized )
   {
      // we either had a failure reading the calibration data, or a software fault
      // caused initialize_APS_cal() to never be called.
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
   }
   else if ( (float)raw_adc >= fAPS_raw_REF )
   {
      fABSTemp = ( raw_adc - fAPS_raw_REF ) * fAPS_gain_POS;
   }
   else
   {
      fABSTemp = ( raw_adc - fAPS_raw_REF ) * fAPS_gain_NEG;
   }

   return( fABSTemp );
}

void set_APS_mmhg_offset (float zero)
{
   if ( zero >= 0 && fAPS_gain_POS != 0 )
   {
      fAPS_raw_REF = fAPS_raw_REF_from_file + zero / fAPS_gain_POS;
   }
   else if ( zero < 0 && fAPS_gain_NEG != 0 )
   {
      fAPS_raw_REF = fAPS_raw_REF_from_file + zero / fAPS_gain_NEG;
   }

   DataLog(log_level_safe_exec_info) << "new APS reference zero=" << zero << " raw=" << fAPS_raw_REF << endmsg;
}

// SPECIFICATION:    calculate elapsed time "running" calculates elasped time
//                   from previous time passed in pstructlastTime , overwrites
//                   pstructlastTime with current time to arm for next delta
//                   calcuation. Used for timing sample  times for RB infusion
//                   calculations 09/13/96.
//
//                   Parameters :
//                   pstructlastTime - pointer to QNX timpespec structure
//                   containing "start" time for elapsed time calc.
//
// ERROR HANDLING:   none.

float calc_elapsed_time_running (struct timespec* pstructlastTime)
{
   float           dt = 0.0;
   struct timespec timeNow;

   clock_gettime(CLOCK_REALTIME, &timeNow);
   dt  = (float)(timeNow.tv_nsec - pstructlastTime->tv_nsec) / BILLION;
   dt += (float)(timeNow.tv_sec - pstructlastTime->tv_sec);

   // overwrite timespec structure with new time for running delta type
   // calculations.

   pstructlastTime->tv_nsec = timeNow.tv_nsec;
   pstructlastTime->tv_sec  = timeNow.tv_sec;

   return dt;
}


// SPECIFICATION:    calculate elapsed time
//                   Parameters :
//                   pstructlastTime - pointer to QNX timpespec structure
//                   containing "start" time for elapsed time calc.
//
// ERROR HANDLING:   none.

float calc_elapsed_time (const struct timespec* pstructlastTime)
{
   float           dt = 0.0;
   struct timespec timeNow;

   clock_gettime(CLOCK_REALTIME, &timeNow);
   dt  = (float)(timeNow.tv_nsec - pstructlastTime->tv_nsec) / BILLION;
   dt += (float)(timeNow.tv_sec - pstructlastTime->tv_sec);

   return dt;
}



aSafetyAlarmMsg_sndr::aSafetyAlarmMsg_sndr()
   : _alarmMsg(MessageBase::SEND_GLOBAL)
{
   _alarm.alarm_name  = NULL_ALARM;
   _alarm.alarm_state = ACTIVE;
}



void aSafetyAlarmMsg_sndr::postAlarm (ALARM_VALUES alarm_name)
{
   _alarm.alarm_name = alarm_name;

   DataLog(log_level_safe_exec_debug) << "aSafetyAlarmMsg_sndr :: postAlarm("
                                      << (int)alarm_name << ") sending structAlarm" << endmsg;

   _alarmMsg.send(_alarm);
}



aSafetyHardwareCommandsExec::aSafetyHardwareCommandsExec()
   : _theSHwOrdersMsg(MessageBase::SEND_GLOBAL),
     _safetyAlarmRepeater(),
     _msgCounter(0),
     _alarmState(NO_SAFETY_ALARM),
     _blockPower(true),
     _alarmTable()

{
   memset(&_SHwOrdersData, 0, sizeof(SHwOrders));

   _SHwOrdersData.pumpPower         = HW_DISABLE;
   _SHwOrdersData.centrifugePower   = HW_DISABLE;
   _SHwOrdersData.valveLED          = HW_ENABLE;
   _SHwOrdersData.doorSolenoidPower = HW_ENABLE;
   _SHwOrdersData.alarmLight        = HW_ENABLE;
   _SHwOrdersData.donorConnectMode  = HW_DISABLE;
   _SHwOrdersData.serviceEnableMode = HW_DISABLE;

   send();
}



/*******************************************************************************
*
*  aSafetyHardwareCommandsExec CLASS METHOD DEFINITION
*
******************************************************************************/
// SPECIFICATION:    respond to alarm
//                   places machine in safe state, and posts alarm to proc
//
//                   Parameters :
//                   cSafeStateAction - OFF_BOTH || OFF_24V || OFF_64V (safe state rqd for alarm)
//                   alarm_name - name of alarm to be generated
// ERROR HANDLING:   FATAL_ERROR.

void aSafetyHardwareCommandsExec::respond_to_alarm (char cSafeStateAction, ALARM_VALUES alarm_name)
{
   // PFR Data
   SafetyPFRDataToSave.PFRSafetyAlarmInProgress = true;

   switch ( cSafeStateAction )
   {
      case OFF_24V :

         _SHwOrdersData.pumpPower = HW_DISABLE;

         break; // end case turn off 24 V power

      case OFF_64V :

         _SHwOrdersData.centrifugePower = HW_DISABLE;

         break; // end case turn off 64 V power

      case OFF_BOTH :

         _SHwOrdersData.pumpPower       = HW_DISABLE;
         _SHwOrdersData.centrifugePower = HW_DISABLE;

         break; // end case turn off 24V and 64 V power

      default :

         DataLog(log_level_critical) << "Safe State Action: "
                                     << (int)cSafeStateAction
                                     << " causes forbidden case switch default"
                                     << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid SafeStateAction");

         break; // end case s/w fault detected based on bad passed value

   } // end          switch( cSafeStateAction )

   _SHwOrdersData.alarmLight = HW_ENABLE;

   DataLog(log_level_safe_exec_info) << "aSafetyHardwareCommandsExec :: respond_to_alarm("
                                     << (int)cSafeStateAction << " " << (int)alarm_name
                                     << ") #1 sending SHwOrdersMsg("
                                     << ( _SHwOrdersData.pumpPower == HW_DISABLE ? "pumpPwrDISABLE" : _SHwOrdersData.pumpPower == HW_ENABLE ? "pumpPwrENABLE" : "pumpPwrUNKNOWN" )
                                     << " "
                                     << ( _SHwOrdersData.centrifugePower == HW_DISABLE ? "centPwrDISABLE" :  _SHwOrdersData.centrifugePower == HW_ENABLE ? "centPwrENABLE" : "centPwrUNKNOWN" )
                                     << " alrmLiteENABLE)" << endmsg;

   send();

   if ( (alarm_name >= FIRST_SAFETY_ALARM) && (alarm_name < NULL_ALARM ))
   {
      char szLogOutput[MAX_CHAR / 2];
      szLogOutput[0] = 0x0;

      AlarmStruct& alarm = _alarmTable[alarm_name];

      sprintf(szLogOutput, "Alarm : %d %s ", (int)alarm_name, alarm.getDescription());

      if (_alarmState != SAFETY_ALARM_ASSERTED)
      {
         _alarmState = SAFETY_ALARM_ASSERTED;
         _safetyAlarmRepeater.enableAlarmRepeater((ALARM_VALUES)alarm_name, szLogOutput);
      }

   } //    if( alarm_name >= FIRST_SAFETY_ALARM  && alarm_name < NULL_ALARM )
   else
   {
      DataLog(log_level_critical) << "Alarm : "
                                  << (int)alarm_name
                                  << " outside safety defined alarms"
                                  << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "invalid alarm_name");

   } // SW Fault got safety alarm name outside safety defined alarms

}


// SPECIFICATION:    set pump power
//                   issues orders to enable/disable 24v power.
//                   won't allow power if an alarm is asserted or
//                   if power has been blocked due to excessive
//                   time paused or stopped
//
// ERROR HANDLING:   LOG.

void aSafetyHardwareCommandsExec::SetPumpPower (HW_ORDERS ucCmd)
{

   if ( ( _alarmState != SAFETY_ALARM_ASSERTED ) && !_blockPower)
   {
      DataLog(log_level_safe_exec_info) << "SetPumpPower Old State : "
                                        << (int)_SHwOrdersData.pumpPower << " New State: " << (int)ucCmd << endmsg;

      _SHwOrdersData.pumpPower = ucCmd;
      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetPumpPower(" << (int)ucCmd << ") #1 sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.pumpPower == HW_DISABLE ? "pumpPwrDISABLE" :  _SHwOrdersData.pumpPower == HW_ENABLE ? "pumpPwrENABLE" : "pumpPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();
   }
   else if ( ( ( _alarmState == SAFETY_ALARM_ASSERTED) || _blockPower) &&
             (_SHwOrdersData.pumpPower != HW_DISABLE) )
   {
      _SHwOrdersData.pumpPower = HW_DISABLE;
      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetPumpPower(" << (int)ucCmd << ") #2 sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.pumpPower == HW_DISABLE ? "pumpPwrDISABLE" :  _SHwOrdersData.pumpPower == HW_ENABLE ? "pumpPwrENABLE" : "pumpPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();

      DataLog(log_level_safe_exec_info) << "Block Power: " << _blockPower << " FORCE Pump Power to HW_DISABLE " << endmsg;
   }
}

// SPECIFICATION:    block pump power
//                   if 24v or 64v power is on, turns them
//                   off and blocks them from being turned
//                   on again.
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::BlockPumpPower (bool state)
{

   // block any incoming requests for power
   _blockPower = state;

   // and turn it off if it is on
   if (_blockPower)
   {
      _SHwOrdersData.pumpPower       = HW_DISABLE;
      _SHwOrdersData.centrifugePower = HW_DISABLE;

      send();

      DataLog(log_level_safe_exec_info) << "Block Power: " << _blockPower << " Disabling Power." << endmsg;
   }
}

// SPECIFICATION:    set centrifuge power
//                   issues orders to enable/disable 64v power.
//                   won't allow power if an alarm is asserted or
//                   if power has been blocked due to excessive
//                   time paused or stopped
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::SetCentrifugePower (HW_ORDERS ucCmd)
{
   if ( (_alarmState != SAFETY_ALARM_ASSERTED) && !_blockPower)
   {
      DataLog(log_level_safe_exec_info) << " SetCentrifugePower Old State : "
                                        << (int)_SHwOrdersData.centrifugePower << " New State: " << (int)ucCmd << endmsg;

      _SHwOrdersData.centrifugePower = ucCmd;
      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetCentrifugePower(" << (int)ucCmd << ") #1 sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.centrifugePower == HW_DISABLE ? "centPwrDISABLE" :  _SHwOrdersData.centrifugePower == HW_ENABLE ? "centPwrENABLE" : "centPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();
   }
   else if ( ( ( _alarmState == SAFETY_ALARM_ASSERTED) || _blockPower) &&
             (_SHwOrdersData.centrifugePower != HW_DISABLE) )
   {
      _SHwOrdersData.centrifugePower = HW_DISABLE;

      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetCentrifugePower("
                                         << (int)ucCmd << ") #2 sending SHwOrdersMsg(" <<
         ( _SHwOrdersData.centrifugePower == HW_DISABLE ? "centPwrDISABLE" :  _SHwOrdersData.centrifugePower == HW_ENABLE ? "centPwrENABLE" : "centPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();

      DataLog(log_level_safe_exec_info) << "FORCE Centrifuge Power to HW_DISABLE " << endmsg;
   }
}

// SPECIFICATION:    set both power
//                   issues orders to enable/disable 24v and 64v power.
//                   won't allow power if an alarm is asserted or
//                   if power has been blocked due to excessive
//                   time paused or stopped
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::SetBothPower (HW_ORDERS ucCmd)
{
   if ( ( _alarmState != SAFETY_ALARM_ASSERTED ) && !_blockPower)
   {
      DataLog(log_level_safe_exec_info) << "Pump Pwr Old: "
                                        << (int)_SHwOrdersData.pumpPower << " New: " << (int)ucCmd
                                        << ", Cent Pwr Old: " << (int)_SHwOrdersData.centrifugePower
                                        << " New: " << (int)ucCmd << endmsg;

      _SHwOrdersData.pumpPower       = ucCmd;
      _SHwOrdersData.centrifugePower = ucCmd;
      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetBothPower(" << (int)ucCmd << ") #1 sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.pumpPower == HW_DISABLE ? "pumpPwrDISABLE" :  _SHwOrdersData.pumpPower == HW_ENABLE ? "pumpPwrENABLE" : "pumpPwrUNKNOWN" )
                                         << " "
                                         << ( _SHwOrdersData.centrifugePower == HW_DISABLE ? "centPwrDISABLE" :  _SHwOrdersData.centrifugePower == HW_ENABLE ? "centPwrENABLE" : "centPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();
   }
   else if ( ( ( _alarmState == SAFETY_ALARM_ASSERTED) || _blockPower) &&
             ( (_SHwOrdersData.pumpPower != HW_DISABLE) ||
               (_SHwOrdersData.centrifugePower != HW_DISABLE) )
             )
   {

      _SHwOrdersData.pumpPower       = HW_DISABLE;
      _SHwOrdersData.centrifugePower = HW_DISABLE;

      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetBothPower("
                                         << (int)ucCmd << ") #1 sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.pumpPower == HW_DISABLE ? "pumpPwrDISABLE" :  _SHwOrdersData.pumpPower == HW_ENABLE ? "pumpPwrENABLE" : "pumpPwrUNKNOWN" )
                                         << " "
                                         << ( _SHwOrdersData.centrifugePower == HW_DISABLE ? "centPwrDISABLE" :  _SHwOrdersData.centrifugePower == HW_ENABLE ? "centPwrENABLE" : "centPwrUNKNOWN" )
                                         << ")" << endmsg;

      send();

      DataLog(log_level_safe_exec_info) << "FORCE Pump and Centrifuge Power to HW_DISABLE " << endmsg;
   }
}

// SPECIFICATION:    set door solenoid power
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::SetDoorPower (HW_ORDERS ucCmd)
{
   if ( _SHwOrdersData.doorSolenoidPower != ucCmd )
   {
      DataLog(log_level_safe_exec_info) << "SetDoorPower Old State : "
                                        << (int)_SHwOrdersData.doorSolenoidPower << " New State: " << (int)ucCmd << endmsg;

      _SHwOrdersData.doorSolenoidPower = ucCmd;

      DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetDoorPower("
                                         << (int)ucCmd << ") sending SHwOrdersMsg("
                                         << ( _SHwOrdersData.doorSolenoidPower == HW_DISABLE ? "doorSolDISABLE" :  _SHwOrdersData.doorSolenoidPower == HW_ENABLE ? "doorSolENABLE" : "doorSolUNKNOWN" )
                                         << ")" << endmsg;

      send();
   }
}


void aSafetyHardwareCommandsExec::SetAlarmLight (HW_ORDERS ucCmd)
{
   _SHwOrdersData.alarmLight = ucCmd;
   DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetAlarmLight(" << (int)ucCmd << ") sending SHwOrdersMsg("
                                      << ( _SHwOrdersData.alarmLight == HW_DISABLE ? "alrmLiteDISABLE" :  _SHwOrdersData.alarmLight == HW_ENABLE ? "alrmLiteENABLE" : "alrmLiteUNKNOWN"  )
                                      << ")" << endmsg;

   send();
}


void aSafetyHardwareCommandsExec::SetAlarmState (bool alarmState)
{
   _alarmState = alarmState;
}


// SPECIFICATION:    set donor mode
//                   used to tell the safety driver that a donor is
//                   connected which causes the driver to start
//                   air-to-donor monitoring if it hasn't started it
//                   already
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::SetDonorMode (HW_ORDERS mode)
{
   _SHwOrdersData.donorConnectMode = mode;

   DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetDonorMode("
                                      << (int)mode << ") sending SHwOrdersMsg("
                                      << ( _SHwOrdersData.donorConnectMode == HW_DISABLE ? "donorConnDISABLE" :  _SHwOrdersData.donorConnectMode == HW_ENABLE ? "donorConnENABLE" : "donorConnUNKNOWN" )
                                      << ")" << endmsg;

   send();

   SafetyPFRDataToSave.PFRdonorConnectMode = _SHwOrdersData.donorConnectMode;
}

void aSafetyHardwareCommandsExec::SetSubstate (long newState)
{
   _SHwOrdersData.test1 = newState;
   DataLog(log_level_safe_exec_debug) << "aSafetyHardwareCommandsExec :: SetSubstate(" << (State_names)newState
                                      << ")" << endmsg;

   send();
}

// SPECIFICATION:    initialize for state
//                   forces assumptions about the donor mode according to state
//
// ERROR HANDLING:   none.

void aSafetyHardwareCommandsExec::initializeForState (long newState, HW_CASSETTE_STATES cassettePosition)
{

   // just to be on the safe side, tell the driver to do air2donor
   // it has probably already been triggered by events in the machine
   switch (newState)
   {
      case DISPOSABLE_TEST :
      case AC_CONNECTED :
      case AC_PRIME :
      case METERED_STORAGE :
      case METERED_STORAGE_DISCONNECT :
         SetDonorMode(HW_DISABLE);
         break;
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
         SetDonorMode(HW_ENABLE);
         break;
      case DONOR_DISCONNECT :
         SetDonorMode(HW_ENABLE);
         SetBothPower(HW_DISABLE);
         // allow the power to be enabled if we are in donor disconnect
         // even if it has been more than the need coagulations time
         if (_blockPower)
         {
            DataLog(log_level_safe_exec_info) << "Block Power: " << _blockPower << " Pump power allowed for disconnect" << endmsg;
         }

         _blockPower = false;
         break;

      default :
         // if we are in PFR and the donor mode is enabled,
         // don't override it unless we are trying to enable
         // air-to-donor monitoring with the cassette up
         if (SafetyPFRDataFromRestore.PFRSafetyOneShot)
         {
            if ( (SafetyPFRDataFromRestore.PFRdonorConnectMode == HW_ENABLE)
                 &&(SafetyPFRDataFromRestore.PFRCassette_position != HW_CASSETTE_DOWN) )
            {
               SetDonorMode(HW_DISABLE);
               DataLog(log_level_safe_exec_info) << "PFR Donor Connect Mode: "
                                                 << SafetyPFRDataFromRestore.PFRdonorConnectMode
                                                 << " Cassette not down, Disable Donor Mode"
                                                 << endmsg;
            }
            else
            {
               SetDonorMode(SafetyPFRDataFromRestore.PFRdonorConnectMode);
               DataLog(log_level_safe_exec_info) << "PFR Donor Connect Mode: "
                                                 << SafetyPFRDataFromRestore.PFRdonorConnectMode
                                                 << " Sticky Donor Mode"
                                                 << endmsg;
            }
         }
         else
         {
            if (cassettePosition == HW_CASSETTE_DOWN)
               SetDonorMode(HW_ENABLE);
            else
               SetDonorMode(HW_DISABLE);
         }
         break;
   }

   SetSubstate(newState);
}



void aSafetyHardwareCommandsExec::send (void)
{
   _SHwOrdersData.sequenceNumber = ++_msgCounter;
   _theSHwOrdersMsg.send(_SHwOrdersData);
}



aSafetyAlarmRepeater::aSafetyAlarmRepeater()
   : _paSafetyAlarmMsg_sndr(),
     _myTimer(2000ul, Callback<aSafetyAlarmRepeater>(this, &aSafetyAlarmRepeater::notify), TimerMessage::ARMED),
     _alarmActive(false),
     _repeatAlarm(NULL_ALARM)
{
   memset(&_alarmText, 0, MAX_CHAR);
}



void aSafetyAlarmRepeater::enableAlarmRepeater (ALARM_VALUES alarm_name, const char* alarmText)
{
   _alarmActive = true;
   _repeatAlarm = alarm_name;
   strncpy(_alarmText, alarmText, MAX_CHAR);

   // post it immediately
   _paSafetyAlarmMsg_sndr.postAlarm(_repeatAlarm);
   DataLog(log_level_safety_alarm) << _alarmText << endmsg;
}




void aSafetyAlarmRepeater::notify ()
{
   if (_alarmActive)
   {
      _paSafetyAlarmMsg_sndr.postAlarm(_repeatAlarm);

      DataLog(log_level_safety_alarm) << "Repeating: " << _alarmText << endmsg;
   }
}

/* FORMAT HASH 422bf40eda6c555c36727523466c7bce */
