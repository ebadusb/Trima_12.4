/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      Rinseback.h
 *
 * ABSTRACT:   Description of the actions and purpose of this state ...
 *
 * To use this state, add the class name in the 'states' file.  All functionality
 *  needed to make this state complete should be coded into the substate.  The transitions
 *  in the states file should only be used for system specification of where to go
 *  next ...
 *
 */


#ifndef RINSEBACK_H  //  Prevents multiple inclusions.
#define RINSEBACK_H

#include "stateabs.h"

class Rinseback
   : public StateAbs
{
   DECLARE_STATE(Rinseback);
public:

   // Default constructor
   Rinseback();

   // Copy constructor
   Rinseback(const Rinseback& state);

   // Default destructor
   virtual ~Rinseback();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   // Clone this state ...
   virtual StateAbs* clone () const { return ( new Rinseback(*this)); }

protected:

   //
   // Functions to do this state's processing ...
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
   virtual void copyOver (const Rinseback& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Declare data members here ...
   //
   float _MaxVreturn;
   int   _evacuating;
   float _evacuationLimit;
   float _evacuationMinVolume;
   float _valveVolume;
   float _Qreturn;
   float _QreturnMax;
   float _VacTotal;
   float _VTotal;
   float _IRMax;

};


#endif

/* FORMAT HASH 7e77dcb67bd1a130110679fcb1dc17e4 */
