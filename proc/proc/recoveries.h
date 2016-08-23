/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      recoveries.h
 *
 */

#ifndef _RECOVERIES_H_
#define _RECOVERIES_H_

#include <recoverytypes.h>

struct RecoveryTransition
{
   RecoveryTransition(RecoveryTypes::Type cur,
                      RecoveryTypes::Type next)
      : _CurrentState(cur),
        _NextState(next)
   { }

   RecoveryTypes::Type _CurrentState;
   RecoveryTypes::Type _NextState;

};

#endif

#ifdef _RECOVERYMAN_CPP_


// very important recoveries

RecoveryTypes::Type RecoveryMan::_Stage1Recoveries[] =
{
   RecoveryTypes::AirBlock,
   RecoveryTypes::CentrifugeSlowScaled,
   RecoveryTypes::CentrifugeStopScaled,
   RecoveryTypes::CentrifugeRBCRecovery,
   RecoveryTypes::ClearLine,
   RecoveryTypes::ClearLineScaled,


   RecoveryTypes::PumpsPause,
   RecoveryTypes::RBCChamberPurge,
   RecoveryTypes::RBCSpillover,

   RecoveryTypes::SalineBolus,
   RecoveryTypes::SalineDrawRecovery,
   RecoveryTypes::SettleChannel,
   RecoveryTypes::WBCChamberPurge,
   RecoveryTypes::PtfCpsHighRecovery,
   RecoveryTypes::None
};


// somewhat important recoveries

RecoveryTypes::Type RecoveryMan::_Stage2Recoveries[] =
{
   RecoveryTypes::PlasmaValveMotion,
   RecoveryTypes::SalinePrime,
   RecoveryTypes::None
};



// these are queued transitions that always occure (no choices)

RecoveryTransition RecoveryMan::_Transitions[] =
{
   RecoveryTransition(RecoveryTypes::AirBlock,          RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::ClearLineScaled),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::None,              RecoveryTypes::None)
};



// these recoveries are added in back of the given recovery
// example:
//      RecoveryTransition( RecoveryTypes::CentrifugeSlow,    RecoveryTypes::RBCSpillover ),
//   the RBCSpillover if needed is queued in back (after) the centrifuge stop

RecoveryTransition RecoveryMan::_QueueableTransitions[] =
{
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,   RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,   RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::RBCChamberPurge),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::SalineDrawRecovery),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::PlasmaValveMotion, RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::ClearLineScaled),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::PtfCpsHighRecovery),
   RecoveryTransition(RecoveryTypes::SalineDrawRecovery, RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::SalineDrawRecovery, RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::SalineDrawRecovery, RecoveryTypes::PtfCpsHighRecovery),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::ClearLineScaled),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::PtfCpsHighRecovery),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::ClearLineScaled),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::SettleChannel),
   RecoveryTransition(RecoveryTypes::None,              RecoveryTypes::None)
};



// these recoveries are added in front of the given recovery
// example:
//   RecoveryTransition( RecoveryTypes::CentrifugeStop,    RecoveryTypes::AirBlock ),
//   the airblock if needed is queued in front of the centrifuge stop

RecoveryTransition RecoveryMan::_StackableTransitions[] =
{
   RecoveryTransition(RecoveryTypes::PumpsPause,        RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::RBCChamberPurge,   RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::PumpsPause,        RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled, RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::PumpsPause,        RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::RBCChamberPurge,   RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::ClearLine),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::ClearLineScaled),
   RecoveryTransition(RecoveryTypes::AirBlock,          RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::PumpsPause,        RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::RBCChamberPurge,   RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::SalineDrawRecovery, RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::SalinePrime,       RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,   RecoveryTypes::PlasmaValveMotion),
   RecoveryTransition(RecoveryTypes::AirBlock,          RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,   RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::SalineBolus),
   RecoveryTransition(RecoveryTypes::AirBlock,          RecoveryTypes::SalineDrawRecovery),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::SalineDrawRecovery),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::SalineDrawRecovery),
   RecoveryTransition(RecoveryTypes::AirBlock,          RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,   RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::SalineBolus,       RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::SalinePrime),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::PumpsPause,        RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::None,              RecoveryTypes::None)
};



// these enrties destroy the recs that are currently in the queue/stack
// example:  RecoveryTransition( RecoveryTypes::CentrifugeStop,    RecoveryTypes::PtfCpsHighRecovery ),
//   if   CentrifugeStop is in the queue and we need a PtfCpsHighRecovery the cent stop is destroyed.

RecoveryTransition RecoveryMan::_QueueConsumingTransitions[] =
{
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::ClearLine,         RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::ClearLineScaled,         RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::RBCSpillover,      RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::AirBlock),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::CentrifugeStopScaled),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::RBCSpillover),
   RecoveryTransition(RecoveryTypes::SettleChannel,     RecoveryTypes::WBCChamberPurge),
   RecoveryTransition(RecoveryTypes::None,              RecoveryTypes::None)
};

// these can not be in a stack or Queue at the same time
// example: (if the first entry is there then the second is forbidden)
//   RecoveryTransition( RecoveryTypes::CentrifugeStop,    RecoveryTypes::CentrifugeSlow ),
//  if we are doing a CentrifugeStop then never allow a CentrifugeSlow in the queue.

RecoveryTransition RecoveryMan::_MutuallyExclusiveRecoveries[] =
{
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::CentrifugeSlowScaled),
   RecoveryTransition(RecoveryTypes::AirBlock,                RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::CentrifugeSlowScaled,    RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::CentrifugeStopScaled,    RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::RBCSpillover,            RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::WBCChamberPurge,         RecoveryTypes::PumpsPause),
   RecoveryTransition(RecoveryTypes::None,                    RecoveryTypes::None)
};

#endif

/* FORMAT HASH d1a81d8eb216543754a8c1286ee8a58b */
