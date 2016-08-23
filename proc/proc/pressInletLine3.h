/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:       pressInletLine3
 *
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/pressInletLine3.h 1.1 2007/02/20 22:10:55Z jheiusb Exp jheiusb $
$Log: pressInletLine3.h $
*/

#ifndef PRESSUREINLET_LINE_3_H  //  Prevents multiple inclusions.
#define PRESSUREINLET_LINE_3_H

#include <an_alarm.h>
#include <stateabs.h>


// Class Declaration
class PressInletLine3
   : public StateAbs
{
   DECLARE_STATE(PressInletLine3);
public:

   // Default constructor
   PressInletLine3();

   // Copy constructor
   PressInletLine3(const PressInletLine3& state);

   // Default destructor
   virtual ~PressInletLine3();

   virtual StateAbs* clone () const { return ( new PressInletLine3(*this)); }

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
   virtual void copyOver (const PressInletLine3& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

private:


   anAlarmMsg _PressureLineAlarm;

   float      _ACVolume;
   bool       _TransitionNow;


};





#endif

/* FORMAT HASH 79e974b5ca7b9c182e458429ba77a70b */
