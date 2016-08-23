/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ValveFault_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the valve fault monitor.
 *
 */

#include "ValveFault_Monitor.h"
#include "procdata.h"

// Define this class to the object dictionary
DEFINE_OBJ(ValveFault_Monitor);

// Constructor
ValveFault_Monitor::ValveFault_Monitor()
{}

// Destructor
ValveFault_Monitor::~ValveFault_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void ValveFault_Monitor::Monitor ()
{
   ProcData pd;

   if ( pd.Status().ValveFault() || pd.PQI().valveErrorDetection.Get() )
   {
      if ( pd.RunTargets().SelectedPlasmaVolume.Get() > 0.0f )
      {
         _sout << "VERIFY PLASMA VOLUME because of Valve Fault" << ends;
         LogIt(PLASMA_VOLUME_ERROR);
         _pqi.PutReason(PLASMA_VOLUME_ERROR);
      }
      if ( pd.RunTargets().SelectedPlateletYield.Get() > 0.0f )
      {
         _sout << "VERIFY PLATELET VOLUME because of Valve Fault" << ends;
         LogIt(PLATELET_VOLUME_ERROR);
         _pqi.PutReason(PLATELET_VOLUME_ERROR);
      }
      if ( pd.RunTargets().SelectedRbcVolume.Get() > 0.0f )
      {
         bool isRAS        =  pd.RunTargets().MeterRbcs.Get();
         bool doneTheSplit = pd.Run().drbcSplitDone.Get();

         // IT 8306 - Always determine whether this is prompted split based on configured
         // rbc dose with no scaling or adjustments
         bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());

         // we need to choose the correct RBC product to flag.......
         if (doubleTargeted && isRAS)
         {
            if (doneTheSplit)
            {
               _sout << "VERIFY RBC VOLUME because of Valve Fault" << ends;
               LogIt(RBC_VOLUME_ERROR);
               _pqi.PutReasonRBCprod2(RBC_VOLUME_ERROR);
            }
            else
            {
               _sout << "VERIFY RBC VOLUME because of Valve Fault" << ends;
               LogIt(RBC_VOLUME_ERROR);
               _pqi.PutReason(RBC_VOLUME_ERROR);
            }
         }
         else
         {
            _sout << "VERIFY RBC VOLUME because of Valve Fault" << ends;
            LogIt(RBC_VOLUME_ERROR);
            _pqi.PutReason(RBC_VOLUME_ERROR);
         }

      }    // end if ( pd.RunTargets().SelectedRbcVolume.Get() > 0.0f )

   }   // end if( pd.Status().ValveFault() || pd.PQI().valveErrorDetection.Get() )

}

/* FORMAT HASH 6e9ce858e1b033e8bb75d0063ef5b92f */
