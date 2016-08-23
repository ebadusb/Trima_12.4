/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResSetup2.h
 *
 *    This state will empty the resevior.
 */

/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/resSetup2.h 1.2 2006/09/11 21:55:47Z jheiusb Exp jheiusb $
$Log: resSetup2.h $
Revision 1.2  2006/09/11 21:55:47Z  jheiusb
7730 -- add second ressetup to insure empty at start of MSSPrime.
Revision 1.1  2006/09/07 20:03:22Z  jheiusb
Initial revision
Revision 1.1  2006/03/03 17:17:49Z  jheiusb
Initial revision
Revision 1.5  2005/10/25 20:58:44Z  jheiusb
7202 -- initial PLT mss path
Revision 1.4  2005/10/12 21:32:03Z  jheiusb
7202 -- reservoir setup initial logic added
Revision 1.3  2005/10/07 22:06:55Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:40Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/
#ifndef RES_SETUP2_H  //  Prevents multiple inclusions.
#define RES_SETUP2_H

#include <stateabs.h>


// Class Declaration
class ResSetup2
   : public StateAbs
{
   DECLARE_STATE(ResSetup2);
public:

   // Default constructor
   ResSetup2();

   // Copy constructor
   ResSetup2(const ResSetup2& state);

   // Default destructor
   virtual ~ResSetup2();

   virtual StateAbs* clone () const { return ( new ResSetup2(*this)); }

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
   virtual void copyOver (const ResSetup2& s);

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

/* FORMAT HASH 784dc65ce58d9a94a1337f1fc56bd13e */
