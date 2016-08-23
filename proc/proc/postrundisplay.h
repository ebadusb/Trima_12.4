/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      postrundisplay.h
 *
 */


#ifndef POSTRUNDISPLAY_H  //  Prevents multiple inclusions.
#define POSTRUNDISPLAY_H

#include "trimamessages.h"
#include "runsummary.h"
#include "stateabs.h"


// Class Declaration
class PostRunDisplay
   : public StateAbs
{
   DECLARE_STATE(PostRunDisplay);
public:

   // Default constructor
   PostRunDisplay();

   // Copy constructor
   PostRunDisplay(const PostRunDisplay& state);

   // Default destructor
   virtual ~PostRunDisplay();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Callback function for the send status request message
   //
   void sendStats ();

   //
   // Callback function for the finished viewing message
   //
   void finished ();

   virtual StateAbs* clone () const { return ( new PostRunDisplay(*this)); }

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
   virtual void copyOver (const PostRunDisplay& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();


protected:

   //
   // Finished viewing stats message
   //
   FinishedViewingStatsMsg _FinishedMsg;

   //
   // Flag to signify we are finished
   //
   int _Finished;
};

#endif // ifndef POSTRUNDISPLAY_H

/* FORMAT HASH 52af52adb4f7403a7992e3e8524f6747 */
