/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Rinseback.cpp
 *
 */

#include "rinseback.h"
#include <math.h>
#include "cobeconfig.h"
#include "procdata.h"
#include "volumeacc.h"

DEFINE_STATE(Rinseback);

Rinseback::Rinseback()
   : StateAbs(),
     _MaxVreturn(0.0f),
     _evacuating(0),
     _evacuationLimit(0.0f),
     _evacuationMinVolume(0.0f),
     _valveVolume(0.0f),
     _Qreturn(0.0f),
     _QreturnMax(0.0f),
     _VacTotal(0.0f),
     _VTotal(0.0f),
     _IRMax(0.0f)
{}

Rinseback::Rinseback(const Rinseback& state)
   : StateAbs(state),
     _MaxVreturn(state._MaxVreturn),
     _evacuating(state._evacuating),
     _evacuationLimit(state._evacuationLimit),
     _evacuationMinVolume(state._evacuationMinVolume),
     _valveVolume(state._valveVolume),
     _Qreturn(0.0f),
     _QreturnMax(0.0f),
     _VacTotal(0.0f),
     _VTotal(0.0f),
     _IRMax(0.0f)
{
   Rinseback::copyOver(state);
}

Rinseback::~Rinseback()
{
   Rinseback::cleanup();
}

int Rinseback::transitionStatus ()
{
   return _TransitionStatus;
}

int Rinseback::preProcess ()
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
   double timeToDeliverAC = _VacTotal / VacPerMin;

   float  Qreturn;
   if ( timeToDeliverAC <= 0.0 )                           // Can't happen, but better SAFE than sorry
      Qreturn = VacPerMin;                                 //  ( assume All fluid in reservoir is AC )
   else
      //
      // Command the pump to run at the maximum safe delivery rate
      //
      Qreturn = _VTotal / timeToDeliverAC;


   // Ensure that the commanded flow is not greater then the
   // current allowed return pump speed.
   if (Qreturn > pd.Adjustments().Return.CurrentCap.Get()) Qreturn = pd.Adjustments().Return.CurrentCap.Get();
   if (Qreturn > _QreturnMax) Qreturn = _QreturnMax;

   // Setup the reservoir input flow rate...
   // Exceed the return pump outlet speed to ensure that you fully
   // drain the channel before draining the reservoir.
   float Qreservoir = Qreturn + CobeConfig::data().RinsebackQcolMargin;
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
   if ( _evacuating || pd.SafetyControlReservoirIsEmpty() )
   {
      Qreturn = 0.0f;
   }


   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(Qreturn);
   pd.Orders().Qin(0.0f);
   if (pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC)
   {
      pd.Orders().Qplasma(Qreservoir);
      pd.Orders().Qcollect(0.0f);
   }
   else
   {
      float Qch = (Qreturn + CobeConfig::data().RinsebackQcolMargin) / 2.0f;
      if (Qch > CobeConfig::data().RinsebackQchamber) Qch = CobeConfig::data().RinsebackQchamber;

      //
      // Keep the plasma pump running at a speed greater than 1.0 ml/min
      if ( Qreservoir - Qch >= 1.0f )
         pd.Orders().Qplasma(Qreservoir - Qch);
      //
      // ... if not, then command it to 0.0
      else
         pd.Orders().Qplasma(0.0f);

      pd.Orders().Qcollect(Qreservoir);
   }
   pd.Orders().RPM(0.0f);

   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   static bool loggedOnce = false;
   if ( !loggedOnce )
   {
      DataLog(log_level_proc_rinseback_info) << "Rinseback started at Qrp=" << Qreturn
                                             << " (" << pd.Adjustments().Return.CurrentCap.Get()
                                             << " " << _QreturnMax << ")"
                                             << " IR=" << IR
                                             << " (" << pd.Run().LastRunInfusion.Get()
                                             << " " << pd.Adjustments().Tingling.CurrentCap.Get()
                                             << " " << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << endmsg;
      loggedOnce = true;
   }

   return NORMAL;
}

int Rinseback::postProcess ()
{
   ProcData pd;

   if (pd.Volumes().Vreturn.Get() > _MaxVreturn)
   {
      DataLog(log_level_proc_rinseback_info) << "Rinseback ended at max volume of " << _MaxVreturn << endmsg;
      _TransitionStatus = 1;
   }

   //
   //
   //  See if it's time to turn the return pump back on
   //
   if (_evacuating && (pd.Volumes().Vp.Get() > _evacuationLimit))
   {
      MidCycleSwitchMsg midCycleSwitchMsg(MessageBase::SEND_GLOBAL);
      midCycleSwitchMsg.send(SWITCH_TO_RETURN);
      _evacuating = 0;
   }

   // Check for air seen at the low-level sensor
   if ((!_evacuating) && (pd.SafetyControlReservoirIsEmpty()))
   {
      if (pd.Volumes().Vreturn.Get() < _evacuationMinVolume)
      {
         DataLog(log_level_proc_rinseback_info) << "Rinseback ended with reservoir empty" << endmsg;
         _TransitionStatus = 1;
      }
      else
      {
         _evacuating          = 1;
         _evacuationLimit     = pd.Volumes().Vp.Get() + CobeConfig::data().RinsebackEvacuationHoldVolume;
         _evacuationMinVolume = pd.Volumes().Vreturn.Get() + CobeConfig::data().RinsebackEvacuationMinVolume;
      }
   }

   //
   //
   //  Check to see if we've hit our valve motion threshold
   //
   if (CobeConfig::data().RinsebackValveVolume > 0.0f)
   {
      if ((pd.Volumes().RBCVolume.Get() == 0.0f) &&
          (pd.Volumes().Vp.Get() > _valveVolume))
      {
         pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
      }
   }

   return NORMAL;
}

int Rinseback::preEnter ()
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
   pd.Run().RinsebackStarted.Set(1);
   pd.MakeRunReadable();

   //
   //   Limit the return rinseback volume to avoid possible failures of the
   //   low level sensor.  See IT3764 for details. Note that the reservoir fill
   //   volume may be negative if we ended during a return cycle.
   //
   float resVolume             = (float)fabs(pd.Cycle().Vres);
   float distance_to_ul_sensor = (pd.Run().DrawCycle.Get() ?
                                  (CobeConfig::data().ReservoirVolume - resVolume)
                                  : resVolume);
   _MaxVreturn = pd.ExtracorporealVolume()                    // a full set
                 + CobeConfig::data().NominalReturnLineVolume // allow all of return line
                 + 5.0f                                       // return prime above low level sensor
                 - pd.TotalResidualVolume()                   // without residuals
                 - distance_to_ul_sensor;


   DataLog(log_level_proc_rinseback_info) << "Rinseback _MaxVreturn  Limit:"  << _MaxVreturn << endmsg;

   _evacuationMinVolume = -1.0f; // don't count until evac process started
   _valveVolume         = pd.Volumes().Vp.Get() + CobeConfig::data().RinsebackValveVolume;

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
   if (_IRMax < MIN_INFUSION_RATE) _IRMax = MIN_INFUSION_RATE;

   float Hin      = pd.Run().AdjustedHct.Get() * (1.0f - 1.0f / ratio);
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
   float VRes     = pd.Cycle().Vres.Get();
   float VResLast = pd.Cycle().VresLast.Get();
   if ( VRes < 0.0f && VResLast > 0.0f ) // return cycle
   {
      VRes = VResLast + VRes - pd.Volumes().VRetRinseback.Get() + CobeConfig::data().NominalReturnLineVolume;
   }
   else // draw cycle
   {
      VRes = fabs(VRes) - pd.Volumes().VRetRinseback.Get() + CobeConfig::data().NominalReturnLineVolume;
   }
   float VacRes = facb * VRes * ( 1 - pd.Cycle().ResHct.Get() );

   //
   // Calculate the volume of AC in the rest of the set
   //
   float VSet   = pd.NominalFillVolume();
   VSet -= CobeConfig::data().ReservoirVolume + CobeConfig::data().NominalReturnLineVolume;
   float VacSet = facb * VSet * ( 1 - pd.Run().AdjustedHct.Get() );

   _VacTotal = VacRes + VacSet;
   _VTotal   = VRes + VSet;

   DataLog(log_level_proc_rinseback_info) << "Rinseback: ratio=" << ratio << " IRMax=" << _IRMax
                                          << " QrpLimit=" << QrpLimit << " facb=" << facb
                                          << " VacTotal=" << _VacTotal << " VTotal=" << _VTotal
                                          << endmsg;
   return NORMAL;
}

int Rinseback::postExit ()
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

   if ( _TransitionStatus == 1 )
   {
      pd.MakeRunWritable();
      pd.Run().RinsebackComplete.Set(RINSEBACK_SUCCESSFUL_NORMAL);
      pd.MakeRunReadable();

      // Tell GUI that you are finished with procedure.
      RunCompleteMsg RunFinished(MessageBase::SEND_GLOBAL);
      RunFinished.send(1);
   }

   //
   // The cassette has now been used and should be considered
   //  in a used state ...
   //
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::USED);

   return NORMAL;
}

void Rinseback::copyOver (const Rinseback&)
{}

void Rinseback::cleanup ()
{
   //
   // Delete the dynamic memory ...
   //
}

/* FORMAT HASH 2945442f9384492bb564d818ad52f944 */
