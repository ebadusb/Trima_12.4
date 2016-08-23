/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * Revision 1.26  2006/08/09 19:38:01Z  jheiusb
 * 7696 -- change the first draw too small logging
 * Revision 1.25  2004/05/07 22:51:16Z  ms10234
 * 6966 - save the ac prime volume at the entrance to donor connected state and subtract that number from the cycle accumulator volume to check for draw too small.
 * Revision 1.24  2004/05/07 21:10:22Z  ms10234
 * 6966 - add logging for both the cycle volume for first draw too small and the cycle volume for first draw too big.
 * Revision 1.23  2004/05/07 16:24:25Z  ms10234
 * 6966 - Change first cycle volume accumulator to not use AC_PRIMEvolume
 * Revision 1.22  2003/09/09 22:44:53Z  ms10234
 * 6406 - fix pfr code
 * Revision 1.21  2003/07/21 21:46:33Z  td07711
 * IT6157 - don't need shutdown in ResetCycleVol()
 * Revision 1.20  2003/07/01 22:33:56Z  ms10234
 * 5818 - changed log levels from debug to info type.
 * Revision 1.19  2003/04/03 22:42:51Z  pn02526
 * Update logging per J. Linder memo in EDHF/design_input/log_categories.doc
 * Revision 1.18  2002/12/26 23:19:39Z  pn02526
 * Runnable safety executive with CRC checking of the code disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.
 * Revision 1.17  2002/08/07 11:32:04  pn02526
 * Remove "trace" external; use DataLog_Default instead.
 * To keep GNU compiler happy for production builds, initialize "result=FLT_MAX" in HypovolemiaCycleSize :: GetPrivateFloatData().
 * Revision 1.16  2002/07/30 10:08:38  pn02526
 * Enable logging fix for First Cycle Error. (May or may not be in V5.0).
 * Revision 1.15  2002/07/23 16:52:23  pn02526
 * Remove "DataLog_Level log" and use external "trace" instead.
 * Revision 1.14  2002/04/15 11:17:45  pn02526
 * Revise logging to use DataLogLevel class.  Revise FATAL_ERROR calls to use the new one.  Change ASSERT() macro calls to trima_assert().
 * Revision 1.13  2002/04/09 09:02:13  ms10234
 * 5648 - Also reset the Plt/Pls/RBC set volume first cycle limits when the set type message is sent out
 * Revision 1.12  2002/04/05 21:07:24  ms10234
 * 5648 - Safety subtracted off the RBC/Plasma adjustment every time the set type message came across from the current first cycle volume limits.  This resulted in much lower limits if the set was raised and lowered several time.  This calculation was changed to an absolute subtraction instead of the -= version.
 * Revision 1.11  2001/08/27 21:37:23  ms10234
 * 5271 -  Remove references to stamp color
 * Revision 1.10  2001/06/26 14:49:42  ms10234
 * 5171 - Added a prev cycle state variable to hold the cycle we were in
 * before rinseback.
 * Revision 1.9  2000/07/11 23:49:25  bs04481
 * Continue Draw Too Big monitoring through Rinseback
 * Revision 1.8  1999/08/01 20:53:02  BS04481
 * Non-critical code review changes identified at the 3.2 release.
 * Revision 1.7  1999/04/02 21:18:50  BS04481
 * When checking the reservoir volume during a return, also
 * include the residual volume from the preceeding draw.  Also,
 * enforce the reservoir volume check during First Return.
 * Revision 1.6  1999/04/01 23:08:24  BS04481
 * Safety failed to catch a draw cycle too long when the inlet pump
 * ran but the return pump did not run during first cycle.  This
 * occurred because Safety was not monitoring draw cycle during
 * first return
 * Revision 1.5  1999/03/31 23:22:55  BS04481
 * Safe PFR data for adjusted first cycle volumes due to black set
 * lack of LRS chamber.
 * Revision 1.4  1998/11/05 19:28:32  bs04481
 * Correct usage of ASSERT.  Code review change.
 * Revision 1.3  1998/06/01 13:44:58  bs04481
 * Correct hypovolemia cycle size monitoring to include AC Prime in
 * the first cycle draw volumes.  This was left out in the conversion.
 * Change to use the volumes accumulated by the bags in the cycle
 * instead of re-accumulating the volume here.
 * Revision 1.2  1998/05/18 17:49:47  bs04481
 * Add set type to hypovolemia cycle volume monitoring in order
 * to adjust first cycle volumes due to a lack of an LRS chamber.
 * Revision 1.1  1998/05/12 19:49:40  bs04481
 * Initial revision
 *
 *
 * TITLE:      hypovolc.cpp, the donor hypovolemia cycle size
 *             monitoring object
 *
 * ABSTRACT:   Safety's monitoring for hypovolemia due to cycle size.
 *             Monitors for the following situations:
 *                Draw cycle too big
 *                First Draw too big
 *                First Draw too small
 */


#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "alarms.h"

#include "hypovol.hpp"
#include "s_pfr.hpp"
#include "s_exec.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  HypovolemiaCycleSize Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety hypovolemia due to cycle size monitor constructor
//
// ERROR HANDLING:   none.

HypovolemiaCycleSize::HypovolemiaCycleSize(InletContainer* InletCont,
                                           ReturnContainer* ReturnCont,
                                           PlateletContainer* PlateletCont,
                                           PlasmaContainer* PlasmaCont,
                                           RBCContainer* RBCCont)
   : _Inlet(InletCont), _Return(ReturnCont), _Platelet(PlateletCont),
     _Plasma(PlasmaCont), _RBC(RBCCont)
{
   _faccumVolume                 = 0.0f;
   _facPrimeVolume               = 0.0f;
   _bHypovolemiaMonitoringActive = FALSE;
   _monType = MON_INACTIVE;
   _PrevMonType                  = MON_INACTIVE;
   _dumpFlag                     = TRUE;
   // these limits are initialized here to the values for the white and grey
   // sets but they may be adjusted later if the set type is an rbc/plasma set.
   _firstCycleMinVolumeAllowed = MIN_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE;
   _firstCycleMaxVolumeAllowed = MAX_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_Return);
   trima_assert(_Platelet);
   trima_assert(_Plasma);
   trima_assert(_RBC);

   SaveToPFR();
}

// SPECIFICATION:    Safety hypovolemia due to cycle size monitor destructor
//
// ERROR HANDLING:   none.

HypovolemiaCycleSize::~HypovolemiaCycleSize()
{
   _Inlet    = NULL;
   _Return   = NULL;
   _Platelet = NULL;
   _Plasma   = NULL;
   _RBC      = NULL;
}

// SPECIFICATION:    Initialize hypovolemia due to cycle size monitoring
//                   for appropriate state
//
// ERROR HANDLING:   none

void HypovolemiaCycleSize::InitializeForState (long newState)
{
   switch (newState)
   {
      case AC_PRIME :
      case DONOR_CONNECTED :
         // Snapshot the AC Prime volume at the entrance to
         //  donor connected state.
         _facPrimeVolume = _faccumVolume;
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
      case DONOR_DISCONNECT :
         _bHypovolemiaMonitoringActive = TRUE;
         break;
      case POST_RUN :
         _bHypovolemiaMonitoringActive = FALSE;
         break;
      default :
         break;

   }

   SaveToPFR();
}

// SPECIFICATION:    Safety hypovolemia due to cycle size set
//                   for set types.
//                   Sets the minimum and maximum limits for first
//                   cycle volumes based on set type.  The drbc
//                   set (for instance) has a different first cycle
//                   volume because it has no LRS chamber.
//
// ERROR HANDLING:   none.

void HypovolemiaCycleSize::SetSetType (SET_TYPE setType)
{

   // set the first cycle limits based on the set type
   // double check the set type
   if ( SafetyExec::instance()->SetType() == SET_RBCPLS )
   {
      _firstCycleMinVolumeAllowed =   MIN_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE
                                    - RBCPLS_SET_FIRST_DRAW_CYCLE_ADJUSTMENT;
      _firstCycleMaxVolumeAllowed =   MAX_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE
                                    - RBCPLS_SET_FIRST_DRAW_CYCLE_ADJUSTMENT;
   }
   else
   {
      _firstCycleMinVolumeAllowed = MIN_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE;
      _firstCycleMaxVolumeAllowed = MAX_ALLOWABLE_DRAW_CYCLE_VOLUME_FIRST_CYCLE;
   }
   DataLog(log_level_safe_exec_info) << "First Cycle limits adjusted to Min: "
                                     << _firstCycleMinVolumeAllowed
                                     << " and Max: "
                                     << _firstCycleMaxVolumeAllowed
                                     << endmsg;

   SaveToPFR();
}


// SPECIFICATION:    Update and test fill volumes.
//
// ERROR HANDLING:   Draw Cycle Too Big alarm
//                   Max Allowable Draw Cycle Volume First Cycle alarm
//                   First Draw Cycle Too Small alarm

void HypovolemiaCycleSize::Update (char cCycle)
{
   float cycleInlet, cycleReturn;
   float cyclePlatelet, cyclePlasma, cycleRBC;
   float faccumPreviousHalfCycle;

   // if hypovolemia monitoring is not active, just return
   if ( (_bHypovolemiaMonitoringActive == FALSE)
        ||(_monType == MON_INACTIVE) )
   {
      SaveToPFR();
      return;
   }

   // accumulate the cycle volume
   if (cCycle == RETURN_CYCLE)
   {
      // get the return cycle volumes up to this point
      cycleInlet    = _Inlet->GetVolume(VOL_RETURN_CYCLE);
      cycleReturn   = _Return->GetVolume(VOL_RETURN_CYCLE);
      cyclePlatelet = _Platelet->GetVolume(VOL_RETURN_CYCLE);
      cyclePlasma   = _Plasma->GetVolume(VOL_RETURN_CYCLE);
      cycleRBC      = _RBC->GetVolume(VOL_RETURN_CYCLE);

      // get the residual volume that may left in the reservoir
      // from the previous draw
      faccumPreviousHalfCycle = _Inlet->GetVolume(VOL_DRAW_CYCLE)
                                - _Return->GetVolume(VOL_DRAW_CYCLE)
                                + _Platelet->GetVolume(VOL_DRAW_CYCLE)
                                + _Plasma->GetVolume(VOL_DRAW_CYCLE)
                                + _RBC->GetVolume(VOL_DRAW_CYCLE);
   }
   else
   {
      // get the draw cycle volumes up to this point
      cycleInlet    = _Inlet->GetVolume(VOL_DRAW_CYCLE);
      cycleReturn   = _Return->GetVolume(VOL_DRAW_CYCLE);
      cyclePlatelet = _Platelet->GetVolume(VOL_DRAW_CYCLE);
      cyclePlasma   = _Plasma->GetVolume(VOL_DRAW_CYCLE);
      cycleRBC      = _RBC->GetVolume(VOL_DRAW_CYCLE);

      // we don't look at volume left in the reservoir
      // from the previous return because we are looking
      // for an over-draw problem, not an over-return.
      faccumPreviousHalfCycle = 0.0f;
   }

   // calculate the reservoir volume for this cycle
   _faccumVolume = cycleInlet
                   - cycleReturn
                   + cyclePlatelet
                   + cyclePlasma
                   + cycleRBC;

   if (    (_monType == MON_FIRST_DRAW)
           || (_monType == MON_FIRST_DRAW_COMPLETE)
           || (_monType == MON_RINSEBACK && (    _PrevMonType == MON_FIRST_DRAW
                                                 || _PrevMonType == MON_FIRST_DRAW_COMPLETE ) )
           )
   {
      // Test for first draw too big
      if ( (_faccumVolume + faccumPreviousHalfCycle) > _firstCycleMaxVolumeAllowed)
      {
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DRAW_CYCLE_TOO_BIG);
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_safety_alarm_detail);
         }
         DataLog(log_level_safety_alarm_detail) << " FIRST DRAW_CYCLE_TOO_BIG  FillVolume: "
                                                << _faccumVolume
                                                << " Residual: "
                                                << faccumPreviousHalfCycle
                                                << endmsg;
      }
   }
   // Test for normal draw cycle too big
   else if (    (_monType == MON_RUN_CYCLES)
                || (_monType == MON_RINSEBACK )
                )
   {
      if ( (_faccumVolume + faccumPreviousHalfCycle) > MAX_ALLOWABLE_SAFETY_DRAW_CYCLE_VOLUME )
      {
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, DRAW_CYCLE_TOO_BIG);
         if (_dumpFlag == TRUE)
         {
            _dumpFlag = FALSE;
            Dump(log_level_safety_alarm_detail);
         }
         DataLog(log_level_safety_alarm_detail) << " RUN Cycles : DRAW_CYCLE_TOO_BIG FillVolume: "
                                                << _faccumVolume
                                                << " Residual: "
                                                << faccumPreviousHalfCycle
                                                << endmsg;
      }
   }

   SaveToPFR();
   return;
}

// SPECIFICATION:    Test for First cycle fill too small which
//                   may indicate the procedure was started with
//                   a wet set.
//
// ERROR HANDLING:   First Draw Cycle Too Small alarm

void HypovolemiaCycleSize::TestFillTooSmallFirstCycle ()
{
   // if hypovolemia monitoring is not active
   // or if this is not a draw cycle, just return
   if ( (_bHypovolemiaMonitoringActive == FALSE)
        ||( (_monType != MON_FIRST_DRAW)
            &&(_monType != MON_FIRST_DRAW_COMPLETE) ) )
   {
      return;
   }
   // Test for first draw too small
   if ( _faccumVolume - _facPrimeVolume < _firstCycleMinVolumeAllowed )
   {
      paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, FIRST_DRAW_CYCLE_TOO_SMALL);
      if (_dumpFlag == TRUE)
      {
         _dumpFlag = FALSE;
         Dump(log_level_safety_alarm_detail);
      }
      DataLog(log_level_safety_alarm_detail) << "Alarm : FIRST_DRAW_CYCLE_TOO_SMALL FillVolume_EC = "
                                             << _faccumVolume - _facPrimeVolume
                                             << ";  Minimum Allowed is -> "
                                             << _firstCycleMinVolumeAllowed
                                             << endmsg;
   }
   else
   {
      // NOMINAL, Non-Error Operation logging
      DataLog(log_level_safe_exec_info) << "End first draw,CycleVolume = "
                                        << _faccumVolume - _facPrimeVolume
                                        << ", CycleVolume w/ ACPrime = "
                                        << _faccumVolume
                                        << endmsg;
   }
   return;
}

// SPECIFICATION:    Hypovolemia monitoring method to
//                   return private data for logging
//
// ERROR HANDLING:   FATAL ERROR for unknown type.

float HypovolemiaCycleSize::GetPrivateFloatData (HYP_PDATA_TYPE type)
{
   float result = FLT_MAX;    // To keep GNU compiler happy for production builds

   switch (type)
   {
      case HYPPD_ACCUMCYCLE :
         result = _faccumVolume;
         break;
      default :
      {
         DataLog(log_level_critical) << "Type: " << type << "Request for unknown data" << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid HYP_PDATA_TYPE");
      }
      break;
   }
   return(result);
}



// SPECIFICATION:    Hypovolemia draw cycle size monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   none.

void HypovolemiaCycleSize::ResetCycleVol (char cCycle)
{
   if (cCycle == DRAW_CYCLE)
   {
      // log the drain volume
      DataLog(log_level_safe_exec_info) << " DrainVolume : "
                                        << _faccumVolume
                                        << endmsg;

      // reset for draw cycle
      _faccumVolume = 0.0f;
   }
   else if (cCycle == RETURN_CYCLE)
   {
      // log the fill volume
      DataLog(log_level_safe_exec_info) << " FillVolume : "
                                        << _faccumVolume
                                        << endmsg;

      // reset for return cycle
      _faccumVolume = 0.0f;
   }

   SaveToPFR();
}

// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.

void HypovolemiaCycleSize::Dump (DataLog_Level log_level)
{
   _Inlet->DumpContainer(log_level);
   _Return->DumpContainer(log_level);
   _Platelet->DumpContainer(log_level);
   _Plasma->DumpContainer(log_level);
   _RBC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.

void HypovolemiaCycleSize::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_faccumVolume   = _faccumVolume;
   SafetyPFRDataToSave.PFR_facPrimeVolume = _facPrimeVolume;
   SafetyPFRDataToSave.PFR_firstCycleMinVolumeAllowed            = _firstCycleMinVolumeAllowed;
   SafetyPFRDataToSave.PFR_firstCycleMaxVolumeAllowed            = _firstCycleMaxVolumeAllowed;
   SafetyPFRDataToSave.PFR_hypovolC_bHypovolemiaMonitoringActive = _bHypovolemiaMonitoringActive;
   SafetyPFRDataToSave.PFR_hypovolC_monType                      = _monType;
   SafetyPFRDataToSave.PFR_hypovolC_PrevMonType                  = _PrevMonType;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.

void HypovolemiaCycleSize::RestoreFromPFR ()
{
   _faccumVolume                 = SafetyPFRDataFromRestore.PFR_faccumVolume;
   _facPrimeVolume               = SafetyPFRDataFromRestore.PFR_facPrimeVolume;
   _firstCycleMinVolumeAllowed   = SafetyPFRDataFromRestore.PFR_firstCycleMinVolumeAllowed;
   _firstCycleMaxVolumeAllowed   = SafetyPFRDataFromRestore.PFR_firstCycleMaxVolumeAllowed;
   _bHypovolemiaMonitoringActive = SafetyPFRDataFromRestore.PFR_hypovolC_bHypovolemiaMonitoringActive;
   _monType = SafetyPFRDataFromRestore.PFR_hypovolC_monType;
   _PrevMonType                  = SafetyPFRDataFromRestore.PFR_hypovolC_PrevMonType;

   DataLog(log_level_safe_exec_info) << "HypovolemiaCycleSize::RestoreFromPFR(), "
                                     << "faccumVolume: " << _faccumVolume
                                     << " facPrimeVolume: " << _facPrimeVolume
                                     << " firstCycleMinVolumeAllowed: " << _firstCycleMinVolumeAllowed
                                     << " firstCycleMaxVolumeAllowed: " << _firstCycleMaxVolumeAllowed
                                     << " bHypovolemiaMonitoringActive: " << _bHypovolemiaMonitoringActive
                                     << " monType: " << (MON_TYPE)_monType
                                     << " PrevMonType: " << _PrevMonType
                                     << endmsg;

   SaveToPFR();
}

/* FORMAT HASH 7d8898e4038c21eaf6a50d226abfa47d */
