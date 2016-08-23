/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      acpressreturnline.cpp
 *
 */

#include "acpressreturnline.h"
#include <math.h>
#include "procdata.h"

const float MIN_RETURN_VOL = 1.0f; // mls
const float RETURN_LIMIT   = 7.0f; // mls

DEFINE_STATE(ACPressReturnLine);

ACPressReturnLine::ACPressReturnLine()
   : StateAbs(),
     _PrimeFailureAlarm(PRIME_FAILURE),
     _ReturnVolume(0.0f)
{}

ACPressReturnLine::ACPressReturnLine(const ACPressReturnLine& state)
   : StateAbs(state),
     _PrimeFailureAlarm(PRIME_FAILURE),
     _ReturnVolume(state._ReturnVolume)
{
   ACPressReturnLine::copyOver(state);
}

ACPressReturnLine::~ACPressReturnLine()
{
   ACPressReturnLine::cleanup();
}

int ACPressReturnLine::transitionStatus ()
{
   ProcData pd;

   //
   // Check for access pressure sensor <= -50mmHg
   //
   if ( pd.Status().APS() <= -50.0f &&
        fabs(_ReturnVolume) > MIN_RETURN_VOL )
   {
      //
      // Log volume accrued for analysis
      //
      DataLog(log_level_proc_ac_prime_info) << "Required " << fabs(_ReturnVolume)
                                            << " of " << RETURN_LIMIT << " ml to achieve pressure." << endmsg;

      return 1;
   }
   return NO_TRANSITION;
}

int ACPressReturnLine::preProcess ()
{
   ProcData pd;

   //
   // Increment the volume ...
   //
   _ReturnVolume += pd.Status().ReturnPump.dV();

   //
   // Ensure that you do not pull to much fluid down the line.
   //
   DataLog(log_level_proc_debug) << "Return volume = " << _ReturnVolume <<
      ", RETURN_LIMIT = " << RETURN_LIMIT <<
      ", APS = " << pd.Status().APS() << endmsg;
   if ( fabs(_ReturnVolume) > RETURN_LIMIT )
   {
      //
      // return line prime failed! - set PRIME alarm
      //
      DataLog(log_level_proc_ac_prime_error) << "Failed to achieve -50mmHg after " << RETURN_LIMIT << "mls" << endmsg;
      _PrimeFailureAlarm.setAlarm();
      DataLog(log_level_proc_debug) << "Set the Prime Failure Alarm " << PRIME_FAILURE << endmsg;

      //
      // Reset the initial volume ...
      //
      _ReturnVolume = 0.0f;
      DataLog(log_level_proc_debug) << "Initialized Return Volume: 0.0f" << endmsg;
   }
   return NORMAL;
}

int ACPressReturnLine::postProcess ()
{
   //
   // Command the pumps...
   //
   startPumps();

   return NORMAL;
}

int ACPressReturnLine::preEnter ()
{
   ProcData pd;

   //
   // Save initial volumes
   //
   _ReturnVolume = 0.0f;
   DataLog(log_level_proc_debug) << "Initialized Return Volume: 0.0f" << endmsg;

   //
   // Start the pumps...
   //
   startPumps();

   //
   // Ensure that the other pumps are stopped.
   //
   pd.Orders().Qac(0.0f);
   pd.Orders().Qin(0.0f);
   pd.Orders().Qcollect(0.0f);
   pd.Orders().Qplasma(0.0f);
   DataLog(log_level_proc_debug) << "Setting AC flow: " << 0.0f << ", Inlet flow: " << 0.0f <<
      ", Collect flow: " << 0.0f << ", Plasma flow: " << 0.0f << endmsg;

   return NORMAL;
}

int ACPressReturnLine::postExit ()
{
   ProcData pd;

   //
   // Stop the pumps ...
   //
   pd.Orders().Qrp(0.0f);
   DataLog(log_level_proc_debug) << "Setting return flow: " << 0.0f << endmsg;

   //
   // Delete my messages ...
   //
   cleanup();

   return NORMAL;
}

void ACPressReturnLine::startPumps ()
{
   ProcData pd;

   //
   // Start the return pump to pull negative pressure.
   //
   pd.Orders().Qrp(-50.0);
   DataLog(log_level_proc_debug) << "Setting return flow: " << -50.0f << endmsg;
}

void ACPressReturnLine::copyOver (const ACPressReturnLine&)
{}

void ACPressReturnLine::cleanup ()
{}

/* FORMAT HASH 27f42f9102d35cdb2d65dbc81f1665c1 */
