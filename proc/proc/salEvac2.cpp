/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salEvac2.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salEvac2.h"
#include "volumeacc.h"

DEFINE_STATE(SalEvac2);

SalEvac2::SalEvac2()
   : StateAbs(),
     _plasmaVolume(0.0f),
     _returnVolume(0.0),
     _transitionNow(false),
     _totalPlasmaVolume(0.0f),
     _justEmptyNow(false),
     _primed(false),
     _qp_prime_volm(0.0f),
     _QreturnMax(0.0f),
     _IRMax(0.0f),
     _VacRes(0.0f),
     _VRes(0.0f)
{}

SalEvac2::~SalEvac2()
{}

int SalEvac2::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;

}

int SalEvac2::preProcess ()
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
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Saline Evac#2 started at Qrp=" << Qreturn << " (" << pd.Adjustments().Return.CurrentCap.Get() << " " << _QreturnMax << ")"
                                             << " IR=" << IR << " (" << pd.Run().LastRunInfusion.Get() << " " << pd.Adjustments().Tingling.CurrentCap.Get() << " " << CobeConfig::data().RinsebackMinInfusion << ")"
                                             << endmsg;
      logDone = true;
   }


   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(0.0f);


   if (_justEmptyNow)
      pd.Orders().Qplasma(0.0f);
   else
      pd.Orders().Qplasma(CobeConfig::data().SrEvac2Qp);


   if (!_primed)
   {
      _qp_prime_volm += pd.Status().PlasmaPump.dV();
      if (_qp_prime_volm >= 15.0f)
      {
         if (!_primed)
         {
            DataLog(log_level_proc_rinseback_info) << "Primed now. " << endmsg;
         }
         _primed        = true;
         _qp_prime_volm = 0.0f;  // reset
      }

   }


   if (pd.SafetyControlReservoirIsEmpty()  )
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
      if (_primed)
      {
         pd.Orders().Qrp(Qreturn);
      }
      else
      {
         pd.Orders().Qrp(0.0f);
      }
   }




   return NORMAL;

}

int SalEvac2::postProcess ()
{

   ProcData pd;


   _returnVolume      += pd.Status().ReturnPump.dV();
   _plasmaVolume      += pd.Status().PlasmaPump.dV();
   _totalPlasmaVolume += pd.Status().PlasmaPump.dV();

   if (_totalPlasmaVolume > CobeConfig::data().SrEvac2MinVp)
   {

      if (_plasmaVolume >= CobeConfig::data().SrEvac2VpLimit)
      {
         if (_returnVolume < CobeConfig::data().SrEvac2VrLimit)
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


   if ( (_totalPlasmaVolume > CobeConfig::data().SrEvac2MaxVp )  && !_justEmptyNow)

   {
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Evacuatin res, exceeded total VP" << endmsg;
      _justEmptyNow = true;
   }

   // update the limiting monitor volume
   float currVolm = pd.Volumes().VRetDuringSalineRinsebackRinseback.Get();

   pd.MakeVolumesWritable();
   pd.Volumes().VRetDuringSalineRinsebackRinseback.Set(currVolm + pd.Status().ReturnPump.dV() - pd.Status().InletPump.dV()  ) ;




   if (pd.Status().ReservoirIsEmpty() && _justEmptyNow)
   {

      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Exiting because exceeded total VP and res empty" << endmsg;
      _transitionNow = true;
   }


   return NORMAL;
}

int SalEvac2::preEnter ()
{
   ProcData pd;


   // these are all calulated and saved in Evac1::PreEnter.....
   _QreturnMax = pd.Run().QReturnMax.Get();
   _IRMax      = pd.Run().IRMax.Get();
   _VRes       = pd.Volumes().Vres.Get();
   _VacRes     = pd.Volumes().VacRes.Get();


   DataLog(log_level_proc_rinseback_info) << "SalineRinseback: SalEvac2: VacRes=" << _VacRes
                                          << " VRes=" << _VRes
                                          << " MaxQr=" << _QreturnMax
                                          << endmsg;






   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   if ( pd.Volumes().RBCDose.Get() < 1.0  )
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_COLLECT);
   }
   else
   {
      pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   }


   return NORMAL;
}

int SalEvac2::postExit ()
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

/* FORMAT HASH da3a02102d14366710c3cf59b00cb758 */
