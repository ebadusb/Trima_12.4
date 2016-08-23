/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoverytypes.h
 *             Recovery ids and mappings from strings to ids
 *
 ******************************************************************************/


#if  !defined( INCLUDE_RECOVERYTYPES )
#define INCLUDE_RECOVERYTYPES


class RecoveryTypes
{
public:
   enum InRecovery
   {
      NoRecovery=0,
      CountAll,
      CountTime,
      CountVolume,
      CountNone
   };

   enum Type
   {
      None=-1,

      AirBlock,
      ClearLine,
      PlasmaValveMotion,

      PumpsPause,
      RBCChamberPurge,
      RBCSpillover,
      SalineBolus,
      SalineDrawRecovery,
      SalinePrime,
      SettleChannel,
      WBCChamberPurge,
      PtfCpsHighRecovery,


      CentrifugeSlowScaled,
      CentrifugeStopScaled,
      ClearLineScaled,
      CentrifugeRBCRecovery,

      RecoveryNum // this is the count


   };

   // Obtain a recovery type if given its name.  0 return if not found.
   static Type RecoveryId (const char* str);

   // Obtain the recovery type name given the Id. 0 return if not found.
   static const char* RecoveryStr (Type id);

};

struct RecoveryStatus
{
   RecoveryTypes::Type       _Type;
   RecoveryTypes::InRecovery _InRecovery;
};

#if defined( RECOVERY_TYPES_CPP )

typedef struct
{
   RecoveryTypes::Type RecoveryID;
   char*               RecoveryName;
} RecoveryNameStruct;

static RecoveryNameStruct RecoveryNames[] =
{
   { RecoveryTypes::None,              "None" },
   { RecoveryTypes::AirBlock,          "AirBlock" },
   { RecoveryTypes::CentrifugeSlowScaled, "CentrifugeSlowScaled" },
   { RecoveryTypes::CentrifugeStopScaled, "CentrifugeStopScaled" },
   { RecoveryTypes::CentrifugeRBCRecovery, "CentrifugeRBCRecovery" },
   { RecoveryTypes::ClearLine,         "ClearLine" },
   { RecoveryTypes::ClearLineScaled,      "ClearLineScaled" },
   { RecoveryTypes::PlasmaValveMotion, "PlasmaValveMotion" },
   { RecoveryTypes::PumpsPause,        "PumpsPause" },
   { RecoveryTypes::RBCChamberPurge,   "RBCChamberPurge" },
   { RecoveryTypes::RBCSpillover,      "RBCSpillover" },
   { RecoveryTypes::SalineBolus,       "SalineBolus" },
   { RecoveryTypes::SalineDrawRecovery, "SalineDrawRecovery" },
   { RecoveryTypes::SalinePrime,       "SalinePrime" },
   { RecoveryTypes::SettleChannel,     "SettleChannel" },
   { RecoveryTypes::WBCChamberPurge,   "WBCChamberPurge" },
   { RecoveryTypes::PtfCpsHighRecovery,  "PtfCpsHighRecovery" },
   { RecoveryTypes::RecoveryNum,       "NumberOfRecoveries" }
};

#endif

#endif

/* FORMAT HASH 44cf15e45ea75ad27c5aec2b00738662 */
