/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acoccmon.h
 *
 * ABSTRACT:  This alarm checks for an occluded AC line using the
 *             APS sensor.
 *
 */


#ifndef _AC_OCC_MON_H_
#define _AC_OCC_MON_H_

#include "monitorbase.h"
#include "objdictionary.h"
#include "an_alarm.h"
#include "fastdata.h"

#define MAX_HISTORY      (120 * 50)    // 120 sec * ( FASTDATALENGTH samples per 2 sec )

class ACOccMon
   : public MonitorBase
{
   DECLARE_OBJ(ACOccMon);
public:
   ACOccMon();
   virtual ~ACOccMon();

   virtual void Monitor ();

   virtual void enable ();
   virtual void disable ();

   void newAPSData ();

protected:

   float analyzeData ();


protected:

   //
   // APS data readings
   Message< FASTDATA > _APSFastDataMsg;

   //
   // Alarm message
   //
   anAlarmMsg _ACOcclusionAlarm;

   //
   // Current consecutive AC occluded cycles
   //
   long _CycleCount;

   //
   // Average distance between peaks
   //
   long _AverageDistance;

   //
   // Average noise value
   //
   long _AverageNoise;

   //
   // Current data count
   //
   long _DataCount;

   //
   // Should monitor flag
   //
   bool _MonitorCycle;

   //
   // Return cycle flag
   //
   bool _ReturnCycle;

   //
   // Data is no good, skip this cycle
   //
   bool _SkipCycle;

   // True if a RAS set is in use
   bool _isRasSet;

   //
   // APS Raw Data Array
   //
   short _RawData[ MAX_HISTORY ];

};


#endif

/* FORMAT HASH ab871edfde8c3c3f8b3389aec266b7be */
