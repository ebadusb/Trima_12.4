/*
 * Copyright (c) 1995-1999 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/safe_drv/rcs/safe_msg.cpp 1.19 2003/06/25 17:09:15Z jl11312 Exp jl11312 $
 * $Log: safe_msg.cpp $
 * Revision 1.19  2003/06/25 17:09:15Z  jl11312
 * - added logging for safety side CPU idle time and memory availability
 * Revision 1.18  2003/05/15 18:40:04Z  rm70006
 * Add support for alarm level logging.
 * Revision 1.17  2003/05/07 20:04:33Z  jl11312
 * - changed aux clock notifiication to use counting semaphores for control and safety driver to avoid missed poll cycles (it 6020)
 * Revision 1.16  2003/04/29 17:03:03Z  jl11312
 * - put shmem_create calls in driver and shmem_open calls in other modules to remove timing dependencies on startup
 * Revision 1.15  2003/03/28 19:48:13Z  jl11312
 * - modified to use new data log levels
 * Revision 1.14  2003/01/10 18:29:38Z  jl11312
 * - changed message latency check
 * Revision 1.13  2003/01/09 20:23:04  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.12  2002/12/17 15:21:00  pn02526
 * Checkins for first operating safety driver under vxWorks
 * Revision 1.11  2002/10/31 11:28:26  pn02526
 * Massive checkin for integration testing.  Changes BOOL to bool, and forces build to be for 486, only.
 * Revision 1.10  2002/08/13 16:44:40  pn02526
 * Remove extraneous #includes of system files.
 * Substitute the temporary shared memory emulation for the QNX one.
 * Revise logging to use the DataLog_Level class through the DataLog_Default macro.
 * Revise FATAL_ERROR calls to use the new one.
 * Substitute #include of messagesystem.h, message.h, timermessage.h, and trimamessages.h
 *   for obsolete message system and dispatcher files.
 * Substitute #include of ostime.h for obsolete kernaltime.h.
 * Remove #include of obsolete kernaltime.h and timedispatch.hpp, and invocations of them.
 * Replace the obsolete dispatcher object with a MessageSystem object.
 * Rather than deriving from a buffered message base class:
 * + add commands :: _theOrderMsg,
 * + add statusMessage :: _theStatusMsg,
 * + add basinDataMessage :: _theBasinTempMsg,
 *   to use the new buffered message class definitions.
 * Rather than deriving from an INT32 message base class,
 *   add requestStatusMessage :: _theRequestStatusMsg,
 *   to use the new trimamessages class definitions.
 * Remove _fd_* members, since they are not used by the shared memory emulation.
 * Insert shmem_close calls into destructor functions for classes that open shared memory.
 * Use message system's definitions of pid (process a.k.a. task Id) and nid (node Id).
 * To keep GNU compiler happy for production builds:
 * + Change the type of the buffer pointer argument iin all crcgen32 calls to const unsigned char *.
 * TEMPORARILY comment out the timeJumpControl class until it is seen whether it is necessary under vxWorks.
 * Rather than deriving from a timer message base class,
 *   add hiPrioTimer :: _myTimer
 *   to use the new timermessage class definitions.
 * Add #ifdef cplusplus and extern "C" sequence to defeat "munching" of the main entry point's symbol name,
 *     so that it can be referenced from the vxWorks shell command line.
 * Remove process priority setting code.
 * Create and initialize the default DataLog_Level for safety driver trace logging and make it the DataLog_Default.
 * Create and initialize the MessageSystem object to send/receive messages to the router.
 * --- Added comments ---  pn02526 [2002/08/13 22:52:55Z]
 * Rather than deriving from a timer message base class,
 *   add loPrioTimer :: _myTimer
 *   to use the new timermessage class definitions.
 * Revision 1.9  2001/04/05 14:16:18  jl11312
 * - internal timer handling changes required for versa logic CPU board
 * Revision 1.8  2000/11/01 19:10:53  bs04481
 * Change fatal error for requestSafetyStatus message to 2.19 sec.
 * Log latencies greather than 1 sec.
 * Revision 1.7  2000/11/01 18:59:40  bs04481
 * Fatal error if the delivery latency for RequestSafetyStatus
 * exceeds 2 sec.  Log latencies above 500ms.
 * Revision 1.6  2000/10/27 14:35:27  bs04481
 * Measure delivery time for request safety status messages.  Log
 * any occurrences over 250ms.  For the time being, do not fatal
 * error on excessive delivery times.  Use the kenel based time
 * dispatcher to avoid sensitivity to setting the real-time clock.
 * Revision 1.5  2000/08/07 23:00:42  bs04481
 * Allow visibility to 10ms data used/created by the drivers.
 * See IT 4741
 * Revision 1.4  2000/07/07 20:54:20  bs04481
 * Bump priorities up 1
 * Revision 1.3  2000/03/17 16:41:31  BS04481
 * Non-essential issues from 3.3 code review
 * Revision 1.2  1999/10/28 20:29:27  BS04481
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
 * Revision 1.1  1999/09/30 04:13:57  BS04481
 * Initial revision
 *
 * TITLE:      safe_msg.cpp Trima task to manage urgent and high-prioroty
 *             messages used by the safety driver
 *
 * ABSTRACT:   Basindata sent.
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 *
 * PUBLIC FUNCTIONS:
 * main() - main entry point
 *
 */

#include <vxWorks.h>
#include <private/memPartLibP.h>
#include <memLib.h>

#define _ALARM_NAME_LOGGING_   // Get alarm name translations.
#include "alarms.h"

#include "an_alarm.h"
#include "crcgen.h"
#include "failure_debug.h"
#include "messagesystem.h"
#include "ostime.hpp"
#include "sh_mem.h"
#include "smsginfo.hpp"
#include "safe_msg.hpp"
#include "trimamessages.h"
#include "trima_datalog.h"


#define MAX_SAFETY_MESSAGE_LATENCY  2190      // in ms.  max time allowed between send and
                                              // delivery of request safety status messages.


#define LOG_SAFETY_MESSAGE_LATENCY  1000      // in ms.  log if delivery time exceeds for
                                              // request safety status messages.
                                              //
// SPECIFICATION:    Control hardware commands, constructor
//
// ERROR HANDLING:   none.

commands::commands()
   : _theOrderMsg(Callback<commands>(this, &commands::notify), MessageBase::SNDRCV_RECEIVE_ONLY)
{
   // link to the shared memory for communication to the driver
   _SHOPtr = (SHO*)shmem_open("safetyOrder");
   if ( !_SHOPtr )
   {
      _FATAL_ERROR(__FILE__, __LINE__, "safetyOrder shmem_open failed");
   }
}

// SPECIFICATION:    Safety hardware commands, destructor
//
// ERROR HANDLING:   none.

commands::~commands()
{
   shmem_close("safetyOrder");
   _SHOPtr = NULL;
}

// SPECIFICATION:    Safety hardware commands, notify() gets data
//
// ERROR HANDLING:   none.

void commands::notify ()
{
   // get data and header information into shmem
   _SHOPtr->msg[_SHOPtr->NextIn].orders = _theOrderMsg.getData();
   _SHOPtr->msg[_SHOPtr->NextIn].pid    = _theOrderMsg.originTask();
   _SHOPtr->msg[_SHOPtr->NextIn].nid    = _theOrderMsg.originNode();
   _SHOPtr->msg[_SHOPtr->NextIn].time   = _theOrderMsg.sentTime();

   // generate a crc
   _SHOPtr->msg[_SHOPtr->NextIn].crc = 0;

   if (crcgen32(&_SHOPtr->msg[_SHOPtr->NextIn].crc, (const unsigned char*)&_SHOPtr->msg[_SHOPtr->NextIn].orders, (sizeof(SHwOrders) + sizeof(unsigned long) + sizeof(unsigned long) + sizeof(struct timespec))) != 0)
      _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
   else
   {
      // test for lapping NextOut
      if (_SHOPtr->NextIn + 1 == _SHOPtr->NextOut)
      {
         DataLog(log_level_critical) << errnoGet()
                                     << " safetyOrders NextIn lapping NextOut!" << endmsg;

         _FATAL_ERROR(__FILE__, __LINE__, "safetyOrders NextIn lapping NextOut!");
      }
      // increment NextIn index
      if (_SHOPtr->NextIn == NUM_SHWORDERS - 1)
         _SHOPtr->NextIn = 0;
      else
         _SHOPtr->NextIn++;
   }
}


// SPECIFICATION:    RequestStatus message, constructor
//
// ERROR HANDLING:   none.

requestStatusMessage::requestStatusMessage()
   : _theRequestStatusMsg(Callback<requestStatusMessage>(this, &requestStatusMessage::notify), MessageBase::SNDRCV_RECEIVE_ONLY)
{
   // create shared memory for communication to the safety driver
   _RSPtr = (RS*)shmem_open("requestStatus");

   if (_RSPtr == (RS*)NULL)
      _FATAL_ERROR(__FILE__, __LINE__, "requestStatus shmem_open failed");

   // initialize counter
   _requestCounter = 0;
}

// SPECIFICATION:    RequestStatus message, destructor
//
// ERROR HANDLING:   none.

requestStatusMessage::~requestStatusMessage()
{
   shmem_close("requestStatus");
   _RSPtr = (RS*)NULL;
}

// SPECIFICATION:    RequestStatus message, notify
//
// ERROR HANDLING:   none.

void requestStatusMessage::notify ()
{
   // check the message delivery latency on this message type.
   // this message needs to be delivered within 500ms of being sent.
   deliveryTime();

   // update the counter
   if (++_requestCounter == 0xfffffff)
      _requestCounter = 0;
   _RSPtr->counter[_RSPtr->NextIn] = _requestCounter;

   // test for lapping NextOut
   if (_RSPtr->NextIn + 1 == _RSPtr->NextOut)
   {
      DataLog(log_level_critical) << errnoGet()
                                  << " requestStatus NextIn lapping NextOut!" << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "requestStatus NextIn lapping NextOut!");
   }
   // increment NextIn index
   if (_RSPtr->NextIn == NUM_SREQUEST - 1)
      _RSPtr->NextIn = 0;
   else
      _RSPtr->NextIn++;

}


// SPECIFICATION:    RequestStatus message, time message delivery
//                   Calculates elapsed time between message sent and message delivered
//
// ERROR HANDLING:   FATAL if elapsed time exceeds limit.

void requestStatusMessage::deliveryTime (void)
{
   // calculate elapsed time in millisecs and fatal if excessive
   const unsigned int elapsedTime = _theRequestStatusMsg.latency();

   if (elapsedTime > MAX_SAFETY_MESSAGE_LATENCY)
   {
      DataLog(log_level_critical) << elapsedTime
                                  << " requestStatus delivery exceeds " << MAX_SAFETY_MESSAGE_LATENCY
                                  << "ms." << endmsg;

      _FATAL_ERROR(__FILE__, __LINE__, "requestStatus delivery time");
   }
   else if (elapsedTime > LOG_SAFETY_MESSAGE_LATENCY)
   {
      DataLog_Default << elapsedTime << "requestStatus delivery > "
                      << LOG_SAFETY_MESSAGE_LATENCY << "ms." << endmsg;
   }

}


// SPECIFICATION:    Safety status message constructor
//
// ERROR HANDLING:   None.

statusMessage::statusMessage()
   : _theStatusMsg(MessageBase::SEND_GLOBAL)
{
   // open shared memory for communication to the safety driver
   _SHSPtr = (SHS*)shmem_open("safetyStatus");
   if ( !_SHSPtr )
   {
      _FATAL_ERROR(__FILE__, __LINE__, "safetyStatus shmem_open failed");
   }

   // initialize the buffer indices
   _SHSPtr->NextIn      = 0;
   _SHSPtr->NextOut     = 0;
   _SHSPtr->timingError = false;
}

// SPECIFICATION:    Safety status message destructur
//
// ERROR HANDLING:   None.

statusMessage::~statusMessage()
{
   shmem_close("safetyStatus");
   _SHSPtr = (SHS*)NULL;
}

// SPECIFICATION:    Safety status message update
//                   Sends new status data
//
void statusMessage::update ()
{
   static rawTime _lastUpdateTime;   // last time timer loop ran
   static bool    firstTime = true;

   if (firstTime)
   {
      osTime::snapshotRawTime(_lastUpdateTime);
      firstTime = false;
   }

   const int deltaTime = osTime::howLongMilliSecAndUpdate(_lastUpdateTime);

   // Time chosen arbitrarily.  Nominal time is 10 ms.  50 seems too long for a high frequency timer.
   if (deltaTime > 50)
   {
      DataLog(log_level_ctl_msg_error) << "WARNING: safe_msg periodic timer too long.  Delta time is " << deltaTime
                                       << " ms." << endmsg;
   }

   if (_SHSPtr->timingError)
   {
      anAlarmMsg timingAlarm(INTERNAL_TIMING_ERROR);
      timingAlarm.setAlarm("Internal timing error alarm");

      _SHSPtr->timingError = false;
   }

   // If we have recieved a new message, process it.
   if (_SHSPtr->NextIn != _SHSPtr->NextOut)
   {
      unsigned long checksum = 0;
      if (crcgen32(&checksum, (const unsigned char*)&_SHSPtr->msg[_SHSPtr->NextOut].status, sizeof(SHwStates)) != 0)
         _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
      else
      {
         if (checksum == _SHSPtr->msg[_SHSPtr->NextOut].crc)
         {
            _theStatusMsg.send(_SHSPtr->msg[_SHSPtr->NextOut].status);
         }
         else
         {
            DataLog(log_level_critical) << "CRC mismatch Expected: 0x"
                                        << hex << checksum << dec << ", Received: 0x" << hex
                                        << _SHSPtr->msg[_SHSPtr->NextOut].crc << dec << endmsg;

            _FATAL_ERROR(__FILE__, __LINE__, "CRC mismatch");
         }
      }
      if (_SHSPtr->NextOut == NUM_SHWSTATUS - 1)
         _SHSPtr->NextOut = 0;
      else
         _SHSPtr->NextOut++;

   }
}



// SPECIFICATION:    sends data for
//                   basin temperature message which informs Procedure
//                   of temperature in the basin.
//
// ERROR HANDLING:   none.

// SPECIFICATION:    basinDataMessage constructor
//
// ERROR HANDLING:   None.

basinDataMessage::basinDataMessage()
   : _theBasinTempMsg(MessageBase::SEND_GLOBAL)
{
   // open shared memory for communication to the safety driver
   _BPtr = (BAS*)shmem_open("basinData");
   if ( !_BPtr )
      _FATAL_ERROR(__FILE__, __LINE__, "basinData shmem_open failed");
}

// SPECIFICATION:    basinDataMessage destructor
//
// ERROR HANDLING:   None.

basinDataMessage::~basinDataMessage()
{
   shmem_close("basinData");
   _BPtr = (BAS*)NULL;
}

// SPECIFICATION:    basinDataMessage update
//                   Sends the basinDataMessage if the
//                   indices indicate new data has been
//                   set by the driver
//
// ERROR HANDLING:   None.

void basinDataMessage::update ()
{
   if (_BPtr->NextIn != _BPtr->NextOut)
   {
      // check the CRC
      unsigned long checksum = 0;
      if (crcgen32(&checksum, (const unsigned char*)&_BPtr->msg[_BPtr->NextOut].data, sizeof(basinTemp)) != 0)
         _FATAL_ERROR(__FILE__, __LINE__, "CRC generation error");
      else
      {
         if (checksum == _BPtr->msg[_BPtr->NextOut].crc)
         {
            _theBasinTempMsg.send(_BPtr->msg[_BPtr->NextOut].data);
         }
         else
         {
            DataLog(log_level_critical) << "CRC mismatch Expected: 0x"
                                        << hex << checksum << dec << ", Received: 0x"
                                        << hex << _BPtr->msg[_BPtr->NextOut].crc << dec << endmsg;

            _FATAL_ERROR(__FILE__, __LINE__, "CRC mismatch");
         }
      }
      if (_BPtr->NextOut == NUM_BASIN - 1)
         _BPtr->NextOut = 0;
      else
         _BPtr->NextOut++;

   }
}

// SPECIFICATION:    lowPriorityTimer constructor
//
// ERROR HANDLING:   None.

loPrioTimer::loPrioTimer()
   : _myTimer(500ul, Callback<loPrioTimer>(this, &loPrioTimer::notify), TimerMessage::DISARMED),
     _lastLoggedMaxWords(0),
     _lastLoggedCurrWords(0),
     _lastLoggedIdle(0)
{
   _basin = new basinDataMessage();
   _myTimer.armTimer(TimerMessage::ARMED);
   osTime::snapshotRawTime(_lastLogTime);
}

// SPECIFICATION:    lowPriorityTimer destructor
//
// ERROR HANDLING:   None.

loPrioTimer::~loPrioTimer()
{
   if (_basin != NULL)
      delete _basin;
   _basin = NULL;
}


// SPECIFICATION:    lowPriorityTimer notify
//                   When the timer goes off, checks to see
//                   if there are any low priority messages
//                   ready to be sent.
//
// ERROR HANDLING:   None.

void loPrioTimer::notify ()
{
   _basin->update();

   if ( osTime::howLongMilliSec(_lastLogTime) > 10000 )
   {
      unsigned int maxWords  = DBG_GetMaxWordsAllocated();
      unsigned int currWords = memSysPartId->curWordsAllocated;
      int          idle      = DBG_GetIdlePercent();

      if ( idle < _lastLoggedIdle - 5 ||
           idle > _lastLoggedIdle + 5 )
      {
         DataLog_Default << "CPU idle=" << idle << "%" << endmsg;
         _lastLoggedIdle = idle;
      }

      if ( maxWords != _lastLoggedMaxWords ||
           currWords != _lastLoggedCurrWords )
      {
         DataLog_Default << "Memory allocation: currWords=" << currWords
                         << " maxWords=" << maxWords << " availWords="
                         << memSysPartId->totalWords << " maxContiguousBlockBytes="
                         << memPartFindMax(memSysPartId) // Return in words to retain logging consistency
                         << endmsg;
         _lastLoggedMaxWords  = maxWords;
         _lastLoggedCurrWords = currWords;
      }

      osTime::snapshotRawTime(_lastLogTime);
   }
}

// SPECIFICATION:    highPriorityTimer constructor
//
// ERROR HANDLING:   None.

hiPrioTimer::hiPrioTimer()
   : _status()
{}

// SPECIFICATION:    highPriorityTimer destructor
//
// ERROR HANDLING:   None.

hiPrioTimer::~hiPrioTimer()
{}


// SPECIFICATION:    highPriorityTimer notify
//                   When the timer goes off, checks to see
//                   if there are any high priority messages
//                   ready to be sent.
//
// ERROR HANDLING:   None.

void hiPrioTimer::notify ()
{
   _status.update();
}

// SPECIFICATION:    main entry point
//
// ERROR HANDLING:   none.
extern "C" void safe_msg ()
{
   loPrioTimer*          l = NULL;
   hiPrioTimer*          h = NULL;
   commands*             c = NULL;
   requestStatusMessage* r = NULL;

   // Initialize default log level
   datalog_SetDefaultLevel(log_handle_safe_msg_info);

   // FS messaging
   MessageSystem ms;
   ms.initNonBlock("safe_msg");

   // set up for sending low priority messages
   l = new loPrioTimer();

   // set up for sending high priority messages
   h = new hiPrioTimer();

   // set up for command message
   c = new commands();

   // set up for request status message
   r = new requestStatusMessage();

   //
   // Attach to aux clock periodic update
   //
   SEM_ID tmrSem = auxClockSemaphoreAttach(10000, AuxClockBinarySemaphore);

   if (!tmrSem)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "auxClockSemaphoreAttach failed");
   }

   while (true)
   {
      semTake(tmrSem, WAIT_FOREVER);

      h->notify();
      ms.dispatchMessages();
   }
}

/* FORMAT HASH 6a59786433ad5f5f61eb4d532f8d045c */
