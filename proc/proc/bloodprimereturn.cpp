/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprimereturn.cpp
 *
 */

#include "bloodprimereturn.h"
#include "run_defs.h"
#include <math.h>
#include "procdata.h"
#include "cobeconfig.h"

DEFINE_STATE(BloodPrimeReturn);


BloodPrimeReturn::BloodPrimeReturn()
   : StateAbs(),
     _TooSoonAlarm(LOW_LEVEL_SENSOR_SEEN_TOO_SOON_FAILURE),
     _TooLateAlarm(LOW_LEVEL_SENSOR_SEEN_TOO_LATE_FAILURE),
     _VolumeAfterSensor(0.0f)
{}

BloodPrimeReturn::BloodPrimeReturn(const BloodPrimeReturn& state)
   : StateAbs(state),
     _TooSoonAlarm(LOW_LEVEL_SENSOR_SEEN_TOO_SOON_FAILURE),
     _TooLateAlarm(LOW_LEVEL_SENSOR_SEEN_TOO_LATE_FAILURE),
     _VolumeAfterSensor(state._VolumeAfterSensor)
{
   BloodPrimeReturn::copyOver(state);
}

BloodPrimeReturn::~BloodPrimeReturn()
{
   BloodPrimeReturn::cleanup();
}

int BloodPrimeReturn::transitionStatus ()
{
   static bool firstTime = true;

   //
   // When the appropriate amount has been pumped, ( ie.
   //  after the low level sensor has been reached ) and
   //  the centrifuge is up to speed,
   //  we are finished ...
   //
   if ( ( -1.0 * _VolumeAfterSensor ) >= 5.0f )
   {
      ProcData pd;
      //
      // Stop all pumps ...
      //
      stopPumps();

      const float Vreturn = pd.Volumes().Vreturn.Get();

      if (firstTime && (Vreturn >= CobeConfig::data().LowLevelTooSoonLimit - 5.0f))
      {
         //
         // Set the alarm ...
         //
         _TooSoonAlarm.setAlarm();

         DataLog(log_level_proc_blood_prime_error) << "Low Level Seen Too Soon Alarm set: volume->"
                                                   << Vreturn << endmsg;

         firstTime = false;

         return NO_TRANSITION;
      }

      if (fabs(pd.Status().CentrifugeRPM() - CobeConfig::data().RPMBloodPrime) <= CobeConfig::data().CentrifugeSpeedErrorPrime)
      {
         DataLog(log_level_proc_blood_prime_info) << "Blood Prime finished: volume-> "
                                                  << _VolumeAfterSensor
                                                  << " centrifuge speed-> " << pd.Status().CentrifugeRPM() << endmsg;

         return 1;
      }
   }

   return NO_TRANSITION;
}

int BloodPrimeReturn::preProcess ()
{
   ProcData pd;
   //
   // If we detect fluid ...
   //
   if ( pd.Status().ReservoirLowSensorCovered() )
   {
      DataLog(log_level_proc_debug) << "Low Level Sensor Covered" << endmsg;
      //
      // Accumulate the volume during this cycle ...
      //
      _VolumeAfterSensor += pd.Status().ReturnPump.dV();
      DataLog(log_level_proc_debug) << "Volume after the Sensor has been covered -> " << _VolumeAfterSensor << endmsg;
   }
   //
   // We see air ...
   //
   else
   {
      DataLog(log_level_proc_debug) << "Low Level Sensor Uncovered" << endmsg;
      _VolumeAfterSensor = 0.0f;
   }

   //
   // Check for low level too late ...
   //
   if ( pd.Volumes().Vreturn.Get() <= CobeConfig::data().LowLevelTooLateLimit )
   {
      //
      // Set the alarm ...
      //
      DataLog(log_level_proc_blood_prime_error) << "Low Level Seen Too Late Alarm set: volume->"
                                                << pd.Volumes().Vreturn.Get() << ", limit is "
                                                << CobeConfig::data().LowLevelTooLateLimit << endmsg;
      _TooLateAlarm.setAlarm();
   }

   return NORMAL;
}

int BloodPrimeReturn::postProcess ()
{
   ProcData pd;
   //
   // Command the return and AC pumps ...
   //
   pd.Orders().Qrp(-1.0f * fabs(CobeConfig::data().QrpBloodPrime) );
   pd.Orders().Qac(fabs(pd.Orders().Qrp() ) / MAX_RATIO);
   DataLog(log_level_proc_debug) << "Return pump command: " << pd.Orders().Qrp()
                                 << " AC pump command: " << pd.Orders().Qac() << endmsg;

   //
   // Command the centrifuge ...
   //
   pd.Orders().RPM(CobeConfig::data().RPMBloodPrime);
   DataLog(log_level_proc_debug) << "Centrifuge command: " << pd.Orders().RPM() << endmsg;

   return NORMAL;
}

int BloodPrimeReturn::preEnter ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();

   return NORMAL;
}

int BloodPrimeReturn::postExit ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();

   return NORMAL;
}

void BloodPrimeReturn::reset ()
{
   //
   // Reset the accumulators ...
   //
   _VolumeAfterSensor = 0.0f;
}

void BloodPrimeReturn::stopPumps ()
{
   ProcData pd;
   //
   // Command all the pumps to stop ...
   //
   pd.Orders().Qin(0.0f);
   pd.Orders().Qac(0.0f);
   pd.Orders().Qrp(0.0f);
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);
   DataLog(log_level_proc_debug) << "Stop all pumps" << endmsg;
}

void BloodPrimeReturn::copyOver (const BloodPrimeReturn&)
{}

void BloodPrimeReturn::cleanup ()
{}

/* FORMAT HASH 3fa031b0473d06d84c89840c214175cf */
