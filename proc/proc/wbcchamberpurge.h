/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      wbcchamberpurge.h
 *
 */

#ifndef _WBCCHAMBERPURGE_H_
#define _WBCCHAMBERPURGE_H_

#include "recoveryabs.h"
#include "trimamessages.h"

class ProcData;
class WBCChamberPurge
   : public RecoveryAbs
{
   DECLARE_OBJ(WBCChamberPurge);
public:

   //
   // Constructor
   WBCChamberPurge();

   //
   // Copy constructor
   WBCChamberPurge(const WBCChamberPurge& rec);

   //
   // Destructor
   virtual ~WBCChamberPurge();

   //
   // Initialize function
   //
   virtual int init ();

   //
   // function which does the recovery, or at least a step of
   //  the recovery.  When the recovery is complete, the
   //  recoveryNeeded function will reflect the information.
   //
   virtual int doRecovery ();

   //
   // function to control whether this recovery may be performed
   //  given the current system state and data
   //
   virtual int mayEnter () const;

   //
   // Callback for the spillover request message ...
   //
   void recoveryRequest ();

   //
   // Function which may be overridden if the recovery is an exception which
   //  is expected and needed during the run ...
   virtual RecoveryTypes::InRecovery recoveryExpected ();


protected:

   //
   // Function which will control whether this recovery has detected
   //  that it is needed give the current system state
   //
   virtual int shouldRecover ();

   //
   // Function to perform the actual number of purge cycles specified
   //
   int performPurge (int numCycles);

   //
   // Function to process the given volume running only the inlet
   //  (pausing the plasma and collect pumps)
   //
   void pauseProcessing (float volume);

   //
   // Function which performs a purge of the LRS Chamber using
   //  the plasma pump for the specified volume
   //
   void purgeChamber (float volume);

   //
   // Function used to round the given double to an integer
   //
   int round (double v);

   //
   // Function used to reset the flags and volumes
   //
   virtual void reset ();

   //
   // Function to copy the dynamic data for the object
   //
   void copyOver (const WBCChamberPurge& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

protected:
   // Fuction to increment the step during recovery
   void IncrementStep (int step);

   //
   // Message for the spillover request
   RbcSpilloverRequestMsg _SpilloverRequest;

   //
   // Flag to specify the chamber has been packed
   int _ChamberPacked;

   //
   // Flag to specify the chamber has been purged
   int _ChamberPurged;

   //
   // Flag to specify the inlet volume has been processed
   int _InletProcessed;

   //
   // Holds the amount of inlet volume processed
   //  for a given period
   float _InletVolume;

   //
   // Holds the current purge volume trigger
   float _VPurgeTrigger;

   //
   // Counter to hold the number of cycles completed so far
   int _NumCyclesComplete;

   //
   // Counter to hold the number of cycles needed
   int _NumCyclesNeeded;

   //
   // Holds the amount of plasma volume processed
   //  for a given period
   float _PlasmaVolume;

   //
   // Flag to specify the purge has been completed
   int _PurgeComplete;

   //
   // Flag to specify that a recovery has been requested
   int _SpilloverRequested;
   int _RecoveryRequested;

   //
   // Flag to hold the fact that we completed the procedure
   //  specified purge per the volume processed
   float _StandardPurgeVolume;

   // Concentration specific variables Std/PPC
   float _QplasmaPurge;
   float _VplasmaPurgeCycle;
   float _VinChamberPack;
   float _VinClearChannel;
   bool  _resSetupDone;

   //
   // Flag to control initial timestamp when entering the purge
   int _PurgeStartTimeRecorded;

   //
   // Flag to indicate Purge suppression
   int _LogPurgeSuppressed;

   //
   // place holder for centrifuge rpm below 3000 or CobeConfig::data().RPMCollect
   // prior to purge, so that centrifuge can be commanded back to this value after
   // purging, IT13923
   //
   float _prePurgeRpm;

   // recovery step being performed
   int _recoveryStep;

};
#endif

/* FORMAT HASH dd3b95c2b7e338304f6f0dc6b75e9797 */
