/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.17.1.2  2009/10/05 16:03:41Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:19Z  dslausb
 * Initial revision
 * Revision 1.17  2009/04/01 19:19:55Z  spriusb
 * IT9100: spiral 3.5 changes to help mitigate return pump halls vs. encoder and improve overall logging
 * Revision 1.16  2008/12/20 02:00:10Z  spriusb
 * Fix for IT 6514: using non-accumulated halls for safety pump checks
 * Revision 1.15  2007/09/19 18:00:26Z  spriusb
 * Fix for IT8210: Various refactors to Container classes in safety model
 * Revision 1.14  2003/09/11 22:19:34Z  ms10234
 * 6406 - update pfr code
 * Revision 1.13  2003/09/09 22:45:01Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.12  2003/07/01 22:34:11Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.11  2003/04/03 23:15:40Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.10  2002/08/07 17:55:42Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.9  2002/07/23 16:52:16  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.8  2002/04/23 17:51:48  pn02526
 * Clean up "eString" definitions left over after DataLogLevel class revisions.
 * Revision 1.7  2002/04/09 19:10:59Z  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.6  2002/03/05 01:02:20Z  ms10234
 * 5613 -  Initialized containers to enable monitoring when the set type is unknown.  Enabled monitoring for AC pump always when the donor is connected.
 * Revision 1.5  2000/06/22 16:46:36  bs04481
 * Plumbing changes for single-stage.  1) Change to RBC flow.
 * 2) Add valve position monitoring during Blood Run and Blood
 * Rinseback. 3)  Add compile flag to support dual-stage
 * Revision 1.4  1999/08/01 20:53:04  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.3  1999/04/06 18:32:39  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.2  1998/06/24 15:19:02  bs04481
 * Changes to make pump stroke volumes dependent on set type.
 * Change necessary for DRBC.
 * Revision 1.1  1998/05/12 19:49:47  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_accnt.cpp, an AC container
 *
 * ABSTRACT:   Safety's view of an AC container.  An AC container has a
 *             pump and a bag but no valve.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_datalog.h"
#include "alarms.h"

#include "s_glob.hpp"
#include "s_set.hpp"
#include "s_cont.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ACContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety ACContainer constructor
//
// ERROR HANDLING:   none.

ACContainer::ACContainer()
   :  DefaultContainer()
{
   _fACPrimeVolume = 0.0f;
   _bag            = new bag(AC_PUMP);
   _bag->SetFlowIO(FLOW_INTO_SET);
   _bag->SetValveIO(HW_VALVE_COLLECT) ;
   SetName("AC Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(NULL_ALARM);

   SaveToPFR();
}


// SPECIFICATION:    Safety ACContainer destructor
//
// ERROR HANDLING:   none.

ACContainer::~ACContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety ACContainer initialize for state transitions
//                   This container initializes the AC pump by convention
//
// ERROR HANDLING:   none.

void ACContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   // At the transition to Donor Connected, we need to capture
   // the amount of AC used to prime the set.
   switch (newState)
   {
      case DONOR_CONNECTED :
         // we'll need this to calculate the average AC ratio
         _fACPrimeVolume = _bag->_accumulatedVolume;
         break;
      default :
         break;
   }

   SaveToPFR();
}

// SPECIFICATION:    Safety ACContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void ACContainer::SetSetType (SET_TYPE setType)
{
   // set the flow direction in the bag based on the set type
   // set type is used in the pump to determine stroke volume
   _bag->SetFlowIO(Set[(int)setType].ACBagFlow);
   _bag->SetValveIO(Set[(int)setType].ACValveFlowAllowed);
   DataLog(log_level_safe_exec_info) << _containerName
                                     << " FlowIO: "
                                     << _bag->_flowIO
                                     << " ValveIO: "
                                     << _bag->_valveIO
                                     << endmsg;

   SaveToPFR();
}

// SPECIFICATION:    Safety ACContainer update from control status
//
// ERROR HANDLING:   none.

void ACContainer::UpdateContainerControl (PumpStatus ACPump, timespec timestamp)
{
   // save PFR data
   SaveToPFR();
}

// SPECIFICATION:    Safety ACContainer update from safety status
//
// ERROR HANDLING:   none.

void ACContainer::UpdateContainerSafety (long ACHalls)
{
   // save PFR data
   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ACContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRAC_pumpErrorLogFlag = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRAC_monType          = _monType;
   SafetyPFRDataToSave.PFRAC_fACPrimeVolume   = _fACPrimeVolume;

   // bag data
   SafetyPFRDataToSave.PFRAC_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRAC_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRAC_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRAC_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRAC_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRAC_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRAC_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRAC_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRAC_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRAC_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRAC_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRAC_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRAC_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRAC_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ACContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag = SafetyPFRDataFromRestore.PFRAC_pumpErrorLogFlag;
   _monType          = SafetyPFRDataFromRestore.PFRAC_monType;
   _fACPrimeVolume   = SafetyPFRDataFromRestore.PFRAC_fACPrimeVolume;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRAC_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRAC_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRAC_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRAC_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRAC_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRAC_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRAC_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRAC_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRAC_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRAC_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRAC_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRAC_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRAC_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRAC_returnCycleVolumeInfusion;

   DataLog(log_level_safe_exec_info) << "ACContainer::RestoreFromPFR(), "
                                     << "pumpErrorLogFlag: " << _pumpErrorLogFlag
                                     << " monType: " << (MON_TYPE)_monType
                                     << " fACPrimeVolume: " << _fACPrimeVolume
                                     << " Bag data... initialVolume: " << _bag->_initialVolume
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


DataLog_Stream& operator << (DataLog_Stream& os, FLOW_DIRECTION flowIO)
{
   switch (flowIO)
   {
      case FLOW_UNKNOWN :               os << "FLOW_UNKNOWN";               break;
      case FLOW_INTO_SET :              os << "FLOW_INTO_SET";              break;
      case FLOW_INTO_BAG :              os << "FLOW_INTO_BAG";              break;
      case FLOW_INTO_BAG_RC :           os << "FLOW_INTO_BAG_RC";           break;
      case FLOW_ERROR :                 os << "FLOW_ERROR";                 break;

      default :                         os << "FLOW_UNKNOWN";               break;
   }
   return os;
}


DataLog_Stream& operator << (DataLog_Stream& os, HW_VALVE_STATES valveIO)
{
   switch (valveIO)
   {
      case HW_VALVE_UNKNOWN :           os << "HW_VALVE_UNKNOWN";           break;
      case HW_VALVE_COLLECT :           os << "HW_VALVE_COLLECT";           break;
      case HW_VALVE_OPEN :              os << "HW_VALVE_OPEN";              break;
      case HW_VALVE_RETURN :            os << "HW_VALVE_RETURN";            break;

      default :                         os << "HW_VALVE_ERROR";             break;
   }
   return os;
}


DataLog_Stream& operator << (DataLog_Stream& os, HW_CASSETTE_STATES position)
{
   switch (position)
   {
      case HW_CASSETTE_UNKNOWN :        os << "HW_CASSETTE_UNKNOWN";            break;
      case HW_CASSETTE_DOWN :           os << "HW_CASSETTE_DOWN";               break;
      case HW_CASSETTE_UP :             os << "HW_CASSETTE_UP";                 break;

      default :                         os << "HW_CASSETTE_ERROR";              break;
   }
   return os;
}

/* FORMAT HASH 174e79c09f32af89fd7eff3e18cdc5ec */
