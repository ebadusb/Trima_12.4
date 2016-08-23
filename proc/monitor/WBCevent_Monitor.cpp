/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * TITLE:      WBCevent_Monitor.cpp
 *
 */

#include "WBCevent_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(WBCEvent_Monitor);

// Constructor, called at the time the "states" file is read in.
WBCEvent_Monitor::WBCEvent_Monitor()
{}

// Destructor
WBCEvent_Monitor::~WBCEvent_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void WBCEvent_Monitor::Monitor ()
{
   if ( enableMonitoring() )
   {
      ProcData pd;

      if ( pd.Run().FlagforShortDrawNow.Get() )
      {

         // flag now not at the endrun...
         LogIt2(SHORT_DRAW, VERIFY_PLATELET_YIELD);
         DataLog(_pqLog) << (int)SHORT_DRAW << " VERIFY PLATELET YIELD because a short draw occurred" << endmsg;
         _pqi.AddReasonToFlag (SHORT_DRAW, VERIFY_PLATELET_YIELD);

         LogIt2(SHORT_DRAW, VERIFY_PLATELET_WBCS);
         DataLog(_pqLog) << (int)SHORT_DRAW << " COUNT PLATELET WBC because a short draw occurred" << endmsg;
         _pqi.AddReasonToFlag (SHORT_DRAW, VERIFY_PLATELET_WBCS);

         // Request the platelet contamination alert.
         pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

         disable();

      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void WBCEvent_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void WBCEvent_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void WBCEvent_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 4914a2296f9773c0022c4ed88c542ed1 */
