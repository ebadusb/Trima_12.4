/*
 * Copyright (c) 1996 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      stdappl.hpp
 *
 */
#include <vxworks.h>
#include <stdio.h>
#include <signal.h>

#include "an_alarm.h"
#include "error.h"
#include "stappl.h"
#include "trima_datalog.h"

void CStdAppl::Terminate (const char* filename,
                          int lineNumber,
                          int logCode)
{
   DataLog(log_level_trima_library_info) << "Process (pid) committed suicide." << endmsg;
   raise(SIGQUIT);
}

CStdAppl::CStdAppl()
   : _InitAlarm(NULL_ALARM),
     _InitAlarmTimer()
{}

CStdAppl::~CStdAppl()
{}

void CStdAppl::Cleanup ()
{}

void CStdAppl::setInitializationAlarm ()
{
   //
   // Set the initialization alarm every time
   //  a message is received.  We cannot process
   //  anything ...
   //
   anAlarmMsg initAlarm(_InitAlarm);
   initAlarm.setAlarm();

   DataLog(log_level_trima_library_info) << "Initialization alarm set - AlarmId=" << _InitAlarm << endmsg;
}

int CStdAppl::InitAlarm (ALARM_VALUES alarm)
{
   _InitAlarm = alarm ;

   if (_InitAlarm != NULL_ALARM)
   {
      //
      // Start to timer to set the alarm every 5 seconds, forever ...
      _InitAlarmTimer.init(5000, Callback<CStdAppl>(this, &CStdAppl::setInitializationAlarm));
   }

   return _InitAlarm;
}

CStdAppl::CStdAppl(const CStdAppl&)
{
   // Not implemented
}

/* FORMAT HASH bea54698f5ace5819f493cdac62cbe49 */
