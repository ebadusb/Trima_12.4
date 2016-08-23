/*
 * Copyright (c) 1995-2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/sfastmsg.hpp 1.3 2003/06/19 19:45:24Z jl11312 Exp $
 * $Log: sfastmsg.hpp $
 * Revision 1.3  2003/06/19 19:45:24Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.2  2002/08/13 23:32:21Z  pn02526
 * Substitute #include of timermessage.h, and trimamessages.h
 *   for obsolete message system files.
 * Rather than deriving from a timer message base class,
 *   add loPrioTimer :: _myTimer
 *   to use the new timermessage class definitions.
 * Make loPrioTimer :: notify() a private method since it doesn't need to be public for the new message system.
 * Revision 1.1  2000/08/07 17:01:23  bs04481
 * Initial revision
 *
 * TITLE:      sfastmsg.hpp Trima task to manage optional and low-priority
 *             messages used by the safety driver.
 *
 * ABSTRACT:
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

#ifndef SFASTMSG_HPP
#define SFASTMSG_HPP

#include "timermessage.h"
#include "trimamessages.h"
#include "fastdata.h"
#include "msginfo.hpp"
#include "fast_msg.hpp"



// SPECIFICATION:    Timer class sends low priority messages.
//
// ERROR HANDLING:   none.

class loPrioTimer
{
public:
   loPrioTimer();
   ~loPrioTimer();
   void enableMessage (DATA_SOURCE src);
private:
   fastdatalongMessage* _fastinlethalltime;
   fastdatalongMessage* _fastachalltime;
   fastdatalongMessage* _fastplasmahalltime;
   fastdatalongMessage* _fastplatelethalltime;
   fastdatalongMessage* _fastreturnhalltime;
   fastdataMessage*     _fastusair;
   fastdataMessage*     _fastusfluid;
   TimerMessage         _myTimer;

   void notify ();
};


// SPECIFICATION:    fastDataControl message
//                   Receieves request for fast data types
//                   and enables or disables message flow
//
// ERROR HANDLING:   none.

class fastDataControl
{
public:
   fastDataControl(loPrioTimer* lo);
   ~fastDataControl();
private:
   loPrioTimer*      _lo;
   EnableFastDataMsg _enableFastDataMsg;

   void notify ();
};



#endif   // SFASTMSG_HPP

/* FORMAT HASH bc749f2c0f13001a0717063ef71833dc */
