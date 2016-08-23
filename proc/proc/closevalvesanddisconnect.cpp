/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      closevalvesanddisconnect.cpp
 *
 */

#include "closevalvesanddisconnect.h"
#include "procdata.h"

DEFINE_STATE(CloseValvesAndDisconnect);

CloseValvesAndDisconnect::CloseValvesAndDisconnect()
   : CloseValves(),
     _LocalValveTimer(0, Callback<CloseValvesAndDisconnect>(this, &CloseValvesAndDisconnect::valvesTimedOut), TimerMessage::DISARMED),
     _PowerTimer(0, Callback<CloseValvesAndDisconnect>(this, &CloseValvesAndDisconnect::powerTooSlow), TimerMessage::DISARMED),
     _PowerRequestResponseMsg(),
     _PowerRespRegistered(false),
     _ValvesCommanded(false)
{}

CloseValvesAndDisconnect::CloseValvesAndDisconnect(const CloseValvesAndDisconnect& state)
   : CloseValves(state),
     _LocalValveTimer(0, Callback<CloseValvesAndDisconnect>(this, &CloseValvesAndDisconnect::valvesTimedOut), TimerMessage::DISARMED),
     _PowerTimer(0, Callback<CloseValvesAndDisconnect>(this, &CloseValvesAndDisconnect::powerTooSlow), TimerMessage::DISARMED),
     _PowerRequestResponseMsg(),
     _PowerRespRegistered(false),
     _ValvesCommanded(false)
{}

CloseValvesAndDisconnect::~CloseValvesAndDisconnect()
{}


int CloseValvesAndDisconnect::preEnter ()
{

   request24VPower();
   DataLog(log_level_proc_info) << "CloseValvesAndDisconnect requesting 24V Power on" << endmsg;

   // give it 15 sec to turn on power and close the valves
   _LocalValveTimer.interval(15000);

   return NORMAL;
}

int CloseValvesAndDisconnect::postExit ()
{
   // Delete my messages ...
   cleanup();

   // Stop the timers ...
   _LocalValveTimer.interval(0);
   _PowerTimer.interval(0);

   return NORMAL;
}

int CloseValvesAndDisconnect::transitionStatus ()
{
   int      retval = NO_TRANSITION;
   ProcData pd;

   if ( !_ValvesCommanded )
   {
      DataLog(log_level_proc_info) << "CloseValvesAndDisconnect commanding valves closed" << endmsg;
      commandToReturn();
      _ValvesCommanded = true;
   }
   else
   {
      if ( pd.Status().RBCValve.IsReturning() &&
           pd.Status().PlasmaValve.IsReturning() &&
           pd.Status().CollectValve.IsReturning() )
      {
         DataLog(log_level_proc_info) << "CloseValvesAndDisconnect valves closed" << endmsg;
         retval = 1;

         // now turn off the power
         RequestSafetyPowerMsg powerRequestMsg(MessageBase::SEND_GLOBAL);
         powerRequestMsg.send(DISABLE_24V_POWER);
         DataLog(log_level_proc_info) << "CloseValvesAndDisconnect Request 24V Power Off" << endmsg;
      }
   }

   return retval;
}

void CloseValvesAndDisconnect::request24VPower ()
{
   if ( !_PowerRespRegistered )
   {
      // Register for the response for the power request ...
      _PowerRequestResponseMsg.init(Callback<CloseValvesAndDisconnect>(this, &CloseValvesAndDisconnect::powerRequestResponse) );
      DataLog(log_level_proc_info) << "CloseValvesAndDisconnect Create power request response message " << endmsg;

      _PowerRespRegistered = true;
   }

   // Request power ...
   RequestSafetyPowerMsg powerRequestMsg(MessageBase::SEND_GLOBAL);
   powerRequestMsg.send(ENABLE_24V_POWER);
   DataLog(log_level_proc_donor_disconnect_info) << "CloseValvesAndDisconnect Sent the request to enable 24V Power " << endmsg;

   // Start the request timer
   _PowerTimer.interval(10000); // give it 10 seconds
   DataLog(log_level_proc_donor_disconnect_info) << "CloseValvesAndDisconnect Started the power timer 10 seconds" << endmsg;
}

void CloseValvesAndDisconnect::powerRequestResponse ()
{

   DataLog(log_level_proc_donor_disconnect_info) << "Received power request response: " << _PowerRequestResponseMsg.getData() << endmsg;
}

void CloseValvesAndDisconnect::powerTooSlow ()
{

   DataLog(log_level_proc_donor_disconnect_error) << "CloseValvesAndDisconnect failed to close valves because power wasn't turned on" << endmsg;
}

void CloseValvesAndDisconnect::valvesTimedOut ()
{
   ProcData pd;

   DataLog(log_level_proc_donor_disconnect_error) << "CloseValvesAndDisconnect valves timed out" << endmsg;

   // Determine which valve(s) FAILED and set appropriate alarm(s)
   if ( !pd.Status().RBCValve.IsReturning() )
   {
      DataLog(log_level_proc_donor_disconnect_error) << "CloseValvesAndDisconnect RBC not closed" << endmsg;
      _RBCAlarm.setAlarm();
   }
   if ( !pd.Status().PlasmaValve.IsReturning() )
   {
      DataLog(log_level_proc_donor_disconnect_error) << "CloseValvesAndDisconnect Plasma not closed" << endmsg;
      _PlasmaAlarm.setAlarm();
   }
   if ( !pd.Status().CollectValve.IsReturning() )
   {
      DataLog(log_level_proc_donor_disconnect_error) << "CloseValvesAndDisconnect Platelet not closed" << endmsg;
      _CollectAlarm.setAlarm();
   }

}

/* FORMAT HASH 771081b486360dfe76999885b7ff4053 */
