/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.21  2004/02/13 21:46:26Z  ms10234
 * 6865 - Remove over-abundance of logging when rinseback stops
 * Revision 1.20  2003/12/04 20:17:36Z  ms10234
 * 6026 6104 - Add PFR values for ac infusion monitoring during plasma rinseback.
 * Revision 1.19  2003/09/09 22:44:41Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.18  2003/07/21 21:24:59Z  td07711
 * IT6157 - no need to shutdown in ResetCycleVol()
 * Revision 1.17  2003/07/21 17:06:05Z  td07711
 * IT6157 - logging change
 * Revision 1.16  2003/07/01 22:33:44Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.15  2003/04/03 21:47:48Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.14  2002/08/07 17:14:25Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.13  2002/07/23 16:52:22  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.12  2002/04/15 09:55:49  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.11  2002/04/05 16:37:22  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.10  2000/09/06 22:57:10Z  bs04481
 * Comment only on return pump speed monitoring during RB
 * --- Added comments ---  pn02526 [2001/09/07 13:57:18Z]
 * This is per 6.20.2 of Safety SRS.
 * Revision 1.9  2000/03/15 18:00:19  BS04481
 * Get cassette position from cassette object
 * Revision 1.8  1999/08/12 21:08:00  BS04481
 * Eliminate pump speed logging on each 500ms sample
 * Revision 1.7  1999/08/01 20:52:52  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.6  1999/06/04 22:58:09  BS04481
 * Change calculation for delta time using control status header
 * information to only happen if there is valid pump data in the
 * control status message.
 * Don't evaluate return pump speed or AC infusion during rinseback
 * if the delta time between message is less than 20ms (avoid
 * reliability problem if messages stack up)
 * Finally, use control status message delta time when evaluating
 * AC infusion during Rinseback.
 * Revision 1.5  1999/05/21 17:27:16  BS04481
 * Corrected delta time calculation being used for Return Pump
 * Speed Limit.  Also, put in temporary speed logging during
 * Rinseback to help diagnose a hardware problem in T11.
 * Revision 1.4  1999/05/13 15:43:37  BS04481
 * Change calculation of return pump speed to use the timestamp from
 * the control side message while calculating delta time.  Add logging
 * of the speed as it is calculated using delta time from the safety
 * side to attempt to detect messaging problems.
 * Revision 1.3  1998/11/12 18:22:12  bs04481
 * New function to check for pump volume violations during Donor
 * Disconnect.  Formerly part of the update function but needed
 * to be split out to allow pump volume to be checked even if we
 * did not have a valid average AC ratio.
 * Revision 1.2  1998/11/05 19:31:36  bs04481
 * Correct ASSERT usage.  Add assert for Control Status structure
 * pointer NULL.  Reset the accumulated volume at the start of
 * a return instead of a draw.  Code Review change.
 * Revision 1.1  1998/05/12 19:49:29  bs04481
 * Initial revision
 *
 *
 * TITLE:      acinfusr.cpp, the AC infusion rate during Rinseback
 *             and the Disconnect Test monitoring object.
 *
 * ABSTRACT:   Safety's monitoring for AC infusion rate during Rinseback
 *             and Disconnect Test.  Monitors for the following situations:
 *                AC pump running during Rinseback or Donor Disconnect test
 *                Return pump running > 250ml/min during Rinseback
 *                AC Infusion rate during Rinseback exceeds 1.6 ml/min per liter TBV
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_assert.h"
#include "alarms.h"

#include "chw.hpp"

#include "acinfus.hpp"
#include "s_donor.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ACInfusionRinseback Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety AC Infusion rate monitoring during Rinseback constructor
//
// ERROR HANDLING:   none.

ACInfusionRinseback::ACInfusionRinseback(InletContainer* InletCont,
                                         ReturnContainer* ReturnCont,
                                         ACContainer* ACCont,
                                         aDonor*  Donor)
   : _Inlet(InletCont), _Return(ReturnCont), _AC(ACCont), _Donor(Donor), _LogInfoTimer(0, Callback< ACInfusionRinseback >(this, &ACInfusionRinseback::logInfo), TimerMessage::DISARMED)
{
   _faccumACDuringRBAndDisconnect = 0.0f;
   _fadjustedDonorHCT             = 0.0f;
   _avgACRatio                    = 0.0f;
   _RBInfusionRate                = 0.0f;
   _deltaACToDonor                = 0.0f;
   clock_gettime(CLOCK_REALTIME, &_rinsebackTimer);
   clock_gettime(CLOCK_REALTIME, &_returnPumpTimer);
   _valveOpenTimer.tv_sec       = 0; _valveOpenTimer.tv_nsec = 0;
   _valveOpenAccumulatedTime    = 0;
   _deltaControlMessageTime     = 0;
   _iRBHighInfusionCounter      = 0;

   _bACInfusionMonitoringActive = FALSE;
   _bPlasmaValveOpenTooLong     = FALSE;
   _monType                     = MON_INACTIVE;
   _dumpFlag                    = TRUE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_AC);
   trima_assert(_Donor);

   SaveToPFR();    // parasoft-suppress PB-26 "Object is fully constructed"
}

// SPECIFICATION:    Safety AC Infusion rate monitoring during Rinseback destructor
//
// ERROR HANDLING:   none.

ACInfusionRinseback::~ACInfusionRinseback()
{
   _Inlet  = NULL;
   _Return = NULL;
   _AC     = NULL;
   _Donor  = NULL;
}

// SPECIFICATION:    Initialize AC infusion rate monitoring during Rinseback for appropriate state
//
// ERROR HANDLING:   none

void ACInfusionRinseback::InitializeForState (long newState)
{
   switch (newState)
   {
      case BLOOD_RINSEBACK :
         _iRBHighInfusionCounter = 0;
         clock_gettime(CLOCK_REALTIME, &_rinsebackTimer);
         clock_gettime(CLOCK_REALTIME, &_returnPumpTimer);
         _bACInfusionMonitoringActive = TRUE;
         _LogInfoTimer.interval(5000);    /*5 seconds*/
         break;
      case DONOR_DISCONNECT :
         _bACInfusionMonitoringActive = TRUE;
         _LogInfoTimer.interval(0);    /*turn it off*/
         break;
      default :
         _bACInfusionMonitoringActive = FALSE;
         _LogInfoTimer.interval(0);    /*turn it off*/
         break;

   }

   SaveToPFR();
}


// SPECIFICATION:    Checks the accumulated amount of AC pumped during
//                   Rinseback and Donor Disconnect Test
//
// ERROR HANDLING:   Inappropriate AC Pump Movement alarm

void ACInfusionRinseback::IsACRunningDuringRBorDisc (HW_CASSETTE_STATES position)
{

   // if this is not Rinseback or Donor Disconnect Test,
   // or the orders are NULL, just return
   if ( ( (_monType != MON_RINSEBACK )
          &&(_monType != MON_DISCONNECT ) )
        ||(_bACInfusionMonitoringActive == FALSE) )
      return;


   // accumulate the amount of AC over Rinseback and Disconnect
   _faccumACDuringRBAndDisconnect += _AC->GetVolume(VOL_DELTA);

   // alarm if we have accumulated more that 1.0 ml.
   // gate with "cassette down" to allow AC pump to run while raising
   // the cassette
   if ( (position == HW_CASSETTE_DOWN)
        &&(_faccumACDuringRBAndDisconnect > MAX_AC_ALLOWED_DURING_RB_OR_DISC) )
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, INAPPROPRIATE_AC_PUMP_MOVEMENT);
      if (_dumpFlag == TRUE)
      {
         _dumpFlag = FALSE;
         Dump(log_level_safety_alarm_detail);
      }
      DataLog(log_level_safety_alarm_detail) << "AC: " << _faccumACDuringRBAndDisconnect << " ml during : RB || D/C state" << endmsg;
   }
}


// SPECIFICATION:    Checks the speed of the Return pump during Rinseback.
//                   This function is part of the protection against
//                   AC over-infusion during rinseback.  During rinseback,
//                   AC infusion rate is calculated on each 500ms sample. See
//                   the Update method for this object.  However, the
//                   over-infusion alarm will not occur until 10 samples
//                   have violated the maximum allowed rate.  If the return pump
//                   is in runaway, the alarm will not occur soon enough to
//                   protect the donor so this function watches for return
///                  pump too fast during rinseback.
//
// ERROR HANDLING:   Return Pump Speed Alarm during Rinseback

void ACInfusionRinseback::IsReturnTooFastDuringRB (long deltaControlMessageTime)
{
   float deltaReturn, deltaTime;

   // if this is not Rinseback, just return
   if ( ( _monType != MON_RINSEBACK )
        ||(_bACInfusionMonitoringActive == FALSE) )
      return;

   // get Return volume pumped since last sample
   deltaReturn = _Return->GetVolume(VOL_DELTA);

   // get time since last sample
   // leave this in for debug of the hardware.  we won't
   // use this time for calculating speed any longer but
   // I want to see if we are have large discrepencies
   // between the time sent and the time received.
   deltaTime = calc_elapsed_time_running(&_returnPumpTimer);

   // calculate and test return pump speed using control side time
   // deltaMessageTime is in microseconds
   float deltaControlMessageTimeInSeconds = (float)deltaControlMessageTime / 1000000.0f;
   float controlReturnPumpSpeed           = 0.0f;
   if ( deltaControlMessageTimeInSeconds >= 0.020f /*secs*/ )
   {
      controlReturnPumpSpeed = (deltaReturn /*mls*/ / deltaControlMessageTimeInSeconds) * 60.0 /*secs/min*/; /*mls/min*/
      if ( controlReturnPumpSpeed > RETURN_FLOW_RATE_LIMIT_RB)
      {
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, RETURN_PUMP_SPEED_ALARM_RB);
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_safety_alarm_detail);
         }
         DataLog(log_level_safety_alarm_detail) << "Return Pump Speed: "
                                                << controlReturnPumpSpeed
                                                << " deltaReturn: "
                                                << deltaReturn
                                                << " deltaControlMessageTime: "
                                                << deltaControlMessageTimeInSeconds
                                                << endmsg;
      }
   }
   // calculate return pump speed using safety side time
   // log it if it gets up to 200 ml/min.  alarm limit is at 250
   // as calculated using control time.
   if (deltaTime != 0.f)
   {
      float safetyReturnPumpSpeed = (deltaReturn /*mls*/ / deltaTime /*secs*/) * 60.0 /*secs/min*/; /*mls/min*/
      if ( safetyReturnPumpSpeed > (RETURN_FLOW_RATE_LIMIT_RB - 50.0f) )
      {
         DataLog(log_level_safe_exec_info) << "Return Pump Speed - Safety: "
                                           << safetyReturnPumpSpeed
                                           << " deltaReturn: "
                                           << deltaReturn
                                           << " deltaTime: "
                                           << deltaTime
                                           << endmsg;
         DataLog(log_level_safe_exec_info) << "Return Pump Speed - Control: "
                                           << controlReturnPumpSpeed
                                           << " deltaReturn: "
                                           << deltaReturn
                                           << " deltaControlMessageTime: "
                                           << deltaControlMessageTimeInSeconds
                                           << endmsg;
      }
   }
}



float ACInfusionRinseback::CalculateACFlow (float avgACRatio, float flowToDonor)
{
   //
   // Get the adjusted donor hematocrit from the ACInfusion Cycle monitor
   //
   float adjustedDonorHCT = SafetyPFRDataToSave.PFR_fadjustedDonorHCT;

   float bagFraction      = 1.0f;
   if ( avgACRatio >= 1.0f && adjustedDonorHCT <= 1.0f ) // Ensure we keep the bag fraction from going negative
      bagFraction = 1.0f / ( 1.0f + ( (avgACRatio - 1.0f) * (1.0f - adjustedDonorHCT) ) );

   if ( bagFraction > 0.0f && bagFraction < 1.0f ) // Make sure the number is a fraction
      return ( flowToDonor * bagFraction );
   else
      return ( flowToDonor ); // Assume all flow is AC
}


// SPECIFICATION:    Monitors AC Infusion rate during Rinseback
//
// ERROR HANDLING:   AC Infusion High Alarm

void ACInfusionRinseback::Update (float avgACRatio, long deltaControlMessageTime /*usecs*/, HW_VALVE_STATES plasmaValveState)
{
   // Calculate the AC Infusion rate
   // if this is not Rinseback, just return
   if ( ( _monType != MON_RINSEBACK )
        ||(_bACInfusionMonitoringActive == FALSE) )
   {
      SaveToPFR();
      return;
   }

   // if the average AC ratio is not valid, do not use
   if (avgACRatio == 0.0)
   {
      if (_dumpFlag == TRUE)
      {
         _dumpFlag = FALSE;
         Dump(log_level_critical);
      }
      _FATAL_ERROR(__FILE__, __LINE__, "Average AC ratio causes divide by zero");
   }
   else
   {
      _avgACRatio              = avgACRatio;
      _deltaControlMessageTime = deltaControlMessageTime /*usecs*/ / 1000000.0f /*usecs/sec*/;
      unsigned long deltaTime = 0;
      if ( _valveOpenTimer.tv_sec != 0 ) // the valve was opened during the last sample
      {
         struct timespec current;
         clock_gettime(CLOCK_REALTIME, &current);
         deltaTime = current.tv_sec - _valveOpenTimer.tv_sec;
      }

      if (    plasmaValveState != HW_VALVE_COLLECT
              && plasmaValveState != HW_VALVE_RETURN )
      {

         _valveOpenAccumulatedTime += deltaControlMessageTime /*usecs*/;
         clock_gettime(CLOCK_REALTIME, &_valveOpenTimer);   // save the current time for PFR case
      }
      else
      {
         _valveOpenTimer.tv_sec  = 0;
         _valveOpenTimer.tv_nsec = 0;
      }

      if (    _valveOpenAccumulatedTime < MAX_PLASMA_VALVE_OPEN_TIME * 1000000 /*usecs*/
              && deltaTime < MAX_PLASMA_VALVE_OPEN_TIME
              && _bPlasmaValveOpenTooLong == FALSE )
      {
         // get the AC delivered to the donor in this sample
         _deltaACToDonor = (_Return->GetVolume(VOL_DELTA) - _Inlet->GetVolume(VOL_DELTA) )
                           / avgACRatio;
      }
      else
      {
         //
         // Calculate the AC flow assuming all flow is plasma mixed with AC
         //
         _deltaACToDonor          = CalculateACFlow(avgACRatio, (_Return->GetVolume(VOL_DELTA) - _Inlet->GetVolume(VOL_DELTA)) );
         _bPlasmaValveOpenTooLong = TRUE;
      }


      // calculate infusion rate for this sample period and test
      if ( _deltaControlMessageTime /*secs*/ >= 0.020f /*secs*/ )
      {
         _RBInfusionRate /*ml/min/liter*/ = ( _deltaACToDonor /*ml*/ * 60.0f /*secs/min*/ ) /
                                            _deltaControlMessageTime /*secs*/ /
                                            _Donor->GetTBV_liters() /*liters*/;

         if (_RBInfusionRate > MAX_ALLOWABLE_AC_INFUSION_THREE_CYCLE_AVG)
         {
            if (++_iRBHighInfusionCounter > RB_INFUSION_THRESHOLD_COUNTER)
            {
               paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_INFUSION_HIGH_ALARM);
               if (_dumpFlag == TRUE)
               {
                  _dumpFlag = FALSE;
                  Dump(log_level_safety_alarm_detail);
               }
               DataLog(log_level_safety_alarm_detail) << "RB AC Infusion Rate : "
                                                      << _RBInfusionRate
                                                      << " deltaACToDonor: "
                                                      << _deltaACToDonor << "=(" << _Return->GetVolume(VOL_DELTA) << "-" << _Inlet->GetVolume(VOL_DELTA) << ")/" << avgACRatio
                                                      << " deltaTime: "
                                                      << _deltaControlMessageTime
                                                      << " allPlasma: " << _bPlasmaValveOpenTooLong
                                                      << " accumTimeW/ValveOpen: " << _valveOpenAccumulatedTime
                                                      << " lastTimeW/ValveOpen: " << _valveOpenTimer.tv_sec << "." << _valveOpenTimer.tv_nsec
                                                      << endmsg;
            }
            else
            {
               DataLog(log_level_safe_exec_info) << "RB AC Infusion = "
                                                 << _RBInfusionRate
                                                 << " Count: "
                                                 << _iRBHighInfusionCounter
                                                 << " deltaACToDonor: "
                                                 << _deltaACToDonor << "=(" << _Return->GetVolume(VOL_DELTA) << "-" << _Inlet->GetVolume(VOL_DELTA) << ")/" << avgACRatio
                                                 << " deltaTime: "
                                                 << _deltaControlMessageTime
                                                 << " allPlasma: " << _bPlasmaValveOpenTooLong
                                                 << " accumTimeW/ValveOpen: " << _valveOpenAccumulatedTime
                                                 << " lastTimeW/ValveOpen: " << _valveOpenTimer.tv_sec << "." << _valveOpenTimer.tv_nsec
                                                 << endmsg;
            }
         }
         else if ( (_RBInfusionRate < MAX_ALLOWABLE_AC_INFUSION_THREE_CYCLE_AVG)
                   &&(_iRBHighInfusionCounter > 0) )
         {
            _iRBHighInfusionCounter--;
         }

      }
   }

   SaveToPFR();
   return;
}

void ACInfusionRinseback::logInfo ()
{
   if ( _RBInfusionRate > 0.1f ) // Don't log it if we aren't infusing AC ...
   {
      DataLog(log_level_safe_exec_info) << "RB AC Infusion = "
                                        << _RBInfusionRate
                                        << " Count: "
                                        << _iRBHighInfusionCounter
                                        << " deltaACToDonor: "
                                        << _deltaACToDonor << "=(" << _Return->GetVolume(VOL_DELTA) << "-" << _Inlet->GetVolume(VOL_DELTA) << ")/" << _avgACRatio
                                        << " deltaTime: "
                                        << _deltaControlMessageTime
                                        << " allPlasma: " << _bPlasmaValveOpenTooLong
                                        << " accumTimeW/ValveOpen: " << _valveOpenAccumulatedTime
                                        << " lastTimeW/ValveOpen: " << _valveOpenTimer.tv_sec << "." << _valveOpenTimer.tv_nsec
                                        << endmsg;
   }
}

// SPECIFICATION:    Rinseback AC infusion monitoring method to
//                   check for pumps which are running out of
//                   tolerance
//
// ERROR HANDLING:   FATAL ERROR for unknown cycle.

void ACInfusionRinseback::UpdatePumpChecks (HW_CASSETTE_STATES position, long deltaControlMessageTime)
{
   // Check for AC running during Rinseback or Disconnect
   IsACRunningDuringRBorDisc(position);

   // Check for Return pump running too fast during Rinseback
   IsReturnTooFastDuringRB(deltaControlMessageTime);

   SaveToPFR();
   return;

}


// SPECIFICATION:    Rinseback AC infusion monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   FATAL ERROR for unknown cycle.

void ACInfusionRinseback::ResetCycleVol (char cCycle)
{
   if (cCycle == RETURN_CYCLE)
   {
      _faccumACDuringRBAndDisconnect = 0.0f;
   }

   SaveToPFR();
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.

void ACInfusionRinseback::Dump (DataLog_Level& log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ACInfusionRinseback::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_faccumACDuringRBAndDisconnect        = _faccumACDuringRBAndDisconnect;
   SafetyPFRDataToSave.PFR_returnPumpTimer                      = _returnPumpTimer;
   SafetyPFRDataToSave.PFR_rinsebackTimer                       = _rinsebackTimer;
   SafetyPFRDataToSave.PFR_valveOpenTimer                       = _valveOpenTimer;
   SafetyPFRDataToSave.PFR_valveOpenAccumulatedTime             = _valveOpenAccumulatedTime;
   SafetyPFRDataToSave.PFR_iRBHighInfusionCounter               = _iRBHighInfusionCounter;
   SafetyPFRDataToSave.PFR_acInfusR_bACInfusionMonitoringActive = _bACInfusionMonitoringActive;
   SafetyPFRDataToSave.PFR_acInfusR_bPlasmaValveOpenTooLong     = _bPlasmaValveOpenTooLong;
   SafetyPFRDataToSave.PFR_acInfusR_monType                     = _monType;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ACInfusionRinseback::RestoreFromPFR ()
{
   _faccumACDuringRBAndDisconnect = SafetyPFRDataFromRestore.PFR_faccumACDuringRBAndDisconnect;
   _returnPumpTimer               = SafetyPFRDataFromRestore.PFR_returnPumpTimer;
   _rinsebackTimer                = SafetyPFRDataFromRestore.PFR_rinsebackTimer;
   _valveOpenTimer                = SafetyPFRDataFromRestore.PFR_valveOpenTimer;
   _valveOpenAccumulatedTime      = SafetyPFRDataFromRestore.PFR_valveOpenAccumulatedTime;
   _iRBHighInfusionCounter        = SafetyPFRDataFromRestore.PFR_iRBHighInfusionCounter;
   _bACInfusionMonitoringActive   = SafetyPFRDataFromRestore.PFR_acInfusR_bACInfusionMonitoringActive;
   _bPlasmaValveOpenTooLong       = SafetyPFRDataFromRestore.PFR_acInfusR_bPlasmaValveOpenTooLong;
   _monType = SafetyPFRDataFromRestore.PFR_acInfusR_monType;

   DataLog(log_level_safe_exec_info) << "ACInfusionRinseback::RestoreFromPFR(), "
                                     << "faccumACDuringRBAndDisconnect: " << _faccumACDuringRBAndDisconnect
                                     << " returnPumpTimer: " << _returnPumpTimer.tv_sec
                                     << " rinsebackTimer: " << _rinsebackTimer.tv_sec
                                     << " valveOpenTimer: " << _valveOpenTimer.tv_sec
                                     << " valveOpenAccumulatedTime: " << _valveOpenAccumulatedTime
                                     << " iRBHighInfusionCounter: " << _iRBHighInfusionCounter
                                     << " bACInfusionMonitoringActive: " << _bACInfusionMonitoringActive
                                     << " bPlasmaValveOpenTooLong: " << _bPlasmaValveOpenTooLong
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH 2f12ccf703d64e5c5ad91c3f8538320c */
