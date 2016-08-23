/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      hypovold.cpp, the donor hypovolemia monitoring object
 *
 * ABSTRACT:   Safety's monitoring for hypovolemia.
 *             Monitors for the following situations:
 *                Excess volume removed from donor
 *                Valves not in return during rinseback
 */


#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "alarms.h"

#include "s_exec.hpp"
#include "hypovol.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  HypovolemiaDonor Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety donor hypovolemia monitor constructor
//
// ERROR HANDLING:   none.
HypovolemiaDonor::HypovolemiaDonor(InletContainer* InletCont,
                                   ReturnContainer* ReturnCont,
                                   ACContainer* ACCont,
                                   PlateletContainer* PlateletCont,
                                   PlasmaContainer* PlasmaCont,
                                   RBCContainer* RBCCont,
                                   aDonor* Donor)
   : _Inlet(InletCont),
     _Return(ReturnCont),
     _AC(ACCont),
     _Platelet(PlateletCont),
     _Plasma(PlasmaCont),
     _RBC(RBCCont),
     _Donor(Donor)
{
   _faccumECVolume                = 0.0f;
   _iRBValveBadPositionCounter    = 0;
   _bHypovolemiaMonitoringActive  = FALSE;
   _bHypervolemiaMonitoringActive = FALSE;
   _bHypoOneShot                  = FALSE;
   _monType  = MON_INACTIVE;
   _dumpFlag = TRUE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_AC);
   trima_assert(_Platelet);
   trima_assert(_Plasma);
   trima_assert(_RBC);
   trima_assert(_Donor);

   SaveToPFR();
}

// SPECIFICATION:    Safety donor hypovolemia monitor destructor
//
// ERROR HANDLING:   none.
HypovolemiaDonor::~HypovolemiaDonor()
{
   _Inlet    = NULL;
   _Return   = NULL;
   _AC       = NULL;
   _Platelet = NULL;
   _Plasma   = NULL;
   _RBC      = NULL;
   _Donor    = NULL;
}

// SPECIFICATION:    Initialize hypovolemia due to cycle size monitoring
//                   for appropriate state
//
// ERROR HANDLING:   none
void HypovolemiaDonor::InitializeForState (long newState)
{
   int         j;
   const char* systemStateName;

   // This is a lookup in a global array containing state names
   for (j = 0;
        state_name_table[j].state != newState && state_name_table[j].state != NOCOLLECT_STATE;
        j++) ;

   if (state_name_table[j].state != newState)
      systemStateName = "UNKNOWN";
   else
      systemStateName = state_name_table[j].state_text;

   DataLog(log_level_safe_exec_info) << "Start " << systemStateName << " EC Volume: " << _faccumECVolume;

   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
         _bHypovolemiaMonitoringActive  = true;
         _bHypervolemiaMonitoringActive = true;
         DataLog(log_level_safe_exec_info) << "; activate hypO/hypERvolemia monitoring" << endmsg;
         break;

      case BLOOD_RINSEBACK :
         _bHypovolemiaMonitoringActive  = true;
         _bHypervolemiaMonitoringActive = true;
         _iRBValveBadPositionCounter    = 0;
         DataLog(log_level_safe_exec_info) << "; activate hypO/hypERvolemia and deactivate RBCvalve monitoring" << endmsg;
         break;

      case DONOR_DISCONNECT :
         _bHypovolemiaMonitoringActive  = true;
         _bHypervolemiaMonitoringActive = false;
         DataLog(log_level_safe_exec_info) << "; activate hypOvolemia and deactivate hypERvolemia monitoring" << endmsg;
         break;

      case POST_RUN :
         _bHypovolemiaMonitoringActive  = false;
         _bHypervolemiaMonitoringActive = false;
         DataLog(log_level_safe_exec_info) << "; deactivate hypO/hypERvolemia monitoring" << endmsg;
         break;

      case METERED_STORAGE :
         _bHypovolemiaMonitoringActive  = false;
         _bHypervolemiaMonitoringActive = false;
         DataLog(log_level_safe_exec_info) << "; deactivate hypO/hypERvolemia monitoring (for MSS)" << endmsg;
         break;

      default :
         DataLog(log_level_safe_exec_info) << endmsg;
         break;

   }

   DataLog(log_level_safe_exec_info) << "Current monitoring status:"
                                     << " HypOvolemia:" << ( (_bHypovolemiaMonitoringActive) ? "ON" : "OFF" )
                                     << "; HypERvolemia:" << ( (_bHypervolemiaMonitoringActive) ? "ON" : "OFF" )
                                     << "; RBC valve:" << ( (_iRBValveBadPositionCounter != 0)  ? "ON" : "OFF" )
                                     << endmsg;

   SaveToPFR();
}


// SPECIFICATION:    Hypovolemia monitoring update method.
//
// ERROR HANDLING:   Donor Hypovolemia alarm
//                   Platelet Valve Out of Position alarm
//                   Plasma Valve Out of Position alarm
//                   RBC Valve Out of Position alarm
void HypovolemiaDonor::Update (char cCycle)
{
   static char logCycle = 0xff;  // force logging first time through

   float       deltaInlet, deltaReturn, deltaAC;
   float       deltaPlatelet, deltaPlasma, deltaRBC;
   float       ECLimit;

   // if hypovolemia monitoring is not active
   // or if this is not a draw cycle, just return.
   if ( (!_bHypovolemiaMonitoringActive && !_bHypervolemiaMonitoringActive) ||
        (_monType == MON_INACTIVE)
        )
   {
      SaveToPFR();
      return;
   }

   //
   //
   //   We should never be monitoring hypovolemia if we don't know where
   //   the volumes are going (IT9874)
   //
   if (_bHypovolemiaMonitoringActive)
   {
      SET_TYPE set = SafetyExec::instance()->SetType();

      if ((set == SET_NOT_LOADED) || (set == SET_UNKNOWN))
      {
         DataLog(log_level_safety_alarm_detail) << "Safety shutdown for hypovolemia monitoring with invalid set type " << set << endmsg;
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
      }
   }

   // get pump volumes for this sample
   deltaInlet    = _Inlet->GetVolume(VOL_DELTA);
   deltaReturn   = _Return->GetVolume(VOL_DELTA);
   deltaAC       = _AC->GetVolume(VOL_DELTA);
   deltaPlatelet = _Platelet->GetVolume(VOL_DELTA);
   deltaPlasma   = _Plasma->GetVolume(VOL_DELTA);
   deltaRBC      = _RBC->GetVolume(VOL_DELTA);

   // accumulate the extra corporeal volume
   // over the entire procedure as long as
   // the donor is connected (to avoid AC prime volume)
   if (( (_monType == MON_FIRST_DRAW) ||
         (_monType == MON_FIRST_DRAW_COMPLETE) ||
         (_monType == MON_RINSEBACK) ||
         (_monType == MON_DISCONNECT) ) &&
       (_Donor->GetDonorConnectedState() == DONOR_IS_CONNECTED))
   {
      _faccumECVolume += (deltaInlet
                          -  deltaAC
                          -  deltaReturn);
   }
   else
      _faccumECVolume -= (deltaAC
                          +  deltaPlatelet
                          +  deltaPlasma
                          +  deltaRBC);

   // test volume against limit
   ECLimit = _Donor->GetECLimit_ml();

   //
   //
   // Log hypovolemia stats on a cycle by cycle basis.
   //
   if (cCycle != logCycle)
   {
      logCycle = cCycle;
      float hypodistance = ECLimit - _faccumECVolume;

      DataLog(log_level_safe_exec_info) << "Safety hypovolemia distance: " << hypodistance
                                        << " (" << ECLimit << " - " << _faccumECVolume << ")" << endmsg;
   }

   // EC accumulates in a positive direction for volume removed and
   // in a negative direction for volume added.

   //
   //
   //  We will make a single attempt when we are approaching the hypovolemia limit
   //  to ask proc to start rinseback.  If proc ignores the request then the hypovolemia
   //  alarm will assert to shut down the system (without rinseback).
   //
   if (_bHypovolemiaMonitoringActive)
   {
      if ((ECLimit - _faccumECVolume) < 5.0f) //  allow 5ml of "time" for proc to respond
      {
         if (!_bHypoOneShot)
         {
            _bHypoOneShot = true;

            DataLog(log_level_safety_alarm_detail) << "Safety Hypovolemia Rinseback request:"
                                                   << " EC Accrual = " << _faccumECVolume << ", Limit = " << ECLimit << endmsg;

            RinsebackRequestMsg procHint(MessageBase::SEND_GLOBAL);
            procHint.send(1);
         }
      }

      if (_faccumECVolume >= ECLimit)
      {
         // hypo shutdown
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DONOR_HYPOVOLEMIA_ALARM);

         if (_dumpFlag)
         {
            _dumpFlag = false;
            Dump(log_level_safety_alarm_detail);
         }

         DataLog(log_level_safety_alarm_detail) << "Alarm Hypovolemia : EC Accrual = "
                                                << _faccumECVolume << ", Limit = " << ECLimit << " Cycle: "
                                                << (int)cCycle << endmsg;
      }
   }

   if ( (_faccumECVolume <= MAX_ALLOWABLE_EC_CHANGE) && _bHypervolemiaMonitoringActive)
   {
      // hyper shutdown
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, HYPERVOLEMIA_DETECTED);

      if (_dumpFlag)
      {
         _dumpFlag = false;
         Dump(log_level_safety_alarm_detail);
      }

      DataLog(log_level_safety_alarm_detail) << "Alarm Hypervolemia : EC Accrual = "
                                             << _faccumECVolume << ", Limit = " << MAX_ALLOWABLE_EC_CHANGE << " Cycle: "
                                             << (int)cCycle << endmsg;
   }

   if ( (_monType == MON_RINSEBACK) && _bHypovolemiaMonitoringActive)
   {
      // during RB all valves must be in return
      // IT5009: no longer check RBC valve
      // IT3834: don't check plasma valve either
      if (_Platelet->_bag->_myValve._positionS != HW_VALVE_RETURN)
      {
         DataLog(log_level_safe_exec_info) << "Rinseback Valves Position Error Plt: "
                                           << (int)_Platelet->_bag->_myValve._positionS
                                           << ", Pls: "
                                           << (int)_Plasma->_bag->_myValve._positionS
                                           << ", RBC: "
                                           << (int)_RBC->_bag->_myValve._positionS
                                           << endmsg;

         if (++_iRBValveBadPositionCounter > RB_VALVE_FAULT_TOLERANT_THRESHOLD)
         {
            if (_Platelet->_bag->_myValve._positionS != HW_VALVE_RETURN)
               paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, PLATELET_OUT_OF_POSITION);

            if (_dumpFlag)
            {
               _dumpFlag = false;
               Dump(log_level_safety_alarm_detail);
            }
         }
      }
   }

   SaveToPFR();
}

// SPECIFICATION:    Hypovolemia monitoring method to
//                   return private data for logging
//
// ERROR HANDLING:   FATAL ERROR for unknown type.
float HypovolemiaDonor::GetPrivateFloatData (HYP_PDATA_TYPE type)
{
   float result = FLT_MAX;   // To keep GNU compiler happy for production builds

   switch (type)
   {
      case HYPPD_EC :
         result = _faccumECVolume;
         break;

      default :
      {
         DataLog(log_level_critical) << "Type: " << type << "Request for unknown data" << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid HYP_PDATA_TYPE");
      }
      break;
   }

   return result;
}


// SPECIFICATION:    Hypovolemia  monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   FATAL ERROR for unknown cycle.
void HypovolemiaDonor::ResetCycleVol (char cCycle)
{
   // no resets
   //   if (cCycle == DRAW_CYCLE);
   //   else if (cCycle == RETURN_CYCLE);
}


// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.
void HypovolemiaDonor::Dump (DataLog_Level log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
   _Platelet->DumpContainer(log_level);
   _Plasma->DumpContainer(log_level);
   _RBC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.
void HypovolemiaDonor::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_faccumECVolume = _faccumECVolume;
   SafetyPFRDataToSave.PFR_iRBValveBadPositionCounter             = _iRBValveBadPositionCounter;
   SafetyPFRDataToSave.PFR_hypovolD_bHypovolemiaMonitoringActive  = _bHypovolemiaMonitoringActive;
   SafetyPFRDataToSave.PFR_hypovolD_bHypervolemiaMonitoringActive = _bHypervolemiaMonitoringActive;
   SafetyPFRDataToSave.PFR_hypovolD_monType                       = _monType;
}

// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.
void HypovolemiaDonor::RestoreFromPFR ()
{
   _faccumECVolume                = SafetyPFRDataFromRestore.PFR_faccumECVolume;
   _iRBValveBadPositionCounter    = SafetyPFRDataFromRestore.PFR_iRBValveBadPositionCounter;
   _bHypovolemiaMonitoringActive  = SafetyPFRDataFromRestore.PFR_hypovolD_bHypovolemiaMonitoringActive;
   _bHypervolemiaMonitoringActive = SafetyPFRDataFromRestore.PFR_hypovolD_bHypervolemiaMonitoringActive;
   _monType = SafetyPFRDataFromRestore.PFR_hypovolD_monType;

   DataLog(log_level_safe_exec_info) << "HypovolemiaDonor::RestoreFromPFR(), "
                                     << "faccumECVolume: " << _faccumECVolume
                                     << " iRBValveBadPositionCounter: " << _iRBValveBadPositionCounter
                                     << " bHypovolemiaMonitoringActive: " << _bHypovolemiaMonitoringActive
                                     << " bHypervolemiaMonitoringActive: " << _bHypervolemiaMonitoringActive
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH 3e44a3950d3ac342a5787e503ed0452e */
