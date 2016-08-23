/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/config/cfg_init/tftpd.cpp#1 $
 *
 */

#include <vxWorks.h>
#include <errnoLib.h>
#include <fcntl.h>
#include <ioLib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sysLib.h>

#include <in.h>
#include <inetLib.h>
#include <selectLib.h>
#include <sockLib.h>
#include <tftpLib.h>

#include "task_start.h"
#include "tftpd.h"
#include "trima_datalog.h"
#include "trima_tasks.h"

static void tftpdNotifyError (int peerSocket, unsigned short errorCode, const char* errorMsg)
{
   struct ErrorPacket
   {
      unsigned short opcode;
      unsigned short errcode;
      char           errmsg[1];
   };

   size_t       packetSize  = sizeof(ErrorPacket) + strlen(errorMsg);
   ErrorPacket* errorPacket = (ErrorPacket*)malloc(packetSize);
   if (errorPacket)
   {
      errorPacket->opcode  = htons(TFTP_ERROR);
      errorPacket->errcode = htons(errorCode);
      strcpy(errorPacket->errmsg, errorMsg);
      send(peerSocket, (char*)errorPacket, packetSize, 0);
      free(errorPacket);
   }
}

static void printHdr (TFTP_MSG* msg)
{
   printf("TFPT HDR: Op(%d), ", ntohs(msg->th_opcode));

   switch (ntohs(msg->th_opcode))
   {
      case TFTP_RRQ :
      case TFTP_WRQ :
         printf("filename(%s)\n", msg->th.request);
         break;

      case TFTP_DATA :
      case TFTP_ACK :
         printf("block #(%d)\n", msg->th.misc.blockOrError);
         break;

      case TFTP_ERROR :
         printf("error #(%d), msg=(%s)\n", msg->th.misc.blockOrError, msg->th.request);
         break;

      default :
         printf("Bogus code!\n");
   }

}
static bool tftpdSendFile (int peerSocket, TFTP_MSG* tftpMsg)
{
   struct timeval readTimeout = {1, 0};  // Don't need to wait the full 2 seconds (TFTP_TIMEOUT) for point to point.
   const char*    fileName    = tftpMsg->th_request;

   int            fd          = open(fileName, O_RDONLY, 0666);

   if (fd < 0)
   {
      printf("failed to open \"%s\" for tftpd transfer: %d\n", fileName, errnoGet());

      tftpdNotifyError(peerSocket, ENOTFOUND, "file not found");

      return false;
   }

   fd_set         readSet;
   int            bytesRead;
   unsigned short block = 1;
   int            retry = 0;
   bool           done  = false;

   // read the first chunk of the file to transfer
   bytesRead = read(fd, tftpMsg->th_data, TFTP_SEGSIZE);

   while (retry < 2 && !done)
   {
      if (retry > 0)
      {
         printf("retrying block %d, retry count %d\n", block, retry);
      }

      tftpMsg->th_opcode = htons(TFTP_DATA);
      tftpMsg->th_block  = htons(block);

      const unsigned int msgSize   = sizeof(TFTP_MSG) + bytesRead - TFTP_SEGSIZE;
      const int          bytesSent = send(peerSocket, (char*)tftpMsg, msgSize, 0);

      if (bytesSent != msgSize)
      {
         perror("tftp send failed");
         printf("bytes sent %d does not equal %d.  Error Code =%d\n", bytesSent, msgSize, errnoGet());
      }

      bool ackOK = false;

      FD_ZERO(&readSet);
      FD_SET(peerSocket, &readSet);

      const int bytesAvailable = select(FD_SETSIZE, &readSet, NULL, NULL, &readTimeout);

      if (bytesAvailable > 0)
      {
         recv(peerSocket, (char*)tftpMsg, sizeof(TFTP_MSG), 0);

         if (htons(tftpMsg->th_opcode) == TFTP_ACK)
         {
            ackOK = true;
         }
      }
      else
      {
         DataLog(log_level_startup_error) << "tftpd socket timeout on block " << block << endmsg;
      }

      if (ackOK)
      {
         block += 1;
         retry  = 0;

#undef DEBUG_PRINTING
#define DEBUG_PRINTING 0
#if DEBUG_PRINTING
         // RKM.  Print out the progress.
         if (block % 40 == 0)
         {
            printf(".");
         }
#endif

         if (bytesRead == TFTP_SEGSIZE)
         {
            // read another chunk of the file to transfer
            bytesRead = read(fd, tftpMsg->th_data, TFTP_SEGSIZE);
         }
         else
         {
            done = true;

#if DEBUG_PRINTING
            printf("\n"); // RKM
#endif
         }
      }
      else
      {
         retry += 1;
      }
   }

   close(fd);

   return done;
}

static bool tftpdEndRequest;
static void tftpd (tftpdStatusCallBack* statusCallBack)
{
   int serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

   sockaddr_in addr;
   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = INADDR_ANY;
   addr.sin_port        = htons((u_short)TFTP_PORT);
   bind(serverSocket, (sockaddr*)&addr, sizeof(addr));

   TFTP_MSG tftpMsg;
   struct sockaddr_in clientAddr;
   int clientSocket;
   int clientAddrSize = sizeof(clientAddr);
   int bytesReceived;

   fd_set serverSet;
   struct timeval readTimeout = { 15, 0};

   printf("Starting TFTPD\n");

   while (!tftpdEndRequest)
   {
      // Need to zero the sockets before each call
      FD_ZERO(&serverSet);
      FD_SET(serverSocket, &serverSet);

      if (select(FD_SETSIZE, &serverSet, NULL, NULL, &readTimeout) > 0)
      {
         int peerSocket = socket(AF_INET, SOCK_DGRAM, 0);
         sockaddr_in peerAddr;
         memset(&peerAddr, 0, sizeof(peerAddr));  /* proper addressing is AF_INET, with addr and port set to 0 */
         peerAddr.sin_family      = AF_INET;
         peerAddr.sin_addr.s_addr = INADDR_ANY;
         peerAddr.sin_port        = 0;

         const int rcvBytes = recvfrom(serverSocket, (char*)&tftpMsg, sizeof(tftpMsg), 0, (sockaddr*)&clientAddr, &clientAddrSize);

         if (rcvBytes > 0)
         {
            const char* fileName = tftpMsg.th_request;
            char remoteHost[INET_ADDR_LEN];
            inet_ntoa_b (clientAddr.sin_addr, remoteHost);

            if (bind(peerSocket, (sockaddr*)&peerAddr, sizeof(peerAddr)) == ERROR)
            {
               perror("tftpd::bind");
               tftpdNotifyError(peerSocket, EACCESS, "bind to remote host failed");
               shutdown(peerSocket, 2);
               close(peerSocket);
               continue;
            }

            if (connect(peerSocket, (sockaddr*)&clientAddr, clientAddrSize) == ERROR)
            {
               perror("tftpd::connect");
               tftpdNotifyError(peerSocket, EACCESS, "connect to remote host failed");
               shutdown(peerSocket, 2);
               close(peerSocket);
               continue;
            }

            tftpMsg.th_opcode = ntohs(tftpMsg.th_opcode);

            if (tftpMsg.th_opcode == TFTP_RRQ)
            {
               // Validate the address and update the state machine
               if (statusCallBack(TFTPD_FileTransferRequest, clientAddr.sin_addr, tftpMsg.th_request))
               {
                  printf("sending %s to host %s\n", fileName, remoteHost);

                  if (tftpdSendFile(peerSocket, &tftpMsg))
                  {
                     // Update the tftpd state machine and print out the fact that the file was
                     // successfully transferred
                     statusCallBack(TFTPD_FileTransferCompleted, clientAddr.sin_addr, NULL);
                  }
                  else
                     printf("tftpd file send failed.  Listening for new request...\n");
               }
               else
               {
                  printf("file access not allowed\n");
                  tftpdNotifyError(peerSocket, EACCESS, "file access not allowed");
               }
            }
            else
            {
               printf("bad tftpd request %d ignored\n", tftpMsg.th_opcode);
               tftpdNotifyError(peerSocket, EACCESS, "tftpd supports only read requests");
            }
         }
         else
         {
            perror("tftpd recvfrom socket error:");
            taskDelay(30);
         }

         shutdown(peerSocket, 2);
         close(peerSocket);
      }
#if 0   // Debug
      else
      {
         perror("tftp select timeout\n");
      }
#endif
   }

   shutdown(serverSocket, 2);
   close(serverSocket);
}

void startTFTPD (tftpdStatusCallBack* statusCallBack)
{
   tftpdEndRequest = false;

   TrimaTaskInfo info;
   trimaGetTaskInfo(TTID_TFTP, &info);

   int tid = taskStart(info.taskName, info.priority, info.stackSize, (FUNCPTR)tftpd, (int)statusCallBack, 0, 0, 0, 0, 0, 0, 0, 0, 0);
   trimaSetTaskID(TTID_TFTP, tid);
}

void endTFTPD (void)
{
   tftpdEndRequest = true;
}

/* FORMAT HASH 6b6517bad15b6a425a9bba227a21e8b4 */
