/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      postrundisplay.cpp
 *
 */

#include "posthct.hpp"
#include "postrundisplay.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "pqinformation.h"
#include "trima_tasks.h"
#include <time.h>


DEFINE_STATE(PostRunDisplay);

PostRunDisplay::PostRunDisplay()
   : StateAbs(),
     _FinishedMsg(),
     _Finished(0)
{}

PostRunDisplay::PostRunDisplay(const PostRunDisplay& state)
   : StateAbs(state),
     _FinishedMsg(),
     _Finished(0)
{
   copyOver(state);
}

PostRunDisplay::~PostRunDisplay()
{
   cleanup();
}

int PostRunDisplay::transitionStatus ()
{
   if ( _Finished )
   {
      return 1;
   }
   return NO_TRANSITION;
}

void PostRunDisplay::finished ()
{
   DataLog(log_level_proc_info) << "Shutdown for next procedure" << endmsg;
   trimaStartShutdown(TTE_NextProcedure);

   //
   // We are done ...
   //
   _Finished = 1;
}

int PostRunDisplay::preProcess ()
{
   return NORMAL;
}

int PostRunDisplay::postProcess ()
{
   return NORMAL;
}

int PostRunDisplay::preEnter ()
{
   ProcData pd;
   //
   // From this point forward, we no longer want to save power fail data
   //
   pd.PFRHandler().DisablePFSave();

   //
   // Finished viewing stats messsage ...
   //
   _FinishedMsg.init(Callback<PostRunDisplay>(this, &PostRunDisplay::finished) );

   //
   // Send POST_RUN state message ...
   //
   pd.SystemState(POST_RUN);

   //
   // We are not finished yet ...
   //
   _Finished = 0;


   return NORMAL;
}

int PostRunDisplay::postExit ()
{
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void PostRunDisplay::copyOver (const PostRunDisplay&)
{}

void PostRunDisplay::cleanup ()
{
   //
   // deregister my messages ...
   //
   _FinishedMsg.deregisterMsg();
}

/* FORMAT HASH 68be619841eb3072dd0c1c51209853bb */
