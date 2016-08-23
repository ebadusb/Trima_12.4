/* gei82543End.c - Intel 82540/82541/82543/82544/82545/82546/ MAC driver */

/*
 * Copyright (c) 2001-2005 Wind River Systems, Inc.
 *
 * The right to copy, distribute, modify or otherwise make use
 * of this software may be licensed only pursuant to the terms
 * of an applicable Wind River license agreement.
 */

/*
modification history
--------------------
02i,29mar06,wap  Add 82573 support
02h,03oct05,dlk  Set packet checksum offset for packet-level checksum
                 for all boards, not just PRO1000_546_BOARD (SPR #113209).
02g,13sep05,wap  Workaround for SPR #112130
02f,30aug05,dlk  Changes from code review:
                 - Add missing ntohs() in IPv6 offload excess padding check
         - Update documentation on checksum offload support.
         Also, add section tags, and decrease the default phyMaxDelay
         parameter to 8 seconds (rather than 25).
02e,22aug05,dlk  Reinstate checksum offload support, but IFCAP_RXCSUM,
                 IFCAP_TXCSUM now control both IPv4 and IPv6 checksum offload.
02d,11aug05,dlk  Back out IPv6 checksum offload support changes checked in for
                 archival purposes in previous mod. Keep compiler warning fixes
         and minor clean-up.
02c,10aug05,dlk  Add IPv6 checksum offload support.
02b,05oct04,jln  fix compiling warnings (spr#102310)
02a,27sep04,jln  SPR#101312
01z,31aug04,mdo  Documentation fixes for apigen
01y,24may04,rcs  added compatability with Generic MIB interface
01x,21may04,jln  support Base6 netPoolCreate; update gei82543Unload;
                 optionally explicitly flush/invalidate data cache;
                 fix SPR#96602; Used 10 byte offset.
01w,27may04,dlk  Modify gei82543Recv() not to examine receive descriptor
                 for checksum status after returning receive descriptor to
         the MAC. In gei82543RxSetup(), adjust M_BLK data pointer
         by pDrvCtrl->offset.
01v,08may04,dlk  Modified to use linkBufPool() or netTupleGet().
01u,23apr04,dlk  SPR 96741: Skip over zero-length M_BLK segments in
                 gei82543LoanTransmit(). Apparently the hardware can't handle
         too many of these. Minor optimizations.
01t,19feb04,dlk  Use CSUM_IP_HDRLEN et. al. in gei82543EndSend() and
         gei82543ExtTxContextNew() to enable source-code compatibility
         between Base 6 and Router Stack 1.3.
         Changes from code review.
01s,09feb04,wap  Remove reference to ipHdrLen
01r,02feb04,wap  Add checksum offload support
01q,03dec03,rcs  added include for taskLib.h
01q,10oct03,mcm  Fixed syntax for string spanning multiple lines
01p,15feb03,jln  remove an unnessary cacheInvalidate for RX buffers (spr 86318)
01o,19dec02,jln  fix polling mode misalignment (spr 85241)
                 add PHY's BSP hookup for 82543-copper-based adapter(spr 84779)
01n,05nov02,jln  used cacheable memory for RX data buffer (spr 83679)
                 merge wpaul's RFC2233 poll mode support (spr 84019)
01m,23may02,jln  support 82545/82546 fiber-based adapter(spr 78084)
                 clean interrupts only when packets can be processed
01l,02may02,jln  support 82540/82545/82546 based adapters spr # 76739;
                 stop device only after device has been started spr # 76511
                 change input argument from vector to unit in SYS_INT_ENABLE
                 and SYS_INT_DISABLE macros
01k,22apr02,rcs  changed gei82543MemAllFree() to use free for
                 pDrvCtrl->pTxDesCtlBase SPR# 76130
01j,22apr02,rcs  removed taskDelay() from gei82543EndPollSend() SPR# 76102
01i,14jan02,dat  Removing warnings from Diab compiler
01h,08nov01,jln  coding workaround for TBI compatibility HW bug (spr# 71363),
                 but disable it by default for testing issues.
01g,01sep01,jln  clean up documentation
01f,10aug01,jln  add support for 82544-based adapters spr# 69774; remove
                 copying method for transmitting; add support for jumbo
                 frames spr# 67477
01e,03may01,jln  fix memory leak in gei82543EndStop (spr# 67116);
                 added read operation before exit ISR to flush write buffer
                 for PCI bridge; polish TX/RX handling
01d,01may01,jln  change MACRO(s) GEI_READ_REG, GEI_READ_DESC_WORD, and
                 GEI_READ_DESC_LONG for coding convention
01c,19apr01,jln  clean up for spr#65326
01b,01apr01,jln  clean up after code review (partial spr#65326).
01a,08jan01,jln  created based on templateEnd.c and fei82557End.c END scheme
*/

/*
DESCRIPTION
The gei82543End driver supports Intel PRO1000 T/F/XF/XT/MT/MF adaptors
These adaptors use Intel 82543GC/82544GC/EI/82540/82541/82545/82546EB/ Gigabit
Ethernet controllers.The 8254x are highly integrated, high-performance LAN
controllers for 1000/100/10Mb/s transfer rates. They provide 32/64 bit 33/66Mhz
interfaces to the PCI bus with 32/64 bit addressing and are fully compliant
with PCI bus specification version 2.2. The 82544, 82545 and 82546 also
provide PCI-X interface.

The 8254x controllers implement all IEEE 802.3 receive and transmit
MAC functions. They provide a Ten-Bit Interface (TBI) as specified in the IEEE
802.3z standard for 1000Mb/s full-duplex operation with 1.25 GHz Ethernet
transceivers (SERDES), as well as a GMII interface as specified in
IEEE 802.3ab for 10/100/1000 BASE-T transceivers, and also an MII interface as
specified in IEEE 802.3u for 10/100 BASE-T transceivers.

The 8254x controllers offer auto-negotiation capability for TBI and GMII/MII
modes and also support IEEE 802.3x compliant flow control. This driver
supports the checksum offload features of the 8254x family as follows:

\ts
--- Chip --- | ---- TX offload capabilities ---- | ---- RX offload capabilities
-------------|-----------------------------------|--------------------------
82543        | TCP/IPv4, UDP/IPv4, IPv4          | TCP/IPv4, UDP/IPv4
-------------|-----------------------------------|--------------------------
82544        | TCP/IPv4, UDP/IPv4, IPv4          | TCP/IPv4, UDP/IPv4, IPv4
-------------|-----------------------------------|--------------------------
8254[5601]   | TCP/IPv4, UDP/IPv4, IPv4          | TCP/IPv4, UDP/IPv4, IPv4
             | TCP/IPv6, UDP/IPv6                | TCP/IPv6, UDP/IPv6
\te

For the 82540/82541/82545/82546, the driver supports the transport
checksum over IPv6 on receive via the packet checksum feature. (The RX IPv6
checksum offload apparently does not function on these chips as documented.)
To avoid doing additional work massaging the packet checksum value when the
received packets might not be destined for this target, receive
checksum offload of TCP or UDP over IPv6 is attempted only when the
IPv6 header follows immediately after a 14-byte ethernet header, there
are no IPv6 extension headers, and there is no excess padding after the
end of the IPv6 payload.

Although these devices also support other features such as jumbo
frames, and provide flash support up to 512KB and EEPROM support, this
driver does NOT support these features.

The 8254x establishes a shared memory communication system with the
CPU, which is divided into two parts: the control/status registers and the
receive/transmit descriptors/buffers. The control/status registers
are on the 8254x chips and are only accessible with PCI or PCI-X
memory cycles, whereas the other structures reside on the host. The buffer
size can be programmed between 256 bytes to 16k bytes. This driver uses the
receive buffer size of 2048 bytes for an MTU of 1500.

The Intel PRO/1000 F/XF/MF adapters only implement the TBI mode of the
8254x controller with built-in SERDESs in the adaptors.

The Intel PRO/1000 T adapters based on 82543GC implement the GMII mode with
a Gigabit Ethernet Transceiver (PHY) of MARVELL's Alaska 88E1000/88E1000S.
However, the PRO/1000 XT/MT adapters based on 82540/82544/82545/82546 use
the built-in PHY in controllers.

The driver on the current release supports both GMII mode for Intel
PRO1000T/XT/MT adapters and TBI mode for Intel PRO1000 F/XF/MF adapters.
However, it requires the target-specific initialization code --
sys543BoardInit () -- to distinguish these kinds of adapters by PCI device
IDs.

EXTERNAL INTERFACE
The driver provides the standard external interface, gei82543EndLoad(), which
takes a string of colon separated parameters. The parameter string is parsed
using strtok_r() and each parameter in converted from a string representation
to a binary.

The format of the parameter string is:

 "<memBase>:<memSize>:<nRxDes>:<nTxDes>:<flags>:<offset>:<mtu>"

TARGET-SPECIFIC PARAMETERS

\is
\i <memBase>
This parameter is passed to the driver via gei82543EndLoad().

The 8254x is a DMA-type device and typically shares access to some region of
memory with the CPU. This driver is designed for systems that directly share
memory between the CPU and the 8254x.

This parameter can be used to specify an explicit memory region for use
by the 8254x chip.  This should be done on targets that restrict the 8254x
to a particular memory region.  The constant `NONE' can be used to indicate
that there are such memory, in which case the driver will allocate cache safe
memory for its use using cacheDmaAlloc().

\i <memSize>
The memory size parameter specifies the size of the pre-allocated memory
region. The driver checks the size of the provided memory region is adequate
with respect to the given number of transmit Descriptor and Receive
Descriptor.

\i <nRxDes>
This parameter specifies the number of transmit descriptors to be
allocated. If this number is 0, a default value of 24 will be used.

\i <nTxDes>
This parameter specifies the number of receive descriptors to be
allocated. If this parameter is 0, a default of 24 is used.

\i <flags>
This parameter is provided for user to customize this device driver for their
application.

GEI_END_SET_TIMER (0x01): a timer will be started to constantly free back the
loaned transmit mBlks.

GEI_END_SET_RX_PRIORITY (0x02): packet transfer (receive) from device to host
memory will have higher priority than the packet transfer (transmit) from host
memory to device in the PCI bus. For end-station application, it is suggested
to set this priority in favor of receive operation to avoid receive overrun.
However, for routing applications, it is not necessary to use this priority.
This option is only for 82543-based adapters.

GEI_END_FREE_RESOURCE_DELAY (0x04): when transmitting larger packets, the
driver will hold mblks(s) from the network stack and return them after the
driver has completed transmitting the packet, and either the timer has expired
or there are no more available descriptors. If this option is not used, the
driver will free mblk(s) when ever the packet transmission is done. This option
will place greater demands on the network pool and should only be used in
systems which have sufficient memory to allocate a large network pool. It is
not advised for the memory-limited target systems.

GEI_END_TBI_COMPATIBILITY (0x200): if this driver enables the workaround for
TBI compatibility HW bugs (#define INCLUDE_TBI_COMPATIBLE), user can set
this bit to enable a software workaround for the well-known TBI compatibility
HW bug in the Intel PRO1000 T adapter. This bug is only occurred in the
copper-and-82543-based adapter, and the link partner has advertised only
1000Base-T capability.

GEI_END_USER_MEM_FOR_DESC_ONLY (0x400): User can provide memory for this driver
through the shMemBase and shMemSize in the load string. By default, this memory
is used for TX/RX descriptors and RX buffer. However, if this flag is set, that
memory will be only used for TX/RX descriptors, and the driver will malloc
other memory for RX buffers and maintain cache coherency for RX buffers. It is
user's responsibility to maintain the cache coherence for memory they provided.

GEI_END_FORCE_FLUSH_CACHE: Set this flag to force flushing the data cache for
transmit data buffers even when bus snooping is enabled on the target.

GEI_END_FORCE_INVALIDATE_CACHE: Set this flag to force invalidating the data
cache for receive data buffers even when bus snooping is enabled on the target.

\i <offset>
This parameter is provided for the architectures which need DWORD (4 byte)
alignment of the IP header. In that case, the value of OFFSET should be two,
otherwise, the default value is zero.

\ie


EXTERNAL SUPPORT REQUIREMENTS

This driver requires one external support function:
\cs
STATUS sys82543BoardInit (int unit, ADAPTOR_INFO *pBoard)
\ce
This routine performs some target-specific initialization such as EEPROM
validation and obtaining ETHERNET address and initialization control words
 (ICWs) from EEPROM. The routine also initializes the adaptor-specific data
structure. Some target-specific functions used later in driver operation
are hooked up to that structure. It's strongly recommended that users provide
a delay function with higher timing resolution. This delay function will be
used in the PHY's read/write operations if GMII is used. The driver will use
taskDelay() by default if user can NOT provide any delay function, and
this will probably result in very slow PHY initialization process. The user
should also specify the PHY's type of MII or GMII. This routine returns OK,
or ERROR if it fails.




SYSTEM RESOURCE USAGE
The driver uses cacheDmaMalloc() to allocate memory to share with the 8254xGC.
The size of this area is affected by the configuration parameters specified
in the gei82543EndLoad() call.

Either the shared memory region must be non-cacheable, or else the hardware
must implement bus snooping. The driver cannot maintain cache coherency for
the device because fields within the command structures are asynchronously
modified by both the driver and the device, and these fields may share the
same cache line.

SYSTEM TUNING HINTS

Significant performance gains may be had by tuning the system and network stack.
This may be especially necessary for achieving gigabit transfer rates.

Increasing the network stack's pools are strongly recommended. This driver
borrows mblks from the network stack to accelerate packet transmitting.
Theoretically, the number borrowed clusters could be the same as the number of
the device's transmit descriptors. However, if the network stack has fewer
available clusters than available transmit descriptors then this will result
in reduced throughput. Therefore, increasing the network stack's number of
clusters relative to the number of transmit descriptors will increase bandwidth.
Of course this technique will eventually reach a point of diminishing return.
There are actually several sizes of clusters available in the network pool.
Increasing any or all of these cluster sizes will result in some increase in
performance. However, increasing the 2048-byte cluster size will likely have
the greatest impact since this size will hold an entire MTU and header.

Increasing the number of receive descriptors and clusters may also have
positive impact.

Increasing the buffer size of sockets can also be beneficial. This can
significantly improve performance for a target system under higher transfer
rates. However, it should be noted that large amounts of unread buffers idling
in sockets reduces the resources available to the rest of the stack. This can,
in fact, have a negative impact on bandwidth.  One method to reduce this effect
is to carefully adjust application tasks' priorities and possibly increase
number of receive clusters.

Callback functions defined in the sysGei82543End.c can be used to dynamically
and/or statically change the internal timer registers such as ITR, RADV, and
RDTR to reduce RX interrupt rate.

\INTERNAL
This library contains two conditional compilation switches: DRV_DEBUG and
INCLUDE_GEI82543_DEBUG_ROUTINE. If defined, debug routines will be included.
And output message can be selected by gei82543GCDebug variable.

SEE ALSO: muxLib, endLib
\tb RS-82543GC GIGABIT ETHERNET CONTROLLER NETWORKING DEVELOPER'S MANUAL
*/

/* includes */

#include "vxWorks.h"
#include "stdlib.h"
#include "cacheLib.h"
#include "intLib.h"
#include "end.h"                /* common END structures. */
#include "endLib.h"
#include "lstLib.h"             /* needed to maintain protocol list. */
#include "wdLib.h"
#include "iv.h"
#include "semLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "errno.h"
#include "errnoLib.h"
#include "memLib.h"
#include "iosLib.h"
#undef    ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"      /* multicast stuff. */

#include "sys/times.h"
#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "net/route.h"
#include "sys/socket.h"
#include "sys/ioctl.h"

#include "string.h"
#include "taskLib.h"

#ifdef WR_IPV6
#include "adv_net.h"
#endif /* WR_IPV6 */

#ifndef _WRS_FASTTEXT
#define _WRS_FASTTEXT
#endif

#define  GEI_USE_LEGACY_BUF      1    /* legacy (vxWork5.5) buf management  */
#define  GEI_USE_LEGACY_LINKBUF  2    /* link buffer management */
#define  GEI_USE_BASE6_BUF       3    /* vxWorks6.0 buffer management */

/* choose one of three buffer management option above */

#ifdef ATTR_AI_SH_ISR
#define GEI_BUF_TYPE             GEI_USE_BASE6_BUF
#define ENDMIB2
#else
#define GEI_BUF_TYPE             GEI_USE_LEGACY_BUF
#endif


#ifdef CSUM_IP
#define GEI_CSUM_OFFLOAD
#endif

#define GEI_RX_CSUM(pDrvCtrl) ((pDrvCtrl)->hwCaps.csum_flags_rx)

#include "gei82543End.h"

/* IMPORT */

IMPORT STATUS sys82543BoardInit (int, ADAPTOR_INFO*);
IMPORT int    endMultiLstCnt (END_OBJ* pEnd);
IMPORT int netTaskId;

/* defines */

#undef INCLUDE_TBI_COMPATIBLE

/* all 8254x chip registers are 32 bit long*/

#if (_BYTE_ORDER == _BIG_ENDIAN)
#define GEI_READ_REG(offset, result)     \
   do {                         \
      UINT32 temp;              \
      temp   = ((*(volatile UINT32*)(pDrvCtrl->devRegBase + (offset)))); \
      result = LONGSWAP(temp);      /* swap the data */  \
   } while ((0))

#define GEI_WRITE_REG(offset, value) \
   ((*(volatile UINT32*)(pDrvCtrl->devRegBase + (offset))) = \
       (UINT32)LONGSWAP(value))

#define GEI_WRITE_DESC_WORD(pDesc, offset, value)      \
   (*(UINT16*)((UINT32)pDesc + offset) =    \
       (MSB(value) | LSB(value) << 8) & 0xffff)

#define GEI_WRITE_DESC_LONG(pDesc, offset, value)      \
   (*(UINT32*)((UINT32)pDesc + offset) =    \
       (UINT32)LONGSWAP(value))

#define GEI_READ_DESC_WORD(pDesc, offset, result)             \
   do {                                                  \
      UINT16 temp;                                       \
      temp   = *(UINT16*)((UINT32)pDesc + offset);        \
      result = (MSB(temp) | (LSB(temp) << 8)) & 0xffff;  \
   } while ((0))

#define GEI_READ_DESC_LONG(pDesc, offset, result)              \
   do {                         \
      UINT32 temp;              \
      temp   = *(UINT32*)((UINT32)pDesc + offset);   \
      result = LONGSWAP(temp);      /* swap the data */  \
   } while ((0))

#else /* (_BYTE_ORDER == _BIG_ENDIAN) */

#define GEI_READ_REG(offset, result)     \
   result = (*(volatile UINT32*)(pDrvCtrl->devRegBase + (offset)))

#define GEI_WRITE_REG(offset, value) \
   ((*(volatile UINT32*)(pDrvCtrl->devRegBase + (offset))) = \
       (UINT32)(value))

#define GEI_WRITE_DESC_WORD(pDesc, offset, value)      \
   (*(UINT16*)((UINT32)pDesc + offset) = (UINT16)(value & 0xffff))

#define GEI_WRITE_DESC_LONG(pDesc, offset, value)      \
   (*(UINT32*)((UINT32)pDesc + offset) = (UINT32)value)

#define GEI_READ_DESC_WORD(pDesc, offset, result)              \
   result = ((UINT16)(*(UINT16*)((UINT32)pDesc + offset)) & 0xffff)

#define GEI_READ_DESC_LONG(pDesc, offset, result)              \
   result = ((UINT32)( *(UINT32*)((UINT32)pDesc + offset)))

#endif /* (_BYTE_ORDER == _BIG_ENDIAN) */

#define GEI_WRITE_DESC_BYTE(pDesc, offset, value)      \
   (*(UINT8*)((UINT32)pDesc + offset) = (UINT8)(value & 0xff))

#define GEI_READ_DESC_BYTE(pDesc, offset)              \
   ((UINT8)( *(UINT8*)((UINT32)pDesc + offset)) & 0xff)

#define GEI_GET_RX_DESC_ADDR(offset)          \
   (pDrvCtrl->pRxDescBase + ((offset) * RXDESC_SIZE))

#define GEI_GET_TX_DESC_ADDR(offset)          \
   (pDrvCtrl->pTxDescBase + ((offset) * TXDESC_SIZE))

#define GEI_GET_TX_DESC_CTL_ADDR(offset)      \
   (pDrvCtrl->pTxDesCtlBase + (offset));

#define GEI_GET_TX_DESC_TAIL_UPDATE(tmp, num)     \
   (tmp) = (pDrvCtrl->txDescTail + (num)) % (pDrvCtrl->txDescNum)

#define GEI_GET_RX_DESC_TAIL_UPDATE(tmp, num)        \
   (tmp) = (pDrvCtrl->rxDescTail + (num)) % (pDrvCtrl->rxDescNum)

#define GEI_ROUND_UP_MULTIPLE(x, y)          \
   ( ( ( x + ( y - 1 ) ) / y ) * y )

/* bus/CPU address translation macros */

#define GEI_VIRT_TO_BUS(virtAddr)                                           \
   (GEI_PHYS_TO_BUS (((UINT32)GEI_VIRT_TO_PHYS (virtAddr))))

#define GEI_PHYS_TO_VIRT(physAddr)                                          \
   GEI_END_CACHE_PHYS_TO_VIRT ((char*)(physAddr))

#define GEI_VIRT_TO_PHYS(virtAddr)                                          \
   GEI_END_CACHE_VIRT_TO_PHYS ((char*)(virtAddr))

#define GEI_PHYS_TO_BUS(physAddr)                                          \
   PHYS_TO_BUS_ADDR (pDrvCtrl->unit, (physAddr))

#define GEI_BUS_TO_PHYS(busAddr)                                           \
   BUS_TO_PHYS_ADDR (pDrvCtrl->unit, (busAddr))

/* cache macros */

#define GEI_END_USR_CACHE_FLUSH(address, len) \
   do \
   { \
      if (*pDrvCtrl->pFlushRtn != NULL)   \
         (*pDrvCtrl->pFlushRtn)(DATA_CACHE, (address), (len));  \
   } while ((0))

#define GEI_END_DRV_CACHE_INVALIDATE(address, len) \
   do \
   { \
      if (*pDrvCtrl->pInvalRtn != NULL)   \
         (*pDrvCtrl->pInvalRtn)(DATA_CACHE, (address), (len));  \
   } while ((0))

#define GEI_END_CACHE_PHYS_TO_VIRT(address) \
   CACHE_DRV_PHYS_TO_VIRT (&cacheDmaFuncs, (address))

#define GEI_END_CACHE_VIRT_TO_PHYS(address) \
   CACHE_DRV_VIRT_TO_PHYS (&cacheDmaFuncs, (address))

/* misc. */

#define GEI_DESC_ALIGN_BYTE         (128)

#if   (GEI_BUF_TYPE == GEI_USE_LEGACY_LINKBUF) || \
   (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)

#define GEI_CL_OVERHEAD 0
#define GEI_TUPLE_GET(pNetPool) (mBlkGet ((pNetPool), M_DONTWAIT, MT_DATA))

#else  /* ! GEI_USE_LEGACY_LINKBUF */

#define GEI_CL_OVERHEAD (sizeof (CL_POOL_ID))

/* Here we use a bufSize of 1, since we know there is only one cluster size */
#define GEI_TUPLE_GET(pNetPool) \
   (netTupleGet ((pNetPool), 1, M_DONTWAIT, MT_DATA, TRUE))

#endif /* ! GEI_USE_LEGACY_LINKBUF */

/*
 * Default macro definitions for BSP interface.
 * These macros can be redefined in a wrapper file, to generate
 * a new module with an optimized interface.
 */
/* macro to connect interrupt handler to vector */

#ifndef GEI_SYS_INT_CONNECT
#define GEI_SYS_INT_CONNECT(pDrvCtrl, rtn, arg, pResult)          \
   {                                                      \
      *pResult = OK;                                         \
      if (pDrvCtrl->adaptor.intConnect)                                  \
         *pResult = (pDrvCtrl->adaptor.intConnect)((VOIDFUNCPTR*)      \
                                                   INUM_TO_IVEC (pDrvCtrl->adaptor.vector), \
                                                   rtn, (int)arg);                          \
   }
#endif

/* macro to disconnect interrupt handler from vector */

#ifndef GEI_SYS_INT_DISCONNECT
#define GEI_SYS_INT_DISCONNECT(pDrvCtrl, rtn, arg, pResult)                    \
   {                                                                   \
      *pResult = OK;                                                      \
      if (pDrvCtrl->adaptor.intDisConnect)                                    \
         *pResult =  (pDrvCtrl->adaptor.intDisConnect)((VOIDFUNCPTR*)       \
                                                       INUM_TO_IVEC (pDrvCtrl->adaptor.vector),              \
                                                       rtn, (int)arg);                                          \
   }
#endif

/* macro to enable the appropriate interrupt level */

#ifndef GEI_SYS_INT_ENABLE
#define GEI_SYS_INT_ENABLE(pDrvCtrl)                            \
   {                                                       \
      pDrvCtrl->adaptor.intEnable(pDrvCtrl->unit);  \
   }
#endif
#ifndef GEI_SYS_INT_DISABLE
#define GEI_SYS_INT_DISABLE(pDrvCtrl)                           \
   {                                                       \
      pDrvCtrl->adaptor.intDisable(pDrvCtrl->unit); \
   }
#endif

/*
 * a shortcut for getting the hardware address
 * from the MIB II stuff.
 */

#define GEI_END_HADDR(pEnd)     \
   ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define GEI_END_HADDR_LEN(pEnd) \
   ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)


/*
 * The offset at which the packet checksum starts.
 * Specified this way to handle and IPv6 header immediately
 * following an RFC 894 Ethernet header.
 */
#define GEI_RXCSUM_PKT_CSUM_OFF (14 + 40)


/* device structure */

typedef struct end_device
{
   END_OBJ          end;                 /* the class we inherit from. */
   int              unit;                /* unit number */
   UINT32           flags;               /* flags for device configuration */
   UINT32           usrFlags;            /* flags for user customization */
   CACHE_FUNCS*     pCacheFuncs;         /* cache function pointers */
   ADAPTOR_INFO     adaptor;             /* adaptor information */
   UINT32           devRegBase;          /* virtual base address for registers */
   P_TX_DESCTL      pTxDesCtlBase;       /* pointer to the TX management array base */
   char*            pTxDescBase;         /* pointer to the TX descriptor base */
   char*            pRxDescBase;         /* pointer to the RX descriptor base */
   char*            pRxBufBase;          /* pointer to the RX buffer base */
   volatile int     txDescTail;          /* index to the TX tail */
   volatile int     rxDescTail;          /* index to the RX tail */
   int              txDescNum;           /* num of TX descriptors */
   int              rxDescNum;           /* num of RX descriptors */
   UINT32           rxBufSize;           /* RX buffer size */
   UINT32           txReqDescNum;        /* request number for TX descriptor */
   int              mtu;                 /* maximum transfer unit */
   volatile int     rxtxHandling;        /* indicator for RX handling */
   UINT32           txStallCount;        /* count of END_ERR_BLOCK returns */
   volatile BOOL    txStall;             /* indicator for transmit stall */
   char*            pRxBufMem;           /* allocated Rx Buffer memory pointer*/
   UINT32           rxPktSz;             /* MRU + offset */
   UINT32           clSz;                /* cluster size for RX buffer */
   int              txIntDelay;          /* delay time for TX interrupt */
   int              rxIntDelay;          /* delay time for RX interrupt */
   int              maxRxNumPerInt;      /* maximum RX packets processed per Int */
   BOOL             dmaPriority;         /* indicator for TX/RX DMA prefer */
   UINT32           timerInterval;       /* interval for timer interrupt */
   BOOL             timerCheckTxStall;   /* indicator for txStall checking */
   UINT32           multiCastFilterType; /* indicator of multicast table type */
   int              flowCtrl;            /* flow control setting */
   STATUS           linkInitStatus;      /* the status of first link */
   volatile UINT32  linkStatus;          /* indicator for link status */
   BOOL             linkMethod;          /* indicator for link approaches */
   WDOG_ID          timerId;             /* timer ID */
   UINT32           txConfigureWord;     /* copy of TX configuration word register */
   UINT32           devCtrlRegVal;       /* control register value */
   UINT32           speed;               /* device speed */
   UINT32           duplex;              /* device duplex mode */
   UINT32           offset;              /* offset for IP header */
   UINT32           cableType;           /* cable type (Fiber or Copper) */
   BOOL             memAllocFlag;        /* indicator that the shared memory */
                                         /*  was allocated by driver */
   char*            pMemBase;            /* memory base for TX/RX area */
   int              memSize;             /* total memory size for RX/TX area */
   char*            pMclkArea;           /* address of Mclk */
   char*            pTxPollBufAdr;       /* address for TX buffer in polling mode */
   PHY_INFO*        pPhyInfo;            /* pointer to phyInfo structure */
   UINT32           phyInitFlags;        /* Initial PHY flags for phyInfo */
   volatile UINT32  txDescLastCheck;     /* index of the last checked TX desc */
   volatile UINT32  txDescFreeNum;       /* available/usable TX desc number */
   M_BLK_ID*        pRxMblkArray;        /* pointer to Rx Buf M_BLK pointer array */
   BOOL             txResoFreeQuick;     /* flag to free loaned mBlk quickly */
   volatile BOOL    attach;              /* indicator for drive attach */
   volatile BOOL    devStartFlag;        /* indicator for device start */
   UINT32           rxtxHandlerNum;      /* num of rxtx handler calling */
   UINT32           rxIntCount;          /* receive interrupt count */
   UINT32           txIntCount;          /* transmit interrupt count */
   UINT32           rxORunIntCount;      /* num of RX overrun interrupt count */
   UINT32           rxPacketNum;         /* statistic RX packet number */
   UINT32           rxPacketDrvErr ;     /* num of RX packet drop due to no resc */
#ifdef INCLUDE_TBI_COMPATIBLE
   BOOL             tbiCompatibility; /* TBI compatibility for HW bug */
#endif
   STA_REG          staRegs;     /* statistic register structure */
#ifdef GEI_CSUM_OFFLOAD
   END_CAPABILITIES hwCaps;      /* interface hardware offload capabilities */
#endif
   UINT32           lastCsumCtx;
   UINT32           lastIpHdrLen;
   /*END_IFDRVCONF endStatsConf;
   END_IFCOUNTERS endStatsCounters; */
   int         bufManageType;
#if (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)
   NETBUF_CFG* pNetBufCfg;
#endif
   FUNCPTR*    pFlushRtn;
   FUNCPTR*    pInvalRtn;
#ifndef MII_PHY_ISO_UNAVAIL
   BOOL        phyIsolate;
#endif
} GEI_END_DEVICE;

/* DEBUG MACROS */

/*
 * Define the following MACROs for debugging
 * #define  DRV_DEBUG
 * #define  DEBUG
 */

#undef  INCLUDE_GEI82543_DEBUG_ROUTINE
#undef  GEI82543_NO_LOCAL

#ifdef DEBUG

int geiEndDebug = 0;

#define LOGMSG(x, a, b, c, d, e, f)       \
   do  {                           \
      if (geiEndDebug)            \
      {                       \
         logMsg (x, a, b, c, d, e, f); \
      }                       \
   } while (0)
#else
#define LOGMSG(x, a, b, c, d, e, f)
#endif /* DEBUG */

#ifdef GEI82543_NO_LOCAL
#undef     LOCAL
#define    LOCAL
#endif

#ifdef  DRV_DEBUG
#define DRV_DEBUG_OFF           0x0000
#define DRV_DEBUG_RX            0x0001
#define DRV_DEBUG_TX            0x0002
#define DRV_DEBUG_INT           0x0004
#define DRV_DEBUG_POLL          (DRV_DEBUG_POLL_RX | DRV_DEBUG_POLL_TX)
#define DRV_DEBUG_POLL_RX       0x0008
#define DRV_DEBUG_POLL_TX       0x0010
#define DRV_DEBUG_LOAD          0x0020
#define DRV_DEBUG_IOCTL         0x0040
#define DRV_DEBUG_TIMER         0x20000
/* My custom levels */

#define DRV_DEBUG_RXD            0x0080
#define DRV_DEBUG_TXD            0x0100
#define DRV_DEBUG_INTD              0x0200
int gei82543GCDebug = ( DRV_DEBUG_TXD | DRV_DEBUG_RXD | DRV_DEBUG_INTD );

/* This one is specifically for use in the ISR */
#define DRV_LOGMSG(FLG, X0, X1, X2, X3, X4, X5, X6) \
   do  {                       \
      if (gei82543GCDebug& FLG)          \
         logMsg(X0, X1, X2, X3, X4, X5, X6);     \
   } while (0)

#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)    \
   do  {                       \
      if (gei82543GCDebug& FLG)                  \
         logMsg (X0, X1, X2, X3, X4, X5, X6);    \
   } while (0)

#define DRV_PRINT(FLG, X)                            \
   do  {                       \
      if (gei82543GCDebug& FLG)          \
         printf X;                   \
   } while (0)

#else /* DRV_DEBUG */
#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_LOGMSG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW, X)
#endif /* DRV_DEBUG */


#ifdef INCLUDE_GEI82543_DEBUG_ROUTINE
GEI_END_DEVICE* gei82543Unit                   (int);
void            gei82543LedOff                         (int);
void            gei82543LedOn                          (int);
void            gei82543StatusShow                     (int);
UINT32          gei82543RegGet                       (int, UINT32);
void            gei82543RegSet                         (int, UINT32, UINT32);
#ifdef INCLUDE_TBI_COMPATIBLE
void gei82543TbiCompWr                      (int, int);
#endif /* INCLUDE_TBI_COMPATIBLE */
#endif /* INCLUDE_GEI82543_DEBUG_ROUTINE */

/* LOCAL */

LOCAL FUNCPTR gei8254xForceFlushRtn;
LOCAL FUNCPTR gei8254xForceInvalRtn;

/* forward functions */

LOCAL void   gei82543EndTimerHandler     (GEI_END_DEVICE*);
LOCAL void   gei82543MemAllFree          (GEI_END_DEVICE*);
LOCAL STATUS gei82543linkStatusCheck     (GEI_END_DEVICE*);
LOCAL void   gei82543TxRxEnable          (GEI_END_DEVICE*);
LOCAL void   gei82543TxRxDisable         (GEI_END_DEVICE*);
LOCAL void   gei82543Delay               (GEI_END_DEVICE*, UINT32);
LOCAL void   gei82543RxSetup             (GEI_END_DEVICE*);
LOCAL void   gei82543TxSetup             (GEI_END_DEVICE*);
LOCAL STATUS gei82543HwInit              (GEI_END_DEVICE*);
LOCAL void   gei82543RxDesUpdate         (GEI_END_DEVICE*);
LOCAL void   gei82543DuplexGet           (GEI_END_DEVICE*);
LOCAL void   gei82543SpeedGet            (GEI_END_DEVICE*);
LOCAL void   gei82543HwStatusDump        (GEI_END_DEVICE*);
LOCAL STATUS gei82543linkInit            (GEI_END_DEVICE*);
LOCAL STATUS gei82543linkTBISetup        (GEI_END_DEVICE*, BOOL);
LOCAL STATUS gei82543TBIlinkForce        (GEI_END_DEVICE*, BOOL, BOOL);
LOCAL STATUS gei82543TBIHwAutoNegotiate  (GEI_END_DEVICE*, BOOL, BOOL);
LOCAL void   gei82543EtherRxAdrSet       (GEI_END_DEVICE*, UINT8 adr[], int);
LOCAL void   gei82543AllRxAdrClean       (GEI_END_DEVICE*);
LOCAL void   gei82543McastAdrClean       (GEI_END_DEVICE*);
LOCAL void   gei82543AllMtaAdrClean      (GEI_END_DEVICE*);
LOCAL void   gei82543AllVlanClean        (GEI_END_DEVICE*);
LOCAL UINT32 gei82543DisableChipInt      (GEI_END_DEVICE*);
LOCAL void   gei82543EnableChipInt       (GEI_END_DEVICE*);
LOCAL void   gei82543Reset               (GEI_END_DEVICE*);
LOCAL void   gei82543TxMblkFree          (GEI_END_DEVICE*, int);
LOCAL void   gei82543TxMblkWaitClean     (GEI_END_DEVICE*);
LOCAL STATUS gei82543EndMCastAdd         (GEI_END_DEVICE*, char*);
LOCAL STATUS gei82543EndMCastDel         (GEI_END_DEVICE*, char*);
LOCAL STATUS gei82543EndMCastGet         (GEI_END_DEVICE*, MULTI_TABLE*);
_WRS_FASTTEXT
LOCAL void gei82543EndInt              (GEI_END_DEVICE*);
_WRS_FASTTEXT
LOCAL void gei82543RxTxIntHandle       (GEI_END_DEVICE*);
_WRS_FASTTEXT
LOCAL STATUS gei82543Recv                (GEI_END_DEVICE*, char*, UINT8);
LOCAL void   gei82543EndConfigure        (GEI_END_DEVICE*);
LOCAL void   gei82543FlowCtrlRegsSet     (GEI_END_DEVICE*);
LOCAL STATUS gei82543PhyWrite            (GEI_END_DEVICE*, UINT8, UINT8, UINT16);
LOCAL STATUS gei82543PhyRead             (GEI_END_DEVICE*, UINT8, UINT8,
                                          UINT16*);
LOCAL STATUS gei82544PhyWrite            (GEI_END_DEVICE*, UINT8, UINT8, UINT16);
LOCAL STATUS gei82544PhyRead             (GEI_END_DEVICE*, UINT8, UINT8,
                                          UINT16*);
_WRS_FASTTEXT
LOCAL void gei82543LoanTransmit        (GEI_END_DEVICE*, M_BLK_ID);
LOCAL void gei82543TxResoFree          (GEI_END_DEVICE*);
_WRS_FASTTEXT
LOCAL int gei82543TxDesCleanGet       (GEI_END_DEVICE*);
LOCAL int gei82543TxDesCleanForce     (GEI_END_DEVICE*);
_WRS_FASTTEXT
LOCAL void   gei82543TxStallCheck        (GEI_END_DEVICE*);
LOCAL void   gei82543GMIIphyConfig       (GEI_END_DEVICE*);
LOCAL void   gei82543GMIIphyReConfig     (GEI_END_DEVICE*);
LOCAL STATUS gei82543linkGMIISetup       (GEI_END_DEVICE*);
LOCAL STATUS gei82543linkGMIIPreInit     (GEI_END_DEVICE*);
#ifdef GEI_CSUM_OFFLOAD
_WRS_FASTTEXT
LOCAL void gei82543ExtTxContextNew     (GEI_END_DEVICE*, M_BLK*);
LOCAL void geiHwCapsTranslate      (GEI_END_DEVICE*);
#endif
LOCAL void gei8254xNowUnload           (GEI_END_DEVICE*);

LOCAL STATUS gei82543EndStatsDump        (GEI_END_DEVICE*);

/* END specific interfaces. */
/* This is the only externally visible interface. */

END_OBJ*     gei82543EndLoad             (char* initString);
LOCAL STATUS gei82543EndStart            (GEI_END_DEVICE* pDrvCtrl);
LOCAL STATUS gei82543EndStop             (GEI_END_DEVICE* pDrvCtrl);
LOCAL int    gei82543EndIoctl            (GEI_END_DEVICE* pDrvCtrl, int cmd,
                                          caddr_t data);
LOCAL STATUS gei82543EndUnload           (GEI_END_DEVICE* pDrvCtrl);
_WRS_FASTTEXT
LOCAL STATUS gei82543EndSend             (GEI_END_DEVICE* pDrvCtrl,
                                          M_BLK_ID pBuf);
LOCAL STATUS gei82543EndMCastAdd         (GEI_END_DEVICE* pDrvCtrl,
                                          char* pAddress);
LOCAL STATUS gei82543EndMCastDel         (GEI_END_DEVICE* pDrvCtrl,
                                          char* pAddress);
LOCAL STATUS gei82543EndMCastGet         (GEI_END_DEVICE* pDrvCtrl,
                                          MULTI_TABLE* pTable);
LOCAL STATUS gei82543EndPollStart        (GEI_END_DEVICE* pDrvCtrl);
LOCAL STATUS gei82543EndPollStop         (GEI_END_DEVICE* pDrvCtrl);
LOCAL STATUS gei82543EndPollSend         (GEI_END_DEVICE* pDrvCtrl,
                                          M_BLK_ID pBuf);
LOCAL STATUS gei82543EndPollRcv          (GEI_END_DEVICE* pDrvCtrl,
                                          M_BLK_ID pBuf);
LOCAL void   gei82543AddrFilterSet       (GEI_END_DEVICE* pDrvCtrl);
LOCAL STATUS gei82543EndParse            (GEI_END_DEVICE*, char*);
LOCAL STATUS gei82543EndMemInit          (GEI_END_DEVICE*);

LOCAL void gei8254xRxTupleFree         (GEI_END_DEVICE*);
#if (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)
LOCAL STATUS gei8254xEndBufMemInit2      (GEI_END_DEVICE*, UINT32);
#endif
LOCAL STATUS gei8254xEndBufMemInit       (GEI_END_DEVICE*, UINT32);

/*
 * Declare our function table.  This is LOCAL across all driver
 * instances.
 */
LOCAL NET_FUNCS gei82543EndFuncTable =
{
   (FUNCPTR)gei82543EndStart,       /* Function to start the device. */
   (FUNCPTR)gei82543EndStop,        /* Function to stop the device. */
   (FUNCPTR)gei82543EndUnload,      /* Unloading function for the driver. */
   (FUNCPTR)gei82543EndIoctl,       /* Ioctl function for the driver. */
   (FUNCPTR)gei82543EndSend,        /* Send function for the driver. */
   (FUNCPTR)gei82543EndMCastAdd,    /* Multicast add function for the */
                                    /* driver. */
   (FUNCPTR)gei82543EndMCastDel,    /* Multicast delete function for */
                                    /* the driver. */
   (FUNCPTR)gei82543EndMCastGet,    /* Multicast retrieve function for */
                                    /* the driver. */
   (FUNCPTR)gei82543EndPollSend,    /* Polling send function */
   (FUNCPTR)gei82543EndPollRcv,     /* Polling receive function */
   endEtherAddressForm,             /* put address info into a NET_BUFFER */
   endEtherPacketDataGet,           /* get pointer to data in NET_BUFFER */
   endEtherPacketAddrGet            /* Get packet addresses. */
};

/*************************************************************************
*
* gei82543EndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device specific parameters are passed in the initString.
*
* The string contains the target specific parameters like this:
* "unitnum:shmem_addr:shmem_size:rxDescNum:txDescNum:usrFlags:offset:mtu"
*
* RETURNS: an END object pointer, NULL if error, or zero
*/

END_OBJ* gei82543EndLoad (char* initString        /* String to be parsed by the driver. */
                          )
{
   GEI_END_DEVICE* pDrvCtrl;   /* pointer to device structure */

   DRV_LOG (DRV_DEBUG_LOAD, "Loading gei82543End Driver...\n",
            1, 2, 3, 4, 5, 6);
   /* sanity check */

   if (initString == NULL)
      return NULL;

   if (initString[0] == 0)
   {
      bcopy ((char*)DEVICE_NAME, (void*)initString, DEVICE_NAME_LENGTH);
      return 0;
   }

   /* allocate the device structure */

   pDrvCtrl = (GEI_END_DEVICE*)calloc (sizeof (GEI_END_DEVICE), 1);

   if (pDrvCtrl == NULL)
      goto errorExit;

   /* clean up driver structure */

   memset((void*)pDrvCtrl, 0, sizeof (GEI_END_DEVICE));

   /* set the buffer management type */

   pDrvCtrl->bufManageType = GEI_BUF_TYPE;

   /* parse the init string, filling in the device structure */

   if (gei82543EndParse (pDrvCtrl, initString) == ERROR)
      goto errorExit;

   /* force 2 byte offset */

   if (pDrvCtrl->offset == 0)
      pDrvCtrl->offset = 0x2;

   /* zero adaptor structure */

   memset ((void*)&pDrvCtrl->adaptor, 0, sizeof(ADAPTOR_INFO));

   /* call BSP routine to get PCI information*/

   if (sys82543BoardInit (pDrvCtrl->unit, &pDrvCtrl->adaptor) != OK)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Error in getting board info\n",
               1, 2, 3, 4, 5, 6);
      goto errorExit;
   }

#ifdef GEI_CSUM_OFFLOAD
   /* Initialize checksum capabilities */
   pDrvCtrl->hwCaps.cap_available = IFCAP_RXCSUM | IFCAP_TXCSUM;

   /*
    * Eventually: IFCAP_TCPSEG for 82544 or later.
    */

   /* Initially enable all capabilities */

   pDrvCtrl->hwCaps.cap_enabled = pDrvCtrl->hwCaps.cap_available;

   geiHwCapsTranslate (pDrvCtrl);
#endif

   /* set up the base register */

   pDrvCtrl->devRegBase = (UINT32)(pDrvCtrl->adaptor.regBaseLow);

   /* set up device structure based on user's flags */
   if (pDrvCtrl->usrFlags & GEI_END_JUMBO_FRAME_SUPPORT)
   {
      if (pDrvCtrl->mtu <= 0)
         pDrvCtrl->mtu = GEI_DEFAULT_JUMBO_MTU_SIZE;

      if (pDrvCtrl->mtu < ETHERMTU)
         pDrvCtrl->mtu = ETHERMTU;
      else if (pDrvCtrl->mtu > GEI_MAX_JUMBO_MTU_SIZE)
         pDrvCtrl->mtu = GEI_MAX_JUMBO_MTU_SIZE;
   }
   else   /* normal frame */
      pDrvCtrl->mtu = ETHERMTU;

   /* increase transmit storage in FIFO for jumbo frames */

   if (pDrvCtrl->mtu > ETHERMTU)
   {
      GEI_WRITE_REG(INTEL_82543GC_PBA, 0x20);   /* 24KB for TX buffer */
   }

   /* perform memory allocation for descriptors */

   if (gei82543EndMemInit (pDrvCtrl) == ERROR)
      goto errorExit;

   /* set up device structure based on user's flags */

   if (pDrvCtrl->usrFlags & GEI_END_SET_TIMER)
   {
      pDrvCtrl->timerId = wdCreate ();
      if (pDrvCtrl->timerId == NULL)
         DRV_LOG (DRV_DEBUG_LOAD, ("create timer fails\n"),
                  1, 2, 3, 4, 5, 6);
   }

   if (pDrvCtrl->usrFlags & GEI_END_SET_RX_PRIORITY)
      pDrvCtrl->dmaPriority = DMA_RX_PRIORITY;
   else
      pDrvCtrl->dmaPriority = DMA_FAIR_RX_TX;

   if (pDrvCtrl->usrFlags & GEI_END_FREE_RESOURCE_DELAY)
   {
      pDrvCtrl->txIntDelay      = TXINT_DELAY_MORE;
      pDrvCtrl->txResoFreeQuick = FALSE;
   }
   else
   {
      pDrvCtrl->txIntDelay      = TXINT_DELAY_LESS;
      pDrvCtrl->txResoFreeQuick = TRUE;
   }

#ifdef INCLUDE_TBI_COMPATIBLE
   pDrvCtrl->tbiCompatibility = FALSE;
#endif /* INCLUDE_TBI_COMPATIBLE */

   /* stop/reset the chip before configuration */

   gei82543Reset (pDrvCtrl);

   /* disable all chip interrupt */

   gei82543DisableChipInt (pDrvCtrl);

   /* turn off system interrupts */

   GEI_SYS_INT_DISABLE(pDrvCtrl);

   /* set the default value for device */

   pDrvCtrl->rxIntDelay     = DEFAULT_RXINT_DELAY;
   pDrvCtrl->maxRxNumPerInt = DEFAULT_RXRES_PROCESS_FACTOR *
                              pDrvCtrl->rxDescNum;
   pDrvCtrl->timerInterval  = DEFAULT_TIMER_INTERVAL;
   pDrvCtrl->flowCtrl       = DEFAULT_FLOW_CONTRL;
   pDrvCtrl->duplex         = DEFAULT_DUPLEX_MODE;

   /* Misc. setting */

   pDrvCtrl->flags               = 0;
   pDrvCtrl->linkStatus          = LINK_STATUS_UNKNOWN;
   pDrvCtrl->linkMethod          = GEI82543_HW_AUTO;
   pDrvCtrl->txConfigureWord     = (TXCW_ANE_BIT | TXCW_FD_BIT);
   pDrvCtrl->multiCastFilterType = DEFAULT_MULTI_FILTER_TYPE;
   pDrvCtrl->attach              = FALSE;
   pDrvCtrl->devStartFlag        = FALSE;

   /* initialize the END and MIB2 parts of the structure */
   /*
    * The M2 element must come from m2Lib.h
    * This setting is for a DIX type ethernet device.
    */


   if (END_OBJ_INIT (&pDrvCtrl->end, (DEV_OBJ*)NULL, DEVICE_NAME,
                     pDrvCtrl->unit, &gei82543EndFuncTable,
                     "gei82543End Driver.") == ERROR)
      goto errorExit;


#ifdef ENDMIB2
   if (endM2Init(&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                 (u_char*)&pDrvCtrl->adaptor.enetAddr[0], 6,
                 pDrvCtrl->mtu, END_SPEED_1000M,
                 IFF_NOTRAILERS | IFF_MULTICAST | IFF_BROADCAST)
       == ERROR)
   {
      logMsg ("%s%d - MIB-II initializations failed\n",
              (int)DEVICE_NAME, pDrvCtrl->unit, 0, 0, 0, 0);
      goto errorExit;
   }
#else
   END_OBJ_READY (&pDrvCtrl->end, (IFF_NOTRAILERS |
                                   IFF_BROADCAST | IFF_MULTICAST | END_MIB_2233));

   if (END_MIB_INIT (&pDrvCtrl->end, M2_ifType_ethernet_csmacd,
                     &pDrvCtrl->adaptor.enetAddr[0], 6, ETHERMTU, 100000000) == ERROR)
   {
      goto errorExit;
   }

   pDrvCtrl->end.pMib2Tbl = m2IfAlloc(M2_ifType_ethernet_csmacd,
                                      (UINT8*)&pDrvCtrl->adaptor.enetAddr[0], 6,
                                      pDrvCtrl->mtu, END_SPEED_1000M,
                                      "gei", pDrvCtrl->unit);

   if (pDrvCtrl->end.pMib2Tbl == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, ("%s%d - MIB-II initializations failed\n"),
               "gei", pDrvCtrl->unit, 3, 4, 5, 6);
      goto errorExit;
   }
#endif

   /* bzero ((char *)&pDrvCtrl->endStatsCounters, sizeof(END_IFCOUNTERS)); */

   /* pDrvCtrl->endStatsConf.ifPollInterval = sysClkRateGet();
   pDrvCtrl->endStatsConf.ifEndObj = &pDrvCtrl->end;
   pDrvCtrl->endStatsConf.ifWatchdog = NULL;
   pDrvCtrl->endStatsConf.ifValidCounters = (END_IFINUCASTPKTS_VALID      |
                                             END_IFINMULTICASTPKTS_VALID  |
                                             END_IFINBROADCASTPKTS_VALID  |
                                             END_IFINOCTETS_VALID         |
                                             END_IFOUTOCTETS_VALID        |
                                             END_IFOUTUCASTPKTS_VALID     |
                                             END_IFOUTMULTICASTPKTS_VALID |
                                             END_IFOUTBROADCASTPKTS_VALID);
   */

   /* disable RX/TX operations now, will be re-enable in Start function */

   gei82543TxRxDisable (pDrvCtrl);

   pDrvCtrl->attach = TRUE;

   DRV_LOG (DRV_DEBUG_LOAD, ("loading gei82543End...OK\n"), 1, 2, 3, 4, 5, 6);

   return (&pDrvCtrl->end);

errorExit:

   /* free all allocated memory */

   gei82543MemAllFree (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, ("Loading gei82543End...Error\n"),
            1, 2, 3, 4, 5, 6);
   return NULL;
}

/*************************************************************************
*
* gei82534EndParse - parse the init string
*
* Parse the input string.  Fill in values in the driver control structure.
*
* RETURNS: OK or ERROR for invalid arguments.
*/

LOCAL STATUS gei82543EndParse (GEI_END_DEVICE*    pDrvCtrl, /* device pointer */
                               char*          initString    /* information string */
                               )
{
   char* tok;
   char* pHolder = NULL;

   DRV_LOG (DRV_DEBUG_LOAD, ("gei82543EndParse...\n"), 1, 2, 3, 4, 5, 6);

   /* parse the initString */

   tok = strtok_r (initString, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->unit = atoi (tok);

   /* address of shared memory */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->pMemBase = (char*)strtoul (tok, NULL, 16);

   /* size of shared memory */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->memSize = strtoul (tok, NULL, 16);

   /* number of rx descriptors */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->rxDescNum = strtoul (tok, NULL, 16);

   /* number of tx descriptors */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->txDescNum = strtoul (tok, NULL, 16);

   /* get the usrFlags */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok == NULL)
      return ERROR;
   pDrvCtrl->usrFlags = strtoul (tok, NULL, 16);

   /* get the offset value */

   tok = strtok_r (NULL, ":", &pHolder);
   if (tok != NULL)
      pDrvCtrl->offset = atoi (tok);

   DRV_LOG (DRV_DEBUG_LOAD,
            ("gei82543EndParse: unit=%d pMemBase=0x%x memSize=0x%x\n"
             "    rxDescNums=%d txDescNum=%d, usrFlags=0x%x\n"),
            pDrvCtrl->unit,
            (int)pDrvCtrl->pMemBase,
            (int)pDrvCtrl->memSize,
            pDrvCtrl->rxDescNum,
            pDrvCtrl->txDescNum,
            pDrvCtrl->usrFlags
            );

   /* check Jumbo Frame support */

   if (pDrvCtrl->usrFlags & GEI_END_JUMBO_FRAME_SUPPORT)
   {
      /* get mtu */

      tok = strtok_r (NULL, ":", &pHolder);
      if (tok != NULL)
         pDrvCtrl->mtu = atoi (tok);

      DRV_LOG (DRV_DEBUG_LOAD, ("mtu = %d\n"), pDrvCtrl->mtu,
               2, 3, 4, 5, 6);
   }

   return OK;
}

/*************************************************************************
*
* gei82543MemAllFree - free all memory allocated by driver
*
* This routine returns all allocated memory by this driver to OS
*
* RETURN: N/A
*/

LOCAL void gei82543MemAllFree (GEI_END_DEVICE* pDrvCtrl    /* device to be initialized */
                               )
{
   if (pDrvCtrl == NULL)
      return;

   /* release TxDesCtl */

   if (pDrvCtrl->pTxDesCtlBase != NULL)
   {
      free (pDrvCtrl->pTxDesCtlBase);
      pDrvCtrl->pTxDesCtlBase = NULL;
   }

   if (pDrvCtrl->pRxMblkArray != NULL)
   {
      free (pDrvCtrl->pRxMblkArray);
      pDrvCtrl->pRxMblkArray = NULL;
   }

   if (pDrvCtrl->pTxPollBufAdr != NULL)
   {
      cacheDmaFree (pDrvCtrl->pTxPollBufAdr);
      pDrvCtrl->pTxPollBufAdr = NULL;
   }

   /* release TX/RX descriptors and RX buffer */

   if (pDrvCtrl->pMemBase != NULL && (pDrvCtrl->memAllocFlag == TRUE))
   {
      cacheDmaFree (pDrvCtrl->pMemBase);
      pDrvCtrl->pMemBase = NULL;
   }

   /* free RX buffer memory */

   if (pDrvCtrl->pRxBufMem != NULL)
   {
      free (pDrvCtrl->pRxBufMem);
      pDrvCtrl->pRxBufMem = NULL;
   }

   /* release RX mBlk pool */

   if (pDrvCtrl->pMclkArea != NULL)
   {
      cfree (pDrvCtrl->pMclkArea);
      pDrvCtrl->pMclkArea = NULL;
   }

   /* Free MIB-II entries */

#ifdef ENDMIB2
   endM2Free(&pDrvCtrl->end);
#else
   m2IfFree(pDrvCtrl->end.pMib2Tbl);
   pDrvCtrl->end.pMib2Tbl = NULL;
#endif

   /* release netPool structure */

#if (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)
   if (pDrvCtrl->bufManageType == GEI_USE_BASE6_BUF)
   {
      if (pDrvCtrl->end.pNetPool != NULL)
         if (netPoolRelease (pDrvCtrl->end.pNetPool, 0) != OK)
         {
            LOGMSG("netPoolRelease fails\n", 0, 0, 0, 0, 0, 0);
         }
   }
   else
   {
      LOGMSG("netPoolDelete is not supported\n", 0, 0, 0, 0, 0, 0);
   }
#endif

   /* free now */

   cfree ((char*)pDrvCtrl);

   return;
}

/*************************************************************************
*
* gei82543EndMemInit - allocate and initialize memory for driver
*
* This routine allocates and initializes memory for descriptors and
* corresponding buffers, and sets up the receive data pool for receiving
* packets.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndMemInit (GEI_END_DEVICE* pDrvCtrl    /* device to be initialized */
                                 )
{
   UINT32 size;                 /* required memory size */
   UINT32 bufSz;                /* real cluster size */
   STATUS status;               /* return type */

   DRV_LOG (DRV_DEBUG_LOAD, ("gei82543EndMemInit...\n"), 1, 2, 3, 4, 5, 6);

   /* set the default TX/RX descriptor Number */

   if (pDrvCtrl->txDescNum == 0)
      pDrvCtrl->txDescNum = DEFAULT_NUM_TXDES;

   if (pDrvCtrl->rxDescNum == 0)
      pDrvCtrl->rxDescNum = DEFAULT_NUM_RXDES;

   /* round up to multiple of 8, hardware requirement */

   pDrvCtrl->txDescNum = GEI_ROUND_UP_MULTIPLE(pDrvCtrl->txDescNum,
                                               INTEL_82543GC_MULTIPLE_DES);

   pDrvCtrl->rxDescNum = GEI_ROUND_UP_MULTIPLE((UINT32)pDrvCtrl->rxDescNum,
                                               (UINT32)INTEL_82543GC_MULTIPLE_DES);

   /*
    * Calculate reasonable receive buffer size. We strip CRC, now,
    * but leave space for it anyway.
    */

   size = pDrvCtrl->mtu + GEI_DEFAULT_ETHERHEADER + ETHER_CRC_LENGTH;

   /* assume MRU is the same as MTU */

   pDrvCtrl->rxPktSz = size + pDrvCtrl->offset;

   if (size > GEI_MAX_FRAME_SIZE)
      return ERROR;

   if (pDrvCtrl->usrFlags & GEI_END_JUMBO_FRAME_SUPPORT)
   {
      for (bufSz = 2048; bufSz <= 16384; bufSz = bufSz << 1)
      {
         if (size <= bufSz)
         {
            pDrvCtrl->rxBufSize = bufSz;
            break;
         }
      }
   }
   else  /* normal frame */
   {
      pDrvCtrl->rxBufSize = 2048;
      bufSz               = size;
   }

   /* add cluster ID and offset */

   bufSz += (pDrvCtrl->offset + GEI_CL_OVERHEAD);

   /*
    * Note, GEI_DESC_ALIGN_BYTE is the alignment requirement for
    * RX and TX descriptors (128 bytes, or 8 descriptors). It
    * is not a hardware requirement to round up the cluster buffer
    * size, but we do so anyway. Note that this also satisfies
    * the NETBUF_ALIGN cluster alignment/size requirement for
    * linkBufPool.
    */

   bufSz = ROUND_UP (bufSz, GEI_DESC_ALIGN_BYTE);

   /* Check or allocate memory for descriptors */

   if ((int)(pDrvCtrl->pMemBase) == NONE)
   {
      if (!CACHE_DMA_IS_WRITE_COHERENT () ||
          !CACHE_DMA_IS_READ_COHERENT ())
      {
         DRV_LOG (DRV_DEBUG_LOAD,
                  ("gei82543EndMemInit: shared descriptor"
                   " memory not cache coherent\n"), 1, 2, 3, 4, 5, 6);
         return ERROR;
      }

      size = pDrvCtrl->txDescNum * TXDESC_SIZE +     /* for TX descriptor */
             pDrvCtrl->rxDescNum * RXDESC_SIZE +     /* for RX descriptor */
             512;                                    /* alignment */

      /* alloc memory in driver */

      pDrvCtrl->pMemBase = cacheDmaMalloc (size);

      if (pDrvCtrl->pMemBase == NULL)        /* no memory available */
      {
         DRV_LOG (DRV_DEBUG_LOAD, ("gei82543EndMemInit: could not obtain "
                                   "memory\n"), 1, 2, 3, 4, 5, 6);
         return (ERROR);
      }

      pDrvCtrl->memSize      = size;

      pDrvCtrl->memAllocFlag = TRUE;
   }
   else
   {
      /*
       * if user has provided a shared memory, we assume
       * it is a cache safe area
       */

      if (pDrvCtrl->usrFlags & GEI_END_USER_MEM_FOR_DESC_ONLY)
      {
         size = pDrvCtrl->txDescNum * TXDESC_SIZE +    /* for TX descriptor */
                pDrvCtrl->rxDescNum * RXDESC_SIZE +    /* for RX descriptor */
                512;
      }
      else
      {
         size = pDrvCtrl->txDescNum * TXDESC_SIZE +    /* for TX descriptor */
                pDrvCtrl->rxDescNum * RXDESC_SIZE +    /* for RX descriptor */
                pDrvCtrl->rxDescNum * bufSz *          /* for RX buffer */
                (DEFAULT_LOAN_RXBUF_FACTOR + 1) +      /* for RX loan buffer */
                1024;                                  /* for alignment */
      }

      if (pDrvCtrl->memSize < size)
      {
         DRV_LOG (DRV_DEBUG_LOAD, ("GEI82543EndMemInit: not enough "
                                   "memory\n"), 1, 2, 3, 4, 5, 6);
         return ERROR;
      }

      pDrvCtrl->memAllocFlag = FALSE;
   }

   /* zero the pre-provided or allocated memory */

   memset((void*)pDrvCtrl->pMemBase, 0, size);

   /* set the TX descriptor base address, align to 128 byte */

   pDrvCtrl->pTxDescBase = (char*)(((UINT32)(pDrvCtrl->pMemBase) +
                                    (GEI_DESC_ALIGN_BYTE - 1)) &
                                   ~(GEI_DESC_ALIGN_BYTE - 1));

   /* set the RX descriptor base Address */

   pDrvCtrl->pRxDescBase = (char*)GEI_GET_TX_DESC_ADDR(pDrvCtrl->txDescNum);

   /* get memory for RX buffer virtual address array */

   size                   = pDrvCtrl->rxDescNum * sizeof(M_BLK_ID);

   pDrvCtrl->pRxMblkArray = (M_BLK_ID*)memalign(GEI_DESC_ALIGN_BYTE, size);

   if (pDrvCtrl->pRxMblkArray == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543Load: not enough memory\n",
               0, 0, 0, 0, 0, 0);
      return ERROR;
   }

   memset (pDrvCtrl->pRxMblkArray, 0, size);

   /* allocate memory for txDescriptor manager array */

   size                    = pDrvCtrl->txDescNum * sizeof(TX_DESCTL);

   pDrvCtrl->pTxDesCtlBase = (P_TX_DESCTL)memalign(GEI_DESC_ALIGN_BYTE, size);

   if ((pDrvCtrl->pTxDesCtlBase) == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: TxDesCtl memory "
               "unavailable\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   /* clean up txDesCtl memory */

   memset((void*)pDrvCtrl->pTxDesCtlBase, 0, size);

   /* get a buffer for TX polling mode */

   pDrvCtrl->pTxPollBufAdr = cacheDmaMalloc (bufSz);

   if ((pDrvCtrl->pTxPollBufAdr) == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: Tx Polling memory "
               "unavailable\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   /* configure the RX buffer management */

   if (pDrvCtrl->bufManageType == GEI_USE_BASE6_BUF)
   {
      /* temp solution, netPoolCreate don't support pass-in buffer address */

      if ((pDrvCtrl->memAllocFlag == FALSE) &&
          !(pDrvCtrl->usrFlags & GEI_END_USER_MEM_FOR_DESC_ONLY))
         pDrvCtrl->bufManageType = GEI_USE_LEGACY_LINKBUF;
   }

#if (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)
   if (pDrvCtrl->bufManageType == GEI_USE_BASE6_BUF)
   {
      bufSz  = ROUND_UP ((pDrvCtrl->rxPktSz), GEI_DESC_ALIGN_BYTE);
      status = gei8254xEndBufMemInit2 (pDrvCtrl, bufSz);
   }
   else
#endif
   status = gei8254xEndBufMemInit (pDrvCtrl, bufSz);

   return status;
}

/*************************************************************************
* gei82543EndBufMemInit - allocate and init the RX buffer memory
*
* This function configures and initializes the RX buffer memory by using
* the legacy network buffer management scheme (netPoolInit).
*
* RETURN: OK/ERROR
*/

LOCAL STATUS gei8254xEndBufMemInit (GEI_END_DEVICE* pDrvCtrl,
                                    UINT32 bufSz
                                    )
{
   M_CL_CONFIG geiMclBlkConfig;  /* Mblk */
   CL_DESC     geiClDesc;        /* Cluster descriptor */
   UINT32      size;
   UINT32      tmpBase;
   POOL_FUNC*  pFuncTbl;        /* Net pool function table */

   if (pDrvCtrl->memAllocFlag ||
       pDrvCtrl->usrFlags & GEI_END_USER_MEM_FOR_DESC_ONLY)
   {
      size = pDrvCtrl->rxDescNum * bufSz *          /* for RX buffer */
             (DEFAULT_LOAN_RXBUF_FACTOR + 1) +      /* for RX loan buffer */
             1024;

      pDrvCtrl->pRxBufMem = (char*)memalign(GEI_DESC_ALIGN_BYTE, size);
      if (pDrvCtrl->pRxBufMem == NULL)
      {
         DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: RxBuffer memory "
                  "unavailable\n", 1, 2, 3, 4, 5, 6);
         return ERROR;
      }

      memset((void*)pDrvCtrl->pRxBufMem, 0, size);

      pDrvCtrl->pCacheFuncs = &cacheUserFuncs;

      tmpBase               = (UINT32)(pDrvCtrl->pRxBufMem);
   }
   else
   {
      pDrvCtrl->pCacheFuncs = &cacheNullFuncs;

      tmpBase               = (UINT32)GEI_GET_RX_DESC_ADDR(pDrvCtrl->rxDescNum);
   }

   /* set up the flushRtn/invalRtn */

   if (pDrvCtrl->usrFlags & GEI_END_FORCE_FLUSH_CACHE)
   {
      gei8254xForceFlushRtn = (FUNCPTR)cacheFlush;
      pDrvCtrl->pFlushRtn   = &gei8254xForceFlushRtn;
   }
   else
      pDrvCtrl->pFlushRtn = &cacheUserFuncs.flushRtn;

   if (pDrvCtrl->usrFlags & GEI_END_FORCE_INVALIDATE_CACHE)
   {
      gei8254xForceInvalRtn = (FUNCPTR)cacheInvalidate;
      pDrvCtrl->pInvalRtn   = &gei8254xForceInvalRtn;
   }
   else
      pDrvCtrl->pInvalRtn = &pDrvCtrl->pCacheFuncs->invalidateRtn;

   tmpBase = ((tmpBase + GEI_CL_OVERHEAD + GEI_DESC_ALIGN_BYTE - 1) &
              ~(GEI_DESC_ALIGN_BYTE - 1));

   pDrvCtrl->pRxBufBase = (char*)tmpBase - GEI_CL_OVERHEAD;

   /*
    * set up RX mBlk pool
    * This is how we set up and END netPool using netBufLib(1).
    * This code is pretty generic.
    */

   if ((pDrvCtrl->end.pNetPool = malloc (sizeof(NET_POOL))) == NULL)
      return ERROR;

   bzero ((char*)&geiMclBlkConfig, sizeof(geiMclBlkConfig));
   bzero ((char*)&geiClDesc, sizeof(geiClDesc));

   geiClDesc.clNum = pDrvCtrl->rxDescNum *
                     (DEFAULT_LOAN_RXBUF_FACTOR + 1);

   geiMclBlkConfig.mBlkNum  = geiClDesc.clNum;

   geiClDesc.clSize         = bufSz - GEI_CL_OVERHEAD;

   pDrvCtrl->clSz           = geiClDesc.clSize;

   geiMclBlkConfig.clBlkNum = geiClDesc.clNum;

   /* calculate the total memory for all the M-Blks and CL-Blks. */
#if (GEI_BUF_TYPE == GEI_USE_LEGACY_LINKBUF)
   if (pDrvCtrl->bufManageType == GEI_USE_LEGACY_LINKBUF)
   {
      geiMclBlkConfig.memSize = geiClDesc.clNum * ML_SIZE + NETBUF_ALIGN;
      pFuncTbl                = _pLinkPoolFuncTbl;
   }
   else
#endif
   {
      geiMclBlkConfig.memSize =
         ((geiMclBlkConfig.mBlkNum * (M_BLK_SZ + sizeof (NET_POOL_ID))) +
          (geiMclBlkConfig.clBlkNum * CL_BLK_SZ));

      pFuncTbl = NULL;
   }

   if ((geiMclBlkConfig.memArea = (char*)
                                  malloc (geiMclBlkConfig.memSize)) == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: MclBlkConfig memory "
               "unavailable\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   pDrvCtrl->pMclkArea = geiMclBlkConfig.memArea;

   /* This was how much cluster memory space we guaranteed above. */

   size = (pDrvCtrl->rxDescNum * bufSz *     /* for RX buffer */
           (DEFAULT_LOAN_RXBUF_FACTOR + 1) + /* for RX loan buffer */
           1024);

   geiClDesc.memSize = size;

   geiClDesc.memArea = (char*)(pDrvCtrl->pRxBufBase);   /* already aligned */

   GEI_END_DRV_CACHE_INVALIDATE (geiClDesc.memArea, geiClDesc.memSize);

   /* initialize the memory pool. */

   if (netPoolInit(pDrvCtrl->end.pNetPool, &geiMclBlkConfig,
                   &geiClDesc, 1, pFuncTbl) == ERROR)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: Could not "
               "init buffering\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   CACHE_PIPE_FLUSH ();

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit...OK\n", 1, 2, 3, 4, 5, 6);

   return OK;
}

#if (GEI_BUF_TYPE == GEI_USE_BASE6_BUF)
/***********************************************************************
* gei8254xEndBufMemInit2 - allocate memory using Base6 buffer management
*
* This function allocates and initializes memory for descriptors and RX
* buffers. It employs Base6 buffer management scheme.
*
* RETURNL: OK or ERROR
*/

LOCAL STATUS gei8254xEndBufMemInit2 (GEI_END_DEVICE* pDrvCtrl,    /* device to be initialized */
                                     UINT32 bufSz
                                     )
{
   /* allocate a NETBUF_CFG */

   if ((pDrvCtrl->pNetBufCfg = (NETBUF_CFG*)memalign (sizeof(long),
                                                      (sizeof(NETBUF_CFG) +
                                                       END_NAME_MAX))) == NULL)
      return (ERROR);

   /* set up the flushRtn/invalRtn */

   if (pDrvCtrl->usrFlags & GEI_END_FORCE_FLUSH_CACHE)
   {
      gei8254xForceFlushRtn = (FUNCPTR)cacheFlush;
      pDrvCtrl->pFlushRtn   = &gei8254xForceFlushRtn;
   }
   else
      pDrvCtrl->pFlushRtn = &cacheUserFuncs.flushRtn;

   if (pDrvCtrl->usrFlags & GEI_END_FORCE_INVALIDATE_CACHE)
   {
      gei8254xForceInvalRtn = (FUNCPTR)cacheInvalidate;
      pDrvCtrl->pInvalRtn   = &gei8254xForceInvalRtn;
   }
   else
      pDrvCtrl->pInvalRtn = &cacheUserFuncs.invalidateRtn;

   bzero((void*)pDrvCtrl->pNetBufCfg, sizeof(NETBUF_CFG));

   /* Initialize the pName field */

   pDrvCtrl->pNetBufCfg->pName = (char*)((int)pDrvCtrl->pNetBufCfg +
                                         sizeof(NETBUF_CFG));

   sprintf(pDrvCtrl->pNetBufCfg->pName, "%s%d", "gei", pDrvCtrl->unit);

   /* Set the attributes to be Cached, Cache aligned, sharable, & ISR safe */

   pDrvCtrl->pNetBufCfg->attributes = ATTR_AI_SH_ISR;

   /* Set pDomain to kernel, use NULL.*/

   pDrvCtrl->pNetBufCfg->pDomain = NULL;

   /* Set ratio of mBlks to clusters */
   pDrvCtrl->pNetBufCfg->ctrlNumber = pDrvCtrl->rxDescNum *
                                      (DEFAULT_LOAN_RXBUF_FACTOR + 1);

   /* Set memory partition of mBlks to kernel, use NULL */

   pDrvCtrl->pNetBufCfg->ctrlPartId = NULL;

   /* Set extra memory size to zero for now */

   pDrvCtrl->pNetBufCfg->bMemExtraSize = 0;

   /* Set cluster's memory partition to kernel, use NULL */

   pDrvCtrl->pNetBufCfg->bMemPartId = NULL;

   /* Allocate memory for network cluster descriptor */

   pDrvCtrl->pNetBufCfg->pClDescTbl =
      (NETBUF_CL_DESC*)memalign (sizeof(long),
                                 sizeof(NETBUF_CL_DESC));

   if (pDrvCtrl->pNetBufCfg->pClDescTbl == NULL)
      return ERROR;

   /* Initialize the Cluster Descriptor */

   pDrvCtrl->pNetBufCfg->pClDescTbl->clSize = bufSz;
   pDrvCtrl->pNetBufCfg->pClDescTbl->clNum  = pDrvCtrl->rxDescNum *
                                              (DEFAULT_LOAN_RXBUF_FACTOR + 1);

   pDrvCtrl->pNetBufCfg->clDescTblNumEnt = 1;

   /* Call netPoolCreate() with the Link Pool Function Table */

   if ((pDrvCtrl->end.pNetPool =
           netPoolCreate ((NETBUF_CFG*)pDrvCtrl->pNetBufCfg,
                          _pLinkPoolFuncTbl)) == NULL)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndBuffMemInit2: Error in "
               "netPoolCreate \n", 1, 2, 3, 4, 5, 6);

      return (ERROR);
   }

   CACHE_PIPE_FLUSH();

   /* free resources */

   free (pDrvCtrl->pNetBufCfg->pClDescTbl);
   free (pDrvCtrl->pNetBufCfg);

   return (OK);
}
#endif

/*************************************************************************
*
* gei82543EndStart - setup and start the device
*
* This routine connects interrupts, initializes receiver and transmitter,
* and enable the device for running
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS gei82543EndStart (GEI_END_DEVICE* pDrvCtrl    /* device ID */
                               )
{
   STATUS result;            /* results of start device */

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart...\n", 1, 2, 3, 4, 5, 6);

   if (pDrvCtrl->attach != TRUE)
      return ERROR;

   /* disable all chip interrupts */

   gei82543DisableChipInt (pDrvCtrl);

   /* turn off system interrupts */

   GEI_SYS_INT_DISABLE(pDrvCtrl);

   /* initialize the hardware and set up link */

   if (gei82543HwInit (pDrvCtrl) != OK)
   {
      DRV_LOG (DRV_DEBUG_LOAD, ("gei82543EndStart: link setup fails\n"),
               1, 2, 3, 4, 5, 6);
      LOGMSG("link fails\n", 1, 2, 3, 4, 5, 6);
   }

   /* disable TX/RX now */

   gei82543TxRxDisable (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: set up TX structure\n",
            1, 2, 3, 4, 5, 6);
   /* initialize SW/HW structures for TX */

   gei82543TxSetup (pDrvCtrl);

   /* initialize SW/HW structure for RX */

   gei82543RxSetup (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: configure device\n",
            1, 2, 3, 4, 5, 6);
   /* configure devices */

   pDrvCtrl->flags = DEFAULT_DRV_FLAGS;

   END_FLAGS_CLR (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);

   gei82543EndConfigure (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: Connect interrupt\n",
            1, 2, 3, 4, 5, 6);
   /* connect interrupt handler */

   GEI_WRITE_REG(INTEL_82546EB_ITR, 16000);
   GEI_SYS_INT_CONNECT(pDrvCtrl, gei82543EndInt, (int)pDrvCtrl, &result);

   /* enable adaptor interrupt */

   sysIntEnablePIC (10);


   if (result == ERROR)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: connect interrupt "
               "ERROR !\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   /* clean up all statistic registers */

   gei82543HwStatusDump (pDrvCtrl);

   /* clean all pending interrupts */

   gei82543DisableChipInt (pDrvCtrl);

   /* turn on system interrupt */

   GEI_SYS_INT_ENABLE(pDrvCtrl);

   /* enable interrupt in chip level */

   gei82543EnableChipInt (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: Interrupt enable\n",
            1, 2, 3, 4, 5, 6);

   /* set the END flags, make driver ready to start */
   END_FLAGS_SET (&pDrvCtrl->end, (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS |
                                   IFF_BROADCAST | IFF_MULTICAST));


   gei82543EndConfigure (pDrvCtrl);

   /* wait for chip to settle down */

   taskDelay (sysClkRateGet () / 20);

   /* check link status again before really start */

   if (gei82543linkStatusCheck (pDrvCtrl) != OK)
   {
      LOGMSG("gei82543EndStart:Link is down \n", 1, 2, 3, 4, 5, 6);
   }

   /* set the internal timer */

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      ADAPTOR_INFO* pBoard = &pDrvCtrl->adaptor;  /* pointer to board specific struct */

      if (pBoard->sysGeiInitTimerSetup != NULL)
         if (pBoard->sysGeiInitTimerSetup (&pDrvCtrl->adaptor))
         {
            if (!(pBoard->devTimerUpdate.rdtrVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82543GC_RDTR, pBoard->devTimerUpdate.rdtrVal);

            if (!(pBoard->devTimerUpdate.radvVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82546EB_RADV, pBoard->devTimerUpdate.radvVal);

            if (!(pBoard->devTimerUpdate.itrVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82546EB_ITR, pBoard->devTimerUpdate.itrVal);

            pDrvCtrl->timerInterval = (pBoard->devTimerUpdate.watchDogIntVal <= 0) ?
                                      DEFAULT_TIMER_INTERVAL : (pBoard->devTimerUpdate.watchDogIntVal);
         }
   }

   /* start the timer if configured */

   if (pDrvCtrl->usrFlags & GEI_END_SET_TIMER)
   {
      if (pDrvCtrl->timerId)
         wdStart(pDrvCtrl->timerId, pDrvCtrl->timerInterval * sysClkRateGet(),
                 (FUNCPTR)gei82543EndTimerHandler, (int)pDrvCtrl);
   }

   /*
    * The values set here for lastCsumCtx and lastIpHdrLen will never occur
    * for real packets; they are set to ensure a context transmit descriptor
    * is created for the first IP packet transmitted with checksum offload.
    */
   pDrvCtrl->lastCsumCtx  = 0xffff;

   pDrvCtrl->lastIpHdrLen = (UINT32)(~0);

   /* set the device start flag */

   pDrvCtrl->devStartFlag = TRUE;

   /* enable TX and RX operation */

   gei82543TxRxEnable (pDrvCtrl);

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart...Done\n", 1, 2, 3, 4, 5, 6);

   return (OK);
}

/*************************************************************************
*
* gei82543EndSend - driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it. This is done by a higher layer.
*
* RETURNS: OK or ERROR.
*
* ERRNO: EINVAL
*/

_WRS_FASTTEXT
LOCAL STATUS gei82543EndSend (GEI_END_DEVICE* pDrvCtrl, /* device ptr */
                              M_BLK_ID pMblk            /* data to send */
                              )
{
   M_BLK_ID pMblkDup;       /* temporary pMblk */
   int      mBlkNum;        /* num of mBlk of a packet */
#ifdef GEI_CSUM_OFFLOAD
   UINT32   csum_flags;
#endif

   DRV_LOG (DRV_DEBUG_TX, "gei82543EndSend...\n", 1, 2, 3, 4, 5, 6);

#ifndef _FREE_VERSION
   if (pMblk == NULL)
      return ERROR;
#endif

   /* return if entering polling mode */

   if (pDrvCtrl->flags & FLAG_POLLING_MODE)
   {
      netMblkClChainFree (pMblk);
      errno = EINVAL;
      return (ERROR);
   }

   /* get the mBlk number in this packet */

   mBlkNum  = 0;
   pMblkDup = pMblk;

   while (pMblkDup != NULL)
   {
      mBlkNum++;
      pMblkDup = pMblkDup->mBlkHdr.mNext;
   }

   END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

   if ((mBlkNum >= pDrvCtrl->txDescFreeNum) &&
       (mBlkNum >= gei82543TxDesCleanGet (pDrvCtrl)))
   {
      /* not enough TX descriptors */

      DRV_LOG (DRV_DEBUG_TXD, ("gei82543EndSend: No TX descriptor "
                               "available\n"), 1, 2, 3, 4, 5, 6);

      /*
        * coalesce mBlks if the number of mBlks in this packet exceeds
        * the maximum  available transmit descriptors.
        * Note that there are at most (txDescNum - 1) descriptors free
        * at any time, and we also need an extra descriptor for the
        * context descriptor.
        */

      if (mBlkNum >= pDrvCtrl->txDescNum - 1)
      {
         UINT32 len;

         DRV_LOG (DRV_DEBUG_TXD,
                  ("gei82543EndSend:mBlks num exceeds max TX desc\n"),
                  1, 2, 3, 4, 5, 6);

         pMblkDup = GEI_TUPLE_GET (pDrvCtrl->end.pNetPool);
         if (pMblkDup == NULL)
         {
            DRV_LOG (DRV_DEBUG_TXD, "No enough MBLKs !\n", 1, 2, 3, 4, 5, 6);
            END_TX_SEM_GIVE (&pDrvCtrl->end);
            return (END_ERR_BLOCK);
         }

         if ((1 < pDrvCtrl->txDescFreeNum) ||
             (1 < gei82543TxDesCleanGet (pDrvCtrl)))
         {
            len = netMblkToBufCopy (pMblk, pMblkDup->mBlkHdr.mData, NULL);

            pMblkDup->mBlkPktHdr.len      = len;
            pMblkDup->mBlkHdr.mLen        = len;
#ifdef GEI_CSUM_OFFLOAD
            pMblkDup->m_pkthdr.csum_flags = pMblk->m_pkthdr.csum_flags;
            pMblkDup->m_pkthdr.csum_data  = pMblk->m_pkthdr.csum_data;
            CSUM_IP_HDRLEN(pMblkDup)      = CSUM_IP_HDRLEN (pMblk);
#endif
            netMblkClChainFree (pMblk);     /* free the given mBlk chain */

            pMblk = pMblkDup;
            goto have_tuples;
         }

         netMblkClChainFree (pMblkDup);
         mBlkNum = 1;
      }

      pDrvCtrl->txStallCount++;

      if (pDrvCtrl->txStall != TRUE)
      {
         pDrvCtrl->txStall      = TRUE;

         pDrvCtrl->txReqDescNum = mBlkNum;

         if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
             pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         {
            /* enable the TXD_LOW interrupt */
            GEI_WRITE_REG(INTEL_82543GC_IMS, IMS_TXDLOW_BIT);
         }

         GEI_WRITE_REG(INTEL_82543GC_TIDV, MIN_TXINT_DELAY);
      }

      END_TX_SEM_GIVE (&pDrvCtrl->end);

      return (END_ERR_BLOCK);
   }

have_tuples:

#ifdef GEI_CSUM_OFFLOAD
   /*
    * Don't change contexts for non-IP datagrams.
    * A change in either the checksum flags or the IP header length
    * requires a new TX context descriptor..
    */

   csum_flags = pMblk->mBlkPktHdr.csum_flags & (CSUM_IP | CSUM_TCP | CSUM_UDP |
                                                CSUM_TCPv6 | CSUM_UDPv6);
   if (csum_flags != 0)
   {
      if (pDrvCtrl->lastCsumCtx != csum_flags ||
          CSUM_IP_HDRLEN (pMblk) != pDrvCtrl->lastIpHdrLen)
      {
         pDrvCtrl->lastCsumCtx  = csum_flags;
         pDrvCtrl->lastIpHdrLen = CSUM_IP_HDRLEN (pMblk);
         gei82543ExtTxContextNew (pDrvCtrl, pMblk);
      }
   }
#endif

   DRV_LOG (DRV_DEBUG_TX, "loan buffers of the packet and transmit...\n",
            1, 2, 3, 4, 5, 6);

   gei82543LoanTransmit (pDrvCtrl, pMblk);

   /* release semaphore now */

   END_TX_SEM_GIVE (&pDrvCtrl->end);

   DRV_LOG (DRV_DEBUG_TX, ("gei82543EndSend...Done\n"), 1, 2, 3, 4, 5, 6);

   return (OK);
}

#ifdef GEI_CSUM_OFFLOAD
/**********************************************************************
*
* gei82543ExtTxContextNew - set up a TCP/IP Context TX descriptor
*
* This routine hands a new TCP/IP Context TX descriptor to the device
* informing it of the offsets for hardware checksum calculation.
*
*
* This routine is only called when at least 2 transmit descriptors
* are free (one for the context descriptor, one for a data descriptor).
*
*
* RETURNS: OK or ERROR.
*/

_WRS_FASTTEXT
LOCAL void gei82543ExtTxContextNew (GEI_END_DEVICE*   pDrvCtrl,
                                    M_BLK*        pMblk /* Head mBlk of packet generating new ctx. */
                                    )
{
   char*  pDesc;                  /* pointer to context descriptor */
   UINT32 ipCSS;
   UINT32 ipCSE;
   UINT32 ipCSO;
   UINT32 tuCSS;
   UINT32 tuCSE;
   UINT32 tuCSO;
   UINT32 value;
   UINT32 txType = 0;

   pDesc = GEI_GET_TX_DESC_ADDR(pDrvCtrl->txDescTail);

   if (pMblk->m_pkthdr.csum_flags & (CSUM_TCP | CSUM_TCPv6))
      txType = (TXD_CMD_CTX_TCP << 24);

   /* Hmm, the CTX_IP bit may not be necessary unless doing segmentation */
   if (pMblk->m_pkthdr.csum_flags & (CSUM_IP | CSUM_TCP | CSUM_UDP))
      txType |= (TXD_CMD_CTX_IP << 24);

   /*
    * Extended TCP/IP Context TX descriptor structure.
    * Identified by DEXT bit = 1 in TUCMD field, and DTYP = 0000b
    */

   value = (((TXD_CMD_CTX_IDE | TXD_CMD_CTX_RS | TXD_CMD_CTX_DEXT) << 24) |
            txType);

   GEI_WRITE_DESC_LONG (pDesc, TXDESC_CTX_PKTLEN_OFFSET, value);

   ipCSS = SIZEOF_ETHERHEADER;

   ipCSE = ipCSS + (CSUM_IP_HDRLEN(pMblk) - 1);
   ipCSO = ipCSS + IP_HDR_CKSUM_OFFSET;  /* don't care for IPv6 */

   value = ipCSS | (ipCSO << 8) | (ipCSE << 16);
   GEI_WRITE_DESC_LONG (pDesc, TXDESC_CTX_IPCSS_OFFSET, value);

   tuCSS = ipCSE + 1;
   tuCSO = tuCSS + CSUM_XPORT_CSUM_OFF(pMblk);
   tuCSE = 0;

   value = tuCSS | (tuCSO << 8) | (tuCSE << 16);

   GEI_WRITE_DESC_LONG (pDesc, TXDESC_CTX_TUCSS_OFFSET, value);

   /* Zero STATUS, reserved, HDRLEN, and MSS fields */

   GEI_WRITE_DESC_LONG (pDesc, TXDESC_CTX_STATUS_OFFSET, 0);

   /* update the tail pointer in the driver structure */

   GEI_GET_TX_DESC_TAIL_UPDATE(pDrvCtrl->txDescTail, 1);

   pDrvCtrl->txDescFreeNum--;

   CACHE_PIPE_FLUSH();

   GEI_WRITE_REG(INTEL_82543GC_TDT, pDrvCtrl->txDescTail);
}
#endif

/**********************************************************************
*
* gei82543LoanTransmit - store the mbuf address into TX descriptor to transmit
*
* This routine stores the mbuf address into the Tx descriptors address field,
* and then directly transfer data from mbuf to chip memory without copying,
* The loaned mBlk will be freed later
*
* RETURNS: N/A.
*/

_WRS_FASTTEXT
LOCAL void gei82543LoanTransmit (GEI_END_DEVICE*     pDrvCtrl, /* END device structure */
                                 M_BLK_ID pMblkFirst           /* pointer to the beginning of a mBlk */
                                 )
{
   P_TX_DESCTL pDescCtl;            /* pointer to a Tx descriptor */
                                    /* control structure*/
   char*       pDesc;               /* pointer to a descriptor */
   M_BLK_ID    pMblk;               /* pointer to a Mblk */
   UINT32      len;                 /* temporary len for each mBlk */
   UINT32      index   = 0;         /* index to the number of mBlk */
   int         tmpTail;             /* temporary tail index of TX descriptor*/
   UINT32      poptSta = 0;         /* value in the option and status filed */
   UINT32      dataCtx;             /* value in the command field */
   UINT32      busAddr;

   dataCtx = (UINT32)((((TXD_CMD_IDE | TXD_CMD_RS | TXD_CMD_DEXT) << 24) |
                       (TXD_DTYP_BIT << 20)));

#ifdef GEI_CSUM_OFFLOAD
   if (pMblkFirst->m_pkthdr.csum_flags & CSUM_IP)
      poptSta |= TXD_DATA_POPTS_IXSM_BIT << 8;

   if (pMblkFirst->m_pkthdr.csum_flags & (CSUM_TCP | CSUM_UDP |
                                          CSUM_TCPv6 | CSUM_UDPv6))
      poptSta |= TXD_DATA_POPTS_TXSM_BIT << 8;
#endif

   pMblk = pMblkFirst;

   FOREVER
   {
      /* get the available TX descriptor and its manager */

      GEI_GET_TX_DESC_TAIL_UPDATE(tmpTail, index);

      pDescCtl = GEI_GET_TX_DESC_CTL_ADDR(tmpTail);

      pDesc    = GEI_GET_TX_DESC_ADDR(tmpTail);

      len      = pMblk->mBlkHdr.mLen;

      /* flush buffer for cache coherence */

      GEI_END_USR_CACHE_FLUSH(((void*)(pMblk->mBlkHdr.mData)), len);

      GEI_WRITE_DESC_LONG(pDesc, TXDESC_BUFADRHIGH_OFFSET, 0);

      busAddr = (UINT32)GEI_VIRT_TO_PHYS (pMblk->mBlkHdr.mData);
      busAddr = GEI_PHYS_TO_BUS (busAddr);

      GEI_WRITE_DESC_LONG(pDesc, TXDESC_BUFADRLOW_OFFSET, busAddr);

      /* zero the status field in the TX descriptor */

      GEI_WRITE_DESC_LONG(pDesc, TXDESC_STATUS_OFFSET, poptSta);

      index++;

      /* Skip to next non-zero-length M_BLK */
      do
      {
         pMblk = pMblk->mBlkHdr.mNext;

         if (pMblk == NULL)
            goto lastseg;

      } while (pMblk->mBlkHdr.mLen == 0);

      pDescCtl->mBlk = NULL;

      /* set up the length field */

      GEI_WRITE_DESC_LONG(pDesc, TXDESC_LENGTH_OFFSET,
                          ((UINT16)len | dataCtx));
   }      /* FOREVER */

lastseg:

   pDescCtl->mBlk = pMblkFirst;

   /* Hardware is already programmed for autopadding. */

   /* set up the length/command field */

   GEI_WRITE_DESC_LONG(pDesc, TXDESC_LENGTH_OFFSET,
                       ((UINT16)len | (dataCtx) |
                        ((TXD_CMD_EOP | TXD_CMD_IFCS) << 24)));

   /* update the tail pointer in the driver structure */

   GEI_GET_TX_DESC_TAIL_UPDATE(pDrvCtrl->txDescTail, index);

   pDrvCtrl->txDescFreeNum -= index;

   /* update the tail pointer in TDT register */

   GEI_WRITE_REG(INTEL_82543GC_TDT, pDrvCtrl->txDescTail);

   CACHE_PIPE_FLUSH();

   return;
}

/*************************************************************************
*
* gei82543TxDesCleanGet - clean up TX descriptors
*
* This routine cleans up TX descriptors and update available TX descriptor
* for transmitting.
*
* RETURNS: number of available TX descriptors
*/

_WRS_FASTTEXT
LOCAL int gei82543TxDesCleanGet (GEI_END_DEVICE* pDrvCtrl       /* device to clean up TX descriptor */
                                 )
{
   P_TX_DESCTL pDescCtl;        /* TX descriptor control structure */
   char*       pDesc;           /* pointer to descriptor */
   int         maxTxDesFree;    /* maximum available TX descriptors to free */
   int         index;           /* index */

   /* check the maximum free TX Desc */

   maxTxDesFree = pDrvCtrl->txDescNum - 1;

   index        = pDrvCtrl->txDescLastCheck; /* last known clean */

   while (pDrvCtrl->txDescFreeNum != maxTxDesFree)
   {
      int next;

      next  = (index == maxTxDesFree) ? 0 : index + 1;

      pDesc =  GEI_GET_TX_DESC_ADDR(next);

      /* check data has been transmitted */

      if (!(GEI_READ_DESC_BYTE(pDesc, TXDESC_STATUS_OFFSET) &
            TXD_STAT_DD))
         break;

      index    = next;

      pDescCtl = GEI_GET_TX_DESC_CTL_ADDR(index);

      if (pDescCtl->mBlk != NULL)
      {
         /* free loaned TX buffers */

         netMblkClChainFree (pDescCtl->mBlk);

         pDescCtl->mBlk = NULL;
      }

      /* update the available TX descriptor */

      pDrvCtrl->txDescFreeNum++;
   }

   /* update the index of descriptor checked */

   pDrvCtrl->txDescLastCheck = index;

   return (pDrvCtrl->txDescFreeNum);
}

/*************************************************************************
*
* gei82543TxDesCleanForce - clean up TX descriptors
*
* This routine cleans up TX descriptors and update available TX descriptor
* for transmitting.
*
* RETURNS: number of available TX descriptors
*/

LOCAL int gei82543TxDesCleanForce (GEI_END_DEVICE* pDrvCtrl       /* device to clean up TX descriptor */
                                   )
{
   P_TX_DESCTL pDescCtl;        /* TX descriptor control structure */
   int         maxTxDesFree;    /* maximum available TX descriptors to free */
   int         index;           /* index */

   /* check the maximum free TX Desc */

   maxTxDesFree = pDrvCtrl->txDescNum - 1;

   index        = pDrvCtrl->txDescLastCheck;

   while (pDrvCtrl->txDescFreeNum != maxTxDesFree)
   {
      index    = (index == maxTxDesFree) ? 0 : index + 1;

      pDescCtl = GEI_GET_TX_DESC_CTL_ADDR(index);

      if (pDescCtl->mBlk != NULL)
      {
         /* free loaned TX buffers */

         netMblkClChainFree (pDescCtl->mBlk);

         pDescCtl->mBlk = NULL;
      }

      /* update the available TX descriptor */

      pDrvCtrl->txDescFreeNum++;
   }

   /* update the index of descriptor checked */

   pDrvCtrl->txDescLastCheck = index;

   return (pDrvCtrl->txDescFreeNum);
}

/*************************************************************************
*
* gei82543EndInt - handle 82543 chip interrupt event
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A.
*/
_WRS_FASTTEXT
LOCAL void gei82543EndInt (GEI_END_DEVICE*    pDrvCtrl    /* device to handle interrupt */
                           )
{
   UINT32 stat;                 /* cause of interrupt */
   UINT32 statusRegVal;         /* status register value */
   UINT32 intTypeChk;           /* type of interrupt to be checked */

   DRV_LOGMSG (DRV_DEBUG_INT, "gei82543EndInt ...\n", 1, 2, 3, 4, 5, 6);

   /* read the device status register */

   GEI_READ_REG(INTEL_82543GC_ICR, stat);
   stat &= INTEL_82543GC_VALID_INT;

   /* return if not interrupt for this device */

   if (stat == 0)
   {
      DRV_LOGMSG (DRV_DEBUG_INTD, "gei82543EndInt: PCI interrupt is not "
                  "caused by this GEI device\n", 1, 2, 3, 4, 5, 6);
      return;
   }

   /* handle link interrupt event */

   if (stat & INT_LINK_CHECK)
   {
      DRV_LOGMSG (DRV_DEBUG_INTD, "gei82543EndInt: get a Link problem\n",
                  1, 2, 3, 4, 5, 6);
      /* link status change, get status register */

      GEI_READ_REG(INTEL_82543GC_STATUS, statusRegVal);

      if (statusRegVal & STATUS_LU_BIT)
      {
         /* link is up */

         if (pDrvCtrl->cableType == GEI_FIBER_MEDIA)
         {
            /*
             * restart link if partner send back a TXCW after
             * force link
             */
            UINT32 rxcwRegVal;
            UINT32 txcwRegVal;

            GEI_READ_REG(INTEL_82543GC_RXCW, rxcwRegVal);
            GEI_READ_REG(INTEL_82543GC_TXCW, txcwRegVal);

            if ((pDrvCtrl->linkMethod == GEI82543_FORCE_LINK) &&
                (rxcwRegVal & RXCW_C_BIT) &&
                (!(txcwRegVal & TXCW_ANE_BIT)))
            {
               DRV_LOGMSG (DRV_DEBUG_INTD, "gei82543EndInt: Hardware "
                           "Auto-Negotiation restart\n", 1, 2, 3, 4, 5, 6);

               if ( (netJobAdd ((FUNCPTR)gei82543linkTBISetup, (int)pDrvCtrl,
                                FALSE, 0, 0, 0)) == ERROR)
                  DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: TBISetup\n", 1, 2, 3, 4, 5, 6);
               return;
            }
         }

         else if (pDrvCtrl->cableType == GEI_COPPER_MEDIA)
         {
            DRV_LOGMSG(DRV_DEBUG_INTD, "Reconfig device ... \n", 1, 2, 3, 4, 5, 6);

            /* re-configure device based on GMII negotiation results */

            if (pDrvCtrl->linkMethod == GEI82543_HW_AUTO)
            {
               if ( (netJobAdd ((FUNCPTR)gei82543GMIIphyReConfig,
                                (int)pDrvCtrl, 0, 0, 0, 0)) == ERROR)
                  DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: Reconfig\n", 1, 2, 3, 4, 5, 6);
               return;
            }
         }
      }
      else    /* if link is down */
      {
         return;
      }
   }

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      if (stat & INT_RXTO_BIT)
         pDrvCtrl->rxIntCount++;

      if (stat & INT_RXO_BIT)
         pDrvCtrl->rxORunIntCount++;

      if (stat & (INT_TXDW_BIT | INT_TXDLOW_BIT))
         pDrvCtrl->txIntCount++;

      intTypeChk = AVAIL_RX_TX_INT | INT_TXDLOW_BIT;
   }
   else
   {
      intTypeChk = AVAIL_RX_TX_INT;
   }

   /* tNetTask handles RX/TX interrupts */

   if ((stat & intTypeChk) && (pDrvCtrl->rxtxHandling != TRUE))
   {
      pDrvCtrl->rxtxHandling = TRUE;

      /* disable receive / transmit interrupt */

      GEI_WRITE_REG(INTEL_82543GC_IMC, intTypeChk);

      /*
       * NOTE: Read back any chip register to flush PCI
       * bridge write buffer in case this adaptor is behind
       * a PCI bridge.
       */

      CACHE_PIPE_FLUSH ();

      GEI_READ_REG(INTEL_82543GC_IMS, stat);

      /* defer the handling ... */

      if ( (netJobAdd ((FUNCPTR)gei82543RxTxIntHandle, (int)pDrvCtrl, 0,
                       0, 0, 0)) == ERROR)
      {
         DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: RxTxIntHandle\n", 1, 2, 3, 4, 5, 6);

      }
   }

   DRV_LOGMSG (DRV_DEBUG_INT, "gei82543EndInt...Done\n", 1, 2, 3, 4, 5, 6);

   return;
}

/*************************************************************************
*
* gei82543TxStallCheck - check for TX descriptors and restart TX if needed
*
* This routine checks the availability of TX descriptors, frees any
* mBlks it can and restarts transmitting if sufficient TX descriptors
* are available.
*
* RETURN: N/A
*/

_WRS_FASTTEXT
LOCAL void gei82543TxStallCheck (GEI_END_DEVICE*    pDrvCtrl    /* device to check TX stall */
                                 )
{
   int txFreeDescNum;           /* available Tx number */

   DRV_LOG (DRV_DEBUG_TX, "handle txStall ...\n", 1, 2, 3, 4, 5, 6);

   /* take a semaphore */

   END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

   /* Consider omitting txStall logic, and just dropping output
    * packets rather than returning END_ERR_BLOCK when we don't have
    * sufficient TX descriptors. At gigabit speeds we're less likely
    * to run into stalls to begin with, and usually the pool of TX
    * descriptors would provide sufficient queueing capacity.
    */
   if (pDrvCtrl->txResoFreeQuick || pDrvCtrl->txStall)
   {
      DRV_LOG (DRV_DEBUG_TXD, "handle txStall ...\n", 1, 2, 3, 4, 5, 6);
      txFreeDescNum = gei82543TxDesCleanGet (pDrvCtrl);

      /* We need an extra descriptor for the TX context descriptor */

      if (pDrvCtrl->txStall && pDrvCtrl->txReqDescNum < txFreeDescNum)
      {
         /* TX Descriptors available, restart transmit */

         DRV_LOG (DRV_DEBUG_TXD, "Restart mux\n", 1, 2, 3, 4, 5, 6);

         if ( (netJobAdd ((FUNCPTR)muxTxRestart, (int)&pDrvCtrl->end,
                          0, 0, 0, 0)) == ERROR)
            DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: muxTxRestart\n", 1, 2, 3, 4, 5, 6);

         if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
             pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         {
            /* disable the TXD_LOW interrupt */

            GEI_WRITE_REG(INTEL_82543GC_IMC, IMC_TXDLOW_BIT);
         }

         GEI_WRITE_REG(INTEL_82543GC_TIDV, pDrvCtrl->txIntDelay);

         pDrvCtrl->txStall = FALSE;
      }
   }

   END_TX_SEM_GIVE (&pDrvCtrl->end);

   return;
}

/*************************************************************************
*
* gei82543RxTxIntHandle - receive/transmit interrupt handle in task mode
*
* This routine gets the receive packet and pass them to upper network stack.
* It also cleans up TX descriptor if necessary.
*
* RETURNS: N/A.
*/

_WRS_FASTTEXT
LOCAL void gei82543RxTxIntHandle (GEI_END_DEVICE*    pDrvCtrl     /* device to handle RX/TX */
                                  )
{
   UINT32 rxCountPerInt = 0;     /* maximum RX descriptors to handle a int */
   UINT32 retry         = 0;     /* retry count */
   char*  pRxDesc;               /* RX descriptor pointer */
   UINT32 stat;                  /* cause of interrupt */
   UINT8  rxdStat;               /* status of recv descriptor */
   UINT32 ims;

   DRV_LOG (DRV_DEBUG_RX, "gei82543RxTxIntHandle...\n", 1, 2, 3, 4, 5, 6);

   do
   {
      while (rxCountPerInt++ < pDrvCtrl->maxRxNumPerInt)
      {
         /* get the current RX descriptor */

         pRxDesc = GEI_GET_RX_DESC_ADDR(pDrvCtrl->rxDescTail);

         /* check RX descriptor status */

         rxdStat = GEI_READ_DESC_BYTE(pRxDesc, RXDESC_STATUS_OFFSET);
         if ((rxdStat & RXD_STAT_DD) == 0)
            break;

         /* process an incoming packet */

         if (gei82543Recv (pDrvCtrl, pRxDesc, rxdStat) != OK)
         {
            break;
         }
      }

      if (retry++ == 2 || rxCountPerInt >= pDrvCtrl->maxRxNumPerInt)
         break;

      GEI_READ_REG(INTEL_82543GC_ICR, stat);

   } while (stat & AVAIL_RX_INT);

   /* handle case that TX stalls, free mBlks if desired */

   gei82543TxStallCheck (pDrvCtrl);

   pDrvCtrl->rxtxHandling = FALSE;

   /* enable RX/TX interrupts */

   ims = AVAIL_RX_TX_INT;

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      pDrvCtrl->rxtxHandlerNum++;
      pDrvCtrl->rxPacketNum += rxCountPerInt;

      if (pDrvCtrl->txStall)
         ims |= IMS_TXDLOW_BIT;
   }

   GEI_WRITE_REG(INTEL_82543GC_IMS, ims);

   CACHE_PIPE_FLUSH();

   DRV_LOG (DRV_DEBUG_RX, "gei82543RxTxIntHandle...Done\n",
            1, 2, 3, 4, 5, 6);
   return;
}

/*************************************************************************
*
* gei82543Recv - process an incoming packet
*
* This routine checks the incoming packet, and passes it the upper layer.
*
* RETURNS: OK, or ERROR if receiving fails
*/

_WRS_FASTTEXT
LOCAL STATUS gei82543Recv (GEI_END_DEVICE*    pDrvCtrl, /* device structure */
                           char*          pRxDesc,      /* pointer to RX descriptor */
                           UINT8 rxdStat                /* RX descriptor status */
                           )
{
   UINT16   len;                    /* len of packet received */
   M_BLK_ID pMblk;                  /* mBlk to store this packet */
   M_BLK_ID pNewMblk;               /* Replacement M_BLK tuple */
   UINT8    rxdErr;                 /* RX descriptor Error field */
   UINT8    csumErr;                /* RX descriptor Error field */
#ifdef GEI_CSUM_OFFLOAD
   UINT32   csum_flags;
#endif
   int      tempTail;
   UINT32   csum;
#if defined (INCLUDE_TBI_COMPATIBILE)
   char*    pData;                          /* receive data pointer */
#endif

   DRV_LOG (DRV_DEBUG_RX, "gei82543Recv start...\n",  1, 2, 3, 4, 5, 6);

   /* check packet status */

   if (!(rxdStat & RXD_STAT_EOP))
   {
      DRV_LOG (DRV_DEBUG_RXD, "Packet EOP not set\n", 1, 2, 3, 4, 5, 6);

      LOGMSG("Packet EOP not set \n", 1, 2, 3, 4, 5, 6);

      goto receiveError;
   }

   csumErr = GEI_READ_DESC_BYTE(pRxDesc, RXDESC_ERROR_OFFSET);
   rxdErr  = csumErr & ~(RXD_ERROR_IPE | RXD_ERROR_TCPE | RXD_ERROR_RSV);

#ifdef INCLUDE_TBI_COMPATIBLE

   GEI_READ_DESC_WORD(pRxDesc, RXDESC_LENGTH_OFFSET, len);

   if (rxdErr != 0)
   {
      BOOL frameAccept = FALSE;

      if (rxdErr == RXD_ERROR_CE && pDrvCtrl->tbiCompatibility)
      {
         UINT8 lastChar;

         /* software workaround for TBI compatibility problem */

         pMblk    = pDrvCtrl->pRxMblkArray [pDrvCtrl->rxDescTail];

         pData    = pMblk->mBlkHdr.mData;
         lastChar = *(UINT8*)(pData + len - 1);

         if (lastChar == CARRIER_EXTENSION_BYTE &&
             len > MIN_ETHER_PACKET_SIZE &&
             len <= (pDrvCtrl->mtu + GEI_DEFAULT_ETHERHEADER + 1))
         {
            frameAccept = TRUE;
            len--;
         }
      }

      if (!frameAccept)
      {
         DRV_LOG (DRV_DEBUG_RXD, "Packet Error 0x%x\n", rxdErr,
                  2, 3, 4, 5, 6);
         LOGMSG("Packet error 0x%x \n", rxdErr, 2, 3, 4, 5, 6);

         goto receiveError;
      }
   }
#else /* !INCLUDE_TBI_COMPATIBLE */

   if (rxdErr != 0)
   {
      DRV_LOG (DRV_DEBUG_RXD, "Packet Error 0x%x\n", rxdErr, 2, 3, 4, 5, 6);

      LOGMSG("Packet error 0x%x \n", rxdErr, 2, 3, 4, 5, 6);

      goto receiveError;
   }

   /* get packet length */

   GEI_READ_DESC_WORD(pRxDesc, RXDESC_LENGTH_OFFSET, len);

#endif /* INCLUDE_TBI_COMPATIBLE */

   if (len > (pDrvCtrl->mtu + GEI_DEFAULT_ETHERHEADER) ||
       len < MIN_ETHER_PACKET_SIZE)
   {
      DRV_LOG (DRV_DEBUG_RXD, ("invalid packet length=0x%x!\n"), len,
               0, 0, 0, 0, 0);
      goto receiveError;
   }

   pNewMblk = GEI_TUPLE_GET (pDrvCtrl->end.pNetPool);

   if (pNewMblk == NULL)
   {
      DRV_LOG (DRV_DEBUG_RXD, "Not free new Cluster!\n", 1, 2, 3, 4, 5, 6);

      goto receiveError;
   }


   /*
    * Pre-invalidate new cluster before handing to hardware to avoid
    * later cache-line write-back of dirty lines, which could overwrite
    * received data. [This is especially important when using RX checksum
    * offload.] Unfortunately, this requires that we invalidate
    * the whole cluster rather than just the received portion.
    */
   GEI_END_DRV_CACHE_INVALIDATE (((UINT32)pNewMblk->mBlkHdr.mData),
                                 pDrvCtrl->rxPktSz);

   pNewMblk->mBlkHdr.mData          += pDrvCtrl->offset;

   tempTail                          = pDrvCtrl->rxDescTail;

   pMblk                             = pDrvCtrl->pRxMblkArray [tempTail];

   pDrvCtrl->pRxMblkArray [tempTail] = pNewMblk;

   {
      char* pNewData = pNewMblk->mBlkHdr.mData;
      GEI_WRITE_DESC_LONG(pRxDesc, RXDESC_BUFADRLOW_OFFSET,
                          (UINT32)GEI_VIRT_TO_BUS(pNewData));
   }

   /* clean up status field */

   GEI_WRITE_DESC_BYTE(pRxDesc, RXDESC_STATUS_OFFSET, 0);

   GEI_READ_DESC_WORD (pRxDesc, RXDESC_CHKSUM_OFFSET, csum);

   /* update the tail pointer */

   {
      int tail = tempTail + 1;
      if (tail == pDrvCtrl->rxDescNum)
         tail = 0;
      pDrvCtrl->rxDescTail = tail;
   }

   /* kick this descriptor for receiving packet */

   GEI_WRITE_REG(INTEL_82543GC_RDT, tempTail);

   CACHE_PIPE_FLUSH();

   /* set up Mblk */

   pMblk->mBlkHdr.mLen    = len;
   pMblk->mBlkHdr.mFlags |= M_PKTHDR;
   pMblk->mBlkPktHdr.len  = len;

#ifdef GEI_CSUM_OFFLOAD
   csum_flags = 0;

   if ((pDrvCtrl->hwCaps.cap_enabled & IFCAP_RXCSUM) != 0)
   {
      if ((rxdStat & RXD_STAT_IXSM) == 0)
      {
         if ((GEI_RX_CSUM(pDrvCtrl) & CSUM_IP) != 0 &&
             (rxdStat & RXD_STAT_IPCS))
         {
            if ((csumErr & RXD_ERROR_IPE) == 0)
               csum_flags = CSUM_IP_CHECKED | CSUM_IP_VALID;
            else
               csum_flags = CSUM_IP_CHECKED;
         }

         /* XXX - not easy to separately enable CSUM_TCP, CSUM_UDP */
         /* Note, RXD_STAT_TCPCS seems never to be set for IPv6 */

         if (rxdStat & RXD_STAT_TCPCS)
         {

            /*
             * It's been observed that the TCP checksum offload
             * logic in the 82541 and 82546 (and probably the
             * 82545 as well) will sometimes report an invalid
             * TCP checksum on a valid packet. The packet in
             * question is an ACK with no data, which is smaller
             * than the minimum ethernet frame size (64 bytes).
             * We sometimes see such ACKs generated by Solaris.
             *
             * The logic here handles this case even though it
             * may not be technically accurate: we are only trusting
             * the card to report valid TCP checksums we ignore
             * the card the rest of the time (instead checking the
             * checksum in software). This is the logic that Intel
             * uses in their FreeBSD driver.
             */

            if ((csumErr & RXD_ERROR_TCPE) == 0)
            {
               csum_flags                 |= (CSUM_DATA_VALID | CSUM_PSEUDO_HDR);
               pMblk->mBlkPktHdr.csum_data = 0xffff;
            }

            goto csum_done;
         }
      }

      /* XXX - RX transport checksum offload over IPv6 seems not
       * to work for the 82546. We use the packet checksum instead.
       * We only catch IPv6 packets where the IPv6 header
       * immediately follows the 14-byte ethernet header.
       * VLAN, SNAP, not handled here.
       */

      if (mtod (pMblk, UINT16*)[6] == htons (ETHERTYPE_IPV6))
      {
         UINT8* pH;
         UINT8  nh;
         UINT16 plen;

         /*
          * It's debatable whether we should do any work backing out
          * the checksum over any extension headers when the packet might
          * not even be destined for us (which we can't easily determine
          * here).  That could add additional useless work when only
          * forwarding the packet. So let's only handle the case in
          * which the UDP or TCP packet immediately follows the IPv6
          * header.  We already checked above that the received length
          * is at least 60, so we know that we have at least 6 bytes
          * beyond the ethernet and IPv4 header.
          */

         /* Set pH to point at the IPv6 header */
         pH = mtod (pMblk, UINT8*) + 14;

         nh = pH [6]; /* next header */

         if (nh != IPPROTO_TCP && nh != IPPROTO_UDP)
            goto csum_done;

         /*
          * Although for UDP over IPv6, the minimum frame length
          * (not counting ethernet CRC) is 14 + 40 + 8 = 62, which
          * is greater than the minimum ethernet frame length,
          * so that a peer device would never HAVE to insert any
          * padding, a peer application MIGHT still send an
          * IPv6 datagram followed by extra padding on the wire.
          * The packet checksum would cover this padding and the
          * checksum over the padding would need to be backed out.
          * In order to avoid spending the time to do that here,
          * (and dealing with the complications) we bail out if
          * we detect extra padding. pH + 4 is where the IPv6 payload
          * length lives.
          */

         plen = *(UINT16*)(pH + 4);

         if (len > ntohs (plen) + 14 + 40)
            goto csum_done;

         /*
          * The packet checksum field in the receive descriptor
          * contains the complemented ones-complement sum over
          * the portion of the packet starting at offset
          * RXCSUM.PCSS, "helpfully" converted to host byte order.
          * Undo the complement.. The byte swap would also
          * have to be undone if we decided to back out any portion
          * of the packet checksum covering extension headers or
          * padding. However the stack presently expects the checksum in
          * 'host' byte order (compensating silliness), so we don't
          * need to do either swap here.
          */
         csum ^= 0xffff;

#if 0
         csum = htons (csum);
         /* back out portions of checksum */
         csum = ntohs (csum);
#endif
         pMblk->mBlkPktHdr.csum_data = csum;

         /* Not CSUM_PSEUDO_HDR */
         csum_flags |= CSUM_DATA_VALID;
      }
   }

csum_done:

   pMblk->mBlkPktHdr.csum_flags = csum_flags;
#endif

   /* call the upper layer's receive routine. */

   END_RCV_RTN_CALL(&pDrvCtrl->end, pMblk);

   DRV_LOG (DRV_DEBUG_RX, "Receive done!\n", 1, 2, 3, 4, 5, 6);

   return OK;

receiveError:

   gei82543RxDesUpdate (pDrvCtrl);

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      pDrvCtrl->rxPacketDrvErr++;
   }

   DRV_LOG (DRV_DEBUG_RXD, "Receive ERROR!\n", 1, 2, 3, 4, 5, 6);

   return ERROR;
}

/*************************************************************************
*
* gei82543RxDesUpdate - clean up the receive descriptor
*
* This routine cleans up receive descriptors, reusing the current cluster.
* Then it updates the tail pointer to make it available to the
* hardware.
*
* RETURNS: N/A
*/

LOCAL void gei82543RxDesUpdate (GEI_END_DEVICE*    pDrvCtrl    /* device to update  */
                                )
{
   int   tempTail;           /* temporary tail index */
   char* pRxDesc;            /* RX descriptor */

   DRV_LOG (DRV_DEBUG_RX, "gei82543RxDesUpdate...\n", 1, 2, 3, 4, 5, 6);

   /* get the RX tail descriptor */

   tempTail = pDrvCtrl->rxDescTail;
   pRxDesc  = GEI_GET_RX_DESC_ADDR(tempTail);

   /* clean up status field */

   GEI_WRITE_DESC_BYTE(pRxDesc, RXDESC_STATUS_OFFSET, 0);

   /* update the tail pointer */

   {
      int tail = tempTail + 1;
      if (tail == pDrvCtrl->rxDescNum)
         tail = 0;
      pDrvCtrl->rxDescTail = tail;
   }

   /* kick this descriptor for receiving packet */

   GEI_WRITE_REG(INTEL_82543GC_RDT, tempTail);

   CACHE_PIPE_FLUSH();

   DRV_LOG (DRV_DEBUG_RX, "gei82543RxDesUpdate...Done\n",
            1, 2, 3, 4, 5, 6);
   return;
}

/*************************************************************************
*
* gei82543TxMblkFree - free transmit mBlk(s)
*
* This routine frees transmit mBlk(s) after packets have been transmitted
* on wire.
*
* RETURNS: N/A.
*/

LOCAL void gei82543TxMblkFree (GEI_END_DEVICE*    pDrvCtrl, /* device to free transmit mBlk(s) */
                               int mode                     /* AUTO/FORCE */
                               )
{
   int maxFreeTxDesc;

   DRV_LOG (DRV_DEBUG_TX, "gei82543TxMblkFree...\n", 1, 2, 3, 4, 5, 6);

   /* take semaphore */

   END_TX_SEM_TAKE (&pDrvCtrl->end, WAIT_FOREVER);

   /* sanity check */

   if (mode == FREE_ALL_FORCE)
   {
      gei82543TxDesCleanForce (pDrvCtrl);

      maxFreeTxDesc = pDrvCtrl->txDescNum - 1;

      if (pDrvCtrl->txDescFreeNum != maxFreeTxDesc)
      {
         LOGMSG("Panic: TX Descriptors are not cleaned up\n", 1, 2, 3, 4, 5, 6);
      }
   }
   else
      gei82543TxDesCleanGet (pDrvCtrl);

   END_TX_SEM_GIVE (&pDrvCtrl->end);

   DRV_LOG (DRV_DEBUG_TX, "gei82543TxMblkFree...Done\n", 1, 2, 3, 4, 5, 6);

   return;
}

/*****************************************************************************
*
* gei82543TxResoFree - free loaned TX mBlks constantly
*
* This routine frees loaned TX mBlks constantly, and updates device statistics,
* it may also execute a callback function defined in sysGei82543End.c
*
* RETURNS: N/A
*/

LOCAL void gei82543TxResoFree (GEI_END_DEVICE* pDrvCtrl   /* END device */
                               )
{
   ADAPTOR_INFO* pBoard;     /* pointer to device specific struct */
   DEVDRV_STAT*  pStat;      /* pointer to device/driver statistics */
   DEV_TIMER*    pTimer;     /* pointer to driver specific timer info */
   int           ti;         /* timer interval */

   DRV_LOG (DRV_DEBUG_TIMER, "gei82543TxResoFree...start\n",
            1, 2, 3, 4, 5, 6);

   gei82543TxMblkFree (pDrvCtrl, FREE_ALL_AUTO);

   /* set the internal timer */

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      pBoard = &pDrvCtrl->adaptor;

      if (pBoard->sysGeiDynaTimerSetup != NULL)
      {
         pStat                 = &pBoard->devDrvStat;
         ti                    = pDrvCtrl->timerInterval;

         pStat->rxtxHandlerNum = pDrvCtrl->rxtxHandlerNum / ti;
         pStat->rxIntCount     = pDrvCtrl->rxIntCount / ti;
         pStat->txIntCount     = pDrvCtrl->txIntCount / ti;
         pStat->rxORunIntCount = pDrvCtrl->rxORunIntCount / ti;
         pStat->rxPacketNum    = pDrvCtrl->rxPacketNum / ti;
         pStat->rxPacketDrvErr = pDrvCtrl->rxPacketDrvErr / ti;

         GEI_READ_REG(INTEL_82543GC_TPT, pDrvCtrl->staRegs.tpt);
         pStat->txPacketNum = pDrvCtrl->staRegs.tpt / ti;

         if (pBoard->sysGeiDynaTimerSetup (pBoard))
         {
            pTimer = &pBoard->devTimerUpdate;

            if (!(pTimer->rdtrVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82543GC_RDTR, pTimer->rdtrVal);

            if (!(pTimer->radvVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82546EB_RADV, pTimer->radvVal);

            if (!(pTimer->itrVal & 0xffff0000))
               GEI_WRITE_REG(INTEL_82546EB_ITR, pTimer->itrVal);

            pDrvCtrl->timerInterval = (pTimer->watchDogIntVal <= 0) ?
                                      DEFAULT_TIMER_INTERVAL : (pTimer->watchDogIntVal);
         }

         /* clean up */

         pDrvCtrl->rxtxHandlerNum = 0;
         pDrvCtrl->rxIntCount     = 0;
         pDrvCtrl->txIntCount     = 0;
         pDrvCtrl->rxORunIntCount = 0;
         pDrvCtrl->rxPacketNum    = 0;
         pDrvCtrl->rxPacketDrvErr = 0;
      }
   }

   /* restart timer */

   wdStart (pDrvCtrl->timerId, pDrvCtrl->timerInterval * sysClkRateGet(),
            (FUNCPTR)gei82543EndTimerHandler, (int)pDrvCtrl);

   DRV_LOG (DRV_DEBUG_TIMER, "gei82543TxResoFree...Done\n",
            1, 2, 3, 4, 5, 6);
}

/*****************************************************************************
*
* gei82543EndTimerHandler - timer interrupt handler routine
*
* This routine handle timer interrupts to free TX resources
*
* RETURNS: N/A
*/

LOCAL void gei82543EndTimerHandler (GEI_END_DEVICE*    pDrvCtrl  /* END device structure */
                                    )
{
   DRV_LOG (DRV_DEBUG_TIMER, ("gei82543EndTimerHandler...\n"),
            1, 2, 3, 4, 5, 6);

   if (pDrvCtrl->txDescFreeNum != (pDrvCtrl->txDescNum - 1) ||
       ((pDrvCtrl->adaptor.sysGeiDynaTimerSetup != NULL) &&
        (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
         pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)))
   {
      if ( (netJobAdd ((FUNCPTR)gei82543TxResoFree, (int)pDrvCtrl, 0, 0, 0, 0)) == ERROR)
         DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: TxResoFree\n", 1, 2, 3, 4, 5, 6);
   }
   else
      wdStart (pDrvCtrl->timerId, pDrvCtrl->timerInterval * sysClkRateGet(),
               (FUNCPTR)gei82543EndTimerHandler, (int)pDrvCtrl);

   DRV_LOG (DRV_DEBUG_TIMER, ("gei82543EndTimerHandler...DONE\n"),
            1, 2, 3, 4, 5, 6);
   return;
}

/*************************************************************************
*
* gei82543EndIoctl - the driver I/O control routine
*
* Process an ioctl request.
*
* RETURNS: A command specific response, usually OK or ERROR.
*/

LOCAL int gei82543EndIoctl (GEI_END_DEVICE*    pDrvCtrl, /* device receiving command */
                            int cmd,                     /* ioctl command code */
                            caddr_t data                 /* command argument */
                            )
{
   int  error = 0;
   long value;

   DRV_LOG (DRV_DEBUG_IOCTL, ("IOCTL command begin\n"), 0, 0, 0, 0, 0, 0);

   switch ((UINT)cmd)
   {
      case EIOCSADDR :
         if (data == NULL)
            return (EINVAL);

         bcopy ((char*)data, (char*)GEI_END_HADDR(&pDrvCtrl->end),
                GEI_END_HADDR_LEN(&pDrvCtrl->end));
         break;

      case EIOCGADDR :
         if (data == NULL)
            return (EINVAL);
         bcopy ((char*)GEI_END_HADDR(&pDrvCtrl->end), (char*)data,
                GEI_END_HADDR_LEN(&pDrvCtrl->end));
         break;

      case EIOCSFLAGS :
         value = (long)data;
         if (value < 0)
         {
            value = -value;
            value--;

            END_FLAGS_CLR (&pDrvCtrl->end, value);
         }
         else
            END_FLAGS_SET (&pDrvCtrl->end, value);

         DRV_LOG (DRV_DEBUG_IOCTL, ("endFlag=0x%x\n"),
                  END_FLAGS_GET(&pDrvCtrl->end), 0, 0, 0, 0, 0);

         END_FLAGS_CLR (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);

         gei82543EndConfigure(pDrvCtrl);

         END_FLAGS_SET (&pDrvCtrl->end, IFF_UP | IFF_RUNNING);
         break;

      case EIOCGFLAGS :
         *(int*)data = END_FLAGS_GET(&pDrvCtrl->end);
         break;

      case EIOCPOLLSTART :     /* Begin polled operation */
         gei82543EndPollStart (pDrvCtrl);
         break;

      case EIOCPOLLSTOP :     /* End polled operation */
         gei82543EndPollStop (pDrvCtrl);
         break;

#ifdef ENDMIB2
      case EIOCGMIB2233 :
      case EIOCGMIB2 :

         error = endM2Ioctl (&pDrvCtrl->end, cmd, data);

         break;
#else
      case EIOCGMIB2233 :
         if ((data == NULL) || (pDrvCtrl->end.pMib2Tbl == NULL))
            error = EINVAL;
         else
            *((M2_ID**)data) = pDrvCtrl->end.pMib2Tbl;
         break;
#endif

      case EIOCMULTIADD :
         error = gei82543EndMCastAdd (pDrvCtrl, (char*)data);
         break;

      case EIOCMULTIDEL :
         error = gei82543EndMCastDel (pDrvCtrl, (char*)data);
         break;

      case EIOCMULTIGET :
         error = gei82543EndMCastGet (pDrvCtrl, (MULTI_TABLE*)data);
         break;
#ifdef GEI_CSUM_OFFLOAD
      case EIOCGIFCAP :
      {
         END_CAPABILITIES* pCap = (END_CAPABILITIES*)data;

         if (pCap == NULL)
         {
            error = EINVAL;
            break;
         }

         *pCap = pDrvCtrl->hwCaps;
         break;
      }

      case EIOCSIFCAP :
      {
         END_CAPABILITIES* pCap = (END_CAPABILITIES*)data;

         if (pCap == NULL)
         {
            error = EINVAL;
            break;
         }

         pDrvCtrl->hwCaps.cap_enabled = pCap->cap_enabled;

      }
      break;
#endif
      /*case EIOCGPOLLCONF:
          if ((data == NULL))
              error = EINVAL;
          else
              *((END_IFDRVCONF **)data) = &pDrvCtrl->endStatsConf;
          break;

      case EIOCGPOLLSTATS:
          if ((data == NULL))
              error = EINVAL;
          else
              {
              error = gei82543EndStatsDump(pDrvCtrl);
              if (error == OK)
                  *((END_IFCOUNTERS **)data) = &pDrvCtrl->endStatsCounters;
              }
          break;
       */
      default :
         DRV_LOG (DRV_DEBUG_IOCTL, ("invalid IOCTL command \n"),
                  0, 0, 0, 0, 0, 0);
         error = EINVAL;
   }

   DRV_LOG (DRV_DEBUG_IOCTL, ("IOCTL command Done\n"), 0, 0, 0, 0, 0, 0);

   return (error);
}

/*************************************************************************
*
* gei82543EndConfigure - re-configure chip interface
*
* This routine re-configures the interface such as promisc, multicast
* and broadcast modes
*
* RETURNS: N/A.
*/

LOCAL void gei82543EndConfigure (GEI_END_DEVICE*    pDrvCtrl        /* device to configure */
                                 )
{
   int oldVal;
   int newVal;

   DRV_LOG (DRV_DEBUG_IOCTL, ("gei82543EndConfig...\n"), 1, 2, 3, 4, 5, 6);

   (void)taskLock ();

   /* save flags for later use */

   oldVal = pDrvCtrl->flags;

   newVal = END_FLAGS_GET(&pDrvCtrl->end);

   /* set the driver flag based on END_FLAGS */

   if (newVal & IFF_PROMISC)
      pDrvCtrl->flags |= FLAG_PROMISC_MODE;
   else
      pDrvCtrl->flags &= ~FLAG_PROMISC_MODE;

   if (newVal & IFF_ALLMULTI)
      pDrvCtrl->flags |= FLAG_ALLMULTI_MODE;
   else
      pDrvCtrl->flags &= ~FLAG_ALLMULTI_MODE;

   if (newVal & IFF_MULTICAST)
      pDrvCtrl->flags |= FLAG_MULTICAST_MODE;
   else
      pDrvCtrl->flags &= ~FLAG_MULTICAST_MODE;

   if (newVal & IFF_BROADCAST)
      pDrvCtrl->flags |= FLAG_BROADCAST_MODE;
   else
      pDrvCtrl->flags &= ~FLAG_BROADCAST_MODE;

   if (oldVal == pDrvCtrl->flags)
   {
      (void)taskUnlock ();
      return;
   }

   /* read RX control register */

   GEI_READ_REG(INTEL_82543GC_RCTL, oldVal);

   newVal = oldVal;

   /* prepare new value for RX control register */

   if (pDrvCtrl->flags & FLAG_PROMISC_MODE)
      newVal |= (RCTL_UPE_BIT | RCTL_MPE_BIT | RCTL_BAM_BIT);
   else
      newVal &= ~(RCTL_UPE_BIT | RCTL_MPE_BIT);

   if (pDrvCtrl->flags & FLAG_ALLMULTI_MODE)
      newVal |= RCTL_MPE_BIT;

   else if (!(pDrvCtrl->flags & FLAG_PROMISC_MODE))
      newVal &= ~RCTL_MPE_BIT;

   if (pDrvCtrl->flags & FLAG_BROADCAST_MODE)
      newVal |= RCTL_BAM_BIT;

   else if (!(pDrvCtrl->flags & FLAG_PROMISC_MODE))
      newVal &= ~RCTL_BAM_BIT;

   /* set up RX control register if needed */

   if (newVal != oldVal)
      GEI_WRITE_REG(INTEL_82543GC_RCTL, newVal);

   /* set up Multicasting address */

   if (pDrvCtrl->flags & FLAG_MULTICAST_MODE)
      gei82543AddrFilterSet (pDrvCtrl);
   else
      gei82543McastAdrClean(pDrvCtrl);

   (void)taskUnlock ();

   DRV_LOG (DRV_DEBUG_IOCTL, ("gei82543EndConfig...Done\n"),
            0, 0, 0, 0, 0, 0);
   return;
}

/*************************************************************************
*
* gei82543AddrFilterSet - set the address filter for multicast addresses
*
* This routine goes through all of the multicast addresses on the list
* of addresses (added with the endAddrAdd() routine) and sets the
* device's filter correctly.
*
* RETURNS: N/A.
*/

LOCAL void gei82543AddrFilterSet (GEI_END_DEVICE* pDrvCtrl    /* device to be updated */
                                  )
{
   ETHER_MULTI* pCurr;                       /* multi table address */
   int          count;                       /* number of multi address */
   int          i;                           /* index */
   int          col;                         /* column in MTA */
   int          row;                         /* row in MTA */
   int          tmp;                         /* temporary */
   UINT8        tmpBuf[ETHER_ADDRESS_SIZE];  /* array to save ETH addr */
   UINT16       hashVal;                     /* hash value for MTA */
   UINT32       lowAdr;                      /* adr low in RTA */
   UINT32       highAdr;                     /* adr high in RTA */

   DRV_LOG (DRV_DEBUG_IOCTL, ("gei82543AddrFilterSet...\n"), 0, 0, 0, 0, 0, 0);

   /* clean up all multicasting address in RTA and MTA */

   gei82543McastAdrClean(pDrvCtrl);

   /* get the number of multicasting address */

   if ((count = END_MULTI_LST_CNT(&pDrvCtrl->end)) == 0)
      return;

   if (count > MAX_NUM_MULTI)
   {
      LOGMSG("only accept %d multicast address\n", MAX_NUM_MULTI, 2, 3, 4, 5, 6);
      count =  MAX_NUM_MULTI;
   }

   /*
    * The first 15 multicast will stored in the RAT (RAL/RAH)
    * the rest are stored in MAT
    */

   pCurr = END_MULTI_LST_FIRST (&pDrvCtrl->end);

   /* the first entry (i=0) in RAT is not for multicast addr */

   for (i = 1; i< NUM_RAR && count; i++, count--)
   {
      memcpy (tmpBuf, (char*)&pCurr->addr, ETHER_ADDRESS_SIZE);

      lowAdr = tmpBuf[0] | (tmpBuf[1] << 8) | (tmpBuf[2] << 16) |
               (tmpBuf[3] << 24);

      highAdr = tmpBuf[4] | (tmpBuf[5] << 8);

      GEI_WRITE_REG((INTEL_82543GC_RAL + 8 * i), lowAdr);

      GEI_WRITE_REG((INTEL_82543GC_RAH + 8 * i), (highAdr | RAH_AV_BIT));

      pCurr = END_MULTI_LST_NEXT(pCurr);
   }

   /* configure MTA table if more multicast address remaining */

   while (count--)
   {
      memcpy (tmpBuf, (char*)&pCurr->addr, ETHER_ADDRESS_SIZE);

      /*
       * compose hash value based on filter type :
       * MULTI_FILTER_TYPE_47_36: 47 - 36 bits for dest multicast addr match
       * MULTI_FILTER_TYPE_46_35: 46 - 35 bits for dest multicast addr match
       * MULTI_FILTER_TYPE_45_34: 45 - 34 bits for dest multicast addr match
       * MULTI_FILTER_TYPE_43_32: 43 - 32 bits for dest multicast addr match
       */

      if (pDrvCtrl->multiCastFilterType == MULTI_FILTER_TYPE_47_36)
         hashVal = (tmpBuf[4] >> 4) | ((UINT16)(tmpBuf[5]) << 4);

      else if (pDrvCtrl->multiCastFilterType == MULTI_FILTER_TYPE_46_35)
         hashVal = (tmpBuf[4] >> 3) | ((UINT16)(tmpBuf[5]) << 5);

      else if (pDrvCtrl->multiCastFilterType == MULTI_FILTER_TYPE_45_34)
         hashVal = (tmpBuf[4] >> 2) | ((UINT16)(tmpBuf[5]) << 6 );

      else if (pDrvCtrl->multiCastFilterType == MULTI_FILTER_TYPE_43_32)
         hashVal = tmpBuf[4] | ((UINT16)(tmpBuf[5]) << 8 );

      else
      {
         LOGMSG("Error in compose multicast hash value\n", 1, 2, 3, 4, 5, 6);
         return;
      }

      /* the first 5 bits is the bit location (32 maximum) in a register */

      col = hashVal & 0x1f;

      /* the remaining 7 bits is for the register offset (128 maximum) */

      row = (hashVal >> 5) & 0x7f;

      GEI_READ_REG((INTEL_82543GC_MTA + row * 32), tmp);

      /* find the right bit location */

      tmp |= (1 << col);

      GEI_WRITE_REG((INTEL_82543GC_MTA + row * 32), tmp);

      pCurr = END_MULTI_LST_NEXT(pCurr);
   }

   taskDelay (sysClkRateGet() / 20);

   DRV_LOG (DRV_DEBUG_IOCTL, ("gei82543AddrFilterSet...Done\n"), 0, 0, 0, 0, 0, 0);

   return;
}

/*************************************************************************
*
* gei82543EndMCastAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndMCastAdd (GEI_END_DEVICE*    pDrvCtrl, /* device pointer */
                                  char*          pAddress      /* new address to add */
                                  )
{
   int error;

   if ((error = etherMultiAdd (&pDrvCtrl->end.multiList, pAddress)) ==
       ENETRESET)
   {
      pDrvCtrl->end.nMulti++;

      if (pDrvCtrl->end.nMulti  > MAX_NUM_MULTI)
      {
         pDrvCtrl->end.nMulti--;

         etherMultiDel (&pDrvCtrl->end.multiList, pAddress);

         return ERROR;
      }
      else
         gei82543AddrFilterSet(pDrvCtrl);
   }

   return (OK);
}

/*************************************************************************
*
* gei82543EndMCastDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndMCastDel (GEI_END_DEVICE*     pDrvCtrl, /* device pointer */
                                  char*           pAddress      /* address to be deleted */
                                  )
{
   int error;

   if ((error = etherMultiDel (&pDrvCtrl->end.multiList, (char*)pAddress))
       == ENETRESET)
   {
      gei82543AddrFilterSet(pDrvCtrl);

      pDrvCtrl->end.nMulti--;
   }

   return (OK);
}

/*****************************************************************************
*
* gei82543EndMCastGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndMCastGet (GEI_END_DEVICE*    pDrvCtrl, /* device pointer */
                                  MULTI_TABLE*   pTable        /* address table to be filled in */
                                  )
{
   return (etherMultiGet (&pDrvCtrl->end.multiList, pTable));
}

/*************************************************************************
*
* gei82453EndPollRcv - routine to receive a packet in polling mode.
*
* The caller must pass in a single M_BLK/CL_BLK/cluster tuple with
* the cluster big enough to hold the maximum packet size (1514 bytes).
* If a new received packet is available, it is copied into the
* passed cluster (the copy makes this a low-performance routine).
* As only the WDB agent presently calls this routine, we don't
* set any checksum offload flags for the received packet.
*
* This routine receive a packet in polling mode
*
* RETURNS: OK or EAGAIN.
*/

LOCAL STATUS gei82543EndPollRcv (GEI_END_DEVICE* pDrvCtrl, /* device to be polled */
                                 M_BLK_ID pMblk            /* mBlk to receive */
                                 )
{
   M_BLK* pRxMblk;            /* M_BLK tuple holding received packet */
   char*  pData;              /* receiving packet addr */
   char*  pRxDesc;            /* RX descriptor addr */
   int    len;                /* packet length */
   UINT8  rxStatus;
   STATUS retVal = OK;        /* return value */
   UINT8  rxdErr;

   /*
    * This routine need not pre-invalidate clusters, as it does not
    * dirty them, and copies out of the cluster after invalidating it.
    * As elsewhere in the driver, linkBuf support would be nice.
    */

   DRV_LOG (DRV_DEBUG_POLL_RX, "gei82543EndPollRcv...\n",
            1, 2, 3, 4, 5, 6);

   if (!(pMblk->mBlkHdr.mFlags & M_EXT))
   {
      DRV_LOG (DRV_DEBUG_POLL_RX, "poll receive: bad mblk\n",
               1, 2, 3, 4, 5, 6);
      return (EAGAIN);
   }

   pRxDesc = GEI_GET_RX_DESC_ADDR(pDrvCtrl->rxDescTail);

   /* check RX descriptor status */

   rxStatus = GEI_READ_DESC_BYTE(pRxDesc, RXDESC_STATUS_OFFSET);

   if (!(rxStatus & RXD_STAT_DD))
      return (EAGAIN);

   if (!(rxStatus & RXD_STAT_EOP))
   {
      DRV_LOG (DRV_DEBUG_POLL_RX, "packet EOP not set\n", 1, 2,
               3, 4, 5, 6);
      retVal = EAGAIN;

      goto errorExit;
   }

   rxdErr = GEI_READ_DESC_BYTE(pRxDesc, RXDESC_ERROR_OFFSET);

   /* get the packet length */

   GEI_READ_DESC_WORD(pRxDesc, RXDESC_LENGTH_OFFSET, len);

#ifdef INCLUDE_TBI_COMPATIBLE

   rxdErr &= (UINT8)(~(RXD_ERROR_IPE | RXD_ERROR_TCPE | RXD_ERROR_RSV));

   if (rxdErr)
   {
      BOOL frameAccept = FALSE;

      if (rxdErr == RXD_ERROR_CE && pDrvCtrl->tbiCompatibility)
      {
         UINT8* bufAdr;
         UINT8  lastChar;

         /* software workaround for TBI compatibility problem */

         GEI_READ_DESC_LONG(pRxDesc, RXDESC_BUFADRLOW_OFFSET, (UINT32)bufAdr);
         bufAdr   = GEI_PHYS_TO_VIRT (GEI_BUS_TO_PHYS (bufAdr));

         lastChar = *(bufAdr + len - 1);

         if (lastChar == CARRIER_EXTENSION_BYTE &&
             len > MIN_ETHER_PACKET_SIZE &&
             len <= (pDrvCtrl->mtu + GEI_DEFAULT_ETHERHEADER + 1))
         {
            frameAccept = TRUE;
            rxdErr      = 0;
            len--;
         }
      }

      if (frameAccept != TRUE)
      {
         DRV_LOG (DRV_DEBUG_POLL_RX, "Packet Error 0x%x\n", rxdErr,
                  2, 3, 4, 5, 6);
         LOGMSG("Packet error 0x%x \n", rxdErr, 2, 3, 4, 5, 6);

         goto errorExit;
      }
   }

#else

   if (rxdErr & (UINT8)(~(RXD_ERROR_IPE | RXD_ERROR_TCPE)))
   {
      DRV_LOG (DRV_DEBUG_POLL_RX, "packet error 0x%x\n",
               rxdErr, 2, 3, 4, 5, 6);
      retVal = EAGAIN;

      goto errorExit;
   }

#endif /* INCLUDE_TBI_COMPATIBLE */

   len &= ~0xc000;

   if (len > (pDrvCtrl->mtu + GEI_DEFAULT_ETHERHEADER) ||
       len < MIN_ETHER_PACKET_SIZE)
   {
      DRV_LOG (DRV_DEBUG_POLL_RX, "invalid packet length=0x%x!\n",
               len, 0, 0, 0, 0, 0);
      retVal = EAGAIN;

      goto errorExit;
   }

   /* upper layer must provide a valid buffer. */

   if (pMblk->mBlkHdr.mLen < len)
   {
      DRV_LOG (DRV_DEBUG_POLL_RX, "invalid mbuf length=0x%x!\n",
               pMblk->mBlkHdr.mLen, 0, 0, 0, 0, 0);
      retVal = EAGAIN;

      goto errorExit;
   }

   pRxMblk = pDrvCtrl->pRxMblkArray [pDrvCtrl->rxDescTail];
   pData   = pRxMblk->mBlkHdr.mData;

   /* for cache coherence */

   GEI_END_DRV_CACHE_INVALIDATE ((UINT32)pData, len);

   /* process device packet into net buffer */

   pMblk->m_data += pDrvCtrl->offset;

   bcopy (pData, pMblk->m_data, len);

   pMblk->mBlkHdr.mFlags |= M_PKTHDR;     /* set the packet header */

   pMblk->mBlkHdr.mLen    = len;          /* set the data len */

   pMblk->mBlkPktHdr.len  = len;          /* set the total len */

   DRV_LOG (DRV_DEBUG_POLL_RX, "gei82543EndPollRcv...Done\n",  1, 2, 3,
            4, 5, 6);
   /* fall through */

errorExit:

   /* Reuse old M_BLK tuple */

   gei82543RxDesUpdate(pDrvCtrl);

   return (retVal);
}

/*************************************************************************
*
* gei82543EndPollSend - routine to send a packet in polling mode.
*
* This routine send a packet in polling mode
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS gei82543EndPollSend (GEI_END_DEVICE* pDrvCtrl, /* device ptr */
                                  M_BLK_ID pMblk            /* data to send */
                                  )
{
   int    len;          /* packet length */
   UINT16 txHead;       /* TX descriptor head index */
   UINT16 txTail;       /* TX descriptor tail index */
   UINT32 tmp;          /* temporary variable for register */
   char*  pDesc;        /* descriptor pointer */
   int    tempTail;     /* temp variable for tail */
   BOOL   complete = FALSE;

   DRV_LOG (DRV_DEBUG_POLL_TX, "gei82543EndPollSend...\n", 1, 2, 3, 4, 5, 6);

   /* check if the transmitter idle */

   GEI_READ_REG(INTEL_82543GC_TDH, tmp);

   txHead = (UINT16)tmp;

   GEI_READ_REG(INTEL_82543GC_TDT, tmp);

   txTail = (UINT16)tmp;

   if (txHead != txTail)
   {
      DRV_LOG (DRV_DEBUG_POLL_TX, "gei82543EndPollSend: TX Active\n",
               1, 2, 3, 4, 5, 6);
      return (EAGAIN);
   }

   /* check if any TX descriptor available */

   GEI_GET_TX_DESC_TAIL_UPDATE(tempTail, 1);

   pDesc = GEI_GET_TX_DESC_ADDR(tempTail);

   if (!(GEI_READ_DESC_BYTE(pDesc, TXDESC_STATUS_OFFSET) & TXD_STAT_DD))
   {
      DRV_LOG (DRV_DEBUG_POLL_TX, ("gei82543EndPollSend...no TX descriptor "
                                   "available\n"), 1, 2, 3, 4, 5, 6);
      return (EAGAIN);
   }

   /* Ok, we have TX descriptor */

   pDesc = GEI_GET_TX_DESC_ADDR(pDrvCtrl->txDescTail);

   tmp   = ((UINT32)(pDrvCtrl->pTxPollBufAdr) + (GEI_DESC_ALIGN_BYTE - 1)) &
           ~(GEI_DESC_ALIGN_BYTE - 1);

   GEI_WRITE_DESC_LONG(pDesc, TXDESC_BUFADRLOW_OFFSET,
                       (UINT32)GEI_VIRT_TO_BUS((char*)tmp));

   len = netMblkToBufCopy(pMblk, (char*)tmp, NULL);

   if (len < ETHERSMALL)
      len = ETHERSMALL;

   /* set up the length field */

   GEI_WRITE_DESC_WORD(pDesc, TXDESC_LENGTH_OFFSET, (UINT16)len);

   /* zero the status field in Tx Desc */

   GEI_WRITE_DESC_LONG(pDesc, TXDESC_STATUS_OFFSET, 0);

   /* set up the command field */

   GEI_WRITE_DESC_BYTE(pDesc, TXDESC_CMD_OFFSET,
                       (TXD_CMD_EOP | TXD_CMD_IFCS |
                        TXD_CMD_IDE | TXD_CMD_RS));

   CACHE_PIPE_FLUSH();

   /* update the tail pointer in TDT register */

   GEI_WRITE_REG(INTEL_82543GC_TDT, tempTail);

   /* wait for packet send out */

   while (!complete)
   {
      if ((UINT8)GEI_READ_DESC_BYTE(pDesc, TXDESC_STATUS_OFFSET) &
          TXD_STAT_DD)
         complete = TRUE;
   }

   complete = FALSE;

   /* wait for transmitter idle */

   while (!complete)
   {
      GEI_READ_REG(INTEL_82543GC_TDH, tmp);

      txHead = (UINT16)tmp;

      GEI_READ_REG(INTEL_82543GC_TDT, tmp);

      txTail = (UINT16)tmp;

      if (txHead == txTail)
         complete = TRUE;

   }

   pDrvCtrl->txDescTail = tempTail;

   DRV_LOG (DRV_DEBUG_POLL_TX, "gei82543EndPollSend...done\n",
            1, 2, 3, 4, 5, 6);
   return (OK);
}

/*************************************************************************
*
* gei82543EndPollStart - start polling mode operations
*
* This routine starts the polling mode operation
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndPollStart (GEI_END_DEVICE* pDrvCtrl    /* device to be polled */
                                   )
{
   int oldLevel;

   oldLevel = intLock ();

   /* disable chip interrupt */

   gei82543DisableChipInt(pDrvCtrl);

   /* turn off system interrupts */

   GEI_SYS_INT_DISABLE(pDrvCtrl);

   /* set the polling flag */

   pDrvCtrl->flags |= FLAG_POLLING_MODE;

   intUnlock (oldLevel);

   DRV_LOG (DRV_DEBUG_POLL, "Polling Start...\n", 1, 2, 3, 4, 5, 6);

   return (OK);
}

/*************************************************************************
*
* gei82543EndPollStop - stop polling mode operations
*
* This function terminates polled mode operation.  The device returns to
* interrupt mode. The device interrupts are enabled, the current mode flag is
* switched to indicate interrupt mode and the device is then reconfigured for
* interrupt operation.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndPollStop (GEI_END_DEVICE* pDrvCtrl    /* device to be polled */
                                  )
{
   int oldLevel;

   oldLevel                = intLock ();

   pDrvCtrl->txDescFreeNum = 0;

   gei82543TxDesCleanForce (pDrvCtrl);

   /* update the txDescLastCheck */

   pDrvCtrl->txDescLastCheck = (pDrvCtrl->txDescTail +
                                pDrvCtrl->txDescNum - 1) % (pDrvCtrl->txDescNum);

   pDrvCtrl->flags &= ~FLAG_POLLING_MODE;

   /* clean pending interrupts */

   gei82543DisableChipInt (pDrvCtrl);

   /* turn on system interrupts */

   GEI_SYS_INT_ENABLE(pDrvCtrl);

   /* enable chip interrupt */

   gei82543EnableChipInt (pDrvCtrl);

   intUnlock (oldLevel);

   DRV_LOG (DRV_DEBUG_POLL, "Polling stop ...done\n", 1, 2, 3, 4, 5, 6);

   return (OK);
}

/*************************************************************************
*
* gei82543TxMblkWaitClean - return borrowed mBlk/clDesc
*
* This function returns borrowed mBlk(s) when transmitting can be done
* in 5 seconds. Otherwise, it will forcefully return those mBlk(s).
*
* RETURNS: N/A
*/

LOCAL void gei82543TxMblkWaitClean (GEI_END_DEVICE* pDrvCtrl          /* device to return mBlk(s) */
                                    )
{
   int ix;

   /* wait until rx done */

   for (ix = 50; ix; ix--)
   {
      /* clean up all loaned mbuf */
      if (pDrvCtrl->txDescFreeNum < (pDrvCtrl->txDescNum - 1))
         gei82543TxMblkFree (pDrvCtrl, FREE_ALL_AUTO);

      if (pDrvCtrl->rxtxHandling != TRUE &&
          pDrvCtrl->txDescFreeNum == (pDrvCtrl->txDescNum - 1))
         break;

      taskDelay (sysClkRateGet () / 10);
   }

   if (pDrvCtrl->rxtxHandling)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "pDrvCtrl->rxtxHandling= TRUE%d, \n", 1,
               2, 3, 4, 5, 6);
   }

   /* force free all loaned mBlk */

   if (pDrvCtrl->txDescFreeNum < (pDrvCtrl->txDescNum - 1))
      gei82543TxMblkFree (pDrvCtrl, FREE_ALL_FORCE);

}

/*************************************************************************
*
* gei82543EndStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndStop (GEI_END_DEVICE* pDrvCtrl    /* device to be stopped */
                              )
{
   STATUS result = OK;

   if (pDrvCtrl->attach != TRUE)
      return ERROR;

   /* mark interface down */

   END_FLAGS_CLR (&pDrvCtrl->end, (IFF_UP | IFF_RUNNING));

   /* turn off system interrupts */

   GEI_SYS_INT_DISABLE(pDrvCtrl);

   /* disable TX/RX operation */

   gei82543TxRxDisable (pDrvCtrl);

   gei82543DisableChipInt (pDrvCtrl);

   /* cancel the timer if needed */

   if (pDrvCtrl->timerId)
   {
      wdCancel (pDrvCtrl->timerId);
   }

   /* check device start flag */

   if (pDrvCtrl->devStartFlag != TRUE)
      return OK;

   if (INT_CONTEXT())
   {
      if ( (netJobAdd ((FUNCPTR)gei82543TxMblkWaitClean, (int)pDrvCtrl,
                       0, 0, 0, 0)) == ERROR)
         DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: TxMblkWaitClean\n", 1, 2, 3, 4, 5, 6);

   }
   else
   {
      gei82543TxMblkWaitClean (pDrvCtrl);
   }

   if (pDrvCtrl->pPhyInfo != NULL && pDrvCtrl->cableType == GEI_COPPER_MEDIA)
   {
      if (INT_CONTEXT())
      {
         if ( (netJobAdd ((FUNCPTR)miiPhyUnInit, (int)(pDrvCtrl->pPhyInfo),
                          0, 0, 0, 0)) == ERROR)
            DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: miiPhyUnInit\n", 1, 2, 3, 4, 5, 6);
         if ( (netJobAdd ((FUNCPTR)cfree, (int)(pDrvCtrl->pPhyInfo),
                          0, 0, 0, 0)) == ERROR)
            DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: cfree\n", 1, 2, 3, 4, 5, 6);
      }
      else
      {
         if (miiPhyUnInit (pDrvCtrl->pPhyInfo) != OK)
         {
            LOGMSG("miiPhyUnInit fails\n", 0, 0, 0, 0, 0, 0);
         }
         cfree ((char*)pDrvCtrl->pPhyInfo);
      }
   }

   /* disconnect interrupt handler */

   GEI_SYS_INT_DISCONNECT(pDrvCtrl, gei82543EndInt, (int)pDrvCtrl, &result);

   if (result == ERROR)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Could not disconnect interrupt!\n", 1, 2,
               3, 4, 5, 6);
   }

   pDrvCtrl->devStartFlag = FALSE;

   return (result);
}

/*************************************************************************
*
* gei82543EndUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS gei82543EndUnload (GEI_END_DEVICE* pDrvCtrl    /* device to be unloaded */
                                )
{
   /* stop the device if not yet */

   if (pDrvCtrl->devStartFlag != FALSE)
   {
      gei82543EndStop (pDrvCtrl);
   }
   /*
    * netJob queue may still have some unfinished work for this device, so
    * inserting the "unload" function after those unfinished work.
    * Warning - It's not a good idea to place much burden on
    * tNetTask since it may be the only vehicle to pass packets (including
    * other interfaces) to the high layer.
    */

   if (netTaskId != ERROR && netTaskId != 0)
   {
      if ( (netJobAdd ((FUNCPTR)gei8254xNowUnload, (int)pDrvCtrl,
                       0, 0, 0, 0)) == ERROR)
         DRV_LOGMSG (DRV_DEBUG_INTD, "netJobAdd Error: NowUnload\n", 1, 2, 3, 4, 5, 6);
   }
   else
      gei8254xNowUnload (pDrvCtrl);

   return OK;
}

/*******************************************************************
* gei8254xNowUnload - free memory when unload the device
*
* This function will free all memory allocated by this devices;
* It is called when gei82543EndUnload is involved
*
* RETURN: N/A
*/

LOCAL void gei8254xNowUnload (GEI_END_DEVICE* pDrvCtrl    /* device to be unloaded */
                              )
{
   gei8254xRxTupleFree (pDrvCtrl);

   if (pDrvCtrl->timerId)
   {
      wdDelete (pDrvCtrl->timerId);
      pDrvCtrl->timerId = NULL;
   }

   if ( pDrvCtrl->end.protocols != NULL )
   {
      END_OBJECT_UNLOAD (&pDrvCtrl->end);
   }

   /* free allocated memory */

   pDrvCtrl->attach = FALSE;

   gei82543MemAllFree (pDrvCtrl);
}

/**************************************************************************
* gei8254xRxTupleFree - free all RX tuple
*
* This function frees all RX tuples
*
* RETURN: n/a
*/
LOCAL void gei8254xRxTupleFree (GEI_END_DEVICE* pDrvCtrl
                                )
{
   int ix;

   for (ix = 0; ix < pDrvCtrl->rxDescNum; ix++)
   {
      if (pDrvCtrl->pRxMblkArray[ix] != NULL)
         netMblkClChainFree (pDrvCtrl->pRxMblkArray[ix]);
   }
}

/**************************************************************************
*
* gei82543TxSetup - configure and setup TX for running
*
* This routine sets up the data structure for TX and configures transmitter
*
* RETURNS: N/A
*/

LOCAL void gei82543TxSetup (GEI_END_DEVICE* pDrvCtrl    /* device to set up TX descriptors */
                            )
{
   UINT32 tctlVal;           /* transmit register value */
   int    ix;

   /* initialization for TX operation*/

   pDrvCtrl->txDescTail      = 0;
   pDrvCtrl->txStall         = FALSE;
   pDrvCtrl->txDescFreeNum   = pDrvCtrl->txDescNum - 1;
   pDrvCtrl->txDescLastCheck = pDrvCtrl->txDescNum - 1;

   /* carve the transmit buffer memory */

   for (ix = 0; ix < pDrvCtrl->txDescNum; ix++)
   {
      P_TX_DESCTL pDescCtl;

      pDescCtl       = pDrvCtrl->pTxDesCtlBase + ix;

      pDescCtl->mBlk = NULL;
   }

   /* pre-init the transmit buffer memory */

   for (ix = 0; ix < pDrvCtrl->txDescNum; ix++)
   {
      char* pTxDesc;

      pTxDesc = GEI_GET_TX_DESC_ADDR(ix);

      /* set up the command field */

      GEI_WRITE_DESC_BYTE(pTxDesc, TXDESC_CMD_OFFSET,
                          (TXD_CMD_EOP | TXD_CMD_IFCS |
                           TXD_CMD_IDE | TXD_CMD_RS));

      /* set DD bits in STATUS field */

      GEI_WRITE_DESC_BYTE(pTxDesc, TXDESC_STATUS_OFFSET, TXD_STAT_DD);

   }

   /* set the TX descriptor BASE register */

   GEI_WRITE_REG(INTEL_82543GC_TDBAL,
                 (UINT32)GEI_VIRT_TO_BUS((void*)(pDrvCtrl->pTxDescBase)));
   GEI_WRITE_REG(INTEL_82543GC_TDBAH, 0);

   /* set the length field */

   GEI_WRITE_REG(INTEL_82543GC_TDLEN, (TXDESC_SIZE * pDrvCtrl->txDescNum));

   /* initialize tail and head registers */

   GEI_WRITE_REG(INTEL_82543GC_TDH, 0);
   GEI_WRITE_REG(INTEL_82543GC_TDT, 0);

   /* Set up the interrupt delay for Transmit*/

   GEI_WRITE_REG(INTEL_82543GC_TIDV, pDrvCtrl->txIntDelay);

   /* Set up LWTHRESH */

   if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      UINT32 txdctlVal;      /* TXDCTL register value */
      UINT32 count;          /* temp var */

      GEI_READ_REG(INTEL_82543GC_TXDCTL, txdctlVal);

      count     = pDrvCtrl->txDescNum / 8 - 1;

      txdctlVal = ((txdctlVal & 0x1ffffff) | (count << 25));

      if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         txdctlVal |= TXDCTL_COUNT_DESC_BIT;

      GEI_WRITE_REG(INTEL_82543GC_TXDCTL, txdctlVal);
   }

   /* set up the IPG register */

   if (pDrvCtrl->cableType ==  GEI_FIBER_MEDIA)
   {
      GEI_WRITE_REG(INTEL_82543GC_TIPG, DEFAULT_TIPG_IPGT_F
                    | (DEFAULT_TIPG_IPGR1 << TIPG_IPGR1_SHIFT)
                    | (DEFAULT_TIPG_IPGR2 << TIPG_IPGR2_SHIFT));

      if (pDrvCtrl->duplex == FULL_DUPLEX_MODE)
         GEI_WRITE_REG(INTEL_82543GC_TCTL, (TCTL_PSP_BIT
                                            | (TX_COLLISION_THRESHOLD << TCLT_CT_SHIFT)
                                            | (FDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT)));
      else
         GEI_WRITE_REG(INTEL_82543GC_TCTL, (TCTL_PSP_BIT
                                            | (TX_COLLISION_THRESHOLD << TCLT_CT_SHIFT)
                                            | (HDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT)));
   }

   else if (pDrvCtrl->cableType ==  GEI_COPPER_MEDIA)
   {
      GEI_WRITE_REG(INTEL_82543GC_TIPG, DEFAULT_TIPG_IPGT_T
                    | (DEFAULT_TIPG_IPGR1 << TIPG_IPGR1_SHIFT)
                    | (DEFAULT_TIPG_IPGR2 << TIPG_IPGR2_SHIFT));

      GEI_READ_REG(INTEL_82543GC_TCTL, tctlVal);

      if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         tctlVal |= TCTL_MULR_BIT;

      GEI_WRITE_REG(INTEL_82543GC_TCTL, tctlVal | (TCTL_PSP_BIT
                                                   | (TX_COLLISION_THRESHOLD << TCLT_CT_SHIFT)));
   }

   CACHE_PIPE_FLUSH();

   return;
}

/*************************************************************************
*
* gei82543RxSetup - configure and setup RX for running
*
* This routine sets up the data structure for RX and configures transmitter
*
* RETURNS: N/A
*/

LOCAL void gei82543RxSetup (GEI_END_DEVICE* pDrvCtrl      /* device to do RX descriptor setting */
                            )
{
   int    ix;                  /* index */
   UINT32 rctl = 0;            /* rctl register value */

   /* initialize the SW/HW for RX */

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndStart: set up RX structure\n",
            1, 2, 3, 4, 5, 6);
   pDrvCtrl->rxDescTail   = 0;

   pDrvCtrl->rxtxHandling = FALSE;

   /* hook up receive buffers to clusters in netpool */

   for (ix = 0; ix < pDrvCtrl->rxDescNum; ix++)
   {
      char*  pTempRxDesc = GEI_GET_RX_DESC_ADDR(ix);
      char*  pTempBuf;
      M_BLK* pMblk       = GEI_TUPLE_GET (pDrvCtrl->end.pNetPool);

      if (pMblk == NULL)
      {
         DRV_LOG (DRV_DEBUG_LOAD, "gei82543EndMemInit: Could not get a "
                  "buffer\n", 1, 2, 3, 4, 5, 6);
         return;
      }

      pDrvCtrl->pRxMblkArray [ix] = pMblk;

      pTempBuf                    = pMblk->mBlkHdr.mData;

      GEI_END_DRV_CACHE_INVALIDATE ((UINT32)pTempBuf, pDrvCtrl->rxPktSz);

      pTempBuf            += pDrvCtrl->offset;

      pMblk->mBlkHdr.mData = pTempBuf;

      GEI_WRITE_DESC_LONG(pTempRxDesc, RXDESC_BUFADRLOW_OFFSET,
                          (UINT32)GEI_VIRT_TO_BUS(pTempBuf));

      GEI_WRITE_DESC_LONG(pTempRxDesc, RXDESC_BUFADRHIGH_OFFSET, 0);

      /* set up STATUS field */

      GEI_WRITE_DESC_BYTE(pTempRxDesc, RXDESC_STATUS_OFFSET, 0);

   }

   GEI_WRITE_REG(INTEL_82543GC_RDTR, (pDrvCtrl->rxIntDelay | RDTR_FPD_BIT));

   /* set up BASE register */

   GEI_WRITE_REG(INTEL_82543GC_RDBAL,
                 (UINT32)GEI_VIRT_TO_BUS((void*)(pDrvCtrl->pRxDescBase)));

   GEI_WRITE_REG(INTEL_82543GC_RDBAH, 0);

   /* set up the LENGTH register*/

   GEI_WRITE_REG(INTEL_82543GC_RDLEN, RXDESC_SIZE * (pDrvCtrl->rxDescNum));

   /* set up head/tail registers */

   GEI_WRITE_REG(INTEL_82543GC_RDH, 0);

   GEI_WRITE_REG(INTEL_82543GC_RDT, (pDrvCtrl->rxDescNum - 1));

   /* set up rctl register */

   rctl = (pDrvCtrl->multiCastFilterType << RCTL_MO_SHIFT);

   if (pDrvCtrl->usrFlags & GEI_END_JUMBO_FRAME_SUPPORT)
   {
      rctl |= RCTL_LPE_BIT;

      /* equal transmit and receive storage in FIFO for jumbo frames */

      if (pDrvCtrl->mtu > ETHERMTU)
      {
         GEI_WRITE_REG(INTEL_82543GC_PBA, 0x20);    /* 24KB for RX/TX buf */
      }
   }

   if (pDrvCtrl->rxBufSize == 2048)
      rctl |= RCTL_BSIZE_2048;
   else if (pDrvCtrl->rxBufSize == 4096)
      rctl |= (RCTL_BSIZE_4096 | RCTL_BSEX_BIT);
   else if (pDrvCtrl->rxBufSize == 8192)
      rctl |= (RCTL_BSIZE_8192 | RCTL_BSEX_BIT);
   else if (pDrvCtrl->rxBufSize == 16384)
      rctl |= (RCTL_BSIZE_16384 | RCTL_BSEX_BIT);
   else
   {
      LOGMSG ("Error: gei82543End.c : invalid rxBufSize =%d\n",
              pDrvCtrl->rxBufSize, 2, 3, 4, 5, 6);
      /* in case */
      rctl |= RCTL_BSIZE_2048;
   }

#ifdef INCLUDE_TBI_COMPATIBLE
   if (pDrvCtrl->tbiCompatibility)
      rctl |= RCTL_SBP_BIT;
#endif /* INCLUDE_TBI_COMPATIBLE */

   rctl |= RCTL_SECRC_BIT;

   GEI_WRITE_REG(INTEL_82543GC_RCTL, rctl);

#ifdef GEI_CSUM_OFFLOAD
   /*
    * At a hardware level, we always enable all capabilities, so
    * that we don't have to reset the device for an administrative
    * change. We simply don't use the info the MAC gives us on
    * receive if some of our RX capabilities are administratively
    * disabled.
    *
    * Turn on RX checksum offload. The 82543 has a bug which
    * prevents us from using both TCP/UDP and IP checksums at
    * the same time, so for that chip we enable TCP/UDP only.
    *
    * Since the packet-level checksum Transport/IPv6 RX checksum
    * offload workaround is used for all chip variants, we need
    * to set the packet-level checksum starting offset for all
    * variants (SPR #113209).
    */

   if (pDrvCtrl->adaptor.boardType != PRO1000_543_BOARD)
   {
      UINT32 rxcsum = (RXCSUM_TUOFL_BIT |
                       RXCSUM_IPOFL_BIT |
                       GEI_RXCSUM_PKT_CSUM_OFF);

      /*
       * We use the packet checksum only for IPv6.
       * The IPV6OFL bit seems hardwired to 0 on an 82546.
       */
      if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
          pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         rxcsum |= RXCSUM_IPV6OFL_BIT;

      GEI_WRITE_REG(INTEL_82543GC_RXCSUM, rxcsum);
   }
   else
   {
      GEI_WRITE_REG(INTEL_82543GC_RXCSUM,
                    RXCSUM_TUOFL_BIT | GEI_RXCSUM_PKT_CSUM_OFF);
   }
#endif

   CACHE_PIPE_FLUSH();

   return;
}

/*************************************************************************
*
* gei82543HwInit - Initialize 82543 MAC chip
*
* This routine initializes MAC and set up link
*
* RETURN: OK if success
*/

LOCAL STATUS gei82543HwInit (GEI_END_DEVICE* pDrvCtrl
                             )
{
   /* clean up all receive address */

   gei82543AllRxAdrClean (pDrvCtrl);

   /* clean up all MTA registers */

   gei82543AllMtaAdrClean (pDrvCtrl);

   /* clean up VLAN stuff */

   gei82543AllVlanClean (pDrvCtrl);

   /* set up the IA for receive */

   gei82543EtherRxAdrSet (pDrvCtrl, pDrvCtrl->adaptor.enetAddr, 0);

   return (gei82543linkInit (pDrvCtrl));
}

/*************************************************************************
*
* gei82543linkInit - set up link for device operation
*
* This routine gets the configuration parameters from eeprom and configure
* device for linking
*
* RETURNS: OK if setup success
*/

LOCAL STATUS gei82543linkInit (GEI_END_DEVICE* pDrvCtrl
                               )
{
   STATUS status = OK;
   UINT32 tmp;
   UINT32 tclRegVal;
   UINT32 eepromWord = 0;
   UINT32 extDevCtrlRegVal;
   UINT32 staReg;

   /* check copper or fiber based adapter */

   GEI_READ_REG(INTEL_82543GC_STATUS, staReg);

   if (staReg & STATUS_TBIMODE_BIT)
   {
      pDrvCtrl->cableType = GEI_FIBER_MEDIA;
   }
   else
   {
      pDrvCtrl->cableType = GEI_COPPER_MEDIA;
   }

   pDrvCtrl->devCtrlRegVal = 0;

   /* compose control register value based on EEPROM config value */
   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      eepromWord              = (UINT32)pDrvCtrl->adaptor.eeprom_icw1;

      tmp                     = (eepromWord & EEPROM_ICW1_SWDPIO_BITS) >> EEPROM_ICW1_SWDPIO_SHIFT;
      pDrvCtrl->devCtrlRegVal = tmp << CTRL_SWDPIOLO_SHIFT;
   }

   tmp                      = (eepromWord & EEPROM_ICW1_ILOS_BIT) >> EEPROM_ICW1_ILOS_SHIFT;
   pDrvCtrl->devCtrlRegVal |= (tmp << CTRL_ILOS_SHIFT);

   /* get the duplex configuration in EEPROM */

   if (pDrvCtrl->duplex == DUPLEX_HW)
   {
      if (eepromWord & EEPROM_ICW1_FRCSPD_BIT)
         pDrvCtrl->duplex = FULL_DUPLEX_MODE;
      else
         pDrvCtrl->duplex = HALF_DUPLEX_MODE;
   }

   /* get the flow control value in EEPROM */

   eepromWord = (UINT32)pDrvCtrl->adaptor.eeprom_icw2;

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_HW)
   {
      if ((eepromWord & EEPROM_ICW2_PAUSE_BITS) == 0)
         pDrvCtrl->flowCtrl = FLOW_CONTRL_NONE;

      else if ((eepromWord & EEPROM_ICW2_PAUSE_BITS) == EEPROM_ICW2_ASM_DIR)
         pDrvCtrl->flowCtrl = FLOW_CONTRL_TRANSMIT;

      else
         pDrvCtrl->flowCtrl = FLOW_CONTRL_ALL;
   }

   DRV_LOG (DRV_DEBUG_LOAD, "gei82543SetLinkFlowCtrl: pDrvCtrl->flowCtrl= "
            "%d\n", pDrvCtrl->flowCtrl, 2, 3, 4, 5, 6);

   /* program the extend control register */

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      tmp = (eepromWord & EEPROM_ICW2_SWDPIO_EXE_BITS) >>
            EEPROM_ICW2_SWDPIO_EXE_SHIFT;

      extDevCtrlRegVal = tmp << CTRL_EXT_SWDPIOHI_SHIFT;

      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, extDevCtrlRegVal);
   }

   /* program the transmit control register */

   GEI_READ_REG(INTEL_82543GC_TCTL, tclRegVal);

   tclRegVal |= (FDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT);

   GEI_WRITE_REG(INTEL_82543GC_TCTL, tclRegVal);

   /* set user's preference of DMA configuration */

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {

      if (pDrvCtrl->dmaPriority == DMA_RX_PRIORITY)
      {
         pDrvCtrl->devCtrlRegVal &= ~CTRL_PRIOR_BIT;
      }
      else if (pDrvCtrl->dmaPriority == DMA_FAIR_RX_TX)
      {
         pDrvCtrl->devCtrlRegVal |= CTRL_PRIOR_BIT;
      }
   }
   else
   {
      pDrvCtrl->devCtrlRegVal &= ~CTRL_PRIOR_BIT;
   }

   /* establish a link */

   if (pDrvCtrl->cableType == GEI_FIBER_MEDIA)
      status = gei82543linkTBISetup (pDrvCtrl, TRUE);

   else if (pDrvCtrl->cableType == GEI_COPPER_MEDIA)
      status = gei82543linkGMIISetup (pDrvCtrl);

   else
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Do not recognize media type\n",
               1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   pDrvCtrl->linkInitStatus = status;

   if (pDrvCtrl->linkInitStatus != OK)
   {
      DRV_LOG (DRV_DEBUG_LOAD, ("gei82543EndStart: link setup fails\n"),
               1, 2, 3, 4, 5, 6);
      LOGMSG("link fails\n", 1, 2, 3, 4, 5, 6);
   }

   return status;
}

/*************************************************************************
*
* gei82543linkTBISetup - set up link to partner in TBI mode
*
* This routine set the link with partner in TBI mode
*
* RETURNS: OK if link success
*/

LOCAL STATUS gei82543linkTBISetup (GEI_END_DEVICE*     pDrvCtrl,
                                   BOOL firstTime /* TRUE for the first time calling */
                                   )
{
   UINT32 devStatus = 0;
   UINT32 intSet;
   STATUS status    = ERROR;

   /* only TBI mode used*/

   GEI_READ_REG(INTEL_82543GC_STATUS, devStatus);

   if (!(devStatus & STATUS_TBIMODE_BIT))
   {
      LOGMSG("The adaptor is not configured in TBI mode\n",  1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   /* disable chip interrupts */

   intSet = gei82543DisableChipInt (pDrvCtrl);

   /* set up txConfigure Word */

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_NONE)
      pDrvCtrl->txConfigureWord = (TXCW_ANE_BIT | TXCW_FD_BIT);

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_TRANSMIT)
      pDrvCtrl->txConfigureWord = (TXCW_ANE_BIT | TXCW_FD_BIT |
                                   TXCW_ASM_DIR);
   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL  ||
            pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE)
      pDrvCtrl->txConfigureWord = (TXCW_ANE_BIT | TXCW_FD_BIT |
                                   TXCW_PAUSE_BITS);
   else
   {
      LOGMSG("Flow control parameter invalid\n", 1, 2, 3, 4, 5, 6);
      return ERROR;
   }

   status = gei82543TBIHwAutoNegotiate (pDrvCtrl, TRUE, firstTime);

   if (status == ERROR &&
       gei82543TBIlinkForce (pDrvCtrl, TRUE, FALSE) == OK)
   {
      UINT32 rxcwRegVal;
      UINT32 ctrlRegVal;

      status = OK;

      GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

      GEI_READ_REG(INTEL_82543GC_RXCW, rxcwRegVal);

      if ((ctrlRegVal & CTRL_SLU_BIT) && (rxcwRegVal & RXCW_C_BIT))
      {
         status = gei82543TBIHwAutoNegotiate (pDrvCtrl, TRUE, FALSE);

         if (status != OK)
         {
            DRV_LOG (DRV_DEBUG_LOAD, "Force link OK and receive RXCW; "
                     "But HW auto Fail\n", 1, 2, 3, 4, 5, 6);
            /* force link again */

            status = gei82543TBIlinkForce (pDrvCtrl, TRUE, FALSE);
         }
      }
   }

   /* one more try */

   if (status == ERROR)
      status = gei82543TBIHwAutoNegotiate (pDrvCtrl, TRUE, FALSE);

   pDrvCtrl->linkStatus = (status == OK) ? LINK_STATUS_OK : LINK_STATUS_ERROR;

   if (status == OK)
   {
      gei82543DuplexGet (pDrvCtrl);

      gei82543SpeedGet (pDrvCtrl);

      gei82543FlowCtrlRegsSet(pDrvCtrl);

      LOGMSG("Link Establish OK. \n", 1, 2, 3, 4, 5, 6);
   }
   else
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Link setup fail", 1, 2, 3, 4, 5, 6);
   }

   if (firstTime != TRUE)
      GEI_WRITE_REG(INTEL_82543GC_IMS, intSet);

   return status;
}

/*************************************************************************
*
* gei82543TBIHwAutoNegotiate - link with hardware auto-negotiation
*
* This routine starts hardware auto-negotiation process
*
* RETURN: OK, or ERROR if link fail
*/

LOCAL STATUS gei82543TBIHwAutoNegotiate (GEI_END_DEVICE* pDrvCtrl,
                                         BOOL waitStatus,
                                         BOOL firstTime
                                         )
{
   UINT32 regVal;
   int    ix = LINK_TIMEOUT_IN_QUAR_SEC;

   /* ack all interrupts again */

   GEI_READ_REG(INTEL_82543GC_ICR, regVal);

   pDrvCtrl->linkStatus = LINK_STATUS_UNKNOWN;

   /* disable RXCFG interrupt */

   if (pDrvCtrl->linkMethod == GEI82543_HW_AUTO)
      GEI_WRITE_REG(INTEL_82543GC_IMC, IMC_RXCFG_BIT);

   /* set up TXCW, start negotiation process */

   GEI_WRITE_REG(INTEL_82543GC_TXCW, pDrvCtrl->txConfigureWord);

   if (firstTime)
      GEI_WRITE_REG(INTEL_82543GC_CTRL, pDrvCtrl->devCtrlRegVal);
   else
   {
      GEI_READ_REG(INTEL_82543GC_CTRL, regVal);

      GEI_WRITE_REG(INTEL_82543GC_CTRL, (regVal & ~CTRL_SLU_BIT));
   }

   pDrvCtrl->linkMethod = GEI82543_HW_AUTO;

   if (waitStatus == FALSE)
      return OK;

   do
   {
      /* wait... */
      taskDelay(sysClkRateGet() / 4);

      if (gei82543linkStatusCheck (pDrvCtrl) == OK)
         return OK;

   } while (ix--);

   return ERROR;
}

/****************************************************************************
*
* gei82543TBIlinkForce - proceed force link
*
* This routine tries to link partner manually
*
* RETURNS: OK, or ERROR if fail
*/

LOCAL STATUS gei82543TBIlinkForce (GEI_END_DEVICE* pDrvCtrl, /* device to force link */
                                   BOOL waitStatus,          /* indicator to wait status */
                                   BOOL rxcfgEnable          /* indicator to enable RXCFG interrupt */
                                   )
{
   int    ix = LINK_TIMEOUT_IN_QUAR_SEC;
   UINT32 statusRegVal;          /* status register value */
   UINT32 rxcwRegVal;            /* RXCW register value */
   UINT32 ctrlRegVal;            /* control register value */
   UINT32 regVal;

   /* ack all interrupts again */

   GEI_READ_REG(INTEL_82543GC_ICR, regVal);

   pDrvCtrl->linkStatus = LINK_STATUS_UNKNOWN;

   /* read status register */

   GEI_READ_REG(INTEL_82543GC_STATUS, statusRegVal);

   /* read receive control register */

   GEI_READ_REG(INTEL_82543GC_RXCW, rxcwRegVal);

   if ((statusRegVal & STATUS_LU_BIT) || (rxcwRegVal & RXCW_C_BIT))
      return ERROR;

   DRV_LOG (DRV_DEBUG_LOAD, "Force Link... \n", 1, 2, 3, 4, 5, 6);

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   /* full duplex is assumed */

   ctrlRegVal |= (CTRL_SLU_BIT | CTRL_FD_BIT);

   /* disable HW auto-negotiation */

   GEI_WRITE_REG(INTEL_82543GC_TXCW, pDrvCtrl->txConfigureWord &
                 ~TXCW_ANE_BIT);

   /* enable RXCFG interrupt if needed*/

   if (rxcfgEnable == TRUE)
      GEI_WRITE_REG(INTEL_82543GC_IMS, IMS_RXCFG_BIT);

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   pDrvCtrl->linkMethod = GEI82543_FORCE_LINK;

   /* read control register for later use */

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_NONE)
      ctrlRegVal &= ~(CTRL_TFCE_BIT | CTRL_RFCE_BIT);

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_TRANSMIT)
   {
      ctrlRegVal &= ~CTRL_RFCE_BIT;
      ctrlRegVal |=  CTRL_TFCE_BIT;
   }

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE)
   {
      ctrlRegVal &= ~CTRL_TFCE_BIT;
      ctrlRegVal |=  CTRL_RFCE_BIT;
   }

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL )
      ctrlRegVal |= (CTRL_TFCE_BIT | CTRL_RFCE_BIT);

   else
   {
      LOGMSG("invalid flow control parameters\n", 1, 2, 3, 4, 5, 6);

      return ERROR;
   }

   /* force link ... */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   if (waitStatus == FALSE)
      return OK;

   /* check status */

   do
   {
      /* wait... */
      taskDelay(sysClkRateGet() / 4);

      if (gei82543linkStatusCheck (pDrvCtrl) == OK)
         return OK;

   } while (ix--);

   return ERROR;
}

/*************************************************************************
*
* gei82543linkGMIISetup - link set up with PHY device
*
* This routine sets up link with PHY device
*
* RETURN: OK if link succeed
*/

LOCAL STATUS gei82543linkGMIISetup (GEI_END_DEVICE* pDrvCtrl       /* driver structure */
                                    )
{
   STATUS status = ERROR;

   /* pre-init for PHY's GMII */

   if ((status = gei82543linkGMIIPreInit(pDrvCtrl)) != OK )
   {
      DRV_LOG (DRV_DEBUG_LOAD, ("Failed to pre-initialize PHY\n"),
               0, 0, 0, 0, 0, 0);
      return ERROR;
   }

   pDrvCtrl->pPhyInfo->phyFlags = pDrvCtrl->phyInitFlags;

   /* perform BSP specific initialization */

   if (pDrvCtrl->adaptor.phyBspPreInit != NULL)
      pDrvCtrl->adaptor.phyBspPreInit (pDrvCtrl->pPhyInfo,
                                       pDrvCtrl->pPhyInfo->phyAddr);

   /* call miiLib's functions */

   if (miiPhyInit (pDrvCtrl->pPhyInfo) != OK)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "fail to initialize PHY\n", 0, 0, 0, 0, 0, 0);
      return (ERROR);
   }

   /* perform vendor specific register initialization */

   pDrvCtrl->adaptor.phySpecInit (pDrvCtrl->pPhyInfo,
                                  pDrvCtrl->pPhyInfo->phyAddr);

   /* configure device after establishing a link */

   gei82543GMIIphyConfig (pDrvCtrl);

   return OK;
}

/*************************************************************************
*
* gei82543GMIIphyConfig - set up registers after PHY link done
*
* This routine configures chip after link is established. It gets the link
* results from PHY, and configure the device based on that.
*
* RETURNS: N/A
*/

LOCAL void gei82543GMIIphyConfig (GEI_END_DEVICE*    pDrvCtrl   /* device to config GMII */
                                  )
{
   UINT32 ctrlRegVal;          /* control register value */
   UINT32 tctlRegVal;          /* transmit ctrl reg value */
   UINT32 speed;               /* link speed */
   UINT32 duplex;              /* link duplex mode */

   /* get the speed setting from PHY */

   speed = pDrvCtrl->pPhyInfo->phySpeed;

   /* get the duplex setting from PHY */

   if (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_FD)
      duplex = FULL_DUPLEX_MODE;
   else
      duplex = HALF_DUPLEX_MODE;

   /* get the flow control setting from PHY */

   if ((pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_TX_FLOW_CTRL) &&
       (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_RX_FLOW_CTRL))
      pDrvCtrl->flowCtrl = FLOW_CONTRL_ALL;

   else if (!(pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_TX_FLOW_CTRL) &&
            (pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_RX_FLOW_CTRL))
      pDrvCtrl->flowCtrl = FLOW_CONTRL_RECEIVE;

   else if ((pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_TX_FLOW_CTRL) &&
            !(pDrvCtrl->pPhyInfo->phyFlags & MII_PHY_RX_FLOW_CTRL))
      pDrvCtrl->flowCtrl = FLOW_CONTRL_TRANSMIT;
   else
      pDrvCtrl->flowCtrl = FLOW_CONTRL_NONE;

   /* prepare re-configure general/transmit control register */

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   ctrlRegVal |= (CTRL_FRCSPD_BIT | CTRL_FRCDPX_BIT);

   ctrlRegVal &= ~(CTRL_SPEED_MASK | CTRL_ILOS_BIT );

   GEI_READ_REG(INTEL_82543GC_TCTL, tctlRegVal);

   tctlRegVal &= ~TCTL_COLD_BIT;

   /* configure duplex information */

   if (duplex == FULL_DUPLEX_MODE)
   {
      ctrlRegVal |= CTRL_FD_BIT;
      tctlRegVal |= (FDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT);
   }
   else  /* half duplex mode */
   {
      ctrlRegVal &= ~CTRL_FD_BIT;

      if (speed == MII_1000MBS)
         tctlRegVal |= (BIG_HDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT) |
                       TCTL_PBE_BIT;
      else
         tctlRegVal |= (HDX_COLLISION_DISTANCE << TCTL_COLD_SHIFT);
   }

   /* configure speed info */
   /*Ampro: Added to test speed*/
   if (speed == MII_1000MBS)
   {
      ctrlRegVal |= CTRL_SPD_1000_BIT;
      /*printf ("\n--------- 1000 MBS---------\n");*/
   }
   else if (speed == MII_100MBS)
   {
      ctrlRegVal |= CTRL_SPD_100_BIT;
      /*printf ("\n--------- 100 MBS---------\n");*/
   }
   else
   {
      ctrlRegVal &= ~(CTRL_SPD_1000_BIT | CTRL_SPD_100_BIT);
      /*printf ("\n--------- 10 MBS---------\n");*/
   }

   /* no flow control with half duplex mode */

   if (duplex == HALF_DUPLEX_MODE)
      pDrvCtrl->flowCtrl = FLOW_CONTRL_NONE;

   /* configure flow control */

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_NONE)
      ctrlRegVal &= ~(CTRL_TFCE_BIT | CTRL_RFCE_BIT);

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_TRANSMIT)
   {
      ctrlRegVal &= ~CTRL_RFCE_BIT;
      ctrlRegVal |=  CTRL_TFCE_BIT;
   }

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE)
   {
      ctrlRegVal &= ~CTRL_TFCE_BIT;
      ctrlRegVal |=  CTRL_RFCE_BIT;
   }

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL)
      ctrlRegVal |= (CTRL_TFCE_BIT | CTRL_RFCE_BIT);

   /* re-program general/transmit control registers */

   GEI_WRITE_REG(INTEL_82543GC_TCTL, tctlRegVal);

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   /* program flow control accordingly */

   gei82543FlowCtrlRegsSet(pDrvCtrl);

#ifdef INCLUDE_TBI_COMPATIBLE
   {
      UINT32 rctlVal;

      /*
       * TBI compatibility bug occurred only in :
       * 82543 based silicon, and
       * copper-based media, and
       * link partner is advertised only 1000Base-T capability.
       * we currently can not determine whether the link partner also
       * supports other capabilities except 1000Base-T, thus we will
       * enable workaround as the link partner has the 1000Base-T capability.
       */

      GEI_READ_REG(INTEL_82543GC_RCTL, rctlVal);

      if ((pDrvCtrl->usrFlags & GEI_END_TBI_COMPATIBILITY) &&
          (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD) &&
          (speed == MII_1000MBS))
      {
         pDrvCtrl->tbiCompatibility = TRUE;

         GEI_WRITE_REG(INTEL_82543GC_RCTL, (rctlVal | RCTL_SBP_BIT));
      }
      else
      {
         pDrvCtrl->tbiCompatibility = FALSE;

         GEI_WRITE_REG(INTEL_82543GC_RCTL, (rctlVal & ~RCTL_SBP_BIT));
      }
   }

   CACHE_PIPE_FLUSH ();

#endif /* INCLUDE_TBI_COMPATIBLE */

   /* set the speed/duplex in the GEI_END_DEVICE structure */

   gei82543DuplexGet (pDrvCtrl);

   gei82543SpeedGet (pDrvCtrl);

   return;
}

/*************************************************************************
*
* gei82543GMIIphyReConfig - re-configure 82543 chip after link is restarted
*
* This routine re-configures PHY chip after link is restarted and established.
* It gets the link results from PHY, and configure the device based on that.
*
* NOTE: *pPhyInfo should have a valid PHY bus number (phyAddr); Its phyFlags
*       should be the one we configured the PHY for Auto-negotiation last time.
*
* RETURNS: N/A
*/

LOCAL void gei82543GMIIphyReConfig (GEI_END_DEVICE*    pDrvCtrl    /* device to do PHY checking */
                                    )
{

   /* in case that the first time initialization fails */

   if (pDrvCtrl->linkInitStatus != OK)
   {
      pDrvCtrl->linkInitStatus = gei82543linkGMIISetup (pDrvCtrl);
      return;
   }

   /* we don't support force link case */

   if (pDrvCtrl->pPhyInfo->phyLinkMethod != MII_PHY_LINK_AUTO)
      return;

   /* restore phyFlags with the flags for Auto-Negotiation */

   pDrvCtrl->pPhyInfo->phyFlags = pDrvCtrl->pPhyInfo->phyAutoNegotiateFlags;

   /* check/update MII auto-negotiation results */

   miiAnCheck (pDrvCtrl->pPhyInfo, pDrvCtrl->pPhyInfo->phyAddr);

   /* configure 82543 chip again */

   gei82543GMIIphyConfig (pDrvCtrl);

}

/*************************************************************************
*
* gei82543linkGMIIPreInit - pre-init PHY for GMII link setup
*
* This routine resets PHY devices through 82543 chip, set up PHY_INFO
* structure
*
* RETURN: OK or ERROR
*/

LOCAL STATUS gei82543linkGMIIPreInit (GEI_END_DEVICE* pDrvCtrl   /* device to do GMII pre-init */
                                      )
{
   UINT32 extCtrlRegVal;    /* extended control reg value */

   /*
    * We will establish a link through PHY, and then manually
    * configure the 82543 chip based on PHY's negotiation results
    */

   pDrvCtrl->devCtrlRegVal |= (CTRL_FRCSPD_BIT | CTRL_FRCDPX_BIT |
                               CTRL_SLU_BIT);

   GEI_WRITE_REG(INTEL_82543GC_CTRL, (pDrvCtrl->devCtrlRegVal));

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      /*
       * SDP_4 pin is also wired to PHY_RST, assert SDP_4(low)
       * could reset PHY device
       */
      /*  reset the PHY device from 82543 chip */

      GEI_READ_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      extCtrlRegVal |= CTRL_PHY_RESET_DIR4_BIT;
      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      taskDelay(sysClkRateGet() / 10);

      GEI_READ_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      extCtrlRegVal &= ~CTRL_PHY_RESET4_BIT;
      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      taskDelay(sysClkRateGet() / 10);

      GEI_READ_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      extCtrlRegVal |= CTRL_PHY_RESET4_BIT;
      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, extCtrlRegVal);
      taskDelay(sysClkRateGet() / 10);
   }
   else if (pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      GEI_WRITE_REG(INTEL_82543GC_CTRL,
                    (pDrvCtrl->devCtrlRegVal) | CTRL_PHY_RST_BIT);

      /* wait PHY settle down */
      taskDelay (sysClkRateGet () / 10);
      GEI_WRITE_REG(INTEL_82543GC_CTRL, (pDrvCtrl->devCtrlRegVal));
      taskDelay (sysClkRateGet () / 10);
   }

   /* allocate memory for PHY_INFO structure */

   if ((pDrvCtrl->pPhyInfo = calloc (sizeof (PHY_INFO), 1)) == NULL)
      return (ERROR);

   /* clean up PHY_INFO structure */

   memset (pDrvCtrl->pPhyInfo, 0, sizeof (PHY_INFO));

   /* set up phyInfo structure */

   pDrvCtrl->pPhyInfo->pDrvCtrl = (void*)pDrvCtrl;
   pDrvCtrl->pPhyInfo->phyAddr  = MII_PHY_DEF_ADDR;

   /* PHY's read/write operation routine */

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      pDrvCtrl->pPhyInfo->phyWriteRtn = (FUNCPTR)gei82543PhyWrite;
      pDrvCtrl->pPhyInfo->phyReadRtn  = (FUNCPTR)gei82543PhyRead;
   }
   else if (pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      pDrvCtrl->pPhyInfo->phyWriteRtn = (FUNCPTR)gei82544PhyWrite;
      pDrvCtrl->pPhyInfo->phyReadRtn  = (FUNCPTR)gei82544PhyRead;
   }

   if (pDrvCtrl->adaptor.phyAddr > 0 &&
       pDrvCtrl->adaptor.phyAddr < MII_MAX_PHY_NUM)
      pDrvCtrl->pPhyInfo->phyAddr = pDrvCtrl->adaptor.phyAddr;
   else
      pDrvCtrl->pPhyInfo->phyAddr = 0;

   if (pDrvCtrl->adaptor.phyDelayRtn != (FUNCPTR)NULL)
   {
      pDrvCtrl->pPhyInfo->phyDelayRtn  =
         (FUNCPTR)(pDrvCtrl->adaptor.phyDelayRtn);
      pDrvCtrl->pPhyInfo->phyMaxDelay  =
         pDrvCtrl->adaptor.phyMaxDelay;
      pDrvCtrl->pPhyInfo->phyDelayParm =
         pDrvCtrl->adaptor.phyDelayParm;
   }
   else
   {
      pDrvCtrl->pPhyInfo->phyDelayRtn  = (FUNCPTR)taskDelay;
      pDrvCtrl->pPhyInfo->phyMaxDelay  = 8 * sysClkRateGet() / 5; /* 8 sec */
      pDrvCtrl->pPhyInfo->phyDelayParm = 5;
   }

   pDrvCtrl->pPhyInfo->phyAnOrderTbl  = NULL;
   pDrvCtrl->pPhyInfo->phyLinkDownRtn = (FUNCPTR)NULL;

   /*
    * All hardware capabilities will be advertised, and let auto-negotiation
    * process figures out the max common ability; half duplex 1000T mode is NOT
    * supported in the 82543-based adapter(Intel 82543 spec update Rev 0.5)
    */
   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      pDrvCtrl->pPhyInfo->phyFlags = GEI_MII_PHY_CAP_FLAGS;   /* ability flags */
   }
   else if (pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      pDrvCtrl->pPhyInfo->phyFlags = GEI_MII_PHY_CAP_FLAGS | MII_PHY_1000T_HD;
   }

   if (pDrvCtrl->adaptor.phyType == GEI_PHY_GMII_TYPE)
      pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_GMII_TYPE;   /* GMII type */
   else
      pDrvCtrl->pPhyInfo->phyFlags &= ~MII_PHY_GMII_TYPE;

   pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_AUTO;  /* allow auto-negotiation */

   /* set the flow control bits in phyFlags */

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_NONE )
      pDrvCtrl->pPhyInfo->phyFlags &= ~(MII_PHY_TX_FLOW_CTRL |
                                        MII_PHY_RX_FLOW_CTRL);

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_TRANSMIT)
   {
      pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_TX_FLOW_CTRL;
      pDrvCtrl->pPhyInfo->phyFlags &= ~MII_PHY_RX_FLOW_CTRL;
   }

   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL ||
            pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE)
      pDrvCtrl->pPhyInfo->phyFlags |= (MII_PHY_TX_FLOW_CTRL |
                                       MII_PHY_RX_FLOW_CTRL);
   else
   {
      LOGMSG(" invalid flow control parameter\n", 1, 2, 3, 4, 5, 6);
   }

   /* mark phyflags to indicated pre-initialization */

   pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_PRE_INIT;

#ifdef MII_PHY_ISO_UNAVAIL
   /*
    * The miiLib code tries to test the functionality
    * of the PHY by setting the isolate bit and seeing if
    * it succeeds. Unfortunately, it looks like the PHY
    * in the 82573 series cards doesn't like being isolated
    * and will stop responding once it is. This upsets the
    * miiLib code, which decides the PHY is broken and aborts
    * the PHY setup. It doesn't make sense to isolate the
    * PHY in this case anyway since it's embedded and not
    * sharing access to the MAC wit another PHY. In VxWorks 6,
    * you can use the MII_PHY_ISO_UNAVAIL flag to tell the
    * miiLib code to skip the isolation step, but this flag
    * isn't available on VxWorks 5.5.x. If we're running
    * on that OS, we use a workaround where the PhyRead
    * and PhyWrite routines trap the attempt to frob the
    * isolate bit in the control register and lie to miiLib,
    * making it think the isolation succeeded when in fact
    * the PHY was left alone.
    */

   if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
      pDrvCtrl->pPhyInfo->phyFlags |= MII_PHY_ISO_UNAVAIL;
#endif

   /* save flags for later reference */

   pDrvCtrl->phyInitFlags = pDrvCtrl->pPhyInfo->phyFlags;

   return OK;
}

/***************************************************************************
*
* gei82543EtherRxAdrSet - set up the first entry on Receive Address Table
*
* This routine sets up the MAC address
*
* RETURNS: N/A
*/

LOCAL void gei82543EtherRxAdrSet (GEI_END_DEVICE* pDrvCtrl,
                                  UINT8 adr[],
                                  int index
                                  )
{
   UINT32 ral = 0;
   UINT32 rah = 0;
   UINT32 reg;

   /* split the address to RAH/RAL */

   ral = (adr[0] | (adr[1] << 8) | (adr[2] << 16) | (adr[3] << 24) );

   rah = (adr[4] | (adr[5] << 8) | (RAH_AV_BIT));

   reg = INTEL_82543GC_RAL + index * 8;

   GEI_WRITE_REG(reg, ral);

   reg = INTEL_82543GC_RAH + index * 8;

   GEI_WRITE_REG(reg, rah);

   return;
}

/*************************************************************************
*
* gei82543Delay - delay function to be used for PHY read/write operation
*
* This routine delays for a specified time in the unit of ns
*
* RETURN: N/A
*/

LOCAL void gei82543Delay (GEI_END_DEVICE*     pDrvCtrl,
                          UINT32 timeDelay /* in unit of ns */
                          )
{
   int count;

   /*
    * we hope BSP can provide a delay routine with higher time resolution.
    * if using taskDelay(), it probably results in a slower initialization
    * process for hardware, such as MII/GMII read/write operations
    */

   if (pDrvCtrl->adaptor.delayFunc == NULL ||
       pDrvCtrl->adaptor.delayUnit == 0)
   {
      /* convert to ticks */

      count = (sysClkRateGet () * timeDelay / 1000000) / 1000 + 1;

      taskDelay (count);

      return;
   }

   count = (timeDelay / (pDrvCtrl->adaptor.delayUnit)) + 1;

   for ( ; count > 0; count--)
   {
      (FUNCPTR)pDrvCtrl->adaptor.delayFunc ();
   }

   return;
}

/**************************************************************************
*
* gei82543Reset - reset 82543/4/5/6 chips
*
* This routine resets 82543/4/5/6 chips
*
* RETURNS: N/A
*/

LOCAL void gei82543Reset (GEI_END_DEVICE* pDrvCtrl
                          )
{
   UINT32 tmp, ix;

   /* disable all interrupt */

   gei82543DisableChipInt (pDrvCtrl);

   /* disable/clean RX CTL */

   GEI_WRITE_REG(INTEL_82543GC_RCTL, 0);

   /* disable/clean TX CTL */

   GEI_WRITE_REG(INTEL_82543GC_TCTL, 0);

   /* Acquire MDIO ownership. */

   if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      GEI_READ_REG(INTEL_82543GC_EXTCNF_CTRL, tmp);
      tmp |= EXTCNF_CTRL_MDIO_SW_BIT;
      for (ix = 0; ix < 1000; ix++)
      {
         GEI_WRITE_REG(INTEL_82543GC_EXTCNF_CTRL, tmp);
         GEI_READ_REG(INTEL_82543GC_EXTCNF_CTRL, tmp);
         if (tmp & EXTCNF_CTRL_MDIO_SW_BIT)
            break;
         else
            tmp |= EXTCNF_CTRL_MDIO_SW_BIT;
         taskDelay (1);
      }
   }

   /* reset the chip */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, CTRL_RST_BIT);

   /* wait hardware ready */

   taskDelay (sysClkRateGet() / 6);

   /* clean up CTRL_EXT for 82543/82544 MACs */

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD)
   {
      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, 0);
   }

   /* Force a reload from EEPROM */

   if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      GEI_READ_REG(INTEL_82543GC_CTRL_EXT, tmp);
      tmp |= CTRL_EXT_EE_RST_BIT;
      GEI_WRITE_REG(INTEL_82543GC_CTRL_EXT, tmp);
      CACHE_PIPE_FLUSH();
      for (ix = 0; ix < 1000; ix++)
      {
         GEI_READ_REG(INTEL_82543GC_EECD, tmp);
         if (tmp & EECD_AUTORD_DONE_BIT)
            break;
         taskDelay(1);
      }
   }

   /* clean all pending interrupts */

   gei82543DisableChipInt (pDrvCtrl);

   return;
}

/****************************************************************************
*
* gei82543AllVlanClean - cleans up all VLAN table entry
*
* This routine cleans up all entries in the VLAN table
*
* RETURNS: N/A
*/

LOCAL void gei82543AllVlanClean (GEI_END_DEVICE*    pDrvCtrl
                                 )
{
   UINT32 ix;
   UINT32 reg;

   GEI_WRITE_REG(INTEL_82543GC_VET, 0x0);

   for (ix = 0; ix < NUM_VLAN; ix++)
   {
      reg = INTEL_82543GC_VLAN + ix * 4;

      GEI_WRITE_REG(reg, 0);
   }

   return;
}

/****************************************************************************
*
* gei82543McastAdrClean - clean up all Multicasting address
*
* This routine cleans up all entries in MTA table and Multicasting Address in
* RTA table
*
* RETURNS: N/A
*/

LOCAL void gei82543McastAdrClean (GEI_END_DEVICE*    pDrvCtrl
                                  )
{
   UINT32 ix;
   UINT32 regVal;

   for (ix = 1; ix < NUM_RAR; ix++)
   {
      GEI_READ_REG((INTEL_82543GC_RAL + ix * 8), regVal);

      /* check multicast address */

      if (regVal & 0x1)
      {
         GEI_WRITE_REG((INTEL_82543GC_RAL + ix * 8), 0);
         GEI_WRITE_REG((INTEL_82543GC_RAH + ix * 8), 0);
      }
   }

   /* clean up MTA entries */

   gei82543AllMtaAdrClean (pDrvCtrl);

   return;
}

/****************************************************************************
*
* gei82543AllMtaAdrClean - clean up all MTA table entries
*
* This routine cleans up all entries in MTA table.
*
* RETURNS: N/A
*/

LOCAL void gei82543AllMtaAdrClean (GEI_END_DEVICE*    pDrvCtrl            /* device to clean up MTA table */
                                   )
{
   UINT32 ix;                           /* index */
   UINT32 reg;                          /* register offset */

   /* clean up MTA */

   for (ix = 0; ix < NUM_MTA; ix++)
   {
      reg = INTEL_82543GC_MTA + ix * 4;

      GEI_WRITE_REG(reg, 0);
   }
   return;
}

/****************************************************************************
*
* gei82543AllRxAdrClean - clean up all entry in receive address table
*
* This routine clean up receive address table (RTA)
*
* RETURNS: N/A
*/

LOCAL void gei82543AllRxAdrClean (GEI_END_DEVICE* pDrvCtrl   /* device to clean up RAT */
                                  )
{
   int    ix;
   UINT32 regAdr;          /* register offset */

   /* clean up RTA */

   for (ix = 0; ix < NUM_RAR; ix++)
   {
      regAdr = INTEL_82543GC_RAL + ix * 8;

      GEI_WRITE_REG(regAdr, 0);

      regAdr = INTEL_82543GC_RAH + ix * 8;

      GEI_WRITE_REG(regAdr, 0);
   }

   return;
}

/****************************************************************************
*
* gei82543FlowCtrlRegsSet - set up the flow control registers
*
* This routine set up the flow control registers
*
* RETURNS: N/A
*/

LOCAL void gei82543FlowCtrlRegsSet (GEI_END_DEVICE*   pDrvCtrl    /* device to set flow control register */
                                    )
{
   /* set up flow control registers */

   GEI_WRITE_REG(INTEL_82543GC_FCAL, FLOW_CONTROL_LOW_ADR);

   GEI_WRITE_REG(INTEL_82543GC_FCAH, FLOW_CONTROL_HIGH_ADR);

   GEI_WRITE_REG(INTEL_82543GC_FCT, FLOW_CONTROL_TYPE);

   GEI_WRITE_REG(INTEL_82543GC_FCTTV, FLOW_CONTROL_TIMER_VALUE);

   if ((pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE) ||
       (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL) )
   {
      GEI_WRITE_REG(INTEL_82543GC_FCRTL,
                    FCRTL_XONE_BIT | FLOW_CONTROL_LOW_THRESH);
      GEI_WRITE_REG(INTEL_82543GC_FCRTH, FLOW_CONTROL_HIGH_THRESH);
   }
   else
   {
      GEI_WRITE_REG(INTEL_82543GC_FCRTL, 0);

      GEI_WRITE_REG(INTEL_82543GC_FCRTH, 0);
   }

   return;
}

/*************************************************************************
*
* gei82543SpeedGet - get the speed from device
*
* This routine get the speed from device control register
*
* RETURN: N/A
*/

LOCAL void gei82543SpeedGet (GEI_END_DEVICE* pDrvCtrl      /* device to get speed */
                             )
{
   UINT32 val;                 /* register value */

   if (pDrvCtrl->cableType == GEI_FIBER_MEDIA)
   {
      /* only 1000Mbs supported */

      pDrvCtrl->speed = END_SPEED_1000M;

      return;
   }

   else if (pDrvCtrl->cableType == GEI_COPPER_MEDIA)

   {
      /* get status register */

      GEI_READ_REG(INTEL_82543GC_STATUS, val);

      if (val & STATUS_SPEED_1000_BIT)
         pDrvCtrl->speed = END_SPEED_1000M;

      else if (val & STATUS_SPEED_100_BIT)
         pDrvCtrl->speed = END_SPEED_100M;

      else
         pDrvCtrl->speed = END_SPEED_10M;
   }

   else
      pDrvCtrl->speed = (UINT32)(UNKNOWN);

   return;
}

/*************************************************************************
*
* gei82543DuplexGet - get the duplex mode
*
* This routine gets duplex mode from device status register
*
* RETURN: N/A
*/

LOCAL void gei82543DuplexGet (GEI_END_DEVICE* pDrvCtrl       /* device to get duplex */
                              )
{
   UINT32 val;                  /* register value */

   GEI_READ_REG(INTEL_82543GC_STATUS, val);

   if (val & STATUS_FD_BIT)
      pDrvCtrl->duplex = FULL_DUPLEX_MODE;

   else
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Half duplex mode\n...",  1, 2, 3, 4, 5, 6);

      pDrvCtrl->duplex = HALF_DUPLEX_MODE;
   }

   return;
}

/*************************************************************************
*
* gei82543linkStatusCheck - check link status
*
* This routine checks link status
*
* RETURN: OK or ERROR
*/

LOCAL STATUS gei82543linkStatusCheck (GEI_END_DEVICE* pDrvCtrl        /* device to check link status */
                                      )
{
   register UINT32 ctrlVal;      /* control register value */
   register UINT32 statusVal;    /* status register value */

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlVal);

   GEI_READ_REG(INTEL_82543GC_STATUS, statusVal);

   if (pDrvCtrl->cableType == GEI_COPPER_MEDIA ||
       pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
       pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      if (statusVal & STATUS_LU_BIT)
         return OK;
   }
   else  /* FIBER media and 82543 based adaptor */
   {
      if (!(ctrlVal & CTRL_SWDPIN1_BIT) && (statusVal & STATUS_LU_BIT))
         return OK;
   }

   return ERROR;
}

/*************************************************************************
*
* gei82543TxRxDisable - disable TX/RX of MAC
*
* This routine disable TX/RX
*
* RETURN: N/A
*/

LOCAL void gei82543TxRxDisable (GEI_END_DEVICE* pDrvCtrl        /* driver to disable TX/RX */
                                )
{
   UINT32 regVal;               /* register value */

   /* disable chip RX/TX */

   GEI_READ_REG(INTEL_82543GC_RCTL, regVal);
   GEI_WRITE_REG(INTEL_82543GC_RCTL, (regVal & ~RCTL_EN_BIT));

   GEI_READ_REG(INTEL_82543GC_TCTL, regVal);
   GEI_WRITE_REG(INTEL_82543GC_TCTL, (regVal & ~TCTL_EN_BIT));
}

/*************************************************************************
*
* gei82543TxRxEnable - Enable TX/RX operation
*
* This routine enable TX/RX operation
*
* RETURN: N/A
*/

LOCAL void gei82543TxRxEnable (GEI_END_DEVICE* pDrvCtrl      /* driver to enable TX/RX */
                               )
{
   UINT32 regVal;              /* register value */

   /* enable chip RX/TX */

   GEI_READ_REG(INTEL_82543GC_RCTL, regVal);
   GEI_WRITE_REG(INTEL_82543GC_RCTL, (regVal | RCTL_EN_BIT));

   GEI_READ_REG(INTEL_82543GC_TCTL, regVal);
   GEI_WRITE_REG(INTEL_82543GC_TCTL, (regVal | TCTL_EN_BIT));

   return;
}

/*************************************************************************
*
* gei82543EnableChipInt - enable 82543 chip interrupt
*
* This routine enables 82543 chip interrupt
*
* RETURNS: N/A
*/

LOCAL void gei82543EnableChipInt (GEI_END_DEVICE* pDrvCtrl            /* enable chip interrupt */
                                  )
{
   UINT32 intSet;                    /* interrupt setting */

   if (pDrvCtrl->linkMethod == GEI82543_FORCE_LINK)
      intSet = (AVAIL_RX_TX_INT | AVAIL_LINK_INT | INT_RXCFG_BIT);
   else
      intSet = (AVAIL_RX_TX_INT | AVAIL_LINK_INT );

   GEI_WRITE_REG(INTEL_82543GC_IMS, intSet);

   return;
}

/*************************************************************************
*
* gei82543DisableChipInt - disable 82543 chip interrupt
*
* This routine disables 82543 chip interrupts
*
* RETURNS: Interrupt setting before disable
*/

LOCAL UINT32 gei82543DisableChipInt (GEI_END_DEVICE* pDrvCtrl        /* driver structure */
                                     )
{
   volatile UINT32 intSet;       /* interrupt setting before */
   volatile UINT32 intIcr;       /* ICR reg value */

   GEI_READ_REG(INTEL_82543GC_IMS, intSet);

   GEI_WRITE_REG(0xdc, 0);
   GEI_WRITE_REG(INTEL_82543GC_IMC, IMC_ALL_BITS);

   /* read back icr to clean up all pending interrupts */

   GEI_READ_REG(INTEL_82543GC_ICR, intIcr);

   return intSet;
}

/*************************************************************************
*
* gei82543miiReadData - shift data in from MDIO pin
*
* This routine shifts data in from MDIO pin.
*
* RETURN: data from MDIO
*/

LOCAL UINT16 gei82543miiReadData (GEI_END_DEVICE* pDrvCtrl       /* driver structure */
                                  )
{
   volatile UINT32 ctrlRegVal;  /* control register value */
   UINT16          data = 0;    /* read-back data */
   UINT8           i;           /* index */

   /* clean up */

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   ctrlRegVal &= ~(CTRL_MDIO_DIR_BIT | CTRL_MDIO_BIT);

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   /* raise clk */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal | CTRL_MDC_BIT));

   gei82543Delay (pDrvCtrl, 1000);

   /* lower clk */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal & ~CTRL_MDC_BIT));

   gei82543Delay (pDrvCtrl, 1000);

   /* read data which is always 16 bits */

   for (i = 0; i < 16; i++)
   {
      /* raise clk */

      GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal | CTRL_MDC_BIT));

      gei82543Delay (pDrvCtrl, 1000);

      GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

      data = (data << 1) | ((ctrlRegVal & CTRL_MDIO_BIT) ? 1 : 0);

      /* lower clk */

      GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal & ~CTRL_MDC_BIT));

      gei82543Delay (pDrvCtrl, 1000);
   }

   /* raise clk */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal | CTRL_MDC_BIT));

   gei82543Delay (pDrvCtrl, 1000);

   /* lower clk */

   GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal & ~CTRL_MDC_BIT));

   gei82543Delay (pDrvCtrl, 1000);

   return (data);
}

/****************************************************************************
*
* gei82543miiWriteData - shift the data out to MDIO pin
*
* This routine shifts data out to MDIO pin.
*
* RETURN: N/A
*/

LOCAL void gei82543miiWriteData (GEI_END_DEVICE* pDrvCtrl, /* driver structure */
                                 UINT32 data,              /* data to shift */
                                 UINT32 numBits            /* number of bits to shift */
                                 )
{
   volatile UINT32 ctrlRegVal;        /* control register value */
   UINT32          mask;              /* mask bits */
   int             ix;                /* index */

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   ctrlRegVal |= (CTRL_MDIO_DIR_BIT | CTRL_MDC_DIR_BIT);

   for (ix = numBits - 1 ; ix >= 0; ix--)
   {
      mask       = 1 << ix;

      ctrlRegVal = (data & mask) ? (ctrlRegVal | CTRL_MDIO_BIT) :
                   (ctrlRegVal & ~CTRL_MDIO_BIT);
      GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

      gei82543Delay (pDrvCtrl, 1000);

      GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal | CTRL_MDC_BIT));

      gei82543Delay (pDrvCtrl, 1000);

      GEI_WRITE_REG(INTEL_82543GC_CTRL, (ctrlRegVal & ~CTRL_MDC_BIT));

      gei82543Delay (pDrvCtrl, 1000);
   }

   return;
}

/*************************************************************************
*
* gei82543PhyRead - read MII register
*
* This routine performs reading of MII registers in PHY devices
*
* RETURN: OK if read correct
*/

LOCAL STATUS gei82543PhyRead (GEI_END_DEVICE* pDrvCtrl, /* device structure */
                              UINT8 phyAddr,            /* PHY device bus number  */
                              UINT8 phyReg,             /* register offset to read */
                              UINT16* data              /* return data address */
                              )
{
   UINT32 cmd;                    /* command data */
   UINT32 val;                    /* read-back data */

   /*
    * compose cmd in the reverse order because the most significant
    * bits of phyAddr and phyReg should be shifted out first
    */

   cmd = (phyReg | (phyAddr << 5) | (PHY_RD_OP << 10) | (PHY_MARK) << 12);

   /* shift the PREAMBLE bits out first */

   gei82543miiWriteData (pDrvCtrl, PHY_PREAMBLE, PHY_PREAMBLE_SIZE);

   /* shift the command out */

   gei82543miiWriteData (pDrvCtrl, cmd, 14);

   /* read return register value */

   val = gei82543miiReadData(pDrvCtrl);

   if (val & MDI_ERR_BIT)
      return ERROR;

   *data = (UINT16)val;

   return OK;
}

/*************************************************************************
*
* gei82543PhyWrite - write MII registers in PHY
*
* This routine performs write operation to MII registers in a PHY device
*
* RETURN: OK
*/

LOCAL STATUS gei82543PhyWrite (GEI_END_DEVICE* pDrvCtrl, /* driver structure */
                               UINT8 phyAddr,            /* PHY bus number */
                               UINT8 phyReg,             /* PHY's register to write */
                               UINT16 data               /* data to write */
                               )
{
   UINT32 cmd;      /* command */

   cmd = (data | (PHY_TURNAR << 16) | (phyReg << 18 ) |
          (phyAddr << 23) | (PHY_WR_OP << 28) | (PHY_MARK) << 30) ;

   gei82543miiWriteData (pDrvCtrl, PHY_PREAMBLE, PHY_PREAMBLE_SIZE);

   gei82543miiWriteData (pDrvCtrl, cmd, 32);

   return OK;
}

/*************************************************************************
*
* gei82544PhyRead - read MII register of 82544 chip
*
* This routine performs reading of MII registers in the internal PHY
*
* RETURN: OK if read correct
*/

LOCAL STATUS gei82544PhyRead (GEI_END_DEVICE* pDrvCtrl, /* device structure */
                              UINT8 phyAddr,            /* PHY device bus number  */
                              UINT8 phyReg,             /* register offset to read */
                              UINT16* data              /* return data address */
                              )
{
   int             count = 20;     /* counter */
   volatile UINT32 mdicRegVal;

#ifndef MII_PHY_ISO_UNAVAIL
   if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      if (phyReg == MII_CTRL_REG && pDrvCtrl->phyIsolate == TRUE)
      {
         *data = MII_CR_ISOLATE;
         return (OK);
      }
   }
#endif

   mdicRegVal = (MDI_READ_BIT | phyAddr << MDI_PHY_SHIFT |
                 phyReg << MDI_REG_SHIFT);

   GEI_WRITE_REG(INTEL_82543GC_MDI, mdicRegVal);

   gei82543Delay (pDrvCtrl, 32000);  /* wait 32 microseconds */

   while (count--)                   /* wait max 96 microseconds */
   {
      GEI_READ_REG(INTEL_82543GC_MDI, mdicRegVal);

      if (mdicRegVal & MDI_READY_BIT)
         break;

      gei82543Delay (pDrvCtrl, 16000);
   }

   if ((mdicRegVal & (MDI_READY_BIT | MDI_ERR_BIT)) != MDI_READY_BIT)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Error: MII read PhyAddr=%d, phyReg=%d\n...",
               phyAddr, phyReg, 3, 4, 5, 6);
   }

   *data = (UINT16)mdicRegVal;

   return OK;
}

/*************************************************************************
*
* gei82544PhyWrite - write MII registers in the internal PHY of 82544
*
* This routine performs write operation to MII registers in the internal
* PHY device
*
* RETURN: OK if success
*/

LOCAL STATUS gei82544PhyWrite (GEI_END_DEVICE* pDrvCtrl, /* driver structure */
                               UINT8 phyAddr,            /* PHY bus number */
                               UINT8 phyReg,             /* PHY's register to write */
                               UINT16 data               /* data to write */
                               )
{
   int             count = 20; /* counter */
   volatile UINT32 mdicRegVal;

#ifndef MII_PHY_ISO_UNAVAIL
   /*
    * Trap attempts to set the isolate bit in the control
    * register and lie about setting it. Sometimes, when you
    * set the bit, reading the control register afterwards
    * shows the bit as clear, which confuses miiLib since
    * it expects to see it set.
    */
   if (pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      if (phyReg == MII_CTRL_REG && (data & MII_CR_ISOLATE))
      {
         pDrvCtrl->phyIsolate = TRUE;
         return (OK);
      }
      else
         pDrvCtrl->phyIsolate = FALSE;
   }
#endif

   mdicRegVal = (MDI_WRITE_BIT | phyAddr << MDI_PHY_SHIFT |
                 phyReg << MDI_REG_SHIFT | data);

   GEI_WRITE_REG(INTEL_82543GC_MDI, mdicRegVal);

   gei82543Delay (pDrvCtrl, 32000);  /* wait 32 microseconds */

   while (count--)                   /* wait max 96 microseconds */
   {
      GEI_READ_REG(INTEL_82543GC_MDI, mdicRegVal);

      if (mdicRegVal & MDI_READY_BIT)
         break;

      gei82543Delay (pDrvCtrl, 16000);
   }

   if ((mdicRegVal & (MDI_READY_BIT | MDI_ERR_BIT)) != MDI_READY_BIT)
   {
      DRV_LOG (DRV_DEBUG_LOAD, "Error: MII write PhyAddr=%d, phyReg=%d\n...",
               phyAddr, phyReg, 3, 4, 5, 6);
   }

   return OK;
}

/**************************************************************************
*
* gei82543HwStatusDump - get and clean up statistic registers
*
* This routine gets/cleans up all statistic registers
*
* RETURNS: N/A
*/

LOCAL void gei82543HwStatusDump (GEI_END_DEVICE* pDrvCtrl      /* device to dump statistic registers */
                                 )
{
   GEI_READ_REG(INTEL_82543GC_CRCERRS, pDrvCtrl->staRegs.crcerrs);
   GEI_READ_REG(INTEL_82543GC_ALGNERRC, pDrvCtrl->staRegs.algnerrc);
   GEI_READ_REG(INTEL_82543GC_SYMERRS,  pDrvCtrl->staRegs.symerrs);
   GEI_READ_REG(INTEL_82543GC_RXERRC,   pDrvCtrl->staRegs.rxerrc);
   GEI_READ_REG(INTEL_82543GC_MPC, pDrvCtrl->staRegs.mpc);
   GEI_READ_REG(INTEL_82543GC_SCC, pDrvCtrl->staRegs.scc);
   GEI_READ_REG(INTEL_82543GC_ECOL, pDrvCtrl->staRegs.ecol);
   GEI_READ_REG(INTEL_82543GC_MCC, pDrvCtrl->staRegs.mcc);
   GEI_READ_REG(INTEL_82543GC_LATECOL, pDrvCtrl->staRegs.latecol);
   GEI_READ_REG(INTEL_82543GC_COLC, pDrvCtrl->staRegs.colc);
   GEI_READ_REG(INTEL_82543GC_TUC,  pDrvCtrl->staRegs.tuc);
   GEI_READ_REG(INTEL_82543GC_DC,   pDrvCtrl->staRegs.dc);
   GEI_READ_REG(INTEL_82543GC_TNCRS, pDrvCtrl->staRegs.tncrs);
   GEI_READ_REG(INTEL_82543GC_SEC,  pDrvCtrl->staRegs.sec);
   GEI_READ_REG(INTEL_82543GC_CEXTEER, pDrvCtrl->staRegs.cexteer);
   GEI_READ_REG(INTEL_82543GC_RLEC,   pDrvCtrl->staRegs.rlec);
   GEI_READ_REG(INTEL_82543GC_XONRXC, pDrvCtrl->staRegs.xonrxc);
   GEI_READ_REG(INTEL_82543GC_XONTXC, pDrvCtrl->staRegs.xontxc);
   GEI_READ_REG(INTEL_82543GC_XOFFRXC, pDrvCtrl->staRegs.xoffrxc);
   GEI_READ_REG(INTEL_82543GC_XOFFTXC, pDrvCtrl->staRegs.xofftxc);
   GEI_READ_REG(INTEL_82543GC_FCRUC, pDrvCtrl->staRegs.fcruc);
   GEI_READ_REG(INTEL_82543GC_PRC64, pDrvCtrl->staRegs.prc64);
   GEI_READ_REG(INTEL_82543GC_PRC127, pDrvCtrl->staRegs.prc127);
   GEI_READ_REG(INTEL_82543GC_PRC255, pDrvCtrl->staRegs.prc255);
   GEI_READ_REG(INTEL_82543GC_PRC511, pDrvCtrl->staRegs.prc511);
   GEI_READ_REG(INTEL_82543GC_PRC1023, pDrvCtrl->staRegs.prc1023);
   GEI_READ_REG(INTEL_82543GC_PRC1522, pDrvCtrl->staRegs.prc1522);
   GEI_READ_REG(INTEL_82543GC_GPRC, pDrvCtrl->staRegs.gprc);
   GEI_READ_REG(INTEL_82543GC_BPRC, pDrvCtrl->staRegs.bprc);
   GEI_READ_REG(INTEL_82543GC_MPRC, pDrvCtrl->staRegs.mprc);
   GEI_READ_REG(INTEL_82543GC_GPTC, pDrvCtrl->staRegs.gptc);
   GEI_READ_REG(INTEL_82543GC_GORL, pDrvCtrl->staRegs.gorl);
   GEI_READ_REG(INTEL_82543GC_GORH, pDrvCtrl->staRegs.gorh);
   GEI_READ_REG(INTEL_82543GC_GOTL, pDrvCtrl->staRegs.gotl);
   GEI_READ_REG(INTEL_82543GC_GOTH, pDrvCtrl->staRegs.goth);
   GEI_READ_REG(INTEL_82543GC_RNBC, pDrvCtrl->staRegs.rnbc);
   GEI_READ_REG(INTEL_82543GC_RUC, pDrvCtrl->staRegs.ruc);
   GEI_READ_REG(INTEL_82543GC_RFC, pDrvCtrl->staRegs.rfc);
   GEI_READ_REG(INTEL_82543GC_ROC, pDrvCtrl->staRegs.roc);
   GEI_READ_REG(INTEL_82543GC_RJC, pDrvCtrl->staRegs.rjc);
   GEI_READ_REG(INTEL_82543GC_TORL, pDrvCtrl->staRegs.torl);
   GEI_READ_REG(INTEL_82543GC_TORH, pDrvCtrl->staRegs.torh);
   GEI_READ_REG(INTEL_82543GC_TOTL, pDrvCtrl->staRegs.totl);
   GEI_READ_REG(INTEL_82543GC_TOTH, pDrvCtrl->staRegs.toth);
   GEI_READ_REG(INTEL_82543GC_TPR, pDrvCtrl->staRegs.tpr);
   GEI_READ_REG(INTEL_82543GC_TPT, pDrvCtrl->staRegs.tpt);
   GEI_READ_REG(INTEL_82543GC_PTC64, pDrvCtrl->staRegs.ptc64);
   GEI_READ_REG(INTEL_82543GC_PTC127, pDrvCtrl->staRegs.ptc127);
   GEI_READ_REG(INTEL_82543GC_PTC255, pDrvCtrl->staRegs.ptc255);
   GEI_READ_REG(INTEL_82543GC_PTC511, pDrvCtrl->staRegs.ptc511);
   GEI_READ_REG(INTEL_82543GC_PTC1023, pDrvCtrl->staRegs.ptc1023);
   GEI_READ_REG(INTEL_82543GC_PTC1522, pDrvCtrl->staRegs.ptc1522);
   GEI_READ_REG(INTEL_82543GC_MPTC, pDrvCtrl->staRegs.mptc);
   GEI_READ_REG(INTEL_82543GC_BPTC, pDrvCtrl->staRegs.bptc);
   GEI_READ_REG(INTEL_82543GC_TSCTC, pDrvCtrl->staRegs.tsctc);
   GEI_READ_REG(INTEL_82543GC_TSCTFC, pDrvCtrl->staRegs.tsctfc);
   GEI_READ_REG(INTEL_82543GC_RDFH, pDrvCtrl->staRegs.rdfh);
   GEI_READ_REG(INTEL_82543GC_RDFT, pDrvCtrl->staRegs.rdft);
   GEI_READ_REG(INTEL_82543GC_RDFHS, pDrvCtrl->staRegs.rdfhs);
   GEI_READ_REG(INTEL_82543GC_RDFTS, pDrvCtrl->staRegs.rdfts);
   GEI_READ_REG(INTEL_82543GC_RDFPC, pDrvCtrl->staRegs.rdfpc);
   GEI_READ_REG(INTEL_82543GC_TDFH, pDrvCtrl->staRegs.tdfh);
   GEI_READ_REG(INTEL_82543GC_TDFT,  pDrvCtrl->staRegs.tdft);
   GEI_READ_REG(INTEL_82543GC_TDFHS, pDrvCtrl->staRegs.tdfhs);
   GEI_READ_REG(INTEL_82543GC_TDFTS, pDrvCtrl->staRegs.tdfts);
   GEI_READ_REG(INTEL_82543GC_TDFPC, pDrvCtrl->staRegs.tdfpc);

   return;
}

/***************************************************************************
*
* gei82543EndStatsDump - Dump statistic registers for MIB2
*
* This routine dumps statistic registers for MIB2 update
*
*   This routine should perhaps accumulate counts internally.
*
* RETURNS: OK
*/
#if 0
LOCAL STATUS gei82543EndStatsDump (GEI_END_DEVICE*    pDrvCtrl    /* device receiving command */
                                   )
{
   END_IFCOUNTERS* pEndStatsCounters;
   UINT32          tmp;

   pEndStatsCounters = &pDrvCtrl->endStatsCounters;

   /*
    * Get number of RX'ed octets
    * Note: the octet counts are 64-bit quantities saved in two
    * 32-bit registers. Reading the high word clears the count,
    * so we have to read the low word first.
    */

   GEI_READ_REG(INTEL_82543GC_GORL, tmp);
   pEndStatsCounters->ifInOctets  = tmp;
   GEI_READ_REG(INTEL_82543GC_GORH, tmp);
   pEndStatsCounters->ifInOctets |= (unsigned long long)tmp << 32;

   /* Get number of TX'ed octets */

   GEI_READ_REG(INTEL_82543GC_GOTL, tmp);
   pEndStatsCounters->ifOutOctets  = tmp;
   GEI_READ_REG(INTEL_82543GC_GOTH, tmp);
   pEndStatsCounters->ifOutOctets |= (unsigned long long)tmp << 32;

   /* Get RX'ed unicasts, broadcasts, multicasts */

   GEI_READ_REG(INTEL_82543GC_GPRC, tmp);
   pEndStatsCounters->ifInUcastPkts     = tmp;
   GEI_READ_REG(INTEL_82543GC_BPRC, tmp);
   pEndStatsCounters->ifInBroadcastPkts = tmp;
   GEI_READ_REG(INTEL_82543GC_MPRC, tmp);
   pEndStatsCounters->ifInMulticastPkts = tmp;
   pEndStatsCounters->ifInUcastPkts    -= (pEndStatsCounters->ifInMulticastPkts +
                                           pEndStatsCounters->ifInBroadcastPkts);
   /* Get TX'ed unicasts, broadcasts, multicasts */

   GEI_READ_REG(INTEL_82543GC_GPTC, tmp);
   pEndStatsCounters->ifOutUcastPkts     = tmp;
   GEI_READ_REG(INTEL_82543GC_BPTC, tmp);
   pEndStatsCounters->ifOutBroadcastPkts = tmp;
   GEI_READ_REG(INTEL_82543GC_MPTC, tmp);
   pEndStatsCounters->ifOutMulticastPkts = tmp;
   pEndStatsCounters->ifOutUcastPkts    -= (pEndStatsCounters->ifOutMulticastPkts +
                                            pEndStatsCounters->ifOutBroadcastPkts);

   return (OK);
}
#endif /* #if 0 */

#ifdef GEI_CSUM_OFFLOAD
/***************************************************************************
*
* geiHwCapsTranslate - translate capability flags into CSUM flags
*
* This routine sets the appropriate per-packet CSUM_* flags depending
* on the interface capabilities enabled.
*
* RETURNS: N/A
*/

LOCAL void geiHwCapsTranslate (GEI_END_DEVICE* pDrvCtrl
                               )
{
   END_CAPABILITIES* pCap = &pDrvCtrl->hwCaps;

   pCap->csum_flags_tx = 0;
   if (pCap->cap_enabled & IFCAP_TXCSUM)
   {
      pCap->csum_flags_tx = CSUM_IP | CSUM_TCP | CSUM_UDP;

      if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
          pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         pCap->csum_flags_tx |= CSUM_TCPv6 | CSUM_UDPv6;
      /*
       * Eventually : CSUM_TCP_SEG for 82544 & later,
       */
   }

   pCap->csum_flags_rx = 0;
   if (pCap->cap_enabled & IFCAP_RXCSUM)
   {
      /*
       * XXX - note that these CSUM flags aren't actually used by the
       * stack on receive! It's not clear how they will be used, other
       * than possibly as administrative info.
       */
      pCap->csum_flags_rx = CSUM_TCP | CSUM_UDP;

      if (pDrvCtrl->adaptor.boardType != PRO1000_543_BOARD)
         pCap->csum_flags_rx |= CSUM_IP;

      if (pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
          pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
         pCap->csum_flags_rx |= (CSUM_TCPv6 | CSUM_UDPv6);

      /* Eventually CSUM_IP_FRAGS ? */
   }
}
#endif

#ifdef INCLUDE_GEI82543_DEBUG_ROUTINE

/***************************************************************************
*
* gei82543StatusShow - show statistic registers
*
* This routine dumps and shows statistic registers
*
* Note, statistics polling may cause this routine to display
* a lot of zeros, as these are clear-on-read registers, and
* gei82543HwStatusDump() and gei82543EndStatsDump() don't accumulate
* counts anywhere.
*
* \NOMANUAL
*
* RETURNS: N/A
*/

void gei82543StatusShow (int unit            /* device unit */
                         )
{
   GEI_END_DEVICE* pDrvCtrl;
   STA_REG*        pStats;
   /* END_CAPABILITIES * pCap;*/
   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return;

   printf ("\n");
   printf ("*****************************************************\n");
   printf ("* Intel 8254x Status *********** Device: %8p *\n", pDrvCtrl);
   printf ("*****************************************************\n");
   printf ("\n");

   /* get speed info */

   gei82543SpeedGet (pDrvCtrl);

   printf (" Device speed:                %d\n", pDrvCtrl->speed);

   /* get duplex info */

   gei82543DuplexGet (pDrvCtrl);

   printf (" Device duplex mode:          %s\n",
           (pDrvCtrl->duplex == FULL_DUPLEX_MODE) ?
           "FULL duplex" : "HALF duplex");

   /* get flow control info */

   if (pDrvCtrl->flowCtrl == FLOW_CONTRL_TRANSMIT)
      printf (" Flow control mode:           %s\n", "TRANSMIT");
   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_RECEIVE)
      printf (" Flow control mode:           %s\n", "RECEIVE");
   else if (pDrvCtrl->flowCtrl == FLOW_CONTRL_ALL)
      printf (" Flow control mode:           %s\n", "RECEIVE/TRANSMIT");
   else
      printf (" Flow control mode:           %s\n", "NONE");

   printf ("\n");

   printf (" Software Rx descriptor tail index:  %d\n", pDrvCtrl->rxDescTail);

   printf (" Software Tx descriptor tail index:  %d\n", pDrvCtrl->txDescTail);

   printf (" Tx descriptor last check index:     %d\n",
           pDrvCtrl->txDescLastCheck);

   printf (" Tx descriptor available number:     %d\n",
           pDrvCtrl->txDescFreeNum);

   printf (" Tx stalls:                          %u\n",
           pDrvCtrl->txStallCount);

   taskDelay(5 * sysClkRateGet() );
#ifdef GEI_CSUM_OFFLOAD
   pCap = &pDrvCtrl->hwCaps;

   printf (" Offload Capabilities (available, enabled, TX, RX):\n"
           "                     (0x%x, 0x%x, 0x%lx, 0x%lx)\n",
           (UINT32)pCap->cap_available, (UINT32)pCap->cap_enabled,
           pCap->csum_flags_tx, pCap->csum_flags_rx);
#endif

   /* dump statistic registers */

   gei82543HwStatusDump (pDrvCtrl);

   /* print out statistic registers */

   pStats = &pDrvCtrl->staRegs;

   printf ("\n");

   printf (" CRC error count:             0x%x\n", pStats->crcerrs);

   printf (" Alignment error count:       0x%x\n", pStats->algnerrc);

   printf (" Symbol error count:          0x%x\n", pStats->symerrs);

   printf (" RX error count:              0x%x\n", pStats->rxerrc);

   printf (" Missed packet count:         0x%x\n", pStats->mpc);

   printf (" Single collision count:      0x%x\n", pStats->scc);

   printf (" Excessive collision count:   0x%x\n", pStats->ecol);

   printf (" Multiple collision count:    0x%x\n", pStats->mcc);

   printf (" late collision count:        0x%x\n", pStats->latecol);

   printf (" Collision count:             0x%x\n", pStats->colc);

   printf (" Transmit underrun count:     0x%x\n", pStats->tuc);

   printf (" Defer count:                 0x%x\n", pStats->dc);

   printf (" Transmit no CRS count:       0x%x\n", pStats->tncrs);

   printf (" Sequence error count:        0x%x\n", pStats->sec);

   printf (" Carrier extension err count: 0x%x\n",
           pStats->cexteer);

   printf (" Receive length error count:  0x%x\n\n", pStats->rlec);

   printf (" XON (received, transmitted):   (0x%x, 0x%x)\n",
           pStats->xonrxc, pStats->xontxc);

   printf (" XOFF (received, transmitted):  (0x%x, 0x%x)\n",
           pStats->xoffrxc, pStats->xofftxc);

   printf (" FC received unsupported count:\t0x%x\n",
           pStats->fcruc);

   printf ("\n");

   taskDelay(5 * sysClkRateGet() );
   printf ("                  Packets Received    Packets Transmitted\n\n");

   printf (" 64 bytes         0x%-8x          0x%x\n",
           pStats->prc64, pStats->ptc64);

   printf (" 65-127 bytes     0x%-8x          0x%x\n",
           pStats->prc127, pStats->ptc127);

   printf (" 128-255 bytes    0x%-8x          0x%x\n",
           pStats->prc255, pStats->ptc255);

   printf (" 256-511 bytes    0x%-8x          0x%x\n",
           pStats->prc511, pStats->ptc511);

   printf (" 512-1023 bytes   0x%-8x          0x%x\n",
           pStats->prc1023, pStats->ptc1023);

   printf (" 1024-1522 bytes  0x%-8x          0x%x\n\n",
           pStats->prc1522, pStats->ptc1522);

   printf (" Total:           0x%-8x          0x%x\n",
           pStats->tpr, pStats->tpt);

   printf (" Good:            0x%-8x          0x%x\n",
           pStats->gprc, pStats->gptc);

   printf (" Broadcast:       0x%-8x          0x%x\n",
           pStats->bprc, pStats->bptc);

   printf (" Multicast:       0x%-8x          0x%x\n",
           pStats->mprc, pStats->mptc);

   printf ("\n");

   printf (" Total octets received:     0x%08x%08x\n",
           pStats->torh, pStats->torl);

   printf (" Good octets received:      0x%08x%08x\n",
           pStats->gorh, pStats->gorl);

   printf (" Total octets transmitted:  0x%08x%08x\n",
           pStats->toth, pStats->totl);

   printf (" Good octets transmitted:   0x%08x%08x\n\n",
           pStats->goth, pStats->gotl);

   taskDelay(5 * sysClkRateGet() );
   printf (" Receive no buffer count:   0x%x\n", pStats->rnbc);

   printf (" Receive under size count:  0x%x\n",  pStats->ruc);

   printf (" Receive fragment count:    0x%x\n", pStats->rfc);

   printf (" Receive oversize count:    0x%x\n", pStats->roc);

   printf (" Receive jabber count:      0x%x\n", pStats->rjc);

   printf ("\n");

   printf (" TCP segmentation context (transmits, fails):\t(0x%x, 0x%x)\n\n",
           pStats->tsctc, pStats->tsctfc);

   printf ("                  RX Data FIFO    TX Data FIFO\n");

   printf (" head             0x%-8x      0x%x\n",
           pStats->rdfh, pStats->tdfh);
   printf (" tail             0x%-8x      0x%x\n",
           pStats->rdft, pStats->tdft);
   printf (" head saved       0x%-8x      0x%x\n",
           pStats->rdfhs, pStats->tdfhs);
   printf (" tail saved       0x%-8x      0x%x\n\n",
           pStats->rdfts, pStats->tdfts);

   printf (" Receive Net Pool ID: %p\n", pDrvCtrl->end.pNetPool);
   printf ("*************************************************************\n");

   taskDelay(5 * sysClkRateGet() );
   return;
}

/****************************************************************************
*
* gei82543RegGet - get the specified register value in 82543 chip
*
* This routine gets and shows the specified register value in 82543 chip
*
* RETURNS: Register value
*/

UINT32 gei82543RegGet (int unit,     /* device unit */
                       UINT32 offset /* register offset */
                       )
{
   UINT32          regVal;
   GEI_END_DEVICE* pDrvCtrl;

   if (offset % 4 != 0)
   {
      printf ("Invalid register index\n");
      return 0;
   }

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
   {
      return (UINT32)-1;
   }

   GEI_READ_REG(offset, regVal);
   return regVal;
}

/****************************************************************************
*
* gei82543RegSet - set the specified register value
*
* This routine sets the specified register value
*
* RETURNS: N/A
*/

void gei82543RegSet (int unit,      /* device unit */
                     UINT32 offset, /* register offset */
                     UINT32 regVal  /* value to write */
                     )
{
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */

   if (offset % 4 != 0)
   {
      printf ("Invalid register index.\n");
      return;
   }

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
   {
      return;
   }

   GEI_WRITE_REG(offset, regVal);

   return;
}

/****************************************************************************
*
* gei82543LedOn - turn on LED
*
* This routine turns LED on
*
* RETURNS: N/A
*/

void gei82543LedOn (int unit        /* device unit */
                    )
{
   UINT32          ctrlRegVal; /* control register value */
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return;

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   ctrlRegVal |= (CTRL_SWDPIO0_BIT | CTRL_SWDPIN0_BIT);

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   return;
}

/***************************************************************************
*
* gei82543LedOff - turn off LED
*
* This routine turns LED off
*
* RETURNS: N/A
*/

void gei82543LedOff (int unit        /* device unit */
                     )
{
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */
   UINT32          ctrlRegVal; /* control register value */

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return;

   GEI_READ_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   ctrlRegVal |= CTRL_SWDPIO0_BIT;

   ctrlRegVal &= ~CTRL_SWDPIN0_BIT;

   GEI_WRITE_REG(INTEL_82543GC_CTRL, ctrlRegVal);

   return;
}

/***************************************************************************
*
* gei82543PhyRegGet - get the register value in PHY
*
* This routine returns the PHY's register value, or -1 if an error occurs.
*
* RETURNS: PHY's register value
*/

int gei82543PhyRegGet (int unit, /* device unit */
                       int reg   /* PHY's register */
                       )
{
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */
   UINT16          tmp;

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return -1;

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      gei82543PhyRead (pDrvCtrl, pDrvCtrl->pPhyInfo->phyAddr, reg, &tmp);
   }
   else if (pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      gei82544PhyRead (pDrvCtrl, pDrvCtrl->pPhyInfo->phyAddr, reg, &tmp);
   }

   return tmp;
}

/***************************************************************************
*
* gei82543PhyRegSet - set the register value in PHY
*
* This routine returns the PHY's register value, or -1 if an error occurs.
*
* RETURNS: PHY's register value
*/

int gei82543PhyRegSet (int unit, /* device unit */
                       int reg,  /* PHY's register */
                       UINT16 tmp
                       )
{
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return -1;

   if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD)
   {
      gei82543PhyWrite (pDrvCtrl, pDrvCtrl->pPhyInfo->phyAddr, reg, tmp);
   }
   else if (pDrvCtrl->adaptor.boardType == PRO1000_544_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_546_BOARD ||
            pDrvCtrl->adaptor.boardType == PRO1000_573_BOARD)
   {
      gei82544PhyWrite (pDrvCtrl, pDrvCtrl->pPhyInfo->phyAddr, reg, tmp);
   }

   return tmp;
}

#ifdef INCLUDE_TBI_COMPATIBLE
/***************************************************************************
*
* gei82543TbiCompWr - enable/disable the TBI compatibility workaround
*
* This routine enables/disables TBI compatibility workaround if needed
*
* Input: unit - unit number of the gei device
*        flag - 0 to turn off TBI compatibility, others to turn on
*
* RETURNS: N/A
*/

void gei82543TbiCompWr (int unit, /* device unit */
                        int flag  /* 0 - off, and others on */
                        )
{
   GEI_END_DEVICE* pDrvCtrl;   /* driver structure */
   UINT32          rctlVal;

   if ( (pDrvCtrl = gei82543Unit (unit)) == NULL)
      return;

   GEI_READ_REG(INTEL_82543GC_RCTL, rctlVal);

   gei82543SpeedGet (pDrvCtrl);

   /* check conditions again */

   if (flag)
   {
      pDrvCtrl->usrFlags |= GEI_END_TBI_COMPATIBILITY;

      if (pDrvCtrl->adaptor.boardType == PRO1000_543_BOARD &&
          pDrvCtrl->cableType == GEI_COPPER_MEDIA &&
          pDrvCtrl->speed == MII_1000MBS)
      {
         pDrvCtrl->tbiCompatibility = TRUE;
         GEI_WRITE_REG(INTEL_82543GC_RCTL, (rctlVal | RCTL_SBP_BIT));

         CACHE_PIPE_FLUSH ();
         printf ("TBI compatibility workaround is enabled for gei%d\n", unit);
         return;
      }
   }
   else
   {
      pDrvCtrl->usrFlags &= ~GEI_END_TBI_COMPATIBILITY;
   }

   pDrvCtrl->tbiCompatibility = FALSE;
   GEI_WRITE_REG(INTEL_82543GC_RCTL, (rctlVal & ~RCTL_SBP_BIT));

   printf ("TBI compatibility workaround is not enabled for gei%d\n", unit);

   CACHE_PIPE_FLUSH ();

   return;
}
#endif /* INCLUDE_TBI_COMPATIBLE */

/***************************************************************************
*
* gei82543Unit - return a pointer to the GEI_END_DEVICE for a gei unit
*
* RETURNS: A pointer the GEI_END_DEVICE for the unit, or NULL.
*/

GEI_END_DEVICE* gei82543Unit (int unit
                              )
{
   GEI_END_DEVICE* pDrvCtrl;

   /* sanity check */

   if (unit < 0)
   {
      printf ("Invalid unit number %d\n", unit);
      return NULL;
   }

   if ( (pDrvCtrl = (GEI_END_DEVICE*)endFindByName (DEVICE_NAME, unit)) == NULL)
   {
      printf ("Can't find device gei%d.\n", unit);
      return NULL;
   }

   return pDrvCtrl;
}

#endif /* INCLUDE_GEI82543_DEBUG_ROUTINE */

/* FORMAT HASH ba093fdb9cc81e62bb1b1c8d438ea4dd */
