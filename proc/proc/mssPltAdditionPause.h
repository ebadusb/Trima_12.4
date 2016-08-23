/*
 * Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      mssPltAdditionPause.h
 *
 *
 */

/*
$Header $
$Log: mssPltAdditionPause.h $
Revision 1.3  2007/01/17 22:08:35Z  jheiusb
7835 -- add MSS PLT spillover code to this baseline
Revision 1.2  2006/03/03 17:32:57Z  jheiusb
initial merge
Revision 1.1  2006/03/03 17:16:59Z  jheiusb
Initial revision
Revision 1.2  2005/11/04 21:03:13Z  jheiusb
7202 -- add pause at the end of PLT prime
Revision 1.1  2005/11/03 23:22:55Z  jheiusb
Initial revision

*/


#ifndef MSS_PLT_ADDITION_PAUSE_H  //  Prevents multiple inclusions.
#define MSS_PLT_ADDITION_PAUSE_H

#include <stateabs.h>

#include "trimamessages.h"


// Class Declaration
class MSSPltAdditionPause
   : public StateAbs
{
   DECLARE_STATE(MSSPltAdditionPause);
public:

   // Default constructor
   MSSPltAdditionPause();

   // Copy constructor
   MSSPltAdditionPause(const MSSPltAdditionPause& state);

   // Default destructor
   virtual ~MSSPltAdditionPause();

   virtual StateAbs* clone () const { return ( new MSSPltAdditionPause(*this)); }

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
   virtual void copyOver (const MSSPltAdditionPause& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

   virtual void opSawSplitNotif ();

   MSSReadyPltAdditionMsg _splitNotifAcknowledgement;

protected:

   bool _opSawSplitNotif;
   bool _transitionNow;
   int  _waitCounter;


};


#endif

/* FORMAT HASH e29d34780f81ddeb36ea6c0245fb8128 */
