/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      CancelProc_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Cancel Procedure message monitor.
 *
 */

#include "CancelProc_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(CancelProc_Monitor);

// Constructor, called at the time the "states" file is read in.
CancelProc_Monitor::CancelProc_Monitor()
{}

// Destructor
CancelProc_Monitor::~CancelProc_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void CancelProc_Monitor::Monitor ()
{
   // In this case, do nothing.
}

// Notify function, called whenever a CancelProcedureMsg is received from GUI,
// indicating that the operator has pressed the "Cancel Procedure" button.
void CancelProc_Monitor::Notify ()
{
   aPQMonitor::RunEndedEarly();
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void CancelProc_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void CancelProc_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _CancelProcedureMsg.init(Callback<CancelProc_Monitor>(this, &CancelProc_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void CancelProc_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 9892864a5f72c1276f673ffd3a190197 */
