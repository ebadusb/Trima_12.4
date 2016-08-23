/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      RBCDetCal_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the RCB Detector Calibration monitor.
 *
 */

#include "RBCDetCal_Monitor.h"
#include "procdata.h"
#include "an_alarm.h"

// Constants

// Define this class to the object dictionary
DEFINE_OBJ(RBCDetCal_Monitor);

// Constructor
RBCDetCal_Monitor::RBCDetCal_Monitor()
{}

// Destructor
RBCDetCal_Monitor::~RBCDetCal_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void RBCDetCal_Monitor::Monitor ()
{
   if ( enableMonitoring()  )
   {
      ProcData pd;

      // If the RBC detector is not calibrated
      if (  !pd.TrimaSet().RBCDetectorCalibrated.Get() )
      {
         // Have we collected any platelets yet?
         if ( nowCollectingPlatelets() )
         {
            _sout << "COUNT PLATELET WBC because RBC detector not calibrated during collect" << ends;
            LogIt2(RBC_DETECTOR_ERROR, VERIFY_PLATELET_WBCS);
            _pqi.AddReasonToFlag(RBC_DETECTOR_ERROR, VERIFY_PLATELET_WBCS);

         }
         // Have we collected any plasma yet?
         if ( nowCollectingPlasma() )
         {
            _sout << "COUNT PLASMA WBC because RBC detector not calibrated during collect" << ends;
            LogIt2(RBC_DETECTOR_ERROR, VERIFY_PLASMA_WBCS);
            _pqi.AddReasonToFlag(RBC_DETECTOR_ERROR, VERIFY_PLASMA_WBCS);
         }
      }
   }
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void RBCDetCal_Monitor::enable ()
{
   ProcData pd;
   MonitorBase::enableMonitoring(pd.TrimaSet().Cassette.Get() != cassette::RBCPLS);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void RBCDetCal_Monitor::disable ()
{
   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH 10eb6af6a272215966212af2f212f96b */
