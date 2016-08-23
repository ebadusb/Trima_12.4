/*
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PostRunPreMss_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the monitor template.
 *
 */

#include "PostRunPreMss_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "software_cds.h"
#include "rbcdat.h"
#include "pqinformation.h"


// Constants


// Define this class to the object dictionary
DEFINE_OBJ(PostRunPreMss_Monitor);

// Constructor
PostRunPreMss_Monitor::PostRunPreMss_Monitor()
   : aPQMonitor(),
     _plateletsWereCollected(false),
     _rbcsWereCollected(false),
     _pasSet(false),
     _rasSet(false),
     _VplateletTarget(0.0),
     _VcolPlateletBag(0.0),
     _PLTfactor(0.0),
     _pd(),
     _pqData()
{}



// Destructor
PostRunPreMss_Monitor::~PostRunPreMss_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void PostRunPreMss_Monitor::Monitor ()
{
   if ( enableMonitoring() )
   {
      // Get the Product Quality Data
      _pqData = _pd.PQI().PQdata.Get();

      if (_plateletsWereCollected && _pasSet)
      {
         //
         // Check product volumes, yields, etc.
         //
         SetPlateletFlags();

         //
         // Set flags for any early-run-terminations.
         //
         SetRunEndedEarlyFlags();
      }
      else
      {
         // If no platelet product was collected, then zero all flags and set the label
         // otherwise, finish flag checks.
         if (_VcolPlateletBag == 0.0f)
         {
            ClearPlateletFlags();
         }
      }

      if (_rbcsWereCollected)
      {
         SetRbcFlags();
      }
      else
      {
         ClearRbcFlags();
      }

      // Save the results
      _pd.MakePQIWritable();
      _pd.PQI().PQdata.Set(_pqData);
      _pd.MakePQIReadable();

      disable();  // Do this only once.

   }
}



//
// Set platelet flags
//
void PostRunPreMss_Monitor::SetPlateletFlags ()
{
   const float Kconfig = _pd.RunTargets().SelectedPlateletConcen.Get();
   const float Yield   = _pd.Volumes().PlateletYield.Get();

   // Platelet Concentration Check (pre-metering)
   // Only if configured concentration is within storage limits
   // The use of configured concentration (Kconfig) for both configured and collect implies standard (non-PPC)
   // collections as the collect range and storage range for PPCs do not overlap.
   //
   if (Software_CDS::withinStorageRange(Kconfig, Kconfig))
   {
      float concentrationLowerRange = Software_CDS::getLowConcentrationRange(Kconfig);
      // Check accrued concentration for within storage limits.

      // Platelet Concentration below storage range
      if (Yield < 1.0E6f * concentrationLowerRange * _VcolPlateletBag)
      {
         DataLog(_pqLog) << (int)PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE
                         << " COUNT PLATELET PRODUCT because PLATELET CONCENTRATION < " << concentrationLowerRange
                         << ".  Final bag volume was " << _VcolPlateletBag << endmsg;

         LogIt(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE);

         _pqData.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE);
      }

   }

   _pqData.label_platelets_leukoreduced = (_pqi.GetReasons(_pqData, VERIFY_PLATELET_WBCS) == 0);
}


//
//
// Clear platelet flags if no product collected
//
void PostRunPreMss_Monitor::ClearPlateletFlags ()
{
   //
   //   Remove flags for products that we didn't collect
   //
   DataLog(_pqLog) << "Clearing all flags for platelet products" << endmsg;

   // Never label a platelet product we didn't collect!
   _pqData.label_platelets_leukoreduced = false;

   // Never request verification of a platelet product we didn't collect!
   _pqData.verify_platelet_wbc_reasons.ClearAll();
   _pqData.verify_platelet_yield_volume_reasons.ClearAll();
   _pqData.platelet_concentration_reasons.ClearAll();
}

//
// Sets run ended early flags
//
void PostRunPreMss_Monitor::SetRunEndedEarlyFlags ()
{
   //
   //   Set flags for any early-run-terminations.
   //
   if ( ( (_pd.TrimaSet().Cassette.Get() != cassette::RBCPLS)                  &&
          (_VcolPlateletBag > 0.0f)                                           &&
          ( (_VplateletTarget - _VcolPlateletBag) > (_VplateletTarget * _PLTfactor) ) &&
          !_pqData.verify_platelet_yield_volume_reasons.Get(RUN_ENDED_EARLY)
          )
        )
   {
      DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                      << " VERIFY PLATELET VOLUME because Run Ended Early with Vcollect=" << _VcolPlateletBag
                      << " which deviates >= " << (_PLTfactor * 100.0f)
                      << "% from VcolTarget=" << _VplateletTarget
                      << endmsg;

      LogIt(RUN_ENDED_EARLY);

      _pqData.verify_platelet_yield_volume_reasons.Put(RUN_ENDED_EARLY); // Yield & Volume are the same flag
   }
   else
   {
      DataLog(_pqLog) << "Run not considered ended early " << endmsg;
   }

}



// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void PostRunPreMss_Monitor::Initialize ()
{}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void PostRunPreMss_Monitor::enable ()
{
   enableMonitoring(1);

   _plateletsWereCollected = _pd.PQI().PlateletsWereCollected.Get();
   _rbcsWereCollected      = _pd.PQI().RbcWereCollected.Get();

   _pasSet                 = _pd.RunTargets().MeterPlatelets.Get();
   _rasSet                 = _pd.RunTargets().MeterRbcs.Get();

   _VplateletTarget        = _pd.RunTargets().SelectedPlateletVolume.Get();
   _VcolPlateletBag        = _pd.Volumes().PlateletVolume.Get();

   _PLTfactor              = CobeConfig::data().Vplt1 / 100.0f;

   DataLog(_pqLog) << " PostRunPreMss_Monitor :: enable() "
                   << "  _VplateletTarget        " <<  _VplateletTarget
                   << "  _VcolPlateletBag        " <<  _VcolPlateletBag
                   << "  _PLTfactor              " <<  _PLTfactor
                   << "  _pasSet     " <<  _pasSet
                   << "  _rasSet     " <<  _rasSet
                   << "  _plateletsWereCollected " <<  _plateletsWereCollected
                   << "  _rbcsWereCollected " <<  _rbcsWereCollected
                   << endmsg;
}



// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void PostRunPreMss_Monitor::disable ()
{
   enableMonitoring(0);
}


void PostRunPreMss_Monitor::SetRbcFlags ()
{
   // Metered Storage Solution flags
   _pqData.label_rbcs_leukoreduced = (_pqi.GetReasons(_pqData, VERIFY_RBC_WBCS) == 0);

   // Never label a 2nd RBC product we didn't collect!
   _pqData.label_rbcs_leukoreduced_prod2 = ( (_pd.Volumes().RBCProduct_B_FinalVolume.Get() > 0.0f) &&
                                             (_pqi.GetReasons(_pqData, VERIFY_RBC_WBCS_PROD2) == 0) );
}

void PostRunPreMss_Monitor::ClearRbcFlags ()
{
   //
   //   Remove flags for products that we didn't collect
   //
   DataLog(_pqLog) << "Clearing all flags for RBC products" << endmsg;

   // Never label a RBC product we didn't collect!
   _pqData.label_rbcs_leukoreduced       = false;
   _pqData.label_rbcs_leukoreduced_prod2 = false;

   // Never request verification of a RBC product we didn't collect!
   _pqData.verify_rbc_volume_reasons.ClearAll();
   _pqData.verify_rbc_volume_reasons_prod2.ClearAll();
   _pqData.verify_rbc_wbc_reasons.ClearAll();
   _pqData.verify_rbc_wbc_reasons_prod2.ClearAll();
   _pqData.verify_rbc_quality_reasons.ClearAll();
   _pqData.verify_rbc_quality_reasons_prod2.ClearAll();
}

/* FORMAT HASH e239e034d31c1e1b459d14642db2b9a3 */
