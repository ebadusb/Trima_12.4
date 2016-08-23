/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.17  2003/12/04 20:17:39Z  ms10234
 * 6026 6104 - Add PFR values for ac infusion monitoring during plasma rinseback.
 * Revision 1.16  2003/09/09 22:44:35Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.15  2003/07/21 21:18:43Z  td07711
 * IT6157 - no need to shutdown in ResetCycleVol()
 * Revision 1.14  2003/07/21 16:48:04Z  td07711
 * IT6157 - log prefatal as log_level_critical
 * Revision 1.13  2003/07/01 22:33:34Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.12  2003/04/03 21:33:06Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.11  2002/08/07 17:08:13Z  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * Revision 1.10  2002/07/23 16:52:21  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.9  2002/04/15 09:55:49  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.8  2002/04/08 17:45:38Z  pn02526
 * "Align #includes for grepping"
 * Revision 1.7  2002/04/05 16:37:21  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.6  1999/10/03 16:00:00  BS04481
 * In the AKO infusion calculation, use the maximum negative
 * excurions of the return pump instead of the return line prime
 * volume as an offset to the return volume.
 * Revision 1.5  1999/09/24 22:49:38  BS04481
 * While examing another problem (IT 4328), I noticed that the new
 * changes to the drivers IIT 4071) apparently are more precise at
 * the transitions and the AKO monitor was calculating the infusion rate
 * when the quantity (return-inlet) was actually negative.  Change the
 * code to look for a positive quantity here in addition to looking for
 * the return pump running during a draw.   We weren't in danger
 * of an incorrect safety failure but it looked weird.
 * Revision 1.4  1999/08/20 00:22:36  BS04481
 * Need to subtract off the return line prime while testing for the AC
 * bolus limit in first draw.  While I was in there, changed the label
 * of the accumulator to make operation clearer
 * Revision 1.3  1999/06/25 17:09:45  BS04481
 * Corrections for AC ratio and AC ratio use during Blood Prime and
 * First cycle of Blood Run.  1) Correct formula used to calculate
 * AC ratio during first cycle by removing 5 ml bolus from the
 * numerator.  2) For purpose of determining if there is anti-coagulated
 * blood in the needle, allow ratio to go below 5.  3) For AKO during
 * Blood Prime, enforce 20ml limit on return volume because we don't
 * really know the AC ratio during Blood Prime so just enforce the
 * bolus.
 * Revision 1.2  1998/11/05 19:28:36  bs04481
 * Correct usage of ASSERT.  Code review change.
 * Revision 1.1  1998/05/12 19:49:25  bs04481
 * Initial revision
 *
 *
 * TITLE:      acinfusa.cpp, the AC infusion rate during Access
 *             Keep Open operations monitoring object.
 *
 * ABSTRACT:   Safety's monitoring for AC infusion rate during Access
 *             Keep Open operations.  Monitors for the following situations:
 *                Volume returned during AKO in the first draw is greater
 *                   than the volume primed in the return line
 *                AC Infusion rate during AKO exceeds 1.6 ml/min per liter TBV
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "trima_datalog.h"
#include "trima_assert.h"
#include "alarms.h"

#include "chw.hpp"

#include "s_exec.hpp"
#include "acinfus.hpp"
#include "s_donor.hpp"
#include "s_pfr.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ACInfusionAKO Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety AC Infusion rate monitoring during AKO constructor
//
// ERROR HANDLING:   none.

ACInfusionAKO::ACInfusionAKO(InletContainer* InletCont,
                             ReturnContainer* ReturnCont,
                             aDonor*   Donor)
   : _Inlet(InletCont), _Return(ReturnCont), _Donor(Donor)
{
   _faccumAKOVolToDonor          = 0.0f;
   _flastReturnToDonorDuringDraw = 0.0f;
   clock_gettime(CLOCK_REALTIME, &_AKODrawCycleTimer);
   _bACInfusionMonitoringActive  = FALSE;
   _blimitReturnVolumeOnly       = FALSE;
   _monType = MON_INACTIVE;
   _dumpFlag                     = TRUE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_Donor);

   SaveToPFR(); // parasoft-suppress PB-26 "Object is fully constructed"
}

// SPECIFICATION:    Safety AC Infusion rate monitoring during AKO destructor
//
// ERROR HANDLING:   none.

ACInfusionAKO::~ACInfusionAKO()
{
   _Inlet  = NULL;
   _Return = NULL;
   _Donor  = NULL;
}

// SPECIFICATION:    Initialize AC infusion rate during AKO monitoring for appropriate state
//
// ERROR HANDLING:   none

void ACInfusionAKO::InitializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
         _bACInfusionMonitoringActive = TRUE;
         _blimitReturnVolumeOnly      = TRUE;
         DataLog(log_level_safe_exec_info) << "Safety activating AKO Infusion monitoring" << endmsg;
         break;
      case BLOOD_RUN :
         _bACInfusionMonitoringActive = TRUE;
         _blimitReturnVolumeOnly      = FALSE;
         DataLog(log_level_safe_exec_info) << "Safety activating AKO Infusion monitoring for blood run" << endmsg;
         break;
      case BLOOD_RINSEBACK :
      case DONOR_DISCONNECT :
      case POST_RUN :
         _bACInfusionMonitoringActive = FALSE;
         DataLog(log_level_safe_exec_info) << "Safety removing AKO Infusion monitoring" << endmsg;
         break;
      default :
         break;

   }
   SaveToPFR();
}


// SPECIFICATION:    Calculate and test AC Infusion Rate during Access Keep Open.
//                   For AKO, AC infusion rate is calculated using the volume
//                   returned during the current draw divided by the time in this
//                   draw cycle.
//
// ERROR HANDLING:   AC Infusion High alarm

void ACInfusionAKO::Update (char cCycle, float avgACRatio, float _fReturnVolumeAwayFromDonor)
{
   float drawCycleReturn, drawCycleReturnAdjusted;
   float fdeltaACToDonor, fAKOReturnCycleTime;
   float fAKOACInfusionPerTBV;
   float TBV_liters = _Donor->GetTBV_liters();

   // if AC infusion rate monitoring is not active,
   // just return
   if ( (_bACInfusionMonitoringActive == FALSE)
        ||(_monType == MON_INACTIVE)
        ||(_monType == MON_RINSEBACK)
        ||(cCycle != DRAW_CYCLE ) )
   {
      SaveToPFR();
      return;
   }

   //
   //
   //   We should never be monitoring infusion if we don't know where
   //   the volumes are going (IT9874)
   //

   if ( (_bACInfusionMonitoringActive == TRUE) )
   {
      SET_TYPE set = SafetyExec::instance()->SetType();
      if ((set == SET_NOT_LOADED) || (set == SET_UNKNOWN))
      {
         DataLog(log_level_safety_alarm_detail) << "Safety shutdown for infusion monitoring with invalid set type " << set << endmsg;
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, SW_FAULT);
      }
   }

   // get the volume returned during this draw cycle
   drawCycleReturn = _Return->GetVolume(VOL_DRAW_CYCLE);

   // If this is during first draw, the return pump volume will be negative.
   // Subtract off the volume that was used to prime the return line to determine
   // how far it has run toward the donor (positive direction).
   if (_monType == MON_FIRST_DRAW)
   {
      drawCycleReturnAdjusted = drawCycleReturn - _fReturnVolumeAwayFromDonor;

      // during Blood Prime, AC ratio is meaningless because we don't know how much
      // AC is in the set.  just limit the return volume to less than 20ml under
      // the assumption that it is pure AC
      if (_blimitReturnVolumeOnly == TRUE)
      {
         if (drawCycleReturnAdjusted >= AC_BOLUS_SPEC)
         {
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_INFUSION_HIGH_ALARM);
            if (_dumpFlag == TRUE)
            {
               _dumpFlag = FALSE;
               Dump(log_level_safety_alarm_detail);
            }
            DataLog(log_level_safety_alarm_detail) << "Too much volume returned during first draw.  Returned Vol " << drawCycleReturnAdjusted << endmsg;
         }
      }

      // if the adjusted return volume during the first draw ever goes negative,
      // we have returned the entire return line prime to the donor during the first
      // draw.  this is a bad thing because we are only sure of the average AC ratio
      // for the volume in the return line.  Alarm if we have done this many AKO
      // operations in the first draw.
      if ( (drawCycleReturnAdjusted > (fabs(_fReturnVolumeAwayFromDonor) ) )
           &&(fabs(_fReturnVolumeAwayFromDonor) != 0.0f) )
      {
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_INFUSION_HIGH_ALARM);
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_safety_alarm_detail);
         }
         DataLog(log_level_safety_alarm_detail) << "Too many Access Keep Open cycles during first draw.  Returned Vol " << drawCycleReturnAdjusted << endmsg;
      }
   }
   else
      drawCycleReturnAdjusted = drawCycleReturn;

   // if we have returned volume in this cycle that is a sufficient amount
   // to worry about.
   if ( (drawCycleReturnAdjusted != _flastReturnToDonorDuringDraw)
        &&(drawCycleReturnAdjusted >= AKO_TRIGGER_VOLUME) )
   {
      // ensure that we have an average AC ratio
      if (avgACRatio == 0.f)
      {
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_critical);
         }
         _FATAL_ERROR(__FILE__, __LINE__, "Average AC ratio causes divide by zero");
         return; // to satisfy parasoft/lint
      }

      // ensure that we have a donor
      if (TBV_liters == 0.0f)
      {
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_critical);
         }
         _FATAL_ERROR(__FILE__, __LINE__, "Donor TBV causes divide by zero");
         return; // to satisfy parasoft/lint
      }

      // we are moving return volume, get the time we have been
      // in the AKO operation
      fAKOReturnCycleTime           = calc_elapsed_time(&_AKODrawCycleTimer);
      _flastReturnToDonorDuringDraw = drawCycleReturnAdjusted;

      // calculate AC we have returned in this cycle
      _faccumAKOVolToDonor += (_Return->GetVolume(VOL_DELTA)
                               - _Inlet->GetVolume(VOL_DELTA) );
      fdeltaACToDonor       = _faccumAKOVolToDonor / avgACRatio;

      // calculate the infusion rate if we actually have any flow toward the donor
      if ( (fAKOReturnCycleTime >= AKO_MIN_CYCLE_TIME)
           &&(fdeltaACToDonor > 0) )
      {
         fAKOACInfusionPerTBV = (fdeltaACToDonor / fAKOReturnCycleTime * 60.0)
                                / (TBV_liters);

         // debug
         DataLog(log_level_safe_exec_info) << "AKO Infusion: "
                                           << "Rate: " << fAKOACInfusionPerTBV
                                           << ", Accum Vol: " << _faccumAKOVolToDonor
                                           << ", R Time: " << fAKOReturnCycleTime
                                           << endmsg;

         if ( fAKOACInfusionPerTBV > MAX_ALLOWABLE_AC_INFUSION_SINGLE_CYCLE )
         {
            // AC infusion shutdown
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_INFUSION_HIGH_ALARM);
            if (_dumpFlag == TRUE)
            {
               _dumpFlag = FALSE;
               Dump(log_level_safety_alarm_detail);
            }
         }
      }
   }

   SaveToPFR();
   return;
}

// SPECIFICATION:    AKO AC infusion monitoring method to reset
//                   volumes at the start of the draw cycle
//
// ERROR HANDLING:   none.

void ACInfusionAKO::ResetCycleVol (char cCycle)
{

   if (cCycle == DRAW_CYCLE)
   {
      _faccumAKOVolToDonor          = 0.0f;
      _flastReturnToDonorDuringDraw = 0.0f;
      clock_gettime(CLOCK_REALTIME, &_AKODrawCycleTimer);
   }
   SaveToPFR();
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.

void ACInfusionAKO::Dump (DataLog_Level& log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ACInfusionAKO::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_faccumAKOVolToDonor                  = _faccumAKOVolToDonor;
   SafetyPFRDataToSave.PFR_flastReturnToDonorDuringDraw         = _flastReturnToDonorDuringDraw;
   SafetyPFRDataToSave.PFR_AKODrawCycleTimer                    = _AKODrawCycleTimer;
   SafetyPFRDataToSave.PFR_acInfusA_bACInfusionMonitoringActive = _bACInfusionMonitoringActive;
   SafetyPFRDataToSave.PFR_acInfusA_blimitReturnVolumeOnly      = _blimitReturnVolumeOnly;
   SafetyPFRDataToSave.PFR_acInfusA_monType                     = _monType;
}

// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ACInfusionAKO::RestoreFromPFR ()
{
   _faccumAKOVolToDonor          = SafetyPFRDataFromRestore.PFR_faccumAKOVolToDonor;
   _flastReturnToDonorDuringDraw = SafetyPFRDataFromRestore.PFR_flastReturnToDonorDuringDraw;
   _AKODrawCycleTimer            = SafetyPFRDataFromRestore.PFR_AKODrawCycleTimer;
   _bACInfusionMonitoringActive  = SafetyPFRDataFromRestore.PFR_acInfusA_bACInfusionMonitoringActive;
   _blimitReturnVolumeOnly       = SafetyPFRDataFromRestore.PFR_acInfusA_blimitReturnVolumeOnly;
   _monType = SafetyPFRDataFromRestore.PFR_acInfusA_monType;

   DataLog(log_level_safe_exec_info) << "ACInfusionAKO::RestoreFromPFR(), "
                                     << "faccumAKOVolToDonor: " << _faccumAKOVolToDonor
                                     << " flastReturnToDonorDuringDraw: " << _flastReturnToDonorDuringDraw
                                     << " AKODrawCycleTimer: " << _AKODrawCycleTimer.tv_sec << "." << _AKODrawCycleTimer.tv_nsec
                                     << " bACInfusionMonitoringActive: " << _bACInfusionMonitoringActive
                                     << " blimitReturnVolumeOnly: " << _blimitReturnVolumeOnly
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH 4d66eee5c8c0faa709a259a0c9af30fb */
