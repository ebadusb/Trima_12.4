/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      pressinletline.cpp
 *
 */

#include "pressinletline.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "CatalogChecker.h"
#include "math.h"

#include "MssCommon.h"

DEFINE_STATE(PressInletLine);

PressInletLine::PressInletLine()
   : StateAbs(),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InletPressureAlarmPossibleWrongSet(FAILURE_TO_PRESSURIZE_INLET_RAS_SET),
     _ACVolume(0.0f),
     _APSatStart(0.0f),
     _TestPassed(NO_TRANSITION),
     _doDispTest_2(false),
     _doDispTest_1(false),
     _CassetteInvalidForTest(CASSETTE_REF_MISMATCH_ALARM),
     _unloadNow(false),
     _MSSRBCset(false), _secondaryACVolume(0.0), _runHoldTest(false), _APSatHoldTestStart(false),
     _latched(false)
{}

PressInletLine::PressInletLine(const PressInletLine& state)
   : StateAbs(state),
     _InletPressureAlarm(FAILURE_TO_PRESSURIZE_INLET),
     _InletPressureAlarmPossibleWrongSet(FAILURE_TO_PRESSURIZE_INLET_RAS_SET),
     _ACVolume(state._ACVolume),
     _APSatStart(state._APSatStart),
     _TestPassed(state._TestPassed),
     _doDispTest_2(false),
     _doDispTest_1(false),
     _CassetteInvalidForTest(CASSETTE_REF_MISMATCH_ALARM),
     _unloadNow(false),
     _MSSRBCset(false), _secondaryACVolume(0.0), _runHoldTest(false), _APSatHoldTestStart(false),
     _latched(false)
{
   PressInletLine::copyOver(state);
}

PressInletLine::~PressInletLine()
{
   PressInletLine::cleanup();
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////    transitionStatus()
////////////////////////////////////////////////////////////////////////////////////////
int PressInletLine::transitionStatus ()
{
   ProcData pd;

   int      retVal = NO_TRANSITION;

   // transition to disp test 1 or Disp test 2............................
   if (_TestPassed == 1)
   {
      if (_doDispTest_1)
      {
         retVal = 1;
      }
      if (_doDispTest_2)
      {
         retVal = 2;
      }
   }

   // transition to unload cassette......................................
   if (_unloadNow && (_CassetteInvalidForTest.getState() != ACTIVE))
   {

      pd.MakeRunWritable();
      pd.Run().CassetteFunctionBits.Set(0);    // reset the funtion bits because all bets are off now
      pd.MakeRunReadable();

      retVal = 3;
   }

   return retVal;
}


////////////////////////////////////////////////////////////////////////////////////////
///////////////    preProcess()
////////////////////////////////////////////////////////////////////////////////////////
int PressInletLine::preProcess ()
{
   ProcData pd;

   if (pd.Status().APS() < CobeConfig::data().DispTestAPSThreshold)
   {
      if (_InletPressureAlarm.getState() == LATCHED)
      {
         DataLog(log_level_proc_disposable_test_info)
            << "UN-Latching alarm at "
            << pd.Status().APS()
            << "mmHg" << endmsg;
         _latched = false;
         _InletPressureAlarm.unlatchAlarm();
      }

      if (_InletPressureAlarmPossibleWrongSet.getState() == LATCHED)
      {
         DataLog(log_level_proc_disposable_test_info)
            << "UN-Latching alarm at "
            << pd.Status().APS()
            << "mmHg" << endmsg;
         _latched = false;
         _InletPressureAlarmPossibleWrongSet.unlatchAlarm();
      }
   }

   //
   //
   //   If currently in an inlet pressure alarm condition, reset the initial APS
   //   and AC volume for the next test attempt.
   if (pd.AlarmActive() && (_CassetteInvalidForTest.getState() != ACTIVE) )
   {
      // in alarm condition now
      _ACVolume          = 0.0f;
      _APSatStart        = pd.Status().APS();
      _secondaryACVolume = 0.0;

      if (pd.Status().APS() > CobeConfig::data().DispTestAPSThreshold)
      {
         if (_InletPressureAlarm.getState() == ACTIVE)
         {
            _latched = true;
            _InletPressureAlarm.latchAlarm();
         }

         if (_InletPressureAlarmPossibleWrongSet.getState() == ACTIVE)
         {
            _latched = true;
            _InletPressureAlarmPossibleWrongSet.latchAlarm();
         }
      }
      //
      //
      // We are currently running the test.  Check the APS to see if the
      // reading meets the "pass test" requirement.
      //
   }
   else
   {

      //
      // Start the AC pump to try to drive the inlet pressure
      // sensor down in pressure.
      //
      if (!pd.AlarmActive())
      {
         pd.Orders().Qac(CobeConfig::data().QinLimitMax);
      }

   }

   return NORMAL;
}

////////////////////////////////////////////////////////////////////////////////////////
///////////////    postProcess()
////////////////////////////////////////////////////////////////////////////////////////
int PressInletLine::postProcess ()
{
   ProcData pd;

   _ACVolume += pd.Status().ACPump.dV();

   if (_runHoldTest)
   {
      _secondaryACVolume += pd.Status().ACPump.dV();
      float deltaAPD_hold = pd.Status().APS() - _APSatHoldTestStart;

      DataLog(log_level_proc_disposable_test_info)
         << "Hold pressure test ->"
         << pd.Status().APS()
         << " mmHg - baseline pressure ->"
         << _APSatHoldTestStart
         << " delta ->"
         << deltaAPD_hold
         << endmsg;

      // if we goto 400 during the hold test then we are a regular set
      if (pd.Status().APS() >= CobeConfig::data().DispTestApsMax)
      {
         if ( _MSSRBCset)
         {
            DataLog(log_level_proc_disposable_test_info)
               << "ERROR -=- Unclamp Crossover Line"
               << endmsg;

            _TestPassed  = 0;
            _runHoldTest = false;    // start over
            // _unloadNow = 1;
            if (!pd.AlarmActive())
            {
               _InletPressureAlarmPossibleWrongSet.setAlarm();
            }

         }
         else       // Not MSS set


         {
            DataLog(log_level_proc_disposable_test_info)
               << "PASS! Disposable test 1 to follow: AC Vol ->"
               << _ACVolume  << "  APS Pressure ->" << pd.Status().APS()
               << endmsg;

            _doDispTest_2 = false;
            _doDispTest_1 = true;
            _TestPassed   = 1;
         }
      }

      if (_ACVolume > CobeConfig::data().DispTestCrackpressureVolm )
      {
         DataLog(log_level_proc_disposable_test_info)
            << "unable to pressurize. only ->"
            << deltaAPD_hold
            << "mmHg @"
            << _ACVolume
            << "mL of AC pump"
            << endmsg;

         _TestPassed  = 0;
         _runHoldTest = false;   // start over
         // _unloadNow = 1;
         if (!pd.AlarmActive())
         {
            _InletPressureAlarm.setAlarm();
         }
      }

      // 11540 added:
      if  ( (pd.Status().APS() > CobeConfig::data().DispTestCrossoverHigh )
            && ( _MSSRBCset) )
      {
         DataLog(log_level_proc_disposable_test_info)
            << "APS exceeds crack pressure high during hold check ->"
            << pd.Status().APS()
            << "mmHg @"
            << endmsg;

         _TestPassed  = 0;
         _runHoldTest = false;   // start over
         // _unloadNow = 1;
         if (!pd.AlarmActive())
         {
            _InletPressureAlarmPossibleWrongSet.setAlarm();
         }
      }


      // if you fall out of the test delta range....
      if (pd.Status().APS() < CobeConfig::data().DispTestCrossoverLow)
      {

         DataLog(log_level_proc_disposable_test_info)
            << "unable to pressurize. pressure fell off in Hold Test! only ->"
            << deltaAPD_hold
            << "mmHg @"
            << endmsg;

         _TestPassed  = 0;
         _runHoldTest = false;   // start over
         // _unloadNow = 1;
         if (!pd.AlarmActive())
         {
            _InletPressureAlarm.setAlarm();
         }
      }



      if (  fabs(deltaAPD_hold) <= CobeConfig::data().DispTestCrossoverDelta )
      {

         if ( _secondaryACVolume >= CobeConfig::data().DispTestVolumeCheck )
         {
            if (_MSSRBCset)
            {
               DataLog(log_level_proc_disposable_test_info)
                  << "PASS! Disposable test 2 to follow: AC Vol/SecondAC volm ->"
                  << _ACVolume
                  << "/"
                  << _secondaryACVolume
                  << endmsg;

               _doDispTest_2 = true;
               _doDispTest_1 = false;
               _TestPassed   = 1;

            }
            else
            {

               DataLog(log_level_proc_disposable_test_info) << "Settype invalid for this test"
                                                            << endmsg;
               _TestPassed = 0;
               _unloadNow  = 1;

               if (!pd.AlarmActive())
               {
                  _CassetteInvalidForTest.setAlarm();
               }
            }

         }
         else
         {

            DataLog(log_level_proc_disposable_test_info) << "Still Checking the running Volm vs Pressure ("
                                                         << _secondaryACVolume
                                                         << " vs. "
                                                         << deltaAPD_hold
                                                         << ")"
                                                         << endmsg;
         }


      }
      else
      {

         DataLog(log_level_proc_disposable_test_info) << " pressure still changing ->"
                                                      << deltaAPD_hold
                                                      << "mmHg @"
                                                      << _secondaryACVolume
                                                      << "mL of running AC volm -- Retry"
                                                      << endmsg;

         _secondaryACVolume  = 0.0;
         _APSatHoldTestStart = pd.Status().APS();

      }

      ///////////////  MAIN TEST STARTS HERE ////////////////////////////////
   }
   else     // if (!_runHoldTest)

   {   // don't do tests if in a latched condition it causes superfluous alarms
      if (_latched)
      {
         return NORMAL;
      }

      if ((pd.Status().APS() >= CobeConfig::data().DispTestApsMax))
      {

         if ( !_MSSRBCset)
         {
            _doDispTest_2 = false;
            _doDispTest_1 = true;
            _TestPassed   = 1;

            DataLog(log_level_proc_disposable_test_info) << "PASS! Disposable test 1 to follow: AC Vol ->"
                                                         << _ACVolume
                                                         << "; APS -> "
                                                         <<  pd.Status().APS()
                                                         << endmsg;
         }
         else
         {

            DataLog(log_level_proc_disposable_test_info)
               << "RAS set crossover clamp error?"
               << " APS -> "
               <<  pd.Status().APS()
               << endmsg;

            _TestPassed = 0;
            // _unloadNow = 1;
            if (!pd.AlarmActive())
            {
               _InletPressureAlarmPossibleWrongSet.setAlarm();
            }

         }

      }
      else         // not reached APS of  DispTestApsMax

      {
         if ( _ACVolume >= CobeConfig::data().DispTestVolumeFail )
         {
            if (!_MSSRBCset)
            {

               DataLog(log_level_proc_disposable_test_info)
                  << "ERROR -=- Failure to Pressurize Inlet"
                  << endmsg;

               _TestPassed = 0;
               if (!pd.AlarmActive())
               {
                  _InletPressureAlarm.setAlarm();
               }
            }
         }

         if (_ACVolume >= CobeConfig::data().DispTestCrackpressureVolm )
         {

            DataLog(log_level_proc_disposable_test_info)
               << "unable to pressurize. only ->"
               << pd.Status().APS()
               << "mmHg @"
               << _ACVolume
               << "mL of AC pump"
               << endmsg;

            _TestPassed = 0;
            if (!pd.AlarmActive())
            {
               if (!_MSSRBCset)
               {
                  _InletPressureAlarm.setAlarm();
               }
               else
               {
                  _InletPressureAlarmPossibleWrongSet.setAlarm();
               }
            }
         }


         if (    (pd.Status().APS() > CobeConfig::data().DispTestCrossoverLow )
                 && (pd.Status().APS() < CobeConfig::data().DispTestCrossoverHigh)
                 )
         {
            if (!_runHoldTest)
            {
               DataLog(log_level_proc_disposable_test_info)
                  << "initiating Hold Pressure test. "
                  << pd.Status().APS()
                  << " mmHg"
                  << endmsg;

               _runHoldTest = true;

               // get new starting pressure
               _APSatHoldTestStart = pd.Status().APS();
               _secondaryACVolume  = 0.0;
            }

         }


      }    // end if ((deltaAPS >= ...

   }
   return NORMAL;
}


////////////////////////////////////////////////////////////////////////////////////////
///////////////    preEnter()
////////////////////////////////////////////////////////////////////////////////////////
int PressInletLine::preEnter ()
{
   ProcData pd;

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);


   //
   // Make sure valves are returning
   //
   pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VRBC, HAL_VALVE_RETURN);
   pd.Orders().ValveCmd(HAL_VPLASMA, HAL_VALVE_RETURN);     //

   //
   // Get the current ac volume pumped reading ...
   //
   _ACVolume           = 0.0f;
   _APSatStart         = pd.Status().APS();
   _TestPassed         = NO_TRANSITION;
   _runHoldTest        = false;
   _secondaryACVolume  = 0.0f;
   _APSatHoldTestStart = 0.0f;
   _latched            = false;
   _MSSRBCset          = CatalogChecker::checkBit(pd.Run().CassetteFunctionBits.Get(), CatalogChecker::MSS_RBC);
   DataLog(log_level_proc_info) << "is this MSS RBC set ->" << _MSSRBCset << endmsg;

   return NORMAL;
}


////////////////////////////////////////////////////////////////////////////////////////
///////////////    postExit()
////////////////////////////////////////////////////////////////////////////////////////
int PressInletLine::postExit ()
{
   ProcData pd;

   //
   // Make sure pumps are stopped
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qrp(0.0f);



   _doDispTest_2       = false;
   _doDispTest_1       = false;
   _unloadNow          = false;

   _latched            = false;
   _ACVolume           = 0.0f;
   _secondaryACVolume  = 0.0f;
   _runHoldTest        = false;
   _APSatHoldTestStart = 0.0f;


   //
   // Reset the test passed flag
   //
   _TestPassed = NO_TRANSITION;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void PressInletLine::copyOver (const PressInletLine&)
{}

void PressInletLine::cleanup ()
{}

/* FORMAT HASH c4df7e093f1b46736b27167db31cd37d */
