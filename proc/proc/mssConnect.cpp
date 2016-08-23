/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssConnect.cpp
 *
 */


/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssConnect.cpp 1.38 2008/11/06 17:58:15Z jheiusb Exp jheiusb $
$Log: mssConnect.cpp $
Revision 1.38  2008/11/06 17:58:15Z  jheiusb
8928 -- make multiple pfr's more robust.
Revision 1.37  2008/09/08 16:30:00Z  dslausb
IT 8306 - Split should always be based on configured RBC dose with no scaling or adjustments.
Revision 1.36  2008/08/11 19:05:53Z  dslausb
Increase PFR robustness of AAS procedure type
Revision 1.35  2007/12/14 23:19:39Z  dslausb
IT 8403 - Calculation for number of bags was wrong.
Revision 1.34  2007/12/07 22:45:56Z  jheiusb
8241
Revision 1.33  2007/11/27 18:34:29Z  jheiusb
8257 -- fixagain

Revision 1.32  2007/11/27 18:31:36Z  jheiusb
8257 -- connect RBC bags in relation to MSS volume needed
Revision 1.31  2007/10/31 16:19:49Z  dslausb
IT 8245 - Change RBC config screen so that it shows the PTF RSF offset, just like the procedure selection screen.
Revision 1.30  2007/10/26 17:08:45Z  dslausb
Changed calculation for total RAS needed on GUI side.
Revision 1.29  2007/08/15 16:52:54Z  jheiusb
7930 -- disable RAS if rinseback failes to complete
Revision 1.28  2007/05/02 20:26:18Z  jheiusb
7579 -- preserve final concentration during SS
Revision 1.27  2007/01/22 22:42:32Z  jheiusb
7843 -- add metering time column to dlogs
Revision 1.26  2006/12/12 23:04:35Z  jheiusb
7458 -- update GUI meter for PTF RBC metering
Revision 1.25  2006/12/11 23:14:59Z  jheiusb
7458 -- merge in msg for Progress bar
Revision 1.24  2006/12/04 16:16:28Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.23  2006/09/28 20:20:34Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.22  2006/08/22 22:29:26Z  jheiusb
7673 -- ceiling the estimated SS
Revision 1.21  2006/08/14 17:15:27Z  jheiusb
7647 -- fix logging for ss estimate
Revision 1.20  2006/08/09 19:51:09Z  jheiusb
7647 -- fix est solution equation
Revision 1.19  2006/07/27 15:40:13Z  jheiusb
7672 -- use the new MSS rinseback entered flag
Revision 1.18  2006/07/26 22:33:53Z  jheiusb
7647 -- fix bag number calc
Revision 1.17  2006/07/21 16:34:04Z  jheiusb
7659 -- remove PLT ss recalc alarm
Revision 1.16  2006/06/09 16:55:33Z  dslausb
Removed fixed volume stuff and remnants of cassette options from config
Revision 1.15  2006/06/05 14:20:10Z  rm70006
IT 7510.  Remove CassetteOptions struct from config and move MSS fields to Procedure.
Revision 1.14  2006/05/08 21:56:04Z  jheiusb
7516 -- recalc the % carryover correctly
Revision 1.13  2006/04/27 20:31:47Z  jheiusb
7468 -- change % carryover to product based
Revision 1.12  2006/04/19 22:08:16Z  jheiusb
7491 -- PFR hangs
Revision 1.11  2006/04/19 20:04:45Z  jheiusb
7491 -- PFR hangs
Revision 1.10  2006/03/28 17:16:37Z  dslausb
Re-fixed the mss target vol message, which obviously didn't get fixed the first time I fixed it.
Revision 1.9  2006/03/28 16:49:48Z  dslausb
Fixed message that sends PLT SS Target Vol from proc to gui
Revision 1.8  2006/03/27 17:52:00Z  jheiusb
7350 -- PFR for MSS
Revision 1.7  2006/03/20 23:23:16Z  dslausb
Revision 1.6  2006/03/20 21:25:20Z  jheiusb
recalc PLT SS
Revision 1.5  2006/03/17 20:26:12Z  dslausb
Put in a message to let GUI know that we've changed the PLT target SS vol.
Revision 1.4  2006/03/15 17:01:57Z  jheiusb
add recalc of PLT ss
Revision 1.3  2006/03/07 22:14:43Z  dslausb
Revision 1.2  2006/03/03 17:31:48Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:33Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:24Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:09Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "math.h"
#include "mssConnect.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"

#include "MssCommon.h"
#include "CatalogChecker.h"



DEFINE_STATE(MSSConnect);

MSSConnect::MSSConnect()
   : StateAbs()
{}

MSSConnect::MSSConnect(const MSSConnect& state)
   : StateAbs(state)
{}

MSSConnect::~MSSConnect()
{}

int MSSConnect::preProcess ()
{


   return NORMAL;
}

int MSSConnect::postProcess ()
{


   return NORMAL;
}

int MSSConnect::transitionStatus ()
{

   // MSSConnect pauses  proc while the operator does stuff.  It will transition
   // out of this state when GUI sends the MSSConnectedMsg. (see the state table)

   // no transition ... message recieved in state table;
   return NO_TRANSITION;


}

int MSSConnect::preEnter ()
{
   ProcData pd;

   pd.MSSRunTime().inactivate();  // dont time this, operator action


   if (!pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_MSSCONNECT;
      pd.MakeRunReadable();
   }
   else
   {

      if ( (pd.Run().currentMSSSubState == SS_MSSCONNECT)  )
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }
   }


   // if we're  doing MSS for PLT then do this stuff
   if (pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK)
   {

      ////////////////////////  Recalc the SS if the PLT volume is different  //////////////

      float ppco = pd.RunTargets().PltPctCarryover.Get();

      DataLog(log_level_proc_info) << "Percent carryover is ->" << ppco << "%" << endmsg;
      DataLog(log_level_proc_info) << "target volm     ->" << pd.RunTargets().PlateletVolumeTarget << endmsg;
      DataLog(log_level_proc_info) << "collect volm    ->" << pd.Volumes().PlateletVolume << endmsg;
      DataLog(log_level_proc_info) << "target Concent. ->" << pd.RunTargets().PlateletConcenTarget << endmsg;
      DataLog(log_level_proc_info) << "Target Yield    ->" << pd.RunTargets().PlateletYieldTarget << endmsg;

      // check actual collection vs target and adjust PAS if needed.
      float newvolmPas = getAdjustedPAS();
      DataLog(log_level_proc_info) << "Adjusted PAS Volume ->" << newvolmPas << endmsg;

      pd.MakeVolumesWritable();
      pd.Volumes().VpltStorageRemaining.Set(pd.Config().Procedure.Get().key_plt_def_bag_vol);
      pd.MakeVolumesReadable();
   }

   // if we're  doing MSS for RBC then do this stuff
   if (pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK)
   {
      bool setIsDrbc = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::DRBC_BAG);

      // IT 8306 - Always determine whether this is prompted split based on configured
      // rbc dose with no scaling or adjustments
      bool targetedAsSplit = setIsDrbc &&
                             ((pd.RunTargets().RbcDoseTarget.Get() > 299.0) ||
                              pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get()));

      float estSolutionUsed = ( pd.RunTargets().RBCSolutionTarget
                                + CobeConfig::data().MssRbcSubstateMaxPrimeVol
                                + CobeConfig::data().MssPtfVolume);

      DataLog(log_level_proc_info) << "RBC target volm ->" << pd.RunTargets().RbcVolumeTarget << endmsg;
      DataLog(log_level_proc_info) << "RBC collect volm ->" << pd.Volumes().RBCVolume.Get() << " (prod1: "
                                   << pd.Volumes().RBCProduct_A_FinalVolume.Get() << "; prod2: " << pd.Volumes().RBCProduct_B_FinalVolume.Get() << ")" << endmsg;
      DataLog(log_level_proc_info) << "RBC Storage Solution target volm ->" << pd.RunTargets().RBCSolutionTarget.Get() << endmsg;

      // See if this procedure was targetted as a split.
      if (targetedAsSplit)
         estSolutionUsed += CobeConfig::data().MssPtfVolume - CobeConfig::data().mssPtfFilterLoss;

      // Set up SS volume available to RBC Metering
      bool isBlackStamp = (pd.TrimaSet().Cassette == cassette::RBCPLS);

      pd.MakeVolumesWritable();
      if (!isBlackStamp
          || ((estSolutionUsed * (1.0f + CobeConfig::data().MssRbcSSPercent / 100.0f)) < pd.Config().Procedure.Get().key_rbc_def_bag_vol) )
      {       // connect one bag only!
         pd.Volumes().VrbcStorageRemaining.Set(pd.Config().Procedure.Get().key_rbc_def_bag_vol);
      }
      else
      {
         // connect two bags!
         pd.Volumes().VrbcStorageRemaining.Set(2.0f * (pd.Config().Procedure.Get().key_rbc_def_bag_vol));
      }
      pd.MakeVolumesReadable();
   }

   MSSSetupCompleteMsg _setupDone(MessageBase::SEND_GLOBAL);
   _setupDone.send(0);

   DataLog(log_level_proc_info) << "MSSSetupCompleteMsg sent " << endmsg;

   return NORMAL;
}

int MSSConnect::postExit ()
{

   ProcData pd;
   pd.MSSRunTime().activate();

   pd.MakeRunWritable();
   pd.Run().SolutionsConnected.Set(true);

   return NORMAL;
}

void MSSConnect::copyOver (const MSSConnect&)
{}

void MSSConnect::cleanup ()
{}

/* FORMAT HASH 97ac8d62ab547d5e925982629975047b */
