/*
 *  Copyright(c) 2004 by Gambro BCT, Inc. All rights reserved.
 *
 * $Header: E:/BCT_Development/TrimaVxPort/Trima/vxworks_target/rcs/pciApicInt.c 1.1 2007/05/22 18:27:54Z dslausb Exp $
 *
 *  PCI bus multiprocessor interrupt support routines.
 *
 * $Log: pciApicInt.c $
 * Revision 1.1  2007/05/22 18:27:54Z  dslausb
 * Initial revision
 * Revision 1.2  2004/07/01 14:59:39Z  wms10235
 * Updated/added files to allow a command line build.
 * Revision 1.1  2004/05/14 14:30:42Z  wms10235
 * Initial revision
 *
 * Initial revision
 *
 */

/* includes */
#include <vxWorks.h>
#include <string.h>
#include <drv/intrCtl/loApic.h>
#include <pc.h>

STATUS scanApicPciIntEntries(UINT32 pciBus, UINT32 pciDevice, UINT8 * pciIrq);

/*******************************************************************************
*
* loApicMpScan -  scans given memory range for the string "_MP_"
*
* This routine scans the memory areas specified in its argument for the
* Intel MP signature string "_MP_".
*
* RETURNS: pointer to "_MP_"  in the range , or NULL
*
*/

LOCAL INT8 * loApicMpScan
    (
    INT8 * start,		/* start address to scan */
    INT8 * end			/* end address to scan */
    )
    {
    INT8 * p;

    for (p = start; (p+3) < end; p++)
	{
	if (strncmp (p, "_MP_", 4) == 0)
	    return (p);
	}

    return (NULL);
    }

/*******************************************************************************
*
* scanApicPciIntEntries - check the multiprocessor tables for PCI interrupt entries
*
* This routine checks the multiprocessor BIOS configuration tables for PCI based
* interrupt entries. If a corresponding entry exists for a PCI device, the
* routine returns status OK with the interrupt entry in irq.
*
* RETURNS: OK, ERROR
*
* NOMANUAL
*/
STATUS scanApicPciIntEntries
    (
    UINT32  pciBus,      /* a PCI bus number */
    UINT32  pciDevice,   /* a PCI device number */
	 UINT8 * pciIrq       /* return an irq for the device */
	 )
{
    MP_FPS *      pFps = NULL;
    MP_HEADER *   pTbl;
    char * p;
    int ix;
	 UINT8 sourceBusId;
	 UINT8 sourceBusIrq;
	 UINT32 ioApicIrqSettings;
	 UINT32 ioApicCpuValue;
	 UINT8 irqFlags;
	 STATUS retStatus = ERROR;

    /* MP Floating Point Structure */

    pFps = (MP_FPS *)loApicMpScan ((char *)EBDA_START, (char *)EBDA_END);
    if (pFps == NULL)
        pFps = (MP_FPS *)loApicMpScan ((char *)BIOS_ROM_START,
				       (char *)BIOS_ROM_END);

    if (pFps == NULL)
	 {
        /* Not multiprocessor compliant */
	     return(ERROR);
	 }

    if ((pFps->featureByte[0] != 0) || (pFps->configTableAddr == 0))
    {
        /* MP configuration table does not exist */
	     return(ERROR);
	 }

    /* Get the MP Configuration Table Header */
    pTbl = (MP_HEADER *)pFps->configTableAddr;

    /* scan the MP configuration table for interrupt entries */

    p = (char *)pTbl + sizeof(MP_HEADER);
    for (ix = 0; ix < pTbl->entryCount; ix++)
	 {
		 switch (*p)
		 {
		 case MP_ENTRY_CPU:				/* Processor Entry */
			  p += sizeof (MP_CPU);
			  break;
	
		 case MP_ENTRY_BUS:				/* Bus Entry */
			  p += 8;
			  break;
	
		 case MP_ENTRY_IOAPIC:			/* IO APIC Entry */
			  p += sizeof (MP_IOAPIC);
			  break;
	
		 case MP_ENTRY_IOINTERRUPT:	/* IO Interrupt Entry */
			  sourceBusId = *(p + 4);
			  sourceBusIrq = *(p + 5);
			
			  /* Determine if this is the I/O interrupt were interested in. */
			  /* For PCI devices the device ID occupies bits 2 to 6 in the  */
			  /* source bus irq parameter. */
			  if( sourceBusId == pciBus && (( sourceBusIrq & 0x7c )>>2 ) == pciDevice )
			  {
					*pciIrq = *(p + 7);
					irqFlags = *(p + 2);
	
					/* Adjust the interrupt settings - level or edge triggered */
					/* and acitve low or active high. */
					retStatus = ioApicRedGet( *pciIrq, &ioApicCpuValue, &ioApicIrqSettings );
	
					if( retStatus == OK )
					{
						if( (irqFlags & 0x03) == 3 )           /* Active low */
							ioApicIrqSettings |= 0x02000;
						else if( (irqFlags & 0x03) == 1 )      /* Active high */
							ioApicIrqSettings &= 0x0FFFFDFFF;
						else if( (irqFlags & 0x03) == 0 )      /* PCI default is active low */
							ioApicIrqSettings |= 0x02000;
						else                                   /* reserved setting */
							return(ERROR);
	
						if( (irqFlags & 0x0c) == 0x0c )        /* level triggered */
							ioApicIrqSettings |= 0x08000;
						else if( (irqFlags & 0x0c) == 4 )      /* edge triggered */
							ioApicIrqSettings &= 0x0FFFF7FFF;
						else if( (irqFlags & 0x0c) == 0 )      /* PCI default is level triggered */
							ioApicIrqSettings |= 0x08000;
						else                                   /* reserved setting */
							return(ERROR);
	
						retStatus = ioApicRedSet( *pciIrq, ioApicCpuValue, ioApicIrqSettings );
					}
	
					return(retStatus);
			  }
			  p += 8;
			  break;
	
		 case MP_ENTRY_LOINTERRUPT:		/* Local Interrupt Entry */
			  p += 8;
			  break;
	
		 default:		/* Unknown Entry */
			  p += 8;		/* wild guess */
		 }
	 }

	 /* couldn't find the interrupt in the table */
	 return(ERROR);
}

