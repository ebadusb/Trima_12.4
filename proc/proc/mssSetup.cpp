/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssSetup.cpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssSetup.cpp 1.9 2007/01/24 21:13:07Z dslausb Exp dslausb $
$Log: mssSetup.cpp $
Revision 1.9  2007/01/24 21:13:07Z  dslausb
Fixed MSS PFR gui messages
Revision 1.8  2007/01/22 22:42:42Z  jheiusb
7843 -- add metering time column to dlogs
Revision 1.7  2006/09/11 19:29:03Z  dslausb
Revision 1.6  2006/06/05 20:57:50Z  jheiusb
7350 -- distinguish between the two MSS setup screens in PFR
Revision 1.5  2006/04/19 22:08:45Z  jheiusb
7491 -- PFR hangs
Revision 1.4  2006/04/19 20:04:51Z  jheiusb
7491 -- PFR hangs
Revision 1.3  2006/03/27 17:52:57Z  jheiusb
7350 -- PFR for MSS
Revision 1.2  2006/03/03 17:33:08Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:38Z  jheiusb
Initial revision
Revision 1.4  2005/10/07 22:06:45Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.3  2005/10/05 20:35:30Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/
#include "mssSetup.h"
#include "states.h"
#include "procdata.h"
#include "guiproc.h"


DEFINE_STATE(MSSSetup);

MSSSetup::MSSSetup()
   : StateAbs()
{}

MSSSetup::MSSSetup(const MSSSetup& state)
   : StateAbs(state)
{
   copyOver(state);
}

MSSSetup::~MSSSetup()
{
   cleanup();
}

int MSSSetup::preProcess ()
{
   return NORMAL;
}

int MSSSetup::postProcess ()
{
   return NORMAL;
}

int MSSSetup::transitionStatus ()
{


   ProcData pd;
   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)  // if we're in PFR
   {

      if ( pd.Run().currentMSSSubState > SS_MSSSETUP  )
         return 1;
   }

   // There's really not much to do here, like i said, its just a place for
   // a pause in proc while the operator does stuff.  It will transition
   // out of this state when GUI sends the MSSSetupMsg. (see the state table)

   // no transition here ... msg in state table transitions
   return NO_TRANSITION;

}

int MSSSetup::preEnter ()
{

   ProcData pd;
   pd.MSSRunTime().inactivate(); // dont time this, operator action

   if (!pd.Run().mssInPFR)
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_MSSSETUP;
      pd.MakeRunReadable();
   }
   else
   {

      // set theses values for channel check and Verify closure3 if in PFR
      // and we havent got to Channel check or Verify closure3 yet!
      if ( (pd.Run().currentMSSSubState == SS_MSSSETUP)  )
      {

         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();

         pd.MakeRunWritable();
         pd.Run().inletVolm.Set(pd.Volumes().VinTotal.Get());
         pd.Run().clampCloseRetVolm.Set(pd.Volumes().Vreturn.Get());
         pd.MakeRunReadable();

      }

   }
   return NORMAL;
}

int MSSSetup::postExit ()
{

   ProcData pd;
   pd.MSSRunTime().activate();


   return NORMAL;
}

void MSSSetup::copyOver (const MSSSetup&)
{}

void MSSSetup::cleanup ()
{}

/* FORMAT HASH 29645c419666fe29a29e7d04fde226d0 */
