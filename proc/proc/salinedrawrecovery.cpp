/*
 * Copyright (c) 2010 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      salinedrawrecovery.cpp
 *
 *  This recovery is initiated to re-prime the saline line and deliver fluid to the donor
 *  in the case of a "draw cycle too long" alert on an RBC/Plasma set.
 */

#include "salinedrawrecovery.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(SalineDrawRecovery);

SalineDrawRecovery::SalineDrawRecovery()
   : RecoveryAbs(),
     _RecoveryReqMsg(Callback<SalineDrawRecovery>(this, &SalineDrawRecovery::recoveryRequest) ),
     _RecoveryRequested(0),
     _RecoveryComplete(0),
     _RecoveryVolume(0.0f)
{}

SalineDrawRecovery::SalineDrawRecovery(const SalineDrawRecovery& rec)
   : RecoveryAbs(rec),
     _RecoveryReqMsg(Callback<SalineDrawRecovery>(this, &SalineDrawRecovery::recoveryRequest) ),
     _RecoveryRequested(rec._RecoveryRequested),
     _RecoveryComplete(rec._RecoveryComplete),
     _RecoveryVolume(0.0f)
{
   copyOver(rec);
}

SalineDrawRecovery::~SalineDrawRecovery()
{
   cleanup();
}

int SalineDrawRecovery::init ()
{
   return 1;
}

int SalineDrawRecovery::doRecovery ()
{
   ProcData pd;

   //
   // The recovery started, reset the flag ...
   if (_RecoveryRequested)
   {
      _RecoveryRequested = 0;
      //
      //
      //  Initiate a return cycle
      //
      MidCycleSwitchMsg cycleswitch(MessageBase::SEND_GLOBAL);
      cycleswitch.send(SWITCH_TO_RETURN);
      pd.SwitchtoReturn();
   }

   //
   // Grab the current replacement fluid volume for logging purposes...
   //
   if ( _RecoveryVolume == 0.0f )
   {
      _RecoveryVolume = pd.Volumes().VSalineBolus.Get();
   }

   if (pd.Run().DrawCycle.Get()) //  we are done
   {
      float volumedelivered = pd.Volumes().VSalineBolus.Get() - _RecoveryVolume;
      DataLog(log_level_proc_recovery_info) << "Total saline recovery volume: " << volumedelivered << endmsg;

      _RecoveryComplete = 1;
      _RecoveryVolume   = 0.0f;

   }
   else // recovery in progress - command pumps
   {
      //
      // Command pumps appropriate for a return cycle.  Note sanity check
      // to make sure that the reservoir eventually empties.
      //
      float Qc    = pd.Predict().Qc(0, SubstateMap::alg(NOCOLLECT_STATE)); // note assumes Qc runs during return
      float Qrp   = pd.Predict().Qrp(0, SubstateMap::alg(NOCOLLECT_STATE));
      float maxQc = Qrp - 10.f;
      if (Qc > maxQc) Qc = maxQc;

      pd.Orders().Qac(0.0f);
      pd.Orders().Qin(0.0f);
      pd.Orders().Qplasma(0.0f);
      pd.Orders().Qcollect(Qc);
      pd.Orders().Qrp(Qrp);

      //
      //
      // Note no change to valves or centrifuge speed for this recovery.
      //
   }

   return 1;
}

int SalineDrawRecovery::mayEnter () const
{
   ProcData pd;

   if ( !pd.Status().CassIsPltPlsRBC() )
   {
      return 1;
   }
   return 0;
}

void SalineDrawRecovery::recoveryRequest ()
{
   int status = _RecoveryReqMsg.getData();

   if ( status )
   {
      //
      // Set the flag ...
      _RecoveryRequested = 1;
      _RecoveryComplete  = 0;
   }
   else
   {
      //
      // Unset the flag ...
      _RecoveryRequested = 0;
      reset();
   }
}

int SalineDrawRecovery::shouldRecover ()
{
   if ( _RecoveryRequested ||
        ( active() && !_RecoveryComplete )
        )
   {
      return 1;
   }

   return 0;
}

void SalineDrawRecovery::reset ()
{
   _RecoveryRequested = 0;
   _RecoveryComplete  = 1;
   _RecoveryVolume    = 0.0f;
}

void SalineDrawRecovery::copyOver (const SalineDrawRecovery&)
{}

void SalineDrawRecovery::cleanup ()
{}

/* FORMAT HASH 9690e101baea1c9c994d065b646a8fba */
