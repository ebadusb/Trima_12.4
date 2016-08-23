/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * vxWorks serial port driver with support for Ampro LB-486
 *
 * $Header$
 *
 */

#ifndef _SERIALDRV_INCLUDE
#define _SERIALDRV_INCLUDE

#include "vxWorks.h"

STATUS setupSerialPort (unsigned int baseAddress, unsigned int intLevel, unsigned int portNumber);

#endif /* ifndef _SERIALDRV_INCLUDE */

/* FORMAT HASH a45f8fa450d2c250bed6a21248d98a4d */
