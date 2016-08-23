/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      SpillReq_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Spillover recovery Request monitor.
 *
 */

#include "SpillReq_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(SpillReq_Monitor);

// Constructor, called at the time the "states" file is read in.
SpillReq_Monitor::SpillReq_Monitor()
   : _alreadyLoggedSpilloverRecovery(false),
     _alreadyLoggedPltContamination(false)
{}

// Destructor
SpillReq_Monitor::~SpillReq_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void SpillReq_Monitor::Monitor ()
{
   if ( enableMonitoring()  )
   {
      // These are Pause- and Purge-dependent flags
      if ( !nowPaused() && !nowPurging() )
      {
         ProcData pd;

         // Spillover recovery
         if ( pd.PQI().rbcSpilloverRecovery.Get() )
         {
            if ( nowCollectingPlatelets() || nowAdjustingPlateletConcentration() )
            {
               bool purgeDueToChSat = pd.Run().PurgeDueToChamberSat.Get();

               // IT 7021 change reaon if for chamber purge:
               if ( purgeDueToChSat && !_alreadyLoggedPltContamination)
               {
                  // To get here the operator had to have pushed the spillover recovery button
                  // so a WBC contamination alert has already been shown, so just add the
                  // reason code for this.
                  _alreadyLoggedPltContamination = true;
                  _sout << "COUNT PLATELET WBC because of platelet contamination detection" << ends;
                  LogIt2(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_WBCS);
                  _pqi.AddReasonToFlag(PLATELET_CONTAMINANT_DETECTION, VERIFY_PLATELET_WBCS);
               }
            }

            if ( nowCollectingPlasma() )
            {
               _sout << "COUNT PLASMA WBC because of rbc spillover recovery" << ends;
               LogIt2(SPILL_RECOVERY, VERIFY_PLASMA_WBCS);
               _pqi.AddReasonToFlag(SPILL_RECOVERY, VERIFY_PLASMA_WBCS);
            }
         }
      }
   }
}

// Notify function, called whenever a SpilloverRecoveryMessage is received from GUI,
// indicating that the operator has pressed the "Spillover Recovery" button.
void SpillReq_Monitor::Notify ()
{
   if ( enableMonitoring()  )
   {
      ProcData pd;

      pd.MakePQIWritable();
      pd.PQI().rbcSpilloverRecovery.Set(true);    // Set the RBC Spillover Recovery flag for later.
      pd.MakePQIReadable();

      // the operator requested a spillover recovery so set the reason flag and show the wbc alert right away
      if (!_alreadyLoggedSpilloverRecovery)   // Only do this once.
      {
         _alreadyLoggedSpilloverRecovery = true;
         _sout << "COUNT PLATELET WBC because of rbc spillover recovery" << ends;
         LogIt2(SPILL_RECOVERY, VERIFY_PLATELET_WBCS);
         _pqi.AddReasonToFlag(SPILL_RECOVERY, VERIFY_PLATELET_WBCS);
         _sout << "VERIFY PLATELET YIELD because of rbc spillover recovery" << ends;
         LogIt2(SPILL_RECOVERY, VERIFY_PLATELET_YIELD);
         _pqi.AddReasonToFlag(SPILL_RECOVERY, VERIFY_PLATELET_YIELD);
      }

      char str[64];
      sprintf(str, "PLT contamination alert: spillover recovery");
      pd.RequestWbcAlarm(__FILE__, __LINE__, str);
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void SpillReq_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void SpillReq_Monitor::enable ()
{
   if ( !enableMonitoring() )
   {
      _SpillReqMsg.init(Callback<SpillReq_Monitor>(this, &SpillReq_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void SpillReq_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 61acb2f9e5c67ba933704dc6abb99c22 */
