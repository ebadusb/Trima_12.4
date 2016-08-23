/*
 * Copyright (c) 1998 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      stdmain.cpp
 *
 */

#include <vxworks.h>
#include <taskLib.h>

#include "messagesystem.h"
#include "stmain.h"


CStdMain::CStdMain()
   : m_pApp(0),
     m_messageSystem()
{}

CStdMain::~CStdMain()
{
   Cleanup();
}

int CStdMain::Init (int argc, char** argv, int qSize)
{
   const char* myTaskName = taskName(taskIdSelf());

   if (myTaskName == NULL)
   {
      _FATAL_ERROR(__FILE__, __LINE__, "Create a task with a null name");
   }
   else
   {
      m_messageSystem.initBlocking(myTaskName, qSize);
#ifndef SIMNT
      trimaSetRouterQ(taskIdSelf(), m_messageSystem.MsgSystem());
#endif
   }

   return 1;
}

int CStdMain::InitApp (CStdAppl* pApp)
{
   if (pApp == 0)
   {
      DataLog(log_level_trima_library_error) << "Failed to create derived standard app class" << endmsg;
      return 0;
   }

   m_pApp = pApp;

   //
   // Initialize all application specific stuff
   //  ( set up the alarm in case of initialization errors )
   //
   m_pApp->InitAlarm(m_pApp->Init());

   return 1;
}

int CStdMain::Process ()
{
   int numMessages = m_messageSystem.dispatchMessages();
   DataLog(log_level_trima_library_info) << "Exit dispatchMessages() with " << numMessages << " on the message queue." << endmsg;

   return 1;
}



void CStdMain::Cleanup ()
{
   m_pApp->Cleanup();

   m_pApp = 0;
}

/* FORMAT HASH f5c127f6de25fb8ec7d8af1aa25ccd79 */
