/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      hypo_alarm.cpp
 *
 */

#include "hypo_alarm.h"
#include "trima_datalog.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "MssCommon.h"

DEFINE_OBJ(HypoAlarm);

HypoAlarm::HypoAlarm()
   : MonitorBase(),
     _HypovolemiaAlarm(HYPOVOLEMIA_ALARM),
     _HypovolemiaChecker(),
     _HypoAlarmSet(0),
     _VolumeLeft(0.0f),
     _PrevDrawCycle(false)
{}


HypoAlarm::~HypoAlarm()
{}


void HypoAlarm::Monitor ()
{
   ProcData pd;

   //
   //
   //   Copy values from procdata to avoid multiple CDS calls
   //
   bool  DrawCycle         = pd.Run().DrawCycle.Get();
   float VAccumOverdraw    = pd.Cycle().VAccumOverdraw.Get();
   float PlateletVolume    = pd.Volumes().PlateletVolume.Get();
   float VacPlateletBag    = pd.Volumes().VacPlateletBag.Get();
   float PlasmaVolume      = pd.Volumes().PlasmaVolume.Get();
   float VacPlasmaBag      = pd.Volumes().VacPlasmaBag.Get();
   float RBCVolume         = pd.Volumes().RBCVolume.Get();
   float VacRBCBag         = pd.Volumes().VacRBCBag.Get();
   float replacementvolume = pd.Volumes().VReplacement.Get() + pd.Volumes().VSalineBolus.Get();
   float Vac               = pd.Volumes().Vac.Get();

   //
   //
   //   Log where we are at the start of each draw cycle
   //
   if ( DrawCycle && (!_PrevDrawCycle) )
   {
      float controlvol = _HypovolemiaChecker.ControlVolumeRemaining(1, // always current procedure
                                                                    VAccumOverdraw,
                                                                    PlateletVolume, VacPlateletBag,
                                                                    PlasmaVolume,  VacPlasmaBag,
                                                                    RBCVolume, VacRBCBag,
                                                                    replacementvolume, Vac);
      float safetyvol = _HypovolemiaChecker.SafetyVolumeRemaining(1, // always current procedure
                                                                  VAccumOverdraw,
                                                                  PlateletVolume, VacPlateletBag,
                                                                  PlasmaVolume,  VacPlasmaBag,
                                                                  RBCVolume, VacRBCBag,
                                                                  replacementvolume, Vac);
      DataLog(log_level_proc_alarm_monitor_info) << "HypoAlarm: Control: " << controlvol << " remaining of "
                                                 << _HypovolemiaChecker.ControlLimit(PlateletVolume, PlasmaVolume)
                                                 << "; Safety: " << safetyvol << " remaining of "
                                                 << _HypovolemiaChecker.SafetyLimit() << endmsg;
   }
   _PrevDrawCycle = DrawCycle;


   _VolumeLeft = _HypovolemiaChecker.VolumeRemainingToHypovolemia(1, // always current procedure
                                                                  VAccumOverdraw,
                                                                  PlateletVolume, VacPlateletBag,
                                                                  PlasmaVolume, VacPlasmaBag,
                                                                  RBCVolume, VacRBCBag,
                                                                  replacementvolume, Vac);

   if ( ( !_HypoAlarmSet ) &&
        ( _VolumeLeft < -10.0f ) )
   {
      _HypovolemiaAlarm.setAlarm();
      _HypoAlarmSet = 1;
   }

   if ( _VolumeLeft < -5.0f)
   {

      if ( pd.Status().CollectValve.IsCollecting() ||
           pd.Status().RBCValve.IsCollecting() ||
           pd.Status().PlasmaValve.IsCollecting() )
      {
         RinsebackRequestMsg procHint(MessageBase::SEND_LOCAL);
         procHint.send(1);

         DataLog(log_level_proc_alarm_monitor_info) << "Rinseback forced by hypovolemia" << endmsg;
      }
   }
}

void HypoAlarm::enable ()
{
   MonitorBase::enable();
   ProcData pd;

   //
   //   First time through create and initialize our hypovolemia
   //   checking object.
   //
   _HypovolemiaChecker.initialize(pd.TrimaSet().Cassette.Get());
   _HypovolemiaChecker.log();
}

void HypoAlarm::disable ()
{
   MonitorBase::disable();
}

/* FORMAT HASH b06d74cddf3fa3cb3afcda91e954ca07 */
