/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Centrifuge_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Centrifuge monitor.
 *
 */

#include "Centrifuge_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "recoverytypes.h"

// Define this class to the object dictionary
DEFINE_OBJ(Centrifuge_Monitor);

// Constructor, called at the time the "states" file is read in.
Centrifuge_Monitor::Centrifuge_Monitor()
   : _alreadyFlaggedCentrifugeStop(false),
     _CentrifugeTrigger(0.0f)
{}

// Destructor
Centrifuge_Monitor::~Centrifuge_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void Centrifuge_Monitor::Monitor ()
{
   if ( enableMonitoring() )
   {
      ProcData          pd;
      const State_names currentSubstate = pd.Run().Substate.Get();    // Get the current Collection substate or INVALID_SUBSTATE if not in Collect

      // Check for centrifuge stop recovery.
      if ( (currentSubstate == SS_RBC)
           || (currentSubstate == SS_RP_RBC)
           || (currentSubstate == SS_RP_RBC_DOUBLE)  )
      {
         // Centrifuge Stop Flag check for RBC Collection Volume.
         if ( nowRecoveringCentrifugeStop() )
         {
            if (pd.Run().drbcSplitDone.Get()
                && pd.Volumes().RBCProduct_B_FinalVolume.Get() > 0.0f)
            {
               _sout << "VERIFY RBC VOLUME (PROD2) because of CENTRIFUGE_STOP_RECOVERY" << ends;
               LogIt2(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL_PROD2);
               _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL_PROD2);
            }
            else
            {
               _sout << "VERIFY RBC VOLUME because of CENTRIFUGE_STOP_RECOVERY" << ends;
               LogIt2(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL);
               _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL);
            }
         }
      }
      else if ( (pd.Status().CentrifugeRPM() < _CentrifugeTrigger)
                && !((pd.Run().PreviousRecovery.Get() == RecoveryTypes::AirBlock)
                     || (pd.Run().CurrentRecovery.Get() == RecoveryTypes::AirBlock)
                     )
                )
      {
         // Flag Platelet Collection WBC and Volume/Yield.
         if ( pd.TrimaSet().Cassette.Get() != cassette::RBCPLS )
         {
            if (!_alreadyFlaggedCentrifugeStop)
            {
               _alreadyFlaggedCentrifugeStop = true;

               _sout << "COUNT PLATELET WBC because of CENTRIFUGE_STOP_TRIGGER" << ends;
               LogIt2(STOPPED_CENTRIFUGE, VERIFY_PLATELET_WBCS);
               _sout << "VERIFY PLATELET YIELD because of CENTRIFUGE_STOP_TRIGGER" << ends;
               LogIt2(STOPPED_CENTRIFUGE, VERIFY_PLATELET_YIELD);
               _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLATELET_WBCS);
               _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLATELET_YIELD);

               // Request the platelet contamination alert.
               pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
            }
         }
         // Flag Plasma Collection WBC.
         {
            _sout << "COUNT PLASMA WBC because of CENTRIFUGE_STOP_TRIGGER" << ends;
            LogIt2(STOPPED_CENTRIFUGE, VERIFY_PLASMA_WBCS);
            _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLASMA_WBCS);
         }
         // Flag Plasma Collection Volume.
         switch (currentSubstate)
         {
            case SS_PREPLATELET_PLASMA :
            case SS_PLATELET_PLASMA :
            case SS_PIR_PLASMA :
            case SS_EXTENDED_PLASMA :
            case SS_RP_PLASMA_ONLY :           // RBC-Plasma plasma only
            {
               _sout << "VERIFY PLASMA VOLUME because of CENTRIFUGE_STOP_TRIGGER" << ends;
               LogIt2(STOPPED_CENTRIFUGE, VERIFY_PLASMA_VOL);
               _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLASMA_VOL);
            }
            default :
               break;
         }
      }
   }
}

// Notify function, called whenever a CentrifugeStopMsg is received.
void Centrifuge_Monitor::Notify ()
{
   if ( enableMonitoring()  )
   {
      ProcData pd;

      if ( nowCollectingPlatelets() || nowAdjustingPlateletConcentration() )
      {
         DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                         << " COUNT PLATELET WBC because of CENTRIFUGE_STOP_NOTIFY"
                         << endmsg;
         DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                         << " VERIFY PLATELET YIELD because of CENTRIFUGE_STOP_NOTIFY"
                         << endmsg;
         _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLATELET_WBCS);
         _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLATELET_YIELD);

         const char str [] =  "PLT contamination alert: centrifuge stopped";
         pd.RequestWbcAlarm(__FILE__, __LINE__, str);

      }

      if ( nowCollectingPlasma() )
      {
         DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                         << " COUNT PLASMA WBC because of CENTRIFUGE_STOP_NOTIFY"
                         << endmsg;
         DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                         << " VERIFY PLASMA VOLUME because of CENTRIFUGE_STOP_NOTIFY"
                         << endmsg;
         _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLASMA_WBCS);
         _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_PLASMA_VOL);
      }
      if ( nowCollectingRbc() )
      {
         if (pd.Run().drbcSplitDone.Get())
         {
            DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                            << " VERIFY RBC VOLUME (PROD2) because of CENTRIFUGE_STOP_NOTIFY"
                            << endmsg;
            _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL_PROD2);
         }
         else
         {
            DataLog(_pqLog) << (int)STOPPED_CENTRIFUGE
                            << " VERIFY RBC VOLUME because of CENTRIFUGE_STOP_NOTIFY"
                            << endmsg;
            _pqi.AddReasonToFlag(STOPPED_CENTRIFUGE, VERIFY_RBC_VOL);
         }
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void Centrifuge_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void Centrifuge_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _CentrifugeStopMsg.init(Callback<Centrifuge_Monitor>(this, &Centrifuge_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);

      /* todo:
      if (sw.dat setting for centrifuge scaling is on)
      {*/
      // Trigger centrifuge stop flat at 1300 rpm
      _CentrifugeTrigger = CobeConfig::data().CentStopRPMTriggerScaled;
      /*}
      else
      {
          // Trigger centrifuge stop flat at 1650 rpm
          _CentrifugeTrigger = CobeConfig::data().CentStopRPMTrigger;
      }*/
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void Centrifuge_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 96164bdbcd6a358defe72a6c4d76ebea */
