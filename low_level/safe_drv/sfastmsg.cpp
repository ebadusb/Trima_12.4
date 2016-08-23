/*
 * Copyright (c) 1995-2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * $Header: H:/BCT_Development/Trima5.R/Trima/low_level/safe_drv/rcs/sfastmsg.cpp 1.7 2008/01/23 17:30:12Z dslausb Exp $
 * $Log: sfastmsg.cpp $
 * Revision 1.7  2008/01/23 17:30:12Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.6  2003/06/19 19:45:20Z  jl11312
 * - moved hardware interface to separate modules so that it is available earlier in the boot process
 * Revision 1.5  2003/03/28 19:48:14Z  jl11312
 * - modified to use new data log levels
 * Revision 1.4  2002/12/26 20:32:48Z  pn02526
 * Runnable safety driver with CRC checking of the code and FATAL timing errors disabled.  Compiling with ENABLE_CRC_CHECKING defined will enable CRC check failures to shut the system down.  At this time, the CRC checking algorithm has not been defined for vxWorks.  Compiling with ENABLE_FATAL_TIMING_ERRORS defined will enable timing errors to shut the system down.
 * Revision 1.3  2002/08/13 17:28:54  pn02526
 * Remove extraneous #includes of system files.
 * Substitute #include of messagesystem.h, message.h, timermessage.h, and trimamessages.h
 *   for obsolete message system and dispatcher files.
 * Replace the obsolete dispatcher object with a MessageSystem object.
 * Rather than deriving from an INT32 message base class,
 * + add fastDataControl :: _enableFastDataMsg
 *   to use the new trimamessages class definitions.
 * Change ASSERT() macro calls to trima_assert().
 * Rather than deriving from a timer message base class,
 *   add loPrioTimer :: _myTimer
 *   add hiPrioTimer :: _myTimer
 *   to use the new timermessage class definitions.
 * Use the new fastdatalongMessage and fastdataMessage classes from low_level/fast_msg.hpp
 * Revise logging to use the DataLog_Level class through the DataLog_Default macro.
 * Add #ifdef cplusplus and extern "C" sequence to defeat "munching" of the main entry point's symbol name,
 *     so that it can be referenced from the vxWorks shell command line.
 * Change the name of the main entry point from "main" to "sfastmsg" and its type to int; add a return 0 to the end.
 * Create and initialize the default DataLog_Level for safety driver trace logging and make it the DataLog_Default.
 * Remove process priority setting code.
 * Create and initialize the MessageSystem object to send/receive messages to the router.
 * --- Added comments ---  pn02526 [2002/08/13 23:35:11Z]
 * The comment:  "add hiPrioTimer :: _myTimer" is incorrect since there is no hiPrioTimer in this module.
 * Revision 1.2  2001/08/21 09:18:38  ms10234
 * 5268 - Use new trima common sys library.
 * Revision 1.1  2000/08/07 23:01:31  bs04481
 * Initial revision
 *
 * TITLE:      sfastmsg.cpp Trima task to manage optional and low-priority
 *             messages used by the safety driver
 *
 * ABSTRACT:
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "trima_datalog.h"
#include "messagesystem.h"

#include "trima_assert.h"
#include "message.h"
#include "timermessage.h"
#include "error.h"
#include "crcgen.h"
#include "fastdata.h"
#include "sfastmsg.hpp"

// SPECIFICATION:    derived message class, enables/disables fast messages
//

// SPECIFICATION:    fastDataControl constructor
//
// ERROR HANDLING:   None.

fastDataControl::fastDataControl(loPrioTimer* lo) :    // constructor
                                                       _enableFastDataMsg(Callback<fastDataControl>(this, &fastDataControl::notify), MessageBase::SNDRCV_RECEIVE_ONLY)
{
   trima_assert(lo);
   _lo = lo;
}

// SPECIFICATION:    fastDataControl denstructor
//
// ERROR HANDLING:   None.

fastDataControl::~fastDataControl()                   // destructor
{
   _lo = NULL;
}



// SPECIFICATION:    fastDataControl
//                   Causes low priority timer to start or stop sending
//                   messages.  Toggle function
//
// ERROR HANDLING:   None.

void fastDataControl::notify ()                        // notify function
{
   DATA_SOURCE src;

   src = (DATA_SOURCE)_enableFastDataMsg.getData();
   _lo->enableMessage(src);
}




// SPECIFICATION:    lowPriorityTimer constructor
//
// ERROR HANDLING:   None.

loPrioTimer::loPrioTimer()
   : _myTimer(500ul, Callback<loPrioTimer>(this, &loPrioTimer::notify), TimerMessage::DISARMED)
{
   _fastinlethalltime    = new fastdatalongMessage("inletHallTime");
   _fastachalltime       = new fastdatalongMessage("acHallTime");
   _fastplasmahalltime   = new fastdatalongMessage("plasmaHallTime");
   _fastplatelethalltime = new fastdatalongMessage("plateletHallTime");
   _fastreturnhalltime   = new fastdatalongMessage("returnHallTime");

   _fastusair            = new fastdataMessage("safetyLSAir");
   _fastusfluid          = new fastdataMessage("safetyLSFluid");

   _myTimer.armTimer(TimerMessage::ARMED);
}

// SPECIFICATION:    lowPriorityTimer destructor
//
// ERROR HANDLING:   None.

loPrioTimer::~loPrioTimer()
{
   if (_fastinlethalltime != NULL)
      delete _fastinlethalltime;
   _fastinlethalltime = NULL;
   if (_fastachalltime != NULL)
      delete _fastachalltime;
   _fastachalltime = NULL;
   if (_fastplasmahalltime != NULL)
      delete _fastplasmahalltime;
   _fastplasmahalltime = NULL;
   if (_fastplatelethalltime != NULL)
      delete _fastplatelethalltime;
   _fastplatelethalltime = NULL;
   if (_fastreturnhalltime != NULL)
      delete _fastreturnhalltime;
   _fastreturnhalltime = NULL;

   if (_fastusair != NULL)
      delete _fastusair;
   _fastusair = NULL;
   if (_fastusfluid != NULL)
      delete _fastusfluid;
   _fastusfluid = NULL;
}


// SPECIFICATION:    lowPriorityTimer enableMessages
//                   Turns on and off message flow.
//
// ERROR HANDLING:   None.

void loPrioTimer::enableMessage (DATA_SOURCE src)
{
   switch (src)
   {
      case SAFETY_LOWER_US_AIR :
         _fastusair->toggle();
         break;
      case SAFETY_LOWER_US_FLUID :
         _fastusfluid->toggle();
         break;
      case AC_PUMP_HALL_TIME :
         _fastachalltime->toggle();
         break;
      case INLET_PUMP_HALL_TIME :
         _fastinlethalltime->toggle();
         break;
      case PLASMA_PUMP_HALL_TIME :
         _fastplasmahalltime->toggle();
         break;
      case PLATELET_PUMP_HALL_TIME :
         _fastplatelethalltime->toggle();
         break;
      case RETURN_PUMP_HALL_TIME :
         _fastreturnhalltime->toggle();
         break;
      default :
         DataLog(log_level_sfastmsg_error) << src << " Unrecognized fastDataControl." << endmsg;
         break;
   }
}

// SPECIFICATION:    lowPriorityTimer notify
//                   When the timer goes off, checks to see
//                   if there are any low priority messages
//                   ready to be sent.
//
// ERROR HANDLING:   None.

void loPrioTimer::notify ()
{
   _fastinlethalltime->update();
   _fastachalltime->update();
   _fastplasmahalltime->update();
   _fastplatelethalltime->update();
   _fastreturnhalltime->update();

   _fastusair->update();
   _fastusfluid->update();
}

// SPECIFICATION:    main entry point
//                   parameter 1 = local queue name
//                   2 = remote node number
//                   3 = remote queue name
//
// ERROR HANDLING:   none.
//
// NOTE: As this task is not required for production build, it has been omitted
// from the default startup list in trima_tasks.h. If it is enabled, then the
// Hall time collectors must also be enabled by the safety driver.
// See FAST_DATA in safe_drv.hpp

extern "C" void sfastmsg ()
{
   class loPrioTimer*     l = NULL;
   class fastDataControl* c = NULL;
//   struct sched_param param;

//   setprio( 0, 12);
//   sched_getparam( 0, &param);
//   sched_setscheduler( 0, SCHED_RR, &param);

   // FS messaging
   MessageSystem ms;
   ms.initBlocking("sfastmsg");

   // Initialize Data Log Level class
   datalog_SetDefaultLevel(log_handle_sfastmsg_info);

   // set up for sending low priority messages
   l = new loPrioTimer();
   c = new fastDataControl(l);

   // run dispatcher
   ms.dispatchMessages();
}

/* FORMAT HASH 2c4a9bc0e013ee7ad52a8f979282a61b */
