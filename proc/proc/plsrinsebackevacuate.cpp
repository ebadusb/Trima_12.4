/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plsrinsebackevacuate.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "plsrinsebackevacuate.h"
#include "trimamessages.h"
#include "volumeacc.h"

DEFINE_STATE(PlsRinsebackEvacuate);

PlsRinsebackEvacuate::PlsRinsebackEvacuate()
   : StateAbs(),
     _Evacuating(false),
     _EvacuationLimit(0.0f),
     _EvacuationMinVolume(0.0f),
     _LogFlag(true),
     _Ratio(0.0)
{}

PlsRinsebackEvacuate::~PlsRinsebackEvacuate()
{}

int PlsRinsebackEvacuate::transitionStatus ()
{
   ProcData pd;

   if ( pd.PlasmaRinseback().Status.Get() & ProcPlsRinseback_CDS::Evacuate )
   {
      return 1;
   }

   return _TransitionStatus;
}

int PlsRinsebackEvacuate::preProcess ()
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
   //
   //   Override return pump speed if we are waiting for the evacuation process
   //
   if (    _Evacuating
           || pd.SafetyControlReservoirIsEmpty() )
   {
      Qreturn = 0.0f;
   }

   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   if ( _LogFlag )
   {
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackEvacuate started at"
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

   float Qch = Qreservoir / 2.0f;
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

int PlsRinsebackEvacuate::postProcess ()
{
   ProcData pd;

   pd.MakeVolumesWritable();
   pd.Volumes().VRetRinseback.Set(pd.Volumes().VRetRinseback.Get() + pd.Status().ReturnPump.dV() );
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
      DataLog(log_level_proc_rinseback_info) << "PlsRinsebackEvacuate ended at max volume of " << pd.Volumes().VMaxPltRinseback.Get() << endmsg;
      _TransitionStatus = 1;
      finished          = true;
   }

   //
   //
   //  See if it's time to turn the return pump back on
   //
   if (    _Evacuating
           && ( pd.Volumes().Vp.Get() > _EvacuationLimit ) )
   {
      MidCycleSwitchMsg msg(MessageBase::SEND_GLOBAL);
      msg.send(SWITCH_TO_RETURN);
      _Evacuating = false;
   }

   // Check for air seen at the low-level sensor
   if (    !_Evacuating
           && pd.SafetyControlReservoirIsEmpty() )
   {
      if ( pd.Volumes().Vreturn.Get() < _EvacuationMinVolume )
      {
         DataLog(log_level_proc_rinseback_info) << "PlsRinsebackEvacuate ended with reservoir empty" << endmsg;
         _TransitionStatus = 1;
         finished          = true;
      }
      else
      {
         _Evacuating          = true;
         _EvacuationLimit     = pd.Volumes().Vp.Get() + CobeConfig::data().RinsebackVpevhold;
         _EvacuationMinVolume = pd.Volumes().Vreturn.Get() + CobeConfig::data().RinsebackVrpevmin;
      }
   }

   if ( finished )
   {
      ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete status = pd.PlasmaRinseback().Status.Get();
      pd.PlasmaRinseback().Status.Set( (ProcPlsRinseback_CDS::PlsRinsebackSubstateComplete)(status | ProcPlsRinseback_CDS::Evacuate) );
   }

   return NORMAL;
}

int PlsRinsebackEvacuate::preEnter ()
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

   _EvacuationMinVolume = -1.0f; // don't count until evac process started

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

int PlsRinsebackEvacuate::postExit ()
{
   ProcData pd;

   //
   // Force all pumps to zero waiting for operator confirmation of run complete
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().RPM(0.0f);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);

   //
   // The cassette has now been used and should be considered
   //  in a used state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState =  ProcTrimaSet_CDS::USED;
   pd.MakeTrimaSetReadable();

   if ( _TransitionStatus == 1 )
   {

      // If we're finished with rinseback and the volume shows greater than 0 and isn't too high, set it to the
      // original requested plasma volume so the operator doesn't think the rinseback didn't finish (IT 10235)
      float remainingPlasma = pd.Volumes().PlasmaVolume.Get() - pd.PlasmaRinseback().VPlasmaTarget.Get();

      if ( remainingPlasma > 0.0 && remainingPlasma < 1.0)
      {
         pd.MakeVolumesWritable();
         pd.Volumes().PlasmaVolume.Set(pd.PlasmaRinseback().VPlasmaTarget.Get());
         pd.MakeVolumesReadable();
      }

      // Tell GUI that you are finished with procedure.
      RunCompleteMsg rcmsg(MessageBase::SEND_GLOBAL);
      rcmsg.send(1);

      // Set the rinseback completed status flag for VIP.
      pd.MakeRunWritable();
      pd.Run().RinsebackComplete.Set(RINSEBACK_SUCCESSFUL_PLASMA);
      pd.MakeRunReadable();
   }

   //
   // Reset the logging flag so the next time thru the logging will happen again.
   //
   _LogFlag = true;

   return NORMAL;
}

/* FORMAT HASH 528dd691332aa2e6ac2ff698457d42b8 */
