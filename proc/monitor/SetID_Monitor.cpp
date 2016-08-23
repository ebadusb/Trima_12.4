/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SetID_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Set ID monitor.
 *
 */

#include "SetID_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(SetID_Monitor);

// Constructor, called at the time the "states" file is read in.
SetID_Monitor::SetID_Monitor()
{}

// Destructor
SetID_Monitor::~SetID_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void SetID_Monitor::Monitor ()
{
   // In this case, do nothing.
}

// Notify function, called whenever a OperatorSetIdentificationMsg is received.
void SetID_Monitor::Notify ()
{
   if ( enableMonitoring() )
   {
      ProcData pd;

      // The fact that the operator identifies a Platelet/Plasma/RBC set
      // is Prima Facie evidence that the RBC detector is acting up.
      if ( (cassette::cassetteType)_IdentifySetMsg.getData() == cassette::PLTPLSRBC)
      {
         pd.MakePQIWritable();
         pd.PQI().RBCDetectorError.Set(true);
         pd.MakePQIReadable();
      }
      disable();    // We don't need to repeat this.
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void SetID_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void SetID_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _IdentifySetMsg.init(Callback<SetID_Monitor>(this, &SetID_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void SetID_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 9865acced106eb789c54635f8df3dd08 */
