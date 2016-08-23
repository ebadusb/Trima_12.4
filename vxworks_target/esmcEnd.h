/* esmcEnd.h - SMSC 91cxxx network interface header */ 

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01a, 28jun02, dnb mostly copied from if_esmc.h
*/


#ifndef INCesmcEndh
#define INCesmcEndh

#include <end.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESMC_DEV_NAME          "esmc"
#define ESMC_DEV_NAME_LEN      5

#define EADDR_LEN         6
#define ESMC_BUFSIZ       (ETHERMTU + SIZEOF_ETHERHEADER + EADDR_LEN)

#define ESMC_BANK_SELECT  0x0e

/* BANK 0 I/O Space mapping */

#define    ESMC_TCR       0x0    /* transmit control register */
#define    ESMC_EPH       0x2    /* EPH status register */
#define    ESMC_RCR       0x4    /* receive control register */
#define    ESMC_COUNTER   0x6    /* counter register */
#define    ESMC_MIR       0x8    /* memory information register */
#define    ESMC_MCR       0xA    /* memory configuration register */

/* Transmit Control Register */
#define ESMC_TCR_TXEN        0x0001    /* transmit enabled when set */ 
#define ESMC_TCR_LOOP        0x0002    /* internal loopback */
#define ESMC_TCR_FORCOL      0x0004    /* force collision, then auto resets */ 
/* unused                    0x0008     */

/* unused                    0x0010     */
/* unused                    0x0020     */
/* unused                    0x0040     */
#define ESMC_TCR_PAD         0x0080    /* pad short frames to 64 bytes */

#define ESMC_TCR_NOCRC       0x0100    /* NOT append CRC to frame if set */
/* unused                    0x0200     */
#define ESMC_TCR_MON_CNS     0x0400    /* monitors the carrier while trans */
#define ESMC_TCR_FDUPLEX     0x0800    /* enable full duplex operation */
#define ESMC_TCR_STP_SQET    0x1000    /* stop trans on signal quality error */
#define ESMC_TCR_EPH_LOOP    0x2000    /* internal loopback at EPH block */
/* unused                    0x4000     */
/* unused                    0x8000     */   


/* EPH Status Register - stores status of last transmitted frame */
/* ## indicates fatal error */
#define ESMC_TS_TX_SUC       0x0001    /* last Transmit was successful */
#define ESMC_TS_SNGLCOL      0x0002    /* single collision detected */
#define ESMC_TS_MULCOL       0x0004    /* multiple collision detected */
#define ESMC_TS_LXT_MULT     0x0008    /* last frame was multicast */

#define ESMC_TS_16COL        0x0010    /* ## 16 collisions reached */
#define ESMC_TS_SQET         0x0020    /* ## signal quality error test */
#define ESMC_TS_LTX_BRD      0x0040    /* last frame was broascast packet */
#define ESMC_TS_TX_DEFR      0x0080    /* transmit deferred - auot cleared */

/* unused                    0x0100     */
#define ESMC_TS_LATCOL       0x0200    /* ## late collision */
#define ESMC_TS_LOST_CARR    0x0400    /* ## lost carrier sense */
#define ESMC_TS_EXC_DEF      0x0800    /* excessive deferal */

#define ESMC_TS_CTR_ROL      0x1000    /* counter rollover */
#define ESMC_TS_RX_OVRN      0x2000    /* on FIFO overrun... */
#define ESMC_TS_LINK_OK      0x4000    /* twisted pair link cond */  
#define ESMC_TS_TXUNRN       0x8000    /* tramsmit under run */


                                     
/* Receive control register */
#define ESMC_RCR_RX_ABORT    0x0001    /* set if receiver overrun */
#define    ESMC_RCR_PRMS     0x0002    /* enable promiscuous mode */
#define ESMC_RCR_ALMUL       0x0004    /* receive all multicast packets */
/* unused                    0x0008     */

/* unused                    0x0010     */
/* unused                    0x0020     */
/* unused                    0x0040     */
/* unused                    0x0080     */

#define ESMC_RCR_RXEN        0x0100    /* ENABLE receiver */  
#define ESMC_RCR_STRIP_CRC   0x0200    /* when set, strips CRC */
#define ESMC_RCR_FILT_CAR    0x4000    /* filter carrier sense 12 bits */ 
#define ESMC_RCR_EPH_RST     0x8000    /* software activated reset */

/* unused                    0x1000     */
/* unused                    0x2000     */
/* unused                    0x4000     */
/* unused                    0x8000     */


/* Memory Information Register */

#define ESMC_MIR_FREE        MIR    /* read at any time for free mem */
#define ESMC_MIR_SIZE        MIR+1    /* determine amount of onchip mem */


/* BANK 1 I/O Space mapping */

#define ESMC_CONFIG          0x0    /* configuration register */
#define    ESMC_BASE         0x2    /* base address register */
#define    ESMC_ADDR_0       0x4    /* individual address register 0 */
#define    ESMC_ADDR_1       0x6    /* individual address register 1 */
#define    ESMC_ADDR_2       0x8    /* individual address register 2 */
#define    ESMC_GENERAL      0xA    /* general purpose register */
#define    ESMC_CONTROL      0xC    /* control register */

/* Configuration register */
/* unused                     0x0001    */
#define ESMC_INT_SEL_0        0x0002
#define ESMC_INT_SEL_2        0x0004
/* reserved                   0x0008     */

/* reserved                   0x0010     */
/* reserved                   0x0020     */
#define ESMC_CFG_DIS_LINK     0x0040    /* disables link test, TP only */
#define ESMC_CFG_16BIT        0x0080    /* usually auto set   16/8 selection */

#define ESMC_CFG_AUI_SELECT   0x0100    /* when set use AUI */
#define ESMC_CFG_SET_SQLCH    0x0200    /* when set squelch level is 240mV */
#define ESMC_CFG_FULL_STEP    0x0400    /* AUI, use full step signaling */
/* unused                     0x0800     */

#define ESMC_CFG_NO_WAIT_ST   0x1000    /* when set */
/* unused                     0x2000     */
/* unused                     0x4000     */
/* unused                     0x8000     */


#define ESMC_INT_INTR_ZERO    0xf9    /* mask to clear interrrupt setting */
#define ESMC_INT_SEL_INTR0    0x0
#define ESMC_INT_SEL_INTR1    0x2
#define ESMC_INT_SEL_INTR2    0x4
#define ESMC_INT_SEL_INTR3    0x6    

/* Control Register */
#define ESMC_CTR_STORE           0x0001
#define ESMC_CTR_RELOAD          0x0002
#define ESMC_CTR_EPROM_SELECT    0x0004    /* EEprom access bit */
/* unused                        0x0008     */

/* unused                        0x0010     */
#define    ESMC_CTR_TE_ENABLE    0x0020    /* transmit error enable */
#define    ESMC_CTR_CR_ENABLE    0x0040    /* counter rollover enable */
#define    ESMC_CTR_LE_ENABLE    0x0080    /* Link error enable */

/* unused                        0x0100     */
/* unused                        0x0200     */
/* unused                        0x0400     */
#define ESMC_CTR_AUTO_RELEASE    0x0800    /* transmit memory auto released */

/* unused                        0x1000     */
#define ESMC_CTR_PWEDN           0x2000    /* high puts chip in powerdown mode */
#define ESMC_CTR_RCV_BAD         0x4000    /* when set bad CRC packets received */
/* unused                        0x8000     */


/* BANK 2 I/O Space mapping */

#define ESMC_MMU                0x0    /* MMU command register */
#define ESMC_PNR                0x2    /* packet number register */    
#define ESMC_ARR                0x3    /* allocation result register */
#define ESMC_FIFO               0x4    /* FIFO ports register */
#define ESMC_PTR                0x6    /* pointer register */
#define ESMC_DATA_1             0x8    /* data register 1 */
#define ESMC_DATA_2             0xA    /* data register 2 */
#define ESMC_INT_STAT           0xC    /* int status & acknowledge register */
#define ESMC_INT_ACK            0xC    /* int status & acknowledge register */
#define ESMC_INT_MASK           0xD    /* int mask register */

/* MMU Command Register */
#define ESMC_MMU_BUSY           0x0001    /* set if MMU busy */
#define ESMC_MMU_N0             0x0001
#define ESMC_MMU_N1             0x0002
#define ESMC_MMU_N2             0x0004
/* unused                       0x0008     */
/* unused                       0x0010     */
#define ESMC_MMU_CMD            0x0020 |   \
                                0x0040 |   \
                                0x0080
                                
/* mmu cmds */
#define ESMC_MMU_ALLOC          0x0020    /* get memory from chip memory/256 */
#define ESMC_MMU_NOP            0x0000
#define ESMC_MMU_RESET          0x0040    
#define ESMC_MMU_RX_REMOVE      0x0060    /* remove current RX frame from FIFO */
#define ESMC_MMU_RX_RELEASE     0x0080    /* also release memory associated */
#define ESMC_MMU_TX_RELEASE     0x00A0    /* free packet memory in PNR register */
#define ESMC_MMU_ENQUEUE        0x00C0    /* Enqueue the packet for transmit */
#define ESMC_MMU_TX_RESET       0x00E0    /* reset TX FIFO's */

/* Allocation Result Register */
#define ESMC_ARR_PACKETNUMBER   0x001 |  \
                                0x002 |  \
                                0x004 |  \
                                0x008 |  \
                                0x010
/* unused                       0x020     */
/* unused                       0x040     */
#define ESMC_ARR_FAILED         0x080

/* FIFO Ports Register */
#define ESMC_FIFO_TX_DONE_PACKET_NUMBER   0x0001  | \
                                          0x0002  | \
                                          0x0004  | \
                                          0x0008  | \
                                          0x0010  
/* unused                                 0x0020     */
/* unused                                 0x0040     */
#define ESMC_FIFO_TEMPTY                  0x0080    /* no tx packets queued in TX FIFO  */

#define ESMC_FIFO_RX_PACKET_NUMBER        0x0100  | \
                                          0x0200  | \
                                          0x0400  | \
                                          0x0800  | \
                                          0x1000
/* unused                                 0x2000     */
/* unused                                 0x4000     */
#define ESMC_FIFO_REMPTY                  0x8000    /* no rx packets queued in RX FIFO */



/* Pointer Register - address to be accessed in chip memory */
#define ESMC_PTR_READ       0x2000    /* type of access to follow */
#define ESMC_PTR_AUTOINC    0x4000    /* auto incs. PTR correct amount */
#define ESMC_PTR_RCV        0x8000    /* when set, refers to receive area */

/* Interrupt Detail */
#define ESMC_INT_RCV           0x01
#define    ESMC_INT_TX         0x02    /* set when at least one packet sent */
#define    ESMC_INT_TX_EMPTY   0x04    /* set id TX_FIFO empty */
#define    ESMC_INT_ALLOC      0x08    /* set when MMU allocation is done */
#define    ESMC_INT_RX_OVRN    0x10    /* set when receiver overruns */
#define    ESMC_INT_EPH        0x20    /* Set when EPH handler fires */
#define    ESMC_INT_ERCV       0x40    /* early receive */
#define    ESMC_INT_TX_IDLE    0x80    /* TX State Machine Not Active */

/* BANK 3 I/O Space mapping */

#define    ESMC_MULTICAST0        0x0    /* multicast table - WORD 0 */
#define    ESMC_MULTICAST2        0x2    /* multicast table - WORD 1 */
#define    ESMC_MULTICAST4        0x4    /* multicast table - WORD 2 */
#define    ESMC_MULTICAST6        0x6    /* multicast table - WORD 3 */
#define    ESMC_MGMT              0x8
#define    ESMC_REVISION          0xA    /* chip set and revision encoded here */
#define ESMC_ERCV                 0xC    /* early receive register */

/* Receive frame status word - located at beginning of each received frame*/

#define ESMC_RS_ALGNERR        0x8000    /* frame had alignment error */
#define ESMC_RS_BRODCAST       0x4000    /* receive frame was broadcast */
#define ESMC_RS_BADCRC         0x2000    /* CRC error */
#define ESMC_RS_ODDFRM         0x1000    /* receive frame had odd byte */
#define ESMC_RS_TOOLONG        0x0800    /* longer then 1518 bytes */
#define ESMC_RS_TOOSHORT       0x0400    /* shorter then 64 bytes */
#define ESMC_RS_MULTICAST      0x0001    /* receive frame was multicast */
#define ESMC_RS_ERROR_MASK    (ESMC_RS_ALGNERR | ESMC_RS_BADCRC | ESMC_RS_TOOLONG | ESMC_RS_TOOSHORT) 

#define ESMC_SWITCH_BANK(pDev, newBank)              \
        WRITE_WORD(pDev, ESMC_BANK_SELECT, newBank);

END_OBJ * esmcEndLoad(char * initString, void * pBsp);

#ifdef __cplusplus
}
#endif

#endif    /* INCesmcEndh */
