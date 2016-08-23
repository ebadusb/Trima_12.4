/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EvacuateBags.h
 *
 */


#ifndef EVACUATEBAGS_H  //  Prevents multiple inclusions.
#define EVACUATEBAGS_H

#include "stateabs.h"
#include "an_alarm.h"


// Class Declaration
class EvacuateBags
   : public StateAbs
{
   DECLARE_STATE(EvacuateBags);
public:

   // Default constructor
   EvacuateBags();

   // Copy constructor
   EvacuateBags(const EvacuateBags& state);

   // Default destructor
   virtual ~EvacuateBags();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   virtual StateAbs* clone () const { return ( new EvacuateBags(*this)); }

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

protected:

   bool reducingPressure (void);
   void setAlarm (void);

   //
   // Failure to pressurize inlet line alarm
   //
   anAlarmMsg _PressureAlarm;
   anAlarmMsg _PressureAlarmJapBmp;
   anAlarmMsg _PressureAlarmLast;
   //
   // Store the test's ac volume accrual
   //
   float _ReturnVolume;
   float _APSatStart;
   float _ReturnVolumeReducePressure;


   //
   // Flag to keep the test status
   //
   int _TestPassed;
   int _NumberofAlarms;
   int _BailOut;


   bool _doPressureAndAlarm;

};


#endif

/* FORMAT HASH 6a386ae14b32a42a35b26bc2e5ebefc3 */
