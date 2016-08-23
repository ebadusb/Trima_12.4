/*******************************************************************************
 *
 * Copyright (c) 2002 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      cycleacc.h
 *             Accumulates set specific volumes for a given cycle.  The
 *              reservoir alarm object uses these data.
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#include "cycleacc.h"
#include "cobeconfig.h"
#include "MssCommon.h"


void CycleAcc::ClearAll ()
{
   ProcData pd;
   pd.MakeCycleWritable();

   pd.Cycle().Vin.Set(0);
   pd.Cycle().Vac.Set(0);
   pd.Cycle().Vret.Set(0);
   pd.Cycle().Vpls.Set(0);
   pd.Cycle().Vplt.Set(0);
   pd.Cycle().Vrbc.Set(0);
   pd.Cycle().VrbcRes.Set(0);
   pd.Cycle().ResRBCs.Set(CobeConfig::data().ReservoirMixVolume * pd.Cycle().ResHct.Get());

   //
   //
   //   We skip a cycle at the start of the draw cycle since the
   //   AC pump may have started an instant before and we want
   //   complete 500ms-frame numbers.  This also keeps glitches
   //   from occurring at the start of a return cycle.
   //
   pd.SkipCycleForRatioCalc(true);

   pd.MakeCycleReadable();
}

void CycleAcc::ClearFillVolume ()
{
   ProcData pd;
   pd.MakeCycleWritable();

   pd.Cycle().VresLast.Set(pd.Cycle().Vres.Get() );
   pd.Cycle().Vres.Set(0);
   //
   // Only clear the volumes for the current type of cycle
   if ( pd.Run().DrawCycle.Get() )
      pd.Cycle().VresDraw.Set(0);
   else
      pd.Cycle().VresReturn.Set(0);

   // Fix for IT 7888: Account for any volume overdraw
   if ( !pd.Run().DrawCycle.Get()
        && ( pd.Run().Substate.Get() > SS_CHANNEL_SETUP )
        && ( pd.Run().Substate.Get() < SS_MSSPFR ) )
   {
      float overdrawVolume;

      if ( ( overdrawVolume = pd.Cycle().VresLast.Get() - OVERDRAW_THRESHOLD_VOLUME ) > 0 )
      {
         pd.Cycle().VAccumOverdraw.Set(pd.Cycle().VAccumOverdraw.Get() + overdrawVolume);

         DataLog(log_level_proc_info) << " Draw volume exceeded "
                                      << " by " << overdrawVolume << " mL" << endmsg;
      }
   }

   pd.MakeCycleReadable();
}

void CycleAcc::UpdateEstimates ()
{
   ProcData pd;
   pd.MakeCycleWritable();

   //
   //   We only update during a draw cycle (reservoir volume > 0)
   //   and then only if the AC pump is running.  Since the AC pump
   //   does not run for the first few ml of a draw cycle we need
   //   to ignore that portion to avoid inaccurate ratio calculations.
   //   We examine both actual volume and commanded flows to ensure
   //   the pump has not just been commanded off (e.g. start of a
   //   return cycle).
   //
   const float resv     = pd.Cycle().Vres;
   const float inletdv  = pd.Status().InletPump.dV();
   const float acdv     = pd.Status().ACPump.dV();
   const float inletcmd = pd.Orders().cData().CmdFlow.Inlet;
   const float accmd    = pd.Orders().cData().CmdFlow.Ac;

   if ( (resv > 0.0f) && (inletdv > 0.0f ) && (inletcmd > 0.0f) && (acdv > 0.0f) && (accmd > 0.0f) )
   {
      if ( !pd.SkipCycleForRatioCalc() )
      {
         float tmp;

         //
         // Calculate the cycle ac ratio
         //
         float ratio = inletdv / acdv;

         //
         // if something is wrong here, bound to reasonable limits
         if ( ratio <= 5.0f )
         {
            DataLog(log_level_proc_info) << "Bad ratio: " << ratio << " " << inletdv << "/" << acdv << endmsg;
            ratio = 5.0f;
         }
         else if ( ratio >= 15.0f)
         {
            DataLog(log_level_proc_info) << "Bad ratio: " << ratio << " " << inletdv << "/" << acdv << endmsg;
            ratio = 15.0f;
         }

         pd.Cycle().ACRatio.Set(ratio);


         //
         //    Calculate Facb
         //
         tmp = ( ratio - 1 ) * ( 1 - pd.Run().AdjustedHct.Get() ) + 1;

         //
         // if something is wrong here, err on the conservative side
         if ( tmp <= 0.0f )
            tmp = 1.0f;
         tmp = 1.0f / tmp;
         if ( tmp > 1.0f )
            tmp = 1.0f;

         pd.Cycle().Facb.Set(tmp);


         //
         //   Calculate hematocrit of RBC line
         //
         if (pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC)
            tmp = PltPlsRBCCycleAcc::CalculateHRBC(ratio, inletdv, acdv);
         else
            tmp = CalculateHRBC(ratio, inletdv, acdv);

         pd.Cycle().Hrbc.Set(tmp);

         //
         //    Calculate reservoir hematocrit
         //
         tmp = pd.Cycle().ResRBCs.Get() / (resv + CobeConfig::data().ReservoirMixVolume);
         if ( tmp > 0.99f )
         {
            tmp = 0.99f;
         }
         if ( tmp < 0 )
         {
            tmp = 0;
         }
         pd.Cycle().ResHct.Set(tmp);

      }
      pd.SkipCycleForRatioCalc(false);
   }
   else
   {
      //
      // either Vin or Vac was zero
      pd.SkipCycleForRatioCalc(true);
   }

   //
   //   RBCs are accumulating in the reservoir even if we don't
   //   have enough information to recalculate.
   //
   if ( !pd.Status().RBCValve.IsCollecting())
   {
      pd.Cycle().ResRBCs.Set(pd.Cycle().ResRBCs + pd.RBCVolumeDeltaForCycle() * pd.Cycle().Hrbc);
   }

   pd.MakeCycleReadable();
}


// Hint: called from interface/ProcData.cpp based on cassette type:
void CycleAcc::Accumulate (const ProcData::UpdateModeType md)
{
   if ( !( ( md == ProcData::BLOODRUN ) ||
           ( md == ProcData::PRIME )    ||
           ( md == ProcData::RECOVERY ) ||
           ( md == ProcData::SOLUTION )
           ) ) return;

   ProcData pd;
   pd.MakeCycleWritable();

   pd.Cycle().Vin. Set(pd.Cycle().Vin  + pd.Status().InletPump.dV() );
   pd.Cycle().Vac. Set(pd.Cycle().Vac  + pd.Status().ACPump.dV() );
   pd.Cycle().Vret.Set(pd.Cycle().Vret + pd.Status().ReturnPump.dV() );
   pd.Cycle().Vpls.Set(pd.Cycle().Vpls + pd.Status().PlasmaPump.dV() );
   pd.Cycle().Vplt.Set(pd.Cycle().Vplt + pd.Status().CollectPump.dV() );

   pd.Cycle().Vres.Set(pd.Cycle().Vres - pd.Status().ReturnPump.dV() );


   if ( !pd.Status().RBCValve.IsCollecting())
   {
      pd.Cycle().VrbcRes.Set(pd.Cycle().VrbcRes + pd.RBCVolumeDeltaForCycle() );
      pd.Cycle().Vres.Set(pd.Cycle().Vres + pd.RBCVolumeDeltaForCycle() );
   }



   //
   // Set the current cycle type data
   if ( pd.Run().DrawCycle.Get() )
      pd.Cycle().VresDraw.Set(pd.Cycle().Vres.Get() );
   else
      pd.Cycle().VresReturn.Set(pd.Cycle().Vres.Get() );

   pd.MakeCycleReadable();


   // Don't need to update the rest of this stuff.  Safer to leave it alone.
   if (md == ProcData::SOLUTION)
   {
      pd.CurrentCycleTime().inactivate();

      return;
   }

   //
   // update R, facb and hrbc
   UpdateEstimates();

   //
   //
   //  If this is a cycle switch point then save off the current
   //   time within the cycle.
   //
   if ( pd.Run().CycleSwitch.Get() )
   {
      if ( pd.Run().DrawCycle.Get() )
      {
         pd.LastReturnCycleTime(pd.CurrentCycleTime().getMins() );
      }
      pd.CurrentCycleTime().init();
   }

   //
   //
   //  Activate or deactivate the timer depending on the
   //   current cycle and if we are paused.  We assume the system is
   //   paused if neither the inlet or return pump is running.
   //
   if ( (pd.Status().InletPump.dV() == 0.0f) && (pd.Status().ReturnPump.dV() == 0.0f) )
   {
      pd.CurrentCycleTime().inactivate();
   }
   else
   {
      pd.CurrentCycleTime().activate();
   }

   pd.MakeCycleReadable();
}

float CycleAcc::CalculateHRBC (float ratio, float inletdv, float acdv)
{
   ProcData pd;

   //
   //   Calculate hematocrit of RBC line
   //
   float tmp = pd.Run().AdjustedHct.Get();


   bool isCollectingRBCs = false;
   if ( pd.Status().RBCValve.IsCollecting())
   {
      isCollectingRBCs = true;
   }
   else
   {
      isCollectingRBCs = false;
   }



   if (  isCollectingRBCs  )
   {
      if (    pd.Predict().RBCVsvn.Get()      > 0.0f
              && pd.Status().InletPump.ActFlow() > pd.Status().PlasmaPump.ActFlow() )
      {
         float Hinr = pd.Run().AdjustedHct.Get() *
                      ( 1.0f - 1.0f / ratio ) *
                      ( 1.0f - CobeConfig::data().RecirculateSnDrawVol /
                        pd.Predict().RBCVsvn.Get() );
         tmp = ( pd.Status().InletPump.ActFlow() * Hinr ) /
               ( pd.Status().InletPump.ActFlow() - pd.Status().PlasmaPump.ActFlow() );
      }
   }
   else
   {
      if ( pd.RBCVolumeDeltaForCycle() > 0.0f && acdv > 0.0f )
      {
         tmp = pd.Run().AdjustedHct.Get() * (inletdv -  acdv) /
               pd.RBCVolumeDeltaForCycle();
      }
   }

   if ( tmp > 0.999999f )
   {
      tmp = 0.999999f;
   }
   else if ( tmp < 0.0f )
   {
      tmp = 0.0f;
   }

   return tmp;
}

// Hint: called from interface/ProcData.cpp based on cassette type:
void RBCPlsCycleAcc::Accumulate (const ProcData::UpdateModeType md)
{
   ProcData pd;
   pd.MakeCycleWritable();

   bool thisIsSalinePrime = (bool)(RecoveryTypes::RecoveryId(pd.RecoveryName().c_str() ) == RecoveryTypes::SalinePrime );

   //
   // Calculate the RBC line volume for this update cycle ...
   pd.RBCVolumeDeltaForCycle(pd.Status().InletPump.dV() - pd.Status().PlasmaPump.dV() );

   //
   // Calculate the reseroir changes for this update cycle ...
   if ( !pd.Status().PlasmaValve.IsCollecting() )
   {
      pd.Cycle().Vres.Set(pd.Cycle().Vres + pd.Status().PlasmaPump.dV() );
   }

   if ( !pd.Status().CollectValve.IsCollecting() && !thisIsSalinePrime)
   {
      pd.Cycle().Vres.Set(pd.Cycle().Vres + pd.Status().CollectPump.dV() );
   }

   pd.Cycle().Vrbc.Set(pd.Cycle().Vrbc + pd.RBCVolumeDeltaForCycle() );

   pd.MakeCycleReadable();
   CycleAcc::Accumulate(md);
}

void PltPlsRBCCycleAcc::Accumulate (const ProcData::UpdateModeType md)
{
   ProcData pd;
   pd.MakeCycleWritable();

   //
   // Calculate the RBC line volume for this update cycle ...
   pd.RBCVolumeDeltaForCycle(pd.Status().InletPump.dV() - pd.Status().CollectPump.dV() );

   if ( pd.Status().PlasmaValve.IsCollecting() )
   {
      pd.RBCVolumeDeltaForCycle(pd.RBCVolumeDeltaForCycle() - pd.Status().PlasmaPump.dV() );
   }

   //
   // Calculate the reseroir changes for this update cycle ...
   if ( !pd.Status().CollectValve.IsCollecting() )
   {
      pd.Cycle().Vres.Set(pd.Cycle().Vres + pd.Status().CollectPump.dV() );
   }

   pd.Cycle().Vrbc.Set(pd.Cycle().Vrbc + pd.RBCVolumeDeltaForCycle() );

   pd.MakeCycleReadable();
   CycleAcc::Accumulate(md);

}


float PltPlsRBCCycleAcc::CalculateHRBC (float ratio, float inletdv, float acdv)
{
   ProcData pd;

   float    Qrbc;

   if (pd.RunTargets().MeterRbcs.Get())
   {
      Qrbc = pd.Status().InletPump.ActFlow() - pd.Status().CollectPump.ActFlow();
   }
   else
   {
      Qrbc = pd.Status().InletPump.ActFlow() - pd.Status().PlasmaPump.ActFlow();
   }

   //
   //   Calculate hematocrit of RBC line
   //
   float tmp = pd.Run().AdjustedHct.Get();

   if ( pd.Status().RBCValve.IsCollecting() )
   {
      if ( (pd.Predict().RBCVsvn.Get() > 0.0f) && (Qrbc > 0.0f) )
      {
         float Hinr = pd.Run().AdjustedHct.Get() *
                      ( 1.0f - 1.0f / ratio ) *
                      ( 1.0f - CobeConfig::data().RecirculateSnDrawVol /
                        pd.Predict().RBCVsvn.Get() );

         tmp = ( pd.Status().InletPump.ActFlow() * Hinr ) / Qrbc;
      }
   }
   else
   {
      if ( pd.RBCVolumeDeltaForCycle() > 0.0f && acdv > 0.0f )
      {
         tmp = pd.Run().AdjustedHct.Get() * (inletdv -  acdv) /
               pd.RBCVolumeDeltaForCycle();
      }
   }

   if ( tmp > 0.99f )
   {
      tmp = 0.99f;
   }
   else if ( tmp < 0.0f )
   {
      tmp = 0.0f;
   }

   return tmp;
}

/* FORMAT HASH 0d53785f1d959b44656210f6cabe3aa2 */
