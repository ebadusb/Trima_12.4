/*
 *
 * TITLE:      showClampForARM.cpp
 **/


#include "showClampingForARM.h"
#include "states.h"
#include "procdata.h"


DEFINE_STATE(ShowClampARM);


ShowClampARM::ShowClampARM()
   : StateAbs()
{}

ShowClampARM::ShowClampARM(const ShowClampARM& state)
   : StateAbs()
{}

ShowClampARM::~ShowClampARM()
{}

int ShowClampARM::preProcess ()
{

   return NORMAL;
}

int ShowClampARM::postProcess ()
{
   return NORMAL;
}


int ShowClampARM::preEnter ()
{

   ProcData pd;


   DataLog(log_level_proc_info) << "sending ShowClampFromARMMsg   "  << endmsg;
   ShowClampFromARMMsg showClamp(MessageBase::SEND_LOCAL);
   showClamp.send(0);


   return NORMAL;
}

int ShowClampARM::postExit ()
{

   return NORMAL;
}

void ShowClampARM::copyOver (const ShowClampARM&)
{}

void ShowClampARM::cleanup ()
{}

/* FORMAT HASH cc9952e9b06d09731988978ce1a15d2e */
