/*
 * Copyright (c) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      dispatcher.h, Message Dispatcher.
 *
 * ABSTRACT:   These classes support message routing. The dispatcher class 
 *             is used to send messages to the system router.   Only one 
 *             dispatcher should be constructed per task.   
 */

#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include "messagebase.h"
#include "messagepacket.h"
#include "messagesystemconstant.h"

class Dispatcher
{
public:

   Dispatcher( );
   virtual ~Dispatcher();

   //
   // Initialize the task's mqueue ...
   void init( const char *qname,
              unsigned int maxMessages,
              const bool block );

   //
   // Send a packet to the router ...
   void send( const MessagePacket &mp, 
              const int priority=MessageSystemConstant::DEFAULT_MESSAGE_PRIORITY );
   void sendTimerMessage( const MessagePacket &mp, 
                          const int priority=MessageSystemConstant::DEFAULT_MESSAGE_PRIORITY );

   //
   // receive messages from the router
   int dispatchMessages();          
   void stopLoop() { _StopLoop=true; }
   void allowLoop() { if ( _Blocking ) _StopLoop=false; }

   //
   // register the message
   void registerMessage( const MessageBase &mb, MessagePacket &mp );
   void registerMessage( const unsigned long messageId, const MessageBase &mb );
   virtual void deregisterMessage( const MessageBase &mb, MessagePacket &mp );
           void deregisterMessage( const unsigned long messageId, const MessageBase &mb );

   void dump( std::ostream &outs );

protected:


   //
   // Distribute the message received from the router ...
   virtual void processMessage( MessagePacket &mp );

   // 
   // Shutdown message queues ...
   void shutdown();

   //
   // Cleanup the message lists ...
   void cleanup();

private:
   Dispatcher( Dispatcher const &);             // not implemented
   Dispatcher& operator=(Dispatcher const&);    // not implemented

protected:


   //
   // Flag which contains which mode the queue was created ...
   bool _Blocking;
   //
   // Flag to free the dispatcher from the loop...
   bool _StopLoop;
};


#endif
