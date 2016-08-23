/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPrime2.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTPrime2.cpp 1.10 2008/04/08 21:42:21Z jheiusb Exp jheiusb $
$Log: mssPLTPrime2.cpp $
Revision 1.10  2008/04/08 21:42:21Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.9  2006/12/04 16:16:38Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.8  2006/09/28 20:20:43Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.7  2006/07/27 15:29:10Z  jheiusb
7672  -- change rinseback entery flag for MSS
Revision 1.6  2006/07/21 16:21:41Z  jheiusb
7601 -- salvage case added
Revision 1.5  2006/04/19 22:08:29Z  jheiusb
7491 -- PFR hangs
Revision 1.4  2006/04/13 17:22:51Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.3  2006/03/27 17:52:37Z  jheiusb
7350 -- PFR for MSS
Revision 1.2  2006/03/03 17:32:31Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:10Z  jheiusb
Initial revision
Revision 1.5  2006/03/02 18:16:13Z  jheiusb
add many MSS values to CobeConfig
Revision 1.4  2005/11/16 19:47:36Z  jheiusb
7230 -- Alarm Processing to split RBC and PLT runs after one fatal errors
Revision 1.3  2005/10/30 21:12:05Z  jheiusb
7230 -- RBC prime and addition for MSS (NO PTF)
Revision 1.2  2005/10/25 20:58:17Z  jheiusb
7202 -- initial PLT mss path
Revision 1.1  2005/10/24 18:32:13Z  jheiusb
Initial revision
Revision 1.1  2005/10/24 17:32:08Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:37Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:23Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "mssPLTPrime2.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(MSSPLTPrime2);

const float PlasmaPumpCmdSpeed =  CobeConfig::data().MssPLTPrimeFinalSpeed;
const float PlasmaVolmGoal     =  CobeConfig::data().MssPLTPrimeFinalVol;


MSSPLTPrime2::MSSPLTPrime2()
   : StateAbs(),
     transitionNow(false),
     initPlasmaPumpVolume(0.0),
     holdPumpsDueToPressure(false)
{}

MSSPLTPrime2::MSSPLTPrime2(const MSSPLTPrime2& state)
   : StateAbs(),
     transitionNow(false),
     initPlasmaPumpVolume(0.0),
     holdPumpsDueToPressure(false)
{}

MSSPLTPrime2::~MSSPLTPrime2()
{}

int MSSPLTPrime2::preProcess ()
{
   ProcData pd;


   if ( pd.Status().APS() >= CobeConfig::data().MssApsHigh  )
   {
      holdPumpsDueToPressure = true;
      return NORMAL;
   }


   if ( holdPumpsDueToPressure )
   {

      if (pd.Status().APS() <=  CobeConfig::data().MssApsBackpressure)
      {
         holdPumpsDueToPressure = false;
      }
      return NORMAL;
   }

   pd.Orders().Qplasma (PlasmaPumpCmdSpeed);

   return NORMAL;
}

int MSSPLTPrime2::postProcess ()
{

   ProcData    pd;

   const float plasmaVolPumped = pd.Volumes().Vp - initPlasmaPumpVolume;

   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(plasmaVolPumped);
   pd.MakeRunReadable();

   if (plasmaVolPumped >= PlasmaVolmGoal)
   {

      // update the SS bag volm remaining
      // pd.MakeVolumesWritable();
      //  pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVolPumped;
      // pd.MakeVolumesReadable();

      // set this when you are complete with priming
      transitionNow = true;
   }


   return NORMAL;

}

int MSSPLTPrime2::transitionStatus ()
{

   ProcData pd;


   // if we're not doing MSS for PLT then get the heck out of here
   if (!(pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK))
   {
      return 1;
   }



   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)  // if we're in PFR
   {
      if ( pd.Run().currentMSSSubState >  SS_MSSPLTPRIME2  )
         return 1;
   }

   if (transitionNow)
   {

      pd.Orders().Qplasma (0.0f);

      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag =>"  << pd.Volumes().VpltStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "Plasma   Volm Pumped =>"  << pd.Volumes().Vp - initPlasmaPumpVolume << endmsg;

      return 1;
   }


   // if we did'nt do rinseback skip this is salvage case
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return 1;
   }

   return NO_TRANSITION;

}

int MSSPLTPrime2::preEnter ()
{

   ProcData pd;

   // get the initial setting of thes volumes
   initPlasmaPumpVolume = pd.Volumes().Vp;

   if (!pd.Run().mssInPFR)   // if we're in PFR
   {

      pd.MakeRunWritable();
      pd.Run().currentMSSSubState = SS_MSSPLTPRIME2;
      pd.MakeRunReadable();

   }
   else
   {

      initPlasmaPumpVolume -= pd.Run().hadPrimedVolm.Get();
      if (pd.Run().currentMSSSubState == SS_MSSPLTPRIME2)
      {

         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR; with PLS previously pumped= " << pd.Run().hadPrimedVolm.Get()   << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();

      }

   }



   return NORMAL;
}

int MSSPLTPrime2::postExit ()
{

   ProcData pd;
   pd.Orders().Qplasma (0.0f);

   // clear at end of state because this is common  to all primes
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(0.0f);
   pd.MakeRunReadable();

   return NORMAL;
}

void MSSPLTPrime2::copyOver (const MSSPLTPrime2&)
{}

void MSSPLTPrime2::cleanup ()
{}

/* FORMAT HASH 8bdc9b0e02a6a5b66d258d291eeb2783 */
