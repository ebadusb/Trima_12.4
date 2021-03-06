
/*
 * $Header: //depot/main/embedded/Trima/Trima_6.0.6/vxworks_target/safety_bengal/node_data_local.c#1 $
 *
 * Trima safety node initialization
 *
 * $Log: node_data_local.c $
 * Revision 1.1  2007/08/28 18:25:04Z  rhecusb
 * Initial revision
 * 
 */

#define OS_BUILD

#include "node_data.c"
#include "filenames.h"

 
SysNodeID sysGetLocalNodeID(void)
{
   return SafetyNode;
}

const char * sysGetLocalNodeInitModule(void)
{
   return SAFETY_INIT_MODULE;
}



