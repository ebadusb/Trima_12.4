/**
 * Copyright (C) 2016 Terumo BCT, Inc.  All rights reserved.
 *
 * @file   cca_pci_stubs.c
 *
 * @author mra1usb
 * @date   Jun 22, 2016
 *
 * @brief  Stubs for kernels that don't INCLUDE_PCI and thus don't need CCA PCI support
 *
 * Needed for compatibility with the PCI-based E-Box 2016 kernels
 *
 */

/* ----------------------------- INCLUDES -------------------------- */
#include <vxWorks.h>
#include "cca_pci_intf.h"

/* ----------------------------- MACROS ---------------------------- */
/* ----------------------------- CONSTANTS ------------------------- */
/* ----------------------------- PROTOTYPES------------------------- */

unsigned int ccaPciResourcesAvailable (void) { return 0; }

UINT32 ccaPciIn32 (UINT8 offset, UINT rsrcIndx, BOOL useBar1) { return 0; }

UCHAR  ccaInByte (CcaIoPort barIdWithOffset) { return 0; }
