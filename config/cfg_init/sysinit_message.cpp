/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/config/cfg_init/rcs/sysinit_message.cpp 1.5 2008/01/23 17:11:02Z dslausb Exp dslausb $
 * $Log: sysinit_message.cpp $
 * Revision 1.4  2003/06/06 22:11:40Z  jl11312
 * - added handling for system shutdown
 * Revision 1.3  2003/05/21 19:52:40Z  jl11312
 * - enhanced memory protection (IT 6091)
 * Revision 1.2  2003/05/10 00:00:26Z  ms10234
 * 5993 - added a new task to connect with the remote gateways for the router.
 * Revision 1.1  2002/08/21 15:52:51Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include "systemoverrides.h"
#include "task_start.h"
#include "node_data.h"
#include "trima_tasks.h"

unsigned long getNetworkAddress (unsigned long remoteNode = 0)
{
   //
   // Get local node data and return IP address
   //
   SysNodeID nodeID = sysGetLocalNodeID();

   if ( remoteNode == 0 )
   {
      SysNodeData nodeData;

      sysGetNodeData(LocalNode, &nodeData);
      return nodeData.ipAddress[nodeID];
   }
   else
   {
      SysNodeData lNodeData;
      SysNodeData rNodeData;
      for ( SysNodeID lID = FirstNode; lID<EndOfNodeEntries; lID = (SysNodeID)(lID + 1) )
      {
         if ( lID != LocalNode )
         {
            sysGetNodeData(lID, &lNodeData);
            for ( SysNodeID rID = FirstNode; rID<EndOfNodeEntries; rID = (SysNodeID)(rID + 1) )
               if ( lNodeData.ipAddress[rID] == remoteNode )
               {
                  sysGetNodeData(rID, &rNodeData);
                  return rNodeData.ipAddress[nodeID];
               }
         }
      }
   }
   return 0;
}

void getNetworkedNodes (map< short, unsigned long >& nodesAndPorts)
{
   //
   // Scan list of known nodes and add data for each to map
   //
   SysNodeData lNodeData, rNodeData;
   SysNodeID   nodeID = sysGetLocalNodeID();
   sysGetNodeData(LocalNode, &lNodeData);
   sysGetNodeData(nodeID, &rNodeData);

   for ( SysNodeID gatewayNodeID = FirstNode; gatewayNodeID<EndOfNodeEntries; gatewayNodeID = (SysNodeID)(gatewayNodeID + 1) )
   {
      if ( rNodeData.gatewayPort[gatewayNodeID] != 0 )
      {
         nodesAndPorts[rNodeData.gatewayPort[gatewayNodeID]] = rNodeData.ipAddress[gatewayNodeID];
      }
   }
   nodesAndPorts[lNodeData.gatewayPort[nodeID]] = lNodeData.ipAddress[nodeID];
}

long spawnGateway (char* gatewayName, FUNCPTR func, const short portNo)
{
   //
   // Make sure task doesn't already exist
   //
   long tId = taskNameToId(gatewayName);
   if ( tId == ERROR )
   {
      //
      // Task does not exist, get information for task and spawn it
      //
      TrimaTaskInfo info;
      trimaGetTaskInfo(TTID_Gateway, &info);

      tId = taskStart(gatewayName, info.priority, info.stackSize, func, portNo, 0, 0, 0, 0, 0, 0, 0, 0, 0);
      trimaSetTaskID(TTID_Gateway, tId);
   }

   return tId;
}

long spawnConnector (char* connectorName, FUNCPTR func, const unsigned long netAddress, const int sock)
{
   //
   // Make sure task doesn't already exist
   //
   long tId = taskNameToId(connectorName);
   if ( tId == ERROR )
   {
      //
      // Task does not exist, get information for task and spawn it
      //
      TrimaTaskInfo info;
      trimaGetTaskInfo(TTID_Connector, &info);

      tId = taskStart(connectorName, info.priority, info.stackSize, func, netAddress, sock, 0, 0, 0, 0, 0, 0, 0, 0);
      trimaSetTaskID(TTID_Connector, tId);
   }

   return tId;
}

/* FORMAT HASH 4dafd4d3b25cb449a23d5be7884ce2a5 */
