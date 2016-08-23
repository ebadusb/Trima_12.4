/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acpressreturnline.h
 *
 */


#ifndef ACPRESSRETURNLINE_H  //  Prevents multiple inclusions.
#define ACPRESSRETURNLINE_H

#include "an_alarm.h"
#include "stateabs.h"


// Class Declaration
class ACPressReturnLine
   : public StateAbs
{
   DECLARE_STATE(ACPressReturnLine);
public:

   // Default constructor
   ACPressReturnLine();

   // Copy constructor
   ACPressReturnLine(const ACPressReturnLine& state);

   // Default destructor
   virtual ~ACPressReturnLine();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new ACPressReturnLine(*this)); }

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
   // Function called to command the pumps
   //
   virtual void startPumps ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const ACPressReturnLine& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Return pressure alarm
   //
   anAlarmMsg _PrimeFailureAlarm;

   //
   //  Return volume accumulated in this state ...
   //
   float _ReturnVolume;

};


#endif

/* FORMAT HASH 59417613c1454d9147e5b21d181f1f53 */
