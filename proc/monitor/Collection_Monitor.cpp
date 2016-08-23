/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Collection_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Collection monitor.
 *
 */

#include "Collection_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"

// Define this class to the object dictionary
DEFINE_OBJ(Collection_Monitor);

// Constructor
Collection_Monitor::Collection_Monitor()
{}

// Destructor
Collection_Monitor::~Collection_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void Collection_Monitor::Monitor ()
{
   if ( enableMonitoring()  )
   {
      ProcData pd;

      // Detect platelet collection phases, if any
      if ( ( nowCollectingPlatelets() || nowAdjustingPlateletConcentration() )
           && pd.Status().CollectPump.CmdRPM() >= CobeConfig::data().MinimumPumpRPM )
      {
         pd.MakePQIWritable();
         pd.PQI().PlateletsWereCollected.Set(true);
         pd.MakePQIReadable();
      }

      // Detect plasma collection phases, if any
      if ( nowCollectingPlasma()
           && ( pd.Status().PlasmaPump.CmdRPM() >= CobeConfig::data().MinimumPumpRPM ) )
      {
         pd.MakePQIWritable();
         pd.PQI().PlasmaWasCollected.Set(true);
         pd.MakePQIReadable();
      }

      // Detect RBC collection phases, if any
      if ( nowCollectingRbc() && ( RbcLineCmdFlow() > 0.0f ) )
      {
         pd.MakePQIWritable();
         pd.PQI().RbcWereCollected.Set(true);
         pd.MakePQIReadable();
      }

      // When all products have collected something, turn off this monitor.
      if ( pd.PQI().PlateletsWereCollected.Get()
           && pd.PQI().PlasmaWasCollected.Get()
           && pd.PQI().RbcWereCollected.Get() )
      {
         disable();
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void Collection_Monitor::Initialize ()
{
   // In this case, do nothing.
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void Collection_Monitor::enable ()
{
   MonitorBase::enableMonitoring(1);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void Collection_Monitor::disable ()
{
   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH 23a6379abe5bba54000421da71210408 */
