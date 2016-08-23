/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSRBCPrime1.cpp
 *
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssRBCPrime1.cpp 1.17 2009/05/27 21:35:43Z jheiusb Exp dslausb $
$Log: mssRBCPrime1.cpp $
Revision 1.17  2009/05/27 21:35:43Z  jheiusb
8248 -- add RAS salvage case
Revision 1.16  2009/02/09 21:22:38Z  jheiusb
9002 -- CPS changes
Revision 1.15  2008/12/11 23:07:05Z  jheiusb
 -- fix the APS relief if it creeps below 500 while an alarm is blocking the inlet pmp
Revision 1.14  2008/11/19 19:41:48Z  jheiusb
--  remove erronious logging
Revision 1.13  2008/11/06 17:58:52Z  jheiusb
8928 -- make multiple pfr's more robust.
Revision 1.12  2008/04/08 21:42:59Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.11  2008/02/26 23:46:27Z  jheiusb
8424 -- fix APS Alarming in PLT mss
Revision 1.10  2007/12/03 22:50:07Z  jheiusb
8290 -- cps alarm flaggin wrong product.
Revision 1.9  2007/11/20 23:28:30Z  jheiusb
8317 -- fix number of alarms
Revision 1.8  2007/11/12 19:58:37Z  jheiusb
8113 -- make the prime to soon a continuable alarm
Revision 1.7  2007/10/22 21:29:45Z  jheiusb
8099 -- fix primes logging
Revision 1.6  2007/08/15 16:53:05Z  jheiusb
7930 -- disable RAS if rinseback failes to complete
Revision 1.5  2007/04/20 21:34:04Z  jheiusb
6174 - ptf and metering ramp up
Revision 1.4  2007/04/02 20:20:11Z  jheiusb
7256 -- do the pressure balanced inlet/return pump running
Revision 1.3  2006/12/11 20:19:23Z  jheiusb
7458 -- fix MSS RBC delay alarm
Revision 1.2  2006/12/06 20:17:20Z  jheiusb
add PFR to old RBC metering
Revision 1.1  2006/11/20 22:49:27Z  jheiusb
Initial revision
Revision 1.9  2006/03/02 18:16:23Z  jheiusb
add many MSS values to CobeConfig
Revision 1.8  2006/02/23 17:15:05Z  jheiusb
7256 -- return and Inlet speeds slowed to better match start stops
Revision 1.7  2006/01/27 16:34:36Z  jheiusb
7376 -- fix prime too soon
Revision 1.6  2006/01/06 20:47:11Z  jheiusb
7320 -- fixed APS pressure alarm to use inlet pump for relief
Revision 1.5  2005/12/21 21:38:49Z  jheiusb
7202 -- fine tune speeds for PLT metering
Revision 1.4  2005/11/30 19:13:31Z  jheiusb
7266 -- inlet alarm fixed, along with inlet pump volm accrual
Revision 1.3  2005/11/16 19:48:11Z  jheiusb
7230 -- Alarm Processing to split RBC and PLT runs after one fatal errors
Revision 1.2  2005/10/30 21:12:27Z  jheiusb
7230 -- RBC prime and addition for MSS (NO PTF)
*/

#include "mssRBCPrime1.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(MSSRBCPrime1);


const float acPumpCmdSpeed     = CobeConfig::data().MssRBCPrimeSpeed;
const float InletPumpCmdSpeed  = acPumpCmdSpeed * CobeConfig::data().MssPrimeSpeedPercent;
const float ReturnPumpCmdSpeed = acPumpCmdSpeed * CobeConfig::data().MssPrimeSpeedPercent;


const float ReturnPrime1Vol       =  CobeConfig::data().MssPrimeReturnVol;
const float pressureRecoverySpeed = CobeConfig::data().MssApsAlertRecoveryQin;


MSSRBCPrime1::MSSRBCPrime1()
   : StateAbs(),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initACPumpVolume(0.0),
     initInletPumpVolume(0.0),
     fluidTooLateAlert(MSS_PRIME_DELAY_RBC_ALERT),
     fluidTooSoonAlarm(MSS_PRIME_TOO_SOON_RBC_ALARM),
     alertCounter(0),
     apsPres2HighAlert(APS_TOO_HIGH_ALERT_MSS),
     apsPres2HighAlarm(APS_TOO_HIGH_ALARM_MSS),
     apsInstanceCount(0),
     apsHigh(false),
     primeTooSoonFlag(false),
     primeTooLateFlag(false),
     numberOfAPSAlerts(4)
{}

MSSRBCPrime1::MSSRBCPrime1(const MSSRBCPrime1& state)
   : StateAbs(state),
     transitionNow(false),
     initReturnPumpVolume(0.0),
     initACPumpVolume(0.0),
     initInletPumpVolume(0.0),
     fluidTooLateAlert(MSS_PRIME_DELAY_RBC_ALERT),
     fluidTooSoonAlarm(MSS_PRIME_TOO_SOON_RBC_ALARM),
     alertCounter(0),
     apsPres2HighAlert(APS_TOO_HIGH_ALERT_MSS),
     apsPres2HighAlarm(APS_TOO_HIGH_ALARM_MSS),
     apsInstanceCount(0),
     apsHigh(false),
     primeTooSoonFlag(false),
     primeTooLateFlag(false),
     numberOfAPSAlerts(4)
{}

MSSRBCPrime1::~MSSRBCPrime1()
{}

int MSSRBCPrime1::preProcess ()
{
   ProcData pd;

   float    CurAPS = pd.Status().APS();

   if (!apsHigh)
   {

      // return pump and inlet pump must run matched
      // the return pump will be commanded to off if res is empty (by safety)
      // so... we have to mock that behavior for inlet pump
      if ( pd.Status().ReservoirIsEmpty() )
      {
         pd.Orders().Qin(0.0f);
         pd.Orders().Qrp(0.0f);        // redundant because safety does it... just added for clarity
         pd.Orders().Qac (acPumpCmdSpeed);

      }
      else       // let 'em rip...

      {
         pd.Orders().Qac (acPumpCmdSpeed);


         // and if APS is less than  mmHg then
         if ( pd.Status().APS() < CobeConfig::data().MssApsBackpressure )
         {
            pd.Orders().Qrp(ReturnPumpCmdSpeed);
            pd.Orders().Qin(0.0f);
            // else APS already at or above 300mmHg
         }
         else
         {
            pd.Orders().Qrp(ReturnPumpCmdSpeed);
            pd.Orders().Qin(InletPumpCmdSpeed);
         }


      }

   }

   if (  CurAPS >= CobeConfig::data().MssApsHigh || apsHigh )
   {

      if ( apsInstanceCount < numberOfAPSAlerts )
      {

         if (!apsHigh)
         {

            DataLog(log_level_proc_info) << "Found APS too High ->" << CurAPS << endmsg;
            apsHigh = true;
            pd.Orders().Qac (0.0f);
            pd.Orders().Qrp(0.0f);


         }
         else
         {

            DataLog(log_level_proc_info) << "trying to reduce APS too High ->" << CurAPS << endmsg;
            pd.Orders().Qin (pressureRecoverySpeed);
            pd.Orders().Qrp (0.0f);
            pd.Orders().Qac (0.0f);

         }

      }
      else           // apsInstanceCount >= numberOfAPSAlerts

      {
         if (apsPres2HighAlarm.getState() == CLEARED)
         {
            DataLog(log_level_proc_info) << "Alarm, its over #" << apsInstanceCount << endmsg;

            pd.SetPluggedFilter();

            // disable MSS for RBC
            pd.MakeRunWritable();
            pd.Run().MeterRbcs_OK = false;
            pd.MakeRunReadable();

            apsPres2HighAlarm.setAlarm();
         }

      }

   }

   if ( (CurAPS <= CobeConfig::data().MssApsBackpressure)
        && (apsInstanceCount < numberOfAPSAlerts)
        && apsHigh )
   {


      DataLog(log_level_proc_info) << "APS to backpressure ->" << CurAPS << endmsg;
      pd.Orders().Qin (0.0);
      pd.Orders().Qac (acPumpCmdSpeed);
      apsHigh = false;
      DataLog(log_level_proc_info) << "Alert Op #" << apsInstanceCount << endmsg;
      apsPres2HighAlert.setAlarm();
      apsInstanceCount++;

   }


   return NORMAL;
}

int MSSRBCPrime1::postProcess ()
{
   ProcData pd;

   float    acVolPumped     = pd.Volumes().VacTotal - initACPumpVolume;
   float    returnVolPumped = pd.Volumes().Vreturn  - initReturnPumpVolume;

   // for pfr
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(acVolPumped);
   pd.Run().hadPrimedReturnVolm.Set(returnVolPumped);
   pd.MakeRunReadable();


   if (!apsHigh)
   {

      // these alarms/alerts will be hard to get to in this state...
      if ((returnVolPumped < ReturnPrime1Vol)  )
      {
         if (acVolPumped >= CobeConfig::data().MssRbcSubstateMaxPrimeVol)
         {

            if (fluidTooLateAlert.getState() == CLEARED)
            {
               alertCounter++;      // bump up the alert counts


               // Why did we fail??
               DataLog(log_level_proc_info) << "fluid Too Late Alert! -- CobeConfig::MssRbcSubstateMaxPrimeVol  =>"  << CobeConfig::data().MssRbcSubstateMaxPrimeVol << endmsg;
               DataLog(log_level_proc_info) << "fluid Too Late Alert! -- Return  Volm Pumped =>"  << pd.Volumes().Vreturn - initReturnPumpVolume << endmsg;
               DataLog(log_level_proc_info) << "fluid Too Late Alert! -- AC      Volm Pumped =>"  << pd.Volumes().VacTotal - initACPumpVolume << endmsg;

               // reset to try again
               initACPumpVolume     = pd.Volumes().VacTotal;
               initReturnPumpVolume = pd.Volumes().Vreturn;
               initInletPumpVolume  = pd.Volumes().VinTotal;


               pd.MakeRunWritable();
               pd.Run().flagMSSRASVolumeError.Set(true);
               pd.MakeRunReadable();
               fluidTooLateAlert.setAlarm();

            }

         }    // end if (acVolPumped >= CobeConfig::data().MssRbcSubstateMaxPrimeVol)

      }
      else       // (returnVolPumped >= ReturnPrimeVol)

      {
         if (acVolPumped <= CobeConfig::data().MssRbcSubstateMinPrimeVol)
         {

            // Alarm...
            if (   (fluidTooSoonAlarm.getState() == CLEARED)
                   && !primeTooSoonFlag    // show this once only
                   && (alertCounter == 0)  // not after we did a prime too long
                   )
            {

               // Why did we fail??
               DataLog(log_level_proc_info) << "fluid Too Soon Alarm! -- CobeConfig::MssRbcSubstateMinPrimeVol  =>"  << CobeConfig::data().MssRbcSubstateMinPrimeVol << endmsg;
               DataLog(log_level_proc_info) << "fluid Too Soon Alarm! -- Return  Volm Pumped =>"  << returnVolPumped << "  " <<  pd.Volumes().Vreturn - initReturnPumpVolume << endmsg;
               DataLog(log_level_proc_info) << "fluid Too Soon Alarm! -- AC      Volm Pumped =>"  <<  acVolPumped    << "  " <<  pd.Volumes().VacTotal - initACPumpVolume << endmsg;

               primeTooSoonFlag = true;
               pd.MakeRunWritable();
               pd.Run().flagMSSRASVolumeError.Set(true);
               pd.MakeRunReadable();

               fluidTooSoonAlarm.setAlarm();

               //////////////////////////////////////////////////////////
               // !! prime too soon is now R1 continuable IT 8113   !!
               /////////////////////////////////////////////////////////
               // special processing to allow continue if RBC fails and PLT passes
               // if (pd.RunTargets().MeterRbcs && pd.RunTargets().MeterPlatelets)
               // {

               // set flag indicating RBC metering (or we got errors) is OFF due to errors
               // pd.MakeRunWritable();
               //    pd.Run().MeterRbcs_OK = false; // this will precipitate a transition
               // pd.MakeRunReadable();
               // }

            }


            // This is the path you should be going down...
         }
         else         // (returnVolPumped >= ReturnPrime1Vol)  &&  (plasmaVolPumped > CobeConfig::data().MssPltSubstateMin1PrimeVol)

         {     // set this when you are complete with priming
            transitionNow = true;

         }

      }
   }
   return NORMAL;
}

int MSSRBCPrime1::transitionStatus ()
{
   ProcData pd;

   // if we're not doing MSS for RBC then get the heck out of here
   if (!(pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK))
   {
      return 1;
   }

   if (pd.Run().mssInPFR)
   {
      if ( pd.Run().currentMSSSubState > SS_MSSRBCPRIME1  )
         return 1;
   }

   if (transitionNow)
   {

      pd.Orders().Qac(0.0);
      pd.Orders().Qin(0.0);
      pd.Orders().Qrp(0.0);

      DataLog(log_level_proc_info) << "MSSRBCPrime1::Storage Solution Remaining in Bag =>"  << pd.Volumes().VrbcStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "MSSRBCPrime1::AC      Volm Pumped =>"  << pd.Volumes().VacTotal - initACPumpVolume << endmsg;
      DataLog(log_level_proc_info) << "MSSRBCPrime1::Return  Volm Pumped =>"  << pd.Volumes().Vreturn - initReturnPumpVolume << endmsg;
      // DataLog(log_level_proc_info) << "MSSRBCPrime1::Inlet   Volm Pumped =>"  << pd.Volumes().VinTotal - initInletPumpVolume << endmsg;


      return 1;
   }


   // if we did'nt do rinseback skip this is salvage case
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered! this is salvage case"   << endmsg;
      return 1;
   }


   return NO_TRANSITION;


}

int MSSRBCPrime1::preEnter ()
{


   ProcData pd;

   // get the initial setting of thes volumes
   initACPumpVolume      = pd.Volumes().VacTotal;
   initReturnPumpVolume  = pd.Volumes().Vreturn;
   initInletPumpVolume   = pd.Volumes().VinTotal;

   initACPumpVolume     -= pd.Run().hadPrimedVolm.Get();
   initReturnPumpVolume -= pd.Run().hadPrimedReturnVolm.Get();

   if (!pd.Run().mssInPFR)  // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState.Set(SS_MSSRBCPRIME1);
      pd.MakeRunReadable();

   }
   else
   {

      if ( (pd.Run().currentMSSSubState.Get() == SS_MSSRBCPRIME1)  )
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR; with AC pumped => " << pd.Run().hadPrimedVolm.Get() << "; Return was at =>" << pd.Run().hadPrimedReturnVolm.Get() << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }


   }

   return NORMAL;
}

int MSSRBCPrime1::postExit ()
{

   ProcData pd;

   transitionNow = false;

   // clear at end of state because this is common  to all primes
   pd.MakeRunWritable();
   pd.Run().hadPrimedVolm.Set(0.0f);
   pd.Run().hadPrimedReturnVolm.Set(0.0f);
   pd.MakeRunReadable();

   // kill the pumps
   pd.Orders().Qac(0.0);
   pd.Orders().Qin(0.0);
   pd.Orders().Qrp(0.0);

   return NORMAL;
}

void MSSRBCPrime1::copyOver (const MSSRBCPrime1&)
{}

void MSSRBCPrime1::cleanup ()
{}

/* FORMAT HASH 8aa6a5509dda7aa15369d2df2512c371 */
