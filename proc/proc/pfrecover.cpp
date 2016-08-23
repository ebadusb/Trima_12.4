/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pfrecover.cpp
 *
 * Revision 1.40  2008/07/31 22:05:55Z  dslausb
 * Nuke some extraneous logging I introduced
 * Revision 1.39  2008/07/30 21:58:00Z  dslausb
 * Fixed some PFR issues
 * Revision 1.38  2008/04/08 21:43:05Z  jheiusb
 * 8115 -- fix prime and General PFR stuff
 * Revision 1.37  2007/06/11 22:56:55Z  dslausb
 * Remove some extra logging
 * Revision 1.36  2007/05/30 20:54:37Z  dslausb
 * IT 8050 - Fixed PFR problems
 * Revision 1.35  2007/05/16 22:23:12Z  jheiusb
 * 7579,7961,7980 -- PFR for donor diconnect
 * Revision 1.34  2007/04/27 15:58:44Z  jheiusb
 * 7961 -- allow continue in Donor Disconnect on PFR
 * Revision 1.33  2007/04/26 17:05:07Z  dslausb
 * Fixed PFR cassette func bits message.
 * Revision 1.32  2007/01/24 21:13:11Z  dslausb
 * Fixed MSS PFR gui messages
 * Revision 1.31  2006/10/10 18:06:44Z  jheiusb
 * 7760 -- disallow MSS after a A2
 * Revision 1.30  2006/06/16 16:49:39Z  jheiusb
 * 7574 -- stop MSS restarting after A2 donodDiconnect.
 * Revision 1.29  2006/03/03 17:33:15Z  jheiusb
 * initial merge
 * Revision 1.28  2003/11/27 00:11:47Z  ms10234
 * 6611 - don't send donor information if we are out of the run phases
 * Revision 1.27  2003/11/04 21:39:47Z  ms10234
 * 6577 - don't ask for prediction during rinseback
 * Revision 1.26  2003/08/15 14:53:26Z  rm70006
 * IT 6362.  Change message proc sends to barcode for pfr restore.
 * Revision 1.25  2003/08/11 15:44:39Z  ms10234
 * 5829 - changes for pfr
 * Revision 1.24  2003/07/14 16:16:47Z  ms10234
 * 6027 - Sent Zero calibration number for APS sensor on PFR.
 * Revision 1.23  2003/07/08 22:18:35Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.22  2003/06/19 18:43:34Z  ms10234
 * 5829 - changes for PFR
 * Revision 1.21  2003/04/30 14:09:48Z  jl11312
 * - start of power fail recovery implementation
 * Revision 1.20  2002/10/03 23:34:01Z  ms10234
 * Changes to make proc execute
 * Revision 1.19  2002/07/16 16:56:58  sb07663
 * Updates for V5 changes since baseline
 * Revision 1.18  2002/06/05 14:30:01  sb07663
 * Modifications for compatibility with new Vx messaging system
 * Revision 1.17  2002/04/18 15:12:18  sb07663
 * Incorporated mods from V5 since Vx baseline established
 * Revision 1.16  2002/04/09 17:22:32  sb07663
 * Modifications for compatibility with VxWorks port
 * Revision 1.15  2002/03/01 20:07:27  jl11312
 * - deregister for rinseback handshake with GUI on state exit (IT 5608)
 * Revision 1.14  2002/02/21 21:01:50  jl11312
 * - save/restore substate across power fails (IT 5601)
 * Revision 1.13  2002/01/09 20:40:11  jl11312
 * - AdjustCtrl message is now sent after selecting procedure during PFR
 * Revision 1.12  2001/12/11 20:16:51  jl11312
 * - added handling for Vista PFR status message
 * Revision 1.11  2001/09/27 17:51:01  jl11312
 * - restore RBC calibration data after power fail (IT 5331)
 * Revision 1.10  2001/07/24 15:19:22  jl11312
 * - added handling for VistaPFRRequest message
 * Revision 1.9  2001/07/18 21:56:33  jl11312
 * - modified to have safety notify proc of safety PFR file status
 * Revision 1.8  2001/07/13 15:35:57  jl11312
 * - added code to send out restored adjust control values following PFR (IT 5196)
 * Revision 1.7  2001/06/21 21:36:28  jl11312
 * - corrected handling of alarms during PFR (IT 4922)
 * Revision 1.6  2001/06/14 18:00:07  jl11312
 * - disabled PFR notifications to predict unless run is resuming (IT 4922)
 * - reorganized function calls to improve readability
 * Revision 1.5  2001/06/13 22:17:38  jl11312
 * - added restoration of alarms during power fail recovery (IT 4922)
 * Revision 1.4  2001/06/07 21:57:05  jl11312
 * - corrections for recovery to AC primed state
 * Revision 1.3  2001/06/05 21:18:37  jl11312
 * - added recovery from AC primed and blood prime states (IT 4922)
 * Revision 1.2  2001/06/05 20:09:50  jl11312
 * - added handling of power-fail recovery to rinseback
 * - changes required to move power test after continue from power-interrupted alarm
 * Revision 1.1  2001/06/01 20:15:41  jl11312
 * Initial revision
 */

#include <vxworks.h>
#include "guiproc.h"
#include "periodic_log.h"
#include "proctime_cds.h"
#include "pfrecover.h"
#include "predict_msgs.h"
#include "states.h"
#include "procdata.h"
#include "MssCommon.h"

DEFINE_STATE(PFRecover);

PFRecover::PFRecover()
   : StateAbs(),
     _pfAlarmHasBeenSet(false),
     _pfAlarmType( (ALARM_VALUES)-1),
     _rinsebackAllowed(false),
     _rinsebackRequestMsg(),
     _rinsebackStatusMsg(),
     _modifyMessageTimer(0, Callback<PFRecover>(this, &PFRecover::sendModifyPredictMessage), TimerMessage::DISARMED)
{}

PFRecover::PFRecover(const PFRecover& state)
   : StateAbs(state),
     _pfAlarmHasBeenSet(false),
     _pfAlarmType( (ALARM_VALUES)-1),
     _rinsebackAllowed(false),
     _rinsebackRequestMsg(),
     _rinsebackStatusMsg(),
     _modifyMessageTimer(0, Callback<PFRecover>(this, &PFRecover::sendModifyPredictMessage), TimerMessage::DISARMED)
{
   PFRecover::copyOver(state);
}

PFRecover::~PFRecover()
{}

int PFRecover::transitionStatus ()
{
   return _TransitionStatus;
}

int PFRecover::preProcess ()
{
   ProcData pd;

   if (    _TransitionStatus == NO_TRANSITION
           && ( pd.PFRHandler().IsPFDataAvailable() || !pd.Status().CassIsUp() )
           )
   {
      setPFAlarm();
   }

   return NORMAL;
}

int PFRecover::postProcess ()
{
   ProcData pd;

   //
   // Check if we have set the appropriate power fail alarm
   //
   if ( _pfAlarmType != (ALARM_VALUES)-1 &&
        pd.AlarmQ().isActive(_pfAlarmType) )
   {
      _pfAlarmHasBeenSet = 1;
   }

   //
   // If we have set a power fail alarm and the alarm is now cleared,
   // we can continue to PFPowerTests
   //
   if (    _pfAlarmType != (ALARM_VALUES)-1
           && _pfAlarmHasBeenSet
           && !pd.AlarmQ().isActive(_pfAlarmType) )
   {
      _TransitionStatus = TransitionToPFPowerTests;
   }

   //
   // If we are not performing a power fail recovery, go immediately
   // to startup tests.
   //
   if (    !pd.PFRHandler().IsPFDataAvailable()
           && pd.Status().CassIsUp() )
   {
      _TransitionStatus = TransitionToStartupTests;
   }

   return NORMAL;
}

int PFRecover::preEnter ()
{
   ProcData pd;

   //
   // Handle interaction with GUI for rinseback status/request
   //
   handleRinseback();

   //
   // Handle proc to safety donor message.  This should only be sent if we are
   // restoring donor data (i.e. if we have reached the connect donor state but have
   // finished rinseback).
   //
   if (    pd.PFRHandler().IsPFDataAvailable()
           && pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::BLOOD
           && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::USED )
   {
      handleProcToSafetyDonorMessage();
   }

   //
   // Notify other tasks if we are restoring power fail data
   //
   DataLog(log_level_proc_pfr_info) << "PFR -> funct bits are !! " << pd.Run().CassetteFunctionBits.Get() << endmsg;
   handlePowerFailRecoverNotification();

   //
   // Restore RBC detector calibration and APS calibration if necessary
   //
   if ( pd.Status().CassIsDown() )
   {
      RBCRestoreCalibrationMsg rbcCalMsg(MessageBase::SEND_GLOBAL);
      rbcCalMsg.send(pd.TrimaSet().RBCDetectorCalibrated.Get() ? 1 : 0);
      DataLog(log_level_proc_pfr_info) << "Restored RBC detector calibration values ( calibrated="
                                       << (bool)pd.TrimaSet().RBCDetectorCalibrated.Get() << ")" << endmsg;

      // Inform HAL and safety of new zero point for APS
      APSZeroMsg apsZeroMsg(MessageBase::SEND_GLOBAL);
      float      apsAverage = pd.TrimaSet().APSZero.Get();
      apsZeroMsg.send((int)(apsAverage + ((apsAverage >= 0) ? 0.5 : -0.5)));
      DataLog(log_level_proc_pfr_info) << "Restored APS Zero calibration value ( APS average=" << apsAverage << ")" << endmsg;

      //
      // Start periodic logging at 6 second rate ...
      //
      PeriodicLog::enablePeriodicity(6.0);

   }

   return NORMAL;
}

int PFRecover::postExit ()
{
   ProcData pd;

   //
   // If we are going to PFPowerTests and we have a product, we need to
   // notify predict and force a reoptimization.
   //
   if (    _TransitionStatus == TransitionToPFPowerTests
           && pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::BLOOD
           && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
           && pd.PFRHandler().IsPFDataAvailable()
           && !( _modifyMessageTimer.timerArmed() ) )
   {
      // handlePredictNotification();
      _modifyMessageTimer.interval(5000);
      DataLog(log_level_proc_pfr_info) << "jph: timer set" << endmsg;
   }

   //
   // If we are not restoring power-fail data, we need to notify
   // GUI that we are transitioning to startup tests.
   //
   if ( _TransitionStatus == TransitionToStartupTests )
   {
      pd.SystemState(STARTUP_TESTS);
   }

   return NORMAL;
}

void PFRecover::reset ()
{
   _pfAlarmType       = (ALARM_VALUES)-1;
   _pfAlarmHasBeenSet = false;
   _TransitionStatus  = NO_TRANSITION;
   _rinsebackAllowed  = false;
}

void PFRecover::cleanup ()
{}

//
// Set the appropriate power fail alarm
//
void PFRecover::setPFAlarm ()
{
   ProcData pd;

   //
   // By default, only end run is allowed
   //
   ALARM_VALUES newAlarmType = POWER_FAIL_ABORT;

   // Declare these out here so we can log 'em.
   bool lookedAtRecoveryByStates        = false,
        rinsebackAllowedByAlarmState    = false,
        rinsebackAllowedByCassetteState = false,
        continueAllowedByAlarmState     = false,
        continueAllowedByCassetteState  = false;

   //
   // Check if continue or rinseback should be allowed
   //
   bool rinsebackAllowed = 0;
   bool continueAllowed  = 0;
   if ( pd.PFRHandler().IsPFDataAvailable() &&
        pd.PFRHandler().IsRunRecoverable() &&
        pd.PFRHandler().IsSafetyPFRFileOK() &&
        pd.Status().CassIsDown() )
   {
      lookedAtRecoveryByStates = true;

      checkRecoveryByAlarmState(continueAllowedByAlarmState, rinsebackAllowedByAlarmState);
      checkRecoveryByCassetteState(continueAllowedByCassetteState, rinsebackAllowedByCassetteState);

      continueAllowed  = continueAllowedByAlarmState && continueAllowedByCassetteState;
      rinsebackAllowed = rinsebackAllowedByAlarmState && rinsebackAllowedByCassetteState;

      if ( continueAllowed )
      {
         newAlarmType = POWER_FAIL_CONTINUE;
      }
      else if ( rinsebackAllowed )
      {
         newAlarmType = POWER_FAIL_RINSEBACK;
      }
      else
      {
         newAlarmType = POWER_FAIL_ABORT;
      }
   }

   //
   // If the alarm type has changed, set the new power fail alarm
   //
   if ( newAlarmType != _pfAlarmType )
   {
      if (lookedAtRecoveryByStates)
      {
         DataLog(log_level_proc_pfr_info) << "PFR alarm type[" << newAlarmType
                                          << "]; continue allowed[" << continueAllowed << "]=(byAlarm["
                                          << continueAllowedByAlarmState << "] && byCassette["
                                          << continueAllowedByCassetteState << "]); rinseback allowed["
                                          << rinsebackAllowed << "]=(byAlarm[" << rinsebackAllowedByAlarmState
                                          << "] && byCassette[" << rinsebackAllowedByCassetteState
                                          << "]); data available[" << pd.PFRHandler().IsPFDataAvailable()
                                          << "]; run recoverable[" << pd.PFRHandler().IsRunRecoverable()
                                          << "]; safety PFR file OK[" << pd.PFRHandler().IsSafetyPFRFileOK()
                                          << "]; cassette down[" << pd.Status().CassIsDown() << "];" << endmsg;
      }
      else
      {
         // Don't bother logging all that cassette and alarm state stuff if we
         // didn't even look at it.
         DataLog(log_level_proc_pfr_info) << "PFR alarm type[" << newAlarmType
                                          << "]; data available[" << pd.PFRHandler().IsPFDataAvailable()
                                          << "]; run recoverable[" << pd.PFRHandler().IsRunRecoverable()
                                          << "]; safety PFR file OK[" << pd.PFRHandler().IsSafetyPFRFileOK()
                                          << "]; cassette down[" << pd.Status().CassIsDown() << "];" << endmsg;
      }

      _rinsebackAllowed = rinsebackAllowed;

      _pfAlarmType      = newAlarmType;
      anAlarmMsg pfAlarm(newAlarmType);
      pfAlarm.setAlarm();

      _pfAlarmHasBeenSet = false;

   }

   // bail on MSS if this is an unrecoverable run
   if (!pd.PFRHandler().IsRunRecoverable())
   {
      // IT 7942 --if cassette is in Donor Disconnect when A2/ or DoDisc fails (like a2) then still give it a try
      if ( !(pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::USED) )
      {


         if (!pd.Run().AllowMSSAfterA2SpecialCase.Get())
         {     // just do this once please!
            if (!pd.Run().cancelMSS.Get())
            {

               DataLog(log_level_proc_pfr_info) << "Cancel any further MSS due to an Unrecoverable PFR run (probably A2)" << endmsg;

               pd.MakeRunWritable();
               pd.Run().cancelMSS.Set(true);
               pd.MakeRunReadable();
            }
         }
         else
         {
            pd.MakeRunWritable();
            pd.Run().cancelMSS.Set(false);
            pd.MakeRunReadable();

            DataLog(log_level_proc_pfr_info) << "allowing MSS after an A2 alarm in rinseback only (probably air alarm)" << endmsg;

         }
      }
   }
}

//
// Check if alarms which were active at power fail disallow a recovery
//
void PFRecover::checkRecoveryByAlarmState (bool& continueAllowed, bool& rinsebackAllowed)

{
   ProcData    pd;

   ALARM_TYPES alarmType = pd.PFRHandler().AlarmState();

   switch ( alarmType )
   {
      case A2 :
      case A1 :
         continueAllowed  = false;
         rinsebackAllowed = false;
         break;

      case R2 :
         continueAllowed  = false;
         rinsebackAllowed = true;
         break;

      case R1 :
      case OPERATOR_ATTENTION :
         continueAllowed  = true;
         rinsebackAllowed = true;
         break;

      default :
         DataLog(log_level_critical) <<  "PFR alarm state: " << pd.PFRHandler().AlarmState() <<  " invalid from power fail data" << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid alarm state from power fail data");
   }
}

//
// Determine the type of power fail alarm which should be set
//
void PFRecover::checkRecoveryByCassetteState (bool& continueAllowed, bool& rinsebackAllowed)
{
   ProcData pd;
   int      cassetteState = pd.TrimaSet().CassetteState.Get();

   switch ( cassetteState )
   {
      case ProcTrimaSet_CDS::ACPRIMED :
         //
         // no time limit on recovery from AC primed state
         //
         continueAllowed  = true;
         rinsebackAllowed = false;
         break;

      case ProcTrimaSet_CDS::BLOOD :
         //
         // no time limit on recovery from connect donor state
         //
         continueAllowed  = true;
         rinsebackAllowed = false;
         break;

      case ProcTrimaSet_CDS::BLOODPRIME :
         //
         // check for coagulated blood at needle
         //
         continueAllowed  = pd.PFRHandler().ACCheck();
         rinsebackAllowed = false;
         break;

      case ProcTrimaSet_CDS::PRIMED :
         //
         // check for coagulated blood at needle
         //
         if ( pd.PFRHandler().ACCheck() )
         {
            //
            // AC check was OK - allow operator to continue
            //
            continueAllowed  = true;
            rinsebackAllowed = pd.Run().FirstDrawComplete.Get();
         }
         else
         {
            continueAllowed  = false;
            rinsebackAllowed = false;
         }

         break;

      case ProcTrimaSet_CDS::RINSEBACK :
         //
         // check for coagulated blood at needle
         //
         continueAllowed = false;
         if ( pd.PFRHandler().ACCheck() )
         {
            //
            // AC check was OK - allow operator to continue with rinseback
            //
            rinsebackAllowed = true;
         }
         else
         {
            rinsebackAllowed = false;
         }

         break;

      case ProcTrimaSet_CDS::STORAGE :
      {

         continueAllowed  = true;
         rinsebackAllowed = false;
         break;
      }

      case ProcTrimaSet_CDS::USED :
      {
         rinsebackAllowed = false;
         continueAllowed  = readyForPLTMSS(true) || readyForRBCMSS(true);

         if (continueAllowed) DataLog (log_level_proc_pfr_info) << "PFR after rinseback. Mss is to take place, so continue is allowed." << endmsg;
         else DataLog (log_level_proc_pfr_info) << "PFR after rinseback. Mss is NOT to take place, so continue is NOT allowed." << endmsg;

         break;
      }

      case ProcTrimaSet_CDS::STORAGE_COMPLETE :
         continueAllowed  = true;
         rinsebackAllowed = false;
         break;

      default :
         continueAllowed  = false;
         rinsebackAllowed = false;
         break;

   }


   // IT 7942 --if cassette is in Donor Disconnect when A2/ or DoDisc fails (like a2) then still give it a try
   if ( !(pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::USED) )
   {
      if (pd.PFRHandler().AlarmState() == A2 )
      {
         if (!pd.Run().AllowMSSAfterA2SpecialCase.Get())
         {
            pd.MakeRunWritable();
            DataLog(log_level_proc_pfr_info) << "Cancel any further MSS due to an A2 alarm" << endmsg;
            pd.Run().cancelMSS.Set(true);
            pd.MakeRunReadable();
         }
         else
         {
            DataLog(log_level_proc_pfr_info) << "allowing MSS after an A2 alarm in rinseback only (probably air alarm)" << endmsg;
         }

      }
   }
}

//
// Handle registering for messages associated with rinseback
//
void PFRecover::handleRinseback ()
{
   //
   // Register for the request for rinseback.  This message is sent when the
   // operator presses the rinseback button on the alarm screen.
   //
   _rinsebackRequestMsg.init(Callback<PFRecover>(this, &PFRecover::rinsebackRequest) );

   //
   // Register for the request for rinseback status.  This is sent by GUI to
   // determine if the rinseback button should be displayed on the alarm screen.
   //
   _rinsebackStatusMsg.init(Callback<PFRecover>(this, &PFRecover::rinsebackStatus), MessageBase::SNDRCV_GLOBAL);
}

//
// Respond to a request for rinseback status from GUI
//
void PFRecover::rinsebackStatus ()
{
   if ( _rinsebackStatusMsg.getData() == REQUEST_RINSEBACK_STATUS)
   {
      RINSEBACK_STATUS status = ( _rinsebackAllowed ? RINSEBACK_OK : RINSEBACK_NOT_OK );
      _rinsebackStatusMsg.send(status);
   }
}

//
// Handle user pressing the rinseback button
//
void PFRecover::rinsebackRequest ()
{
   ProcData pd;
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::RINSEBACK);
   _TransitionStatus = TransitionToPFPowerTests;
}

//
// Send donor information to safety for verification
//
void PFRecover::handleProcToSafetyDonorMessage ()
{
   ProcData        pd;

   DVitalsFromProc donorMsg(MessageBase::SEND_GLOBAL);
   SDonorVitals    donor;
   pd.FillInDonorStruct(donor);

   donorMsg.send(donor);
}

//
// Notify other tasks if we are restoring power fail data
//
void PFRecover::handlePowerFailRecoverNotification ()
{
   ProcData pd;

   //
   // Notify bar code task of PFR status
   //
   PFRToOccurMsg       pfrBarcodeMsg(MessageBase::SEND_GLOBAL);
   Message<PFR_Status> pfrMsg(MessageBase::SEND_LOCAL);

   if ( pd.PFRHandler().IsPFDataAvailable() )
   {
      DataLog (log_level_proc_pfr_info) << "PFR is to occur.  Sending appropriate messages." << endmsg;
      pfrMsg.send(RestorePFRData);

      Proc2GuiPfrSetFuncBits pfrCassetteFuncBitsMsg(MessageBase::SEND_LOCAL);
      pfrCassetteFuncBitsMsg.send(pd.Run().CassetteFunctionBits.Get());
   }
   else
   {
      pfrMsg.send(NoRestore);
   }

}

//
// Notify predict that we are recovering power fail data.  Set up the
// appropriate product and force a reoptimization.
//
void PFRecover::handlePredictNotification ()
{
   ProcData pd;

   DataLog (log_level_proc_alarm_monitor_info) << "handlePredictNotification" << endmsg;
   //  this block resets the pump pause timer to NOW this should suppress startup related pause alerts.
   //
   // IT-12729 allow no 1/3/10 minute alarms after a PFR.
   struct timespec temptime;
   clock_gettime(CLOCK_REALTIME, &temptime);
   pd.MakeRunWritable();
   pd.Run().StartPauseTime.Set(temptime);
   pd.MakeRunReadable();
   DataLog (log_level_proc_alarm_monitor_info) << "Reset start Pause timer because of PFR :" << pd.Run().StartPauseTime.Get().tv_sec << endmsg;

   //
   // Restore product
   //
   DataLog(log_level_proc_pfr_info) << "Setting current product number " << pd.RunTargets().ProcNumber.Get() << endmsg;
   Message<ModifyProductStruct> modifymsg(MessageBase::SEND_LOCAL);

   ModifyProductStruct          mod;
   mod.product_number = pd.RunTargets().ProcNumber.Get();

   //
   // Force predict to reoptimize for the current product
   //
   mod.action = REOPTIMIZE_PRODUCT;
   modifymsg.send(mod);

   mod.action   = SET_PRODUCT_STATUS;
   mod.aux_data = (int)GOOD_PRODUCT;
   modifymsg.send(mod);

   mod.action = SET_PRODUCT_AS_CURRENT;
   modifymsg.send(mod);
}

void PFRecover::sendModifyPredictMessage ()
{

   DataLog (log_level_proc_alarm_monitor_info) << "sendModifyPredictMessage" << endmsg;
   ProcData pd;
   if (    pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::BLOOD
           && pd.TrimaSet().CassetteState.Get() < ProcTrimaSet_CDS::RINSEBACK
           && pd.PFRHandler().IsPFDataAvailable() )
   {
      handlePredictNotification();
   }
   _modifyMessageTimer.armTimer(TimerMessage::DISARMED);
}

/* FORMAT HASH 5033f8bee231717bc90edddb6cbbe4ed */
