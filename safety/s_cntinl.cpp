/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.17.1.2  2009/10/05 16:03:44Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:23Z  dslausb
 * Initial revision
 * Revision 1.17  2009/04/01 19:20:21Z  spriusb
 * IT9100: spiral 3.5 changes to help mitigate return pump halls vs. encoder and improve overall logging
 * Revision 1.16  2008/12/20 02:00:22Z  spriusb
 * Fix for IT 6514: using non-accumulated halls for safety pump checks
 * Revision 1.15  2007/09/19 18:00:36Z  spriusb
 * Fix for IT8210: Various refactors to Container classes in safety model
 * Revision 1.14  2003/09/11 22:19:38Z  ms10234
 * 6406 - update pfr code
 * Revision 1.13  2003/09/09 22:45:04Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.12  2003/07/01 22:34:14Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.11  2003/04/03 23:17:07Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.10  2002/08/07 17:56:04Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.9  2002/07/23 16:52:16  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.8  2002/04/23 17:51:49  pn02526
 * Clean up "eString" definitions left over after DataLogLevel class revisions.
 * Revision 1.7  2002/04/09 19:15:42Z  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.6  2002/03/05 01:02:23Z  ms10234
 * 5613 -  Initialized containers to enable monitoring when the set type is unknown.  Enabled monitoring for AC pump always when the donor is connected.
 * Revision 1.5  1999/08/01 20:53:06  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.4  1999/04/06 18:32:40  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.3  1998/10/26 21:46:50  bs04481
 * Move save of the accumulated cycle error to occur at the time
 * of cycle switch instead of on the next sample.
 * Revision 1.2  1998/06/24 15:19:03  bs04481
 * Changes to make pump stroke volumes dependent on set type.
 * Change necessary for DRBC.
 * Revision 1.1  1998/05/12 19:49:49  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_cntinl.cpp, an Inlet container
 *
 * ABSTRACT:   Safety's view of an Inlet container.  An Inlet container has a bag
 *             and a pump but no valve.  The Inlet container is combined with the
 *             Return container to make a donor.
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_datalog.h"
#include "alarms.h"

#include "s_glob.hpp"
#include "s_cont.hpp"
#include "s_pfr.hpp"


// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  InletContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety InletContainer constructor
//
// ERROR HANDLING:   none.

InletContainer::InletContainer()
   : DefaultContainer()
{

   _bag = new bag(INLET_PUMP);
   _bag->SetFlowIO(FLOW_INTO_SET);
   _bag->SetValveIO(HW_VALVE_COLLECT) ;
   SetName("Inlet Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(NULL_ALARM);

   SaveToPFR();
}


// SPECIFICATION:    Safety InletContainer destructor
//
// ERROR HANDLING:   none.

InletContainer::~InletContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety InletContainer initialize for state transitions
//                   This container initializes the Inlet pump by convention
//
// ERROR HANDLING:   none.

void InletContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   SaveToPFR();
}

// SPECIFICATION:    Safety InletContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void InletContainer::SetSetType (SET_TYPE setType)
{

   // set the flow direction in the bag based on the set type
   // set type is used in the pump to determine stroke volume
   _bag->SetFlowIO(Set[(int)setType].InletFlow);
   _bag->SetValveIO(Set[(int)setType].InletValveFlowAllowed);
   DataLog(log_level_safe_exec_info) << _containerName
                                     << " FlowIO: "
                                     << _bag->_flowIO
                                     << " ValveIO: "
                                     << _bag->_valveIO
                                     << endmsg;

   SaveToPFR();
}

// SPECIFICATION:    Safety InletContainer update from control status
//
// ERROR HANDLING:   none.

void InletContainer::UpdateContainerControl (PumpStatus InletPump, timespec timestamp, short accessPressure)
{
   // save PFR data
   SaveToPFR();
}

// SPECIFICATION:    Safety InletContainer update from safety status
//
// ERROR HANDLING:   none.

void InletContainer::UpdateContainerSafety (long InletHalls)
{
   // save PFR data
   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void InletContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRInlet_pumpErrorLogFlag = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRInlet_monType          = _monType;

   // bag data
   SafetyPFRDataToSave.PFRInlet_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRInlet_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRInlet_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRInlet_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRInlet_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRInlet_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRInlet_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRInlet_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRInlet_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRInlet_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRInlet_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRInlet_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRInlet_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRInlet_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void InletContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag = SafetyPFRDataFromRestore.PFRInlet_pumpErrorLogFlag;
   _monType          = SafetyPFRDataFromRestore.PFRInlet_monType;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRInlet_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRInlet_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRInlet_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRInlet_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRInlet_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRInlet_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRInlet_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRInlet_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRInlet_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRInlet_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRInlet_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRInlet_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRInlet_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRInlet_returnCycleVolumeInfusion;

   // if we were in Rinseback when we went down, we need to reset the delta volume
   // because we will restart Rinseback
   if (_monType == MON_RINSEBACK)
      _bag->_deltaVolume = 0.0f;

   DataLog(log_level_safe_exec_info).precision(2);
   DataLog(log_level_safe_exec_info) << "InletContainer::RestoreFromPFR(), "
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

   SaveToPFR();
}

/* FORMAT HASH 504437b5822918954009ce861789cfa4 */
