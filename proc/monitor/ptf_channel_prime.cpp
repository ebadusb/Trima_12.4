/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PTFChannelPrimeTest
 *
 * ABSTRACT:
 *    This module only applies to Press Thru Filtered sets on Trima.
 *    This module looks to see if the CPS pressure rises greater than 300 mmHg (CC) during Channel
 *    prime.  If so, then a CDS flag is set (defaults to zero).  This flag is checked duing the
 *    state transition to ramp centrifuge.
 *
 */

#include "ptf_channel_prime.h"
#include "procdata.h"
#include "trima_datalog.h"
#include "cobeconfig.h"

DEFINE_OBJ(PTFChannelPrimeTest);

PTFChannelPrimeTest::PTFChannelPrimeTest()
{}


PTFChannelPrimeTest::~PTFChannelPrimeTest()
{}



void PTFChannelPrimeTest::Monitor ()
{
   ProcData    pd;

   const float zeroAdjustedCPS = pd.Status().CPS() - pd.TrimaSet().CPSZero;

   if (zeroAdjustedCPS > CobeConfig::data().PTFCPSTestChange)
   {
      // log it.. but only once.
      if (!pd.TrimaSet().PTFCPSTestPassed.Get())
         DataLog(log_level_proc_alarm_monitor_info) << "CPS pressure target of "
                                                    << CobeConfig::data().PTFCPSTestChange << " reached." << endmsg;

      pd.MakeTrimaSetWritable();
      pd.TrimaSet().PTFCPSTestPassed.Set(true);
      pd.MakeTrimaSetReadable();

      // Trigger satisfied.  No further need to monitor
      disable();
   }

}

/* FORMAT HASH 7fc119a721f4a7982f429b16b84c01cf */
