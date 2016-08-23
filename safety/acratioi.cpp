/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.10  2003/09/09 22:44:47Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.9  2003/07/21 21:10:41Z  td07711
 * IT6157 - no need to shutdown in ResetCycleVol()
 * Revision 1.8  2003/07/01 22:33:50Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.7  2003/04/03 22:14:27Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.6  2002/08/07 17:25:10Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.5  2002/07/23 16:52:22  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.4  2002/04/15 09:55:50  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.3  2002/04/08 09:45:44  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.2  1998/11/05 12:28:34  bs04481
 * Correct usage of ASSERT.  Code review change.
 * Revision 1.1  1998/05/12 19:49:34  bs04481
 * Initial revision
 *
 *
 * TITLE:      acratioi.cpp, the object which monitors instantaneous
 *             AC ratio
 *
 * ABSTRACT:   Safety's monitoring for instantaneous AC ratio which
 *             ensures that the AC ratio stays within expected
 *             boundaries over a period of time
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "alarms.h"

#include "acratio.hpp"
#include "s_pfr.hpp"
#include "s_donor.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
*
*  ACRatioInstantaneous Methods
*
******************************************************************************/

// SPECIFICATION:    Safety instantaneous AC Ratio monitoring constructor
//
// ERROR HANDLING:   none.

ACRatioInstantaneous::ACRatioInstantaneous(InletContainer* InletCont,
                                           ReturnContainer* ReturnCont,
                                           ACContainer* ACCont)
   : _Inlet(InletCont), _Return(ReturnCont), _AC(ACCont)
{
   _iHighRatioCounter        = 0;
   _iLowRatioCounter         = 0;
   _bACRatioMonitoringActive = FALSE;
   _monType                  = MON_INACTIVE;
   _dumpFlag                 = TRUE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_AC);

   SaveToPFR();
}


// SPECIFICATION:    Safety ionstantaneous AC Ratio monitoring destructor
//
// ERROR HANDLING:   none.

ACRatioInstantaneous::~ACRatioInstantaneous()
{
   _Inlet  = NULL;
   _Return = NULL;
   _AC     = NULL;
}


// SPECIFICATION:    Initialize instantaneous AC Ratio monitoring
//                   for appropriate state
//
// ERROR HANDLING:   none

void ACRatioInstantaneous::InitializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
         _bACRatioMonitoringActive = TRUE;
         break;
      case BLOOD_RINSEBACK :
      case DONOR_DISCONNECT :
         _bACRatioMonitoringActive = FALSE;
         break;
      default :
         break;

   }
   SaveToPFR();
}

// SPECIFICATION:    Test for sustained AC ratio outside of
//                   acceptable limits
//
//
// ERROR HANDLING:   AC Ratio Too Low alarm
//                   AC Ratio Too High alarm

void ACRatioInstantaneous::Update (char cCycle)
{
   float acRatio, deltaAC, deltaInlet, deltaReturn;

   // if AC ratio monitoring is active, do the tests
   if ( (_bACRatioMonitoringActive == TRUE)
        &&(_monType != MON_INACTIVE)
        &&(cCycle == DRAW_CYCLE) )
   {
      // Get the volumes added in this sample period
      deltaAC     = _AC->GetVolume(VOL_DELTA);
      deltaInlet  = _Inlet->GetVolume(VOL_DELTA);
      deltaReturn = _Return->GetVolume(VOL_DELTA);

      if (deltaAC > .0005f)
      {
         // calculate the instantaneous AC ratio
         acRatio = (deltaInlet - deltaReturn) / deltaAC;

         // test against the lower limit and increment counter
         // alarm if counter goes too high
         if ( acRatio < MIN_ALLOWABLE_AC_RATIO )
         {
            if ( ++_iLowRatioCounter > LOW_RATIO_FILTER )
            {
               paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_RATIO_TOO_LOW_ALARM);
               if (_dumpFlag == TRUE)
               {
                  _dumpFlag = FALSE;
                  Dump(log_level_safety_alarm_detail);
               }

               DataLog(log_level_safety_alarm_detail) << "AC_RATIO_TOO_LOW_ALARM Ratio: "
                                                      << acRatio
                                                      << ", Low Count: "
                                                      << _iLowRatioCounter
                                                      << endmsg;
            }
            else
            {
               DataLog(log_level_safe_exec_info) << "deltaAC >= 0.0005  Ratio: "
                                                 << acRatio
                                                 << ", Low Count: "
                                                 << _iLowRatioCounter
                                                 << " deltaAC: "
                                                 << deltaAC
                                                 << ", deltaInlet: "
                                                 << deltaInlet
                                                 << ", deltaReturn:"
                                                 << deltaReturn
                                                 << endmsg;
            }
         }
         else
         {
            if ( _iLowRatioCounter != 0 )
               _iLowRatioCounter--;
         } // end countdown low ratio filter


         // test against the upper limit and increment counter
         // alarm if counter goes too high
         if ( acRatio > MAX_ALLOWABLE_AC_RATIO )
         {
            if ( ++_iHighRatioCounter > HIGH_RATIO_FILTER )
            {
               paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_RATIO_TOO_HIGH_ALARM);
               if (_dumpFlag == TRUE)
               {
                  _dumpFlag = FALSE;
                  Dump(log_level_safety_alarm_detail);
               }

               DataLog(log_level_safety_alarm_detail) << "AC_RATIO_TOO_HIGH_ALARM Ratio: "
                                                      << acRatio
                                                      << ", High Count: "
                                                      << _iHighRatioCounter
                                                      << endmsg;
            }
            else
            {
               // log ratio too high occurance
               DataLog(log_level_safe_exec_info) << "deltaAC >= 0.0005 Ratio: "
                                                 << acRatio
                                                 << ", High Count: "
                                                 << _iHighRatioCounter
                                                 << " deltaAC: "
                                                 << deltaAC
                                                 << ", deltaInlet: "
                                                 << deltaInlet
                                                 << ", deltaReturn: "
                                                 << deltaReturn
                                                 << endmsg;
            }
         }
         else
         {
            if ( _iHighRatioCounter != 0 )
               _iHighRatioCounter--;
         } // end count down high ratio filter

      }
   }

   // save data to PFR
   SaveToPFR();
   return;
}

// SPECIFICATION:    Instantaneous AC Ratio  monitoring method to reset
//                   counters at the start of the draw cycle
//
// ERROR HANDLING:   none.

void ACRatioInstantaneous::ResetCycleVol (char cCycle)
{
   if (cCycle == DRAW_CYCLE)
   {
      _iHighRatioCounter = 0;
      _iLowRatioCounter  = 0;
   }
   SaveToPFR();
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.

void ACRatioInstantaneous::Dump (DataLog_Level log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
}


// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ACRatioInstantaneous::SaveToPFR ()
{

   SafetyPFRDataToSave.PFR_iHighRatioCounter                 = _iHighRatioCounter;
   SafetyPFRDataToSave.PFR_iLowRatioCounter                  = _iLowRatioCounter;
   SafetyPFRDataToSave.PFR_acRatioI_bACRatioMonitoringActive = _bACRatioMonitoringActive;
   SafetyPFRDataToSave.PFR_acRatioI_monType                  = _monType;

}

// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ACRatioInstantaneous::RestoreFromPFR ()
{

   _iHighRatioCounter        = SafetyPFRDataFromRestore.PFR_iHighRatioCounter;
   _iLowRatioCounter         = SafetyPFRDataFromRestore.PFR_iLowRatioCounter;
   _bACRatioMonitoringActive = SafetyPFRDataFromRestore.PFR_acRatioI_bACRatioMonitoringActive;
   _monType                  = SafetyPFRDataFromRestore.PFR_acRatioI_monType;

   DataLog(log_level_safe_exec_info) << "ACRatioInstantaneous::RestoreFromPFR(), "
                                     << "iHighRatioCounter: " << _iHighRatioCounter
                                     << " iLowRatioCounter: " << _iLowRatioCounter
                                     << " bACRatioMonitoringActive: " << _bACRatioMonitoringActive
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH f2a687cef5b2e7a7f37f8c95f025aff4 */
