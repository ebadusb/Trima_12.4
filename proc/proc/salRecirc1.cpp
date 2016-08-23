/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salRecirc1.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salRecirc1.h"
#include "volumeacc.h"

DEFINE_STATE(SalRecirc1);

SalRecirc1::SalRecirc1()
   : StateAbs(),
     _inletVolume(0.0f),
     _returnVolume(0.0),
     _transitionNow(false),
     _ulsSpeedUp(false)
{}

SalRecirc1::~SalRecirc1()
{}

int SalRecirc1::transitionStatus ()
{

   ProcData pd;
   // skip out if no RBCs collected.
   if (pd.Volumes().RBCDose.Get() < 1.0)
   {
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Skipping Recirc1, no RBC collected" << endmsg;
      return 1;
   }


   if (_transitionNow)
      return 1;

   return NO_TRANSITION;
}

int SalRecirc1::preProcess ()
{
   ProcData pd;
   float    modSpeed = CobeConfig::data().SrRecirc1Mod;

   if (modSpeed > pd.Run().QReturnMax.Get())
      modSpeed = pd.Run().QReturnMax.Get();

   pd.Orders().Qplasma(CobeConfig::data().SrRecirc1Qp);

   if ( pd.Status().ReservoirIsFull() || _ulsSpeedUp )
   {

      if (pd.Status().ReservoirIsEmpty())
         pd.Orders().Qrp(0.0f);
      else
         pd.Orders().Qrp(CobeConfig::data().SrRecirc1Qin + modSpeed);

      _ulsSpeedUp   = true;
      _returnVolume = 0.0f;  // start the volume check for this speed up

   }
   else
   {

      if (pd.Status().ReservoirIsEmpty())
         pd.Orders().Qrp(0.0f);
      else
         pd.Orders().Qrp(CobeConfig::data().SrRecirc1Qin);

   }

   if (pd.SafetyControlReservoirIsEmpty())
      pd.Orders().Qin(0.0f);
   else
      pd.Orders().Qin(CobeConfig::data().SrRecirc1Qin);


   return NORMAL;
}

int SalRecirc1::postProcess ()
{
   ProcData pd;
   _inletVolume += pd.Status().InletPump.dV();

   if (_ulsSpeedUp)
      _returnVolume += pd.Status().ReturnPump.dV();

   if (_returnVolume >= CobeConfig::data().SrRecirc1Vr)
      _ulsSpeedUp = false;

   if (  (_inletVolume >= CobeConfig::data().SrRecirc1Vin)
         || (pd.Status().ReservoirIsEmpty()) )
      _transitionNow = true;

   // update the limiting monitor volume
   float currVolm = pd.Volumes().VRetDuringSalineRinsebackRinseback.Get();

   pd.MakeVolumesWritable();
   pd.Volumes().VRetDuringSalineRinsebackRinseback.Set(currVolm + pd.Status().ReturnPump.dV() - pd.Status().InletPump.dV()  ) ;


   return NORMAL;
}

int SalRecirc1::preEnter ()
{
   ProcData pd;



   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);


   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);

   // from here out the centrifuge is off.....
   // pd.Orders().RPM(0.0f);


   //
   //   Zero out the return volume.
   //
   VolumeAcc::ClearReturn();

   pd.MakeRunWritable();
   pd.Run().RinsebackStarted = true;
   pd.MakeRunReadable();


   return NORMAL;
}

int SalRecirc1::postExit ()
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

/* FORMAT HASH 331458269524e65a2e99b3c975991e56 */
