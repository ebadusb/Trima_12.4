/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      postrun.h
 *
 */


#ifndef POSTRUN_H  //  Prevents multiple inclusions.
#define POSTRUN_H

#include <stateabs.h>

// Class Declaration
class PostRun
   : public StateAbs
{
   DECLARE_STATE(PostRun);
public:

   // Default constructor
   PostRun();

   // Copy constructor
   PostRun(const PostRun& state);

   // Default destructor
   virtual ~PostRun();

   virtual StateAbs* clone () const { return ( new PostRun(*this)); }

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

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   //
   // State specific data members
   //
};


#endif

/* FORMAT HASH ae0fac5322f0c72063f23aba26a336c4 */
