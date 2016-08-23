/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ChannelCheck.cpp
 *
 *    the vacuum purge consist of three parts that get run
 *    CobeConfig::data().MssVacuumCycles number of times.
 *
 *
 *
 *
 *
 */

#include "vacPurge.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"


const float PAUSE_TIME = CobeConfig::data().MssVacPurgeCyclePause * 1000.0f; // 3000; // in millisecs of course (


DEFINE_STATE(VacPurge);

VacPurge::VacPurge()
   : StateAbs(),
     transitionNow(false),
     plateletVol(0.0f),
     plasmaVol(0.0f),
     part1done(false),
     part2done(false),
     part3done(false),
     holdPumpsDueToPressure(false),
     pauseFlag(false),
     pauseTimer(0, Callback<VacPurge>(this, &VacPurge::clearPause), TimerMessage::DISARMED),
     subpause1(false),
     subpause2(false),
     cycle(1) // start cycles at 1 so it works with the cobeconfig value
{}

VacPurge::VacPurge(const VacPurge& state)
   : StateAbs(),
     transitionNow(false),
     plateletVol(0.0f),
     plasmaVol(0.0f),
     part1done(false),
     part2done(false),
     part3done(false),
     holdPumpsDueToPressure(false),
     pauseFlag(false),
     subpause1(false),
     subpause2(false),
     pauseTimer(0, Callback<VacPurge>(this, &VacPurge::clearPause), TimerMessage::DISARMED),
     cycle(1) // start cycles at 1 so it works with the cobeconfig value
{
   copyOver(state);
}

VacPurge::~VacPurge()
{
   cleanup();
}

void VacPurge::clearPause ()
{

   DataLog(log_level_proc_info) << "End second timer"   << endmsg;
   pauseTimer.interval(0);
   pauseFlag = false;


}

int VacPurge::preProcess ()
{

   ProcData pd;


   // do a shortcircuit to pause....
   if (pauseFlag)
   {

      pd.Orders().Qcollect(0.0);
      pd.Orders().Qplasma (0.0);

      // DataLog( log_level_proc_info ) << "pausing cycles"   << endmsg;
      return NORMAL;
   }

   if (!part1done)
   {

      // DataLog(log_level_proc_info) << "START Cycle => "  << cycle << ";"
      //                              << "Phase -> 1 : " << plasmaVol << "ml pumped" << endmsg;

      pd.Orders().Qcollect(CobeConfig::data().MssVac1CollectSpeed);
      pd.Orders().Qplasma (CobeConfig::data().MssVac1PlasmaSpeed);
   }
   else
   {
      if (!part2done)
      {

         //   DataLog(log_level_proc_info) << "START Cycle => "  << cycle << ";"
         //                                << "Phase -> 2 : " << plasmaVol << "ml pumped" << endmsg;


         pd.Orders().Qcollect (CobeConfig::data().MssVac2CollectSpeed);
         pd.Orders().Qplasma (CobeConfig::data().MssVac2PlasmaSpeed);
      }
      else
      {
         if (!part3done)
         {

            //   DataLog(log_level_proc_info) << "START Cycle => "  << cycle << ";"
            //                                << "Phase -> 3 : " << plasmaVol << "ml pumped" << endmsg;

            pd.Orders().Qcollect(CobeConfig::data().MssVac3CollectSpeed);
            pd.Orders().Qplasma (CobeConfig::data().MssVac3PlasmaSpeed);
         }
      }
   }
   return NORMAL;
}



int VacPurge::postProcess ()
{

   // do a shortcircuit to pause....
   if (pauseFlag)
   {

      return NORMAL;

   }


   ProcData pd;

   plateletVol += pd.Status().CollectPump.dV();
   plasmaVol   += pd.Status().PlasmaPump.dV();

   // save these incase of PFR
   pd.MakeRunWritable();
   pd.Run().vacPurgePltVolm =  plateletVol;
   pd.Run().vacPurgePlsVolm =  plasmaVol;
   pd.MakeRunReadable();

   if (!part1done)
   {

      if (    (plateletVol >= CobeConfig::data().MssVac1CollectVol)
              && (plasmaVol   >= CobeConfig::data().MssVac1PlasmaVol) )
      {

         part1done = true;

         DataLog(log_level_proc_info) << "Cycle => "  << cycle << ";"
                                      << "Phase -> 1 Completed: " << plasmaVol << "ml pumped" << endmsg;

         // update the SS bag volm remaining
         // pd.MakeVolumesWritable();
         //    pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVol;
         // pd.MakeVolumesReadable();


         pd.MakeRunWritable();
         pd.Run().mssVacPhase =  2;
         pd.MakeRunReadable();

         plateletVol = 0.0f;
         plasmaVol   = 0.0f   ;
      }

   }
   else     // part1 is done


   {
      if (!part2done)
      {

         if (plasmaVol >=  CobeConfig::data().MssVac2PlasmaVol)
         {
            if (PAUSE_TIME > 0.0f)
            {
               DataLog(log_level_proc_info) << "Start second timer"   << endmsg;
               part2done = true;
               pauseFlag = true;         // pause everthing for a bit.
               pauseTimer.interval(PAUSE_TIME);
            }


            DataLog(log_level_proc_info) << "Cycle => "  << cycle << ";"
                                         << "Phase -> 2 Completed: " << plasmaVol << "ml pumped" << endmsg;

            // update the SS bag volm remaining
            // pd.MakeVolumesWritable();
            //    pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVol;
            // pd.MakeVolumesReadable();

            pd.MakeRunWritable();
            pd.Run().mssVacPhase =  3;
            pd.MakeRunReadable();

            plateletVol = 0.0f;
            plasmaVol   = 0.0f   ;
         }

      }
      else        // part2 is done

      {
         if (!part3done)
         {



            if (PAUSE_TIME > 0.0f)
            {

               if (( plasmaVol >=  CobeConfig::data().MssVac3PlasmaVol / 3.0f )  && !subpause1 )
               {

                  DataLog(log_level_proc_info) << "pausing cycle3 at " << plasmaVol   << endmsg;
                  subpause1 = true;       // pause everthing for a bit.
                  pauseFlag = true;       // pause everthing for a bit.
                  pauseTimer.interval(PAUSE_TIME);
               }

               if (( plasmaVol >=  CobeConfig::data().MssVac3PlasmaVol / 1.5f ) && !subpause2 )
               {

                  DataLog(log_level_proc_info) << "pausing cycle3 at " << plasmaVol   << endmsg;
                  subpause2 = true;       // pause everthing for a bit.
                  pauseFlag = true;       // pause everthing for a bit.
                  pauseTimer.interval(PAUSE_TIME);

               }
            }

            if (plasmaVol >=  CobeConfig::data().MssVac3PlasmaVol)
            {

               part3done = true;
               DataLog(log_level_proc_info) << "Cycle => " << cycle << ";"
                                            << "Phase -> 3 Completed: " << plasmaVol << "ml pumped" << endmsg;


               // update the SS bag volm remaining
               // pd.MakeVolumesWritable();
               //   pd.Volumes().VpltStorageRemaining = pd.Volumes().VpltStorageRemaining - plasmaVol;
               // pd.MakeVolumesReadable();

               pd.MakeRunWritable();
               pd.Run().mssVacPhase = 1;
               pd.MakeRunReadable();

               plateletVol = 0.0f;
               plasmaVol   = 0.0f;

               subpause1   = false;
               subpause2   = false;

            }

         }
         else         // all parts done

         {      // increment a cycle
            cycle++;

            pd.MakeRunWritable();
            pd.Run().mssVacCycle = cycle;
            pd.Run().mssVacPhase = 1;
            pd.MakeRunReadable();

            // reset all the parts of the next cycle to not done
            part1done = false;
            part2done = false;
            part3done = false;


         }

      }

   }

   return NORMAL;
}

int VacPurge::transitionStatus ()
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
              && ( pd.Run().currentPLTMSSSubState > SS_MSSVACPURGE )  )
         return 1;
   }

   // if we did'nt do rinseback skip this is salvage case
   if (!pd.Run().rinsebackVolumeSuccessMss.Get() )
   {
      DataLog(log_level_proc_info) << "Skipping out because rinseback not entered!"   << endmsg;
      return 1;
   }


   // dont exit when we've completed the proper number of cycles of vac purging
   if (cycle > CobeConfig::data().MssVacuumCycles) // make 3 a cobe config
   {

      DataLog(log_level_proc_info) << "Vacuum purge cycles run => "  << CobeConfig::data().MssVacuumCycles << endmsg;
      DataLog(log_level_proc_info) << "Storage Solution Remaining in Bag => "  << pd.Volumes().VpltStorageRemaining << endmsg;

      // stop the pumps
      pd.Orders().Qplasma  (0.0f);
      pd.Orders().Qcollect (0.0f);

      return 1;
   }

   return NO_TRANSITION;
}

int VacPurge::preEnter ()
{

   ProcData pd;


   if (!pd.Run().mssInPFR)   // if we're NOT in PFR
   {

      pd.MakeRunWritable();
      pd.Run().currentPLTMSSSubState = SS_MSSVACPURGE;
      pd.Run().mssVacCycle           = cycle;
      pd.Run().mssVacPhase           = 1;
      pd.MakeRunReadable();



      // i have initially accrued zero volume on these pumps
      plateletVol = 0.0f;
      plasmaVol   = 0.0f;



   }
   else       // if we're in PFR


   {
      if (pd.Run().currentPLTMSSSubState.Get() == SS_MSSVACPURGE)
      {

         DataLog(log_level_proc_info) << "Back to PFR point, continuing as non-PFR" << endmsg;

         // get where we left off
         plateletVol = pd.Run().vacPurgePltVolm;
         plasmaVol   = pd.Run().vacPurgePlsVolm   ;

         cycle       =  pd.Run().mssVacCycle ;
         if (pd.Run().mssVacPhase == 1 )
         {
            part1done = false;
            part2done = false;
            part3done = false;
            DataLog(log_level_proc_info) << "PFR starting at Cycle => "  << cycle << ";" << " Phase -> 1" << endmsg;
         }
         else if (pd.Run().mssVacPhase == 2)
         {
            part1done = true;
            part2done = false;
            part3done = false;
            DataLog(log_level_proc_info) << "PFR starting at Cycle => "  << cycle << ";" << " Phase -> 2" << endmsg;
         }
         else if (pd.Run().mssVacPhase == 3)
         {
            part1done = true;
            part2done = true;
            part3done = false;
            DataLog(log_level_proc_info) << "PFR starting at Cycle => "  << cycle << ";" << " Phase -> 3" << endmsg;
         }
         else
         {
            part1done = false;
            part2done = false;
            part3done = false;
            DataLog(log_level_proc_info) << "PFR starting at DEFAULT Cycle => "  << cycle << ";" << " Phase -> 1" << endmsg;
         }


         // back to where we PFR'ed reset flag and carry on
         pd.MakeRunWritable();
         pd.Run().mssInPFR.Set(false);
         pd.MakeRunReadable();

      }
   }

   // send a message to gui to let it know we are entering the Vacuum purge states
   MSSVacPurgeMsg _updateGUI(MessageBase::SEND_GLOBAL);
   _updateGUI.send(0);

   return NORMAL;
}

int VacPurge::postExit ()
{

   ProcData pd;


   pauseTimer.interval(0);

   // stop the pumps
   pd.Orders().Qplasma (0.0f);
   pd.Orders().Qcollect(0.0f);

   cycle = 0;

   return NORMAL;
}


void VacPurge::copyOver (const VacPurge&)
{}

void VacPurge::cleanup ()
{}

/* FORMAT HASH 02fee264c349c13e43ea746e3399d757 */
