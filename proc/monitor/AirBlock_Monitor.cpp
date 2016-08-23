/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      AirBlock_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the AirBlock monitor.
 *
 */

#include "AirBlock_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(AirBlock_Monitor);

// Constructor, called at the time the "states" file is read in.
AirBlock_Monitor::AirBlock_Monitor()
{}

// Destructor
AirBlock_Monitor::~AirBlock_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void AirBlock_Monitor::Monitor ()
{
   if ( enableMonitoring() )
   {
      // Check for plasma line air block  recovery.
      if ( nowRecoveringAirBLock() )
      {
         ProcData pd;

         // Flag Platelet Collection WBC and Volume/Yield.
         if ( pd.TrimaSet().Cassette.Get() != cassette::RBCPLS )
         {
            _sout << "COUNT PLATELET WBC because of AirBlock recovery" << ends;
            LogIt2(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_WBCS);
            _sout << "VERIFY PLATELET YIELD because of AirBlock recovery" << ends;
            LogIt2(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_YIELD);
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_WBCS);
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_YIELD);

            pd.RequestWbcAlarm(__FILE__, __LINE__, "air block recovery");
         }
         // Flag Plasma Collection WBC and Volume.
         {
            _sout << "COUNT PLASMA WBC because of AirBlock recovery" << ends;
            LogIt2(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_WBCS);
            _sout << "VERIFY PLASMA VOLUME because of AirBlock recovery" << ends;
            LogIt2(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_VOL);
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_WBCS);
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_VOL);
         }

         // Flag RBC Collection Volume.
         switch (pd.Run().Substate.Get())
         {
            case SS_RBC :
            case SS_RP_RBC :                   // RBC collection
            {
               if (pd.Run().drbcSplitDone.Get())
               {
                  _sout << "VERIFY RBC VOLUME because of AirBlock recovery (prod2)" << ends;
                  LogIt2(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL_PROD2);
                  _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL_PROD2);
               }
               else
               {
                  _sout << "VERIFY RBC VOLUME because of AirBlock recovery" << ends;
                  LogIt2(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL);
                  _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL);
               }
            }
            default :
               break;
         }
      }
   }
}

// Notify function, called whenever a AirInPlasmaLineMsg is received from GUI,
// indicating that the operator has pressed the "Air in Plasma Line" button.
void AirBlock_Monitor::Notify ()
{
   if ( enableMonitoring()  )
   {
      ProcData pd;

      if ( nowCollectingPlatelets() || nowAdjustingPlateletConcentration() )
      {
         DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                         << " COUNT PLATELET WBC because of AirBlock recovery"
                         << endmsg;
         DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                         << " VERIFY PLATELET YIELD because of AirBlock recovery"
                         << endmsg;
         _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_WBCS);
         _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLATELET_YIELD);

         pd.RequestWbcAlarm(__FILE__, __LINE__, "air block recovery");
      }
      if ( nowCollectingPlasma() )
      {
         DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                         << " COUNT PLASMA WBC because of AirBlock recovery"
                         << endmsg;
         DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                         << " VERIFY PLASMA VOLUME because of AirBlock recovery"
                         << endmsg;
         _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_WBCS);
         _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_PLASMA_VOL);
      }
      if ( nowCollectingRbc() )
      {
         if (pd.Volumes().RBCProduct_B_FinalVolume.Get() > 0.0f)
         {
            DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                            << " VERIFY RBC VOLUME because of AirBlock recovery (prod2)"
                            << endmsg;
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL_PROD2);
         }
         else
         {
            DataLog(_pqLog) << (int)AIR_BLOCK_RECOVERY
                            << " VERIFY RBC VOLUME because of AirBlock recovery"
                            << endmsg;
            _pqi.AddReasonToFlag(AIR_BLOCK_RECOVERY, VERIFY_RBC_VOL);
         }
      }
   }
}

// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void AirBlock_Monitor::Initialize ()
{
   // Do nothing
}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void AirBlock_Monitor::enable ()
{
   if ( !enableMonitoring()  )
   {
      _AirInPlasmaLineMsg.init(Callback<AirBlock_Monitor>(this, &AirBlock_Monitor::Notify), MessageBase::SNDRCV_RECEIVE_ONLY);
      enableMonitoring(1);
   }
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void AirBlock_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 364908288095958052038786162412f9 */
