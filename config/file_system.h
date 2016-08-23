/*
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/config/rcs/file_system.h 1.1 2007/11/07 16:54:41Z dslausb Exp $
 *
 * file system configuration
 *
 * $Log: file_system.h $
 * Revision 1.1  2007/11/07 16:54:41Z  dslausb
 * Initial revision
 * Revision 1.5  2007/09/17 21:48:27Z  rhecusb
 * Removed prototype for trimaSysGetPartitionData. It is in trimaOs.h.
 * Revision 1.3  2007/07/09 22:10:43Z  dslausb
 * Revision 1.2  2007/07/06 21:35:39Z  dslausb
 * Got rid of some duplicate stuff
 * Revision 1.1  2007/05/22 18:03:12Z  dslausb
 * Initial revision
 *
*/

#ifndef _FILE_SYSTEM_INCLUDE
#define _FILE_SYSTEM_INCLUDE

#include "filenames.h"
/*
 *  Make sure this file uses only constructs valid in both C and C++.  Although
 * it is used mostly from .cpp files, the operating system image build
 * uses it from a .c file.
 */

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/*
 * Disk partition information
*/
typedef enum
{
   /*
    *  If changing this enum, be sure to also change the taosPartData
   * array defined below.
   */
   FirstPartition  = 0,                   /* must be defined as 0 */
   VxBootPartition = FirstPartition,
   TrimaPartition,
   ConfigPartition,
   RunDataPartition,
   EndOfPartitionEntries                   /* must be last entry */
}TrimaPartitionID;

typedef struct
{
   long long   bytesTotal;         /* total bytes in partition */
   long long   bytesOffset;        /* offset of partition from start of disk */
   int         maxOpenFiles;       /* maximum number of open files allowed on partition */
   int         readOnly;           /* 0 if read/write, non-zero if read-only */
   const char* name;               /* mount point for partition */
} TrimaPartitionData;

#ifdef OS_BUILD
/*
 *  Although defined here so that partition information is contained
 * completely within one file, this data is intended only for use
 * by the Taos OS image.  All other applications should use the
 * taosSysGetPartitionData function.
 */
TrimaPartitionData trimaPartitionData[EndOfPartitionEntries] =
{
   /* size and offset information is filled in at boot time */
   { 0, 0, 16,  1,  VXBOOT_PARTITION_NAME},
   { 0, 0, 256, 1,  TRIMA_PARTITION_NAME},
   { 0, 0, 16,  0,  CONFIG_PARTITION_NAME},
   { 0, 0, 64,  0,  MACHINE_PARTITION_NAME}
};


/* Define if read-protected file systems should be mounted read-only.
 * This should be defined for a production release
 */
#define MOUNT_READONLY

/*
 *  Cache sizes are in terms of 512 byte blocks
*/
#define TOTAL_CACHE_BLOCKS 2048
#define WRITE_CACHE_BLOCKS 160

/*
 *  Minimum free space (at boot time) in MB on machine partition
*/
#define MIN_MACHINE_PARTITION_SPACE 500

#endif /* ifdef OS_BUILD */

/*int  trimaSysGetPartitionData(TrimaPartitionID id, TrimaPartitionData * result);*/

#ifdef __cplusplus
}  // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _FILE_SYSTEM_INCLUDE */

/* FORMAT HASH 2aef72ff472641ce3bdb62fa441af0ee */
