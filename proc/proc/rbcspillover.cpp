/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbcspillover.cpp
 *
 */

#include "rbcspillover.h"
#include "recoveryman.h"
#include "an_alarm.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(RBCSpillover);

RBCSpillover::RBCSpillover()
   : RecoveryAbs(),
     _SpilloverReqMsg(Callback<RBCSpillover>(this, &RBCSpillover::spilloverRequest) ),
     _SpilloverDetMsg(Callback<RBCSpillover>(this, &RBCSpillover::spilloverDetected) ),
     _LineClearedMsg(Callback<RBCSpillover>(this, &RBCSpillover::lineCleared) ),
     _SpilloverRequested(0),
     _SpilloverDetected(0),
     _SpilloverStarted(0),
     _LineCleared(0)
{}

RBCSpillover::RBCSpillover(const RBCSpillover& rec)
   : RecoveryAbs(rec),
     _SpilloverReqMsg(Callback<RBCSpillover>(this, &RBCSpillover::spilloverRequest) ),
     _SpilloverDetMsg(Callback<RBCSpillover>(this, &RBCSpillover::spilloverDetected) ),
     _LineClearedMsg(Callback<RBCSpillover>(this, &RBCSpillover::lineCleared) ),
     _SpilloverRequested(rec._SpilloverRequested),
     _SpilloverDetected(rec._SpilloverDetected),
     _SpilloverStarted(rec._SpilloverStarted),
     _LineCleared(rec._LineCleared)
{
   copyOver(rec);
}

RBCSpillover::~RBCSpillover()
{
   cleanup();
}

int RBCSpillover::init ()
{
   return 1;
}

int RBCSpillover::doRecovery ()
{
   ProcData pd;

   if (    pd.Run().Substate.Get() != SS_PIR_PLASMA
           && pd.Run().Substate.Get() != SS_PIR_NOPLASMA
           )
   {
      //
      // Send the clear line request message ...
      //
      ClearLineMsg cl(MessageBase::SEND_LOCAL);
      cl.send(1);

      //
      // Set the flags to 0 ...
      //
      _SpilloverDetected  = 0;
      _SpilloverRequested = 0;
      _SpilloverStarted   = 1;
      _LineCleared        = 0;
   }
   else
   {
      pd.MakeRunWritable();
      pd.Run().SpilloverDuringPIR.Set(1);
   }

   return 1;
}

int RBCSpillover::mayEnter () const
{
   ProcData pd;

   if (    ( pd.Status().CassIsPltPlsRBC()
             )
           && ( pd.Run().Substate.Get() >= SS_CHANNEL_SETUP
                )
           && ( pd.Run().Substate.Get() < SS_RBC_PTF_SETUP_1
                )
           )
   {
      return 1;
   }
   return 0;
}

void RBCSpillover::reset ()
{
   //
   // Set the flags to 0 ...
   //
   _SpilloverRequested = 0;
   _SpilloverDetected  = 0;
}

void RBCSpillover::spilloverRequest ()
{
   requestSpillover();
   _LineCleared = 0;
}

void RBCSpillover::lineCleared ()
{
   _LineCleared = 1;
}

void RBCSpillover::spilloverDetected ()
{
   ProcData pd;

   DataLog(log_level_proc_recovery_info) << "Spillover detected message" << endmsg;

   _SpilloverDetected = 1;

   if (    !_SpilloverStarted
           && !_SpilloverRequested
           && (    pd.Status().PlasmaValve.IsCollecting()
                   || pd.Status().CollectValve.IsCollecting() )
           && mayEnter() )
   {
      if ( !( queuedUp() || active() ) )
      {
         DataLog(log_level_proc_debug) << "Set Spillover alarm" << endmsg;

         //
         // ALARM! - spillover detected
         //
         anAlarmMsg spilloverAlarm(SPILLOVER_DETECTED);
         spilloverAlarm.setAlarm();
      }

      requestSpillover();
   }
}

int RBCSpillover::shouldRecover ()
{
   //
   // If we still detect a spillover ...
   //
   if (    _SpilloverDetected
           && _LineCleared )
   {
      _SpilloverRequested = 1;
      _SpilloverStarted   = 0;
      DataLog(log_level_proc_recovery_info) << "Spillover detected again ..." << endmsg;
      requestSpillover();
   }
   else if ( _LineCleared )
   {
      _SpilloverRequested = 0;
      _SpilloverStarted   = 0;
   }

   if ( ( _SpilloverRequested ||
          ( active() && !_SpilloverStarted ) )
        )
   {
      return 1;
   }

   //
   // Set the flags to 0 ...
   //
   _SpilloverDetected = 0;

   return 0;
}

void RBCSpillover::requestSpillover ()
{
   DataLog(log_level_proc_info) << "Request Spillover" << endmsg;

   //
   // Send the spillover request message ...
   //
   RbcSpilloverRequestMsg sd(MessageBase::SEND_LOCAL);
   sd.send(1);

   _SpilloverRequested = 1;
}

void RBCSpillover::copyOver (const RBCSpillover&)
{}

void RBCSpillover::cleanup ()
{}

/* FORMAT HASH 493f386824fa79ba24dafce9b26fe206 */
