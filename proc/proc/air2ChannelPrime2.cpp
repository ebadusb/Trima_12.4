/*
 * Copyright (c) 2011 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      air2ChannelPrime2.cpp
 *
 */

#include "air2ChannelPrime2.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "CatalogChecker.h"
//

DEFINE_STATE(Air2ChannelPrime2);

Air2ChannelPrime2::Air2ChannelPrime2()
   : StateAbs(),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _alarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2ChannelPrime2::Air2ChannelPrime2(const Air2ChannelPrime2& state)
   : StateAbs(state),
     _transition(false),
     _airEvacMitigation(false),
     _currentReturnVolm(0.0f),
     _alarm(FAILURE_TO_PRESSURIZE_INLET)
{}

Air2ChannelPrime2::~Air2ChannelPrime2()
{}

int Air2ChannelPrime2::transitionStatus ()
{
   ProcData pd;


   const bool setIsDrbc     = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::DRBC_BAG);
   const bool setIsRbc      = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::RBC_BAG);
   const bool setHasSomeRBC = ( setIsRbc || setIsDrbc );
   if ( !setHasSomeRBC )
   {
      DataLog(log_level_proc_disposable_test_info) << " No RBC bags, state not needed" << endmsg;
      return 1;
   }

   if (  !(Software_CDS::GetInstance().getFeature(AiroutMitigation))  )
   {
      DataLog(log_level_proc_disposable_test_info) << " Air mitigation off" << endmsg;
      return 1;
   }


   if (pd.TrimaSet().Cassette == cassette::RBCPLS)
   {
      DataLog(log_level_proc_info) << "No Air2ChannelPrime2 on Black stamp set -- skipping Air2Channel" << endmsg;
      return 1;
   }


   if (_transition)
   {
      return 1;
   }
   return NO_TRANSITION;
}

int Air2ChannelPrime2::preProcess ()
{
   ProcData pd;
   pd.Orders().Qrp(-110.0f);

   return NORMAL;
}

int Air2ChannelPrime2::postProcess ()
{
   ProcData pd;

   _currentReturnVolm += pd.Status().ReturnPump.dV();

   if ( pd.Status().APS() <= 50.0f )
   {
      _transition = true;
      pd.Orders().Qrp(0.0f);

      DataLog(log_level_proc_disposable_test_info) << "Air2ChannelPrime2 COMPLETED at " << pd.Status().APS() << endmsg;

   }

   // Qrp is running negative direction so we need abs value here
   // it13438
   if (fabs(_currentReturnVolm) > 115.0f)
   {
      _alarm.setAlarm();
      _currentReturnVolm = 0.0f;
   }


   return NORMAL;
}

int Air2ChannelPrime2::preEnter ()
{
   return NORMAL;
}

int Air2ChannelPrime2::postExit ()
{
   ProcData pd;

   pd.Orders().Qrp(0.0f);
   _currentReturnVolm = 0.0f;

   return NORMAL;
}

void Air2ChannelPrime2::copyOver (const Air2ChannelPrime2&)
{}

void Air2ChannelPrime2::cleanup ()
{}

/* FORMAT HASH c47de186c02d8ad7fc32bde322e601a7 */
