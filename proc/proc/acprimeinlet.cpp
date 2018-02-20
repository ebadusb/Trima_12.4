/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acprimeinlet.cpp
 *
 */

#include "acprimeinlet.h"
#include "procdata.h"
#include "volumeacc.h"
#include "cobeconfig.h"

DEFINE_STATE(ACPrimeInlet);

ACPrimeInlet::ACPrimeInlet()
   : StateAbs(),
     _AccumulatedTime(), _ClearLineTimer(),
     _PrimeFailureAlarm(PRIME_FAILURE),
     _ACLevelAlarm(AC_LEVEL_ALARM),
     _ACVolume(0.0f),
     _ACSeen(0),
     _PrimeAttempts(0),
     waitDone(false),
     runPauseTimer(false)
{}

ACPrimeInlet::ACPrimeInlet(const ACPrimeInlet& state)
   : StateAbs(state),
     _AccumulatedTime(), _ClearLineTimer(),
     _PrimeFailureAlarm(PRIME_FAILURE),
     _ACLevelAlarm(AC_LEVEL_ALARM),
     _ACVolume(state._ACVolume),
     _ACSeen(state._ACSeen),
     _PrimeAttempts(state._PrimeAttempts),
     waitDone(state.waitDone),
     runPauseTimer(state.runPauseTimer)
{
   ACPrimeInlet::copyOver(state);
}

ACPrimeInlet::~ACPrimeInlet()
{
   ACPrimeInlet::cleanup();
}

int ACPrimeInlet::transitionStatus ()
{
   ProcData pd;

   //
   // Check for AC line primed after the AC is seen.
   //
   if (    pd.TrimaSet().CassetteState.Get() >= ProcTrimaSet_CDS::ACPRIMED
           || ( _ACVolume >= CobeConfig::data().ACPrimeVolume && _ACSeen )
           )
   {
      DataLog(log_level_proc_ac_prime_info) << "AC Prime Completed " << _PrimeAttempts << endmsg;
      return 1;
   }

   return NO_TRANSITION;
}

void ACPrimeInlet::primeTooLong ()
{
   //
   // Set the prime alarm ...
   //
   _PrimeFailureAlarm.setAlarm();
   DataLog(log_level_proc_ac_prime_info) <<  "Set Prime Failure Alarm" << PRIME_FAILURE << endmsg;
}

int ACPrimeInlet::preProcess ()
{
   ProcData pd;

   //
   // Increment the AC volume with the delta ...
   //
   _ACVolume += pd.Status().ACPump.dV();

   //
   // Check to see if AC is seen at sensor.
   // If the sensor does not see fluid before 4.0 mls have been
   // pumped, then the AC level alarm should be set and the prime
   // should be restarted.
   //
   if ( !_ACSeen )
   {
      if ( ( _ACVolume ) >= 4.0f )
      {
         //
         // If we have already failed this test once.
         //
         if ( _PrimeAttempts >= 1 )
         {
            //
            // AC line prime failed! - set PRIME alarm
            //
            if (_PrimeFailureAlarm.getState() == CLEARED)
            {
               _PrimeFailureAlarm.setAlarm();
               DataLog(log_level_proc_ac_prime_info) << "Set Prime Failure Alarm " << PRIME_FAILURE << endmsg;
            }
         }
         else
         {
            //
            // Increment the number of attempts to pass this test, and
            //  save the volumes for the next pass
            //
            _PrimeAttempts++;
            saveValues();

            //
            // Send it to the alarm handler.
            //
            _ACLevelAlarm.latchAlarm();
            DataLog(log_level_proc_ac_prime_info) << "Set Prime Failure Alarm " << AC_LEVEL_ALARM << endmsg;
         }

      }
      else if ( pd.Status().ACDetectFluid() )
      {
         //
         // Report that AC has been seen at the sensor.
         //
         _ACSeen = 1;
         DataLog(log_level_proc_ac_prime_info) << "AC has been seen at the sensor" << endmsg;

         //
         // Set the AC volume to zero because this is
         // where the AC is really being started.
         //
         VolumeAcc::ClearVac();
         VolumeAcc::ClearVacTotal();
         _ACVolume = 0.0f;
         DataLog(log_level_proc_ac_prime_info) << "Setting AC volumes to 0.0" << endmsg;
      }
   }

   //
   // After AC is seen for the first time you should not lose the signal.
   //  If there is no signal after 5ml, then set AC level alarm
   //  (The 5.0 mls is because the air in the line cause the ac sensor
   //   to see fluid and then air and then fluid... until about 5mls
   //   after first fluid is seen.)
   //
   // If the alarm is unlatched for the first time when AC is seen
   // and operator does not continue, there can be a case where air is detected again
   // or the line gets pulled off the sensor, just latch the alarm then
   else if ( !pd.Status().ACDetectFluid() &&
            (( _ACVolume >= 5.0f ) || (_ACSeen && _ACVolume == 0.0f)) &&
              _ACLevelAlarm.getState() != LATCHED )
   {
      //
      // Latch the alarm
      //
      _ACLevelAlarm.latchAlarm();
      DataLog(log_level_proc_ac_prime_info) << "Set AC Level Alarm " << AC_LEVEL_ALARM << endmsg;
   }
   else if ( pd.Status().ACDetectFluid() && _ACLevelAlarm.getState() == LATCHED )
   {
      DataLog(log_level_proc_ac_prime_info) << "AC detected, unlatching " << AC_LEVEL_ALARM << endmsg;
      _ACLevelAlarm.unlatchAlarm();
   }


   //
   if (_ACSeen && (_ACVolume >= 5.0f) && !waitDone)
   {
      runPauseTimer = true;
   }

   return NORMAL;
}

int ACPrimeInlet::postProcess ()
{
   ProcData pd;

   //
   // Command the pumps
   //
   if  (!waitDone)
   {
      if ( !runPauseTimer)
      {
         startPumps();
      }
      else
      {
         pd.Orders().Qin(0.0f);
         pd.Orders().Qac(0.0f);
      }
   }
   else
   {
      startPumpsAt30();
   }

   //
   if (_ACSeen && runPauseTimer)
   {
      _ClearLineTimer.activate();

      if (_ClearLineTimer.getSecs() > CobeConfig::data().MssACprimePause)
      {
         DataLog(log_level_proc_ac_prime_info) << "Line Clear Time: " << CobeConfig::data().MssACprimePause  << " seconds up!" << endmsg;
         waitDone      = true;
         runPauseTimer = false;
         _ClearLineTimer.inactivate();
      }

      return NORMAL;
   }


   //
   // Activate the time in a non-paused state ...
   //
   if ( !pd.AlarmActive() )
      _AccumulatedTime.activate();
   else
      _AccumulatedTime.inactivate();

   //
   // Check the total time trying to prime ...
   //
   if ( _AccumulatedTime.getSecs() > 45.0f ) // 45 seconds of priming
   {
      //
      // Set the prime alarm ...
      //
      primeTooLong();
   }

   return NORMAL;
}

int ACPrimeInlet::preEnter ()
{
   //
   // Initialize the timer ...
   //
   _AccumulatedTime.init();
   _ClearLineTimer.init();
   _ClearLineTimer.inactivate();

   //
   // Reset volumes ...
   //
   resetVolumes();

   //
   // Get the initial volumes ...
   //
   saveValues();

   //
   // Start the pumps
   //
   startPumps();

   return NORMAL;
}

int ACPrimeInlet::postExit ()
{
   ProcData pd;

   //
   // AC line primed - stop the pumps
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   DataLog(log_level_proc_ac_prime_info) << "Stopping inlet and AC pumps" << endmsg;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void ACPrimeInlet::resetVolumes ()
{
   //
   // Reset pump volumes
   //
   VolumeAcc::Clear();
   DataLog(log_level_proc_ac_prime_info) << "Resetting all pump volumes to 0.0" << endmsg;
}

void ACPrimeInlet::saveValues ()
{
   //
   // Reset the AC volume
   //
   _ACVolume = 0.0f;
   DataLog(log_level_proc_ac_prime_info) << "AC volume set to 0.0f" << endmsg;
}

void ACPrimeInlet::startPumps ()
{
   ProcData pd;

   //
   // Start the pumps
   //
   pd.Orders().Qin(50.0f);
   pd.Orders().Qac(50.0f);

}
void ACPrimeInlet::startPumpsAt30 ()
{
   ProcData pd;

   //
   // Start the pumps
   //
   pd.Orders().Qin(30.0f);
   pd.Orders().Qac(30.0f);

}

void ACPrimeInlet::copyOver (const ACPrimeInlet&)
{}

void ACPrimeInlet::cleanup ()
{}

/* FORMAT HASH a858373fbd0d54579e4addff3febc591 */
