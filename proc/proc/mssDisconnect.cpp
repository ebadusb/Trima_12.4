/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssDisconnect.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssDisconnect.cpp 1.7 2007/06/11 22:37:09Z dslausb Exp jheiusb $
$Log: mssDisconnect.cpp $
Revision 1.7  2007/06/11 22:37:09Z  dslausb
IT 8084 - PFR button returning to Two-Button screen
Revision 1.6  2007/03/08 21:10:30Z  jheiusb
7907 -- disable leak detector after MSS addition
Revision 1.5  2006/05/03 17:24:28Z  jheiusb
7515 -- PFR for MSS Disconnect
Revision 1.4  2006/03/27 17:52:05Z  jheiusb
7350 -- PFR for MSS
Revision 1.3  2006/03/15 17:02:06Z  jheiusb
add recalc of PLT ss
Revision 1.2  2006/03/03 17:31:55Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:40Z  jheiusb
Initial revision
Revision 1.4  2005/12/13 18:12:44Z  jheiusb
7265 -- make metered storage a full state
Revision 1.3  2005/10/07 22:06:30Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:16Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "mssDisconnect.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"


DEFINE_STATE(MSSDisconnect);

MSSDisconnect::MSSDisconnect()
   : StateAbs()
{}

MSSDisconnect::MSSDisconnect(const MSSDisconnect& state)
   : StateAbs(state)
{}

MSSDisconnect::~MSSDisconnect()
{}

int MSSDisconnect::preProcess ()
{
   return NORMAL;
}

int MSSDisconnect::postProcess ()
{

   return NORMAL;
}

int MSSDisconnect::transitionStatus ()
{

   // It will transition out of this state when GUI sends the
   // MSSAckCompletionMsg. (see the state table)

   // no transition here ... msg in state table transitions
   return NO_TRANSITION;

}

int MSSDisconnect::preEnter ()
{

   ProcData pd;

   pd.MakeRunWritable();
   // setup this for PFR
   pd.Run().currentMSSSubState.Set(METERED_STORAGE_DISCONNECT);
   pd.MakeRunReadable();

   // lets see how long they took to do metering and see if we need to
   // flag for WBC contamination due to waiting too long
   pd.MakeRunWritable();
   pd.Run().MeteringDisconnectTime.Set(pd.GetAbsTimeNowinMinutes() );
   //
   // Set the system state to AC Prime
   //
   pd.SystemState(METERED_STORAGE_DISCONNECT);


   // change the set status to STORAGE COMPLETE
   // this will disable the leak detector for postRun unloading
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::STORAGE_COMPLETE);
   pd.MakeTrimaSetReadable();

   return NORMAL;
}

int MSSDisconnect::postExit ()
{
   ProcData pd;

   pd.Orders().UnlockDoor();
   return NORMAL;
}

void MSSDisconnect::copyOver (const MSSDisconnect&)
{}

void MSSDisconnect::cleanup ()
{}

/* FORMAT HASH 9baea46cf17f6af34d70aa82eac43b8d */
