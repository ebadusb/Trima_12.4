/*
 * Copyright (C) 2005 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */



#include <vxworks.h>
#include <time.h>

#include "power_fail.h"
#include "os_comm.h"
#include "portnumbers.h"
#include "in.h"




#ifdef SAFETY_BUILD
void trimaSysSaveSafetyPFData (char* data, unsigned int dataLength)
{

   struct sockaddr_in addr;
   int                clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
   unsigned int       cmd          = OSComm_WriteSafetyPFData;


   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = nodeData[SafetyNode].ipAddress[ControlNode];
   addr.sin_port        = htons(Trima_OSPort);

   sendto(clientSocket, (char*)&cmd, sizeof(cmd), 0, (struct sockaddr*)&addr, sizeof(addr));
   sendto(clientSocket, (char*)&dataLength, sizeof(dataLength), 0, (struct sockaddr*)&addr, sizeof(addr));
   sendto(clientSocket, data, dataLength, 0, (struct sockaddr*)&addr, sizeof(addr));
   close(clientSocket);
}
#else
void trimaSysSaveSafetyPFData (char* data, unsigned int dataLength) { printf("--Error-- CONTROL calling trimaSysSaveSafetyPFData\n"); }
#endif




#ifdef CONTROL_BUILD
void trimaSysWriteSafetyPFData (int serverSocket)
{


   int                rcvBytes;
   unsigned int       dataLength;
   char*              data;

   struct sockaddr_in clientAddr;
   int                clientAddrSize = sizeof(clientAddr);



   rcvBytes = recvfrom(serverSocket, (char*)&dataLength, sizeof(dataLength), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
   if ( rcvBytes > 0 && dataLength <= trimaSysSafetyPFSaveArea.nSecs * 512 - sizeof(dataLength) )
   {
      data     = (char*)malloc(trimaSysSafetyPFSaveArea.nSecs * 512);
      memcpy(data, &dataLength, sizeof(&dataLength));
      rcvBytes = recvfrom(serverSocket, (char*)&data[sizeof(dataLength)], dataLength, 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
      if ( rcvBytes > 0 )
      {
         trimaSysSafetyPFSaveArea.pBuf      = data;
         trimaSysSafetyPFSaveArea.direction = 1;
         ataRawio(0, 0, &trimaSysSafetyPFSaveArea);
      }

      free(data);
   }
}
#else
void trimaSysWriteSafetyPFData (int serverSocket) { printf("--Error-- SAFETY calling trimaSysWriteSafetyPFData\n"); }
#endif /* ifdef CONTROL_BUILD */



#ifdef CONTROL_BUILD
void trimaSysClearSafetyPFData (void)
{
   char* data = (char*)malloc(trimaSysSafetyPFSaveArea.nSecs * 512);
   memset(data, 0, trimaSysSafetyPFSaveArea.nSecs * 512);

   trimaSysSafetyPFSaveArea.pBuf      = data;
   trimaSysSafetyPFSaveArea.direction = 1;
   ataRawio(0, 0, &trimaSysSafetyPFSaveArea);
   free(data);
}
#else
void trimaSysClearSafetyPFData (void) { printf("--Error-- SAFETY calling trimaSysClearSafetyPFData\n"); }
#endif /* ifdef CONTROL_BUILD */



#ifdef CONTROL_BUILD
void trimaSysClearAllPFData (void)
{
   char* data = (char*)malloc(trimaSysControlPFSaveArea.nSecs * 512);
   memset(data, 0, trimaSysControlPFSaveArea.nSecs * 512);

   trimaSysControlPFSaveArea.pBuf      = data;
   trimaSysControlPFSaveArea.direction = 1;
   ataRawio(0, 0, &trimaSysControlPFSaveArea);
   free(data);

   data = (char*)malloc(trimaSysSafetyPFSaveArea.nSecs * 512);
   memset(data, 0, trimaSysSafetyPFSaveArea.nSecs * 512);

   trimaSysSafetyPFSaveArea.pBuf      = data;
   trimaSysSafetyPFSaveArea.direction = 1;
   ataRawio(0, 0, &trimaSysSafetyPFSaveArea);
   free(data);
}
#endif /* ifdef CONTROL_BUILD */

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/* the time sync functions */

#ifdef SAFETY_BUILD
void trimaSysReceiveTimeSync (int serverSocket)    /* in safety */
{
   struct sockaddr_in clientAddr;
   int                rcvBytes;
   char*              data;
   int                clientAddrSize = sizeof(clientAddr);

   data     = (char*)malloc(sizeof(struct timespec));
   rcvBytes = recvfrom(serverSocket, data, sizeof(struct timespec), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
   if (rcvBytes > 0)
   {
      /*printf("Safety received a time of %d from control. \n", ((struct timespec*)data)->tv_sec);*/
      clock_settime(CLOCK_REALTIME,  ((struct timespec*)data) );
   }
   free(data);


}
#else
void trimaSysReceiveTimeSync (int serverSocket) { printf("--Error-- CONTROL calling trimaSysReceiveTimeSync\n"); }
#endif /* ifdef SAFETY_BUILD */




#ifdef CONTROL_BUILD
void trimaSysSendSafetyTimeSync (void) /* in control */
{
   struct timespec    now;
   struct sockaddr_in addr;
   int                clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
   unsigned int       cmd          = OSComm_SendTimeData;
   unsigned int       dataLength   = -1;

   memset(&addr, 0, sizeof(addr));
   addr.sin_family      = AF_INET;
   addr.sin_addr.s_addr = nodeData[ControlNode].ipAddress[SafetyNode];
   addr.sin_port        = htons(Trima_OSPort);

   clock_gettime(CLOCK_REALTIME, &now);

   dataLength =  sizeof(struct timespec);
   sendto(clientSocket, (char*)&cmd, sizeof(cmd), 0, (struct sockaddr*)&addr, sizeof(addr));
   sendto(clientSocket, (char*)&now, sizeof(struct timespec), 0, (struct sockaddr*)&addr, sizeof(addr));
   close(clientSocket);

}
#else
void trimaSysSendSafetyTimeSync (void) { printf("--Error-- SAFETY calling trimaSysSendSafetyTimeSync\n"); }
#endif /* ifdef CONTROL_BUILD */


#ifdef SAFETY_BUILD
void trimaSysRequestSafetytimeSync (void)    /* in safety */
{
   osCommSend(OSComm_RequestTimeData);        // calls trimaSysSendSafetyTimeSync on control
}
#else
void trimaSysRequestSafetytimeSync (void) { printf("--Error-- CONTROL calling trimaSysSendSafetyTimeSync"); }
#endif /* ifdef SAFETY_BUILD */

/* FORMAT HASH 582f259ea1f990e12dd5499e2f0c1425 */
