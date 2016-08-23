/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      startprotocol.h
 *
 */


#ifndef STARTPROTOCOL_H  //  Prevents multiple inclusions.
#define STARTPROTOCOL_H

#include "stateabs.h"
#include "timermessage.h"
#include "trimamessages.h"


// Class Declaration
class StartProtocol
   : public StateAbs
{
   DECLARE_STATE(StartProtocol);
public:

   // Default constructor
   StartProtocol();

   // Copy constructor
   StartProtocol(const StartProtocol& state);

   // Default destructor
   virtual ~StartProtocol();

   //
   // Transition determination function ...
   //
   virtual int transitionStatus ();

   //
   // Functions which will receive the responsed
   //  from GUI and safety ...
   //
   void guiResponded ();
   void safetyResponded ();

   //
   // Functions which send a handshake request
   //  to our other subsystem processes ...
   //
   void sendGUIHandshake ();
   void sendSafetyHandshake ();

   //
   // Callback for non-responsive timer
   void systemNotResponding ();

   virtual StateAbs* clone () const { return ( new StartProtocol(*this)); }

protected:

   //
   // Function called upon first entrance to this state
   //
   virtual int preEnter ();

   //
   // Function called upon transition from this state
   //
   virtual int postExit ();

   //
   // Functions to do this states processing ...
   //
   virtual int preProcess ();
   virtual int postProcess ();

   //
   // Perform a deep copy of the items in this object
   //
   virtual void copyOver (const StartProtocol& s);

   //
   // Dispose of any dynamically allocated memory
   //
   virtual void cleanup ();

protected:

   //
   // Timers used to resend handshake ...
   //
   TimerMessage _GUIHandshakeTimer;
   TimerMessage _SafetyHandshakeTimer;

   //
   // Messages that signify a response and
   //  an exit out of this state
   //
   QueryGUIAliveMsg     _QueryGUIMessage;
   QuerySafetyAliveMsg  _QuerySafetyMessage;
   AnswerGUIAliveMsg    _GUIResponseMessage;
   AnswerSafetyAliveMsg _SafetyResponseMessage;

   //
   // Timer used to check the system to make sure it doesn't
   //  hang indefinitely
   TimerMessage _NonResponsiveTimer;

   //
   // Flags which store the handshake status
   //  from GUI and Safety ...
   //
   int _GUIResponded;
   int _SafetyResponded;
};


#endif

/* FORMAT HASH 40fff0897052aa3c8f7e01d54d61f661 */
