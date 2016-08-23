/************************************************************
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      .cpp
 *
 ************************************************************
 *
 *  $Log: cpsPFRTestAlarm.cpp $
 *  Revision 1.4  2007/10/26 16:49:48Z  jheiusb
 *  8082 -- PTF CPS alarms end product based
 *  Revision 1.3  2007/09/04 21:44:49Z  jheiusb
 *  8055 -- add high and low alarm limits for the CPS zero.
 *  Revision 1.2  2007/07/11 23:23:19Z  jheiusb
 *  7180 -- move the CPS alarm to after first draw to allow Rinseback
 *  Revision 1.1  2007/07/03 15:32:24Z  jheiusb
 *  Initial revision
 *
 *
 ************************************************************/

#include "cpsPFRTestAlarm.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_OBJ(cpsPFRTestAlarm);

cpsPFRTestAlarm::cpsPFRTestAlarm()
   : _PFRTestAlarm(CPS_PTF_PRIME_TEST),
     _done(false)
{}

cpsPFRTestAlarm::~cpsPFRTestAlarm()
{}

void cpsPFRTestAlarm::Monitor ()
{
   ProcData pd;

   if (!pd.RunTargets().MeterRbcs.Get())
   {
      return; // only for MSS PTF ie. MSSRBC
   }

   if ( pd.RunTargets().MeterRbcs.Get()
        && pd.Run().FirstDrawComplete.Get()
        && !pd.TrimaSet().PTFCPSTestPassed.Get()
        && !_done)
   {
      const float zeroAdjustedCPS = pd.Status().CPS() - pd.TrimaSet().CPSZero;
      _PFRTestAlarm.setAlarm();

      DataLog(log_level_proc_alarm_monitor_info)
         << "PTF Cassette detected.  CPS Pressure didn't rise "
         << CobeConfig::data().PTFCPSTestChange
         << " mm/Hg as expected. Pressure delta is "
         << zeroAdjustedCPS
         << " mm/Hg"
         << endmsg;


      // Trigger satisfied.  No further need to monitor
      _done = true ;
   }



}

/* FORMAT HASH 06f2bdb8a332e03fcb6ba05a37f8fb83 */
