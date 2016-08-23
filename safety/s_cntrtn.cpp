/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.17.1.2  2009/10/05 16:03:59Z  dslausb
 * Shawn's safety code
 * Revision 1.1  2009/10/02 19:12:23Z  dslausb
 * Initial revision
 * Revision 1.17  2009/04/01 19:20:45Z  spriusb
 * IT9100: spiral 3.5 changes to help mitigate return pump halls vs. encoder and improve overall logging
 * Revision 1.16  2008/12/20 02:00:59Z  spriusb
 * Fix for IT 6514: using non-accumulated halls for safety pump checks
 * Revision 1.15  2007/09/19 18:01:30Z  spriusb
 * Fix for IT8210: Various refactors to Container classes in safety model
 * Revision 1.14  2003/09/11 22:19:50Z  ms10234
 * 6406 - update pfr code
 * Revision 1.13  2003/09/09 22:45:16Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.12  2003/07/01 22:34:26Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.11  2003/04/03 23:24:08Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.10  2002/08/07 17:57:35Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.9  2002/07/23 16:52:18  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.8  2002/04/23 17:51:49  pn02526
 * Clean up "eString" definitions left over after DataLogLevel class revisions.
 * Revision 1.7  2002/04/09 19:47:57Z  pn02526
 * "Revise logging to use DataLogLevel class."
 * Revision 1.6  2002/03/05 01:02:26Z  ms10234
 * 5613 -  Initialized containers to enable monitoring when the set type is unknown.  Enabled monitoring for AC pump always when the donor is connected.
 * Revision 1.5  1999/08/01 20:53:12  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.4  1999/04/06 18:32:48  BS04481
 * Remove default of set type to white when unknown.  If set type is
 * not known in states from Donor Connect to Rinseback, inclusive,
 * and any pumps run, alarm.  Default to white if unknown in Donor
 * Disconnect
 * Revision 1.3  1998/10/26 21:46:48  bs04481
 * Move save of the accumulated cycle error to occur at the time
 * of cycle switch instead of on the next sample.
 * Revision 1.2  1998/06/24 15:19:06  bs04481
 * Changes to make pump stroke volumes dependent on set type.
 * Change necessary for DRBC.
 * Revision 1.1  1998/05/12 19:49:55  bs04481
 * Initial revision
 *
 *
 * TITLE:      s_cntrtn.cpp, a Return container
 *
 * ABSTRACT:   Safety's view of a Return container.  A Return container has a
 *             pump and a bag.  The Return container is combined with an Inlet
 *             container to make a Donor.
 *
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
 *  ReturnContainer Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety ReturnContainer constructor
//
// ERROR HANDLING:   none.

ReturnContainer::ReturnContainer()
   : DefaultContainer()
{
   _fReturnVolumeAwayFromDonor = 0.0f;
   _fReturnPrimeVolume         = 0.0f;
   _bag = new bag(RETURN_PUMP);
   _bag->SetFlowIO(FLOW_INTO_SET);
   _bag->SetValveIO(HW_VALVE_COLLECT) ;
   SetName("Return Container");
   SetMinimumAlarm(NULL_ALARM);
   SetMaximumAlarm(NULL_ALARM);

   SaveToPFR();
}


// SPECIFICATION:    Safety ReturnContainer destructor
//
// ERROR HANDLING:   none.

ReturnContainer::~ReturnContainer()
{
   delete _bag;
   _bag = NULL;
}


// SPECIFICATION:    Safety ReturnContainer initialize for state transitions
//                   This container initializes the  Return pump by convention
//
// ERROR HANDLING:   none.

void ReturnContainer::InitializeForState (long newState)
{
   // Initialize the bag volumes for the new state.
   _bag->InitializeForState(newState);

   // We need the maximum excursion away from the donor which occurs
   // at the transition from Blood Prime to Blood Run and we also
   // neet the volume of blood which was used to prime the return line
   switch (newState)
   {
      case BLOOD_PRIME :
         // we'll need this to calculate the volume of blood in the return line
         _fReturnVolumeAwayFromDonor = _bag->_accumulatedVolume;
         break;
      case BLOOD_RUN :
         // we only want to get the prime volumes once per procedure so if they
         // are already set, assume this is a power fail recovery.
         if (_fReturnPrimeVolume == 0.0f)
         {
            // calculate the volume in the return line
            _fReturnPrimeVolume = _bag->_accumulatedVolume
                                  - _fReturnVolumeAwayFromDonor;
            // now capture the maximum excursion also
            _fReturnVolumeAwayFromDonor = _bag->_accumulatedVolume;
         }
         DataLog(log_level_safe_exec_info) << "Prime Volumes: Return line: "
                                           << _fReturnPrimeVolume
                                           << ", Maximum negative: "
                                           << _fReturnVolumeAwayFromDonor
                                           << endmsg;
         break;
      default :
         break;
   }

   SaveToPFR();
}

// SPECIFICATION:    Safety ReturnContainer set for set types
//                   Sets the normal flow direction for the bag
//                   based on the set type
//
// ERROR HANDLING:   none.

void ReturnContainer::SetSetType (SET_TYPE setType)
{
   // set the flow direction in the bag based on the set type
   // set type is used in the pump to determine stroke volume
   // double check the set type
   _bag->SetFlowIO(Set[(int)setType].ReturnFlow);
   _bag->SetValveIO(Set[(int)setType].ReturnValveFlowAllowed);
   DataLog(log_level_safe_exec_info) << _containerName
                                     << " FlowIO: "
                                     << _bag->_flowIO
                                     << " ValveIO: "
                                     << _bag->_valveIO
                                     << endmsg;

   SaveToPFR();
}

// SPECIFICATION:    Safety ReturnContainer update from control status
//
// ERROR HANDLING:   none.

void ReturnContainer::UpdateContainerControl (PumpStatus ReturnPump, timespec timestamp)
{
   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Safety ReturnContainer update from safety status
//
// ERROR HANDLING:   none.

void ReturnContainer::UpdateContainerSafety (long ReturnHalls)
{
   // save to PFR
   SaveToPFR();
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ReturnContainer::SaveToPFR ()
{
   // container data
   SafetyPFRDataToSave.PFRReturn_pumpErrorLogFlag           = _pumpErrorLogFlag;
   SafetyPFRDataToSave.PFRReturn_monType                    = _monType;
   SafetyPFRDataToSave.PFRReturn_fReturnVolumeAwayFromDonor = _fReturnVolumeAwayFromDonor;
   SafetyPFRDataToSave.PFRReturn_fReturnPrimeVolume         = _fReturnPrimeVolume;

   // bag data
   SafetyPFRDataToSave.PFRReturn_initialVolume             = _bag->_initialVolume;
   SafetyPFRDataToSave.PFRReturn_maximumVolume             = _bag->_maximumVolume;
   SafetyPFRDataToSave.PFRReturn_minimumVolume             = _bag->_minimumVolume;
   SafetyPFRDataToSave.PFRReturn_initialVolumeKnown        = _bag->_initialVolumeKnown;
   SafetyPFRDataToSave.PFRReturn_maximumVolumeKnown        = _bag->_maximumVolumeKnown;
   SafetyPFRDataToSave.PFRReturn_minimumVolumeKnown        = _bag->_minimumVolumeKnown;
   SafetyPFRDataToSave.PFRReturn_bagflowIO                 = _bag->_flowIO;
   SafetyPFRDataToSave.PFRReturn_bagvalveIO                = _bag->_valveIO;
   SafetyPFRDataToSave.PFRReturn_accumulatedVolume         = _bag->_accumulatedVolume;
   SafetyPFRDataToSave.PFRReturn_deltaVolume               = _bag->_deltaVolume;
   SafetyPFRDataToSave.PFRReturn_drawCycleVolume           = _bag->_drawCycleVolume;
   SafetyPFRDataToSave.PFRReturn_returnCycleVolume         = _bag->_returnCycleVolume;
   SafetyPFRDataToSave.PFRReturn_drawCycleVolumeInfusion   = _bag->_drawCycleVolumeInfusion;
   SafetyPFRDataToSave.PFRReturn_returnCycleVolumeInfusion = _bag->_returnCycleVolumeInfusion;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ReturnContainer::RestoreFromPFR ()
{
   // container data
   _pumpErrorLogFlag           = SafetyPFRDataFromRestore.PFRReturn_pumpErrorLogFlag;
   _monType                    = SafetyPFRDataFromRestore.PFRReturn_monType;
   _fReturnVolumeAwayFromDonor = SafetyPFRDataFromRestore.PFRReturn_fReturnVolumeAwayFromDonor;
   _fReturnPrimeVolume         = SafetyPFRDataFromRestore.PFRReturn_fReturnPrimeVolume;

   // bag data
   _bag->_initialVolume             = SafetyPFRDataFromRestore.PFRReturn_initialVolume;
   _bag->_maximumVolume             = SafetyPFRDataFromRestore.PFRReturn_maximumVolume;
   _bag->_minimumVolume             = SafetyPFRDataFromRestore.PFRReturn_minimumVolume;
   _bag->_initialVolumeKnown        = SafetyPFRDataFromRestore.PFRReturn_initialVolumeKnown;
   _bag->_maximumVolumeKnown        = SafetyPFRDataFromRestore.PFRReturn_maximumVolumeKnown;
   _bag->_minimumVolumeKnown        = SafetyPFRDataFromRestore.PFRReturn_minimumVolumeKnown;
   _bag->_flowIO                    = SafetyPFRDataFromRestore.PFRReturn_bagflowIO;
   _bag->_valveIO                   = SafetyPFRDataFromRestore.PFRReturn_bagvalveIO;
   _bag->_accumulatedVolume         = SafetyPFRDataFromRestore.PFRReturn_accumulatedVolume;
   _bag->_deltaVolume               = SafetyPFRDataFromRestore.PFRReturn_deltaVolume;
   _bag->_drawCycleVolume           = SafetyPFRDataFromRestore.PFRReturn_drawCycleVolume;
   _bag->_returnCycleVolume         = SafetyPFRDataFromRestore.PFRReturn_returnCycleVolume;
   _bag->_drawCycleVolumeInfusion   = SafetyPFRDataFromRestore.PFRReturn_drawCycleVolumeInfusion;
   _bag->_returnCycleVolumeInfusion = SafetyPFRDataFromRestore.PFRReturn_returnCycleVolumeInfusion;

   // if we were in Rinseback when we went down, we need to reset the delta volume
   // because we will restart Rinseback
   if (_monType == MON_RINSEBACK)
      _bag->_deltaVolume = 0.0f;

   DataLog(log_level_safe_exec_info) << "ReturnContainer::RestoreFromPFR(), "
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

/* FORMAT HASH bdb0dd8dd31dda926cca61de47d263fe */
