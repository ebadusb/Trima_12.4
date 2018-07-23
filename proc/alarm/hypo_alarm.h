/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      hypo_alarm.h
 *
 * ABSTRACT: This object checks for donor hypovolemia give the
 *            current state of the system
 */

#ifndef HYPO_ALARM_H
#define HYPO_ALARM_H

#include "monitorbase.h"
#include "an_alarm.h"
#include "hypovolemia.h"
#include "objdictionary.h"

class HypoAlarm
   : public MonitorBase
{
   DECLARE_OBJ(HypoAlarm);
public:
   HypoAlarm();
   virtual ~HypoAlarm();

   virtual void Monitor ();

   // Function to enable alarm checking ...
   virtual void enable ();
   // Function to disable alarm checking ...
   virtual void disable ();

protected:

   anAlarmMsg  _HypovolemiaAlarm;
   Hypovolemia _HypovolemiaChecker;

   int         _HypoAlarmSet;
   float       _VolumeLeft;
   bool        _PrevDrawCycle;
   bool        _RinsebackLogged;
   bool        _AlarmLogged;

};

#endif

/* FORMAT HASH fb9f5256a6701ba7b07aba830e7b1104 */
