/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 * TITLE:     fast_msg.hpp Trima task to manage fast data messages
 *             used by the control and safety drivers.
 *
 */

#include "fastdata.h"
#include "message.h"
#include "msginfo.hpp"

#ifndef _FAST_MSG_INCLUDE
#define _FAST_MSG_INCLUDE

// SPECIFICATION:    derived message class, fastdata with 10 or 20
//                   millisecond rate data for aps and pumps message.
//
// ERROR HANDLING:   none.

class fastdataMessage
{
public:
   fastdataMessage(const char* sharename);
   ~fastdataMessage();

   void update ();
   void activate ();
   void deactivate ();
   void toggle ();

private:
   FASTPAK*          _FPtr;
   bool              _msgInitialized;
   Message<FASTDATA> _msg;
};

// SPECIFICATION:    derived message class, fastdatalong with 10
//                   millisecond rate data for pump control loop data message.
//
// ERROR HANDLING:   none.

class fastdatalongMessage
{
public:
   fastdatalongMessage(const char* sharename);
   ~fastdatalongMessage();

   void update ();
   void activate ();
   void deactivate ();
   void toggle ();

private:
   FASTPAKLONG*          _FPtr;
   bool                  _msgInitialized;
   Message<FASTDATALONG> _msg;
};

#endif /* ifndef _FAST_MSG_INCLUDE */

/* FORMAT HASH ff8c002274114b947126ca60ea01b49a */
