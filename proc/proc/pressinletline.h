/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pressinletline.h
 *
 */


#ifndef PRESSINLETLINE_H  //  Prevents multiple inclusions.
#define PRESSINLETLINE_H

#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class PressInletLine
   : public StateAbs
{
   DECLARE_STATE(PressInletLine);
public:

   // Default constructor
   PressInletLine();

   // Copy constructor
   PressInletLine(const PressInletLine& state);

   // Default destructor
   virtual ~PressInletLine();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new PressInletLine(*this)); }

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
   virtual void copyOver (const PressInletLine& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Failure to pressurize inlet line alarm
   //
   anAlarmMsg
      _InletPressureAlarm,
      _InletPressureAlarmPossibleWrongSet,
      _CassetteInvalidForTest;

   bool _unloadNow;
   //
   // Store the test's ac volume accrual
   //
   float _ACVolume;
   float _APSatStart;

   float _APSatHoldTestStart;

   //
   // Flag to keep the test status
   //
   int   _TestPassed;

   bool  _doDispTest_1;
   bool  _doDispTest_2;

   bool  _MSSRBCset;

   float _secondaryACVolume;
   bool  _runHoldTest;
   bool  _latched;

};


#endif

/* FORMAT HASH cd6ccb33fb8d58918ffc7dfc0a4511e5 */
