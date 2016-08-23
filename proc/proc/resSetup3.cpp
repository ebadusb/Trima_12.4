/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResSetup3.cpp
 *
 */


#include "resSetup3.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values

DEFINE_STATE(ResSetup3);

const float returnInletMatchSpeed = CobeConfig::data().MssReturnInletMatchSpeed;

ResSetup3::ResSetup3()
   : StateAbs(),
     transitionNow(false),
     initialRetVol(0.0),
     initialInletVol(0.0),
     holdPumpsDueToPressure(false)
{}

ResSetup3::ResSetup3(const ResSetup3& state)
   : StateAbs(),
     transitionNow(false),
     initialRetVol(0.0),
     initialInletVol(0.0),
     holdPumpsDueToPressure(false)
{}

ResSetup3::~ResSetup3()
{}

int ResSetup3::preProcess ()
{

   ProcData pd;

   // skip this if rinseback not entered.
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      transitionNow = true;
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return NORMAL;
   }


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

int ResSetup3::postProcess ()
{
   return NORMAL;
}

int ResSetup3::transitionStatus ()
{

   ProcData pd;


   // if we're not doing MSS for PLT & RBC's then this is not needed
   if (
      !(   (pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK)
           && (pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK)
           )
      )
   {
      return 1;
   }


   // check if PFR and we need to do this yet!
   if (pd.Run().mssInPFR)  // if we're in PFR
   {

      if ( pd.Run().currentMSSSubState > SS_RESSETUP3  )
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

int ResSetup3::preEnter ()
{
   ProcData pd;

   if (!pd.Run().mssInPFR)  // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentMSSSubState = SS_RESSETUP3;
      pd.MakeRunReadable();
   }
   else
   {

      if ( (pd.Run().currentMSSSubState.Get() == SS_RESSETUP3)  )
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

int ResSetup3::postExit ()
{

   ProcData pd;

   transitionNow = false;
   // Stop the pumps ...  we are done
   pd.Orders().Qin(0.0f);
   pd.Orders().Qrp(0.0f);

   return NORMAL;
}

void ResSetup3::copyOver (const ResSetup3&)
{}

void ResSetup3::cleanup ()
{}

/* FORMAT HASH 5fb794f5d8d10e6224bb3142d6c8a8d9 */
