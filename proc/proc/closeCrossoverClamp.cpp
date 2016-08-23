/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      NegPressTest2.cpp
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/negPressTest2.cpp 1.1 2007/02/20 22:10:52Z jheiusb Exp jheiusb $
$Log: negPressTest2.cpp $


*/

#include "closeCrossoverClamp.h"
#include "states.h"
#include "procdata.h"

#include "trimamessages.h"

DEFINE_STATE(CloseCrossoverClamp);

CloseCrossoverClamp::CloseCrossoverClamp()
   : StateAbs()
{}

CloseCrossoverClamp::CloseCrossoverClamp(const CloseCrossoverClamp& state)
   : StateAbs(state)
{}

CloseCrossoverClamp::~CloseCrossoverClamp()
{}

int CloseCrossoverClamp::preProcess ()
{

   // ProcData pd;

   // pd.Orders().Qcollect(0.0f);


   return NORMAL;
}

int CloseCrossoverClamp::postProcess ()
{
   return NORMAL;
}

int CloseCrossoverClamp::transitionStatus ()
{
   // no transition ... message recieved in state table;
   return NO_TRANSITION;
}

int CloseCrossoverClamp::preEnter ()
{

   DataLog(log_level_proc_disposable_test_info) << "Sending CloseCrossoverClampMsg to GUI" << endmsg;

   // Send a message to GUI to tell it that we have completed SS addition.
   CloseCrossoverClampMsg _closeCrossoverClampMsg(MessageBase::SEND_LOCAL);
   _closeCrossoverClampMsg.send(0);

   return NORMAL;
}

int CloseCrossoverClamp::postExit ()
{

   return NORMAL;
}

void CloseCrossoverClamp::copyOver (const CloseCrossoverClamp&)
{}

void CloseCrossoverClamp::cleanup ()
{}

/* FORMAT HASH e7fad5c9711dfd0a618fae4da5a8b403 */
