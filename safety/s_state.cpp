/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 * $Log: s_state.cpp $
 * Revision 1.96.1.2  2009/10/05 16:04:35Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:25Z  dslausb
 * Initial revision
 * Revision 1.96  2008/03/07 22:45:06Z  spriusb
 * IT 5802: failure status messages for AC connect, Donnor Connect, and Blood Run phases of T1 test now provides more fine-grain coding to assess actual failure mode.
 * Revision 1.95  2006/06/16 17:02:13Z  jheiusb
 * 7574 -- A2 alrm skipping out of MSS needs new transitions allowed
 * Revision 1.94  2006/04/19 21:49:04Z  rm70006
 * IT 7389.  Fix bad safety logging statement.
 * Revision 1.93  2006/03/18 00:07:32Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.92  2006/03/03 17:34:17Z  jheiusb
 * initial merge
 * Revision 1.91  2003/11/14 20:14:35Z  ms10234
 * 5316 - rework 64v power tests and fix logging level
 * Revision 1.90  2003/10/23 16:26:57Z  ms10234
 * 6488 - changed state transition table to allow transition from disposable_test to post_run.
 * Revision 1.89  2003/07/21 22:18:31Z  td07711
 * IT6157 - no need to alarm when going fatal
 * Revision 1.88  2003/07/01 22:35:07Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.87  2003/04/03 20:40:44Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.86  2002/10/11 09:45:57  pn02526
 * Massive checkin of fixed modules for integration checking
 * Revision 1.85  2002/08/08 13:40:01  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Add aSafetySystemStateChangeMsg_rcvr :: _theSystemStateChangeMsg
 * + to use the new buffered message class definition,
 * + rather than deriving from an INT32 message base class.
 * Use the new buffered message base class definition to send the TransitionValidationMsg,
 * + rather than deriving class aSafetyTransitionValidationMsg_sndr from an INT32 message base class.
 * Change the way the search is performed in aSafetySystemStateChangeMsg_rcvr :: FindStateNameIndex()
 * + to derive the loop limit from the size of the state_name_table array
 * + rather than counting on NULL_STATE to be the last entry.
 * To keep GNU compiler happy for production builds:
 * + Initialize "gateway" in aSafetySystemStateChangeMsg_rcvr
 * +   :: getStateTransGate() to a new constant "UNDEFINED_GATE".
 * + Add a new "case UNDEFINED_GATE" to aSafetySystemStateChangeMsg_rcvr
 * +   :: doStateTransGate() that queues an A2 alarm.
 * Revision 1.84  2002/07/23 16:52:28  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.83  2002/04/15 08:50:43  pn02526
 * trima_datalog.hpp -> trima_datalog.h
 * Revision 1.82  2002/04/11 17:18:32  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.
 * Revision 1.81  2001/12/14 14:26:34Z  jl11312
 * - modified centrifuge spinning with door open/unlocked alarm to occur at 1 second instead of 2.5 during startup tests
 * Revision 1.80  2001/08/27 21:37:30  ms10234
 * 5271 -  Remove references to stamp color
 * Revision 1.79  2001/06/22 19:57:27  ms10234
 * 4922 -  Initialize the pfr save state to the recovered state.
 * Revision 1.78  2001/06/07 21:47:25  ms10234
 * 3303 - PFR for v5.0
 * Revision 1.77  2001/01/24 21:47:13  ms10234
 * IT4881 - changed to allow transitions from SELF_TEST to POST_RUN.
 * Revision 1.76  2000/07/11 23:50:31  bs04481
 * Honor MidCycleSwitch messages during Blood Run only.
 * Ignore it at all other times.
 * Revision 1.75  2000/06/08 21:36:30  bs04481
 * Have state object access containers via the reservoir instead
 * of by a discrete reference.
 * Revision 1.74  2000/05/26 17:12:33  BS04481
 * More maintanence changes:  This set contains changes for
 * 1. Moving valve object from container to bag
 * 2. Removing SafetyTimer->SafetySoftTimer and letting the
 *      switch and disconnect tests have their own timers.
 * 3.  All message types are changed to NO_BOUNCE to cut down
 *      the on unnecessary stuff in the message que.
 * Revision 1.73  2000/03/15 18:27:31  BS04481
 * Remove ineffective code designed to allow a transition from
 * startup to donor disconnect with a wet set. (was replaced
 * with the state transition table a while ago); start all objects in FIRST
 * state when not a PFR and allow transitions out of FIRS;
 * create SafetyTransitionValidation message where it is used;
 * 3.3 non-essential code review issues
 * Revision 1.72  1999/11/15 23:51:58  BS04481
 * Move T1 for valve and cassette position, blink test, cassette type
 * to AC Connect instead of AC Prime.
 * Revision 1.71  1999/10/08 15:37:36  BS04481
 * Restore air-to-donor monitoring in the safety driver following a
 * PF if the procedure is not recoverable and the cassette is down.
 * Revision 1.70  1999/08/13 17:44:46  BS04481
 * Restore donor mode command to the driver as soon as the safety
 * executive decides that the procedure is recoverable.
 * Revision 1.69  1999/08/01 20:53:32  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.68  1999/06/23 19:35:03  BS04481
 * Change T1 tests to notify the operator as soon as possible of
 * a T1 failure.  This is done to attempt to save sets and donors
 * when there is a hardware problem.  See IT 3428.
 * Revision 1.67  1999/04/06 18:33:00  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.66  1998/11/12 18:32:07  bs04481
 * Add a gateway to the gate tables for both normal and pfr transitions
 * that will cause us to look at the donor data when we enter donor
 * disconnect.  If the donor data does not look reliable, we will attempt
 * to get the necessary data (donor EC limit, set type, current
 * hypovolemia status) from the PFR data.
 * Revision 1.65  1998/11/05 19:16:05  bs04481
 * Make AC Prime to AC Prime a forbidden transition to avoid
 * resetting pump volumes more than once.  Code Review change.
 * Revision 1.64  1998/11/01 20:47:04  bs04481
 * Make the transition from Donor Disconnect to Self Test or to
 * Startup Test an ignored transition during PFR.  Also make Donor
 * Disconnect to Donor Disconnect a Normal-or-PFR transition.  This
 * change is part of the previous change to restore monitoring
 * immediately after PFR.
 * Revision 1.63  1998/10/21 23:20:17  bs04481
 * Make the state transition from Blood Run to Disconnect and
 * Rinseback to Disconnect either a normal transition or a PFR
 * transition.  It might be a PFR transition if Safety believes the
 * PF is recoverable but Procedure believes it is not.
 * Revision 1.62  1998/10/20 20:37:13  bs04481
 * Change power fail recovery to restore PF data and resume all
 * active monitoring immediately after initialization if Safety determines
 * that the procedure is recoverable.
 * Following state transitions are now allowed as PFR transitions:
 * Blood Run to Blood Run; Rinseback to Rinseback
 * Following state transitions are allowed during a PFR run but
 * Safety will not carry out the actions for the transition:  Blood Run
 * to Self Test; Blood Run to Startup; Rinseback to Self Test;
 * Rinseback to Startup.
 * Following transitions are illegal: Startup to Blood Run; Startup to
 * Rinseback.
 * Revision 1.61  1998/09/29 19:49:33  bs04481
 * Fix SIGSEV which occurred when Safety received
 * unexpected data in the state change message
 * Revision 1.60  1998/09/28 22:23:37  bs04481
 * Add includes for headers s_disc.hpp and s_power.hpp
 * Revision 1.59  1998/07/30 01:31:54  bs04481
 * Change to restore container data and then monitor data for
 * a PFR.  Not absolutely necessary but is clearer in this order.
 * Revision 1.58  1998/06/16 21:26:12  bs04481
 * Check for a few more incorrect state transitions in the state
 * transition table.  Formerly, these incorrect transitions were
 * caught by other monitoring functions (i.e. ac ratio unuseable) instead.
 * Specifically changed DonorConnect to BloodRinseback to forbidden
 * and BloodPrime to BloodRinseback to forbidden.  Also changed
 * FirstState to any state to forbidden as we never enter first state.
 * Revision 1.57  1998/06/09 18:05:55  bs04481
 * Add call to safety status message initialize for state function to
 * allow it to disable the PFR timer when we transition to donor
 * disconnect
 * Revision 1.56  1998/05/12 19:47:04  bs04481
 * Change reservoir monitoring, which includes ac ratio, ac infusion
 * and hypovolemia montiroing, to an object oriented design.
 * Previously, the volumes were accumulated in the s_res code.
 * With this change, the machine is modeled as six containers, each
 * consisting of some combination of bags, valves and pumps.  The
 * safety monitors now query the containers and calculate the
 * safety variables.  See the safety executive design document for
 * version 3.2 software.
 * Revision 1.55  1998/02/07 00:15:43  bs04481
 * Allow state transition of Post to Post to allow PFR in the
 * second screen of Post Run.
 * Revision 1.54  1997/12/17 21:47:54  bs04481
 * Change to check only Trima CRCs when transitioning into Donor
 * Disconnect in a PFR procedure.
 * Revision 1.53  1997/12/12 17:09:34  bs04481
 * Code Cleanup and comments
 * Revision 1.52  1997/12/10 18:22:36  bs04481
 * Changes to use the Control Reservoir sensor to see what cycle
 * we should be in after a power fail.
 * Add CRC check gate to Donor Disconnect transition from a PF.
 * Added (some) comments .
 * Revision 1.51  1997/12/05 21:13:12  bs04481
 * Changes to block the transition to Blood Rinseback if attempted
 * during first draw.
 * Revision 1.50  1997/12/02 15:58:31  bs04481
 * Changed to table driven structure to make it easier to implement
 * power fail recovery
 * Revision 1.49  1997/08/01 18:58:00  SM02805
 * Modifications to correct alarms related t aborted first draw.
 * Modification 1 : do not call cal ac to bag ( R may be zero ) if
 * the donor is not connected. This fixes the case where a run
 * was aborted during first draw and as the cassette raises the
 * low level sensor goes from fluid to air causes a call to
 * switch_to_draw_cycle which calls calc ac to bag.
 * Modification 2 : If abort from first draw and desire rb, added
 * code to calculate a ratio ( using first draw eqn ) if ratio is 0
 * and if the calc ratio is > 5.0, rb can occur.
 * Modification 3 : If the system is shutdown for a ratio alarm, the
 * value of the _favgACRatio is set to MIN_ALLOWABLE_AC_RATIO
 * to prevent divide by zero problems elsewhare in the code which
 * result in FATAL ERROR.  The system has been safed by the alarm,
 * this mod prevents a FATAL ERROR which causes screen to go
 * to black.
 *
 * Revision 1.48  1997/07/31 18:58:13  TM02109
 * Modified to use the Draw cycle volume of AC processed for the
 * ACPrimeVolume.
 * Revision 1.47  1997/07/30 15:53:13  SM02805
 * Modified text search implementations for state names and
 * alarm names that are used for safety trace logging to be consistent
 * with enumeration changes to states ( /proc/states.h ) and
 * alarms ( /proc/alarms.h ). These files and enums are common
 * between the domains.
 * Revision 1.46  1997/07/29 16:03:12  SM02805
 * 1. Modify to use procedure states.h file for state enumeration
 * definitions to prvent future maintenance problems when proc
 * defines new states/substates.
 * 2. Modifications to rb state transition to allow for ratio robustness
 * to be applied to rb ratios.
 * Revision 1.45  1997/06/20 16:37:50  SM02805
 * Added loging associated with continuous hct range testing.
 * Revision 1.44  1997/06/02 20:08:10  bs04481
 * Remove check for CRCs from the Blood Prime -> Blood Run transifition .
 * Change it to the transition to Donor Connect
 * Revision 1.43  1997/06/02 04:42:40  bs04481
 * Enable background checking of CRCs by safety.  Add safety files
 * to those checked.   Shutdown is enabled if CRCs do not match.
 * Revision 1.42  1997/05/23 23:30:26  bs04481
 * Check in first pass of code to check the CRCs in safety as a T1 test.  However, this pass will not stop the procedure if the CRC does not match or is not available.
 * Revision 1.41  1997/05/15 00:48:31  SM02805
 * Allow state trans from SELF_TEST OR STARTUP_PRELOAD to
 * POST_RUN to provide easy way to get to POST_RUN from
 * power on restart ( FSNS support for ftp ).
 * Revision 1.40  1997/04/21 15:43:41  SM02805
 * Pursuant to issue tracker #1806, set looking for first fluid flag
 * to FALSE at rinseback OR donor disconnect trans.  Issue tracker
 * 1806 is a case where a run was aborted prior to first fluid detection
 * by safety.  Sloshing upon unload following d/c test caused fluid to be
 * detectected for the 'first time' and safety requires the state to be
 * blood prime for this condition to be allowable.  To prevent alarms
 * safety will no longer arm the looking for first fluid monitor in the rinseback
 * or donor d/c states.
 * Revision 1.39  1997/03/13 23:21:06  bs04481
 * Changes to set the new donorMode in the safety command data
 * Revision 1.38  1997/03/03 15:22:58  bs04481
 * Revision 1.38  1997/02/28 23:07:05  bs04481
 * Merge in ratio monitoring code which was included in 2.3 but did
 * not get transferred to 2.5.
 * Revision 1.37  1997/02/11 18:00:46  bs04481
 * Changes due to enumerated types in the hardware structures
 * Revision 1.36.1.1  1997/02/15 22:37:03  TM02109
 * Modification to rinseback monitoring.
 * Revision 1.36  1997/01/23 15:05:56  SM02805
 * Make state trans to SERVICE_CONTROL invalid.  Previoulsy the system
 * could be transitioned to service from SELF_TEST.  Barry intends to impl
 * service without the safety executive running therefore this trans
 * should never be detected by the safety executive and is forbidden.
 * Revision 1.35  1997/01/23 14:59:22  SM02805
 * Modify to call method to zero accumulated inlet pump rev error at
 * state trans identical to return pump.
 * Revision 1.34  1997/01/17 04:04:34  TM02109
 * Modifications for new rev error monitoring for return pump. Had
 * bugs in previous attempt for single return cycle rev error calcs.
 * Revision 1.33  1997/01/16 20:16:57  SM02805
 * Modify return pump rev error monitoring as per 01/15/97 safety
 * meeting as documented in PPQA memo dated 01/16/97.
 * Revision 1.32  1997/01/07 18:33:56  TM02109
 * Modified impl for System Operation state.
 * Revision 1.31  1997/01/06 23:08:27  SM02805
 * Made modifications to :
 * 1. detect paused condition resulting from situations
 *  other than pause key presses ( "procedural pauses" ).
 * 2.  Problems resulting from reservoir fillup secondary
 * to cent shutdown following 10 minute pauses ( AC OFF During Draw ) .
 * Revision 1.30  1996/12/04 00:32:16  SM02805
 * Made modifications required for removing valve position timing from
 * safety.
 * Revision 1.29  1996/12/03 21:16:00  TM02109
 * Fix bug in logging of T1 status for valves.
 * Revision 1.28  1996/11/28 00:08:23  SM02805
 * Revision 1.27  1996/09/22 18:14:14  SM02805
 * 1. Respond to 09/19/96 code review action items.
 * 2. Implement power recyle alarm for any state transition
 * from POST_RUN to prevent starting a new procedure without
 * recycling power.
 * Revision 1.26  1996/09/18 15:23:46  SM02805
 * Moved set sn cycle to draw from conor connected state trans
 * to ac onnected state trans to be consistent with safety first draw
 * volume calculations which includes ac prime.
 * Revision 1.25  1996/09/18 00:18:02  SM02805
 * Donor disconnect test bug for powerup with cassette down - invalid
 * state transition to post run from disconnect test passed.
 * Revision 1.24  1996/09/16 17:01:22  SM02805
 * 09/17/96 marlene resolution to infusion calculation use return
 * to return cycle for infusion calc.
 * Revision 1.23  1996/09/16 17:01:22  SM02805
 * Significantly reduce print instrumentation in source files.
 * Implement _LOG_ERROR processing for cases where
 * data is important for post-run safety analysis.
 * Revision 1.22  1996/09/16 17:01:22  SM02805
 * 09/12/96 xfuc resolutions :
 * 1.Implement infusion monitoring for RINSEBACK (RB).
 * 2.Implement valves must be in return monitoring for RB.
 * 3. Monitor for inappropriate movement of the AC pump
 * during RB and donor disconnect test.
 *
 * Revision 1.21  1996/09/16 17:01:22  SM02805
 * 09/12/96 xfunc resolutions :
 * 1. Zero rev error for return pump at all state transitions where
 * direction reversal may be a problem with encoder:hall
 * congruence.
 * Revision 1.19  1996/08/21 22:39:22  SM02805
 * Revision 1.18  1996/08/19 23:05:56  SM02805
 * Change state trans logic to allow for cassette unload following
 * "bad casette position" detected by procedure.  The state trans from
 * DONOR_DISCONNECT to POST_RUN is valid if the donor disconnect
 * test has been passed OR the cassette position is not LOADED at
 * the trans. Thsi allows proc to trans to post run when the cassette has
 * been unloaded for a "bad position" detection.
 * Revision 1.17  1996/08/14 23:02:16  SM02805
 * Revision 1.16  1996/08/14 13:26:50  SM02805
 * Fixed bug at DONOR_CONNECTED to BLOOD_PRIME tranisition
 *  : trans was being incorrectly interpreted as invalid following test
 * for donor hct in range.
 * Revision 1.15  1996/08/14 03:39:22  SM02805
 * Remove platelet precount range testing as condition
 * for trans to BLOOD_PRIME state per Marlene.  Donor
 * HCT IS tested at this trans and range test failure will
 * cause INVALID state trans alarm.
 * Revision 1.14  1996/08/13 18:57:52  SM02805
 * Implement alarm for cassette not down at trans to AC_CONNECTED.
 *
 * Implement condition of fluid at low level sensor at trans to
 * BLOOD_RUN as additional condition for valid state trans.
 *
 * Revision 1.13  1996/07/31 23:26:18  SM02805
 * Revision 1.12  1996/07/31 23:24:44  SM02805
 * T1 Failure alarm assertion added.
 * Revision 1.11  1996/07/30 23:52:04  SM02805
 * Turn off ratio monitoring for rinseback. Allow any state to trans to donor
 * disconnect.
 * Revision 1.10  1996/07/30 20:36:36  SM02805
 * Corrected bug for ac prime volume and blood prime volume
 * calcs.
 * Revision 1.9  1996/07/30 02:37:30  SM02805
 * Lint fixes per 07/23/96, 07/26/96 reviews.
 * Revision 1.8  1996/07/24 17:14:26  SM02805
 * Rename _paRes to _paReservoir for consistencyt with other
 * modules.
 * Revision 1.7  1996/07/24 13:57:11  SM02805
 * Mods to remove absolute paths for include files, includes source
 * code modifications and makefile ( safety.mak) modifications.
 *
 * safety.mak mods use CPPFLAGS , now getting warnings level 3.
 *
 * Sorce code mods to implement donor disconnect test, only partially tested
 * - works as expected when test is passed first time, ie access line
 * properly clamped on first test try...
 *
 * TITLE:      s_state.cpp
 *             Safety state management class implementations.
 *
 *
 * ABSTRACT:   This file contains the class definitions for safety
 *             state management classes.
 *
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/
#define STATE_LIST

#include "trima_datalog.h"
#include "s_exec.hpp"
#include "s_state.hpp"
#include "s_res.hpp"
#include "s_pfr.hpp"
#include "s_disc.hpp"
#include "s_power.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

// Safety only cares about the first 14 states so this
// code will assume any states greater than this are don't care.

#define SAFETY_STATES 14


/*******************************************************************************
*
*  aSafetySystemStateChangeMsg_rcvr CLASS METHOD DEFINITION
*
******************************************************************************/
// SPECIFICATION:    notify for state change msgs
//
//
// ERROR HANDLING:   Invalid transition.
//
void aSafetySystemStateChangeMsg_rcvr::notify ()
{
   int        iSystemStateAllowed = TRANSITION_UNVERIFIED;
   int        iCurrentStateIndex, iNewStateIndex;
   Permission StateTransPermit;
   short      StateTransGate;
   bool       GateOK;

   // get new system state from message
   const State_names newSystemState = (State_names)_theSystemStateChangeMsg.getData();
   const State_names currentState   = SafetyExec::instance()->SystemState();

   // safety only allows one transition during a power cycle
   // that is classified as a PFR Transition.  After that
   // single PFR transition, all subsequent transitions
   // must follow the normal course of a procedure.
   // if the last transition was a PFR, don't allow any more.
   if (  (SafetyPFRDataFromRestore.PFRSafetyOneShot) &&
         (_iLastSystemStateTransitionType == PFR_STATE_TRANSITION)
         // && (currentState != SELF_TEST )
         )
   {
      SafetyPFRDataFromRestore.PFRSafetyOneShot = false;
      SafetyPFRDataToSave.PFRSafetyOneShot      = false;
      DataLog(log_level_safe_exec_error) << "PFR Allowance Used.   currentState: " << currentState << endmsg;
   }

   // if we are changing states
   if (currentState != newSystemState)
   {
      if (SafetyPFRDataFromRestore.PFRSafetyOneShot)
      {
         // if we are in PFR,  we don't want to let the cassette move
         // from the position it was in at power fail until we are
         // on our way again.  check to see that it hasn't moved
         if (SafetyPFRDataFromRestore.PFRCassette_position != _paCassette->GetCassettePosition())
         {
            DataLog(log_level_safe_exec_error) << "Cassette position has changed from "
                                               << SafetyPFRDataFromRestore.PFRCassette_position
                                               << " to "
                                               << _paCassette->GetCassettePosition()
                                               << endmsg;
            /// invalid state transition
            iSystemStateAllowed = INVALID_STATE_TRANSITION;
         }
      }
   }


   // validate the state transition
   if (iSystemStateAllowed != INVALID_STATE_TRANSITION)
   {

      StateTransPermit = getStateTransPermission(currentState, newSystemState);

      DataLog(log_level_safe_exec_info) << "new state =  " << newSystemState << endmsg;
      DataLog(log_level_safe_exec_info) << "curr state =  " << currentState << endmsg;
      DataLog(log_level_safe_exec_info) << "state Trans Type=  " << StateTransPermit << endmsg;

      // if a forbidden transition, mark it so
      if (StateTransPermit == FORBID)
      {
         iSystemStateAllowed = INVALID_STATE_TRANSITION;
      }
      // if a normal transition, mark it so
      else if (StateTransPermit == NORMAL)
      {
         iSystemStateAllowed = VALID_STATE_TRANSITION;
      }
      // if a transition only allowed in power fail recovery,
      // make sure we are in power fail recovery
      else if (StateTransPermit == PFR_OK)
      {
         if (SafetyPFRDataFromRestore.PFRSafetyOneShot == TRUE)
         {
            DataLog(log_level_safe_exec_error) << "PFR_OK with PFR On - pft trans" << endmsg;
            iSystemStateAllowed = PFR_STATE_TRANSITION;
         }
         else
         {
            DataLog(log_level_safe_exec_error) << "PFR_OK with PFR off - invalid " << endmsg;
            iSystemStateAllowed = INVALID_STATE_TRANSITION;
         }
      }
      // if the transition is allowed in either normal or power fail recover,
      // check to see if we are in power fail recovery
      else if (StateTransPermit == N_OR_P)
      {
         if (SafetyPFRDataFromRestore.PFRSafetyOneShot == TRUE)
         {
            DataLog(log_level_safe_exec_error) << "N_OR_P with PFR On - pft trans" << endmsg;
            iSystemStateAllowed = PFR_STATE_TRANSITION;
         }
         else
         {
            DataLog(log_level_safe_exec_error) << "N_OR_P with PFR off - normal trans" << endmsg;
            iSystemStateAllowed = VALID_STATE_TRANSITION;
         }
      }
      // if this is not a safety state transition
      else if (StateTransPermit == NOT_AP)
      {
         iSystemStateAllowed = UNDEFINED_SAFETY_STATE;
      }
      // if this is a state transition which we are ignoring because
      // we are in a PFR condition, make sure we really are in a PFR
      // because the transition is invalid if we are not
      else if (StateTransPermit == IGN_PF)
      {
         if ( (SafetyPFRDataFromRestore.PFRSafetyOneShot) ||
              (newSystemState == SELF_TEST)
              )
         {
            DataLog(log_level_safe_exec_error) << "IGN_PF with PFR On - ignore trans" << endmsg;
            iSystemStateAllowed = IGNORED_TRANSITION_DURING_PFR;
         }
         else
         {
            DataLog(log_level_safe_exec_error) << "IGN_PF with PFR Off - invalid  " << endmsg;
            iSystemStateAllowed = INVALID_STATE_TRANSITION;
         }
      }
   }


   // if a possible transition, do the gates necessary to enter the new state
   if ( (iSystemStateAllowed != INVALID_STATE_TRANSITION)
        &&(iSystemStateAllowed != UNDEFINED_SAFETY_STATE)
        &&(iSystemStateAllowed != IGNORED_TRANSITION_DURING_PFR) )
   {
      StateTransGate = getStateTransGate(newSystemState, iSystemStateAllowed);

      DataLog(log_level_safe_exec_info) << "Gate to use : " <<  StateTransGate << endmsg;

      DataLog(log_level_safe_exec_info) << "Gate   lNewSystemState: "
                                        <<  newSystemState  << " iSystemStateAllowed:" << iSystemStateAllowed << endmsg;


      GateOK = doStateTransGate(_sGateDone, StateTransGate);

      if (!GateOK)
      {
         iSystemStateAllowed = INVALID_STATE_TRANSITION;
      }

      _sGateDone = StateTransGate;
   }


   // if a possible transition and we passed the gate criteria, do the action
   if ( (iSystemStateAllowed != INVALID_STATE_TRANSITION)
        && (iSystemStateAllowed != UNDEFINED_SAFETY_STATE)
        && (iSystemStateAllowed != IGNORED_TRANSITION_DURING_PFR) )
   {
      SafetyExec::instance()->update(newSystemState);

      initializeForState(newSystemState);
   }


   // log the state change
   iCurrentStateIndex = FindStateNameIndex(currentState);
   iNewStateIndex     = FindStateNameIndex(newSystemState);

   if ( iCurrentStateIndex != -1
        && iNewStateIndex != -1 )
   {
      if ( iSystemStateAllowed == VALID_STATE_TRANSITION )
      {
         DataLog(log_level_safe_exec_info) << "VALID State Change Old : "
                                           << state_name_table[iCurrentStateIndex].state_text
                                           << ", New : "
                                           << state_name_table[iNewStateIndex].state_text
                                           << endmsg;


      }
      else if ( iSystemStateAllowed == PFR_STATE_TRANSITION )
      {
         DataLog(log_level_safe_exec_info) << "PFR State Change Old : "
                                           << state_name_table[iCurrentStateIndex].state_text
                                           << ", New : "
                                           << state_name_table[iNewStateIndex].state_text
                                           << endmsg;


      }
      else if ( iSystemStateAllowed == INVALID_STATE_TRANSITION )
      {
         DataLog(log_level_safe_exec_error) << "INVALID State Change Old : "
                                            << state_name_table[iCurrentStateIndex].state_text
                                            << ", New : "
                                            << state_name_table[iNewStateIndex].state_text
                                            << endmsg;

      }
      else if ( iSystemStateAllowed == UNDEFINED_SAFETY_STATE )
      {
         DataLog(log_level_safe_exec_error) << "UNDEFINED_SAFETY_STATE internal state unchanged, State Old : "
                                            << state_name_table[iCurrentStateIndex].state_text
                                            << ", Requested : "
                                            << state_name_table[iNewStateIndex].state_text
                                            << endmsg;

      }
      else if ( iSystemStateAllowed == IGNORED_TRANSITION_DURING_PFR )
      {
         DataLog(log_level_safe_exec_info) << "Ignored State Change during PFR.  State unchanged, State Old : "
                                           << state_name_table[iCurrentStateIndex].state_text
                                           << ", Requested : "
                                           << state_name_table[iNewStateIndex].state_text
                                           << endmsg;

      }
   }
   else
   {
      DataLog(log_level_critical) << "State match NOT found for transition from " << currentState << " to " << (int)newSystemState << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "unrecognized state transition");
   }


   // get your ducks in a row for the next time
   if ( ( iSystemStateAllowed == VALID_STATE_TRANSITION )
        ||( iSystemStateAllowed == PFR_STATE_TRANSITION ) )
   {
      // update system state in class
      DataLog(log_level_safe_exec_info) << "aSafetySystemStateChangeMsg_rcvr :: notify() #1 sending SafetyTransitionValidationMsg(TRUE)" << endmsg;
      _paSafetyTransitionValidationMsg_sndr->send( (long)TRUE);
   }
   else if ( iSystemStateAllowed == INVALID_STATE_TRANSITION )
   {
      // alarm
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, STATE_TRANSITION_INVALID);
      DataLog(log_level_safety_alarm_detail) << "aSafetySystemStateChangeMsg_rcvr :: notify() sending SafetyTransitionValidationMsg(FALSE)" << endmsg;
      _paSafetyTransitionValidationMsg_sndr->send( (long)FALSE);
   }
   else if ( iSystemStateAllowed == UNDEFINED_SAFETY_STATE )
   {
      // assume procedure state not known to safety.
      // respond OK, don't change safety state.
      DataLog(log_level_safe_exec_info) << "aSafetySystemStateChangeMsg_rcvr :: notify() #2 sending SafetyTransitionValidationMsg(TRUE)" << endmsg;
      _paSafetyTransitionValidationMsg_sndr->send( (long)TRUE);
   }
   else if ( iSystemStateAllowed == IGNORED_TRANSITION_DURING_PFR )
   {
      // we are not going along with this transition
      // but respond OK anyway.
      DataLog(log_level_safe_exec_info) << "aSafetySystemStateChangeMsg_rcvr :: notify() #3 sending SafetyTransitionValidationMsg(TRUE)" << endmsg;
      _paSafetyTransitionValidationMsg_sndr->send( (long)TRUE);
   }

   _iLastSystemStateTransitionType = iSystemStateAllowed;

}  // END void aSafetySystemStateChangeMsg_rcvr :: notify()



// SPECIFICATION:    find text to match state enum
//
//
// ERROR HANDLING:   None
// RETURN:           index to state table or -1 if not found
//
int aSafetySystemStateChangeMsg_rcvr::FindStateNameIndex (State_names State)
{
   const int nStates = (sizeof(state_name_table) / sizeof(the_state_struct));

   // walk the state_name_table to find a match
   //   for( int i = 0; state_name_table[i].state != NULL_STATE; i++ )
   for (int i = 0; i < nStates; i++ )
   {
      if (state_name_table[i].state == State)
      {
         return ( i );
      }
   }

   // no match
   return ( -1 );
}



// SPECIFICATION:    Determine if a state change requested
//                   could be a valid progression
//
//
// ERROR HANDLING:   None.
// RETURN:           FORBID = never valid
//                   NORMAL = normal progression
//                   N_OR_P = normal or a PFR progression
//                   PFR_OK = only normal if in PFR
//                   NOT_AP = not a safety state so we don't care
//
Permission aSafetySystemStateChangeMsg_rcvr::getStateTransPermission (State_names oldState, State_names newState)
{
   Permission permit;
#define PERMISSION_STRING(p) (p==FORBID ? "FORBID" :\
                              p==NORMAL ? "NORMAL" :\
                              p==N_OR_P ? "NORMAL_OR_PFR" :\
                              p==PFR_OK ? "NORMAL_IN_PFR" :\
                              p==IGN_PF ? "IGNORE_IN_PFR" :\
                              p==NOT_AP ? "DONT_CARE" : "UNKNOWN")
   // State_Transition[old_state][new_state]
   Permission State_Transition[SAFETY_STATES][SAFETY_STATES] =
   {                                                                     // FIRST_STATE
      { FORBID, NORMAL, FORBID, FORBID, FORBID, PFR_OK,                  // 1st,  self, start, dsp, acc,  acp
        PFR_OK, PFR_OK, PFR_OK, PFR_OK, NORMAL, NORMAL, FORBID, FORBID}, // dcon, bpri, brun,  brb, ddis, pr , mss , mss_disc

      // SELF_TEST
      { FORBID, NORMAL, NORMAL, NORMAL, NORMAL, PFR_OK,                   // 1st,  self, start, dsp, acc,  acp
        PFR_OK, PFR_OK, PFR_OK, PFR_OK, NORMAL, NORMAL, PFR_OK, PFR_OK }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // STARTUP_TESTS
      { FORBID, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, N_OR_P, NORMAL, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // DISPOSABLE_TEST
      { FORBID, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, NORMAL, NORMAL, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // AC_CONNECTED
      { FORBID, NORMAL, NORMAL, NORMAL, NORMAL, NORMAL,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, NORMAL, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // AC_PRIME
      { FORBID, IGN_PF, IGN_PF, NORMAL, NORMAL, N_OR_P,                   // 1st,  self, start, dsp, acc,  acp
        NORMAL, FORBID, FORBID, FORBID, N_OR_P, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // DONOR_CONNECTED
      { FORBID, IGN_PF, IGN_PF, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        N_OR_P, NORMAL, FORBID, FORBID, N_OR_P, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // BLOOD_PRIME
      { FORBID, IGN_PF, IGN_PF, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, N_OR_P, NORMAL, FORBID, N_OR_P, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // BLOOD_RUN
      { FORBID, IGN_PF, IGN_PF, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, N_OR_P, NORMAL, N_OR_P, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // BLOOD_RINSEBACK
      { FORBID, IGN_PF, IGN_PF, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, N_OR_P, N_OR_P, FORBID, FORBID, FORBID }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // DONOR_DISCONNECT
      { FORBID, IGN_PF, IGN_PF, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, N_OR_P, NORMAL, N_OR_P, N_OR_P }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // POST_RUN
      { FORBID, IGN_PF, FORBID, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, FORBID, NORMAL, N_OR_P, N_OR_P }, // dcon, bpri, brun,  brb, ddis, pr , mss , mss_disc

      // MSS    // on PFR selftest is always run even prior to MSS.
      { FORBID, IGN_PF, FORBID, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, PFR_OK, NORMAL, N_OR_P, N_OR_P }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

      // MSS_DISCON
      { FORBID, IGN_PF, FORBID, FORBID, FORBID, FORBID,                   // 1st,  self, start, dsp, acc,  acp
        FORBID, FORBID, FORBID, FORBID, PFR_OK, NORMAL, N_OR_P, N_OR_P }, // dcon, bpri, brun,  brb, ddis, pr, mss , mss_disc

   };

   if ( (oldState < 0 ) || (newState < 0) )
   {
      permit = FORBID;

      DataLog(log_level_safe_exec_error) << "State Trans: Unknown SafetyState, State Old : "
                                         << oldState
                                         << ", Requested : "
                                         << newState
                                         << ", Permit : "
                                         << PERMISSION_STRING(permit)
                                         << endmsg;

   }
   else if ( (oldState >= SAFETY_STATES ) || (newState >= SAFETY_STATES) )
   {
      permit = NOT_AP;

      DataLog(log_level_safe_exec_error) << "State Trans: Not a SafetyState, State Old : "
                                         << oldState
                                         << ", Requested : "
                                         << newState
                                         << ", Permit : "
                                         << PERMISSION_STRING(permit)
                                         << endmsg;

   }
   else
   {
      permit = State_Transition[oldState][newState];

      DataLog(log_level_safe_exec_info) << "State Trans: State Old : "
                                        << state_name_table[oldState].state_text
                                        << ", Requested : "
                                        << state_name_table[newState].state_text
                                        << ", Permit : "
                                        << PERMISSION_STRING(permit)
                                        << endmsg;
   }
   return(permit);
}



// SPECIFICATION:    Determine which gateways must be satisifed
//                   before a new state can be entered
//                   Gateways for normal transitions are
//                   different than gateways for PFR transitions
//
//
// ERROR HANDLING:   None.
// RETURN:           16bit bit-significant number which represents
//                   the gates required
//
short aSafetySystemStateChangeMsg_rcvr::getStateTransGate (State_names newState,
                                                           long transitionType)
{
   short gateway = UNDEFINED_GATE;   // To keep GNU compiler happy for production builds

   short Normal_State_Transition_Gate[SAFETY_STATES] =
   {
      NO_GATE,                      // FIRST_STATE
      NO_GATE,                      // SELF_TEST
      NO_GATE,                      // STARTUP_TESTS
      NO_GATE,                      // DISPOSABLE_TEST
      ACCON_T1_GATE,                // AC_CONNECTED
      NO_GATE,                      // AC_PRIME
      DCON_T1_GATE | DVIT_GATE,     // DONOR_CONNECTED
      NO_GATE,                      // BLOOD_PRIME
      BRUN_UPDATE | BRUN_T1_GATE,   // BLOOD_RUN
      FIRST_DRAW_GATE,              // BLOOD_RINSEBACK
      DISC_RESTORE,                 // DONOR_DISCONNECT
      DISC_GATE                     // POST_RUN
   };

   short PFR_State_Transition_Gate[SAFETY_STATES] =
   {
      NO_GATE,                                    // FIRST_STATE
      NO_GATE,                                    // SELF_TEST
      NO_GATE,                                    // STARTUP_TESTS
      NO_GATE,                                    // DISPOSABLE_TEST
      NO_GATE,                                    // AC_CONNECTED
      NO_GATE,                                    // AC_PRIME
      NO_GATE,                                    // DONOR_CONNECTED
      NO_GATE,                                    // BLOOD_PRIME
      DCON_T1_GATE | DVIT_GATE | PFR_WHATCYCLE,   // BLOOD_RUN
      DCON_T1_GATE | DVIT_GATE | FIRST_DRAW_GATE, // BLOOD_RINSEBACK
      DISC_RESTORE | CRC_GATE,                    // DONOR_DISCONNECT
      DISC_GATE                                   // POST_RUN
   };


   if ( (newState >= SAFETY_STATES) || (transitionType == INVALID_STATE_TRANSITION) )
   {
      DataLog(log_level_safe_exec_error) << "State Gate: Not a SafetyState, Requested : "
                                         << state_name_table[newState].state_text << ", Transition: " << transitionType << endmsg;

      gateway = NO_GATE;
   }
   else if (transitionType == VALID_STATE_TRANSITION)
   {
      gateway = Normal_State_Transition_Gate[newState];
   }
   else if (transitionType == PFR_STATE_TRANSITION)
   {
      gateway = PFR_State_Transition_Gate[newState];
   }

   DataLog(log_level_safe_exec_info) << "State Gate: Requested : "
                                     << state_name_table[newState].state_text << " , Transition: "
                                     << transitionType << ", Gate: 0x" << std::hex << gateway << std::dec << endmsg;

   return ( gateway );
}

// SPECIFICATION:    Do the gateway actions which were returned
//                   by the function getStateTransGate
//                   Gateway actions (if more than one) are terminated
//                   by the first action which fails
//
//
// ERROR HANDLING:   None.
// RETURN:           TRUE if all (boolean) gateways are satisified,
//                   FALSE if at least one gateway is not satified.
//
bool aSafetySystemStateChangeMsg_rcvr::doStateTransGate (short oldGate, short newGate)
{
   // do the gateway action
   bool result = true;

   if ( (oldGate == newGate) || (newGate == NO_GATE) )
   {
      DataLog(log_level_safe_exec_info) << "State Gate: No gate required, OldGate : "
                                        << std::hex << oldGate
                                        << ", NewGate: "
                                        << newGate << std::dec
                                        << endmsg;

   }
   else
   {
      // if gateway actions are required, cycle through the 16bit bit-significant
      // newGate word doing them.  if any of the gateways fail, result will be set
      // to FALSE and the cycling will terminate.  (Note that not all gateways
      // return a boolean)
      short gatewayMask = 0x0001;
      while ( (gatewayMask  < LAST_GATE) && (result == TRUE) )
      {
         short doGate = (newGate & gatewayMask);
         switch (doGate)
         {
            // if the gate is a test and the test fails, clear the TRUE in the result
            case NO_GATE :
               break;
            case DISC_RESTORE :
               checkToRestoreDisconnectData();
               DataLog(log_level_safe_exec_info) << "State Gate: checkToRestoreDonorData = " << (int)result << endmsg;
               break;
            case PFR_WHATCYCLE :
               whatCycleAreWeIn();
               DataLog(log_level_safe_exec_info) << "State Gate: Do whatCycleAreWeIn = " << (int)result << endmsg;
               break;
            case CRC_GATE :
               result = checkCRCDisconnect();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkCRCDisconnect = " << (int)result << endmsg;
               break;
            case ACCON_T1_GATE :
               result = checkACConnectT1();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkACConnectT1 = " << (int)result << endmsg;
               break;
            case DCON_T1_GATE :
               result = checkDonorConnectT1();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkDonorConnectT1 = " << (int)result << endmsg;
               break;
            case FIRST_DRAW_GATE :
               result = checkFirstDrawComplete();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkFirstDrawComplete = " << (int)result << endmsg;
               break;
            case DVIT_GATE :
               result = checkDonorVitals();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkDonorVitals = " << (int)result << endmsg;
               break;
            case BRUN_UPDATE :
               updateBloodPrimeToBloodRun();
               DataLog(log_level_safe_exec_info) << "State Gate: Do updateBloodPrimeToBloodRun = " << (int)result << endmsg;
               break;
            case BRUN_T1_GATE :
               result = checkBloodRunT1();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkBloodRunT1 = " << (int)result << endmsg;
               break;
            case DISC_GATE :
               result = checkDisconnectTest();
               DataLog(log_level_safe_exec_info) << "State Gate: Do checkDisconnectTest = " << (int)result << endmsg;
               break;
            case LAST_GATE :
               break;
            case UNDEFINED_GATE :
            {
               _FATAL_ERROR(__FILE__, __LINE__, "State Gate: Undefined Gate");
            }
            break;
            default :
            {
               DataLog(log_level_critical) << "State Gate: Unknown Gate = " << doGate << endmsg;
               _FATAL_ERROR(__FILE__, __LINE__, "unrecognized gate");
            }
            break;
         }
         long temp = gatewayMask;
         temp        = temp << 1;
         gatewayMask = (short)temp;
      }
   }
   return(result);

}



// SPECIFICATION:    Instruct various objects of the new state
//                   of the machine so they can set up correctly
//
//
// ERROR HANDLING:   None.
// RETURN:           NA
//
void aSafetySystemStateChangeMsg_rcvr::initializeForState (long newState)
{
   paSafetyHardwareCommandsExec->initializeForState(newState, _paCassette->GetCassettePosition());
   _paReservoir->initializeForState(newState);
   _needle->initializeForState(newState);
   _paDonor->initializeForState(newState);
   _paCassette->initializeForState(newState);
   _paDCTest->initializeForState(newState);
   _paRequestSafetyPowerMsg_rcvr->initializeForState(newState);
   _paDoor->initializeForState(newState);
}


// SPECIFICATION:    Gateway to determine if the CRCs
//                   have been checked
//
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = CRCs OK
//                   FALSE = CRCs not OK
//
bool aSafetySystemStateChangeMsg_rcvr::checkCRCDisconnect ()
{
   // protection from run fault if we have already deleted
   // the crc timer object.
   if (_paCRC == NULL)
      return(FALSE);

   return(_paCRC->CRCDisconnectOK());
}

// SPECIFICATION:    Gateway to determine if the blink test
//                   is working, if the valves have all been
//                   seen, if the cassette positions have
//                   been seens, if the set type is rbc/pls,
//                   plt/pls/rbc.
//
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = OK to do donor connect
//                   FALSE = Not OK to do donor connect
//
bool aSafetySystemStateChangeMsg_rcvr::checkACConnectT1 ()
{
   bool result = TRUE;

   int  test   = (int)FALSE;
   test |= (int)0x08 & ((int)_paCassette->CassetteT1OK() << 3);
   test |= (int)0x04 & ((int)_blink->BlinkT1OK() << 2);
   test |= (int)0x02 & ((int)_paReservoir->CheckValveT1() << 1);
   test |= (int)0x01 & (int)_paReservoir->CheckSetType();

   if (test != 0x0f)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SAFETY_T1_FAILURE);

      DataLog(log_level_safety_alarm_detail) << "0x" << std::hex << test << std::dec << " T1 Fail" << endmsg;

      result = FALSE;
   }
   return(result);
}

// SPECIFICATION:    Gateway to determine if the CRCs
//                   have been checked and power T1
//                   tests have been run and the set type
//                   is plt/pls/rbc, rbc/pls
//
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = OK to do donor connect
//                   FALSE = Not OK to do donor connect
//
bool aSafetySystemStateChangeMsg_rcvr::checkDonorConnectT1 ()
{
   bool result = TRUE;

   int  test   = (int)FALSE;

   test |= (int)0x04 & ((int)_paCRC->CRCT1OK() << 2);
   test |= (int)0x02 & ((int)_paRequestSafetyPowerMsg_rcvr->PowerT1OK() << 1);
   test |= (int)0x01 & (int)_paReservoir->CheckSetType();

   if (test != 0x07)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SAFETY_T1_FAILURE);

      DataLog(log_level_safety_alarm_detail) << "0x" << std::hex << test << std::dec << " T1 Fail" << endmsg;

      result = FALSE;
   }

   return(result);
}

// SPECIFICATION:    Gateway to determine if we have completed
//                   first draw
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = first draw complete
//                   FALSE = first draw not complete
//
bool aSafetySystemStateChangeMsg_rcvr::checkFirstDrawComplete ()
{
   return(_paReservoir->FirstDrawComplete());
}


// SPECIFICATION:    Gateway to determine if we have received
//                   a donor vitals info and it is all acceptable
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = donor vital info acceptable
//                   FALSE = donor vital info not acceptable
//
bool aSafetySystemStateChangeMsg_rcvr::checkDonorVitals ()
{
   return(_paDonor->DonorVitalsOK());
}


// SPECIFICATION:    Capture T1 status on centrifuge at Blood
//                   Prime.
//
// ERROR HANDLING:   None.
// RETURN:           NA
//
void aSafetySystemStateChangeMsg_rcvr::updateBloodPrimeToBloodRun ()
{

   _paCentrifuge->CentrifugeAtBloodPrimeT1();

}

// SPECIFICATION:    Gateway to determine that all T1 tests have
//                   been performed and successfully passed.
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = T1 tests all OK
//                   FALSE = At least one T1 test not OK
//
bool aSafetySystemStateChangeMsg_rcvr::checkBloodRunT1 ()
{
   /// lint fix 07/27/96 parens
   //
   // 08/13/96 to implement test for fluid at low level sensor as condition
   // for transition per David Shouldice add following to if
   // && (_pSHwStateData->reservoir) == (char)RESERVOIR_LOW_FLUID )
   //
   // 09/14/96 modify to allow for seeing unknown position during startup testing
   // if valve is moving and status timing is such that unknown is seen, that is a valid condition
   // that can not be guaranteed in every startup case.
   //

   bool result = TRUE;

   int  test   = (int)FALSE;

   test |= (int)0x02 & ((int)_paCentrifuge->CentrifugeT1OK() << 1);

   DataLog(log_level_safety_alarm_detail) << "0x" << std::hex << test << std::dec << "  Centrifuge T1 test result" << endmsg;

   test |= (int)0x01 & (int)_paDoor->DoorT1OK();

   DataLog(log_level_safety_alarm_detail) << "0x" << std::hex << test << std::dec << "  Door T1 test result" << endmsg;

   if (test != 0x03)
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SAFETY_T1_FAILURE);

      DataLog(log_level_safety_alarm_detail) << "0x" << std::hex << test << std::dec << " T1 Fail" << endmsg;

      result = FALSE;
   }
   else
   {

      DataLog(log_level_safe_exec_info) << "0x" << std::hex << test << std::dec << " T1 Pass" << endmsg;
   }
   // delete the CRC timer
   delete _paCRC;

   return(result);
}


// SPECIFICATION:    Gateway to determine if the disconnect
//                   test has passed.
//
// ERROR HANDLING:   None.
// RETURN:           TRUE = Disconnect Test has passed
//                          or the cassette is not down
//                   FALSE = Disconnect Test has not passed
//                           and the cassette is down
//
bool aSafetySystemStateChangeMsg_rcvr::checkDisconnectTest ()
{
   bool               result = FALSE;
   char               dcTest;
   HW_CASSETTE_STATES statusCass;

   // 08/19/96 fix conditonal to allow for cassette unloading and
   // trans to POST_RUN when power up w/ cassette position
   // NOT up AND NOT down ie. "bad cassette position "
   //
   dcTest     = _paDCTest->GetDonorDisconnectTest();
   statusCass = _paCassette->GetCassettePosition();

   if ( (dcTest == TRUE )
        ||(statusCass != HW_CASSETTE_DOWN ) )
      result = TRUE;

   DataLog(log_level_safe_exec_info) << "Result: "
                                     << result
                                     << ", Disconnect Test: "
                                     << (int)dcTest
                                     << ", Cassette Position : "
                                     << statusCass
                                     << endmsg;

   return(result);
}


// SPECIFICATION:    Restores the object data from the PFR
//                   saved data area.  Called once during PFR.
//
// ERROR HANDLING:   None.
// RETURN:           NA
//
void aSafetySystemStateChangeMsg_rcvr::ifPFRThenRestore ()
{

   if (SafetyPFRDataFromRestore.PFRSafetyOneShot == TRUE)
   {
      DataLog(log_level_safe_exec_info) << "Restoring PFR data and monitoring objects" << endmsg;

      // do "new" restore operations
      SafetyExec::instance()->RestoreFromPFR();

      // restore data to donor objects
      _paDonor->restoreDonorDataFromPFR();

      // restore data to reservoir objects including containers and monitors
      _paReservoir->restoreFromPFR();

      // restore data to the timer object
      _needle->restoreFromPFR();

      // restore data to the cassette object
      _paCassette->restoreFromPFR();

      // restore donor mode in the driver
      paSafetyHardwareCommandsExec->initializeForState(SafetyExec::instance()->SystemState(), HW_CASSETTE_UNKNOWN);

   }
   else
      // set everyone to FIRST_STATE
      initializeForState(FIRST_STATE);

}


// SPECIFICATION:    Restores the data needed to do hypovolemia
//                   monitoring during Donor Disconnect test
//                   from the PFR saved data area if we don't
//                   have donor data when we enter donor disconnect.
//                   Called whenever entering donor disconnect.
//
// ERROR HANDLING:   None.
// RETURN:           NA
//
void aSafetySystemStateChangeMsg_rcvr::checkToRestoreDisconnectData ()
{
   // check if we have donor vitals because we don't want to
   // restore if we already have them
   if ( (_paDonor->GetDonorVitalStatus()) != DONOR_VITAL_NOMINAL_COMPLETE )
   {
      // if we are here, we have transitioned to disconnect without donor data.
      // that means we probably got here from powering up with the cassette down.

      // check if the donor vitals in the PFR data are OK
      // and restore if they are.
      if (SafetyPFRDataFromRestore.PFR_cDonorVitalStatus == DONOR_VITAL_NOMINAL_COMPLETE)
      {
         _paDonor->restoreDonorDataFromPFR();
      }

      // we also need to restore hypovolemia monitoring if it is available
      _paReservoir->restoreReservoirMonitoringForDisconnect();

   }
}


// SPECIFICATION:    Determines what cycle we are in following
//                   a power fail in blood run and switches
//                   cycles if appropriate
//
// ERROR HANDLING:   None.
// RETURN:           NA
//
void aSafetySystemStateChangeMsg_rcvr::whatCycleAreWeIn ()
{
   _paReservoir->switchCycleForPFR();
}

/* FORMAT HASH 94c4d63f1b63b309623157b8fefc6527 */
