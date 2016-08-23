/*******************************************************************
*    Copyright(c) 2011 CaridianBCT, Inc. All rights reserved
*
* TITLE:      sumo.hpp
*             Stack Usage Monitor (SUMO)
*
* AUTHOR:     Jason Holdeman
*
* DESCRIPTION:
  This class monitors the stack usage of the other tasks and writes to dlog when
  it exceeds a predefined % limit, such as 90% of initial size.

*******************************************************************/

#ifndef SUMO_HPP
#define SUMO_HPP

// Includes

#include "timermessage.h"
#include "trima_tasks.h"
#include "trimamessages.h"
#include "messagesystem.h"


class sumo
   : public TimerMessage
{
public:
   sumo(MessageSystem& ms);
   virtual ~sumo();

protected:
   float                    stackmargin;
   TASK_DESC                taskDescriptor;
   TrimaTaskInfo            TTaskInfo;
   MessageSystem&           _ms;

   SystemStateChangeMsg     _SystemStateChangeMsg;
   StackMonitorTaskEndMsg   _EndTask;
   StackMonitorTaskStartMsg _StartTask;

protected:
   sumo();  // Hide default constructor

   void checkStack ();

   //
   // Callback function for System State Changed message
   //
   void systemStateChanged ();

   void endTask ();

   void startTask ();

};

#endif
////////////////////////////////////////////////////////////////////////////////
// end of sumo.hpp

/* FORMAT HASH 1b526f3164b1c76e18c850fdab3075bf */
