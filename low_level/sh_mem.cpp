/*
 * Copyright (C) 2002 Gambro BCT, Inc.  All rights reserved.
 *
 *  vxWorks named shared memory blocks interface
 *
 * $Header$
 *
 */

#include <vxWorks.h>
#include <a_out.h>
#include <semLib.h>
#include <stdlib.h>
#include <string.h>
#include <symLib.h>
#include <sysSymTbl.h>
#include "sh_mem.h"
#include "trima_datalog.h"

/* semaphore to protect access internal data */
static SEM_ID shmem_sem = NULL;

/* header data for a shared memory block */
struct shmem_block
{
   unsigned int linkCount;     /* count of tasks linked to this block */
   unsigned int blockStart;    /* start of client memory block */
};

static char* shmem_create_sym_name (const char* name)
{
   const char* symPrefix = "shmem_";
   char*       symName   = (char*)malloc(strlen(name) + strlen(symPrefix) + 1);

   strcpy(symName, symPrefix);
   strcat(symName, name);
   return symName;
}

static void shmem_acquire_lock (void)
{
   if (shmem_sem == NULL)
      shmem_sem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);

   semTake(shmem_sem, WAIT_FOREVER);
}

static void shmem_release_lock (void)
{
   semGive(shmem_sem);
}


/* Create a named memory block */
void* shmem_create (const char* name, size_t size)
{
   DataLog_Critical critical;
   char*            symName = shmem_create_sym_name(name);
   shmem_block*     memBlock;
   SYM_TYPE         memType;
   void*            result = NULL;

   shmem_acquire_lock();
   if (symFindByName(sysSymTbl, symName, (char**)&memBlock, &memType) == OK)
   {
      DataLog(critical) << "shmem_create - \"" << name << "\" already exists" << endmsg;
   }
   else
   {
      int allocSize = sizeof(shmem_block);
      if (size > sizeof(unsigned int))
         allocSize += size - sizeof(unsigned int);

      memBlock            = (shmem_block*)malloc(allocSize);
      memset(memBlock, 0, allocSize);
      memBlock->linkCount = 1;

      symAdd(sysSymTbl, symName, (char*)memBlock, N_DATA, 0);
      result = (void*)&(memBlock->blockStart);
   }

   shmem_release_lock();
   return result;
}

/* Get a pointer to an existing shared memory block */
void* shmem_open (const char* name)
{
   char*        symName = shmem_create_sym_name(name);
   shmem_block* memBlock;
   SYM_TYPE     memType;
   void*        result = NULL;
   int          retry  = 0;

   while (retry < 1000 && !result)
   {
      shmem_acquire_lock();
      if (symFindByName(sysSymTbl, symName, (char**)&memBlock, &memType) == OK)
      {
         memBlock->linkCount += 1;
         result               = (void*)&(memBlock->blockStart);
      }

      shmem_release_lock();
      if (!result)
      {
         taskDelay(2);
         retry += 1;
      }
   }

   return result;
}


/* End access to an existing shared memory block */
void shmem_close (const char* name)
{
   char*        symName = shmem_create_sym_name(name);
   shmem_block* memBlock;
   SYM_TYPE     memType;

   shmem_acquire_lock();
   if (symFindByName(sysSymTbl, symName, (char**)&memBlock, &memType) == OK)
   {
      memBlock->linkCount -= 1;
      if (memBlock->linkCount <= 0)
      {
         symRemove(sysSymTbl, symName, N_DATA);
         free(memBlock);
      }
   }

   shmem_release_lock();
   return;
}

/* FORMAT HASH b7ef86d4f7f9345b1c2226347f7f72af */
