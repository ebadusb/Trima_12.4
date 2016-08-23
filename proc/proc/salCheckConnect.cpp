/*
 * Copyright (c) 2008 by Caridian BCT, Inc.  All rights reserved.
 *
 * TITLE:      salrinseback.cpp
 *
 */

#include "cobeconfig.h"
#include "procdata.h"
#include "salCheckConnect.h"
#include "volumeacc.h"

DEFINE_STATE(SalCheckConnect);

SalCheckConnect::SalCheckConnect()
   : StateAbs(),
     _salineBagEmpty(REPLACEMENT_FLUID_ALARM_RINSEBACK),
     _transitionNow(false)
{}

SalCheckConnect::~SalCheckConnect()
{}

int SalCheckConnect::transitionStatus ()
{
   if (_transitionNow)
      return 1;

   return NO_TRANSITION;

}

int SalCheckConnect::preProcess ()
{
   ProcData pd;


   //
   //   Calculate new commanded centrifuge speed per IT3080.  We use +/- 1% of
   //   3000RPM (per spec) to determine if the centrifuge is at speed.
   //
   int   centrifugeAtSpeed = 1;
   float commandedRPM      = 0.0f;
   float speedDelta        = (float)fabs(pd.Status().CentrifugeRPM() - pd.Status().CmdCentrifugeRPM());
   if (speedDelta > 30.0f)
      centrifugeAtSpeed = 0;

   if (pd.Status().CentrifugeRPM() < CobeConfig::data().RPMRinsebackThreshold)
   {
      commandedRPM = 0.0f;

   }
   else if (centrifugeAtSpeed)
   {

      commandedRPM = pd.Status().CmdCentrifugeRPM();
      //
      //
      //   The centrifuge is not at speed but the current speed is above the
      //   threshold speed.  The desire is to maintain the current centrifuge
      //   speed.  We can't simply copy the current speed into the command
      //   since we would then have the command attempting to seek to the current
      //   speed as the current speed changes.  If for some reason we enter this
      //   state right at 2000 and then the centrifuge speed drops below the
      //   threshold, the above logic will set the command to zero (and it will
      //   stay there from then on).
      //
   }
   else
   {

      if (commandedRPM < CobeConfig::data().RPMRinsebackThreshold)
      {
         commandedRPM = pd.Status().CentrifugeRPM(); // which we know is >= threshold
      }

   }
   //
   //
   //  For robustness limit to collect RPM speed (this condition should never
   //  actually occur unless our centrifuge is outside spec).
   //
   if (commandedRPM > CobeConfig::data().RPMCollect)
      commandedRPM = CobeConfig::data().RPMCollect;

   pd.Orders().RPM(commandedRPM);


   if (commandedRPM != pd.Status().CmdCentrifugeRPM() )
      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: commandedRPM to " << commandedRPM <<   endmsg;




   return NORMAL;

}

int SalCheckConnect::postProcess ()
{


   ProcData pd;

   if ( _salineBagEmpty.getState() == CLEARED )
   {

      DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Saline Bag alarm is clear -- transit" <<   endmsg;
      _transitionNow = true;
   }

   return NORMAL;
}

int SalCheckConnect::preEnter ()
{
   ProcData pd;

   if (!pd.RunTargets().RequiresRepFluid.Get())
   {
      if (!pd.Run().SalineConnected.Get())
      {
         DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Saline Bag alarm needed!" <<   endmsg;
         _salineBagEmpty.setAlarm();
      }
      else
         DataLog(log_level_proc_rinseback_info) << "SalineRinseback: Replacement fluid was off but we did a bolus so assumed saline connected" <<   endmsg;
   }

   return NORMAL;
}

int SalCheckConnect::postExit ()
{

   ProcData pd;
   pd.Orders().RPM(0.0f);

   _transitionNow = false;
   return NORMAL;
}

/* FORMAT HASH e5fbf83d59e06e13901ea259ea54de01 */
