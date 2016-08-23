/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      substatemap.cpp
 *
 ******************************************************************************/
#include <vxworks.h>

#define SUBSTATE_MAP_CPP
#define STATE_LIST

#include "substatemap.h"
#include "string.h"
#include "trima_datalog.h"


int SubstateMap::alg (const State_names id, const bool rbcPlasmaProcedure)
{
   State_names enumid = id;

   if ( rbcPlasmaProcedure )
      switch ( id )
      {
         case SS_CHANNEL_SETUP :
            enumid = SS_RP_CHANNEL_SETUP;
            break;
         case SS_PREPLATELET_PLASMA :
            enumid = SS_RP_PLASMA_ONLY;
            break;
         case SS_RBC_PTF_SETUP_1 :
            enumid = SS_RP_RBC_PTF_SETUP_1;
            break;
         case SS_RBC_PTF_SETUP_2 :
            enumid = SS_RP_RBC_PTF_SETUP_2;
            break;
         case SS_RBC_PTF_PRIME :
            break;
         case SS_RBC :
            enumid = SS_RP_RBC;
            break;
         default :
            break;
      }

   int i   = 0;
   int num = sizeof( SubstateMapTable ) / sizeof( SubstateMapStruct );
   while ( i < num )
   {
      if ( enumid == SubstateMapTable[i].TrimaID )
      {
         return SubstateMapTable[i].AlgorithmID;
      }
      i++;
   }

   DataLog(log_level_trima_library_error) << "ERROR: no state name found for " << id << " !" << endmsg;

   return -1;
}

State_names SubstateMap::ss (const int algid, const bool rbcPlasmaProcedure)
{
   int i   = 0;
   int num = sizeof( SubstateMapTable ) / sizeof( SubstateMapStruct );

   while ( i < num )
   {
      if ( (    !rbcPlasmaProcedure
                || SubstateMapTable[i].TrimaID >= SS_RP_CHANNEL_SETUP )

           && ( algid == SubstateMapTable[i].AlgorithmID )
           )
      {
         return SubstateMapTable[i].TrimaID;
      }
      i++;
   }

   return INVALID_SUBSTATE;
}

State_names SubstateMap::ss (const char* str)
{
   if ( str == 0 )
      return INVALID_SUBSTATE;

   int i   = 0;
   int num = sizeof( state_name_table ) / sizeof( the_state_struct );

   while ( i < num )
   {
      if ( strcmp(str, state_name_table[i].state_text) == 0 )
      {
         return state_name_table[i].state;
      }
      i++;
   }

   return INVALID_SUBSTATE;
}

const char* SubstateMap::algStr (int id, const bool rbcPlasmaProcedure)
{
   int i   = 0;
   int num = sizeof( SubstateMapTable ) / sizeof( SubstateMapStruct );
   while ( i < num )
   {
      if ( (    !rbcPlasmaProcedure
                || SubstateMapTable[i].TrimaID >= SS_RP_CHANNEL_SETUP )

           && ( id == SubstateMapTable[i].AlgorithmID )
           )
      {
         return SubstateMapTable[i].AlgorithmStr;
      }
      i++;
   }
   return 0;
}

const char* SubstateMap::ssStr (int id)
{
   int i   = 0;
   int num = sizeof( state_name_table ) / sizeof( the_state_struct );
   while ( i < num )
   {
      if ( id == state_name_table[i].state )
      {
         return state_name_table[i].state_text;
      }
      i++;
   }
   return 0;
}

/* FORMAT HASH e207c2292dcbbe91d5baa3bfc2c78961 */
