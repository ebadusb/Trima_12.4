/* esmcEnd.c - SMSC 91cxx  END network interface driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,31jul07,rb   performance enhancements for GAM1996
01b,15jul02,dnb  removed all byte accesses to simplify port to Kanis board
01a,07jun02,dnb  written from templateEnd.c and if_emsc.c
*/

/*

DESCRIPTION
This module implements the SMSC 91CXX network interface driver.

EXTERNAL SUPPORT REQUIREMENTS
This driver requires several external support functions, defined as macros:
.CS
    SYS_INT_CONNECT(pDev, routine, arg)
    SYS_INT_DISCONNECT (pDev, routine, arg)
    SYS_INT_ENABLE(pDev)
    SYS_INT_DISABLE(pDev)
    data = READ_WORD(pDev, reg)
    WRITE_WORD(pDev, reg, data)
.CE

These macros allow the driver to be customized for BSPs that use
special versions of these routines.

The macro SYS_INT_CONNECT is used to connect the interrupt handler to
the appropriate vector.  By default it is the routine intConnect().

The macro SYS_INT_DISCONNECT is used to disconnect the interrupt handler prior
to unloading the module.  By default this is a dummy routine that
returns OK.

The macro SYS_INT_ENABLE is used to enable the interrupt level for the
end device.  It is called once during initialization. It calls an
external board level routine sysSmc91cxxIntEnable().

The macro SYS_INT_DISABLE is used to disable the interrupt level for the
end device.  It is called once during shutdown. It calls an
external board level routine sysSmc91cxxIntDisable().

The macros READ_WORD and WRITE_WORD are used for accessing the
esmc device.  The default macros map these operations onto
sysInWord() and sysOutWord().

The HI_BYTE and LO_BYTE macros return the high and low bytes, respectively,
of the word-length registers.

ON_EXIT_ISR and SWAP_BYTES_IF_NECESSARY are (hopefully self-explanatory) macros
used to aid porting.

INCLUDES:
end.h endLib.h etherMultiLib.h

SEE ALSO: muxLib, endLib
.I "Writing an Enhanced Network Driver"
*/

/* includes */

#include  "vxWorks.h"
#include  "stdio.h"
#include  "logLib.h"
#include  "sysLib.h"
#include  "taskLib.h"
#include  "iv.h"
#include "time.h"
#include  "intLib.h"
#include  "netLib.h"
#include  "etherMultiLib.h"
#include  "end.h"            /* Common END structures. */
#include  "endLib.h"
#include  "net/mbuf.h"

/* Make local to BSP: */
/* #include  "drv/end/esmcEnd.h" */
#include  "esmcEnd.h"
#include  "netinet/if_ether.h"

/* defines */

/* Configuration items */
/* Auto Negotiation */
 
#define ENET_AUTO        0    /* 0:Enable  Auto-Negotiation */
                              /* 1:Disable Auto-Negotiation */
/* Ethernet Speed */
#define ENET_SPEED_10          10000000   /* 10 Mbit/sec interface */
#define ENET_SPEED_100         100000000  /* 100 Mbit/sec interface */

/* Duplex Mode */
#define ENET_DUPLEX_HALF    0
#define ENET_DUPLEX_FULL    1

#define ATTACHMENT_DEFAULT      0       /* use card as configured */
#define ATTACHMENT_AUI          1       /* AUI  (thick, DIX, DB-15) */
#define ATTACHMENT_BNC          2       /* BNC  (thin, 10BASE-2) */
#define ATTACHMENT_RJ45         3       /* RJ-45 (twisted pair, TPE, 10BASE-T)*/

/* Definitions for the flags field */

#define END_POLLING                   0x01
#define ESMC_RCV_HANDLING_FLAG        0x02
#define ESMC_TX_HANDLING_FLAG         0x04

#define DRV_DEBUG

/* Debug macros */
#ifdef  DRV_DEBUG
#define DRV_DEBUG_OFF                 0x0000
#define DRV_DEBUG_RX                  0x0001
#define DRV_DEBUG_TX                  0x0002
#define DRV_DEBUG_INT                 0x0004
#define DRV_DEBUG_POLL                (DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define DRV_DEBUG_POLL_RX             0x0008
#define DRV_DEBUG_POLL_TX             0x0010
#define DRV_DEBUG_LOAD                0x0020
#define DRV_DEBUG_IOCTL               0x0040
#define DRV_DEBUG_ERR                 0x0080
#define DRV_DEBUG_MEM_INIT            0x0100
#define DRV_DEBUG_CONFIG              0x0200
#define DRV_DEBUG_ASSERT              0x0400


#define DRV_ASSERT(arg)                    \
   do{                                     \
        if(esmcDebug & DRV_DEBUG_ASSERT)   \
        {                                  \
            if(!(arg))                     \
            {                              \
                logMsg("Assertion failed: %s, %s, %d",     \
                (int) #arg,(int)__FILE__, __LINE__,4,5,6); \
            }                              \
        }                                  \
    } while(0)

#define DRV_DEBUG_FN_TRACE            0x8000

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)        \
    do                                                  \
        {                                               \
    if (esmcDebug & (FLG))                              \
        logMsg (X0, X1, X2, X3, X4, X5, X6);            \
    }                                                   \
    while (0)


#else /* DRV_DEBUG */
#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)    do {} while (0)
#define DRV_ASSERT(arg) ((void)0)
#endif /* DRV_DEBUG */


/*
 * Default macro definitions for BSP interface.  These macros can be
 * redefined in a wrapper file, to generate a new module with an
 * optimized interface.
 */

/* Macro to connect interrupt handler to vector */

#ifndef SYS_INT_CONNECT
#define SYS_INT_CONNECT(pDev,rtn,arg,pResult)                              \
    {                                                                      \
    IMPORT STATUS sysIntConnect();                                         \
    *(pResult) = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDev)->ivec),   \
               rtn, (int)(arg));                                           \
    }
#endif

/* Macro to disconnect interrupt handler from vector */

LOCAL VOID dummyIsr (void) { };

#ifndef SYS_INT_DISCONNECT
#define SYS_INT_DISCONNECT(pDev,rtn,arg,pResult)                           \
    {                                                                      \
    IMPORT STATUS intConnect();                                            \
    *(pResult) = intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC ((pDev)->ivec),   \
                 dummyIsr, (int)(arg));                                    \
    }
#endif

/* Macro to enable the appropriate interrupt level */

#ifndef SYS_INT_ENABLE
#define SYS_INT_ENABLE(pDev)                                             \
    {                                                                    \
    IMPORT void sysEsmcIntEnable();                                      \
    sysEsmcIntEnable ((pDev)->ilevel);                                   \
    }
#endif

/* Macro to disable the appropriate interrupt level */

#ifndef SYS_INT_DISABLE
#define SYS_INT_DISABLE(pDev)                                           \
    {                                                                   \
    IMPORT void sysEsmcIntDisable();                                    \
    sysEsmcIntDisable ((pDev)->ilevel);                                 \
    }
#endif

/*
 * Macros to do word access to the chip.  Default assumes an
 * I/O mapped device accessed in the x86 fashion.
 */

#ifndef WRITE_WORD
#define WRITE_WORD(pDev, addr, value) \
     (sysOutWord ((pDev)->base+(addr), value))
#endif

#ifndef READ_WORD
#define READ_WORD(pDev, addr) \
     sysInWord ((pDev)->base+(addr))
#endif

/* Innovator requires some post-processing upon ISR exit. */
IMPORT void sysEsmcOnExitIsr(END_OBJ *pDev);
#define ON_EXIT_ISR(pDev) sysEsmcOnExitIsr((END_OBJ *)pDev)

#ifndef ON_EXIT_ISR
#define ON_EXIT_ISR(pDev) (void *)(0);
#endif

#ifndef SWAP_BYTES_IF_NECESSARY
#define SWAP_BYTES_IF_NECESSARY(x) (x)
#endif

#ifndef LO_BYTE
#define LO_BYTE(x) (x & 0xff)
#endif

#ifndef HI_BYTE
#define HI_BYTE(x) (x >> 8)
#endif

/* cache macros */
#ifndef ESMC_CACHE_INVALIDATE
#define ESMC_CACHE_INVALIDATE(address, len) \
        CACHE_DRV_INVALIDATE (&pDev->cacheFuncs, (address), (len))
#endif

#ifndef ESMC_CACHE_PHYS_TO_VIRT
#define SMSC_CACHE_PHYS_TO_VIRT(address) \
        CACHE_DRV_PHYS_TO_VIRT (&pDev->cacheFuncs, (address))
#endif

/* A shortcut for getting the hardware address from the MIB II stuff. */

#define END_HADDR(pEnd)    \
        ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define END_HADDR_LEN(pEnd) \
        ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)
        
/* Maximum number of ticks to delay transmission if the FIFO is full */

#define ESMC_FIFO_DELAY         (5)

/* The definition of the driver control structure */

typedef struct esmc_device
    {
    END_OBJ       endObj;          /* The class we inherit from. */
    END_ERR       lastError;       /* Last error passed to muxError */
    int           unit;            /* unit number */
    unsigned long base;            /* device I/O base address */
    int           ilevel;          /* h/w interrupt level */
    int           ivec;            /* interrupt vector */
    int           config;          /* configuration flags */
    char          enetAddr[6];     /* ethernet address */
    char          mcastFilter[8];  /* multicast filter */
    int           offset;          /* packet data offset */
    int           flags;
    unsigned short imask;
    unsigned int  rxOverrun; 
    unsigned int  lateCollisions;
    unsigned int  _16Collisions;
    unsigned int  fifoTxUnderrun;
    unsigned int  sqet;
    unsigned int  lostCarrier;
    unsigned int  alignErr;
    unsigned int  badCrc;
    unsigned int  tooLong;
    unsigned int  tooShort;

    CL_POOL_ID    clPoolId;        /* cluster pool */

    int           esmcTxStall;     /* Flag when no room for tx packet */
    int           esmcTxActive;    /* Flag that we're currently transmitting */

    int           txPacketNo;      /* Packet number for reserved TX space */
    
    } ESMC_DEVICE;


/* globals */

#ifdef    DRV_DEBUG
unsigned int esmcDebug = 0; 
#endif    /* DRV_DEBUG */

/* locals */
typedef struct
    {
    char irq;
    char reg;
    } IRQ_TABLE;

LOCAL NET_FUNCS esmcFuncTable;

/* forward declarations */
END_OBJ* esmcEndLoad (char *initString, void *);

STATUS esmcParse ( ESMC_DEVICE * pDev, char * initString );
STATUS esmcPollReceive ( END_OBJ * pObj, M_BLK_ID  pMblk );
void esmcShow(int unit);

/* imports */
IMPORT    int endMultiLstCnt (END_OBJ* pEnd);
IMPORT void netPoolShow(NET_POOL_ID);
void esmcHandleTxInt(ESMC_DEVICE  *pDev);

/*******************************************************************************
*
* esmcChipReset - reset SMC91c9x chip
*
* RETURNS: N/A
*/

LOCAL void esmcChipReset 
    (
    ESMC_DEVICE *pDev
    )
    {

    unsigned short  val;
    int i;

    DRV_LOG (DRV_DEBUG_FN_TRACE, "%s%d: esmcChipReset\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    
    ESMC_SWITCH_BANK (pDev, 2);
    WRITE_WORD(pDev,ESMC_INT_STAT, 0);        /* lock INT */

    ESMC_SWITCH_BANK (pDev,0);

    val = READ_WORD(pDev, ESMC_RCR);

    WRITE_WORD (pDev, ESMC_RCR, ESMC_RCR_EPH_RST);    /* software reset */


    val = READ_WORD(pDev, ESMC_RCR);

    WRITE_WORD (pDev, ESMC_RCR, 0x0000);        /* RX disable */
    WRITE_WORD (pDev, ESMC_TCR, 0x0000);        /* TX disable */

    pDev->esmcTxActive = 0;
    pDev->esmcTxStall = 0;

    /* Program buffer size */
    /*    WRITE_WORD(pDev, ESMC_MCR, 0x0006);             No need anymmore */
    WRITE_WORD(pDev, ESMC_MCR, 0x0000);             /* 1532 bytes */

    ESMC_SWITCH_BANK (pDev,1);

    val = READ_WORD (pDev, ESMC_CONTROL);
    WRITE_WORD (pDev, ESMC_CONTROL, val | ESMC_CTR_LE_ENABLE);  /* link error enable */
        
    ESMC_SWITCH_BANK (pDev,2);

    /* MMU reset, frees all allocated memory */
    WRITE_WORD (pDev, ESMC_MMU, ESMC_MMU_RESET);    
    

    /* allocate transmit memory */
    WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_ALLOC | (1532/256));  /* alloc mem */

    for (i = 0 ; i < 100 ; i++)
    {    
        val = READ_WORD(pDev, ESMC_INT_STAT);

        if (val &  ESMC_INT_ALLOC)
        {
           break;
        }            
        taskDelay(1);
    }
    
    /*
     * If no buffers are available,
     * release the semaphore and return END_ERR_BLOCK.
     * Do not free packet
     */
    pDev->txPacketNo = READ_WORD(pDev, ESMC_ARR - 1) >> 8;

    if (pDev->txPacketNo & 0x80) /* check for valid packet number */
    {
       DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcReset: FATAL No space avail for static TX packet\n",
                (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    }
    else
    {
       DRV_LOG (DRV_DEBUG_TX, "%s%d: esmcReset: Allocated initial TX Packet %d\n",
                (int) ESMC_DEV_NAME, pDev->unit, pDev->txPacketNo, 4, 5, 6);
    }

    }

/*******************************************************************************
*
* esmcMemInit - initialize memory for the chip
*
* Using data in the control structure, setup and initialize the memory
* areas needed.  If the memory address is not already specified, then allocate
* cache safe memory.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcMemInit
    (
    ESMC_DEVICE *    pDev,  /* device to be initialized */
    int            clNum        /* number of clusters to allocate */
    )
    {
    CL_DESC        clDesc;                      /* cluster description */
    M_CL_CONFIG    clConfig;

    bzero ((char *)&clConfig, sizeof(clConfig));
    bzero ((char *)&clDesc, sizeof(clDesc));
    
    pDev->endObj.pNetPool = (NET_POOL_ID) malloc (sizeof(NET_POOL));
    if (pDev->endObj.pNetPool == NULL)
        return (ERROR);

    clDesc.clNum   = clNum;
    clDesc.clSize  = MEM_ROUND_UP(ESMC_BUFSIZ + pDev->offset);
    clDesc.memSize = ((clDesc.clNum * (clDesc.clSize + 4)) + 4);

    clConfig.mBlkNum = clDesc.clNum * 2;
    clConfig.clBlkNum = clDesc.clNum;

    /*
     * mBlk and cluster configuration memory size initialization
     * memory size adjusted to hold the netPool pointer at the head.
     */
    clConfig.memSize =
      (clConfig.mBlkNum * (M_BLK_SZ + sizeof (long)))
      + (clConfig.clBlkNum * (CL_BLK_SZ + sizeof (long)));
    clConfig.memArea = (char *) memalign(sizeof (long),
                          clConfig.memSize);
    if (clConfig.memArea == NULL)
    {
        free (pDev->endObj.pNetPool);
        pDev->endObj.pNetPool = NULL;
        return (ERROR);
    }
    
    clDesc.memArea = (char *) malloc (clDesc.memSize);
    if (clDesc.memArea == NULL)
    {
        free (clConfig.memArea);
        free (pDev->endObj.pNetPool);
        pDev->endObj.pNetPool = NULL;
        return (ERROR);
    }

    /* Initialize the net buffer pool with transmit buffers */
    if (netPoolInit (pDev->endObj.pNetPool, &clConfig,
             &clDesc, 1, NULL) == ERROR)
    {
        free (clDesc.memArea);
        free (clConfig.memArea);
        free (pDev->endObj.pNetPool);
        pDev->endObj.pNetPool = NULL;
        return (ERROR);
    }

    pDev->clPoolId = clPoolIdGet (pDev->endObj.pNetPool,
                      ESMC_BUFSIZ + pDev->offset, FALSE);
    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_MEM_INIT, "%s%d: esmcMemInit: %d clusters\n",
         (int) ESMC_DEV_NAME, pDev->unit, clNum, 4, 5, 6);

    return (OK);
    }

/******************************************************************************
*
* update_crc - helper routine 
*
*/

static unsigned long update_crc(unsigned char byte,  unsigned long current_crc)
{
    unsigned int bit;
    unsigned char ah;
    unsigned char carry;

    ah = 0;
    for(bit = 0 ; bit < 8 ; bit++)
    {
        carry = current_crc >> 31;
        current_crc <<= 1;
        ah = ((ah << 1) | carry) ^ byte;
        if( ah & 1 )
            current_crc ^= 0x4c11db7;
        ah >>= 1;
        byte >>= 1;
    }
    return current_crc;
}
         
/******************************************************************************
*
* esmcAddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the esmcMCastAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

LOCAL void esmcAddrFilterSet
    (
    ESMC_DEVICE *    pDev        /* device pointer */
    )
    {          
    unsigned char *pCp;
    unsigned long crc;
    int           len;
    ETHER_MULTI * pCurr;

    DRV_LOG (DRV_DEBUG_FN_TRACE, "%s%d: esmcAddrFilterSet\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    
    pDev->mcastFilter[0] = 0x00;
    pDev->mcastFilter[1] = 0x00;
    pDev->mcastFilter[2] = 0x00;
    pDev->mcastFilter[3] = 0x00;
    pDev->mcastFilter[4] = 0x00;
    pDev->mcastFilter[5] = 0x00;
    pDev->mcastFilter[6] = 0x00;
    pDev->mcastFilter[7] = 0x00;

    for (pCurr = END_MULTI_LST_FIRST (&pDev->endObj);
         pCurr != NULL;
         pCurr = END_MULTI_LST_NEXT(pCurr))
    {
        /*
         * AUTODIN-II
         */
        pCp = (unsigned char *)&pCurr->addr;
        crc = 0xffffffff;
        for (len = 6; --len >= 0;)
            crc = update_crc(*pCp++, crc);

        /* Just want the 6 most significant bits. */
        crc = crc >> 26;

        /* Turn on the corresponding bit in the filter. */
        pDev->mcastFilter [crc >> 3] |= (1 << (crc & 0x07));
    }
    }


/******************************************************************************
*
* esmcConfig - reconfigure the interface under us.
*
* Reconfigure the interface setting promiscuous mode, and changing the
* multicast interface list.
*
* RETURNS: N/A.
*/

LOCAL void esmcConfig
    (
    ESMC_DEVICE *    pDev    /* device to be re-configured */
    )
    {
    USHORT   val;
    USHORT   i;

    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_CONFIG, "%s%d: esmcConfig: flags=%x\n",
         (int) ESMC_DEV_NAME, pDev->unit, pDev->flags, 4, 5, 6);
      
    /* reset the chip */
    esmcChipReset(pDev);

    /* get/set interface */
    ESMC_SWITCH_BANK (pDev,1);
    val = READ_WORD(pDev, ESMC_CONFIG);

    if(pDev->config == ATTACHMENT_DEFAULT)    /* EEPROM */
    {
        if (val & ESMC_CFG_AUI_SELECT)        /* EEPROM - AUI */
            pDev->config = ATTACHMENT_AUI;
        else                                  /* EEPROM - RJ45 */
        {
            pDev->config = ATTACHMENT_RJ45;
            WRITE_WORD(pDev,ESMC_CONFIG, val);
        }
    }
    else if (pDev->config == ATTACHMENT_AUI)        /* AUI */
    {
        WRITE_WORD (pDev,ESMC_CONFIG, val | ESMC_CFG_AUI_SELECT);
    }
    else if (pDev->config == ATTACHMENT_RJ45)        /* RJ45 */
    {
        WRITE_WORD (pDev,ESMC_CONFIG, val & ~ESMC_CFG_AUI_SELECT);
    }
 
    if(END_MULTI_LST_CNT(&pDev->endObj) > 0)
        esmcAddrFilterSet (pDev);

    if(END_FLAGS_GET(&pDev->endObj) & IFF_PROMISC)
    {
        ESMC_SWITCH_BANK (pDev,0);
        val = READ_WORD(pDev, ESMC_RCR);
        WRITE_WORD(pDev, ESMC_RCR, val | 0x02 );

        DRV_LOG (DRV_DEBUG_CONFIG | DRV_DEBUG_IOCTL, 
          "%s%d: Setting promiscuous mode on!\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    }
    
    if(END_FLAGS_GET(&pDev->endObj) & IFF_ALLMULTI)
    {
        ESMC_SWITCH_BANK (pDev,0);
        val = READ_WORD(pDev, ESMC_RCR);
        WRITE_WORD(pDev, ESMC_RCR, val | 0x04);

        DRV_LOG (DRV_DEBUG_CONFIG | DRV_DEBUG_IOCTL, 
          "%s%d: Setting all multicast mode on!\n",
          (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);


    }


    /* Program ethernet address */
    ESMC_SWITCH_BANK (pDev,1);
    for(i = 0 ; i < 6 ; i+= 2)
    {
        val = pDev->enetAddr[i] | (pDev->enetAddr[i+1] << 8);
        WRITE_WORD (pDev,ESMC_ADDR_0 + i ,val);
    }

    /* Program multicast mask  */
    ESMC_SWITCH_BANK (pDev,3);
    for(i = 0 ; i < 8 ; i+=2)
    {
        val =  pDev->mcastFilter[i] | (pDev->mcastFilter[i+1] << 8);
        WRITE_WORD (pDev, ESMC_MULTICAST0 + i,val);
    }

    ESMC_SWITCH_BANK (pDev,2);

    return;

    }

/******************************************************************************
*
* esmcPollStart - start polled mode operations
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcPollStart
    (
    ESMC_DEVICE *pDev
    )
    {     
    int oldLevel;
    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_POLL, "%s%d: esmcPollStart\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    oldLevel = intLock ();
    ESMC_SWITCH_BANK (pDev,2);   
    WRITE_WORD(pDev,ESMC_INT_STAT, 0);             /* mask all interrupts */
    pDev->flags |= END_POLLING;
    intUnlock (oldLevel);

    return (OK);
    }

/******************************************************************************
*
* esmcPollStop - stop polled mode operations
*
* This function terminates polled mode operation.  The device returns to
* interrupt mode.
*
* The device interrupts are enabled, the current mode flag is switched
* to indicate interrupt mode and the device is then reconfigured for
* interrupt operation.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcPollStop
    (
    ESMC_DEVICE *pDev
    )
    {     
    int    oldLevel;

    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_POLL, "%s%d: esmcPollStop\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    oldLevel = intLock ();
    ESMC_SWITCH_BANK (pDev,2);                             /* int enable */
    WRITE_WORD (pDev, ESMC_INT_STAT, pDev->imask); 
    pDev->flags &= ~END_POLLING;
    intUnlock (oldLevel);

    return (OK);
    }


/* Additions to support setting new MAC address */
#define IA_ADDRESS_OFFSET  0x20
/*******************************************************************************
*
* esmcSetMACAddr - assign new MAC address
*
* This routine assigns a new MAC address to the SMC chip and writes it out
* to serial EPROM.
*
* RETURNS: N/A
*/

LOCAL void esmcSetMACAddr
    (
    ESMC_DEVICE *pDev,   /* pointer to device to change */
    char *macAddr        /* new MAC address as an array of 6 bytes */
    )
{
    UINT16 val, currentWord, status, i;
    int key;

    key = intLock();

    ESMC_SWITCH_BANK (pDev, 1);  /* read and save initial value in control register */
    val = READ_WORD (pDev, ESMC_CONTROL);

    for (i = 0; i < 3; i++)
    {
        ESMC_SWITCH_BANK (pDev, 2);
        WRITE_WORD(pDev, ESMC_PTR, IA_ADDRESS_OFFSET + i);  /* set EEPROM dest address offset */

        currentWord = (macAddr[(i * 2)] | macAddr[(i * 2) + 1] << 8);     /* set up data */
        ESMC_SWITCH_BANK (pDev, 1);
        WRITE_WORD(pDev, ESMC_GENERAL, currentWord);        /* write current word of MAC addr to GA register */

        WRITE_WORD(pDev, ESMC_CONTROL, val | ESMC_CTR_EPROM_SELECT | ESMC_CTR_STORE);  /* perform the write */

        /* Must poll before continuing: */
        do
        {
            status = READ_WORD(pDev, ESMC_CONTROL);
        }
        while (status & ESMC_CTR_STORE);  /* wait for bit to clear */
    }

    /* update IADDR register from EEPROM */
    WRITE_WORD(pDev, ESMC_CONTROL, val | ESMC_CTR_EPROM_SELECT | ESMC_CTR_RELOAD);  /* perform the write */

    /* Must poll before continuing: */
    do
    {
        status = READ_WORD(pDev, ESMC_CONTROL);
    }
    while (status & ESMC_CTR_STORE);  /* wait for bit to clear */

    /* populate private structure with new values */
    for(i = 0 ; i < 6 ; i+=2)
    {
        val = READ_WORD(pDev, ESMC_ADDR_0 + i);
        pDev->enetAddr[i] = LO_BYTE(val);
        pDev->enetAddr[i+1] = HI_BYTE(val);        
    }

    /* restore original ESMC_CONTROL value */
    WRITE_WORD(pDev, ESMC_CONTROL, val);  /* perform the write */

    intUnlock(key);
}

/*******************************************************************************
*
* esmcIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, usually OK or ERROR.
*/

LOCAL int esmcIoctl
    (
    END_OBJ * pObj,     /* device receiving command */
    int cmd,            /* ioctl command code */
    caddr_t data        /* command argument */
    )
    {
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;
    int error = 0;
    long value;

    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_IOCTL, "%s%d: esmcIoctl: cmd=0x%0x\n",
         (int) ESMC_DEV_NAME, pDev->unit, cmd, 4, 5, 6);

    switch ((unsigned long)cmd)
    {
        case EIOCSADDR:      /* set MAC address */
            if (data == NULL)
                error =  EINVAL;
            else
            {
                bcopy ((char *)data, (char *)END_HADDR(&pDev->endObj),
                END_HADDR_LEN(&pDev->endObj));
                esmcSetMACAddr(pDev, data);  /* Commit change to EEPROM */
            }
            DRV_LOG(DRV_DEBUG_IOCTL, "%s%d: EIOCSADDR result = %d\n",
             (int) ESMC_DEV_NAME, pDev->unit, error, 4, 5, 6);

            break;

        case EIOCGADDR:        /* get MAC address */
            if (data == NULL)
                error = EINVAL;
            else
            {
                bcopy ((char *)END_HADDR(&pDev->endObj), (char *)data,
                END_HADDR_LEN(&pDev->endObj));
            }
            
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCGADDR  result = %d\n",
             (int) ESMC_DEV_NAME, pDev->unit, error, 4, 5, 6);

            break;

        case EIOCSFLAGS:    /* set (or clear) flags */
            value = (long)data;
            if (value < 0)
            {
                value = -value;
                value--;
                END_FLAGS_CLR (&pDev->endObj, value);
            }
            else
            {
                END_FLAGS_SET (&pDev->endObj, value);
            }
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCSFLAGS  flags = 0x%0x\n",
             (int) ESMC_DEV_NAME, pDev->unit, value, 4, 5, 6);

            esmcConfig (pDev);
            break;

        case EIOCGFLAGS:    /* get flags */
            *(int *)data = END_FLAGS_GET(&pDev->endObj);
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCGFLAGS  flags = 0x%0x\n",
             (int) ESMC_DEV_NAME, pDev->unit, *(int *)data, 4, 5, 6);
            break;

        case EIOCPOLLSTART:    /* Begin polled operation */
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCPOLLSTART\n",
             (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
            esmcPollStart (pDev);
            break;

        case EIOCPOLLSTOP:    /* End polled operation */
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCPOLLSTOP\n",
             (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
            esmcPollStop (pDev);
            break;

        case EIOCGMIB2:        /* return MIB information */
            if (data == NULL)
                error = EINVAL;
            else
            {
                bcopy((char *)&pDev->endObj.mib2Tbl, (char *)data,
                      sizeof(pDev->endObj.mib2Tbl));
            }

            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCGMIB2  result = %d\n",
             (int) ESMC_DEV_NAME, pDev->unit, error, 4, 5, 6);

            break;

        case EIOCGHDRLEN:
            if (data == NULL)
                error = EINVAL;
            else
                *(int *)data = SIZEOF_ETHERHEADER;

            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: EIOCGHDRLEN  result = %d\n",
             (int) ESMC_DEV_NAME, pDev->unit,error, 4, 5, 6);

            break;

        default:        /* unknown request */
            DRV_LOG (DRV_DEBUG_IOCTL, "%s%d: Unknown request\n",
             (int) ESMC_DEV_NAME, pDev->unit,3, 4, 5, 6);

            error = EINVAL;
        }

    return (error);
    }


/******************************************************************************
*
* esmcHandleRcvInt -
*
* RETURNS: N/A.
*/

LOCAL void esmcHandleRcvInt
(
    ESMC_DEVICE  *pDev
)
{
    unsigned char  *pBuf;
    unsigned short *pCur;
    unsigned short packetNo;
    unsigned short statusRx;
    unsigned short len;
    unsigned short i;
    unsigned short val;

    int            oldLevel;
    
    CL_BLK_ID    pClBlk;
    M_BLK_ID    pMblk;      /* MBLK to send upstream */
    int maxLoops = 16;

    DRV_LOG (DRV_DEBUG_FN_TRACE | DRV_DEBUG_RX, "%s%d: esmcHandleRcvInt\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    ESMC_SWITCH_BANK (pDev, 2);

    while(!((packetNo = READ_WORD(pDev,ESMC_FIFO)) & ESMC_FIFO_REMPTY) && 
          (maxLoops > 0))
        {
        /* Lock the transmit path so that nobody tries to use the data 
        register while we're downloading to it */
        END_TX_SEM_TAKE (&pDev->endObj, WAIT_FOREVER);

        WRITE_WORD (pDev,ESMC_PTR, ESMC_PTR_READ | ESMC_PTR_RCV | ESMC_PTR_AUTOINC);
        
        statusRx = READ_WORD(pDev, ESMC_DATA_1);    /* status */
        len = READ_WORD (pDev, ESMC_DATA_1);   /* byte count */

        DRV_LOG (DRV_DEBUG_RX, "%s%d: esmcHandleRcvInt: statusRx = 0x%0x len = 0x%0x\n",
                 (int) ESMC_DEV_NAME, pDev->unit, statusRx, len, 0, 0);

        /* check for receive errors */
        if ((statusRx & ESMC_RS_ERROR_MASK) || (len == 0))
        {
           /* increment appropriate error counter */
            if(statusRx &  ESMC_RS_ALGNERR)
                pDev->alignErr++;
            
            if(statusRx &  ESMC_RS_BADCRC)
                pDev->badCrc++;

            if(statusRx &  ESMC_RS_TOOLONG)
                pDev->tooLong++;

            if(statusRx &  ESMC_RS_TOOSHORT)
                pDev->tooShort++;

            do
            {
                 val = READ_WORD(pDev, ESMC_MMU);
            } while(val & 0x0001);

            WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */

            END_TX_SEM_GIVE (&pDev->endObj);
        }
        else
        {
            pBuf = netClusterGet (pDev->endObj.pNetPool, pDev->clPoolId);
            if (!pBuf)
            {
               DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcHandleRcvInt: Could not allocate cluster\n",
                 (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0);

                /* re-schedule ourselves */
                netJobAdd((FUNCPTR) esmcHandleRcvInt,(int) pDev, 0, 0, 0, 0);
                END_TX_SEM_GIVE (&pDev->endObj);
                goto exit;

            }

            pMblk = mBlkGet (pDev->endObj.pNetPool, M_DONTWAIT, MT_DATA);
            if (!pMblk)
        {
                netClFree (pDev->endObj.pNetPool, (UCHAR *) pBuf);
                DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcHandleRcvInt: Could not allocate mBlk\n",
                  (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0);

                /* re-schedule ourselves */
                netJobAdd((FUNCPTR) esmcHandleRcvInt,(int) pDev, 0, 0, 0, 0);
                END_TX_SEM_GIVE (&pDev->endObj);
                goto exit;
        }
                    
            pClBlk = netClBlkGet (pDev->endObj.pNetPool, M_DONTWAIT);
            if (!pClBlk)
        {
                netMblkFree (pDev->endObj.pNetPool, (M_BLK_ID)pMblk);
                netClFree (pDev->endObj.pNetPool, (UCHAR *) pBuf);
                DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcHandleRcvInt: Could not allocate clBlk\n",
                  (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0);

                /* re-schedule ourselves */
                netJobAdd((FUNCPTR) esmcHandleRcvInt,(int) pDev, 0, 0, 0, 0);
                END_TX_SEM_GIVE (&pDev->endObj);
                goto exit;
        }

            /* Associate the data pointer with the MBLK */
            netClBlkJoin (pClBlk, pBuf, ESMC_BUFSIZ, NULL, 0, 0, 0);
    
            /* Associate the data pointer with the MBLK */
            netMblkClJoin (pMblk, pClBlk);

            /* finally - read in the packet */
            pCur = (unsigned short *) (pBuf + pDev->offset);

            /* adjust len for first 4 bytes read */
            len -=4;
            for(i = 0 ; i < (len >> 1) ; i++)
            {
                val =  READ_WORD(pDev,ESMC_DATA_1);
                pCur[i] = SWAP_BYTES_IF_NECESSARY(val);
            }

            /* adjust length for control byte and odd byte */
            if (statusRx & ESMC_RS_ODDFRM)
                len -= 1;
            else
                len -= 2;

            do
            {
                 val = READ_WORD(pDev, ESMC_MMU);
            } while(val & 0x0001);

        

            WRITE_WORD(pDev,ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */
             
            END_TX_SEM_GIVE (&pDev->endObj);

            pMblk->mBlkHdr.mFlags |= M_PKTHDR;
            pMblk->mBlkHdr.mLen    = len;
            pMblk->mBlkPktHdr.len  = len;
            pMblk->mBlkHdr.mData  += pDev->offset;

            /* Call the upper layer's receive routine. */
            END_RCV_RTN_CALL (&pDev->endObj, pMblk);

            /* Record received packet */
            END_ERR_ADD (&pDev->endObj, MIB2_IN_UCAST, +1);
        }
        
        /* Decrement the maxLoops counter.  Restricting the number of RX
        frames that can be handled in one instance of the RX routine prevents
        the RX handler from hogging the netJobTask.  This can be a problem in
        situations with a steady flow of RX and TX packets, such as FTP. */
        maxLoops --;
        
        /* Free the MUX TX sem in case a higher priority task is waiting for
        access to the resources */
       
    }             

    if (maxLoops == 0)
        {
       DRV_LOG (DRV_DEBUG_RX, "%s%d: esmcHandleRcvInt: RX hit max iterationsn",
         (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0);
        netJobAdd((FUNCPTR) esmcHandleRcvInt,(int) pDev, 0, 0, 0, 0);
        return;
        }

    /* re-enable receive interrupt */
    oldLevel = intLock();
    pDev->flags &= ~ESMC_RCV_HANDLING_FLAG;
    pDev->imask |= (ESMC_INT_RCV << 8);
    intUnlock(oldLevel);
exit:
    oldLevel = intLock();
    WRITE_WORD(pDev, ESMC_INT_STAT, pDev->imask);
    intUnlock(oldLevel);

    return;
}

/******************************************************************************
*
* esmcIsr - handle controller interrupt
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/

LOCAL void esmcIsr
(
    ESMC_DEVICE  *pDev
)

{
    unsigned short bank;
    unsigned short val;
    unsigned char  status;

    bank = READ_WORD(pDev,ESMC_BANK_SELECT);   /* save bank */
      if((bank & 0x03) != 2)
        ESMC_SWITCH_BANK (pDev,2);

    val = READ_WORD(pDev, ESMC_INT_STAT);
    DRV_ASSERT(((val & 0xff00) & ~pDev->imask) == 0);

    WRITE_WORD(pDev,ESMC_INT_STAT, 0);  /* mask all interrupts */
    DRV_ASSERT((READ_WORD(pDev,ESMC_BANK_SELECT) & 0x03) == 2);

    DRV_LOG (DRV_DEBUG_FN_TRACE, "%s%d: esmcIsr status=%x\n",
         (int) ESMC_DEV_NAME, pDev->unit, val, 4, 5, 6);

    status = LO_BYTE(val);  /* Don't use the mask so we can ACK RXOVRN bits */

    if(status == 0)
        goto exit;

    if (status & (ESMC_INT_RCV | ESMC_INT_RX_OVRN)) /* RX done */
    {
        if(!(pDev->flags & ESMC_RCV_HANDLING_FLAG))
        {
            if (netJobAdd ((FUNCPTR) esmcHandleRcvInt,
                    (int) pDev, 0, 0, 0, 0) == ERROR)
            {
               DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcIsr: netJobAdd failed\n",
                 (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0);

                /* failed netJobAdd causes panic */
                goto exit;
             }
             else
             {
                 pDev->flags |= ESMC_RCV_HANDLING_FLAG;
                 pDev->imask &= ~(ESMC_INT_RCV << 8);  /* disable further receive interrupts */
             }
        }

        if (status & ESMC_INT_RX_OVRN)
        {
            pDev->rxOverrun++;

            /* acknowledge the interrupt */
            WRITE_WORD(pDev,ESMC_INT_ACK, ESMC_INT_RX_OVRN);
            DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcIsr: Receive Overrun-> esmcTxStall = %d\n",
                    (int) ESMC_DEV_NAME, pDev->unit, pDev->esmcTxStall, 0, 0, 0);
        }
    }


    if (status & ESMC_INT_TX_EMPTY) /* transmit que empty */
    {

        /* disable further interrupts */
        pDev->imask &= ~(ESMC_INT_TX_EMPTY << 8);        

        /* acknowledge the interrupt */
        WRITE_WORD(pDev,ESMC_INT_ACK, ESMC_INT_TX_EMPTY);

        DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcIsr: Transmit Que Empty-> esmcTxStall = %d\n",
                (int) ESMC_DEV_NAME, pDev->unit, pDev->esmcTxStall, 0, 0, 0);

    }

    if (status & ESMC_INT_TX) /* transmit complete */
    {
       if(!(pDev->flags & ESMC_TX_HANDLING_FLAG)) {
          pDev->imask &= ~(ESMC_INT_TX << 8);
          netJobAdd((FUNCPTR)esmcHandleTxInt, (int)pDev,0,0,0,0);
          pDev->flags |= ESMC_TX_HANDLING_FLAG;
       }
    }
   
exit:    
    /* do cleanup if necessary */
    ON_EXIT_ISR(pDev);                

    /* restore mask and bank */
    WRITE_WORD(pDev, ESMC_INT_STAT, pDev->imask);
    ESMC_SWITCH_BANK (pDev,bank);
    return;
}

/******************************************************************************
*
* esmcStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS esmcStart
    (
    END_OBJ *pObj        /* device ID */
    )
    {
    STATUS result;
    int    oldLevel;
    ESMC_DEVICE * pDev = (ESMC_DEVICE *)pObj;
    
    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcStart\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    
    
    oldLevel = intLock();
    SYS_INT_CONNECT (pDev, esmcIsr, (int)pDev, &result);
    intUnlock(oldLevel);
    
    if (result == ERROR)
    {
    DRV_LOG (DRV_DEBUG_LOAD,
         "%s%d: esmcStart: could not attach interrupt\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
    return (ERROR);
    }


    /* reset error counters */
    pDev->rxOverrun = 0;
    pDev->lateCollisions = 0;
    pDev->_16Collisions = 0;
    pDev->fifoTxUnderrun = 0;
    pDev->sqet = 0;
    pDev->lostCarrier = 0;
    pDev->alignErr = 0;
    pDev->badCrc = 0;
    pDev->tooLong = 0;
    pDev->tooShort = 0;

    ESMC_SWITCH_BANK (pDev,0);
    WRITE_WORD (pDev, ESMC_TCR, ESMC_TCR_PAD | ESMC_TCR_TXEN); /* TXenable */
    WRITE_WORD (pDev, ESMC_RCR, ESMC_RCR_RXEN);                /* RX enable */


    /* Enable interrupts on the chip:
     *
     * ERCV: (Early Receive Interrupt) set whenever a packet is beeing
     *     received, and the number of bytes received into memory exeeds
     *     the value programmed as ERCV_THRESHOLD.
     * EPH: (Ethernet protocol Handler) is the result of the merge of
     *     several possible conditions.
     * RX_OVRN: The receiver aborts due to an overrun due to failed
     *     memory allocation.
     * TX_EMPTY: set if the TX FIFO goes empty, normally after a
     *     succesful transmission.
     * TX: set when at least one packet transmission was
     *     completed. After servicing a packet in the FIFO the TX is set.
     * RCV: set when a receive interrupt has been genetated, the first
     *     packet received can be read from FIFO.
     */


    /* mark the interface as up and running */
    END_FLAGS_SET (&pDev->endObj, (IFF_UP | IFF_RUNNING));
     
    pDev->imask =  ( /*ESMC_INT_RX_OVRN | */ ESMC_INT_RCV |  /*ESMC_INT_ERCV | */ ESMC_INT_TX) << 8;

    oldLevel = intLock();    
    ESMC_SWITCH_BANK (pDev,2);    /* int enable */
    WRITE_WORD(pDev, ESMC_INT_STAT, pDev->imask);

    SYS_INT_ENABLE(pDev);
    intUnlock(oldLevel);

    return (OK);
    }

/*******************************************************************************
*
* esmcStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcStop
    (
    END_OBJ *pObj    /* device to be stopped */
    )
    {
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;
    STATUS result = OK;
    USHORT val;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcStop\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    END_FLAGS_CLR (&pDev->endObj, IFF_UP | IFF_RUNNING);

    /* stop/disable the device. */
    ESMC_SWITCH_BANK (pDev, 2);
    WRITE_WORD(pDev,ESMC_INT_STAT, 0);        /* mask all interrupts INT */

    ESMC_SWITCH_BANK (pDev,0);
    val = READ_WORD(pDev, ESMC_TCR);
    WRITE_WORD (pDev, ESMC_TCR, val & ~ESMC_TCR_TXEN);  /* TX disable */

    val = READ_WORD(pDev, ESMC_RCR);
    WRITE_WORD (pDev, ESMC_RCR, val & ~ESMC_RCR_RXEN);  /* RX disable */

    SYS_INT_DISCONNECT (pDev, (FUNCPTR)esmcIsr, (int)pDev, &result);

    if (result == ERROR)
    {
    DRV_LOG (DRV_DEBUG_LOAD, "Could not disconnect interrupt!\n",
        1, 2, 3, 4, 5, 6);
    }

    ESMC_SWITCH_BANK (pDev, 2);

    return (result);
    }

/******************************************************************************
*
* esmcUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcUnload
    (
    END_OBJ *pObj    /* device to be unloaded */
    )
    {
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcUnload\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    END_OBJECT_UNLOAD (&pDev->endObj);

    netPoolDelete (pDev->endObj.pNetPool);
    free(pDev->endObj.pNetPool);
    pDev->endObj.pNetPool = NULL;

    return OK;
    }

/*******************************************************************************
*
* esmcSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.
*
* RETURNS: OK, ERROR, or END_ERR_BLOCK.
*/
   int txSleep = -1;
LOCAL STATUS esmcSend
    (
    END_OBJ *pObj,
    M_BLK_ID     pMblk        /* data to send */
    )
{
/*    unsigned int    pages; */
    unsigned short  packetNo;
    unsigned short  val = 0;   
    unsigned int    ndx;
    unsigned int    len; 
    unsigned int    writeOut;
    int             oldLevel;
    struct timespec delay;
    /*    unsigned int    i; */
    STATUS          erCode = OK;


    M_BLK_ID        pCur = pMblk;
    
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;
    
    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcSend\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    /* Only allow one TX packet at a time so that we don't burn up all 
    of the FIFO space */
    if (pDev->esmcTxActive == TRUE)
        {
        DRV_LOG (DRV_DEBUG_ASSERT, "%s%d: esmcSend: Throttled\n",
                 (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);
        pDev->esmcTxStall = 1;
        return END_ERR_BLOCK;
        }

    /*
     * Obtain exclusive access to transmitter.  This is necessary because
     * we might have more than one stack transmitting at once.
     */

    END_TX_SEM_TAKE (&pDev->endObj, WAIT_FOREVER);

    if (txSleep == pDev->unit) {
       delay.tv_nsec = 900;
       delay.tv_sec = 0;

       if ( nanosleep(&delay, NULL) != OK ) {

          DRV_LOG ( DRV_DEBUG_RX, "%s%d: esmcHandleRcvInt: NANOSLEEP FAILED!\n",
                    (int) ESMC_DEV_NAME, pDev->unit, 0, 0, 0, 0 );
       }

    }

     /* get length from mblks */
    len = 0;
    while(pCur != NULL)
    {
        len += pCur->mBlkHdr.mLen;
        pCur = pCur->mBlkHdr.mNext;
    }

    packetNo = pDev->txPacketNo;
    
    WRITE_WORD(pDev,ESMC_PNR, packetNo);          /* set the tx packet */
    WRITE_WORD( pDev,ESMC_PTR, ESMC_PTR_AUTOINC); /* set the pointer */

    /* copy the data to the device */
    WRITE_WORD(pDev, ESMC_DATA_1, 0);             /* status */
    WRITE_WORD(pDev, ESMC_DATA_1, (len + 6));     /* byte count */

    pCur = pMblk;
    writeOut = 0; 

    while(pCur)
       {
       len = pCur->mBlkHdr.mLen;

          for(ndx = 0 ; ndx < pCur->mBlkHdr.mLen; ndx++)
          {
             if (writeOut == 1)
             {
                writeOut = 0;

                val |= ((unsigned char)(pCur->mBlkHdr.mData[ndx]) << 8);
                WRITE_WORD(pDev,ESMC_DATA_1, val);
             }
            else
            {
                writeOut = 1;
                val = (unsigned char )pCur->mBlkHdr.mData[ndx];
            }
          }

          pCur = pCur->mBlkHdr.mNext;
       }
            
       /* check for odd number of bytes */
       if (writeOut == 1)
       {
         /*
         * For odd size frames, write the last data byte
         * and the Control byte as 0x20 (ODD=1).
         */
          WRITE_WORD(pDev,ESMC_DATA_1, 0x2000 | val);
       }
       else
       {
         /*
          * For even size frames, write the last data byte as zero
          * and the Control byte as 0.
         */
          WRITE_WORD(pDev,ESMC_DATA_1, 0);
       }

       DRV_ASSERT((READ_WORD(pDev,ESMC_BANK_SELECT) & 0x03) == 2);
       WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_ENQUEUE); /* send it */

       pDev->esmcTxActive = TRUE;

       END_TX_SEM_GIVE (&pDev->endObj);

       DRV_LOG (DRV_DEBUG_RX, "%s%d: esmcSend: TX_SEM_GIVE\n",
            (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

       /* Bump the statistics counters. */
       END_ERR_ADD (&pDev->endObj, MIB2_OUT_UCAST, +1);

       /*
        * Cleanup.  The driver must either free the packet now or
        * set up a structure so it can be freed later after a transmit
        * interrupt occurs.
       */

       netMblkClChainFree (pMblk);
    

    oldLevel = intLock();
    WRITE_WORD(pDev,ESMC_INT_STAT, pDev->imask);        
    intUnlock(oldLevel);

    return erCode;
}

/*****************************************************************************
*
* esmcMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcMCastAdd
    (
    END_OBJ *pObj,        /* device pointer */
    char *    pAddress    /* new address to add */
    )
    {     
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;    /* device pointer */

    unsigned int i;
    unsigned short val;
    int            oldLevel;

    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcMCastAdd\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    if (etherMultiAdd (&pDev->endObj.multiList, pAddress) == ENETRESET)
    {
        esmcAddrFilterSet(pDev);

        /* disable interrupts while we change filter */
        oldLevel = intLock();
        ESMC_SWITCH_BANK (pDev, 2);
        WRITE_WORD(pDev,ESMC_INT_STAT, 0);        /* lock INT */
        intUnlock(oldLevel);
        
        ESMC_SWITCH_BANK (pDev, 3);
        for(i = 0 ; i < 8 ; i+=2)
        {
            val = pDev->mcastFilter[i] | (pDev->mcastFilter[i+1] << 8); 
            WRITE_WORD(pDev, ESMC_MULTICAST0 + i,val);
        }

        /* re-enable interrupts */
        oldLevel = intLock();
        ESMC_SWITCH_BANK (pDev, 2);
        WRITE_WORD(pDev,ESMC_INT_STAT, pDev->imask);        /* unlock INT */
        intUnlock(oldLevel);
    }
    return (OK);
    }

/*****************************************************************************
*
* esmcMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcMCastDel
    (
    END_OBJ *    pObj,        /* device pointer */
    char *    pAddress    /* address to be deleted */
    )
    {
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;    /* device pointer */

    unsigned int i;
    unsigned short val;
    int            oldLevel;
    
    DRV_LOG (DRV_DEBUG_LOAD, "%s%d: esmcMCastDel\n",
         (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

    if (etherMultiDel (&pDev->endObj.multiList, pAddress) == ENETRESET)
    {
        esmcAddrFilterSet(pDev);

        /* disable interrupts while we change filter */
        oldLevel = intLock();
        ESMC_SWITCH_BANK (pDev, 2);
        WRITE_WORD(pDev,ESMC_INT_STAT, 0);        /* lock INT */
        intUnlock(oldLevel);
        
        ESMC_SWITCH_BANK (pDev, 3);
        for(i = 0 ; i < 8 ; i+=2)
        {
            val = pDev->mcastFilter[i] | (pDev->mcastFilter[i+1] << 8); 
            WRITE_WORD(pDev, ESMC_MULTICAST0 + i,val);
        }

        /* re-enable interrupts */
        oldLevel = intLock();
        ESMC_SWITCH_BANK (pDev, 2);
        WRITE_WORD(pDev,ESMC_INT_MASK, pDev->imask);        /* unlock INT */
        intUnlock(oldLevel);                
    }
    return (OK);
    }

/*****************************************************************************
*
* esmcMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS esmcMCastGet
    (
    END_OBJ *    pObj,        /* device pointer */
    MULTI_TABLE    *    pTable        /* address table to be filled in */
    )
    {     
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;    /* device pointer */

    return (etherMultiGet (&pDev->endObj.multiList, pTable));
    }

/******************************************************************************
*
* esmcParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* The initialization string format is:
* .CS
*   "unit:adrs:vecnum:intLvl:byteAccess:usePromEnetAddr:offset"
* .CE
*
* .IP <unit>
* Device unit number, a small integer.
* .IP <adrs>
* Base address
* .IP <vecNum>
* Interrupt vector number (used with sysIntConnect)
* .IP <intLvl>
* Interrupt level (used with sysLanIntEnable)
* .IP <byteAccess>
* Use 8-bit access mode.
* .IP <usePromEnetAddr>
* get ethernet address from PROM.
* .IP <offset>
* offset for memory alignment
* .LP
*
* RETURNS: OK or ERROR for invalid arguments.
*/

STATUS esmcParse
    (
    ESMC_DEVICE * pDev,
    char * initString
    )
    {
    char*    tok;
    char*    holder = NULL;

    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->unit = atoi (tok);

    /* Base address. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->base = strtoul (tok, NULL, 16);

    /* Interrupt vector */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->ivec = strtoul (tok, NULL, 16);

    /* Interrupt level. */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->ilevel = strtoul (tok, NULL, 16);

    /* config */
    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->config = strtoul (tok, NULL, 16);


    /* memory alignment offset */

    tok = strtok_r (NULL, ":", &holder);
    if (tok == NULL)
    return (ERROR);
    pDev->offset = strtoul (tok, NULL, 16);

    return (OK);
    }

/*******************************************************************************
*
* esmcPollSend - the driver polled-mode send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.
*
* RETURNS: OK, ERROR, or END_ERR_BLOCK.
*/

LOCAL STATUS esmcPollSend
    (
    END_OBJ *pObj,
    M_BLK_ID     pMblk        /* data to send */
    );
LOCAL STATUS esmcPollSend
    (
    END_OBJ *pObj,
    M_BLK_ID     pMblk        /* data to send */
    )
    {
    unsigned int    pages;
    unsigned short  packetNo;
    unsigned short  val = 0;   
    unsigned int    ndx; 
    unsigned int    len; 
    unsigned int    writeOut;
    unsigned int    i;

    M_BLK_ID        pCur = pMblk;
    
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;

    /* get length from mblks */
    len = 0;
    while(pCur != NULL)
    {
        len += pCur->mBlkHdr.mLen;
        pCur = pCur->mBlkHdr.mNext;
    }

    DRV_LOG (DRV_DEBUG_POLL_TX, "%s%d: esmcPollSend, length = %d\n",
         (int) ESMC_DEV_NAME, pDev->unit, len, 4, 5, 6);

    pages = len / 256;
    
    ESMC_SWITCH_BANK (pDev, 2);

    WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_ALLOC | pages);  /* alloc mem */

    for(i = 0 ; i < 10 ; i++)
    {
        val = READ_WORD(pDev, ESMC_INT_STAT);
        if(val & ESMC_INT_ALLOC)
            break;
    }
    
    /*
     * If no buffers are available, return END_ERR_BLOCK.
     */
    packetNo = READ_WORD(pDev, ESMC_ARR - 1) >> 8;
    if(packetNo & 0x80) /* check for valid packet number */
    {
        return END_ERR_BLOCK;
    }
 
    WRITE_WORD(pDev,ESMC_PNR, packetNo);          /* set the tx packet */
    WRITE_WORD( pDev,ESMC_PTR, ESMC_PTR_AUTOINC); /* set the pointer */

    /* copy the data to the device */
    WRITE_WORD(pDev, ESMC_DATA_1, 0);          /* status */
    WRITE_WORD(pDev, ESMC_DATA_1, (len + 6));  /* byte count */


    pCur = pMblk;
    writeOut = 0; 

    while(pCur)
    {
        len = pCur->mBlkHdr.mLen;

        for(ndx = 0 ; ndx < pCur->mBlkHdr.mLen; ndx++)
        {
            if(writeOut == 1)
            {
                writeOut = 0;
                val |= (unsigned char)(pCur->mBlkHdr.mData[ndx] << 8);
                WRITE_WORD(pDev,ESMC_DATA_1, val);
            }
            else
            {
                writeOut = 1;
                val = (unsigned char)pCur->mBlkHdr.mData[ndx];
            }
        }

        pCur = pCur->mBlkHdr.mNext;
    }
            
    /* check for odd number of bytes */
    if(writeOut == 1)
    {
        WRITE_WORD(pDev,ESMC_DATA_1, 0x2000 | val);
    }
    else
    {
        WRITE_WORD(pDev,ESMC_DATA_1, 0);
    }

    DRV_ASSERT((READ_WORD(pDev,ESMC_BANK_SELECT) & 0x03) == 2);
    WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_ENQUEUE); /* send it */

    /* Bump the statistics counters. */
    END_ERR_ADD (&pDev->endObj, MIB2_OUT_UCAST, +1);

    /*
     * The packet has been handed over to the device so it is
     * not necessary to wait for completion of the tx packet
     * (by checking TX_INT in the status register) before
     * the user can free the mblk.
     */

    return (OK);
    }

/*******************************************************************************
*
* esmcPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURN: OK 
*/
STATUS esmcPollReceive
    (
    END_OBJ * pObj,
    M_BLK_ID  pMblk
    )
    {
    ESMC_DEVICE *pDev = (ESMC_DEVICE *)pObj;

    unsigned char  status;
    unsigned short statusRx;
    unsigned short len;
    unsigned short val;

    unsigned int   i;
    unsigned short *pCur;

    ESMC_SWITCH_BANK(pDev,2);
    
    val = READ_WORD(pDev, ESMC_INT_STAT);

    status = LO_BYTE(val);

    DRV_LOG (DRV_DEBUG_POLL_RX, "%s%d: esmcPollReceive. status=%x\n",
         (int) ESMC_DEV_NAME, pDev->unit, status, 4, 5, 6);

    if ((status & ESMC_INT_RCV) == 0)           /* RX done ? */
    {
        DRV_LOG (DRV_DEBUG_POLL_RX,
                "%s%d: esmcPollReceive: No data to read\n",
                (int) ESMC_DEV_NAME, pDev->unit, 3, 4, 5, 6);

        return (EAGAIN);
    }

    ESMC_SWITCH_BANK(pDev,2);
    WRITE_WORD (pDev,ESMC_PTR, ESMC_PTR_READ | ESMC_PTR_RCV | ESMC_PTR_AUTOINC);

    statusRx = READ_WORD(pDev, ESMC_DATA_1);    /* status */
    len = READ_WORD (pDev, ESMC_DATA_1);        /* byte count */

    /* check for receive errors */
    if (statusRx & ESMC_RS_ERROR_MASK)
    {
        /* increment appropriate error counter */
        if(statusRx &  ESMC_RS_ALGNERR)
            pDev->alignErr++;
            
        if(statusRx &  ESMC_RS_BADCRC)
            pDev->badCrc++;

        if(statusRx &  ESMC_RS_TOOLONG)
            pDev->tooLong++;

        if(statusRx &  ESMC_RS_TOOSHORT)
            pDev->tooShort++;

        do
            {
            val = READ_WORD(pDev, ESMC_MMU);
            }
        while(val & 0x0001);

        WRITE_WORD(pDev, ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */
    }
    else
    {
        pCur = (unsigned short *) (pMblk->mBlkHdr.mData + pDev->offset);

        /* adjust len for first 4 bytes read */
        len -=4;

        for(i = 0 ; i < (len >> 1); i++)
        {
            val = READ_WORD(pDev,ESMC_DATA_1);
            pCur[i] = SWAP_BYTES_IF_NECESSARY(val);
	}
	
        /* adjust length for control byte and odd byte */
        if (statusRx & ESMC_RS_ODDFRM)
            len -= 1;
        else
            len -= 2;

        do
            {
            val = READ_WORD(pDev, ESMC_MMU);
            }
        while (val & 0x0001);

        WRITE_WORD(pDev,ESMC_MMU, ESMC_MMU_RX_RELEASE); /* release */

        /* intialize the mBlk */
        pMblk->mBlkHdr.mFlags |= M_PKTHDR;
        pMblk->mBlkHdr.mLen    = len;
        pMblk->mBlkPktHdr.len  = len;
        pMblk->mBlkHdr.mData  += pDev->offset;

        DRV_LOG (DRV_DEBUG_POLL_RX, "%s%d: esmcPollReceive, length=%d\n",
            (int) ESMC_DEV_NAME, pDev->unit, len, 4, 5, 6);

        /* Record received packet */
        END_ERR_ADD (&pDev->endObj, MIB2_IN_UCAST, +1);
    }

    return OK;
}


/*******************************************************************************
*
* esmcEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>:<ioAddr>:<vecNum>:<intLvl>:<config>:<offset>"
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "esmc") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/

END_OBJ* esmcEndLoad
    (
    char *    initString,    /* String to be parsed by the driver. */
	 void *    pBsp
    )
    {
    unsigned int i;
    unsigned int val;
    
    ESMC_DEVICE *    pDev;
    
    DRV_LOG (DRV_DEBUG_LOAD, "esmcEndLoad: loading\n",
         (int) ESMC_DEV_NAME, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == '\0')
        {
        bcopy ((char *)ESMC_DEV_NAME, initString, ESMC_DEV_NAME_LEN);
        return (NULL);
        }
    
    /* allocate the device structure */
            
    pDev = (ESMC_DEVICE *)calloc (sizeof (ESMC_DEVICE), 1);
    if (pDev == NULL)
        goto errorExit;

    /* parse the init string, filling in the device structure */

    if (esmcParse (pDev, initString) == ERROR)
    {
    logMsg ("esmcEndLoad: bad initialization string\n",
        1, 2, 3, 4, 5, 6);
    goto errorExit;
    }

    DRV_LOG (DRV_DEBUG_LOAD,
         "esmcEndLoad: unit=%d ivec = %x ilevel=%x base=%x config=%x offset = %x\n",
         pDev->unit,
         pDev->ivec,
         pDev->ilevel,
         pDev->base,
         pDev->config, pDev->offset);

    /* initialize the function table */
    esmcFuncTable.start         = esmcStart;
    esmcFuncTable.stop          = esmcStop;
    esmcFuncTable.unload        = esmcUnload;
    esmcFuncTable.ioctl         = esmcIoctl;
    esmcFuncTable.send          = esmcSend;
    esmcFuncTable.mCastAddrAdd  = esmcMCastAdd;
    esmcFuncTable.mCastAddrDel  = esmcMCastDel;
    esmcFuncTable.mCastAddrGet  = esmcMCastGet;
    esmcFuncTable.pollSend      = esmcPollSend;
    esmcFuncTable.pollRcv       = esmcPollReceive;
    esmcFuncTable.formAddress   = endEtherAddressForm;
    esmcFuncTable.packetDataGet = endEtherPacketDataGet;
    esmcFuncTable.addrGet       = endEtherPacketAddrGet;

    /* Get ethernet address from card */

    /* this is first write to card - do it twice so chip can
       identify 68000 access mode */

    ESMC_SWITCH_BANK (pDev,1);
    taskDelay(1);  /* Need to delay after first write to chip */
    ESMC_SWITCH_BANK (pDev,1);

    for(i = 0 ; i < 6 ; i+=2)
    {
        val = READ_WORD(pDev, ESMC_ADDR_0 + i);
        pDev->enetAddr[i] = LO_BYTE(val);
        pDev->enetAddr[i+1] = HI_BYTE(val);        
    }
    /* Reset and reconfigure the device */
    esmcChipReset(pDev);

    /* initialize the END and MIB2 parts of the structure */
    if (END_OBJ_INIT (&pDev->endObj, (DEV_OBJ *)pDev, ESMC_DEV_NAME,
                      pDev->unit, &esmcFuncTable,
                      "SMSC 91CXXX Enhanced Network Driver.") == ERROR)
        goto errorExit;

    if (END_MIB_INIT (&pDev->endObj, M2_ifType_ethernet_csmacd,
                      &pDev->enetAddr[0], 6, ETHERMTU,
              ENET_SPEED_10) == ERROR)
        goto errorExit;

    /* Perform memory allocation */
    if (esmcMemInit (pDev, 128) == ERROR)
    {
        goto errorExit;
    }

    DRV_LOG (DRV_DEBUG_LOAD, "esmcLoad: done\n", 1, 2, 3, 4, 5, 6);

    /* Set the flags to indicate readiness */
    END_OBJ_READY (&pDev->endObj, IFF_NOTRAILERS | IFF_BROADCAST | IFF_MULTICAST | IFF_SIMPLEX);
    

    return (&pDev->endObj);

errorExit:
    DRV_LOG (DRV_DEBUG_LOAD, "esmcLoad: errorExit\n", 1, 2, 3, 4, 5, 6);

    if (pDev != NULL)
    {
        free ((char *)pDev);
    }
    return (NULL);
    }

void esmcShow(int unit)
    {
    ESMC_DEVICE *    pDev;
    UINT16 val;

    if ((pDev = (ESMC_DEVICE *)endFindByName(ESMC_DEV_NAME, unit)) == NULL)
        {
        printf("Error finding %s%d\n", ESMC_DEV_NAME, unit);
        return;
        }
    
    semShow(pDev->endObj.txSem,1);

    printf(" TX Stall = %d  TX Active = %d RXHandling=%d\n", pDev->esmcTxStall, pDev->esmcTxActive, pDev->flags & ESMC_RCV_HANDLING_FLAG);
    printf(" txPacketNo=%d\n", pDev->txPacketNo);

    ESMC_SWITCH_BANK (pDev,2);

    val = READ_WORD(pDev, ESMC_INT_STAT);

    printf(" INT STAT       = %04x\n", val);

    printf("  rxOverrun     = %d\n", pDev->rxOverrun); 
    printf(" lateCollisions = %d\n", pDev->lateCollisions);
    printf(" _16Collisions  = %d\n", pDev->_16Collisions);
    printf(" fifoTxUnderrun = %d\n", pDev->fifoTxUnderrun);
    printf(" sqet           = %d\n", pDev->sqet);
    printf(" lostCarrier    = %d\n", pDev->lostCarrier);
    printf(" alignErr       = %d\n", pDev->alignErr);
    printf(" badCrc         = %d\n", pDev->badCrc);
    printf(" tooLong        = %d\n", pDev->tooLong);
    printf(" tooShort       = %d\n", pDev->tooShort);

    ESMC_SWITCH_BANK (pDev,0);
    val = READ_WORD(pDev, ESMC_TCR);
    printf(" TCR            = 0x%04x\n", val);
    val = READ_WORD(pDev, ESMC_EPH);
    printf(" EPH Status     = 0x%04x\n", val);
    val = READ_WORD(pDev, ESMC_RCR);
    printf(" RCR            = 0x%04x\n", val);
    val = READ_WORD(pDev, ESMC_MIR);
    printf(" MIR            = 0x%04x\n", val);
    val = READ_WORD(pDev, ESMC_MCR);
    printf(" MCR            = 0x%04x\n", val);

    ESMC_SWITCH_BANK (pDev,1);
    val = READ_WORD(pDev, ESMC_CONFIG);
    printf(" CONFIG         = 0x%04x\n", val);
    val = READ_WORD(pDev, ESMC_CONTROL);
    printf(" CONTROL        = 0x%04x\n", val);

    ESMC_SWITCH_BANK (pDev,2);

}


void esmcHandleTxInt(ESMC_DEVICE  *pDev)
{
    int savedPointer, savedPacket;
    int val;
    BOOL restart = FALSE;

    END_TX_SEM_TAKE(&pDev->endObj, 60);

    /* save point and packet */
    savedPointer = READ_WORD(pDev, ESMC_PTR);
    savedPacket  = READ_WORD(pDev, ESMC_PNR) & 0x00ff;
         
    /* set pointer to tx  packet */
    val = READ_WORD(pDev, ESMC_FIFO);
    WRITE_WORD(pDev, ESMC_PNR, val);
    WRITE_WORD(pDev,ESMC_PTR, ESMC_PTR_AUTOINC | ESMC_PTR_READ);

     /* get status */
     val = READ_WORD(pDev, ESMC_DATA_1);

     /* check for fatal errors */
     if (val & ESMC_TS_LATCOL)
         {
         pDev->lateCollisions++;
         }
         else if(val & ESMC_TS_16COL)
         {
         pDev->_16Collisions++;
         }
         else if(val & ESMC_TS_SQET)
         {
         pDev->sqet++;
         }
         else if(val & ESMC_TS_TXUNRN)
         {
         pDev->fifoTxUnderrun++;
         }
         else if(val & ESMC_TS_LOST_CARR)
         {
         pDev->lostCarrier++;
         }

      val = intLock();

      pDev->imask |= (ESMC_INT_TX << 8);
      pDev->flags &= ~ESMC_TX_HANDLING_FLAG;

      /* acknowledge the interrupt */
      WRITE_WORD(pDev,ESMC_INT_ACK, ESMC_INT_TX | pDev->imask);

      pDev->esmcTxActive = 0;

      intUnlock(val);

      /* re-enable transmitter */
      ESMC_SWITCH_BANK (pDev, 0);
      val = READ_WORD(pDev, ESMC_TCR);
      WRITE_WORD(pDev, ESMC_TCR, val | ESMC_TCR_TXEN);
      ESMC_SWITCH_BANK (pDev,2);

      /* restore pointers */
      WRITE_WORD(pDev, ESMC_PTR, savedPointer);
      WRITE_WORD(pDev, ESMC_PNR, savedPacket);

        

      if ( pDev->esmcTxStall ) 
         {		/* TX complete, and packets waiting, inform the stack */
         restart = TRUE;
         pDev->esmcTxStall = 0;
         }      

     END_TX_SEM_GIVE(&pDev->endObj);
     if (restart) 
        {
        muxTxRestart(&pDev->endObj);            
        }
    }

