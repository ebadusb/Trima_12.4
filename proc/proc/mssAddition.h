/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssAddition.h
 *
 *    This state will deliver the SS to the products.
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssAddition.h 1.3 2006/04/13 17:22:38Z jheiusb Exp jheiusb $
$Log: mssAddition.h $
Revision 1.3  2006/04/13 17:22:38Z  jheiusb
7484 -- APS checking for Prime and addition enabled
Revision 1.2  2006/03/03 17:31:22Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:22Z  jheiusb
Initial revision
Revision 1.5  2005/11/13 19:35:39Z  jheiusb
7230 -- more state file cleanup
Revision 1.4  2005/10/25 20:57:37Z  jheiusb
7202 -- initial PLT mss path
Revision 1.3  2005/10/07 22:06:20Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:05Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#ifndef MSS_ADDITION_H  //  Prevents multiple inclusions.
#define MSS_ADDITION_H

#include <stateabs.h>

// Class Declaration
class MSSAddition
   : public StateAbs
{
   DECLARE_STATE(MSSAddition);
public:

   // Default constructor
   MSSAddition();

   // Copy constructor
   MSSAddition(const MSSAddition& state);

   // Default destructor
   virtual ~MSSAddition();

   virtual StateAbs* clone () const { return ( new MSSAddition(*this)); }

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
   virtual void copyOver (const MSSAddition& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:



};


#endif

/* FORMAT HASH c84657a331a54529054355950f6164b1 */
