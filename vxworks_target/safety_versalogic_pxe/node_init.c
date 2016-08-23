/*
 * $Header$
 *
 * Trima safety node initialization
 *
 */

#include <vxWorks.h>
#include <stdio.h>
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
#include <taskLib.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include <setjmp.h>

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

/* declared in platform_init.cpp */
extern void usrCommonInitSetError( const char *msg );
extern jmp_buf usrInitAbort;


END_OBJ * gei82543EndLoad(char * initString, void * bsp);

static void usrSetupNFS(void)
{
   int mount;
   int errors = 0;

   for (mount = 0; mount < NFS_MOUNT_POINTS; mount+=1)
   {
      printf("mounting %s\n", nfsMountData[mount]._mountPath); 

      if (nfsMount("control", (char *)nfsMountData[mount]._mountPath, (char *)nfsMountData[mount]._mountPath) != OK)
      {
         perror("mount failed");

         if (nfsMountData[mount]._mountRequired)
         {
            errors = 1;
         }
      }
   }

   if (errors)
   {
      longjmp(usrInitAbort, 1);
   }
}



void nodeInit(void)
{	
	SysNodeData	controlNodeData, safetyNodeData;
	struct in_addr ipAddress;
	char		inetAddr[INET_ADDR_LEN]; /* buffer for inet address */
	void *cookie;

	cookie = muxDevLoad(0, gei82543EndLoad, "-1:0x00:0x20:0x20:0x00", TRUE, NULL);

    if (muxDevStart(cookie) == ERROR)
    {
       perror("muxDevStart");
       longjmp(usrInitAbort, 1);
    }

	ipAttach(0, "gei");

	sysGetNodeData( ControlNode, &controlNodeData );
	sysGetNodeData( SafetyNode, &safetyNodeData );

	if (ifMaskSet("gei0", 0xffffff00) == ERROR)
    {
       perror("ifMaskSet");
       longjmp(usrInitAbort, 1);
    }

	/* Set up the safety network address */
	ipAddress.s_addr = controlNodeData.ipAddress[SafetyNode];
	inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */

    if (ifAddrSet("gei0", inetAddr) == ERROR)
    {
       perror("ifAddrSet");
       longjmp(usrInitAbort, 1);
    }

	ipAddress.s_addr = safetyNodeData.ipAddress[ControlNode];
	inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */

    if (hostAdd("control_safety", inetAddr) == ERROR)
    {
       perror("hostAdd");
       longjmp(usrInitAbort, 1);
    }

	usrSetupNFS();

   /* load option files (hw.dat, features.bin) needed early in the boot sequence */
	if ( trimaSysLoadOptionFiles() < 0 )
	{
		usrCommonInitSetError("failed to load system option files");
	}

   loadTrimaHWDriver("safety_hardware_driver");   
}

