/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTAddition.cpp
 *
 *       This state will meter storage solution to the platelet product.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPLTAddition.cpp 1.33 2009/04/24 16:46:08Z jheiusb Exp jheiusb $
$Log $

*/


#include "mssPLTAddition.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"    // for all the cobe config values
#include "MssCommon.h"
#include "guiproc.h"



const int PurgeCycleCount = 2;

// IT9818 reduce from 3 to 1
const int NumberOfPurgesPerRun = 1; // this is the maximum number of times we will re-purge during PAS addition



DEFINE_STATE(MSSPLTAddition);



MSSPLTAddition::MSSPLTAddition()
   : StateAbs(),
     transitionNow(false),
     solutionToBeMeteredOut(0.0),
     initialCollectVolume(0.0),
     initialPlasmaVolume(0.0),
     part1done(false),
     part2done(false),
     part3done(true), // true to skip till end
     purgeplasmaVol(0.0),
     purgeplateletVol(0.0),
     cycle(0),
     _setupplasmaPumped(0.0),
     _VerifyClosureAlarm(CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT),
// valveTimer( 0,Callback<MSSPLTAddition>( this, &MSSPLTAddition::moveValve ), TimerMessage::DISARMED ),
     numberOfPurges(0),
     prepurgeplasmaVol(0.0f)

{}

MSSPLTAddition::MSSPLTAddition(const MSSPLTAddition& state)
   : StateAbs(state),
     transitionNow(false),
     solutionToBeMeteredOut(0.0),
     initialCollectVolume(0.0),
     initialPlasmaVolume(0.0),
     part1done(false),
     part2done(false),
     part3done(true),
     purgeplasmaVol(0.0),
     purgeplateletVol(0.0),
     cycle(0),
     _setupplasmaPumped(0.0),
     _VerifyClosureAlarm(CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT),
// valveTimer( 0,Callback<MSSPLTAddition>( this, &MSSPLTAddition::moveValve ), TimerMessage::DISARMED ),
     numberOfPurges(0),
     prepurgeplasmaVol(0.0f)
{}

MSSPLTAddition::~MSSPLTAddition()
{}

int MSSPLTAddition::preProcess ()
{

   ProcData pd;

   // dont do pruges more than NumberOfPurgesPerRun times.
   if  (numberOfPurges < NumberOfPurgesPerRun )
   {

      if ( pd.Run().redoCassettePurge.Get() )
      {

         if (cycle < PurgeCycleCount)
         {
            doAPurgePre();
            return NORMAL;
         }
      }


      if (    pd.Run().cassettePurgeSetup.Get()
              && !pd.Run().redoCassettePurge.Get() )
      {

         float red;
         float green;

         red   = (float)pd.Status().RedValue();
         green = (float)pd.Status().GreenValue();

         pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

         _setupplasmaPumped += pd.Status().PlasmaPump.dV();

         if (_setupplasmaPumped < CobeConfig::data().MssVac1PlasmaVol)
         {

            pd.Orders().Qac (0.0f);
            pd.Orders().Qcollect(0.0f);
            pd.Orders().Qin(0.0f);
            pd.Orders().Qrp(0.0f);

            pd.Orders().Qplasma (CobeConfig::data().MssVac1PlasmaSpeed);

            DataLog(log_level_proc_info) << "cassettePurgeSetup pumped ->" <<  _setupplasmaPumped  << " : RGratio ->" << red / green << endmsg;
         }
         else
         {

            DataLog(log_level_proc_info) << "cassettePurgeSetup Alarming " <<  _setupplasmaPumped << " : RGratio ->" << red / green << endmsg;

            pd.MakeRunWritable();
            pd.Run().redoCassettePurge.Set(true);
            pd.Run().cassettePurgeSetup.Set(false);
            pd.MakeRunReadable();

            pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
            pd.Orders().Qac (0.0f);
            pd.Orders().Qplasma (0.0f);
            pd.Orders().Qcollect(0.0f);
            pd.Orders().Qin(0.0f);
            pd.Orders().Qrp(0.0f);


            if ( ( _VerifyClosureAlarm.getState() == CLEARED) )
            {
               _VerifyClosureAlarm.setAlarm("MSS RbcMonitor Triggered alarm");



               pd.MakePQIWritable();
               pd.PQI().ClampClosureFailedFullFailure.Set(true);
               pd.MakePQIReadable();
            }
         }

         return NORMAL;
      }




      if (    pd.Run().pauseCassettePurge.Get()
              && !pd.Run().redoCassettePurge.Get())
      {
         pd.Orders().Qplasma (0.0);
         pd.Orders().Qcollect(0.0);

         return NORMAL;
      }

   }  // end if (numberOfPurges < 3)


   if ( !pd.Status().CollectValve.IsCollecting() )
   {
      return NORMAL;
   }

   // valveTimer.interval( 0 ); // disable timer

   // start up the pumps
   pd.Orders().Qplasma (calculatePumpSpeed(pd.Volumes().VstoragePltBag) );
   pd.Orders().Qcollect(calculatePumpSpeed(pd.Volumes().VstoragePltBag) );



   // PLT addition
   return NORMAL;
}

int MSSPLTAddition::postProcess ()
{


   ProcData pd;

   // dont do pruges more than tree times.
   if  (numberOfPurges < NumberOfPurgesPerRun )
   {
      if ( pd.Run().redoCassettePurge.Get()  )
      {

         if (cycle < PurgeCycleCount)
         {
            doAPurgePost();
            return NORMAL;
         }
      }


      if (    pd.Run().cassettePurgeSetup.Get()
              && !pd.Run().redoCassettePurge.Get() )
      {
         return NORMAL;
      }

      if ( pd.Run().pauseCassettePurge.Get()
           && !pd.Run().redoCassettePurge.Get())
      {
         return NORMAL;
      }
   }

   // plasma pump determines SS use

   // record the values in procvolume_cds
   pd.MakeVolumesWritable();

   // amount thats gone to product so far = amount the plasma pump has pumped so far
   pd.Volumes().VstoragePltBag = pd.Volumes().VstoragePltBag + pd.Status().PlasmaPump.dV();


   pd.MakeVolumesReadable();

   // disable the replace bag alarm if we are close to comleteing
   if (!pd.Run().stopCheckingPltASBagEmpty.Get() &&
       pd.Volumes().VstoragePltBag >= solutionToBeMeteredOut - 5.0f /*mL*/)
   {
      pd.MakeRunWritable();
      pd.Run().stopCheckingPltASBagEmpty.Set(true);
      pd.MakeRunReadable();
   }

   // if the amount thats been sent to the product bag is >= the solution volume we set to be metered...
   //   then we're done.
   if ( pd.Volumes().VstoragePltBag >= solutionToBeMeteredOut)
   {

      // stop the metering!
      pd.Orders().Qplasma (0.0);
      pd.Orders().Qcollect(0.0);

      // close the PLT valve

      DataLog(log_level_proc_info) << "Commanding PLT valve to Return" << endmsg;
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);


      // set this when SS has been completely added
      transitionNow = true;

   }

   // Tell GUI where we're at so we can update the status bar.
   GuiAasUpdater::sendUpdate();

   return NORMAL;
}

int MSSPLTAddition::transitionStatus ()
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
      if (    (pd.Run().currentMSSSubState ==  SS_MSSPARALLELPROC )
              && (pd.Run().currentPLTMSSSubState >  SS_MSSPLTADDITION )  )
         return 1;
   }


   if (transitionNow)
   {

      // if we arnt doing a PLT mss then we can send the completed message now
      if ( !(pd.RunTargets().MeterRbcs && pd.Run().MeterRbcs_OK)
           || pd.Run().RBCMeteringDone )
      {
         // Send a message to GUI to tell it that we have completed SS addition.
         MSSCompletionMsg _completeMeteredStorage(MessageBase::SEND_GLOBAL);
         _completeMeteredStorage.send(END_MSS_MESSAGE_SENDER_PAS);
      }

      DataLog(log_level_proc_info) << "Storage Solution to PLT Product Bag =>"  << solutionToBeMeteredOut  << endmsg;
      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag   =>"  << pd.Volumes().VpltStorageRemaining << endmsg;
      DataLog(log_level_proc_info) << "Platelet Volm Pumped =>"  << pd.Volumes().Vc - initialCollectVolume << endmsg;
      DataLog(log_level_proc_info) << "Plasma Volm Pumped   =>"  << pd.Volumes().Vp - initialPlasmaVolume << endmsg;
      DataLog(log_level_proc_info) << "Completed all MSSPLTAddition"  << endmsg;

      // signal that we've passed the gate
      pd.MakeRunWritable();
      pd.Run().PltMeteringDone = true;
      pd.MakeRunReadable();

      return 1;
   }


   return NO_TRANSITION;

}

int MSSPLTAddition::preEnter ()
{

   ProcData pd;


   // get the solution vol to be metered
   solutionToBeMeteredOut = pd.RunTargets().PltSolutionTarget  *  pd.Run().PltSSVolumeFactor;
   DataLog(log_level_proc_info) << "Raw Storage Solution volume for    PLT  =>"  << pd.RunTargets().PltSolutionTarget << endmsg;
   DataLog(log_level_proc_info) << "PLT Storage Solution adjustment factor  =>"    << pd.Run().PltSSVolumeFactor << endmsg;

   // WARNING:  OR uses the below line... do not remove or alter text....
   DataLog(log_level_proc_info) << "Actual Storage Solution to Meter to PLT Product =>"  << solutionToBeMeteredOut << endmsg;

   if (!pd.Run().mssInPFR)   // if we're NOT  in PFR
   {
      pd.MakeRunWritable();
      // setup this for PFR
      pd.Run().currentPLTMSSSubState = SS_MSSPLTADDITION;
      pd.MakeRunReadable();
   }
   else
   {
      if (pd.Run().currentPLTMSSSubState.Get() == SS_MSSPLTADDITION)
      {
         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;

         DataLog(log_level_proc_info) << "PFR: plt Storage Solution Remaining in Bag   =>"  << pd.Volumes().VpltStorageRemaining << endmsg;
         DataLog(log_level_proc_info) << "PFR: plt Storage Solution in PLT Product Bag =>"  << pd.Volumes().VstoragePltBag << endmsg;

         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();
      }
   }



   if ( (pd.RunTargets().MeterPlatelets && pd.Run().MeterPlatelets_OK) )
   {
      // Send msg to GUI to let tell it we have entered SS Addition.
      MSSPASAdditionMsg _addMeteredStorage(MessageBase::SEND_GLOBAL);
      _addMeteredStorage.send(0);

      // make sure the valve to the plt bag is still in collect
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);
   }

   // get the starting point  of the collect pumps volume
   initialCollectVolume = pd.Volumes().Vc;
   initialPlasmaVolume  = pd.Volumes().Vp;

   return NORMAL;
}

int MSSPLTAddition::postExit ()
{

   ProcData pd;

   transitionNow = false ;

   pd.Orders().Qplasma (0.0f);
   pd.Orders().Qcollect(0.0f);

   // close the PLT valve

   DataLog(log_level_proc_info) << "Commanding PLT valve to Return" << endmsg;
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

   return NORMAL;
}

void MSSPLTAddition::copyOver (const MSSPLTAddition&)
{}

void MSSPLTAddition::cleanup ()
{}

void MSSPLTAddition::doAPurgePre ()
{
   ProcData pd;


   if (!part1done)
   {
      pd.Orders().Qcollect(CobeConfig::data().MssVac1CollectSpeed);
      pd.Orders().Qplasma (CobeConfig::data().MssVac1PlasmaSpeed);
   }
   else
   {
      if (!part2done)
      {
         pd.Orders().Qcollect (CobeConfig::data().MssVac2CollectSpeed);
         pd.Orders().Qplasma (CobeConfig::data().MssVac2PlasmaSpeed);
      }
      else
      {
         if (!part3done)
         {
            pd.Orders().Qcollect(CobeConfig::data().MssVac4CollectSpeed);
            pd.Orders().Qplasma (CobeConfig::data().MssVac4PlasmaSpeed);
         }
      }
   }


}

void MSSPLTAddition::doAPurgePost ()
{
   ProcData pd;


   float red;
   float green;

   red   = (float)pd.Status().RedValue();
   green = (float)pd.Status().GreenValue();


   purgeplateletVol += pd.Status().CollectPump.dV();
   purgeplasmaVol   += pd.Status().PlasmaPump.dV();


   if (!part1done)
   {

      if (    (purgeplateletVol >= CobeConfig::data().MssVac1CollectVol)
              && (purgeplasmaVol   >= CobeConfig::data().MssVac1PlasmaVol) )
      {

         part1done = true;
         // transitionNow = true;

         DataLog(log_level_proc_info) << "MSS Recovery Phase -> 1 Completed: " << purgeplasmaVol << "ml pumped" <<  " : RGratio ->" << red / green << endmsg;

         purgeplateletVol = 0.0f;
         purgeplasmaVol   = 0.0f   ;
      }

   }
   else      // part1 is done


   {
      if (!part2done)
      {

         if (purgeplasmaVol >=  CobeConfig::data().MssVac2PlasmaVol)
         {

            part2done = true;
            DataLog(log_level_proc_info) << "MSS Recovery Phase -> 2 Completed: " << purgeplasmaVol << "ml pumped" <<  " : RGratio ->" << red / green << endmsg;


            purgeplateletVol = 0.0f;
            purgeplasmaVol   = 0.0f;
         }

      }
      else         // part2 is done


      {
         if (!part3done || (PurgeCycleCount == 1))
         {


            if (purgeplasmaVol >=  CobeConfig::data().MssVac4PlasmaVol)
            {

               part3done = true;
               DataLog(log_level_proc_info) << "Cycle => " << cycle << ";"
                                            << "Phase -> 4 Completed: " << purgeplasmaVol << "ml pumped" <<  " : RGratio ->" << red / green << endmsg;

               purgeplateletVol = 0.0f;
               purgeplasmaVol   = 0.0f;

            }

         }
         else          // all parts done

         {
            purgeplateletVol = 0.0f;
            purgeplasmaVol   = 0.0f   ;

            DataLog(log_level_proc_info) << "MSS Recovery Phase Completed: for cycle " <<  cycle  << " : RGratio ->" << red / green << endmsg;
            cycle++;
            endAPurge();
         }

      }

   }



}

void MSSPLTAddition::endAPurge ()
{
   ProcData pd;

   DataLog(log_level_proc_info) << "endAPurge: cycle->" << cycle << endmsg;

   // reset all the parts o
   part1done = false;
   part2done = false;
   if (cycle == PurgeCycleCount - 1)
      part3done = false;
   else
      part3done = true;

   purgeplateletVol = 0.0f;
   purgeplasmaVol   = 0.0f;

   // notArmed = true;

   if (cycle >= PurgeCycleCount)
   {

      pd.Orders().Qac (0.0f);
      pd.Orders().Qplasma (0.0f);
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qin(0.0f);
      pd.Orders().Qrp(0.0);


      _setupplasmaPumped = 0.0;

      // reset these flags
      pd.MakeRunWritable();
      pd.Run().redoCassettePurge.Set(false);
      pd.MakeRunReadable();

      pd.MakeRunWritable();
      pd.Run().pauseCassettePurge.Set(false);
      pd.MakeRunReadable();

      pd.MakeRunWritable();
      pd.Run().cassettePurgeSetup.Set(false);
      pd.MakeRunReadable();


      cycle = 0;

      // no more timer, just move it!
      DataLog(log_level_proc_info) << "Commanding PLT valve to COLLECT" << endmsg;
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_COLLECT);

      numberOfPurges++;   // add um up, we only get three purges

      DataLog(log_level_proc_info) << "Re-do of Vac Purge for MSS Recovery completed" << endmsg;
   }

}

float MSSPLTAddition::calculatePumpSpeed (float v)
{

   ProcData pd;

   // double check constraint
   if (v < 0.0)
      v = 0.0;


   float speed =  124.7f * v + 2.95f;

   // if the speed is too high or we arent doing the pink PAS routine
   if (    (  speed > CobeConfig::data().MssPltAddSpeed    )
           || !(  CobeConfig::data().MssPltSSAddRampup > 0 )
           )
   {
      speed = CobeConfig::data().MssPltAddSpeed;
   }




   return speed;


}

/* FORMAT HASH 024d9074c28ace65fb5f5ac064e70ecc */
