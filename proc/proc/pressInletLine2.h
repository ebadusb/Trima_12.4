/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       pessInletLine2
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/negPressTest2.h 1.1 2007/02/20 22:10:55Z jheiusb Exp jheiusb $
$Log: negPressTest2.h $
*/

#ifndef PRESS_INLET_LINE_2_H  //  Prevents multiple inclusions.
#define PRESS_INLET_LINE_2_H



#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class PressInletLine2
   : public StateAbs
{
   DECLARE_STATE(PressInletLine2);
public:

   // Default constructor
   PressInletLine2();

   // Copy constructor
   PressInletLine2(const PressInletLine2& state);

   // Default destructor
   virtual ~PressInletLine2();

   virtual StateAbs* clone () const { return ( new PressInletLine2(*this)); }

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
   virtual void copyOver (const PressInletLine2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

private:

   anAlarmMsg _InletPressureAlarm;

   float      _ReturnVolume;
   bool       _TransitionNow;

};





#endif

/* FORMAT HASH eee615912d1986f6202a8bd879c9ac67 */
