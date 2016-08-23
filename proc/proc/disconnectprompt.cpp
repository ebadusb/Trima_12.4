/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DisconnectPrompt.cpp
 *
 */

#include "disconnectprompt.h"
#include "procdata.h"

DEFINE_STATE(DisconnectPrompt);

DisconnectPrompt::DisconnectPrompt()
   : StateAbs()
{}

DisconnectPrompt::~DisconnectPrompt()
{}

int DisconnectPrompt::preProcess ()
{
   return NORMAL;
}

int DisconnectPrompt::postProcess ()
{
   return NORMAL;
}

int DisconnectPrompt::preEnter ()
{
   ProcData pd;

   //
   // Save the end run time here ...
   //
   time_t t;
   time(&t);
   pd.MakeRunWritable();
   pd.Run().ProcEndRunTime.Set(t);
   DataLog(log_level_proc_donor_disconnect_info) << "End of run time: " << pd.Run().ProcEndRunTime.Get() << endmsg;

   return NORMAL;
}

int DisconnectPrompt::postExit ()
{
   return NORMAL;
}

/* FORMAT HASH 1714bdab6385ca3b321ec956edfa9841 */
