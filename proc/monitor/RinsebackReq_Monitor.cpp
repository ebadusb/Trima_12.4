/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RinsebackReq_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Rinseback Request monitor.
 *
 */

#include "RinsebackReq_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(RinsebackReq_Monitor);

// Constructor, called at the time the "states" file is read in.
RinsebackReq_Monitor::RinsebackReq_Monitor()
{}

// Destructor
RinsebackReq_Monitor::~RinsebackReq_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void RinsebackReq_Monitor::Monitor ()
{
   // In this case, do nothing.
}

// Notify function, called whenever a RinsebackRequestMsg is received from GUI,
// indicating that the operator has pressed the button requesting Rinseback.
void RinsebackReq_Monitor::Notify ()
{
   if ( enableMonitoring() )
      aPQMonitor::RunEndedEarly();
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void RinsebackReq_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void RinsebackReq_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _RinsebackRequestMsg.init(Callback<RinsebackReq_Monitor>(this, &RinsebackReq_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void RinsebackReq_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH ff4a23d690d03855c244d6afbd044746 */
