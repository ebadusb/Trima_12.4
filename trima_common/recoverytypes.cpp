/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoverytypes.cpp
 *             Recovery id to string mapping class
 *
 ******************************************************************************/
#include <vxworks.h>

#define RECOVERY_TYPES_CPP

#include "recoverytypes.h"
#include "string.h"

const int numElements = sizeof( RecoveryNames ) / sizeof( RecoveryNameStruct );

RecoveryTypes::Type RecoveryTypes::RecoveryId (const char* str)
{
   if ( str == 0 )
      return None;

   int i = 0;
   while ( i < numElements )
   {
      if ( strcmp(str, RecoveryNames[i].RecoveryName) == 0 )
      {
         return RecoveryNames[i].RecoveryID;
      }
      i++;
   }

   return None;
}

const char* RecoveryTypes::RecoveryStr (Type id)
{
   int i = 0;
   while ( i < numElements )
   {
      if ( RecoveryNames[i].RecoveryID == id )
      {
         return RecoveryNames[i].RecoveryName;
      }
      i++;
   }

   return 0;
}

/* FORMAT HASH 84912596a89cf9395066e501c849599d */
