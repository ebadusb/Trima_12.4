/***************************************
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      InletVolmAlarm.cpp
 *
 **********************************************************************
 *  $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/alarm/rcs/inletVolmAlarm.cpp 1.11 2006/12/04 16:18:14Z jheiusb Exp jheiusb $
 *  $Log: inletVolmAlarm.cpp $
 *  Revision 1.11  2006/12/04 16:18:14Z  jheiusb
 *  remerge original 52 rbc mss
 *  Revision 1.10  2006/09/28 20:17:23Z  jheiusb
 *  7707 -- add rinseback monitor for MSS
 *  Revision 1.9  2006/09/26 20:47:46Z  jheiusb
 *  7747 -- skip inlet monitor if in salvage
 *  Revision 1.8  2006/09/11 18:45:44Z  jheiusb
 *  7747 -- inlet volm is 0.0ml for salvage case
 *  Revision 1.7  2006/07/21 16:04:09Z  jheiusb
 *  7405 -- remove CVR limits
 *  Revision 1.6  2006/05/19 19:03:01Z  jheiusb
 *  7549  -- fix cassette state in leeak detecter for MSS
 *  Revision 1.5  2006/05/12 20:49:36Z  jheiusb
 *  7272 - clean up inlet alarm
 *  Revision 1.4  2006/04/11 20:23:55Z  jheiusb
 *  7405 -- add inlet CRV buffer %
 *  Revision 1.3  2006/03/30 20:49:54Z  jheiusb
 *  7350 -- MSS PFR
 *  Revision 1.2  2006/03/03 17:27:56Z  jheiusb
 *  initial merge
 *  Revision 1.1  2006/03/03 17:12:57Z  jheiusb
 *  Revision 1.4  2006/01/11 22:29:18Z  jheiusb
 *  7320 -- fix logging statement
 *  Revision 1.3  2005/11/16 19:50:01Z  jheiusb
 *  7230 -- add  alarm processing to split RBC and PLT runs if one errors
 *  Revision 1.2  2005/11/03 22:58:38Z  jheiusb
 *  7230 -- inletVolmAlarm added
 *  Revision 1.1  2005/11/03 18:24:21Z  jheiusb
 *  Initial revision
 *
 *
 ***********************************************************************/

#include "inletVolmAlarm.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_OBJ(InletVolmAlarm);

InletVolmAlarm::InletVolmAlarm()
   : _inletVolmAlarm(INLET_VOLM_ALARM),
     _accumulated(0.0f),
     _alarmed(false),
     _cvl(0.0f),
     _cvr(0.0f),
     _setupDone(false)
{}

InletVolmAlarm::~InletVolmAlarm()
{}

void InletVolmAlarm::Monitor ()
{
   ProcData pd;

//     // skip if salvage case;
//     if ( !pd.Run().rinsebackVolumeSuccessMss )
//     {
//       return;
//     }

   setup();  // Is there a better way to initialize values in alarms.. like a preEnter ???

   _accumulated += pd.Status().InletPump.dV();

   // do the channel volm check....
   _cvr = _cvl - _accumulated;

   if ( (_cvr <  0.0f) && !_alarmed)
   {
      DataLog(log_level_proc_info) << "Inlet Volm Greater than CVR!" << endmsg;
      DataLog(log_level_proc_info) << "   Channel Volm Limit => " << _cvl << endmsg;
      DataLog(log_level_proc_info) << "   Channel CVR   => " << _cvr << endmsg;
      DataLog(log_level_proc_info) << "   InletPumpVolm => " << _accumulated  << endmsg;

      _alarmed = true;    // don't reenter the setAlarm func
      _inletVolmAlarm.setAlarm();
   }


}

void InletVolmAlarm::setup ()
{

   ProcData pd;

   // do this stuff once
   if (!_setupDone)
   {

      // get the Volm that channel check accrued
      _accumulated = pd.Run().channelInletVolm;

      // get the channel volm based on set type:
      if (cassette::PLTPLSRBC == pd.TrimaSet().Cassette)
      {

         // channel volm limit (cvl) for this set type
         _cvl = CobeConfig::data().MssCvlWhite;      // ml

      }
      // black stamp:
      else if (cassette::RBCPLS == pd.TrimaSet().Cassette)
      {
         // channel volm limit (cvl) for this set type
         _cvl =  CobeConfig::data().MssCvlBlack;      // ml

      }
      else
      {

         // Error case do something!
         DataLog(log_level_proc_info) << "ERROR: cassette type unknown for channel check!! setting CVR to -1.0" << endmsg;
         _cvl = -1.0f;     // this will fail right away
      }

      // salvage case 10337 -- we dont check but there needs to be some upper sanity limit
      if ( !pd.Run().rinsebackVolumeSuccessMss )
      {
         _cvl =  CobeConfig::data().MssCvlSalvage;      // ml;
      }


      DataLog(log_level_proc_info) << "the Channel Volm Limit is  " << _cvl << endmsg;

   }
   _setupDone = true;
}





// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void InletVolmAlarm::enable ()
{
   MonitorBase::enableMonitoring(1);
}

// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void InletVolmAlarm::disable ()
{

   DataLog(log_level_proc_info) << "InletVolmAlarm processed ->   " << _accumulated << "mL" << endmsg;
   MonitorBase::enableMonitoring(0);
}

/* FORMAT HASH 3ace6ad4c6cfdfb0ac267cabcff70a3f */
