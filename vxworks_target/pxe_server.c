/*
 * $Header$
 *
 * This file contains the server-side of the PXE boot server. This
 * server implements a small subset of DHCP server functionality to
 * support the PXE boot process.
 *
 * $Log: $
 *
 */

#include "vxWorks.h"
#include "envLib.h"
#include "sockLib.h"
#include "inetLib.h"
#include "stdioLib.h"
#include "strLib.h"
#include "ioLib.h"
#include "fioLib.h"
#include "boot_mode.h"

#include "node_data.h"
#include "filenames.h"

#define SERVER_PORT_NUM         67      /* server's DHCP listen port number for bind() */
#define CLIENT_PORT_NUM         68      /* client's port number */
#define REQUEST_MSG_SIZE        600     /* max size of request/response message */
#define DHCP_MESSAGE_TYPE_TAG   0x35

struct dhcpHeaderStruct
{
	UINT8		_op			__attribute__ ((packed)); /* Message op code */
	UINT8		_htype		__attribute__ ((packed)); /* Hardware type 1=10MB ethernet */
	UINT8		_hlen			__attribute__ ((packed)); /* Hardware address length 6 for 10M ethernet */
	UINT8		_hops			__attribute__ ((packed)); /* Number of DHCP hops */
	UINT32	_xid			__attribute__ ((packed)); /* Transaction ID - random number chosen by client */
	UINT16	_secs			__attribute__ ((packed)); /* Seconds passed since the client began request process */
	UINT16	_flags		__attribute__ ((packed)); /* DHCP flags */
	UINT32	_ciaddr		__attribute__ ((packed)); /* Client IP address */
	UINT32	_yiaddr		__attribute__ ((packed)); /* Your client IP address. Server's response to client. */
	UINT32	_siaddr		__attribute__ ((packed)); /* Server's IP address. */
	UINT32	_giaddr		__attribute__ ((packed)); /* Relay agent IP address */
	UINT8		_chaddr[16]	__attribute__ ((packed)); /* Client hardware address */
	UINT8		_sname[64]	__attribute__ ((packed)); /* Optional server host name. Null terminated string */
	UINT8		_file[128]	__attribute__ ((packed)); /* Boot file name. Must be fully qualified. */
	UINT32	_cookie		__attribute__ ((packed)); /* Options cookie */
};

STATUS pxeServer(void);
LOCAL STATUS processPxeRequest(unsigned char *pBuffer);
LOCAL STATUS pxeDiscover( unsigned char *pBuffer, unsigned long serverIp, unsigned long clientIp, const char *pBootFile );
LOCAL STATUS pxeRequest( unsigned char *pBuffer, unsigned long serverIp, unsigned long clientIp, const char *pBootFile );
LOCAL STATUS sendResponse(unsigned char *pBuffer, int length, unsigned long sourceIp);
LOCAL unsigned char* findOption(unsigned char *pBuffer, unsigned char optionNum);

int pxeEnable = 1;

/*********************************************************************
* pxeServer - read broadcast messages from UDP port 67 and process any
*             extended DHCP packets that contain the PXEClient class code.
*
* RETURNS: ERROR if a resources could not be allocated.
*/
STATUS pxeServer (void)
{
	struct sockaddr_in  serverAddr;    /* server's socket address */
	struct sockaddr_in  clientAddr;    /* client's socket address */
	unsigned char       msgBuffer[REQUEST_MSG_SIZE]; /* request/Message from client */
	int                 sockAddrSize;  /* size of socket address structure */
	int                 sFd;           /* socket file descriptor */
	char                inetAddr[INET_ADDR_LEN]; /* buffer for client's inet addr */
	int					recvBytes;
	struct dhcpHeaderStruct *dhcpHdr = NULL;

	/* set up the local address */
	sockAddrSize = sizeof (struct sockaddr_in);
	bzero ((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (SERVER_PORT_NUM);
	serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	/* create a UDP-based socket */
	if ((sFd = socket (AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
		perror ("socket");
		return (ERROR);
	}

	/* bind socket to local address */
	if (bind (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
	{
		perror ("bind");
		close (sFd);
		return (ERROR);
	}

	/* Begin reading data from the socket. */

	while(pxeEnable == 1)
	{
		recvBytes = recvfrom (sFd, msgBuffer, sizeof (msgBuffer), 0,
						  (struct sockaddr *) &clientAddr, &sockAddrSize);

		if( recvBytes == ERROR )
		{
			perror ("PXEsrv recvfrom");
			close (sFd);
			return ERROR;
		}

		/* Ignore packets that don't meet the DHCP message minimum size */
		if( recvBytes > sizeof(struct dhcpHeaderStruct) + 4 )
		{
			processPxeRequest( msgBuffer );
		}
	}

	close(sFd);
	return OK;
}

LOCAL STATUS processPxeRequest(unsigned char *pBuffer)
{
	static unsigned int dhcpDiscCnt = 0;
	static unsigned int dhcpLogFlag = 0x1;

	struct dhcpHeaderStruct *dhcpHdr = NULL;
	unsigned char *pOptionTag = NULL;
	char pxeClassId[33];
	STATUS retVal = 0;
	SysNodeData controlNodeData, safetyNodeData;
	int i;

	sysGetNodeData( ControlNode, &controlNodeData );
	sysGetNodeData( SafetyNode, &safetyNodeData );

	dhcpHdr = (struct dhcpHeaderStruct*)pBuffer;

	do
	{
		/* determine if this is a boot request */
		if(dhcpHdr->_op != 1)
			break;

		/* determine if this is a PXE message (option 60) */
		pOptionTag = findOption(pBuffer, 60);
		if( pOptionTag )
		{
			/* capture the PXE class ID */
			memcpy(pxeClassId, &pOptionTag[2], 32);
			pxeClassId[32] = 0;
		}

		/* determine if this is a DHCP message type */
		pOptionTag = findOption(pBuffer, DHCP_MESSAGE_TYPE_TAG);

		if( !pOptionTag )
		{
            printf("BOOTP boot request from MAC address 0x");

            if (dhcpHdr->_hlen > 16)
               dhcpHdr->_hlen = 6; /* default to hardware (MAC) address length of 6 */

            for(i=0; i<dhcpHdr->_hlen; i++)
            {
               printf("%02X", dhcpHdr->_chaddr[i] );
            }

            printf("\n");

			retVal = pxeRequest( pBuffer, safetyNodeData.ipAddress[ControlNode], controlNodeData.ipAddress[SafetyNode], SAFETY_BOOTROM_IMAGE );

			break;
		}
		else
		{
			/* Determine DHCP message type */
			switch( pOptionTag[2] )
			{
			case 1: /* DHCP discover message */
				/* print a boot request message only when
				 * a DHCP discover message is received.
				 *
				 * But limit the logging so that we don't miss other messages
				 * that may be printed to the console in the case of an anomalous
				 * boot sequence (e.g., to diagnose overnight boot robustness tests)
				 */
				if (++dhcpDiscCnt == dhcpLogFlag)
				{
					printf("PXE boot request from MAC address 0x");

					if (dhcpHdr->_hlen > 16)
						dhcpHdr->_hlen = 6; /* default to hardware (MAC) address length of 6 */

					for(i=0; i<dhcpHdr->_hlen; i++)
					{
						printf("%02X", dhcpHdr->_chaddr[i] );
					}

					/* Print the counter and turn off logging after we hit 32 */
					if (dhcpDiscCnt < 32)
					{
						printf(" [#%d]", dhcpDiscCnt);
						dhcpLogFlag <<= 1;
					}
					else
					{
						printf(" [#%d ; logging turned off]", dhcpDiscCnt );
						dhcpLogFlag = 0;
					}

					printf("\n");
				}

				retVal = pxeDiscover( pBuffer, safetyNodeData.ipAddress[ControlNode], controlNodeData.ipAddress[SafetyNode], SAFETY_BOOTROM_PXE_IMAGE );
	
				break;
	
			case 3: /* DHCP request message */
				retVal = pxeRequest( pBuffer, safetyNodeData.ipAddress[ControlNode], controlNodeData.ipAddress[SafetyNode], SAFETY_BOOTROM_PXE_IMAGE );
	
				break;
	
			default:
				break;
			}
		}
	} while(0);

	return retVal;
}

/* Respond to a DHCP discover message with a DHCP offer message */
LOCAL STATUS pxeDiscover( unsigned char *pBuffer, unsigned long serverIp, unsigned long clientIp, const char *pBootFile )
{
	unsigned char responseBuf[REQUEST_MSG_SIZE];
	unsigned char *pBuf = NULL;
	unsigned char dhcpMessageType[] = { 53,1,2 };		/* DHCP offer message type */
	unsigned char serverIpTag[] = { 54,4,0,0,0,0 };		/* DHCP server IP */
	unsigned char networkMask[] = {1,4,255,255,255,0};	/* Client's network mask 255.255.255.0 */
	unsigned char gatewayIp[] = { 3,4,0,0,0,0 };			/* specifiy the router IP address */
	unsigned char classId[] = {60,9,80,88,69,67,108,105,101,110,116}; /* Class ID = PXEClient */
	unsigned char vendorOptions[] = { 43,0 };				/* Encapsulated vendor (PXE) options */
	unsigned char pxeDiscoveryCtrl[] = {6,1,10};			/* PXE dicovery control = disable multicast, download boot file */
	struct dhcpHeaderStruct *dhcpHdr = NULL;
	/* add up the size of all the vendor options */
	unsigned char vendorOptionSize = sizeof(pxeDiscoveryCtrl) + 1; /* add one for the end of options character */
	unsigned long serverIpHbo;
	unsigned long messageSize;
	STATUS retVal = 0;

	/* prepare response */
	memset( responseBuf, 0, REQUEST_MSG_SIZE );
	memcpy( responseBuf, pBuffer, sizeof(struct dhcpHeaderStruct) );

	dhcpHdr = (struct dhcpHeaderStruct*)responseBuf;

	/* prepare response message items */
	memset( responseBuf, 0, REQUEST_MSG_SIZE );
	memcpy( responseBuf, pBuffer, sizeof(struct dhcpHeaderStruct) );

	dhcpHdr = (struct dhcpHeaderStruct*)responseBuf;

	/* setup the DHCP header */
	dhcpHdr->_op = 2;				/* Code for BOOTP reply. */
	dhcpHdr->_yiaddr = clientIp; /* client IP address 90.X.0.1 in network byte order*/
	dhcpHdr->_ciaddr = 0;
	dhcpHdr->_siaddr = 0;
	strcpy( dhcpHdr->_file, pBootFile ); /* note that the boot file name is limited to 128 bytes */
	strcpy( dhcpHdr->_sname, "control" );

	/* setup the option tags */
	vendorOptions[1] = vendorOptionSize;
	serverIpHbo = ntohl( serverIp );
	memcpy( &serverIpTag[2], &serverIp, 4 ); /* insert the server IP */
	memcpy( &gatewayIp[2], &serverIp, 4 ); /* insert the server IP */

	/* assemble response message */
	pBuf = &responseBuf[ sizeof(struct dhcpHeaderStruct) ];
	
	memcpy( pBuf, dhcpMessageType, sizeof( dhcpMessageType ));
	pBuf += sizeof( dhcpMessageType );
	
	memcpy( pBuf, serverIpTag, sizeof( serverIpTag ));
	pBuf += sizeof( serverIpTag );

	memcpy( pBuf, networkMask, sizeof( networkMask ));
	pBuf += sizeof( networkMask );

	memcpy( pBuf, gatewayIp, sizeof( gatewayIp ));
	pBuf += sizeof( gatewayIp );
	
	memcpy( pBuf, classId, sizeof( classId ));
	pBuf += sizeof( classId );

	memcpy( pBuf, vendorOptions, sizeof( vendorOptions ));
	pBuf += sizeof( vendorOptions );

	/* add the vendor options */
	memcpy( pBuf, pxeDiscoveryCtrl, sizeof( pxeDiscoveryCtrl ));
	pBuf += sizeof( pxeDiscoveryCtrl );

	/* add option terminators for the regular and vendor */
	*pBuf++ = 255;
	*pBuf++ = 255;

	/* compute the message size and send the message */
	messageSize = pBuf - responseBuf;
	retVal = sendResponse( responseBuf, messageSize, serverIp );

	return retVal;
}

LOCAL STATUS pxeRequest( unsigned char *pBuffer, unsigned long serverIp, unsigned long clientIp, const char *pBootFile )
{
	unsigned char responseBuf[REQUEST_MSG_SIZE];
	unsigned char *pBuf = NULL;
	unsigned char dhcpMessageType[] = { 53,1,5 };		/* DHCP boot reply message type */
	unsigned char serverIpTag[] = { 54,4,0,0,0,0 };		/* DHCP server IP */
	unsigned char networkMask[] = {1,4,255,255,255,0};	/* Client's network mask 255.255.255.0 */
	unsigned char classId[] = {60,9,80,88,69,67,108,105,101,110,116}; /* Class ID = PXEClient */
	unsigned char vendorOptions[] = { 43,0 };				/* Encapsulated vendor (PXE) options */
	unsigned char pxeDiscoveryCtrl[] = {6,1,10};			/* PXE dicovery control = disable multicast, download boot file */
	unsigned char pxeBootServer[] = {8,7,0,0,1,0,0,0,0}; /* PXE boot server type and IP */
	unsigned char pxeBootMenu[] = {9,5,0,0,2,65,65};	/* Boot menu required but not used in this application = "AA" */
	unsigned char pxeBootMenuPrompt[] = {10,3,1,65,65}; /* Boot prompt required but not used = "AA" */
	unsigned char pxeBootItem[] = {71,4,0,0,0,0};		/* Boot menu item. Type 0 is boot strap server. */
													/* 43,31,6,1,10,8,7,0,1,1,90,0,0,2,9,5,0,1,2,65,65,10,3,1,65,65,71,4,0,1,0,0,255,255}; */
	struct dhcpHeaderStruct *dhcpHdr = NULL;
	/* add up the size of all the vendor options */
	unsigned char vendorOptionSize = sizeof(pxeDiscoveryCtrl) + sizeof(pxeBootServer) +
												sizeof(pxeBootMenu) + sizeof(pxeBootMenuPrompt) +
												sizeof(pxeBootItem) + 1; /* add one for the end of options character */
	unsigned long serverIpHbo;
	unsigned long messageSize;
	STATUS retVal = 0;

	/* prepare response message items */
	memset( responseBuf, 0, REQUEST_MSG_SIZE );
	memcpy( responseBuf, pBuffer, sizeof(struct dhcpHeaderStruct) );

	dhcpHdr = (struct dhcpHeaderStruct*)responseBuf;

	/* setup the DHCP header */
	dhcpHdr->_op = 2;				/* Code for BOOTP reply. */
	dhcpHdr->_yiaddr = clientIp; /* client IP address 90.X.0.1 in network byte order*/
	dhcpHdr->_ciaddr = 0;
	dhcpHdr->_siaddr = 0;
	strcpy( dhcpHdr->_file, pBootFile ); /* note that the boot file name is limited to 128 bytes */
	strcpy( dhcpHdr->_sname, "control" );

	/* setup the option tags */
	vendorOptions[1] = vendorOptionSize;
	serverIpHbo = ntohl( serverIp );
	memcpy( &serverIpTag[2], &serverIp, 4 ); /* insert the server IP */
	memcpy( &pxeBootServer[5], &serverIp, 4 ); /* insert the server IP */

	/* assemble response message */
	pBuf = &responseBuf[ sizeof(struct dhcpHeaderStruct) ];
	
	memcpy( pBuf, dhcpMessageType, sizeof( dhcpMessageType ));
	pBuf += sizeof( dhcpMessageType );
	
	memcpy( pBuf, serverIpTag, sizeof( serverIpTag ));
	pBuf += sizeof( serverIpTag );

	memcpy( pBuf, networkMask, sizeof( networkMask ));
	pBuf += sizeof( networkMask );

	memcpy( pBuf, classId, sizeof( classId ));
	pBuf += sizeof( classId );

	memcpy( pBuf, vendorOptions, sizeof( vendorOptions ));
	pBuf += sizeof( vendorOptions );

	/* add the vendor options */
	memcpy( pBuf, pxeDiscoveryCtrl, sizeof( pxeDiscoveryCtrl ));
	pBuf += sizeof( pxeDiscoveryCtrl );

	memcpy( pBuf, pxeBootServer, sizeof( pxeBootServer ));
	pBuf += sizeof( pxeBootServer );

	memcpy( pBuf, pxeBootMenu, sizeof( pxeBootMenu ));
	pBuf += sizeof( pxeBootMenu );

	memcpy( pBuf, pxeBootMenuPrompt, sizeof( pxeBootMenuPrompt ));
	pBuf += sizeof( pxeBootMenuPrompt );

	memcpy( pBuf, pxeBootItem, sizeof( pxeBootItem ));
	pBuf += sizeof( pxeBootItem );
	
	/* add option terminators for the regular and vendor */
	*pBuf++ = 255;
	*pBuf++ = 255;

	/* compute the message size and send the message */
	messageSize = pBuf - responseBuf;
	retVal = sendResponse( responseBuf, messageSize, serverIp );

    printf("PXE Response: %s -> %x. File(%s)\n",
           dhcpHdr->_sname,
           ntohl(dhcpHdr->_yiaddr),
           dhcpHdr->_file);

	return retVal;
}

/* This function locates a option and returns a pointer
 * to the option tag.
 */
LOCAL unsigned char* findOption(unsigned char *pBuffer, unsigned char optionNum)
{
	struct dhcpHeaderStruct *dhcpHdr = NULL;
	unsigned char *ptr = NULL;
	unsigned char optLength;
	unsigned int index = 0;

	dhcpHdr = (struct dhcpHeaderStruct*)pBuffer;

	/* Verify the option cookie */
	if( ntohl( dhcpHdr->_cookie ) == 0x63825363 )
	{
		/* point to the byte after the cookie */
		ptr = &pBuffer[ sizeof(struct dhcpHeaderStruct) ];

		while( *ptr != optionNum && *ptr != 255 && index < 312 )
		{
			/* Not the option we are looking for. Determine
			the length of this option and skip to the next option. */
			ptr++;
			optLength = *ptr;
			ptr += optLength + 1; /* point to the next option tag */
			index += optLength + 1;
		}

		if( *ptr == 255 || index >= 312 )
			ptr = NULL;
	}

	return ptr;
}

LOCAL STATUS sendResponse(unsigned char *pBuffer, int length, unsigned long sourceIp)
{
	struct sockaddr_in  clientAddr;    /* client socket address */
	struct sockaddr_in  serverAddr;    /* server's socket address */
	int                 sockAddrSize;  /* size of socket address structure */
	int                 buflen;        /* length of message */

    /* create client's socket */
	int                 sFd;

	if ((sFd = socket(AF_INET, SOCK_DGRAM, 0)) == ERROR)
	{
		perror ("socket");
		return (ERROR);
	}

	/* Create a broadcast address for the client */
	sockAddrSize = sizeof (struct sockaddr_in);
	bzero ((char *) &clientAddr, sockAddrSize);
	clientAddr.sin_len = (u_char) sockAddrSize;
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(CLIENT_PORT_NUM);
	clientAddr.sin_addr.s_addr = sourceIp | inet_addr ("0.0.0.255");;

	/* set up the local address */
	bzero ((char *) &serverAddr, sockAddrSize);
	serverAddr.sin_len = (u_char) sockAddrSize;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons (12000);
	serverAddr.sin_addr.s_addr = sourceIp;

	/* bind socket to local address */
	if (bind (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
	{
		perror( "bind" );
		close( sFd );
		return( ERROR );
	}

	/* Setup for a broadcast packet */
	if (setsockopt(sFd, SOL_SOCKET, SO_BROADCAST, (char *)&buflen, sizeof(buflen)) < 0)
	{
	  printf("setsockopt() broadcast");
	  close( sFd );
	  return( ERROR );
	}

   /* send request to server */
	if(sendto( sFd, pBuffer, length, 0, (struct sockaddr *) &clientAddr, sockAddrSize) == ERROR)
	{
		perror ("sendto");
		close (sFd);
		return (ERROR);
	}

	close (sFd);

	return (OK);
}
