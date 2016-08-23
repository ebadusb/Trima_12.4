/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackspillover.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackspillover.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinsebackSpillover);

PlsRinsebackSpillover::PlsRinsebackSpillover()
   : StateAbs(),
     _LogFlag(true)
{}

PlsRinsebackSpillover::~PlsRinsebackSpillover()
{}

int PlsRinsebackSpillover::transitionStatus ()
{
   ProcData pd;
   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Spillover )
   {
      return 1;
   }

   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
   {
      return 2;
   }
   return _TransitionStatus;
}

int PlsRinsebackSpillover::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   float ratio = pd.PlasmaRinseback().Ratio.Get();

   // Calc infusion rate, this will be used to run the return pump
   // in order to maintain infusion during rinseback.
   float IR = pd.PlasmaRinseback().IRMax.Get();
   if ( IR > pd.Adjustments().Tingling.CurrentCap.Get() ) IR = pd.Adjustments().Tingling.CurrentCap.Get();

   //
   // Calculate the volume of AC in the reservoir
   //
   float vRes     = pd.Cycle().Vres.Get();
   float vResLast = pd.Cycle().VresLast.Get();
   if ( vRes < 0.0f && vResLast > 0.0f ) // return cycle
   {
      vRes = vResLast + vRes + CobeConfig::data().NominalReturnLineVolume;
   }
   else // draw cycle
   {
      vRes = fabs(vRes) + CobeConfig::data().NominalReturnLineVolume;
   }
   float vacRes = pd.PlasmaRinseback().Facb.Get() * vRes * ( 1 - pd.Cycle().ResHct.Get() );

   //
   // Calculate the maximum rate we could deliver the AC in the reservoir to
   //  the donor
   //
   float vacPerMin = IR * ( pd.Donor().TBV.Get() / 1000.0 );

   if ( vacPerMin <= 0.0 )                                 // Can't happen, but better SAFE than sorry
      vacPerMin = IR;                                      //  ( assume 1 liter donor )

   //
   // Calculate the minimum time we could deliver all of the fluid in the reservoir to
   //  the donor
   float timeToDeliverAC = vacRes / vacPerMin;
   float Qreturn;
   if ( timeToDeliverAC <= 0.0 )                           // Can't happen, but better SAFE than sorry
   {
      Qreturn = vacPerMin;                                 //  ( assume All fluid in reservoir is AC )
   }
   else
   {
      //
      // Command the pump to run at the maximum safe delivery rate
      //
      Qreturn = vRes / timeToDeliverAC;
   }

   // Ensure that the commanded flow is not greater than the
   // current allowed return pump speed.
   if (Qreturn > pd.Adjustments().Return.CurrentCap.Get()) Qreturn = pd.Adjustments().Return.CurrentCap.Get();
   if (Qreturn > pd.PlasmaRinseback().QReturnMax.Get()) Qreturn = pd.PlasmaRinseback().QReturnMax.Get();

   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   if ( _LogFlag )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackSpillover started at Qrp=" << Qreturn
                                             << " (" << pd.Adjustments().Return.CurrentCap.Get() << " " << pd.PlasmaRinseback().QReturnMax.Get() << ")"
                                             << " IR=" << IR
                                             << " (" << pd.Run().LastRunInfusion.Get() << " "
                                             << pd.Adjustments().Tingling.CurrentCap.Get() << " "
                                             << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << endmsg;
      _LogFlag = false;
   }

   //
   //  Override return pump speed if we have emptied the reservoir
   //
   if ( pd.SafetyControlReservoirIsEmpty() )
   {
      Qreturn = 0.0f;
   }

   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(CobeConfig::data().RinsebackQcspill);
   pd.Orders().Qrp(Qreturn);
   pd.Orders().RPM(CobeConfig::data().RPMRinsebackSpillAndPurge);

   return NORMAL;
}

int PlsRinsebackSpillover::postProcess ()
{
   ProcData pd;
   //
   // Accumulate return volume
   pd.MakeVolumesWritable();
   pd.Volumes().VRetRinseback.Set(pd.Volumes().VRetRinseback.Get() + pd.Status().ReturnPump.dV() );
   pd.Volumes().VRinseSpill.Set(pd.Volumes().VRinseSpill.Get() + pd.Status().CollectPump.dV() );
   pd.MakeVolumesReadable();

   //
   // Decrement the set volume
   //
   float totalVolume = pd.PlasmaRinseback().Vtotal.Get();
   totalVolume -= pd.Status().ReturnPump.dV();
   pd.PlasmaRinseback().Vtotal.Set(totalVolume);
   float acVolume = pd.PlasmaRinseback().Vac.Get();
   acVolume -= ( pd.PlasmaRinseback().Facb.Get() * pd.Status().ReturnPump.dV() * ( 1 - pd.Cycle().ResHct.Get() ) );
   pd.PlasmaRinseback().Vac.Set(acVolume);

   bool finished = false;
   if ( pd.Volumes().VRinseSpill.Get() > CobeConfig::data().RinsebackVs )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackSpillover ended at plt volume of " << pd.Volumes().VRinseSpill.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   if ( pd.Volumes().VRetRinseback.Get() > pd.Volumes().VMaxPltRinseback.Get() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackSpillover ended at max volume of " << pd.Volumes().VRetRinseback.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   if ( finished )
   {
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Spillover) );
   }

   return NORMAL;
}

int PlsRinsebackSpillover::preEnter ()
{
   ProcData pd;

   //
   //   Zero out the return volume.
   //
   VolumeAcc::ClearReturn();

   return NORMAL;
}

int PlsRinsebackSpillover::postExit ()
{
   //
   // Reset the logging flag so the next time thru the logging will happen again.
   //
   _LogFlag = true;

   return NORMAL;
}

/* FORMAT HASH ea92c9bfb0910cf453ab334bbcdc3e34 */
