/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSPLTPrime2.h
 *
 *     This state will prime the system with storage solution
 */
/*
$Header: //BCTquad3/home/BCT_Development/Trima5.1x/Trima/proc/proc/rcs/mssPLTPrime2.h 1.2 2006/03/03 17:32:34Z jheiusb Exp jheiusb $
$Log: mssPLTPrime2.h $
Revision 1.2  2006/03/03 17:32:34Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:13Z  jheiusb
Initial revision
Revision 1.2  2005/10/25 20:58:21Z  jheiusb
7202 -- initial PLT mss path
Revision 1.1  2005/10/24 18:32:18Z  jheiusb
Initial revision
Revision 1.1  2005/10/24 17:32:14Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:41Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:27Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/


#ifndef MSS_PLTPRIME_2_H  //  Prevents multiple inclusions.
#define MSS_PLTPRIME_2_H

#include <stateabs.h>


// Class Declaration
class MSSPLTPrime2
   : public StateAbs
{
   DECLARE_STATE(MSSPLTPrime2);
public:

   // Default constructor
   MSSPLTPrime2();

   // Copy constructor
   MSSPLTPrime2(const MSSPLTPrime2& state);

   // Default destructor
   virtual ~MSSPLTPrime2();

   virtual StateAbs* clone () const { return ( new MSSPLTPrime2(*this)); }

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
   virtual void copyOver (const MSSPLTPrime2& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:
   bool transitionNow;
   bool holdPumpsDueToPressure;


   float initPlasmaPumpVolume;
};


#endif

/* FORMAT HASH 1e68687c84b816e7a77bd8a79134a1df */
