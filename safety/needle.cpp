/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.8  2003/09/09 22:44:58Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.7  2003/07/01 22:34:02Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.6  2003/04/03 22:54:59Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.5  2003/01/06 18:13:48Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.4  2002/08/07 11:44:06  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Add needleMonitor::_myTimer to use the new timer message class definition, rather than deriving from a timer message class.
 * Revision 1.3  2002/07/23 16:52:14  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.2  2002/04/09 11:37:52  pn02526
 * "Revise logging to use DataLogLevel class.  Change ASSERT() macro calls to trima_assert()."
 * Revision 1.1  2000/03/15 18:28:50Z  BS04481
 * Initial revision
 *
 * TITLE:      needle.cpp
 *             Safety needle coagulation monitor.
 *
 *
 * ABSTRACT:   This file contains the class methods for the
 *             needle coagulation monitor that makes sure the pump moves
 *             during pauses and PFRs
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 ******************************************************************************/

#include "trima_assert.h"
#include "trima_datalog.h"
#include "needle.hpp"
#include "s_glob.hpp"
#include "s_cont.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
*
*  needleMonitor CLASS METHODS DEFINITION
*
******************************************************************************/
// SPECIFICATION:    needle monitor constructor
//                   The needle monitor ensures that, if there is not
//                   anti-coagulated blood in the needle, the pumps run
//                   in time to not worry about coagulation or they never
//                   run again
//
// ERROR HANDLING:   None
//
needleMonitor::needleMonitor(InletContainer* Inlet,
                             ReturnContainer* Return)
   : _Inlet(Inlet)
     , _Return(Return)
//          , _myTimer( 5000ul, Callback<needleMonitor>(this,&needleMonitor::notify), TimerMessage::DISARMED )
     , _myTimer(5000ul, Callback<needleMonitor>(this, &needleMonitor::notify), TimerMessage::ARMED)
{
   trima_assert(_Inlet);
   trima_assert(_Return);
   _monType                    = MON_INACTIVE;
   _timerHasFired              = FALSE;
   _startTime                  = 0;
   _duration                   = 0;
   _pumpTimerInletAccumulator  = 0.0f;
   _pumpTimerReturnAccumulator = 0.0f;
//   _myTimer.armTimer( TimerMessage::ARMED );

   saveToPFR();
}


// SPECIFICATION:    needle monitor destructor
//
// ERROR HANDLING:   None
//
needleMonitor::~needleMonitor()
{
   _Inlet  = NULL;
   _Return = NULL;
}

// SPECIFICATION:    enables/disables needle monitoring
//
// ERROR HANDLING:   None
//
void needleMonitor::initializeForState (long newState)
{
   switch (newState)
   {
      case BLOOD_PRIME :
      case BLOOD_RUN :
         _monType = MON_RUN_CYCLES;
         break;
      case BLOOD_RINSEBACK :
         _monType = MON_RINSEBACK;
         break;
      case DONOR_DISCONNECT :
         // if we have a pump power timer going, turn it off so we can do the disconnect test
         turnOffTimer();
         _monType = MON_INACTIVE;
         break;
      default :
         _monType = MON_INACTIVE;
         break;
   }
   saveToPFR();
}


// SPECIFICATION:    starts pump timing
//
// ERROR HANDLING:   None
//
void needleMonitor::turnOnTimer (long duration)
{
   timespec now;

   if (_monType == MON_INACTIVE)
      return;

   if (_startTime == 0)
   {
      clock_gettime(CLOCK_REALTIME, &now);
      _startTime                  = now.tv_sec;
      _duration                   = duration;
      _timerHasFired              = FALSE;
      _pumpTimerReturnAccumulator = 0.0f;
      _pumpTimerInletAccumulator  = 0.0f;
      {
         DataLog(log_level_safe_exec_info) << "Timeout: " << _duration <<  " - Pump power timer started." << endmsg;
      }
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "Pump power timer already running." << endmsg;
   }

   saveToPFR();
}

// SPECIFICATION:    checks to be sure sufficient volume is pumped
//                   to assure there is not a clot in the needle
//
// ERROR HANDLING:   None
//
bool needleMonitor::isEnoughPumped ()
{
   bool result = FALSE;
   // if start time is 0, the timer is not active
   // so don't bother to accumulate
   if ( (_startTime == 0)
        ||(_monType == MON_INACTIVE) )
      return(TRUE);

   // accumulate the pumped volume
   _pumpTimerReturnAccumulator += _Return->GetVolume(VOL_DELTA);
   _pumpTimerInletAccumulator  += _Inlet->GetVolume(VOL_DELTA);

   // see if we have had sufficient flow
   // to ensure the needle is uncoagulated
   if (_monType == MON_RUN_CYCLES )
   {
      // if the volume is sufficient, turn off the timer
      if ( (_pumpTimerReturnAccumulator >= PFR_AC_IN_NEEDLE_VOL)
           ||(_pumpTimerInletAccumulator >= PFR_AC_IN_NEEDLE_VOL) )
      {
         turnOffTimer();
         result = TRUE;
      }
   }
   else if (_monType == MON_RINSEBACK)
   {
      // if we are in rinseback, we can also turn off the timer
      // if we have 3ml more return than inlet volume.
      if ( (_monType == MON_RINSEBACK)
           &&( (_pumpTimerReturnAccumulator - _pumpTimerInletAccumulator)
               >= PFR_AC_IN_NEEDLE_VOL) )
      {
         turnOffTimer();
         result = TRUE;
      }
   }
   saveToPFR();
   return(result);
}


// SPECIFICATION:    Returns TRUE if timer is already running
//
// ERROR HANDLING:   None
//
bool needleMonitor::isTimerRunning ()
{
   if (_startTime != 0)
      return(TRUE);
   else
      return(FALSE);
}


// SPECIFICATION:    stops pump timing
//
// ERROR HANDLING:   None
//
void needleMonitor::turnOffTimer ()
{
   if (_startTime != 0)
   {
      DataLog(log_level_safe_exec_info) << "Delete pump power timer. Return Vol: "
                                        << _pumpTimerReturnAccumulator
                                        << ", Inlet Vol: "
                                        << _pumpTimerInletAccumulator
                                        << endmsg;
   }

   // rezero the start time to disable the timer
   _startTime = 0;
   saveToPFR();
}


// SPECIFICATION:    every 5 seconds check to see if the timer is
//                   running and has expired
//
// ERROR HANDLING:   If the timer is expired, turn off 24v and 64v and
//                   keep it off
//
void needleMonitor::notify ()
{
   timespec now;
   long     dt = 0;

   clock_gettime(CLOCK_REALTIME, &now);
   if (_startTime != 0)
      dt = now.tv_sec - _startTime;
   else
      return;

   // if more than the maximum power outage duration
   // has gone by, block the pump power.
   if ( (dt > _duration)
        &&(_timerHasFired == FALSE) )
   {
      paSafetyHardwareCommandsExec->BlockPumpPower(TRUE);
      {
         DataLog(log_level_safe_exec_error) << "Timer: " << dt
                                            << " - Pump power exceeds maximum time of " << _duration
                                            << " seconds. Power requests blocked" << endmsg;
      }
      _timerHasFired = TRUE;
   }
   saveToPFR();
}


// SPECIFICATION:    Save data for PFR
//
// ERROR HANDLING:   None
//
void needleMonitor::saveToPFR ()
{
   SafetyPFRDataToSave.PFRNeedle_monType                    = _monType;
   SafetyPFRDataToSave.PFRNeedle_startTime                  = _startTime;
   SafetyPFRDataToSave.PFRNeedle_duration                   = _duration;
   SafetyPFRDataToSave.PFRNeedle_timerHasFired              = _timerHasFired;
   SafetyPFRDataToSave.PFRNeedle_pumpTimerInletAccumulator  = _pumpTimerInletAccumulator;
   SafetyPFRDataToSave.PFRNeedle_pumpTimerReturnAccumulator = _pumpTimerReturnAccumulator;
}


// SPECIFICATION:    Restore data for PFR
//
// ERROR HANDLING:   None
//
void needleMonitor::restoreFromPFR ()
{
   _monType                    = SafetyPFRDataFromRestore.PFRNeedle_monType;
   _startTime                  = SafetyPFRDataFromRestore.PFRNeedle_startTime;
   _duration                   = SafetyPFRDataFromRestore.PFRNeedle_duration;
   _timerHasFired              = SafetyPFRDataFromRestore.PFRNeedle_timerHasFired;
   _pumpTimerInletAccumulator  = SafetyPFRDataFromRestore.PFRNeedle_pumpTimerInletAccumulator;
   _pumpTimerReturnAccumulator = SafetyPFRDataFromRestore.PFRNeedle_pumpTimerReturnAccumulator;

   saveToPFR();
}

/* FORMAT HASH 641732209a83a99bfb02916ba0b07556 */
