/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/safety/rcs/s_exec.cpp 1.109 2008/05/27 21:21:24Z spriusb Exp $
 * $Log: s_exec.cpp $
 * Revision 1.109  2008/05/27 21:21:24Z  spriusb
 * Fix for IT6383: Using new timestamp field to mitigate risk from latency/purtibation in messaging system time.
 * Revision 1.108  2006/07/21 15:54:38Z  jheiusb
 * 7634 -- disregard "air to donor" if no donor.
 * Revision 1.107  2003/12/05 00:17:56Z  ms10234
 * 6658 - only check for moving centrifuge if we have tried to command 64V or 64V is on according to control.
 * Revision 1.106  2003/12/02 22:24:22Z  ms10234
 * 6637 - verify centrifuge/door state whether or not we think power is on.
 * Revision 1.105  2003/07/21 21:56:29Z  td07711
 * IT6157 - logging change
 * Revision 1.104  2003/07/07 21:04:34Z  jl11312
 * - implemented APS auto-zero
 * Revision 1.103  2003/07/01 22:34:43Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.102  2003/04/29 15:57:00Z  ms10234
 * Added some useful logging for the cases when safety doesn't come up.
 * Revision 1.101  2003/04/04 00:13:09Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.100  2003/01/10 18:25:18Z  jl11312
 * - changed check for message latency
 * Revision 1.99  2003/01/09 20:25:19  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.98  2002/12/18 08:18:35  pn02526
 * Checkins for first operating safety executive under vxWorks
 * Revision 1.97  2002/10/11 09:24:40  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.96  2002/08/07 14:46:10  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Add aSafetyStatusMsg_rcvr :: _theSHwStatesMsg to use the new buffered message class definition, rather than deriving from a buffered message base class.
 * Add aControlCommandsMsg_rcvr :: _theCHwOrdersMsg to use the new buffered message class definition, rather than deriving from a buffered message base class.
 * Add aControlStatusMsg_rcvr :: _theCHwStatesMsg to use the new buffered message class definition, rather than deriving from a buffered message base class.
 * Add aSafetyCommandsMsg_rcvr :: _theSHwOrdersMsg to use the new buffered message class definition, rather than deriving from a buffered message base class.
 * Use message system's definitions of pid (process a.k.a. task Id) and nid (node Id).
 * Use message system methods to fetch pid, nid, and sent time of messages.
 * Use the new buffered message base class definition to send the AnswerSafetyAliveMsg, rather than deriving class anAnswerSafetyAliveMsg_sndr from it.
 * Use the new buffered message base class definition to send the RequestSafetyStatusMsg, rather than deriving class aRequestSafetyStatusMsg_sndr from it.
 * Revision 1.95  2002/07/23 16:52:26  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.94  2002/04/11 14:53:13  pn02526
 * "Revise logging to use DataLogLevel class.  Change ASSERT() macro calls to trima_assert()."
 * Revision 1.93  2001/07/18 21:57:35Z  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.92  2001/07/18 17:10:12  ms10234
 * 4712 - Fixed return/draw switching problem
 * Revision 1.91  2001/04/03 15:03:01  ms10234
 * Add logging to debug startup glitch
 * Revision 1.90  2000/11/01 19:12:17  bs04481
 * Change alarm for delivery latency of safetyStatus messages to
 * 2.19 sec.  Log latencies above 1sec.
 * Revision 1.89  2000/11/01 18:57:52  bs04481
 * Alarm if SafetyStatus delivery latency is greater than 2 sec.  Log 
 * latencies about 500ms.  Handle AC Pump Runaway event from
 * the driver.
 * Revision 1.88  2000/10/27 14:21:17  bs04481
 * Measure the delivery latency for safety status messages.  For the
 * time being, log if above 250msec.  Alarm is temporarily disabled.
 * Revision 1.87  2000/07/20 19:32:43  bs04481
 * Change function name to be more representative.
 * Revision 1.86  2000/05/26 17:12:10  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the 
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down 
 *      the on unnecessary stuff in the message que.
 * Revision 1.85  2000/03/15 18:17:57  BS04481
 * Remove cross-referencing between message handlers by 
 * pushing the work back into the objects instead of the message 
 * handlers; use new needle monitor to monitor pumps when 
 * the needle is un-coagulated;  use new proc operation 
 * monitor to monitor commanded state of the machine for 
 * correct operation during pause and stop; remove obsolete
 * defines, methods and message handlers
 * Revision 1.84  1999/09/14 19:27:56  TD10216
 * IT4333
 * Revision 1.83  1999/08/31 18:22:12  BS04481
 * Remove unnecessary printfs
 * Revision 1.82  1999/07/20 19:45:33  BS04481
 * Match the time measurement between messages one-to-one with
 * evaluation the of return  pump speed.  Formerly, time measurements
 * were occasionally done more often than speed evaluation.
 * Revision 1.81  1999/06/06 17:22:14  BS04481
 * Start 10minute timer any time the pumps stop and the needle may
 * contain un-anticoagulated blood.
 * Revision 1.80  1999/06/04 22:58:15  BS04481
 * Change calculation for delta time using control status header 
 * information to only happen if there is valid pump data in the
 * control status message.
 * Don't evaluate return pump speed or AC infusion during rinseback
 * if the delta time between message is less than 20ms (avoid 
 * reliability problem if messages stack up)
 * Finally, use control status message delta time when evaluating
 * AC infusion during Rinseback.
 * Revision 1.79  1999/05/13 15:43:40  BS04481
 * Change calculation of return pump speed to use the timestamp from
 * the control side message while calculating delta time.  Add logging
 * of the speed as it is calculated using delta time from the safety
 * side to attempt to detect messaging problems.
 * Revision 1.78  1999/04/06 18:32:52  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor 
 * Disconnect
 * Revision 1.77  1998/11/12 18:24:41  bs04481
 * If we power up with the cassette down, ask the reservoir if it knows
 * the cassette type.  If not, default it to unknown.   The PFR data
 * may be restored later to over-write this but for the first few samples,
 * we will assume white.
 * Revision 1.76  1998/10/20 20:43:47  bs04481
 * Change power fail recovery to restore PF data and result all
 * active monitoring immediately after Safety determines the 
 * procedure is recoverable.
 * Add all donor data to PFR data set instead of just TBV
 * Do an exact TBV comparison of donor vital data received during
 * PFR with previously restored PFR data even if TBVLockOut 
 * is in place
 * Revision 1.75  1998/09/28 22:21:51  bs04481
 * Remove RequestSafetyPowerMsg_rcvr from these files and move
 * it to s_disc.cpp and s_disc.hpp
 * Revision 1.74  1998/08/11 00:34:39  bs04481
 * Clean up disconnect test timers for situations where Procedure
 * restarts the disconnect test unexpectedly
 * Revision 1.73  1998/08/10 22:49:55  bs04481
 * Fixed logic error (introduced in previous revision) which caused 
 * the fail flag to be set in the disconnect test after it had passed.
 * Revision 1.72  1998/07/21 15:23:29  bs04481
 * More disconnect test changes for release 3.2.  Also, restructured
 * disconnect test as a state machine to help with maintanence.
 * Revision 1.71  1998/06/19 00:05:38  bs04481
 * Do not constrain the cassette when entering the donor disconnect 
 * if the cassette is not completely down.
 * Revision 1.70  1998/06/09 17:47:15  bs04481
 * Disconnect test changes.  
 * Revision 1.69  1998/05/12 19:47:19  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design. 
 * Previously, the volumes were accumulated in the s_res code.  
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the 
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.68  1998/03/10 01:01:18  bs04481
 * Debounce the hardware centrifuge error to 15 counts before alarming.
 * Revision 1.67  1998/02/27 07:25:56  bs04481
 * Change disconnect test
 * Revision 1.66  1997/12/26 23:08:31  bs04481
 * Create boolean flags to signal when both drivers have checked
 * in and have their Hall and Encoder offsets reset.  Do not compare
 * halls vs encoders until both offsets are reset.
 * Revision 1.65  1997/12/23 02:33:52  bs04481
 * Restore call to SnapShotCycleRevError.
 * Revision 1.64  1997/12/17 21:55:33  bs04481
 * Move setting accumulated halls offset to the first sample instead 
 * of adding to each reading.
 * Also, ensure both drivers are sending us messages before
 * comparing halls vs encoders.
 * Revision 1.63  1997/12/12 17:13:45  bs04481
 * Start a pump power timer if the stop or start button is pushed  
 * during Blood Run or Blood Rinseback and there is not AC in 
 * the needle
 * Revision 1.62  1997/12/02 15:59:53  bs04481
 * Changes to implement power fail recovery
 * Revision 1.61  1997/07/29 16:00:16  SM02805
 * Changed reference to STARTUP_PRELOAD 
 * state to STARTUP_TESTS to support use of 
 * procedure states.h file to prevent future maintenance
 * problems.
 * Revision 1.60  1997/05/24 05:32:56  bs04481
 * Change to allow 64v on with the door open if we are in STARTUP_PRELOAD state.
 * Revision 1.59  1997/05/15 22:50:06  SM02805
 * Fixed bug in door power T1 : diable on init, do not enable until
 * 24V T1 is completed.
 * Revision 1.58  1997/05/15 00:46:37  SM02805
 * 1. Implemented three point APS calibration.
 * 2. Implement centrifuge error monitoring and alarm gen.
 * 3. Implemented forbidden service mode detection monitoring and
 * alarm gen.
 * 4. Implement monitoring and alarm gen for safety command msg
 * detection for msgs that did not originate from saf_exec process.
 * 5. Implement T1 door power : hold off door power enable until
 * after 24V T1 passed.
 * Revision 1.57  1997/05/13 23:34:47  bs04481
 * Remove safety error handling of end procedure message.  Will reboot instead.
 * Revision 1.56  1997/04/09 19:56:47  bs04481
 * Added yet more logging to catch disconnect test failures after
 * power off.  Also lowered pressure limit to 95.
 * Revision 1.55  1997/03/03 15:22:26  bs04481
 * Revision 1.55  1997/02/28 23:07:53  bs04481
 * Remove air-to-donor monitoring from the safety executive
 * Revision 1.54  1997/02/11 18:00:36  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.53  1997/02/03 20:11:42  TM02109
 * Removed air to donor at saf_exec for rinseback and donor disconnect :
 * safe driver maintains it during these states.
 * Revision 1.52  1997/02/03 19:57:14  TM02109
 * Leave air to donor in for rinseback and donor disconnect test, test 
 * amount of pump movement against 1/2 rev or 2 halls.
 * Revision 1.51  1997/02/03 19:52:56  TM02109
 * Remove saf_exec air to donor monitoring for rinseback and donor disconnect 
 * test.
 * Revision 1.50  1997/02/03 16:08:24  TM02109
 * Fixed bug in air to donor prevention prior to first fluid detection when 
 * saf_exec is responsible for monitoring.
 * Revision 1.49  1997/01/27 23:54:37  TM02109
 * Modify false echo filter for reservoir.  If res goes middle to low to 
 * middle to low, reset false echo filter ( == 5 ). Must have 5
 * consecutive indications of middle before another low indication
 * would switch cycle to draw for safety monitoring.Previously, did
 * not zero filter for lows following middles.
 * Revision 1.48  1997/01/24 20:33:55  TM02109
 * Modify implementation of cycle rev error for return and inlet pumps.
 * Measure and monitor delat rev error per cycle against cycle limit.
 * Revision 1.47  1997/01/23 23:05:59  TM02109
 * Fixed bug for inlet rev error logging not being done.
 * Revision 1.46  1997/01/23 17:40:41  TM02109
 * Fix bug where halls and encoders were not being updated for
 * inlet cycle monitor in propoer place ( s_exec for halls, s_res for
 *  encoders)
 * Revision 1.45  1997/01/23 16:56:13  TM02109
 * Moved cycle rev error reset ( zeroing ) from s_res to s_exec so
 * log output would display cycle rev error properly for both pumps.
 * Revision 1.44  1997/01/23 14:56:52  SM02805
 * Modify to call new methods for inlet pump rev error monitoring
 * ( accumulated and cycle based ).
 * Revision 1.43  1997/01/17 14:55:37  TM02109
 * Added a cycle transition variable to the reservoir class to allow
 * sync for logging of return pump rev error data when cycle switches
 * from reutrn to draw.  s_exec updates rev error monitoring after
 * the cycle is switched. Not syncing caused the rev error logged
 * to disagree with the revs. Encoders had been updated, halls
 * had not when retport is done in reservoir code that switcheds cycle.
 * Revision 1.42  1997/01/17 04:04:28  TM02109
 * Modifications for new rev error monitoring for return pump. Had 
 * bugs in previous attempt for single return cycle rev error calcs.
 * Revision 1.41  1997/01/16 20:16:47  SM02805
 * Modify return pump rev error monitoring as per 01/15/97 safety 
 * meeting as documented in PPQA memo dated 01/16/97.
 * Revision 1.40  1997/01/07 18:16:52  TM02109
 * Added inlet pump RPM test to pause test.
 * Revision 1.39  1997/01/07 18:14:26  TM02109
 * Revision 1.38  1997/01/07 01:29:57  SM02805
 * Revision 1.37  1997/01/06 23:08:22  SM02805
 * Made modifications to :
 * 1. detect paused condition resulting from situations
 *  other than pause key presses ( "procedural pauses" ).
 * 2.  Problems resulting from reservoir fillup secondary 
 * to cent shutdown following 10 minute pauses ( AC OFF During Draw ) .  
 * Revision 1.36  1996/12/27 15:53:33  TM02109
 * Disabled ( commented out ) delta time ( encoder snapshot to hall snapshot )
 * logging for pump monitoring to see if loading is a problem.
 * Revision 1.35  1996/12/21 00:04:37  TM02109
 * Update pump monitoring calls individual methods for rev error calcs for
 * each pump to support impl of new rev error tolerances for each pump.
 * Revision 1.34  1996/12/20 02:05:01  bs04481
 * Added logging of the time difference between when the command
 * status (with the encoder info) and the safety status (with the halls
 * info) are received.  This is temporary to help collect data on the 
 * halls discrepency.
 * Revision 1.33  1996/12/13 18:17:53  bs04481
 * Added default cases for forbidden switches with FATAL_ERROR
 * in three places.
 * Revision 1.32  1996/12/09 18:38:07  TM02109
 * Made modifications correct early switching bug for reservoir fillup
 * following centrifuge shutdown.  The cycle is not switched to return
 * until re-start even if reservoir fills due to expressed fluid from cent.
 * Revision 1.31  1996/12/04 00:33:33  SM02805
 * 1. Modify maximum allowable centrifuge speed limit from 3090 RPM 
 * to 3200 RPM pursuant to outstanding safety action item from reviews.
 * 2. Modifications to remove valve position timing from safety.
 * Revision 1.30  1996/11/28 00:01:33  SM02805
 * D/C all reservoir and infusion blinding for centrifuge shutdown.
 * Revision 1.29  1996/10/25 21:25:48  SM02805
 * Remove a debugprintf statement from answer safety alive.
 * Revision 1.28  1996/10/01 17:13:14  SM02805
 * Implement a four consecutive sample filter for "door" alarms per tim
 * gordon's recommendations. Safety will allow a fault tolerance of four
 * consectuive safety status samples before generating the safety door
 * alarms that results in procedural shutdown. Crtiteria for alarm generation
 * is four consecutive indications of door NOT closed and locked AND
 * centrifuge RPM > 60 AND centrifuge power enabled.
 * Revision 1.27  1996/09/22 20:34:33  tm02109
 * Alarm when you recieve the end procedure message.
 * Revision 1.26  1996/09/22 18:09:54  SM02805
 * 1. Respond to 09/19/96 code review action items.
 * 2.Implement reset of reservoir and infusion blinding counter for
 * all stop spin actions during run ( previously counter was not
 * reset if subsequnet stop/start occurred while blinding was
 * counting down ).
 * Revision 1.25  1996/09/17 22:05:20  SM02805
 * 09/17/96 marlene resolution to infusion calculation use return
 * to return cycle for infusion calc.
 * Revision 1.24  1996/09/16 17:18:52  SM02805
 * Remove TCP/IP references.
 * Revision 1.23  1996/09/16 18:13:46  SM02805
 * Remove all TCP/IP references.
 * Revision 1.22  1996/09/16 18:13:46  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.21  1996/09/16 18:13:46  SM02805
 * xfunc resolutions 09/12/96 :
 * 1. Implement air to donor protective monitoring for states
 * AC_CONNECTED to BLOOD_RUN, RINSEBACK and DONOR
 * DISCONNECT.  Low level protection requries fluid at low level
 * sensor to initiate.
 * Revision 1.20  1996/09/16 18:13:46  SM02805
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
 * Revision 1.18  1996/09/04 19:28:22  SM02805
 * Modify rcvr for query safety alive to clear _cAlarmState at positive
 * response to safety alive query - this flag when asserted prevents
 * power enabling messages from bening sent to the safety driver.
 * Revision 1.17  1996/08/21 22:36:05  SM02805
 * Modify implementation of donor discoonect test : test would
 * not work if starting pressure was > +80 mmHg.  Modify algo
 * for going low and holding low parts of test.
 * Revision 1.16  1996/08/14 12:16:48  SM02805
 * Instrument for first fluid detection testing.
 * Revision 1.15  1996/08/13 18:50:35  SM02805
 * Provided for reservoir and infusion monitoring lockout of
 * 2 "half cycles" following centrifuge shutdown during run. Requried
 * modifications to the control commands msg notify function to
 * switch blackout on when centrifuge commanded to 0 during
 * run.
 * Revision 1.14  1996/07/31 02:07:06  SM02805
 * Revision 1.13  1996/07/31 02:07:06  SM02805
 * Found bug : inlet commanded RPM was not being sent to
 * inlet pump object making stoke volume correction in error.
 * Revision 1.12  1996/07/30 03:57:48  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.11  1996/07/25 23:40:03  SM02805
 * Moved safety status ping to immediately follow control
 * status "get" if event is "routine" update of control status.  Goal :
 * minimize delay between control encoder readings and hall readings.
 * Revision 1.10  1996/07/25 03:47:24  SM02805
 * Fixed bug for pump monitoring on platelet pump : init of the "offset"
 * for the halls did not include the divide by 2 for platelet rotor mags.
 *
 * Revision 1.9  1996/07/24 17:15:48  SM02805
 * Gate call to _paReservoir->update_res_mon() on _cCycleMon
 * != SN_MON_INACTIVE to address(?) bug in hypovolemia detection
 * at startup.  EC limit has not been calculated "yet" and SN monitoring
 * is not active yet.
 * Revision 1.8  1996/07/24 13:56:58  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 * Revision 1.7  1996/07/22 17:16:25  SM02805
 *
 * TITLE:      s_exec.cpp
 *             Safety Executive class implementations.
 *
 *
 * ABSTRACT:   This file contains the class methods for the message handlers
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/
#include "trima_datalog.h"
#include "s_exec.hpp"
#include "s_glob.hpp"
#include "s_disc.hpp"
#include "s_power.hpp"
#include "s_dev.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;

// SPECIFICATION:    notify for safety status msg receiver
//
//
// ERROR HANDLING:   various alarms, fatal error.

void aSafetyStatusMsg_rcvr :: notify()
{
   SHwStateData = _theSHwStatesMsg.getData();

   // if we are in donor connect mode, check the message delivery 
   // latency on this message type. this message needs to be delivered 
   // within 500ms of being sent.
   deliveryTime();

   // update centrifuge status
   _paCentrifuge->Update( SHwStateData.centrifugeRPM
                          ,SHwStateData.centrifugeError);

   // update door status
   _paDoor->Update(SHwStateData.doorLocks);
   // 06/27/96 when spin centrifuge by hand, can exceed 60 RPM so also gate this alarm with
   // state vbl indicating if safety has enabled 64V power( based on last safety command ).
   if ( paSafetyHardwareCommandsExec->GetCentrifugePower() == HW_ENABLE )
      _paDoor->UpdateAndTestDoorFault();

   // update cassette status
   _paCassette->Update(SHwStateData.cassettePosition);

   // update reservoir status
   _paRes->UpdateReservoirSafety(SHwStateData.reservoir);

   // check blink test status
   _blink->Update(SHwStateData.valveLedTest);

   // check for service mode
   checkForbiddenMode(SHwStateData.serviceEnableMode);


   switch ( SHwStateData.event )
   {
   case SHW_NORMAL_UPDATE:

      // update the hall data and compare to encoders
      _paRes->UpdateSafety(SHwStateData);

      break; 

   case SHW_PAUSE_EVENT:

      _paPauseSwitch->SetSwitchTimerStatus( TIMER_ARMED );

      // if there is not AC in the needle, start a pump power timer
      if ( _paRes->shouldWeWorry() == TRUE )
         _needle->turnOnTimer(PUMP_MAX_DURATION);

      break;  

   case SHW_STOP_EVENT:

      _paStopSwitch->SetSwitchTimerStatus( TIMER_ARMED );     

      // if there is not AC in the needle, start a pump power timer
      if ( _paRes->shouldWeWorry() == TRUE )
         _needle->turnOnTimer(PUMP_MAX_DURATION);

      break;  

   case SHW_DOOR_EVENT:

      break;  

   case SHW_RESERVOIR_EVENT:
      // check to make sure first fluid is during Blood Prime
      _paRes->firstFluid();

      break;  

   case SHW_RETURN_EVENT:

      break; 

   case SHW_AIR_EVENT:
      if (_paDonor->GetDonorConnectedState() == DONOR_IS_CONNECTED )
      {
           paSafetyHardwareCommandsExec->respond_to_alarm( OFF_BOTH, DONOR_AIR_ALARM );
      } else {
          DataLog(log_level_safe_exec_info) << "Safety saw SHW_AIR_EVENT with no donor connected" << endmsg;
      }
      break; 

   case SHW_RETURN_PUMP_TOO_FAST_EVENT:

      paSafetyHardwareCommandsExec->respond_to_alarm( OFF_BOTH, RETURN_PUMP_HALL_TOO_BIG );

      break; 

   case SHW_AC_PUMP_TOO_FAST_EVENT:

      paSafetyHardwareCommandsExec->respond_to_alarm( OFF_BOTH, AC_PUMP_HALL_TOO_BIG );

      break; 

   default:
      {
          DataLog( log_level_critical ) << "Undefined SHwStateData.event: "
                                << (int)SHwStateData.event
                                << "  causes forbidden case switch default"
                                << endmsg;
          _FATAL_ERROR( __FILE__, __LINE__, "invalid event type");
      }

      break;

   }

   ++_iSafetyStatusCount;

} // END aSafetyStatusMsg_rcvr :: notify()


// SPECIFICATION:    calculates elapsed time between message sent
//                   and message delivery for safety status msg receiver.
//                   in-sensitive to setting of the real-time clock
//
//
// ERROR HANDLING:   SW Fault alarm

void aSafetyStatusMsg_rcvr :: deliveryTime(void)      
{
   // don't test if the donor is not connected.  During initialization
   // of tasks, especially during cal.dat file read, messages can be delayed.
   if ( SHwStateData.donorConnectMode == HW_DISABLE )
      return;

	// alarm if we exceed the maximum allowed latency.
   // log it if we get close.        
   unsigned int elapsedTime = _theSHwStatesMsg.latency();
   if ( elapsedTime > MAX_SAFETY_MESSAGE_LATENCY )
   {
      paSafetyHardwareCommandsExec->respond_to_alarm( OFF_BOTH, SW_FAULT);
      DataLog(log_level_safety_alarm_detail) << "SafetyStatusMsg delivery time of "
                                << elapsedTime
                                << " exceeds 500ms."
                                << endmsg;
   }
   else if ( elapsedTime > LOG_SAFETY_MESSAGE_LATENCY )
   {
       DataLog(log_level_safe_exec_info) << "SafetyStatusMsg delivery time = " << elapsedTime << endmsg;
   }
}


// SPECIFICATION:    notify for proc to safety query alive msg.
//
//
// ERROR HANDLING:   none.

void aQuerySafetyAliveMsg_rcvr :: notify()
{
   if ( (_paSafetyStatusMsg_rcvr->GetSafetyStatusCount() != 0)
        &&(_paControlCommandsMsg_rcvr->GetControlCommandsCount() != 0)
        &&(_paControlStatusMsg_rcvr->GetControlStatusCount() != 0) )
   {
       // OK to respond to ping
       DataLog(log_level_safe_exec_debug) << "aQuerySafetyAliveMsg_rcvr :: notify() sending AnswerSafetyAliveMsg(TRUE)" << endmsg;
      _panAnswerSafetyAliveMsg_sndr->send( (long)TRUE );

      // tell system alarm state is not asserted
      paSafetyHardwareCommandsExec->SetAlarmState( NO_SAFETY_ALARM );

      // turn alarm light off
      paSafetyHardwareCommandsExec->SetAlarmLight( HW_DISABLE );

      // 05/15/97 door power T1, init with disable
      paSafetyHardwareCommandsExec->SetDoorPower( HW_DISABLE );

      // Let proc know status of safety PFR file, since we know at this point
      // that proc must be running.
      if ( !_PFRStatusMsgSent )
      {
         _paPFR_Safety->SendPFRStatusMsg();
         _PFRStatusMsgSent = TRUE;
      }
      
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "Safety Status Message Count: "
                      << _paSafetyStatusMsg_rcvr->GetSafetyStatusCount()
                      << endmsg;
      DataLog(log_level_safe_exec_info) << "Control Commands Message Count: "
                      << _paControlCommandsMsg_rcvr->GetControlCommandsCount()
                      << endmsg;
      DataLog(log_level_safe_exec_info) << "Control Status Message Count: "
                      << _paControlStatusMsg_rcvr->GetControlStatusCount()
                      << endmsg;
   }

} // END void aQuerySafetyAliveMsg_rcvr :: notify()


// SPECIFICATION:    notify for control orders msg rcvr.
//
//
// ERROR HANDLING:   none.

void  aControlCommandsMsg_rcvr :: notify()
{
   CHwOrderData = _theCHwOrdersMsg.getData();

   // update data for command monitor
   _paProcOpMonitor->Update(CHwOrderData);

   // update to get ready to calculate stroke volume.
   // we'll calculate it at the inlet container
   // when we have the actual pressure data also
   _panInletPump->SetCommandRPM( CHwOrderData.inletRPM );

   // update order data and calculate stroke volumes
   // for AC, plasma and platelet pumps
   _panACPump->SetCommandRPM( CHwOrderData.acRPM );
   _panACPump->SetMlsPerRev();
   _paPlasmaPump->SetCommandRPM( CHwOrderData.plasmaRPM );
   _paPlasmaPump->SetMlsPerRev();
   _paPlateletPump->SetCommandRPM( CHwOrderData.plateletRPM );
   _paPlateletPump->SetMlsPerRev();

   // update order data for centrifuge
   _paCentrifuge->SetCommandRPM( CHwOrderData.centrifugeRPM);

   ++_iControlCommandsCount;

} // END void aControlCommandsMsg_rcvr :: notify()


// SPECIFICATION:    notify for control status msg rcvr.
//
//
// ERROR HANDLING:   none.

void  aControlStatusMsg_rcvr :: notify()
{
   SYSTEM_OP_STATUS OperationalStatus;
   struct timespec sendTime;                  // time msg sent

   CHwStateData = _theCHwStatesMsg.getData();

   // request safety status for matching halls and encoders
   if ( (CHwStateData.event == CHW_NORMAL_UPDATE)
        ||(CHwStateData.event == CHW_RESERVOIR_EVENT) )
   {
       DataLog(log_level_safe_exec_debug) << "aControlStatusMsg_rcvr :: notify() sending RequestSafetyStatusMsg(0x0ABCDEF0)" << endmsg;
      _paRequestSafetyStatusMsg_sndr->send( 0x0ABCDEF0 ); // Got to send SOMETHING, and safe_dvr doesn't care what.
   }

   if ( CHwStateData.sixtyFourVoltSwitched > ACCEPT_64V_OFF_RAW_ADC /* ADC value in which we see the power as off */)
      _paDoor->UpdateAndTestDoorFault();

   // update reservoir level
   _paReservoir->UpdateControl(CHwStateData);

   // update power readings
   _pa24vPowerTest->SetCurrentVoltage(CHwStateData.twentyFourVoltSwitched);
   _pa64vPowerTest->SetCurrentVoltage(CHwStateData.sixtyFourVoltSwitched);

   // update info for disconnect tests
   _paDonorDisconnectTest->Update(calc_APS_mmhg_3_pt(CHwStateData.accessPressure)
                                  ,CHwStateData.inletPump.Accum
                                  ,CHwStateData.msgCount);

   //
   // take the timestamp off the message header in order
   // to calculate delta time from the control side.
   // some calculations using encoder data need to use 
   // control time to be correct (see return pump speed).
   //
   sendTime = CHwStateData.timestamp;

#define DEBUGMSG 1
#if DEBUGMSG
   struct debugtime {
	   timespec proc;
	   timespec safe;
   };

   static debugtime times[100] = { 0 };
   static timespec receivedTime = { 0 };
   static int index = 0;

   clock_gettime( CLOCK_REALTIME, &receivedTime );

   times[index].proc = sendTime;
   times[index].safe = receivedTime;

   ++index;

   if (index >= 100) {
	   for (int i = 0; i < 100; ++i) {
		   DataLog(log_level_safe_exec_info) 
		       << "MSG" 
		       << " Proc: " 
			   << times[i].proc.tv_sec 
			   << ":" 
			   << times[i].proc.tv_nsec
		       << " Safe: " 
			   << times[i].safe.tv_sec 
			   << ":" 
			   << times[i].safe.tv_nsec
			   << endmsg;
	   }

	   index = 0;
   }
#endif

   // if we are debouncing the lower sensor, increment the counter
   if ( (_lowLevelIgnore == TRUE)
        &&(CHwStateData.reservoir == CHW_RESERVOIR_MIDDLE) )
   {
      if ( ++_lowLevelDebounceCount >= FALSE_ECHO_FILTER )
      {
         _lowLevelIgnore = FALSE;
         _lowLevelDebounceCount = 0;
      }
   }

   // what is proc doing?
   OperationalStatus = _paProcOpMonitor->GetOperationalStatus();

   switch ( CHwStateData.event )
   {
   case CHW_NORMAL_UPDATE:
      // this is the normal 500ms update.
      //
      // calculate the delta time between messages with pump data.
      _paReservoir->SetDeltaControlStatusTime(sendTime);

      // update containers and run all the physiological monitors 
      _paReservoir->update_res_mon();

      break;

   case CHW_AC_EVENT:
   case CHW_DOOR_EVENT:
   case CHW_PAUSE_EVENT:
   case CHW_STOP_EVENT:                              
      break;

   case CHW_RESERVOIR_EVENT:
      // this is a reservoir sensor event 

      // for reservoir events, we run the physiological monitors first 
      // and then switch the cycle.

      // if we have a true reservoir switch event, update the monitors
      if ( (CHwStateData.reservoir == CHW_RESERVOIR_HIGH )
           ||( (CHwStateData.reservoir == CHW_RESERVOIR_EMPTY)
               &&(_lowLevelIgnore == FALSE) ) )
      {
         // calculate the delta time between messages with pump data.
         _paReservoir->SetDeltaControlStatusTime(sendTime);
         // update containers and run all the physiological monitors 
         _paReservoir->update_res_mon();
      }

      // if we are normal operation with a high event,switch the cycle
      if ( CHwStateData.reservoir == CHW_RESERVOIR_HIGH )
      {
         _paReservoir->UpdateInfusionTimers(CHwStateData.reservoir);
         _paReservoir->switch_to_return_cycle();
      }


      // if we have a true low reservoir event, force a switch to draw
      if ( (CHwStateData.reservoir == CHW_RESERVOIR_EMPTY )
           &&(_lowLevelIgnore == FALSE) )
      {
         _paReservoir->UpdateInfusionTimers(CHwStateData.reservoir);
         _paReservoir->switch_to_draw_cycle();

         // set to ignore low level sensor bounce
         _lowLevelIgnore = TRUE;
      }
      break;

   default:

      {
          DataLog( log_level_critical ) << "Undefined CHwStateData.event: "
                                << (int)CHwStateData.event
                                << "  causes forbidden case switch default"
                                << endmsg;
          _FATAL_ERROR( __FILE__, __LINE__, "invalid event type" );
      }
      break;

   }
   ++_iControlStatusCount;

} // end void  aControlStatusMsg_rcvr :: notify()


// SPECIFICATION:    notify for APS zero message from control
//
//
// ERROR HANDLING:   none.

void aControlStatusMsg_rcvr :: newAPSZero(void)
{
	int zero = _theAPSZeroMsg.getData();
	set_APS_mmhg_offset(zero);
}

// SPECIFICATION:    Receiver for SafetyCommands
//                   Alarms if it came from anyone other
//                   than itself
//
//
// ERROR HANDLING:   none.

void aSafetyCommandsMsg_rcvr :: notify()
{
   unsigned long pid;                  // sending pid
   unsigned long nid;                  // sending nid
   struct timespec timestamp;
   SHwOrders structSHwOrderData; 

   // get message data into local structure.
   structSHwOrderData = _theSHwOrdersMsg.getData();

//   msgHeader( pid, nid, timestamp );
   pid = _theSHwOrdersMsg.originTask();
   nid = _theSHwOrdersMsg.originNode();

   if ( pid != _pidSafExec || nid != _nidSafetyProcessor )
   {
      // illegal msg
      paSafetyHardwareCommandsExec->respond_to_alarm( OFF_BOTH, SW_FAULT );
      DataLog(log_level_safety_alarm_detail) << "Forbidden safety command msg _pidSafExec="
                                << _pidSafExec
                                << ",sndrpid="
                                << pid
                                << ",_nidSafetyProcessor="
                                << _nidSafetyProcessor
                                << ",sndrnid="
                                << nid
                                << endmsg;
   }

}  // END void aSafetyCommandsMsg_rcvr :: notify()
