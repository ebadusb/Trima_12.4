/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      negativepressrelief.h
 *
 */


#ifndef NEG2NEGPRESSRELIEF__H  //  Prevents multiple inclusions.
#define NEG2NEGPRESSRELIEF__H

#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class NegativePressRelief
   : public StateAbs
{
   DECLARE_STATE(NegativePressRelief);
public:

   // Default constructor
   NegativePressRelief();

   // Copy constructor
   NegativePressRelief(const NegativePressRelief& state);

   // Default destructor
   virtual ~NegativePressRelief();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new NegativePressRelief(*this)); }

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
   virtual void copyOver (const NegativePressRelief& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   void checkDirection ();

protected:

   //
   // Negative pressure failed to relieve alarm
   //
   anAlarmMsg _NegativePressureReliefAlarm;

   //
   // Delta volume pumped through the RET pump
   //
   float _TestVolume;       // air2 dodnr mitigation uses this pump
   bool  _transition;
   float _direction;
   float _entryAPS;
   float _lastCheckAPS;
   float _slowingFactor;
};
#endif

/* FORMAT HASH c29db4cb2fe111d9abf8506145416292 */
