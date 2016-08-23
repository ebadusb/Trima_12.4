/*******************************************************************************
 *
 * Copyright (c) 2001 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      timewinow.h
 *             TRIMA REMOTE ACCESS PROTOCOL
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file defines the interface for the TRAP protocol
 *
 *
 * HISTORY:    $Log: timewindow.h $
 * HISTORY:    Revision 1.4  2003/09/22 22:07:32Z  rm70006
 * HISTORY:    IT 6412.  Clean up code.
 * HISTORY:    Revision 1.3  2003/04/02 22:38:36Z  rm70006
 * HISTORY:    IT 5832.
 * HISTORY:
 * HISTORY:    Changed logging to match new spec.
 * HISTORY:    Revision 1.2  2002/10/28 21:11:27Z  rm70006
 * HISTORY:    Port logging and sleep call.
 * HISTORY:    Revision 1.1  2002/01/08 23:57:10Z  rm70006
 * HISTORY:    Initial revision
 * HISTORY:    Revision 1.1  2001/12/04 22:41:18  rm70006
 * HISTORY:    Initial revision
 *
 ******************************************************************************/
#ifndef __TIMEWINDOW__
#define __TIMEWINDOW__

#include <time.h>
#include "trapif.h"

class TimeWindow
{
public:
   TimeWindow(int beginWindow, int endWindow, TRAP_DAYS_OF_WEEK days);
   ~TimeWindow();


   bool PastWindow ();

   bool BeforeWindow ();

   void Wait ();

private:
   TRAP_DAYS_OF_WEEK DayOfWeek (int day);

private:
   struct tm         now;

   int               _BeginWindow;
   int               _EndWindow;

   TRAP_DAYS_OF_WEEK _Days;
};

#endif

/* FORMAT HASH 921de2ebb5b9996fa094c8d3a6b5fe64 */
