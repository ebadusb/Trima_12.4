/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: K:/BCT_Development/vxWorks/Trima/low_level/ctl_drv/rcs/rbcshmem.h 1.2 1999/09/23 00:26:01 BS04481 Exp jl11312 $
 * $Log: rbcshmem.h $
 * Revision 1.2  1999/09/23 00:26:01  BS04481
 * Add crc to shared memory data for rbc detector
 * Revision 1.1  1999/09/17 15:04:21  BS04481
 * Initial revision
 *
 * TITLE:      rbcshmem.h, RBC port shared memory
 *
 * ABSTRACT:   Definition of shared memory area between control
 *             driver rbcDetector class and the rbcPort task
 *
 * DOCUMENTS
 * Requirements:     I:\ieee1498\SSS3.DOC
 * Test:             I:\ieee1498\STD3.DOC
 *
 */

#ifndef _RBCSHMEM_INCLUDE
#define _RBCSHMEM_INCLUDE

#include "chw.hpp"
#include "hw.h"

struct writeDrive
{
   enum { DriveValuesLen = 80 };
   char          newDriveValues[DriveValuesLen]; // new drive values to be written to the micro by the rbcPort task
   int           newDriveResult;                 // result of writting the new drive values back to rbcDetector

   unsigned long crc;                       // everything prior to crc member is included in checksum
   int           writeCount;                // shmem flag.  incremented by rbcDetector.  decremented by rbcPort
};

typedef struct
{
   short         red;               // red reflectance reading from the micro via the rbcPort task
   short         green;             // green reflectance reading from the micro via the rbcPort task
   HW_ERROR      errors;            // rbc communication errors detected by the rbcPort task

   unsigned long crc;               // everything prior to crc member is included in checksum
   unsigned int  portReadCount;     // counter updated by rbcPort task everytime it reads from the micro
} readReflectance;

#endif /* ifndef _RBCSHMEM_INCLUDE */

/* FORMAT HASH 774ef2eb44c821e7c008458c4ac19b9e */
