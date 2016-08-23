/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfbranch.cpp
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/pfbranch.cpp 1.21 2008/07/30 21:57:52Z dslausb Exp jheiusb $
 * $Log: pfbranch.cpp $
 * Revision 1.21  2008/07/30 21:57:52Z  dslausb
 * Fixed some PFR issues
 * Revision 1.20  2007/06/11 22:37:18Z  dslausb
 * IT 8084 - PFR button returning to Two-Button screen
 * Revision 1.19  2007/05/30 20:54:25Z  dslausb
 * IT 8050 - Fixed PFR problems
 * Revision 1.18  2006/03/27 17:53:07Z  jheiusb
 * 7350 -- PFR for MSS
 * Revision 1.17  2006/03/18 00:06:01Z  jheiusb
 * revised for PFR in MSS
 * Revision 1.16  2003/11/05 22:13:52Z  ms10234
 * 5959 - compensate for time drift between control and safety.
 * Revision 1.15  2003/07/08 22:18:29Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.14  2003/04/08 17:05:00Z  jl11312
 * - datalog level changes
 * Revision 1.13  2002/10/28 23:31:23Z  ms10234
 * Changed proc files to use interface class - ProcData
 * Revision 1.12  2002/10/03 23:33:58  ms10234
 * Changes to make proc execute
 * Revision 1.11  2002/08/06 18:56:12  ms10234
 * Changed all int message names to end with "Msg"
 * Revision 1.10  2002/06/21 21:25:07  sb07663
 * Mods to remove dynamic data and implement CDS
 * Revision 1.9  2002/06/05 14:30:00  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.8  2002/04/09 17:22:31  sb07663
 * Modifications for compatibility with VxWorks port
 * Revision 1.7  2002/03/01 20:07:24  jl11312
 * - deregister for rinseback handshake with GUI on state exit (IT 5608)
 * Revision 1.6  2001/08/31 20:17:56  jl11312
 * - changed pfbranch state to close valves before resuming procedure
 * Revision 1.5  2001/07/26 21:58:07  jl11312
 * - added message for proc to notify control driver that air to donor monitor should be enabled (IT 5235)
 * Revision 1.4  2001/06/21 21:36:24  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.3  2001/06/13 22:16:01  jl11312
 * - added handling of transition to start draw screen from power fail
 * - added restoration of alarms during power fail recovery (IT 4922)
 * Revision 1.2  2001/06/07 21:57:19  jl11312
 * - corrections for recovery to AC primed state
 * Revision 1.1  2001/06/05 21:20:02  jl11312
 * Initial revision
 */

#include "guiproc.h"
#include "pfbranch.h"
#include "states.h"
#include "procpfr.h"
#include "procdata.h"
#include "MssCommon.h"

DEFINE_STATE(PFBranch);

PFBranch::PFBranch()
   : StateAbs(),
     _currentAlarm(NULL_ALARM),
     _currentAlarmSet(0),
     _currentRinsebackStatusSet(0),
     _alarmsRestored(0),
     _rinsebackStatusMsg(),
     _ValveTimer(0, Callback<PFBranch>(this, &PFBranch::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

PFBranch::PFBranch(const PFBranch& state)
   : StateAbs(state),
     _currentAlarm(state._currentAlarm),
     _currentAlarmMsgTime(state._currentAlarmMsgTime),
     _currentAlarmSet(state._currentAlarmSet),
     _currentRinsebackStatusSet(state._currentRinsebackStatusSet),
     _alarmsRestored(state._alarmsRestored),
     _rinsebackStatusMsg(),
     _ValveTimer(0, Callback<PFBranch>(this, &PFBranch::valvesTimedOut), TimerMessage::DISARMED),
     _RBCAlarm(RBC_VALVE_FAILURE),
     _PlasmaAlarm(PLASMA_VALVE_FAILURE),
     _CollectAlarm(COLLECT_VALVE_FAILURE)
{}

PFBranch::~PFBranch()
{
   cleanup();
}

int PFBranch::transitionStatus ()
{
   ProcData pd;
   //
   // Check and see if ALL valves are in the proper position
   //
   if ( pd.Status().RBCValve.IsReturning() &&
        pd.Status().PlasmaValve.IsReturning() &&
        pd.Status().CollectValve.IsReturning() )
   {
      return _TransitionStatus;
   }

   return NO_TRANSITION;
}

int PFBranch::preProcess ()
{
   commandValves();
   if ( !_alarmsRestored )
   {
      if ( _currentAlarm == NULL_ALARM ||
           (_currentAlarmSet && _currentRinsebackStatusSet) )
      {
         ProcData pd;
         _currentAlarm = pd.PFRHandler().GetNextAlarm();
         if ( _currentAlarm == NULL_ALARM )
         {
            _alarmsRestored = 1;
         }
         else
         {
            anAlarmMsg alarmMsg(_currentAlarm);
            alarmMsg.setAlarm();

            _currentAlarmSet           = 0;
            _currentRinsebackStatusSet = 0;
            clock_gettime(CLOCK_REALTIME, &_currentAlarmMsgTime);
         }
      }
   }

   return NORMAL;
}

int PFBranch::postProcess ()
{
   ProcData pd;
   if ( pd.AlarmQ().isActive(_currentAlarm) )
   {
      _currentAlarmSet = 1;
   }

   if ( _currentAlarm != NULL_ALARM &&
        (!_currentAlarmSet || !_currentRinsebackStatusSet) )
   {
      struct timespec now;
      clock_gettime(CLOCK_REALTIME, &now);

      double dt = (now.tv_sec - _currentAlarmMsgTime.tv_sec);
      dt += (now.tv_nsec - _currentAlarmMsgTime.tv_nsec) / 1.0e9;

      //
      // Wait a maximum of 1/2 second for alarm to be set and rinseback
      // status message to be received from GUI.  If we haven't gotten
      // notification by then, just go to the next alarm
      //
      if ( dt < 0.0 || dt > 0.5 )
      {
         _currentAlarmSet           = 1;
         _currentRinsebackStatusSet = 1;
      }
   }

   if ( _alarmsRestored )
   {
      handleTransition();
   }

   return NORMAL;
}

int PFBranch::preEnter ()
{
   ProcData pd;
   //
   // Set up to restore alarms from power fail recovery data
   //
   pd.PFRHandler().ResetAlarmIndex();

   //
   // Register for the request for rinseback status.  This is sent by GUI to
   // determine if the rinseback button should be displayed on the alarm screen.
   //
   _rinsebackStatusMsg.init(Callback<PFBranch>(this, &PFBranch::rinsebackStatus), MessageBase::SNDRCV_LOCAL);

   //
   // Command the valves and start the timer for valve motion
   //
   commandValves();
   _ValveTimer.interval(10000);   // 10 seconds ( 10000 msecs )

   return NORMAL;
}

int PFBranch::postExit ()
{
   ProcData pd;
   cleanup();
   pd.PFRHandler().EnablePFSave();
   return NORMAL;
}

void PFBranch::reset ()
{
   _TransitionStatus = NO_TRANSITION;
}

void PFBranch::cleanup ()
{
   _ValveTimer.interval(0);
   _rinsebackStatusMsg.deregisterMsg();
}

//
// Determine transition type to resume run
//
void PFBranch::handleTransition (void)
{
   ProcData pd;


   switch ( pd.TrimaSet().CassetteState.Get() )
   {
      case ProcTrimaSet_CDS::ACPRIMED :
         // Stack objects must be scoped
      {
         //
         // Set Startup Test State message to get GUI to two button screen
         //
         SystemStateChangeMsg systemStateMsg(MessageBase::SEND_GLOBAL);
         systemStateMsg.send( (State_names)STARTUP_TESTS);
         DataLog(log_level_proc_pfr_info) << "Sent system state change message " << (State_names)STARTUP_TESTS << endmsg;

         //
         // Inform GUI of the current donor and cassette state
         //
         GUISysDonorStateMsg guiStateMsg(MessageBase::SEND_LOCAL);
         guiStateMsg.send(SETUP_COMPLETE_DONOR_NOTCOMPLETE);
         DataLog(log_level_proc_pfr_info) << "Sent GUI state message" << endmsg;

         DataLog(log_level_proc_pfr_info) << "Transitioning to AC prime" << endmsg;
         _TransitionStatus = TransitionToACPrime;
      }
      break;

      case ProcTrimaSet_CDS::BLOOD :
         DataLog(log_level_proc_pfr_info) << "Transitioning to connect donor" << endmsg;
         _TransitionStatus = TransitionToConnectDonor;
         break;

      case ProcTrimaSet_CDS::STORAGE :
         DataLog(log_level_proc_pfr_info) << "Transitioning to Metered Storage" << endmsg;


         pd.MakeRunWritable();
         // setup this for PFR
         pd.Run().mssInPFR = true; // looks like we're in MSS PFR to me!
         pd.MakeRunReadable();


         _TransitionStatus = TransitionToMSS;
         break;

      case ProcTrimaSet_CDS::STORAGE_COMPLETE :
         DataLog(log_level_proc_pfr_info) << "Transitioning to Metered Storage Disconnect" << endmsg;
         _TransitionStatus = TransitionToMSSDisconnect;
         break;

      case ProcTrimaSet_CDS::USED :

         DataLog(log_level_proc_pfr_info) << "Transitioning to Donor Disconnect" << endmsg;
         _TransitionStatus = TransitionToDonorDisconnect;

         break;


      default :
         // Stack objects must be scoped
      {

         DataLog(log_level_proc_pfr_info) << "PFBranch is in Default case!" << endmsg;
         PFRForceAirToDonorMonitorMsg forceAirToDonorMonitor(MessageBase::SEND_GLOBAL);
         forceAirToDonorMonitor.send(1);
         DataLog(log_level_proc_pfr_info) << "Sent force air to donor monitor message" << endmsg;

         DataLog(log_level_proc_pfr_info) << "Transitioning to donor connected" << endmsg;
         _TransitionStatus = TransitionToDonorConnected;
      }
      break;
   }

}

//
// Respond to a request for rinseback status from GUI
//
void PFBranch::rinsebackStatus (void)
{
   ProcData pd;
   if ( _rinsebackStatusMsg.getData() == REQUEST_RINSEBACK_STATUS)
   {
      RINSEBACK_STATUS status = ( pd.Run().FirstDrawComplete.Get() ? RINSEBACK_OK : RINSEBACK_NOT_OK );
      _rinsebackStatusMsg.send(status);

      _currentRinsebackStatusSet = 1;
   }
}

void PFBranch::commandValves ()
{
   ProcData pd;
   //
   // Command all valves
   //
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
}

void PFBranch::valvesTimedOut ()
{
   ProcData pd;
   //
   // Determine which valve(s) FAILED and set appropriate alarm(s)
   //
   if ( !pd.Status().RBCValve.IsReturning() )
   {
      _RBCAlarm.setAlarm();
   }
   if ( !pd.Status().PlasmaValve.IsReturning() )
   {
      _PlasmaAlarm.setAlarm();
   }
   if ( !pd.Status().CollectValve.IsReturning() )
   {
      _CollectAlarm.setAlarm();
   }
}

/* FORMAT HASH 25c51456c8817c9de7f999bac5a1c8cc */
