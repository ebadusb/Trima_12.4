/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      msgdefs.h, Common message header.
 *
 * ABSTRACT:   This file defines the message interface for the
 *             Common dispatcher, and router.
 */

#ifndef _MSGDEFS_H_                     
#define _MSGDEFS_H_

#include <iostream>
#include <sys/types.h>
#include <string>
#include <ctime>

#include "datalog.h"
#include "messagesystemconstant.h"
#include "msgcrc.h"


class MessageData
{
public:
   
   enum OperationType
   {
      TASK_REGISTER,                            // register task with router
      TASK_DEREGISTER,                          // deregister task with router
      MESSAGE_NAME_REGISTER,                    // register message name with router
      MESSAGE_REGISTER,                         // register message with router
      MESSAGE_DEREGISTER,                       // deregister message with router
      GATEWAY_CONNECT,                          // register a gateway task
      GATEWAY_DISCONNECT,                       // deregister a gateway task
      SPOOF_MSG_REGISTER,                       // register the spoofer task
      SPOOF_MSG_DEREGISTER,                     // deregister the spoofer task

      DISTRIBUTE_GLOBALLY,                      // distribute the message to everyone
                                                //  who registered 
      DISTRIBUTE_LOCALLY,                       // distribute the message to everyone
                                                //  who registered on the local node
      SPOOFED_GLOBALLY,                         // spoofer has modified message for everyone
      SPOOFED_LOCALLY,                          // spoofer has modified message for everyone 
                                                //  on the local node

      TIME_UPDATE,                              // time update from the ISR
   };

public:

   MessageData() { memset( this, 0, sizeof( MessageData ) ); }
   MessageData( const MessageData &d ) { operator=( d ); }
   ~MessageData() {}

   MessageData &operator=( const MessageData &d )
   {
      if ( &d != this )
      {
         osCode(       d.osCode() );
         msgId(        d.msgId() );
         msgLength(    d.msgLength() );
         nodeId(       d.nodeId() );
         taskId(       d.taskId() );
         seqNum(       d.seqNum() );
         totalNum(     d.totalNum() );
         packetLength( d.packetLength() );
         msg(          d.msg(), MessageSystemConstant::MAX_MESSAGE_SIZE );
      }
      return *this;
   }

   bool operator==( const MessageData &d ) const
   {
      if (    msgId()        == d.msgId() 
           && msgLength()    == d.msgLength() 
           && seqNum()       == d.seqNum()
           && totalNum()     == d.totalNum()
           && packetLength() == d.packetLength()
         )
      {
         return true;
      }
      return false;
   }

   //
   // Set/Get for the OS Code instruction
   void osCode( const OperationType v ) ;
   const OperationType osCode() const ;

   //
   // Set/Get for the message Id
   void msgId( const unsigned long v ) ;
   const unsigned long msgId() const ;
   //
   // Set/Get for the message Length
   void msgLength( const unsigned short v ) ;
   const unsigned short msgLength() const ;
   //
   // Set/Get for the node Id
   void nodeId( const unsigned long v ) ;
   const unsigned long nodeId() const ;

   //
   // Set/Get for the task Id
   void taskId( const unsigned long v ) ;
   const unsigned long taskId() const ;

   //
   // Set/Get for the sent time of the message packet
   void sendTime( const struct timespec v ) ;
   const struct timespec sendTime() const;

   //
   // Set/Get for this packet sequence of the total number
   void seqNum( const unsigned short v );
   const unsigned short seqNum() const ;

   //
   // Set/Get for the total number of message packets 
   void totalNum( const unsigned short v ) ;
   const unsigned short totalNum() const;
   
   //
   // Set/Get for the packet length
   void packetLength( const unsigned short v ) ;
   const unsigned short packetLength() const ;

   //
   // Set/Get for the message data ...
   //
   void msg( const unsigned char * v, const int length ) 
   {
       UNUSED_ARGUMENT(v); UNUSED_ARGUMENT(length);
   }
   const unsigned char *msg() const { return 0;} 

   //
   // Dump the contents of the class ...
   void dump( std::ostream &outs )
   {
       UNUSED_ARGUMENT(outs);
   }

protected:

};


class MessagePacket
{
public:

   MessagePacket() : _MessageData(), _CRC( 0 ), _Unopened( false ) { }
   MessagePacket( const MessagePacket &mp ) { operator=( mp ); }
   ~MessagePacket() { }

   MessagePacket &operator=( const MessagePacket &d )
   {
      if ( &d != this )
      {
         _MessageData = d._MessageData;
         _CRC = d._CRC;
         _Unopened = d._Unopened;
      }
      return *this;
   }

   bool operator==( const MessagePacket &d ) const
   {
      return ( _MessageData == d._MessageData );
   }

   //
   // Get/Set the message data ...
   MessageData &msgData() { return _MessageData; }
   const MessageData &msgData() const { return _MessageData; }
   void msgData( const MessageData &md ) { _MessageData = md; }

   //
   // Generate the crc for the message
   void updateCRC()
   {
      _CRC = msgcrc32( (unsigned char *) &_MessageData, sizeof( MessageData) );
   }

   //
   // Validate the crc for the message
   bool validCRC() const
   {
      if ( _CRC != msgcrc32( (unsigned char *) &_MessageData, sizeof( MessageData) ) )
      {
         //
         // The crc's do not match, return an error ...
         return false;
      }
      return true;
   }

   //
   // Set/Get for the CRC
   void crc( const unsigned long v ) { _CRC = v; } 
   const unsigned long crc() const { return _CRC; } 

   //
   // Set/Get for the unopened flag 
   //   ( meaning the packet hasn't been read since its been received )
   void unopened( const bool v ) { _Unopened = v; } 
   const bool unopened() const { return _Unopened; } 

   //
   // Dump the contents of the class ...
   void dump( std::ostream &outs )
   {
      outs << "^^^^^^^^^^^^^^^^^^^^^^^^ Message Packet ^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endmsg;
      _MessageData.dump( outs );
      outs << "CRC: " << std::hex << _CRC << std::dec << " Unopened: " << _Unopened << endmsg;
      outs << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endmsg;
   }

protected:

   MessageData   _MessageData;   // Actual message information
   unsigned long _CRC;           // 32 bit CRC of message information
   bool          _Unopened;      // flag to signify the packet is newly received

};

#endif
