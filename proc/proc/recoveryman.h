/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoveryman.h
 *
 */

#ifndef _RECOVERYMAN_H_
#define _RECOVERYMAN_H_

#include "recoveries.h"
#include "recoveryabs.h"

#include <list.h>

#ifdef WIN32
using namespace std;
#endif

class RecoveryMan
{
public:

   //
   // Function used to determine if a specific recovery is available
   static int recoveryAvailable (const char* rec);
   static int recoveryAvailable (const RecoveryTypes::Type rec);

   //
   // Function used to determine if the system is currently considered to be in
   //  a recovery state
   static RecoveryTypes::InRecovery inRecovery ();

   //
   // Function used to return the current recovery
   static RecoveryTypes::Type current ();

private:

   static RecoveryMan*        _This;

   static RecoveryTypes::Type _Stage1Recoveries[];
   static RecoveryTypes::Type _Stage2Recoveries[];
   static RecoveryTransition  _Transitions[];
   static RecoveryTransition  _QueueableTransitions[];
   static RecoveryTransition  _StackableTransitions[];
   static RecoveryTransition  _QueueConsumingTransitions[];
   static RecoveryTransition  _MutuallyExclusiveRecoveries[];

public:

   //
   // Default constructor
   RecoveryMan();

   //
   // Destructor
   virtual ~RecoveryMan();

   //
   // Initialization function
   int init ();
   //
   // Function called to find out if a recovery is needed,
   //  determine which recovery should be active, and perform
   //  the action needed to do the recovery
   //
   int doRecovery ();

   //
   // Returns the recovery at the head of the queue, or the
   //  currently executing recovery of the specified stage type
   //  ( Default to NullStage )
   //
   RecoveryAbs* currentRecovery (const RecoveryAbs::RecoveryType type = RecoveryAbs::NullStage);

   //
   // Function which returns the string id of the current
   //  recovery.  If no current recovery -- 0 is given.
   //
   const char* currentState ();

   //
   // Function to delete any dynamic memory
   //
   void cleanup ();

protected:

   //
   // Function to iterate through the recovery list and order
   //  the needed recoveries
   //
   void processRecoveries (const RecoveryAbs::RecoveryType type);

   //
   // Function called to add the substate to the queue in the
   //  correct order using the protocal lists
   //
   int addRecovery (RecoveryAbs* rec);

   //
   // Functions which add substates to the transition queue
   //  based on priority order
   //
   void addRecoveryAsQueue (RecoveryAbs* rec);
   void addRecoveryAsStack (RecoveryAbs* rec);
   void clearQueueThenAddRecovery (RecoveryAbs* rec);

   //
   // Function to remove a substate from the queue
   //
   void removeRecoveryFromQueue (RecoveryAbs* rec);

   //
   // Function which returns true if the recovery
   //  is in the queue
   //
   int recoveryInQueue (RecoveryAbs* rec);

   //
   // Functions to manage the queue
   //
   int  addRecoveryToQueue (RecoveryAbs* rec);
   int  addRecoveryToStack (RecoveryAbs* rec);
   void removeRecovery (RecoveryAbs* rec);
   void clearQueue ();

   //
   // Functions to initialize the transition lists
   //
   int initRecoveries ();
   int initTransitionList ();
   int initQueueableTransList ();
   int initStackableTransList ();
   int initQueueConsumingTransList ();
   int initMutuallyExclusiveRecsList ();

   //
   // Functions to output the lists
   //
   void dump ();
   void dumpQueue ();

protected:

   //
   // List which owns all recoveries used by the system
   list< RecoveryAbs* > _Recoveries;

   //
   // List which maintains the current recovery queue
   list< RecoveryAbs* > _RecoveryQueue;

   //
   // Current recovery pointer ( used as a history )
   RecoveryAbs* _CurrentRecovery[ RecoveryAbs::NullStage ];

   //
   // Keep track of my previously reported recovery type ...
   //
   RecoveryTypes::Type _ReportedRecovery;
};

#endif

/* FORMAT HASH 7b517b00e2b03079c72ab01a344f8864 */
