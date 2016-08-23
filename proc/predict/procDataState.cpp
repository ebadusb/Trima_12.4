#include "procDataState.h"
#include <cstdio>
#include "trima_datalog_levels.h"
#include "scoped_cds_lock.hpp"
#include "predictConfig.h"

#include "cobeconfig.h"

// some CDS modes are simulation dependent
#if CPU==SIMNT
# define ADJUST_ROLE ROLE_RW
#else
# define ADJUST_ROLE ROLE_RO
#endif



ProcDataState::ProcDataState()
   : _postcount(),
     _posthct(),
     _hypovolemia(),
     _cassette(),

     _predictCDS (ROLE_RW),
     _donorCDS   (ROLE_RO),
     _volumesCDS (ROLE_RO),
     _timeCDS    (ROLE_RO),
     _setCDS     (ROLE_RO),
     _runCDS     (ROLE_RO),
     _cycleCDS   (ROLE_RO),
     _adjCDS     (ADJUST_ROLE), // see above

     _logDonor(false),
     _rcvDonorTimeMsg(false),
     _QinistdMax(0.0f),
     _maxTime(0.0f),
     _predictRequest(CLEAR_ALL)
{
   memset(&_current, 0, sizeof(CurrentConditions) );
   _current.run.substate = INVALID_SUBSTATE;

#if CPU==SIMNT

   const CobeConfig::CobeConfigStruct& _cc = CobeConfig::data();

   ScopedCDSLock adjustlock(_adjCDS);

   _adjCDS.Draw.Active           .Set(false);
   _adjCDS.Draw.Maximum          .Set(_cc.QinLimitMax);
   _adjCDS.Draw.Minimum          .Set(_cc.QinLimitMin);
   _adjCDS.Draw.CurrentCap       .Set(_cc.QinLimitMax);
   _adjCDS.Draw.CurrentValue     .Set(_cc.QinLimitMax);

   _adjCDS.Return.Active         .Set(false);
   _adjCDS.Return.Maximum        .Set(_cc.QneedleLimitMax);
   _adjCDS.Return.Minimum        .Set(50.0f);
   _adjCDS.Return.CurrentCap     .Set(_cc.QneedleLimitMax);
   _adjCDS.Return.CurrentValue   .Set(_cc.QneedleLimitMax);

   _adjCDS.Tingling.Active       .Set(false);
   _adjCDS.Tingling.Maximum      .Set(MAX_INFUSION_RATE);
   _adjCDS.Tingling.Minimum      .Set(MIN_INFUSION_RATE);
   _adjCDS.Tingling.CurrentCap   .Set(MAX_INFUSION_RATE);
   _adjCDS.Tingling.CurrentValue .Set(MAX_INFUSION_RATE);

   _adjCDS.Clumping.Active       .Set(false);
   _adjCDS.Clumping.Maximum      .Set(MAX_RATIO);
   _adjCDS.Clumping.Minimum      .Set(MIN_RATIO);
   _adjCDS.Clumping.CurrentCap   .Set(MAX_RATIO);
   _adjCDS.Clumping.CurrentValue .Set(MAX_RATIO);
#endif

}

ProcDataState::~ProcDataState()
{}

//
// Compares two values. old and new. If old != new, old = new and turns on logging for that update
//

bool ProcDataState::Update ()
{
   // DataLog(log_level_predict_debug) << " Update() " << endmsg;
   //
   ///Donor
   ScopedCDSLock donorlock(_donorCDS);

   UpdateIfNeq(_current.donor.tbv, _donorCDS.TBV.Get(), _logDonor);
   UpdateIfNeq(_current.donor.bmi, _donorCDS.BMI.Get(), _logDonor);
   UpdateIfNeq(_current.donor.preCount, _donorCDS.PreCnt.Get(), _logDonor);
   UpdateIfNeq(_current.donor.hct,
               (_runCDS.AdjustedHct.Get() <= 0.0f)
               ? _donorCDS.Hct.Get()
               : _runCDS.AdjustedHct.Get(), _logDonor);
   UpdateIfNeq(_current.donor.initialHct, _donorCDS.Hct.Get(), _logDonor);
   UpdateIfNeq(_current.donor.weight, _donorCDS.Weight.Get(), _logDonor);
   UpdateIfNeq(_current.donor.bloodType, _donorCDS.BloodType.Get(), _logDonor);
   UpdateIfNeq(_current.donor.female, (bool)_donorCDS.Female.Get(), _logDonor);
   UpdateIfNeq(_current.donor.sampleVol, _donorCDS.SampleVolume.Get(), _logDonor);

   donorlock.release();

   // Volumes
   //
   //  Set current substate for optimization.  Note that the algorithm substate
   //  is -1 for "prerun" predictions.
   //
   _current.run.substate = _runCDS.Substate.Get();

   if (( (_current.run.substate == SS_PIR_PLASMA) || (_current.run.substate == SS_PIR_NOPLASMA)) &&
       _runCDS.SpilloverDuringPIR.Get())
   {
      _current.run.substate = SS_EXTENDED_PCA;
   }


   ScopedCDSLock volumeslock(_volumesCDS);

   _current.bag.Yield             = _volumesCDS.PlateletYield.Get() / 1.0e11f;
   _current.bag.Vcol              = _volumesCDS.PlateletVolume.Get();
   _current.bag.Vac_col           = _volumesCDS.VacPlateletBag.Get();
   _current.bag.Vplasma           = _volumesCDS.PlasmaVolume.Get();
   _current.bag.Vac_plasma        = _volumesCDS.VacPlasmaBag.Get();
   _current.bag.rbcDose           = _volumesCDS.RBCDose.Get();
   _current.bag.Vrbc              = _volumesCDS.RBCVolume.Get();
   _current.bag.Vac_rbc           = _volumesCDS.VacRBCBag.Get();
   _current.bag.Vrbc_a_final      = _volumesCDS.RBCProduct_A_FinalVolume.Get();
   _current.bag.Vrbc_b_final      = _volumesCDS.RBCProduct_B_FinalVolume.Get();

   _current.volumes.rbcDoubleDose = _volumesCDS.RBCDoubleDose.Get();
   _current.volumes.Vrep          = _volumesCDS.VReplacement.Get();
   _current.volumes.VrbcS1        = _volumesCDS.RBCSetup1Volume.Get();
   _current.volumes.VrbcS2        = _volumesCDS.RBCSetup2Volume.Get();
   _current.volumes.VrbcDouble    = _volumesCDS.RBCDoubleVolume.Get();
   _current.volumes.VrbcS1Double  = _volumesCDS.RBCSetup1DoubleVolume.Get();
   _current.volumes.VrbcS2Double  = _volumesCDS.RBCSetup2DoubleVolume.Get();
   _current.volumes.VrbcPrime     = _volumesCDS.RBCPrimeVolume.Get();
   _current.volumes.Vin           = _volumesCDS.Vin.Get();
   _current.volumes.Vac           = _volumesCDS.Vac.Get();
   _current.volumes.Vincv         = _volumesCDS.Vincv.Get();
   _current.volumes.VinEpcaStart  = _volumesCDS.VinEPCAStart.Get();
   _current.volumes.VincvTime     = _volumesCDS.TimeOfVincv.Get();
   _current.volumes.VSalineBolus  = _volumesCDS.VSalineBolus.Get();

   volumeslock.release();


   _current.time.procRunTime = _timeCDS.ProcRunTime();


   ScopedCDSLock runlock(_runCDS);
   ScopedCDSLock cyclelock(_cycleCDS);
   ScopedCDSLock predictlock(_predictCDS);

   _current.run.Qch                   = _runCDS.Qchcurrent.Get();
   _current.run.QchMin                = _runCDS.Qchmin.Get();
   _current.run.npurges               = _runCDS.NumberWBCPurges.Get();
   _current.run.nScheduledPurges      = _runCDS.NScheduledWBCPurges.Get();
   _current.run.recovery              = _runCDS.CurrentRecovery.Get();
   //  _current.run.VinClearLine                = _runCDS.VinClearLine.Get();
   _current.run.WbcPurgeStartTime     = _runCDS.WBCPurgeStartTime.Get();
   _current.run.WbcPurgeType          = _runCDS.WBCPurgeType.Get();
   _current.run.WbcLastProcPurge      = _runCDS.LastProcPurge.Get();
   _current.run.PirSpillover          = _runCDS.SpilloverDuringPIR.Get();
   _current.run.FirstDrawComplete     = _runCDS.FirstDrawComplete.Get();
   _current.run.FirstCycleComplete    = _runCDS.FirstCycleComplete.Get();
   _current.run.SubstateStartTime     = _runCDS.SubstateStartTime.Get();
   _current.run.MidrunEntered         = _runCDS.MidrunEntered.Get();
   _current.run.StopRamping           = _runCDS.stopRamp.Get();
   _current.run.DeadRampMaxQin        = _runCDS.deadRampMaxQin.Get();
   _current.run.LastCentrifugeSpeed   = _runCDS.LastCentrifugeSpeed.Get();

   _current.cycle.Hrbc                = _cycleCDS.Hrbc.Get();
   _current.cycle.VAccumOverdraw      = _cycleCDS.VAccumOverdraw.Get();
   _current.cycle.ResCrit             = _cycleCDS.ResHct.Get();

   _current.run.PtfRbcProd1Disabled   = _runCDS.PTFRBCprod1Disabled.Get();

   _current.cassette.PtfCpsTestPassed = _setCDS.PTFCPSTestPassed.Get();

   _current.predict.LastCalcQchmax    = _predictCDS.lastCalcQchMax.Get();
   _current.predict.PirQinLimit       = _predictCDS.PirQinLimit.Get();
   _current.predict.OptimizingSelProc = _predictCDS.OptimizingSelProc.Get();
   _current.predict.AccumPurges       = _predictCDS.accumulatedPurges.Get();
   _current.predict.AccumYdPur        = _predictCDS.accumulatedYdpur.Get();
   _current.predict.PirCsf            = _predictCDS.PirCsf.Get();
   _current.predict.OptimizingSelProc = _predictCDS.OptimizingSelProc.Get();
   _current.predict.PIRLockin         = _predictCDS.PIRLockin.Get();
   _current.predict.Qcmr              = _predictCDS.Qcmr.Get();
   _current.predict.PlateletPlasmaQch = _predictCDS.PlateletPlasmaQch.Get();
   _current.predict.PrePirQch         = _predictCDS.PrePirQch.Get();

   LogCurrentConditions();
   if (_logDonor)
   {
      _logDonor = false;
   }
   return true;
}

void ProcDataState::LogCurrentConditions () const
{
   if (_logDonor)
   {
      // Careful changing the log format. Always append or add new lines.
      DataLog(log_level_predict_debug) << "Donor Stats.  TBV: " << _current.donor.tbv
                                       << " PreCnt: " << _current.donor.preCount
                                       << " Hct: " << _current.donor.hct
                                       << " Wt: " << _current.donor.weight
                                       << " BT: " << _current.donor.bloodType
                                       << " SampleVol: " << _current.donor.sampleVol
                                       << " Ht: " << _current.donor.height
                                       << endmsg;
   }

   DataLog(log_level_predict_debug) << "Predict: ProcData: subst " << _current.run.substate
                                    << " yld " << _current.bag.Yield
                                    << " Vc " << _current.bag.Vcol << " - " << _current.bag.Vac_col
                                    << " Vp " << _current.bag.Vplasma << " - " <<  _current.bag.Vac_plasma
                                    << " Vrep " << _current.volumes.Vrep
                                    << " Vrbc " << _current.bag.Vrbc
                                    << " Dose " << _current.bag.rbcDose
                                    << " Vin " << _current.volumes.Vin
                                    << " Vac " << _current.volumes.Vac
                                    << " Qch " << _current.run.Qch
                                    << " minQch " << _current.run.QchMin
                                    << " maxQch " << _current.predict.LastCalcQchmax // set in Procdetpltplsrbc now
                                    << " time " << _current.time.procRunTime
                                    << " PIRSpill " << _current.run.PirSpillover
                                    << " Purges " << _current.run.npurges
                                    << " Vincv " << _current.volumes.Vincv
                                    << " VincvTm " << _current.volumes.VincvTime
                                    << " EPCAstart " << _current.volumes.VinEpcaStart
                                    << " VrbcS1 " << _current.volumes.VrbcS1
                                    << " VrbcS2 " << _current.volumes.VrbcS2
                                    << " VrbcS1-double " << _current.volumes.VrbcS1Double
                                    << " VrbcS2-double " << _current.volumes.VrbcS2Double
                                    << " VrbcPr " << _current.volumes.VrbcPrime
                                    << " Vrbc-double " << _current.volumes.VrbcDouble
      // << " CurrentRecovery " << _current.run.recovery
      // << " VinClearLine " << _current.run.VinClearLine
                                    << " WBCPurgeStartTime " << _current.run.WbcPurgeStartTime
                                    << " WBCPurgeType " << _current.run.WbcPurgeType
                                    << " SubstateStartTime " << _current.run.SubstateStartTime
                                    << " MidrunEntered " << _current.run.MidrunEntered
                                    << " Scheduled Purges " << _current.run.nScheduledPurges
                                    << " PredictRequest " << _current.predict.PredictRequest /* this is really coming in from proc, but as a message not a CDS var */
                                    << " LastProcPurge "  << _current.run.WbcLastProcPurge
                                    << endmsg;

   DataLog(log_level_predict_debug) << "PredictData: AccumPurges " << _current.predict.AccumPurges
                                    << " AccumYdPur " << _current.predict.AccumYdPur
                                    << " LastCalcQchmax "  << _current.predict.LastCalcQchmax
                                    << " PirCsf " << _current.predict.PirCsf
                                    << " OptimizingSelProc " << _current.predict.OptimizingSelProc
                                    << " PIRLockin " << _current.predict.PIRLockin
                                    << " PirQinLimit " <<  _current.predict.PirQinLimit
                                    << " Qcmr " <<  _current.predict.Qcmr
                                    << " PlateletPlasmaQch " <<  _current.predict.PlateletPlasmaQch
                                    << " PrePirQch " <<  _current.predict.PrePirQch
                                    << endmsg;
}

// Call reset whenever the 'current' procedure changes.
void ProcDataState::Reset (int substate)
{
   ScopedCDSLock predictlock(_predictCDS);
   if (substate <= Config::__MIDRUN)
   {
      _predictCDS.PirCsf.Set(1.0f);
      _predictCDS.PirQinLimit.Set(0.0f);
   }
   // _predictCDS.lastCalcQchMax.Set(0.0f);
   // _predictCDS.OptimizingSelProc.Set(false);
   _predictCDS.PIRLockin.Set(0.0f);
   // _predictCDS.Qcmr.Set(0.0f);
}


//////////////////////////////////////////////////////////////////////
//
// Meyers singleton -- no pointers or dynamic memory.  Guaranteed to be
// constructed before first use.  This code is *not* thread safe.  It's
// reentrancy is somewhat debatable.  Neither issue is a problem as long as
// it is only used in the Predict Task.
//
ProcDataState& ProcDataState::Instance ()
{
   static ProcDataState instance;
   return instance;
}

/* FORMAT HASH 8b593cbf7e58e9017f6355c24a2377a2 */
