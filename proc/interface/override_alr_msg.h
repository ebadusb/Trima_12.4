/*
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      override_alr_msg.h
 *
 * ABSTRACT:   Message sent from the alarm handler to proc to
 *              set the current alarm state
 *
 */

#ifndef OVERRIDE_ALR_MSG_H
#define OVERRIDE_ALR_MSG_H

#include "override_alr.h"
#include "message.h"

class override_alr_msg
{
public:

   override_alr_msg();
   override_alr_msg(const CallbackBase& cb);
   virtual ~override_alr_msg();

   // Function the set the message to an unalarmed state
   void clear ();

   // Send out my data
   void SendMsg ();

   // get/set funcs
   void Vrbc(HAL_VALVESTATE) ;
   HAL_VALVESTATE Vrbc (void) const;

   void Vplt(HAL_VALVESTATE) ;
   HAL_VALVESTATE Vplt (void) const;

   void Vpls(HAL_VALVESTATE) ;
   HAL_VALVESTATE Vpls (void) const;

   bool AlarmActive (void) const;
   void AlarmActive (bool);

   // pump flows in ml/min
   float Qac (void) const;
   void  Qac (float flow);
   float Qin (void) const;
   void  Qin (float flow);
   float Qrp (void) const;
   void  Qrp (float flow);
   float Qplasma (void) const;
   void  Qplasma (float flow);
   float Qcollect (void) const;
   void  Qcollect (float flow);

   // centrifuge on/off
   bool centSpeed (void) const;
   void centSpeed (bool newVal);
   bool centPausedSpeed (void) const;
   void centPausedSpeed (bool newVal);

   // turn lights on/off for operator alert
   void                 alarmLight (HAL_ALARMLIGHTORDERS newVal);
   HAL_ALARMLIGHTORDERS alarmLight (void) const;

   ALARM_VALUES rcvdAlarmID (void) const;
   void rcvdAlarmID(ALARM_VALUES);

   ALARM_VALUES respAlarmID (void) const;
   void respAlarmID(ALARM_VALUES);

   ALARM_VALUES dispAlarmID (void) const;
   void                dispAlarmID(ALARM_VALUES);

   const override_alr& getOverrides () const;
   bool                changed (const override_alr& overrides) const;

protected:

   Message<override_alr> _OverrideMsg;

};

#endif

/* FORMAT HASH 63bcd9b387bd46e931e61fe78bff7682 */
