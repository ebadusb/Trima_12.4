/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      settlechannel.cpp
 *
 */

#include "settlechannel.h"
#include "recoveryman.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "software_cds.h"

DEFINE_OBJ(SettleChannel);

SettleChannel::SettleChannel()
   : RecoveryAbs(),
     _SettleChannelReqMsg(Callback<SettleChannel>(this, &SettleChannel::settleChannelRequest) ),
     _RecoveryRequested(0),
     _ChannelSettled(0),
     _InletVolume(0.0f)
{}

SettleChannel::SettleChannel(const SettleChannel& rec)
   : RecoveryAbs(rec),
     _SettleChannelReqMsg(Callback<SettleChannel>(this, &SettleChannel::settleChannelRequest) ),
     _RecoveryRequested(rec._RecoveryRequested),
     _ChannelSettled(rec._ChannelSettled),
     _InletVolume(rec._InletVolume)
{
   copyOver(rec);
}

SettleChannel::~SettleChannel()
{
   cleanup();
}

int SettleChannel::init ()
{
   return 1;
}

int SettleChannel::doRecovery ()
{
   ProcData pd;

   //
   // Check to see if the inlet pump needs adjustment ...
   //
   if ( pd.Orders().Qin() > CobeConfig::data().SettleChannelQin )
   {
      //
      // Command the inlet pump ...
      //
      float ratio = pd.Orders().Qin() / pd.Orders().Qac();
      DataLog(log_level_proc_debug) << "SettleChannel: ratio->" << ratio << endmsg;

      pd.Orders().Qin(CobeConfig::data().SettleChannelQin);
      DataLog(log_level_proc_debug) << "SettleChannel: Qin->" << pd.Orders().Qin() << endmsg;
      if ( pd.Run().DrawCycle.Get() )
         pd.Orders().Qac(pd.Orders().Qin() / ratio);
      else
         pd.Orders().Qac(0.0f);
      DataLog(log_level_proc_debug) << "SettleChannel: Qac->" << pd.Orders().Qac() << endmsg;
   }

   if (Software_CDS::isPPC(pd.RunTargets().PlateletConcenTarget.Get()))
   {
      //
      // Keep the valves from moving ...
      //
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   }

   //
   // Accumulate inlet volume ...
   //
   _InletVolume += pd.Status().InletPump.dV();
   DataLog(log_level_proc_debug) << "SettleChannel: inlet volume processed->" << _InletVolume << endmsg;

   //
   // When enough volume has been processed the settling is
   //  complete ...
   //
   if ( _InletVolume > CobeConfig::data().SettleChannelVin )
   {
      _ChannelSettled = 1;
      DataLog(log_level_proc_debug) << "Channel Settled " << (int)CobeConfig::data().SettleChannelVin << endmsg;
   }

   _RecoveryRequested = 0;
   return 1;
}

int SettleChannel::mayEnter () const
{
   ProcData pd;

   if ( pd.Status().CassIsPltPlsRBC() )
   {
      return 1;
   }
   return 0;
}

RecoveryTypes::InRecovery SettleChannel::recoveryExpected ()
{
   return RecoveryTypes::CountTime;
}

int SettleChannel::shouldRecover ()
{

   if ( _RecoveryRequested ||
        ( active() && !_ChannelSettled )
        )
   {
      DataLog(log_level_proc_debug) << "Recovery Needed" << endmsg;
      return 1;
   }

   //
   // Reset the chamber purged flag and the
   //  volume accumulated ...
   //
   _ChannelSettled = 0;
   _InletVolume    = 0.0f;

   return 0;
}

void SettleChannel::reset ()
{
   _RecoveryRequested = 0;
   _ChannelSettled    = 0;
   _InletVolume       = 0.0f;
}

void SettleChannel::settleChannelRequest ()
{
   _RecoveryRequested = 1;
}

void SettleChannel::copyOver (const SettleChannel&)
{}

void SettleChannel::cleanup ()
{}

/* FORMAT HASH d76e3f620ae1ae1698d7006bfe8f1238 */
