/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      ThreeSpillover.h
 *
 * ABSTRACT:  This alarm checks to see if too many
 *            consecutive spillovers occured.
 *
 */

#ifndef _THREE_SPILLOVER_ALARM_H_
#define _THREE_SPILLOVER_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class ThreeSpillover
   : public MonitorBase
{

   DECLARE_OBJ(ThreeSpillover);

public:
   ThreeSpillover();
   virtual ~ThreeSpillover();
   virtual void Monitor ();


   void checkConsecutiveSpillover (bool, bool, bool);
protected:

   anAlarmMsg _threeSpilloversDetected;
   int        _spilloverCounter;

   bool       _inRBC;
   bool       _inWBC;

};

#endif

/* FORMAT HASH 633a75a96ee483f0e2d5d84b46e0a90c */
