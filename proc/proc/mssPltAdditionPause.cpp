/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPltAdditionPause.cpp
 *
 */


/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPltAdditionPause.cpp 1.12 2008/04/08 21:42:53Z jheiusb Exp jheiusb $
$Log: mssPltAdditionPause.cpp $
Revision 1.12  2008/04/08 21:42:53Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.11  2007/05/01 22:29:54Z  jheiusb
7579 -- complete PFR for the parallel states
Revision 1.10  2007/04/17 15:47:00Z  jheiusb
7579 -- make parallel prime PFR
Revision 1.9  2007/03/06 23:53:04Z  dslausb
- Changed MSS progress bar to a one-bar system.
- Changed the order of the connect screens
- Changed the way proc updates GUI of the MSS volumes
Revision 1.8  2007/01/22 22:42:37Z  jheiusb
7843 -- add metering time column to dlogs
Revision 1.7  2007/01/17 22:08:29Z  jheiusb
7835 -- add MSS PLT spillover code to this baseline
Revision 1.6  2006/12/04 16:16:45Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.5  2006/04/19 22:08:36Z  jheiusb
7491 -- PFR hangs
Revision 1.4  2006/04/04 16:04:52Z  jheiusb
7350 -- PFR
Revision 1.3  2006/03/27 17:52:47Z  jheiusb
7350 -- PFR for MSS
Revision 1.2  2006/03/03 17:32:53Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:55Z  jheiusb
Initial revision
Revision 1.5  2006/01/27 23:06:51Z  jheiusb
test
Revision 1.4  2005/11/16 19:47:44Z  jheiusb
7230 -- Alarm Processing to split RBC and PLT runs after one fatal errors
Revision 1.3  2005/11/11 23:55:40Z  jheiusb
7230 -- fix position of return
Revision 1.2  2005/11/04 21:03:08Z  jheiusb
7202 -- add pause at the end of PLT prime
Revision 1.1  2005/11/03 23:22:49Z  jheiusb
Initial revision
*/

#include "mssPltAdditionPause.h"
#include "states.h"
#include "procdata.h"

const int WAIT_COUNT = 12;    // six seconds of samples.

DEFINE_STATE(MSSPltAdditionPause);

MSSPltAdditionPause::MSSPltAdditionPause()
   : StateAbs(),
     _transitionNow(false),
     _waitCounter(0),
     _opSawSplitNotif(false)
{
   _splitNotifAcknowledgement.init(Callback<MSSPltAdditionPause>(this, &MSSPltAdditionPause::opSawSplitNotif), MessageBase::SNDRCV_LOCAL);
}



MSSPltAdditionPause::MSSPltAdditionPause(const MSSPltAdditionPause& state)
   : StateAbs(state),
     _transitionNow(false),
     _waitCounter(0),
     _opSawSplitNotif(false)
{
   _splitNotifAcknowledgement.init(Callback<MSSPltAdditionPause>(this, &MSSPltAdditionPause::opSawSplitNotif), MessageBase::SNDRCV_LOCAL);
}

MSSPltAdditionPause::~MSSPltAdditionPause()
{}

int MSSPltAdditionPause::preProcess ()
{
   _waitCounter++;

   // DataLog(log_level_proc_info) << " click " << _waitCounter  << endmsg;
   return NORMAL;
}

int MSSPltAdditionPause::postProcess ()
{
   ProcData pd;
   if  (!pd.Config().Procedure.Get().key_plt_mss_split_notif)
   {
      if (_waitCounter > WAIT_COUNT)
      {
         DataLog(log_level_proc_info) << "Wait complete -- transition " << _waitCounter  << endmsg;
         _transitionNow = true;
      }
   }
   return NORMAL;
}

int MSSPltAdditionPause::transitionStatus ()
{
   ProcData pd;

   // if we're not doing MSS for PLT then get the heck out of here
   if (!(pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK))
   {
      return 1;
   }

   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)   // if we're in PFR
   {
      if (    (pd.Run().currentMSSSubState ==  SS_MSSPARALLELPROC )
              && (pd.Run().currentPLTMSSSubState > SS_MSSPLTADDITIONPAUSE )  )
      {
         return 1;
      }
   }

   // MSSConnect pauses  proc while the operator does stuff.  It will transition
   // out of this state when GUI sends the MSSConnectedMsg. (see the state table)
   if  (pd.Config().Procedure.Get().key_plt_mss_split_notif)
   {    // no transition ... message recieved in state table;
      if (_opSawSplitNotif && _waitCounter > WAIT_COUNT)
      {

         DataLog(log_level_proc_info) << "Button push recieved && wait count satisfied cnt = " << _waitCounter << endmsg;
         return 1;    // transition
      }
      else
         return NO_TRANSITION;
   }

   if (_transitionNow)
   {
      DataLog(log_level_proc_info) << " transition! with no GUI"   << endmsg;
      return 1;
   }

   // no transition ... message recieved in state table;
   return NO_TRANSITION;

}

int MSSPltAdditionPause::preEnter ()
{

   ProcData pd;


   if  (pd.Config().Procedure.Get().key_plt_mss_split_notif)
   {
      pd.MSSRunTime().inactivate();  // dont time this, operator action
   }


   if (!pd.Run().mssInPFR)  // if we're in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentPLTMSSSubState = SS_MSSPLTADDITIONPAUSE;
      pd.MakeRunReadable();

   }
   else
   {

      if (pd.Run().currentPLTMSSSubState.Get() == SS_MSSPLTADDITIONPAUSE)
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }

   }

   return NORMAL;
}

int MSSPltAdditionPause::postExit ()
{
   _splitNotifAcknowledgement.deregisterMsg();

   _transitionNow = false ;

   ProcData pd;
   pd.MSSRunTime().activate();

   return NORMAL;
}

void MSSPltAdditionPause::copyOver (const MSSPltAdditionPause&)
{}

void MSSPltAdditionPause::cleanup ()
{}


void MSSPltAdditionPause::opSawSplitNotif ()
{

   DataLog(log_level_proc_info) << "Button push recieved" << endmsg;

   _opSawSplitNotif = true;


}

/* FORMAT HASH 299652c2d966bbd47e0ea8f33200eee5 */
