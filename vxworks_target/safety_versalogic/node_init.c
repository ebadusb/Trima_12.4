/*
 * $Header: //bctquad3/home/BCT_Development/Trima5.R/Trima/vxworks_target/safety_versalogic/rcs/node_init.c 1.1 2007/11/08 01:06:19Z dslausb Exp rm70006 $
 *
 * TAOS safety node initialization
 *
 * $Log: node_init.c $
 * Revision 1.1  2007/11/08 01:06:19Z  dslausb
 * Initial revision
 * Revision 1.1  2007/08/28 18:25:05Z  rhecusb
 * Initial revision
 * Revision 1.5  2005/04/15 17:15:26Z  jl11312
 * - removed default network route (IT 60)
 * - use configuration values for network setup
 * Revision 1.4  2005/01/18 18:16:47Z  ms10234
 * - changes for first revision of startup code
 * Revision 1.3  2004/10/29 00:49:25Z  ms10234
 * - added the ability to switch into service mode
 * Revision 1.2  2004/06/07 14:26:52Z  ms10234
 * removed watchdog task
 * Revision 1.1  2004/05/20 17:23:16Z  jl11312
 * Initial revision
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

extern void usrCommonInitSetError( const char *msg );

extern int shellTaskPriority;

void nodeInit(void)
{	
   /* load option files (hw.dat, features.bin) needed early in the boot sequence */

	if ( trimaSysLoadOptionFiles() < 0 )
	{
		usrCommonInitSetError("failed to load system option files");
	}

   loadTrimaHWDriver("safety_hardware_driver");   
}



