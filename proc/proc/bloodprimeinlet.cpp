/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      bloodprimeinlet.cpp
 *
 */

#include "bloodprimeinlet.h"
#include "run_defs.h"
#include "limitprimeratio.h"
#include "cobeconfig.h"
#include "procdata.h"

DEFINE_STATE(BloodPrimeInlet);

BloodPrimeInlet::BloodPrimeInlet()
   : StateAbs(),
     _CustomRatio(0.0f),
     _DoorOpenAlarm(DOOR_OPEN_ALARM)
{}

BloodPrimeInlet::BloodPrimeInlet(const BloodPrimeInlet& state)
   : StateAbs(state),
     _CustomRatio(state._CustomRatio),
     _DoorOpenAlarm(DOOR_OPEN_ALARM)
{}

BloodPrimeInlet::~BloodPrimeInlet()
{}

int BloodPrimeInlet::transitionStatus ()
{
   ProcData pd;

   //
   // We are done when we have accumulated
   //  enough volume through the inlet line ...
   //
   const float vol = pd.Volumes().VinTotal.Get();

   if ( (vol >= CobeConfig::data().VinBloodPrimeInlet) &&
        (fabs(pd.Status().CentrifugeRPM() - CobeConfig::data().RPMBloodPrime) <= CobeConfig::data().CentrifugeSpeedErrorPrime))
   {
      DataLog(log_level_proc_blood_prime_info) << "VinTotal ( " << vol << " ) > "
                                               << CobeConfig::data().VinBloodPrimeInlet << " @ " << pd.Status().CentrifugeRPM() << endmsg;

      return 1;
   }

   DataLog(log_level_proc_debug) << "VinTotal ( " << vol << " ) <= "
                                 << CobeConfig::data().VinBloodPrimeInlet  << endmsg;

   return NO_TRANSITION;
}

int BloodPrimeInlet::preProcess ()
{
   ProcData pd;

   // Shut the front door! it 12372
   const bool doorIsOpen = pd.Status().DoorIsOpen();
   if (doorIsOpen)
      _DoorOpenAlarm.setAlarm();

   return NORMAL;
}

int BloodPrimeInlet::postProcess ()
{
   ProcData pd;

   //
   // Command the inlet and ac pumps ...
   //
   const float ratio = LimitPrimeRatio(pd.Config().Procedure.Get().key_custom_ratio,
                                       pd.Donor().Hct.Get());
   if ( ratio != _CustomRatio )
   {
      //
      // Log the priming ratio ...
      DataLog(log_level_proc_blood_prime_info) << "Custom ratio: " << ratio << endmsg;
      _CustomRatio = ratio;
   }

   pd.Orders().Qin(CobeConfig::data().QinBloodPrime);
   pd.Orders().Qac(pd.Orders().Qin() / ratio);

   DataLog(log_level_proc_debug) << "Inlet pump command: " << pd.Orders().Qin()
                                 << " AC pump command: " << pd.Orders().Qac() << endmsg;

   //
   // Command the centrifuge to start ...
   //
   //
   //
   // if the 64V is not pulled (for an alarm condition say), command the centrifuge to spin.
   // else, don't start the centrifuge yet... it causes a fatal error :( (IT7092)
   if ( pd.Status().PS64Vsw.Acceptable())
   {
      pd.Orders().RPM(CobeConfig::data().RPMBloodPrime);
   }
   else if (   !pd.AlarmActive()
               &&  pd.Status().DoorIsLocked()
               && !pd.Status().PS64Vsw.Acceptable() )
   {

      DataLog(log_level_proc_info) << "No alarm and 64v off -- requesting 64v On "  << endmsg;
      RequestSafetyPowerMsg request(MessageBase::SEND_GLOBAL);
      request.send(ENABLE_64V_POWER);
   }
   else
   {
      DataLog(log_level_proc_info) << "Active Alarm! : Centrifuge RPM commanded to 0.0 rpm's "  << endmsg;
      pd.Orders().RPM(0.0);
   }

   DataLog(log_level_proc_debug) << "Centrifuge RPM -> " << pd.Orders().RPM() << endmsg;

   return NORMAL;
}

int BloodPrimeInlet::preEnter ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();

   return NORMAL;
}

int BloodPrimeInlet::postExit ()
{
   //
   // Stop all pumps ...
   //
   stopPumps();

   return NORMAL;
}

void BloodPrimeInlet::reset ()
{}

void BloodPrimeInlet::stopPumps ()
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

void BloodPrimeInlet::copyOver (const BloodPrimeInlet&)
{}

void BloodPrimeInlet::cleanup ()
{}

/* FORMAT HASH a71fb1be259fa59d6be9e392a72638c2 */
