/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      procstate.h
 *
 ********************************************************************************/

#ifndef _PROCSTATE_H_
#define _PROCSTATE_H_

#include "states.h"

// string size including null
#define NAME_SIZE 36

class ProcState
{
public:

   //
   // Default constructor
   //
   ProcState();

   //
   // Default destructor
   //
   ~ProcState();

   //
   // Set/Get for the state name
   //
   void        stateName (const char* n);
   const char* stateName () const;

   //
   // Get the State_names enum for the current state
   //   ( it will be INVALID_STATE if undefined )
   //
   const State_names stateNum () const;

   //
   // Set/Get for the paused state
   //
   void      paused (int f);
   const int paused () const;

   //
   // Set/Get for the recovery state
   //
   void        recoveryState (const char* n);
   const char* recoveryState () const;

protected:

   //
   // Delete any dynamic memory
   //
   void cleanup ();

protected:

   //
   // Current state
   //
   char _StateName[ NAME_SIZE ];

   //
   // Current proc paused state
   //
   int _Paused;

   //
   // Current proc recovery state
   //
   char _RecoveryState[ NAME_SIZE ];

};

#endif

/* FORMAT HASH 597cd4c15fa6e8d0b89b7fc153627c97 */
