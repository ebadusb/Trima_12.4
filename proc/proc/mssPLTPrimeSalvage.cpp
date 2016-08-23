/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTPrimeSalvage.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTPrimeSalvage.cpp 1.8 2008/11/06 17:58:42Z jheiusb Exp jheiusb $
$Log: mssPLTPrimeSalvage.cpp $
Revision 1.8  2008/11/06 17:58:42Z  jheiusb
8928 -- make multiple pfr's more robust.
Revision 1.7  2008/04/08 21:42:28Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.6  2008/03/26 18:04:14Z  jheiusb
8521 -- fix extra logging on transition
Revision 1.5  2006/12/04 16:16:42Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.4  2006/09/28 20:20:48Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.3  2006/07/26 22:37:54Z  jheiusb
7672 -- add MSS rinseback started flag
Revision 1.2  2006/07/21 16:21:48Z  jheiusb
7601 -- salvage case added

*/

#include "mssPLTPrimeSalvage.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(MSSPLTPrimeSalvage);


const float PlateletPumpCmdSpeed =  CobeConfig::data().MssPLTPrimeSpeed;
const float CollectPumpCmdSpeed  =  CobeConfig::data().MssPLTPrimeSpeed;
const float InletPumpCmdSpeed    =  CobeConfig::data().MssPLTPrimeSpeed * CobeConfig::data().MssPrimeSpeedPercent;
const float ReturnPumpCmdSpeed   =  CobeConfig::data().MssPLTPrimeSpeed * CobeConfig::data().MssPrimeSpeedPercent;
const float MaxPrimeVol          =  CobeConfig::data().MssPltSubstateMaxPrimeVol;
const float MinPrimeVol          =  CobeConfig::data().MssPltSubstateMinPrimeVol;
const float ReturnPrime1Vol      =  CobeConfig::data().MssPrimeReturnVol;


MSSPLTPrimeSalvage::MSSPLTPrimeSalvage()
   : StateAbs(),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initPlasmaPumpVolume(0.0),
     initCollectPumpVolume(0.0),
     initInletPumpVolume(0.0),
     holdPumpsDueToPressure(false)
{}

MSSPLTPrimeSalvage::MSSPLTPrimeSalvage(const MSSPLTPrimeSalvage& state)
   : StateAbs(state),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initPlasmaPumpVolume(0.0),
     initCollectPumpVolume(0.0),
     initInletPumpVolume(0.0),
     holdPumpsDueToPressure(false)
{}

MSSPLTPrimeSalvage::~MSSPLTPrimeSalvage()
{}

int MSSPLTPrimeSalvage::preProcess ()
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


   pd.Orders().Qplasma (CollectPumpCmdSpeed);
   pd.Orders().Qcollect(PlateletPumpCmdSpeed);

   return NORMAL;
}




int MSSPLTPrimeSalvage::postProcess ()
{
   ProcData pd;

   float    plasmaVolPumped = pd.Volumes().Vp - initPlasmaPumpVolume;
   float    returnVolPumped = pd.Volumes().Vreturn - initReturnPumpVolume;

   // for pfr
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(plasmaVolPumped);
   pd.Run().hadPrimedReturnVolm.Set(returnVolPumped);
   pd.MakeRunReadable();

   // check if we're done
   if (plasmaVolPumped >= CobeConfig::data().MssPltSubstateSalvageVol)
   {

      // update the SS bag volm remaining
      // pd.MakeVolumesWritable();
      //   pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVolPumped;
      // pd.MakeVolumesReadable();

      // set this when you are complete with priming
      transitionNow = true;

   }

   return NORMAL;

}

int MSSPLTPrimeSalvage::transitionStatus ()
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
      if ( pd.Run().currentMSSSubState >  SS_MSSPLTPRIMESALVAGE  )
         return 1;
   }

   if (transitionNow)
   {

      pd.Orders().Qplasma (0.0);
      pd.Orders().Qcollect(0.0);
      pd.Orders().Qin(0.0);
      pd.Orders().Qrp(0.0);

      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag =>"  << pd.Volumes().VpltStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "Return  Volm Pumped for prime =>"  << pd.Volumes().Vreturn - initReturnPumpVolume << endmsg;
      DataLog(log_level_proc_info) << "Plasma  Volm Pumped for prime =>"  << pd.Volumes().Vp - initPlasmaPumpVolume << endmsg;
      DataLog(log_level_proc_info) << "Inlet   Volm Pumped for prime =>"  << pd.Volumes().VinTotal - initInletPumpVolume << endmsg;
      DataLog(log_level_proc_info) << "Collect Volm Pumped for prime =>"  << pd.Volumes().Vc - initCollectPumpVolume << endmsg;

      return 1;
   }


   // if we DID do enough rinseback skip this salvage case
   if (pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because Not a salvage case MSS!"   << endmsg;
      return 1;
   }
   return NO_TRANSITION;


}

int MSSPLTPrimeSalvage::preEnter ()
{


   ProcData pd;



   if (!pd.Run().mssInPFR)  // if we're in PFR
   {

      pd.MakeRunWritable();
      pd.Run().currentMSSSubState = SS_MSSPLTPRIMESALVAGE;
      pd.MakeRunReadable();

   }
   else
   {
      if ( pd.Run().currentMSSSubState.Get() == SS_MSSPLTPRIMESALVAGE)
      {

         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR; with PLS pumped => " << pd.Run().hadPrimedVolm.Get() << "; Return was at =>" << pd.Run().hadPrimedReturnVolm.Get() << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }

   }


   // get the initial setting of thes volumes
   initPlasmaPumpVolume  = pd.Volumes().Vp;
   initReturnPumpVolume  = pd.Volumes().Vreturn;
   initInletPumpVolume   = pd.Volumes().VinTotal;
   initCollectPumpVolume = pd.Volumes().Vc;

   initPlasmaPumpVolume -= pd.Run().hadPrimedVolm.Get();
   initReturnPumpVolume -= pd.Run().hadPrimedReturnVolm.Get();

   return NORMAL;
}

int MSSPLTPrimeSalvage::postExit ()
{

   ProcData pd;
   pd.Orders().Qplasma (0.0);
   pd.Orders().Qcollect(0.0);
   pd.Orders().Qin(0.0);
   pd.Orders().Qrp(0.0);

   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(0.0f);
   pd.Run().hadPrimedReturnVolm.Set(0.0f);
   pd.MakeRunReadable();

   transitionNow = false;

   return NORMAL;
}

void MSSPLTPrimeSalvage::copyOver (const MSSPLTPrimeSalvage&)
{}

void MSSPLTPrimeSalvage::cleanup ()
{}

/* FORMAT HASH 4c00d2d2788cb72f296797bf8e3d3b52 */
