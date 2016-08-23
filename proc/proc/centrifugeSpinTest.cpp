/*
 * Copyright (c) 2009 by CaridianBCT BCT, Inc.  All rights reserved.
 *
 * TITLE:      centrifugeSpinTest.h
 *
 *     This state performs the centrifuge spin test.
 */

#include "centrifugeSpinTest.h"
#include "states.h"
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(CentrifugeSpinTest);

#define WITHIN_ONE_PERCENT  (0.99f * CobeConfig::data().DispTestTestSpinSpeed)

CentrifugeSpinTest::CentrifugeSpinTest()
   : StateAbs(),
     _runTime(1000 * (int)CobeConfig::data().DispTestTestSpinTime),
     _centSpinTimer(0, Callback<CentrifugeSpinTest>(this, &CentrifugeSpinTest::stopTest), TimerMessage::DISARMED),
     _transitionNow(false),
     _thisTestIsComplete(false)
{}

CentrifugeSpinTest::~CentrifugeSpinTest()
{}


int CentrifugeSpinTest::preEnter ()
{
   // DataLog( log_level_proc_info ) << "CentrifugeSpinTest :: preEnter()" << endmsg;

   _transitionNow      = false;
   _thisTestIsComplete = false;

   return NORMAL;
}


int CentrifugeSpinTest::preProcess ()
{
   // DataLog( log_level_proc_info ) << "CentrifugeSpinTest :: preProcess()" << endmsg;
   ProcData pd;
   if (!_thisTestIsComplete)
   {
      pd.Orders().LockDoor();
      pd.Orders().RPM(CobeConfig::data().DispTestTestSpinSpeed);

      if ( !pd.Status().PS64Vsw.Acceptable()
           && pd.Status().CmdCentrifugeRPM() >= 1.0f )
      {
         DataLog(log_level_proc_info) << "Requesting 64V Power On" << endmsg;
         RequestSafetyPowerMsg requestPwrMsg(MessageBase::SEND_GLOBAL);
         requestPwrMsg.send(ENABLE_64V_POWER);
      }
   }
   return NORMAL;
}


int CentrifugeSpinTest::postProcess ()
{
   // DataLog( log_level_proc_info ) << "CentrifugeSpinTest :: postProcess()" << endmsg;
   ProcData pd;
   if ( pd.Status().CentrifugeRPM() > WITHIN_ONE_PERCENT )
   {
      if ( TimerMessage::DISARMED == _centSpinTimer.timerArmed() )
      {
         _centSpinTimer.interval(_runTime);
         DataLog(log_level_proc_info) << "Start Timer::" << endmsg;
      }
   }
   return NORMAL;
}



int CentrifugeSpinTest::postExit ()
{
   DataLog(log_level_proc_info) << "concluding spin test. " << endmsg;

   _centSpinTimer.interval(0);

   ProcData pd;
   pd.Orders().RPM(0.0f);


   // pull power  or this sets off door lock/unlock chain reaching 10328
   RequestSafetyPowerMsg requestPwrMsg(MessageBase::SEND_GLOBAL);
   requestPwrMsg.send(DISABLE_64V_POWER);

   pd.DoorLockTestFinished(); // dont confuse.
   return NORMAL;
}


int CentrifugeSpinTest::transitionStatus ()
{

   // DataLog( log_level_proc_info ) << "CentrifugeSpinTest :: postExit()" << endmsg;

   if (_transitionNow)
   {
      _transitionNow = false;   // for next time.
      return TRANSITION;
   }
   else
      return NO_TRANSITION;
}


void CentrifugeSpinTest::stopTest ()
{
   DataLog(log_level_proc_info) << "Stop Test and End Timer::" << endmsg;
   _transitionNow      = true;
   _thisTestIsComplete = true;
}

/* FORMAT HASH 74ce12782fdccf9a0f446f59d4d252a7 */
