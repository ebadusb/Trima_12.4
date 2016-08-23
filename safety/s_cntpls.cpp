/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      s_plscnt.cpp, an Plasma container
 *
 * ABSTRACT:   Safety's view of an Plasma container.  A Plasma container has a
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

#define PLASMA_BAG_PROCEDURE_MAX -1000.0   // procedure's limit for plasma volume

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  PlasmaContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety PlasmaContainer constructor
//
// ERROR HANDLING:   none.

PlasmaContainer::PlasmaContainer()
   : DefaultContainer()
{
   _bag = new bag(PLASMA_PUMP);
   _bag->_myValve.MakeItReal("Plasma Valve", PLASMA_OUT_OF_POSITION);

   SetName("Plasma Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(DONOR_HYPOVOLEMIA_ALARM);

   SaveToPFR();
}


// SPECIFICATION:    Safety PlasmaContainer destructor
//
// ERROR HANDLING:   none.

PlasmaContainer::~PlasmaContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety PlasmaContainer initialize for state transitions
//                   This container initializes the Plasma pump by convention
//
// ERROR HANDLING:   none.

void PlasmaContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   // Initialize the valve for the new state
   _bag->_myValve.InitializeForState(newState);

   SaveToPFR();
}

// SPECIFICATION:    Safety PlasmaContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void PlasmaContainer::SetSetType (SET_TYPE setType)
{
   float maxVol;

   // set the flow direction in the bag based on the set type
   // set type is used in the pump to determine stroke volume
   _bag->SetFlowIO(Set[(int)setType].PlasmaBagFlow);
   _bag->SetValveIO(Set[(int)setType].PlasmaValveFlowAllowed);

   // if we are collecting with this set, set the bag limits
   if ( (Set[(int)setType].PlasmaBagFlow == FLOW_INTO_BAG) ||
        (Set[(int)setType].PlasmaBagFlow == FLOW_INTO_BAG_RC) )
   {
      _bag->SetMaxVol(PLASMA_BAG_PROCEDURE_MAX * MAX_BAG_MULTIPLIER);
   }

   DataLog(log_level_safe_exec_info) << _containerName << " FlowIO: " << _bag->_flowIO
                                     << " ValveIO: " << _bag->_valveIO;

   if (_bag->GetMaxVol(maxVol))
      DataLog(log_level_safe_exec_info) << " VolLimit: " << maxVol;

   DataLog(log_level_safe_exec_info) << endmsg;

   SaveToPFR();
}

// SPECIFICATION:    Safety PlasmaContainer update from control status
//
// ERROR HANDLING:   none.

void PlasmaContainer::UpdateContainerControl (PumpStatus PlasmaPump, timespec timestamp, HW_VALVE_STATES positionC)
{
   // Update valve from control
   _bag->_myValve.UpdateControlValve(positionC);

   SaveToPFR();
}

// SPECIFICATION:    Safety PlasmaContainer update from safety status
//
// ERROR HANDLING:   none.

void PlasmaContainer::UpdateContainerSafety (long PlasmaHalls, HW_VALVE_STATES positionS)
{
   // Update valve from safety
   _bag->_myValve.UpdateSafetyValve(positionS);

   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void PlasmaContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRPlasma_pumpErrorLogFlag = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRPlasma_monType          = _monType;

   // bag data
   SafetyPFRDataToSave.PFRPlasma_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRPlasma_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRPlasma_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRPlasma_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRPlasma_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRPlasma_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRPlasma_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRPlasma_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRPlasma_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRPlasma_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRPlasma_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRPlasma_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRPlasma_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRPlasma_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;

   updateValveToPFR();
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void PlasmaContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag = SafetyPFRDataFromRestore.PFRPlasma_pumpErrorLogFlag;
   _monType          = SafetyPFRDataFromRestore.PFRPlasma_monType;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRPlasma_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRPlasma_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRPlasma_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRPlasma_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRPlasma_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRPlasma_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRPlasma_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRPlasma_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRPlasma_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRPlasma_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRPlasma_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRPlasma_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRPlasma_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRPlasma_returnCycleVolumeInfusion;

   DataLog(log_level_safe_exec_info) << "PlasmaContainer::RestoreFromPFR(), "
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

   DataLog(log_level_safe_exec_info) << "PlasmaContainer::RestoreValveFromPFR(), "
                                     << "positionC: " << _bag->_myValve._positionC
                                     << " positionS: " << _bag->_myValve._positionS
                                     << " iT1Status " << _bag->_myValve._iT1Status
                                     << " bValvePositionMonitorEnabled: " << _bag->_myValve._bValvePositionMonitorEnabled
                                     << endmsg;

   SaveToPFR();
}


void PlasmaContainer::updateValveToPFR ()
{
   SafetyPFRDataToSave.PFRPlasma_positionC                    = _bag->_myValve._positionC;
   SafetyPFRDataToSave.PFRPlasma_positionS                    = _bag->_myValve._positionS;
   SafetyPFRDataToSave.PFRPlasma_iT1Status                    = _bag->_myValve._iT1Status;
   SafetyPFRDataToSave.PFRPlasma_bValvePositionMonitorEnabled = _bag->_myValve._bValvePositionMonitorEnabled;
}

void PlasmaContainer::restoreValveFromPFR ()
{
   _bag->_myValve._positionC                    = SafetyPFRDataToSave.PFRPlasma_positionC;
   _bag->_myValve._positionS                    = SafetyPFRDataToSave.PFRPlasma_positionS;
   _bag->_myValve._iT1Status                    = SafetyPFRDataToSave.PFRPlasma_iT1Status;
   _bag->_myValve._bValvePositionMonitorEnabled = SafetyPFRDataToSave.PFRPlasma_bValvePositionMonitorEnabled;
}

/* FORMAT HASH 03663ea23c1cffeb48898049a0d67f3b */
