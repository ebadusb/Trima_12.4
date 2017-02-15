/**
 * Copyright (C) 2016 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   safe_hw_intf_impl.c
 *
 * @author mra1usb
 * @date   May 9, 2016
 *
 * @brief  Implements Safety's generic hardware I/O interface
 *
 * Implements various forms of the HW_INTF_DB macro defined in safe_hw_intf_map.hxx
 * to generate the necessary enums and array initialization.
 *
 */

#ifndef _SAF_HW_INTF_IMPL_
#define _SAF_HW_INTF_IMPL_

/* From Common */
#include "hw_intf.h"
#include "cca_pci_intf.h"

#define CCA_0   0
#define BAR_0   0

/* Use these to define non-applicable entries in the HW_INTF_DB map */
#define ISA_REG_NA  HwPortReg_NA
#define CCA_NA      HwPortReg_NA
#define BAR_NA      HwPortReg_NA
#define REG_NA      HwPortReg_NA

/*
 * Define macro to extract the portId enum which serves as index to theMap[]
 */
#undef  HW_INTF_DB
#define HW_INTF_DB(portName, isaPortReg, ccaIndx, barIndx, barOffset)  portName,

enum HwPortId_Safety_t
{
   #include "safe_hw_intf_map.hxx"
   HWIF_LAST_PORT_ID
};


typedef struct _HwIfMapItem
{
   const char* portName;
   HwPortReg   isaPortReg;
   HwPortReg   ccaPortReg;
} HwIfMapItem;

/*
 * Define macro to populate the map of port registers.
 */
#undef  HW_INTF_DB
#define HW_INTF_DB(portName, isaPortReg, ccaIndx, barIndx, barOffset) \
   {#portName,	isaPortReg, CCA_IO_PORT(ccaIndx, barIndx, barOffset) },

static const HwIfMapItem theHwIfMap[] =
{
   #include "safe_hw_intf_map.hxx"
};

////////////////////////////////////////////////////////////////////////////////

static const char* hwGetPortNameImpl (HwPortId portId)
{
   if (portId < HWIF_LAST_PORT_ID)
   {
      return theHwIfMap[portId].portName;
   }
   return "PortId_UNKNOWN";
}

static HwPortReg hwGetPortRegisterForISA (HwPortId portId)
{
   if (portId < HWIF_LAST_PORT_ID)
   {
      return theHwIfMap[portId].isaPortReg;
   }
   return HwPortReg_NA;
}

static HwPortReg hwGetPortRegisterForPCI (HwPortId portId)
{
   if (portId < HWIF_LAST_PORT_ID)
   {
      return theHwIfMap[portId].ccaPortReg;
   }
   return HwPortReg_NA;
}

static BOOL hwSetHwIfImplForISA (void)
{
   HwInterfaceImpl isaImpl = {};

   isaImpl.GetPortName     = &hwGetPortNameImpl;
   isaImpl.GetPortRegister = &hwGetPortRegisterForISA;
   isaImpl.InByte          = &sysInByte;
   isaImpl.InWord          = &sysInWord;
   isaImpl.InLong          = &sysInLong;
   isaImpl.OutByte         = &sysOutByte;
   isaImpl.OutWord         = &sysOutWord;
   isaImpl.OutLong         = &sysOutLong;
   isaImpl.name            = "SafHwIntfImpl_ISA";
   isaImpl.numPorts        = HWIF_LAST_PORT_ID;

   return hwInitInterface(&isaImpl);
}

static BOOL hwSetHwIfImplForPCI (void)
{
   HwInterfaceImpl pciImpl = {};

   pciImpl.GetPortName     = &hwGetPortNameImpl;
   pciImpl.GetPortRegister = &hwGetPortRegisterForPCI;
   pciImpl.InByte          = &ccaInByte;
   pciImpl.InWord          = &ccaInWord;
   pciImpl.InLong          = &ccaInLong;
   pciImpl.OutByte         = &ccaOutByte;
   pciImpl.OutWord         = &ccaOutWord;
   pciImpl.OutLong         = &ccaOutLong;
   pciImpl.name            = "SafHwIntfImpl_PCI";
   pciImpl.numPorts        = HWIF_LAST_PORT_ID;

   return hwInitInterface(&pciImpl);
}

#endif /* _SAF_HW_INTF_IMPL_ */

/* FORMAT HASH 9dc2147f40e7bd631e6828a75a2d7d84 */
