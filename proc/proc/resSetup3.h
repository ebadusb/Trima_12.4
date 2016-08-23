/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResSetup3.h
 *
 *    This state will empty the resevior.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/resSetup3.h 1.1 2007/06/04 20:06:29Z jheiusb Exp jheiusb $
$Log: resSetup3.h $
Revision 1.1  2007/06/04 20:06:29Z  jheiusb
Initial revision
*/
#ifndef RES_SETUP3_H  //  Prevents multiple inclusions.
#define RES_SETUP3_H

#include <stateabs.h>


// Class Declaration
class ResSetup3
   : public StateAbs
{
   DECLARE_STATE(ResSetup3);
public:

   // Default constructor
   ResSetup3();

   // Copy constructor
   ResSetup3(const ResSetup3& state);

   // Default destructor
   virtual ~ResSetup3();

   virtual StateAbs* clone () const { return ( new ResSetup3(*this)); }

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
   virtual void copyOver (const ResSetup3& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   bool  transitionNow;
   float initialRetVol;
   float initialInletVol;

   bool  holdPumpsDueToPressure;
};


#endif

/* FORMAT HASH a5be04da6ea47a1a97de9cba4b3cf6b7 */
