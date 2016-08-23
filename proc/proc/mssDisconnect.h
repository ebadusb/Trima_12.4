/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssDisconnect.h
 *
 *   This state allows a pause to processing in proc. while the operator disconects and clamps
 *   the appropriate lines.
 *   This state will wait for a the message MSSAckCompletionMsg (via the state table) inorder to
 *   transition to UnloadCassette substate.
 *
 */

/*
$Header: //bctquad3/home/BCT_Development/Trima5.1x/Trima/proc/proc/rcs/mssDisconnect.h 1.1 2006/03/03 17:16:44Z jheiusb Exp jheiusb $
$Log: mssDisconnect.h $
Revision 1.1  2006/03/03 17:16:44Z  jheiusb
Initial revision
Revision 1.3  2005/10/07 22:07:29Z  jheiusb
7202 ADD END RUN LOCIG AND SOME CLEANUP
Revision 1.2  2005/10/05 20:35:20Z  jheiusb
7202 -- initial flow of MSS proc-GUI model stubs

*/

#ifndef MSS_DISCONNECT_H  //  Prevents multiple inclusions.
#define MSS_DISCONNECT_H

#include <stateabs.h>


// Class Declaration
class MSSDisconnect
   : public StateAbs
{
   DECLARE_STATE(MSSDisconnect);
public:

   // Default constructor
   MSSDisconnect();

   // Copy constructor
   MSSDisconnect(const MSSDisconnect& state);

   // Default destructor
   virtual ~MSSDisconnect();

   virtual StateAbs* clone () const { return ( new MSSDisconnect(*this)); }

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
   virtual void copyOver (const MSSDisconnect& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();




};





#endif

/* FORMAT HASH 9d5580f446d6a423c1c2ac4133eac359 */
