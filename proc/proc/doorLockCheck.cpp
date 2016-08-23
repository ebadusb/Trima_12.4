/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprimeinlet.cpp
 *
 */

#include "doorLockCheck.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(DoorLockCheck);

DoorLockCheck::DoorLockCheck()
   : StateAbs(),
     _CloseTheDoorAlarm(DOOR_OPEN_ALARM)
{}

DoorLockCheck::DoorLockCheck(const DoorLockCheck& state)
   : StateAbs(state),

     _CloseTheDoorAlarm(DOOR_OPEN_ALARM)
{}

DoorLockCheck::~DoorLockCheck()
{}

int DoorLockCheck::transitionStatus ()
{
   ProcData pd;
   if ( (!pd.Status().DoorIsOpen()) && pd.Status().DoorIsLocked()
        )
   {
      DataLog(log_level_proc_info) << "Doors closed and locked -- all systems go to transition to blood prime" << endmsg;


      RequestSafetyPowerMsg request(MessageBase::SEND_GLOBAL);
      request.send(ENABLE_64V_POWER);

      return 1;
   }

   return NO_TRANSITION;
}

int DoorLockCheck::preProcess ()
{
   ProcData pd;
   if (
      !pd.Status().DoorIsOpen()
      && (!pd.Status().DoorIsLocked() )
      && (!pd.AlarmActive() )

      )
   {

      DataLog(log_level_proc_info) << "Doors closed and needs to be locked -- order lock" << endmsg;
      pd.Orders().LockDoor();
   }


   if (   pd.Status().DoorIsOpen()
          && !pd.AlarmActive()  )
   {
      DataLog(log_level_proc_info) << "Doors open raise alarm" << endmsg;
      _CloseTheDoorAlarm.setAlarm();
   }

   return NORMAL;
}

int DoorLockCheck::postProcess ()
{

   return NORMAL;
}

int DoorLockCheck::preEnter ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();


   ProcData pd;
   if ( pd.Status().DoorIsOpen()  )
   {
      DataLog(log_level_proc_info) << "Doors open raise alarm" << endmsg;
      _CloseTheDoorAlarm.setAlarm();
   }

   return NORMAL;
}

int DoorLockCheck::postExit ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();

   return NORMAL;
}

void DoorLockCheck::reset ()
{}

void DoorLockCheck::stopPumps ()
{
   ProcData pd;

   //
   // Command all the pumps to stop ...
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);

   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;
}

void DoorLockCheck::copyOver (const DoorLockCheck&)
{}

void DoorLockCheck::cleanup ()
{}

/* FORMAT HASH b11e0eaf43b639bd18a99b1cba8b1c0c */
