/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      timewinow.cpp
 *             TRIMA REMOTE ACCESS PROTOCOL
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface for the TRAP protocol
 *
 *
 * HISTORY:    $Log: timewindow.cpp $
 * HISTORY:    Revision 1.8  2003/11/05 16:37:37Z  rm70006
 * HISTORY:    IT 5830.  Fix bug with end connection message.
 * HISTORY:    Revision 1.7  2003/09/29 17:03:13Z  rm70006
 * HISTORY:    IT 6463.  Make time logging statement better.
 * HISTORY:    Revision 1.6  2003/09/22 22:07:27Z  rm70006
 * HISTORY:    IT 6412.  Clean up code.
 * HISTORY:    Revision 1.5  2003/04/02 22:38:33Z  rm70006
 * HISTORY:    IT 5832.
 * HISTORY:
 * HISTORY:    Changed logging to match new spec.
 * HISTORY:    Revision 1.4  2002/10/28 21:11:27Z  rm70006
 * HISTORY:    Port logging and sleep call.
 * HISTORY:    Revision 1.3  2002/04/09 16:46:00Z  rm70006
 * HISTORY:    Initial port to VxWorks comlete.  Code compiles.  Need to convert sleep to conforming VxWorks call.
 * HISTORY:    Revision 1.2  2002/01/14 20:02:44Z  rm70006
 * HISTORY:    IT 5507.
 * HISTORY:    Found bug in print statement.  Missing begin of window parameter.
 * HISTORY:    Revision 1.1  2002/01/08 23:57:20Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.2  2001/12/20 15:32:43  rm70006
 * HISTORY:    IT 5505.
 * HISTORY:    Fix bug in sleep calculation.
 * HISTORY:    Revision 1.1  2001/12/04 22:41:00  rm70006
 * HISTORY:    Initial revision
 *
 ******************************************************************************/

#include <unistd.h>
#include <stdio.h>

#include "time.h"

#include "timewindow.h"

#include "error.h"
#include "units.h"
#include "trima_datalog.h"


//
// Default Constructor
//
TimeWindow::TimeWindow(int beginWindow, int endWindow, TRAP_DAYS_OF_WEEK days)
   : _BeginWindow(beginWindow),
     _EndWindow(endWindow),
     _Days(days)
{}




//
// Default Destructor
//
TimeWindow::~TimeWindow()
{}



//
// PastWinodw
//
bool TimeWindow::PastWindow ()
{
   time_t currentTime;

   time(&currentTime);

   struct tm*        now      = gmtime(&currentTime);

   TRAP_DAYS_OF_WEEK today    = DayOfWeek(now->tm_wday);
   TRAP_DAYS_OF_WEEK tomorrow = DayOfWeek(now->tm_wday + 1);

   DataLog(log_level_trap_info) << "today is day " << today << ", " << now->tm_hour << " hours, "
                                << now->tm_min << " minutes.  window is " << _BeginWindow
                                << "->" << _EndWindow << ", day mask is " << _Days << endmsg;

   // If today is correct day.
   if ( (today & _Days) == today)
   {
      if (now->tm_hour > _EndWindow)
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }
   else if ( (tomorrow & _Days) == tomorrow)
   {
      return 0;
   }
   else
   {
      return 1;
   }
}



//
// BeforeWindow
//
bool TimeWindow::BeforeWindow ()
{
   time_t currentTime;

   time(&currentTime);

   struct tm*        now      = gmtime(&currentTime);

   TRAP_DAYS_OF_WEEK today    = DayOfWeek(now->tm_wday);
   TRAP_DAYS_OF_WEEK tomorrow = DayOfWeek(now->tm_wday + 1);


   // If today is correct day.
   if ( (today & _Days) == today)
   {
      if (now->tm_hour < _BeginWindow)
      {
         return 1;
      }
      else
      {
         return 0;
      }
   }
   else if ( (tomorrow & _Days) == tomorrow)
   {
      return 1;
   }
   else
   {
      return 0;
   }
}



//
// Wait
//
void TimeWindow::Wait ()
{
   int             secondsToWait;

   time_t          currentTime;
   struct timespec waitTime = {0, 0};

   time(&currentTime);

   struct tm*        now      = gmtime(&currentTime);

   TRAP_DAYS_OF_WEEK today    = DayOfWeek(now->tm_wday);
   TRAP_DAYS_OF_WEEK tomorrow = DayOfWeek(now->tm_wday + 1);


   // If today is correct day.
   if ( (today & _Days) == today)
   {
      if (now->tm_hour < _BeginWindow)
      {
         secondsToWait = (SECONDS_PER_HOUR * (_BeginWindow - now->tm_hour - 1)) +
                         (SECONDS_PER_MINUTE * (SECONDS_PER_MINUTE - now->tm_min - 1)) +
                         (SECONDS_PER_MINUTE - now->tm_sec);
      }
      else
      {
         secondsToWait = 0;
      }
   }
   else if ( (tomorrow & _Days) == tomorrow)
   {
      secondsToWait = (SECONDS_PER_HOUR * (HOURS_PER_DAY - now->tm_hour) ) + (SECONDS_PER_HOUR * _BeginWindow ) +
                      (SECONDS_PER_MINUTE * (SECONDS_PER_MINUTE - now->tm_min - 1) ) +
                      (SECONDS_PER_MINUTE - now->tm_sec);
   }
   else
   {
      secondsToWait = 0;
   }

   DataLog(log_level_trap_info) << "Wait " << secondsToWait << " seconds" << endmsg;

   waitTime.tv_sec = secondsToWait;

   nanosleep (&waitTime, 0);
}



//
// DayOfWeek
//
TRAP_DAYS_OF_WEEK TimeWindow::DayOfWeek (int day)
{
   switch (day)
   {
      case 0 :
         return TRAP_SUNDAY;
      case 1 :
         return TRAP_MONDAY;
      case 2 :
         return TRAP_TUESDAY;
      case 3 :
         return TRAP_WEDNESDAY;
      case 4 :
         return TRAP_THURSDAY;
      case 5 :
         return TRAP_FRIDAY;
      case 6 :
         return TRAP_SATURDAY;
      default :
         return TRAP_NO_DAYS;
   }
}

/* FORMAT HASH 952bb6a778a88b45bb87ca3d65cdd10a */
