
/*
 * $Header: //depot/main/embedded/Trima/Trima_6.0.6/vxworks_target/control_fox/node_data_local.c#1 $
 *
 * Trima control node initialization
 *
 * $Log: node_data_local.c $
 * Revision 1.1  2007/08/28 18:19:57Z  rhecusb
 * Initial revision
 * 
 */

#define OS_BUILD

#include "config/node_data.c"
#include "filenames.h"

 
SysNodeID sysGetLocalNodeID(void)
{
   return ControlNode;
}

const char * sysGetLocalNodeInitModule(void)
{
   return CONTROL_INIT_MODULE;
}



