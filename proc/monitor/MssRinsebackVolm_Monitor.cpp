/*
 * Copyright (c) 2006 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MssRinsebackVolmMonitor.cpp
 *
 */

#include "MssRinsebackVolm_Monitor.h"
#include "cobeconfig.h"
#include "trima_datalog.h"
#include "procdata.h"


DEFINE_OBJ(MssRinsebackVolmMonitor);


MssRinsebackVolmMonitor::MssRinsebackVolmMonitor()
   : AccruedReturn_Vol(0.0f),
     AccruedInlet_Vol(0.0f),
     isSetup(false),
     channelVolume(0.0f),
     rinsebackSuccessVolume(0.0f),
     doingSalineRB(false),
     doingPlasmaRB(false),
     startingResVolume(0.0)
{}


MssRinsebackVolmMonitor::~MssRinsebackVolmMonitor()
{}


void MssRinsebackVolmMonitor::Monitor ()
{
   ProcData pd;

   setup();

   // watching the volumes
   AccruedReturn_Vol += pd.Status().ReturnPump.dV();
   AccruedInlet_Vol  += pd.Status().InletPump.dV();

   // for sure if we fininsh rinseback successfully !
//   if (pd.Run().RinsebackComplete.Get() != RINSEBACK_UNSUCCESSFUL)
//    {
//       testSuccessful(); // this ends the monitor
//    } // else
// otherwise the monitor expires at the end of the state
// without setting the MSS succesful rinseback CDS

   // keep checking until we finish state or pass
   if (pd.Run().rinsebackVolumeSuccessMss.Get() == false)
   {
      checkVolume();
   }
}

void MssRinsebackVolmMonitor::checkVolume ()
{

   ProcData pd;


   bool theCheck = false;

   if (pd.isSalineRinsebackActive() || doingSalineRB)
   {
      doingSalineRB = true;     // sticky bool to keep from race condition with states at end of rinseback
      theCheck      = (rinsebackSuccessVolume >=
                       (channelVolume + startingResVolume + CobeConfig::data().MssSalineRinsebackVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) );
   }
   else if (pd.isPlasmaRinsebackActive() || doingPlasmaRB)
   {
      doingPlasmaRB = true;  // sticky bool to keep from race condition with states at end of rinseback
      theCheck      = (rinsebackSuccessVolume >=
                       (channelVolume + startingResVolume + CobeConfig::data().MssPlasmaRinsebackVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) );
   }
   else
   {

      theCheck = (
         rinsebackSuccessVolume >= ( (channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) )
         );




   }

   if (theCheck)
   {



      DataLog(log_level_proc_info) << "Data:   "
                                   <<  " rinsebackSuccessVolume (cc)->" << rinsebackSuccessVolume
                                   <<  " startingResVolume      ->"    << startingResVolume
                                   <<  " channelVolume          ->"    << channelVolume
                                   <<  " AccruedReturn_Vol      ->"    << AccruedReturn_Vol
                                   <<  " AccruedInlet_Vol       ->"    << AccruedInlet_Vol
                                   << endmsg;

      DataLog(log_level_proc_info) << "Data2:   "
                                   <<  " (channelVolume + startingResVolume)       ->"    << (channelVolume + startingResVolume)
                                   <<  " (AccruedReturn_Vol - AccruedInlet_Vol)    ->"    << (AccruedReturn_Vol - AccruedInlet_Vol)
                                   <<  " (channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) ->" << ((channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) )
                                   <<  " rinsebackSuccessVolume >= etc...   ->"    << bool( rinsebackSuccessVolume >= ( (channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) ))
                                   << endmsg;

      testSuccessful(); // this ends the monitor
   }                    // else
   // otherwise the monitor expires at the end of the state
   // without setting the MSS succesful rinseback CDS

}

float MssRinsebackVolmMonitor::estimateReservoirVolume ()
{

   ProcData pd;

   float    dr  = pd.Cycle().VresDraw.Get();
   float    ret = pd.Cycle().VresReturn.Get(); // return is a negative number

   // so that Draw + return will get the last res volume before rinseback.
   float resVolm = (dr + ret);


   DataLog(log_level_proc_info) << "Called: Volume in the Res   -> "    << resVolm  << "ml " << endmsg;
   DataLog(log_level_proc_info) << "Called: Volume in the Res   dr-> "  << dr  << "ml " << endmsg;
   DataLog(log_level_proc_info) << "Called: Volume in the Res   ret-> " << ret  << "ml " << endmsg;
   return resVolm;

}

void MssRinsebackVolmMonitor::testSuccessful ()
{

   ProcData pd;

   // just for some future data gatherer
   if (pd.Run().rinsebackVolumeSuccessMss.Get() == false)  // just log it once!!!!
   {
      if (doingPlasmaRB)
      {

         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Plasma Rinseback) detects volume returned sufficient for nominal MSS! "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;
      }
      else if (doingSalineRB)
      {

         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Saline Rinseback) detects volume returned sufficient for nominal MSS! "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;
      }
      else
      {

         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Normal Rinseback) detects volume returned sufficient for nominal MSS! "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;

      }

   }

   pd.MakeRunWritable();
   // rinseback considered complete for MSS purposes.. not a slavage case.
   pd.Run().rinsebackVolumeSuccessMss.Set(true);      // the flag that is used by MSS
   pd.MakeRunReadable();

   enableMonitoring(0);  // disable monitor ... we's done!

}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void MssRinsebackVolmMonitor::enable ()
{
   MonitorBase::enableMonitoring(1);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void MssRinsebackVolmMonitor::disable ()
{

   ProcData pd;
//    if (doingPlasmaRB)
//    {
//      // DataLog(log_level_proc_info) << "End of Rinseback monitoring... Volume in the Res + Channel + PlasmaRB -> " << (channelVolume + startingResVolume + CobeConfig::data().MssPlasmaRinsebackVolume )  << "ml " << endmsg;
//      // DataLog(log_level_proc_info) << "End of Rinseback monitoring... rinseback cleared -> " << (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml " << endmsg;
//    }
//    else if (doingSalineRB)
//    {
//       DataLog(log_level_proc_info) << "End of Rinseback monitoring... Volume in the Res + Channel + SalineRB -> " << (channelVolume + startingResVolume + CobeConfig::data().MssSalineRinsebackVolume )  << "ml " << endmsg;
//       DataLog(log_level_proc_info) << "End of Rinseback monitoring... rinseback cleared -> " << (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml " << endmsg;
//    }
//    else
//    {
//
//       DataLog(log_level_proc_info) << "End of Rinseback monitoring... Volume in the Res + Channel -> " << (channelVolume + startingResVolume )  << "ml " << endmsg;
//       DataLog(log_level_proc_info) << "End of Rinseback monitoring... rinseback cleared -> " << (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml " << endmsg;
//
//    }

   if (!pd.Run().rinsebackVolumeSuccessMss.Get())
   {

      DataLog(log_level_proc_info) << "Data:   "
                                   <<  " rinsebackSuccessVolume (cc)->" << rinsebackSuccessVolume
                                   <<  " startingResVolume      ->"    << startingResVolume
                                   <<  " channelVolume          ->"    << channelVolume
                                   <<  " AccruedReturn_Vol      ->"    << AccruedReturn_Vol
                                   <<  " AccruedInlet_Vol       ->"    << AccruedInlet_Vol
                                   << endmsg;

      DataLog(log_level_proc_info) << "Data2:   "
                                   <<  " (channelVolume + startingResVolume)       ->"    << (channelVolume + startingResVolume)
                                   <<  " (AccruedReturn_Vol - AccruedInlet_Vol)    ->"    << (AccruedReturn_Vol - AccruedInlet_Vol)
                                   <<  " (channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) ->" << ((channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) )
                                   <<  " rinsebackSuccessVolume >= etc...   ->"    << bool( rinsebackSuccessVolume >= ( (channelVolume + startingResVolume) - (AccruedReturn_Vol - AccruedInlet_Vol) ))
                                   << endmsg;

      if (doingPlasmaRB)
      {

         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Plasma Rinseback) detects volume returned INSUFFICIENT for nominal MSS! "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;
      }
      else if (doingSalineRB)
      {

         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Saline Rinseback) detects volume returned INSUFFICIENT for nominal MSS! "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;
      }
      else
      {


         DataLog(log_level_proc_info) << "MssRinsebackVolmMonitor (Normal Rinseback) detects volume returned INSUFFICIENT for nominal MSS! must run Salvage MSS "
                                      <<  (AccruedReturn_Vol - AccruedInlet_Vol)  << "ml cleared" << endmsg;

      }
   }

   MonitorBase::enableMonitoring(0);
}

void MssRinsebackVolmMonitor::setup ()
{

   ProcData pd;

   if (!isSetup)
   {

      startingResVolume = estimateReservoirVolume();
      DataLog(log_level_proc_info) << "Volume in the Res at monitor start  -> " << startingResVolume << "ml " << endmsg;


      if (cassette::PLTPLSRBC == pd.TrimaSet().Cassette)
      {
         // white stamp setup
         channelVolume          =  CobeConfig::data().MssChannelVolume_w;
         rinsebackSuccessVolume =  CobeConfig::data().MssRinsebackSuccess_w;
      }
      else
      {
         // black stamp setup
         channelVolume          =  CobeConfig::data().MssChannelVolume_b;
         rinsebackSuccessVolume =  CobeConfig::data().MssRinsebackSuccess_b;
      }
      isSetup = true; // is it done;

      DataLog(log_level_proc_info) << "Volume in the Channel at start   -> " << channelVolume  << "ml " << endmsg;
   }

}

/* FORMAT HASH 450dc44743534da3161ece3819605ec0 */
