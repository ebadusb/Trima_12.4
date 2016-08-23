/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * File: rbcdetectoryield.cpp
 *
 * ABSTRACT:  See rbcdetectoryield.h.
 *
 */

#include <vxWorks.h>
#include <math.h>
#include <float.h>

#include "caldat.h"
#include "rbcdat.h"
#include "trima_datalog.h"
#include "rbcdetectoryield.h"
#include "procdata.h"

RBCDetectorYield::RBCDetectorYield()
   : _Cwset(0.0f),
     _Yr(0.0f),
     _timer(YIELD_INTEGRATION_INTERVAL, Callback<RBCDetectorYield>(this, &RBCDetectorYield::ComputeYield), TimerMessage::DISARMED),
     _previousVolume(0.0f),
     _VCuvette(0.0f),
     _state(RBC_DETECTOR_BASELINE_STATE)
{}

RBCDetectorYield::~RBCDetectorYield()
{
   Finalize();
}

void RBCDetectorYield::Initialize ()
{
   ProcData pd;

   // Get base Red level for yield calculation.
   _Cwset = RbcDat::data().YieldReflectancePltPlsRBCSet;

   DataLog(log_level_proc_info) << "Cwset = " << _Cwset << endmsg;


   _Yr = 0.0f;
   pd.MakeRunWritable();
   pd.Run().IntegratedPlateletYield.Set(0.0f);  // Initialize the data-logged Yield value.
   pd.MakeRunReadable();

   _state = RBC_DETECTOR_BASELINE_STATE;
}


float RBCDetectorYield::FilteredRED ()  // Return the current RED value, within min/max limits
{
   ProcData pd;
   float    REDnow = (float)pd.Run().Red.Get();
   float    REDmax = (float)RbcDat::data().RBCTestMaxRed;
   float    REDmin = (float)RbcDat::data().RBCTestMinRed;
   return( REDnow < REDmin ? REDmin : ( REDnow > REDmax ? REDmax : REDnow ));
}


void RBCDetectorYield::StartYieldIntegration (void)   //  Start the Yield Integration.
{
   _VCuvette       = 0.0f;
   _previousVolume = _VCuvette;
   _state          = INTEGRATING_YIELD_STATE;
   _timer.interval(YIELD_INTEGRATION_INTERVAL);    // Start the Yield Integrator.
}

void RBCDetectorYield::ComputeCr ()
{
   ProcData pd;

   float    RED   = FilteredRED();
   float    Crdet = 100 * ( ( 1.0f - RED / pd.Run().RedPl.Get() ) / _Cwset );
   pd.MakeRunWritable();
   pd.Run().Cr.Set(Crdet);
   pd.MakeRunReadable();
}

void RBCDetectorYield::ComputeYield ()   // Yield Integrator, called every YIELD_INTEGRATION_INTERVAL milliseconds
{
   if ( _state == INTEGRATING_YIELD_STATE )  // Check this class's state, just in case.
   {
      ProcData pd;
      float    RED = FilteredRED();

      // Ensure there are no divide by zero errors
      if ( pd.Run().RedPl.Get() == 0.0f )
      {
         pd.MakeRunWritable();
         pd.Run().RedPl.Set(100.0f);
         pd.MakeRunReadable();
         DataLog(log_level_proc_info) << "ComputeYield() using 100.0f for REDpl; RED = " << RED << endmsg;
      }

      if ( RED <= pd.Run().RedPl.Get() )  // Don't integrate <= 0 yield deltas.
      {
         if ( _Cwset < 0.1f  || _Cwset > 999.9f )      // Assume maximum yield if we are blind.
         {
            _Yr = FLT_MAX;
         }
         else
         {
            float dVCuvette = _VCuvette - _previousVolume;
            float Crdet     = pd.Run().Cr.Get();
            _Yr += dVCuvette * Crdet * 1.0E9f;
         }
      }

      pd.MakeRunWritable();
      pd.Run().IntegratedPlateletYield.Set(_Yr);  // Put the yield where it will be data-logged.
      pd.MakeRunReadable();

      _previousVolume = _VCuvette;  // Record the current cuvette volume for computing the next dCuvette.

      _timer.interval(YIELD_INTEGRATION_INTERVAL);    // Re-arm for the next yield calculation.
   }
   else
   {
      // We will end up here whenever the collect valve closes during collection
      // Make sure Yield Integration is stopped.
      _timer.interval(0);
      // Log a message about it.
   }

}


void RBCDetectorYield::ProcessHalStatus ()    // To be called for every new HAL Status message
{
   ProcData pd;
   // Obtain maximum Red reading during a configured non-alarm deltaVin
   /* if (!alarming) { */
   if ( _state == RBC_DETECTOR_BASELINE_STATE )
   {
      // Per IT 5002, the REDPL baseline value is equal to the peak RED reading
      // during early plasma flow through the cuvette from Vin processed
      // volumes of 210 ml to 260 ml (defaults).
      float deltaVin = pd.Volumes().Vin.Get() - RbcDat::data().VinStartSamplingREDpl;
      if ( deltaVin >=  0.0f /*ml*/ )
      {
         if ( deltaVin <  RbcDat::data().dVinSamplingREDpl /*ml*/ )
         {
            float RED = FilteredRED();
            // Clamp maximum REDpl baseline value in between 100 to 1800
            if ( RED > pd.Run().RedPl.Get() )
            {
               pd.MakeRunWritable();
               pd.Run().RedPl.Set(RED < 1800.0f ? RED : 1800.0f);
               pd.MakeRunReadable();
            }
         }
         else
         {
            DataLog(log_level_proc_info) << "VcBag = " << pd.Volumes().PlateletVolume.Get() << "ml; "
                                         << "Vin = " << pd.Volumes().Vin.Get() << "ml; "
                                         << "Final REDpl = " << pd.Run().RedPl.Get() << endmsg;
            _state = WAITING_FOR_COLLECT_VALVE_STATE;
         }
      }
   }
   else
   {
      ComputeCr();
      if ( _state == WAITING_FOR_COLLECT_VALVE_STATE )
      {
         if ( pd.Status().CollectValve.IsCollecting() ) StartYieldIntegration();
      }
      else if ( _state == INTEGRATING_YIELD_STATE )
      {
         if ( pd.Status().PlasmaValve.IsCollecting() )
            _VCuvette += pd.Status().CollectPump.dV();
         else
            _VCuvette += pd.Status().CollectPump.dV() - pd.Status().PlasmaPump.dV();  // Per IT 4921 plasma is added post-cuvette.
         if ( !pd.Status().CollectValve.IsCollecting() )
         {
            // Make sure Yield Integration is stopped.
            _timer.interval(0);
            ComputeYield();
            _state = WAITING_FOR_COLLECT_VALVE_STATE;
         }
      }
   }
}


void RBCDetectorYield::Finalize ()
{
   ProcData pd;
   float    volumeNow = pd.Volumes().PlateletVolume.Get();

   if ( volumeNow >= 50.0f /*ml*/ )
   {
      // Ensure that Yr is the "latest and greatest"
      ComputeCr();
      ComputeYield();
   }
   _timer.interval(0);    // Make sure the Yield Integrator is stopped.
   _state = EVALUATING_INTEGRATED_YIELD;
}

/* FORMAT HASH 6c59547bd4965c500034b235d075db15 */
