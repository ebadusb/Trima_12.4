/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      resalr.cpp
 *             Performs a check on the reservoir volumes and alarms where
 *             appropriate.  Provides the switch to return sound.  May
 *             force a cycle switch.
 *
 *
 ******************************************************************************/


#include "resalr.h"
#include "safproc.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "cycleacc.h"
#include "trima_datalog_levels.h"
#include "CatalogChecker.h"
#include "MssCommon.h"

#include <math.h>

const int   MAX_CONSECUTIVE_DRAW_ALARMS     = 3;
const int   MAX_CONSECUTIVE_RETURN_ALARMS   = 2;
const int   MAX_RETURN_ALARMS_IN_ENTIRE_RUN = 6;
const float flaggingDeltaVol                = CobeConfig::data().DrawReturnDeltaForWBCflagging; // see IT9161
const float MAX_DRAW_RET_DIFF               = 3.0;


DEFINE_OBJ(ReservoirAlarm);

ReservoirAlarm::ReservoirAlarm()
   : _MsgMidCycleSwitch(MessageBase::SEND_GLOBAL),
     _AlrReservoirSensor(RESERVOIR_ALARM),
     _AlrDrawTooLongRbcPlOneClamp(RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_ONE_CLAMP),
     _AlrDrawTooLongRbcPlNoClamp(RESERVOIR_DRAW_TOO_LONG_ALARM_RBC_PL_NO_CLAMP),
     _AlrReturnTooLong_A1(RESERVOIR_RETURN_TOO_LONG_ALARM_A1),
     _AlrFirstCycleTooSoon(FIRST_CYCLE_TOO_SOON_ALARM),
     _AlrFirstCycleTooLate(FIRST_CYCLE_TOO_LATE_ALARM),
     _AlrTooManyResDraw(TOO_MANY_RESERVOIR_DRAW_ALARMS),
     _AlrTooManyResReturn(TOO_MANY_RESERVOIR_RETURN_ALARMS),
     _AlrResFullAfterReturn(RESERVOIR_FULL_AFTER_RETURN),

// THREE CHANNEL CLAMP VERSIONS
     _AlrDrawTooShortThreeClamp(RESERVOIR_DRAW_TOO_SHORT_ALARM_THREE_CLAMP),
     _AlrDrawTooLongThreeClamp(RESERVOIR_DRAW_TOO_LONG_ALARM_THREE_CLAMP),
     _AlrReturnTooShortThreeClamp(RESERVOIR_RETURN_TOO_SHORT_ALARM_THREE_CLAMP),
     _AlrReturnTooLongThreeClamp(RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP),

// ONE CHANNEL CLAMP VERSIONS
     _AlrDrawTooShortOneClamp(RESERVOIR_DRAW_TOO_SHORT_ALARM_ONE_CLAMP),
     _AlrDrawTooLongOneClamp(RESERVOIR_DRAW_TOO_LONG_ALARM_ONE_CLAMP),
     _AlrReturnTooShortOneClamp(RESERVOIR_RETURN_TOO_SHORT_ALARM_ONE_CLAMP),
     _AlrReturnTooLongOneClamp(RESERVOIR_RETURN_TOO_LONG_ALARM_ONE_CLAMP),

// NO CHANNEL CLAMP VERSIONS
     _AlrDrawTooShortNoClamp(RESERVOIR_DRAW_TOO_SHORT_ALARM_NO_CLAMP),
     _AlrDrawTooLongNoClamp(RESERVOIR_DRAW_TOO_LONG_ALARM_NO_CLAMP),
     _AlrReturnTooShortNoClamp(RESERVOIR_RETURN_TOO_SHORT_ALARM_NO_CLAMP),
     _AlrReturnTooLongNoClamp(RESERVOIR_RETURN_TOO_LONG_ALARM_NO_CLAMP),

     _BlindCondition(0),
     _InnerLimit(0),
     _CheckingActive(0),
     _StateChange(Callback<ReservoirAlarm>(this, &ReservoirAlarm::systemStateReceived)),
     _CentSlowDown(false),
     _firststCycleTooLate(false),
     _inBolusReturn(false),
     _drawTooShortVolume(0.0f),
     _returnTooShortVolume(0.0f),
     _disableCycleMatchCheck(false),
     _inWbcPurge(false),
     _adjustLimits(false),
     _theCleaningClycle(0.0f),
     _SilentPauseForRTS(SILENT_PAUSE_FOR_RET_TOO_SHORT_CHECK),
     _RTSCompareVolume(0.0f),
     _QueueRTS(false),
     _checkRTSVolm(false),
     _showStatLineMsg(MessageBase::SEND_LOCAL),
     _SilentPauseTimer(0, Callback<ReservoirAlarm>(this, &ReservoirAlarm::clearTimer), TimerMessage::DISARMED)
{}

ReservoirAlarm::~ReservoirAlarm()
{}

void ReservoirAlarm::Initialize ()
{}

void ReservoirAlarm::systemStateReceived ()
{
   ProcData    pd;

   State_names state = (State_names)_StateChange.getData() ;

   int         check = (state == BLOOD_PRIME) ||  (state == BLOOD_RUN);

   if (check != _CheckingActive)
   {
      _CheckingActive = check;
      DataLog(log_level_proc_debug) << "reservoir alarm checking=" << check << " state=" << (int)state << endmsg;
   }

}

void ReservoirAlarm::Monitor ()
{
   ProcData pd;


   if ( pd.Status().ReservoirError() )
   {
      DataLog(log_level_proc_error) << "Reservoir Error" << endmsg;
      _AlrReservoirSensor.setAlarm();
      return;
   }

   if ( !_CheckingActive ) return;

   // If we are in the first draw cycle or
   // if we just switched to the first return cycle,
   // then check the first cycle volumes
   if (    pd.Run().NumReturn.Get() <= 0
           || (    pd.Run().DrawCycle.Get() == false
                   && pd.Run().CycleSwitch.Get() == true
                   && pd.Run().NumDraw.Get() == 1
                   )
           )
   {
      // Debug log, verify that we get the cycle switch to the first return
      if ( pd.Run().CycleSwitch.Get() )
         DataLog(log_level_proc_info) << "Switch to first return" << endmsg;
      DoFirstDrawCheck();
   }
   else
   {


      // After the switch to the first return,
      // check draw and return data, if appropriate.
      _BlindCondition = ( pd.Status().RBCValve.IsReturning() &&
                          pd.Status().CollectValve.IsReturning() &&
                          pd.Status().PlasmaValve.IsReturning()
                          );

      static string SALINE_BOLUS(RecoveryTypes::RecoveryStr(RecoveryTypes::SalineBolus) );
      static string CENT_SLOW(RecoveryTypes::RecoveryStr(RecoveryTypes::CentrifugeSlowScaled) );
      static string WBC_PURGE(RecoveryTypes::RecoveryStr(RecoveryTypes::WBCChamberPurge) );

      if  (    ( pd.RecoveryName() ==  SALINE_BOLUS) || ( pd.RecoveryName() ==  CENT_SLOW) )
      {
         _inBolusReturn = true;
      }

      /////////// CHECK if wbc purge will modify the limits IT-10808/////////////////////////////////////
      if (pd.RecoveryName() ==  WBC_PURGE)
      {
         if (!_inWbcPurge)
         {
            DataLog(log_level_proc_error) << "WBC purge Started: " << endmsg;
            if (!pd.Run().DrawCycle.Get())
            {
               DataLog(log_level_proc_error) << "wbcPurge start adjustLimits (draw =" << pd.Cycle().VresDraw << ")" << endmsg;
               _adjustLimits      = true;
               _theCleaningClycle = pd.Cycle().VresDraw ;
            }
            else
            {

               DataLog(log_level_proc_error) << "wbcPurge stop adjustLimits 1 " << endmsg;
               _adjustLimits = false;
            }

         }

         _inWbcPurge = true;

      }
      else
      {

         if (_inWbcPurge)
            DataLog(log_level_proc_error) << "WBC purge ended: " << endmsg;
         _inWbcPurge        = false;
         _adjustLimits      = false;
         _theCleaningClycle = 0.0f;

      }
      ////////////////////////////////////////////////////////////////////////////////////////////


      DoDrawCheck();
      DoReturnCheck();
   }

}

void ReservoirAlarm::DoFirstDrawCheck ()
{
   ProcData pd;
   float    fVol = pd.Volumes().VinTotal.Get() - pd.Volumes().Vreturn.Get();

   if ( fVol > pd.FirstCycleTooLate() )
   {
      DataLog(log_level_proc_error) << "1st Cycle too late.  Draw volume of "
                                    << fVol << " exceeded limit of " << pd.FirstCycleTooLate() << endmsg;

      _AlrFirstCycleTooLate.setAlarm();
      _MsgMidCycleSwitch.send(SWITCH_TO_RETURN);
      pd.SwitchtoReturn();
      _firststCycleTooLate = true;
      return;
   }

   if ( pd.Run().CycleSwitch.Get() )
   {
      if ( fVol < pd.FirstCycleTooShort() )
      {
         DataLog(log_level_proc_error) << "1st Cycle too short.  Draw volume of "
                                       << fVol << " below limit of " << pd.FirstCycleTooShort() << endmsg;

         _AlrFirstCycleTooSoon.setAlarm();
         return;
      }
   }

}

void ReservoirAlarm::DoReturnCheck ()  // Note: currently Return Too Long checking is never blinded.
{
   ProcData pd;
   // if this is draw cycle data, just exit
   if (    pd.Run().DrawCycle.Get() == true
           && pd.Run().CycleSwitch.Get() == false )
   {
      // turn off bolus margin
      _inBolusReturn = false;
      return;
   }

   // fill volume is -ve during return
   float fV = pd.Cycle().VresReturn;

//    if (_inWbcPurge)
//    {
//        DataLog(log_level_proc_info) << "FV(ret) =  " << fV << endmsg;
//    }
//

   float shortLimit = pd.ReturnTooShort() ; // too short limit: -45
   float maxvol     = pd.ReturnTooLong();   // too long limit: note this value is negative -65


   if (_inBolusReturn)
   {
      maxvol = pd.ReturnTooLong() - 16.0f;
      // DataLog(log_level_proc_info) << "maxVol for too long increased for Saline Bolus/Prime return to " << maxvol << endmsg;
   }



   ///////////////////////////////////////////////////////////////////////////////////////////////////
   if ( _inWbcPurge && _adjustLimits )
   {

      float resVolm = CobeConfig::data().ReservoirVolume;
      if (resVolm == 0.0f) resVolm = 55.0f; // never too smart for divide by zero....

      // wbc offset. this makes the limits come out correct in a short draw induced by the wbc purgwe cleaning (numerical analysis)
      const float offset     = 3.0f;

      float       ratioLong  = fabs(pd.ReturnTooLong() / resVolm);
      float       ratioShort = fabs( (pd.ReturnTooShort() + offset) / resVolm); // add the offset to make the short = 38 at a full res.

      maxvol     = (-1.0f) * ((ratioLong  * _theCleaningClycle) + offset);
      shortLimit = (-1.0f) * ((ratioShort * _theCleaningClycle) - offset);

      if (maxvol < pd.ReturnTooLong()) maxvol = pd.ReturnTooLong();
      if (shortLimit > 0.0f) shortLimit = 0.0f;


      DataLog(log_level_proc_info) << "maxVol for too long increased for Purge return to " << maxvol
                                   << ",  Data:  VresDraw(fV) = " << _theCleaningClycle
                                   << ",  original limit ReturnTooLong = " << pd.ReturnTooLong()
                                   << endmsg;


      DataLog(log_level_proc_info) << "shortLimit for too short for Purge return  " << shortLimit
                                   << ",  Data:  VresDraw(fV) = " << _theCleaningClycle
                                   << ",  original limit ReturnTooShort = " << pd.ReturnTooShort()
                                   << endmsg;

   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////

   if ( pd.Run().NumReturn.Get() <= 1 ) maxvol -= CobeConfig::data().ReturnFirstCycleMargin;

   // IT5946 keep track of cent slow down for our A1 that we're adding below.
   // Cent Slow down is defined as:
   //      centrifuge RPM is < 2000 anytime after
   //      (Vin + abs(VreturnLinePrime)) > (first_cycle_too_soon_limit - 15) AND prior to end of 1st return.
   // note that we use abs() because VreturnLinePrime is negative during first draw and
   // the - 15 is the potential volume expressed during centrifuge slowdown.
   if (pd.Run().NumReturn.Get() <= 1) // prior to end of 1st return
   {
      if ((pd.Volumes().Vin.Get() + fabs(pd.Volumes().Vreturn.Get())) > (pd.FirstCycleTooShort() - 15))
      {
         if (pd.Status()._centRPM < 2000)
         {
            // no need to output this message more than once
            if (!_CentSlowDown)
               DataLog(log_level_proc_info) << "Prior to end of 1st return it appears we have had a cent slow down." << endmsg;
            _CentSlowDown = true;
         }
      }
   }

   //////////////////// RETURN TOO LONG /////////////////////////////////////////////////////
   if ( fV < maxvol )
   {
      _InnerLimit = 1;
      _MsgMidCycleSwitch.send(SWITCH_TO_DRAW);
      pd.SwitchtoDraw();
      pd.MakeRunWritable();  // Increment BOTH ReturnTooLong counters
      pd.Run().NumRetTooLong       = pd.Run().NumRetTooLong + 1;
      pd.Run().NumConsecRetTooLong = pd.Run().NumConsecRetTooLong + 1;
      {
         DataLog(log_level_proc_error) << "Return Cycle too long: Vol=" << fV
                                       << "ml Consecutive=" << pd.Run().NumConsecRetTooLong
                                       << " TOTAL=" << pd.Run().NumRetTooLong
                                       << " cycle=" << pd.Run().NumReturn.Get()
                                       << " maxvol=" << pd.ReturnTooLong()
                                       << " margin=" << CobeConfig::data().ReturnFirstCycleMargin
                                       << endmsg;
      }
      if ( pd.Status().ReservoirIsFull() ) // If the reservoir is still full.
                                           // If it has ran that long, but still hasn't got the upper
                                           // level sensor uncovered, then either the line is clogged or
                                           // there is a bunch of fluid that could be AC.
      {
         DataLog (log_level_proc_error) << "Reservoir full after return cycle." << endmsg;
         _AlrResFullAfterReturn.setAlarm();
      }
      else if ( ( pd.Run().NumRetTooLong >= MAX_RETURN_ALARMS_IN_ENTIRE_RUN ) // Check run total.
                || ( pd.Run().NumConsecRetTooLong >=
                     MAX_CONSECUTIVE_RETURN_ALARMS ) ) // Check consecutive total.
      {
         DataLog (log_level_proc_error) << "Too many Reservoir Return Alarms." << endmsg;
         _AlrTooManyResReturn.setAlarm();
      }
      else
      {
         // is this an R1 or our new A1 (IT5946)
         // the criteria for our new A1 is
         //   if this is the 1st return AND
         //   cent slow down did not occur AND
         //   long 1st draw did not occur
         if (pd.Run().NumReturn.Get() <= 1 && !_CentSlowDown && !_firststCycleTooLate)
         {
            DataLog (log_level_proc_error) << "No cent slow down and no 1st draw too long; setting A1" << endmsg;
            _AlrReturnTooLong_A1.setAlarm();
         }
         else
         {
            DataLog (log_level_proc_error) << "Return too long alarm; setting R1" << endmsg;
            setResAlr(RETURN, TOO_LONG);
         }
      }
   }

   //////////////////// RETURN TOO SHORT /////////////////////////////////////////////////////
   // if just switched from return to draw
   if (    pd.Run().CycleSwitch.Get()
           && pd.Run().DrawCycle.Get() )
   {

      if (_adjustLimits)
      {
         DataLog(log_level_proc_info) << "doReturn check one shot as drw starts " << endmsg;
      }

      if (_inBolusReturn)
      {
         shortLimit = pd.ReturnTooShort() + 38.0;
         DataLog(log_level_proc_info) << "shortLimit for too short changed for Recovery return to " << shortLimit << endmsg;
      }


      _returnTooShortVolume = fV;
      if (_drawTooShortVolume > 0.0f && !_disableCycleMatchCheck)
      {
         if (fabs(fabs(_returnTooShortVolume) - fabs(_drawTooShortVolume)) < flaggingDeltaVol)
         {

            DataLog(log_level_proc_info) << "Return Cycle And Draw Cycle short volumes match within margin: deltaVol="
                                         << fabs(fabs(_returnTooShortVolume) - fabs(_drawTooShortVolume)  )
                                         << ", no WBC/Volume-Yield flag will apply"
                                         << endmsg;

            // turn the WBC for PLTs
            pd.MakeRunWritable();
            pd.Run().BalanceCyclesDisableWBCPLTFlag = true;
            _drawTooShortVolume = 0.0f;  // reset
         }
         else
         {

            // dont repeatedly cause alarm and flag... once is good.
            if (!pd.Run().FlagforShortDrawNow.Get())
            {


               DataLog(log_level_proc_info) << "Return Cycle And Draw Cycle short volumes beyond margin: deltaVol="
                                            << fabs(fabs(_returnTooShortVolume) - fabs(_drawTooShortVolume)  )
                                            << ",WBC/Volume-Yield flagged; no further cycle matching will happen"
                                            << endmsg;

               // Request the platelet contamination alert
               pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

               pd.MakeRunWritable();
               pd.Run().BalanceCyclesDisableWBCPLTFlag.Set(false);
               pd.Run().FlagforShortDrawNow.Set(true);
               _drawTooShortVolume     = 0.0f; // reset
               _disableCycleMatchCheck = true;
               pd.MakeRunReadable();


            }
         }
      }


      // IT 11825 ////////////////////////////////////////
      if (_QueueRTS == true)
      {
         _QueueRTS     = false;
         _checkRTSVolm = false;
         DataLog(log_level_proc_info) << "  Queued Return Too Short Fired" << endmsg;
         setResAlr(RETURN, TOO_SHORT);
         _showStatLineMsg.send(1);
      }
      // IT 11825 ////////////////////////////////////////
      //

      if ( fV > shortLimit )
      {
         DataLog(log_level_proc_info) << "Return Cycle too short: Vol=" << fV << " is GT the limit = " << shortLimit << endmsg;

         // IT 11825 ////////////////////////////////////////

         if (pd.TrimaSet().Cassette == cassette::RBCPLS)
         {
            DataLog(log_level_proc_info) << " RTS alarm supressed for compare to draw" << endmsg;

            // silent alarm to pause us
            _SilentPauseForRTS.setAlarm();
            _showStatLineMsg.send(1);

            // set timer to CB and cancel silent alarm
            _SilentPauseTimer.interval(20000.0f);

            //  save fv for compare
            _RTSCompareVolume = fV;

            _checkRTSVolm     = true;

         }
         else
         {
            setResAlr(RETURN, TOO_SHORT);
         }

         //////////////////////////////////////////////////////

      }

      // Reset consecutive alarm counter if no
      // Return Too Long occurred during this cycle
      if ( !_InnerLimit )
      {
         pd.MakeRunWritable();
         pd.Run().NumConsecRetTooLong = 0; // Clear consecutive total.
      }

      // Unconditionally clear the Inner Limit flag
      // for the draw cycle that is just starting.
      _InnerLimit = 0;


      if (_adjustLimits)
      {
         DataLog(log_level_proc_error) << "wbcPurge stop adjustLimits 2" << endmsg;
         _adjustLimits = false;
      }
   }
}

void ReservoirAlarm::DoDrawCheck ()
{
   ProcData pd;

   // if this is return cycle data, just exit
   if (    pd.Run().DrawCycle.Get() == false
           && pd.Run().CycleSwitch.Get() == false )
      return;

   // fill volume is +ve during draw
   float fV = pd.Cycle().VresDraw;


//    if (_inWbcPurge)
//       DataLog(log_level_proc_info) << "FV(draw) =  " << fV << endmsg;

   bool RBCS_COLLECTING = false;

   if (pd.Status().RBCValve.IsCollecting())
   {
      RBCS_COLLECTING = true;
   }
   else
   {
      RBCS_COLLECTING = false;
   }


   if (
      !_InnerLimit &&      // Don't check for Draw Too Long more than once per Draw cycle.
      !_BlindCondition &&  // Don't check for Draw Too Long if blinded.
      ( fV > pd.DrawTooLong() )
      )
   {
      pd.MakeRunWritable();

      if ( (RBCS_COLLECTING) &&
           ( pd.Status().ACPump.ActFlow() > 0.0f
             || pd.Status().InletPump.ActFlow() > 0.0f
             || pd.Status().PlasmaPump.ActFlow() > 0.0f
             || pd.Status().CollectPump.ActFlow() > 0.0f ) )
      {
         // If we're working on product two, that's the one that gets the flag.
         if (pd.Run().drbcSplitDone.Get())
         {
            DataLog (log_level_proc_error) << "Setting DrawTooLongInRBCColProd2 to true." << endmsg;
            // Set product flag for Product Verify operator messaging later in the run.
            pd.Run().DrawTooLongInRBCColProd2.Set(true);
         }
         else
         {
            DataLog (log_level_proc_error) << "Setting DrawTooLongInRBCCol to true." << endmsg;
            // Set product flag for Product Verify operator messaging later in the run.
            pd.Run().DrawTooLongInRBCCol.Set(true);
         }
      }
      _InnerLimit = 1;

      //
      //
      //  The most likely reason for draw too long on an rbc/plasma set is failure to connect
      //  replacement fluid.  If we are in that scenario ask for a recovery (IT9854)
      //
      bool setIsBlackStamp = ((CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP)) == 0);
      if (setIsBlackStamp && (pd.Volumes().VReplacement.Get() > 0.0f))
      {
         DataLog(log_level_proc_info) << "Requesting saline draw recovery" << endmsg;
         SalineDrawRecoveryMsg recover(MessageBase::SEND_LOCAL);
         recover.send(1);
      }

      pd.Run().NumConsecDrawTooLong = pd.Run().NumConsecDrawTooLong + 1;
      {
         DataLog(log_level_proc_error) << "Draw Cycle too long: Vol=" << fV << "ml Consecutive=" << pd.Run().NumConsecDrawTooLong <<
            " Limit= " << pd.DrawTooLong() << endmsg;
      }
      if ( pd.Run().NumConsecDrawTooLong >= MAX_CONSECUTIVE_DRAW_ALARMS )
      {
         _AlrTooManyResDraw.setAlarm();
      }
      else
      {
         setResAlr(DRAW, TOO_LONG);
      }
   }

   // terminate run if beyond outer limit; always check this.
   if (
      fV > pd.DrawHighLimit()
      )
   {
      _AlrTooManyResDraw.setAlarm();
   }

   // if just switched to return
   if (
      pd.Run().CycleSwitch.Get() &&
      !pd.Run().DrawCycle.Get()
      )
   {

      float shortLimit = pd.DrawTooShort();

      // IT 11825 /////////////////////////////////////////
      // compare volumes here
      if (_checkRTSVolm)
      {
         float DR_Diff = fabs(fabs(_RTSCompareVolume) - fabs(fV) );

         if ( DR_Diff > MAX_DRAW_RET_DIFF   )
         {
            DataLog(log_level_proc_info) << " RTS Queued. Diff GT " << MAX_DRAW_RET_DIFF << " ;volume compares abs val Draw vol=" << fabs(fV)
                                         << "  Last Return too Short volm=" << fabs(_RTSCompareVolume)
                                         << "  Diff = " << DR_Diff
                                         << endmsg;
            _QueueRTS = true;

         }
         else
         {
            DataLog(log_level_proc_info) << " RTS canceled. Diff LT " << MAX_DRAW_RET_DIFF << " ;volume compares abs val Draw vol="
                                         << fabs(fV) << "  Last Return too Short volm="
                                         << fabs(_RTSCompareVolume)
                                         << "  Diff = " << DR_Diff
                                         << endmsg;

            _QueueRTS = false;
         }
         _RTSCompareVolume = 0.0f;
         _checkRTSVolm     = false;
      }
      // if more than 3 ml queue alarm Return too short.

      //////////////////////////////////////////////////////////

      if ( _inWbcPurge && _adjustLimits )
      {
         DataLog(log_level_proc_info) << "Draw Cycle forced by WBC purge, cannot trip too short alarm. Draw vol=" << fV << endmsg;
      }
      else
      {
         _drawTooShortVolume = 0.0f;   // 9161

         pd.MakeRunWritable();

         if ( !_BlindCondition && ( fV < shortLimit )   )
         {
            {
               DataLog(log_level_proc_error) << "Draw Cycle too short: Vol=" << fV << endmsg;
               DataLog(log_level_proc_error) << "Cent. speed at alarm=" <<  pd.Status().CentrifugeRPM() << " rpm" << endmsg;

               _drawTooShortVolume = fV;      // 9161 save the volume for the compare

            }
            if ( pd.Status().PlasmaValve.IsCollecting() && ( pd.Status().PlasmaPump.ActFlow() > 0.0f ) )
            {
               // Set product flag for Product Verify operator messaging later in the run.
               pd.Run().DrawTooShortInPlsCol = true;
            }
            if ( pd.Status().CassIsPltPlsRBC() && pd.Status().CollectValve.IsCollecting() && ( pd.Status().CollectPump.ActFlow() > 0.0f ) )
            {

               pd.MakeRunWritable();    // 9161
               // Set product flag for Product Verify operator messaging later in the run.
               pd.Run().DrawTooShortInPltCol           = true;  // 9161
               pd.Run().BalanceCyclesDisableWBCPLTFlag = false; // another short so we need to re-evalate this statement if the return is short.

            }
            setResAlr(DRAW, TOO_SHORT);
         }
      }

      // reset consecutive alarm counter if no
      // Draw Too Long occurred during this cycle
      if ( !_InnerLimit )
      {
         pd.MakeRunWritable();
         pd.Run().NumConsecDrawTooLong = 0;
      }

      // Unconditionally clear the Inner Limit flag
      // for the return cycle that is just starting.
      _InnerLimit = 0;
   }

   // IT5946 keep track of cent slow down for our A1 that we're adding below.
   // Cent Slow down is defined as:
   //      centrifuge RPM is < 2000 anytime after
   //      (Vin + abs(VreturnLinePrime)) > (first_cycle_too_soon_limit - 15) AND prior to end of 1st return.
   // note that we use abs() because VreturnLinePrime is negative during first draw and
   // the - 15 is the potential volume expressed during centrifuge slowdown.
   if (pd.Run().NumReturn.Get() <= 1) // prior to end of 1st return
   {
      if ((pd.Volumes().Vin.Get() + fabs(pd.Volumes().Vreturn.Get())) > (pd.FirstCycleTooShort() - 15))
      {
         if (pd.Status()._centRPM < 2000)
         {
            // no need to output this message more than once
            if (!_CentSlowDown)
               DataLog(log_level_proc_info) << "Prior to end of 1st return it appears we have had a cent slow down." << endmsg;
            _CentSlowDown = true;
         }
      }
   }
}


void ReservoirAlarm::setResAlr (DRAW_OR_RETURN drawOrReturn, LONG_OR_SHORT longOrShort)
{
   anAlarmMsg* alarmToSet = NULL;

   ProcData    pd;
   bool        setIsRas        = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC);
   bool        setIsPas        = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_PLT);
   bool        setIsBlackStamp = ((CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::STAMP)) == 0);

   if (setIsPas)
   {
      if (drawOrReturn == DRAW)
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrDrawTooLongThreeClamp;
         else alarmToSet = &_AlrDrawTooShortThreeClamp;
      }
      else
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrReturnTooLongThreeClamp;
         else alarmToSet = &_AlrReturnTooShortThreeClamp;
      }
   }
   else if (setIsRas)
   {
      if (drawOrReturn == DRAW)
      {
         if (longOrShort == TOO_LONG)
         {
            if (setIsBlackStamp) alarmToSet = &_AlrDrawTooLongRbcPlOneClamp;
            else alarmToSet = &_AlrDrawTooLongOneClamp;
         }
         else alarmToSet = &_AlrDrawTooShortOneClamp;
      }
      else
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrReturnTooLongOneClamp;
         else alarmToSet = &_AlrReturnTooShortOneClamp;
      }
   }
   else
   {
      if (drawOrReturn == DRAW)
      {
         if (longOrShort == TOO_LONG)
         {
            if (setIsBlackStamp) alarmToSet = &_AlrDrawTooLongRbcPlNoClamp;
            else alarmToSet = &_AlrDrawTooLongNoClamp;
         }
         else alarmToSet = &_AlrDrawTooShortNoClamp;
      }
      else
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrReturnTooLongNoClamp;
         else alarmToSet = &_AlrReturnTooShortNoClamp;
      }
   }


   // Make sure we're not escaping with a null pointer for our alarm.
   if (!alarmToSet)
   {
      DataLog (log_level_proc_error) << "Invalid alarm choice. Something is wrong. Setting reservoir alarm for fully-enabled AAS." << endmsg;
      if (drawOrReturn == DRAW)
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrDrawTooLongThreeClamp;
         else alarmToSet = &_AlrDrawTooShortThreeClamp;
      }
      else
      {
         if (longOrShort == TOO_LONG) alarmToSet = &_AlrReturnTooLongThreeClamp;
         else alarmToSet = &_AlrReturnTooShortThreeClamp;
      }
   }

   alarmToSet->setAlarm();
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ReservoirAlarm::clearTimer ()
{

   ProcData pd;
   _SilentPauseTimer.interval(0);   // disable timer
   _SilentPauseForRTS.clearAlarm();
   _showStatLineMsg.send(2);
   DataLog(log_level_proc_info) << " Clearing pause and silent alarm for RTS compare and reset timer" << endmsg;

}

/* FORMAT HASH f33f65ec1223baf4271386a08331076d */
