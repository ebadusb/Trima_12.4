/*
 * $Header$
 *
 *	NFS mount data
 *
 */

#ifndef _NFS_DATA_INCLUDE
#define _NFS_DATA_INCLUDE

#include "filenames.h"

#ifdef OS_BUILD
/*
 *	List of NFS mount points
 */

typedef enum
{
   NFS_READ_WRITE,
   NFS_READ_ONLY
} NFS_MOUNT_TYPE;

typedef struct
{
   const char*    _mountPath;
   NFS_MOUNT_TYPE _mountType;
   int            _mountRequired;
} NFSMountData;

#define NFS_MOUNT_POINTS    sizeof(nfsMountData) / sizeof(nfsMountData[0])


NFSMountData nfsMountData[] =
{
   { TRIMA_PARTITION_NAME, NFS_READ_ONLY, 1},
   { CONFIG_PATH, NFS_READ_ONLY,          1},
   { TEMP_PATH, NFS_READ_WRITE,           1},
   { SAVEDATA_PATH, NFS_READ_WRITE,       1},
   { TOOLS_PATH, NFS_READ_WRITE,          0},
};
#endif /* ifdef OS_BUILD */

#endif /* ifndef _NFS_DATA_INCLUDE */

/* FORMAT HASH 4b2976fef7d204ddf5aeb2e4cabb5618 */
