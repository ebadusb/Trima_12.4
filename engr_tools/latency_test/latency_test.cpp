/*
 * latency_test.cpp
 *
 * Test program for checking enforcement of message latency by safety.  Messages
 * of type RequestSafetyDriverStatus or SafetyHardwareStatus are received and
 * immediately sent back out with the time stamp modified.  The message type and
 * amount of change to the time stamp are set with command line arguments (see
 * Usage() for details).
 *
 * $Header$
 * $Log$
 *
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 */
 
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/kernel.h>
#include <sys/qnx_glob.h>
#include <sys/sched.h>
#include <sys/psinfo.h>
#include <sys/mman.h>

#include "buffmsg.hpp"
#include "crc.h"
#include "crcgen.h"
#include "dispatch.hpp"
#include "error.h"
#include "kernaltime.hpp"
#include "msg.hpp"
#include "smsginfo.hpp"
#include "safe_msg.hpp"
#include "sinver.h"

// public data

dispatcher* dispatch = NULL;                 // message dispatcher
int         msecDelay = 0;                   // apparent delay time for messages

class aSafetyStatusMsg_rcvr : public focusBufferMsg<SHwStates>
{
private:
   SHwStates   SHwStateData;

public:
   aSafetyStatusMsg_rcvr(void) : focusBufferMsg<SHwStates>(SafetyHardwareStatus, NO_BOUNCE) { memset(&SHwStateData, 0, sizeof(SHwStates)); }
   virtual void notify(void);
};

class aRequestSafetyStatus_rcvr : public focusInt32Msg
{
public:
   aRequestSafetyStatus_rcvr(void) : focusInt32Msg(focusInt32Msg::RequestSafetyDriverStatus, NO_BOUNCE) { }
   virtual void notify(void);
};

class timeDispatcher : public dispatcher
{
public:
   timeDispatcher( int argc, char** argv, kTime* kt);
   ~timeDispatcher();

   virtual void send ( routeBuffer * m );
   static void SetTimeStamp(int sec, int nanoSec) { timeStampSec = sec; timeStampNanoSec = nanoSec; }

private:
   kTime * _timeTeller;
   static int  timeStampSec, timeStampNanoSec;

   timeDispatcher();
   timeDispatcher( timeDispatcher const & );
   timeDispatcher& operator=( timeDispatcher const& );
};

int timeDispatcher::timeStampSec = 0;
int timeDispatcher::timeStampNanoSec = 0;

timeDispatcher::timeDispatcher(int argc, char** argv, kTime* kt) :
   dispatcher(argc, argv),
   _timeTeller(kt)
{
   ASSERT(_timeTeller);
}

timeDispatcher::~timeDispatcher()
{
   _timeTeller = NULL;
}

void
timeDispatcher::send( routeBuffer* m )
{
   if (m == NULL)                            // internal check
   {
      fError( __LINE__, 0, "msg length");
   }

// range checks

   MSGHEADER* mhdr = (MSGHEADER*) m->message;
   unsigned short mid = mhdr->msgID;
   logData( __LINE__, mid, MSG_SEND);
   unsigned int mLen = mhdr->length;
   if (mLen > BSIZE)
   {
      fError( __LINE__, mLen, "message length too large");
   }


// send message to router

   mhdr->osCode = MSG_MULTICAST;             // tell router to multicast message
   mhdr->taskPID = getpid();                   // task PID
   mhdr->taskNID = getnid();                   // task NID

   mhdr->sendTime.tv_sec = timeStampSec;
   mhdr->sendTime.tv_nsec = timeStampNanoSec;
   updateFocusMsgCRC( mhdr);                 // update CRC

   unsigned short k = 0;

// place message in router's input queue
   while((mq_send( routerQueue, mhdr, mhdr->length, 0) == MQ_ERROR) &&
         (taskRunning) )
   {
      if((errno==EINTR) && (k<RETRY_COUNT))  // signals
      {
         k++;                                // try again
      }
      else                                   // fatal error
      {
         fError( __LINE__, 0, "mq_send()");
         break;
      }
   }
}

void aSafetyStatusMsg_rcvr::notify(void)
{
   pid_t pid;
   nid_t nid;
   struct timespec sendTime;

   get( &SHwStateData );
   focusBufferMsg::routeBuffer::msgHeader( pid,nid,sendTime );
   sendTime.tv_nsec -= msecDelay*1000000;
   while ( sendTime.tv_nsec < 0 )
   {
      sendTime.tv_nsec += 1000000000;
      sendTime.tv_sec -= 1;
   }

   timeDispatcher::SetTimeStamp(sendTime.tv_sec, sendTime.tv_nsec);
   set( &SHwStateData );

   kill(getpid(), SIGQUIT);
}

void aRequestSafetyStatus_rcvr::notify(void)
{
   pid_t pid;
   nid_t nid;
   struct timespec sendTime;

   focusInt32Msg::routeBuffer::msgHeader( pid,nid,sendTime );
   sendTime.tv_nsec -= msecDelay*1000000;
   while ( sendTime.tv_nsec < 0 )
   {
      sendTime.tv_nsec += 1000000000;
      sendTime.tv_sec -= 1;
   }

   timeDispatcher::SetTimeStamp(sendTime.tv_sec, sendTime.tv_nsec);
   set();

   kill(getpid(), SIGQUIT);
}

void Usage( void )
{
   fprintf(stderr, "Usage: latency_test [-r | -s] [-d msec_delay]\n");
   fprintf(stderr, " -r selects RequestSafetyStatus test\n");
   fprintf(stderr, " -s selects SafetyStatusMsg test\n");
   fprintf(stderr, " -d specifies the delay (in msec) for the message\n");

   if ( dispatch )
   {
      delete dispatch;
   }

   exit(1);
}

void main(int argc, char * argv[])
{
   int   argsOK = 0;
   int   parseArg = 1;
   enum { UnknownTest, RequestSafetyStatusTest, SafetyStatusMsgTest }   testType = UnknownTest;

   while ( parseArg < argc )
   {
      if ( strcmp(argv[parseArg], "-r") == 0 &&
           testType == UnknownTest )
      {
         testType = RequestSafetyStatusTest;
         parseArg += 1;
      }
      else if ( strcmp(argv[parseArg], "-s") == 0 &&
                testType == UnknownTest )
      {
         testType = SafetyStatusMsgTest;
         parseArg += 1;
      }
      else if ( strncmp(argv[parseArg], "-d", 2) == 0 &&
                msecDelay == 0 )
      {
         if ( strlen(argv[parseArg]) > 2 )
         {
            if ( sscanf(argv[parseArg]+2, "%d", &msecDelay) != 1 )
            {
               Usage();
            }
            else
            {
               parseArg += 1;
            }
         } else if ( parseArg+1 < argc )
         {
            if ( sscanf(argv[parseArg+1], "%d", &msecDelay) != 1 )
            {
               Usage();
            }
            else
            {
               parseArg += 2;
            }  
         }
      }
      else
      {
         Usage();
      }
   }

   // create a kernal time access object
   kTime* kt = new kTime();
 
   //
   // create dispatcher for test - note that this tool is expected to be run on safety
   // and will not work properly on the control side.
   //
   int d_argc = 4;
   char* d_argv[5];
   d_argv[0] = argv[0];
   d_argv[1] = getenv("CQUE");
   if(d_argv[1] == 0)
   {
      d_argv[1] = "c";
   }
   d_argv[2] = getenv("SNODE");
   if(d_argv[2] == 0)
   {
      d_argv[2] = "1";
   }
   d_argv[3] = getenv("SQUE");
   if(d_argv[3] == 0)
   {
      d_argv[3] = "s";
   }
   d_argv[4] = 0;

   timeDispatcher* td = new timeDispatcher( d_argc, d_argv, kt );
   dispatch = (dispatcher *)td;

   aSafetyStatusMsg_rcvr   *     safetyStatusMsg;
   aRequestSafetyStatus_rcvr *   requestSafetyStatus;

   switch ( testType )
   {
   case RequestSafetyStatusTest:
      requestSafetyStatus = new aRequestSafetyStatus_rcvr();
      break;

   case SafetyStatusMsgTest:
      safetyStatusMsg = new aSafetyStatusMsg_rcvr();
      break;

   default:
      Usage();
   }

   dispatch->dispatchLoop();
   exit(0);
}

