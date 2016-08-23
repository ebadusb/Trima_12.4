/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      clearlineabs.cpp
 *
 */

#include "clearlineabs.h"
#include "recoveryman.h"
#include "substatemap.h"
#include "cobeconfig.h"

ClearLineAbs::ClearLineAbs()
   : RecoveryAbs(),
     _SpilloverReqMsg(Callback<ClearLineAbs>(this, &ClearLineAbs::spilloverRequest) ),
     _ClearLineReqMsg(Callback<ClearLineAbs>(this, &ClearLineAbs::clearLineRequest) ),
     _SpilloverRequested(0),
     _ClearLineRequested(0 /*NONE*/),
     _LineCleared(0),
     _CollectVolume(0.0f),
     _VinClearLineSet(0)
{}

ClearLineAbs::ClearLineAbs(const ClearLineAbs& rec)
   : RecoveryAbs(rec),
     _SpilloverReqMsg(Callback<ClearLineAbs>(this, &ClearLineAbs::spilloverRequest) ),
     _ClearLineReqMsg(Callback<ClearLineAbs>(this, &ClearLineAbs::clearLineRequest) ),
     _SpilloverRequested(rec._SpilloverRequested),
     _ClearLineRequested(rec._ClearLineRequested),
     _LineCleared(rec._LineCleared),
     _CollectVolume(rec._CollectVolume),
     _VinClearLineSet(rec._VinClearLineSet)
{
   copyOver(rec);
}

ClearLineAbs::~ClearLineAbs()
{
   cleanup();
}

int ClearLineAbs::init ()
{
   return 1;
}

int ClearLineAbs::doRecovery ()
{
   ProcData pd;
   if (!_VinClearLineSet)
   {
      pd.MakeRunWritable();
      pd.Run().VinClearLine.Set(pd.Volumes().Vin.Get());
      pd.MakeRunReadable();
      _VinClearLineSet = 1;
   }

   setPumpSpeeds(pd);

   //
   // Keep the valves from moving ...
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);

   //
   // Accumulate collect volume ...
   //
   _CollectVolume += pd.Status().CollectPump.dV();

   //
   // Clear the collect line ...
   //
   float clearLineVolume;

   switch ( _ClearLineRequested )
   {
      case 1 : // SPILLOVER:
         clearLineVolume = CobeConfig::data().SpillOverClearLineVcollect;
         break;

      case 2 : // STANDARD:
         clearLineVolume = CobeConfig::data().ClearLineVcollect;
         break;

      case 0 : // NONE:
      default :
         return 0;
   }

   if ( _CollectVolume >= clearLineVolume )
   {
      DataLog(log_level_proc_recovery_info) << ( _ClearLineRequested == 1 ? "RBC Spillover " : "" )
                                            << "ClearLineAbs recovery complete: Collect Volume->" << _CollectVolume << endmsg;

      //
      // Send the appropriate clear line complete message ...
      ClearLineCompleteMsg clcompletemsg(MessageBase::SEND_LOCAL);
      clcompletemsg.send(_ClearLineRequested);

      pd.MakeRunWritable();
      pd.Run().WBCPurgeType.Set(WBCPURGE_NONE);
      pd.MakeRunReadable();

      //
      // The recovery is complete ...
      //
      _SpilloverRequested = 0;
      _ClearLineRequested = 0;
      _LineCleared        = 1;
      _CollectVolume      = 0.0f;
      _VinClearLineSet    = 0;

      return 1;
   }

   //
   // Set the flags to 0 ...
   //
   _LineCleared = 0;

   return 1;
}

int ClearLineAbs::mayEnter () const
{
   if (disabled()) return 0;

   return ( _ClearLineRequested == 0 /*NONE*/ ? 0 : 1);
}

void ClearLineAbs::reset ()
{
   //
   // Set the flags to 0 ...
   //
   _SpilloverRequested = 0;
   _ClearLineRequested = 0;
   _LineCleared        = 0;
   _CollectVolume      = 0.0f;
}

void ClearLineAbs::spilloverRequest ()
{
   if (disabled()) return;

   _SpilloverRequested = 1;
}

void ClearLineAbs::clearLineRequest ()
{
   if (disabled()) return;

   _ClearLineRequested = _ClearLineReqMsg.getData();
}

RecoveryTypes::InRecovery ClearLineAbs::recoveryExpected ()
{
   if (disabled()) return RecoveryTypes::NoRecovery;

   ProcData pd;
   if (pd.Run().WBCPurgeType.Get() == WBCPURGE_SCHEDULED)
   {
      return RecoveryTypes::CountTime;
   }
   else
      return RecoveryTypes::CountNone;
}

void ClearLineAbs::copyOver (const ClearLineAbs&)
{}

void ClearLineAbs::cleanup ()
{}

/* FORMAT HASH 44df5773c807e6607348a562c637042c */
