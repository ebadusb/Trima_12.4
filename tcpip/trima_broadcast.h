/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      trima_broadcast.h
 *             Trima Broadcast Message Class
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the Trima Broadcast Message function.  This is a stand-alone
 *             process that will turn on and off the external broadcast message when commanded.
 *
 ******************************************************************************/

#ifndef INCLUDE_TRIMABROADCAST
#define INCLUDE_TRIMABROADCAST

#include "messagesystem.h"     // Get message system stuff
#include "timermessage.h"
#include "trimamessages.h"

#include "sockinet.h"          // Get socket lib defns.
#include "tedif.h"             // Get external broadcast message defns.
#include "xif_cds.h"           // Gets External Interface CDS.

class TrimaBroadcast
{
// Class Methods
public:
   TrimaBroadcast();

   TrimaBroadcast(MessageSystem* ms);
   virtual ~TrimaBroadcast();

   // Initialize class
   void Initialize (void);

   // Public command interface
   void StartBroadcast (void);
   void StopBroadcast (void);

   int MessageCount (void) { return _messageCount; }

   void PackageMessage (TED_BroadcastMsg& msg);


// Class Methods
private:
   // Message Handlers
   void SendExternalBroadcast (void);



// Member data
private:
   External_IF_CDS   _xifCds;

   TedIf             _tedif;

   int               _messageCount;

   BroadcastStartMsg _broadcastStart;
   BroadcastStopMsg  _broadcastStop;
   TimerMessage      _udpBroadcastTimer;

   sockbuf           _broadcastSocket;
   sockinetaddr      _externalListener;

   sockbuf           _vistaDirectSendSocket;
   sockinetaddr      _vistaDirectSendAddress;

   TED_BroadcastMsg  _broadcastMsg;

   MessageSystem*    _ms;

   bool              _vistaDirectSendAddressConfigured;

   enum { BROADCAST_INTERVAL       = 1000}; // 1 second
   enum { MAX_NUMBER_OF_BROADCASTS = 10};
};


#endif

/* FORMAT HASH 2cb47c4093c76d22078deb6d4619a02a */
