/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      plasmarinseback.h
 *
 * ABSTRACT:   This state controls the trima actions during the blood rinseback
 *             phases of the run.
 *
 */


#ifndef _PLASMA_RINSEBACK_H_  //  Prevents multiple inclusions.
#define _PLASMA_RINSEBACK_H_

#include "stateabs.h"
#include "timekeeper.h"
#include "timermessage.h"

class PlasmaRinseback
   : public StateAbs
{
   DECLARE_STATE(PlasmaRinseback);
public:

   // Default constructor
   PlasmaRinseback();

   // Default destructor
   virtual ~PlasmaRinseback();

   //
   // Function to send of the state message
   //
   void rinsebackState ();

protected:

   //
   // Functions to do this state's processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Keep track of how long the plasma valve is open ...
   //
   void monitorPlasmaValve ();

   //
   // Callback when the plasma valve has been open too long ...
   //
   void volumeAllPlasma ();

   //
   // Calculate the values which will be used throughout plasma rinseback
   //
   void calculateMaxVolumeLimit ();
   void calculateRunningLimits ();
   void calculateVolumes ();

   void logRinsebackValues ();

protected:

   //
   // Declare data members here ...
   //
   TimerMessage _PeriodicStateMsg;
   TimerMessage _LogRinsebackValuesTimer;
   TimeKeeper   _PlasmaValveOpenAccumulator;

};


#endif

/* FORMAT HASH 46dd12a9ece2d9d82832548b70a0f104 */
