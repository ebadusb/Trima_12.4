/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      \
 *
 */


#ifndef MSSPARALLELPROC_H  //  Prevents multiple inclusions.
#define MSSPARALLELPROC_H

#include <stateabs.h>
#include "mssPLT.h"
#include "mssAddition.h"

// Class Declaration
class MSSParallelProcessing
   : public StateAbs
{
   DECLARE_STATE(MSSParallelProcessing);
public:

   // Default constructor
   MSSParallelProcessing();

   // Default destructor
   virtual ~MSSParallelProcessing();

   //
   // Function to initialize the parallel, CentrifugeTests, state.  This
   //  is overridden from the stateabs class which must be called after
   //  this function does its processing.
   //
   virtual int init ();

   virtual const char* currentState () const;

protected:

   bool alreadySetAasStartTime;

   virtual int transitionStatus ();
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
   bool        transitionNow;

   MSSPLT      _MSSPrimeState;
   int         _MSSPrimeStatus;

   MSSAddition _MSSAdditionState;
   int         _MSSAdditionStatus;
};


#endif

/* FORMAT HASH 187040b342c46f51982e7d66d47f886e */
