/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.h
 *
 */


#ifndef EVACUATEPLTPRESSRELIEF_BAGS_H  //  Prevents multiple inclusions.
#define EVACUATEPLTPRESSRELIEF_BAGS_H

#include "stateabs.h"
#include "an_alarm.h"


// Class Declaration
class PltBagEvacPressureRelief
   : public StateAbs
{
   DECLARE_STATE(PltBagEvacPressureRelief);
public:

   // Default constructor
   PltBagEvacPressureRelief();

   // Copy constructor
   PltBagEvacPressureRelief(const PltBagEvacPressureRelief& state);

   // Default destructor
   virtual ~PltBagEvacPressureRelief();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new PltBagEvacPressureRelief(*this)); }

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
   virtual void copyOver (const PltBagEvacPressureRelief& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();



protected:

   float _currentVcoll, _currentVpls;

   bool  _transitionNow;
};


#endif

/* FORMAT HASH c9ca7917dc2730b57c2771544c52ca5b */
