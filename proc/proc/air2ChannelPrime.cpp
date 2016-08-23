/*
 * Copyright (c) 2011 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      air2ChannelPrime.cpp
 *
 */

#include "air2ChannelPrime.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "CatalogChecker.h"
//

DEFINE_STATE(Air2ChannelPrime);

Air2ChannelPrime::Air2ChannelPrime()
   : StateAbs(),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _alarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2ChannelPrime::Air2ChannelPrime(const Air2ChannelPrime& state)
   : StateAbs(state),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _alarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2ChannelPrime::~Air2ChannelPrime()
{}

int Air2ChannelPrime::transitionStatus ()
{
   ProcData   pd;

   const bool setIsDrbc     = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::DRBC_BAG);
   const bool setIsRbc      = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::RBC_BAG);
   const bool setHasSomeRBC = ( setIsRbc || setIsDrbc );
   if ( !setHasSomeRBC )
   {
      DataLog(log_level_proc_disposable_test_info) << " No RBC bags, state not needed" << endmsg;
      return 1;
   }

   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation off" << endmsg;
      return 1;
   }


   if (pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      DataLog(log_level_proc_info) << "No Air2ChannelPrime on Black stamp set -- skipping Air2Channel" << endmsg;
      return 1;
   }


   if (_transition)
   {
      return 1;
   }
   return NO_TRANSITION;
}

int Air2ChannelPrime::preProcess ()
{
   ProcData pd;
   pd.Orders().Qrp(110.0f);

   return NORMAL;
}

int Air2ChannelPrime::postProcess ()
{
   ProcData pd;

   _currentReturnVolm += pd.Status().ReturnPump.dV();

   if ( pd.Status().APS() >= 50.0f )
   {
      _transition = true;
      pd.Orders().Qrp(0.0f);

   }


   if (_currentReturnVolm > 115.0f)
   {
      _alarm.setAlarm();
      _currentReturnVolm = 0.0f;
   }


   return NORMAL;
}

int Air2ChannelPrime::preEnter ()
{
   return NORMAL;
}

int Air2ChannelPrime::postExit ()
{
   ProcData pd;

   pd.Orders().Qrp(0.0f);
   _currentReturnVolm = 0.0f;

   return NORMAL;
}

void Air2ChannelPrime::copyOver (const Air2ChannelPrime&)
{}

void Air2ChannelPrime::cleanup ()
{}

/* FORMAT HASH 7a6eb8cba7accaf06d06a271cd3bf126 */
