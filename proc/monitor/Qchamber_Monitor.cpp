/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Qchamber_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the LRS chamber flow monitor.
 *
 */

#include "Qchamber_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "pqi_constants.h"


// Define this class to the object dictionary
DEFINE_OBJ(Qchamber_Monitor);

// Constructor
Qchamber_Monitor::Qchamber_Monitor()
{}

// Destructor
Qchamber_Monitor::~Qchamber_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void Qchamber_Monitor::Monitor ()
{
   ProcData pd;

   if ( enableMonitoring()  )
   {
      // This is a Pause- and Purge-dependent flag
      if ( !nowPaused() && !nowPurging() && nowCollectingPlatelets() )
      {
         float A = LRSChamberCmdFlow();
         if ( A < scaleQch1(CobeConfig::data().Qch1) && A > VERY_SMALL_FLOW_RATE )
         {
            // LowChamberFlow
            _sout << "COUNT PLATELET PRODUCT because QchamberCommanded < " << scaleQch1(CobeConfig::data().Qch1) << ends;
            LogIt(LOW_CHAMBER_FLOW);
            _pqi.PutReason(LOW_CHAMBER_FLOW);

            pd.RequestWbcAlarm(__FILE__, __LINE__, "low chamber flow");
         }
      }
   }
}

float Qchamber_Monitor::scaleQch1 (const float Qch1)
{
   ProcData pd;
   return Qch1 * pd.Predict().Csf(pd.Run().Substate.Get() );
}

/* FORMAT HASH b9966544050e9eb3e7593e0d887aaf32 */
