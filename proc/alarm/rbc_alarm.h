/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      rbc_alarm.h
 *
 */


#ifndef _RBC_ALARM_H_
#define _RBC_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class RBCAlarm
   : public MonitorBase
{
   DECLARE_OBJ(RBCAlarm);
public:

   RBCAlarm();
   virtual ~RBCAlarm();

   virtual void Monitor ();

protected:

   void checkForSpillover (const float& ratio);

protected:

   bool inPrime () const;

   //
   // Counter used to measure alarm duration
   //
   int _UpdateCounter;

   //
   // Flag used to save the spillover detection status
   //
   int        _SpilloverDetected;

   anAlarmMsg _spilloverAlarm;

   bool       _alreadyShowedSpilloverAlarm;

};

#endif

/* FORMAT HASH 41854ec752dc705aaa0e57ea6d334f4a */
