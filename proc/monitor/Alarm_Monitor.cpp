/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Alarm_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Alarm monitor.
 *
 */

#include "Alarm_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(Alarm_Monitor);

// Constructor, called at the time the "states" file is read in.
Alarm_Monitor::Alarm_Monitor()
{}

// Destructor
Alarm_Monitor::~Alarm_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void Alarm_Monitor::Monitor ()
{
   // In this case, do nothing.
}

// Notify function, called whenever a AlarmActiveMessage is received.
// Upon being called, processes the following:
//   1. the Valve Error alarms by setting an internal flag,
//   2. the Low Platelet Concentration Alert by setting the VERIFY_PLATELET_WBCS flag.
void Alarm_Monitor::Notify ()
{
   if ( enableMonitoring()  )
   {
      ProcData        pd;

      AlarmQueueEntry Alarm = _AlarmMsg.getData();

      switch ( Alarm.name() )
      {
         case COLLECT_VALVE_FAILURE :
         case PLASMA_VALVE_FAILURE :
         case RBC_VALVE_FAILURE :
         {
            pd.MakePQIWritable();
            pd.PQI().valveErrorDetection.Set(true);
            pd.MakePQIReadable();
            break;
         }
         case LOW_PLATELET_CONCENTRATION_ALERT :
         {
            float A = 1.0E-6f * pd.Run().IntegratedPlateletYield.Get() / pd.Volumes().PlateletYield.Get();
            DataLog(_pqLog) << (int)LOW_DETECTED_PLATELET_CONCENTRATION
                            << " COUNT PLATELET WBC because operator alerted to low detected platelet concentration (=" << A << ")"
                            << endmsg;
            _pqi.AddReasonToFlag(LOW_DETECTED_PLATELET_CONCENTRATION, VERIFY_PLATELET_WBCS);

            // Request the platelet contamination alert.
            pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

            break;
         }
         default :
            break;
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void Alarm_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void Alarm_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _AlarmMsg.init(Callback<Alarm_Monitor>(this, &Alarm_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      MonitorBase::enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void Alarm_Monitor::disable ()
{
   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH 82a7c94793889ecf7563ce672c324ad5 */
