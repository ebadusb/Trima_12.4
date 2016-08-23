/*******************************************************************************
 *
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_res.cpp
 *             Safety reservoir class implementations.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <math.h>

#include "trima_datalog.h"
#include "s_res.hpp"
#include "s_pfr.hpp"
#include "state_objects.hpp"
#include "s_exec.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;


/// logging text
char* cyclemon_text_table[] =
{
   "SN_MON_INACTIVE",
   "SN_MON_FIRST_DRAW",
   "SN_MON_RUN_CYCLES",
   "DRAW_CYCLE_LOCKOUT",
   "RETURN_CYCLE_LOCKOUT",
   "RINSEBACK_MON",
   "CENT_OFF_BLACKOUT",
   "CENT_OFF_BLACKOUT_COUNTING",
   "SN_MON_FIRST_DRAW_COMPLETE",
   "CENT_OFF_BLACKOUT_COMPLETE",
   "DISCONNECT_MON",
};

/*******************************************************************************
*
*  aReservoir_Safety CLASS METHOD DEFINITION
*
******************************************************************************/
// SPECIFICATION:    update reservoir monitoring
//                   called on every normal or reservoir event
//
// ERROR HANDLING:   many alarms.

void aReservoir_Safety::update_res_mon (void)
{
   float avgACRatio;
   bool  flag;

   // Update containers with the new data
   _Inlet->UpdateContainer(_cSNCycle);
   _Return->UpdateContainer(_cSNCycle);
   _AC->UpdateContainer(_cSNCycle);
   _Plasma->UpdateContainer(_cSNCycle);
   _Platelet->UpdateContainer(_cSNCycle);
   _RBC->UpdateContainer(_cSNCycle);


   // during pause or stop , we need to ensure that the blood in the
   // needle does not coagulate.
   // if pumps are stopped, check for AC in the needle and start a
   // time if necessary
   if ( (arePumpsStopped() == TRUE)
        &&(_needle->isTimerRunning() == FALSE) )
   {
      if (shouldWeWorry() == TRUE)
         _needle->turnOnTimer(PUMP_MAX_DURATION);
   }

   // evaluate the volumes pumped
   if (_needle->isEnoughPumped() == TRUE)
   {
      // if this is a draw cycle, snap shot the inlet volume.
      // if we did an AKO, we will need this volume if they stop
      // again in this draw cycle.
      if (_cSNCycle == DRAW_CYCLE)
         _pumpTimerInletVolumeSnap = _Inlet->GetVolume(VOL_DRAW_CYCLE);
   }

   // don't allow a wet set early in the procedure
   checkForWetSet();

   ControlCommand_Export& commands = SafetyExec::instance()->data().orders;

   // Test for AC ratio alarms related to pumps running at mismatched
   // speeds or at unexpected times
   _ACRatioPumpVolumeMonitor->Update(_cSNCycle, commands.pumpRpm[AC_PUMP] /*_AC->_ACPump->GetCommandRPM()*/);

   // Test for AC ratio out of acceptable ranges for a sustained period
   _ACRatioInstantaneousMonitor->Update(_cSNCycle);

   // Update AC infusion rate accumulators
   // (We will calculate and test at the switch to return)
   _ACInfusionMonitor->UpdateAccumulators(_cSNCycle);

   // Update the pump checks needed during rinseback and disconnect
   // (The object will protect for proper state)
   _ACInfusionRinsebackMonitor->UpdatePumpChecks(_paCassette->GetCassettePosition(), _ldeltaControlStatusTime);

   // see if we have a valid average AC ratio
   avgACRatio = _ACRatioAverageMonitor->GetAverageACRatio(flag);
   if (flag == TRUE)
   {
      // Test AC infusion rate during Rinseback or Disconnect
      // (The object will protect for the proper state)
      _ACInfusionRinsebackMonitor->Update(avgACRatio, _ldeltaControlStatusTime, _Plasma->_bag->_myValve.GetSafetyValve() );

      // Test AC infusion rate during Access Keep Open
      // (The object will protect if we are not in AKO)
      _ACInfusionAKOMonitor->Update(_cSNCycle, avgACRatio, _Return->_fReturnVolumeAwayFromDonor);
   }

   // Test hypovolemia cycle size limits
   _HypovolemiaCycleSizeMonitor->Update(_cSNCycle);

   // Test donor hypovolemia limit
   _HypovolemiaDonorMonitor->Update(_cSNCycle);

   // Test for component depletion limits
   _ComponentDepletionMonitor->Update(_cSNCycle);

   updateToPFR();

} // END void aReservoir_Safety :: update_res_mon( void )


// SPECIFICATION:    collect necessary information from the control status message
//
// ERROR HANDLING:   none.

void aReservoir_Safety::UpdateControl (const CHwStates& controlStatus)
{
   _resLevel = controlStatus.reservoir;

   // Update all physical devices with new data
   _Inlet->UpdateContainerControl(controlStatus.inletPump, controlStatus.timestamp, controlStatus.accessPressure);
   _Return->UpdateContainerControl(controlStatus.returnPump, controlStatus.timestamp);
   _AC->UpdateContainerControl(controlStatus.acPump, controlStatus.timestamp);
   _Plasma->UpdateContainerControl(controlStatus.plasmaPump, controlStatus.timestamp, controlStatus.plasmaValve);
   _Platelet->UpdateContainerControl(controlStatus.plateletPump, controlStatus.timestamp, controlStatus.plateletValve);
   _RBC->UpdateContainerControl(controlStatus.rbcValve, controlStatus.plasmaValve, controlStatus.plateletValve);

}

// SPECIFICATION:    collect hall information from the safety status message
//
// ERROR HANDLING:   none.

void aReservoir_Safety::UpdateSafety (SHwStates& safetyStatus)
{
   // Update all physical devices with new data
   // and compare halls vs encoders
   _Inlet->UpdateContainerSafety(safetyStatus.inletAccumRevs);
   _Return->UpdateContainerSafety(safetyStatus.returnAccumRevs);
   _AC->UpdateContainerSafety(safetyStatus.acAccumRevs);
   _Plasma->UpdateContainerSafety(safetyStatus.plasmaAccumRevs, safetyStatus.plasmaValve);
   _Platelet->UpdateContainerSafety(safetyStatus.plateletAccumRevs, safetyStatus.plateletValve);
   _RBC->UpdateContainerSafety(safetyStatus.rbcValve);

}

// SPECIFICATION:    Just a big save of reservoir data
//                   to the power fail data area
//                   Called at the end of every reservoir
//                   monitoring function (500ms or less)
//
//
// ERROR HANDLING:   N/A
// RETURN:           N/A
//
void aReservoir_Safety::updateToPFR ()
{
   SafetyPFRDataToSave.PFR_bLookingForFirstFluid    =  _bLookingForFirstFluid;
   SafetyPFRDataToSave.PFR_bPauseMonitoringActive   = _bPauseMonitoringActive;
   SafetyPFRDataToSave.PFR_cCycleMon                =  _cCycleMon;
   SafetyPFRDataToSave.PFR_cSNCycle                 =  _cSNCycle;
   SafetyPFRDataToSave.PFR_pumpTimerInletVolumeSnap = _pumpTimerInletVolumeSnap;
   SafetyPFRDataToSave.PFR_inMeteredStorage         = _inMeteredStorage;

}

// SPECIFICATION:    Just a big restore of reservoir data
//                   from the power fail data area to the
//                   reservoir object
//                   Called once if we are going to continue
//                   a recoverable procedure
//
// ERROR HANDLING:   N/A
// RETURN:           N/A
//
void aReservoir_Safety::restoreFromPFR ()
{
   _bLookingForFirstFluid    = SafetyPFRDataFromRestore.PFR_bLookingForFirstFluid;
   _bPauseMonitoringActive   = SafetyPFRDataFromRestore.PFR_bPauseMonitoringActive;
   _cCycleMon                = SafetyPFRDataFromRestore.PFR_cCycleMon;
   _cSNCycle                 = SafetyPFRDataFromRestore.PFR_cSNCycle;
   _pumpTimerInletVolumeSnap = SafetyPFRDataFromRestore.PFR_pumpTimerInletVolumeSnap;
   _inMeteredStorage         =   SafetyPFRDataFromRestore.PFR_inMeteredStorage ;


   updateToPFR();

   // Restore data to container objects
   _Inlet->RestoreFromPFR();
   _Return->RestoreFromPFR();
   _AC->RestoreFromPFR();
   _Plasma->RestoreFromPFR();
   _Platelet->RestoreFromPFR();
   _RBC->RestoreFromPFR();

   // Restore the AC ratio monitoring objects
   _ACRatioAverageMonitor->RestoreFromPFR();
   _ACRatioPumpVolumeMonitor->RestoreFromPFR();
   _ACRatioInstantaneousMonitor->RestoreFromPFR();

   // Restore the AC infusion rate monitoring objects
   _ACInfusionMonitor->RestoreFromPFR();
   _ACInfusionRinsebackMonitor->RestoreFromPFR();
   _ACInfusionAKOMonitor->RestoreFromPFR();

   // Restore the Hypovolemia monitoring objects
   _HypovolemiaCycleSizeMonitor->RestoreFromPFR();
   _HypovolemiaDonorMonitor->RestoreFromPFR();
   _ComponentDepletionMonitor->RestoreFromPFR();
}

// SPECIFICATION:    A specialized restore of PFR Data
//                   Called if we are not recovering a
//                   procedure but would like to continue
//                   hypovolemia monitoring during
//                   Donor Disconnect.
//
// ERROR HANDLING:   N/A
// RETURN:           N/A
//
void aReservoir_Safety::restoreReservoirMonitoringForDisconnect ()
{
   // we need to do hypovolemia monitoring, so restore the hypovolemia
   // monitor if the data is there.  If we had a safety hypovolemia alarm
   // when we went down, it will immediately reassert at this point.
   _HypovolemiaDonorMonitor->RestoreFromPFR();
   DataLog(log_level_safe_exec_info) << "Restoring hypovolemia monitoring.  Current EC Volume: "
                                     << _HypovolemiaDonorMonitor->GetPrivateFloatData(HYPPD_EC)
                                     << endmsg;

   // we need to do component depletion monitoring, so restore the monitor
   // if the data is there.  If we had a safety component depletion alarm
   // when we went down, it will immediately reassert at this point.
   _ComponentDepletionMonitor->RestoreFromPFR();
   DataLog(log_level_safe_exec_info) << "Restoring component depletion monitoring.  Current WBPinRBC: "
                                     << _ComponentDepletionMonitor->GetPrivateFloatData(HYPPD_WBRBC)
                                     << ",  WPBNotRBC: "
                                     << _ComponentDepletionMonitor->GetPrivateFloatData(HYPPD_WBNOTRBC)
                                     << endmsg;
}


// SPECIFICATION:    switch SN cycle from draw to return
//
// ERROR HANDLING:
//
// NOTES :
// 07/28/97 msm
// Modifications for Small Cycle Ratio Robustness.
//
void aReservoir_Safety::switch_to_return_cycle (void)
{
   // set by methods that determine R
   // FALSE is bad, TRUE returning functions will have set the
   // value of _favgACRatio to correct value for continuation.
   //
   float avgACRatio;
   bool  flag;

   if (_inMeteredStorage)
   {
      return;
   }

   DataLog(log_level_safe_exec_info) << RETURN_CYCLE << " --------------- Return Cycle ---------------"  << endmsg;

   // 09/07/96 modify to provide U/S robustness
   // pursuant to xfunc ruling 09/06/96
   // do not switch cycle again if cycle already return.
   //
   if ( _cSNCycle != RETURN_CYCLE )
   {
      // switch cycle to return
      // ratio monitoring is activated at DONOR_CONNECTED transition,
      // deactivated for BLOOD_RINSEBACK, DONOR_DISCONNECT ( and consequently post run ) .
      //
      // 07/29/97 msm
      // At RB transition, cycle is forced to a return if in a draw independant of
      // uppper level sensor - RB is in fact a 'return' cycle,
      // Not improbable that rb transition could be on a short cycle, therefore,
      // want to use ratio robustness on rb ratio - this is a change from previous implementation.
      //
      // The first TRUE parameter causes the AC Ratio monitor to alarm
      // if the AC ratio is found to be below 5. The second TRUE parameter allows
      // allows the inlet volume accumulator to alarm if sufficient volume has
      // been pumped with the ratio above 15.
      _ACRatioAverageMonitor->UpdateAverageRatio(TRUE, TRUE);
      avgACRatio = _ACRatioAverageMonitor->GetAverageACRatio(flag);

      if (flag == TRUE)
      {
         _ACInfusionMonitor->UpdateInfusionRate(avgACRatio);
      }
      else
      {
         // you're done - an alarm has been generated
         // inside the UpdateAverageRatio call
         // just logging...
         DataLog(log_level_safety_alarm_detail) << "Un-useable average AC ratio: " << avgACRatio << endmsg;
      }

      if (_cCycleMon == MON_FIRST_DRAW)
      {
         _HypovolemiaCycleSizeMonitor->TestFillTooSmallFirstCycle();
      }

      // tracelog entries
      LogDrawCycle();
      // zero return cycle data
      ResetReturnCycleData();


   } // if _cSNCycle != RETURN_CYCLE
   else
   {
      // log occurance of switch_to_return call even if already in return...
      DataLog(log_level_safe_exec_info) << "called switch_to_return_cycle - already in return" << endmsg;
   }

} // END void aReservoir_Safety :: switch_to_return_cycle( void )


// SPECIFICATION:    switch SN cycle from return to draw
//
// ERROR HANDLING:   none.

void aReservoir_Safety::switch_to_draw_cycle (void)
{


   if (_inMeteredStorage)
   {
      return;
   }

   DataLog(log_level_safe_exec_info) << DRAW_CYCLE << " --------------- Draw Cycle ---------------" << endmsg;

   if ( _cSNCycle != DRAW_CYCLE )
   {
      // switch cycle to draw
      LogReturnCycle();
      ResetDrawCycleData();

   } // if _cSNCycle != DRAW_CYCLE
   else
   {
      // log occurance of switch_to_draw call even if already in return...
      DataLog(log_level_safe_exec_info) << "called switch_to_draw_cycle - already in draw" << endmsg;
   }
} // END void aReservoir_Safety :: switch_to_draw_cycle( void )




// SPECIFICATION:    informs containers and monitors of Draw/Return switch
//
// ERROR HANDLING:   none.

void aReservoir_Safety::SetCycleMon (MON_TYPE cCycleMon)
{

   if ( _cCycleMon >= MON_INACTIVE && _cCycleMon <= MON_DISCONNECT )
   {

      _cCycleMon = cCycleMon;

      // tell the containers ('tho only inlet and return care)
      _Inlet->SetMonitoringType(cCycleMon);
      _Return->SetMonitoringType(cCycleMon);
      _AC->SetMonitoringType(cCycleMon);
      _Platelet->SetMonitoringType(cCycleMon);
      _Plasma->SetMonitoringType(cCycleMon);
      _RBC->SetMonitoringType(cCycleMon);

      // tell the monitors (they all care)
      _ACRatioAverageMonitor->SetMonitoringType(cCycleMon);
      _ACRatioPumpVolumeMonitor->SetMonitoringType(cCycleMon);
      _ACRatioInstantaneousMonitor->SetMonitoringType(cCycleMon);
      _ACInfusionMonitor->SetMonitoringType(cCycleMon);
      _ACInfusionRinsebackMonitor->SetMonitoringType(cCycleMon);
      _ACInfusionAKOMonitor->SetMonitoringType(cCycleMon);
      _HypovolemiaCycleSizeMonitor->SetMonitoringType(cCycleMon);
      _HypovolemiaDonorMonitor->SetMonitoringType(cCycleMon);
      _ComponentDepletionMonitor->SetMonitoringType(cCycleMon);

      DataLog(log_level_safe_exec_info) << "Set Cycle Mon : "
                                        << (int)_cCycleMon
                                        << " "
                                        << cyclemon_text_table[(int)_cCycleMon]
                                        << endmsg;
      updateToPFR();

   }
   else
   {

      DataLog(log_level_safety_alarm_detail) << "SW Fault : cycle mon outside range : " << (int)_cCycleMon << endmsg;
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, (ALARM_VALUES)SW_FAULT);
   }
}

// SPECIFICATION:    Log at end of procedure
//
// ERROR HANDLING:   none.

void aReservoir_Safety::LogProcedureTotals (void)
{

   // 01/24/97 missing procedure totals based on fact that
   // last cycle b4 rinseback switch will not complete....
   // RB entered when product volume targets made...accumulators for
   // following log entries are only updated on cycle switches...
   // following code fixes this problem

   DataLog(log_level_safe_exec_info) << "PlasColl:"
                                     << _Plasma->GetVolume(VOL_ACCUM)
                                     << ", PltColl:"
                                     << _Platelet->GetVolume(VOL_ACCUM)
                                     << ", RBCColl:"
                                     << _RBC->GetVolume(VOL_ACCUM)
                                     << endmsg;

   DataLog(log_level_safe_exec_info) << "PlasAC:"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLASMA)
                                     << ", PltAC:"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLATELET)
                                     << ", RBCAC:"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_RBC)
                                     << endmsg;

   DataLog(log_level_safe_exec_info) << "AC to Donor:"
                                     << ( _AC->GetVolume(VOL_ACCUM)
        + _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLASMA)
        + _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLATELET)
        + _ACInfusionMonitor->GetPrivateFloatData(INFPD_RBC) )
                                     << ", Extra-corporeal:"
                                     << _HypovolemiaDonorMonitor->GetPrivateFloatData(HYPPD_EC)
                                     << endmsg;
}

// SPECIFICATION:    Log at end draw cycle
//
// ERROR HANDLING:   none.

void aReservoir_Safety::LogDrawCycle (void)
{
   bool flag;
   /// 09/18/96 log stuff for post-run ac infusion calculations
   // NOMINAL, Non-Error Operation logging
   DataLog(log_level_safe_exec_info) << "R "
                                     << _ACRatioAverageMonitor->GetAverageACRatio(flag)
                                     << " , I "
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_INF)
                                     << ", t "
                                     << (_ACInfusionMonitor->_fdrawCycleTime + _ACInfusionMonitor->_freturnCycleTime)
                                     << ", EC "
                                     << _HypovolemiaDonorMonitor->GetPrivateFloatData(HYPPD_EC)
                                     << ", WB "
                                     << _ComponentDepletionMonitor->GetPrivateFloatData(HYPPD_WBNOTRBC)
                                     << ", WBRBC "
                                     << _ComponentDepletionMonitor->GetPrivateFloatData(HYPPD_WBRBC)
                                     << endmsg;
   if (flag == TRUE) ; // keep compiler happy
}



// SPECIFICATION:    Reset cycle data at start of return
//
// ERROR HANDLING:   none.

void aReservoir_Safety::ResetReturnCycleData (void)
{
   if ( _cCycleMon == MON_FIRST_DRAW )
   {
      SetCycleMon(MON_FIRST_DRAW_COMPLETE);
   }
   // debug
   _Inlet->DumpContainer(log_level_safe_exec_info);
   _Return->DumpContainer(log_level_safe_exec_info);
   _AC->DumpContainer(log_level_safe_exec_info);
   _Plasma->DumpContainer(log_level_safe_exec_info);
   _Platelet->DumpContainer(log_level_safe_exec_info);
   _RBC->DumpContainer(log_level_safe_exec_info);

   // cause the next safety update to log the hall vs encoder error
   _Inlet->_pumpErrorLogFlag    = TRUE;
   _Return->_pumpErrorLogFlag   = TRUE;
   _AC->_pumpErrorLogFlag       = TRUE;
   _Plasma->_pumpErrorLogFlag   = TRUE;
   _Platelet->_pumpErrorLogFlag = TRUE;

   // reset bags
   _Inlet->ResetCycleVol(RETURN_CYCLE);
   _Return->ResetCycleVol(RETURN_CYCLE);
   _AC->ResetCycleVol(RETURN_CYCLE);
   _Plasma->ResetCycleVol(RETURN_CYCLE);
   _Platelet->ResetCycleVol(RETURN_CYCLE);
   _RBC->ResetCycleVol(RETURN_CYCLE);

   // reset monitoring objects
   _ACRatioAverageMonitor->ResetCycleVol(RETURN_CYCLE);
   _ACRatioPumpVolumeMonitor->ResetCycleVol(RETURN_CYCLE);
   _ACRatioInstantaneousMonitor->ResetCycleVol(RETURN_CYCLE);
   _ACInfusionMonitor->ResetCycleVol(RETURN_CYCLE);
   _ACInfusionRinsebackMonitor->ResetCycleVol(RETURN_CYCLE);
   _ACInfusionAKOMonitor->ResetCycleVol(RETURN_CYCLE);
   _HypovolemiaCycleSizeMonitor->ResetCycleVol(RETURN_CYCLE);
   _HypovolemiaDonorMonitor->ResetCycleVol(RETURN_CYCLE);
   _ComponentDepletionMonitor->ResetCycleVol(RETURN_CYCLE);

   _cSNCycle = RETURN_CYCLE;

   updateToPFR();
}

// SPECIFICATION:    Log at end return cycle
//
// ERROR HANDLING:   none.

void aReservoir_Safety::LogReturnCycle (void)
{

   DataLog(log_level_safe_exec_info) << "Return:t,"
                                     << _ACInfusionMonitor->_freturnCycleTime
                                     << ", ACPlt,"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLATELET)
                                     << ",ACPlas,"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_PLASMA)
                                     << ",ACRBC,"
                                     << _ACInfusionMonitor->GetPrivateFloatData(INFPD_RBC)
                                     << endmsg;

}

// SPECIFICATION:    Reset cycle data at start of draw
//
// ERROR HANDLING:   none.

void aReservoir_Safety::ResetDrawCycleData (void)
{
   if ( _cCycleMon == MON_FIRST_DRAW_COMPLETE )
   {
      SetCycleMon(MON_RUN_CYCLES);
   }

   // debug
   _Inlet->DumpContainer(log_level_safe_exec_info);
   _Return->DumpContainer(log_level_safe_exec_info);
   _AC->DumpContainer(log_level_safe_exec_info);
   _Plasma->DumpContainer(log_level_safe_exec_info);
   _Platelet->DumpContainer(log_level_safe_exec_info);
   _RBC->DumpContainer(log_level_safe_exec_info);

   // reset bags
   _Inlet->ResetCycleVol(DRAW_CYCLE);
   _Return->ResetCycleVol(DRAW_CYCLE);
   _AC->ResetCycleVol(DRAW_CYCLE);
   _Plasma->ResetCycleVol(DRAW_CYCLE);
   _Platelet->ResetCycleVol(DRAW_CYCLE);
   _RBC->ResetCycleVol(DRAW_CYCLE);

   // reset monitoring objects
   _ACRatioAverageMonitor->ResetCycleVol(DRAW_CYCLE);
   _ACRatioPumpVolumeMonitor->ResetCycleVol(DRAW_CYCLE);
   _ACRatioInstantaneousMonitor->ResetCycleVol(DRAW_CYCLE);
   _ACInfusionMonitor->ResetCycleVol(DRAW_CYCLE);
   _ACInfusionRinsebackMonitor->ResetCycleVol(DRAW_CYCLE);
   _ACInfusionAKOMonitor->ResetCycleVol(DRAW_CYCLE);
   _HypovolemiaCycleSizeMonitor->ResetCycleVol(DRAW_CYCLE);
   _HypovolemiaDonorMonitor->ResetCycleVol(DRAW_CYCLE);
   _ComponentDepletionMonitor->ResetCycleVol(DRAW_CYCLE);

   _cSNCycle = DRAW_CYCLE;

   updateToPFR();
}


// SPECIFICATION:    Takes the appropriate action
//                   as required to set up the
//                   reservoir monitoring for each
//                   state
//
//
// ERROR HANDLING:   None
// RETURN:           N/A
//
void aReservoir_Safety::initializeForState (long newState)
{
   switch (newState)
   {

      case METERED_STORAGE :
      case METERED_STORAGE_DISCONNECT :
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = FALSE;
         _inMeteredStorage       = TRUE;
         break;
      case DISPOSABLE_TEST :
         _cSNCycle               = DRAW_CYCLE;
         _bWetSetNotAllowed      = TRUE;
         _bPauseMonitoringActive = FALSE;
         break;
      case AC_PRIME :
         _bLookingForFirstFluid  = TRUE;
         _bPauseMonitoringActive = FALSE;
         SetCycleMon(MON_FIRST_DRAW);
         _cSNCycle               = DRAW_CYCLE;
         _bWetSetNotAllowed      = FALSE;
         break;
      case DONOR_CONNECTED :
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = FALSE;
         break;
      case BLOOD_PRIME :
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = TRUE;
         // snap shot the inlet volume at the transition to blood prime.
         // we'll use this to gate the start of pause monitoring during prime.
         _startOfBloodPrimeInletVolumeSnap = _Inlet->GetVolume(VOL_DRAW_CYCLE);
         break;
      case BLOOD_RUN :
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = TRUE;
         break;
      case BLOOD_RINSEBACK :
         // 07/29/97 msm
         // do not set rb ratio , use ratio robustness code in
         // switch_to_return cascaded to get "best" ratio using robustness
         // criteria in case switch occurs consituting an "invalid" cycle.
         //
         // SetRBRatio();

         // switch cycle to return if rb began during a draw cycle
         if ( _cSNCycle == DRAW_CYCLE )
         {
            _ACInfusionMonitor->UpdateTimers(CHW_RESERVOIR_HIGH);

            update_res_mon();

            // call here to force cycle switch to use rb ratio which has been
            // range limited in case draw just started yielding bad ratio on start of rb.
            //
            switch_to_return_cycle();
            SetCycleMon(MON_RINSEBACK);
         }
         else
         {
            SetCycleMon(MON_RINSEBACK);
         }

         // NOMINAL, Non-Error Operation logging

         DataLog(log_level_safe_exec_info) << "Start RB EC : " << _HypovolemiaDonorMonitor->GetPrivateFloatData(HYPPD_EC) << endmsg;

         // 04/21/97 MSM : Pursuant to issue tracker 1806, disable looking for first fluid
         // at trans to rinseback and donor disconnect to protect against odd where fluid has not been detected prior to
         // trans to rb ( early run abort ) , but there may be fluid present in set that
         // could 'slosh' and show fluid causing alarm as is thought to have occurred in #1806
         //
         _bLookingForFirstFluid  = FALSE;
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = TRUE;
         break;
      case DONOR_DISCONNECT :
         SetCycleMon(MON_DISCONNECT);

         // 04/21/97 MSM : Pursuant to issue tracker 1806, disable looking for first fluid
         // at trans to rinseback and donor disconnect to protect against odd where fluid has not been detected prior to
         // trans to rb ( early run abort ) , but there may be fluid present in set that
         // could 'slosh' and show fluid causing alarm as is thought to have occurred in #1806
         //
         _bLookingForFirstFluid  = FALSE;
         _bWetSetNotAllowed      = FALSE;
         _bPauseMonitoringActive = FALSE;
         break;
      default :
         break;
   }

   // Initialize containers
   _Inlet->InitializeForState(newState);
   _Return->InitializeForState(newState);
   _AC->InitializeForState(newState);
   _Plasma->InitializeForState(newState);
   _Platelet->InitializeForState(newState);
   _RBC->InitializeForState(newState);

   // Initialize the AC ratio monitoring objects for the new state
   _ACRatioAverageMonitor->InitializeForState(newState);
   _ACRatioPumpVolumeMonitor->InitializeForState(newState);
   _ACRatioInstantaneousMonitor->InitializeForState(newState);

   // Initialize the AC infusion rate monitoring objects for the new state
   _ACInfusionMonitor->InitializeForState(newState);
   _ACInfusionRinsebackMonitor->InitializeForState(newState);
   _ACInfusionAKOMonitor->InitializeForState(newState);

   // Initialize the Hypovolemia monitoring objects for the new state
   _HypovolemiaCycleSizeMonitor->InitializeForState(newState);
   _HypovolemiaDonorMonitor->InitializeForState(newState);
   _ComponentDepletionMonitor->InitializeForState(newState);

   updateToPFR();
}



// SPECIFICATION:    Sets the disposable type in the
//                   container objects which controls the
//                   flowIO in the bags
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE/FALSE
//
void aReservoir_Safety::SetSetType (SET_TYPE disposableID)
{
   DataLog(log_level_safe_exec_info) << "Received notification of set type: " << disposableID << endmsg;

   SafetyExec::instance()->update(disposableID);

   // tell the containers the set type so they can set
   // up the flow in the bags
   _Inlet->SetSetType(disposableID);
   _Return->SetSetType(disposableID);
   _AC->SetSetType(disposableID);
   _Platelet->SetSetType(disposableID);
   _Plasma->SetSetType(disposableID);
   _RBC->SetSetType(disposableID);

   // tell the cycle size monitor the set type so it can
   // set the minimum and maximum first cycle volumes
   _HypovolemiaCycleSizeMonitor->SetSetType(disposableID);

   // tell the ac infusion cycle monitor the set type so
   // it can use adjusted donor hematocrit if this is
   // a RBC/Plasma procedure
   _ACInfusionMonitor->SetSetType(disposableID);

   updateToPFR();
}

// SPECIFICATION:    Checks that the valve positions have
//                   been seen.  Required to transition to
//                   AC Connect
//
// ERROR HANDLING:   None
// RETURN:           TRUE/FALSE
//
bool aReservoir_Safety::CheckValveT1 ()
{
   bool result = FALSE;

   int  test   = (int)TRUE;
   test  &= (int)_RBC->_bag->_myValve.ValveT1OK();
   test  &= (int)_Plasma->_bag->_myValve.ValveT1OK();
   test  &= (int)_Platelet->_bag->_myValve.ValveT1OK();

   result = (bool)test;

   return(result);
}

// SPECIFICATION:    Checks that the disposable type is
//                   known.  It is required that the type
//                   be known from Donor Connect to
//                   Rinseback, inclusive.
//
// ERROR HANDLING:   None
// RETURN:           TRUE/FALSE
//
bool aReservoir_Safety::CheckSetType ()
{
   bool     result = FALSE;

   SET_TYPE set    = SafetyExec::instance()->SetType();
   if ( (set >= SET_PLTPLSRBC) && (set <= SET_RBCPLS) )
      result = TRUE;

   DataLog(log_level_safe_exec_info) << "Check " << (int)set << " for known set type = " << (int)result << endmsg;
   return(result);
}

// SPECIFICATION:    Returns TRUE if First Draw is complete.
//                   Used to determine if we can switch to
//                   Rinseback
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE/FALSE
//
bool aReservoir_Safety::FirstDrawComplete ()
{
   bool result = FALSE;

   // if we're really done with first draw return true
   if (_cCycleMon != MON_FIRST_DRAW)
      result = TRUE;
   // or if we have pumped sufficient volume to have
   // a valid ratio, calculate it and return true
   if (_HypovolemiaCycleSizeMonitor->GetPrivateFloatData(HYPPD_ACCUMCYCLE)
       >= MIN_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE)
   {
      // update the ac ratio.  The first TRUE parameter allows the
      // alarm to occur if the the ratio is too low.  The second
      // FALSE parameter stops the inlet volume accumulator from
      // alarming if the ratio is too high.  We'll test it at the
      // end of the cycle and alarm then if we need to.
      _ACRatioAverageMonitor->UpdateAverageRatio(TRUE, FALSE);
      result = TRUE;
   }

   return(result);
}


// SPECIFICATION:    Used during a PF Recovery into Blood Run.
//                   Determines if we should switch to a return
//                   cycle depending on the control side
//                   reservoir status.  Also adds in the duration
//                   of the power fail so we can take advantage
//                   of that time in infusion monitoring.
//
//
// ERROR HANDLING:   None
// RETURN:           NA
//
void aReservoir_Safety::switchCycleForPFR ()
{
   struct timespec now;
   float           previousTime, powerOffTime;

   if ( ( _cSNCycle == DRAW_CYCLE )
        &&( _resLevel == CHW_RESERVOIR_HIGH) )
   {

      // save the time we had been in draw before the power fail
      previousTime = _ACInfusionMonitor->_fdrawCycleTime;

      // get the time from when we powered off to now
      clock_gettime(CLOCK_REALTIME, &now);
      powerOffTime = now.tv_sec - SafetyPFRDataFromRestore.PFTimeOfFailure;
      _ACInfusionMonitor->_fdrawCycleTime = previousTime + powerOffTime;

      // debug
      DataLog(log_level_safe_exec_info) << "Draw Time in PFR: Before PF: "
                                        << previousTime
                                        << ", PF: "
                                        << powerOffTime
                                        << ", Total Draw Time: "
                                        << _ACInfusionMonitor->_fdrawCycleTime
                                        << endmsg;


      update_res_mon();

      switch_to_return_cycle();

   }
   else
   {
      // save the time we had been in return before the power fail
      previousTime = _ACInfusionMonitor->_freturnCycleTime;

      // get the time from when we powered off to now
      clock_gettime(CLOCK_REALTIME, &now);
      powerOffTime = now.tv_sec - SafetyPFRDataFromRestore.PFTimeOfFailure;
      _ACInfusionMonitor->_freturnCycleTime = previousTime + powerOffTime;

      // debug
      DataLog(log_level_safe_exec_info) << "Return Time in PFR: Before PF: "
                                        << previousTime
                                        << ", PF: "
                                        << powerOffTime
                                        << ", Total Return Time: "
                                        << _ACInfusionMonitor->_freturnCycleTime
                                        << endmsg;


   }
}




// SPECIFICATION:    Determines if there is AC in the needle.
//                   Used during a pause or stop to determine
//                   if we need to make sure the pumps move.
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE = there is not AC in the needle and
//                   we need to worry about whether the pumps run
//                   FALSE = there is AC in the needle and we
//                   don't have to worry
//

bool aReservoir_Safety::shouldWeWorry ()
{

   bool  result                  = TRUE;
   bool  flag;
   float drawCycleReturnAdjusted = 0;
   float avgACRatio              = 0;

   // first, are we in one of the blood prime, run or rinseback monitoring states?
   // or are we at least in donor connect?
   // if not, just return false
   // we don't have an average AC ratio until then
   if ( (_cCycleMon == MON_INACTIVE)
        ||(_cCycleMon == MON_DISCONNECT)
        ||(_bPauseMonitoringActive == FALSE) )
      result = FALSE;
   else if (SafetyExec::instance()->SystemState() == BLOOD_PRIME)
   {
      // if we are in blood prime, don't start monitoring until the
      // inlet pump has pumped a bit
      if ( ( _Inlet->GetVolume(VOL_DRAW_CYCLE) - _startOfBloodPrimeInletVolumeSnap) <= .5f)
         // not yet time to start pause monitoring.  return false
         result = FALSE;
      else
      {
         DataLog(log_level_safe_exec_info) << "Enable pause monitoring.  Inlet in blood prime = "
                                           << (_Inlet->GetVolume(VOL_DRAW_CYCLE) - _startOfBloodPrimeInletVolumeSnap)
                                           << endmsg;
      }
   }

   if (result == TRUE)
   {
      // get the average AC ratio.  The first FALSE parameter allows
      // the AC Ratio monitor to return an AC ratio below 5
      // without causing an alarm.  For the purposes of this call
      // we are only trying to see if the needle is anti-coagulated and
      // we don't care if the ratio is too low until we start to return.
      // The second FALSE parameter stops the inlet volume accumulator from
      // alarming if the ratio is above 15.  We'll test the volume at the
      // end of the cycle and alarm then if we need to.
      _ACRatioAverageMonitor->UpdateAverageRatio(FALSE, FALSE);
      avgACRatio = _ACRatioAverageMonitor->GetAverageACRatio(flag);

      // if it is during first draw, we have to adjust the return volume
      // for the maximum volume pumped into the return line away from the donor
      if (_cCycleMon == MON_FIRST_DRAW)
         drawCycleReturnAdjusted = _Return->GetVolume(VOL_DRAW_CYCLE) - _Return->_fReturnVolumeAwayFromDonor;
      else
         drawCycleReturnAdjusted = _Return->GetVolume(VOL_DRAW_CYCLE);

      if (flag == TRUE)
      {
         if (avgACRatio < MAX_ALLOWABLE_AC_RATIO )
         {
            // if we are in a return cycle during blood run, then make sure that
            // at least 3 ml of fluid was pumped out the needle.
            if ( ( (_cCycleMon == MON_RUN_CYCLES) || (_cCycleMon == MON_FIRST_DRAW_COMPLETE) )
                 &&(_cSNCycle == RETURN_CYCLE)
                 &&( (_Return->GetVolume(VOL_RETURN_CYCLE) - _Inlet->GetVolume(VOL_RETURN_CYCLE) ) >= PFR_AC_IN_NEEDLE_VOL ) )
               result = FALSE;

            // alternatively, if we are in rinseback and there has been at least
            // 3 ml of fluid pumped out the needle, then there is AC in the needle
            else if ( (_cCycleMon == MON_RINSEBACK)
                      &&( (_Return->GetVolume(VOL_RETURN_CYCLE) - _Inlet->GetVolume(VOL_RETURN_CYCLE) ) >= PFR_AC_IN_NEEDLE_VOL ) )
               result = FALSE;

            // yet another possibility is, if we are in a draw cycle and we have performed
            // AKO, the needle is anti-coagulated if the inlet pump has not run since the AKO.
            else if ( ( (_cCycleMon == MON_RUN_CYCLES) || (_cCycleMon == MON_FIRST_DRAW) )
                      &&(_cSNCycle == DRAW_CYCLE)
                      &&(drawCycleReturnAdjusted >= PFR_AC_IN_NEEDLE_VOL)
                      &&(_Inlet->GetVolume(VOL_DRAW_CYCLE) <= (_pumpTimerInletVolumeSnap + 0.2f) ) )
               result = FALSE;

         }
      }
      else
      {
         // you're done - an alarm has been generated
         // inside the UpdateAverageRatio call
         // just logging...
         DataLog(log_level_safety_alarm_detail) << "Un-useable average AC ratio: " << avgACRatio << endmsg;
      }
   }
   if (result == TRUE)
   {
      // debug
      DataLog(log_level_safe_exec_info) << "result:"
                                        << ( (result == TRUE) ? "TRUE" :
           (result == FALSE) ? "FALSE" :
           "UNKNOWN" )
                                        << "(" << (int)result << ")"
                                        << "; Needle Status CycleMon: "
                                        << ( ( _cCycleMon >= MON_INACTIVE && _cCycleMon <= MON_DISCONNECT ) ?
           cyclemon_text_table[(int)_cCycleMon] :
           "UNKNOWN")
                                        << "(" << (int)_cCycleMon << ")"
                                        << "; ACRatio: "
                                        << avgACRatio
                                        << "; Cycle: "
                                        << ((_cSNCycle == DRAW_CYCLE) ? "DRAW_CYCLE" :
          (_cSNCycle == RETURN_CYCLE) ? "RETURN_CYCLE" :
          "UNKNOWN")
                                        << "(" << (int)_cSNCycle << ")"
                                        << "; InletVol(rtn;drw): ("
                                        << _Inlet->GetVolume(VOL_RETURN_CYCLE)
                                        << ";"
                                        << _Inlet->GetVolume(VOL_DRAW_CYCLE) << ")"
                                        << " ReturnVol(rtn;drw): ("
                                        << _Return->GetVolume(VOL_RETURN_CYCLE)
                                        << ";"
                                        << drawCycleReturnAdjusted << ")"
                                        << endmsg;
   }


   return(result);

}

void aReservoir_Safety::UpdateReservoirSafety (SHW_RESERVOIR_STATES reservoir)
{
   _lowLevel = reservoir;

   //
   //
   //   All this is just to log reservoir status.  Log in bursts of six (nominal 3-second rate)
   //   to avoid excessive logging when the reservoir status is bouncing quickly.
   //
   char logvalue = '?';
   if (_lowLevel == SHW_RESERVOIR_EMPTY)
   {
      logvalue = 'E';
   }
   else if (_lowLevel == SHW_RESERVOIR_LOW)
   {
      logvalue = 'L';
   }

   //
   //
   //   It's time to fill the log buffer if there has been a transition that's different
   //   from the last logged value
   //
   if ((_lowLevelLogIndex == LOW_LEVEL_LOG_SAMPLES) && (logvalue != _lowLevelLastLogged))
   {
      _lowLevelLogIndex = 0;
   }

   if (_lowLevelLogIndex < LOW_LEVEL_LOG_SAMPLES) // we are filling the buffer
   {
      _lowLevelLog[_lowLevelLogIndex] = _lowLevelLastLogged = logvalue;
      if (++_lowLevelLogIndex == LOW_LEVEL_LOG_SAMPLES) // buffer now full
      {
         DataLog(log_level_safe_exec_info) << "Safety LL Sensor (new to old): ";
         for (int logidx = 0; logidx < LOW_LEVEL_LOG_SAMPLES; logidx++)
         {
            //
            //
            //    Log backwards to be similar to ultrasonics reservoir logging
            //
            DataLog(log_level_safe_exec_info) << _lowLevelLog[LOW_LEVEL_LOG_SAMPLES - logidx - 1] << " ";
         }
         DataLog(log_level_safe_exec_info) << endmsg;
      }
   }
}

// SPECIFICATION:    Looks at actual to see if inlet or return are stopped
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE = Inlet and return are stopped
//
bool aReservoir_Safety::arePumpsStopped ()
{
   if ( (_Inlet->GetVolume(VOL_DELTA) <= 0.005f)
        && fabs(_Return->GetVolume(VOL_DELTA) <= 0.005f) )
      return(TRUE);
   else
      return(FALSE);
}

// SPECIFICATION:    Looks for wet set and alarms if detected but not allowed
//                   If the cassette is constrained to be down, a wet set at
//                   startup is OK
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE = Inlet and return are stopped
//
void aReservoir_Safety::checkForWetSet ()
{

   if ( (_bWetSetNotAllowed == TRUE)
        &&(_paCassette->isFirstSampleIn() == TRUE )
        &&(_paCassette->GetDeviceConstraint() == CASSETTE_MAY_BE_MOVED)
        &&( !_inMeteredStorage )
        )
   {


      if ( _lowLevel != SHW_RESERVOIR_EMPTY )
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, RESERVOIR_FAILED_T0);
   }
}

// SPECIFICATION:    Looks for wet set and alarms if detected while
//                   not in Blood Prime
//
//
// ERROR HANDLING:   None
// RETURN:           TRUE = Inlet and return are stopped
//
void aReservoir_Safety::firstFluid ()
{
   if ( _bLookingForFirstFluid &&
        (SafetyExec::instance()->SystemState() != BLOOD_PRIME)
        )
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, PRIME_TOO_SOON);
   }
   else
      _bLookingForFirstFluid = FALSE;

   updateToPFR();
}


// SPECIFICATION:    Calculates the time which has gone by on the
//                   Control computer side since it last sent a
//                   status message
//                   As a maintainence issue, compare the control
//                   side time to the safety side time to see if
//                   we are having communications problems.
//
//
// ERROR HANDLING:   None
// RETURN:           NA
//
void aReservoir_Safety::SetDeltaControlStatusTime (timespec newTime)
{
   long     deltaSafetyStatusTime;
   long     timeSlip;
   timespec safetyNow;

   _ldeltaControlStatusTime  = (newTime.tv_sec - _tLastStatusTime.tv_sec) * 1000000;  // sec to us
   _ldeltaControlStatusTime += ((newTime.tv_nsec - _tLastStatusTime.tv_nsec) / 1000); // nsec to us

   if ( (_ldeltaControlStatusTime < 0)
        ||(_ldeltaControlStatusTime > 10000000) ) // somebody reset the clock
   {
      _ldeltaControlStatusTime = 1000000;        // default to 1 sec
   }

   _tLastStatusTime = newTime;

   // calculate the safety side delta
   clock_gettime(CLOCK_REALTIME, &safetyNow);
   deltaSafetyStatusTime  = (safetyNow.tv_sec - _tSafetyEquivTime.tv_sec ) * 1000000; // sec to us
   deltaSafetyStatusTime += ((safetyNow.tv_nsec - _tSafetyEquivTime.tv_nsec) / 1000);

   if ( (deltaSafetyStatusTime < 0)
        ||(deltaSafetyStatusTime > 10000000) ) // somebody reset the clock
   {
      deltaSafetyStatusTime = 1000000;        // default to 1 sec
   }

   _tSafetyEquivTime = safetyNow;

   // compare safety side delta to control side delta
   // if they differ by at least 1sec, log it
   timeSlip = _ldeltaControlStatusTime - deltaSafetyStatusTime;

   if ( (timeSlip > 1000000) || (timeSlip < -1000000) )
   {
      DataLog(log_level_safe_exec_error) << "Communication time slip = " << timeSlip << endmsg;
   }
}

/* FORMAT HASH 9c97bf05ea47379d81623cb2650e00af */
