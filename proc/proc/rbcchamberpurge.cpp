/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcchamberpurge.cpp
 *
 */

#include "rbcchamberpurge.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(RBCChamberPurge);

RBCChamberPurge::RBCChamberPurge()
   : RecoveryAbs(),
     _RecoveryReqMsg(Callback<RBCChamberPurge>(this, &RBCChamberPurge::recoveryRequest) ),
     _RecoveryRequested(0),
     _ChamberPurged(0),
     _PlasmaVolume(0.0f)
{}

RBCChamberPurge::RBCChamberPurge(const RBCChamberPurge& rec)
   : RecoveryAbs(rec),
     _RecoveryReqMsg(Callback<RBCChamberPurge>(this, &RBCChamberPurge::recoveryRequest) ),
     _RecoveryRequested(rec._RecoveryRequested),
     _ChamberPurged(rec._ChamberPurged),
     _PlasmaVolume(rec._PlasmaVolume)
{
   copyOver(rec);
}

RBCChamberPurge::~RBCChamberPurge()
{
   cleanup();
}

int RBCChamberPurge::init ()
{
   return 1;
}

int RBCChamberPurge::doRecovery ()
{
   ProcData pd;

   //
   // We are doing the recovery so the
   //  request has been acknowledged ...
   //
   _RecoveryRequested = 0;

   //
   // Set the plasma valve to be in the recirculate position
   //
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);

   //
   //
   //   Set the collect valve to the current position.  In case the valve is in the unknown position (IT10906)
   //   choose the return position (it's either coming from return or moving to return, so either way
   //   that is an appropriate choice)
   //
   HAL_VALVESTATE collectcmd = pd.Status().CollectValve.State();
   if ( collectcmd == HAL_VALVE_UNKNOWN )
   {
      DataLog(log_level_proc_info) << "RBCChamberPurge: collect valve position overridden from unknown to return" << endmsg;
      collectcmd = HAL_VALVE_RETURN;
   }
   pd.Orders().ValveCmd(HAL_VCOLLECT, collectcmd);

   //
   // Command the collect pump ...
   //
   pd.Orders().Qin(pd.Predict().Qin(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qac(pd.Predict().Qac(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qrp(pd.Predict().Qrp(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qcollect(CobeConfig::data().RBCPurgeQcollect);

   //
   // When the valve is in the recirculate position ...
   //
   if ( pd.Status().PlasmaValve.IsReturning() )
   {
      //
      // Set the plasma flow ...
      //
      pd.Orders().Qplasma(CobeConfig::data().RBCPurgeQplasma);

      //
      // Accumulate volume ...
      //
      _PlasmaVolume += pd.Status().PlasmaPump.dV();

   }
   else
   {
      //
      // Stop the plasma pump ...
      //
      pd.Orders().Qplasma(0.0f);
   }

   //
   // When enough volume has been processed the purge is
   //  complete ...
   //
   if ( _PlasmaVolume >= CobeConfig::data().RBCPurgeVplasma )
   {
      _ChamberPurged = 1;
   }

   return 1;
}

int RBCChamberPurge::mayEnter () const
{
   ProcData pd;

   if ( pd.Status().CassIsPltPlsRBC() )
   {
      return 1;
   }
   return 0;
}

RecoveryTypes::InRecovery RBCChamberPurge::recoveryExpected ()
{
   if ( recoveryCount() <= CobeConfig::data().RBCPurgeNumExpected )
   {
      return RecoveryTypes::CountAll;
   }
   return RecoveryTypes::CountNone;
}

void RBCChamberPurge::recoveryRequest ()
{
   //
   // Set the request flag
   //
   _RecoveryRequested = 1;
}

int RBCChamberPurge::shouldRecover ()
{
   if (    ( _RecoveryRequested && CobeConfig::data().RBCPurgeVplasma > 0.0f )
           || ( active() && !_ChamberPurged )
           )
   {
      return 1;
   }

   //
   // Reset the chamber purged flag and the
   //  volume accumulated ...
   //
   _ChamberPurged = 0;
   _PlasmaVolume  = 0.0f;

   return 0;
}

void RBCChamberPurge::copyOver (const RBCChamberPurge&)
{}

void RBCChamberPurge::cleanup ()
{}

/* FORMAT HASH 025b919feb983af8624778123eae75cc */
