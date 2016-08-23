/*
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      calibverification.h
 *
 * ABSTRACT:   This header file defines the global data and classes for the
 *                  Startup Tests - Calibration verification.
 *
 */


#ifndef CALIBVERIFICATION_H  //  Prevents multiple compilations.
#define CALIBVERIFICATION_H

#include <stateabs.h>

class CalibVerification
   : public StateAbs
{
   DECLARE_STATE(CalibVerification);
public:

   // Constructor
   CalibVerification();

   // Destructor
   virtual ~CalibVerification();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Create a duplicate of myself
   //
   virtual StateAbs* clone () const { return ( new CalibVerification(*this)); }

protected:

   //
   // Main processing function
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Function called once upon initial entrance to the state
   //
   virtual int preEnter ();

   //
   // Reset the state
   //
   virtual void reset ();

   //
   // Check for (approximate) equality of two float values
   //
   virtual int compareFloatEqual (double val1, double val2);

protected:

   //
   // Flags to show whether sensors have been calibrated
   //
   int _validAPSCal;
   int _validCPSCal;
   int _validRBCCal;
};

#endif // ifndef CALIBVERIFICATION_H

/* FORMAT HASH bfb7f437a59dfa341e18cb25e71e9e95 */
