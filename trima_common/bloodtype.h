/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodtype.h
 *             blood type enumerations
 *
 ******************************************************************************/


#if !defined( INCLUDE_BLOODTYPE )
#define INCLUDE_BLOODTYPE

#include "datalog.h"

typedef enum
{
   BT_UNKNOWN             = 0,
   BT_O_NEG               = 2,
   BT_O_POS               = 4,
   BT_AB_NEG              = 8,
   BT_AB_POS              = 16,
   BT_A_NEG               = 32,
   BT_A_POS               = 64,
   BT_B_NEG               = 128,
   BT_B_POS               = 256,

   BT_OP_ON               = BT_O_POS  + BT_O_NEG,
   BT_ON                  = BT_O_NEG,
   BT_OP_ON_AP_AN         = BT_O_POS  + BT_O_NEG + BT_A_POS + BT_A_NEG,
   BT_OP_ON_AN            = BT_O_POS  + BT_O_NEG + BT_A_NEG,
   BT_ON_AN               = BT_O_NEG  + BT_A_NEG,
   BT_OP_ON_BP_BN         = BT_O_POS  + BT_O_NEG + BT_B_POS + BT_B_NEG,
   BT_OP_ON_AP_AN_BP_BN   = BT_O_POS  + BT_O_NEG + BT_A_POS + BT_A_NEG + BT_B_POS  + BT_B_NEG,
   BT_OP_ON_AP_AN_BN      = BT_O_POS  + BT_O_NEG + BT_A_POS + BT_A_NEG + BT_B_NEG,
   BT_AP_AN               = BT_A_POS  + BT_A_NEG,
   BT_AP_AN_BP_BN_ABP_ABN = BT_A_POS  + BT_A_NEG + BT_B_POS + BT_B_NEG + BT_AB_POS + BT_AB_NEG,
   BT_BP_BN               = BT_B_POS  + BT_B_NEG,
   BT_ABP_ABN             = BT_AB_POS + BT_AB_NEG,

   BT_ALL                 = BT_O_NEG + BT_O_POS + BT_AB_NEG + BT_AB_POS +
                            BT_A_NEG + BT_A_POS + BT_B_NEG + BT_B_POS
} BLOODTYPE;

DataLog_Stream& operator << (DataLog_Stream& os, const BLOODTYPE bt);


#define MAX_BLOOD_TYPE_STRLEN 64
//
// BloodTypeString
// This function converts a blood type enum into a string
// BT_O_NEG  = O-
// BT_AB_POS = AB+
// BT_UNKNOWN = ' '
//
// Null is returned for unknown values
//
// NOTE.  This string assumes you will define a string of size MAX_BLOOD_TYPE_STRLEN and that
// the string is 0'ed before calling function.  Bad things happen if you don't.
//
//
void BloodTypeString (BLOODTYPE bt, char* btString);

#endif

/* FORMAT HASH 50347ec2d87dc7a81c0ee1ca020d2219 */
