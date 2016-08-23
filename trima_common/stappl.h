/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      stappl.h
 *             Defn of the CStdAppl class.  The behavior of a process
 *             is defined by deriving from this base class.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#if !defined( INCLUDE_STAPPL )
#define INCLUDE_STAPPL

#include "alarms.h"
#include "timermessage.h"

//
// Macro used to automatically place the filename, and linenumber in
//  the trace log.
//
#define KILL_ME(Cd) CStdAppl::Terminate(__FILE__, __LINE__, Cd)

class anAlarmMsg;

class CStdAppl
{
public:
   //
   // Function called only by me to terminate myself.
   //  Pass the name of the file, and line number of the function
   //  calling the terminate function.
   static void Terminate (const char* file, int lineNumber, int logCode);

public:
   // Default Constructor
   CStdAppl();

   // Default Destructor
   virtual ~CStdAppl();

   // Initialize the application, and create and set up all messages
   //  (i.e. do any initialization that needs to happen after the dispatcher
   //        is created )
   //
   // return NULL_ALARM -- Initialized properly
   //        {alarm code} - Initialize error
   //
   virtual ALARM_VALUES Init (void) = 0;

   // Cleanup any memory used by the object
   virtual void Cleanup (void);

   //
   // Callback function for the initialization alarm ...
   void setInitializationAlarm ();

   // Function to create the init alarm ...
   int InitAlarm (ALARM_VALUES alarm);

protected:
   //
   // Initialization alarm enum
   //
   ALARM_VALUES _InitAlarm;
   TimerMessage _InitAlarmTimer;

private:
   // Copy Constructor illegal
   CStdAppl(const CStdAppl&);
};


#endif

/* FORMAT HASH 70f18761e599d02bbb59b6cd10b5f324 */
