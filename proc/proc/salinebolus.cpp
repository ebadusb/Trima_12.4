/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      salinebolus.cpp
 *
 */

#include "salinebolus.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(SalineBolus);

SalineBolus::SalineBolus()
   : RecoveryAbs(),
     _RecoveryReqMsg(Callback<SalineBolus>(this, &SalineBolus::recoveryRequest) ),
     _RecoveryRequested(0),
     _BolusComplete(0),
     _BolusVolume(0.0f)
{}

SalineBolus::SalineBolus(const SalineBolus& rec)
   : RecoveryAbs(rec),
     _RecoveryReqMsg(Callback<SalineBolus>(this, &SalineBolus::recoveryRequest) ),
     _RecoveryRequested(rec._RecoveryRequested),
     _BolusComplete(rec._BolusComplete),
     _BolusVolume(0.0f)
{
   copyOver(rec);
}

SalineBolus::~SalineBolus()
{
   cleanup();
}

int SalineBolus::init ()
{
   return 1;
}

int SalineBolus::doRecovery ()
{
   ProcData pd;

   //
   // The recovery started, reset the flag ...
   _RecoveryRequested = 0;

   //
   // Put the valves in the return postion ...
   //

   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);     //

   //
   // Command the centrifuge to pause speed ...
   //
   pd.Orders().RPM(CobeConfig::data().CentrifugePumpsPausedSpeed);

   //
   // Stop all pumps ...
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);

   //
   // Command the replacement pump ...
   //
   pd.Orders().Qcollect(pd.Predict().Qc(0, SubstateMap::alg(NOCOLLECT_STATE)));

   //
   // Grab the current replacement fluid volume ...
   //
   if ( _BolusVolume == 0.0f )
   {
      _BolusVolume = pd.Volumes().VSalineBolus.Get();
   }

   if ( pd.Volumes().VSalineBolus.Get() - _BolusVolume < 100.0f )
   {
      if ( pd.Volumes().VSalineBolus.Get() - _BolusVolume > 12.0f ||
           pd.Status().ReservoirIsFull() )
      {
         //
         // Start the return pump at the same
         //  commanded speed as the replacement pump ...
         //
         pd.Orders().Qrp(pd.Orders().Qcollect() );
      }
   }
   else
   {
      //
      // The bolus has been given ...
      //
      _BolusComplete = 1;
      _BolusVolume   = 0.0f;

      DataLog(log_level_proc_recovery_info) << "Total bolus volume: " << pd.Volumes().VSalineBolus.Get() << endmsg;
   }


   return 1;
}

int SalineBolus::mayEnter () const
{
   ProcData pd;

   if ( !pd.Status().CassIsPltPlsRBC() )
   {
      return 1;
   }
   return 0;
}

void SalineBolus::recoveryRequest ()
{
   int status = _RecoveryReqMsg.getData();

   if ( status )
   {
      //
      // Set the flag ...
      _RecoveryRequested = 1;
      _BolusComplete     = 0;
   }
   else
   {
      //
      // Unset the flag ...
      _RecoveryRequested = 0;
      reset();
   }
}

int SalineBolus::shouldRecover ()
{
   if ( _RecoveryRequested ||
        ( active() && !_BolusComplete )
        )
   {
      return 1;
   }

   return 0;
}

void SalineBolus::reset ()
{
   _RecoveryRequested = 0;
   _BolusComplete     = 1;
   _BolusVolume       = 0.0f;
}

void SalineBolus::copyOver (const SalineBolus&)
{}

void SalineBolus::cleanup ()
{}

/* FORMAT HASH 0fa42292f4f685568fb239f5faab4806 */
