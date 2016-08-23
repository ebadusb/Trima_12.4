/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      DatagramMessageObject
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the a generic datagram message object used
 *             to send datagram messages to an external client (Vista or EMS).
 *
 * HISTORY:    $Log: datagram_message_obj.h $
 * HISTORY:    Revision 1.10  2004/01/19 22:05:23Z  rm70006
 * HISTORY:    IT 6725.  Added extra logging for issue.
 * HISTORY:    Revision 1.9  2004/01/12 20:41:49Z  rm70006
 * HISTORY:    IT 6725.  Fix failed message counting scheme.
 * HISTORY:    Revision 1.8  2003/09/08 17:34:53Z  rm70006
 * HISTORY:    IT 6281.  Add setAddr call to set the sending address of a message.
 * HISTORY:    Revision 1.7  2003/07/01 15:57:41Z  rm70006
 * HISTORY:    IT 5830.  Remove use of sockstream classes.
 * HISTORY:    Revision 1.6  2003/05/16 13:33:14Z  rm70006
 * HISTORY:    IT 5830.
 * HISTORY:
 * HISTORY:    Add debug code.
 * HISTORY:    Revision 1.5  2003/04/09 20:04:03Z  ms10234
 * HISTORY:    removed log_level_critical
 * HISTORY:    Revision 1.4  2003/04/07 21:42:49Z  rm70006
 * HISTORY:    IT 5818.
 * HISTORY:
 * HISTORY:    Change log levels.
 * HISTORY:    Revision 1.3  2003/02/04 18:08:18Z  rm70006
 * HISTORY:    Add debug logic
 * HISTORY:    Revision 1.2  2003/01/02 22:32:02Z  rm70006
 * HISTORY:    Fill in some of the holes.
 * HISTORY:    Revision 1.1  2002/12/20 21:47:12Z  rm70006
 * HISTORY:    Initial revision
 *
 *******************************************************************************/



#ifndef __DATAGRAM_MESSAGE_OBJECT__
#define __DATAGRAM_MESSAGE_OBJECT__

#include <unistd.h>
#include <sockLib.h>

#include "datalog.h"
#include "trima_datalog.h"

#include "sockinet.h"
#include "netShow.h"




//
// DatagramMessageObject
// This helper class encapsulates standard things you want to do with a datagram message.
// You can prepare it for delivery and send it.
// This class assumes all messages have a message header and are sent using UDP to a
// knonw client(non-broadcast).
//
template <class MSGTYPE, class MSGIDTYPE> class DatagramMessageObject
{
public:
   DatagramMessageObject(MSGTYPE& obj, const sockinetaddr& addr, DataLog_Level& debug, DataLog_Level& error)
      : _obj(obj), _sock(socket(AF_INET, SOCK_DGRAM, 0)), _addr(addr), _prepared(false), _debug(debug), _error(error), _failedMsgSends(0) { }

   virtual ~DatagramMessageObject() { close (_sock); }
   // Prepare message for sending (must be defined by encapsulating protocol)
   virtual void prepareMsg (MSGIDTYPE messageId) = 0;

   virtual void setAddr (const sockinetaddr& addr) { _addr = addr; }

   // Send prepared message.
   virtual void Send (void)
   {
      if (_prepared)
      {
         const int sentSize = sendto(_sock, (char*)&_obj, MSG_SIZE, 0, _addr.addr(), _addr.size());

         if ( (sentSize != MSG_SIZE) && (_failedMsgSends < 3) )
         {
            DataLog(_error) << "Msg Failed to send to " << _addr << " (" << sentSize
                            << "/" << MSG_SIZE << ").  Count = " << _failedMsgSends++ << ".  Errno = "
                            << errnoMsg << endmsg;

         }
         else
         {
            DataLog(_debug) << "Sent " << MSG_SIZE << " bytes to client " << _addr << "." << endmsg;
         }
      }
      else
      {
         DataLog(_error) << "Msg not prepared for " << _addr << endmsg;
      }
   }

   // Send unprepared message.
   virtual void Send (MSGIDTYPE messageId)
   {
      prepareMsg (messageId);

      Send();
   }

protected:
   MSGTYPE&     _obj;

   sockinetaddr _addr;

   int          _sock;
   bool         _prepared;

   enum { MSG_SIZE = sizeof(MSGTYPE)};

   DataLog_Level& _debug;
   DataLog_Level& _error;

   int            _failedMsgSends;

private:

   DatagramMessageObject() {}

};


#endif

/* FORMAT HASH e7f72de04abe649d4c773fe25478c30e */
