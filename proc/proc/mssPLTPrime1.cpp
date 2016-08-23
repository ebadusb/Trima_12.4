/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTPrime1.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTPrime1.cpp 1.19 2008/10/01 21:34:27Z jheiusb Exp jheiusb $
$Log: mssPLTPrime1.cpp $
Revision 1.19  2008/10/01 21:34:27Z  jheiusb
8007 -- redo flagging for clampclosure and mssPltPrime errors.
Revision 1.18  2008/04/08 21:42:06Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.17  2007/02/26 22:30:50Z  jheiusb
7864 add plt flagging in mss for short volume
Revision 1.16  2006/12/04 16:16:35Z  jheiusb
remerge from original rbc+plt mss 5.2
Revision 1.15  2006/09/28 20:20:39Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.14  2006/09/11 18:44:52Z  jheiusb
7737 -- stop return pump at 2ml if after alarm
Revision 1.13  2006/07/26 22:37:51Z  jheiusb
7672 -- add MSS rinseback started flag
Revision 1.12  2006/07/21 16:21:37Z  jheiusb
7601 -- salvage case added
Revision 1.11  2006/06/28 20:14:50Z  dslausb
Removed a bunch of obsolete strings and a couple unused alarms that were calling them so that we aren't wasting resources translating them.
Revision 1.10  2006/05/17 17:39:13Z  jheiusb
7534 -- do not zero pls and collect pumps volms after prime too soon
Revision 1.9  2006/05/09 20:59:13Z  jheiusb
7523 -- fix prime too soons recovery
Revision 1.8  2006/04/19 22:08:24Z  jheiusb
7491 -- PFR hangs
Revision 1.7  2006/04/18 21:29:34Z  jheiusb
Revision 1.6  2006/04/13 17:22:45Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.5  2006/04/07 16:54:55Z  jheiusb
7350 -- fix primeing return pump
Revision 1.4  2006/03/27 17:52:28Z  jheiusb
7350 -- PFR for MSS
Revision 1.3  2006/03/22 23:25:20Z  dslausb
Changed misleading alarm name
Revision 1.2  2006/03/03 17:32:25Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:03Z  jheiusb
Initial revision
Revision 1.15  2006/03/02 18:16:07Z  jheiusb
add many MSS values to CobeConfig
Revision 1.14  2006/02/23 17:14:56Z  jheiusb
7256 -- return and Inlet speeds slowed to better match start stops
Revision 1.13  2006/01/24 22:13:38Z  jheiusb
7329 --  back off prime to take into account the 10 ml pre-prime
Revision 1.12  2006/01/12 17:19:30Z  jheiusb
7343 -- change ret/inlet pump speed to new spec'ed value
Revision 1.11  2005/12/16 23:28:26Z  jheiusb
7309 -- fix so no prime delay alarms
Revision 1.10  2005/12/01 21:45:48Z  jheiusb
7233 -- take out prime too soon fatal alarm
Revision 1.9  2005/11/30 19:13:26Z  jheiusb
7266 -- inlet alarm fixed, along with inlet pump volm accrual
Revision 1.8  2005/11/29 22:58:22Z  jheiusb
7233 -- fix the prime too soon to be one continue then fail
Revision 1.7  2005/11/16 19:47:32Z  jheiusb
7230 -- Alarm Processing to split RBC and PLT runs after one fatal errors
Revision 1.6  2005/11/04 22:30:06Z  jheiusb
7230 -- cleanup cobe config values
Revision 1.5  2005/11/03 23:00:40Z  jheiusb
7230 -- fix prime too soon alaert
Revision 1.4  2005/10/30 21:11:48Z  jheiusb
7230 -- RBC prime and addition for MSS (NO PTF)
Revision 1.3  2005/10/27 16:40:42Z  jheiusb
7202 -- add prime too late alarms/alerts
Revision 1.2  2005/10/25 20:58:10Z  jheiusb
7202 -- initial PLT mss path
Revision 1.1  2005/10/24 18:32:04Z  jheiusb
Initial revision
Revision 1.1  2005/10/24 17:32:08Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:37Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:23Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "mssPLTPrime1.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(MSSPLTPrime1);


const float PlateletPumpCmdSpeed =  CobeConfig::data().MssPLTPrimeSpeed;
const float CollectPumpCmdSpeed  =  CobeConfig::data().MssPLTPrimeSpeed;
const float InletPumpCmdSpeed    =  CobeConfig::data().MssPLTPrimeSpeed * CobeConfig::data().MssPrimeSpeedPercent;
const float ReturnPumpCmdSpeed   =  CobeConfig::data().MssPLTPrimeSpeed * CobeConfig::data().MssPrimeSpeedPercent;
const float ReturnPrime1Vol      =  CobeConfig::data().MssPrimeReturnVol;


MSSPLTPrime1::MSSPLTPrime1()
   : StateAbs(),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initPlasmaPumpVolume(0.0),
     initCollectPumpVolume(0.0),
     initInletPumpVolume(0.0),
     fluidTooLateAlert(MSS_PRIME_DELAY_PLT_ALERT),
     fluidTooSoonAlert(MSS_PRIME_TOO_SOON_PLT_ALERT),
     alertCounter(0),
     alertCounterPrime2soon(0),
     primeTooSoonFlag(false),
     primeTooLateFlag(false),
     holdPumpsDueToPressure(false),
     cleanRes(false)
{}

MSSPLTPrime1::MSSPLTPrime1(const MSSPLTPrime1& state)
   : StateAbs(state),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initPlasmaPumpVolume(0.0),
     initCollectPumpVolume(0.0),
     initInletPumpVolume(0.0),
     fluidTooLateAlert(MSS_PRIME_DELAY_PLT_ALERT),
     fluidTooSoonAlert(MSS_PRIME_TOO_SOON_PLT_ALERT),
     alertCounter(0),
     alertCounterPrime2soon(0),
     primeTooSoonFlag(false),
     primeTooLateFlag(false),
     holdPumpsDueToPressure(false),
     cleanRes(false)
{}

MSSPLTPrime1::~MSSPLTPrime1()
{}

int MSSPLTPrime1::preProcess ()
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

   if (!cleanRes)
   {
      pd.Orders().Qplasma (CollectPumpCmdSpeed);
      pd.Orders().Qcollect(PlateletPumpCmdSpeed);

      if ( pd.Status().ReservoirIsEmpty() )
      {
         pd.Orders().Qin(0.0f);
         pd.Orders().Qrp(0.0f);
      }
      else
      {
         if ( pd.Status().APS() < CobeConfig::data().MssApsBackpressure )
         {
            pd.Orders().Qrp(ReturnPumpCmdSpeed);
            pd.Orders().Qin(0.0f);
         }
         else
         {
            pd.Orders().Qrp(ReturnPumpCmdSpeed);
            pd.Orders().Qin(InletPumpCmdSpeed);
         }
      }

   }
   //
   // run inlet to clean res if prime too soon... constrained by InletVolm alarm
   //
   else
   {

      if ( !pd.Status().ReservoirIsEmpty() )
      {
         if ( pd.Status().APS() < CobeConfig::data().MssApsBackpressure )
         {
            pd.Orders().Qrp(20.0f);
            pd.Orders().Qin(0.0f);
         }
         else
         {
            pd.Orders().Qrp(20.0f);
            pd.Orders().Qin(20.0f);
         }

      }
      else
      {
         pd.Orders().Qin(0.0f);
         pd.Orders().Qrp(0.0f);
         cleanRes = false;

         // reset to try again
         initReturnPumpVolume = pd.Volumes().Vreturn;
         initInletPumpVolume  = pd.Volumes().VinTotal;
         // dont reset the plasma or collect pump... we keep adding there portion in this case
      }


   }
   return NORMAL;
}




int MSSPLTPrime1::postProcess ()
{
   ProcData pd;

   float    plasmaVolPumped = pd.Volumes().Vp - initPlasmaPumpVolume;
   float    returnVolPumped = pd.Volumes().Vreturn - initReturnPumpVolume;

   // for pfr
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(plasmaVolPumped);
   pd.Run().hadPrimedReturnVolm.Set(returnVolPumped);
   pd.MakeRunReadable();

   if (!cleanRes)
   {

      if (returnVolPumped < ReturnPrime1Vol)
      {
         if (plasmaVolPumped >= CobeConfig::data().MssPltSubstateMaxPrimeVol)
         {
            if (fluidTooLateAlert.getState() == CLEARED)
            {
               alertCounter++;       // bump up the alert counts

               DataLog(log_level_proc_info) << "fluid Too Late Alert!" << endmsg;

               // Why did we fail??
               DataLog(log_level_proc_info) << "CobeConfig::MssPltSubstateMaxPrimeVol  =>"  << CobeConfig::data().MssPltSubstateMaxPrimeVol << endmsg;
               DataLog(log_level_proc_info) << "Plasma Volm Pumped =>" << plasmaVolPumped << endmsg;
               DataLog(log_level_proc_info) << "Return Volm Pumped =>" << returnVolPumped << endmsg;

               // flag as possible volume error IT 7853
               pd.MakeRunWritable();
               pd.Run().flagMSSPASVolumeError.Set(true);
               pd.MakeRunReadable();


               // reset to try again
               initCollectPumpVolume = pd.Volumes().Vc;
               initPlasmaPumpVolume  = pd.Volumes().Vp;

               fluidTooLateAlert.setAlarm();

            }


         }

      }
      else  // if (returnVolPumped >= ReturnPrime1Vol)
      {
         if ( (plasmaVolPumped <= CobeConfig::data().MssPltSubstateMinPrimeVol)
              && (alertCounter < 1) )   // ignore this if we did a prime delay already
         {

            // Alarm...  don't do this alarm if you did the too late alarm
            // if the too late alert occurs and you continue you may over prime so we disable this one
            if ( ( fluidTooSoonAlert.getState() == CLEARED)  )
            {

               DataLog(log_level_proc_info) << "fluid Too Soon Alert!" << endmsg;

               // Why did we fail??
               DataLog(log_level_proc_info) << "CobeConfig::MssPltSubstateMinPrimeVol  =>"  << CobeConfig::data().MssPltSubstateMinPrimeVol << endmsg;
               DataLog(log_level_proc_info) << "Return  Volm Pumped =>"  << returnVolPumped << endmsg;
               DataLog(log_level_proc_info) << "Plasma  Volm Pumped =>"  << plasmaVolPumped << endmsg;

               // flag as possible volume error (IT 13621)
               pd.MakeRunWritable();
               pd.Run().flagMSSPASVolumeError.Set(true);
               pd.MakeRunReadable();

               alertCounterPrime2soon++;

               fluidTooSoonAlert.setAlarm();


               pd.MakePQIWritable();
               pd.PQI().ClampClosureFailedPartFailure.Set(true);
               pd.MakePQIReadable();

               DataLog(log_level_proc_mss_info) << "ClampClosureFailedPartFailure Set"  << endmsg;

               cleanRes = true;

            }


         }
         //
         // This is the path you should be going down...
         //
         else // (returnVolPumped >= ReturnPrime1Vol)  &&  (plasmaVolPumped > CobeConfig::data().MssPltSubstateMin1PrimeVol)
         {
            // update the SS bag volm remaining
            // pd.MakeVolumesWritable();
            // pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVolPumped;
            // pd.MakeVolumesReadable();

            // set this when you are complete with priming
            transitionNow = true;

         }

      }
   }
   return NORMAL;


}

int MSSPLTPrime1::transitionStatus ()
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
      if ( pd.Run().currentMSSSubState >  SS_MSSPLTPRIME1  )
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

   // if we did'nt do rinseback skip this is salvage case
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return 1;
   }


   return NO_TRANSITION;


}

int MSSPLTPrime1::preEnter ()
{


   ProcData pd;



   if (!pd.Run().mssInPFR)  // if we're in PFR
   {

      pd.MakeRunWritable();
      pd.Run().currentMSSSubState = SS_MSSPLTPRIME1;
      pd.MakeRunReadable();

   }
   else
   {

      if (pd.Run().currentMSSSubState == SS_MSSPLTPRIME1)
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

int MSSPLTPrime1::postExit ()
{

   ProcData pd;
   pd.Orders().Qplasma (0.0);
   pd.Orders().Qcollect(0.0);
   pd.Orders().Qin(0.0);
   pd.Orders().Qrp(0.0);


   transitionNow = false;

   // clear at end of state because this is common  to all primes
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(0.0f);
   pd.Run().hadPrimedReturnVolm.Set(0.0f);
   pd.MakeRunReadable();

   return NORMAL;
}

void MSSPLTPrime1::copyOver (const MSSPLTPrime1&)
{}

void MSSPLTPrime1::cleanup ()
{}

/* FORMAT HASH 1f82f8b80da22756164f20efc493e2c1 */
