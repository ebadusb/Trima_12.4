/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      unloadcassette.cpp
 *
 */

#include "states.h"
#include "unloadcassette.h"
#include "guiproc.h"
#include "procdata.h"
#include "MssCommon.h"   // common MSS functions

#include "software_cds.h"     // Get Optional sw.dat settings


DEFINE_STATE(UnloadCassette);

UnloadCassette::UnloadCassette()
   : StateAbs(),
     _PendingAlarmMsg(Callback<UnloadCassette>(this, &UnloadCassette::setPendingAlarm)),
     _PendingAlarm(NULL_ALARM)
{}

UnloadCassette::UnloadCassette(const UnloadCassette& state)
   : StateAbs(state),
     _PendingAlarmMsg(Callback<UnloadCassette>(this, &UnloadCassette::setPendingAlarm)),
     _PendingAlarm(state._PendingAlarm)
{
   UnloadCassette::copyOver(state);
}

UnloadCassette::~UnloadCassette()
{
   UnloadCassette::cleanup();
}

int UnloadCassette::preProcess ()
{
   return NORMAL;
}

int UnloadCassette::postProcess ()
{
   return NORMAL;
}

int UnloadCassette::preEnter ()
{
   ProcData pd;
   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      pd.Orders().Qcollect(0.0f);
      pd.Orders().Qplasma(0.0f);

   }
   //
   // Log the current APS reading ...
   //
   DataLog(log_level_proc_unload_cassette_info) << "Begin unloading pumps: APS=" << pd.Status().APS() << endmsg;

   CassettePositionStatusMsg posmsg(MessageBase::SEND_LOCAL);
   posmsg.send(CASSETTE_RAISING);
   return NORMAL;
}

int UnloadCassette::postExit ()
{
   ProcData pd;
   if ( pd.TrimaSet().CassetteState.Get() == ProcTrimaSet_CDS::TESTED )
   {
      //
      // Set the system state to the STARTUP_TESTS
      //
      pd.SystemState(STARTUP_TESTS, false);

      pd.MakeTrimaSetWritable();
      pd.TrimaSet().CassetteState.Set(ProcTrimaSet_CDS::DRY);
      pd.MakeTrimaSetReadable();
      DataLog(log_level_proc_debug) << "SystemStateChange message sent " << (State_names)STARTUP_TESTS << endmsg;
   }
   else if ( pd.TrimaSet().CassetteState.Get() != ProcTrimaSet_CDS::DRY )
   {
      // If we're in the MSS Disconnect state, or we are configured for MSS,
      // Send the MSS Unload message.
      if ((pd.SystemState() == METERED_STORAGE_DISCONNECT) ||
          readyForPLTMSS() ||
          (
             readyForRBCMSS() &&
             (!RASBlockedDueToCPSalarms())
          )
          )
      {
         // send the unload message to GUI for confirmation
         MSSUnloadMsg unload(MessageBase::SEND_LOCAL);
         unload.send(0);
         DataLog(log_level_proc_info) << "Sent confirm MSS Complete message " << endmsg;
      }
      else
      {
         //
         // The donor cannot be connected because the cassette is raised ...
         //  Set donor disconnected ...
         //
         ConfirmDonorDisconnectedMsg donorDisconnected(MessageBase::SEND_GLOBAL);
         donorDisconnected.send(1);
         DataLog(log_level_proc_debug) << "Sent confirm donor disconnect message " << endmsg;
      }
   }

   //
   //   Tell everyone that we no longer have a valid ID
   //
   DisposableIDMsg type(MessageBase::SEND_GLOBAL);
   type.send(cassette::NOT_LOADED);
   DataLog(log_level_proc_debug) << "Sent DisposableID message of type NOT_LOADED " << endmsg;

   //
   // Set the current cassette id to be not loaded ...
   //
   pd.Orders().CassetteId(cassette::NOT_LOADED);
   pd.MakeTrimaSetWritable();
   pd.TrimaSet().Cassette.Set(cassette::NOT_LOADED);
   pd.MakeTrimaSetReadable();

   //
   // Send the gui handshake message.  This sends GUI from the sysclamp
   // screen back to the sysdisp screen.
   //
   DisposableRaisedMsg guiMsg(MessageBase::SEND_LOCAL);
   guiMsg.send(1);
   DataLog(log_level_proc_debug) << "GUI Message sent : DisposableRaised " << endmsg;

   //
   //
   //   This message sends GUI back to the sysdonor screen in case we were
   //   at the sysclamp screen.
   //
   UnsuccessfulCassetteLoweringMsg badLower(MessageBase::SEND_LOCAL);
   badLower.send(1);
   DataLog(log_level_proc_debug) << "Sent UnsuccessfulCassetteLowering message " << endmsg;

   //
   //
   //   Tell GUI what we're doing so it can update the status line
   //
   CassettePositionStatusMsg posmsg(MessageBase::SEND_LOCAL);
   posmsg.send(CASSETTE_RAISED);
   DataLog(log_level_proc_debug) << "Sent CassettePositionStatus message of type CASSETTE_RAISED " << endmsg;

   //
   // Reset the generic WBC alarm flag so it can be shown again if needed
   // IFF we're long past the rbc detector cal
   //
   if ( pd.SystemState() < DONOR_CONNECTED )
   {
      pd.MakeRunWritable();
      pd.Run().ShowedPltWbcContamAlarm.Set(false);
      pd.MakeRunReadable();
      DataLog(log_level_proc_info) << "Reset ShowedPltWbcContamAlarm flag" << endmsg;
   }

   //
   //
   //   Output requested alarm (if any)
   //
   if (_PendingAlarm != NULL_ALARM)
   {
      anAlarmMsg raiseAlarm(_PendingAlarm);
      raiseAlarm.setAlarm();
      _PendingAlarm = NULL_ALARM;

   }

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void UnloadCassette::copyOver (const UnloadCassette&)
{}

void UnloadCassette::cleanup ()
{}

void UnloadCassette::setPendingAlarm ()
{
   _PendingAlarm = (ALARM_VALUES)_PendingAlarmMsg.getData();
   DataLog(log_level_proc_unload_cassette_error) << "Pending cassette raise alarm " << _PendingAlarm << " received" << endmsg;
}

/* FORMAT HASH ee7e572e4b30d6463f7af3405864a0ad */
