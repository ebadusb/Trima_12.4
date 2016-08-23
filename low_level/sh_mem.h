/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *	vxWorks named shared memory blocks interface
 *
 * $Header$
 *
 */

#ifndef _SH_MEM_INCLUDE
#define _SH_MEM_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif /* ifdef __cplusplus */

/* Create a named memory block */
void* shmem_create (const char* name, /* name of shared memory block to be created */
                    size_t size       /* size (in bytes) of shared memory block */
                    );

/* Get a pointer to an existing shared memory block */
void* shmem_open (const char* name       /* name of existing shared memory block */
                  );

/* End access to an existing shared memory block */
void shmem_close (const char* name       /* name of existing shared memory block */
                  );

#ifdef __cplusplus
}; // extern "C"
#endif /* ifdef __cplusplus */

#endif /* ifndef _SH_MEM_INCLUDE */

/* FORMAT HASH 66144074d4d62bf3ea5ff7ff7d271836 */
