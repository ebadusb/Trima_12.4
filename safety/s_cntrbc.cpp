/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_rbccnt.cpp, an RBC container
 *
 * ABSTRACT:   Safety's view of an RBC container.  A RBC container has a
 *             pseudo pump, a bag and a valve.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_datalog.h"
#include "alarms.h"

#include "s_exec.hpp"
#include "s_glob.hpp"
#include "s_cont.hpp"
#include "s_pfr.hpp"

#define RBC_BAG_PROCEDURE_MAX -500.0   // procedure's limit for RBC volume

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  RBCContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety RBCContainer constructor
//
// ERROR HANDLING:   none.

RBCContainer::RBCContainer()
   :  baseContainer()
{
   _bag = new rbcBag();
   SetName("RBC Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(DONOR_HYPOVOLEMIA_ALARM);

   // save to PFR
   SaveToPFR();
}


// SPECIFICATION:    Safety RBCContainer destructor
//
// ERROR HANDLING:   none.

RBCContainer::~RBCContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety RBCContainer initialize for state transitions
//
// ERROR HANDLING:   none.

void RBCContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   // Initialize the valve for the new state
   _bag->_myValve.InitializeForState(newState);

   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Safety RBCContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void RBCContainer::SetSetType (SET_TYPE setType)
{
   float maxVol = 0.0f;

   // set the flow direction in the bag based on the set type
   _bag->SetFlowIO(Set[(int)setType].RBCBagFlow);
   _bag->SetValveIO(Set[(int)setType].RBCValveFlowAllowed);

   // if we are collecting with this set, set the bag limits
   if ( (Set[(int)setType].RBCBagFlow == FLOW_INTO_BAG) ||
        (Set[(int)setType].RBCBagFlow == FLOW_INTO_BAG_RC) )
   {
      if (setType == SET_RBCPLS)
         _bag->SetMaxVol(2.0 * RBC_BAG_PROCEDURE_MAX * MAX_BAG_MULTIPLIER);
      else
         _bag->SetMaxVol(RBC_BAG_PROCEDURE_MAX * MAX_BAG_MULTIPLIER);
   }

   DataLog(log_level_safe_exec_info) << _containerName << " FlowIO: " << _bag->_flowIO
                                     << " ValveIO: " << _bag->_valveIO;

   if (_bag->GetMaxVol(maxVol))
      DataLog(log_level_safe_exec_info) << " VolLimit: " << maxVol;

   DataLog(log_level_safe_exec_info) << endmsg;

   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Safety RBCContainer update from control status
//
// ERROR HANDLING:   none.

void RBCContainer::UpdateContainerControl (HW_VALVE_STATES positionC,
                                           HW_VALVE_STATES plasmaPositionC,
                                           HW_VALVE_STATES plateletPositionC)
{
   _bag->setCollectMode(positionC, plateletPositionC);

   float InletMultiplier, PlateletMultiplier, PlasmaMultiplier;

   // The pumps do not know which direction they are pumping so
   // we have to tell them by giving them a multiplier in order
   // to correctly accumulate the RBC flow
   //
   // the inlet pump is always FLOW_INTO_SET so it always adds to
   // the RBC flow
   SET_TYPE setType = SafetyExec::instance()->SetType();

   if (Set[setType].InletFlow == FLOW_INTO_BAG)
      InletMultiplier = -1.0;
   else
      InletMultiplier = 1.0;

   // the platelet pump is FLOW_INTO_BAG in most sets but it is
   // FLOW_INTO_SET for the rbc/pls set when it pumps replacement fluid.
   // the replacement fluid does not affect RBC flow
   if (Set[setType].PlateletBagFlow == FLOW_INTO_BAG)
      PlateletMultiplier = -1.0;
   else
      PlateletMultiplier = 0.0;

   // the plasma pump is FLOW_INTO_BAG in most sets but it is
   // FLOW_INTO_BAG_RC for the single-stage plt/pls/rbc set when
   // it is sometimes used to maintain flow in the collect line.
   // if the plasma valve is collecting, then it affects RBC flow
   // but if it is not collecting, then it does not affect RBC flow
   if (Set[setType].PlasmaBagFlow == FLOW_INTO_BAG)
      PlasmaMultiplier = -1.0;
   else if (Set[setType].PlasmaBagFlow == FLOW_INTO_BAG_RC)
   {
      if (plasmaPositionC == HW_VALVE_COLLECT)
         PlasmaMultiplier = -1.0;
      else
         PlasmaMultiplier = 0.0;
   }
   else
      PlasmaMultiplier = 0.0;

   // Update valve from control
   _bag->_myValve.UpdateControlValve(positionC);

   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Safety RBCContainer update from safety status
//
// ERROR HANDLING:   none.

void RBCContainer::UpdateContainerSafety (HW_VALVE_STATES positionS)
{
   // Update valve from safety
   _bag->_myValve.UpdateSafetyValve(positionS);

   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void RBCContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRRBC_pumpErrorLogFlag = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRRBC_monType          = _monType;

   // bag data
   SafetyPFRDataToSave.PFRRBC_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRRBC_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRRBC_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRRBC_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRRBC_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRRBC_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRRBC_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRRBC_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRRBC_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRRBC_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRRBC_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRRBC_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRRBC_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRRBC_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;

   updateValveToPFR();
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void RBCContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag = SafetyPFRDataFromRestore.PFRRBC_pumpErrorLogFlag;
   _monType          = SafetyPFRDataFromRestore.PFRRBC_monType;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRRBC_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRRBC_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRRBC_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRRBC_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRRBC_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRRBC_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRRBC_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRRBC_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRRBC_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRRBC_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRRBC_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRRBC_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRRBC_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRRBC_returnCycleVolumeInfusion;

   DataLog(log_level_safe_exec_info) << "RBCContainer::RestoreFromPFR(), "
                                     << "pumpErrorLogFlag: " << _pumpErrorLogFlag
                                     << " monType: " << (MON_TYPE)_monType
                                     << " Bag data: initialVolume: " << _bag->_initialVolume
                                     << " maximumVolume: " << _bag->_maximumVolume
                                     << " minimumVolume: " << _bag->_minimumVolume
                                     << " flowIO: " << _bag->_flowIO
                                     << " valveIO: " << _bag->_valveIO
                                     << " accumulatedVolume: " << _bag->_accumulatedVolume
                                     << " deltaVolume: " << _bag->_deltaVolume
                                     << " drawCycleVolume: " << _bag->_drawCycleVolume
                                     << " returnCycleVolume: " << _bag->_returnCycleVolume
                                     << " drawCycleVolumeInfusion: " << _bag->_drawCycleVolumeInfusion
                                     << " returnCycleVolumeInfusion: " << _bag->_returnCycleVolumeInfusion
                                     << endmsg;

   // valve data
   restoreValveFromPFR();

   DataLog(log_level_safe_exec_info) << "RBCContainer::RestoreValveFromPFR(), "
                                     << "positionC: " << _bag->_myValve._positionC
                                     << " positionS: " << _bag->_myValve._positionS
                                     << " iT1Status " << _bag->_myValve._iT1Status
                                     << " bValvePositionMonitorEnabled: " << _bag->_myValve._bValvePositionMonitorEnabled
                                     << endmsg;

   // save to PFR
   SaveToPFR();
}


void RBCContainer::updateValveToPFR ()
{
   SafetyPFRDataToSave.PFRRBC_positionC                    = _bag->_myValve._positionC;
   SafetyPFRDataToSave.PFRRBC_positionS                    = _bag->_myValve._positionS;
   SafetyPFRDataToSave.PFRRBC_iT1Status                    = _bag->_myValve._iT1Status;
   SafetyPFRDataToSave.PFRRBC_bValvePositionMonitorEnabled = _bag->_myValve._bValvePositionMonitorEnabled;
}

void RBCContainer::restoreValveFromPFR ()
{
   _bag->_myValve._positionC                    = SafetyPFRDataToSave.PFRRBC_positionC;
   _bag->_myValve._positionS                    = SafetyPFRDataToSave.PFRRBC_positionS;
   _bag->_myValve._iT1Status                    = SafetyPFRDataToSave.PFRRBC_iT1Status;
   _bag->_myValve._bValvePositionMonitorEnabled = SafetyPFRDataToSave.PFRRBC_bValvePositionMonitorEnabled;

}

/**
* Base Container Interface Implementation
**/

// calculate the volumes
void RBCContainer::UpdateContainer (char cCycle)
{
   VOLUME_VALID result;

   result = _bag->UpdateVol(cCycle);

   if (result == VOLUME_FLOW_ERROR)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " flow with unknown flowIO not allowed: " << _bag->_flowIO << endmsg;

      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
   }
   else if (result == VOLUME_MIN_EXCEEDED)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " Minimum Volume Exceeded: " << _bag->_accumulatedVolume << endmsg;

      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _minimumAlarm);
   }
   else if (result == VOLUME_MAX_EXCEEDED)
   {
      DataLog(log_level_safety_alarm_detail) << _containerName
                                             << " Maximum Volume Exceeded: " << _bag->_accumulatedVolume << endmsg;

      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, _maximumAlarm);
   }

   // Call Container-specific PFR routine
   SaveToPFR();
}

float RBCContainer::GetVolume (VOL_TYPE type) const
{
   float volume;

   if (_bag != NULL)
   {
      switch (type)
      {
         case VOL_DELTA :
            volume = _bag->_deltaVolume;
            break;
         case VOL_ACCUM :
            volume = _bag->_accumulatedVolume;
            break;
         case VOL_DRAW_CYCLE :
            volume = _bag->_drawCycleVolume;
            break;
         case VOL_RETURN_CYCLE :
            volume = _bag->_returnCycleVolume;
            break;
         case VOL_DRAW_CYCLE_INFUSION :
            volume = _bag->_drawCycleVolumeInfusion;
            break;
         case VOL_RETURN_CYCLE_INFUSION :
            volume = _bag->_returnCycleVolumeInfusion;
            break;
         default :
         {
            _FATAL_ERROR(__FILE__, __LINE__, "Request for unknown VOL_TYPE");
            break;
         }
      }
   }
   else
   {
      DataLog(log_level_safe_exec_info) << "No bag" << endmsg;
      volume = 0.0f;
   }

   return volume;
}

void RBCContainer::DumpContainer (DataLog_Level& log_level) const
{
   if (_bag != NULL)                         // a one pump bag (i.e.platelet)
      DataLog(log_level) << _containerName
                         << " flowIO=" << _bag->_flowIO
                         << " deltaV=" << _bag->_deltaVolume
                         << " accumV=" << _bag->_accumulatedVolume
                         << " drawV=" << _bag->_drawCycleVolume
                         << " returnV=" << _bag->_returnCycleVolume
                         << " drawVI=" << _bag->_drawCycleVolumeInfusion
                         << " returnVI=" << _bag->_returnCycleVolumeInfusion
                         << " valveIO=" << _bag->_valveIO
                         << " valvePosCtl=" << _bag->_myValve._positionC
                         << " valvePosSfT=" << _bag->_myValve._positionS
                         << endmsg;
   else
      DataLog(log_level) << "No bag" << endmsg;
}

// reset cycle volumes and valve counters
void RBCContainer::ResetCycleVol (char cCycle)
{
   if (_bag != NULL)
      _bag->ResetCycleVol(cCycle);
}

/* FORMAT HASH 77ae00c2c4f401d26ae3437af2421011 */
