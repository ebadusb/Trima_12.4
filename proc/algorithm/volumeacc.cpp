/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      VolumeAcc.cpp
 *             Accumulates Volumes for products and pumps
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#include "cobeconfig.h"
#include "volumeacc.h"
#include "procdata.h"
#include "rtplateletyield.h"
#include "MssCommon.h"


void VolumeAcc::Clear ()
{
   ProcData pd;
   pd.MakeVolumesWritable();

   pd.Volumes().Vincv.Set(0.0f);
   pd.Volumes().TimeOfVincv.Set(0.0f);

   //
   //
   //  Product information
   //
   pd.Volumes().PlateletYield.Set(0.0f);
   pd.Volumes().PlateletVolume.Set(0.0f);
   pd.Volumes().VacPlateletBag.Set(0.0f);
   pd.Volumes().PlasmaVolume.Set(0.0f);
   pd.Volumes().VacPlasmaBag.Set(0.0f);
   pd.Volumes().RBCVolume.Set(0.0f);
   pd.Volumes().RBCDose.Set(0.0f);
   pd.Volumes().VacRBCBag.Set(0.0f);

   //
   //
   //   Pumped volume information
   //
   pd.Volumes().Vin.Set(0.0f);
   pd.Volumes().VinTotal.Set(0.0f);
   pd.Volumes().Vac.Set(0.0f);
   pd.Volumes().VacTotal.Set(0.0f);
   pd.Volumes().Vp.Set(0.0f);
   pd.Volumes().Vc.Set(0.0f);
   pd.Volumes().Vreturn.Set(0.0f);
   pd.Volumes().VReplacement.Set(0.0f);
   pd.Volumes().VSalineBolus.Set(0.0f);
   pd.Volumes().VBP.Set(0.0f);
   pd.Volumes().VBPPlatelet.Set(0.0f);

}

void VolumeAcc::ClearVac ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().Vac.Set(0.0f);
}
void VolumeAcc::ClearVacTotal ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().VacTotal.Set(0.0f);
}
void VolumeAcc::ClearVin ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().Vin.Set(0.0f);
}
void VolumeAcc::ClearVinTotal ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().VinTotal.Set(0.0f);
}
void VolumeAcc::ClearReturn ()
{
   ProcData pd;
   pd.MakeVolumesWritable();
   pd.Volumes().Vreturn.Set(0.0f);
}

void VolumeAcc::ClearPlateletBag ()
{
   ProcData pd;
   pd.MakeVolumesWritable();

   pd.Volumes().PlateletYield.Set(0.0f);
   pd.Volumes().PlateletVolume.Set(0.0f);
   pd.Volumes().VacPlateletBag.Set(0.0f);
   pd.Volumes().Vincv.Set(0.0f);
   pd.Volumes().TimeOfVincv.Set(0.0f);
}

void VolumeAcc::UpdateNeedleAC (const ProcData::UpdateModeType mode)
{
   ProcData pd;
   int      active = ( ( mode == ProcData::PRIME ) ||
                       ( mode == ProcData::BLOODRUN ) ||
                       ( mode == ProcData::RECOVERY ) ||
                       ( mode == ProcData::RINSEBACK )
                       );

   float manifold            = CobeConfig::data().AKOVolume;
   float vNonACBloodAtNeedle = pd.Volumes().VnonACBloodAtNeedle.Get();

   //
   // special case, if inlet and return commaned flows are equal,
   // then no assumptions can be made, so we assume all is without AC
   //
   int matched = (    pd.Status().InletPump.CmdFlow() != 0.0f
                      && pd.Status().InletPump.CmdFlow() == pd.Status().ReturnPump.CmdFlow() );

   if ( active && !matched )
   {
      vNonACBloodAtNeedle += ( pd.Status().InletPump.dV() - pd.Status().ReturnPump.dV() );

      if ( vNonACBloodAtNeedle > manifold )
      {
         vNonACBloodAtNeedle = manifold;
      }
      else if ( vNonACBloodAtNeedle < -0.1f )
      {
         vNonACBloodAtNeedle = -0.1f;
      }
   }
   else
   {
      vNonACBloodAtNeedle = manifold;
   }

   pd.MakeVolumesWritable();
   pd.Volumes().VnonACBloodAtNeedle.Set(vNonACBloodAtNeedle);
}


void VolumeAcc::Accumulate (const ProcData::UpdateModeType md)
{
   ProcData pd;

   int      recovery = ( md == ProcData::RECOVERY );
   int      run      = ( md == ProcData::BLOODRUN );

   float    facb     = pd.Cycle().Facb;
   float    inletDv  = pd.Status().InletPump.dV(); // IT9729 drift fix

   pd.MakeVolumesWritable();
   pd.Volumes().VacTotal.Set(pd.Volumes().VacTotal.Get() + pd.Status().ACPump.dV());
   pd.Volumes().VinTotal.Set(pd.Volumes().VinTotal.Get() + pd.Status().InletPump.dV());
   pd.Volumes().Vp.Set(pd.Volumes().Vp.Get() + pd.Status().PlasmaPump.dV());
   pd.Volumes().Vreturn.Set(pd.Volumes().Vreturn.Get() + pd.Status().ReturnPump.dV());

   // this is for tracking return volume for dlog analysis.
   // this CDS value is not used in proc calulations and should not be!
   // this value is not PFR persistant:
   pd.Volumes().ReturnPumpTotalVolume.Set(pd.Volumes().ReturnPumpTotalVolume.Get() + pd.Status().ReturnPump.dV());

   if ( pd.Run().DrawCycle.Get() &&
        ( !recovery || !pd.Run().FirstCycleComplete.Get() )
        )
   {
      pd.Volumes().Vin.Set(pd.Volumes().Vin.Get() + inletDv);
      pd.Volumes().Vac.Set(pd.Volumes().Vac.Get() + pd.Status().ACPump.dV());
   }


   if ( run )
   {
      float vbp = pd.Volumes().Vin.Get() - pd.Volumes().Vac.Get();
      if ( vbp < 0.0f ) vbp = 0.0f; // IT4873
      pd.Volumes().VBP.Set(vbp);
   }

   if ( !run && !recovery ) return;

   if ( pd.Status().PlasmaValve.IsCollecting() )
   {
      pd.Volumes().VacPlasmaBag.Set(pd.Volumes().VacPlasmaBag.Get() + ( facb * pd.Status().PlasmaPump.dV() ));
      pd.Volumes().PlasmaVolume.Set(pd.Volumes().PlasmaVolume.Get() + pd.Status().PlasmaPump.dV() );
   }

}

float VolumeAcc::DiscountForRecycled ()
{
   ProcData pd;
   pd.MakeVolumesWritable();

   float vRecycled = pd.Volumes().VRecycled.Get();
   float inletDv   = pd.Status().InletPump.dV();

   if ( pd.Run().CycleSwitch.Get() && pd.Run().DrawCycle.Get() )
   {
      vRecycled = CobeConfig::data().RecirculateSnDrawVol;
   }
   if ( pd.Run().DrawCycle.Get() )
   {
      vRecycled -= inletDv;
      inletDv    = -( vRecycled);
      if ( inletDv < 0.0f ) inletDv = 0.0f;
      if ( vRecycled< 0.0f ) vRecycled = 0.0f;
   }
   pd.Volumes().VRecycled.Set(vRecycled);

   return inletDv;
}


void RBCPlsVolumeAcc::Accumulate (const ProcData::UpdateModeType md)
{
   VolumeAcc::Accumulate(md);

   ProcData pd;
   pd.MakeVolumesWritable();

   int recovery  = ( md == ProcData::RECOVERY );
   int run       = ( md == ProcData::BLOODRUN );
   int rinseback = ( md == ProcData::RINSEBACK );


   float facb = pd.Cycle().Facb.Get();
   float hrbc = pd.Cycle().Hrbc.Get(); // - CobeConfig::data().RbcHctDelta;

   if (hrbc > 0.8f)
   {
      hrbc = 0.8f;
   }


   if ( recovery )
   {
      pd.Volumes().VSalineBolus.Set(pd.Volumes().VSalineBolus.Get() + pd.Status().CollectPump.dV());
   }
   else
   {
      if (!rinseback)   // keep this from recording in saline rinseback
         pd.Volumes().VReplacement.Set(pd.Volumes().VReplacement.Get() + pd.Status().CollectPump.dV());
   }

   pd.Volumes().VBPPlatelet.Set(0.0f); // by definition

   if ( !run && !recovery ) return;

   bool isCollectingRBCs = false;
   if ( pd.Status().RBCValve.IsCollecting())
   {
      isCollectingRBCs = true;
   }
   else
   {
      isCollectingRBCs = false;
   }



   if ( isCollectingRBCs )
   {
      // calculate the volume up the rbc line
      float rbc = pd.Status().InletPump.dV() - pd.Status().PlasmaPump.dV();

      rbc /= CobeConfig::data().RBCVolumeKluge;

      if ( hrbc > 0 )
      {

         pd.Volumes().VacRBCBag.Set(pd.Volumes().VacRBCBag.Get() + ( rbc * facb * ( 1 - hrbc ) ));
         pd.Volumes().RBCDose.Set(pd.Volumes().RBCDose.Get() + (rbc * hrbc));
      }
      pd.Volumes().RBCVolume.Set(pd.Volumes().RBCVolume.Get() + rbc);



      // if we are in the second half of collection.... accrue for the second RBC product deduct whats in A (done)
      if (pd.Run().drbcSplitDone.Get())
      {
         // for predicts evil purposes.....
         pd.Volumes().RBCDoubleVolume.Set(pd.Volumes().RBCVolume.Get() - pd.Volumes().RBCProduct_A_FinalVolume.Get() );
         pd.Volumes().RBCDoubleDose.Set(pd.Volumes().RBCDose.Get() - pd.Volumes().RBCProduct_A_FinalDose.Get() );

         // Product B accumulation
         pd.Volumes().RBCProduct_B_FinalDose.Set(pd.Volumes().RBCDose.Get() - pd.Volumes().RBCProduct_A_FinalDose.Get()) ;
         pd.Volumes().RBCProduct_B_FinalVolume.Set(pd.Volumes().RBCVolume.Get() - pd.Volumes().RBCProduct_A_FinalVolume.Get());
         pd.Volumes().RBCProduct_B_FinalVac.Set(pd.Volumes().VacRBCBag.Get() - pd.Volumes().RBCProduct_A_FinalVac.Get() );

      }
      else
      {
         // Product A accumulation
         pd.Volumes().RBCProduct_A_FinalDose.Set(pd.Volumes().RBCDose.Get()) ;
         pd.Volumes().RBCProduct_A_FinalVolume.Set(pd.Volumes().RBCVolume.Get());
         pd.Volumes().RBCProduct_A_FinalVac.Set(pd.Volumes().VacRBCBag.Get());
      }


   }

}


void PltPlsRBCVolumeAcc::Accumulate (const ProcData::UpdateModeType md)
{
   VolumeAcc::Accumulate(md);

   ProcData pd;
   pd.MakeVolumesWritable();

   //
   // Calculate the LRS chamber flow
   float QchCmd, QchAct;
   if ( pd.Status().PlasmaValve.IsCollecting() )
   {
      QchCmd = pd.Status().CollectPump.CmdFlow();
      QchAct = pd.Status().CollectPump.ActFlow();
   }
   else
   {
      QchCmd = pd.Status().CollectPump.CmdFlow() - pd.Status().PlasmaPump.CmdFlow();
      QchAct = pd.Status().CollectPump.ActFlow() - pd.Status().PlasmaPump.ActFlow();
   }
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CmdQch.Set(QchCmd);
   pd.TrimaSet().ActQch.Set(QchAct);
   pd.MakeTrimaSetReadable();

   int   recovery = ( md == ProcData::RECOVERY );
   int   run      = ( md == ProcData::BLOODRUN );

   float facb     = pd.Cycle().Facb.Get();
   float hrbc     = pd.Cycle().Hrbc.Get();

   if ( pd.Status().PlasmaValve.IsOpen() )
   {
      float dv = pd.Status().PlasmaPump.dV() - pd.Status().CollectPump.dV();

      if ( pd.Volumes().VacPlasmaBag.Get() > 0.0f )
         pd.Volumes().VacPlasmaBag.Set(pd.Volumes().VacPlasmaBag.Get() + ( facb * dv ));
      if ( pd.Volumes().PlasmaVolume.Get() > 0.0f )
         pd.Volumes().PlasmaVolume.Set(pd.Volumes().PlasmaVolume.Get() + dv);
   }

   pd.Volumes().Vc.Set(pd.Volumes().Vc.Get() + pd.Status().CollectPump.dV());

   if ( !run && !recovery ) return;

   if ( pd.Status().RBCValve.IsCollecting() )
   {
      // calculate the volume up the rbc line
      float rbc = pd.Status().InletPump.dV();
      //
      // Subtract off the plasma pump if it is collecting ...
      //  ( otherwise it has no effect )
      if ( pd.Status().PlasmaValve.IsCollecting() )
         rbc -= pd.Status().PlasmaPump.dV();
      // Subtract off the platelet pump always
      rbc -= pd.Status().CollectPump.dV();

      rbc /= CobeConfig::data().RBCVolumeKluge;

      if ( hrbc > 0 )
      {
         pd.Volumes().VacRBCBag.Set(pd.Volumes().VacRBCBag.Get() + ( rbc * facb * ( 1 - hrbc ) ));
         pd.Volumes().RBCDose.Set(pd.Volumes().RBCDose.Get() + (rbc * hrbc));
      }
      pd.Volumes().RBCVolume.Set(pd.Volumes().RBCVolume.Get() + rbc);

      // update only product A:
      // Product A accumulation
      pd.Volumes().RBCProduct_A_FinalDose.Set(pd.Volumes().RBCDose.Get()) ;
      pd.Volumes().RBCProduct_A_FinalVolume.Set(pd.Volumes().RBCVolume.Get());
      pd.Volumes().RBCProduct_A_FinalVac.Set(pd.Volumes().VacRBCBag.Get());



   }

   // if in platelet collection
   if ( pd.Status().CollectValve.IsCollecting() &&
        ( pd.Status().CollectPump.dV() > 0 ) )
   {
      pd.Volumes().VBPPlatelet.Set(pd.Volumes().VBP.Get());
      pd.Volumes().VacPlateletBag.Set(pd.Volumes().VacPlateletBag.Get() + ( pd.Cycle().Facb * pd.Status().CollectPump.dV()));
      pd.Volumes().PlateletVolume.Set(pd.Volumes().PlateletVolume.Get() + pd.Status().CollectPump.dV());
   }
   pd.MakeVolumesReadable();
   CalculateYield();
}

void PltPlsRBCVolumeAcc::CalculateYield ()
{
   ProcData pd;
   pd.MakeRunWritable();
   pd.MakeVolumesWritable();

   //
   // Save the start volume of PIR ...
   if ( pd.Run().Substate.Get() < SS_PIR_PLASMA )
      pd.Volumes().VinPIRStart.Set(pd.Predict().VinPIRStart.Get());
   //
   // Save the start volume of EPCA ...
   if ( pd.Run().Substate.Get() == SS_EXTENDED_PCA
        && pd.Volumes().VinEPCAStart.Get() == 0.0f )
      pd.Volumes().VinEPCAStart.Set(pd.Volumes().Vin.Get());

   //
   // Only execute once ...
   if ( pd.Status().CollectValve.IsCollecting() &&
        (pd.Volumes().Vincv.Get() == 0.0f)      &&
        (pd.Volumes().VinTotal.Get() > (CobeConfig::data().VinPrimeVent + 10.0f))  // IT 7189.  Don't set before prime vent.
        )                                                                          // Give margin for the valve to move.
   {
      // Save the time and volume when platelet collect begins
      pd.Volumes().Vincv.Set(pd.Volumes().Vin.Get());
      pd.Volumes().TimeOfVincv.Set(pd.ProcRunTime().getMins());

   }

   float QchCmd = pd.TrimaSet().CmdQch.Get();

   //
   // Calculate the minimum commanded flow during this period ...
   if ( pd.Run().Substate.Get() < SS_EXTENDED_PCA )
   {
      if ( QchCmd >= CobeConfig::data().CollectMinimumFlow
           && QchCmd < pd.Run().Qchmin.Get() )
         pd.Run().Qchmin.Set(QchCmd);
      if ( QchCmd >= CobeConfig::data().CollectMinimumFlow )
         pd.Run().Qchcurrent.Set(QchCmd);
   }
   pd.MakeRunReadable();
   pd.MakeVolumesReadable();

   //
   // Calculate the platelet yield ...
   RTPlateletYield::calculateYield(QchCmd);

}

/* FORMAT HASH fc8f9c7fc8d002e505fc0fd4320659e1 */
