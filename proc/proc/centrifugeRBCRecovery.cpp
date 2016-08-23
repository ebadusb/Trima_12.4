/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeslow.cpp
 *
 */

#include "centrifugeRBCRecovery.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"


DEFINE_OBJ(CentrifugeRBCRecovery);

CentrifugeRBCRecovery::CentrifugeRBCRecovery()
   : RecoveryAbs(),
     _inletVolume(0.0f),
     _CentErrAlarm(CENTRIFUGE_HARDWARE_FAILURE),
     _imDisabled(false)
{}

CentrifugeRBCRecovery::CentrifugeRBCRecovery(const CentrifugeRBCRecovery& rec)
   : RecoveryAbs(),
     _inletVolume(0.0f),
     _CentErrAlarm(CENTRIFUGE_HARDWARE_FAILURE),
     _imDisabled(false)
{}

CentrifugeRBCRecovery::~CentrifugeRBCRecovery()
{}

int CentrifugeRBCRecovery::init ()
{
   return 1;
}

int CentrifugeRBCRecovery::doRecovery ()
{
   // If this is disabled, then don't do it.
   if (disabled()) return 0;

   ProcData pd;

   if (centrifugeIsUpToSpeed())
   {
      DataLog(log_level_proc_recovery_info) << "Centrifuge reached speed after: " << _inletVolume << " of Qin" << endmsg;
      disable(true);   // this recovery is done we dont need it anymore...

      return 0;   // once we hit the speed we're done.
   }

   _inletVolume += pd.Status().InletPump.dV();
   //
   // Close the valves ...
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);

   pd.Orders().RPM(CobeConfig::data().RPMCollect);

   //
   // Command the pumps to stop ...
   //
   pd.Orders().Qplasma (0.0f);
   pd.Orders().Qcollect(0.0f);

   //
   //  go thru the cycles if needed...
   //
   if ( pd.Run().DrawCycle.Get() )
   {
      pd.Orders().Qin(pd.Predict().Qin(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qac(pd.Predict().Qac(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE)));
      pd.Orders().Qrp(0.0f);
   }
   else
   {
      pd.Orders().Qin(0.0f);
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(pd.Predict().Qrp(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE) ));
   }

   ///////////////////////////////////////////
   //  bail out condition ......
   if (_inletVolume > CobeConfig::data().CentRecoveryMaxVolume)
   {
      pd.Orders().Qplasma (0.0f);
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qrp(0.0f);
      pd.Orders().Qin(0.0f);
      pd.Orders().Qac(0.0f);

      DataLog(log_level_proc_recovery_info) << "Centrifuge not recovering speed after: " << _inletVolume << " of Qin" << endmsg;
      _CentErrAlarm.setAlarm();
      return 0;
   }
   ///////////////////////////////////////
   return 1;
}



int CentrifugeRBCRecovery::mayEnter ()  const
{
   ProcData pd;
   if ( !pd.Status().CassIsPltPlsRBC() )
      return 0;
   return 1;
}



int CentrifugeRBCRecovery::shouldRecover ()
{
   // If this is disabled, then we should not recover.
   if (disabled())
      return 0;

   ProcData pd;
   if ( pd.Run().Substate.Get() > SS_EXTENDED_PLASMA )
   {
      if (!centrifugeIsUpToSpeed())
      {
         return 1;
      }
      else
      {

         DataLog(log_level_proc_recovery_info) << "Centrifuge is at speed. RPMs=" << pd.Status().CentrifugeRPM() << endmsg;

         // we are here and the cent is OK.... so give it up.
         disable(true);   // this recovery is done we dont need it anymore...
         return 0;
      }
   }

   return 0;
}


bool CentrifugeRBCRecovery::centrifugeIsUpToSpeed () const
{
   ProcData pd;
   // within 1% of targeted speed.
   return (pd.Status().CentrifugeRPM() >= (CobeConfig::data().RPMCollect - CobeConfig::data().RPMCollect * 0.01f));
}


bool CentrifugeRBCRecovery::disabled () const
{
   return _imDisabled;
}

/* FORMAT HASH eb81a91c6fab2de3b1fe402d0fae041d */
