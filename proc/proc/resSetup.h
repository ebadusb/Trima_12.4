/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      ResSetup.h
 *
 *    This state will empty the resevior.
 */

/*
$Header: //bctquad3/home/BCT_Development/AccelPressThruFilter5.1/proc/proc/rcs/resSetup.h 1.5 2005/10/25 20:58:44Z jheiusb Exp $
$Log: resSetup.h $
Revision 1.5  2005/10/25 20:58:44Z  jheiusb
7202 -- initial PLT mss path
Revision 1.4  2005/10/12 21:32:03Z  jheiusb
7202 -- reservoir setup initial logic added
Revision 1.3  2005/10/07 22:06:55Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:40Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/
#ifndef RES_SETUP_H  //  Prevents multiple inclusions.
#define RES_SETUP_H

#include <stateabs.h>


// Class Declaration
class ResSetup
   : public StateAbs
{
   DECLARE_STATE(ResSetup);
public:

   // Default constructor
   ResSetup();

   // Copy constructor
   ResSetup(const ResSetup& state);

   // Default destructor
   virtual ~ResSetup();

   virtual StateAbs* clone () const { return ( new ResSetup(*this)); }

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
   virtual void copyOver (const ResSetup& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   bool  transitionNow;
   float initialRetVol;
   float initialInletVol;

};


#endif

/* FORMAT HASH 027eee1a85caddaa40f4f4437225a569 */
