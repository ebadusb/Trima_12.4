/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssAddition.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssAddition.cpp 1.11 2007/10/29 21:35:12Z jheiusb Exp jheiusb $
$Log: mssAddition.cpp $
Revision 1.11  2007/10/29 21:35:12Z  jheiusb
8237 -- add more logging
Revision 1.10  2007/04/17 15:46:45Z  jheiusb
7579 -- make parallel prime PFR
Revision 1.9  2007/04/02 20:21:09Z  jheiusb
7579 -- patch up PFR for 5.R
Revision 1.8  2007/01/22 22:42:26Z  jheiusb
7843 -- add metering time column to dlogs
Revision 1.7  2006/12/04 16:16:24Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.6  2006/08/21 17:06:55Z  jheiusb
7519 -- remove APS pressure alarm as spec'd
Revision 1.5  2006/04/19 22:08:12Z  jheiusb
7491 -- PFR hangs
Revision 1.4  2006/04/13 17:22:34Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.3  2006/03/27 17:51:55Z  jheiusb
7350 -- PFR for MSS
Revision 1.2  2006/03/03 17:31:19Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:19Z  jheiusb
Initial revision
Revision 1.7  2005/12/02 17:11:45Z  jheiusb
7267 -- stop pumps on exiting the state (postExit)
Revision 1.6  2005/11/19 23:39:34Z  jheiusb
7230 -- fix the stuttering pumps during MSS PTF addition
Revision 1.5  2005/11/13 19:35:33Z  jheiusb
7230 -- more state file cleanup
Revision 1.4  2005/10/25 20:57:34Z  jheiusb
7202 -- initial PLT mss path
Revision 1.3  2005/10/07 22:06:17Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:36:19Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/


#include "mssAddition.h"
#include "states.h"
#include "procdata.h"

#include "cobeconfig.h"

DEFINE_STATE(MSSAddition);



MSSAddition::MSSAddition()
   : StateAbs()
{}

MSSAddition::MSSAddition(const MSSAddition& state)
   : StateAbs(state)
{}

MSSAddition::~MSSAddition()
{}

int MSSAddition::preProcess ()
{

   return NORMAL;
}

int MSSAddition::postProcess ()
{

   return NORMAL;
}


int MSSAddition::preEnter ()
{

   ProcData pd;

   pd.MakeRunWritable();
   // setup this for PFR
   pd.Run().currentRBCMSSSubState = SS_MSSADDITION;
   pd.MakeRunReadable();

   return NORMAL;
}

int MSSAddition::postExit ()
{

   ProcData pd;

   // make sure all pumps that do MSS prime are off!
   pd.Orders().Qac (0.0f);
   pd.Orders().Qplasma (0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0);

   return NORMAL;
}

void MSSAddition::copyOver (const MSSAddition&)
{}

void MSSAddition::cleanup ()
{}

/* FORMAT HASH 5b9a469457a6f80a2b42b3cd4c379472 */
