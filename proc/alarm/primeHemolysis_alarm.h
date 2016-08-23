/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      primeHemolysis_alarm.h
 *
 */


#ifndef _PRIME_HEMOLYSIS_ALARM_H_
#define _PRIME_HEMOLYSIS_ALARM_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class PrimeHemolysisAlarm
   : public MonitorBase
{
   DECLARE_OBJ(PrimeHemolysisAlarm);
public:

   PrimeHemolysisAlarm();
   virtual ~PrimeHemolysisAlarm();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();


protected:
   float makeRatio ();
   void  checkForHemolysis (const float& ratio);

private:
   bool       _WeSawGT50;
   anAlarmMsg _HemolysisAlarm;


};

#endif

/* FORMAT HASH bd592924bb79ad579943df1aa9978749 */
