/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackno1.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackno1.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinsebackNo1);

PlsRinsebackNo1::PlsRinsebackNo1()
   : StateAbs(),
     _LogFlag(true),
     _Ratio(0.0)
{}

PlsRinsebackNo1::~PlsRinsebackNo1()
{}

int PlsRinsebackNo1::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::No1 )
   {
      return 1;
   }

   if ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC )
   {
      return 2;
   }
   return _TransitionStatus;
}

int PlsRinsebackNo1::preProcess ()
{
   ProcData pd;

   if ( pd.Volumes().RBCVolume.Get() > 0.0f )
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   else
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

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
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   if ( _LogFlag )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackNo1 started at"
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

   //
   //   Override return pump speed if we have emptied the reservoir
   //
   if ( pd.SafetyControlReservoirIsEmpty() )
   {
      Qreturn = 0.0f;
   }

   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(Qreturn);

   float Qch = ( Qreturn + CobeConfig::data().RinsebackQpm ) / 2.0f;
   if ( Qch > CobeConfig::data().RinsebackQchrb2 ) Qch = CobeConfig::data().RinsebackQchrb2;

   //
   // Keep the plasma pump running at a speed greater than 1.0 ml/min
   if ( Qreservoir - Qch >= 1.0f )
      pd.Orders().Qplasma(Qreservoir - Qch);
   //
   // ... if not, then command it to 0.0
   else
      pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(Qreservoir);
   pd.Orders().RPM(0.0f);

   return NORMAL;
}

int PlsRinsebackNo1::postProcess ()
{
   ProcData pd;

   //
   // Accumulate return volume
   pd.MakeVolumesWritable();
   pd.Volumes().VRetRinseback.Set(pd.Volumes().VRetRinseback.Get() + pd.Status().ReturnPump.dV() );
   pd.Volumes().Vcrb1.Set(pd.Volumes().Vcrb1.Get() + pd.Status().CollectPump.dV() );
   pd.MakeVolumesReadable();

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

   bool finished = false;
   if ( pd.Volumes().VRetRinseback.Get() > pd.Volumes().VMaxPltRinseback.Get() )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackNo1 ended at max volume of " << pd.Volumes().VRetRinseback.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   // Check for air seen at the low-level sensor
   if (    pd.SafetyControlReservoirIsEmpty()
           && pd.Volumes().Vcrb1.Get() >= CobeConfig::data().RinsebackVcrb1 )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackNo1 ended with reservoir empty and Vc = " << pd.Volumes().Vcrb1.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   if ( finished )
   {
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::No1) );
   }

   return NORMAL;
}

int PlsRinsebackNo1::preEnter ()
{
   ProcData pd;

   //
   //   Zero out the return volume.
   //
   VolumeAcc::ClearReturn();

   pd.MakeRunWritable();
   pd.Run().RinsebackStarted = true;
   pd.MakeRunReadable();

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

int PlsRinsebackNo1::postExit ()
{
   //
   // Reset the logging flag so the next time thru the logging will happen again.
   //
   _LogFlag = true;

   return NORMAL;
}

/* FORMAT HASH 916bd9671d7aa82ab6bbd14890af6568 */
