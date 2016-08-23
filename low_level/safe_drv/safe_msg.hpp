/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/safe_drv/rcs/safe_msg.hpp 1.12 2003/06/25 17:09:20Z jl11312 Exp jl11312 $
 * $Log: safe_msg.hpp $
 * Revision 1.12  2003/06/25 17:09:20Z  jl11312
 * - added logging for safety side CPU idle time and memory availability
 * Revision 1.11  2003/06/19 19:45:17Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.10  2002/12/26 22:49:56Z  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.9  2002/12/17 09:53:08  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.8  2002/10/31 18:28:26Z  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.7  2002/08/13 17:09:36  pn02526
 * Substitute #include of message.h, timermessage.h, and trimamessages.h
 *   for obsolete message system files.
 * Substitute #include of ostime.h for obsolete kernaltime.h.
 * Rather than deriving from a buffered message base class:
 * + add commands :: _theOrderMsg,
 * + add statusMessage :: _theStatusMsg,
 * + add basinDataMessage :: _theBasinTempMsg,
 *   to use the new buffered message class definitions.
 * Rather than deriving from an INT32 message base class,
 *   add requestStatusMessage :: _theRequestStatusMsg,
 *   to use the new trimamessages class definitions.
 * Remove _fd_* members, since they are not used by the shared memory emulation.
 * Make loPrioTimer :: notify() a private method since it doesn't need to be public for the new message system.
 * Make hiPrioTimer :: notify() a private method since it doesn't need to be public for the new message system.
 * Rather than deriving from a timer message base class,
 *   add loPrioTimer :: _myTimer
 *   add hiPrioTimer :: _myTimer
 *   to use the new timermessage class definitions.
 * Make commands :: notify() a private method since it doesn't need to be public for the new message system.
 * Change requestStatusMessage :: _timeTeller from a pointer to a kTime object
 *   to a pointer to an osTime object.
 * Make requestStatusMessage :: notify() a private method since it doesn't need to be public for the new message system.
 * TEMPORARILY comment out the timeJumpControl class until it is seen whether it is necessary under vxWorks.
 * Revision 1.6  2000/11/01 12:10:55  bs04481
 * Change fatal error for requestSafetyStatus message to 2.19 sec.
 * Log latencies greather than 1 sec.
 * Revision 1.5  2000/11/01 18:59:44  bs04481
 * Fatal error if the delivery latency for RequestSafetyStatus
 * exceeds 2 sec.  Log latencies above 500ms.
 * Revision 1.4  2000/10/27 14:35:43  bs04481
 * Measure delivery time for request safety status messages.  Log
 * any occurrences over 250ms.  For the time being, do not fatal
 * error on excessive delivery times.  Use the kenel based time
 * dispatcher to avoid sensitivity to setting the real-time clock.
 * Revision 1.3  2000/08/07 23:00:43  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.2  1999/10/28 20:29:29  BS04481
 * Code review change.  Previous design disabled soft watchdogs
 * while the clock was being set.  This was unacceptable.  This code
 * changes the soft watchdogs to run off of an element of the
 * kernel's ticks space which is not sensitive to changes in the
 * real-time clock.    All code with disables the soft watchdogs is
 * disabled.  The soft watchdog will go off if not petted within 2
 * seconds under all conditions.  The machine will also safe-state
 * if the soft watchdogs ever go backward.   In addition, time set
 * changes are rejected if received while air-to-donor monitoring is
 * in effect.  This is done because the time jumps in the real-time
 * clocks effect the control loops.
 *
 * Revision 1.1  1999/09/30 04:14:00  BS04481
 * Initial revision
 *
 * TITLE:      safe_msg.hpp Trima task to manage low priority messages
 *             used by the safety driver.
 *
 * ABSTRACT:   Basindata msg sent.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

#ifndef SAFE_MSG_HPP
#define SAFE_MSG_HPP

#include "message.h"
#include "timermessage.h"
#include "trimamessages.h"

#include "shw.hpp"
#include "smsginfo.hpp"
#include "ostime.hpp"



// SPECIFICATION:    class used to send status info
//
// ERROR HANDLING:   none.

class statusMessage
{
public:
   statusMessage();
   virtual ~statusMessage();

   void update ();

private:
   SHS*               _SHSPtr;
   Message<SHwStates> _theStatusMsg;
};

// SPECIFICATION:    class used to receive basin temperature data.
//
// ERROR HANDLING:   none.

class basinDataMessage
{
public:
   basinDataMessage();                      // constructor
   ~basinDataMessage();                     // destructor
   void update ();
private:
   BAS*               _BPtr;
   Message<basinTemp> _theBasinTempMsg;
};


// SPECIFICATION:    timer class used to send low priority messages.
//
// ERROR HANDLING:   none.

class loPrioTimer
{
public:
   loPrioTimer();
   ~loPrioTimer();

private:
   loPrioTimer(const loPrioTimer* l);                // Prevent usage.
   loPrioTimer& operator = (const loPrioTimer& l);   // Prevent usage.

   void notify ();                         // notify function

private:
   basinDataMessage* _basin;
   TimerMessage      _myTimer;

   rawTime           _lastLogTime;
   unsigned int      _lastLoggedMaxWords;
   unsigned int      _lastLoggedCurrWords;
   int               _lastLoggedIdle;
};

// SPECIFICATION:    timer class used to send high priority messages.
//
// ERROR HANDLING:   none.

class hiPrioTimer
{
public:
   hiPrioTimer();
   virtual ~hiPrioTimer();

   void notify ();                         // notify function

private:
   hiPrioTimer(const hiPrioTimer* l);                // Prevent usage.
   hiPrioTimer& operator = (const hiPrioTimer& l);   // Prevent usage.

private:
   statusMessage _status;
};



// SPECIFICATION:    class used to get order info
//
// ERROR HANDLING:   none.

class commands
{
public:
   commands();
   virtual ~commands();                           // destructor

private:
   SHO*               _SHOPtr;
   Message<SHwOrders> _theOrderMsg;

   void notify ();                         // notify function
};

// SPECIFICATION:    derived message class, used to request status
//
// ERROR HANDLING:   none.

class requestStatusMessage
{
public:
   requestStatusMessage();
   virtual ~requestStatusMessage();               // destructor

private:
   RS* _RSPtr;
   int _requestCounter;
   RequestSafetyDriverStatusMsg _theRequestStatusMsg;

   void deliveryTime (void);
   void notify ();                   // notify function
};

#endif   // SAFE_MSG_HPP

/* FORMAT HASH 416f4b974c056257ba84a1af6d51c562 */
