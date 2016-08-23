/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PIR_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the PIR monitor.
 *
 */

#include "PIR_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(PIR_Monitor);

// Constructor, called at the time the "states" file is read in.
PIR_Monitor::PIR_Monitor()
 : _previousSubstate(INVALID_SUBSTATE)
{}

// Destructor
PIR_Monitor::~PIR_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void PIR_Monitor::Monitor ()
{
   if ( enableMonitoring()  )
   {
      ProcData    pd;

      State_names newSubstate = pd.Run().Substate.Get();    // Get the current Collection substate or INVALID_SUBSTATE if not in Collect

      if ( newSubstate == SS_PIR_PLASMA || newSubstate == SS_PIR_NOPLASMA )
      {
         // If transitioning into PIR...
         if ( _previousSubstate != SS_PIR_PLASMA && _previousSubstate != SS_PIR_NOPLASMA &&
              _previousSubstate != INVALID_SUBSTATE )
         {
            DataLog(_pqLog) << "PIR_Monitor: Start of PIR ( VinStartPIR=" << pd.Volumes().VinPIRStart.Get()
                            << " VinTargEndPIR=" << pd.Predict().volumeTarget(newSubstate) << " )"
                            << endmsg;
         }
         pd.MakePQIWritable();
         pd.PQI().wasInPIR.Set(true);
         pd.PQI().VinTargetEndPIR.Set(pd.Predict().volumeTarget(newSubstate));
         pd.PQI().VinEndedPIR.Set(pd.Volumes().Vin);
         pd.MakePQIReadable();
      }
      else if ( (_previousSubstate == SS_PIR_PLASMA || _previousSubstate == SS_PIR_NOPLASMA) &&
                (newSubstate != SS_PIR_PLASMA && newSubstate != SS_PIR_NOPLASMA)  // If transitioning out of PIR...
                )
      {
         // ... finalize PIR's actual ending Vin.
         pd.MakePQIWritable();
         pd.PQI().VinEndedPIR.Set(pd.Volumes().Vin.Get() );     // If transitioning out of PIR, current Vin is the actual end Vin of PIR.
         pd.MakePQIReadable();
         DataLog(_pqLog) << "PIR_Monitor: End of PIR ( VinTargEndPIR=" << pd.PQI().VinTargetEndPIR.Get()
                         << " VinEndedPIR=" << pd.PQI().VinEndedPIR.Get() << " )"
                         << endmsg;
      }
      _previousSubstate = newSubstate;    // Save the current substate as previous.
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void PIR_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void PIR_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      ProcData pd;

      _previousSubstate = pd.Run().Substate.Get();    // Get the current Collection substate or INVALID_SUBSTATE if not in Collect
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void PIR_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 972fe5538617a7dbb6c840fbd62ef720 */
