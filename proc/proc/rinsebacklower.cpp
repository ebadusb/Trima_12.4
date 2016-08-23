/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rinsebacklower.cpp
 *
 */

#include "rinsebacklower.h"
#include <math.h>
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(RinsebackLower);

RinsebackLower::RinsebackLower()
   : StateAbs(),
     _QreturnMax(0.0f),
     _IRMax(0.0f),
     _VacRes(0.0f),
     _VRes(0.0f),
     _VResNet(0.0f)
{}

RinsebackLower::RinsebackLower(const RinsebackLower& state)
   : StateAbs(state),
     _QreturnMax(0.0f),
     _IRMax(0.0f),
     _VacRes(0.0f),
     _VRes(0.0f),
     _VResNet(0.0f)
{
   RinsebackLower::copyOver(state);
}

RinsebackLower::~RinsebackLower()
{
   RinsebackLower::cleanup();
}

int RinsebackLower::transitionStatus ()
{
   return _TransitionStatus;
}

int RinsebackLower::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);

   // Calc infusion rate, this will be used to run the return pump
   // in order to maintain infusion during rinseback.
   float IR = _IRMax;
   if (IR > pd.Adjustments().Tingling.CurrentCap.Get()) IR = pd.Adjustments().Tingling.CurrentCap.Get();
   if (IR < CobeConfig::data().RinsebackMinInfusion) IR = CobeConfig::data().RinsebackMinInfusion;

   //
   // Calculate the maximum rate we could deliver the AC in the reservoir to
   //  the donor
   //
   double VacPerMin = IR * ( pd.Donor().TBV.Get() / 1000.0 );

   if ( VacPerMin <= 0.0 )                                 // Can't happen, but better SAFE than sorry
      VacPerMin = IR;                                      //  ( assume 1 liter donor )

   //
   // Calculate the minimum time we could deliver all of the fluid in the reservoir to
   //  the donor
   double timeToDeliverAC = _VacRes / VacPerMin;


   float Qreturn;
   if ( timeToDeliverAC <= 0.0 )                           // Can't happen, but better SAFE than sorry
      Qreturn = VacPerMin;                                 //  ( assume All fluid in reservoir is AC )
   else
      //
      // Command the pump to run at the maximum safe delivery rate
      //
      Qreturn = _VRes / timeToDeliverAC;

   // Ensure that the commanded flow is not greater then the
   // current allowed return pump speed.
   if (Qreturn > pd.Adjustments().Return.CurrentCap.Get()) Qreturn = pd.Adjustments().Return.CurrentCap.Get();
   if (Qreturn > _QreturnMax) Qreturn = _QreturnMax;

   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   static bool logDone = false;
   if (!logDone)
   {
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower started at Qrp=" << Qreturn << " (" << pd.Adjustments().Return.CurrentCap.Get() << " " << _QreturnMax << ")"
                                             << " IR=" << IR << " (" << pd.Run().LastRunInfusion.Get() << " " << pd.Adjustments().Tingling.CurrentCap.Get() << " " << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << endmsg;
      logDone = true;
   }


   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qrp(Qreturn);

   // find the accumulated res volume
   _VResNet = _VResNet - pd.Status().ReturnPump.dV();

   return NORMAL;
}

int RinsebackLower::postProcess ()
{
   ProcData pd;

   //
   // Accumulate return volume
   pd.MakeVolumesWritable();
   pd.Volumes().VRetRinseback.Set(pd.Volumes().VRetRinseback.Get() + pd.Status().ReturnPump.dV());

   if ( pd.Volumes().VRetRinseback.Get() > CobeConfig::data().RinsebackVretLower )
   {
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower ended at max volume of " << pd.Volumes().VRetRinseback.Get() << endmsg;
      _TransitionStatus = 1;
   }

   if (  ( _VResNet <= 10.0 ) ) // IT 10526
   {
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower reservoir limit " << _VResNet << endmsg;
      _TransitionStatus = 1;
   }

   // Check for air seen at the low-level sensor
   if ( pd.SafetyControlReservoirIsEmpty() )
   {
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower ended with reservoir empty" << endmsg;
      _TransitionStatus = 1;
   }

   return NORMAL;
}

int RinsebackLower::preEnter ()
{
   ProcData pd;

   //
   //  Check to see if we have processed enough volume.  Before this point
   //  we always return assuming a ratio of 5 to avoid safety shutdowns.
   //
   float ratio = (pd.Volumes().VinTotal.Get() > 325.0f) ? pd.Run().LastRunRatio.Get() : 5.0f;
   if (ratio < 5.0f) ratio = 5.0f;

   //
   //
   //   Calculate maximum infusion rate and return flow allowable given procedure constraints
   //
   _IRMax = pd.RunTargets().InfusionRateTarget.Get();
   if (_IRMax < MIN_INFUSION_RATE)
      _IRMax = MIN_INFUSION_RATE;


   float Hin;
   if ( pd.Cycle().ResHct.Get() > pd.Run().AdjustedHct.Get() )
   {
      Hin = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
   }
   else
   {
      Hin = pd.Cycle().ResHct.Get() * (1.0f - 1.0f / ratio);
   }
   float QrpLimit = 520.0f * (1.0f - Hin);
   _QreturnMax = _IRMax * ratio * (pd.Donor().TBV.Get() / 1000.0f);
   if (_QreturnMax > QrpLimit) _QreturnMax = QrpLimit;

   //
   //
   //  Safety has a hardcoded limit of 100ml/min.  Do not exceed this
   //  limit or safety will shut the system down.
   //
   if (_QreturnMax > 100.0f) _QreturnMax = 100.0f;

   double facb;
   if ( ratio <= 1.0 || pd.Run().AdjustedHct.Get() >= 1.0 ) // Can't happen, but better SAFE than sorry
      facb = 1.0;                                           //  ( assume All fluid in the reservoir is AC )
   else
      //
      // Calculate the fraction of AC from the fluid in the reservoir
      //
      facb = 1 / ( 1 + ( ratio - 1.0 ) * ( 1.0 - pd.Run().AdjustedHct.Get() ) );

   //
   // Calculate the volume of AC in the reservoir
   //
   float vRes     = pd.Cycle().Vres.Get();
   float vResLast = pd.Cycle().VresLast.Get();
   DataLog(log_level_proc_rinseback_info) << "RinsebackLower: vRes=" << vRes << " vResLast=" << vResLast << endmsg;
   DataLog(log_level_proc_rinseback_info) << "RinsebackLower: vResDraw=" << pd.Cycle().VresDraw.Get() << " vResReturn=" << pd.Cycle().VresReturn.Get() << endmsg;

   if ( vRes < 0.0f && vResLast > 0.0f ) // return cycle
   {
      _VRes = vResLast + vRes + CobeConfig::data().NominalReturnLineVolume;
   }
   else // draw cycle
   {
      _VRes = fabs(vRes) + CobeConfig::data().NominalReturnLineVolume;
   }
   _VacRes = facb * _VRes * ( 1 - pd.Cycle().ResHct.Get() );

   DataLog(log_level_proc_rinseback_info) << "RinsebackLower: ratio=" << ratio << " IRMax=" << _IRMax
                                          << " QrpLimit=" << QrpLimit << " facb=" << facb
                                          << " VacRes=" << _VacRes << " VRes=" << _VRes
                                          << endmsg;

   // initialize the accumulated res volume
   // if we're doing a rinseback in the first return use max res vol for the draw volume
   if ( pd.Run().FirstDrawComplete && !pd.Run().FirstCycleComplete )
   {
      _VResNet = CobeConfig::data().ReservoirVolume + pd.Cycle().VresReturn.Get();
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower: in first return initial _VResNet=" << _VResNet << endmsg;
   }
   else
   {
      _VResNet = pd.Cycle().VresDraw.Get() + pd.Cycle().VresReturn.Get();
      DataLog(log_level_proc_rinseback_info) << "RinsebackLower: initial _VResNet=" << _VResNet << endmsg;
   }

   return NORMAL;
}

int RinsebackLower::postExit ()
{
   return NORMAL;
}

void RinsebackLower::copyOver (const RinsebackLower&)
{}

void RinsebackLower::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH a939105d98277b7596abf6f57618dfc9 */
