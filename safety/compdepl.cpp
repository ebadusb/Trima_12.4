/*******************************************************************************
 *
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:      compdepl.cpp, the component depletion monitoring object
 *
 * ABSTRACT:   Safety's monitoring for component depletion.
 *             Monitors for the following situations:
 *                Whole Blood processed during RBC collect is greater
 *                   than allowed.
 *                Whole Blood processed not during RBC collect is greater
 *                   than allowed.
 */


#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "trima_assert.h"
#include "trima_datalog.h"
#include "alarms.h"

#include "hypovol.hpp"
#include "s_pfr.hpp"
#include "acratio.hpp"

// global definition for Safety PFR data
extern SPFRecovery SafetyPFRDataToSave;
extern SPFRecovery SafetyPFRDataFromRestore;

/*******************************************************************************
 *
 *  ComponentDepletion Methods
 *
 ******************************************************************************/

// SPECIFICATION:    Safety donor component depletion monitor constructor
//
// ERROR HANDLING:   none.

ComponentDepletion::ComponentDepletion(InletContainer* InletCont,
                                       ACContainer* ACCont,
                                       PlateletContainer* PlateletCont,
                                       RBCContainer* RBCCont,
                                       aDonor* Donor)
   : _Inlet(InletCont),
     _AC(ACCont),
     _Platelet(PlateletCont),
     _RBC(RBCCont),
     _Donor(Donor),
     _initWBProcessedDuringRBCCollectAfterPFR(false),
     _initWBProcessedNotDuringRBCCollectAfterPFR(false)
{
   _fWBProcessedDuringRBCCollect    = 0.0f;
   _fWBProcessedNotDuringRBCCollect = 0.0f;
   _bDepletionMonitoringActive      = false;
   _bPlateletCollectionStarted      = false;
   _bDrawCycleBarrier               = FALSE;
   _monType  = MON_INACTIVE;
   _dumpFlag = TRUE;

   // fatal if containers are not initialized
   trima_assert(_Inlet);
   trima_assert(_AC);
   trima_assert(_Platelet);
   trima_assert(_RBC);
   trima_assert(_Donor);

   SaveToPFR();
}


// SPECIFICATION:    Safety donor component depletion monitor constructor
//
// ERROR HANDLING:   none.

ComponentDepletion::~ComponentDepletion()
{
   _Inlet    = NULL;
   _AC       = NULL;
   _Platelet = NULL;
   _RBC      = NULL;
   _Donor    = NULL;
}


// SPECIFICATION:    Initialize component depletion monitoring
//                   for appropriate state
//
// ERROR HANDLING:   none
void ComponentDepletion::InitializeForState (long newState)
{
   switch (newState)
   {
      case DONOR_CONNECTED :
      case BLOOD_PRIME :
      case BLOOD_RUN :
      case BLOOD_RINSEBACK :
      case DONOR_DISCONNECT :
         _bDepletionMonitoringActive = true;
         break;
      case POST_RUN :
         _bDepletionMonitoringActive = false;
         break;
      default :
         break;
   }

   SaveToPFR();
}


// SPECIFICATION:    Component depletion monitoring update method.
//
// ERROR HANDLING:   RBC WB Processed Too Large alarm
//                   The same alarm is used for non-RBC WBP also.
void ComponentDepletion::Update (char cCycle)
{
   float deltaInlet, deltaAC;
   float WBProcessedRBCLimit, WBProcessedNotRBCLimit;
   float TBV_mls;
   bool  logMe = false;

   // if component depletion monitoring is not active,
   // just return
   if (!_bDepletionMonitoringActive || (_monType == MON_INACTIVE))
   {
      SaveToPFR();
      return;
   }

   // get pump volumes for this sample
   deltaInlet = _Inlet->GetVolume(VOL_DELTA);
   deltaAC    = _AC->GetVolume(VOL_DELTA);

   // IT5642 Include recirc volume in safety VBP equation during RBC collection
   if ((cCycle == DRAW_CYCLE) && (!_bDrawCycleBarrier))
   {
      // IT8322 & IT 8329 Added a latching variable to indicate every first
      // iteration through draw cycle to discriminate when to use this recirc
      // correction
      _bDrawCycleBarrier = true;

      if (_initWBProcessedDuringRBCCollectAfterPFR)
      {
         DataLog(log_level_safe_exec_info) << "Init WBProcessed volume during RBCCollect After PFR: " << _initWBProcessedDuringRBCCollectAfterPFR << endmsg;
         _initWBProcessedDuringRBCCollectAfterPFR = false;
      }
      _fWBProcessedDuringRBCCollect -=  3.0f;
   }
   else if (cCycle != DRAW_CYCLE)
   {
      _bDrawCycleBarrier = false;
   }

   // add the volumes to the proper accumulator
   if (_RBC->_bag->_myValve._positionS != HW_VALVE_RETURN)
   {
      if (_initWBProcessedDuringRBCCollectAfterPFR)
      {
         DataLog(log_level_safe_exec_info) << "Init WBProcessed volume during RBCCollect After PFR: " << _initWBProcessedDuringRBCCollectAfterPFR << endmsg;
         _initWBProcessedDuringRBCCollectAfterPFR = false;
      }
      _fWBProcessedDuringRBCCollect += (deltaInlet - deltaAC);
   }
   // accumulated during a draw cycle only because during the return
   // we are not processing fresh blood (fresh platelets)
   if ( ( !_bPlateletCollectionStarted ||
          (_bPlateletCollectionStarted && _Platelet->_bag->_myValve._positionS == HW_VALVE_COLLECT)
          ) &&
        (cCycle == DRAW_CYCLE)
        )
   {
      bool modCheck = false;

      if ( ( (int)_fWBProcessedNotDuringRBCCollect ) % 50 == 0)
         modCheck = true;

      if (_initWBProcessedNotDuringRBCCollectAfterPFR)
      {
         DataLog(log_level_safe_exec_info) << "Init WBProcessed volume not during RBCCollect After PFR: " << _initWBProcessedNotDuringRBCCollectAfterPFR << endmsg;
         _initWBProcessedNotDuringRBCCollectAfterPFR = false;
      }
      _fWBProcessedNotDuringRBCCollect += (deltaInlet - deltaAC);

      if (modCheck && ( (int)_fWBProcessedNotDuringRBCCollect ) % 50 != 0)
         logMe = true;
   }

   if ( (_fWBProcessedNotDuringRBCCollect >= MIN_PLATELET_COLLECTION_START) &&
        (_Platelet->_bag->_myValve._positionS == HW_VALVE_COLLECT)
        )
   {
      _bPlateletCollectionStarted = true;
   }

   // calculate the allowed limits
   if (_Donor->GetTBV_liters() != 0.0f)
   {
      TBV_mls = _Donor->GetTBV_liters() * 1000.0f;

      // IT 9924 -- Post HCT Calculation Modifications for v6.0 //////
      // calculate the WB processed during RBC limit
      const static float SampleBagVolm = 20.0f;
      const static float MAX_CRIT      = 0.55f;

      // the white stamp set volume is assumed for worst case, even though black ecv is smaller.
      const static float SetVolume = 187.0f;
      const static float ResVolume = 55.0f;

      float              HCT       = _Donor->GetHCT_decimal();

      if (HCT != 0.0f)
      {
         float k   = 0.2277f * ( 1.0f + fabs( (HCT - MAX_CRIT) / MAX_CRIT) );

         float rbc = (HCT - MIN_ALLOWABLE_POST_CRIT) * TBV_mls
                     - ( (SetVolume - ResVolume) * ( 1 - (1 / MAX_USEABLE_HIGH_RATIO) ) ) * HCT
                     - SampleBagVolm * HCT;

         // LambertExpantionTerm
         float LET = (-k * rbc) / (TBV_mls * HCT);

         // = LET - LET^2 + 3/2 LET^3 - 8/3 LET^4 + 125/24 LET^5
         float lambertFunc = LET
                             - LET * LET
                             + 3.0f / 2.0f * LET * LET * LET
                             - 8.0f / 3.0f * LET * LET * LET * LET
                             + 125.0f / 24.0f * LET * LET * LET * LET * LET;

         WBProcessedRBCLimit =  lambertFunc / (-k / TBV_mls);
      }
      else
      {
         // i dont know donor HCT
         DataLog(log_level_safe_exec_info) << "Donor Crit not available for WBProcessedRBCLimit"    << endmsg;
         WBProcessedRBCLimit = 0.0f;
      }

      ///////////////////////////////////////////////////////////////
      WBProcessedNotRBCLimit = TBV_mls * WBP_MULTIPLIER_LIMIT;

      //
      // Log the value every 50 mls ...
      //
      if (logMe)
      {
         DataLog(log_level_safe_exec_info) << "WBProcessedNotDuringRBCCollect: " << _fWBProcessedNotDuringRBCCollect
                                           << " Limit: " << WBProcessedNotRBCLimit << endmsg;

         DataLog(log_level_safe_exec_info) << "WBProcessedDuringRBCCollect: " << _fWBProcessedDuringRBCCollect
                                           << " Limit: " << WBProcessedRBCLimit << endmsg;
      }

      // Test for WBP during RBC
      if ( (_fWBProcessedDuringRBCCollect != 0.f) &&
           (_fWBProcessedDuringRBCCollect > WBProcessedRBCLimit ) &&
           (_RBC->_bag->_myValve._positionS != HW_VALVE_RETURN) )
      {
         // WB processed too large alarm
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, RBC_WB_PROCESSED_TOO_LARGE);

         if (_dumpFlag)
         {
            _dumpFlag = false;
            Dump(log_level_safety_alarm_detail);
         }

         DataLog(log_level_safety_alarm_detail) << "Alarm Limit for whole blood processed when in RBC collect : "
                                                << _fWBProcessedDuringRBCCollect << ", Limit : " << WBProcessedRBCLimit << endmsg;
      }

      // Test for WBP not during RBC
      if ( (_fWBProcessedNotDuringRBCCollect != 0.f) &&
           (_fWBProcessedNotDuringRBCCollect > WBProcessedNotRBCLimit ) &&
           (cCycle == DRAW_CYCLE) )
      {
         // hit the whole blood processed limit.  Use the existing alarm for now.
         paSafetyHardwareCommandsExec->respond_to_alarm(OFF_BOTH, RBC_WB_PROCESSED_TOO_LARGE);

         if (_dumpFlag)
         {
            _dumpFlag = false;
            Dump(log_level_safety_alarm_detail);
         }

         DataLog(log_level_safety_alarm_detail) << "Alarm Limit for whole blood processed when not in RBC collect : "
                                                << _fWBProcessedNotDuringRBCCollect << ", Limit : " << WBProcessedNotRBCLimit << endmsg;
      }
   }

   SaveToPFR();
}

// SPECIFICATION:    Component depletion monitoring method to
//                   return private data for logging
//
// ERROR HANDLING:   FATAL ERROR for unknown type.
float ComponentDepletion::GetPrivateFloatData (HYP_PDATA_TYPE type)
{
   float result = FLT_MAX;   // To keep GNU compiler happy for production builds

   switch (type)
   {
      case HYPPD_WBRBC :
         result = _fWBProcessedDuringRBCCollect;
         break;
      case HYPPD_WBNOTRBC :
         result = _fWBProcessedNotDuringRBCCollect;
         break;
      default :
      {
         if (_dumpFlag)
         {
            _dumpFlag = false;
            Dump(log_level_critical);
         }

         DataLog(log_level_critical) << "Request for unknown data " << (int)type << endmsg;
         _FATAL_ERROR(__FILE__, __LINE__, "invalid HYP_PDATA_TYPE");
      }
      break;
   }

   return result;
}

// SPECIFICATION:    Component depletion  monitoring method to reset
//                   volumes at the start of the draw or return cycle
//
// ERROR HANDLING:   FATAL ERROR for unknown cycle.
void ComponentDepletion::ResetCycleVol (char cCycle)
{
   // nothing to reset
   // if (cCycle == DRAW_CYCLE);
   // else if (cCycle == RETURN_CYCLE);
}


// SPECIFICATION:    Dump container data
//
// ERROR HANDLING:   none.
void ComponentDepletion::Dump (DataLog_Level log_level)
{
   _Inlet->DumpContainer(log_level);
   _AC->DumpContainer(log_level);
   _Platelet->DumpContainer(log_level);
   _RBC->DumpContainer(log_level);
}

// SPECIFICATION:    Save data to PFR
//
// ERROR HANDLING:   none.
void ComponentDepletion::SaveToPFR ()
{
   SafetyPFRDataToSave.PFR_fWBProcessedDuringRBCCollect        = _fWBProcessedDuringRBCCollect;
   SafetyPFRDataToSave.PFR_fWBProcessedNotDuringRBCCollect     = _fWBProcessedNotDuringRBCCollect;
   SafetyPFRDataToSave.PFR_compdepl_bDepletionMonitoringActive = _bDepletionMonitoringActive;
   SafetyPFRDataToSave.PFR_compdepl_bPlateletCollectionStarted = _bPlateletCollectionStarted;
   SafetyPFRDataToSave.PFR_compdepl_monType                    = _monType;
}


// SPECIFICATION:    Restore data from PFR
//
// ERROR HANDLING:   none.
void ComponentDepletion::RestoreFromPFR ()
{
   _fWBProcessedDuringRBCCollect    = SafetyPFRDataFromRestore.PFR_fWBProcessedDuringRBCCollect;
   _fWBProcessedNotDuringRBCCollect = SafetyPFRDataFromRestore.PFR_fWBProcessedNotDuringRBCCollect;
   _bDepletionMonitoringActive      = SafetyPFRDataFromRestore.PFR_compdepl_bDepletionMonitoringActive;
   _bPlateletCollectionStarted      = SafetyPFRDataFromRestore.PFR_compdepl_bPlateletCollectionStarted;
   _monType = SafetyPFRDataFromRestore.PFR_compdepl_monType;

   DataLog(log_level_safe_exec_info) << "ComponentDepletion::RestoreFromPFR(), "
                                     << "fWBProcessedDuringRBCCollect: " << _fWBProcessedDuringRBCCollect
                                     << " fWBProcessedNotDuringRBCCollect: " << _fWBProcessedNotDuringRBCCollect
                                     << " bDepletionMonitoringActive: " << _bDepletionMonitoringActive
                                     << " bPlateletCollectionStarted: " << _bPlateletCollectionStarted
                                     << " monType: " << (MON_TYPE)_monType
                                     << endmsg;

   _initWBProcessedDuringRBCCollectAfterPFR    = true;
   _initWBProcessedNotDuringRBCCollectAfterPFR = true;

   SaveToPFR();
}

DataLog_Stream& operator << (DataLog_Stream& os, MON_TYPE monType)
{
   switch (monType)
   {
      case MON_INACTIVE :               os << "MON_INACTIVE";               break;
      case MON_FIRST_DRAW :             os << "MON_FIRST_DRAW";             break;
      case MON_RUN_CYCLES :             os << "MON_RUN_CYCLES";             break;
      case MON_RINSEBACK :              os << "MON_RINSEBACK";              break;
      case MON_FIRST_DRAW_COMPLETE :    os << "MON_FIRST_DRAW_COMPLETE";    break;
      case MON_DISCONNECT :             os << "MON_DISCONNECT";             break;

      default :                         os << "UNKNOWN";                    break;
   }
   return os;
}

/* FORMAT HASH 14c3e93697be6e4cca57d982ed4b104c */
