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


// Includes
#include <vxWorks.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <usrLib.h>
#include <taskLib.h>
#include <wvLib.h>      // Needed for Event Timing

// public data

#include "sumo.hpp"
#include "units.h"
#include "trima_datalog.h"
#include "an_alarm.h"
#include "states.h"


// Check every 10 minutes
const int checkRate = 10 * MILLISEC_PER_SEC * SEC_PER_MIN;

//
// Constructor
//
sumo::sumo(MessageSystem& ms)
   : TimerMessage(checkRate, Callback<sumo>(this, &sumo::checkStack), DISARMED),
     stackmargin(0.0),
     _StartTask(Callback<sumo>(this, &sumo::startTask)),
     _EndTask(Callback<sumo>(this, &sumo::endTask)),
     _ms(ms)
{}


//
// Destructor
//
sumo::~sumo()
{}

//
// checkStack
//
void sumo::checkStack ()
{
   int taskID = 0;
   int event_type;

   // windView instrumentation
   //   string name   = "Stack Monitor";
   //   event_type = 56;
   //   wvEvent(event_type, (char *)name.c_str(), name.length());
   //   DataLog(log_level_service_info) << "Stack Monitor checking stack sizes" << endmsg;

   // Loop through all of the tasks
   for (int ttaskID = TTID_FirstTask; ttaskID < TTID_EndOfTasks; ttaskID++)
   {
      if (trimaGetTaskInfo((TrimaTaskID)ttaskID, &TTaskInfo) == 0)
      {
         taskID = TTaskInfo.tid;

         // Get the information on the task
         if ( (taskID != 0) && taskInfoGet (taskID, &taskDescriptor) == OK)
         {
            // Make sure we're not blocking the system
            taskDelay (int(sysClkRateGet() * 0.5));   // Wait 1/2 of a second

            const float PercentMargin = (float)taskDescriptor.td_stackMargin / (float)taskDescriptor.td_stackSize;

            DataLog(log_level_service_info) << "Task: "
                                            << TTaskInfo.taskName << " Allocated: " << taskDescriptor.td_stackSize
                                            << " Current: " << taskDescriptor.td_stackCurrent
                                            << " Max: " << taskDescriptor.td_stackHigh
                                            << " Margin: " << taskDescriptor.td_stackMargin
                                            << " %Margin: " << PercentMargin
                                            << ( (PercentMargin >= 0.2f) ? " OK" : " MARGIN NEEDED!")
                                            << endmsg;

            // If the task is over the stack limit
            if (taskDescriptor.td_stackCurrent >= taskDescriptor.td_stackSize ||
                taskDescriptor.td_stackHigh >= taskDescriptor.td_stackSize)
            {
               anAlarmMsg alarm(SW_FAULT);
               alarm.setAlarm("Stack size exceeded");
               DataLog(log_level_service_error) << "Stack size for task " << TTaskInfo.taskName << " has exceeded allocated stack size." << endmsg;

               if (taskDescriptor.td_stackCurrent > (2 * taskDescriptor.td_stackSize) ||
                   taskDescriptor.td_stackHigh > (2 * taskDescriptor.td_stackSize))
               {
                  DataLog(log_level_service_error) << "Stack size violation *may* be an anomaly." << endmsg;
               }

               // Shut off the timer so we don't check any more
               armTimer(TimerMessage::DISARMED);
            }
            else
            {
               // If the task is within 10% of the limit write to the log
               stackmargin = (float)taskDescriptor.td_stackMargin / (float)taskDescriptor.td_stackSize;

               if ( stackmargin < 0.1 )
               {
                  DataLog(log_level_service_error) << "Stack margin for task " << TTaskInfo.taskName << " is " << stackmargin * 100 << "%" << endmsg;
               }
            }
         }
      }
      else
      {
         DataLog(log_level_service_error) << "Could not retreive Trima task information for Trima taskid " << ttaskID << endmsg;
      }
   }

   // windView instrumentation
   //   event_type = 57;
   //   wvEvent(event_type, (char *)name.c_str(), name.length());
}

void sumo::systemStateChanged ()
{
   const State_names newState = (State_names)_SystemStateChangeMsg.getData();

   if (newState == POST_RUN)
   {
      DataLog(log_level_service_info) << "Stack Monitor end run stack sizes" << endmsg;

      // check the stack sizes at end of run
      checkStack();

      // Only run once
      _SystemStateChangeMsg.deregisterMsg();
   }
}


void sumo::endTask ()
{
   DataLog(log_level_service_info) << "Task shutting down" << endmsg;

   // Turn off the timer
   armTimer(TimerMessage::DISARMED);

   // Exit the message processing loop, which should exit the task
   _ms.stopLoop();
}



void sumo::startTask ()
{
   DataLog(log_level_service_info) << "Starting task monitoring" << endmsg;

   _SystemStateChangeMsg.init(Callback<sumo>(this, &sumo::systemStateChanged));

   // Run once on command and then on the timer.
   checkStack();

   // Turn on the timer
   armTimer(TimerMessage::ARMED);
}



//
// sumo executable
//
extern "C" void stackmon ()
{
   DataLog(log_level_service_info) << "Stack Monitor started" << endmsg;

   MessageSystem ms;
   ms.initBlocking("SUMO");

   sumo stackmonitor(ms);

   ms.dispatchMessages();

   DataLog(log_level_service_info) << "Stack Monitor terminating" << endmsg;
}



/* telnet command to start the SUMO tasks */
extern "C" void startSumo ()
{
   MessageSystem ms;
   ms.initNonBlock("startSumo");

   StackMonitorTaskStartMsg msg(MessageBase::SEND_GLOBAL);
   msg.send(1);

   printf("Starting SUMO\n");
}

/* FORMAT HASH bb008b2069b174a5db6dcc58f4c75e66 */
