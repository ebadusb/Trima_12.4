/*
 * $Header: E:/BCT_Development/TrimaVxPort/Trima/config/rcs/node_data.c 1.1 2007/05/22 18:03:16Z dslausb Exp $
 *
 * Node Data common functions
 *
 * $Log: node_data.c $
 * Revision 1.1  2007/05/22 18:03:16Z  dslausb
 * Initial revision
 *
 */
#include <vxWorks.h>
#include <string.h>

#include "node_data.h"

int sysGetNodeData (SysNodeID id, SysNodeData* result)
{
   int retVal = -1;

   if (id >= FirstNode && id < EndOfNodeEntries)
   {
      memcpy(result, &nodeData[id], sizeof(SysNodeData));
      retVal = 0;
   }

   return retVal;
}

SysNodeID sysGetNodeID (unsigned long networkAddress)
{
   SysNodeID   nodeID = EndOfNodeEntries;
   SysNodeData lNodeData;
   SysNodeID   rID;

   sysGetNodeData(sysGetLocalNodeID(), &lNodeData);

   for (rID = FirstNode; rID<EndOfNodeEntries; rID = (SysNodeID)(rID + 1))
   {
      if (lNodeData.ipAddress[rID] == networkAddress)
      {
         nodeID = rID;
         break;
      }
   }

   return nodeID;
}

SysNodeID sysGetNodeIDFromString (const char* nodeName)
{
   SysNodeID   nodeID = EndOfNodeEntries;
   SysNodeData lNodeData;
   SysNodeID   rID;

   for (rID = FirstNode; rID<EndOfNodeEntries; rID = (SysNodeID)(rID + 1))
   {
      if (strcmp(nodeData[rID].name, nodeName) == 0)
      {
         nodeID = rID;
         break;
      }
   }

   return nodeID;
}


const char* sysGetNodeIDString (SysNodeID id)
{
   return nodeData[sysGetLocalNodeID()].name;
}

/* FORMAT HASH a18320901673dd77babc730ca5849d32 */
