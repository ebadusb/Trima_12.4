/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.h
 *
 */


#ifndef EVACUATEPLT_BAGS_H  //  Prevents multiple inclusions.
#define EVACUATEPLT_BAGS_H

#include "stateabs.h"
#include "an_alarm.h"


// Class Declaration
class PltBagEvac
   : public StateAbs
{
   DECLARE_STATE(PltBagEvac);
public:

   // Default constructor
   PltBagEvac();

   // Copy constructor
   PltBagEvac(const PltBagEvac& state);

   // Default destructor
   virtual ~PltBagEvac();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new PltBagEvac(*this)); }

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
   virtual void copyOver (const PltBagEvac& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();



protected:

   float myGAIN;
   float myDesiredAPS;
   float myEndQinSpeed;

   float _lastQin;
   float myQret;
   float myStartingQin;
   float _watchVin;

   float _minQin;

   float _minRunVolm;
   float _maxRunVolm;
   float _currentVin;

   bool  _transitionNow;
   bool  _closePltBagvlave;

   bool  _highPressModeFinished;
   bool  _highPressMode;

   float _currentVcoll, _currentVpls;

   float _totalVreturn;

   //
};


#endif

/* FORMAT HASH 0d00c67473ef3d312ba4974c1ccf95a8 */
