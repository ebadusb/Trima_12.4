/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPLT.h
 *
 *     This state will prime the system with storage solution
 */
/*
$Header: $
$Log:  $
*/


#ifndef MSS_PLT__H  //  Prevents multiple inclusions.
#define MSS_PLT__H

#include <stateabs.h>

// Class Declaration
class MSSPLT
   : public StateAbs
{
   DECLARE_STATE(MSSPLT);
public:

   // Default constructor
   MSSPLT();

   // Copy constructor
   MSSPLT(const MSSPLT& state);

   // Default destructor
   virtual ~MSSPLT();

   virtual StateAbs* clone () const { return ( new MSSPLT(*this)); }

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
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const MSSPLT& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:


};


#endif

/* FORMAT HASH a66604336c50ad8035a27e76d287da62 */
