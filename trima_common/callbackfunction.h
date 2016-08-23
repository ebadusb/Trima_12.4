/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      callbackfunction.h
 *             class for objects to give functions that receive
 *             stdmsg notifications.
 *
 ******************************************************************************/

#ifndef _CALLBACK_FUNCTION_H_
#define _CALLBACK_FUNCTION_H_

#include "callback.h"

class CallbackFunction
   : public CStdRcvr
{
public:
   CallbackFunction(Callback& cb)
      : _Callback(cb) { }

   virtual void StdMsgRcvd (int mid) { _Callback(); }
   virtual ~CallbackFunction() {}
}
};

#endif

/* FORMAT HASH 33e5830cc91ade13829e1d89b6dde84f */
