/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ptf_cps_alarm.h
 *
 * ABSTRACT:  This alarm checks for centrifuge pressure problems
 *             during the run
 *
 * Revision 1.1  2009/02/09 23:37:52Z  jheiusb
 * Initial revision
 * Revision 1.5  2007/10/18 20:00:56Z  jheiusb
 * 8254 -- remove the  Autosplit
 * Revision 1.4  2007/08/30 22:52:52Z  jheiusb
 * 8057 -- auto split black stamp
 * Revision 1.3  2007/06/08 17:15:00Z  jheiusb
 * 8060, 7180 -- fix the PTF alarms for CPS and the baselining algorithm for doubles
 * Revision 1.2  2007/06/05 17:17:10Z  jheiusb
 * 8060 -- clean up CPS PTF monitor
 * Revision 1.1  2007/03/15 17:03:35Z  jheiusb
 * Initial revision
 * Revision 1.3  2005/12/22 20:45:56Z  rm70006
 * IT 7197.  Fix baseline pressure calculation.
 *
 */

#ifndef PTF_CPS_ALARM_MONITOR_H
#define PTF_CPS_ALARM_MONITOR_H


#include "aPQMonitor.h"
#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"
#include "predict_cds.h"

class PTFCPS_Alarm_Monitor
   : public aPQMonitor
{
   DECLARE_OBJ(PTFCPS_Alarm_Monitor);

public:

   PTFCPS_Alarm_Monitor();
   virtual ~PTFCPS_Alarm_Monitor();

   virtual void Monitor ();

   void setBaseline ();
   void findBaseline (float thePressure, float currentRBCVolume);
   void checkPressure (float thePressure);

protected:


   void flagRBCVolume ();
   void flagRBCQuality ();
   void flagWBCContamination ();


protected:
   anAlarmMsg _PTFCPSBaselineTestAlert;
   anAlarmMsg _PTFCPSBaselineTestAlarm;



   anAlarmMsg _PTFCPSAlert;
   anAlarmMsg _PTFCPSAlert2;
   anAlarmMsg _PTFCPSAlarm;

   // PTF CPS Baseline Calculation
   float _accumCPSPressure;
   float _averageCPSPressure;
   float _lowCPSPressure;
   float _highCPSPressure;

   int   _numberOfSamples;

   bool  _baselineDetermined;
   int   _numberOfAlerts;
   int   _AllowableFailures;

   float _HighPTFCPSBaselinePressureAlarmPoint;

   float _baselineVolumeEndPoint;

   bool  _resetForDouble;
   float _blackout_volume;




};

#endif

/* FORMAT HASH 62309edcdbbbba7692b75590d5ef6437 */
