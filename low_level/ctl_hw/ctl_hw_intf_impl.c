/**
 * Copyright (C) 2016 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   ctl_hw_intf_impl.c
 *
 * @author mra1usb
 * @date   Apr 26, 2016
 *
 * @brief  Implements Control's generic hardware I/O interface
 *
 * Implements various forms of the HW_INTF_DB macro defined in ctl_hw_intf_map.hxx
 * to generate the necessary enums and array initialization.
 *
 */

#ifndef _CTL_HW_INTF_IMPL_
#define _CTL_HW_INTF_IMPL_

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

enum HwPortId_Control_t
{
   #include "ctl_hw_intf_map.hxx"
   HWIF_LAST_PORT_ID
};


typedef struct _HwIfMapItem
{
   HwPortReg isaPortReg;
   HwPortReg ccaPortReg;
} HwIfMapItem;

/*
 * Define macro to populate the map of port registers.
 */
#undef  HW_INTF_DB
#define HW_INTF_DB(portName, isaPortReg, ccaIndx, barIndx, barOffset) \
   {isaPortReg, CCA_IO_PORT(ccaIndx, barIndx, barOffset) },

static const HwIfMapItem theHwIfMap[] =
{
   #include "ctl_hw_intf_map.hxx"
};

////////////////////////////////////////////////////////////////////////////////

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

   isaImpl.GetPortRegister = &hwGetPortRegisterForISA;
   isaImpl.InByte          = &sysInByte;
   isaImpl.InWord          = &sysInWord;
   isaImpl.InLong          = &sysInLong;
   isaImpl.OutByte         = &sysOutByte;
   isaImpl.OutWord         = &sysOutWord;
   isaImpl.OutLong         = &sysOutLong;
   isaImpl.name            = "CtlHwIntfImpl_ISA";
   isaImpl.numPorts        = HWIF_LAST_PORT_ID;

   return hwInitInterface(&isaImpl);
}

static BOOL hwSetHwIfImplForPCI (void)
{
   HwInterfaceImpl pciImpl = {};

   pciImpl.GetPortRegister = &hwGetPortRegisterForPCI;
   pciImpl.InByte          = &ccaInByte;
   pciImpl.InWord          = &ccaInWord;
   pciImpl.InLong          = &ccaInLong;
   pciImpl.OutByte         = &ccaOutByte;
   pciImpl.OutWord         = &ccaOutWord;
   pciImpl.OutLong         = &ccaOutLong;
   pciImpl.name            = "CtlHwIntfImpl_PCI";
   pciImpl.numPorts        = HWIF_LAST_PORT_ID;

   return hwInitInterface(&pciImpl);
}

#endif /* _CTL_HW_INTF_IMPL_ */

/* FORMAT HASH d0ab5137f918c2aeb97cf6c1f0eea4a4 */
