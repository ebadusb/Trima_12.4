/* sysEnd.c - System Enhanced network interface support library */

/* Copyright 1997-2002 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
*/

/*
DESCRIPTION
This file contains the board-specific routines for Ethernet adapter
initialization of the SMC 91C96 adapter.


Refer to the BSP reference entry for any eventual limitations or
problems related to the BSP.


SEE ALSO: ifLib
*/

#if defined(INCLUDE_ESMC_END)

#include "vxWorks.h"
#include "config.h"
#include "cacheLib.h"
#include "muxTkLib.h"
#include "end.h"
#include "esmcEnd.h"

#include "stdio.h"
#include "stdlib.h"


/* forward declarations */

END_OBJ *sysEsmcEndLoad(char *initString);
void sysEsmcIntEnable(int level);
void sysEsmcIntDisable(int level);
void sysEsmcOnExitIsr ( END_OBJ  *pDev  /* Unused, but provided by esmcIsr in case we need it. */ 
							 );
void sysEnetAddrGet ( int unitNum,   /* instance of esmc driver to modify */ 
							 char *macAddr  /* buffer for six bytes of MAC address */ 
						  );
END_OBJ* esmcEndLoad
    (
    char *    initString,    /* String to be parsed by the driver. */
	 void *    pBsp
    );

/*******************************************************************************
*
* sysEsmcEndLoad - load the SMC END driver
*
* This routine provides BSP support for loading the esmcEnd driver.
*
* RETURNS: A pointer to an END_OBJ, or NULL upon failure.
*
*/

END_OBJ *sysEsmcEndLoad(char *initString)
{
    END_OBJ * pEnd;
    char      paramStr [END_INIT_STR_MAX];

    static const char * const paramTemplate = "%d:%#x:%#x:%#x:%#x:%#x:%d";


    if (strlen (initString) == 0)
        {
        /* PASS (1)
         * The driver load routine returns the driver name in <initString>.
         */

        pEnd = esmcEndLoad (initString,NULL);
        }
    else
        {
        /* PASS (2)
         * The END <unit> number is prepended to <initString>.  Construct
         * the rest of the driver load string by appending parameters to
         * the END <unit> number.
         */

        char * holder  = NULL;
        int    endUnit = atoi (strtok_r (initString, ":", &holder));


        /* finish off the initialization parameter string */

        sprintf (paramStr, paramTemplate, 
                 endUnit,
                 IO_ADRS_ESMC,
                 INT_NUM_GET (INT_LVL_ESMC),
                 INT_LVL_ESMC,
                 CONFIG_ESMC,
                 0,
                 0);

        if ((pEnd = esmcEndLoad (paramStr,NULL)) == (END_OBJ *) NULL)
            {
            printf ("Error esmcEndLoad:  failed to load driver.\n");
            }
        }

    return (pEnd);
}


/*******************************************************************************
*
* sysEsmcIntEnable - enable an interrupt level for ethernet
*
* This routine provides BSP support for enabling the ethernet interrupt for the SMC
* END driver.
*
* RETURNS: N/A
*/

void sysEsmcIntEnable(int level)
{
    sysIntEnablePIC (level);
}


/*******************************************************************************
*
* sysEsmcIntDisable - disable an interrupt level for ethernet
*
* This routine provides BSP support for disabling the ethernet interrupt for the SMC
* END driver.
*
* RETURNS: N/A
*/

void sysEsmcIntDisable(int level)
{
    sysIntDisablePIC (level);
}



/*******************************************************************************
*
* sysEsmcOnExitIsr - clean up after handling ethernet interrupt
*
* This routine is to be called from the esmcIsr interrupt handler upon exit
* in order for the Innovator BSP to do some board-specific end-of-interrupt
* handling.
*
* RETURNS: N/A
*/

void sysEsmcOnExitIsr
    (
    END_OBJ  *pDev  /* Unused, but provided by esmcIsr in case we need it. */
    )
{
}

#if 0
/*******************************************************************************
*
* sysInWord - read a 16-bit quanity
*
* This routine provides BSP support for reading a 16-bit quantity at the specified
* address.
*
* RETURNS: 16-bit value at addres <addr>
*/

USHORT sysInWord
    (
    ULONG port   /* Address to read */
    )
{
    return *((USHORT *)port);
}


/*******************************************************************************
*
* sysOutWord - write a 16-bit quanity
*
* This routine provides BSP support for reading a 16-bit quantity to the specified
* address.
*
* RETURNS: N/A
*/

void sysOutWord
    (
    ULONG port,   /* Address to write */
    UINT16 data   /* Value to write */
    )
{
    *((UINT16 *)port) = data;
}

#endif

LOCAL int sysEnetUnitNum = 0;

/*******************************************************************************
*
* sysEnetAddrGet - return current MAC address
*
* This routine provides BSP support for reading the current ethernet MAC address
*
* RETURNS: N/A
*/

void sysEnetAddrGet
    (
    int unitNum,   /* instance of esmc driver to modify */
    char *macAddr  /* buffer for six bytes of MAC address */
    )
{
    void *cookie;
    sysEnetUnitNum = unitNum;


    cookie = muxTkCookieGet(ESMC_DEV_NAME, sysEnetUnitNum);

    if (cookie == NULL)
        return;

    muxIoctl(cookie, EIOCGADDR, macAddr);
}


/*******************************************************************************
*
* sysEnetAddrSet - modify current MAC address
*
* This routine provides BSP support for writing the current ethernet MAC address
*
* RETURNS: N/A
*/

void sysEnetAddrSet
    (
    char byte0,  /* first MAC address byte */
    char byte1,
    char byte2,
    char byte3,
    char byte4,
    char byte5   /* last MAC address byte */
    );
void sysEnetAddrSet
    (
    char byte0,  /* first MAC address byte */
    char byte1,
    char byte2,
    char byte3,
    char byte4,
    char byte5   /* last MAC address byte */
    )
{
    char buffer[6];
    void *cookie;

    cookie = muxTkCookieGet(ESMC_DEV_NAME, sysEnetUnitNum);

    if (cookie == NULL)
        return;

    buffer[0] = byte0;
    buffer[1] = byte1;
    buffer[2] = byte2;
    buffer[3] = byte3;
    buffer[4] = byte4;
    buffer[5] = byte5;

    muxIoctl(cookie, EIOCSADDR, buffer);
}


#endif /* INCLUDE_ESMC_END */
