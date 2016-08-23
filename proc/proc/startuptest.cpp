/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startuptest.cpp
 *
 */

#include "startuptest.h"
#include "states.h"
#include "trimamessages.h"
#include "procdata.h"

DEFINE_STATE(StartupTest);

StartupTest::StartupTest()
   : StateAbs()
{}

StartupTest::StartupTest(const StartupTest& state)
   : StateAbs(state)
{}

StartupTest::~StartupTest()
{}

int StartupTest::preProcess ()
{
   return NORMAL;
}

int StartupTest::postProcess ()
{
   return NORMAL;
}

int StartupTest::preEnter ()
{
   //
   // Set Startup Test State message ...
   //
   ProcData pd;
   pd.SystemState(STARTUP_TESTS);


   if (pd.Config().Procedure.Get().key_autoflow)
   {
      DataLog(log_level_proc_info) << "AutoFlow ON" << endmsg;
   }
   else
   {


      DataLog(log_level_proc_info) << "AutoFlow OFF" << endmsg;
   }

   return NORMAL;
}

void StartupTest::copyOver (const StartupTest&)
{}

void StartupTest::cleanup ()
{}

/* FORMAT HASH 06ffc1a31b0964def95b6c5f40a2ace9 */
