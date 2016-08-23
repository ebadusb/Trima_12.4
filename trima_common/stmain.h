/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      stmain.h
 *             Defn of the CStdMain class used by the MAIN macro when
 *             setting up processes.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/


#if !defined( INCLUDE_STMAIN )
#define INCLUDE_STMAIN

#include "stappl.h"

#include "trima_datalog.h"
#include "messagesystem.h"

#if CPU!=SIMNT
#include "trima_tasks.h"
#else
#define TASK_DONE 0
#endif

// Needed for predictionator
#ifndef WIN32
#include "error.h"
#endif


// Standard Mainline processing for PROC processes
class CStdMain
{
public:
   CStdMain();
   virtual ~CStdMain();

   // Create the dispatcher
   int Init (int argc, char** argv, int qSize);

   // Initialize the application
   int InitApp (CStdAppl* pApp);

   // Start dispatch loop
   int Process (void);

protected:

   // Cleanup member data memory
   void Cleanup ();

protected:
   // Pointer to the application object
   //
   // StdMain has ownership
   //
   CStdAppl*     m_pApp;
   MessageSystem m_messageSystem;
};



//
//   Macro definition for the main function for each process in 'proc'
//
//   'NEW_STD_APPL_CLASS' must be derived off of StdAppl.
//   'QSIZE'              the queue size specified for the dispatcher.
//
#define MAIN2(NEW_STD_APPL_CLASS, QSIZE) \
    \
   extern "C" int NEW_STD_APPL_CLASS##_main(int argc, char** argv) \
   { \
      CStdMain theMain; \
     \
      if ( !theMain.Init(argc, argv, QSIZE) ) \
      { \
         DataLog(log_level_trima_library_error) << "Failed to init the StdMain Class" << endmsg; \
         return 1; \
      } \
      \
      if ( !theMain.InitApp(new NEW_STD_APPL_CLASS) ) \
      {\
         return 1; \
      } \
      \
      \
      /* Let sysinit know that we are finished with our message initialization */ \
      /* and it is safe to continue booting the other tasks. */ \
      TASK_DONE; \
      \
      if ( !theMain.Process() ) \
      { \
         DataLog(log_level_trima_library_error) << "Process ended abnormally." << endmsg; \
         return 1; \
      } \
     \
      return 0; \
   }

#define MAIN(NEW_STD_APPL_CLASS) MAIN2(NEW_STD_APPL_CLASS, 30)

#endif

/* FORMAT HASH dae7a9f04d70e84ba9c77f21aa5eef39 */
