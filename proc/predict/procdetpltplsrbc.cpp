/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procdetpltplsrbc.cpp
 *
 */

#include <vxworks.h>
#include <cassert>
#include <cmath>

#include "procdetpltplsrbc.h"
#include "substatemap.h"
#include "recoverytypes.h"
#include "residualAlgorithm.h"
#include "software_cds.h"
#include "purgecalcs.h"



#ifndef __SKIPMSGSYS__
#include "an_alarm.h"
#endif

procedureDetailPltPlsRBC::procedureDetailPltPlsRBC(PIRType pt)
   : procedureDetail(),
     _startingsubstate(0),
     _Vincv(0.0f),
     _VincvTime(0.0f),
     _Qnmax(0.0f),
     _Qinr(0.0f),
     _QinrQnLimit(0.0f),
     _TBV(0.0f),
     _hct(0.0f),
     _precount(0.0f),
     _bmi(0.0f),
     _ratio(0.0f),
     _oneM1Ratio(0.0f),
     _totalCollectTime(0.0f),
     _QcmrTime(0.0f),
     _QcmrVol(0.0f),
     _latchQcmr(false),
     _Yield(0.0f),
     // _PIRRequired(false),
     _PIRAvgQin(0.0f),
     _RBCTime(0.0f),
     _EPCAVc(0.0f),
     _PCAVc(0.0f),
     _maxEPCAVc(0.0f),
     _PCAmlVinpermlVc(0.0f),
     _EPCAmlVinpermlVc(0.0f),
     _MidrunmlVinpermlVc(0.0f),
     _ACinRBC(0.0f),
     _RBCVin(0.0f),
     _RBCVac(0.0f),
     _VinPIR(0.0f),
     _PIRVcAtMax(0.0f),
     _VcOvercollect(0.0f),
     _lastPredVc(0.0f),
     _RBCSetup1Time(0.0f),
     _RBCSetup1Vin(0.0f),
     _RBCSetup1Vac(0.0f),
     _RBCSetup2Time(0.0f),
     _RBCSetup2Vin(0.0f),
     _RBCSetup2Vac(0.0f),
     _RBCPrimeTime(0.0f),
     _RBCPrimeVin(0.0f),
     _RBCPrimeVac(0.0f),
     _PTF(false),
     _purges(0),
     _deltaPurge(0.0f),
     _Ypur(0.0f),
     _purgeTime(0.0f),
     _purgeEndTime(0.0f),
     _purgeQcmrLock(0.0f),
     _deadbandBeingEnforced(false),
     _QchConvergenceFloor(0.0f),
     _lastPredPIR(false),
     _currentvp(0.0f),
     _currentacinvp(0.0f),
     _currentvc(0.0f),
     _currentacinvc(0.0f),
     _currenttime(0.0f),
     _currentQch(0.0f),
     _Qch(0.0f),


     _prevQcmr(0.0f),
     _CollectMaximumFlowLock(0.0f),
     _CollectMaximumFlow(0.0f),
     _ppcQcmrDropFlag(false),
     _EndVinPir(0.0f),
     _DTp(0.0f),
     _purgeSkipped(false),
     _Fw(0.0f),
     _QcmrConvCoeff(0.4f),


     _nextProcPurge(0.0f),
     _nextProcPurgeTime(0.0f)
{

   if (pt == PIR_STD)
      _pirAlgo =  new PirAlgo(this);
   else
      _pirAlgo = new PirAlgoLR(this);

   _numOfSubstates      = Predict_CDS::NumOfSubstates;
   _recommendedCassette = cassette::PLTPLSRBC;


   memset(&_prePltPlsEndOfRamp, 0, sizeof(IntermediateResults) );
   memset(&_prePltNoPlsEndOfRamp, 0, sizeof(IntermediateResults) );


   DataLog(log_level_predict_info) << "PIR algorithm for procedure: "
                                   << ( (pt == PIR_STD) ? "STD" : "PPC" )
                                   << endmsg;
}


procedureDetailPltPlsRBC::~procedureDetailPltPlsRBC()
{
   delete   _pirAlgo;
}

std::string procedureDetailPltPlsRBC::LogQin (int subndx) const
{
   std::ostringstream s;
   if ( subndx == Config::__PREPLATELET_PLASMA)
   {
      s << substate[subndx].Qininstd << "/" << _prePltPlsEndOfRamp.Qininstd;
   }
   else
   {
      if (subndx == Config::__PREPLATELET_NOPLASMA)
         s << substate[subndx].Qininstd << "/" << _prePltNoPlsEndOfRamp.Qininstd;
      else
         s << substate[subndx].Qininstd;
   }
   return s.str();
}

std::string procedureDetailPltPlsRBC::LogQp (int subndx) const
{
   std::ostringstream s;
   if ( subndx == Config::__PREPLATELET_PLASMA)
   {
      s << substate[subndx].Qpinstd << "/" << _prePltPlsEndOfRamp.Qpinstd;
   }
   else
   {
      if (subndx == Config::__PREPLATELET_NOPLASMA)
         s << substate[subndx].Qpinstd << "/" << _prePltNoPlsEndOfRamp.Qpinstd;
      else
         s << substate[subndx].Qpinstd;
   }
   return s.str();
}

void procedureDetailPltPlsRBC::LogFirstReturnNoCollectFlows () const
{
   DataLog(log_level_predict_debug) << "FirstReturn_NoCollect" << " "
                                    << _firstReturnNoCollect.T << " "
                                    << _firstReturnNoCollect.EndVin << " "
                                    << _firstReturnNoCollect.AccumVac << " "
                                    << _firstReturnNoCollect.Qininstd << " " << _firstReturnNoCollect.Qininstr << " "
                                    << _firstReturnNoCollect.Qpinstd << " "
                                    << _firstReturnNoCollect.Qcinstd << " "
                                    << _firstReturnNoCollect.Qacinst << " "
                                    << _firstReturnNoCollect.Qrinst << " "
                                    << _firstReturnNoCollect.Qch << " "
                                    << _firstReturnNoCollect.AccumVp << " "
                                    << _firstReturnNoCollect.AccumVc << " "
                                    << _firstReturnNoCollect.HypovolemiaDistance << " "
                                    << _firstReturnNoCollect.Vsvn << " "
                                    << _firstReturnNoCollect.Csf  << " "
                                    << _firstReturnNoCollect.Rpm << " "
                                    << /*Yield=0 here*/ 0 << " "
                                    << _firstReturnNoCollect.D << " "
                                    << _firstReturnNoCollect.Tr << " "
                                    << MIN(_I, getMaxACRate())
                                    << endmsg;

}

//
// -----------------------------------------------------------------------------
//
void procedureDetailPltPlsRBC::LogTargets () const
{
   DataLog(log_level_predict_debug) << "Substate T EndVin AccumVac Qind Qinr Qp Qc Qac Qr Qch AccumVp AccumVc HypoD Vsvn Csf Rpm Yield D Tr I" << endmsg;

   DataLog(log_level_predict_debug).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_predict_debug).precision(3);
   if (_startingsubstate < Config::__PREPLATELET_PLASMA)
   {
      LogFirstReturnNoCollectFlows();
   }
   for (int subndx = 0; subndx<getNumOfSubstates(); subndx++)
   {
      if (subndx==0 || substate[subndx].T > 0.0f)
      {
         DataLog(log_level_predict_debug) << SubstateMap::algStr(subndx) << " "
                                          << substate[subndx].T << " "
                                          << substate[subndx].EndVin << " "
                                          << substate[subndx].AccumVac << " "
                                          << LogQin(subndx) << " " << substate[subndx].Qininstr << " "
                                          << LogQp(subndx) << " "
                                          << substate[subndx].Qcinstd << " "
                                          << substate[subndx].Qacinst << " "
                                          << substate[subndx].Qrinst << " "
                                          << substate[subndx].Qch << " "
                                          << substate[subndx].AccumVp << " "
                                          << substate[subndx].AccumVc << " "
                                          << substate[subndx].HypovolemiaDistance << " "
                                          << substate[subndx].Vsvn << " "
                                          << substate[subndx].Csf  << " "
                                          << substate[subndx].Rpm << " "
                                          << substate[subndx].Yield << " "
                                          << substate[subndx].D << " "
                                          << substate[subndx].Tr << " "
                                          << instantaneousInfusion(subndx)
                                          << endmsg;
      }
   }

   if (_pirAlgo->getPirRequired()) DataLog(log_level_predict_debug) << "PIR volume at max: " << _PIRVcAtMax << endmsg;


   DataLog(log_level_predict_debug) << " PIRBracket: " << _pirAlgo->getLockin()
                                    << " purges: " << _purges
                                    << " ydpur:" << _Ydpur
                                    << " ypur:" << Ypur(Config::__MIDRUN)
                                    << " Ydch: " << _Ydch
                                    << " Purge Skipped: " << _purgeSkipped
                                    << " total remaining purge time: " << _purgeTime
                                    << " this purge ends at: " << _purgeEndTime
                                    << endmsg;

   DataLog(log_level_predict_debug) << "Calculated Purge point: " << _Vinpur << " delta: " << _deltaPurge
                                    << " nextPurge: " << _nextProcPurge << " nextPurgeTime: " << _nextProcPurgeTime
                                    << " YdchNoPir: " << YdchNoPIR() << " YdchPir: " << YdchPIR()
                                    << " Vintarg: " << _Vintarg   << " Qcmr: " << _Qcmr
                                    << " Ec: " << _Ec << " Qinc: " << _Qinc << " Fw: " << _Fw << endmsg;

}
//
// -----------------------------------------------------------------------------
//
void procedureDetailPltPlsRBC::SetConstants ()
{

   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::SetConstants()";

   //
   //
   //   Calculate starting substate
   //
   _startingsubstate = AlgSubstate(_procState.SubState());
   if (_startingsubstate < 1) _startingsubstate = 1;


   if (!(_procState.getPredictRequest() & REPREDICT_ONLY) && (_convergenceCtr == 0) )
   {
      if (_pirAlgo->getPirRequired())
      {
         DataLog(log_level_predict_info) << "procedure " << getProcedureNumber() <<  " Reset PIR flag" << endmsg;
         _pirAlgo->setPirRequired(0);
      }
   }



   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   _TBV        = _procState.Tbv();
   _hct        = _procState.Hct();
   _bmi        = _procState.Bmi();
   _ratio      = _config.getACRatio();
   _oneM1Ratio = (1.0f - 1.0f / _ratio);
   _precount   = _procState.PreCount();
   _PTF        = getMSSRbc();

   //
   //
   //   Zero other members
   //
   _Vincv                 = 0.0f;
   _VincvTime             = 0.0f;
   _Qnmax                 = 0.0f;
   _PIRAvgQin             = 0.0f;
   _maxEPCAVc             = 0.0f;
   _PCAmlVinpermlVc       = 0.0f;
   _EPCAmlVinpermlVc      = 0.0f;
   _MidrunmlVinpermlVc    = 0.0f;
   _ACinRBC               = 0.0f;
   _RBCVin                = 0.0f;
   _RBCVac                = 0.0f;
   _purges                = 0;
   _purgeTime             = 0.0f;
   _purgeEndTime          = 0.0f;
   //  _purgeQcmrLock        = 0.0f;  do not reset
   _deadbandBeingEnforced = (_procState.getPredictRequest() & REPREDICT_ONLY); // deadband active for current procedure repredictions only
   _QchConvergenceFloor   = 0.0f;
   _deltaPurge            = 0.0f;
   _Ypur                  = 0.0f;
   _Qch                   = 0.0f;
   // _PIRRequired = 0;
   _RBCTime               = 0.0f;
   _Qinr                  = 0.0f;
   _QinrQnLimit           = 0.0f;
   _VinPIR                = 0.0f;
   // _PIRConv = 0;
   _PIRVcAtMax            = 0.0f;
   _VcOvercollect         = 0.0f;
   _EPCAVc                = 0.0f;
   _PCAVc                 = 0.0f;
   _PredVc                = 0.0f;
   _PredVp                = 0.0f;
   _lastPredVc            = 0.0f;
   _RBCSetup1Time         = 0.0f;
   _RBCSetup1Vin          = 0.0f;
   _RBCSetup1Vac          = 0.0f;
   _RBCSetup2Time         = 0.0f;
   _RBCSetup2Vin          = 0.0f;
   _RBCSetup2Vac          = 0.0f;
   _RBCPrimeTime          = 0.0f;
   _RBCPrimeVin           = 0.0f;
   _RBCPrimeVac           = 0.0f;
   _QcmrConvCoeff         = 0.4f;
   _nextProcPurge         = 0.0f;
   _nextProcPurgeTime     = 0.0f;
   _ppcQcmrDropFlag       = false;

   _extracorporealVolume  = cobeConfig.PltPlsRBCExtracorporealVolume;

   // lookup the base residual based on settype and Rinseback selection
   _residualRBCVolume = ResidualAlgorithm::getBaseResidual(_recommendedCassette,
                                                           _config.getPlasmaRinseback(),
                                                           _config.getSalineRinseback());


   _primeDuration = (_procState.ActualPrimeDuration() != 0.0f) ? _procState.ActualPrimeDuration() : cobeConfig.PlateletPlasmaPrimeTime;
   _vinPrime      = cobeConfig.PlateletPlasmaPrimeVolume;

   _HypoMargin    = 0.0f; // IT9379, 11220 no longer use margins in predict
   _HypoMarginMin = 0.0f;



   for (int i = 0; i < ALLOCATED_SUBSTATES; i++)
   {
      memset(&(substate[i]), 0, sizeof(IntermediateResults));
      substate[i].Csf = cobeConfig.CsfMax;
      substate[i].Rpm = cobeConfig.RPMCollect;
   }

   // set the correct cobe config values based on concentration.
   setChamberFlowConstants();


   //
   //
   //   The following toplevel values are important since the closer we
   //   guess the better things converge.  Formulas are of course empirical;
   //   worse "guesses" lead to midrun flow/time anomalies.
   //
#ifdef __WIN32__
   if ( _precount <= 0 ) _precount = 200; // simulator fix
#endif
   _Tc               = 10.0f + 10.0f * _DesiredYield * (250.0f / _precount);
   _Vintarg          = _procState._predictCDS.VinTarget.Get();
   _Vintarg          = ( _Vintarg != 0.0f ) ? _Vintarg : 500.0f + 700.0f * _DesiredYield * (250.0f / _precount);
   _totalCollectTime = _Tc - _primeDuration;
   _QcmrTime         = _totalCollectTime;
   _QcmrVol          = _ProcFixedCollectVol;

   _Vincv            = cobeConfig.MinVinCollectStart;
   _VincvTime        = 5.0f;
   if (_procState.Vincv() != 0.0f)
   {
      _Vincv     = _procState.Vincv();
      _VincvTime = _procState.VincvTime();
   }


   //    If there has been a purge since our last optimization account for
   //    the yield decrement it represents
   UpdateYdpur();
}

void procedureDetailPltPlsRBC::UpdateYdpur ()
{
   int newPurges = _procState.Purges() - _procState.getAccumPurges();
   if (newPurges > 0)
   {
      float Ypurge = Ypur(_startingsubstate);  // Fch * (1.0f-Fw)/ Npsat;
      _procState.setAccumYdPur(_procState.getAccumYdPur() + (newPurges * Ypurge));
      _procState.setAccumPurges(_procState.Purges());

      float timeForThisPurge = 0.0f; // unscheduled purges do not accumulate procedure time
      if (_procState.WbcPurgeType() == WBCPURGE_SCHEDULED )
      {
         timeForThisPurge = _procState._predictCDS.TimeNextPurge.Get();
         _purgeQcmrLock   = _procState.getQcmr(); // IT13940 only lock Qcmr for scheduled purges
         DataLog(log_level_predict_info) << " Purge Qcmr locked at " << _purgeQcmrLock << endmsg;
      }
      _purgeEndTime = _procState.WbcPurgeStartTime() + timeForThisPurge;
      _procState._predictCDS.AccumulatedPurgeTime.Set(_procState._predictCDS.AccumulatedPurgeTime.Get() + timeForThisPurge);
      _procState._predictCDS.PurgeEndTime.Set(_purgeEndTime);

      DataLog(log_level_predict_info) << "Proc reported a purge: Ypur: " << Ypurge << " Ydpur: " <<
         _procState.getAccumYdPur() << " Purges done: " << _procState.getAccumPurges() <<
         " last scheduled purge was: " << _procState.WbcLastProcPurge() <<
         " current purge will start at " << _procState.WbcPurgeStartTime() << " last for " << timeForThisPurge <<
         " minutes and end at " << _purgeEndTime <<
         ".  Total accumulated purge time is " << _procState._predictCDS.AccumulatedPurgeTime.Get() <<
         endmsg;
   }
   _purgeEndTime = _procState._predictCDS.PurgeEndTime.Get(); // member variable for convenience so we don't access CDS each time
}

INTERNAL_PREDICT_RESULTS procedureDetailPltPlsRBC::PreCheckProcedure ()
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PreCheckProcedure()";
   //
   //
   //  If plasma rinseback is enabled but would exceed our allowable maximum
   //  that we can collect then disqualify.
   //
   if ( _config.getPlasmaRinseback()
        && (PlasmaHctOK())
        && _ProcCollectPlasma
        && _ProcPlasmaVolFixed  )
   {
      if ( (_ProcFixedPlasmaVol + cobeConfig.RinsebackVplrb) > _MaxPlasma)
      {
         return PLASMA_NOT_ACHIEVED;
      }
   }

   // chech if we should show RBC procedures base on CPS alarms
   // IT8160
   if (   !_procState.PtfCpsTestPassed()
          && _procState.FirstDrawComplete()
          && (getPredRBCDose() > 0.0f)
          && getMSSRbc()   )
   {
      return CPS_PRESSURE_PRECLUDES_RBCS;
   }

   return GOOD_PRODUCT;
}

INTERNAL_PREDICT_RESULTS procedureDetailPltPlsRBC::PostCheckProcedure ()
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PostCheckProcedure()";
   //
   //
   //   Check to see if the VBP has been exceeded.
   //
   if (!NoMorePlatelets() && (_ProcFixedCollectVol != 0.0f))
   {
      if (substate[Config::__PIR_NOPLASMA].EndVin > (_procState.Tbv() * 2.2f))
      {
         return VBP_EXCEEDED;
      }
      else if (substate[Config::__PIR_NOPLASMA].EndVin >= cobeConfig.VinMaxTarget)
      {
         return VIN_MAX_EXCEEDED;
      }
      else if (substate[Config::__PIR_NOPLASMA].EndVin <= cobeConfig.VinMinTarget)
      {
         return VIN_MIN_NOT_MET;
      }
   }

   //
   //
   //  If plasma rinseback selected, ensure we have collected the needed extra
   //  plasma (normally the logic does this automatically except in cases where
   //  we hit max bag volume)
   //
   if (_config.getPlasmaRinseback() && (PlasmaHctOK()))
   {
      if (_ProcCollectPlasma)
      {
         if (_ProcPlasmaVolFixed)   // need fixed + rinseback
         {
            if (fabs(_PredVp - _ProcFixedPlasmaVol - cobeConfig.RinsebackVplrb) > Config::__plasmaTolerance)
            {
               return PLASMA_NOT_ACHIEVED;
            }
         }
         else     // need AMAP min + rinseback
         {
            if (fabs(_PredVp - cobeConfig.RinsebackVplrb) < (_config.getAMAPVolume() - Config::__plasmaTolerance))
            {
               return PLASMA_NOT_ACHIEVED;
            }
         }
      }
      else     // need at least rinseback vol. IT 10187
      {
         if (_PredVp < cobeConfig.RinsebackVplrb - Config::__plasmaTolerance)
         {
            return PLASMA_NOT_ACHIEVED;
         }
      }
   }
   return GOOD_PRODUCT;
}

//
// -----------------------------------------------------------------------------
//
void procedureDetailPltPlsRBC::setHypovolemia ()
{
   float mindistance = substate[_startingsubstate].HypovolemiaDistance;
   for (int subst = _startingsubstate + 1; subst<_numOfSubstates; subst++)
   {
      if (substate[subst].HypovolemiaDistance < mindistance) mindistance = substate[subst].HypovolemiaDistance;
   }
   hypovolemiaDistance(mindistance);
   hypovolemic(mindistance < _HypoMargin);
}

//
// -----------------------------------------------------------------------------
//
int procedureDetailPltPlsRBC::RBCState () const
{
   return Config::__RBC;
}

int procedureDetailPltPlsRBC::RBCDoubleState () const
{
   return Config::__RBC;
}
//
//
// -----------------------------------------------------------------------------
//
bool procedureDetailPltPlsRBC::PlasmaHctOK () const
{
   if (_procState.InitialHct() > 0.5f) return false;
   return true;
}
//
//
// -----------------------------------------------------------------------------
//
void procedureDetailPltPlsRBC::do_SetTargets (substates_t& info)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::SetTargets()";

   //   Valve position table
   //
   static const HAL_VALVESTATE valvepos[Predict_CDS::NumOfSubstates][3] = {
      //  platelet             plasma             RBC
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 0 = nocollect
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 1 = channel setup
      { HAL_VALVE_RETURN,  HAL_VALVE_COLLECT, HAL_VALVE_RETURN }, // 2 = prepltpl
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 3 = preplt-nopl
      { HAL_VALVE_COLLECT, HAL_VALVE_COLLECT, HAL_VALVE_RETURN }, // 4 = plt+pl
      { HAL_VALVE_COLLECT, HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 5 = PCA
      { HAL_VALVE_COLLECT, HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 6 = midrun
      { HAL_VALVE_COLLECT, HAL_VALVE_COLLECT, HAL_VALVE_RETURN }, // 7 = PIR+Pl
      { HAL_VALVE_COLLECT, HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 8 = PIR nopl
      { HAL_VALVE_COLLECT, HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 9 = EPCA
      { HAL_VALVE_RETURN,  HAL_VALVE_COLLECT, HAL_VALVE_RETURN }, // 10 = EP
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN }, // 11 = RBC setup 1
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN},  // 12 = RBC setup 2
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_RETURN},  // 13 =  RBC Prime
      { HAL_VALVE_RETURN,  HAL_VALVE_RETURN,  HAL_VALVE_COLLECT}, // 14 = RBC
   };

   // NOTE: we should really lock _predictCDS to improve performance across all
   // the writes below... we don't, because I happen to know that it's already
   // been locked by the calling function (predict.cpp).  A debatable choice. --DGS

   // Purge, Pir stuff
   _procState._predictCDS.VinPurge.Set(_Vinpur);
   _procState._predictCDS.VinDeltaPurge.Set(_deltaPurge);
   _procState._predictCDS.VinNextPurge.Set(_nextProcPurge);
   _procState._predictCDS.TimeNextPurge.Set(_nextProcPurgeTime);
   _procState._predictCDS.VinPIRStart.Set(_VinPIR);
   _procState._predictCDS.MaxPIRQc.Set(getPIRLockin());
   _procState._predictCDS.Ydch.Set(YdchNoPIR());
   _procState._predictCDS.YdchPIR.Set(YdchPIR());

   // IT 8954
   _procState.setPIRLockin(getPIRLockin());
   _procState.setQcmr(_Qcmr);
   // Scaled PIR lockin - for collect.cpp
   _procState.setLastCalcQchMax(_CollectMaximumFlow);

   if (secondPartofPTFDouble())
      _procState._predictCDS.RBCVsvn.Set(substate[RBCDoubleState()].Vsvn);
   else
      _procState._predictCDS.RBCVsvn.Set(substate[RBCState()].Vsvn);

   // LastCsf
   if (substate[Config::__MIDRUN].T > 0.0f)
      _procState.setPirCsf(substate[Config::__MIDRUN].Csf);
   else if (substate[Config::__PLATELET_PLASMA].T > 0.0f)
      _procState.setPirCsf(substate[Config::__PLATELET_PLASMA].Csf);

   // Save off Midrun Qin for PIR
   if (_startingsubstate <= Config::__MIDRUN)
   {
      if (substate[Config::__MIDRUN].T > 0.0f)
         _procState.setPirQinLimit(substate[Config::__MIDRUN].Qininstd);
      else if (substate[Config::__PLATELET_PLASMA].T > 0.0f)
         _procState.setPirQinLimit(substate[Config::__PLATELET_PLASMA].Qininstd);

      _procState.setPrePirQch(_Qch);
   }

   // Save off Qch for Platelet Plasma - used to predict PCA.
   if (_startingsubstate < Config::__PCA)
      _procState.setPlateletPlasmaQch(substate[_startingsubstate].Qch);

   // For use in collect.cpp to limit Midrun return needle flow
   if (_startingsubstate < Config::__MIDRUN)
   {
      if (substate[Config::__PLATELET_PLASMA].T > 0.0)
      {
         _procState.setLimitingMidrunQrp(substate[Config::__PLATELET_PLASMA].Qnret + substate[Config::__MIDRUN].Qininstr);
      }
      else if (substate[Config::__PREPLATELET_PLASMA].T > 0.0)
      {
         _procState.setLimitingMidrunQrp(substate[Config::__PREPLATELET_PLASMA].Qnret + substate[Config::__MIDRUN].Qininstr);
      }
   }


   for (int substateindex = 0; substateindex < getNumOfSubstates(); substateindex++)
   {
      //   Set appropriate targets
      State_names procsubstate = SubstateMap::ss(substateindex);

      info[substateindex].T      = substate[substateindex].T;
      info[substateindex].EndVin = substate[substateindex].EndVin;
      info[substateindex].Csf    = substate[substateindex].Csf;
      info[substateindex].Rpm    = substate[substateindex].Rpm;

      //  Set valve positions
      info[substateindex].PlateletValve = valvepos[substateindex][0];
      info[substateindex].PlasmaValve   = valvepos[substateindex][1];
      info[substateindex].RBCValve      = valvepos[substateindex][2];

      switch (procsubstate)
      {
         case NOCOLLECT_STATE :
            info[substateindex].Vtarget       = 0.0f;
            info[substateindex].VtargetType   = VTarget::NOCOLLECT;
            info[substateindex].OrVtarget     = 0.0f;
            info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            break;
         //  These substates have volume based targets
         case SS_EXTENDED_PLASMA :
            info[substateindex].Vtarget       = _PredVp;
            info[substateindex].VtargetType   = VTarget::VPBAG;
            info[substateindex].OrVtarget     = 0.0f;
            info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            break;
         case SS_RBC :
            info[substateindex].Vtarget       = getPredRBCVolume();
            info[substateindex].VtargetType   = VTarget::VRBCBAG;
            info[substateindex].OrVtarget     = 0.0f;
            info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            break;

         //
         //
         //  These substates have Vin targets
         //
         case SS_CHANNEL_SETUP :
         case SS_PREPLATELET_NOPLASMA :
         case SS_PCA :
         case SS_MIDRUN :
         case SS_RBC_PTF_SETUP_1 :
         case SS_RBC_PTF_SETUP_2 :
         case SS_RBC_PTF_PRIME :
            if (substate[substateindex].T > 0.0f)
            {
               info[substateindex].Vtarget     = substate[substateindex].EndVin;
               info[substateindex].VtargetType = VTarget::VIN;
            }
            else
            {
               info[substateindex].Vtarget     = 0.0f;
               info[substateindex].VtargetType = VTarget::NOCOLLECT;
            }
            info[substateindex].OrVtarget     = 0.0f;
            info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            break;

         case SS_PIR_NOPLASMA :
            if ((substate[substateindex].T > 0.0f) && (!_procState.PirSpillover()))
            {
               info[substateindex].Vtarget     = substate[substateindex].EndVin;
               info[substateindex].VtargetType = VTarget::VIN;
            }
            else
            {
               info[substateindex].Vtarget     = 0.0f;
               info[substateindex].VtargetType = VTarget::NOCOLLECT;
            }
            info[substateindex].OrVtarget     = 0.0f;
            info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            break;

         //
         //
         //   Special case for extended PCA.
         //
         case SS_EXTENDED_PCA :


            if ( Software_CDS::isPPC(getProcPlateletProdCon() ) )
            {
               if (substate[substateindex].T > 0.0f)
               {
                  info[substateindex].Vtarget     = _ProcFixedCollectVol;
                  info[substateindex].VtargetType = VTarget::VCOLBAG;
               }
               else
               {
                  info[substateindex].Vtarget     = 0.0f;
                  info[substateindex].VtargetType = VTarget::NOCOLLECT;
               }
               info[substateindex].OrVtarget     = 0.0f;
               info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            }
            else
            {
               info[substateindex].Vtarget       = _PredVc; // can't use procCollectFixedVol to avoid EPCA overrun
               info[substateindex].VtargetType   = VTarget::VCOLBAG;
               info[substateindex].OrVtarget     = 0.0f;
               info[substateindex].OrVtargetType = VTarget::NOCOLLECT;

            }
            break;

         //
         //
         //  These substates have multiple targets.
         //
         case SS_PIR_PLASMA :
            if (substate[substateindex].T > 0.0f)
            {
               info[substateindex].Vtarget       = substate[substateindex].EndVin;
               info[substateindex].VtargetType   = VTarget::VIN;
               info[substateindex].OrVtarget     = _PredVp;
               info[substateindex].OrVtargetType = VTarget::VPBAG;

            }
            else
            {
               info[substateindex].Vtarget       = 0.0f;
               info[substateindex].VtargetType   = VTarget::NOCOLLECT;
               info[substateindex].OrVtarget     = 0.0f;
               info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            }
            break;

         //
         //
         //  Preplatelet+plasma runs to Vincv in most cases.  If we are within
         //  hypovolemia limits we run to predicted Vp to avoid short platelet+plasma
         //  substates as described in IT9547.  If we are limited by hypovolemia we
         //  need to run to the volume as predicted (IT9740).  We need at least as much
         //  margin as we would overcollect in a return cycle.
         //
         case SS_PREPLATELET_PLASMA :
            if (substate[substateindex].T > 0.0f)
            {
               info[substateindex].Vtarget     = _Vincv;
               info[substateindex].VtargetType = VTarget::VIN;

               float hypoMarginNeeded = substate[substateindex].Qpinstr * substate[substateindex].Tr + _HypoMargin;
               if (substate[substateindex].HypovolemiaDistance < hypoMarginNeeded)
               {
                  info[substateindex].OrVtarget     = substate[substateindex].AccumVp;
                  info[substateindex].OrVtargetType = VTarget::VPBAG;
               }
               else
               {
                  info[substateindex].OrVtarget     = _PredVp;
                  info[substateindex].OrVtargetType = VTarget::VPBAG;
               }

            }
            else
            {
               info[substateindex].Vtarget       = 0.0f;
               info[substateindex].VtargetType   = VTarget::NOCOLLECT;
               info[substateindex].OrVtarget     = 0.0f;
               info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            }
            break;

         //
         //
         //   We normally attempt to run the platelet+plasma substate
         //   to the plasma bag volume if possible (and always in case
         //   of rbc/pl procedures).  We run to Vin if the predicted
         //   volumes are not close to the substate volume targets.
         //
         //   Case 1: bounded by hypovolemia: run to predicted accumulated plasma volume
         //   Case 2: not bounded by hypovolemia but within 5ml of Vp: run to Vp target
         //   Case 3: not bounded by hypovolemia and not near Vp target: run to Vin
         //
         case SS_PLATELET_PLASMA :
            if (substate[substateindex].T > 0.0f)
            {
               float hypoMarginNeeded = substate[substateindex].Qpinstr * substate[substateindex].Tr + _HypoMargin;
               if (substate[substateindex].HypovolemiaDistance < hypoMarginNeeded) // case 1
               {
                  info[substateindex].Vtarget       = substate[substateindex].AccumVp;
                  info[substateindex].VtargetType   = VTarget::VPBAG;
                  info[substateindex].OrVtarget     = 0.0f;
                  info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
               }
               else if ((_DesiredYield == 0.0f) || (fabs(_PredVp - substate[substateindex].AccumVp) <= 5.0f))  // case 2
               {
                  info[substateindex].Vtarget       = _PredVp;
                  info[substateindex].VtargetType   = VTarget::VPBAG;
                  info[substateindex].OrVtarget     = 0.0f;
                  info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
               }
               else  // case 3
               {
                  info[substateindex].Vtarget       = substate[substateindex].EndVin;
                  info[substateindex].VtargetType   = VTarget::VIN;
                  info[substateindex].OrVtarget     = 0.0f;
                  info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
               }

            }
            else  // no time in substate
            {
               info[substateindex].Vtarget       = 0.0f;
               info[substateindex].VtargetType   = VTarget::NOCOLLECT;
               info[substateindex].OrVtarget     = 0.0f;
               info[substateindex].OrVtargetType = VTarget::NOCOLLECT;
            }
            break;

         default :
#ifndef __SKIPMSGSYS__
            anAlarmMsg badsubstate(PREDICT_BAD_TARGET_SUBSTATE);
            badsubstate.setAlarm();
#endif
            DataLog(log_level_predict_error) << "Bad substate " << (int)procsubstate << endmsg;
            break;

      }
      // Override targets if substate has been completed
      if (procsubstate < _startingsubstate)
      {
         info[substateindex].Vtarget     = 0.0f;
         info[substateindex].VtargetType = VTarget::NOCOLLECT;
      }
   }

}


//
//
// -----------------------------------------------------------------------------
//
float procedureDetailPltPlsRBC::instantaneousInfusion (int substateindex) const
{
   const IntermediateResults& info = substate[substateindex];
   if (info.Tcn == 0.0f) return 0.0f; // substate is unused

   float dcf         = (1.0f - cobeConfig.RecirculateSnDrawVol / info.Vsvn);
   float ACinDraw    = info.Qacinst * info.Td * dcf; // this one is still OK since AC is not added to the draw cycle for first 3ml
   float TBV         = _procState.Tbv() / 1000.0f;

   float ACinProduct = 0.0f;
   switch (SubstateMap::ss(substateindex))
   {
      default : // all nocollection states
         break;

      case SS_PREPLATELET_PLASMA :
      case SS_EXTENDED_PLASMA :
         ACinProduct = info.Qpinstd * info.Tcn * info.Facb;
         break;

      case SS_PCA :
      case SS_MIDRUN :
      case SS_PIR_NOPLASMA :
      case SS_EXTENDED_PCA :
         ACinProduct = info.Qcinstd * info.Tcn * info.Facb;
         break;

      case SS_RBC :
         ACinProduct = (info.Qininstd - info.Qcinstd) * info.Td * info.Facb * (1.0f - _PredRBCCrit / 100.0f);
         break;

      case SS_PLATELET_PLASMA :
      case SS_PIR_PLASMA :
         ACinProduct = (info.Qpinstd + info.Qcinstd) * info.Tcn * info.Facb;
         break;
   }

   float IR = (ACinDraw - ACinProduct) / info.Tcn / TBV;

   return IR;
}
//
//
// -----------------------------------------------------------------------------
//
int procedureDetailPltPlsRBC::AlgSubstate (const State_names procsubstate) const
{
   //
   //
   //  Special case (IT5691): if we are switching to a RBC/Pl procedure from
   //  a platelet driven procedure ensure that we do not include any platelet
   //  substates.
   //
   int algsubstate = SubstateMap::alg(procsubstate, 0);
   if (algsubstate > 0 && (_DesiredYield == 0.0f) &&
       (algsubstate < Config::__EXTENDED_PLASMA)) // IT6450
      algsubstate = Config::__EXTENDED_PLASMA;

   return algsubstate;
}
//
//
// -----------------------------------------------------------------------------
//
int procedureDetailPltPlsRBC::NoMorePlatelets () const
{
   return ((AlgSubstate(_procState.SubState()) > Config::__PIR_NOPLASMA) ||
           (_DesiredYield == 0.0f));
}


//
//
// -----------------------------------------------------------------------------
//
float procedureDetailPltPlsRBC::VinTarget () const
{
   return substate[Config::__PIR_NOPLASMA].EndVin;
}

float procedureDetailPltPlsRBC::VinPIRStart () const
{
   return substate[Config::__MIDRUN].EndVin;
}

float procedureDetailPltPlsRBC::YdchNoPIR () const
{
   return Ydch(_Vintarg, 0);
}

float procedureDetailPltPlsRBC::YdchPIR () const
{
   return Ydch(_Vintarg, 1);
}

//
// =================================================================================
// =================================================================================
// =================================================================================
void procedureDetailPltPlsRBC::generatesubstates ()
{

   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::generatesubstates()"<< endmsg;

   setconvergenceCtr(_numOfTimes < 4); // minimum iterations allowed
   _numOfTimes++;
   Calculate_Flows::_numCalculations = 0;
   // DataLog(log_level_predict_debug) << " generatesubstates()   _numOfTimes=" << _numOfTimes << endmsg;

   int   subst;
   float _prevCsf = 0.0f;
   for (subst = 1; subst < _numOfSubstates; subst++)
   {
      _prevCsf            = ( substate[subst].Csf == 0.0f ) ? cobeConfig.CsfMax : substate[subst].Csf;
      memset(&(substate[subst]), 0, sizeof(IntermediateResults));
      substate[subst].Csf = _prevCsf;
      substate[subst].Rpm = sqrt(substate[subst].Csf) * cobeConfig.RPMCollect;
   }

   //
   //  Our starting conditions
   //
   _currentvp     = _procState.PlasmaVolume();
   _currentacinvp = _procState.AcInPlasma();
   _currentvc     = _procState.CollectVolume();
   _currentacinvc = _procState.AcInCollect();
   _currenttime   = _procState.ProcRunTime();
   _currentQch    = _procState.Qch();
   _Yield         = _procState.Yield();


   //
   //
   //    Calculate generic parameters (from V5 "general" and "toplevel" spreadsheet sections)
   //
   Toplevel();

   //
   //
   //     Accumulate previous substate end points, if any
   //
   AccumulateVolumes(); // to set previous substate end points, if any



   //
   //   PCA, EPCA, Midrun and RBC (setup and collect).
   //
   PrecalculateFlows();

   //
   //  Jump into the procedure at the appropriate current point.
   //
   for (subst = _startingsubstate; subst < _numOfSubstates; subst++)
   {
      IntermediateResults& currentsubstate  = substate[subst];
      IntermediateResults& previoussubstate = substate[subst - 1]; // note use of [0] to avoid [1] blowup
      currentsubstate.EndVin = previoussubstate.EndVin;            // in case no time in substate

      switch (subst)
      {
         case 1 :  ChannelSetup(currentsubstate, previoussubstate)        ; break;
         case 2 :  PreplateletPlasma(currentsubstate, previoussubstate)   ; break;
         case 3 :  PreplateletNoplasma(currentsubstate, previoussubstate) ; break;
         case 4 :  PlateletPlasma(currentsubstate, previoussubstate)      ; break;
         case 5 :  PCA(currentsubstate, previoussubstate)                 ; break;
         case 6 :  Midrun(currentsubstate, previoussubstate)              ; break;
         case 7 :  PIRPlasma(currentsubstate, previoussubstate)           ; break;
         case 8 :  PIRNoplasma(currentsubstate, previoussubstate)         ; break;
         case 9 :  EPCA(currentsubstate, previoussubstate)                ; break;
         case 10 : ExtendedPlasma(currentsubstate, previoussubstate)      ; break;
         case 11 : RBCSetup1(currentsubstate, previoussubstate)           ; break;
         case 12 : RBCSetup2(currentsubstate, previoussubstate)           ; break;
         case 13 : RBCPrime(currentsubstate, previoussubstate)            ; break;
         case 14 : RBC(currentsubstate, previoussubstate)                 ; break;
      }

      AccumulateVolumes();
      currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                                 _procState.VAccumOverdraw(),
                                                                                                 currentsubstate.AccumVc, currentsubstate.AccumVacc,
                                                                                                 currentsubstate.AccumVp, currentsubstate.AccumVacp,
                                                                                                 currentsubstate.Vr, currentsubstate.Vacr,
                                                                                                 0.0f, currentsubstate.AccumVac);
   }
   setHypovolemia();


   //
   //   Calculate collection time for next pass.  Note that the usage of individual
   //   substate times will give us "time remaining in collection" in case of midrun
   //   reoptimizations.
   //
   _Tc = substate[Config::__PLATELET_PLASMA].T + substate[Config::__PCA].T
         + substate[Config::__MIDRUN].T + substate[Config::__PIR_PLASMA].T
         + substate[Config::__PIR_NOPLASMA].T;
   _totalCollectTime = substate[Config::__PIR_NOPLASMA].AccumTime - _VincvTime;

   //
   //
   //   Remove purge time from any total collect time so far.  Purge time is not real "collect"
   //   time and should not factor into Qinc.  If we are in the middle of a purge then the
   //   accumulated time already accounts for the full purge.
   //
   _totalCollectTime -= _procState._predictCDS.AccumulatedPurgeTime.Get(); // this includes current purge if in a purge now
   if (_procState.ProcRunTime() < _purgeEndTime)                           // in a purge now, add back in the time not used so far
   {
      float timeRemainingThisPurge = _purgeEndTime - _procState.ProcRunTime();
      _totalCollectTime += timeRemainingThisPurge;
   }


   if (_startingsubstate < Config::__PIR_PLASMA)
   {
      if (substate[Config::__PLATELET_PLASMA].T + substate[Config::__MIDRUN].T > 0.0f)
         _QcmrTime = substate[Config::__PLATELET_PLASMA].T + substate[Config::__MIDRUN].T;    // _totalCollectTime - (substate[Config::__PIR_PLASMA].T + substate[Config::__PIR_NOPLASMA].T);
      if (substate[Config::__PLATELET_PLASMA].Vc + substate[Config::__MIDRUN].Vc > 0)
         _QcmrVol =    // _PredVc - (substate[Config::__PIR_PLASMA].Vc + substate[Config::__PIR_NOPLASMA].Vc);
                    substate[Config::__PLATELET_PLASMA].Vc + substate[Config::__MIDRUN].Vc;
   }
   _Vintarg = substate[Config::__PIR_NOPLASMA].EndVin;
   _EPCAVc  = substate[Config::__EXTENDED_PCA].Vc;
   _PCAVc   = substate[Config::__PCA].Vc;

   //   Calculate Qinr for next pass
   if (substate[Config::__PREPLATELET_PLASMA].EndVin > _Vincv)
   {
      _Qinr = substate[Config::__PREPLATELET_PLASMA].Qininstd;
   }
   else if (substate[Config::__PREPLATELET_NOPLASMA].EndVin > _Vincv)
   {
      _Qinr = substate[Config::__PREPLATELET_NOPLASMA].Qininstd;
   }
   else if (substate[Config::__PLATELET_PLASMA].EndVin > _Vincv)
   {
      _Qinr        = substate[Config::__PLATELET_PLASMA].Qininstd;
      _QinrQnLimit = (_prePltPlsEndOfRamp.Qininstd == 0.0f) ? substate[Config::__PREPLATELET_PLASMA].Qininstd
                     : _prePltPlsEndOfRamp.Qininstd;
   }
   else
   {
      _Qinr        = substate[Config::__MIDRUN].Qininstd;
      _QinrQnLimit = (_prePltNoPlsEndOfRamp.Qininstd == 0.0f) ? substate[Config::__MIDRUN].Qininstd
                     : _prePltNoPlsEndOfRamp.Qininstd;
   }

   //	Vincv for next pass
   CalculateVincv();

   // _Ypur, _Ydch, _Vinpur, _deltaPurge and if PIR is required
   // IT13946: Don't exit if the number of purges is changing
   int lastpurges = _purges;
   CalculatePurgePoints();
   if ((_numOfTimes < (MAXITERATIONS - 1)) && (_purges != lastpurges)) setconvergenceCtr(9);

   //   Save the average PIR Qin if needed
   if (_pirAlgo->getPirRequired())
   {
      IntermediateResults* PIRstate = ((substate[Config::__PIR_NOPLASMA].T > 0.0f) ?
                                       &substate[Config::__PIR_NOPLASMA]
                                       : &substate[Config::__PIR_PLASMA]);
      if (PIRstate->Vsvn > 0.0f)
      {
         _PIRAvgQin = PIRstate->Qininstd * PIRstate->D;
      }

      //
      //
      //   IT7401 keep trying if PIR volume at max flow is not optimal.  Note that once we actually
      //   hit max collect speed in PIR this test will always fail, since we do not keep track of
      //   "amount of Vc already accrued at max during PIR".  This has the side effect that we may
      //   get a few extra iterations before convergence near the end of the procedure.
      //
      float pirvcerr = fabs(_PIRVcAtMax - cobeConfig.PIRVcAtMax);
      if ((_numOfTimes < 12 ) && (pirvcerr > cobeConfig.PIRAllowableVcError)) setconvergenceCtr(8);
   }

   //  for AMAP+RBC collections, ensure that we have collected the proper amount
   //  of plasma.
   if ((getPredRBCDose() > 0.0f) && _ProcCollectPlasma && (!_PlasmaTargetCannotChange))
   {
      if (hypovolemic() && (substate[Config::__EXTENDED_PLASMA].Vp > 0.0f) &&
          (_PredVp > _Vpmin + 0.1f))   // too much collected
      {
         if (_PredVp > (_procState.PlasmaVolume() + 0.1f))   // and it's not already been collected (we can't change that)
         {
            if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(2);
         }
      }
   }

   //  Don't allow exit while concentration is still off.


   float pltVolErr = (float)fabs(_PredVc - _ProcFixedCollectVol);
   float pltErr    = pltVolErr / _ProcFixedCollectVol;
   if (Software_CDS::isPPC(getProcPlateletProdCon() ))
   {
      // See IT 9648
      if (_Tc > 2.0f && _currentvc < _ProcFixedCollectVol)
      {
         if (_procState.getPredictRequest() & REPREDICT_ONLY)
         {
            if (pltErr > cobeConfig.CollectVolErr)
            {
               if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(3);
            }
         }
      }
   }

   if ((_numOfTimes < 12) && (pltErr > 0.01f)) setconvergenceCtr(3);
   if ((_numOfTimes < 12) && (pltVolErr > 10.0f)) setconvergenceCtr(3);
   if ((_numOfTimes < 16) && (pltErr > 0.02f)) setconvergenceCtr(3);






   //  Disallow exit if we have collected too much AMAP or MC plasma
   if (_ProcCollectPlasma && (!_PlasmaTargetCannotChange))
   {
      if (hypovolemic() && (_PredVp > (_Vpmin + 0.1f)))   // we have collected too much plasma
      {
         if (_PredVp > (_procState.PlasmaVolume() + 0.1f))   // and it's not already been collected (we can't change that)
         {
            if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(4);
         }
      }
   }

   //  If we are hypovolemic because we have collected too much Vc force reiteration
   _VcOvercollect = _PredVc - _ProcFixedCollectVol;
   if (hypovolemic() && (_PredVc > (_ProcFixedCollectVol + 1.0f)) && (_ProcFixedCollectVol > 0.0f) &&
       ((_hypovolemiaDistance + _PredVc - _ProcFixedCollectVol) > _HypoMargin))
   {
      if (!_ProcCollectPlasma || (_PredVp + _PredVc - _ProcFixedCollectVol > _Vpmin))
      {
         if (_Qcmr > cobeConfig.CollectMinimumFlow)   // we can still collect less
         {
            if (_hypovolemiaDistance < _HypoMargin )
            {
               if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(5); // 13196 choose hypovolemia status over unconverged
            }
         }
      }
   }


   //
   //
   //  The combination of "midrun" and "EPCA" is nonoptimal
   //
   if ((_numOfTimes < cobeConfig.NumIterationsMidrunEpca ) && (substate[Config::__MIDRUN].T > 0.0f) &&
       (substate[Config::__EXTENDED_PCA].T > 0.0f)) setconvergenceCtr(6);

   //
   //
   //  Recalculate PIR lockin bracket and set convergence criteria if needed
   //
   int lockinCtr = _pirAlgo->getConvCriteria(_Qch);
   if ((lockinCtr != 0) && (_numOfTimes < 16)) setconvergenceCtr(lockinCtr);


   // a trial that looks like 3 - 6 or 6 - 3 means that we are changing Qcmr is overshooting
   // the ideal value. Tweak the convergence coeff. accordingly.
   if (_numOfTimes >= 2 && (_convergenceCtr == 3 || _convergenceCtr == 6))
   {
      short prevCtr = _convergenceTrail[_numOfTimes - 2];
      if ((_convergenceCtr == 3 && prevCtr == 6)
          || (_convergenceCtr == 6 && prevCtr == 3))
      {
         _QcmrConvCoeff /= 2;
         if (_QcmrConvCoeff < 0.1f)
            _QcmrConvCoeff = 0.1f;

      }
   }

}

float procedureDetailPltPlsRBC::MovePurgePoint (float vinPur)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::MovePurgePoint()" << endmsg;
   if ( (_numOfTimes > 4) && (vinPur < _Vintarg) )
   {

      float purgeDistance = cobeConfig.MinPurgeDistanceFromMidrun;

      if (_procState.getPredictRequest() & REPREDICT_ONLY)
      {
         if (_purgeSkipped)
         {
            purgeDistance += 225.0f;
         }
         else
         {
            purgeDistance -= 225.0f;
         }
      }

      float d1              = (_VinPIR != 0.0f) ? (_VinPIR - vinPur) : (_Vintarg - vinPur);
      float d2              = (_VinPIR != 0.0f) ?  (_Vintarg - vinPur) : cobeConfig.MinPurgeDistanceFromPir;
      bool  midrunCondition = false;
      if (d1 < 0.0f)
      {
         if (vinPur > _Vintarg)
         {
            midrunCondition = false;
         }
         else
         {
            midrunCondition = true;
         }
      }
      else
      {
         if (d1 < purgeDistance)
         {
            midrunCondition = true;
         }
         else
         {
            midrunCondition = false;
         }
      }
      bool pirCondition = ( (_VinPIR == 0.0f) || (d2 > 0.0f) &&
                            (d2 < cobeConfig.MinPurgeDistanceFromPir) ? true : false);
      if ( midrunCondition )
      {
         if ( pirCondition )
         {
            vinPur        = _Vintarg + cobeConfig.MinPurgeDistanceFromMidrun;
            _purgeSkipped = true;
         }
         else
         {
            vinPur        = _VinPIR - cobeConfig.MinPurgeDistanceFromMidrun;
            _purgeSkipped = false;

         }
      }
      else
         _purgeSkipped = false;
   }
   return vinPur;
}

// ========================================================================================
//   "toplevel" calculations (V5 cpp and general.cpp equivalents)
//
void procedureDetailPltPlsRBC::Toplevel ()
{

   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::Toplevel()";

   const float maxqinc = cobeConfig.QinLimitMax * cobeConfig.DutyCycleLimit;

   //    Maximum inlet flow
   float qininstdmax = cobeConfig.TbvQinMaxCoefficient * _config.getTbvQinmaxScaleConst() * _TBV;
   qininstdmax = MIN(qininstdmax, _config.getMaxInstQin());
   this->_procState.setQinistdMax(qininstdmax);


   //   Maximum needle flow
   _Qnmax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * _TBV;
   _Qnmax = MIN(_Qnmax, _config.getMaxNeedleRetFlow());

   //   Infusion
   _I = 0.93f * _config.getReferenceACRate() / (1.0f - exp((double)(-_PredTp / 37.0f)));
   _I = MIN(_I, getMaxACRate());

   //   Plasma related
   //   Calculate minimum allowable plasma volume _Vpmin and desired
   //   maximum volume _Vptarg
   _Vptarg = getProcFixedPlasmaVol();

   if (getProcCollectPlasma())
   {
      if (!getPlasmaTargetCannotChange())    // AMAP
      {
         _Vptarg = getMaxPlasma();
      }
      else if (getProcPlasmaProductVol() > 0.0f)     // product definition, replace with equivalent including AC
      {
         _Vptarg = getProcPlasmaProductVol();
      }
   }

   if (_Vptarg < 0.0f)
      _Vptarg = 0.0f;

   if (getProcCollectPlasma())
   {
      if (getProcPlasmaVolFixed())
      {
         _Vpmin = _Vptarg;
      }
      else
      {
         _Vpmin = (float)_config.getAMAPVolume();
      }
   }
   else
   {
      _Vpmin = 0.0f; // no plasma collection specified
   }

   //   If plasma rinseback is selected add additional needed plasma
   //   into both min and target values
   if (_config.getPlasmaRinseback() && (PlasmaHctOK()))
   {
      float maxpl = _procState._cassette.maxPlasma(_recommendedCassette,
                                                   _ProcFixedCollectVol,
                                                   _ProcRBCDose);
      _Vpmin += cobeConfig.RinsebackVplrb;
      if (_Vpmin > maxpl) _Vpmin = maxpl;
      //  The current procedure may already have the rinseback plasma
      //  added in
      if (!_PlasmaTargetCannotChange) _Vptarg += cobeConfig.RinsebackVplrb;
      if (_Vptarg > maxpl) _Vptarg = maxpl;
      if (_Vptarg < _Vpmin) _Vptarg = _Vpmin; // for no configured plasma cases
   }

   //	Platelet Related
   CalculateQcmr();

   //	Set Qch (generic across substates)
   if (InPir())
      _Qch = _currentQch;
   else
      _Qch = MIN(_Qcmr, getPIRLockin());

   //   Calculate _Qinc (average Qin during collection)
   if (_Qinc == 0.0f) _Qinc = 70.0f;  // defensive coding
   if (_totalCollectTime > 0.0f)      // divide by zero protection
   {
      _Qinc = (_Vintarg - _Vincv) / _totalCollectTime;
   }
   _Qinc = MIN(_Qinc, maxqinc);

   //
   //   PIR related
   _PIRVcAtMax = 0.0f;

   //
   //
   //  Calculate nocollect flows (used by purge calculations).  Nocollect flows require infusion rate
   //  and purge time calculations require the nocollect flows.  Since time is changing each iteration
   //  (and therefore infusion) we recalcuate these each pass.
   //
   CalculateNoCollectFlows();
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::CalculateQcmr ()
{
   _prevQcmr = _Qcmr;

   //  Save off Csf for convenience.  Note we should always have midrun CSF
   //  since those flows are precalculated regardless of current substate.
   //  Logical test for redundancy only.
   float midrunCsf = (substate[Config::__MIDRUN].Csf > 0.0f) ? substate[Config::__MIDRUN].Csf : 1.0f;

   float maxQch    = midrunCsf * _CollectMaximumFlow;

   if ((_DesiredYield > 0.0f) && (!NoMorePlatelets()))
   {
      float qcmrdelta = 0.0f;
      //
      //
      // --------------- _Qcmr determination -----------------------
      //
      //   The following code determines the value of _Qcmr for the next
      //   optimization pass.  _Qcmr has wide-ranging impacts on procedure
      //   time, _Ydch, etc. so it is important to have the correct value.
      //   The below code was empirically developed to guess/converge
      //   upon the best value for _Qcmr.  Use extreme caution when modifying
      //   the below code!  Side effects include nonoptimal procedures,
      //   convergence issues, and mid-procedure flow and time anomalies.
      //

      int holditerations = Software_CDS::isPPC(getProcPlateletProdCon()) ? cobeConfig.NumIterationsQcmrHoldPpc : cobeConfig.NumIterationsQcmrHoldStd;
      if (_numOfTimes <= holditerations)
      {
         _latchQcmr = false;
         if (_procState.getQcmr() != 0.0f && _procState.getOptimizingSelectedProc())
            _Qcmr = _procState.getQcmr();
         else
            _Qcmr = _ProcFixedCollectVol / _totalCollectTime;
      }

      //
      //
      //   Qcmr "deadband" implementation
      //
      else if ((_deadbandBeingEnforced) &&      // this is our first set of iterations
               (!_procState.getPrePirRamp()) && // Qc is not being otherwise controlled
               (_Qcmr <= maxQch))               // deadband applies to all chamber flow limited conditions per systems team
      {
         _Qcmr = _procState.getQcmr();
         if (_numOfTimes >= cobeConfig.NumIterationsDeadband)  // done, check volume discrepancy
         {
            _deadbandBeingEnforced = false;
            float pltErr = (float)fabs(_PredVc - _ProcFixedCollectVol) / _ProcFixedCollectVol;
            if (pltErr >= cobeConfig.CollectVolErr) // outside of tolerance, start over
            {
               float scaledFloor = cobeConfig.QchConvergenceFloor * midrunCsf;
               _QchConvergenceFloor = (scaledFloor < _procState.getQcmr()) ? scaledFloor : _procState.getQcmr();
               DataLog(log_level_predict_info) << "Qcmr deadband out of tolerance, Vc=" << _PredVc
                                               << " now enforcing Qch floor of " << _QchConvergenceFloor << endmsg;
               _numOfTimes = holditerations;
            } // else the lack of convergence is due to something else, let nature take its course.
         }
         else if (_numOfTimes == (holditerations + 1))  // first pass through logging
         {
            DataLog(log_level_predict_info) << "Qcmr deadband being enforced at " << _Qcmr << endmsg;
         }
      }
      //
      //
      //   Normal non-deadband flow
      //
      else
      {
         float remainingTime = MAX(_Tc, 10.0f);    // ten minutes chosen for divide by zero or negative delta protection
                                                   // empirical testing showed that 5 minutes caused Qcmr instability near the end of the run
         qcmrdelta = ((_ProcFixedCollectVol - _PredVc + _EPCAVc) / remainingTime) * _QcmrConvCoeff;

         //
         //
         //   IT13552: if we are in a case where we have already collected more volume than has been requested then
         //   there is no good solution.  Allow the procedure to run to completion by keeping Qch unchanged.
         //   We limit chamber flow to a safe minimum  (the lowest PIR bracket) to avoid excessive time changes
         //   and chamber flow "tanking" in case we have already diverged.
         //
         if (_currentvc > _ProcFixedCollectVol)
         {
            qcmrdelta = 0.0f;
            _Qcmr     = _currentQch;
            if (_Qcmr < cobeConfig.FirstPIRFlowLimitStandard) _Qcmr = cobeConfig.FirstPIRFlowLimitStandard;
         }

         //
         //
         //     Qcmr is allowed to change per iteration for non-current procedures but can
         //     only change from current Qcmr for the current procedure. ("chamber tanking" fix)
         //
         float vcdelta = (float)fabs(_PredVc - _EPCAVc - _ProcFixedCollectVol);
         float qcmrmax = _Qcmr * (1.0f + vcdelta / _ProcFixedCollectVol);
         float qcmrmin = _Qcmr * (1.0f - vcdelta / _ProcFixedCollectVol);
         if (_procState.getQcmr() != 0.0f && _procState.getOptimizingSelectedProc() &&
             (_procState.getPredictRequest() & REPREDICT_ONLY))
         {
            qcmrmax = _procState.getQcmr() + cobeConfig.QchIterationChange;
            qcmrmin = _procState.getQcmr() - cobeConfig.QchIterationChange;

            //
            //
            //   Chamber tanking fix (IT13668):  If we are moving in the wrong direction
            //   by decreasing chamber flow then hold current minimum.  Chamber tanking occurs late in a
            //   procedure when Qcmr is decreased in an attempt to meet concentration.  At lower flows
            //   this can actually have the opposite effect as lowering the chamber flow increases the
            //   yield decrement and increases Vin, thus increasing collect volume.
            //
            //   Note for V6.4 this is logging only to determine if this is effective.  Chamber flow
            //   tanking is also mitigated by the Qch floor below.
            //
            if (((_Qcmr < _procState.getQcmr()) && // qcmr is decreasing
                 (_Qcmr < maxQch) &&               // and we are in the range where tanking occurs
                 (_PredVc > _lastPredVc))          // but volume is increasing
                || _latchQcmr)                     // latch this condition once detected
            {
               // qcmrmin = _procState.getQcmr(); // this is the real fix.  Commented out in 6.4.  Logging only.
               if (!_latchQcmr)
                  DataLog(log_level_predict_debug) << "Proc " << getProcedureNumber() << " chamber flow tanking detected at "
                                                   << _Qcmr << " lastVc:" << _lastPredVc << " Vc:" << _PredVc << endmsg;
               _latchQcmr = true;
            }
         }

         //
         //
         //   Limit min value.  Do not let Qcmr drop below minimum CC value.
         //
         if (_QchConvergenceFloor > 0.0f)
         {
            if (qcmrmin < _QchConvergenceFloor) qcmrmin = _QchConvergenceFloor;
         }

         if (cobeConfig.QchDropSwitch != 0)
         {
            float minlimit = Software_CDS::isPPC(getProcPlateletProdCon()) ? cobeConfig.QchDropLimitPPC : cobeConfig.QchDropLimitStd;
            if (qcmrmin < minlimit) qcmrmin = minlimit;
         }

         //
         //
         //  Cusp case fix: if we are decreasing _Qcmr but will disqualify PCA,
         //  limit _Qcmr to where PCA is still viable.
         //
         if ((_Qcmr >= maxQch) &&
             (_Qcmr + qcmrdelta < maxQch) &&
             (_PCAVc > 10.0f))
         {
            _Qcmr = maxQch;
         }
         else
         {
            _Qcmr += qcmrdelta;
         }
         if (_Qcmr > qcmrmax) _Qcmr = qcmrmax;
         if (_Qcmr < qcmrmin) _Qcmr = qcmrmin;
      }
      _lastPredVc = _PredVc;

      //
      //
      //  In PIR substates Qch is dictated to us as the current chamber flow
      //
      if ((_startingsubstate == Config::__PIR_NOPLASMA) ||
          (_startingsubstate == Config::__PIR_PLASMA))
      {
         _Qcmr = _procState.getQcmr();
      }

      //
      //
      //   Qcmr is locked during purge
      //
      if (_purgeQcmrLock > 0.0f)
      {
         if ((_procState.ProcRunTime() >= _purgeEndTime) && (_procState.Recovery() == RecoveryTypes::None))
         {
            _purgeQcmrLock = 0.0f;
            DataLog(log_level_predict_info) << "Purge Qcmr lock released." << endmsg;
         }
         else // still in purge
         {
            _Qcmr = _purgeQcmrLock;
         }
      }

      const float maxqinc = cobeConfig.QinLimitMax * cobeConfig.DutyCycleLimit;
      if (_Qcmr > maxqinc) _Qcmr = maxqinc;
      if (_Qcmr < cobeConfig.CollectMinimumFlow) _Qcmr = cobeConfig.CollectMinimumFlow;

      //
      //   If we are in the deadband where Qp can't run decrease/increase _Qcmr to match
      if ((_Qcmr > maxQch) &&
          (_Qcmr < (maxQch + cobeConfig.CollectMinimumFlow)))
      {
         if (qcmrdelta > 0.0f)
         {
            _Qcmr = maxQch + cobeConfig.CollectMinimumFlow;
         }
         else
            _Qcmr = maxQch;

      }
      // float debug = 0.0f;
      // if (debug != 0.0f) _Qcmr = debug;
   }
   else
   {   // no platelet collection remaining
      _Tc   = 0.0f;
      _Qcmr = _CollectMaximumFlow;
   }

   //   Re-restrict _Qcmr in cases where the PIR bracket lockin forces
   //   the midrun flow.
   /*if (_Qcmr < _CollectMaximumFlow) {
   if (_Qcmr > _PIRLockin) _Qcmr = _PIRLockin;
   } else if (_PIRLockin < _CollectMaximumFlow) {
       _Qcmr = _PIRLockin;
   }*/
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::ChannelSetup (IntermediateResults& currentsubstate, IntermediateResults&)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::ChannelSetup()";

   Calc_Channel_Setup instflows;
   float              QinLimit, QnLimit;
   initialFlowRamp(cobeConfig.PlateletPlasmaPrimeVolume, QinLimit, QnLimit);
   float              Qch = MIN(_CollectMaximumFlow, _Qcmr);
   instflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow() ) ;
   instflows.Initialize(_hct, _precount, _TBV, _I, _ratio, QinLimit, cobeConfig.MaxInstantQreturn, QnLimit, Qch);
   instflows.calculate(&currentsubstate);

   currentsubstate.T      = (cobeConfig.VinPlasmaStart - cobeConfig.PlateletPlasmaPrimeVolume) / currentsubstate.Qininstd; // assumed no return cycle in this substate
   currentsubstate.EndVin = cobeConfig.VinPlasmaStart;
   currentsubstate.Vac    = (currentsubstate.EndVin - cobeConfig.PlateletPlasmaPrimeVolume) / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Csf    = cobeConfig.CsfMax; // No speed factor scaling for this substate
   currentsubstate.Rpm    = cobeConfig.RPMCollect;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PreplateletPlasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PreplateletPlasma()";
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;
   if (_Vpmin == 0.0f) return;

   Calc_Preplatelet_Plasma plasmaflows;
   IntermediateResults     plsubstate;
   memset(&plsubstate, 0, sizeof(IntermediateResults));

   float maxqin = this->_procState.getQinistdMax();
   if (_Qch < cobeConfig.MaxQchForQinDuringPC)
   {
      if (maxqin > cobeConfig.MaxQinPC) maxqin = cobeConfig.MaxQinPC;
   }
   else
   {
      if (maxqin > cobeConfig.MaxQinPlasmaCollect) maxqin = cobeConfig.MaxQinPlasmaCollect;
   }
   // Ending Qch
   float EndCsf          = MAX(MIN(_Qinr * cobeConfig.InletFlowRampConstant / cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   float Qch             = MIN(_Qcmr, EndCsf * _pirAlgo->getCollectMaxFlow() );

   int   firstpass       = 1;
   bool  plasmaSatisfied = false;
   if (getProcPlasmaProductVol() > 0.0f)   // we are running to product volume not including AC
   {
      plasmaSatisfied = ((_currentvp - _currentacinvp) > _Vptarg);
   }
   else
   {
      plasmaSatisfied = (_currentvp >= _Vptarg);
   }
   while (!plasmaSatisfied)
   {

      //
      //
      //   Calculate flows for this draw cycle
      //
      float QinLimit, QnLimit;
      initialFlowRamp(currentsubstate.EndVin, QinLimit, QnLimit);
      if (QinLimit > maxqin) QinLimit = maxqin;

      plasmaflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow() ) ;
      plasmaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, QinLimit, cobeConfig.MaxInstantQreturn, QnLimit, Qch);
      plasmaflows.calculate(&plsubstate);

      //
      //
      //  First time through - these are the commanded flows for the moment.  Save
      //  these in the official repository.
      //
      if (firstpass)
      {
         firstpass              = 0;
         currentsubstate        = plsubstate;
         currentsubstate.EndVin = previoussubstate.EndVin; // since we just overwrote it
      }

      //
      //
      //  Calculate plasma collected during this cycle and apply to our target
      //
      const float vp        = plsubstate.Qpinstd * plsubstate.Tcn;
      const float vindelta  = plsubstate.Qininstd * plsubstate.Td;

      float       pctneeded = limitState(currentsubstate, previoussubstate, vp, 0.0f, vindelta, _Vincv);

      //
      //
      //    Add collected plasma to totals
      //
      if (pctneeded < 1.0f)
      {
         plasmaSatisfied = 1;
         if (pctneeded < 0.0f) pctneeded = 0.0f;
      }
      currentsubstate.T      += plsubstate.Tcn * pctneeded;
      currentsubstate.Vp     += vp * pctneeded;
      currentsubstate.Vacp   += vp * pctneeded * plsubstate.Facb;
      currentsubstate.Vac    += vindelta * pctneeded / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / plsubstate.Vsvn);
      currentsubstate.EndVin += vindelta * pctneeded;
   }

   currentsubstate.Qch = currentsubstate.Qcinstd;
   // plsubstate.Qininstd = _Qinr * cobeConfig.InletFlowRampConstant;
   // currentsubstate.Csf = MAX(MIN(plsubstate.Qininstd/cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin); /* Csf is based on end of ramp speed */
   // currentsubstate.Rpm = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;
   _prePltPlsEndOfRamp = plsubstate;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PreplateletNoplasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   //  DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PreplateletNoplasma()";
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;

   Calc_Channel_Setup  instflows;

   IntermediateResults plsubstate;
   memset(&plsubstate, 0, sizeof(IntermediateResults));

   // Ending Qch
   float EndCsf = MAX(MIN(_Qinr * cobeConfig.InletFlowRampConstant / cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   float Qch    = MIN(_Qcmr, EndCsf *  _pirAlgo->getCollectMaxFlow() );

   // float maxqin = this->_procState.getQinistdMax();

   int   firstpass = 1;
   float pctneeded = 1.0f;
   while (1)
   {
      float QinLimit, QnLimit;
      initialFlowRamp(currentsubstate.EndVin, QinLimit, QnLimit);
      // if (QinLimit > maxqin) QinLimit = maxqin;

      instflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()  );
      instflows.Initialize(_hct, _precount, _TBV, _I, _ratio, QinLimit, cobeConfig.MaxInstantQreturn, QnLimit, Qch);
      instflows.calculate(&plsubstate);
      //
      //
      //  First time through - these are the commanded flows for the moment.  Save
      //  these in the official repository.
      //
      if (firstpass)
      {
         firstpass              = 0;
         currentsubstate        = plsubstate;
         currentsubstate.EndVin = previoussubstate.EndVin; // since we just overwrote it
      }
      //
      //
      //   This state terminates when we hit Vincv
      //
      float vindelta = plsubstate.Qininstd * plsubstate.Td;
      float vinnow   = currentsubstate.EndVin + vindelta;
      if (vinnow > _Vincv)
      {
         pctneeded = (_Vincv - currentsubstate.EndVin) / vindelta;
      }

      //
      //
      //    Add collected volumes to totals
      //
      if (pctneeded < 0.0f) pctneeded = 0.0f;
      currentsubstate.T      += plsubstate.Tcn * pctneeded;
      currentsubstate.Vac    += vindelta * pctneeded / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += vindelta * pctneeded;

      if (pctneeded < 1.0f) break;
   }

   _VincvTime          = previoussubstate.AccumTime + currentsubstate.T;
   _Yield              = 0.0f; // by definition
   currentsubstate.Qch = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   // plsubstate.Qininstd = _Qinr * cobeConfig.InletFlowRampConstant;
   // currentsubstate.Csf = MAX(MIN(plsubstate.Qininstd/ cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   // currentsubstate.Rpm = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;
   _prePltNoPlsEndOfRamp = plsubstate;
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PlateletPlasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PlateletPlasma()";
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;
   if (_Vpmin == 0.0f) return;


   float maxqin = this->_procState.getQinistdMax();
   if (_Qch < cobeConfig.MaxQchForQinDuringPC)
   {
      if (maxqin > cobeConfig.MaxQinPC) maxqin = cobeConfig.MaxQinPC;
   }
   else
   {
      if (maxqin > cobeConfig.MaxQinPlasmaCollect) maxqin = cobeConfig.MaxQinPlasmaCollect;
   }

   //
   //
   //   Calculate flows for this substate.  Flows do not change so they only need
   //   to be calculated once.
   //
   Calc_Platelet_Plasma plasmaflows;
   plasmaflows.SetCollectMaxFlow(_CollectMaximumFlow);
   plasmaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqin, cobeConfig.MaxInstantQreturn, _Qnmax, _Qcmr);
   plasmaflows.calculate(&currentsubstate);

   const float vp              = currentsubstate.Qpinstd * currentsubstate.Tcn;
   const float vc              = currentsubstate.Qcinstd * currentsubstate.Tcn;
   const float deltavin        = currentsubstate.Qininstd * currentsubstate.Td;

   bool        plasmaSatisfied = false;
   if (getProcPlasmaProductVol() > 0.0f)   // we are running to product volume not including AC
   {
      plasmaSatisfied = ((previoussubstate.AccumVp - previoussubstate.AccumVacp) > _Vptarg);
   }
   else
   {
      plasmaSatisfied = (previoussubstate.AccumVp >= _Vptarg);
   }

   while (!plasmaSatisfied)
   {

      float pctneeded = limitState(currentsubstate, previoussubstate, vp, vc, deltavin, _VinPIR);

      //
      //
      //   This substate can be bounded by EPCA.  If we can't catch up from here
      //   terminate; the remaining plasma will be collected in EP.
      //
      float maxVcRemainingBeforeVintarg = (_Vintarg - currentsubstate.EndVin - deltavin) / _PCAmlVinpermlVc;
      float maxvc = previoussubstate.AccumVc + currentsubstate.Vc + vc + maxVcRemainingBeforeVintarg + _maxEPCAVc;
      if (maxvc < _ProcFixedCollectVol)
      {
         float epcapctneeded = (_ProcFixedCollectVol - previoussubstate.AccumVc - currentsubstate.Vc - _maxEPCAVc
                                - (_Vintarg - currentsubstate.EndVin) / _PCAmlVinpermlVc) / (vc - deltavin / _PCAmlVinpermlVc);
         if (epcapctneeded < pctneeded) pctneeded = epcapctneeded;
      }

      //
      //
      //    Add collected plasma to totals
      //
      if (pctneeded < 1.0f)
      {
         plasmaSatisfied = 1;
         if (pctneeded < 0.0f) pctneeded = 0.0f;
         //
         //
         //   recalculate yield since we have altered the ending Vin
         //
         _Yield = getYield(currentsubstate.EndVin + deltavin * pctneeded,
                           previoussubstate.AccumVac + currentsubstate.Vac + (deltavin / _ratio) * pctneeded);
      }

      currentsubstate.T      += currentsubstate.Tcn * pctneeded;
      currentsubstate.Vp     += vp * pctneeded;
      currentsubstate.Vc     += vc * pctneeded;
      currentsubstate.Vacp   += vp * pctneeded * currentsubstate.Facb;
      currentsubstate.Vacc   += vc * pctneeded * currentsubstate.Facb;
      currentsubstate.Vac    += pctneeded * deltavin / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += deltavin * pctneeded;
   }
   currentsubstate.Qch   = currentsubstate.Qcinstd;
   currentsubstate.Yield = _Yield;
   currentsubstate.Csf   = MAX(MIN(currentsubstate.Qininstd / cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   currentsubstate.Rpm   = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;

   if (currentsubstate.T > 0.0f)
   {
      substate[Config::__PREPLATELET_PLASMA].Csf = substate[Config::__PREPLATELET_NOPLASMA].Csf =
                                                      currentsubstate.Csf;
      substate[Config::__PREPLATELET_PLASMA].Rpm = substate[Config::__PREPLATELET_NOPLASMA].Rpm =
                                                      currentsubstate.Rpm;
   }
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PCA (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   // DataLog(log_level_predict_debug) << " procedureDetailPltPlsRBC::PCA()";
   // figure out Qch during plt pls.
   float pltplsQch = substate[Config::__PLATELET_PLASMA].Qch;
   if ( _startingsubstate == Config::__PCA)
   {
      pltplsQch = _procState.getPlateletPlasmaQch();
   }

   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;
   if (_Qcmr <= pltplsQch) return;
   if (previoussubstate.AccumVp == 0.0f) return;

   //   Note flows are precalculated above
   const float vc        = currentsubstate.Qcinstd * currentsubstate.Tcn;
   const float acinvc    = vc * currentsubstate.Facb;
   const float vin       = currentsubstate.Qininstd * currentsubstate.Td;
   const float ac        = vin / _ratio;

   float       pctneeded = 1.0f;
   while (1)
   {

      //
      //
      //   This substate can end on hypovolemia
      //
      float vacnow    = previoussubstate.AccumVac + currentsubstate.Vac + ac;
      float remaining = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                             _procState.VAccumOverdraw(),
                                                                             previoussubstate.AccumVc + currentsubstate.Vc + vc, previoussubstate.AccumVacc + currentsubstate.Vacc + acinvc * pctneeded,
                                                                             previoussubstate.AccumVp, previoussubstate.AccumVacp,
                                                                             0.0f, 0.0f,
                                                                             0.0f, vacnow);
      if (remaining < _HypoMargin) break;
      //
      //
      //  This substate can end on yield achieved
      //
      float yieldnow = getYield(currentsubstate.EndVin + vin,
                                previoussubstate.AccumVac + currentsubstate.Vac + ac);
      if (yieldnow > _DesiredYield)
      {
         pctneeded = (_DesiredYield - _Yield) / (yieldnow - _Yield);
         yieldnow  = _DesiredYield;
      }
      _Yield = yieldnow;


      //
      //   This state typically ends on achieving concentration
      //
      // float midrunVc = (_Vintarg-currentsubstate.EndVin-vin*pctneeded)/_MidrunmlVinpermlVc;
      // Determine Time spent in platelet plasma.
      float timeInPltPlasma = substate[Config::__PLATELET_PLASMA].T;
      float timeInPCA       = 0.0f;
      if ( _startingsubstate == Config::__PLATELET_PLASMA)
      {
         timeInPltPlasma = timeInPltPlasma + (_procState.ProcRunTime() - _procState.VincvTime());
      }
      else if (_startingsubstate == Config::__PCA)
      {
         timeInPCA       = _procState.ProcRunTime() - _procState.SubstateStartTime();
         timeInPltPlasma = (_procState.ProcRunTime() - _procState.VincvTime()) - timeInPCA;
      }
      float Vc_reqd      = (_Qcmr - pltplsQch) *  timeInPltPlasma - (substate[Config::__PCA].Qcinstd * timeInPCA) - currentsubstate.Vc;
      // float endvc = previoussubstate.AccumVc + currentsubstate.Vc + vc*pctneeded + midrunVc;
      float pcapctneeded = 0.0f;
      if (Vc_reqd > 0.0f)                            // (endvc > _ProcFixedCollectVol) {
         pcapctneeded = Vc_reqd / (vc * pctneeded);  // (_ProcFixedCollectVol-previoussubstate.AccumVc-currentsubstate.Vc
      // -(_Vintarg-currentsubstate.EndVin)/_MidrunmlVinpermlVc)/(vc-vin/_MidrunmlVinpermlVc);
      else
         pcapctneeded = 0.0f;
      if (pcapctneeded < pctneeded) pctneeded = pcapctneeded;
      // }

      //
      //
      //    Add collected volumes to totals
      //
      if (pctneeded < 0.0f) pctneeded = 0.0f;
      currentsubstate.T      += currentsubstate.Tcn * pctneeded;
      currentsubstate.Vc     += vc * pctneeded;
      currentsubstate.Vacc   += acinvc * pctneeded;
      currentsubstate.Vac    += ac * pctneeded * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += vin * pctneeded;

      if (pctneeded < 1.0f) break;

   }
   currentsubstate.Qch   = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Yield = _Yield;
   currentsubstate.Csf   = MAX(MIN(currentsubstate.Qininstd / cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   currentsubstate.Rpm   = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::Midrun (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;

   const float vc        = currentsubstate.Qcinstd * currentsubstate.Tcn;
   const float acinvc    = vc * currentsubstate.Facb;
   const float vin       = currentsubstate.Qininstd * currentsubstate.Td;
   const float ac        = vin / _ratio;

   float       pctneeded = 1.0f;
   while (1)
   {


      //
      //
      //    This state can terminate on PIR volume required
      //
      if ((_VinPIR > 0.0f) && (currentsubstate.EndVin + vin > _VinPIR))
      {
         pctneeded = (_VinPIR - currentsubstate.EndVin) / vin;
      }

      //
      //
      //   This state should usually end on yield achieved
      //
      float yieldnow = getYield(currentsubstate.EndVin + vin,
                                previoussubstate.AccumVac + currentsubstate.Vac + ac);
      if (yieldnow > _DesiredYield)
      {
         float yieldpctneeded = (_DesiredYield - _Yield) / (yieldnow - _Yield);
         if (yieldpctneeded < pctneeded)
         {
            pctneeded = yieldpctneeded;
            yieldnow  = _DesiredYield;
         }
      }
      _Yield = yieldnow;


      //
      //
      //    Add collected volumes to totals
      //
      if (pctneeded < 0.0f) pctneeded = 0.0f;
      currentsubstate.T      += currentsubstate.Tcn * pctneeded;
      currentsubstate.Vc     += vc * pctneeded;
      currentsubstate.Vacc   += acinvc * pctneeded;
      currentsubstate.Vac    += ac * pctneeded * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += vin * pctneeded;

      if ((pctneeded < 1.0f) || (currentsubstate.EndVin > cobeConfig.VinMaxTarget)) break;

   }
   currentsubstate.Qch   = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Yield = _Yield;
   currentsubstate.Csf   = MAX(MIN(currentsubstate.Qininstd / cobeConfig.CsfQin, cobeConfig.CsfMax), cobeConfig.CsfMin);
   currentsubstate.Rpm   = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;

   if (!(substate[Config::__PLATELET_PLASMA].T > 0.0f))
   {
      substate[Config::__PREPLATELET_PLASMA].Csf = substate[Config::__PREPLATELET_NOPLASMA].Csf =
                                                      currentsubstate.Csf;
      substate[Config::__PREPLATELET_PLASMA].Rpm = substate[Config::__PREPLATELET_NOPLASMA].Rpm =
                                                      currentsubstate.Rpm;
   }

}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PIRPlasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   currentsubstate.Qch = currentsubstate.Qcinstd = currentsubstate.Qcinstr =  getPIRLockin() ;
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;
   if (!_pirAlgo->getPirRequired()) return;
   if (previoussubstate.T > 0.0f) return; // disallow "Midrun + PIRPlasma"
   if (_Vpmin == 0.0f) return;

   float maxqin = this->_procState.getQinistdMax();
   if (_Qch < cobeConfig.MaxQchForQinDuringPC)
   {
      if (maxqin > cobeConfig.MaxQinPC) maxqin = cobeConfig.MaxQinPC;
   }
   else
   {
      if (maxqin > cobeConfig.MaxQinPlasmaCollect)
         maxqin = cobeConfig.MaxQinPlasmaCollect;
   }
   float pirQinLimit = this->_procState.getPirQinLimit();

   if (substate[Config::__MIDRUN].T > 0.0f)
      pirQinLimit = substate[Config::__MIDRUN].Qininstd;
   else if (substate[Config::__PLATELET_PLASMA].T > 0.0f)
      pirQinLimit = substate[Config::__PLATELET_PLASMA].Qininstd;

   maxqin = (pirQinLimit != 0.0f) ? MIN(pirQinLimit, maxqin) : maxqin;
   float PrePirQch = _procState.getPrePirQch();
   if (PrePirQch == 0.0f)
   {
      PrePirQch = _Qch;
   }

   while (1)
   {
      //
      //
      //   Calculate flows for this substate.  Although Qc ramps throughout the flows
      //   for other pumps are fixed throughout the substate.
      //
      Calc_PIR_Plasma plasmaflows;
      plasmaflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow() );

      plasmaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqin, cobeConfig.MaxInstantQreturn, _Qnmax, PrePirQch);
      plasmaflows.calculate(&currentsubstate);
      currentsubstate.Qch = currentsubstate.Qcinstd = currentsubstate.Qcinstr = getPIRLockin();

      const float vin = currentsubstate.Qininstd * currentsubstate.Td;
      const float vp  = currentsubstate.Qpinstd * currentsubstate.Tcn;

      while (1)
      {

         //
         //
         //   Calculate ending Qc and therefore vc for this iteration
         //
         float vc        =  _pirAlgo->averagePIRQc(currentsubstate, _PIRVcAtMax, _Qch);

         float pctneeded = limitState(currentsubstate, previoussubstate, vp, vc, vin, cobeConfig.VinMaxTarget);

         //
         //
         //    Add collected plasma to totals
         //
         if (pctneeded < 0.0f) pctneeded = 0.0f;
         currentsubstate.T      += currentsubstate.Tcn * pctneeded;
         currentsubstate.Vp     += vp * pctneeded;
         currentsubstate.Vc     += vc * pctneeded;
         currentsubstate.Vacp   += vp * pctneeded * currentsubstate.Facb;
         currentsubstate.Vacc   += vc * pctneeded * currentsubstate.Facb;
         currentsubstate.Vac    += pctneeded * vin / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
         currentsubstate.EndVin += vin * pctneeded;

         if (pctneeded < 1.0f)
         {
            _PIRVcAtMax -= (1.0f - pctneeded) * vc; // we already accumulated one whole cycle
            break;
         }
      }
      currentsubstate.Qch = _pirAlgo->CalculatePIRQch(currentsubstate.T, _Qch);
      if (currentsubstate.Qch > getPIRLockin()) currentsubstate.Qch = getPIRLockin();

      //
      //
      //   If we have to split PIR then per design we have to enforce the minimum
      //   of Qin between the plasma and noplasma phases.  This is a V5 legacy requirement.
      //
      if ((_Yield >= _DesiredYield) || (currentsubstate.T == 0.0f)) break; // no split

      //
      //
      //  This is a split PIR.  Pre-calculate the noplasma flows and determine if we
      //  need to redo the PIR portion due to inlet flow limitations.
      //
      Calc_PIR_NoPlasma pirflows;

      pirflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()  );
      pirflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqin, cobeConfig.MaxInstantQreturn, _Qnmax, PrePirQch);
      pirflows.calculate(&substate[Config::__PIR_NOPLASMA]);

      if (maxqin <= substate[Config::__PIR_NOPLASMA].Qininstd) break; // this is the limiting flow already
      maxqin                 = substate[Config::__PIR_NOPLASMA].Qininstd;
      currentsubstate.T      = 0.0f;
      currentsubstate.Vp     = 0.0f;
      currentsubstate.Vc     = 0.0f;
      currentsubstate.Vacp   = 0.0f;
      currentsubstate.Vacc   = 0.0f;
      currentsubstate.Vac    = 0.0f;
      currentsubstate.EndVin = previoussubstate.EndVin;
   }
   currentsubstate.Csf   = PirCsf();
   currentsubstate.Rpm   = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;
   currentsubstate.Yield = _Yield;
}

float procedureDetailPltPlsRBC::PirCsf () const
{
   float Csf = ( _procState.getOptimizingSelectedProc() && (_procState.getPirCsf() != 0.0f) )
               ? _procState.getPirCsf()
               : 1.0f;

   if ((_procState.MidrunEntered() == 1) || (_startingsubstate > Config::__MIDRUN))
   {
      if (_procState.getPirCsf() != 0.0f)
         Csf = _procState.getPirCsf();
      else
         Csf = substate[Config::__MIDRUN].Csf;
   }
   else
   {
      if (substate[Config::__MIDRUN].T > 0.0f)
         Csf = substate[Config::__MIDRUN].Csf;
      else if (substate[Config::__PLATELET_PLASMA].T > 0.0f)
         Csf = substate[Config::__PLATELET_PLASMA].Csf;
   }
   return Csf;
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::PIRNoplasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   currentsubstate.Qch = currentsubstate.Qcinstd = currentsubstate.Qcinstr = getPIRLockin()  ;  // informational if substate not performed
   if ((_DesiredYield == 0.0f) || (_Yield >= _DesiredYield)) return;
   if (!_pirAlgo->getPirRequired() ) return;


   //
   //
   //   Check to see if this is a continuation of a PIR+Plasma portion of the run.
   //
   if (currentsubstate.Qininstd == 0.0f)
   {
      Calc_PIR_NoPlasma pirflows;
      float             qininstdmax = this->_procState.getQinistdMax();
      float             pirQinLimit = this->_procState.getPirQinLimit();
      if (substate[Config::__MIDRUN].T > 0.0f)
         pirQinLimit = substate[Config::__MIDRUN].Qininstd;
      else if (substate[Config::__PLATELET_PLASMA].T > 0.0f)
         pirQinLimit = substate[Config::__PLATELET_PLASMA].Qininstd;

      qininstdmax = (pirQinLimit != 0.0f) ? MIN(pirQinLimit, qininstdmax) : qininstdmax;

      pirflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()  );
      float PrePirQch = _procState.getPrePirQch();
      if (PrePirQch == 0.0f)
      {
         PrePirQch = _Qch;
      }
      pirflows.Initialize(_hct, _precount, _TBV, _I, _ratio, qininstdmax, cobeConfig.MaxInstantQreturn, _Qnmax, PrePirQch);
      pirflows.calculate(&currentsubstate);
      currentsubstate.Qch = currentsubstate.Qcinstd = currentsubstate.Qcinstr =  getPIRLockin();
   }


   const float vin = currentsubstate.Qininstd * currentsubstate.Td;
   const float ac  = vin / _ratio;

   currentsubstate.T = previoussubstate.T; // to continue PIR ramping appropriately
   float pctneeded = 1.0f;
   while (1)
   {

      float vc = _pirAlgo->averagePIRQc(currentsubstate, _PIRVcAtMax, _Qch);

      //
      //
      //   This state ends on yield achieved
      //
      float yieldnow = getYield(currentsubstate.EndVin + vin,
                                previoussubstate.AccumVac + currentsubstate.Vac + ac);
      if (yieldnow > _DesiredYield)
      {
         pctneeded = (_DesiredYield - _Yield) / (yieldnow - _Yield);
         if (pctneeded < 1.0f) yieldnow = _DesiredYield;
      }
      _Yield = yieldnow;

      //
      //
      //    Add collected volumes to totals
      //
      if (pctneeded < 0.0f) pctneeded = 0.0f;
      currentsubstate.T      += currentsubstate.Tcn * pctneeded;
      currentsubstate.Vc     += vc * pctneeded;
      currentsubstate.Vacc   += vc * currentsubstate.Facb * pctneeded;
      currentsubstate.Vac    += ac * pctneeded * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += vin * pctneeded;
      if ((pctneeded < 1.0f) || (currentsubstate.EndVin > cobeConfig.VinMaxTarget))
      {
         _PIRVcAtMax -= (1.0f - pctneeded) * vc; // we already accumulated one whole cycle
         break;
      }
   }

   currentsubstate.T  -= previoussubstate.T; // restore T for this substate only
   currentsubstate.Qch = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   //

   // Set Csf
   currentsubstate.Csf   = PirCsf();
   currentsubstate.Rpm   = sqrt(currentsubstate.Csf) * cobeConfig.RPMCollect;
   currentsubstate.Yield = _Yield;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::EPCA (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   currentsubstate.Csf = cobeConfig.CsfMax;  // By definition
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   if ((_ProcFixedCollectVol == 0.0f) || ((previoussubstate.AccumVc + 1.0f) >= _ProcFixedCollectVol)) return;

   //
   //
   //   Flows for this substate are precalculated above.
   //
   float vin = (_ProcFixedCollectVol - previoussubstate.AccumVc) * _EPCAmlVinpermlVc;
   if (vin > cobeConfig.MaximumEpcaVolume) vin = cobeConfig.MaximumEpcaVolume;
   if (_procState.VinEpcaStart() > 0.0f)
   {
      float maxendVin = _procState.VinEpcaStart() + cobeConfig.MaximumEpcaVolume;
      float maxVin    = maxendVin - previoussubstate.EndVin;
      if (vin > maxVin) vin = maxVin;
   }

   currentsubstate.Vc     = vin / _EPCAmlVinpermlVc;
   currentsubstate.T      = currentsubstate.Vc / currentsubstate.Qcinstd;
   currentsubstate.Vacc   = currentsubstate.Vc * currentsubstate.Facb;
   currentsubstate.EndVin = previoussubstate.EndVin + vin;
   currentsubstate.Vac    = vin / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::ExtendedPlasma (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{
   if (_Vpmin == 0.0f) return;

   //
   //
   //   Extended plasma collection is not allowed for maximum concurrent unless
   //   this is the only place we can collect it.
   //
   if ((_ProcAMAPorMC == 1) && (previoussubstate.AccumVp >= _Vpmin)) return;

   //
   //
   //   Calculate flows for this draw cycle
   //
   float                   maxqin = MIN(this->_procState.getQinistdMax(), _config.getMaxInstQin());
   if (maxqin > cobeConfig.MaxQinPCE) maxqin = cobeConfig.MaxQinPCE;
   Calc_Preplatelet_Plasma plasmaflows;

   plasmaflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()   );
   plasmaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqin, cobeConfig.MaxInstantQreturn, _Qnmax, _pirAlgo->getCollectMaxFlow());
   plasmaflows.calculate(&currentsubstate);

   //
   //
   //  Calculate plasma collected during this cycle and apply to our target
   //
   const float vp       = currentsubstate.Qpinstd * currentsubstate.Tcn;
   const float vindelta = currentsubstate.Qininstd * currentsubstate.Td;


   //
   //
   //   First "run" to minimum plasma needed regardless of time or hypo constraints.
   //   This causes stability in prediction for cusp cases.
   //
   float minplasma       = _Vpmin;
   if (_PlasmaTargetCannotChange) minplasma = _Vptarg;
   int   plasmaSatisfied = 0;

   if (previoussubstate.AccumVp < minplasma)
   {
      float cycles = (minplasma - previoussubstate.AccumVp) / vp;
      currentsubstate.T       = currentsubstate.Tcn * cycles;
      currentsubstate.Vp      = vp * cycles;
      currentsubstate.Vacp    = vp * cycles * currentsubstate.Facb;
      currentsubstate.Vac     = vindelta * cycles / _ratio * (1 - cobeConfig.RecirculateSnDrawVol / currentsubstate.Vsvn);
      currentsubstate.EndVin += vindelta * cycles;
      if (_ProcAMAPorMC == 1) plasmaSatisfied = 1; // this assumes AMAP and MC plasma products are always defined including AC
   }

   while (!plasmaSatisfied)
   {

      float pctneeded = limitState(currentsubstate, previoussubstate, vp, 0.0f, vindelta, 0.0f, 0);

      //
      //
      //   amap plasma collection can end if we hit maximum time
      //
      float lastCycleTime = previoussubstate.AccumTime + currentsubstate.T + _RBCSetup1Time + _RBCSetup2Time + _RBCTime + _purgeTime;
      float timenow       = lastCycleTime + currentsubstate.Tcn;
      if (timenow > _ProcTime)
      {
         float timepctneeded = (_ProcTime - lastCycleTime) / currentsubstate.Tcn;
         if (timepctneeded < pctneeded) pctneeded = timepctneeded;
      }


      //
      //
      //    Add collected plasma to totals
      //
      if (pctneeded < 1.0f)
      {
         plasmaSatisfied = 1;
         if (pctneeded < 0.0f) pctneeded = 0.0f;
      }
      currentsubstate.T      += currentsubstate.Tcn * pctneeded;
      currentsubstate.Vp     += vp * pctneeded;
      currentsubstate.Vacp   += vp * pctneeded * currentsubstate.Facb;
      currentsubstate.Vac    += vindelta * pctneeded / _ratio;
      currentsubstate.EndVin += vindelta * pctneeded;

   }

   //
   //
   //   Filter out "small but unneccesary" cases
   //
   if ((!_PlasmaTargetCannotChange)                                                   // we're flexible
       && (previoussubstate.AccumVp >= _Vpmin)                                        // and we don't really need the Vp
       && (currentsubstate.Vp < 20.0f)                                                // and it's small enough to ignore
       && ((previoussubstate.AccumVp > _MaxPlasma)                                    // for plasma rinseback, we may have hit max already
           || ((currentsubstate.Vp + previoussubstate.AccumVp + 0.4f) < _MaxPlasma))) // and it didn't help us hit max
   {
      currentsubstate.T      = 0.0f;
      currentsubstate.Vp     = 0.0f;
      currentsubstate.Vacp   = 0.0f;
      currentsubstate.Vac    = 0.0f;
      currentsubstate.EndVin = previoussubstate.EndVin;
   }
   currentsubstate.Csf = cobeConfig.CsfMax; // By definition
   currentsubstate.Rpm = cobeConfig.RPMCollect;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::RBCSetup1 (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{

   if (_PredRBCDose == 0.0f) return;
   if (!_PTF) return;

   currentsubstate.EndVin = previoussubstate.EndVin + _RBCSetup1Vin;
   currentsubstate.Vac    = _RBCSetup1Vac;
   currentsubstate.T      = _RBCSetup1Time;
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Csf    = cobeConfig.CsfMax; // By definition
   currentsubstate.Rpm    = cobeConfig.RPMCollect;
}
//
// ========================================================================================
//
void procedureDetailPltPlsRBC::RBCSetup2 (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{

   if (_PredRBCDose == 0.0f) return;
   if (!_PTF) return;

   currentsubstate.EndVin = previoussubstate.EndVin + _RBCSetup2Vin;
   currentsubstate.Vac    = _RBCSetup2Vac;
   currentsubstate.T      = _RBCSetup2Time;
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Csf    = cobeConfig.CsfMax; // By definition
   currentsubstate.Rpm    = cobeConfig.RPMCollect;
}


//
// ========================================================================================
//
void procedureDetailPltPlsRBC::RBCPrime (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{

   if (_PredRBCDose == 0.0f) return;
   if (!_PTF) return;

   currentsubstate.EndVin = previoussubstate.EndVin + _RBCPrimeVin;
   currentsubstate.Vac    = _RBCPrimeVac;
   currentsubstate.T      = _RBCPrimeTime;
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;

   currentsubstate.Vr     =   cobeConfig.PtfPltPlsRbcPrimeVolume;
   currentsubstate.Vacr   = _RBCPrimeVac;
   currentsubstate.Csf    = cobeConfig.CsfMax; // By definition
   currentsubstate.Rpm    = cobeConfig.RPMCollect;

}


//
// ========================================================================================
//
void procedureDetailPltPlsRBC::RBC (IntermediateResults& currentsubstate, IntermediateResults& previoussubstate)
{

   if (_PredRBCDose == 0.0f) return;

   currentsubstate.EndVin = previoussubstate.EndVin + _RBCVin;
   currentsubstate.Vac    = _RBCVac;
   currentsubstate.Vr     =  _PredRBCVolume - previoussubstate.Vr;
   currentsubstate.Vacr   = previoussubstate.Vac + _ACinRBC;
   currentsubstate.T      = _RBCTime;
   currentsubstate.Qch    = currentsubstate.Qcinstd - currentsubstate.Qpinstd;
   currentsubstate.Csf    = cobeConfig.CsfMax; // By definition
   currentsubstate.Rpm    = cobeConfig.RPMCollect;
}
//
// ========================================================================================
//
float procedureDetailPltPlsRBC::getYield (float Vin, float Vac)
{

   const float Qinc = MAX(_Qinc, cobeConfig.EfficiencyMinQinc);

   // _Ec = cobeConfig.PlateletEfficiencyMultiplier*(1.0f- Qinc/(cobeConfig.YieldQinConstant*1000.0f))*(1.0f-Vin/(cobeConfig.YieldVinConstant*100000.0f));
   const float postcount = _precount - (_DesiredYield * 1E5 / _TBV); // Never use mobilization!
   _Ec = cobeConfig.YieldEfficiencyConstant +
         cobeConfig.YieldQinConstant * Qinc + cobeConfig.YieldPostPltConstant * MIN(postcount, cobeConfig.EfficiencyMinPostcount);

   float Vsu = Software_CDS::isPPC(getProcPlateletProdCon()) ? cobeConfig.YieldStartupVolumePPC : cobeConfig.YieldStartupVolumeStd;
   float Yp  = 0.00001f * _config.getYSF() * _precount * _TBV * (1.0f - exp(-_Ec * (Vin - Vac - Vsu) / _TBV))
               - _Ydch // (Vin, _PIRRequired)
               - _Ydpur
               - _procState.getAccumYdPur();

   return Yp;
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::countRemainingPurges (float startVin, float endVin)
{
   _purgeTime = 0.0f;
   _purges    = 0;    // remaining scheduled purges
   _Ydpur     = 0.0f; // decrement due to remaining scheduled purges

   //
   //
   //  Sanity check numbers.  This is prevent excessive iteration especially in the
   //  case of bad purge data from proc.  Note a reasonable max vin assuming 2.2*TBV
   //  and a excessively large donor would be about 30000ml.
   //
   static const float VIN_TOO_BIG = 60000.0f;
   if ((startVin < 0.0f) || (endVin < 0.0f) || (startVin > VIN_TOO_BIG) || (endVin > VIN_TOO_BIG))
   {
      DataLog(log_level_predict_debug) << "countRemainingPurges: bad input: " << startVin << " " << endVin
                                       << " assuming zero purges" << endmsg;
      return;
   }

   while (startVin < endVin)
   {
      _purgeTime += CalculatePurgeTime(endVin, startVin);
      ++_purges;
      startVin   +=  _deltaPurge;
      startVin    =  MovePurgePoint(startVin);
      _Ydpur     +=  _Ypur;
   }

   //
   //
   //    Add in any time for purges currently in progress
   //
   if (_procState.ProcRunTime() < _purgeEndTime)
   {
      _purgeTime += (_purgeEndTime - _procState.ProcRunTime());
   }
}

float procedureDetailPltPlsRBC::getWBCRmpUpRmpDnPurgeTime (float centrifugeSpeed, const CobeConfig::CobeConfigStruct& cobeConfig)
{
   float totalTime = 0;
   DataLog(log_level_predict_debug) << "Last rpm purge time at predict: " << centrifugeSpeed << endmsg;
   if ( centrifugeSpeed > 0.0f)
   {

      /* quick reference
       StandardHighCentrifugeRamp; 20 rpm/sec
       StandardLowCentrifugeRamp; 100 rmp/sec
       CentrifugeRampDownRate; 50 rpm/sec
       StandardCentrifugeRampTransition; 2500
      */
      float speedDiff  = cobeConfig.RPMCollect - centrifugeSpeed;
      float rumpUpTime = 0;

      if ( centrifugeSpeed > cobeConfig.StandardCentrifugeRampTransition && cobeConfig.StandardHighCentrifugeRamp != 0.0f )
         rumpUpTime = speedDiff / cobeConfig.StandardHighCentrifugeRamp;

      else if ( centrifugeSpeed <= cobeConfig.StandardCentrifugeRampTransition && cobeConfig.StandardHighCentrifugeRamp != 0.0f
                && cobeConfig.StandardLowCentrifugeRamp != 0.0f)
         rumpUpTime = (cobeConfig.RPMCollect - cobeConfig.StandardCentrifugeRampTransition) / cobeConfig.StandardHighCentrifugeRamp + (cobeConfig.StandardCentrifugeRampTransition - centrifugeSpeed) / cobeConfig.StandardLowCentrifugeRamp;

      float rumpDnTime = 0;
      if ( speedDiff > 0.0f && cobeConfig.CentrifugeRampDownRate != 0.0f)  // divide by zero protection
         rumpDnTime = speedDiff / cobeConfig.CentrifugeRampDownRate;

      totalTime += rumpUpTime + rumpDnTime;

      DataLog(log_level_predict_debug) << "Last rpm purge time at predict: " << centrifugeSpeed << " rumpUpTime: " << rumpUpTime << " rumpDnTime: " << rumpDnTime << endmsg;
   }
   return totalTime;
}

// ========================================================================================

float procedureDetailPltPlsRBC::Ydch (float Vin, int PIRRequired) const
{
   // Get PIR Csf. Either Plt-pls or Midrun
   int substateId = Config::__MIDRUN;
   if (substate[Config::__MIDRUN].T == 0.0f)
      if (substate[Config::__PLATELET_PLASMA].T != 0.0f)
         substateId = Config::__PLATELET_PLASMA;

   const float Csf = substate[substateId].Csf;

   // Qch
   const float Qch      = (PIRRequired) ? getPIRLockin() : MIN(_Qch, _CollectMaximumFlow * Csf);
   // Nwsat
   const float Aw       = cobeConfig.WBCAwSaturationConstant * Csf;
   const float Bw       = cobeConfig.WBCBwSaturationConstant;
   const float Nwsat    = 1.0f / (Qch / Aw + Bw);
   // Nwsat_mr (Nwsat for Qch prior to PIR)
   const float Nwsat_mr = 1.0f / (MIN(_Qch, _CollectMaximumFlow * Csf) / Aw  + Bw);

   // Fw
   const float Cw          = cobeConfig.MNCPrecountForPltLoss;
   const float Cwp         = MNCPrecountForPurge();
   const float Nwr         = PostPurgeWBCInChamber();
   float       Npurges     = 0.0f;
   float       remPurge    = 0.0f;
   const int   totalpurges = _purges + _procState.getAccumPurges();
   float       Npur_mod    = 0.0f;
   if ( totalpurges > 0)
   {
      const float DVinpur =  1000.0f * (Nwsat_mr - Nwr) / (Cwp * _oneM1Ratio);
      if (DVinpur > 0.0f)    // divide by zero sanity check
      {
         remPurge = (Vin - _Vinpur) / DVinpur ;
         Npurges  = 1.0f + MAX(remPurge, 0.0f);
         Npur_mod = Npurges - totalpurges;
      }
   }

   // Npsat
   const float Ap    =  cobeConfig.PlateletSaturationAp * Csf;
   const float Bp    = cobeConfig.PlateletSaturationBp;
   const float Npsat = 1.0f / (Qch / Ap + Bp);

   // Fw
   float Fw = 0.0f;
   if (totalpurges == 0.0f)
   {
      const float Nwch = cobeConfig.WBCChannelInventory;
      Fw = (Cw * _oneM1Ratio * (Vin / 1000.0f) - Nwch) / Nwsat_mr;
      if (Fw < 0.0f) Fw = 0.0f;
   }
   else
   {
      Fw = Nwr * (1.0f - (Cw / Cwp) * Npur_mod) / Nwsat + (Cw / Cwp) * Npur_mod;
      if (Fw > 1.0f) Fw = 1.0f;
   }

   // Ydch
   float Ydch = 0.0f;

   if (PIRRequired)
      Ydch = (1.0f - (Fw * Nwsat_mr / Nwsat)) * Npsat;    // Fw correction.
   else
      Ydch = (1.0f - Fw) * Npsat;   // Look for Np in Bob's alg. document

   _Fw = Fw;  // for logging
   return Ydch;
}

// For substates prior to PIR
float procedureDetailPltPlsRBC::Ypur (int substateId) const
{
   float Ypur = 0.0f;
   // if ( (substateId >= Config::__PLATELET_PLASMA)
   //	 && (substateId <= Config::__MIDRUN)
   //	 )
   {
      const float Fch = cobeConfig.PurgeLossPltInChamber;
      float       Csf = substate[substateId].Csf;
#ifndef __WIN32__
      if (Csf == 1.0f && substate[substateId].T == 0.0f)
      {
         Csf = _procState.getCsf(_procState.SubState());
         if (Csf <= 0.0f)
            Csf = 1.0f;
      }
#endif
      // Qch
      float Qch = MIN(_Qch, _CollectMaximumFlow * Csf);
      if (Qch == 0.0f)
      {
         Qch = _procState.Qch();
      }
      // Npsat
      const float Ap    =  cobeConfig.PlateletSaturationAp * Csf;
      const float Bp    = cobeConfig.PlateletSaturationBp;
      const float Npsat = 1.0f / (Qch / Ap + Bp);

      // Fw
      const float Cw  = cobeConfig.MNCPrecountForPltLoss;
      const float Cwp = MNCPrecountForPurge();
      const float Fw  = Cw / Cwp;

      // Ypur
      const float Np = Npsat * ( 1.0f - Fw );
      Ypur = Fch * Np;
   }
   return Ypur;
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::initialFlowRamp (float Vin, float& QinLimit, float& QnLimit)
{
   //   Default return in case ramp complete or not configured
   QinLimit = this->_procState.getQinistdMax();
   QnLimit  = _Qnmax;

   // cannot get a stop ramping msg unless we're using autoflow.
   if ( _config.isAutoFlowOn() && _procState.StopRamping() )
   {
      QinLimit =  _procState.DeadRampQinmax();
      static bool doitonce1 = false;
      if (!doitonce1)
      {
         doitonce1 = true;
         DataLog(log_level_predict_debug) << "AutoFlow: aps stopping Ramping, capped at " << QinLimit << endmsg;
      }
   }

   float Vinr = (_Vincv < cobeConfig.VinEndOfQinRamp ? _Vincv : cobeConfig.VinEndOfQinRamp);
   if (Vin < Vinr)
   {
      float Qin = cobeConfig.InletFlowRampConstant * (cobeConfig.QinStartup + (Vin - cobeConfig.PlateletPlasmaPrimeVolume) * ((_Qinr - cobeConfig.QinStartup) / (Vinr - cobeConfig.PlateletPlasmaPrimeVolume)));
      Qin     = MIN(MAX(Qin, cobeConfig.QinStartup), QinLimit);
      QnLimit = _Qnmax * (Qin / ((_QinrQnLimit == 0.0f) ? _Qinr : _QinrQnLimit));
      QnLimit = MIN(QnLimit, _Qnmax);
      QnLimit = MAX(QnLimit, cobeConfig.QneedleLimitMin); // IT9693
      if (_config.getInletFlowRamp()) QinLimit = Qin;
   }
}
//
// ========================================================================================
//  Limiting state information for states that collect plasma
//
float procedureDetailPltPlsRBC::limitState (const IntermediateResults& currentsubstate,
                                            const IntermediateResults& previoussubstate,
                                            float deltavp,
                                            float deltavc,
                                            float deltavin,
                                            float maxvin,
                                            int endonyield)
{
   float       pctneeded     = 1.0f;
   float       plasmanow     = previoussubstate.AccumVp + currentsubstate.Vp + deltavp;

   const float acinvp        = deltavp * currentsubstate.Facb;
   const float acinvc        = deltavc * currentsubstate.Facb;
   const int   extendedstate = (&currentsubstate == &substate[Config::__EXTENDED_PLASMA]);

   //
   //
   //   both fixed and amap plasma volumes can end on collected volume
   //
   if (_Vptarg > 0)
   {
      if (getProcPlasmaProductVol() > 0.0f)    // we are running to product volume not including AC
      {
         pctneeded = (_Vptarg - previoussubstate.AccumVp + previoussubstate.AccumVacp - currentsubstate.Vp + currentsubstate.Vacp) / (deltavp - acinvp);
      }
      else
      {
         pctneeded = (_Vptarg - previoussubstate.AccumVp  - currentsubstate.Vp) / deltavp;
      }
      if (pctneeded > 1.0f) pctneeded = 1.0f;  // typical during plasma collection unless this is the final cycle
      if (pctneeded < 0.0f) pctneeded = 0.0f;  // robustness.  Vptarg should always be a minimum of already collected volume
   }



   //
   //
   //    This state can terminate on volume required
   //
   if (maxvin > 0.0f)
   {
      if (currentsubstate.EndVin + deltavin > maxvin)
      {
         float pirpctneeded = (maxvin - currentsubstate.EndVin) / deltavin;
         if (pirpctneeded < pctneeded) pctneeded = pirpctneeded;
      }
   }

   //
   //
   //   Check to see if we have exceeded hypvolemia limits.  If so reduce vp collected
   //   this cycle until within constraints.
   //
   float delta         = 0.0f;
   float endvac        = previoussubstate.AccumVac + (_Vintarg - previoussubstate.EndVin) / _ratio + _RBCVac;
   float remainingnow, endremaining;
   float hypopctneeded = pctneeded;
   while (1)
   {
      float vacnow        = previoussubstate.AccumVac + currentsubstate.Vac + deltavin * hypopctneeded / _ratio;
      plasmanow    = previoussubstate.AccumVp + currentsubstate.Vp + deltavp * hypopctneeded;
      float acinplasmanow = previoussubstate.AccumVacp + currentsubstate.Vacp + acinvp * hypopctneeded;
      remainingnow = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                          _procState.VAccumOverdraw(),
                                                                          previoussubstate.AccumVc + currentsubstate.Vc + deltavc * hypopctneeded, previoussubstate.AccumVacc + currentsubstate.Vacc + acinvc * hypopctneeded,
                                                                          plasmanow, acinplasmanow,
                                                                          0.0f, 0.0f,
                                                                          0.0f, vacnow);
      if (extendedstate)
      {
         endremaining = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                             _procState.VAccumOverdraw(),
                                                                             currentsubstate.AccumVc, currentsubstate.AccumVacc,
                                                                             plasmanow, acinplasmanow,
                                                                             _PredRBCVolume, _ACinRBC,
                                                                             0.0f, vacnow + _RBCVac);
      }
      else
      {
         endremaining = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                             _procState.VAccumOverdraw(),
                                                                             (_ProcFixedCollectVol + _VcOvercollect), (_ProcFixedCollectVol + _VcOvercollect) * currentsubstate.Facb,
                                                                             plasmanow, acinplasmanow,
                                                                             _PredRBCVolume, _ACinRBC,
                                                                             0.0f, endvac);
      }
      float remaining = (endremaining < remainingnow ? endremaining : remainingnow);
      if (remaining > _HypoMargin) break;             // not bounded or no longer bounded by hypovolemia
      delta        += _HypoMargin - remaining + 0.4f; // next try, shoot for less than 1ml for display rounding
      hypopctneeded = (deltavp - delta) / deltavp;
      if (hypopctneeded < 0.0f) break; // we can't get any more plasma this iteration
   }
   if (hypopctneeded < pctneeded) pctneeded = hypopctneeded;

   //
   //
   //  This state can also end on Yield achieved
   //
   if (endonyield)
   {
      float yieldnow = getYield(currentsubstate.EndVin + deltavin,
                                previoussubstate.AccumVac + currentsubstate.Vac + deltavin / _ratio);
      if (yieldnow > _DesiredYield)
      {
         float yieldpctneeded = (_DesiredYield - _Yield) / (yieldnow - _Yield);
         if (yieldpctneeded < pctneeded)
         {
            pctneeded = yieldpctneeded;
            yieldnow  = _DesiredYield;
         }
      }
      _Yield = yieldnow;
   }

   if (pctneeded < 0.0f) pctneeded = 0.0f;
   return pctneeded;
}

float procedureDetailPltPlsRBC::MNCPrecountForPurge () const
{
   if ( _bmi < cobeConfig.BMIForIncreasedPurges )
      return cobeConfig.MNCPrecountForPurge;
   else
      return cobeConfig.MNCPrecountForPurgeLargeBMI;
}

float procedureDetailPltPlsRBC::PostPurgeWBCInChamber () const
{
   if ( _bmi < cobeConfig.BMIForIncreasedPurges )
      return cobeConfig.PostPurgeWBCInChamber;
   else
      return cobeConfig.PostPurgeWBCInChamberLargeBMI;
}




void procedureDetailPltPlsRBC::setChamberFlowConstants ()
{
   if ( Software_CDS::isPPC(getProcPlateletProdCon()) )
   {
      _CollectMaximumFlow = cobeConfig.CollectMaximumFlowHigh ;
   }
   else
   {
      _CollectMaximumFlow = cobeConfig.CollectMaximumFlowStandard;
   }

}

float procedureDetailPltPlsRBC::CalculateCsf (int substateId) const
{
   switch (substateId)
   {
      case -1 :
      case 0 : /* NOCOLLECT */
         return 1.0f;
      default :
         if (substateId > Config::__PIR_NOPLASMA)
            return 1.0f;
         else
            return substate[substateId].Csf;

   }
}

/* Uses : Ccv, _Qcmr, CobeConfig, precount, ratio */
void procedureDetailPltPlsRBC::CalculateVincv ()
{
   if (_procState.Vincv() == 0.0f)
   {
      int substateId = ((substate[Config::__PREPLATELET_NOPLASMA].T > 2.0f) || (substate[Config::__PREPLATELET_PLASMA].T ==0.0f)) ?
                       Config::__PREPLATELET_NOPLASMA : Config::__PREPLATELET_PLASMA;

      float       CsfVincv = substate[substateId].Csf;
      // float Qch = MIN(_Qcmr, CsfVincv * _CollectMaximumFlow);
      float       Qch      = MIN(_Qcmr, substate[substateId].Qch);
      const float Ap       = cobeConfig.PlateletSaturationAp;
      const float Bp       = cobeConfig.PlateletSaturationBp;
      const float Npsat    = 1.0f / (Qch / (Ap * CsfVincv) + Bp);
      const float Ccv      = cobeConfig.VinCollectStartConstant;
      const float Npch     = cobeConfig.ChannelPlateletInventory;

      _Vincv = Ccv * 100000.0f * (Npsat + Npch) / (_precount * _oneM1Ratio); // Converge properly
      if (_Vincv < cobeConfig.MinVinCollectStart) _Vincv = cobeConfig.MinVinCollectStart;
      if (_Vincv > cobeConfig.MaxVinCollectStart) _Vincv = cobeConfig.MaxVinCollectStart;
   }
   else
   {
      _Vincv     = _procState.Vincv();
      _VincvTime = _procState.VincvTime();
   }
}

void procedureDetailPltPlsRBC::PrecalculateFlows ()
{
   // Pre-calculate flows for PCA and EPCA.  These are used by other substates in
   // determining if the procedure is limited by collect volume.
   Calc_PCA pcaflows;
   float    maxqin    = this->_procState.getQinistdMax();
   float    maxqinpca = MIN(maxqin, cobeConfig.MaxQinPCA);

   pcaflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow() ) ;    // set the max flow based on concentration
   pcaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqinpca, cobeConfig.MaxInstantQreturn, _Qnmax, _Qcmr);
   pcaflows.calculate(&substate[Config::__PCA]);

   _PCAmlVinpermlVc = substate[Config::__PCA].Qininstd * substate[Config::__PCA].Td / (substate[Config::__PCA].Qcinstd * substate[Config::__PCA].Tcn);

   // Note that max EPCA Vc is only used in states preceeding EPCA; therefore if those
   // states are calculated then the maximum is available.
   if (maxqin > cobeConfig.MaxQinPCAE) maxqin = cobeConfig.MaxQinPCAE;

   pcaflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()  ) ;   // set the max flow based on concentration

   float qchEpca = cobeConfig.EpcaChamberFlow;

   if ( Software_CDS::isPPC(getProcPlateletProdCon() )  )
   {
      qchEpca = cobeConfig.Qch1;   // Run at the wbc flag limit
      if ((getPIRLockin() != 0.0f) && qchEpca > getPIRLockin())
         qchEpca = getPIRLockin();
   }


   pcaflows.Initialize(_hct, _precount, _TBV, _I, _ratio, maxqin, cobeConfig.MaxInstantQreturn, _Qnmax, qchEpca);
   pcaflows.calculate(&substate[Config::__EXTENDED_PCA]);
   _EPCAmlVinpermlVc = substate[Config::__EXTENDED_PCA].Qininstd * substate[Config::__EXTENDED_PCA].Td / (substate[Config::__EXTENDED_PCA].Qcinstd * substate[Config::__EXTENDED_PCA].Tcn);
   _maxEPCAVc        = cobeConfig.MaximumEpcaVolume / _EPCAmlVinpermlVc;

   // Precalculate flows for midrun for use in PCA substate
   Calc_Midrun midrunflows;

   midrunflows.SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()  ) ;
   midrunflows.Initialize(_hct, _precount, _TBV, _I, _ratio, this->_procState.getQinistdMax(), cobeConfig.MaxInstantQreturn, _Qnmax, _Qcmr);
   midrunflows.calculate(&substate[Config::__MIDRUN]);
   _MidrunmlVinpermlVc = substate[Config::__MIDRUN].Qininstd * substate[Config::__MIDRUN].Td / (substate[Config::__MIDRUN].Qcinstd * substate[Config::__MIDRUN].Tcn);
   if (_Qinr == 0.0f) _Qinr = substate[Config::__MIDRUN].Qininstd;

   //  Precalculate RBC flows if needed.  Precalculation is required in case RBC time
   //  limits AMAP collection time.
   _RBCVin = _RBCVac = _ACinRBC = 0.0f;

   if (_PredRBCDose > 0.0f)
   {   // Set RBC max flow
      float Qrbcmax = this->_procState.getQinistdMax();


      //
      // Calculate RBC Setup states
      //
      if (  _PTF &&
            (_procState.RbcVolume() == 0.0f)  // don't need to predict after start of RBC collection
            )
      {

         /////// SETUP 1 ////////
         Calc_RBC_Setup setupflows;

         const float    rbcSetup1Hrbc   = cobeConfig.PtfPltPlsRbcSetup1Hrbc / 100.0f;
         const float    rbcSetup1Volume = (cobeConfig.PtfPltPlsRbcSetup1Volume) ? cobeConfig.PtfPltPlsRbcSetup1Volume : _procState.RbcSetup1Volume();

         setupflows.Initialize(_hct,
                               _precount,
                               _TBV,
                               _I,
                               _config.getACRatioDuringRBC(),
                               this->_procState.getQinistdMax(),
                               cobeConfig.MaxInstantQreturn,
                               _Qnmax,
                               cobeConfig.RBCQch,
                               rbcSetup1Hrbc);

         setupflows.calculate(&substate[Config::__RBC_SETUP_1]);

         _RBCSetup1Time = (rbcSetup1Volume - _procState.RbcSetup1Volume()) / substate[Config::__RBC_SETUP_1].Qininstd;
         _RBCSetup1Vin  = rbcSetup1Volume - _procState.RbcSetup1Volume();
         _RBCSetup1Vac  = _RBCSetup1Vin / _config.getACRatioDuringRBC()
                          * ( 1.0 - cobeConfig.RecirculateSnDrawVol / substate[Config::__RBC_SETUP_1].Vsvn );


         /////// SETUP 2 ////////
         const float    rbcSetup2Hrbc   = cobeConfig.PtfPltPlsRbcSetup2Hrbc / 100.0f;
         const float    rbcSetup2Volume = (cobeConfig.PtfPltPlsRbcSetup2Volume) ? cobeConfig.PtfPltPlsRbcSetup2Volume : _procState.RbcSetup2Volume();

         Calc_RBC_Setup setupflows1;
         setupflows1.Initialize(_hct,
                                _precount,
                                _TBV,
                                _I,
                                _config.getACRatioDuringRBC(),
                                this->_procState.getQinistdMax(),
                                cobeConfig.MaxInstantQreturn,
                                _Qnmax,
                                cobeConfig.RBCQch,
                                rbcSetup2Hrbc);

         setupflows1.calculate(&substate[Config::__RBC_SETUP_2]);

         _RBCSetup2Time = (rbcSetup2Volume - _procState.RbcSetup2Volume()) / substate[Config::__RBC_SETUP_2].Qininstd;
         _RBCSetup2Vin  = rbcSetup2Volume - _procState.RbcSetup2Volume();
         _RBCSetup2Vac  = _RBCSetup2Vin / _config.getACRatioDuringRBC()
                          * ( 1.0 - cobeConfig.RecirculateSnDrawVol / substate[Config::__RBC_SETUP_2].Vsvn );


         /////// PRIME   ////////
         const float        rbcPrimeHrbc   = cobeConfig.PtfPltPlsRbcPrimeHrbc / 100.0f;
         const float        rbcPrimeVolume = (cobeConfig.PtfPltPlsRbcPrimeVolume) ? cobeConfig.PtfPltPlsRbcPrimeVolume : _procState.RbcPrimeVolume();
         const float        rbcPrimeQrbc   = cobeConfig.PtfPltPlsRbcPrimeQrbc;

         Calc_RBC_PTF_PRIME primeflows;

         primeflows.Initialize(_hct,
                               _precount,
                               _TBV,
                               _I,
                               _config.getACRatioDuringRBC(),
                               this->_procState.getQinistdMax(),
                               cobeConfig.MaxInstantQreturn,
                               _Qnmax,
                               rbcPrimeHrbc,
                               cobeConfig.RBCQch,
                               rbcPrimeQrbc);

         primeflows.calculate(&substate[Config::__RBC_PRIME]);

         _RBCPrimeTime = (rbcPrimeVolume - _procState.RbcPrimeVolume()) / substate[Config::__RBC_PRIME].Qininstd;
         _RBCPrimeVin  = rbcPrimeVolume - _procState.RbcPrimeVolume();
         _RBCPrimeVac  = _RBCPrimeVin / _config.getACRatioDuringRBC()
                         * ( 1.0 - cobeConfig.RecirculateSnDrawVol / substate[Config::__RBC_PRIME].Vsvn );

      }


      /////////////////////////////////////////////////////////////////////////////////////////////////
      // get the collect flow types:
      Calc_RBC_PTF ptf_rbc_flows;
      Calc_RBC     non_ptf_rbc_flows;

      // Moved to not effect Setup1/2 per L.F. 4-9-07
      // not limited unless PTF active
      if (_PTF && (Qrbcmax > cobeConfig.PtfQrbc) )
         Qrbcmax = cobeConfig.PtfQrbc;

      /////// COLLECT ////////

      //
      // RBC Collection
      //
      Calc_RBC* rbcflows = NULL;

      // Use the PTF flows if using PTF sets
      if (_PTF)
         rbcflows = &ptf_rbc_flows;
      else
         rbcflows = &non_ptf_rbc_flows;


      // Generate RBC flows

      rbcflows->SetCollectMaxFlow(_pirAlgo->getCollectMaxFlow()   ) ;

      rbcflows->Initialize(_hct,
                           _precount,
                           _TBV, _I,
                           _config.getACRatioDuringRBC(),
                           this->_procState.getQinistdMax(),
                           cobeConfig.MaxInstantQreturn,
                           _Qnmax,
                           _PredRBCCrit / 100.0f,
                           cobeConfig.RBCQch,
                           Qrbcmax);

      rbcflows->calculate(&substate[Config::__RBC]);

      const IntermediateResults* results = &substate[Config::__RBC];

      _ACinRBC = _PredRBCVolume * results->Facb * (1.0f - _PredRBCCrit / 100.0f);

      _RBCTime = ((_PredRBCVolume - _procState.RbcVolume()) * results->Tcn) / ( (results->Qininstd - results->Qcinstd) * results->Td);
      _RBCVin  = results->Qininstd * results->Td * _RBCTime / results->Tcn;
      _RBCVac  = _RBCVin / _config.getACRatioDuringRBC()
                 * ( 1.0 - cobeConfig.RecirculateSnDrawVol / substate[Config::__RBC].Vsvn );
   }
}

//
// Call this after precalculate flows.
//
/* Csf, CobeConfig, Ratio */
void procedureDetailPltPlsRBC::CalculatePurgePoints ()
{


   _Ypur = Ypur(Config::__MIDRUN);
   float       CsfVinpurge = substate[Config::__MIDRUN].Csf;
   const float Qch         = (_pirAlgo->getPirRequired()) ? getPIRLockin() : MIN(_Qch, _CollectMaximumFlow * CsfVinpurge);

   // Nwsat
   const float Aw    = cobeConfig.WBCAwSaturationConstant * CsfVinpurge;
   const float Bw    = cobeConfig.WBCBwSaturationConstant;
   const float Nwsat = 1.0f / (Qch / Aw + Bw);

   // Vinpur
   const float Cwp  = MNCPrecountForPurge();
   const float Nwr  = PostPurgeWBCInChamber();
   const float Nwch = cobeConfig.WBCChannelInventory;

   //
   //
   //    Calculate size of a purge delta.  Note this value should always be greater than zero as both Qch and Aw
   //    scale with CSF, forcing Nwsat greater than Nwr.
   //
   _deltaPurge = 1000.0f * (Nwsat - Nwr) / (Cwp * _oneM1Ratio);

   //
   //
   //   If we have done a purge, then the next one is a delta from the last one.  Otherwise
   //   we are still waiting for the first purge point.
   //
   if (_procState.WbcLastProcPurge() > 0.0f) // we have done at least one, all are deltas from now on
   {
      _Vinpur        = _procState._predictCDS.VinPurge.Get(); // never update after first purge
      _nextProcPurge = MovePurgePoint(_procState.WbcLastProcPurge() + _deltaPurge);
   }
   else // no purges done yet, calculate first point
   {
      _Vinpur        = MovePurgePoint(1000.0f * (Nwsat + Nwch) / (Cwp * _oneM1Ratio));
      // _Vinpur        = 500.0f; // DEBUG ONLY to create early purges for testing
      _nextProcPurge = _Vinpur;
   }
   _nextProcPurgeTime = CalculatePurgeTime(_Vintarg, _nextProcPurge);

   _pirAlgo->calcPirRequired();

   // Calculate Vinpir
   if (_pirAlgo->getPirRequired())
   {
      if (_PIRAvgQin == 0.0f)
         _PIRAvgQin = _Qinc;

      _VinPIR = _Vintarg
                - _PIRAvgQin * cobeConfig.PIRVcAtMax / getPIRLockin() // Vin required at final flow
                - _PIRAvgQin * _pirAlgo->CalculatePIRRampTime(MIN(_Qch, _CollectMaximumFlow * CsfVinpurge), getPIRLockin());
      // IT 9261.
      if ((_procState.Vin() > _VinPIR) && !InPir())
      {
         if (_pirAlgo->getPirRequired())
         {
            _VinPIR = _procState.Vin();
         }
         else
         {
            _pirAlgo->setPirRequired(false);
            _lastPredPIR = false;
            _VinPIR      = 0.0;
         }
      }
   }
   else
      _VinPIR = 0.0f;


   //
   //
   //  recalculate purge points for next pass.  Note scheduled purges only
   //  occur through the midrun substate. Nocollect flows require infusion rate (calculated in toplevel)
   //  and purge time calculations require the nocollect flows.  Since time is changing each iteration
   //  (and therefore infusion) we recalcuate these each pass.
   //
   float endOfpurges = (_VinPIR>0.0f ? _VinPIR : _Vintarg);
   countRemainingPurges(_nextProcPurge, endOfpurges);
   _Ydch = Ydch(_Vintarg, _pirAlgo->getPirRequired() );
}

//
// ========================================================================================
//
void procedureDetailPltPlsRBC::AccumulateVolumes ()
{
   //
   //
   //   First call base class to do the substate volume accumulations
   //
   procedureDetail::AccumulateVolumes();

   //
   //
   //   Add in any time of remaining purges
   //
   _PredTp = substate[getNumOfSubstates() - 1].AccumTime + _purgeTime;
}

//
// ========================================================================================
//
float procedureDetailPltPlsRBC::CalculatePurgeTime (float vintarg, float purgePoint)
{

   float nocollectQin = substate[0].Qininstd;

   //
   //
   //   Calculate settle channel time.  Assume all in a single draw cycle (no duty cycle adjustment)
   //   Settle channel runs Qin at the substate draw flow so we need to find out which substate
   //   we are in (use nocollect for fallback).
   //
   float settleChannelQin = nocollectQin;
   for (int subndx = 0; subndx<getNumOfSubstates(); subndx++)
   {
      if ((substate[subndx].T > 0.0f) && (substate[subndx].EndVin  >= purgePoint)
          && (substate[subndx].Qininstd > 0.0f))   // should not happen, divide by zero protection
      {
         settleChannelQin = substate[subndx].Qininstd;
         break;
      }
   }
   if (settleChannelQin > cobeConfig.SettleChannelQin) settleChannelQin = cobeConfig.SettleChannelQin;
   float settleChannelTime = cobeConfig.SettleChannelVin / settleChannelQin;

   //
   //
   //   Calculate clear line time.
   //
   float clearlineQc = cobeConfig.ClearLineQcollect;
   if (nocollectQin < cobeConfig.ClearLineQin)
   {
      clearlineQc *= nocollectQin / cobeConfig.ClearLineQin;
   }
   float clearLineTime = cobeConfig.ClearLineVcollect / clearlineQc;


   //
   //
   //   Calculate number of purge cycles we are going to do
   //
   int numcycles = PurgeCalcs::calculateNumCycles(Software_CDS::isPPC(getProcPlateletProdCon()), vintarg, purgePoint);

   //
   //
   //  WBC chamber purge starts with an initial packing stage
   //
   float packTime = cobeConfig.VinChamberPack / nocollectQin;

   //
   //
   //  Each purge cycle is made up of a plasma purge and a pause
   //
   float QplasmaPurge       = cobeConfig.QplasmaPurgeStd;
   float VplasmaPurgeCycle1 = cobeConfig.VplasmaPurgeCycle1Std;
   float VplasmaPurgeCycle2 = cobeConfig.VplasmaPurgeCycle2Std;
   float VinClearChannel    = cobeConfig.VinClearChannelStd;
   if (Software_CDS::isPPC(getProcPlateletProdCon()))
   {
      QplasmaPurge       = cobeConfig.QplasmaPurgeHigh;
      VplasmaPurgeCycle1 = cobeConfig.VplasmaPurgeCycle1High;
      VplasmaPurgeCycle2 = cobeConfig.VplasmaPurgeCycle2High;
      VinClearChannel    = cobeConfig.VinClearChannelHigh;
   }

   float plasmaPurgeTime  = VplasmaPurgeCycle1 / QplasmaPurge + (numcycles - 1) * (VplasmaPurgeCycle2 / QplasmaPurge);
   float pausePurgeTime   = (float)numcycles * VinClearChannel / nocollectQin;
   float chamberPurgeTime = packTime + plasmaPurgeTime + pausePurgeTime;

   //
   //
   //  While all this is going on there are a number of return cycles.  Note we do not include
   //  clearline or settlechannel since the return cycle runs concurrently (with recirc) with the
   //  required volume accruals.  This is just an estimate to make the time "close" since where
   //  the return cycles hit is indeterminate for standards.
   //
   float totalVinAccrued = (float)numcycles * VinClearChannel + cobeConfig.VinChamberPack;
   float returncycles    = (totalVinAccrued / cobeConfig.ReservoirVolume); // since it's an estimate okay to have partial cycles
   float returnTime      = returncycles * (cobeConfig.ReservoirVolume / substate[0].Qrinst);

   // Ramp up / Ramp down RPM dependent purge time
   float prevStartVin = 0.0f;
   float rampUpDnTime = 0.0f;
   for (int subndx = 0; subndx<getNumOfSubstates(); subndx++)
   {
      if ((substate[subndx].T > 0.0f) && (substate[subndx].EndVin  >= purgePoint && prevStartVin < purgePoint ) )
      {
         rampUpDnTime = getWBCRmpUpRmpDnPurgeTime(substate[subndx].Rpm, cobeConfig) / 60.0f;
         break;
      }
      prevStartVin = substate[subndx].EndVin;
   }

   //
   //
   //  Total time per purge is the sum of the recovery substate times plus time for return cycles
   //
   return (chamberPurgeTime + clearLineTime + settleChannelTime + returnTime + rampUpDnTime);
}

#ifdef __WIN32__
void procedureDetailPltPlsRBC::ResetProcDet ()
{

//	if(_procState) {
   _procState.Reset(SubstateMap::alg(_procState.SubState()) );
//	}

   memset(&substate, 0, sizeof(substate));

   _PredVp        = 0.0f;
   _PredVc        = 0.0f;
   _PredVac       = 0.0f;
   _PredVrep      = 0.0f;
   _PredTp        = 0.0f;
   _PredRBCCrit   = 0.0f;
   _PredRBCVolume = 0.0f;
   _PredRBCDose   = 0.0f;

   SetPIRRequired(false);


}
#endif

/* FORMAT HASH 3ad7ef6e41f9afcc58d840f7f265aee0 */
