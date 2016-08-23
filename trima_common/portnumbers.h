/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * Trima network port numbers
 *
 * $Header$
 *
 */

#ifndef _PORTNUMBERS_INCLUDE
#define _PORTNUMBERS_INCLUDE

enum
{
   Telnet_ServerPort        = 23,          /* port for Telnet server (included in operating system impage */
   DHCP_ServerPort          = 67,          /* port for DHCP server (included in operating system image */
   DHCP_ClientPort          = 68,          /* port for DHCP client (included in operating system image */
   Trima_SafetyGatewayPort  = 2221,        /* port for safety message system gateway */
   Trima_ControlGatewayPort = 2222,        /* port for control message system gateway */
   Trima_ServiceCmdPort     = 7000,        /* port used by EMS to connect to Trima */
   Trima_ServiceReplyPort   = 7001,        /* port used by Trima to reply to EMS */
   Trima_BroadcastPort      = 7002,        /* port used by Trima to broadcast network presence */
   Trima_VistaListenPort    = 7010,        /* port used by Trima to receive Vista connections */
   Trima_DataLogPort        = 7100,        /* port used to connect to DataLog network task */
   Trima_OSPort             = 7110         /* port used for OS communication between safety and control */
};

#endif /* ifndef _PORTNUMBERS_INCLUDE */

/* FORMAT HASH 7121ecd72fd79d3a0e0ead1676debfc3 */
