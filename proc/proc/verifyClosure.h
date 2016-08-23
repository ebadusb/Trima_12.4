/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      verifyClosure.h
 *
 *    This state will verify the AC line is sealed/clamped and that the channel
 *    lines are clamped.
 *
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/verifyClosure.h 1.2 2006/12/12 23:05:35Z jheiusb Exp jheiusb $
$Log: verifyClosure.h $
Revision 1.2  2006/12/12 23:05:35Z  jheiusb
7458 -- update GUI meter for PTF RBC metering
Revision 1.1  2006/11/20 22:49:05Z  jheiusb
Initial revision
Revision 1.6  2006/01/19 23:09:03Z  jheiusb
7355 -- rework clamp closure tests
Revision 1.5  2005/10/21 21:50:14Z  rm70006
IT 7202.  Fix AC alarm.
Revision 1.4  2005/10/21 19:03:01Z  rm70006
IT 7202.  Passed unit test.
Revision 1.3  2005/10/07 22:07:05Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:57Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#ifndef VERIFY_CLOSURE_H  //  Prevents multiple inclusions.
#define VERIFY_CLOSURE_H

#include <stateabs.h>

#include "ProcData.h"



// Class Declaration
class VerifyClosure
   : public StateAbs
{
   DECLARE_STATE(VerifyClosure);
public:

   // Default constructor
   VerifyClosure();

   // Copy constructor
   VerifyClosure(const VerifyClosure& state);

   // Default destructor
   virtual ~VerifyClosure();

   virtual StateAbs* clone () const { return ( new VerifyClosure(*this)); }

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


   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const VerifyClosure& s);

   //
   // Reset this state when it exits
   //
   virtual void reset ();

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   void AlertOrAlarm ();

   ProcData   _pd;

   int        _numberOfClosureAttempts;

   anAlarmMsg _fluidDetectedAlarm;
   anAlarmMsg _acClampNotClosedAlert;
   anAlarmMsg _acClampNotClosedAlarm;
   anAlarmMsg _acClampNotClosedAlarm_butContinuePAS;


   bool  _transitionNow;
   bool  _alarmTriggered;
   bool  _fluidAtSensor;
   bool  _pressureUp;
   bool  _inPumpDown;

   float _initialApsPressure;
   float _initialVac;
   float _initialVin;
   float _pumpDownInletVolm;





};
#endif

/* FORMAT HASH 421d67c600501fc9b793f321bd3b4831 */
