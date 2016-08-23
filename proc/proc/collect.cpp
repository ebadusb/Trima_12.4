/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      collect.cpp
 *
 */

#include <cmath>


#include "collect.h"
#include "recoverytypes.h"
#include "substatemap.h"
#include "predict_msgs.h"
#include "procdata.h"
#include "cobeconfig.h"

#include "CatalogChecker.h"

#include "router.h"
#include "MssCommon.h"
#include "messagesystem.h"

DEFINE_STATE(Collect);

Collect::Collect()
   : StateAbs (),
     _RecoveryMan      (),
     _CollectDone      (0),
     _LimitReturnSpeed (0),
     _CommandTimer     (),
     _StartVin         (0),
     _TargetChecker    (),
     _QcollectPrev     (0),
     _PrevQac          (0),
     _PrevQin          (0),
     _PrevQrp          (0),
     _PrevQpls         (0),
     _PrevQplt         (0),
     _PrevRpm          (0),
     _doneSplitYet         (false),
     _DRBCSplitAlarm       (DRBC_SPLIT_ALERT),
     _prePirRamp           (false),
     _LimitedMidrunReturn  (false),
     _FirstMidrunReturnNum (0)
{}

Collect::~Collect()
{}

const char* Collect::currentState () const
{
   ProcData pd;

   if ( pd.Run().Substate.Get() != INVALID_SUBSTATE )
   {
      return SubstateMap::ssStr(pd.Run().Substate.Get() );
   }

   return StateAbs::currentState();
}

int Collect::transitionStatus ()
{
   if ( _CollectDone )
   {
      DataLog(log_level_proc_blood_run_info) << "Collection complete" << endmsg;
      return 1;
   }
   return NO_TRANSITION;
}


int Collect::preProcess ()
{
   ProcData pd;

   //
   // Check to see if we should switch to the next
   //  substate ...
   //
   //
   // Save the current state
   //
   State_names saveState = pd.Run().Substate.Get();
   State_names nextState = INVALID_SUBSTATE;

   if (saveState  != INVALID_SUBSTATE )
   {
      //
      // Determine if we have reached a volume ...
      //
      _TargetChecker.addTarget(pd.Predict().targetEnum(saveState), pd.Predict().volumeTarget(saveState) );
      _TargetChecker.addTarget(pd.Predict().orTargetEnum(saveState), pd.Predict().orVolumeTarget(saveState) );

      if (  _TargetChecker.evaluateTargets(saveState) ||
            ( ( saveState == SS_RP_RBC) && pd.Run().PTFRBCprod1Disabled.Get()  )
            )
      {
         logPredictData();

         nextState = findPlace();


         DataLog(log_level_proc_blood_run_info) << "Target reached -- switch from state "
                                                << SubstateMap::ssStr(saveState) << " to "
                                                << SubstateMap::ssStr(nextState) << endmsg;

         /////////////////////////////////////////////////
         // DRBC split alarm is now here FOR PTF!!
         // you have hit the split point
         // must base it on state changes not dose/volm due to rounding issues
         // ... besides it works better this way!

         if ((  (nextState == SS_RP_RBC_PTF_SETUP_1_DOUBLE)
                ||  (nextState == SS_RP_RBC_PTF_SETUP_2_DOUBLE)
                ||  (nextState == SS_RP_RBC_DOUBLE)
                )
             && (saveState  == SS_RP_RBC)
             && !_doneSplitYet)
         {

            _doneSplitYet = true;
            _DRBCSplitAlarm.setAlarm();


            pd.MakeRunWritable();
            pd.Run().drbcSplitDone.Set(true);
            pd.MakeRunReadable();

         }

         /////////////////////////////////////////////////



         //
         // If the next substate is NULL ...
         //
         if ( nextState == INVALID_SUBSTATE )
         {
            //
            // We are finished collecting ...
            //
            _CollectDone = 1;
         }
         else
         {
            //
            // Check to see that we were not already in the substate
            //  we are switching into ...
            //
            if ( saveState == nextState )
            {
               DataLog(log_level_proc_blood_run_error) << "Invalid Substate Target data " << saveState << endmsg;
            }
            pd.MakeRunWritable();
            pd.Run().Substate.Set(nextState);

            pd.Run().SubstateStartTime.Set(pd.ProcRunTime().getMins() );
            if (nextState == SS_MIDRUN)
            {
               pd.Run().MidrunEntered.Set(1);
            }
            pd.MakeRunReadable();

            saveState = nextState;
            ProcSubstateChangeMsg substateChanged(MessageBase::SEND_LOCAL);
            substateChanged.send(nextState);


         }
         _StartVin = pd.Volumes().Vin.Get();
      }

   }
   else
   {
      //
      // ... the current state is NULL and predict has responded to our
      //       request, so we are finished collecting ...
      //
      _CollectDone = 1;
   }

   //
   // Save the delta vin ...
   pd.MakeVolumesWritable();
   pd.Volumes().VinCurrentSubstate.Set(pd.Volumes().Vin.Get() - _StartVin);

   //
   // If PTF, accrew volume during setup states for predict.
   //
   if ( pd.RunTargets().MeterRbcs.Get() )
   {
      if ( (saveState == SS_RBC_PTF_SETUP_1) ||
           (saveState == SS_RP_RBC_PTF_SETUP_1)  )
      {
         pd.Volumes().RBCSetup1Volume.Set(pd.Volumes().VinCurrentSubstate);
      }
      else if ( (saveState == SS_RBC_PTF_SETUP_2) ||
                (saveState == SS_RP_RBC_PTF_SETUP_2)  )
      {
         pd.Volumes().RBCSetup2Volume.Set(pd.Volumes().VinCurrentSubstate);
      }
      else if (saveState == SS_RP_RBC_PTF_SETUP_1_DOUBLE )
      {
         pd.Volumes().RBCSetup1DoubleVolume.Set(pd.Volumes().VinCurrentSubstate);
      }
      else if (saveState == SS_RP_RBC_PTF_SETUP_2_DOUBLE )
      {
         pd.Volumes().RBCSetup2DoubleVolume.Set(pd.Volumes().VinCurrentSubstate);
      }
      else if (saveState == SS_RBC_PTF_PRIME) //  not need black stamp prime
      {
         pd.Volumes().RBCPrimeVolume.Set(pd.Volumes().VinCurrentSubstate);

      }
      //  move to volumeaccume to be more correct
      // else if (saveState == SS_RP_RBC)
      // {
      //    pd.Volumes().RBCProduct_A_FinalDose.Set(pd.Volumes().RBCDose.Get()) ;
      //    pd.Volumes().RBCProduct_A_FinalVolume.Set(pd.Volumes().RBCVolume.Get());
      //    pd.Volumes().RBCProduct_A_FinalVac.Set(pd.Volumes().VacRBCBag.Get());
      // } else if (saveState == SS_RP_RBC_DOUBLE)
      // {
      //   pd.Volumes().RBCProduct_B_FinalDose.Set(pd.Volumes().RBCDose.Get() - pd.Volumes().RBCProduct_A_FinalDose.Get()) ;
      //    pd.Volumes().RBCProduct_B_FinalVolume.Set(pd.Volumes().RBCVolume.Get() - pd.Volumes().RBCProduct_A_FinalVolume.Get());
      //    pd.Volumes().RBCProduct_B_FinalVac.Set(pd.Volumes().VacRBCBag.Get() - pd.Volumes().RBCProduct_A_FinalVac.Get() );
      // }


   }

   return NORMAL;
}

int Collect::postProcess ()
{
   ProcData pd;

   commandPumps(pd.Run().Substate.Get() );
   commandCentrifuge(pd.Run().Substate.Get() );




   //
   // Do any recovery procedures ...
   //
   _RecoveryMan.doRecovery();

   //
   // Set the recovery state ...
   //
   const char* rn = _RecoveryMan.currentState();
   if ( !rn ) { rn = RecoveryTypes::RecoveryStr(RecoveryTypes::None); }

   string rs(rn);
   pd.RecoveryName(rs);

   //
   // Calculate the infusion rate ...
   //
   calculateIR();

   //
   // Request new substate targets and
   //  repredict at the beginning of every return cycle ...
   //
   if (    pd.Run().CycleSwitch.Get()
           && (!pd.Run().DrawCycle.Get() ) ) // IT9729 drift fix
   {
      pd.MakeRunWritable();
      pd.Run().PrePirRamp.Set(_prePirRamp);
      pd.Run().PredictRequest.Set(pd.Run().PredictRequest.Get() | REPREDICT_ONLY);
      pd.MakeRunReadable();
      logPredictData();
   }

   return NORMAL;
}

int Collect::preEnter ()
{
   ProcData pd;
   DataLog(log_level_proc_debug) << "Created and initialized the Substate Targets message" << endmsg;

   //
   // Create and initialize the recovery manager ...
   //
   if ( _RecoveryMan.init() == 0 )
   {
      DataLog(log_level_proc_blood_run_error) << "Failed to initialize recoveries" << endmsg;
      return PROCESSING_ERROR;
   }
   DataLog(log_level_proc_debug) << "Created and initialized the Recovery Manager" << endmsg;

   //
   // Request an RBC chamber purge ...
   //
   const ProcRun_CDS::recovery_count_type& info = pd.Run().RecoveryCountArray.Get();
   int numrbcpurge = info[RecoveryTypes::RBCChamberPurge];
   if ((CobeConfig::data().RBCPurgeVplasma > 0.0f) && numrbcpurge == 0 )
   {
      RBCChamberPurgeMsg rbcChamberPurge(MessageBase::SEND_LOCAL);
      rbcChamberPurge.send(1);
      DataLog(log_level_proc_blood_run_info) << "Triggered RBC Chamber Purge " << endmsg;
   }

   //
   // Initialize the start vin ...
   _StartVin         = pd.Volumes().Vin.Get();
   _LimitReturnSpeed = 1;

   //
   // Initialize the command timer ...
   _CommandTimer.init();
   _CommandTimer.inactivate();

   //
   // Initialize the min chamber flow ...
   pd.MakeRunWritable();
   pd.Run().Qchmin.Set(pd.Predict().lastCalcQchMax.Get() );
   pd.Run().ActualPrimeDuration.Set (pd.ProcRunTime().getMins() );

   //
   // Set the initial substate
   if ( pd.TrimaSet().Cassette == cassette::PLTPLSRBC )
      pd.Run().Substate = SS_CHANNEL_SETUP;
   else
      pd.Run().Substate = SS_RP_CHANNEL_SETUP;

   // Mark the entry to this state
   pd.Run().EnteredCollect.Set(true);
   pd.MakeRunReadable();

   //
   // Log my current run targets ...
   logPredictData();

   return NORMAL;
}

int Collect::postExit ()
{
   //
   // Shut down the recovery manager ...
   //
   _RecoveryMan.cleanup();
   DataLog(log_level_proc_debug) << "Cleaned up the Recovery Manager" << endmsg;

   //
   // Cleanup my dynamic memory
   //
   cleanup();

   return NORMAL;
}

void Collect::reset ()
{
   //
   // Reset my flags ...
   //
   _CollectDone = 0;
}

void Collect::commandPumps (const State_names ws)
{
   ProcData pd;

   //
   // Determine what cycle we are in ... (Draw or Return)
   //
   int   drawCycle = pd.Run().DrawCycle.Get();

   float qac       = pd.Predict().Qac(drawCycle, ws);
   float qin       = rampInlet(drawCycle, ws);
   float qrp       = rampReturn(drawCycle, ws, qin);
   //  float qp = pd.Predict().Qp( drawCycle, ws );
   float qp        = derampPlasmaForDRBC(drawCycle, ws); // just pd.Predict().Qp( drawCycle, ws ) for non PTF
   float qcol      = rampCollect(drawCycle, ws);

   //
   // Limit the return speed ...
   //
   limitQrp(qrp);

   if ( qac == 0.0f && qin == 0.0f && qrp == 0.0f && qp == 0.0f && qcol == 0.0f )
   {
      DataLog(log_level_proc_error) << "Command pumps - all flows zero, using flows from last cycle" << endmsg;
   }
   else
   {
      //
      // Set the pump commands ...
      //
      pd.Orders().Qac(qac);
      pd.Orders().Qin(qin);
      pd.Orders().Qrp(qrp);
      pd.Orders().Qplasma(qp);
      pd.Orders().Qcollect(qcol);

      // save the Qch for later use
      if ( pd.Run().CurrentRecovery.Get() == RecoveryTypes::None && (qcol - qp) > 0 &&
           (qp != _PrevQpls || qcol != _PrevQplt) )
      {
         pd.MakeRunWritable();
         pd.Run().LatestQch.Set(qcol - qp);
         pd.MakeRunReadable();
      }
   }

   if ( qac  != _PrevQac  || qin  != _PrevQin  || qrp  != _PrevQrp  || qp   != _PrevQpls || qcol != _PrevQplt )
   {
      pd.MakeRunWritable();
      pd.Run().LastCommandedQp.Set(_PrevQpls);
      pd.MakeRunReadable();
      DataLog(log_level_proc_info) << "Command pumps - AC: " << qac << "," << pd.Orders().Qac() <<
         " Inlet: "  << qin << "," << pd.Orders().Qin() <<
         " Return: "  << qrp << "," << pd.Orders().Qrp() <<
         " Collect: "  << qcol << "," << pd.Orders().Qcollect() <<
         " Plasma: "  << qp << "," << pd.Orders().Qplasma() << endmsg;
      _PrevQac = qac; _PrevQin  = qin; _PrevQrp  = qrp; _PrevQpls = qp; _PrevQplt = qcol;
   }

   //
   // Set the valve positions ...
   //
   pd.Orders().ValveCmd(HAL_VRBC,     pd.Predict().rbcValve(ws) );
   pd.Orders().ValveCmd(HAL_VPLASMA,  pd.Predict().plasmaValve(ws) );
   pd.Orders().ValveCmd(HAL_VCOLLECT, pd.Predict().plateletValve(ws) );
   DataLog(log_level_proc_debug) << "Command valves - RBC: " << pd.Orders().ValveCmd(HAL_VRBC)
                                 << " Plasma: " << pd.Orders().ValveCmd(HAL_VPLASMA)
                                 << " Collect: " << pd.Orders().ValveCmd(HAL_VCOLLECT) << endmsg;

}

float Collect::rampInlet (const int drawCycle, const State_names ws)
{
   ProcData pd;
   float    qin = pd.Predict().Qin(drawCycle, ws);

   //
   // If we are still in the pre-platelet collection phases, and we are past the end
   //  of the prime phase, ramp the inlet pump ...
   //
   {
      //
      // Check for a new inlet speed target (ie. the current pediction changed)
      //
      // Ramp the inlet pump to a new step at the beginning of each draw cycle ...
      //  so if the draw cycle is just starting calculate the new qin ...
      //
      //  * Set qin = qinpp ( qinpp -> qin value at the end of the prime phase )
      //  * Calculate qinr - qinpp ( qinr -> qin value at the end of the ramping phase )
      //  * Calculate vin - vinpp ( vinpp -> vin at the end of the prime phase ) -> volume of inlet since prime phase complete
      //  * Calculate vinr - vinpp ( vinr -> vin at the end of the ramping phase ) -> volume of inlet during ramping phase
      //  * Calculate (vin - vinpp)/(Vinr - vinpp) -> fractional value of the ramping completion status
      //  * Calculate (qinr - qinpp)*ramping status -> Current speed increment
      //  * Increment qin by the current speed increment
      //  * Save off the current draw cycle
      //

   }

   return qin;
}

float Collect::rampReturn (const int drawCycle, const State_names ws, const float qin)
{
   ProcData pd;
   float    qrp = pd.Predict().Qrp(drawCycle, ws);
   //
   //
   //  Proc recalculates the return pump commanded speed dynamically
   //
   if (!drawCycle)
   {
      //
      //
      //   Calculate how far we are into the current cycle
      //
      float lastTime = pd.LastReturnCycleTime();
      if (lastTime > 0.0f)
      {
         float timeDistance = pd.CurrentCycleTime().getMins() / lastTime;
         if (timeDistance > 1.0f) timeDistance = 1.0f;

         //
         //
         //   Calculate the dynamic flow based upon the ramping coefficient
         //   and the distance into the return cycle.
         //
         float factor = ( CobeConfig::data().QneedleRampCoefficient +
                          2.0f * (1.0f - CobeConfig::data().QneedleRampCoefficient) * timeDistance);
         qrp *= factor;

         //
         //
         //  Limit to max instantaneous flow rate and max needle flow rate
         //
         if (qrp > CobeConfig::data().MaxInstantQreturn)
            qrp = CobeConfig::data().MaxInstantQreturn;
         float qn = qrp - qin;
         if (qn > pd.Adjustments().Return.CurrentCap.Get())
            qrp = qin + pd.Adjustments().Return.CurrentCap.Get();
      }
   }
   return qrp;
}

float Collect::rampCollect (const int drawCycle, const State_names ws)
{
   ProcData    pd;

   float       qcol;
   float       qchPres  = 0.0f;
   float       qchFut   = 0.0f;
   float       qcolFut  = qcol = pd.Predict().Qc(drawCycle, ws);
   float       qplsFut  = pd.Predict().Qp(drawCycle, ws);
   State_names substate = pd.Run().Substate.Get();

   DataLog(log_level_proc_debug) << "QcFut: " << qcolFut;

   //
   // Save the previous commanded value ...
   if ( ( RecoveryMan::inRecovery() == RecoveryTypes::NoRecovery ||
          RecoveryMan::inRecovery() == RecoveryTypes::CountAll ||
          RecoveryMan::inRecovery() == RecoveryTypes::CountVolume ) &&
        pd.Status().CollectPump.CmdFlow() >= CobeConfig::data().CollectMinimumFlow )
   {
      _QcollectPrev = pd.Status().CollectPump.CmdFlow();
   }

   if (    pd.Status().CassIsPltPlsRBC()
           && (    pd.Status().CollectValve.IsCollecting()
                   || pd.Status().PlasmaValve.IsCollecting()
                   || ((SubstateMap::alg(pd.Run().Substate.Get() ) < SubstateMap::alg(SS_PLATELET_PLASMA)) && (pd.Volumes().Vin.Get() > /*400.0f*/ CobeConfig::data().MinVinCollectStart) ) )
           )
   {
      qcol = _QcollectPrev;
      DataLog(log_level_proc_debug) << " QcPres: " << qcol;

      //
      // Determine the chamber flow ...
      //
      qchPres = pd.Run().Qchcurrent.Get();
      if ( pd.Status().PlasmaValve.IsCollecting() )
      {
         qchFut = qcolFut;
      }
      else
      {
         qchFut = qcolFut - qplsFut;
         //
         // Determine what the collect pump should run at to keep the chamber flow the same ...
         qcol = qchPres + qplsFut;
      }
      DataLog(log_level_proc_debug) << " Qch(Pres Fut): " << qchPres << " " << qchFut << " Qc: " << qcol;

      //
      // If a the chamber flow is positive ...
      if ( qchPres >= CobeConfig::data().CollectMinimumFlow ||
           qchFut >= CobeConfig::data().CollectMinimumFlow )
      {
         //
         // If we command a chamber flow greater than the current, then ramp it up ...
         //
         if ( qchFut  > qchPres )
         {
            //
            //  Get the time since the last command ...
            //
            double interval = _CommandTimer.getMins();
            DataLog(log_level_proc_debug) << " int( mins ): " << interval;

            // Qcdot = Cchr * Qch if Qch >= 1.5, Qcdot = 0.1 if Qch < 1.5
            State_names substate = pd.Run().Substate.Get();
            if ( (substate == SS_PIR_PLASMA
                  || substate == SS_PIR_NOPLASMA))
            /*
             * IT 12733 - The line below was commented out to be the same as 6.1.
             * TODO: Regionalize this in future release.
             * */
            // && (pd.RunTargets().PlateletConcenTarget > 2100))
            {
               float linearRampLimit = CobeConfig::data().PIRLinearRampQchLimit * pd.Predict().PirCsf.Get();

               if (pd.Run().Qchcurrent.Get() >= linearRampLimit)
               {
                  static float totInterval = 0.0f;
                  static float Qcho        = qcol;
                  totInterval += interval;
                  qcol         =  Qcho * std::exp(CobeConfig::data().PIRRampRateCoefficient * totInterval);
                  DataLog(log_level_proc_debug) << " totInt: " << totInterval;
               }
               else
                  qcol +=  CobeConfig::data().PIRLinearRampRate * interval;
            }
            else
               qcol += CobeConfig::data().CollectRampRate * interval;
            _prePirRamp = true;
            if ( qcol > qcolFut ) qcol = qcolFut;
            DataLog(log_level_proc_debug) << " QcNew: " << qcol;
         }
         else // ... a smaller chamber flow is OK ...
         {
            qcol        = pd.Predict().Qc(drawCycle, ws);
            _prePirRamp = false;
         }
      }
      _CommandTimer.init();
      _CommandTimer.activate();


      if ( pd.Status().PlasmaValve.IsCollecting() )
      {
         // Limit maximum collect speed
         if ( qcol > pd.Predict().lastCalcQchMax.Get() )
            qcol = pd.Predict().lastCalcQchMax.Get();
         // Limit minimum collect speed
         if ( qcol < CobeConfig::data().CollectMinimumFlow )
            qcol = CobeConfig::data().CollectMinimumFlow;
      }
      else
      {
         // Limit maximum collect speed
         if ( ( qcol - qplsFut )  > pd.Predict().lastCalcQchMax.Get() )
            qcol = qplsFut + pd.Predict().lastCalcQchMax.Get();
         // Limit minimum collect speed
         if ( ( qcol - qplsFut ) < CobeConfig::data().CollectMinimumFlow )
            qcol = qplsFut;
      }
   }
   else
   {
      _CommandTimer.inactivate();
   }

   DataLog(log_level_proc_debug) << endmsg;

   return qcol;
}

void Collect::limitQrp (float& qrp)
{

   ProcData pd;


   /* this is PTFRBCCollectDouble && this is its first return */
   if (      pd.Run().Substate.Get() == SS_RP_RBC
             && !pd.Run().FinishedFirstRBCReturn.Get()
             )
   {
      // slow the return
      if   (!pd.Run().DrawCycle.Get())
      {

         if (qrp > CobeConfig::data().PtfSetupReturnMax)
            qrp = CobeConfig::data().PtfSetupReturnMax;

      }

      // knock this behavior off after the first return
      if ( pd.Run().CycleSwitch.Get() &&     // if just switched from return to draw
           pd.Run().DrawCycle.Get() )
      {
         pd.MakeRunWritable();
         pd.Run().FinishedFirstRBCReturn.Set(true) ;
      }
   }


   /* this is PTFRBCCollectDouble && this is its first return */
   if (      pd.Run().Substate.Get() == SS_RP_RBC_DOUBLE
             && !pd.Run().FinishedFirstRBCDoubleReturn.Get()
             )
   {
      // slow the return
      if   (!pd.Run().DrawCycle.Get())
      {

         if (qrp > CobeConfig::data().PtfSetupReturnMax)
            qrp = CobeConfig::data().PtfSetupReturnMax;

      }

      // knock this behavior off after the first return
      if ( pd.Run().CycleSwitch.Get() &&    // if just switched from return to draw
           pd.Run().DrawCycle.Get() )
      {
         pd.MakeRunWritable();
         pd.Run().FinishedFirstRBCDoubleReturn.Set(true) ;
      }
   }



   //
   // Limit the return speed at the beginning of a procedure ...
   //
   if ( _LimitReturnSpeed )
   {
      if ( qrp > CobeConfig::data().QrpStartup )
         qrp = CobeConfig::data().QrpStartup;
   }

   //
   // Reset the return speed limit flag after the first recovery
   //  is complete ...
   //
   if ( _RecoveryMan.currentState() == 0 )
   {
      _LimitReturnSpeed = 0;
   }


   if (!_LimitedMidrunReturn
       && (pd.Run().Substate.Get() == SS_MIDRUN)
       && (!pd.Run().DrawCycle.Get())
       && (qrp > pd.Predict().LimitingMidrunQrp.Get()) )
   {

      if (_FirstMidrunReturnNum == 0 )
      {
         _FirstMidrunReturnNum = pd.Run().NumReturn.Get();

         DataLog(log_level_proc_info) << "Setting FirstMidrunReturnNum to " << _FirstMidrunReturnNum << endmsg;
      }
      else if ((_FirstMidrunReturnNum < pd.Run().NumReturn.Get()) || (pd.Predict().LimitingMidrunQrp.Get() == 0.0f))
      {
         DataLog(log_level_proc_info) << "Removing Midrun Return flow limit FirstMidrunReturnNum="
                                      <<  _FirstMidrunReturnNum << " Num Returns=" << pd.Run().NumReturn.Get()
                                      << " Limiting Midrun Qrp=" << pd.Predict().LimitingMidrunQrp.Get()
                                      << endmsg;

         _LimitedMidrunReturn = true;
      }

      if (!_LimitedMidrunReturn)
      {
         DataLog(log_level_proc_info) << "Limiting Midrun Return flow to "
                                      << pd.Predict().LimitingMidrunQrp.Get() << " from " << qrp
                                      << " FirstMidrunReturnNum=" << _FirstMidrunReturnNum << endmsg;

         qrp = pd.Predict().LimitingMidrunQrp.Get();

      }

   }


}




float Collect::derampPlasmaForDRBC (const int drawCycle, const State_names substate)
{
   ProcData pd;

   float    Qp           = pd.Predict().Qp(drawCycle, substate);
   float    Vin          = pd.Volumes().VinCurrentSubstate.Get();
   float    setupRampVin = CobeConfig::data().PtfRbcPlsSetup2BPlasmaRampVolume;

   // we need to save the last reported Qp in ptf RBC collect 1 for
   // our start point for ramping later in setup double #2
   if (substate == SS_RP_RBC)
   {
      if (drawCycle)
      {
         pd.MakeRunWritable();
         pd.Run().LastRbcQp.Set(Qp);
         pd.MakeRunReadable();
      }
   }


   if (substate == SS_RP_RBC_PTF_SETUP_2_DOUBLE)
   {
      if (drawCycle)
      {
         Qp = (int)( pd.Run().LastRbcQp.Get()  * (1.0f -  Vin / setupRampVin ) );

         if (Qp <= 1.0f)
            Qp = 0.0f;

         // DataLog( log_level_proc_info ) << " Qp deramping at  " << Qp << endmsg;
      }
   }

   return Qp;
}




void Collect::calculateIR ()
{
   ProcData pd;

   //
   //
   //   Calculate and save off the "last run infusion".  Use either the
   //   no-collect or current infusion rate, determined by the current
   //   valve positions.  We use infusion rate during return (which is
   //   by Trima definition the same as draw since we calculate per cycle)
   //
   float IR               = pd.Predict().InfusionRate(SubstateMap::alg(NOCOLLECT_STATE) );

   bool  isCollectingRBCs = false;

   if ( pd.Status().RBCValve.IsCollecting() )
   {
      isCollectingRBCs = true;
   }
   else
   {
      isCollectingRBCs = false;

   }



   if ( pd.Status().CollectValve.IsCollecting() ||
        pd.Status().PlasmaValve.IsCollecting() ||
        isCollectingRBCs )
   {
      IR = pd.Predict().InfusionRate(pd.Run().Substate.Get() );
   }
   pd.MakeRunWritable();
   pd.Run().LastRunInfusion.Set(IR);
}

State_names Collect::findPlace ()
{
   ProcData    pd;

   int         rp /* rbc/plasma procedure */ = ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC );
   int         totalSubstates                = (rp ? (int)Predict_CDS::NumOfRBCPlsSubstates : (int)Predict_CDS::NumOfSubstates);

   State_names nextstate                     = INVALID_SUBSTATE;
   int         i = SubstateMap::alg(pd.Run().Substate.Get(), rp); i++;
   for ( ( i = ( i > 1 ) ? i : 1 ) ;
         ( i < totalSubstates && nextstate == INVALID_SUBSTATE ) ;
         i++ )
   {
      //
      // Check this substate's targets ...
      //
      _TargetChecker.addTarget(pd.Predict().targetEnum(i), pd.Predict().volumeTarget(i) );
      _TargetChecker.addTarget(pd.Predict().orTargetEnum(i), pd.Predict().orVolumeTarget(i) );
      if (
         _TargetChecker.evaluateTargets(SubstateMap::ss(i, rp) )
         || BogusSubState(SubstateMap::ss(i, rp) )
         )
      {
         // One of the two targets has been achieved, move to the next state ...
         // or the substate is logically incoherent and cannot be completed.
         continue;
      }
      else
      {
         //
         // This substate might be the next one ???
         nextstate = SubstateMap::ss(i, rp);
         DataLog(log_level_proc_debug) << " next substate# " << i << endmsg;
      }
   }
   return nextstate;
}

void Collect::logPredictData ()
{
   ProcData pd;

   DataLog(log_level_proc_blood_run_info)
      << "VinTarget VinPIRStart VinNextPurge MaxPIRQc Ydch YdchPIR RBCVsvn"
      << endmsg;
   DataLog(log_level_proc_blood_run_info) << pd.Predict().VinTarget << " "
                                          << pd.Predict().VinPIRStart << " "
                                          << pd.Predict().VinNextPurge << " "
                                          << pd.Predict().MaxPIRQc << " "
                                          << pd.Predict().Ydch << " "
                                          << pd.Predict().YdchPIR << " "
                                          << pd.Predict().RBCVsvn << " "
                                          << endmsg;
   DataLog(log_level_proc_blood_run_info)
      << "i DutyCycle InfusionRate OrVtarget OrVtargetType PlasmaValve PlateletValve "
      << "Qac QcD QcR QinD QinR QpD QpR Qrp RBCValve "
      << "T UnadjustedQin UnadjustedQn UnadjustedIR UnadjustedRatio Vtarget VtargetType Csf Rpm"
      << endmsg;

   /* rbc/plasma procedure */
   const int                    rp                = ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC );
   const int                    numberOfSubstates = rp ? (int)pd.Predict().NumOfRBCPlsSubstates : (int)pd.Predict().NumOfSubstates;

   Predict_CDS::substate_info_t allinfo           = pd.Predict().SubstateInfo;

   for ( int i = 0 ; i < numberOfSubstates; i++ )
   {
      const Predict_CDS::one_substate_info_t& info = allinfo[i];
      DataLog(log_level_proc_blood_run_info) << SubstateMap::algStr(i, rp) << " "
                                             << info.DutyCycle << " "
                                             << info.InfusionRate << " "
                                             << info.OrVtarget << " "
                                             << info.OrVtargetType << " "
                                             << info.PlasmaValve << " "
                                             << info.PlateletValve << " "
                                             << info.Qac << " "
                                             << info.QcD << " "
                                             << info.QcR << " "
                                             << info.QinD << " "
                                             << info.QinR << " "
                                             << info.QpD << " "
                                             << info.QpR << " "
                                             << info.Qrp << " "
                                             << info.RBCValve << " "
                                             << info.T << " "
                                             << info.UnadjustedQin << " "
                                             << info.UnadjustedQn << " "
                                             << info.UnadjustedIR << " "
                                             << info.UnadjustedRatio << " "
                                             << info.Vtarget << " "
                                             << info.VtargetType << " "
                                             << info.Csf << " "
                                             << info.Rpm
                                             << endmsg;
   }
}



void Collect::commandCentrifuge (const State_names substate)
{

   ProcData pd;

   float    rpm = CobeConfig::data().RPMCollect;

   if (true /*this is 6.1 baseline*/) // this will need fixin in the 6.3 followon baselines
   {
      rpm = pd.Predict().Rpm(substate);
      if ( rpm  != _PrevRpm )
      {
         DataLog(log_level_proc_info) << "Command rpm : " << rpm << "," << endmsg;
         _PrevRpm = rpm;
      }
   }
   pd.Orders().RPM(rpm);
}


bool Collect::BogusSubState (const State_names state)
{
   ProcData                 pd;

   bool                     isBogus = false;

   VTarget::VolTargetDetail volType = pd.Predict().targetEnum(state);

   if (volType == VTarget::VIN)
   {
      const float Qin_Draw = pd.Predict().Qin(true, state);
      const float targVolm = pd.Predict().volumeTarget(state);

      // but if ....
      if ( (Qin_Draw == 0.0f) && (targVolm != 0.0f) )
      {
         isBogus = true;

         const int rp                = ( pd.TrimaSet().Cassette.Get() != cassette::PLTPLSRBC );
         const int numberOfSubstates = rp ? (int)pd.Predict().NumOfRBCPlsSubstates : (int)pd.Predict().NumOfSubstates;

         DataLog(log_level_proc_blood_run_info) << "Detected Bogus state, skipping state=" << SubstateMap::algStr(state, rp) << endmsg;

      }
   }
   return isBogus;

}

/* FORMAT HASH 776e2095e8e109f7cb804be73a0d1356 */
