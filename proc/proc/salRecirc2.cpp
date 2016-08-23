/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salRecirc2.h"
#include "volumeacc.h"

DEFINE_STATE(SalRecirc2);

SalRecirc2::SalRecirc2()
   : StateAbs(),
     _inletVolume(0.0),
     _transitionNow(false)
{}

SalRecirc2::~SalRecirc2()
{}

int SalRecirc2::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;
}

int SalRecirc2::preProcess ()
{
   ProcData pd;

   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(CobeConfig::data().SrRecirc2Qp);

   if (pd.SafetyControlReservoirIsEmpty())
   {
      pd.Orders().Qrp(0.0f);
      pd.Orders().Qin(0.0f);
   }
   else
   {
      pd.Orders().Qin(CobeConfig::data().SrRecirc2Qin);
      pd.Orders().Qrp(CobeConfig::data().SrRecirc2Qr);
   }

   return NORMAL;
}

int SalRecirc2::postProcess ()
{
   ProcData pd;


   _inletVolume += pd.Status().InletPump.dV();



   if (  (_inletVolume >= CobeConfig::data().SrRecirc2Vin)
         || (pd.Status().ReservoirIsEmpty()) )
      _transitionNow = true;



   // update the limiting monitor volume
   float currVolm = pd.Volumes().VRetDuringSalineRinsebackRinseback.Get();

   pd.MakeVolumesWritable();
   pd.Volumes().VRetDuringSalineRinsebackRinseback.Set(currVolm + pd.Status().ReturnPump.dV() - pd.Status().InletPump.dV()  ) ;


   return NORMAL;
}

int SalRecirc2::preEnter ()
{
   ProcData pd;



   return NORMAL;
}

int SalRecirc2::postExit ()
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

/* FORMAT HASH c6842f16f25db9016dac56605c597c07 */
