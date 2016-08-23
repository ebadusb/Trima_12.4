/*
 *
 * TITLE:      showClampForARM.h
 *
 *     This state will prime the system with storage solution
 */


#ifndef showClampForARM_H  //  Prevents multiple inclusions.
#define showClampForARM_H

#include "trimamessages.h"
#include <stateabs.h>

// Class Declaration
class ShowClampARM
   : public StateAbs
{
   DECLARE_STATE(ShowClampARM);
public:

   // Default constructor
   ShowClampARM();

   // Copy constructor
   ShowClampARM(const ShowClampARM& state);

   // Default destructor
   virtual ~ShowClampARM();

   virtual StateAbs* clone () const { return ( new ShowClampARM(*this)); }

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
   virtual void copyOver (const ShowClampARM& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:



};


#endif

/* FORMAT HASH 016883a9c1bd483163b577474bdc8d9a */
