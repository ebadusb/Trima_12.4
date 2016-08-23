/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.31  2003/09/09 22:44:44Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.30  2003/07/21 22:23:29Z  td07711
 * IT6157 - no need to shutdown in ResetCycleVol()
 * Revision 1.29  2003/07/21 21:03:44Z  td07711
 * IT6157 - logging change
 * Revision 1.28  2003/07/01 22:33:47Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.27  2003/04/03 22:07:52Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.26  2002/12/26 23:10:42Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.25  2002/08/07 11:23:42  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * To keep GNU compiler happy for production builds:
 * + Initialize "bLowRatioUsable = FALSE" in ACRatioAverage :: TestACLowRatioVolume().
 * + Remove unused "bCycleValid" variable from ACRatioAverage :: UpdateAverageRatio().
 * Revision 1.24  2002/07/23 16:52:22  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.23  2002/04/15 09:55:50  pn02526
 * Change log(LOG_ALARM_DETAIL) to alarm_detail(LOG_ALARM_DETAIL)
 * Revision 1.22  2002/04/08 15:51:35  pn02526
 * "Put allocation of DataLogLevel first in code block."
 * Revision 1.21  2002/04/05 16:37:23  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.20  2001/04/23 19:58:05Z  ms10234
 * 5080 -  Moved the running average ratio calculation to a point after the
 * volume accumulation/ratio alarm checks have been done.
 * Revision 1.19  2001/04/18 18:35:16  ms10234
 * 5076 - Changed the acratio averaging to start on the second cycle.
 * Revision 1.18  2001/04/11 15:08:01  ms10234
 * IT5058 -  Added ratio averaging.  When the ratio goes lower, the new lower
 * value is used for the ratio.  When the ratio goes higher, the higher ratio is
 * averaged with the old, lower ratio to give the new ratio.
 * Revision 1.17  2000/10/27 14:19:14  bs04481
 * Correct error in accumulator for inlet volume when AC ratio exceeds
 * 15.  Because we now call this function every time we pause, we
 * were accumulating the volume more than once per cycle.  Also,
 * change the name of the variable that controls allowing AC ratios to
 * fall below 5 to make the code more readable.
 * Revision 1.16  2000/07/11 23:48:17  bs04481
 * Extend pause monitoring back to Blood Prime
 * Revision 1.15  2000/07/10 15:23:00  bs04481
 * Don't log first draw AC ratio calculations if it is being requested
 * due to a pump stop.  It just fills the log unnecessarity.
 * Revision 1.14  2000/05/26 17:05:36  BS04481
 * Change alarm message sender to repeat alarm to Procedure.
 * Revision 1.13  2000/03/15 18:01:10  BS04481
 * Only log valid ratio when it changes instead of on each sample
 * Revision 1.12  1999/08/06 15:55:07  BS04481
 * Remove code review change to average AC ratio calculation.  It
 * messes up saline bolus.
 * Revision 1.11  1999/08/01 20:52:56  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.10  1999/06/25 17:09:51  BS04481
 * Corrections for AC ratio and AC ratio use during Blood Prime and
 * First cycle of Blood Run.  1) Correct formula used to calculate
 * AC ratio during first cycle by removing 5 ml bolus from the
 * numerator.  2) For purpose of determining if there is anti-coagulated
 * blood in the needle, allow ratio to go below 5.  3) For AKO during
 * Blood Prime, enforce 20ml limit on return volume because we don't
 * really know the AC ratio during Blood Prime so just enforce the
 * bolus.
 * Revision 1.9  1998/11/05 19:28:34  bs04481
 * Correct usage of ASSERT.  Code review change.
 * Revision 1.8  1998/10/21 23:18:55  bs04481
 * Return an AC ratio during disconnect even though it is stale.
 * Revision 1.7  1998/07/31 20:09:40  bs04481
 * Return the average AC ratio and mark it valid if we are in
 * Rinseback.   Even though we haven't calculated it for a while, it
 * is still good because we have only been circulating the blood and
 * not pumping anything into the set.
 * Revision 1.6  1998/07/30 01:33:00  bs04481
 * Save PFR data when monitor enable switch is taken.
 * Revision 1.5  1998/07/23 23:47:56  bs04481
 * While calculating the AC ratio, remove the AC prime volume from
 * the calculation during the First Draw and First Return instead of
 * only during First Draw.
 * Revision 1.4  1998/06/09 17:46:50  bs04481
 * Correct error make during safety architecture conversion.
 * The accumulated volumes for high and low AC ratio deliveries
 * should not be zeroed at the draw or return transitions.
 * Revision 1.3  1998/06/01 13:46:10  bs04481
 * Correct AC ratio calculation to look at the draw cycle volume of the
 * return pump during first draw instead of the return cycle volume.
 * This was an error introduced in the conversion.
 * Revision 1.2  1998/05/18 17:47:23  bs04481
 * Change to logging only.
 * Revision 1.1  1998/05/12 19:49:32  bs04481
 * Initial revision
 *
 *
 * TITLE:      acratioa.cpp, the average AC Ratio monitoring object
 *
 * ABSTRACT:   Safety's monitoring for average AC ratio.
 *
 */


#include <stdio.h>
#include <stdlib.h>

#include "trima_assert.h"
#include "alarms.h"

#include "acratio.hpp"
#include "s_pfr.hpp"
#include "s_donor.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ACRatioAverage Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety Average AC Ratio monitoring constructor
//
// ERROR HANDLING:   none.

ACRatioAverage::ACRatioAverage(InletContainer* InletCont,
                               ReturnContainer* ReturnCont,
                               ACContainer* ACCont)
   : _Inlet(InletCont), _Return(ReturnCont), _AC(ACCont)
{
   _fAccumInletVolumeAtHighRatio = 0.0f;
   _fAccumACVolumeAtLowRatio     = 0.0f;
   _bUseableACRatio              = FALSE;
   _bACRatioMonitoringActive     = FALSE;
   _monType = MON_INACTIVE;
   _dumpFlag                     = TRUE;

   _fAvgACRatio                  = 0.0f;
   _fLastKnownGoodRatio          = 0.0f;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_AC);

   SaveToPFR();  // parasoft-suppress PB-26 "Object is fully constructed"
}

// SPECIFICATION:    Safety Average AC Ratio monitoring destructor
//
// ERROR HANDLING:   none.

ACRatioAverage::~ACRatioAverage()
{
   _Inlet  = NULL;
   _Return = NULL;
   _AC     = NULL;
}

// SPECIFICATION:    Initialize AC Ratio monitoring for appropriate state
//
// ERROR HANDLING:   none

void ACRatioAverage::InitializeForState (long newState)
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



// SPECIFICATION:    Return Safety's Average AC Ratio
//
// ERROR HANDLING:   none.

float ACRatioAverage::GetAverageACRatio (bool& valid)
{
   valid = _bUseableACRatio;
   return(_fAvgACRatio);
}


// SPECIFICATION: Update average AC ratio .
//
//
// ABSTRACT:   Calculates the average AC ratio.  Note the calculation is
//             different for first cycle or anytime the return volume
//             is negative.
//             If the ratio is invalid, the boolean bUseableACRatio is
//             set to false.
//
//             If lowRatioAllowed is TRUE, the function will not alarm for
//             ratios lower than 5.  This is used when testing the ratio for
//             determing if the needle is anti-coagulated.
//             If accumulateVolumeAtHighRatio is TRUE, the function will
//             test for volumes exceeding the allowed at high ratio.
//
// NOTES :
// 07/28/97 msm
// see PPQA memo Ratio Robustness and Short Cycles
//
void ACRatioAverage::UpdateAverageRatio (bool testLowRatio, bool accumulateVolumeAtHighRatio)
{
   float favgRatio_temp = 0.f;
//   bool   bCycleValid      = TRUE;

   // if this is rinseback or disconnect, we aren't calculating the ratio
   // any longer but let them have it if they ask.  they might ask if somebody
   // a stop or pause or if we do a PFR in rinseback and for infusion
   // monitoring in rinseback and disconnect.
   if ( (_monType == MON_RINSEBACK)
        ||(_monType == MON_DISCONNECT) )
   {
      _bUseableACRatio = TRUE;
      return;
   }

   // if AC ratio monitoring is not active, just return
   if ( (_bACRatioMonitoringActive == FALSE)
        ||(_monType == MON_INACTIVE) )
   {
      _bUseableACRatio = FALSE;
      SaveToPFR();
      return;
   }

   // determine average AC Ratio for this cycle
   // Ratio calculation for first draw differs from run cycles
   // because of the need to remove ac prime volume from ac denominator.
   //
   if ( ( _monType == MON_FIRST_DRAW )
        ||( _monType == MON_FIRST_DRAW_COMPLETE )
        ||(_fAvgACRatio == 0.f ) )
   {
      if (_Return->GetVolume(VOL_DRAW_CYCLE) >= 0.0f)
      {
         favgRatio_temp = (_Inlet->GetVolume(VOL_DRAW_CYCLE)
                           - (_AC->_fACPrimeVolume - AC_PRIME_OFFSET) )
                          / (_AC->GetVolume(VOL_DRAW_CYCLE)
                             - (_AC->_fACPrimeVolume - AC_PRIME_OFFSET) );
      }
      else
      {
         favgRatio_temp = (_Inlet->GetVolume(VOL_DRAW_CYCLE) - _Return->GetVolume(VOL_DRAW_CYCLE)
                           - _AC->_fACPrimeVolume)
                          / (_AC->GetVolume(VOL_DRAW_CYCLE)
                             - (_AC->_fACPrimeVolume - AC_PRIME_OFFSET) );
      }

      // nominal logging
      if (testLowRatio == TRUE)
      {
         // don't log if this is just a calculation for ac ratio during pumps paused.
         // it just fills the logs unnecessarily and you'll see the ratio in the logging
         // for pumps paused anyway
         DataLog(log_level_safe_exec_info) << "First Draw Ratio: "
                                           << favgRatio_temp
                                           << " ACDrawVol: "
                                           << _AC->GetVolume(VOL_DRAW_CYCLE)
                                           << " InletDrawVol: "
                                           << _Inlet->GetVolume(VOL_DRAW_CYCLE)
                                           << " ReturnDrawVol: "
                                           << _Return->GetVolume(VOL_DRAW_CYCLE)
                                           << " ACPrimeVolume: "
                                           << _AC->_fACPrimeVolume
                                           << endmsg;
      }

      // ditto for LNGR
      // there is some exposure here - could the first draw be big enough to
      // satisfy minimal volume constarints in procedure and safety and result in
      // a ratio < MIN_ALLOWABLE_AC_RATIO ?
      // if so, switch to return ( could be for rb ) will cause a shutdown for a valid cycle
      // R < MIN_ALLOWABLE_AC_RATIO.
      // trace PPQA memo Ratio Robustness and Short Cycles :
      // VC7
      //
      // YOU MUST SET _fLastKnowGoodRatio HERE SO THAT EVEN IF THE RATIO IS
      // OUT OF RANGE YOU WILL HAVE A RATIO TO USE FOR THE FIRST CYCLE AND
      // A POSSSIBLE SECOND CYCLE WHICH IS SHORT OR INVALID. (Ratio monitoring
      // is still in effect.)
      if ( favgRatio_temp > MAX_USEABLE_HIGH_RATIO )
      {
         _fLastKnownGoodRatio = MAX_USEABLE_HIGH_RATIO;

         // max useable ratio
         favgRatio_temp = MAX_USEABLE_HIGH_RATIO;
      }
      else
      {
         _fLastKnownGoodRatio = favgRatio_temp;
      }
   }
   else // calculate average ratio for non-ac prime cases
   {
      if ( _AC->GetVolume(VOL_DRAW_CYCLE) == 0.f )
      {
         // prevent divide-by-zero
         DataLog(log_level_safe_exec_info) << "ACDrawVolume is ZERO" << endmsg;
         favgRatio_temp = MAX_USEABLE_HIGH_RATIO;
      }
      else if ( _Return->GetVolume(VOL_DRAW_CYCLE) >= 0.0f)
      {
         favgRatio_temp = _Inlet->GetVolume(VOL_DRAW_CYCLE)
                          / _AC->GetVolume(VOL_DRAW_CYCLE);
      }
      else
      {
         favgRatio_temp = ( _Inlet->GetVolume(VOL_DRAW_CYCLE)
                            - _Return->GetVolume(VOL_DRAW_CYCLE) )
                          / _AC->GetVolume(VOL_DRAW_CYCLE);
      }


      // trace PPQA memo Ratio Robustness and Short Cycles :
      // VC2 ( can clamp here for valid and invalid cycles )
      //
      if ( (_AC->GetVolume(VOL_DRAW_CYCLE) == 0.f)
           ||(favgRatio_temp > MAX_USEABLE_HIGH_RATIO ) )
      {
         DataLog(log_level_safe_exec_info) << "Ratio Clamping Rcalc : "
                                           << favgRatio_temp
                                           << " R = "
                                           << MAX_USEABLE_HIGH_RATIO
                                           << endmsg;

         // max useable ratio
         //
         favgRatio_temp = MAX_USEABLE_HIGH_RATIO;
      }

   }

   // continue processing if ratio is for a run draw cycle
   // ie draw 2 thru start of rinseback if rb starts in draw.
   // valid cycle ?
   if ( favgRatio_temp >= MAX_ALLOWABLE_AC_RATIO )
   {
      // trace PPQA memo Ratio Robustness and Short Cycles :
      // VC1, VC2, IVC1
      // For the purposes of evaluating the AC in the needle, we
      // don't want to accumulate the volume if the ratio is above 15
      // until we are at the end of the draw cycle.
      // don't accumulate if accumulateVolumeAtHighRatio = FALSE;
      _bUseableACRatio = TestACHighRatioVolume(favgRatio_temp, accumulateVolumeAtHighRatio);
      // true continue
      // false stop, alarm gen in called
   }
   // UpdateAndTestAccumACLowRatio is the function that does both LowRatio
   // monitoring and _fLastKnownGoodRatio assignment.
   else // if( favgRatio_temp < MAX_ALLOWABLE_AC_RATIO )
   {
      // trace PPQA memo Ratio Robustness and Short Cycles :
      // VC4, IVC3
      // 24June99
      // For the purposes of evaluating the AC in the needle, we
      // don't really have a need to limit the low ratio so
      // don't test it if testLowRatio = FALSE;
      _bUseableACRatio = TestACLowRatioVolume(favgRatio_temp, testLowRatio);
      // true continue
      // false stop, alarm gen in called
   }
   SaveToPFR();
}




// SPECIFICATION: When ratio is above the high limit, check for
//                the amount of inlet fluid which has been processed
//                at this high ratio.
//                If the amount of fluid is not above the limit, set
//                the average AC ratio for use by other objects
//
// ERROR HANDLING:   If too much inlet volume has been processed at
//                a high ratio, the alarm AC_RATIO_TOO_HIGH is called
//
// RETURNS:       TRUE if the AC ratio is useable (i.e. below volume limit)
//                FALSE if the AC ratio should not be used (i.e. is invalid).
//
// NOTES :
// 07/28/97 msm
// see PPQA memo Ratio Robustness and Short Cycles
//
bool ACRatioAverage::TestACHighRatioVolume (float favgRatio, bool accumulateVolumeAtHighRatio)
{
   bool bHighRatioUsable;
   bool bCycleValid;

   // if we have a high ratio, we want to accumulate the inlet volume pumped for
   // this cycle and test it.  However, this method is called every time we pause
   // which can be many times in a draw cycle and we don't want to accumulate the
   // volume too often.  Only accumulate if the flag indicates we should
   if (accumulateVolumeAtHighRatio == TRUE)
      _fAccumInletVolumeAtHighRatio += _Inlet->GetVolume(VOL_DRAW_CYCLE);

   // trace PPQA memo Ratio Robustness and Short Cycles :
   // VC3, IVC2
   if ( _fAccumInletVolumeAtHighRatio > HIGH_RATIO_ACCUM_INLET_TOLERANCE )
   {
      // alarm high ratio
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_RATIO_TOO_HIGH_ALARM);
      if (_dumpFlag == TRUE)
      {
         _dumpFlag = FALSE;
         Dump(log_level_safety_alarm_detail);
      }

      DataLog(log_level_safety_alarm_detail) << "High Ratio NOT ALLOWED, Ratio: "
                                             << favgRatio
                                             << " _fAccumInletVolumeAtHighRatio: "
                                             << _fAccumInletVolumeAtHighRatio
                                             << endmsg;

      // you're done
      bHighRatioUsable = FALSE;
      // set to MIN to avoid div by zero probs elsewhere.
      //
      _fAvgACRatio = MIN_ALLOWABLE_AC_RATIO;

   }
   else // _faccumInletHighRatio <= HIGH_RATIO_ACCUM_INLET_TOLERANCE
   {
      // have we processed sufficient inlet volume?
      bCycleValid = (_Inlet->GetVolume(VOL_DRAW_CYCLE) >= INLET_SMALL_CYCLE_VOLUME_LIMIT) ? TRUE : FALSE;

      if ( bCycleValid == TRUE )
      {
         // trace PPQA memo Ratio Robustness and Short Cycles :
         // VC1, VC2
         _fAvgACRatio = ComputeAvgACRatio(favgRatio);
         // log high ratio override
         {
            DataLog(log_level_safe_exec_info) << "High Ratio ALLOWED within volume limits Ratio: "
                                              << favgRatio
                                              << " _fAccumInletVolumeAtHighRatio: "
                                              << _fAccumInletVolumeAtHighRatio
                                              << endmsg;
         }

      }
      else // invalid cycle
      {
         DataLog(log_level_safe_exec_error) << "Short cycle Inlet: "
                                            << _Inlet->GetVolume(VOL_DRAW_CYCLE)
                                            << endmsg;
         // trace PPQA memo Ratio Robustness and Short Cycles :
         // IVC1
         _fAvgACRatio = _fLastKnownGoodRatio;

         // log high ratio override
         DataLog(log_level_safe_exec_info) << "High Ratio ALLOWED Using LNGR : "
                                           << _fLastKnownGoodRatio
                                           << " Calculated: "
                                           << favgRatio
                                           << endmsg;

      }

      // for clarity can continue processing with _fAvgACRatio
      // upon return to caller.
      //
      bHighRatioUsable = TRUE;
   }

   return( bHighRatioUsable );

}

// SPECIFICATION: Validate AC ratio calculations which are not above the allowable
//                high ratio.  Also validate the cycle to ensure that sufficient
//                inlet volume has been pumped.
//
// RETURN : TRUE if sufficient inlet volume has been pumped and the calculated ratio
//                   is withing acceptable range.  fLastKnownGoodRatio is set.
//          FALSE if sufficient inlet volume has been pumped and the calculated ratio
//                   is lower than allowed.  This will cause AC_RATIO_TOO_LOW alarm.
//          FALSE if sufficient inlet volume has not been pumped and the acceptable
//                   limit of AC volume has also been violated.  This will cause
//                   AC_RATIO_TOO_LOW alarm.
//          TRUE if sufficient inlet volume has not been pumped but the acceptable
//                   limit of AC volume has not been violated. fLastKnownGoodRatio is set.
//          TRUE means the ratio is usable and FALSE means it should not be used.
//
// ERROR HANDLING: divide by zero protection for _fLastKnownGoodRatio. This should never be
//                 0 on entry as "it" is set for first return to first draw cycle value
//                 and upadted on subsequent valid return cycle when R calc is
//                 in range.
//
//
// NOTES :
// 07/28/97 msm
// see PPQA memo Ratio Robustness and Short Cycles
//
bool ACRatioAverage::TestACLowRatioVolume (float favgRatio, bool testLowRatio)
{
   bool bLowRatioUsable = FALSE;   // To keep GNU compiler happy for production builds
   bool bCycleValid;

   // have we processed sufficient volume?
   bCycleValid = (_Inlet->GetVolume(VOL_DRAW_CYCLE) >= INLET_SMALL_CYCLE_VOLUME_LIMIT) ? TRUE : FALSE;
   if ( bCycleValid == TRUE )
   {
      // trace PPQA memo Ratio Robustness and Short Cycles :
      // VC4
      if ( favgRatio < MIN_ALLOWABLE_AC_RATIO )
      {
         if (testLowRatio == TRUE)
         {
            // no override for this condition
            // valid cycle, low ratio shutdown
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_RATIO_TOO_LOW_ALARM);
            if (_dumpFlag == TRUE)
            {
               _dumpFlag = FALSE;
               Dump(log_level_safety_alarm_detail);
            }

            DataLog(log_level_safety_alarm_detail) << "AC Low Alarm Ratio: "
                                                   << favgRatio
                                                   << " LastKnownGoodRatio:%3.3f"
                                                   << _fLastKnownGoodRatio
                                                   << endmsg;

            // you're done
            bLowRatioUsable = FALSE;

            // the alarm has occurred but set ratio anyway to avoid divide by zero alarms
            _fAvgACRatio = MIN_ALLOWABLE_AC_RATIO;
         }
         else
         {
            // if low ratio is allowed, just set the ac ratio
            _fAvgACRatio    = ComputeAvgACRatio(favgRatio);
            bLowRatioUsable = TRUE;
         }
      }
      else // good ratio, valid cycle
      {
         // trace PPQA memo Ratio Robustness and Short Cycles :
         // VC5
         _fAvgACRatio = ComputeAvgACRatio(favgRatio);

         if (favgRatio != _fLastKnownGoodRatio || _monType == MON_FIRST_DRAW)
         {
            DataLog(log_level_safe_exec_info) << "Valid Cycle, Ratio : "
                                              << _fAvgACRatio
                                              << endmsg;
         }

         // update last known good ratio
         // trace PPQA memo Ratio Robustness and Short Cycles :
         // VC6
         _fLastKnownGoodRatio = favgRatio;

         // for clarity ratio good can continue in caller
         bLowRatioUsable = TRUE;
      }

   }    // end valid cycle processing
   else // invalid cycle processing
   {
      DataLog(log_level_safe_exec_error) << "Short cycle Inlet: "
                                         << _Inlet->GetVolume(VOL_DRAW_CYCLE)
                                         << endmsg;

      // divide by zero protection
      if ( _fLastKnownGoodRatio != 0.f )
      {
         // trace PPQA memo Ratio Robustness and Short Cycles :
         // IVC3
         _fAccumACVolumeAtLowRatio += (_AC->GetVolume(VOL_DRAW_CYCLE)
                                       - (_Inlet->GetVolume(VOL_DRAW_CYCLE) / _fLastKnownGoodRatio) );

         if ( _fAccumACVolumeAtLowRatio > LOW_RATIO_ACCUM_AC_TOLERANCE )
         {
            // alarm low ratio
            // trace PPQA memo Ratio Robustness and Short Cycles :
            // IVC5
            paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, AC_RATIO_TOO_LOW_ALARM);
            if (_dumpFlag == TRUE)
            {
               _dumpFlag = FALSE;
               Dump(log_level_safety_alarm_detail);
            }

            DataLog(log_level_safety_alarm_detail) << "Low Ratio NOT ALLOWED, Ratio: "
                                                   << favgRatio
                                                   << " _fAccumACVolumeAtLowRatio: "
                                                   << _fAccumACVolumeAtLowRatio
                                                   << endmsg;

            // you're done
            bLowRatioUsable = FALSE;

            // the alarm has occurred but set ratio anyway to avoid divide by zero alarms
            _fAvgACRatio = MIN_ALLOWABLE_AC_RATIO;
         }
         else // if( _fAccumACVolumeAtLowRatio <= LOW_RATIO_ACCUM_AC_TOLERANCE )
         {

            // log low ratio override
            DataLog(log_level_safe_exec_info) << "Low Ratio ALLOWED within volume limits, Ratio: "
                                              << favgRatio
                                              << " _fAccumACVolumeAtLowRatio: "
                                              << _fAccumACVolumeAtLowRatio
                                              << endmsg;

            // range limit ac accum to positive
            // trace PPQA memo Ratio Robustness and Short Cycles :
            // IVC4
            if ( _fAccumACVolumeAtLowRatio < 0.f )
            {
               // log reset
               DataLog(log_level_safe_exec_info) << "Reset _fAccumACVolumeAtLowRatio : "
                                                 << _fAccumACVolumeAtLowRatio
                                                 << endmsg;

               _fAccumACVolumeAtLowRatio = 0.f;
            }

            // ratio override allowed
            // trace PPQA memo Ratio Robustness and Short Cycles :
            // IVC3
            _fAvgACRatio = _fLastKnownGoodRatio;

            DataLog(log_level_safe_exec_info) << "Low Ratio ALLOWED Using LNGR : "
                                              << _fLastKnownGoodRatio
                                              << endmsg;

            // for clarity ratio good can continue in caller
            bLowRatioUsable = TRUE;
         }
      }
      else
      {
         // divide by zero protection
         _FATAL_ERROR(__FILE__, __LINE__, "divide by zero");
      }

   }  // end invalid cycle ratio processing

   return( bLowRatioUsable );
}

// SPECIFICATION:    Average AC Ratio monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   FATAL ERROR for unknown cycle.

void ACRatioAverage::ResetCycleVol (char cCycle)
{
   // nothing reset
   // if (cCycle == DRAW_CYCLE);
   // else if (cCycle == RETURN_CYCLE);
}

float ACRatioAverage::ComputeAvgACRatio (const float ratio)
{

   float avg = ratio;
   //
   // Only compute the average if we are past the first
   //  draw cycle ...
   //
   if ( _monType == MON_RUN_CYCLES )
   {
      //
      // Average the ratio (as prev 3 cycle running average ) if the ratio was
      //  higher than the last ratio ...
      //
      //  (if the ratio went to a smaller value then use the smaller value
      //   as the average )
      //
      if (    _fAvgACRatio > 0.0f
              && ratio > _fAvgACRatio )
      {
         avg = ( ( 2.0 * _fAvgACRatio ) + ratio ) / 3.0f;
      }

      DataLog(log_level_safe_exec_info) << "Cycle Ratio: "
                                        << ratio
                                        << " Avg Ratio: "
                                        << _fAvgACRatio
                                        << " New Avg: "
                                        << avg
                                        << endmsg;
   }

   return avg;
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.

void ACRatioAverage::Dump (DataLog_Level& log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void ACRatioAverage::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_fAccumInletVolumeAtHighRatio      = _fAccumInletVolumeAtHighRatio;
   SafetyPFRDataToSave.PFR_fAccumACVolumeAtLowRatio          = _fAccumACVolumeAtLowRatio;
   SafetyPFRDataToSave.PFR_bUseableACRatio                   = _bUseableACRatio;
   SafetyPFRDataToSave.PFR_fAvgACRatio                       = _fAvgACRatio;
   SafetyPFRDataToSave.PFR_fLastKnownGoodRatio               = _fLastKnownGoodRatio;
   SafetyPFRDataToSave.PFR_acRatioA_bACRatioMonitoringActive = _bACRatioMonitoringActive;
   SafetyPFRDataToSave.PFR_acRatioA_monType                  = _monType;

}

// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void ACRatioAverage::RestoreFromPFR ()
{
   _fAccumInletVolumeAtHighRatio = SafetyPFRDataFromRestore.PFR_fAccumInletVolumeAtHighRatio;
   _fAccumACVolumeAtLowRatio     = SafetyPFRDataFromRestore.PFR_fAccumACVolumeAtLowRatio;
   _bUseableACRatio              = SafetyPFRDataFromRestore.PFR_bUseableACRatio;
   _fAvgACRatio                  = SafetyPFRDataFromRestore.PFR_fAvgACRatio;
   _fLastKnownGoodRatio          = SafetyPFRDataFromRestore.PFR_fLastKnownGoodRatio;
   _bACRatioMonitoringActive     = SafetyPFRDataFromRestore.PFR_acRatioA_bACRatioMonitoringActive;
   _monType = SafetyPFRDataFromRestore.PFR_acRatioA_monType;

   DataLog(log_level_safe_exec_info) << "ACRatioAverage::RestoreFromPFR(), "
                                     << "fAccumInletVolumeAtHighRatio: " << _fAccumInletVolumeAtHighRatio
                                     << " fAccumACVolumeAtLowRatio: " << _fAccumACVolumeAtLowRatio
                                     << " bUseableACRatio: " << _bUseableACRatio
                                     << " fAvgACRatio: " << _fAvgACRatio
                                     << " fLastKnownGoodRatio: " << _fLastKnownGoodRatio
                                     << " bACRatioMonitoringActive: " << _bACRatioMonitoringActive
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH 219a7b09dd724a4d2d778b5d73b1b71b */
