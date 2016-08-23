/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoveryabs.h
 *
 */

#ifndef _RECOVERYABS_H_
#define _RECOVERYABS_H_

#include "objdictionary.h"
#include "recoverytypes.h"

#include <list.h>
#include <stdlib.h>
#include <string>


class RecoveryAbs
{
public:
   //
   // Type of recovery...
   //   Stage1 -- Executes first
   //   Stage2 -- Possibly reacts after stage1
   //
   enum RecoveryType { Stage1Recovery, Stage2Recovery, NullStage };

   typedef enum { REGULAR, SCALED } CENT_SPEED_ALGORITHM;

public:

   //
   // Constructor
   RecoveryAbs();

   //
   // Copy constructor
   RecoveryAbs(const RecoveryAbs& rec);

   //
   // Destructor
   virtual ~RecoveryAbs();

   //
   // Initialize function -- Abstract
   //
   virtual int init () = 0;

   //
   // Assignment operator
   //
   RecoveryAbs& operator = (const RecoveryAbs& rec);

   //
   // Equality operator
   //
   int operator == (const RecoveryAbs& rec) const;
   int operator == (const string& name) const;
   int operator == (const char* name) const;
   //
   // Comparison operator
   //
   int operator < (const RecoveryAbs& rec) const;
   int operator < (const string& name) const;
   int operator < (const char* name) const;
   int operator < (RecoveryAbs& rec);
   int operator < (const string& name);
   int operator < (const char* name);

   //
   // Function which will decide if the passed in recovery
   //  can be transitioned to from this recovery
   //
   int canTransition (const RecoveryAbs* rec);

   //
   // Abstract function to control whether this recovery may be performed
   //  given the current system state and configuration
   //
   virtual int mayEnter () const = 0;

   //
   // Funtion which determines whether this recovery should
   //  be active or not based on the information provided to
   //  it
   //
   int recoveryNeeded ();

   //
   // Function which may be overridden if the recovery is an exception which
   //  is expected and needed during the run ...
   virtual RecoveryTypes::InRecovery recoveryExpected () { return RecoveryTypes::CountNone; }

   //
   // Abstract function which does the recovery, or at least a step of
   //  the recovery.  When the recovery is complete, the
   //  recoveryNeeded function will reflect the information.
   //
   virtual int doRecovery () = 0;

   //
   // Function to reset the recovery
   //
   virtual void reset () { }

   //
   // Set/Get for the recovery name
   //
   void        recoveryName (const string& n) { _RecoveryName = n; }
   void        recoveryName (const char* n) { _RecoveryName = n; }
   const char* recoveryName () const { return _RecoveryName.c_str(); }

   //
   // Set/Get the recovery type
   //
   void               recoveryType (const RecoveryType t) { _RecoveryType = t; }
   const RecoveryType recoveryType () const { return _RecoveryType; }

   //
   // Set/Get the queued up flag
   //
   void      queuedUp (const int f) { _QueuedUp = f; }
   const int queuedUp () const { return _QueuedUp; }

   //
   // Set/Get the active flag
   //
   void      active (const int f) { _Active = f; }
   const int active () const { return _Active; }

   //
   // Set/Get the recovery count flag
   //
   void               recoveryCountIndex (int f) { _RecoveryCountIndex = f; }
   const int          recoveryCount () const;
   const int operator ++ (int);

   //
   // Get transition list
   //
   list< RecoveryAbs* >& transitions () { return _TransitionsList; }

   //
   // Get possible queueable transition list
   //
   list< RecoveryAbs* >& queueableTransitions () { return _QueueableList; }

   //
   // Get possible stackable transition list
   //
   list< RecoveryAbs* >& stackableTransitions () { return _StackableList; }

   //
   // Get possible queue erasing transition list
   //
   list< RecoveryAbs* >& queueConsumingTransitions () { return _QueueConsumingList; }

   //
   // Get possible queue erasing transition list
   //
   list< RecoveryAbs* >& mutuallyExclusiveRecs () { return _MutuallyExclusiveRecsList; }

protected:

   //
   // Abstract function to control whether this recovery thinks it should
   //  be performed given the current system state and data
   //
   virtual int shouldRecover () = 0;

   virtual bool disabled () const;

   //
   // Function to compare this object with another
   //
   int compare (const RecoveryAbs& rec) const;
   int compare (const string& name) const;
   int compare (const char* name) const;

   //
   // Function to copy the dynamic data for the object
   //
   virtual void copyOver (const RecoveryAbs& rec);

   //
   // Function to delete the objects dynamic memory
   //
   void cleanup ();

   CENT_SPEED_ALGORITHM getCentAlgType () const;

protected:

   //
   // Character name of this recovery
   string _RecoveryName;

   //
   // Type of recovery ...
   //
   RecoveryType _RecoveryType;
   //
   // Flag to signify this recovery is in the queue and should
   //  be completed
   //
   int _QueuedUp;

   //
   // Flag to signify this recovery currently has control
   //
   int _Active;

   //
   // List of transitions that must occur after this recovery
   list< RecoveryAbs* > _TransitionsList;

   //
   // List of recoveries that are allowed after this recovery
   list< RecoveryAbs* > _QueueableList;

   //
   // List of recoveries that are allowed to pre-empt this recovery
   list< RecoveryAbs* > _StackableList;

   //
   // List of recoveries that remove this recovery from the queue
   list< RecoveryAbs* > _QueueConsumingList;

   //
   // List of recoveries that cannot be in the queue with this recovery
   list< RecoveryAbs* > _MutuallyExclusiveRecsList;

   //
   // Index into common data store recovery count buffer
   int _RecoveryCountIndex;

};

#endif

/* FORMAT HASH 7e5abcdf6e11a33459111e0bb3c4c93d */
