/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      resSetup.cpp
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/resSetup.cpp 1.7 2008/04/08 21:43:11Z jheiusb Exp jheiusb $
$Log: resSetup.cpp $
Revision 1.7  2008/04/08 21:43:11Z  jheiusb
8115 -- fix prime and General PFR stuff
Revision 1.6  2006/09/28 20:21:24Z  jheiusb
7707 -- add rinseback monitor for MSS
Revision 1.5  2006/07/26 22:38:04Z  jheiusb
7672 -- add MSS rinseback started flag
Revision 1.4  2006/07/21 16:21:54Z  jheiusb
7601 -- salvage case added
Revision 1.3  2006/04/19 22:08:51Z  jheiusb
7491 -- PFR hangs
Revision 1.2  2006/03/27 17:53:10Z  jheiusb
7350 -- PFR for MSS
Revision 1.1  2006/03/03 17:17:45Z  jheiusb
Initial revision
Revision 1.10  2006/03/02 18:16:29Z  jheiusb
add many MSS values to CobeConfig
Revision 1.9  2006/02/23 17:15:11Z  jheiusb
7256 -- return and Inlet speeds slowed to better match start stops
Revision 1.8  2005/12/21 21:38:54Z  jheiusb
7202 -- fine tune speeds for PLT metering
Revision 1.7  2005/11/30 19:13:39Z  jheiusb
7266 -- inlet alarm fixed, along with inlet pump volm accrual
Revision 1.6  2005/11/17 22:47:26Z  jheiusb
7230 -- stop pump condition
Revision 1.5  2005/10/25 20:58:40Z  jheiusb
7202 -- initial PLT mss path
Revision 1.4  2005/10/12 21:31:57Z  jheiusb
7202 -- reservoir setup initial logic added
Revision 1.3  2005/10/07 22:06:52Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:36:23Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#include "resSetup.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(ResSetup);

const float returnInletMatchSpeed = CobeConfig::data().MssReturnInletMatchSpeed;

ResSetup::ResSetup()
   : StateAbs(),
     transitionNow(false),
     initialRetVol(0.0),
     initialInletVol(0.0)
{}

ResSetup::ResSetup(const ResSetup& state)
   : StateAbs(),
     transitionNow(false),
     initialRetVol(0.0),
     initialInletVol(0.0)
{}

ResSetup::~ResSetup()
{}

int ResSetup::preProcess ()
{

   ProcData pd;

   // skip this if rinseback not entered.
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      transitionNow = true;
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return NORMAL;
   }

   // if reservoir is NOT Empty
   if ( !pd.Status().ReservoirIsEmpty()  )
   {

      // and if APS is less than 300 mmHg then
      if ( pd.Status().APS() < CobeConfig::data().MssApsBackpressure )
      {
         // run the return pump at 50ml/min
         pd.Orders().Qrp(returnInletMatchSpeed);
         // dont run inlet pump in this case.
         pd.Orders().Qin(0.0f);



         // else APS already at or above 300mmHg
      }
      else
      {

         // run inlet and return pump at 50ml/min to maintain the APS pressure
         pd.Orders().Qrp(returnInletMatchSpeed);
         pd.Orders().Qin(returnInletMatchSpeed);
      }

      // else reservoir is empty
   }
   else
   {


      DataLog(log_level_proc_info) << "Reservoir Empty -- ResSetup complete" << endmsg;
      // if res is empty we are done with reservoir setup state ...
      //   no matter what the APS pressure is

      // Stop the pumps ...  we are done
      pd.Orders().Qin(0.0f);
      pd.Orders().Qrp(0.0f);


      DataLog(log_level_proc_info) << "Inlet  Volume Pumped => " << (pd.Volumes().VinTotal - initialInletVol)   << endmsg;
      DataLog(log_level_proc_info) << "Return Volume Pumped => " << (pd.Volumes().Vreturn - initialRetVol) << endmsg;

      // set this to true when you are complete with resSetup
      transitionNow = true;

   }

   return NORMAL;
}

int ResSetup::postProcess ()
{
   return NORMAL;
}

int ResSetup::transitionStatus ()
{

   ProcData pd;
   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)  // if we're in PFR
   {

      if ( pd.Run().currentMSSSubState > SS_RESSETUP  )
         return 1;
   }
   if (transitionNow)
   {

      return 1;
   }
   // if we did'nt do rinseback skip this salvage case
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {

      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return 1;
   }
   return NO_TRANSITION;

}

int ResSetup::preEnter ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)  // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_RESSETUP;
      pd.MakeRunReadable();
   }
   else
   {

      if ( (pd.Run().currentMSSSubState.Get() == SS_RESSETUP)  )
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;
         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }


   }
   initialRetVol   = pd.Volumes().Vreturn;
   initialInletVol = pd.Volumes().VinTotal;

   return NORMAL;
}

int ResSetup::postExit ()
{

   ProcData pd;

   transitionNow = false;
   // Stop the pumps ...  we are done
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0f);

   return NORMAL;
}

void ResSetup::copyOver (const ResSetup&)
{}

void ResSetup::cleanup ()
{}

/* FORMAT HASH 7e2785701515923c0cb0e19d355493a8 */
