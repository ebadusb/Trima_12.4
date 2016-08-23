/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      guistarted.h
 *
 */


#ifndef GUISTARTED_H  //  Prevents multiple inclusions.
#define GUISTARTED_H

#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"


// Class Declaration
class GUIStarted
   : public StateAbs
{
   DECLARE_STATE(GUIStarted);
public:

   // Default constructor
   GUIStarted();

   // Copy constructor
   GUIStarted(const GUIStarted& state);

   // Default destructor
   virtual ~GUIStarted();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Functions which will receive the responsed
   //  from GUI ...
   //
   void guiResponded ();

   //
   // Functions which send a handshake request
   //  to our other subsystem processes ...
   //
   void sendGUIHandshake ();

   virtual StateAbs* clone () const { return ( new GUIStarted(*this)); }

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
   virtual void copyOver (const GUIStarted& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timers used to resend handshake ...
   //
   TimerMessage _GUIHandshakeTimer;

   //
   // Messages that signify a response and
   //  an exit out of this state
   //
   ConfirmEnableLoadSystemMsg _GUIResponseMessage;

   //
   // Flags which store the handshake status
   //  from GUI ...
   //
   int _GUIResponded;
};


#endif

/* FORMAT HASH f15d7de269e832abf65c5b300aa727c6 */
