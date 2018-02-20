/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      adjustctrl.cpp
 *        This object manages the interaction between the GUI adjust
 *        screen and PROC.
 *
 * AUTHOR:     Bruce Dietrich
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#include "adjustctrl.h"
#include "run_defs.h"
#include "cobeconfig.h"
#include "trimamessages.h"
#include "states.h"
#include "predict_msgs.h"
#include "procdata.h"

//
// Adjustment constants ...
//
static const float MAX_ADJUSTMENT_SCALE   = 10.0f;
static const float ACCESS_ADJUSTMENT_STEP = 5.0f;
static const float RETURN_ADJUSTMENT_STEP = 30.0f; // IT3828
static const float AUTOFLOW_INCREASE_STEP = CobeConfig::data().AutoflowIncreaseIncrement;
static const float AUTOFLOW_DECREASE_STEP = CobeConfig::data().AutoflowDecreaseIncrement;

//


#if (CPU==SIMNT)
static const int UPDATE_DELTA = 0; // turn off for the simulator
#else
static const int UPDATE_DELTA = 5000; // milli-seconds
#endif /* (CPU!=SIMNT) */

using namespace procedure_adjustment_status;

AdjustCtrl::AdjustCtrl()
   : _RequestMsg  (Callback<AdjustCtrl>(this, &AdjustCtrl::ProcessRequest) ),
     _StatusMsg   (MessageBase::SEND_LOCAL),
     _UpdateTimer (UPDATE_DELTA, Callback< AdjustCtrl >(this, &AdjustCtrl::SendAdjustStatus), TimerMessage::ARMED),
     _LastCassetteState      (ProcTrimaSet_CDS::DRY),
     _ChangeCount            (0),
     _AlarmActivePrevious    (false),
     _MaxQinMonitoringActive (false),
     _autoFlowAdjustRequest  (false),
     _currentAdjustment      (0)
{}

AdjustCtrl::~AdjustCtrl()
{}

void AdjustCtrl::Initialize ()
{
   ProcData pd;
   pd.MakeAdjustmentsWritable();

   pd.Adjustments().Draw.Active.Set(false);
   pd.Adjustments().Draw.Maximum.Set(CobeConfig::data().QinLimitMax);
   pd.Adjustments().Draw.Minimum.Set(CobeConfig::data().QinLimitMin);
   pd.Adjustments().Draw.CurrentCap.Set(CobeConfig::data().QinLimitMax);
   pd.Adjustments().Draw.CurrentValue.Set(CobeConfig::data().QinLimitMax);

   pd.Adjustments().Return.Active.Set(false);
   pd.Adjustments().Return.Maximum.Set(CobeConfig::data().QneedleLimitMax);
   pd.Adjustments().Return.Minimum.Set(CobeConfig::data().QneedleLimitMin);
   pd.Adjustments().Return.CurrentCap.Set(CobeConfig::data().QneedleLimitMax);
   pd.Adjustments().Return.CurrentValue.Set(CobeConfig::data().QneedleLimitMax);

   pd.Adjustments().Tingling.Active.Set(false);
   pd.Adjustments().Tingling.Maximum.Set(MAX_INFUSION_RATE);
   pd.Adjustments().Tingling.Minimum.Set(MIN_INFUSION_RATE);
   pd.Adjustments().Tingling.CurrentCap.Set(MAX_INFUSION_RATE);
   pd.Adjustments().Tingling.CurrentValue.Set(MAX_INFUSION_RATE);

   pd.Adjustments().Clumping.Active.Set(false);
   pd.Adjustments().Clumping.Maximum.Set(MAX_RATIO);
   pd.Adjustments().Clumping.Minimum.Set(MIN_RATIO);
   pd.Adjustments().Clumping.CurrentCap.Set(MAX_RATIO);
   pd.Adjustments().Clumping.CurrentValue.Set(MAX_RATIO);

   pd.MakeAdjustmentsReadable();

   _LastCassetteState   = ProcTrimaSet_CDS::DRY;
   _ChangeCount         = 0;
   _AlarmActivePrevious = false;
}



bool AdjustCtrl::Update ()
{
   ProcData pd;
   bool     DrawActive, ReturnActive, IRActive, ClumpingActive;
   bool     needUpdate = false;

   //
   //
   //   IT9829: Do not update meters during an alarm condition, or the
   //   following cycle.  Waiting one cycle allows the alarm clear condition
   //   to propogate through proc and reset commanded speeds.
   //
   bool ignoreDueToAlarm = false;

   if ( pd.AlarmActive() || _AlarmActivePrevious)
   {
      _AlarmActivePrevious = pd.AlarmActive();
      ignoreDueToAlarm     = true;
   }

   //
   //
   //  IT13202: also ignore flow changes during pumps pause recovery
   //
   if (RecoveryTypes::RecoveryId(pd.RecoveryName().c_str()) == RecoveryTypes::PumpsPause)
   {
      _AlarmActivePrevious = true; // will get reset by alarm logic above
      ignoreDueToAlarm     = true;
   }

   float qin   = pd.Adjustments().Draw.CurrentValue.Get();
   float qrp   = pd.Adjustments().Return.CurrentValue.Get();
   float ir    = pd.Adjustments().Tingling.CurrentValue.Get();
   float ratio = pd.Adjustments().Clumping.CurrentValue.Get();

   if ( pd.TrimaSet().CassetteState.Get() != ProcTrimaSet_CDS::RINSEBACK )
   {
      if ( pd.Run().DrawCycle.Get() )
      {
         if ( pd.Status().ReturnPump.CmdFlow() == 0.0f )
         {
            if ( !ignoreDueToAlarm && (pd.Status().InletPump.CmdFlow() > 0.0f ) )
               qin = pd.Status().InletPump.CmdFlow();
         }
      }
      else
      {
         if ( pd.Status().ReturnPump.CmdFlow() > 0.0f )
         {
            if ( !ignoreDueToAlarm )
               qrp = pd.Status().ReturnPump.CmdFlow() - pd.Status().InletPump.CmdFlow();
         }
      }

      ir    = pd.Run().LastRunInfusion.Get();
      ratio = pd.Run().LastRunRatio.Get();

      //
      //
      //   Determine which meters are active
      //
      DrawActive     = true;

      ReturnActive   = ( pd.Run().FirstCycleComplete.Get() || !pd.Run().DrawCycle.Get());

      IRActive       = ( pd.Run().FirstCycleComplete.Get() && (pd.Run().LastRunInfusion.Get() > 0.0f) );

      ClumpingActive = (    IRActive
                            && ( !pd.Status().RBCValve.IsCollecting() )
                            && ( pd.TrimaSet().Cassette.Get() == cassette::PLTPLSRBC ) // is it PLT
                            && ( pd.Run().FirstCycleComplete.Get() )                   // make clumping button availabel after first draw
                            && ( pd.Run().NumDraw.Get() > 1 )                          // make clumping button availabel after first draw
                            );
   }
   else
   {
      if ( pd.Status().ReturnPump.CmdFlow() > 0.0f )
         qrp = pd.Status().ReturnPump.CmdFlow();

      //
      //
      //   Determine which meters are active
      //
      DrawActive     = false;
      ClumpingActive = false;

      if ( pd.Run().RinsebackStarted.Get() )
      {
         //
         // past recirc point
         ReturnActive = true;
         IRActive     = true;
      }
      else
      {
         ReturnActive = false;
         IRActive     = false;
      }

      //
      //
      //   In rinseback we do not change the last dyndata info although we
      //   will allow the cap to lower the values.  In order to make the display
      //   look right we can use the same logic that the rinseback phase does
      //   to ensure that the operator is seeing the actual behavior.
      //
      float irCap = pd.Adjustments().Tingling.CurrentCap.Get();

      if ( irCap > pd.Run().LastRunInfusion.Get() )
      {
         irCap = pd.Run().LastRunInfusion.Get();
      }

      if ( irCap != ir )
      {
         ir         = irCap;
         needUpdate = true;
      }
   }


   //
   //   Update internal meter variables
   //
   needUpdate |= (_LastCassetteState != pd.TrimaSet().CassetteState.Get());
   needUpdate |= SetQin     (DrawActive, qin);
   needUpdate |= SetQreturn (ReturnActive, qrp);
   needUpdate |= SetInfusion(IRActive, ir);
   needUpdate |= SetRatio   (ClumpingActive, ratio);

   //
   // Update the cassette state var...
   //
   _LastCassetteState = pd.TrimaSet().CassetteState.Get();

   return needUpdate;
}

BUTTON_VISIBILITY_STATUS AdjustCtrl::BtnAirblock () const
{
   ProcData pd;
   return pd.Adjustments().AirBlock.Get();
}

void AdjustCtrl::BtnAirblock (BUTTON_VISIBILITY_STATUS st)
{
   ProcData pd;
   pd.MakeAdjustmentsWritable();
   pd.Adjustments().AirBlock.Set(st);
   pd.MakeAdjustmentsReadable();
}

BUTTON_VISIBILITY_STATUS AdjustCtrl::BtnSpillover () const
{
   ProcData pd;
   return pd.Adjustments().Spillover.Get();
}

void AdjustCtrl::BtnSpillover (BUTTON_VISIBILITY_STATUS st)
{
   ProcData pd;
   pd.MakeAdjustmentsWritable();
   pd.Adjustments().Spillover.Set(st);
   pd.MakeAdjustmentsReadable();
}

BUTTON_VISIBILITY_STATUS AdjustCtrl::BtnSalinebolus () const
{
   ProcData pd;
   return pd.Adjustments().SalineBolus.Get();
}

void AdjustCtrl::BtnSalinebolus (BUTTON_VISIBILITY_STATUS st)
{
   ProcData pd;
   pd.MakeAdjustmentsWritable();
   pd.Adjustments().SalineBolus.Set(st);
   pd.MakeAdjustmentsReadable();
}

void AdjustCtrl::ProcessRequest ()
{
   ProcData          pd;
   AdjustmentPayload payload(_RequestMsg.getData());
   // The type of requested adjustment is in the payload's enum; the serial
   // number of the adjustment is in the value.
   int rq = payload._enum;
   _currentAdjustment = payload._value;

   DataLog(log_level_gui_info) << "ADJCTRL: got adjust request, payload " << payload
                               << endmsg;

   int beginChangeCount = _ChangeCount;
   _autoFlowAdjustRequest = false;

   switch ( rq )
   {


      case AUTOFLOW_QIN_INCREASE :
         _autoFlowAdjustRequest = true;
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Draw, AUTOFLOW_INCREASE_STEP);
         pd.MakeAdjustmentsReadable();
         DataLog (log_level_gui_info) << "AUTOFLOW_INCREASE_STEP"  << endmsg;
         break;
      case AUTOFLOW_QIN_DECREASE :
         _autoFlowAdjustRequest = true;
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Draw, -AUTOFLOW_DECREASE_STEP);
         pd.MakeAdjustmentsReadable();
         DataLog (log_level_gui_info) << "AUTOFLOW_DECREASE_STEP"  << endmsg;
         break;
      case ACCESS_PRESSURE_UP :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Draw, ACCESS_ADJUSTMENT_STEP);
         pd.MakeAdjustmentsReadable();
         break;
      case ACCESS_PRESSURE_DOWN :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Draw, -ACCESS_ADJUSTMENT_STEP);
         pd.MakeAdjustmentsReadable();
         break;
      case RETURN_PRESSURE_UP :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Return, RETURN_ADJUSTMENT_STEP);
         pd.MakeAdjustmentsReadable();
         break;
      case RETURN_PRESSURE_DOWN :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Return, -RETURN_ADJUSTMENT_STEP);
         pd.MakeAdjustmentsReadable();
         break;
      case TINGLING_UP :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Tingling,
                       (   pd.Adjustments().Tingling.Maximum.Get()
                           - pd.Adjustments().Tingling.Minimum.Get() )
                       / MAX_ADJUSTMENT_SCALE);
         pd.MakeAdjustmentsReadable();
         break;
      case TINGLING_DOWN :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Tingling,
                       (   pd.Adjustments().Tingling.Maximum.Get()
                           - pd.Adjustments().Tingling.Minimum.Get() )
                       / -MAX_ADJUSTMENT_SCALE);
         pd.MakeAdjustmentsReadable();
         break;
      case CLUMPING_UP :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Clumping,
                       (   pd.Adjustments().Clumping.Maximum.Get()
                           - pd.Adjustments().Clumping.Minimum.Get() )
                       / MAX_ADJUSTMENT_SCALE);
         pd.MakeAdjustmentsReadable();
         break;
      case CLUMPING_DOWN :
         pd.MakeAdjustmentsWritable();
         IncAdjustment(pd.Adjustments().Clumping,
                       (   pd.Adjustments().Clumping.Maximum.Get()
                           - pd.Adjustments().Clumping.Minimum.Get() )
                       / -MAX_ADJUSTMENT_SCALE);
         pd.MakeAdjustmentsReadable();
         break;
      case SPILLOVER :
      {
         SpilloverRecoveryMsg spillover(MessageBase::SEND_LOCAL);
         spillover.send(1);
         return;
      }
      case AIR_IN_PLASMA_LINE :
      {
         AirInPlasmaLineMsg airinplasma(MessageBase::SEND_LOCAL);
         airinplasma.send(1);
         return;
      }
      case SALINE_BOLUS :
      {
         SalineBolusMsg salinebolus(MessageBase::SEND_LOCAL);
         salinebolus.send(1);
         return;
      }
      case ADJUSTMENT_SCREEN_CLOSED :
         // The trouble screen doesn't wait for an ack for ADJUSTMENT_SCREEN_CLOSED, and we don't
         // care, so no processing for this case.
         return;
      default :
         return;
   }
   pd.MakeAdjustmentsReadable();


   const bool internal_autoAdjustment =    (rq == AUTOFLOW_QIN_INCREASE )
                                        || (rq == AUTOFLOW_QIN_DECREASE );


   //
   // If something changed ...
   //
   bool need_update = false;

   if ( beginChangeCount != _ChangeCount )
   {
      ProcData pd;
      pd.MakeRunWritable();

      if ( internal_autoAdjustment )
      {
         pd.Run().PredictRequest.Set(pd.Run().PredictRequest.Get() | AUTO_FLOW_ADJUSTMENT);
      }
      else
      {
         pd.Run().PredictRequest.Set(pd.Run().PredictRequest.Get() | ADJUSTMENT);
      }

      pd.MakeRunReadable();

      need_update = Update();
   }


   // for gui feedback and direct predict call
   // // noop for autoflow
   if (!internal_autoAdjustment)
   {
      // If we get to here, the incoming ProcedureAdjustmentMsg was an adjustment
      // i.e. ACCESS_PRESSURE_UP <= rq <= CLUMPING_DOWN.  In this case, we need to ack the message.

      AdjustmentStatusPayload status_payload( (need_update
                                               ? ADJUSTMENT_UPDATE
                                               : ADJUSTMENT_ACK),
                                              _currentAdjustment);

      DataLog(log_level_gui_info) << "ADJCTRL: Sending Status ACK: " << status_payload
                                  << endmsg;

      _StatusMsg.send(status_payload);

      // ... and we need to immediately ping Predict if we did something.
      if (need_update)
      {
         DoPredictionPayload pred_payload(ADJUSTMENT, _currentAdjustment);
         DoPredictionMsg     do_predict (MessageBase::SEND_LOCAL);
         do_predict.send(pred_payload);

         DataLog(log_level_gui_info) << "ADJCTRL: Sending DoPredict: " << pred_payload
                                     << endmsg;
      }
   }
   else
   {
      DataLog(log_level_gui_info) << "AUTOFLOW-ADJCTRL: AUTO_FLOW_ADJUSTMENT queued for predict. (" << rq << ")" << endmsg;
   }

}

void AdjustCtrl::SendAdjustStatus ()
{
   _StatusMsg.send(ADJUSTMENT_UPDATE);
}

void AdjustCtrl::IncAdjustment (ProcAdjustBar_CDS& cds, float val)
{
   const float maximum      = cds.Maximum.Get();
   const float minimum      = cds.Minimum.Get();
   const float currentValue = cds.CurrentValue.Get();
   float currentCap = cds.CurrentCap.Get();

   DataLog(log_level_proc_info) << "Adjustment Increment: " << maximum
                                   << " " << minimum
                                   << " " << currentValue
                                   << " " << val
                                   << ", current cap " << currentCap
                                   << endmsg;

   if(!_autoFlowAdjustRequest)
   {
      // if no autoflow adjustment request then make cap down before increase
      if((currentCap > currentValue) && (val < 0))
      {
         currentCap = currentValue;
      }
      DataLog(log_level_proc_info) << "Adjustment Increment: oldcap: " << currentCap << endmsg;
      currentCap = currentCap + val;
   }
   else
   {
      DataLog(log_level_proc_info) << "Adjustment Increment: oldcap: " << currentCap << endmsg;
      currentCap = currentValue + val;
   }

   if ( currentCap > maximum )
   {
      currentCap = maximum;
   }

   if ( currentCap < minimum )
   {
      currentCap = minimum;
   }

   //
   // Keep track of any changes ...
   //
   _ChangeCount += (( cds.CurrentCap.Get() == currentCap ) ? 0 : 1);

   cds.CurrentCap.Set(currentCap);
   DataLog(log_level_proc_info) << "Adjustment Increment: newcap: " << cds.CurrentCap.Get() << endmsg;
}

bool AdjustCtrl::SetQin (const bool active, float current)
{
   ProcData     pd;
   static float lastLoggedQin = 0.0f;

   if ( lastLoggedQin != current )
   {
      DataLog(log_level_gui_info) << "Adjustment Qin change: " << current
                                  << " from: " << lastLoggedQin << endmsg;
      lastLoggedQin = current;
   }

   if ( current > pd.Adjustments().Draw.CurrentCap.Get() )
      current = pd.Adjustments().Draw.CurrentCap.Get();

   //
   //
   //    Capture max draw flow during procedure for end run summary display
   //
   float maxqin = pd.Adjustments().MaxQin.Get();

   //
   //
   //   Monitoring starts if we are in any collection state.  Assumption is CDS
   //   initializes MaxQin to zero as default.
   //
   bool shouldmonitor = pd.IsProductCollectionSubstate();
   if (shouldmonitor != _MaxQinMonitoringActive)
   {
      if (!_MaxQinMonitoringActive)
      {
         DataLog(log_level_gui_info) << "Adjustment max Qin monitoring enabled." << endmsg;
      }
      else
      {
         DataLog(log_level_gui_info) << "Adjustment max Qin monitoring disabled." << endmsg;
      }
      _MaxQinMonitoringActive = shouldmonitor;
   }

   if ( _MaxQinMonitoringActive && (current > maxqin) )
   {
      maxqin = current;
      DataLog(log_level_gui_info) << "Adjustment new max Qin found: " << current << endmsg;
   }

   const bool updated = ( (pd.Adjustments().Draw.Active.Get() != active) ||
                          (pd.Adjustments().Draw.CurrentValue.Get() != current) );

   pd.MakeAdjustmentsWritable();
   pd.Adjustments().Draw.Active.Set(active);
   pd.Adjustments().Draw.CurrentValue.Set(current);
   pd.Adjustments().MaxQin.Set(maxqin);
   pd.MakeAdjustmentsReadable();

   return updated;
}

bool AdjustCtrl::SetQreturn (const bool active, float current)
{
   ProcData pd;

   if ( current > pd.Adjustments().Return.CurrentCap.Get() )
      current = pd.Adjustments().Return.CurrentCap.Get();

   bool updated = ( (pd.Adjustments().Return.Active.Get() != active) ||
                    (pd.Adjustments().Return.CurrentValue.Get() != current) );

   pd.MakeAdjustmentsWritable();
   pd.Adjustments().Return.Active.Set(active);
   pd.Adjustments().Return.CurrentValue.Set(current);
   pd.MakeAdjustmentsReadable();

   return updated;
}

bool AdjustCtrl::SetInfusion (const bool active, float current)
{
   ProcData pd;

   if ( current > pd.Adjustments().Tingling.CurrentCap.Get() )
      current = pd.Adjustments().Tingling.CurrentCap.Get();

   bool updated = ( (pd.Adjustments().Tingling.Active.Get() != active) ||
                    (pd.Adjustments().Tingling.CurrentValue.Get() != current) );

   pd.MakeAdjustmentsWritable();
   pd.Adjustments().Tingling.Active.Set(active);
   pd.Adjustments().Tingling.CurrentValue.Set(current);
   pd.MakeAdjustmentsReadable();

   return updated;
}

bool AdjustCtrl::SetRatio (const bool active, float current)
{
   ProcData pd;

   if ( current > pd.Adjustments().Clumping.CurrentCap.Get() )
      current = pd.Adjustments().Clumping.CurrentCap.Get();

   bool updated = ( (pd.Adjustments().Clumping.Active.Get() != active) ||
                    (pd.Adjustments().Clumping.CurrentValue.Get() != current) );

   pd.MakeAdjustmentsWritable();
   pd.Adjustments().Clumping.Active.Set(active);
   pd.Adjustments().Clumping.CurrentValue.Set(current);
   pd.MakeAdjustmentsReadable();

   return updated;
}

/* FORMAT HASH cbc614c2ed1ee55d796455e188083b59 */
