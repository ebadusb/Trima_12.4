/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plasmavalvemotion.cpp
 *
 */

#include "plasmavalvemotion.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_OBJ(PlasmaValveMotion);

PlasmaValveMotion::PlasmaValveMotion()
   : RecoveryAbs(),
     _WaitTimer(0, Callback<PlasmaValveMotion>(this, &PlasmaValveMotion::valveCanMove), TimerMessage::DISARMED),
     _TimerStarted(0),
     _ValveCanMove(0),
     _ValveInPosition(1),
     _NewOrder(HAL_VALVE_UNKNOWN)
{}

PlasmaValveMotion::PlasmaValveMotion(const PlasmaValveMotion& rec)
   : RecoveryAbs(rec),
     _WaitTimer(0, Callback<PlasmaValveMotion>(this, &PlasmaValveMotion::valveCanMove), TimerMessage::DISARMED),
     _TimerStarted(0),
     _ValveCanMove(rec._ValveCanMove),
     _ValveInPosition(rec._ValveInPosition),
     _NewOrder(rec._NewOrder)
{
   copyOver(rec);
}

PlasmaValveMotion::~PlasmaValveMotion()
{
   cleanup();
}

int PlasmaValveMotion::init ()
{
   //
   // Set up the timer ...
   //
   _WaitTimer.interval(0);

   //
   // Don't let the valve move ...
   //
   _ValveCanMove = 0;

   return 1;
}

int PlasmaValveMotion::doRecovery ()
{
   ProcData pd;

   //
   // Stop the pumps ...
   //
   stopPumps();

   //
   // Make sure the valves don't move ...
   //
   pd.Orders().ValveCmd(HAL_VRBC,     pd.Status().RBCValve.State() );
   pd.Orders().ValveCmd(HAL_VPLASMA,  pd.Status().PlasmaValve.State() );
   pd.Orders().ValveCmd(HAL_VCOLLECT, pd.Status().CollectValve.State() );

   if ( !_ValveInPosition )
   {
      if ( !_TimerStarted )
      {
         //
         // Start the timer ...
         //
         _WaitTimer.interval( (int)( CobeConfig::data().PumpsStopWaitTime * 1000 ) ); // 1000 milliseconds = 1 second
         _TimerStarted = 1;
         DataLog(log_level_proc_debug) << "Wait timer started " << (int)( CobeConfig::data().PumpsStopWaitTime) << endmsg;
      }

      if ( _ValveCanMove )
      {
         //
         // Order the valve to the new position ...
         //
         pd.Orders().ValveCmd(HAL_VPLASMA, _NewOrder);
         DataLog(log_level_proc_debug) << "Plasma Valve commanded to new position " << _NewOrder << endmsg;
      }

   }

   if ( _NewOrder == pd.Status().PlasmaValve.State() )
   {
      //
      // The valve has reached its requested postion, the
      //  procedure can be started again ...
      //
      _ValveInPosition = 1;
      _TimerStarted    = 0;
      _ValveCanMove    = 0;
   }

   return 1;
}

void PlasmaValveMotion::valveCanMove ()
{
   //
   // Let the valve move ...
   //
   _ValveCanMove = 1;
   DataLog(log_level_proc_debug) << "Plasma Valve can move" << endmsg;

   //
   // Stop the timer ...
   //
   _WaitTimer.interval(0);
   DataLog(log_level_proc_debug) << "Wait timer stopped" << endmsg;
}

int PlasmaValveMotion::mayEnter () const
{
   ProcData pd;

   //
   // This recovery only needs be done with a Plt/Pl/RBC set ...
   //
   if ( pd.Status().CassIsPltPlsRBC() )
   {
      return 1;
   }
   return 0;
}

int PlasmaValveMotion::shouldRecover ()
{
   //
   // Check the command vs. position ...
   //
   if ( valveCommandedToMove() )
   {
      return 1;
   }
   //
   // Set the valve in position flag ...
   //
   _ValveInPosition = 1;

   return 0;
}

int PlasmaValveMotion::valveCommandedToMove ()
{
   //
   // If we know the valve is out of position ...
   //
   if ( !_ValveInPosition )
   {
      return 1;
   }

   //
   // If the new command is different than the current status ...
   //
   ProcData pd;
   if ( pd.Orders().ValveCmd(HAL_VPLASMA) != pd.Status().PlasmaValve.State() )
   {
      //
      // Save the order ...
      //
      _NewOrder = pd.Orders().ValveCmd(HAL_VPLASMA);

      //
      // The valve is now out of position ...
      //
      _ValveInPosition = 0;
      _TimerStarted    = 0;
      _ValveCanMove    = 0;

      return 1;
   }

   return 0;
}

void PlasmaValveMotion::stopPumps ()
{
   ProcData pd;

   //
   // Order all pumps to zero ...
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);
   DataLog(log_level_proc_debug) << "Pumps stopped" << endmsg;
}

void PlasmaValveMotion::copyOver (const PlasmaValveMotion&)
{}

void PlasmaValveMotion::cleanup ()
{}

/* FORMAT HASH 130973aad78612b9d6df435e1c38270e */
