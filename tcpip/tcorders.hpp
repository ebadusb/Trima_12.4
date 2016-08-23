/*******************************************************************************
 *
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      tcorders.hpp
 *             TED Control Hardware Orders Message (Internal)
 *
 * AUTHOR:     Shawn Price
 *
 * ABSTRACT:   This file defines Control orders message specialization.
 *
 *	This file provides a wrapper for the Control and Safety hardware
 *	orders sent out from TED.  In version 5.1, TED sent out unwrapped
 *	hw orders, which gooddog repeated every half second.  Since gooddog
 *	was listening for the same message it was sending out, there was a
 *	timing issue.  Gooddog was drinking from its own bathwater, so to
 *	speak.
 *
 *	Now, TED sends these wrapped HW orders, and the message system
 *	is able to distinguish them.  When gooddog recieves them, it
 *	caches a copy of the data, and then sends them out periodically
 *	in their unwrapped form so that the rest of the software can
 *	deal with them.  This eliminates the timing issue.
 *
 *	The original reason for adding the control wrapper was actually
 *	just for logging.  See Shawn's note below:
 *
 *                   Fix for IT7742: we now define specialized
 *                   CHwOrders structure message type to
 *                   distinquish TED hardware orders for Control
 *                   from the actual orders going to Control
 *                   driver.  This was done as part of the
 *                   effort to introduce HAL into Service Mode
 *                   operations with minimal effort, and without
 *                   having to deinterleave/decouple the message
 *                   traffic from the previous design.  Note that
 *                   TED maintains the responsibility of denying
 *                   hardware control during Normal Mode.
 **/

#ifndef TEDCONTROLORDERS_MSG_
#define TEDCONTROLORDERS_MSG_

#include "chw.hpp"
#include "message.h"

class TedControlOrdersMsg
   : public Message<CHwOrders>
{
public:
   TedControlOrdersMsg(){ }

   TedControlOrdersMsg(MessageBase::SendType t)
   { init(t); }

   TedControlOrdersMsg(const CallbackBase& cb,
                       MessageBase::SendReceiveType t)
   { init(cb, t); }

   virtual ~TedControlOrdersMsg() { }
};

class TedSafetyOrdersMsg
   : public Message<SHwOrders>
{
public:
   TedSafetyOrdersMsg(){ }

   TedSafetyOrdersMsg(MessageBase::SendType t)
   { init(t); }

   TedSafetyOrdersMsg(const CallbackBase& cb,
                      MessageBase::SendReceiveType t)
   { init(cb, t); }

   virtual ~TedSafetyOrdersMsg() { }
};

#endif  // TEDCONTROLORDERS_MSG_

/* FORMAT HASH b3e39e0291f58b66d17c8f1f5031fcd2 */
