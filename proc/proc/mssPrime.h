/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPrime.h
 *
 *     This state will prime the system with storage solution
 */
/*
$Header: H:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssPrime.h 1.3 2006/04/13 17:23:07Z jheiusb Exp jheiusb $
$Log: mssPrime.h $
Revision 1.3  2006/04/13 17:23:07Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.2  2006/03/03 17:33:05Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:34Z  jheiusb
Initial revision
Revision 1.5  2005/11/03 18:03:26Z  jheiusb
7202 -- state file cleanup
Revision 1.4  2005/11/03 17:04:33Z  jheiusb
7230 -- DRBC split alarm
Revision 1.3  2005/10/07 22:06:41Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:27Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/


#ifndef MSS_PRIME_H  //  Prevents multiple inclusions.
#define MSS_PRIME_H

#include <stateabs.h>

// Class Declaration
class MSSPrime
   : public StateAbs
{
   DECLARE_STATE(MSSPrime);
public:

   // Default constructor
   MSSPrime();

   // Copy constructor
   MSSPrime(const MSSPrime& state);

   // Default destructor
   virtual ~MSSPrime();

   virtual StateAbs* clone () const { return ( new MSSPrime(*this)); }

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
   virtual void copyOver (const MSSPrime& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   bool apsHigh;

};


#endif

/* FORMAT HASH d2d6a349df496ccad311a7bd92554d52 */
