/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinseback.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinseback);

PlsRinseback::PlsRinseback()
   : StateAbs(),
     _ValveVolume(0.0f),
     _LogFlag(true),
     _Ratio(0.0)
{}

PlsRinseback::~PlsRinseback()
{}

int PlsRinseback::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Rinseback )
   {
      return 1;
   }

   if ( pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC )
   {
      return 2;
   }
   return _TransitionStatus;
}

int PlsRinseback::preProcess ()
{
   ProcData pd;

   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   //
   //
   //  Check to see if we've hit our valve motion threshold
   //
   if ( CobeConfig::data().RinsebackVpvalve > 0.0f )
   {
      if (    ( pd.Volumes().RBCVolume.Get() == 0.0f )
              && ( pd.Volumes().Vp.Get() > _ValveVolume)
              )
      {
         pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
      }
   }


   // Calc infusion rate, this will be used to run the return pump
   // in order to maintain infusion during rinseback.
   float IR = pd.PlasmaRinseback().IRMax.Get();
   if (IR > pd.Adjustments().Tingling.CurrentCap.Get()) IR = pd.Adjustments().Tingling.CurrentCap.Get();

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
   double timeToDeliverAC = pd.PlasmaRinseback().Vac.Get() / VacPerMin;

   float  Qreturn;
   if ( timeToDeliverAC <= 0.0 )                           // Can't happen, but better SAFE than sorry
      Qreturn = VacPerMin;                                 //  ( assume All fluid in reservoir is AC )
   else
      //
      // Command the pump to run at the maximum safe delivery rate
      //
      Qreturn = pd.PlasmaRinseback().Vtotal.Get() / timeToDeliverAC;


   // Ensure that the commanded flow is not greater then the
   // current allowed return pump speed.
   if (Qreturn > pd.Adjustments().Return.CurrentCap.Get()) Qreturn = pd.Adjustments().Return.CurrentCap.Get();
   if (Qreturn > pd.PlasmaRinseback().QReturnMax.Get()) Qreturn = pd.PlasmaRinseback().QReturnMax.Get();


   // Setup the reservoir input flow rate...
   // Exceed the return pump outlet speed to ensure that you fully
   // drain the channel before draining the reservoir.
   float Qreservoir = Qreturn + CobeConfig::data().RinsebackQpmrb1;
   if ( pd.Status().ReservoirIsFull() )
   {
      // Match the return pump outlet speed to keep from overfilling
      // the reservoir.
      Qreservoir = Qreturn;
   }

   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   if ( _LogFlag )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinseback started at"
                                             << " Qrp=" << Qreturn
                                             << " (" << pd.Adjustments().Return.CurrentCap.Get()
                                             << " " << pd.PlasmaRinseback().QReturnMax.Get() << ")"
                                             << " IR=" << IR
                                             << " (" << pd.Run().LastRunInfusion.Get()
                                             << " " << pd.Adjustments().Tingling.CurrentCap.Get()
                                             << " " << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << " R=" << pd.PlasmaRinseback().Ratio.Get()
                                             << " (" <<  pd.Run().LastRunRatio.Get() << ")"
                                             << endmsg;
      _LogFlag = false;
   }


   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(Qreturn);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(Qreservoir);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);

   return NORMAL;
}

int PlsRinseback::postProcess ()
{
   ProcData pd;

   //
   // Decrement the set volume
   //
   float totalVolume = pd.PlasmaRinseback().Vtotal.Get();
   totalVolume -= pd.Status().ReturnPump.dV();
   pd.PlasmaRinseback().Vtotal.Set(totalVolume);
   float acVolume = pd.PlasmaRinseback().Vac.Get();
   if ( _Ratio > 1 )
      acVolume -= pd.Status().ReturnPump.dV() / _Ratio;
   else
      acVolume -= pd.Status().ReturnPump.dV();
   pd.PlasmaRinseback().Vac.Set(acVolume);

   pd.MakeVolumesWritable();
   pd.Volumes().VRetRinseback.Set(pd.Volumes().VRetRinseback.Get() + pd.Status().ReturnPump.dV() );
   pd.MakeVolumesReadable();
   bool finished = false;
   if ( pd.Volumes().VRetRinseback.Get() > pd.Volumes().VMaxPltRinseback.Get() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinseback ended at max volume of " << pd.Volumes().VMaxPltRinseback.Get() << endmsg;
      finished = true;
   }

   // Check for air seen at the low-level sensor
   if ( pd.SafetyControlReservoirIsEmpty() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinseback ended with the resevoir empty" << endmsg;
      finished = true;
   }

   if ( finished )
   {
      _TransitionStatus = 1;
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Rinseback) );
   }

   return NORMAL;
}

int PlsRinseback::preEnter ()
{
   ProcData pd;

   //
   //
   //   Zero out the return volume.  This has two effects: it makes the
   //   max return volume look reasonable and allows us to keep track
   //   of the returned volume for proper calculation of hypovolemia
   //   during the donor disconnect test.
   //
   VolumeAcc::ClearReturn();
   pd.MakeRunWritable();
   pd.Run().RinsebackStarted = true;
   pd.MakeRunReadable();
   _ValveVolume = pd.Volumes().Vp.Get() + CobeConfig::data().RinsebackVpvalve;

   //
   // What's the actual Ratio
   //
   if ( pd.PlasmaRinseback().Vac.Get() >= 0.1 )
      _Ratio = pd.PlasmaRinseback().Vtotal.Get() / pd.PlasmaRinseback().Vac.Get();
   else
      _Ratio = 1.0;
   DataLog(log_level_proc_rinseback_info) << "Calculated AC ratio of remaining fluid " << _Ratio << endmsg;

   return NORMAL;
}

int PlsRinseback::postExit ()
{
   //
   // Reset the logging flag so the next time thru the logging will happen again.
   //
   _LogFlag = true;

   return NORMAL;
}

/* FORMAT HASH 867c7b9e97e83fba5fad05d3fcd743a1 */
