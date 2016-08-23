/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MssRbc_Monitor.h
 *
 */


#ifndef __MSS_RINSEBACK_VOLUME_MONITIOR_H_
#define __MSS_RINSEBACK_VOLUME_MONITIOR_H_

#include "monitorbase.h"
#include "an_alarm.h"
#include "objdictionary.h"

class MssRinsebackVolmMonitor
   : public MonitorBase
{
   DECLARE_OBJ(MssRinsebackVolmMonitor);
public:

   MssRinsebackVolmMonitor();
   virtual ~MssRinsebackVolmMonitor();



   // Monitor function, called AFTER hardware status is received, the values
   // are processed by the methods of the state to which the monitor is attached
   // in the "states" file and all of its substates (if any),
   // and no transition out of the state results.
   virtual void Monitor ();


   // Function to enable monitoring, called by the state machine AFTER the preEnter()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void enable ();

   // Function to disable monitoring, called by the state machine BEFORE the postExit()
   // method of the state to which the monitor is attached in the "states" file.
   virtual void disable ();

protected:

   void  checkVolume ();
   float estimateReservoirVolume ();
   void  testSuccessful ();
   void  setup ();


protected:
   float AccruedReturn_Vol;
   float AccruedInlet_Vol;
   bool  isSetup;

   float channelVolume;
   float rinsebackSuccessVolume;

   bool  doingSalineRB; // keep knowledge of which type of rinseback were doing
   bool  doingPlasmaRB;

   float startingResVolume; //



};

#endif

/* FORMAT HASH e1e53e77f434e0963ce9f6951e444fae */
