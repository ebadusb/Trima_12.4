/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DonorHCT_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the Donor HCT monitor.
 *
 */

#include "DonorHCT_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"

// Define this class to the object dictionary
DEFINE_OBJ(DonorHCT_Monitor);

// Constructor
DonorHCT_Monitor::DonorHCT_Monitor()
{}

// Destructor
DonorHCT_Monitor::~DonorHCT_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the value
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void DonorHCT_Monitor::Monitor ()
{
   ProcData pd;


   if ( ( nowCollectingPlatelets() || nowAdjustingPlateletConcentration() )
        && pd.Status().CollectPump.CmdRPM() >= CobeConfig::data().MinimumPumpRPM )
   {
      const float currentDonorHCT = pd.Donor().Hct.Get() * 100.0f;    // Get donor hct.  Scale for comparison with Cobe Config value

      if ( pd.PQI().PlateletsWereCollected.Get() )
      {
         if ( ( CobeConfig::data().Tss1 <= 0.0f )
              || ( pd.ProcRunTime().getMins() - pd.Volumes().TimeOfVincv.Get() ) > CobeConfig::data().Tss1 )
         {
            if ( currentDonorHCT > CobeConfig::data().HCThigh )
            {
               // HighHCT
               _sout << "COUNT PLATELET PRODUCT because entered hematocrit of " << currentDonorHCT
                     << " > " << CobeConfig::data().HCThigh
                     << ends;
               LogIt(HIGH_HCT);
               _pqi.PutReason(HIGH_HCT);

               pd.RequestWbcAlarm(__FILE__, __LINE__, "high HCT");
            }
         }
      }
   }
}

/* FORMAT HASH 12363eac303c86d8e2d04a435adc0627 */
