/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      lowernotification.h
 *
 */


#ifndef _LOWERNOTIFICATION_H_  //  Prevents multiple inclusions.
#define _LOWERNOTIFICATION_H_

#include <stateabs.h>


// Class Declaration
class LowerNotification
   : public StateAbs
{
   DECLARE_STATE(LowerNotification);
public:

   // Default constructor
   LowerNotification();

   // Copy constructor
   LowerNotification(const LowerNotification& state);

   // Default destructor
   virtual ~LowerNotification();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new LowerNotification(*this)); }

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
   virtual void copyOver (const LowerNotification& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Flag to say the state is complete
   //
   int _NotificationSent;
};


#endif

/* FORMAT HASH 13858a77a900805a15e30014f04a3921 */
