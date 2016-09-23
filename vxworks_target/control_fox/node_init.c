/*
 * $Header: //depot/main/embedded/Trima/Trima_6.0.6/vxworks_target/control_fox/node_init.c#5 $
 *
 * Trima control node initialization
 *
 */

#include <vxWorks.h>

#include <muxLib.h>

#include <arpLib.h>    /* include before dhcps */
#include <ipProto.h>
#include <hostLib.h>


/* PXE Server dummy function */
STATUS pxeServer(void);


/* local functions */
static void usrSetupNetworkApps(void);
static void usrInitNetworkDevices(void);

/* include the common node init function for Control */
#include "controlFunctions.c"

static void usrSetupNetworkApps(void)
{
   if (bootPromptSingleStep("initialize network software") == SingleStep_Abort)
   {
      longjmp( usrInitAbort, 1 ); 
   }

   /* Nothing to do here locally */
}


static void usrInitNetworkDevices(void)
{
   char *       safetyIfStr = 0;
   char *       externalIfStr = 0;
   SysNodeData  controlNodeData, safetyNodeData;
   struct       in_addr ipAddress;
   char         inetAddr[INET_ADDR_LEN]; /* buffer for inet address */
   void         *cookie;
   int          status = OK;

   sysGetNodeData( ControlNode, &controlNodeData );
   sysGetNodeData( SafetyNode, &safetyNodeData );

   if (!getenv("EXTERNALIP"))
   {
      usrCommonInitSetError("missing EXTERNALIP setting in globvars - defaulting to 172.21.127.255");
      putenv("EXTERNALIP=172.21.127.255");
   }

   /*
    * Note: Ethernet drivers have been loaded during kernel initialization via
    * the END table settings in configNet.h
    */

   /* Setup the external network interface */
   externalIfStr = "rdc0";
   status = ipAttach(0, "rdc");
   if (status != OK) perror("ipAttach(rdc0)");

   /* set up the external addr and mask */
   ifAddrSet(externalIfStr, getenv("EXTERNALIP"));
   ifMaskSet(externalIfStr, 0xffff0000);

   /* Get the address of control as seen by safety */
   ipAddress.s_addr = safetyNodeData.ipAddress[ControlNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */

   /* Setup the internal ethernet, control <-> safety */
   safetyIfStr = "gei0";
   status = ipAttach(0, "gei");
   if (status != OK) perror("ipAttach(gei0)");

   /* set up the internal addr and mask */
   ifMaskSet(safetyIfStr, 0xffffff00);
   ifAddrSet(safetyIfStr, inetAddr);

   printf("internal network using device %s at address %s\n", safetyIfStr, inetAddr);
   printf("external network using device %s at address %s\n", externalIfStr, getenv("EXTERNALIP"));

   /*
    *  Add host names
    */
   hostAdd("control_external", getenv("EXTERNALIP"));
   hostAdd("control", getenv("EXTERNALIP"));
   hostAdd("control_safety", inetAddr);

   /* Get the address of safety as seen by control */
   ipAddress.s_addr = controlNodeData.ipAddress[SafetyNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */
   hostAdd("safety", inetAddr);
   
   /* set the gateway address */
   usrInitGateway(getenv("EXTERNALIP"));
}
