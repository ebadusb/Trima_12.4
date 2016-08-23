/*
 * $Header$
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
   char  *safetyIfStr = 0;
   char  *externalIfStr = 0;
   SysNodeData  controlNodeData, safetyNodeData;
   struct       in_addr ipAddress;
   char         inetAddr[INET_ADDR_LEN]; /* buffer for inet address */
   void         *cookie;

   sysGetNodeData( ControlNode, &controlNodeData );
   sysGetNodeData( SafetyNode, &safetyNodeData );

   if (!getenv("EXTERNALIP"))
   {
      usrCommonInitSetError("missing EXTERNALIP setting in globvars - defaulting to 172.21.127.255");
      putenv("EXTERNALIP=172.21.127.255");
   }

   /* Setup the first ethernet, control <-> safety */
   if (endDevTbl[0].endLoadFunc == END_TBL_END)
   {
      /* Setup the external network interface */
      cookie = muxDevLoad(1, esmcEndLoad, "0x360:0x2a:0x0a:3:0:0", TRUE, NULL);
      muxDevStart(cookie);

      externalIfStr = "esmc1";
      ipAttach(1, "esmc");

      /* Setup the internal ethernet, control <-> safety */
      cookie = muxDevLoad(0, ne2000EndLoad, "0x320:0x25:0x05:0:1:0", TRUE, NULL);
      muxDevStart(cookie);

      safetyIfStr = "ene0";
      ipAttach(0, "ene");
   }
   else
   {
      /* Setup the external network interface */
      externalIfStr = "fei0";
      ipAttach(0, "fei");

      /* Setup the internal ethernet, control <-> safety */
      safetyIfStr = "fei1";
      ipAttach(1, "fei");
   }

   /* set up the external addr and mask */
   ifMaskSet(externalIfStr, 0xffff0000);
   ifAddrSet(externalIfStr, getenv("EXTERNALIP"));

   /* set up the internal addr and mask */
   ifMaskSet(safetyIfStr, 0xffffff00);

   /* Get the address of control as seen by safety */
   ipAddress.s_addr = safetyNodeData.ipAddress[ControlNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */
   ifAddrSet(safetyIfStr, inetAddr);

   printf("internal network using device %s at address %s\n", safetyIfStr, inetAddr);
   printf("external network using device %s at address %s\n", safetyIfStr, getenv("EXTERNALIP"));

   /*
    *  Add host names
    */
   hostAdd("control_external", getenv("EXTERNALIP"));
   hostAdd("control", getenv("EXTERNALIP"));


   /* Get the address of control as seen by safety */
   ipAddress.s_addr = safetyNodeData.ipAddress[ControlNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */
   hostAdd("control_safety", inetAddr);

   ipAddress.s_addr = controlNodeData.ipAddress[SafetyNode];
   inet_ntoa_b(ipAddress, inetAddr); /* convert numeric IP to string */
   hostAdd("safety", inetAddr);
   
   /* set the gateway address */
   usrInitGateway(getenv("EXTERNALIP"));
}
