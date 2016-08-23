/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      substatemap.h
 *
 *             Maps the algorithm spreadsheet and document names to the
 *              Trima enumerations.
 *
 ******************************************************************************/


#if  !defined( INCLUDE_SUBSTATEMAP )
#define INCLUDE_SUBSTATEMAP

#include "states.h"

class SubstateMap
{
public:

   // Obtain a algorithm# if given its Trima enum.  -1 returned if not found.
   static int alg (const State_names enumid, const bool rbcPlasmaProcedure = false);

   // Obtain the substate type name given the Algorithm#. INVALID_SUBSTATE returned if not found.
   static State_names ss (const int algid, const bool rbcPlasmaProcedure = false);
   static State_names ss (const char* str);

   // Obtain the substate type name given the Algorithm#. 0 return if not found.
   static const char* algStr (int id, const bool rbcPlasmaProcedure = false);
   static const char* ssStr (int id);

};

#if defined( SUBSTATE_MAP_CPP )

typedef struct
{
   State_names TrimaID;
   int         AlgorithmID;
   char*       AlgorithmStr;
} SubstateMapStruct;


static SubstateMapStruct SubstateMapTable[] =
{

   { NOCOLLECT_STATE,         0,   "No_Collect" },
   { SS_CHANNEL_SETUP,        1,   "Channel_setup" },
   { SS_PREPLATELET_PLASMA,   2,   "Preplatelet_plasma" },
   { SS_PREPLATELET_NOPLASMA, 3,   "Preplatelet_noplasma" },
   { SS_PLATELET_PLASMA,      4,   "Platelet_Plasma" },
   { SS_PCA,                  5,   "PCA" },
   { SS_MIDRUN,               6,   "Midrun" },
   { SS_PIR_PLASMA,           7,   "PIR_plasma" },
   { SS_PIR_NOPLASMA,         8,   "PIR_noplasma" },
   { SS_EXTENDED_PCA,         9,   "Extended_PCA" },
   { SS_EXTENDED_PLASMA,      10,  "Extended_plasma" },
   { SS_RBC_PTF_SETUP_1,      11,  "RBC_PTF_setup_1" },
   { SS_RBC_PTF_SETUP_2,      12,  "RBC_PTF_setup_2" },
   { SS_RBC_PTF_PRIME,        13,  "RBC_PTF_prime" },
   { SS_RBC,                  14,  "RBC" },

   { SS_RP_CHANNEL_SETUP,     1,   "RP_Channel_setup" },
   { SS_RP_PLASMA_ONLY,       2,   "RP_Plasma" },
   { SS_RP_RBC_PTF_SETUP_1,   3,   "RP_RBC_PTF_setup_1" },
   { SS_RP_RBC_PTF_SETUP_2,   4,   "RP_RBC_PTF_setup_2" },
   { SS_RP_RBC,               5,   "RP_RBC" },
   { SS_RP_RBC_PTF_SETUP_1_DOUBLE,   6,   "RP_RBC_PTF_setup_1_Double" },
   { SS_RP_RBC_PTF_SETUP_2_DOUBLE,   7,   "RP_RBC_PTF_setup_2_Double" },
   { SS_RP_RBC_DOUBLE,               8,   "RP_RBC_Double" },
   { INVALID_SUBSTATE,        -1,  "INVALID_SUBSTATE" }
};

#endif

#endif

/* FORMAT HASH a45ecb6c88b4604f4a114b52d5dcfb78 */
