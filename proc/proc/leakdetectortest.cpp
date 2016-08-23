/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      leakdetectortest.cpp
 *
 */


#include "leakdetectortest.h"
#include "procdata.h"

DEFINE_STATE(LeakDetectorTest) LeakDetectorTest::LeakDetectorTest()
   : StateAbs(),
     _LeakDetectorAlarm(LEAK_TEST_FAILURE),
     _TestPassed(0)
{}

LeakDetectorTest::LeakDetectorTest(const LeakDetectorTest& state)
   : StateAbs(state),
     _LeakDetectorAlarm(LEAK_TEST_FAILURE),
     _TestPassed(state._TestPassed)
{}

LeakDetectorTest::~LeakDetectorTest()
{
   cleanup();
}

int LeakDetectorTest::transitionStatus ()
{
   //
   // Check for leak detection pass
   //
   if ( _TestPassed )
   {
      DataLog(log_level_proc_debug) << "Leak detector test passed" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}

int LeakDetectorTest::preProcess ()
{
   ProcData pd;
   if ( pd.Status().LeakDetected() ||
        pd.Status().LeakError() )
   {
      //
      // Send alarm message indicating leak detector test failed
      //
      _LeakDetectorAlarm.setAlarm();
      DataLog(log_level_proc_startup_test_info) << "Leak test failed -- Leak value: " << pd.Status().LeakValue() << endmsg;
   }
   else
   {
      //
      // The test passed ...
      //
      _TestPassed = 1;
   }
   return NORMAL;
}

int LeakDetectorTest::postProcess ()
{
   return NORMAL;
}

void LeakDetectorTest::reset ()
{
   //
   // Reset the flags ...
   //
   _TestPassed = 0;
}

void LeakDetectorTest::cleanup ()
{}

/* FORMAT HASH 41a080b8a3a9f3d35ce5c0515f5fbfb6 */
