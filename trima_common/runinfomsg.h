/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      runinfomsg.h
 *             Local object to send/receive run information between Proc
 *            and GUI.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#if !defined( INCLUDE_RUNINFOMSG )
#define INCLUDE_RUNINFOMSG

#include "runinfo.h"
#include "configdef.h"
#include "message.h"
#include "proctime_cds.h"
#include "procvolumes_cds.h"
#include "procrun_cds.h"
#include "config_cds.h"

class CRunInfoMsg
{
public:
   CRunInfoMsg();
   virtual ~CRunInfoMsg();


   CURRENT_PROCEDURE_STATUS_STRUCT&       Data (void);
   const CURRENT_PROCEDURE_STATUS_STRUCT& cData (void) const;

   // Call to send off the data to GUI
   virtual void SendMsg (void);

   // Setting the data member fns
   virtual void Initialize ();

   // This  resets the start time to zero.
   void SetStartTime (BaseElement<float>& startTime);

   // Call this whenever receive optimizer targets
   void SetTargets (float targYield,     // *1E11 platlets
                    float targPlasma,    // ml
                    float targRBC        // ml
                    );

   // Set the alarm status
   void AlarmStatus (bool alarmActive) { m_Data.alarm_condition = (char)alarmActive; }

   // Each status update after volume accumulation
   void Update ();

   // Call with updated pressure info each status update
   void Pressure (const float APSLow, const float APSHigh, const float APSCur, const int meter);


protected:

   CURRENT_PROCEDURE_STATUS_STRUCT          m_Data;
   Message<CURRENT_PROCEDURE_STATUS_STRUCT> m_message;
   //
   // Common data store access
   //
   ProcTime_CDS    _timecds;
   ProcVolumes_CDS _volumescds;
   ProcRun_CDS     _runcds;
   Config_CDS      _configcds;

   bool            _isPtfDrbc; // Was this initially targetted as a PTF DRBC procedure?


};
#endif

/* FORMAT HASH c31cd1e0618c176be047a6ba0cb15ef6 */
