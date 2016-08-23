/*******************************************************************************
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      tedif.h
 *             Trima External Diagnostic Interface
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface between Trima and EMS.
 *
 *
 *******************************************************************************/



#ifndef __VIP_STATUS_MSG__
#define __VIP_STATUS_MSG__

#include "message.h"

#include "datagram_message_obj.h"


//
//
//
class SequenceNumber
{
public:
   SequenceNumber() {}
   virtual ~SequenceNumber(){}

protected:
   static int _sequenceNumber;
};

int SequenceNumber::_sequenceNumber = 0;




//
// VipMessageObj
// This helper class encapsulates standard things you want to do with a status message to Vista.
// This class derives from the DatagramMessageObject
//
template <class MSGTYPE> class VipMessageObj
   : public DatagramMessageObject<MSGTYPE, int>, public SequenceNumber
{
public:
   VipMessageObj(MSGTYPE& obj, const sockinetaddr& addr)
      : DatagramMessageObject<MSGTYPE, int>(obj, addr, log_level_vip_debug, log_level_vip_error) {}

   virtual ~VipMessageObj() {}

   // Prepare status message for sending to Vista
   virtual void prepareMsg (int messageId)
   {
      _vipif.prepareMsg(&_obj, messageId, ++_sequenceNumber, 0, MSG_SIZE);
      _prepared = true;
   }

   static void reset () { _sequenceNumber = 0; }

private:
   VipIf _vipif;
};



//
// VipOneToOneStatusMsg
// This helper class encapsulates the necessary steps to receive an internal Trima message,
// format it, and send out a datagram message to an external client using UDP.
//
template <class CLASS, class TRIMA_IN_MSG, class VISTA_OUT_MSG> class VipOneToOneStatusMsg
{
public:
   typedef bool (CLASS::* FillFuncType)(const TRIMA_IN_MSG&, VISTA_OUT_MSG&);

// Methods
public:
   VipOneToOneStatusMsg(const sockinetaddr& addr, int msgId, CLASS* me, FillFuncType fn)
      : _trimaMsgObj (Callback<VipOneToOneStatusMsg>(this, &VipOneToOneStatusMsg::receive), MessageBase::SNDRCV_RECEIVE_ONLY),
        _vistaMsgObj (_vistaOutMsg, addr),
        _fn(fn),
        _msgId(msgId),
        _class(me)
   {}

   virtual ~VipOneToOneStatusMsg() {}

   void deregisterMsg (void) { _trimaMsgObj.deregisterMsg(); }

   void setAddr (const sockinetaddr& addr) { _vistaMsgObj.setAddr(addr); }

// Methods
private:
   void receive ()
   {
      // Receive the message
      const TRIMA_IN_MSG trimaMsgIn = _trimaMsgObj.getData();

      // fill in the message
      const bool sendMe = (_class->*_fn)(trimaMsgIn, _vistaOutMsg);

      if (sendMe)
      {
         // Send the message
         _vistaMsgObj.Send(_msgId);
      }
   }

// Data Members
private:
   const int                    _msgId;
   const FillFuncType           _fn;

   Message<TRIMA_IN_MSG>        _trimaMsgObj;
   VipMessageObj<VISTA_OUT_MSG> _vistaMsgObj;
   VISTA_OUT_MSG                _vistaOutMsg;
   CLASS* _class;
};



//
// VipOneToOneIntStatusMsg
// This helper class encapsulates the necessary steps to receive an internal Trima message,
// format it, and send out a datagram message to an external client using UDP.
//
template <class CLASS, class TRIMA_INT_MSG, class VISTA_OUT_MSG> class VipOneToOneIntStatusMsg
{
public:
   typedef bool (CLASS::* FillFuncType)(const int&, VISTA_OUT_MSG&);

// Methods
public:
   VipOneToOneIntStatusMsg(const sockinetaddr& addr, int msgId, CLASS* me, FillFuncType fn)
      : _trimaMsgObj (Callback<VipOneToOneIntStatusMsg>(this, &VipOneToOneIntStatusMsg::receive), MessageBase::SNDRCV_RECEIVE_ONLY),
        _vistaMsgObj (_vistaOutMsg, addr),
        _fn(fn),
        _msgId(msgId),
        _class(me)
   {}

   ~VipOneToOneIntStatusMsg() {}

   void deregisterMsg (void) { _trimaMsgObj.deregisterMsg(); }

   void setAddr (const sockinetaddr& addr) { _vistaMsgObj.setAddr(addr); }

// Methods
private:
   void receive ()
   {
      // Receive the message
      const int trimaMsgIn = _trimaMsgObj.getData();

      // fill in the message
      const bool sendMe = (_class->*_fn)(trimaMsgIn, _vistaOutMsg);

      if (sendMe)
      {
         // Send the message
         _vistaMsgObj.Send(_msgId);
      }
   }

// Data Members
private:
   const int                    _msgId;
   const FillFuncType           _fn;

   TRIMA_INT_MSG                _trimaMsgObj;
   VipMessageObj<VISTA_OUT_MSG> _vistaMsgObj;
   VISTA_OUT_MSG                _vistaOutMsg;
   CLASS*                       _class;
};



//
// VipOneToOneWrapperedStatusMsg
// This helper class encapsulates the necessary steps to receive an internal Trima message,
// format it, and send out a datagram message to an external client using UDP.
//
template <class CLASS, class TRIMA_IN_WRAPPER, class TRIMA_IN_MSG, class VISTA_OUT_MSG> class VipOneToOneWrappedStatusMsg
{
public:
   typedef bool (CLASS::* FillFuncType)(const TRIMA_IN_MSG&, VISTA_OUT_MSG&);

// Methods
public:
   VipOneToOneWrappedStatusMsg(const sockinetaddr& addr, int msgId, CLASS* me, FillFuncType fn)
      : _trimaMsgObj (Callback<VipOneToOneWrappedStatusMsg>(this, &VipOneToOneWrappedStatusMsg::receive), MessageBase::SNDRCV_RECEIVE_ONLY),
        _vistaMsgObj (_vistaOutMsg, addr),
        _fn(fn),
        _msgId(msgId),
        _class(me)
   {}

   ~VipOneToOneWrappedStatusMsg() {}

   void deregisterMsg (void) { _trimaMsgObj.deregisterMsg(); }

   void setAddr (const sockinetaddr& addr) { _vistaMsgObj.setAddr(addr); }

// Methods
private:
   void receive ()
   {
      // Receive the message
      const TRIMA_IN_MSG trimaMsgIn = _trimaMsgObj.getData();

      // fill in the message
      const bool sendMe = (_class->*_fn)(trimaMsgIn, _vistaOutMsg);

      if (sendMe)
      {
         // Send the message
         _vistaMsgObj.Send(_msgId);
      }
   }

// Data Members
private:
   const int                    _msgId;
   const FillFuncType           _fn;

   TRIMA_IN_WRAPPER             _trimaMsgObj;
   VipMessageObj<VISTA_OUT_MSG> _vistaMsgObj;
   VISTA_OUT_MSG                _vistaOutMsg;
   CLASS* _class;
};

#endif

/* FORMAT HASH 1661a935bfb316add16b6d4536966c49 */
