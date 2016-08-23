/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_pltcnt.cpp, an Platelet container
 *
 * ABSTRACT:   Safety's view of an Platelet container.  A Platelet container has a
 *             pump, a bag and a valve.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_datalog.h"
#include "alarms.h"

#include "s_glob.hpp"
#include "s_cont.hpp"
#include "s_pfr.hpp"

#define PLATELET_BAG_PROCEDURE_MAX -1200.0   // procedure's limit for platelet volume

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  PlateletContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety PlateletContainer constructor
//
// ERROR HANDLING:   none.

PlateletContainer::PlateletContainer()
   : DefaultContainer()
{
   _bag = new bag(PLATELET_PUMP);
   _bag->_myValve.MakeItReal("Platelet Valve", PLATELET_OUT_OF_POSITION);
   SetName("Platelet Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(DONOR_HYPOVOLEMIA_ALARM);
   SaveToPFR();
}


// SPECIFICATION:    Safety PlateletContainer destructor
//
// ERROR HANDLING:   none.

PlateletContainer::~PlateletContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety PlateletContainer initialize for state transitions
//                   This container initializes the Platelet pump by convention
//
// ERROR HANDLING:   none.

void PlateletContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   // Initialize the valve for the new state
   _bag->_myValve.InitializeForState(newState);

   SaveToPFR();
}

// SPECIFICATION:    Safety PlateletContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void PlateletContainer::SetSetType (SET_TYPE setType)
{
   float maxVol;

   // set the flow direction in the bag based on the set type
   // set type is used in the pump to determine stroke volume
   _bag->_maximumVolumeKnown = false;
   _bag->SetFlowIO(Set[(int)setType].PlateletBagFlow);
   _bag->SetValveIO(Set[(int)setType].PlateletValveFlowAllowed);
   _bag->SetInitialVol(0.0f); // default in case operator switches sets back and forth

   // if we are collecting with this set, set the bag limits
   if ( (Set[(int)setType].PlateletBagFlow == FLOW_INTO_BAG) ||
        (Set[(int)setType].PlateletBagFlow == FLOW_INTO_BAG_RC) )
   {
      _bag->SetMaxVol(PLATELET_BAG_PROCEDURE_MAX * MAX_BAG_MULTIPLIER);
   }
   else
   {
      static const float SALINE_PRIME_VOLUME = 9.0f;

      _bag->SetInitialVol(-1.0f * SALINE_PRIME_VOLUME);  // note this is negative since saline is into the set

      DataLog(log_level_safe_exec_info) << "Safety accounting for saline prime volume of " << SALINE_PRIME_VOLUME << endmsg;
   }

   DataLog(log_level_safe_exec_info) << _containerName << " FlowIO: " << _bag->_flowIO
                                     << " ValveIO: " << _bag->_valveIO;

   if (_bag->GetMaxVol(maxVol))
      DataLog(log_level_safe_exec_info) << " VolLimit: " << maxVol;

   DataLog(log_level_safe_exec_info) << endmsg;

   SaveToPFR();
}

// SPECIFICATION:    Safety PlateletContainer update from control status
//
// ERROR HANDLING:   none.

void PlateletContainer::UpdateContainerControl (PumpStatus PlateletPump, timespec timestamp, HW_VALVE_STATES positionC)
{
   // Update valve from control
   _bag->_myValve.UpdateControlValve(positionC);

   SaveToPFR();
}

// SPECIFICATION:    Safety PlateletContainer update from safety status
//
// ERROR HANDLING:   none.

void PlateletContainer::UpdateContainerSafety (long PlateletHalls, HW_VALVE_STATES positionS)
{
   // Update valve from safety
   _bag->_myValve.UpdateSafetyValve(positionS);

   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void PlateletContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRPlatelet_pumpErrorLogFlag = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRPlatelet_monType          = _monType;

   // bag data
   SafetyPFRDataToSave.PFRPlatelet_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRPlatelet_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRPlatelet_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRPlatelet_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRPlatelet_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRPlatelet_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRPlatelet_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRPlatelet_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRPlatelet_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRPlatelet_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRPlatelet_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRPlatelet_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRPlatelet_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRPlatelet_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;

   updateValveToPFR();
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void PlateletContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag = SafetyPFRDataFromRestore.PFRPlatelet_pumpErrorLogFlag;
   _monType          = SafetyPFRDataFromRestore.PFRPlatelet_monType;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRPlatelet_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRPlatelet_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRPlatelet_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRPlatelet_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRPlatelet_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRPlatelet_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRPlatelet_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRPlatelet_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRPlatelet_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRPlatelet_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRPlatelet_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRPlatelet_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRPlatelet_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRPlatelet_returnCycleVolumeInfusion;

   DataLog(log_level_safe_exec_info) << "PlateletContainer::RestoreFromPFR(), "
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

   DataLog(log_level_safe_exec_info) << "PlateletContainer::RestoreValveFromPFR(), "
                                     << "positionC: " << _bag->_myValve._positionC
                                     << " positionS: " << _bag->_myValve._positionS
                                     << " iT1Status " << _bag->_myValve._iT1Status
                                     << " bValvePositionMonitorEnabled: " << _bag->_myValve._bValvePositionMonitorEnabled
                                     << endmsg;

   SaveToPFR();

}


void PlateletContainer::updateValveToPFR ()
{
   SafetyPFRDataToSave.PFRPlatelet_positionC                    = _bag->_myValve._positionC;
   SafetyPFRDataToSave.PFRPlatelet_positionS                    = _bag->_myValve._positionS;
   SafetyPFRDataToSave.PFRPlatelet_iT1Status                    = _bag->_myValve._iT1Status;
   SafetyPFRDataToSave.PFRPlatelet_bValvePositionMonitorEnabled = _bag->_myValve._bValvePositionMonitorEnabled;
}

void PlateletContainer::restoreValveFromPFR ()
{
   _bag->_myValve._positionC                    = SafetyPFRDataToSave.PFRPlatelet_positionC;
   _bag->_myValve._positionS                    = SafetyPFRDataToSave.PFRPlatelet_positionS;
   _bag->_myValve._iT1Status                    = SafetyPFRDataToSave.PFRPlatelet_iT1Status;
   _bag->_myValve._bValvePositionMonitorEnabled = SafetyPFRDataToSave.PFRPlatelet_bValvePositionMonitorEnabled;
}

/* FORMAT HASH 10d3a543f71bb12db20e49f8ef522aa0 */
