/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeSpeedRecoveryAbs.cpp
 *
 */

#include "centrifugeSpeedRecoveryAbs.h"
#include "substatemap.h"
#include "cobeconfig.h"

//////////////////////////////////////////////////////////////
//
// FUNCTIONS FOR CLASS CentrifugeSpeedRecoveryAbs
//
// Abstract base class for all centrifuge speed recoveries
// (Slow or stop, scaled or normal)
//
/////////////////////////////////////////////////////////////
CentrifugeSpeedRecoveryAbs::CentrifugeSpeedRecoveryAbs()
   : RecoveryAbs(),
     _InterfaceSetupTimer(0, Callback<CentrifugeSpeedRecoveryAbs>(this, &CentrifugeSpeedRecoveryAbs::interfaceEstablished), TimerMessage::DISARMED),
     _InterfaceEstablished(0),
     _TimerStarted(0)
{}

CentrifugeSpeedRecoveryAbs::CentrifugeSpeedRecoveryAbs(const CentrifugeSpeedRecoveryAbs& rec)
   : RecoveryAbs(rec),
     _InterfaceSetupTimer(0, Callback<CentrifugeSpeedRecoveryAbs>(this, &CentrifugeSpeedRecoveryAbs::interfaceEstablished), TimerMessage::DISARMED),
     _InterfaceEstablished(rec._InterfaceEstablished),
     _TimerStarted(rec._TimerStarted)
{
   copyOver(rec);
}

CentrifugeSpeedRecoveryAbs::~CentrifugeSpeedRecoveryAbs()
{
   cleanup();
}

int CentrifugeSpeedRecoveryAbs::init ()
{
   return 1;
}

int CentrifugeSpeedRecoveryAbs::doRecovery ()
{
   // If this is disabled, then don't do it.
   if (disabled()) return 0;

   ProcData pd;

   //
   // Close the valves ...
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);

   //
   // Command the pumps to stop ...
   //
   pd.Orders().Qin(pd.Predict().Qin(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qac(pd.Predict().Qac(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qrp(pd.Predict().Qrp(pd.Run().DrawCycle.Get(), SubstateMap::alg(NOCOLLECT_STATE)));
   pd.Orders().Qplasma(CobeConfig::data().CentStopQplasma);

   //
   // If we have an LRS chamber ( cassette is PltPlsRBC ) and
   //  we are not collecting plasma ...
   //
   if ( pd.Status().CassIsPltPlsRBC() &&
        pd.Status().PlasmaValve.State() == HAL_VALVE_RETURN )
      pd.Orders().Qcollect(CobeConfig::data().CentStopQplasma);
   else
      //
      // ... maintain a chamber flow  of zero ...
      //
      pd.Orders().Qcollect(0.0f);

   //
   // Command the centrifuge to return to
   //  speed ...
   //
   returnCentrifugeToSpeed(pd);

   //
   // Check to see if it is up to speed ...
   //
   if ( centrifugeIsUpToSpeed(pd) &&
        pd.Status().InletPump.dV() > 0.0f )
   {
      if ( !_TimerStarted )
      {
         //
         // Start the timer
         //
         _InterfaceSetupTimer.interval(interfaceSetupTimerInterval() );
         _TimerStarted = 1;
      }
   }
   else
   {
      //
      // Stop the timer and wait for the centrifuge to reach speed ...
      //
      _InterfaceSetupTimer.interval(0);
      _TimerStarted = 0;
   }

   return 1;
}

int CentrifugeSpeedRecoveryAbs::mayEnter () const
{
   // If this is disabled, then you may not enter.
   if (disabled()) return 0;

   return 1;
}

void CentrifugeSpeedRecoveryAbs::interfaceEstablished ()
{
   DataLog (log_level_proc_info) << "Centrifuge speed recovery established the interface." << endmsg;
   //
   // Set the interface established flag ...
   //
   _InterfaceEstablished = 1;

   //
   // Stop the timer ...
   //
   _InterfaceSetupTimer.interval(0);
   _TimerStarted = 0;
}

RecoveryTypes::InRecovery CentrifugeSpeedRecoveryAbs::recoveryExpected ()
{
   // If this is disabled, then it doesn't count
   if (disabled()) return RecoveryTypes::NoRecovery;

   if ( recoveryCount() <= numRecoveriesExpected() )
   {
      return RecoveryTypes::CountAll;
   }
   return RecoveryTypes::CountNone;
}

void CentrifugeSpeedRecoveryAbs::copyOver (const CentrifugeSpeedRecoveryAbs&)
{}

void CentrifugeSpeedRecoveryAbs::cleanup ()
{}



//////////////////////////////////////////////////////////////
//
// FUNCTIONS FOR CLASS CentrifugeSlowRecoveryAbs
//
// Abstract base class for all centrifuge slow recoveries
// (scaled or normal)
//
/////////////////////////////////////////////////////////////
CentrifugeSlowRecoveryAbs::CentrifugeSlowRecoveryAbs()
   : CentrifugeSpeedRecoveryAbs()
{}

CentrifugeSlowRecoveryAbs::CentrifugeSlowRecoveryAbs(const CentrifugeSlowRecoveryAbs& rec)
   : CentrifugeSpeedRecoveryAbs(rec)
{}

int CentrifugeSlowRecoveryAbs::interfaceSetupTimerInterval ()
{
   return (int)( CobeConfig::data().CentSlowInterfaceSetupTime * 1000 );
}

float CentrifugeSlowRecoveryAbs::numRecoveriesExpected ()
{
   return CobeConfig::data().CentSlowNumExpected;
}



//////////////////////////////////////////////////////////////
//
// FUNCTIONS FOR CLASS CentrifugeStopRecoveryAbs
//
// Abstract base class for all centrifuge slow recoveries
// (scaled or normal)
//
/////////////////////////////////////////////////////////////
CentrifugeStopRecoveryAbs::CentrifugeStopRecoveryAbs()
   : CentrifugeSpeedRecoveryAbs(),
     _RecoveryReqMsg(Callback<CentrifugeStopRecoveryAbs>(this, &CentrifugeStopRecoveryAbs::recoveryRequest) ),
     _RecoveryRequested(false)
{}

CentrifugeStopRecoveryAbs::CentrifugeStopRecoveryAbs(const CentrifugeStopRecoveryAbs& rec)
   : CentrifugeSpeedRecoveryAbs(rec),
     _RecoveryReqMsg(Callback<CentrifugeStopRecoveryAbs>(this, &CentrifugeStopRecoveryAbs::recoveryRequest) ),
     _RecoveryRequested(false)
{}

int CentrifugeStopRecoveryAbs::doRecovery ()
{
   //
   // The recovery is being performed so reset the
   //  request flag ...
   //
   _RecoveryRequested = 0;

   return CentrifugeSpeedRecoveryAbs::doRecovery();
}

void CentrifugeStopRecoveryAbs::recoveryRequest ()
{
   // If this is disabled, then we can't request it.
   if (disabled()) return;

   //
   // Set the recovery request flag ...
   //
   _RecoveryRequested = 1;
}

int CentrifugeStopRecoveryAbs::interfaceSetupTimerInterval ()
{
   return (int)( CobeConfig::data().CentStopInterfaceSetupTime * 1000 );
}

float CentrifugeStopRecoveryAbs::numRecoveriesExpected ()
{
   return CobeConfig::data().CentStopNumExpected;
}

/* FORMAT HASH 426b83e2eabd68d2cc7fa250a743dce4 */
