/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DisconnectPrompt.h
 *
 */


#ifndef DISCONNECTPROMPT_H  //  Prevents multiple inclusions.
#define DISCONNECTPROMPT_H

#include "stateabs.h"

class DisconnectPrompt
   : public StateAbs
{
   DECLARE_STATE(DisconnectPrompt);
public:

   // Default constructor
   DisconnectPrompt();

   // Default destructor
   virtual ~DisconnectPrompt();

protected:

   //
   // Functions to do this states processing ...
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

protected:

};


#endif

/* FORMAT HASH 2ca5d3ed6172d7fd21940f69874b3346 */
