/*
 * $Header: //depot/main/embedded/Trima/Trima_6.0.6/vxworks_target/safety_bengal/node_init.c#3 $
 *
 * Trima safety node initialization
 *
 */

#include <vxWorks.h>
#include <end.h>
#include <hostLib.h>
#include <ftpLib.h>
#include <ifLib.h>
#include <inetLib.h>
#include <ipProto.h>
#include <mountLib.h>
#include <muxLib.h>
#include <nfsDrv.h>
#include <routeLib.h>
#include <shellLib.h>
#include <stdio.h>
#include <taskLib.h>
#include <symLib.h>
#include <sysSymTbl.h>

#include "config.h"
#include "os/os_comm.h"
#include "os/trima_options.h"
#include "os/load_module.h"
#include "os/trima_driver.h"

#define OS_BUILD
#include "config/file_system.h"
#include "config/nfs_data.h"
#include "node_data.h"
#include "portnumbers.h"
#include "trima_tasks.h"
#undef OS_BUILD

void nodeInit(void);

/* declared in platform.cpp */
extern void usrCommonInitSetError( const char *msg );

static void usrSetupNFS(void)
{
   int mount;

   for (mount = 0; mount < NFS_MOUNT_POINTS; mount += 1)
   {
      printf("mounting %s\n", nfsMountData[mount]._mountPath); 

      if (nfsMount("control", (char *)nfsMountData[mount]._mountPath, (char *)nfsMountData[mount]._mountPath) != OK)
         perror("mount failed");
   }
}

void nodeInit(void)
{
   SysNodeData safetyNodeData;
   struct in_addr ipAddress;
   char inetAddr[INET_ADDR_LEN]; /* buffer for inet address */

   /*
    * It's assumed the Ethernet driver has been setup (loaded, started, attached and
    * masked) via bootrom boot-line parameters and END table settings in configNet.h
    */

   sysGetNodeData( SafetyNode, &safetyNodeData );
   ipAddress.s_addr = safetyNodeData.ipAddress[ControlNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */
   hostAdd("control_safety", inetAddr);

   usrSetupNFS();

    /* load option files (hw.dat, sw.dat) needed early in the boot sequence */
   if (trimaSysLoadOptionFiles() < 0)
   {
      usrCommonInitSetError("failed to load system option files");
   }

   loadTrimaHWDriver("safety_hardware_driver");   
}

