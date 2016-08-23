/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DeltaHCT_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Donor Delta HCT monitor.
 *
 */

#include "DeltaHCT_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"

// Define this class to the object dictionary
DEFINE_OBJ(DeltaHCT_Monitor);

// Constructor, called at the time the "states" file is read in.
DeltaHCT_Monitor::DeltaHCT_Monitor()
   : _previousDonorHCT(0.0f), _hctTrigger(0.0f)
{}

// Destructor
DeltaHCT_Monitor::~DeltaHCT_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void DeltaHCT_Monitor::Monitor ()
{
   if ( enableMonitoring()  )
   {
      ProcData    pd;

      const float currentDonorHCT = pd.Donor().Hct.Get();

      // Per IT 5682 and followup meetings/emails, ALWAYS look for high delta-HCT.
      if ( ( currentDonorHCT - _previousDonorHCT ) >= _hctTrigger)
      {
         // HighDeltaHCT
         DataLog(_pqLog) << "COUNT PLATELET PRODUCT because entered hematocrit increase of >= " << CobeConfig::data().Hcmin
                         << "% from " << _previousDonorHCT
                         << " to " << currentDonorHCT << " occurred"
                         << endmsg;

         _pqi.PutReason(HIGH_DELTA_HCT);
         // Keep sampling current Donor Hct
         _previousDonorHCT = currentDonorHCT;

         pd.RequestWbcAlarm(__FILE__, __LINE__, "high delta HCT");
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void DeltaHCT_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void DeltaHCT_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      ProcData pd;

      // Sample the current Donor Hct to seed the checking algorithm
      _previousDonorHCT = pd.Donor().Hct.Get();

      // Set the crit trigger value.  Match % hct for correct comparison
      _hctTrigger = CobeConfig::data().Hcmin / 100.0f;

      enableMonitoring(1);

      DataLog(_pqLog) << "Baseline Donor Crit is " << _previousDonorHCT
                      << "%.  Crit Delta is " << _hctTrigger << "." << endmsg;
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void DeltaHCT_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 3d13b9b2e4b4443ac9c718c0a9e52fc6 */
