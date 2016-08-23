/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      wbcchamberpurge.cpp
 *
 */

#include "wbcchamberpurge.h"
#include "substatemap.h"
#include "cobeconfig.h"
#include "procdata.h"
#include "purgecalcs.h"
#include <math.h>
#include <time.h>
#include "pumpspause.h"

DEFINE_OBJ(WBCChamberPurge);

static PumpsPause pumpPauseRecovery;

WBCChamberPurge::WBCChamberPurge()
   : RecoveryAbs(),
     _SpilloverRequest(Callback<WBCChamberPurge>(this, &WBCChamberPurge::recoveryRequest) ),
     _ChamberPacked(0),
     _ChamberPurged(0),
     _InletProcessed(0),
     _InletVolume(0.0f),
     _VPurgeTrigger(0.0f),
     _NumCyclesComplete(0),
     _NumCyclesNeeded(0),
     _PlasmaVolume(0.0f),
     _PurgeComplete(0),
     _SpilloverRequested(0),
     _RecoveryRequested(0),
     _StandardPurgeVolume(0.0f),
     _QplasmaPurge(0.0f),
     _VplasmaPurgeCycle(0.0f),
     _VinChamberPack(0.0f),
     _VinClearChannel(0.0f),
     _resSetupDone(false),
     _PurgeStartTimeRecorded(0),
     _LogPurgeSuppressed(0),
     _prePurgeRpm(0.0f),
     _recoveryStep(0)

{}

WBCChamberPurge::WBCChamberPurge(const WBCChamberPurge& rec)
   : RecoveryAbs(rec),
     _SpilloverRequest(Callback<WBCChamberPurge>(this, &WBCChamberPurge::recoveryRequest) ),
     _ChamberPacked(rec._ChamberPacked),
     _ChamberPurged(rec._ChamberPurged),
     _InletProcessed(rec._InletProcessed),
     _InletVolume(rec._InletVolume),
     _VPurgeTrigger(rec._VPurgeTrigger),
     _NumCyclesComplete(rec._NumCyclesComplete),
     _NumCyclesNeeded(rec._NumCyclesNeeded),
     _PlasmaVolume(rec._PlasmaVolume),
     _PurgeComplete(rec._PurgeComplete),
     _SpilloverRequested(rec._SpilloverRequested),
     _RecoveryRequested(rec._RecoveryRequested),
     _StandardPurgeVolume(rec._StandardPurgeVolume),
     _QplasmaPurge(rec._QplasmaPurge),
     _VplasmaPurgeCycle(rec._VplasmaPurgeCycle),
     _VinChamberPack(rec._VinChamberPack),
     _VinClearChannel(rec._VinClearChannel),
     _resSetupDone(rec._resSetupDone),
     _PurgeStartTimeRecorded(rec._PurgeStartTimeRecorded),
     _LogPurgeSuppressed(rec._LogPurgeSuppressed),
     _prePurgeRpm(rec._prePurgeRpm),
     _recoveryStep(rec._recoveryStep)
{
   copyOver(rec);
}

WBCChamberPurge::~WBCChamberPurge()
{
   cleanup();
}

int WBCChamberPurge::init ()
{
   _recoveryStep = 0;
   return 1;
}

int WBCChamberPurge::doRecovery ()
{
   ProcData pd;

   bool     hyperconcentration = Software_CDS::isPPC(pd.RunTargets().PlateletConcenTarget.Get());

   if (!_PurgeStartTimeRecorded)
   {
      pd.MakeRunWritable();
      pd.Run().WBCPurgeStartTime.Set(pd.ProcRunTime().getMins());
      pd.Run().LastCentrifugeSpeed.Set(pd.Status().CentrifugeRPM());
      pd.MakeRunReadable();
      DataLog(log_level_proc_recovery_info) << "WBC Purge Start Time: " << pd.Run().WBCPurgeStartTime.Get() << endmsg;
      _PurgeStartTimeRecorded = 1;
   }
   //
   // We have processed the recovery request by entering this function ...
   //
   _RecoveryRequested = 0;

   // Set Concentration Specific Variables
   const CobeConfig::CobeConfigStruct& cc = CobeConfig::data();
   _VinChamberPack = cc.VinChamberPack;
   if (!hyperconcentration)
   {
      _QplasmaPurge      = cc.QplasmaPurgeStd;
      _VplasmaPurgeCycle = cc.VplasmaPurgeCycle1Std;
      _VinClearChannel   = cc.VinClearChannelStd;
   }
   else
   {
      _QplasmaPurge      = cc.QplasmaPurgeHigh;
      _VplasmaPurgeCycle = cc.VplasmaPurgeCycle1High;
      _VinClearChannel   = cc.VinClearChannelHigh;
   }

   if (pd.Status().ReservoirIsEmpty() && hyperconcentration)
   {
      _resSetupDone = true;
   }


   do
   {
      //
      // Step 0: Command the valves
      //   CollectValve = PlasmaValve = RBCValve = RETURN
      //
      pd.Orders().ValveCmd(HAL_VRBC,     HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VPLASMA,  HAL_VALVE_RETURN);
      pd.Orders().ValveCmd(HAL_VCOLLECT, HAL_VALVE_RETURN);

      float returnFlow =   pd.Predict().Qrp(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE))
                         - pd.Predict().Qin(/* DrawCycle == */ false, SubstateMap::alg(NOCOLLECT_STATE));
      if (  _recoveryStep == 0 )
      {


         if ( !_resSetupDone && hyperconcentration ) // only do this if hyper conc
         {
            // force a switch to return:
            if ( pd.Run().DrawCycle.Get() == true)
            {
               DataLog(log_level_proc_recovery_info) << "Force Switch to return @ res Volm =" << pd.Cycle().VresDraw << endmsg;
               // pd.MakeRunWritable();
               // pd.Run().WBCPurgeResVolm.Set( pd.Cycle().VresDraw.Get() );

               pd.SwitchtoReturn();
            }

            pd.Orders().Qrp(returnFlow);
            pd.Orders().Qin(0.0f);
            pd.Orders().Qac(0.0f);
            pd.Orders().Qcollect(0.0f);
            pd.Orders().Qplasma(0.0f);

            break;
         }
         IncrementStep(1);

      }

      if ( _recoveryStep == 1)
      {
         // Step 1
         //	IT13923

         // pause pumps
         pd.Orders().Qrp(0.0f);
         pd.Orders().Qin(0.0f);
         pd.Orders().Qac(0.0f);
         pd.Orders().Qcollect(0.0f);
         pd.Orders().Qplasma(0.0f);

         // set the centrifuge if below 3000 and pause pumps
         if ( pd.Status().CentrifugeRPM() < CobeConfig::data().RPMCollect)
         {
            pd.Orders().RPM(CobeConfig::data().RPMCollect, true);
            // record the rpm
            if ( _prePurgeRpm == 0.0f)
            {
               pumpPauseRecovery.reset();
               _prePurgeRpm = pd.Status().CentrifugeRPM();
               DataLog(log_level_proc_recovery_info) << "Pre Purge condition centrifuge RPM " << _prePurgeRpm << endmsg;
            }
            // continue this while rpm is not up to 3000
            break;
         }
         else
            IncrementStep(2);
      }


      // res is empty continue:
      float vPackChamber = CobeConfig::data().VinChamberPack;

      // Run the pumps at QinAvg of 45mls/min flows, with no recirculation
      //
      if ( pd.Run().DrawCycle.Get() )
      {
         pd.Orders().Qin(pd.Predict().Qin(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE)));
         pd.Orders().Qac(pd.Predict().Qac(/* DrawCycle == */ true, SubstateMap::alg(NOCOLLECT_STATE)));
         pd.Orders().Qrp(0.0f);
      }
      else
      {
         pd.Orders().Qin(0.0f);
         pd.Orders().Qac(0.0f);
         pd.Orders().Qrp(returnFlow);
      }

      //
      // Step 2: Calculate the number of purge cycles required ...
      //  NumCycles gets rounded up to nearest whole number of cycles ...
      //
      //
      // Only calculate the number of cycles needed once per recovery ...
      //
      if ( _NumCyclesNeeded  == 0)
      {
         _NumCyclesNeeded = PurgeCalcs::calculateNumCycles(hyperconcentration, pd.Predict().VinTarget.Get(), _VPurgeTrigger);
         DataLog(log_level_proc_recovery_info) << "Number of Purge Cycles Needed: " << _NumCyclesNeeded << endmsg;
      }

      if ( !_ChamberPacked )
      {
         //
         // Step 3: Pack the chamber
         //   Stop the collect and plasma pumps
         //   Continue until inlet volume processed reaches VPackChamber
         //
         IncrementStep(3);
         pauseProcessing(vPackChamber);

         if ( _InletProcessed )
         {
            //
            // The chamber is now packed ...
            //
            _ChamberPacked = 1;

            //
            // Reset the flags before starting purge cycles ...
            //
            _InletProcessed = 0;
         }

         break;
      }
      //
      // Step 4: Perform Cycled Purges
      //
      IncrementStep(4);
      if ( _NumCyclesComplete < _NumCyclesNeeded )
      {
         performPurge(_NumCyclesNeeded);
         break;
      }

      // Step 5
      IncrementStep(5);

      bool requiresRampDn = ( _prePurgeRpm != 0.0f && pd.Status().CentrifugeRPM() > _prePurgeRpm );

      if ( requiresRampDn && _recoveryStep == 5 )
      {
         pd.Orders().RPM(_prePurgeRpm, true);
         pd.Orders().Qrp(0);
         pd.Orders().Qin(0.0f);
         pd.Orders().Qac(0.0f);
         pd.Orders().Qcollect(0.0f);
         pd.Orders().Qplasma(0.0f);
         break;
      }

      IncrementStep(6);

      // pump pause recovery
      if ( _prePurgeRpm > 0.0 && pumpPauseRecovery.recoveryNeeded() )
      {
         DataLog(log_level_proc_recovery_info) << "PumpsPause in WBC purge needed" << endmsg;
         pumpPauseRecovery.doRecovery();
         break;
      }
      else
         DataLog(log_level_proc_recovery_info) << "Pause in WBC purge not needed" << endmsg;

      //
      // Step 6:  WBC Purge Complete
      //
      _PurgeComplete          = 1;
      _NumCyclesNeeded        = 0;
      _SpilloverRequested     = 0;
      _PurgeStartTimeRecorded = 0;
      _prePurgeRpm            = 0.0f;
      _recoveryStep           = 0;
      pd.Orders().RPMClearOverride();
      pumpPauseRecovery.reset();

      switch (pd.Run().WBCPurgeType.Get())
      {
         case WBCPURGE_UNSCHEDULED :
            pd.MakeRunWritable();
            pd.Run().WBCPurgeType.Set(WBCPURGE_NONE);
            pd.Run().LastCentrifugeSpeed.Set(0);
            pd.MakeRunReadable();
            break;
         default :
            break;
      }

   } while (false);

   return 1;
}

void WBCChamberPurge::IncrementStep (int step)
{
   if (_recoveryStep < step )
   {
      DataLog(log_level_proc_recovery_info) << "WBCChamberPurge recovery step " << _recoveryStep << " done" << endmsg;
      _recoveryStep = step;
   }
}

int WBCChamberPurge::mayEnter () const
{
   ProcData pd;

   //
   // This recovery may only be done for platelet procedures ...
   //
   if ( !pd.Status().CassIsPltPlsRBC() )
   {
      return 0;
   }
   return 1;
}

void WBCChamberPurge::recoveryRequest ()
{
   ProcData pd;
   //
   // We have a request for a spillover which means
   //  we need the purge the chamber also ...
   //
   _SpilloverRequested = 1;
   _RecoveryRequested  = 1;

   bool ppc = Software_CDS::isPPC(pd.RunTargets().PlateletConcenTarget.Get());
   if (ppc)
   {
      _VplasmaPurgeCycle = CobeConfig::data().VplasmaPurgeCycle1High;
   }
   else
   {
      _VplasmaPurgeCycle = CobeConfig::data().VplasmaPurgeCycle1Std;
   }
   //
   // Set up the maximum number of cycles to purge ...
   //
   _NumCyclesNeeded = (int)ceil(CobeConfig::data().PurgeMaxTotalVolume / _VplasmaPurgeCycle);

   //
   // Log the number of purge cycles ...
   //
   DataLog(log_level_proc_recovery_info)
      << " Unscheduled Purge, Number of Purge Cycles Needed: "
      << _NumCyclesNeeded << endmsg;
   pd.MakeRunWritable();
   pd.Run().WBCPurgeType.Set(WBCPURGE_UNSCHEDULED);
   pd.Run().NumberWBCPurges.Set(pd.Run().NumberWBCPurges.Get() + 1);
   pd.MakeRunReadable();
}

int WBCChamberPurge::shouldRecover ()
{
   ProcData pd;

   //
   // This recovery is not allowed in PIR or RBC collection ...
   //
   if (    (    pd.Run().Substate.Get() < SS_PIR_PLASMA        //  Don't include the two PIR
                || pd.Run().Substate.Get() > SS_PIR_NOPLASMA ) //   substates

           && ( pd.Run().Substate.Get() < SS_RBC_PTF_SETUP_1 ) //  ... And don't allow during RBC collect
           )
   {
      //
      // Only perform Standard, volume triggered purges before PIR, after
      //  doesn't make sense cause we don't collect any more platelets ...
      //
      if ( pd.Run().Substate.Get() < SS_PIR_PLASMA )
      {
         _VPurgeTrigger = pd.Predict().VinNextPurge.Get();
         if (    ( _VPurgeTrigger > _StandardPurgeVolume )
                 && ( !_RecoveryRequested && !active() )
                 && pd.Volumes().Vin.Get() > _VPurgeTrigger )
         {
            DataLog(log_level_proc_recovery_info) << "Purge volume reached: Vin( " << pd.Volumes().Vin.Get()
                                                  << " ) > VPurgeTrigger( " << _VPurgeTrigger << " )" << endmsg;

            _RecoveryRequested   = 1;
            _StandardPurgeVolume = pd.Volumes().Vin.Get();

            //
            // Increment the number of purges done this procedure, and flag it as a scheduled purge...
            pd.MakeRunWritable();
            pd.Run().NumberWBCPurges.Set(pd.Run().NumberWBCPurges.Get() + 1);
            pd.Run().NScheduledWBCPurges.Set(pd.Run().NScheduledWBCPurges.Get() + 1);
            pd.Run().WBCPurgeType.Set(WBCPURGE_SCHEDULED);
            pd.Run().LastProcPurge.Set(_VPurgeTrigger);
            pd.MakeRunReadable();

            //
            // Send the request to clear the line and settle the channel after this
            //  recovery is complete
            ClearLineMsg     cl(MessageBase::SEND_LOCAL);
            SettleChannelMsg sc(MessageBase::SEND_LOCAL);
            cl.send(2 /* standard */);
            sc.send(1);

         }
      }

      if ( ( _RecoveryRequested ||
             ( active() && !_PurgeComplete ) )
           )
      {
         return 1;
      }

      //
      // Reset my flags ...
      //
      reset();
   }

   return 0;
}

int WBCChamberPurge::performPurge (int numCycles)
{
   ProcData pd;
   if (_NumCyclesComplete >= 1)
   {
      _VplasmaPurgeCycle = Software_CDS::isPPC(pd.RunTargets().PlateletConcenTarget.Get())
                           ? CobeConfig::data().VplasmaPurgeCycle2High
                           : CobeConfig::data().VplasmaPurgeCycle2Std;
   }
   //
   // As long as we still have some cycles needed to complete ...
   //
   if ( _NumCyclesComplete < numCycles )
   {

      //    4a:  Purge Chamber
      //       Qac = Qin = Qcollect = 0.0;
      //       Purge until plasma volume processed >= VPurgeCycle
      //
      purgeChamber(_VplasmaPurgeCycle);

      if ( _ChamberPurged )
      {
         //    4b:  Clear Channel
         //       Qplasma = Qcollect = 0.0;
         //       Qin, Qac, Qreturn are not overridden
         //       Clear until inlet volume processed >= VinClearChannel;
         //
         pauseProcessing(_VinClearChannel);

         if ( _InletProcessed )
         {
            //
            // The purge cycle is complete ...
            //
            _NumCyclesComplete++;

            _ChamberPurged  = 0;
            _InletProcessed = 0;
         }
      }

      //
      // The purge is not complete ...
      //
      return 0;
   }

   //
   // Keep the chamber flow at zero ...
   //
   pd.Orders().Qplasma(0.0f);
   pd.Orders().Qcollect(0.0f);

   //
   // The purge is complete
   //
   _NumCyclesComplete = 0;

   return 1;
}

void WBCChamberPurge::pauseProcessing (float volume)
{
   if ( !_InletProcessed )
   {
      ProcData pd;

      //
      // Stop the plasma and collect pumps ...
      //
      pd.Orders().Qplasma(0.0f);
      pd.Orders().Qcollect(0.0f);

      //
      // Accumulate inlet volume ...
      //
      _InletVolume += pd.Status().InletPump.dV();

      //
      // Run till we accumulate enough volume ...
      //
      if ( _InletVolume >= volume )
      {
         _InletProcessed = 1;
         _InletVolume    = 0.0f;
      }
   }
}

void WBCChamberPurge::purgeChamber (float volume)
{
   if ( !_ChamberPurged )
   {
      ProcData pd;

      //
      // Stop the all pumps but the plasma pump ...
      //
      pd.Orders().Qin(0.0f);
      pd.Orders().Qac(0.0f);
      pd.Orders().Qrp(0.0f);
      pd.Orders().Qcollect(0.0f);

      //
      // Command the plasma pump to run ...
      //
      pd.Orders().Qplasma(_QplasmaPurge);

      //
      // Accumulate plasma volume ...
      //
      _PlasmaVolume += pd.Status().PlasmaPump.dV();

      //
      // Run till we accumulate enough volume ...
      //
      if ( _PlasmaVolume >= volume )
      {
         _ChamberPurged = 1;
         _PlasmaVolume  = 0.0f;
      }
   }
}

int WBCChamberPurge::round (double v)
{
   if ( ( v - floor(v) ) < 0.5 )
      return (int)floor(v);
   else
      return (int)ceil(v);
}

RecoveryTypes::InRecovery WBCChamberPurge::recoveryExpected ()
{
   ProcData pd;

   if ( pd.Run().WBCPurgeType.Get() == WBCPURGE_SCHEDULED )
      return RecoveryTypes::CountTime;
   else
      return RecoveryTypes::CountNone;
}

void WBCChamberPurge::reset ()
{
   DataLog(log_level_proc_debug) << "WBCChamberPurge reset" << endmsg;
   ProcData pd;
   pd.Orders().RPMClearOverride();
   pumpPauseRecovery.reset();

   _prePurgeRpm        = 0.0f;
   _recoveryStep       = 0;
   _VPurgeTrigger      = 0.0f;
   _NumCyclesComplete  = 0;
   _NumCyclesNeeded    = 0;
   _ChamberPurged      = 0;
   _PlasmaVolume       = 0.0f;
   _InletProcessed     = 0;
   _InletVolume        = 0.0f;
   _ChamberPacked      = 0;
   _SpilloverRequested = 0;
   _RecoveryRequested  = 0;
   _PurgeComplete      = 0;
   _QplasmaPurge       = 0.0f;
   _VplasmaPurgeCycle  = 0.0f;
   _VinChamberPack     = 0.0f;
   _resSetupDone       = false;
}

void WBCChamberPurge::copyOver (const WBCChamberPurge&)
{}

void WBCChamberPurge::cleanup ()
{}

/* FORMAT HASH 982ec89a22f3f1a84d7d07ecbc25df36 */
