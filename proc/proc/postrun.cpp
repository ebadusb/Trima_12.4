/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      postrun.cpp
 *
 */

#include "postrun.h"
#include "states.h"

DEFINE_STATE(PostRun);

PostRun::PostRun()
   : StateAbs()
{}

PostRun::PostRun(const PostRun& state)
   : StateAbs(state)
{
   copyOver(state);
}

PostRun::~PostRun()
{
   cleanup();
}

int PostRun::preProcess ()
{
   return NORMAL;
}

int PostRun::postProcess ()
{
   return NORMAL;
}

int PostRun::preEnter ()
{
   return NORMAL;
}

int PostRun::postExit ()
{
   return NORMAL;
}

void PostRun::cleanup ()
{}

/* FORMAT HASH 614244e41dd8f609c9b68fe0c1a97d21 */
