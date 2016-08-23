/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      MSSSetup.h
 *
 *   This state allows a pause to processing in proc. while the operator disconects and clamps
 *   the appropriate lines.
 *   This state will wait for a the message MSSSetupMsg (via the state table) inorder to
 *   transition to the next metered storage state (ChannelCheck).
 */

/*
$Header: E:/BCT_Development/Trima5.R/Trima/proc/proc/rcs/mssSetup.h 1.3 2006/06/05 20:57:57Z jheiusb Exp dslausb $
$Log: mssSetup.h $
Revision 1.3  2006/06/05 20:57:57Z  jheiusb
7350 -- distinguish between the two MSS setup screens in PFR
Revision 1.2  2006/03/03 17:33:12Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:17:41Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:06:48Z  jheiusb
7202 - ADD END RUN LOGIC AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:34Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#ifndef MSS_SETUP_H  //  Prevents multiple inclusions.
#define MSS_SETUP_H

#include <stateabs.h>
#include "trimamessages.h"

// Class Declaration
class MSSSetup
   : public StateAbs
{
   DECLARE_STATE(MSSSetup);
public:

   // Default constructor
   MSSSetup();

   // Copy constructor
   MSSSetup(const MSSSetup& state);

   // Default destructor
   virtual ~MSSSetup();

   virtual StateAbs* clone () const { return ( new MSSSetup(*this)); }

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

   // check if its time for this state to transition (or not)
   virtual int transitionStatus ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const MSSSetup& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

};



#endif

/* FORMAT HASH 9d56dc13a65580a7a9a30a9c44218439 */
