/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      negativepresstest.cpp
 *
 */

#include "negativepresstest.h"
#include "run_defs.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "software_cds.h"     // Get Optional sw.dat settings

DEFINE_STATE(NegativePressTest);

NegativePressTest::NegativePressTest()
   : StateAbs(),
     _NegativePressureAlarm(ACCESS_NEGATIVE_ALARM_NOT_REACHED),
     _testVolume(0.0f),
     _airEvacMitigation(false),
     _transition(false)
{


   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
      _airEvacMitigation = true;
   else
      _airEvacMitigation = false;

}

NegativePressTest::NegativePressTest(const NegativePressTest& state)
   : StateAbs(state),
     _NegativePressureAlarm(ACCESS_NEGATIVE_ALARM_NOT_REACHED),
     _testVolume(state._testVolume),
     _airEvacMitigation(state._airEvacMitigation),
     _transition(false)
{
   copyOver(state);
}

NegativePressTest::~NegativePressTest()
{
   cleanup();
}

int NegativePressTest::transitionStatus ()
{
   ProcData pd;

   //
   // Check for negative pressure ...
   //
   if ( _transition )
   {
      return 1;
   }

   return NO_TRANSITION;
}

int NegativePressTest::preProcess ()
{
   ProcData pd;

   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);

   if (_airEvacMitigation)
   {
      pd.Orders().Qrp( (-1.0) * CobeConfig::data().MaxInstantQreturn);
      pd.Orders().Qin(0.0f);
   }
   else
   {
      pd.Orders().Qrp(0.0f);
      pd.Orders().Qin(CobeConfig::data().QinLimitMax);
   }


   return NORMAL;
}

int NegativePressTest::postProcess ()
{
   //
   // Accrue return volume ...
   //   mult by -1 to keep us working with positive volumes. just for ease of bookkeeping
   //
   //
   ProcData pd;
   if (_airEvacMitigation)
      _testVolume += (-1.0) * pd.Status().ReturnPump.dV();
   else
      _testVolume += pd.Status().InletPump.dV();


   if ( _testVolume >= CobeConfig::data().DispTestVolumeFail2 )
   {
      _NegativePressureAlarm.setAlarm();
      _testVolume = 0.0f;
   }



   if ( pd.Status().APS() <= CobeConfig::data().DispTestApsMin )
   {

      DataLog(log_level_proc_disposable_test_info) << "Negative pressure reached ( APS=" << pd.Status().APS() << "mmHg)" << endmsg;
      _transition = true;
   }

   return NORMAL;
}

int NegativePressTest::preEnter ()
{
   //
   // Initialize the inlet volume ...
   //
   _testVolume = 0.0f;

   return NORMAL;
}

int NegativePressTest::postExit ()
{
   ProcData pd;

   //
   // Stop the pump ...
   //
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qin(0.0f);

   _testVolume = 0.0f;
   _transition = false;
   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void NegativePressTest::copyOver (const NegativePressTest&)
{}

void NegativePressTest::cleanup ()
{}

/* FORMAT HASH d48826064108e0c586299824711ce773 */
