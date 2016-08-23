/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      procstate.h
 *
 ********************************************************************************/
#include <vxworks.h>

#include "procstate.h"
#include <string.h>
#include "substatemap.h"

ProcState::ProcState()
   : _Paused(0)
{
   memset(_StateName,     0, NAME_SIZE);
   memset(_RecoveryState, 0, NAME_SIZE);
}

ProcState::~ProcState()
{
   cleanup();
}

void ProcState::stateName (const char* n)
{
   strncpy(_StateName, n, NAME_SIZE);
}

const char* ProcState::stateName () const
{
   return _StateName;
}

const State_names ProcState::stateNum () const
{
   return SubstateMap::ss(_StateName);
}

void ProcState::paused (int f)
{
   _Paused = f;
}

const int ProcState::paused () const
{
   return _Paused;
}

void ProcState::recoveryState (const char* n)
{
   if ( n )
   {
      strncpy(_RecoveryState, n, NAME_SIZE);
   }
   else
   {
      strncpy(_RecoveryState, "None", NAME_SIZE);
   }
}

const char* ProcState::recoveryState () const
{
   return _RecoveryState;
}

void ProcState::cleanup ()
{}

/* FORMAT HASH 329b037d3fc8363442945b1b26854678 */
