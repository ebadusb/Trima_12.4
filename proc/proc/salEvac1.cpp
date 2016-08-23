/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salEvac1.h"
#include "volumeacc.h"

DEFINE_STATE(SalEvac1);

SalEvac1::SalEvac1()
   : StateAbs(),
     _plasmaVolume(0.0f),
     _returnVolume(0.0f),
     _transitionNow(false),
     _totalPlasmaVolume(0.0f),
     _returnVolumeForULSCutout(0.0f),
     _ulsCutout(false),
     _Qr(0.0f),
     _justEmptyNow(false),
     _primed(false),
     _qp_prime_volm(0.0f),
     _QreturnMax(0.0f),
     _IRMax(0.0f),
     _VacRes(0.0f),
     _VRes(0.0f)
{}

SalEvac1::~SalEvac1()
{}

int SalEvac1::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;

}

int SalEvac1::preProcess ()
{
   ProcData pd;

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


   // float Qreturn;
   if ( timeToDeliverAC <= 0.0 )                       // Can't happen, but better SAFE than sorry
      _Qr = VacPerMin;                                 //  ( assume All fluid in reservoir is AC )
   else
      //
      // Command the pump to run at the maximum safe delivery rate
      //
      _Qr = _VRes / timeToDeliverAC;


   // Ensure that the commanded flow is not greater then the
   // current allowed return pump speed.
   if (_Qr > pd.Adjustments().Return.CurrentCap.Get())
      _Qr = pd.Adjustments().Return.CurrentCap.Get();

   if (_Qr > _QreturnMax )
      _Qr = _QreturnMax;


   //
   //
   //   Log pertinant information once (it will only change if the
   //   operator makes adjustments during rinseback)
   //
   static bool logDone = false;
   if (!logDone)
   {
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Saline Evac#1 started at Qrp=" << _Qr << " (" << pd.Adjustments().Return.CurrentCap.Get() << " " << _QreturnMax << ")"
                                             << " IR=" << IR << " (" << pd.Run().LastRunInfusion.Get() << " " << pd.Adjustments().Tingling.CurrentCap.Get() << " " << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << endmsg;
      logDone = true;
   }

   if ( (pd.Volumes().RBCDose.Get() < 1.0) && centrifugeStopped() )
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   }
   else
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   }

   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(0.0f);


   DataLog(log_level_proc_rinseback_info) << "========================"  << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _ulsCutout=" << _ulsCutout << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _primed=" << _primed << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _justEmptyNow=" << _justEmptyNow << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _totalPlasmaVolume=" << _totalPlasmaVolume << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _returnVolumeForULSCutout=" << _returnVolumeForULSCutout << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _plasmaVolume=" << _plasmaVolume << endmsg;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: _returnVolume=" << _returnVolume << endmsg;


   if (_returnVolumeForULSCutout < 10.0f & _ulsCutout)
   {
      pd.Orders().Qplasma(0.0f);
   }
   else
   {
      _ulsCutout = false;

      if (_justEmptyNow)
         pd.Orders().Qplasma(0.0f);
      else
         pd.Orders().Qplasma(CobeConfig::data().SrEvac1Qp);

   }


   if (!_primed)
   {
      _qp_prime_volm += pd.Status().PlasmaPump.dV();
      if (_qp_prime_volm >= 15.0f || _justEmptyNow)
      {
         if (!_primed)
         {
            DataLog(log_level_proc_rinseback_info) << "Primed now. " << endmsg;
         }
         _primed        = true;
         _qp_prime_volm = 0.0f;  // reset
      }

   }


   if (pd.SafetyControlReservoirIsEmpty())
   {
      if (_primed)
      {
         DataLog(log_level_proc_rinseback_info) << "Not primed now. Qr=0" << endmsg;
      }
      _primed = false;
      pd.Orders().Qrp(0.0f);
   }
   else
   {
      if (_primed || _ulsCutout)
      {
         pd.Orders().Qrp(_Qr);
      }
      else
      {
         pd.Orders().Qrp(0.0f);
      }
   }


   return NORMAL;

}

int SalEvac1::postProcess ()
{

   ProcData pd;

   _returnVolume             += pd.Status().ReturnPump.dV();
   _plasmaVolume             += pd.Status().PlasmaPump.dV();
   _totalPlasmaVolume        += pd.Status().PlasmaPump.dV();
   _returnVolumeForULSCutout += pd.Status().ReturnPump.dV();

   if (_totalPlasmaVolume > CobeConfig::data().SrEvac1MinVp)
   {
      if (_plasmaVolume >= CobeConfig::data().SrEvac1VpLimit)
      {
         if (_returnVolume < CobeConfig::data().SrEvac1VrLimit)
         {
            _transitionNow = true;
         }
         else
         {

            // redo another VpLimit and check Vr
            _returnVolume = 0.0f;
            _plasmaVolume = 0.0f;

         }
      }
   }

   if ( (_totalPlasmaVolume > CobeConfig::data().SrEvac1MaxVp ) && !_justEmptyNow)
   {
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Evacuatin res, exceeded total VP" << endmsg;
      _justEmptyNow = true;
   }


   if (pd.Status().ReservoirIsFull() && !_ulsCutout)
   {
      _ulsCutout                = true;
      _returnVolumeForULSCutout = 0.0f;

      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: ULL Sensor Hit, Qp will be stopped" << endmsg;
   }


   if (pd.Status().ReservoirIsEmpty()
       && (_Qr > 0.0)
       && (_Qr < 40.0)
       )
   {
      _transitionNow = true;
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Transition, Res Empty and pump speed < 40.0mL/min" << endmsg;
   }


   if (pd.Status().ReservoirIsEmpty() && _justEmptyNow)
   {

      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Exiting because exceeded total VP and res empty" << endmsg;
      _transitionNow = true;
   }


   // update the limiting monitor volume
   float currVolm = pd.Volumes().VRetDuringSalineRinsebackRinseback.Get();

   pd.MakeVolumesWritable();
   pd.Volumes().VRetDuringSalineRinsebackRinseback.Set(currVolm + pd.Status().ReturnPump.dV() - pd.Status().InletPump.dV()  ) ;



   return NORMAL;
}

int SalEvac1::preEnter ()
{
   ProcData pd;

   /*
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


   float Hin = pd.Run().AdjustedHct.Get()*(1.0f - 1.0f/ratio);
   float QrpLimit = 520.0f * (1.0f - Hin);

   _QreturnMax = _IRMax * ratio * (pd.Donor().TBV.Get() / 1000.0f);

   if (_QreturnMax > QrpLimit) _QreturnMax = QrpLimit;

   //
   //
   //  Safety has a hardcoded limit of 90ml/min.  Do not exceed this
   //  limit or safety will shut the system down.
   //
   if (_QreturnMax > 90.0f) _QreturnMax = 90.0f;

   DataLog( log_level_proc_rinseback_info ) << "SalineRinseback: _QreturnMax = " << _QreturnMax << endmsg;
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
   float vRes = pd.Cycle().Vres.Get();
   float vResLast = pd.Cycle().VresLast.Get();
   if ( vRes < 0.0f && vResLast > 0.0f ) // return cycle
   {
      _VRes = vResLast + vRes + CobeConfig::data().NominalReturnLineVolume;
   }
   else // draw cycle
   {
      _VRes = fabs( vRes ) + CobeConfig::data().NominalReturnLineVolume;
   }
   _VacRes = facb * _VRes * ( 1 - pd.Cycle().ResHct.Get() );

   DataLog( log_level_proc_rinseback_info ) << "SalineRinseback: SalEvac1: ratio=" << ratio << " IRMax=" << _IRMax
                                            << " QrpLimit=" << QrpLimit << " facb=" << facb
                                            << " VacRes=" << _VacRes << " VRes=" << _VRes << " MaxQr=" << _QreturnMax
                                            << endmsg;

   // set these values for use in evac2 also
    pd.MakeRunWritable();
    pd.Run().QReturnMax.Set( _QreturnMax );
    pd.Run().IRMax.Set( _IRMax );


    pd.MakeVolumesWritable();
    pd.Volumes().Vres.Set( _VRes );
    pd.Volumes().VacRes.Set( _VacRes );

    */


   // these are all calulated and saved in Evac1::PreEnter.....
   _QreturnMax = pd.Run().QReturnMax.Get();
   _IRMax      = pd.Run().IRMax.Get();
   _VRes       = pd.Volumes().Vres.Get();
   _VacRes     = pd.Volumes().VacRes.Get();


   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: SalEvac1: VacRes=" << _VacRes
                                          << " VRes=" << _VRes
                                          << " MaxQr=" << _QreturnMax
                                          << endmsg;





   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   if ( (pd.Volumes().RBCDose.Get() < 1.0) && centrifugeStopped() )
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   }
   else
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   }


   return NORMAL;
}

int SalEvac1::postExit ()
{


   ProcData pd;
   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: limit checked at "
                                          << pd.Volumes().VRetDuringSalineRinsebackRinseback.Get()
                                          << "  Max="
                                          << pd.Volumes().VMaxSalRinseback.Get()
                                          << endmsg;

   _transitionNow = false;
   return NORMAL;
}


bool SalEvac1::centrifugeStopped ()
{

   ProcData pd;
   if (pd.Status().CentrifugeRPM() < 200.0f)
      return true;
   else
      return false;

}

/* FORMAT HASH 6ef1f1ef10627e31257ad20cf304242d */
