/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SpillDet_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the SpillDet monitor.
 *
 */

#include "SpillDet_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(SpillDet_Monitor);

// Constructor, called at the time the "states" file is read in.
SpillDet_Monitor::SpillDet_Monitor()
   : _spilloverDetected(false)
{}

// Destructor
SpillDet_Monitor::~SpillDet_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void SpillDet_Monitor::Monitor ()
{
   if (enableMonitoring())
   {
      //
      // For this flag, the spillover should be flagged against both platelet or plasma products
      // regardless of the relationship between the spill and the time of collection.  If the products
      // are not collected, then the post run monitor will clear the flag.
      //
      if (_spilloverDetected)
      {
         const bool purgeDueToChSat = _pd.Run().PurgeDueToChamberSat.Get();

         // IT 7021.  If we are doing a chamber purge, then we use the platelet contaminant reason
         if (purgeDueToChSat)
         {
            _sout << "COUNT PLATELET WBC because of platelet contamination detection" << ends;
            LogIt2(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_WBCS);
            _pqi.AddReasonToFlag(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_WBCS);

            _sout << "VERIFY PLATELET YIELD because of platelet contamination detection" << ends;
            LogIt2(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_YIELD);
            _pqi.AddReasonToFlag(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_YIELD);
         }
         else
         {
            _sout << "COUNT PLATELET WBC because of rbc spillover detection" << ends;
            LogIt2(SPILL_DETECTION, VERIFY_PLATELET_WBCS);
            _pqi.AddReasonToFlag(SPILL_DETECTION, VERIFY_PLATELET_WBCS);

            _sout << "VERIFY PLATELET YIELD because of rbc spillover detection" << ends;
            LogIt2(SPILL_DETECTION, VERIFY_PLATELET_YIELD);
            _pqi.AddReasonToFlag(SPILL_DETECTION, VERIFY_PLATELET_YIELD);
         }

         // Trigger the Operator alert if needed
         char str[64];
         sprintf(str, "PLT contamination alert: %s detected", (purgeDueToChSat ? "contamination" : "spillover"));
         _pd.RequestWbcAlarm(__FILE__, __LINE__, str);

         _sout << "COUNT PLASMA WBC because of rbc spillover detection" << ends;
         LogIt2(SPILL_DETECTION, VERIFY_PLASMA_WBCS);
         _pqi.AddReasonToFlag(SPILL_DETECTION, VERIFY_PLASMA_WBCS);

         // This monitor only fires once.
         disable();
      }
   }
}


// Notify function, called whenever a SpilloverDetectedMsg is received.
void SpillDet_Monitor::Notify ()
{
   if (enableMonitoring())
   {
      _spilloverDetected = true;

      DataLog(_pqLog) << "Received RBC Spillover Notification.  Enabling spillover detection." << endmsg;

      // Add guard to only set this once.
      if (!_pd.PQI().rbcSpilloverDetection.Get())
      {
         _pd.MakePQIWritable();
         _pd.PQI().rbcSpilloverDetection.Set(true);  // Set the RBC Spillover Detection flag for later.
         _pd.MakePQIReadable();
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void SpillDet_Monitor::Initialize ()
{}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void SpillDet_Monitor::enable ()
{
   if ( !enableMonitoring() &&
        _pd.TrimaSet().Cassette.Get() != cassette::RBCPLS ) // IT 5825
   {
      _SpillDetMsg.init(Callback<SpillDet_Monitor>(this, &SpillDet_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void SpillDet_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 01dc90cfd035c8e1742e80cdd0bee21f */
