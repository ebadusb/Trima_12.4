/*
 *
 * TITLE:      procdet.h
 *             Header file for Focussed system prediction functions.
 */

#include <cmath>
#include <vxworks.h>
#include "procdet.h"

#include "limitprimeratio.h"
#include "software_cds.h"
#include "scoped_cds_lock.hpp"
#include "mathdefs.hpp"


#ifndef __WIN32__
#include "cfg_vars.h"
#include "procruntargets_cds.h"
#endif


procedureDetail::procedureDetail()
   : _PredVc(0.0f),
     _PredVp(0.0f),
     _PredRBCVolume(0.0f),
     _PredVac(0.0f),
     _PredVrep(0.0f),
     _PredTp(0.0f),
     _inDisplayList(0),
     _numOfSubstates(0),
     _recommendedCassette(cassette::UNKNOWN),
     _extracorporealVolume(0.0f),
     _primeDuration(0.0f),
     _vinPrime(0.0f),
     _residualRBCVolume(0.0f),
     _numOfTimes (0),
     _convergenceCtr (0),
     _ProcNumber (0),
     _Designator (0),
     _ProcAMAPorMC (0),
     _ProcBloodType (0),
     _ProcCollectPlasma(false),
     _ProcCollectVolFixed (false),
     _ProcFixedCollectVol(0.0f),
     _ProcFixedPlasmaVol(0.0f),
     _ProcPlasmaProductVol(0.0f),
     _ProcPlasmaVolFixed (0),
     _ProcPlateletProdCon(0.0f),
     _ProcRBCDose(0.0f),
     _ProcRBCHct(0.0f),
     _ProcRBCVolume(0.0f),
     _ProcTime(0.0f),
     _DesiredYield(0.0f),
     _ProcPtfIsSplit(false),
     _mssPltVolume(0),
     _mssPltPctCarryover(0),
     _mssRbcVolume (0),
     _mssOriginalPltYield(0.0f),
     _mssOriginalPltVolume(0.0f),
     _mssPlt(false),
     _mssRbc(false),
     _CalcedRBCCrit(0.0f),
     _CannotBeDisqualifiedAsDuplicate(false),
     _PlasmaLimitedByHct(false),
     _MaxPlasma(0.0f),
     _hypovolemic(0),
     _hypovolemiaDistance(0.0f),
     _HypoMargin(0.0f),
     _HypoMarginMin(0.0f),
     _PlasmaTargetCannotChange (0),
     _MaxACRate(0.0f),
     _PredRBCDose(0.0f),
     _PredRBCCrit(0.0f),
     _status(UNOPTIMIZED),
     _optimized(false),
     _I(0.0f),
     _Tc(0.0f),
     _Ytarg(0.0f),
     _Ec(0.0f),
     _Qinc(0.0f),
     _Ydpur(0.0f),
     _Vintarg(0.0f),
     _Vptarg(0.0f),
     _Vrtarg(0.0f),
     _Qcmr(0.0f),
     _Vpmin(0.0f),
     _Vinpur(0.0f),
     _Ydch(0.0f),
     cobeConfig(CobeConfig::data()),
     _config(Config::Instance()),
     _procState(ProcDataState::Instance()),
     _nocollectFlows(),
     _PltAmap(false),
     _amapPlateletHighestYield(0.0f),
     _amapPlateletSize(AMAP_PLT_NONE),
     _amapPltInvalid(false),
     _AMAP_PLATELETS_ON(false),
     _convergenceTrail()
{

#if CPU==SIMNT
   __MIN_CRIT = 0.55f;
   __MAX_CRIT = 0.80f;
#else   // Set up the min/max RBC crit.
   CCfgVarInfo critRangeLimits(KEY_RBC_CRIT_1);
   __MIN_CRIT = critRangeLimits.Min() / 100.0f;
   __MAX_CRIT = critRangeLimits.Max() / 100.0f;
#endif /* if CPU=SIMNT */
   memset(&substate, 0, sizeof(IntermediateResults) * ALLOCATED_SUBSTATES);
   memset(&_firstReturnNoCollect, 0, sizeof(Predict_CDS::one_substate_info_t));

   //////////////////////////////////////////////////////////////////////////////
   // jph

   if (Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable))
   {
      // DataLog(log_level_predict_debug) << "amapp: AMAP PLT are ON" << endmsg;
      // _stepSize = cobeConfig.YieldStepSize;
      _AMAP_PLATELETS_ON = true;
   }
   else
   {
      // DataLog(log_level_predict_debug) << "amapp: AMAP PLT are OFF" << endmsg;
      _AMAP_PLATELETS_ON = false;
   }
   //////////////////////////////////////////////////////////////////////////////

}

procedureDetail::~procedureDetail()
{}

//
//
//  Note that this copy does not set all data members (on purpose).
//  Some variables like _Predxxxx are calculated later; some like
//  numOfSubstates and recommendedCassette are set explicitly by the
//  derived classes.  Include any variables here that need to propogate
//  between white / black stamp sets, such as configuration values.
//
void procedureDetail::copy (const procedureDetail& orig)
{
   _ProcNumber = orig._ProcNumber;
   _Designator = orig._Designator;
   _status     = orig._status;
   _optimized  = 0;                      // not guaranteed of course
   _extracorporealVolume            = orig._extracorporealVolume;
   _residualRBCVolume               = orig._residualRBCVolume;
   _primeDuration                   = orig._primeDuration;
   _vinPrime                        = orig._vinPrime;
   _DesiredYield                    = orig._DesiredYield;

   _ProcAMAPorMC                    = orig._ProcAMAPorMC;
   _ProcBloodType                   = orig._ProcBloodType;
   _ProcCollectPlasma               = orig._ProcCollectPlasma;
   _ProcCollectVolFixed             = orig._ProcCollectVolFixed;
   _ProcFixedCollectVol             = orig._ProcFixedCollectVol;
   _ProcFixedPlasmaVol              = orig._ProcFixedPlasmaVol;
   _ProcPlasmaProductVol            = orig._ProcPlasmaProductVol;
   _ProcPlasmaProductVol            = orig._ProcPlasmaProductVol;
   _ProcPlasmaVolFixed              = orig._ProcPlasmaVolFixed;
   _ProcPlateletProdCon             = orig._ProcPlateletProdCon;
   _ProcRBCDose                     = orig._ProcRBCDose;
   _ProcRBCHct                      = orig._ProcRBCHct;
   _ProcRBCVolume                   = orig._ProcRBCVolume;
   _ProcPtfIsSplit                  = orig._ProcPtfIsSplit;
   _ProcTime                        = orig._ProcTime;
   _MaxPlasma                       = orig._MaxPlasma;
   _HypoMargin                      = orig._HypoMargin;
   _PlasmaTargetCannotChange        = orig._PlasmaTargetCannotChange;
   _inDisplayList                   = orig._inDisplayList;

   _mssPltVolume                    = orig._mssPltVolume;
   _mssRbcVolume                    = orig._mssRbcVolume;
   _mssPltPctCarryover              = orig._mssPltPctCarryover;
   _mssOriginalPltYield             = orig._mssOriginalPltYield;
   _mssOriginalPltVolume            = orig._mssOriginalPltVolume;

   _mssPlt                          = orig._mssPlt;
   _mssRbc                          = orig._mssRbc;
   _CalcedRBCCrit                   = orig._CalcedRBCCrit;

   _PltAmap                         = orig._PltAmap;
   _amapPlateletHighestYield        = orig._amapPlateletHighestYield;
   _amapPlateletSize                = orig._amapPlateletSize;
   _amapPltInvalid                  = orig._amapPltInvalid;
   _AMAP_PLATELETS_ON               = orig._AMAP_PLATELETS_ON;

   _CannotBeDisqualifiedAsDuplicate = orig._CannotBeDisqualifiedAsDuplicate;
}

float procedureDetail::getMaxQin () const
{
   float maxQin     = 0.0f;
   int   nSubstates = getNumOfSubstates();

   for (int i = 0; i<nSubstates; i++)
   {
      if ((substate[i].T > 0) && (substate[i].Qininstd > maxQin))
         maxQin = substate[i].Qininstd;
   }

   //
   //  IT13203 truncate to int
   //
   maxQin = trunc(maxQin);

   return maxQin;
}


void procedureDetail::copySubstateInfoToCds (substates_t& info)
{
   // DataLog(log_level_predict_info) << "procedureDetail::copySubstateInfoToCds "   << endmsg;

   bool update = false;

   for (int subndx = 0; subndx < getNumOfSubstates(); subndx++)
   {
      info[subndx].QinD         = substate[subndx].Qininstd;
      info[subndx].QinR         = substate[subndx].Qininstr;
      info[subndx].Qac          = substate[subndx].Qacinst;
      info[subndx].Qrp          = substate[subndx].Qrinst;
      info[subndx].QpD          = substate[subndx].Qpinstd;
      info[subndx].QpR          = substate[subndx].Qpinstr;
      info[subndx].QcD          = substate[subndx].Qcinstd;
      info[subndx].QcR          = substate[subndx].Qcinstr;
      info[subndx].DutyCycle    = substate[subndx].D;
      info[subndx].InfusionRate = instantaneousInfusion(subndx);
      info[subndx].AccumVac     = substate[subndx].AccumVac;
      info[subndx].Vacr         = substate[subndx].Vacr;
      info[subndx].AccumVacp    = substate[subndx].AccumVacp;
      info[subndx].AccumVp      = substate[subndx].AccumVp;
      info[subndx].AccumVrep    = substate[subndx].AccumVrep;
      info[subndx].HypoD        = substate[subndx].HypovolemiaDistance;
      info[subndx].Hend         = substate[subndx].Hend;
      info[subndx].Csf          = substate[subndx].Csf;
      info[subndx].Rpm          = substate[subndx].Rpm;
      info[subndx].T            = substate[subndx].T;

      // See below; ITS 13361, 13212, 11234, and 13572.
      if (substate[subndx].T > 0.0f) // valid substates have T>0
      {
         update = true;   // we have found a valid substate

         // If the current procedure's input and return pump speeds have been
         // commanded to zero, shout.
         if ( ( info[subndx].QinD == 0 ) &&
              ( info[subndx].QinR == 0 ) &&
              ( info[subndx].Qrp  == 0 ) )
         {
            DataLog(log_level_predict_info) << "ProcDet::copySubstateInfoToCDS, substate " << subndx
                                            << " has no input or output flow."
                                            << endmsg;
            update = false;
            break;
         }
      }
   }

   //
   //   See ITs 13361, 13212, 11234, and 13572.  There are cases where predict
   //   does not create any substates, generally when predict thinks the
   //   procedure is over (for example, AMAP target repredicted or yield close
   //   to the end of a procedure) or when the procedure is invalid.
   //
   if ( !update )
   {

      // If the procedure is good, then it's complete and we force rinseback...
      if (_status == GOOD_PRODUCT)
      {
         RinsebackRequestMsg procHint(MessageBase::SEND_LOCAL);
         procHint.send(1);
         DataLog(log_level_predict_info) << "Rinseback forced by predict complete." << endmsg;
      }
      // ... otherwise, the current procedure is invalid, and we log that...
      else
         DataLog(log_level_predict_info) << "ProcDet::copySubstateInfoToCDS, procedure looks invalid."
                                         << endmsg;
      // ... and either way, we're done -- exit without writing to CDS
      return;
   }

   // If we get here, the loop has completed and the procedure is OK, so write the
   // substate info to CDS.
   _procState._predictCDS.SubstateInfo.Set(info);
}



void procedureDetail::restoreSubstateInfoFromCds (substates_t& info)
{

   //   DataLog(log_level_predict_info) << " procedureDetail::restoreSubstateInfoFromCds "   << endmsg;
   _procState._predictCDS.SubstateInfo.Get(&info);

   for (int subndx = 0; subndx < getNumOfSubstates(); subndx++)
   {
      substate[subndx].Qininstd            = info[subndx].QinD;
      substate[subndx].Qininstr            = info[subndx].QinR;
      substate[subndx].Qacinst             = info[subndx].Qac;
      substate[subndx].Qrinst              = info[subndx].Qrp;
      substate[subndx].Qpinstd             = info[subndx].QpD;
      substate[subndx].Qpinstr             = info[subndx].QpR;
      substate[subndx].Qcinstd             = info[subndx].QcD;
      substate[subndx].Qcinstr             = info[subndx].QcR;
      substate[subndx].D                   = info[subndx].DutyCycle;
      substate[subndx].AccumVac            = info[subndx].AccumVac;
      substate[subndx].Vacr                = info[subndx].Vacr;
      substate[subndx].AccumVacp           = info[subndx].AccumVacp;
      substate[subndx].AccumVp             = info[subndx].AccumVp;
      substate[subndx].AccumVrep           = info[subndx].AccumVrep;
      substate[subndx].HypovolemiaDistance = info[subndx].HypoD;
      substate[subndx].Hend                = info[subndx].Hend;
      substate[subndx].Csf                 = info[subndx].Csf;
      substate[subndx].Rpm                 = info[subndx].Rpm;


   }
}




void procedureDetail::AccumulateVolumes ()
{
   // DataLog(log_level_predict_info) << " procedureDetail::AccumulateVolumes "   << endmsg;
   int startingsubstate = AlgSubstate(_procState.SubState());
   if (startingsubstate <= 0) startingsubstate = 1;

   //
   //
   //   Conveniently use the [0] substate to avoid -1 constructs in calculations
   //
   substate[0].T         = getPrimeDuration();
   substate[0].AccumTime = getPrimeDuration();
   substate[0].AccumVc   = 0.0f;
   substate[0].AccumVacc = 0.0f;
   substate[0].AccumVp   = 0.0f;
   substate[0].AccumVacp = 0.0f;
   substate[0].AccumVrep = 0.0f;
   substate[0].AccumVac  = getVinPrime() / _config.getACRatio() + cobeConfig.ACPrimeVolume;
   substate[0].EndVin    = getVinPrime();

   if (startingsubstate > 1)
   {
      for (int x = 1; x<startingsubstate; x++)
      {
         substate[x].T                   = 0.0f;
         substate[x].Vc                  = 0.0f;
         substate[x].Vacc                = 0.0f;
         substate[x].Vp                  = 0.0f;
         substate[x].Vacp                = 0.0f;
         substate[x].Vrep                = 0.0f;
         substate[x].Vac                 = 0.0f;
         substate[x].Yield               = 0.0f;
         substate[x].AccumTime           = _procState.ProcRunTime();
         substate[x].AccumVc             = _procState.CollectVolume();
         substate[x].AccumVacc           = _procState.AcInCollect();
         substate[x].AccumVp             = _procState.PlasmaVolume();
         substate[x].AccumVacp           = _procState.AcInPlasma();
         substate[x].AccumVrep           = _procState.ReplacementVolume();
         substate[x].AccumVac            = _procState.Vac();
         substate[x].EndVin              = _procState.Vin();
         substate[x].HypovolemiaDistance = 0.0f;
      }
   }


   //
   //
   //  Calculate accumulated volumes for all substates.  Note zero of volumes
   //  since the equations give indeterminate results for unused substates.
   //
   for (int x = startingsubstate; x < getNumOfSubstates(); x++)
   {
      if ((substate[x].T == 0.0f) || (substate[x].EndVin < substate[x - 1].EndVin))
      {
         substate[x].EndVin = substate[x - 1].EndVin;
      }

      if ((x==startingsubstate) || ((substate[x].EndVin - substate[x - 1].EndVin) > 1.0f))
      {
         substate[x].AccumVc   = substate[x - 1].AccumVc + substate[x].Vc;
         substate[x].AccumVacc = substate[x - 1].AccumVacc + substate[x].Vacc;
         substate[x].AccumVp   = substate[x - 1].AccumVp + substate[x].Vp;
         substate[x].AccumVacp = substate[x - 1].AccumVacp + substate[x].Vacp;
         substate[x].AccumVrep = substate[x - 1].AccumVrep + substate[x].Vrep;
         substate[x].AccumVac  = substate[x - 1].AccumVac + substate[x].Vac;
         substate[x].AccumTime = substate[x - 1].AccumTime + substate[x].T;
      }
      else
      {
         substate[x].AccumVc   = substate[x - 1].AccumVc;
         substate[x].AccumVacc = substate[x - 1].AccumVacc;
         substate[x].AccumVp   = substate[x - 1].AccumVp;
         substate[x].AccumVacp = substate[x - 1].AccumVacp;
         substate[x].AccumVrep = substate[x - 1].AccumVrep;
         substate[x].AccumVac  = substate[x - 1].AccumVac;
         substate[x].AccumTime = substate[x - 1].AccumTime;
         substate[x].EndVin    = substate[x - 1].EndVin;
         substate[x].T         = 0.0f;
      }

   }

   //
   //
   //   Save off predicted volumes.  Note RBC predicted volumes are set during precheck.
   //
   _PredVp   = substate[getNumOfSubstates() - 1].AccumVp;
   _PredVc   = substate[getNumOfSubstates() - 1].AccumVc;
   _PredVac  = substate[getNumOfSubstates() - 1].AccumVac;
   _PredVrep = substate[getNumOfSubstates() - 1].AccumVrep;
   _PredTp   = substate[getNumOfSubstates() - 1].AccumTime;

}

INTERNAL_PREDICT_RESULTS procedureDetail::Optimize ()
{
   //  DataLog(log_level_predict_info) << " procedureDetail::Optimize "   << endmsg;
   _procState._hypovolemia.initialize(getRecommendedCassette());

   // if(!_procState.getOptimizingSelectedProc()) {
   //	_procState.Reset();
   // }

   SetConstants();
   // Proc level preckecks
   setStatus(GeneralPreCheck());
   if (_status != GOOD_PRODUCT)
   {
      return _status;
   }
   // Start optimization
   setnumOfTimes(0);

   setMaxACRate(_config.getACRate());
   float mintime = 40.0f; // in lieu of other information

   if (_procState.ProcRunTime() > (mintime - 5.0f))
      mintime = _procState.ProcRunTime() + 5.0f;

   float lastproceduretime = _PredTp = mintime;

   generatesubstates();
   _convergenceTrail[getnumOfTimes() - 1] = getconvergenceCtr();
   bool converged = (!_procState.getOptimizingSelectedProc()&& (getStatus() != GOOD_PRODUCT));

   while (!converged)
   {
      if (getnumOfTimes() >= MAXITERATIONS)
      {
         if (getconvergenceCtr() == 0)
            setconvergenceCtr(-1);

         break;
      }

      converged = true;

      //
      //
      //  If we're just not going to get there, then what we have is close enough.
      //  Different margins exist since RBC/Pl times can converge toward max with
      //  AMAP plasma due to dynamic crit.
      //
      float maxtime = _config.getMaxProcedureTime() + (getRecommendedCassette() == cassette::RBCPLS ? 5.0f : 2.0f);

      if ((_PredTp > maxtime) && (lastproceduretime > _config.getMaxProcedureTime()) && (getnumOfTimes() > 4))
      {
         setconvergenceCtr(0); // pretend we're optimized
         break;
      }

      //
      //
      //  We can try to exit if this procedure didn't change more
      //  than the maximum time (this is our specification limit).
      //
      float              delta_time             = fabs(_PredTp - lastproceduretime);

      static const float PROC_TIME_CLOSE_ENOUGH = 3.0f;

      if ((delta_time > PROC_TIME_CLOSE_ENOUGH) && (getnumOfTimes() < MAXITERATIONS - 3))
         converged = false;

      //
      //
      //   Allow procedure to override exit conditions
      //
      if (getconvergenceCtr())
         converged = false;

      if (!converged)
      {
         _PredTp = 0.8f * _PredTp + 0.2f * lastproceduretime;
         if (_PredTp > _config.getMaxProcedureTime())
            _PredTp = _config.getMaxProcedureTime() + 1.0f; // +1 to allow early max time disqualification

         lastproceduretime = _PredTp;
         generatesubstates();
         _convergenceTrail[getnumOfTimes() - 1] = getconvergenceCtr();
      }
   }

   CalculateNoCollectFlows();
   CalculateNoCollectFirstReturnFlows();

   setStatus(GeneralPostCheck());

   if (!getProcPlasmaVolFixed() && (!getPlasmaTargetCannotChange()))
   {
      setProcFixedPlasmaVol(_PredVp);
      setPlasmaTargetCannotChange(1);
   }
   return _status;
}

INTERNAL_PREDICT_RESULTS procedureDetail::GeneralPreCheck ()
{
   //  DataLog(log_level_predict_info) << " procedureDetail::preCheckProcedure "   << endmsg;
   unsigned int func_bits = _procState.GetCassetteFunctionBits();

   // if this is non-MSS PLT then disqual all MSS PLT process
   bool  procedureIsPas = getMSSPlt();
   bool  procedureIsRas = getMSSRbc();
   float rbcVol         = getProcRBCVolume();

   bool  procedureIsPlt = (getProcPlateletProdCon() > 0.0f);
   bool  procedureIsPls = (getProcPlasmaVolFixed() > 0.0f);
   bool  procedureIsRbc = (rbcVol > 0.0f);

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool procedureExceedsDrbcThreshold = _config.isPromptedDrbcSplit(getProcedureNumber());


   if (func_bits) // if funct bits exits (not 0)
   {
      bool setIsPas  = CatalogChecker::checkBit(func_bits, CatalogChecker::MSS_PLT);
      bool setIsRas  = CatalogChecker::checkBit(func_bits, CatalogChecker::MSS_RBC);
      bool setIsDrbc = CatalogChecker::checkBit(func_bits, CatalogChecker::DRBC_BAG);

      bool setIsPlt  = CatalogChecker::checkBit(func_bits, CatalogChecker::PLATELET_BAG);
      bool setIsPls  = CatalogChecker::checkBit(func_bits, CatalogChecker::PLASMA_BAG);
      bool setIsRbc  = setIsDrbc || CatalogChecker::checkBit(func_bits, CatalogChecker::RBC_BAG);

      DataLog (log_level_predict_info)
         << "PreCheck Compatability Check --> Cassette(PAS=" << setIsPas
         << ";RAS=" << setIsRas
         << "); Procedure(PAS=" << procedureIsPas
         << ";RAS=" << procedureIsRas
         << ";RBCVOL=" << rbcVol << ");" << endmsg;

      if (procedureExceedsDrbcThreshold && (!setIsDrbc))
      {
         DataLog(log_level_predict_info) << "Loaded set is not DRBC; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      // If set is non-PAS then disqual PAS procedures
      if ((!setIsPas) && procedureIsPas)
      {
         DataLog(log_level_predict_info) << "Loaded set is not PAS; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      // If set is non-RAS then disqual RAS procedures
      if ((!setIsRas) && procedureIsRas)
      {
         DataLog(log_level_predict_info) << "Loaded set is not RAS; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      // If set is RAS then disqual procedures that collect non-RAS RBC products.
      // (RAS implies press through filtration, and we can't perform an unfiltered
      // procedure when the set has an inline filter, right?
      if (setIsRas && procedureIsRbc && (!procedureIsRas) )
      {
         DataLog(log_level_predict_info) << "Loaded set is RAS; Procedure is not. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      if ((!setIsPlt) && procedureIsPlt)
      {
         DataLog(log_level_predict_info) << "Loaded set is not PLT; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      if ((!setIsPls) && procedureIsPls)
      {
         DataLog(log_level_predict_info) << "Loaded set is not PLASMA; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }

      if ((!setIsRbc) && procedureIsRbc)
      {
         DataLog(log_level_predict_info) << "Loaded set is not RBC; Procedure is. Disqualify procedure." << endmsg;
         return CASSETTE_TYPE_MISMATCH;
      }
   }

   if (checkForTraliFiltering()) return TRALI_CRITERIA_NOT_MET;

   //
   // make sure donor vitals are entered.
   //
   if (_procState.Tbv() == 0 || _procState.PreCount() == 0 || _procState.Hct() == 0)
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber() << ": Invalid system state for optimization - missing donor info" << endmsg;
      return UNOPTIMIZED;
   }

   //
   //
   // the current designator must match the procedure designator
   //
   /*if ((!_procState.getOptimizingSelectedProc()) && (_currentDesignator != getDesignator())) {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber() << ": designator inactive" << endmsg;
      return INCORRECT_DESIGNATOR;
    }*/





   //
   // must specify product
   //
   if (!getProcCollectPlasma()
       && getProcFixedCollectVol() == 0
       && getProcRBCDose() == 0)
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber() << ": inactive optimization request - no specified product" << endmsg;
      return PRODUCT_ACHIEVED;
   }

   //
   //
   //  Redundant donor information check per IT6119
   //
   SDonorVitals vitals;
   vitals.fDonorHct         = _procState.InitialHct() * 100.0f;
   vitals.fDonorWeight      = _procState.Weight();
   vitals.cDonorWeightUnits = _config.getWeightUnits();
   CDonor donorchk(vitals);
   if ((!donorchk.ValidHct()) || (!donorchk.ValidWeight()))
   {
      return DONOR_INFO_OUT_OF_RANGE;
   }

   //
   //
   //  If this product collects platelets then we cannot have exceeded the
   //  desired yield if plasma is also required.
   //
   if ((!_procState.getOptimizingSelectedProc()) &&
       (getDesiredYield() > 0.0f) && (getProcCollectPlasma()) &&
       ((getDesiredYield() + 0.05f) < _procState.Yield()))
   {
      return INVALID_PRODUCT_COMBINATION;
   }

   float maxpl = _procState._cassette.maxPlasma(getRecommendedCassette(),
                                                getProcFixedCollectVol(),
                                                getProcRBCDose());
   if ((!getProcPlasmaVolFixed()) && (maxpl > _config.getAMAPMaxVolume())) maxpl = _config.getAMAPMaxVolume();
   setMaxPlasma(maxpl);

   //
   // default temp plasma volume to 0.0
   //
   float plasma_volume = 0.0;

   // Are we collecting any plasma???
   if ( getProcCollectPlasma() )
   {
      // Is the Volume a fixed volume
      if ( getPlasmaTargetCannotChange() )
      {
         plasma_volume = getProcFixedPlasmaVol();
      }
      // Is the volume maximum Concurrent or AMAP
      else
      {
         plasma_volume = _config.getAMAPVolume();   // As Much As Possible
      }
   }

#ifndef __PREDICTROBUSTNESS__
   if (!_procState._cassette.compatible(getRecommendedCassette(),
                                        getProcFixedCollectVol(),
                                        plasma_volume,
                                        getProcRBCDose() ))
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << ": cassette type " << getRecommendedCassette()
                                       << " incompatible with requested volumes of "
                                       << getProcFixedCollectVol() << " "
                                       << plasma_volume << " "
                                       << getProcRBCDose()
                                       << endmsg;
      return CASSETTE_TYPE_MISMATCH;
   }

   maxpl = _procState._cassette.maxPlasma(getRecommendedCassette(),
                                          getProcFixedCollectVol(),
                                          getProcRBCDose() );
   if ( plasma_volume > maxpl )
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << ": requested plasma volume "
                                       << plasma_volume
                                       << " exceeds cassette capacity "
                                       << maxpl
                                       << " Platelets "
                                       << getProcFixedCollectVol()
                                       << " RBC Dose: "
                                       << getProcRBCDose()
                                       << endmsg;

      return CASSETTE_TYPE_MISMATCH;
   }


   if (procedureExceedsDrbcThreshold && procedureIsPlt)
   {
      DataLog(log_level_predict_info) << "Procedure is both DRBC and PLT. No sets are compatible." << endmsg;
      return CASSETTE_TYPE_MISMATCH;
   }
#endif

   //
   //
   //  Check postcount on desired target
   //
   if (!_procState._postcount.postcountOK(_procState.PreCount(),
                                          _procState.Tbv(),
                                          getDesiredYield() * 1.0e11f,
                                          _config.getMinPostCount() / 1000.0f))
   {
      const float endingPostCount = _procState._postcount.postcount();
      const float minPostCount    = _config.getMinPostCount() / 1000.0f;
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " preCheck BAD_POSTCOUNT "
                                       << ": Ending postcount "
                                       << endingPostCount
                                       << "  Min postcount "
                                       << minPostCount
                                       << endmsg;

      return BAD_POSTCOUNT;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////
   // Check the postCrit vs. the Targeted Dose (non-rsf modified, what the admin configured)
   //
   // JPH -- note: i believe this test to be superfluious, but it was here when i got here and it doesnt hurt anything
   //                       the other post crit check in this function will always catch any problem
   if (!_procState._posthct.postHctOK(_procState.InitialHct(),
                                      _procState.Tbv(),
                                      getProcRBCDose(), // this is the unmodified Dose no RSF applied!
                                      getResidualRBCVolume(),
                                      _procState.SampleVolume(),
                                      _config.getMinPostHct() / 100.0f))
   {
      DataLog(log_level_predict_info) << "procedure " << getProcedureNumber()
                                      << " preCheck BAD_POSTHCT "
                                      << ": control posthct "
                                      << _procState._posthct.postHct(_procState.InitialHct(), _procState.Tbv(), getProcRBCDose(), getResidualRBCVolume(),
                                     _procState.SampleVolume())
                                      << "  Min posthct "
                                      << _config.getMinPostHct() / 100.0f
                                      << endmsg;
      return BAD_POSTHCT;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////
   //
   //
   //   Check hypovolemia with zero volumes to ensure safety will not shut us down
   //   at the start of the procedure.
   //
   float volToHypovolemia = _procState._hypovolemia.VolumeRemainingToHypovolemia(0, // never assume current proc
                                                                                 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
   if ( volToHypovolemia <= getHypoMargin())
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " preCheck HYPOVOLEMIA_EXCEEDED "
                                       << ": Volume remaining to Hypovolemia "
                                       << volToHypovolemia
                                       << endmsg;
      return HYPOVOLEMIA_EXCEEDED;
   }

   //
   //
   //   To make the flow equations work, we optimize to the RBC dose needed.
   //   In cases where the run has already started, we may need to fudge the
   //   crit in order to get as close as we can to the proper dose.  Convention is
   //   "ProcRBC" variables describe product as configured (with RSF) and "PredRBC"
   //   variables describe product as collected.
   //
   if (getProcRBCDose() > 0.0f)   // this product needs RBCs

   {
      float desireddose = getProcRBCVolume() * (getProcRBCHct() / 100.0f);

      //
      //  Per IT4597 we attempt to get the dose desired for the product.  In case
      //  of a midrun product change we fudge the crit as much as possible to
      //  try and get the desired dose.  Note that for postcrit this effectively
      //  applies the RSF to the dose as opposed to the volume; we need to do
      //  this since the safety side will calculate postcrit from volume and the
      //  collection crit.
      //
      //   We do not attempt to set the crit if we are near volume limits
      //   already.  This disqualification should be rare since it can only happen
      //   at the very end of a procedure.  This check is really done to avoid
      //   divide by zero problems in the below code.
      //
      float crit = getProcRBCHct() / 100.0f;

      if (_procState.RbcVolume() < (getProcRBCVolume() - 0.1f))
      {
         crit = (desireddose - _procState.RbcDose()) / (getProcRBCVolume() - _procState.RbcVolume());
      }

      // set the calculated crit here to act like safety's value in the instantaneousInfusion
      // calculations for Black stamp PTF!
      setRBCCalculatedCrit(_procState.Hrbc() );


      if (crit < __MIN_CRIT) crit = __MIN_CRIT;
      if (crit > __MAX_CRIT) crit = __MAX_CRIT;

      setPredRBCCrit(crit * 100.0f);
      setPredRBCDose(desireddose);  // this is the modified Dose with RSF applied via getProcRBCVolume()!
      setPredRBCVolume(_procState.RbcVolume() + (desireddose - _procState.RbcDose()) / crit);

      DataLog(log_level_predict_debug) << "Recalc Dose and Volume "
                                       << ": _procState.RbcVolume() =" << _procState.RbcVolume()
                                       << " desireddose =" << desireddose
                                       << " _procState.RbcDose() =" << _procState.RbcDose()
                                       << " crit =" << crit
                                       << " Calculated Crit (for IR) =" <<  getRBCCalculatedCrit()
                                       << " setPredRBCVolume to =" << getPredRBCVolume()
                                       << endmsg;


   }
   else
   {
      setPredRBCCrit(80.0f); // to avoid /0 throughout
      setPredRBCDose(0.0f);
      setPredRBCVolume(0.0f) ;
   }

   _Vrtarg = getPredRBCVolume();

   /////////////////////////////////////////////////////////////////////////////////////////////////////
   //
   //
   //  Check postcrit on the actual collection.  RSF must be backed out since it should not effect postHCT(IT 8232)
   //
   //
   if (!_procState._posthct.postHctOK(_procState.InitialHct(),
                                      _procState.Tbv(),
                                      getPredRBCDose() * _config.getRSF(),
                                      getResidualRBCVolume(),
                                      _procState.SampleVolume(),
                                      _config.getMinPostHct() / 100.0f))
   {
      DataLog(log_level_predict_debug) << "BAD_POSTHCT procedure " << getProcedureNumber()
                                       << ": control posthct "
                                       << _procState._posthct.postHct(_procState.InitialHct(), _procState.Tbv(),
                                     getPredRBCDose() * _config.getRSF(), getResidualRBCVolume(), _procState.SampleVolume())
                                       << "  Min posthct "
                                       << _config.getMinPostHct() / 100.0f
                                       << endmsg;

      return BAD_POSTHCT;
   }

   /*
   // leave my debug here for future problems please!
   DataLog(log_level_predict_info) << " JPH procedure " << getProcedureNumber()
           << " preCheck  "
           << ": control planned posthct ="   << _procState._posthct.postHct(getInitialDonorHct(), _procState.Tbv(), getProcRBCDose(), getResidualRBCVolume(), getSampleVolume())
           << ": control actual posthct ="    << _procState._posthct.postHct(getInitialDonorHct(), _procState.Tbv(), getPredRBCDose() * getRSF(), getResidualRBCVolume(), getSampleVolume())
           << ": getInitialDonorHct="         << getInitialDonorHct()
           << ": _procState.Tbv() ="                      <<  _procState.Tbv()
           << ": RSF ="                      <<  getRSF()
           << endmsg;

   DataLog(log_level_predict_info) << " JPH procedure (cont.)"  << getProcedureNumber()
           << " preCheck  "
           << ": getProcRBCDose() ="       <<  getProcRBCDose()
           << ": getPredRBCDose() * rsf =" <<  getPredRBCDose() * getRSF()
           << ": getResidualRBCVolume() =" <<  getResidualRBCVolume()
           << endmsg;
   */
   /////////////////////////////////////////////////////////////////////////////////////////////////////////


   //// check blood type
   if (_procState.BloodType() == BT_UNKNOWN)
   {
      if (getProcBloodType() != BT_ALL)
      {
         DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                          << ": desired blood type of " << getProcBloodType()
                                          << " does not match " << _procState.BloodType()
                                          << endmsg;
         return BLOOD_TYPE_MISMATCH;
      }
   }
   else
   {
      if (!(getProcBloodType() & _procState.BloodType()))
      {
         DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                          << ": desired blood type of " << getProcBloodType()
                                          << " does not match " << _procState.BloodType()
                                          << endmsg;

         return BLOOD_TYPE_MISMATCH;
      }
   }


   // Check maximum procedure time
   //
   //   The current procedure can use the maximum of the times
   //   IT6792: always limit to global max procedure time
   //
   if ( _procState.getOptimizingSelectedProc())
   {
      // IT 11860; we always need to consider the maxDonorTime.
      // _procState.setMaxTime(MIN(_config.getMaxProcedureTime(), _procState._predictCDS.maxDonorTime.Get()));

      float maxProcTime = MIN(_config.getMaxProcedureTime(), _procState._predictCDS.maxDonorTime.Get());

      // if it's a AMAPP procedure and DonorTimeMsg message is not received, Config Proc time needs to be counted
      if ( (!_procState.getRcvDonorTimeMsg()) && _AMAP_PLATELETS_ON && (_amapPlateletSize > AMAP_PLT_NONE) )
      {
         maxProcTime = MIN(maxProcTime, getProcTime());
      }

      _procState.setMaxTime(maxProcTime);
   }
   else
   {
      //   Other procedures use the minimum of all the options
      _procState.setMaxTime(MIN(MIN(_procState._predictCDS.maxDonorTime.Get(),
                                    (const int)getProcTime()), _config.getMaxProcedureTime() ) );
   }


   //// acceptable HCT
   if (getProcCollectPlasma())
   {
      if (!PlasmaHctOK())
      {
         return BAD_HCT_WITH_PLASMA;
      }
   }

   //
   //
   //  No sense in optimizing if we have already collected what was requested.
   //  We always try AMAP plasma since it's likely that more can be collected.
   //
   if ((!_procState.getOptimizingSelectedProc()) &&
       (_procState.Yield() >= getDesiredYield()) &&
       (getProcAMAPorMC() >= 0.0f) && (_procState.PlasmaVolume() >= plasma_volume) &&
       ( _procState.RbcVolume()  >= getPredRBCVolume()))
   {
      return PRODUCT_ACHIEVED;
   }

   //
   //
   //  Special disqualification: AMAP or MC selected with bogus configuration
   //
   if (getProcCollectPlasma() && !getPlasmaTargetCannotChange())
   // if(getProcAMAPorMC())
   {
      if (_config.getAMAPVolume() >= _config.getAMAPMaxVolume())
      {
         DataLog(log_level_predict_debug) << "Bogus configuration" << endmsg;
         return PLASMA_NOT_ACHIEVED;
      }
   }

   // check if we should show RBC procedures based on CPS alarms
   // IT8160.
   // IT 10286 - Moved criteria from rbcpls to procdet
   if (   !_procState.PtfCpsTestPassed()
          && _procState.FirstDrawComplete()
          && (getPredRBCDose() > 0.0f)
          && getMSSRbc())
   {
      return CPS_PRESSURE_PRECLUDES_RBCS;
   }

   //
   //
   //  Generic checks all pass - do procedure-specific checks
   //
   return PreCheckProcedure();
}

INTERNAL_PREDICT_RESULTS procedureDetail::GeneralPostCheck ()
{
   // DataLog(log_level_predict_info) << " procedureDetail::postCheckProcedure "   << endmsg;

   float residualRBCVolume = getResidualRBCVolume();

   DataLog(log_level_predict_debug) << "  Predicted RBC residual volume = " << residualRBCVolume << " ml" << endmsg;

   //
   //
   //   The optimization will return the various targets.  In cases where
   //   the current value is already larger than the target make sure
   //   to return the current value instead.
   //
   _Ytarg = getDesiredYield(); // force match
   if (NoMorePlatelets())
   {
      _Ytarg = _procState.Yield();
   }
   if (_Ytarg < _procState.Yield()) _Ytarg = _procState.Yield(); // IT6772

   if (getPredRBCVolume() < _procState.RbcVolume() )
   {
      setPredRBCVolume(_procState.RbcVolume() );
   }
   if (getPredRBCDose() < _procState.RbcDose())
   {
      setPredRBCDose(_procState.RbcDose());
   }

   //
   //
   //  Plasma collection is finished if RBC collection has started
   //

   if ((AlgSubstate(_procState.SubState()) == RBCState()) || (_PredVp < _procState.PlasmaVolume()))
   {
      _PredVp = _procState.PlasmaVolume();
   }

   //
   //
   //   Specifically cut off plasma collection if the crit is above max.  This is
   //   to address cases where PFR occurs during a "no procedure available" condition.
   //
   if (getProcCollectPlasma() && (!PlasmaHctOK()))
   {
      _PredVp = _procState.PlasmaVolume();
   }

   //
   //
   //   Perform set-specific checking
   //
   INTERNAL_PREDICT_RESULTS setresult = PostCheckProcedure();
   if (setresult != GOOD_PRODUCT) return setresult;

   //
   //
   //  If we haven't converged return unoptimized regardless of where
   //  we actually got to.
   //
   if (getconvergenceCtr() != 0)
   {
      return UNOPTIMIZED;
   }

   //
   //
   //   Ensure max time constraint is met
   //
   if (_PredTp > (_procState.getMaxTime() + 0.4f))
   {
      return TIME_EXCEEDED;
   }

   ///////////////////////////////////////////////////////////////////////////////////
   //
   //
   //  Check postcrit on the actual collection.  RSF must be backed out since it should not effect postHCT(IT 8232)
   //
   if (!_procState._posthct.postHctOK(_procState.InitialHct(),
                                      _procState.Tbv(),
                                      getPredRBCDose() * _config.getRSF(),
                                      residualRBCVolume,
                                      _procState.SampleVolume(),
                                      _config.getMinPostHct() / 100.0f))
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " postCheck BAD_POSTHCT"
                                       << ": control posthct "
                                       << _procState._posthct.postHct(_procState.InitialHct(), _procState.Tbv(), getProcRBCDose(), getResidualRBCVolume(),
                                     _procState.SampleVolume())
                                       << "  Min posthct "
                                       << _config.getMinPostHct() / 100.0f
                                       << endmsg;

      return BAD_POSTHCT;
   }


   //  Check postcrit on the actual collection.  RSF must be backed out since it should not effect postHCT(IT 8232)
   //  This one checks for postCrit too High!
   //  Check that post HCT is less then maximum allowed (IT 4227)
   const float postHct =
      _procState._posthct.postHct(_procState.InitialHct(),
                                  _procState.Tbv(),
                                  getPredRBCDose() * _config.getRSF(),
                                  residualRBCVolume,
                                  _procState.SampleVolume()); // > 0.55f ) {

   DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                    << ": control posthct " << postHct << endmsg;

   if (postHct > 0.55f)
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " postCheck BAD_POSTHCT_TYPE2"
                                       << ": control posthct "
                                       << _procState._posthct.postHct(_procState.InitialHct(), _procState.Tbv(), getProcRBCDose(), getResidualRBCVolume(),
                                     _procState.SampleVolume())
                                       << "  Min posthct "
                                       << _config.getMinPostHct() / 100.0f
                                       << endmsg;

      return BAD_POSTHCT_TYPE2;
   }



   /*
   // leave my debug here for future problems please!
   DataLog(log_level_predict_info) << "JPH procedure " << getProcedureNumber()
           << " postCheck   "
           << ": control planned posthct ="   << _procState._posthct.postHct(getInitialDonorHct(), _procState.Tbv(), getProcRBCDose(), getResidualRBCVolume(), getSampleVolume())
           << ": control actual posthct ="    << _procState._posthct.postHct(getInitialDonorHct(), _procState.Tbv(), getPredRBCDose() * getRSF(), getResidualRBCVolume(), getSampleVolume())
           << ": getInitialDonorHct="         << getInitialDonorHct()
           << ": _procState.Tbv() ="                      <<  _procState.Tbv()
           << ": RSF ="                       <<  getRSF()    << endmsg;


   DataLog(log_level_predict_info) << " JPH procedure (cont.)"  << getProcedureNumber()
           << " postCheck   "
           << ": getPredRBCDose() * rsf =" <<  getPredRBCDose() * getRSF()
           << ": getProcRBCDose() ="       <<  getProcRBCDose()
           << ": getResidualRBCVolume() =" <<  getResidualRBCVolume()
           << endmsg;
   */
   ///////////////////////////////////////////////////////////////////////////////////////


   //
   //
   //   Check for VBP processed during RBC collection limit (IT5626).
   //
   if (getPredRBCDose() > 0.0f)
   {
      float VBPDuringRBC = estimateVbpForRbcCollection();
      //
      //
      //   Account for the fact that safety does not account for recirculation volume
      //
      // removed because of IT 5642, which is the real fix for safety
      // VBPDuringRBC /= (1.0f-cobeConfig.RecirculateSnDrawVol/substate[RBCState()].Vsvn);

      //
      //
      //   Mimic of safety side equation, including 25ml difference in TBV which is
      //   the max margin between control and safety
      //


      static const float MIN_ALLOWABLE_POST_CRIT = 0.28f;
//
//       float TBV_mls = _procState.Tbv() - 25.0f;
//       float WBProcessedRBCLimit = TBV_mls
//                             - ( (TBV_mls * MIN_ALLOWABLE_POST_CRIT) + 53.0f)
//                               / (_procState.InitialHct())
//                             - 20.0f;
//
//       if (_procState.getOptimizingSelectedProc()) DataLog(log_level_predict_debug)
//                            << "procedure " << getProcedureNumber()
//                            << ": calc RBC WB VBP=" << VBPDuringRBC
//                            << ", RBC WB VBP limit=" << WBProcessedRBCLimit
//                            << endmsg;

      const float acRatioEstimate = 15.0f;

      // using ECV for white stamp sets since it is the more conservative approach
      const float RBCs = ( (_procState.InitialHct() - MIN_ALLOWABLE_POST_CRIT) * _procState.Tbv())
                         - ( ((cobeConfig.PltPlsRBCExtracorporealVolume - 55.0f) * (1.0f - 1.0f / acRatioEstimate)) + 20.0f) * _procState.InitialHct();


      const float efficiencyConst = 0.2277f * (1 + abs((_procState.InitialHct() - 0.55f) / 0.55f));


      /* call it the postcrit factor x is simpler for the eqn. below*/
      const float x                   = -1.0f * efficiencyConst * RBCs / (_procState.Tbv() * _procState.InitialHct());

      float       WBProcessedRBCLimit = ((x - pow(x, 2) + 3.0f / 2.0f * pow(x, 3) - 8.0f / 3.0f * pow(x, 4) + 125.0f / 24.0f * pow(x, 5))
                                         / (-1.0f * efficiencyConst / _procState.Tbv()));


      //
      //   Allow 25ml of margin from safety
      //
      if (VBPDuringRBC > (WBProcessedRBCLimit - 25.0f))
      {

         DataLog(log_level_predict_info)
            << "RBC_VBP_EXCEEDED, procedure " << getProcedureNumber()
            << ": calc RBC WB VBP=" << VBPDuringRBC << " is greater than"
            << ", (RBC WB VBP limit=" << WBProcessedRBCLimit << ") -25.0"
            << endmsg;

         return RBC_VBP_EXCEEDED;
      }
   }


   //
   //
   //  Check postcount on predicted yield
   //
   if (!_procState._postcount.postcountOK(_procState.PreCount(),
                                          _procState.Tbv(),
                                          getDesiredYield() * 1.0e11f,
                                          _config.getMinPostCount() / 1000.0f))
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " postCheck BAD_POSTCOUNT"
                                       << ": Ending postcount "
                                       << _procState._postcount.postcount()
                                       << "  Min postcount "
                                       << _config.getMinPostCount() / 1000.0f
                                       << endmsg;

      return BAD_POSTCOUNT;
   }

   //
   //
   //  Check to ensure product has not already been collected
   //
   if (!_procState.getOptimizingSelectedProc())
   {
      int vinachieved = ((_Ytarg == 0.0f) || (_procState.Vin() >= _Vintarg));
      if ((vinachieved || NoMorePlatelets()) &&
          (_procState.PlasmaVolume() >= _PredVp) &&
          (_procState.RbcVolume() >= getPredRBCVolume()))
      {
         return PRODUCT_ACHIEVED;
      }
   }


   //
   //
   //   Check hypovolemia.  The current procedure shouldn't be disqualified until
   //   as late as possible (the hypovolemia routine has margin built in already).

   float TotalVr  = 0.0f;

   float TotalVac = 0.0f;
   if (doingPTFDouble())
   {
      TotalVr  = substate[RBCDoubleState()].Vr  ;
      TotalVac = substate[RBCDoubleState()].Vacr;
   }
   else
   {
      TotalVr  = substate[RBCState()].Vr;
      TotalVac = substate[RBCState()].Vacr;
   }

   hypovolemiaDistance(_procState._hypovolemia.VolumeRemainingToHypovolemia(_procState.getOptimizingSelectedProc(),
                                                                            _procState.VAccumOverdraw(),
                                                                            _PredVc, substate[getNumOfSubstates() - 1].AccumVacc,
                                                                            _PredVp, substate[getNumOfSubstates() - 1].AccumVacp,
                                                                            TotalVr, TotalVac,
                                                                            _PredVrep + _procState.VSalineBolus(), _PredVac));
   //
   //
   //  For this specific check, make use of the margin built in to the hypovolemia alarm.
   //  Rinseback is forced at -5ml so we call predict "good" to -2ml.  Note we don't want
   //  to change the margins in predict because we still want to aim for -0.0; the additional
   //  margin is just for qualification.
   //
   float mindistance =  getHypoMargin() - 2.0f;

   //
   //
   //  IT13632 use equivalent disqualification criteria for AMAP plasma (predict to hypo limit) vs.
   //  laddered procedures (predict to completion).  We apply this criteria across the board
   //  to plasma-only procedures to make sure they are all consistent.  Since AMAP already predicts
   //  to the hypovolemia limit this really has no impact on that procedure type.
   //
   bool plasmaonly = ((_PredVc == 0.0f) && (_PredVp > 0.0f) && (TotalVr == 0.0f));
   if (plasmaonly)
   {
      mindistance = getHypoMargin();
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber() << " postcheck hypovolemia limit enforced at " << mindistance << endmsg;
   }

   hypovolemic(hypovolemiaDistance() <= mindistance);

   if (hypovolemic())
   {
      DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
                                       << " postCheck HYPOVOLEMIA_EXCEEDED "
                                       << ": Volume remaining to Hypovolemia "
                                       << hypovolemiaDistance()
                                       << " min distance "
                                       << mindistance
                                       << endmsg;

      return HYPOVOLEMIA_EXCEEDED;
   }



//    if (_AMAP_PLATELETS_ON)
//    {
//
//       if (getPltAmap())
//       {
//
//          if ( hypovolemiaDistance() <
//              (cobeConfig.HypovolemiaAdjust + cobeConfig.HypovolemiaAMAPPLTMargin) )
//          {
//               hypovolemic(1);
//
//               DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
//               << " postCheck amapp: HYPOVOLEMIA_EXCEEDED "
//               << ": Volume remaining to Hypovolemia "
//               << hypovolemiaDistance()
//               << " min distance "
//               << (cobeConfig.HypovolemiaAdjust + cobeConfig.HypovolemiaAMAPPLTMargin)
//               << endmsg;
//
//               return HYPOVOLEMIA_EXCEEDED;
//
//          }  else
//          {
//
//
//              DataLog(log_level_predict_debug) << "procedure " << getProcedureNumber()
//              << " postCheck amapp: HYPOVOLEMIA OK "
//              << ": Volume remaining to Hypovolemia "
//              << hypovolemiaDistance()
//              << " min distance "
//              << (cobeConfig.HypovolemiaAdjust + cobeConfig.HypovolemiaAMAPPLTMargin)
//              << endmsg;
//
//
//              hypovolemic(0);
//          }
//       }
//    }



   //
   //
   //  Check to ensure we have reached our required yield
   //
   static const float yieldTolerance = 0.049f; // display resolution
   if ((!NoMorePlatelets()) && (_Ytarg < (getDesiredYield() - yieldTolerance)))
   {
      return YIELD_NOT_ACHIEVED;
   }

   //
   // check to ensure we reached required plasma volume
   //
   if (getProcCollectPlasma()) // plasma collection
   {
      if (_PredVp < (_Vpmin - Config::__plasmaTolerance))
      {
         DataLog(log_level_predict_debug) << "Predicted Vp " << _PredVp << " below target min of "
                                          << _Vpmin << ".  Tolerance is " << Config::__plasmaTolerance << endmsg;
         return PLASMA_NOT_ACHIEVED;
      }

      //
      //
      //  For procedures that have plasma targeted without AC, ensure that the correct amount
      //  of plasma has been collected.
      //
      if (getProcPlasmaProductVol() > 0.0f)
      {
         const float plasma_without_ac = getPredVp() - substate[getNumOfSubstates() - 1].AccumVacp;
         if (plasma_without_ac < (_Vpmin - Config::__plasmaTolerance))
         {
            DataLog(log_level_predict_info) << "procedure " << getProcedureNumber()
                                            << " requested plasma volume without AC of " << getProcPlasmaProductVol() << " mL but only achieved "
                                            << plasma_without_ac << " (" << getPredVp() << " - " << substate[getNumOfSubstates() - 1].AccumVacp << ")"
                                            << endmsg;
            return PLASMA_NOT_ACHIEVED;
         }
      }
//      if (getPlasmaTargetCannotChange()) { // fixed or AMAP after selection
//         if (fabs(_PredVp - _Vptarg) > __plasmaTolerance) {
//            return PLASMA_NOT_ACHIEVED;
//         }
//      }
   }

   //
   //
   //   Per IT4901, any substate that has a instantaneous Qin that is less
   //   than our 20ml/min spec should be disqualified.
   //
   for (int subndx = 0; subndx < getNumOfSubstates(); subndx++)
   {
      if (substate[subndx].T > 0.0f)
      {
         if (substate[subndx].Qininstd < cobeConfig.QinLimitMin)
         {
            DataLog(log_level_predict_debug) << "Qin " << substate[subndx].Qininstd
                                             << " below min spec of " << cobeConfig.QinLimitMin << endmsg;

            return QIN_BELOW_MINIMUM_SPEC;
         }

         if (substate[subndx].Qrinst < cobeConfig.QrpLimitMin)
            return QRP_BELOW_MINIMUM_SPEC;
      }
   }

   //
   //
   //  If the product didn't start as good we are not allowed to change it
   //  to a good status here.
   //
   if (getStatus() != GOOD_PRODUCT) return getStatus();

   return GOOD_PRODUCT;
}

void procedureDetail::CalculateNoCollectFlows ()
{

   //  DataLog(log_level_predict_info) << " procedureDetail::CalculateNoCollectFlows "   << endmsg;
   IntermediateResults& substateInfo = substate[0];

   float                ratio        = _config.getACRatio();
   if (AlgSubstate(_procState.SubState()) < 0) ratio = LimitPrimeRatio(ratio, _procState.Hct());



   //
   //
   //   Calculate cap values (limit Qrp to first cycle rate per IT5070)
   //
   float qincap = MIN(_procState.AdjQinCap(), cobeConfig.TbvQinMaxCoefficient * _config.getTbvQinmaxScaleConst() * _procState.Tbv() );
   qincap = MIN(qincap, cobeConfig.QinStartup);

   float qrpcap   = cobeConfig.QrpStartup; // _procState.AdjQrpCap();

   float qntbvmax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * _procState.Tbv();
   float qncap    = MIN(_procState.AdjQrpCap(), qntbvmax);
   qncap = MAX(qncap, cobeConfig.QneedleLimitMin); // IT9693
   float acRate   = MIN(_I, getMaxACRate());

   _nocollectFlows.Initialize(_procState.Hct(),
                              _procState.PreCount(),
                              _procState.Tbv(),
                              acRate,
                              ratio,
                              qincap,
                              qrpcap,
                              qncap);

   _nocollectFlows.calculate(&substateInfo);

   //
   //
   //   Save off instantaneous flow details
   //
   substateInfo.T                   = 0.0f; // no real time spent in this state
   substateInfo.EndVin              = 0.0f;
   substateInfo.Vac                 = 0.0f;
   substateInfo.Td                  = substateInfo.Vsvn / substateInfo.Qininstd;
   substateInfo.Qcc                 = 0.0f;
   substateInfo.Qpc                 = 0.0f;
   substateInfo.Qch                 = 0.0f;
   substateInfo.HypovolemiaDistance = 0.0f;
   substateInfo.Qcinstd             = substateInfo.Qcc;
   substateInfo.Qcinstr             = substateInfo.Qcc;
   substateInfo.Qpinstd             = substateInfo.Qpc;
   substateInfo.Qpinstr             = substateInfo.Qpc;
   substateInfo.Csf                 = 1.0f; // By definition
   substateInfo.Rpm                 = cobeConfig.RPMCollect;

   //
   //
   //  Override Qcinstr for saline bolus flows per convention
   //
   if (getRecommendedCassette() == cassette::RBCPLS)
   {
      float bolusFlow = _procState.Tbv() * _I * _config.getACRatio() / 1000.0f;
      if ( bolusFlow > 60.0f ) bolusFlow = 60.0f;
      substateInfo.Qcinstr = bolusFlow;
   }
}

void procedureDetail::CalculateNoCollectFirstReturnFlows ()
{
   // DataLog(log_level_predict_info) << " procedureDetail::CalculateNoCollectFirstReturnFlows "   << endmsg;
   // _firstReturnNoCollect is member data... not in the states array

   float ratio = _config.getACRatio();

   // if we cant find the state in the lookup the limitPrime Ratio
   if (AlgSubstate(_procState.SubState()) < 0) ratio = LimitPrimeRatio(ratio, _procState.Hct());

   //
   //
   //   Calculate cap values (limit Qrp to first cycle rate per IT5070)
   //
   float qincap      = _procState.AdjQinCap();
   if (qincap > cobeConfig.QinStartup) qincap = cobeConfig.QinStartup;
   float qininstdmax = cobeConfig.TbvQinMaxCoefficient * _config.getTbvQinmaxScaleConst() * _procState.Tbv();
   if (qincap > qininstdmax) qincap = qininstdmax;

   float qrpcap   = cobeConfig.QrpStartup;

   float qncap    = _procState.AdjQrpCap();
   float qntbvmax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * _procState.Tbv();
   if (qncap > qntbvmax) qncap = qntbvmax;
   if (qncap > cobeConfig.FirstReturnNoCollectQnCap) qncap = cobeConfig.FirstReturnNoCollectQnCap;
   float acRate = MIN(_I, getMaxACRate());

   // DataLog(log_level_predict_info) << "JPH Calculating No Collect First Return " << endmsg;
   _nocollectFlows.Initialize(_procState.Hct(),
                              _procState.PreCount(),
                              _procState.Tbv(),
                              acRate,
                              ratio,
                              qincap,
                              qrpcap,
                              qncap);




   // DataLog(log_level_predict_info) << "JPH _DonorHct " << _DonorHct << endmsg;
   // DataLog(log_level_predict_info) << "JPH _DonorPreCount " << _DonorPreCount << endmsg;
   // DataLog(log_level_predict_info) << "JPH _procState.Tbv() " << _procState.Tbv() << endmsg;
   // DataLog(log_level_predict_info) << "JPH IR " << cobeConfig.FirstReturnNocollectInfusionRate << endmsg;
   // DataLog(log_level_predict_info) << "JPH ratio " << ratio << endmsg;
   // DataLog(log_level_predict_info) << "JPH qincap " << qincap << endmsg;
   // DataLog(log_level_predict_info) << "JPH qrpcap " << qrpcap << endmsg;
   // DataLog(log_level_predict_info) << "JPH qncap "  << qncap << endmsg;


   _nocollectFlows.calculate(&_firstReturnNoCollect);

   //
   //
   //   Save off instantaneous flow details
   //
   _firstReturnNoCollect.T                   = 0.0f; // no real time spent in this state
   _firstReturnNoCollect.EndVin              = 0.0f;
   _firstReturnNoCollect.Vac                 = 0.0f;
   _firstReturnNoCollect.Td                  = _firstReturnNoCollect.Vsvn / _firstReturnNoCollect.Qininstd;
   _firstReturnNoCollect.Qcc                 = 0.0f;
   _firstReturnNoCollect.Qpc                 = 0.0f;
   _firstReturnNoCollect.Qch                 = 0.0f;
   _firstReturnNoCollect.HypovolemiaDistance = 0.0f;
   _firstReturnNoCollect.Qcinstd             = _firstReturnNoCollect.Qcc;
   _firstReturnNoCollect.Qcinstr             = _firstReturnNoCollect.Qcc;
   _firstReturnNoCollect.Qpinstd             = _firstReturnNoCollect.Qpc;
   _firstReturnNoCollect.Qpinstr             = _firstReturnNoCollect.Qpc;
   _firstReturnNoCollect.Csf                 =  1.0f; // By definition
   _firstReturnNoCollect.Rpm                 = cobeConfig.RPMCollect;

   // DataLog(log_level_predict_info) << " JPH Calculated No Collect First Return Qrp = " << _firstReturnNoCollect.Qrinst << endmsg;
   //
   //
   //  Override Qcinstr for saline bolus flows per convention
   //
   if (getRecommendedCassette() == cassette::RBCPLS)
   {
      float bolusFlow = _procState.Tbv() * _I * _config.getACRatio() / 1000.0f;
      if ( bolusFlow > 60.0f ) bolusFlow = 60.0f;
      _firstReturnNoCollect.Qcinstr = bolusFlow;
   }
}

void procedureDetail::setNoCollectFirstReturn ()
{
   //  DataLog(log_level_predict_info) << " procedureDetail::setNoCollectFirstReturn "   << endmsg;
   Predict_CDS::one_substate_info_t ncr;
   // DataLog(log_level_predict_info) << "JPH Setting my NoCollectFirstReturn values. Qrp= " << _firstReturnNoCollect.Qrinst << endmsg;

   ncr.QinD            = _firstReturnNoCollect.Qininstd;
   ncr.QinR            = _firstReturnNoCollect.Qininstr;
   ncr.Qac             = _firstReturnNoCollect.Qacinst;
   ncr.Qrp             = _firstReturnNoCollect.Qrinst;
   ncr.QpD             = _firstReturnNoCollect.Qpinstd;
   ncr.QpR             = _firstReturnNoCollect.Qpinstr;
   ncr.QcD             = _firstReturnNoCollect.Qcinstd;
   ncr.QcR             = _firstReturnNoCollect.Qcinstr;
   ncr.DutyCycle       = _firstReturnNoCollect.D;
   ncr.InfusionRate    = getMaxACRate();
   ncr.T               = _firstReturnNoCollect.T;
   ncr.EndVin          = _firstReturnNoCollect.EndVin;
   ncr.Vtarget         = 0.0f;               // The specific end volume target for this substate
   ncr.OrVtarget       = 0.0f;               //  an alternative specific end volume target for this substate
   ncr.VtargetType     = VTarget::NOCOLLECT; // The specific end volume target type for this substate
   ncr.OrVtargetType   = VTarget::NOCOLLECT; //  an alternative specific end volume target type for this substate
   ncr.UnadjustedQin   = 0.0f;               // Max Qin for this substate if no adjustments were performed
   ncr.UnadjustedQn    = 0.0f;
   ncr.UnadjustedIR    = 0.0f;
   ncr.UnadjustedRatio = 0.0f;
   ncr.PlateletValve   = HAL_VALVE_RETURN;
   ncr.PlasmaValve     = HAL_VALVE_RETURN;
   ncr.RBCValve        = HAL_VALVE_RETURN;
   ncr.Rpm             = _firstReturnNoCollect.Rpm;
   ncr.Csf             = _firstReturnNoCollect.Csf;

   _procState._predictCDS.NoCollectFirstReturnOnly.Set(ncr);
}

float procedureDetail::estimateVbpForRbcCollection () const
{
   // DataLog(log_level_predict_info) << " procedureDetail::estimateVbpForRbcCollection "   << endmsg;
   float VBPDuringRBC = 0.0;
   if (getPredRBCDose() > 0.0)
   {
      //
      //
      //   Calculate VBP that is required to process the current RBC collection request
      //
      if (doingPTFDouble())
      {
         VBPDuringRBC = (substate[RBCState()].EndVin - substate[RBCState() - 1].EndVin
                         + substate[RBCDoubleState()].EndVin - substate[RBCDoubleState() - 1].EndVin) * (1.0f - 1.0f / _config.getACRatioDuringRBC());
      }
      else
      {
         VBPDuringRBC = (substate[RBCState()].EndVin - substate[RBCState() - 1].EndVin) * (1.0f - 1.0f / _config.getACRatioDuringRBC());
      }
      VBPDuringRBC += _procState.RbcDose() / ((_procState.InitialHct() + _procState.Hct()) / 2.0f); // rough estimate
   }
   return VBPDuringRBC;
}

bool procedureDetail::checkForTraliFiltering ()
{
   //  DataLog(log_level_predict_info) << " procedureDetail::checkForTraliFiltering "   << endmsg;
   const bool maleOnlyPlasmaOptionOn = (_config.getPlasmaGenderOption() != MALE_ONLY_OPTION_ALL);
   const bool maleOnlyPltOptionOn    = ( _config.getPltGenderOption() != MALE_ONLY_OPTION_ALL);
   const bool procedureHasPlasma     = getProcCollectPlasma();
   const bool procedureHasPlts       = (getProcFixedCollectVol() > 0);

   // If the donor is male,
   if ((!_procState.Female()) ||
       // OR neither TRALI filtering option is on,
       ((!maleOnlyPlasmaOptionOn) && (!maleOnlyPltOptionOn)) ||
       // OR the procedure collects neither platelets nor plasma
       ((!procedureHasPlasma) && (!procedureHasPlts)))
   {
      // Return false, because this procedure is fine.
      return false;
   }
   else
   {
      const bool abFemalesOkForPlasma = (_config.getPlasmaGenderOption() == MALE_ONLY_OPTION_MALES_AND_AB_FEMALES);
      const bool abFemalesOkForPlt    = (_config.getPltGenderOption() == MALE_ONLY_OPTION_MALES_AND_AB_FEMALES);
      const bool donorIsAb            = (_procState.BloodType() == BT_AB_NEG) || (_procState.BloodType() == BT_AB_POS);

      // Remember: we've already established the donor is female at this point.

      // If the procedure has plasma and male-only plasma option is turned on
      if (procedureHasPlasma && maleOnlyPlasmaOptionOn)
      {
         // If the donor is AB and we are allowing AB females for plasma
         if (donorIsAb && abFemalesOkForPlasma)
         {
            DataLog (log_level_proc_info) << "Don't filter for plama because donor is AB." << endmsg;
         }
         else
         {
            DataLog (log_level_proc_info) << "Procedure has plasma, and donor is female, so filter for TRALI." << endmsg;
            return true;
         }
      }

      // If the procedure has platelets and male-only platelets option is turned on
      if (procedureHasPlts && maleOnlyPltOptionOn)
      {
         // If the donor is AB and we are allowing AB females for plasma
         if (donorIsAb && abFemalesOkForPlt)
         {
            DataLog (log_level_proc_info) << "Don't filter for platelets because donor is AB." << endmsg;
         }
         else
         {
            DataLog (log_level_proc_info) << "Procedure has platelets, and donor is female, so filter for TRALI." << endmsg;
            return true;
         }
      }
   }

   return false;
}

void procedureDetail::initialize (float yield,
                                  float platelet_volume,
                                  float plasma_volume,         // typical, includes AC
                                  float plasma_product_volume, // special regionalization, product volume without AC
                                  float rbc_dose,
                                  float rbc_crit,
                                  float rsf,
                                  float max_procedure_time,
                                  int blood_type,
                                  int designator,
                                  bool mss_plt,
                                  float mss_plt_volume,
                                  int mss_plt_pct_carryover,
                                  bool mss_rbc,
                                  int mss_rbc_volume,
                                  int procedure_number)
{

   // DataLog(log_level_predict_info) << " procedureDetail::initialize "   << endmsg;
   setProcedureNumber(procedure_number);
   setDesignator(designator);
   setOptimized(0);

   if (plasma_product_volume > 0.0f)  // plasma product without AC
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(1);
      setProcAMAPorMC(0);
      setProcFixedPlasmaVol(plasma_product_volume); // for now, overridden later
      setProcPlasmaProductVol(plasma_product_volume);
   }
   else if (plasma_volume > 0.0)  // normal fixed plasma product
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(1);
      setProcFixedPlasmaVol(plasma_volume);
      setProcAMAPorMC(0);
      setProcPlasmaProductVol(0.0f);
   }
   else if (plasma_volume == PLASMA_AMAP_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(0);
      setProcAMAPorMC(0);
      setProcFixedPlasmaVol(0.0);
      setProcPlasmaProductVol(0.0f);
   }
   else if (plasma_volume == PLASMA_MAX_CONCURRENT_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(0);
      setProcAMAPorMC(1);
      setProcFixedPlasmaVol(0.0);
      setProcPlasmaProductVol(0.0f);
   }
   else if (plasma_volume == PLASMA_FFP_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(1);
      setProcAMAPorMC(0);
      setProcFixedPlasmaVol(_config.getFFPVolume());
      setProcPlasmaProductVol(0.0f);
   }
   else // no plasma product
   {
      setProcCollectPlasma(0);
      setProcPlasmaVolFixed(1);
      setProcFixedPlasmaVol(0.0);
      setProcAMAPorMC(0);
      setProcPlasmaProductVol(0.0f);
   }

   setProcCollectVolFixed(true);
   setProcFixedCollectVol(platelet_volume);

   if (platelet_volume > 0.0)
   {
      setDesiredYield(yield);
      setProcPlateletProdCon(100000.0f * yield / platelet_volume);
      setMSSPltVolume(mss_plt ? (int)mss_plt_volume : 0);
   }
   else
   {
      setDesiredYield(0.0f);
      setProcPlateletProdCon(0);
      setMSSPltVolume(0);
   }

   _PltAmap = false;
   setAmapPltSize(AMAP_PLT_NONE);

   ///////////////// RBC STUFF //////////////////////////////////////////////////////
   if (rbc_dose > 0.0 && rbc_crit > 0.0)
   {
      float        filterLoss        = 0.0f;
      bool         needTwoFilterLoss = false;
      unsigned int funcBits          =  _procState.GetCassetteFunctionBits();

      needTwoFilterLoss = _config.isDrbcAlertOn()
                          && (rbc_dose >= _config.getDrbcThreshold())
                          && mss_rbc;

      // DataLog(log_level_predict_info) << "funcBits->" << funcBits << endmsg;
      // DataLog(log_level_predict_info) << "needTwoFilterLoss->" << needTwoFilterLoss << endmsg;
      // REF number set then go with what it tells us
      if (funcBits != 0)
      {
         bool setIsRas  = CatalogChecker::checkBit(funcBits, CatalogChecker::MSS_RBC);
         bool setIsDrbc = CatalogChecker::checkBit(funcBits, CatalogChecker::DRBC_BAG);

         // DataLog(log_level_predict_info) << "setIsRas->" << setIsRas << endmsg;
         // DataLog(log_level_predict_info) << "setIsDrbc->" << setIsDrbc << endmsg;

         if (setIsRas)
         {
            DataLog(log_level_predict_info) << "REF given: filter loss is RAS" << endmsg;
            filterLoss = cobeConfig.mssPtfFilterLoss;
         }
         if (setIsDrbc && needTwoFilterLoss)
         {

            DataLog(log_level_predict_info) << "REF given: filter loss is RAS + DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(true);

            filterLoss += cobeConfig.mssPtfFilterLoss;
         }
         else
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is not DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(false);
         }


         // else, we just need to look at the clues
      }
      else
      {

         // one
         if (mss_rbc)
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is RAS" << endmsg;
            filterLoss =  cobeConfig.mssPtfFilterLoss ;
         }

         // two
         if (needTwoFilterLoss && (platelet_volume == 0.0f))
         {

            DataLog(log_level_predict_info) << "NO REF given: filter loss is RAS + DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(true);

            filterLoss +=  cobeConfig.mssPtfFilterLoss ;
         }
         else
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is not DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(false);
         }

      }

      setProcRBCVolume(  (  ( rbc_dose * 100.0f / (rsf) ) / rbc_crit ) +  filterLoss);
      setProcRBCHct(rbc_crit);
      setMSSRbcVolume(mss_rbc ? mss_rbc_volume : 0);
   }
   else
   {
      setProcRBCVolume(0);
      setProcRBCHct(80.0f); // avoid some /0 constructs
      setMSSRbcVolume(0);
      setProcPtfIsSplit(false);
   }

   setProcRBCDose((int)rbc_dose);
   setProcBloodType(blood_type);
   setProcTime(max_procedure_time);

   // Set cassette options
   setMSSPlt(mss_plt);
   setMSSRbc(mss_rbc);
   setMSSPltPctCarryover(mss_plt_pct_carryover);
   setOriginalPltVolume(platelet_volume);
   setOriginalPltYeild(yield);


   // for black stamp, restore the intermediate substates
   // if(_ProcRunCDS.Substate.Get() ==  && getRecommendedCassette() == cassette::RBCPLS) {
//	   _procState._predictCDS->IntermediateSubstates.Get(&substate);
//   }
   DataLog(log_level_predict_external) << "Predict: Created product " << procedure_number
                                       << " (" << designator << ")"
                                       << " " << yield << " " << platelet_volume << " " << plasma_volume
                                       << " " << rbc_dose << " " << rbc_crit << " " << max_procedure_time
                                       << " " << blood_type << " ("
                                       << (mss_plt ? "MP/" : "-/")
                                       << (mss_rbc ? "MR/" : "-/")
                                       << ") "
                                       << (mss_plt ? mss_plt_pct_carryover : 0)
                                       << " "
                                       << (mss_rbc ? mss_rbc_volume : 0)
                                       << " "
                                       << rsf
                                       << endmsg;
}



///////////////////////////////////////////////////////


void procedureDetail::initializeAmapPlt (float requestedYield,
                                         AMAPPLTTYPES amap_platelet_size,
                                         float plasma_volume,
                                         float rbc_dose,
                                         float rbc_crit,
                                         float rsf,
                                         float max_procedure_time,
                                         int blood_type,
                                         int designator,
                                         bool mss_plt,
                                         bool mss_rbc,
                                         int mss_rbc_volume,
                                         int procedure_number)
{

   DataLog(log_level_predict_info) << " procedureDetail::initializeAmapPlt "   << endmsg;

   setProcedureNumber(procedure_number);
   setDesignator(designator);
   setOptimized(0);


   ///////////////  RAS PAS ////////////////////////////
   //  set this before calculating AMAP concentration
   //  input for MSS only indicates whether metering is enabled.
   //  If metering is off, AMAP+PAS products are disqualified.
   //
   setMSSRbc(mss_rbc);
   if (mss_plt)
   {
      if (amap_platelet_size >= AMAP_PLT_PAS_SINGLE)   // requested PAS product and it is enabled
      {
         setMSSPlt(true);
      }
      else     // metering is on but this is a non-PAS product
      {
         setMSSPlt(false);
      }
   }
   else if (amap_platelet_size >= AMAP_PLT_PAS_SINGLE)     // metering is off but this is a PAS product
   {
      setMSSPlt(false);
      amap_platelet_size = (AMAPPLTTYPES)(((amap_platelet_size - 1) % NUM_PLT_AMAP_PAS) + 1); // transform into the non-PAS equivalent
      DataLog(log_level_predict_info) << "AMAP: product " << procedure_number << " is PAS but metering is off." << endmsg;
   }


   float amapYldMin, amapYldMax, amapConc = 0.0f;
   setMyAMAPPltYieldRange(amap_platelet_size, amapYldMin, amapYldMax, amapConc);

   DataLog(log_level_predict_info) << "AMAP: product with: min="  << amapYldMin
                                   << " max=" << amapYldMax
                                   << " amap conc=" << amapConc
                                   << " amap_platelet_size=" << amap_platelet_size
                                   << endmsg;



   ////////////  AMAP PLT ///////////////////////////////////////////////////////
   setProcCollectVolFixed(true);

   // jph  PASS amap palatelet information down to the procedures....
   if (amap_platelet_size > AMAP_PLT_NONE && amap_platelet_size <= AMAP_PLT_PAS_TRIPLE)
   {
      _PltAmap = true;
      setAmapPltSize((AMAPPLTTYPES)(((amap_platelet_size - 1) % NUM_PLT_AMAP_PAS) + 1));
      setProcFixedCollectVol(100000.0f * requestedYield / amapConc);
      setDesiredYield(requestedYield);
      setProcPlateletProdCon(amapConc);
      setOriginalPltVolume(100000.0f * requestedYield / amapConc);
      setOriginalPltYeild(requestedYield);

      float plt_vlm = 100000.0f * requestedYield / amapConc;

      setProcFixedCollectVol(plt_vlm);
      setOriginalPltVolume  (plt_vlm);

      if (mss_plt)
      {
         float percent_co = ((float)getMSSPltPctCarryover()) / 100.0f;
         float PAS_Volume = round (plt_vlm  * (  (1 - percent_co) / percent_co  ));

         DataLog(log_level_predict_info) << " PAS volm is: " << PAS_Volume << endmsg;
         setMSSPltVolume((int)PAS_Volume);

      }
      else
      {
         setMSSPltVolume(0);
      }
   }
   else
   {
      setProcFixedCollectVol(0.0f);
      _PltAmap = false;
      setAmapPltSize(AMAP_PLT_NONE);
      setDesiredYield(0.0f);
      setProcPlateletProdCon(0.0f);
      setMSSPltVolume(0);
      setOriginalPltVolume(0.0f);
      setOriginalPltYeild(0.0f);
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   //
   // Now that it is created, initialize it.
   //
   // setDesignator(designator);
   // setOptimized(0);

   ///////// PLASMA ////////////////////////////////////////////////////////////////////////////////////////////////

   if (plasma_volume > 0.0)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(1);
      // new_product->setPlasmaTargetCannotChange(1);
      setProcFixedPlasmaVol(plasma_volume);
      setProcAMAPorMC(0);
   }
   else if (plasma_volume == PLASMA_AMAP_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(0);
      // new_product->setPlasmaTargetCannotChange(0);
      setProcAMAPorMC(0);
      setProcFixedPlasmaVol(0.0);
   }
   else if (plasma_volume ==  PLASMA_MAX_CONCURRENT_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(0);
      // new_product->setPlasmaTargetCannotChange(0);
      setProcAMAPorMC(1);
      setProcFixedPlasmaVol(0.0);
   }
   else if (plasma_volume == PLASMA_FFP_FLAG)
   {
      setProcCollectPlasma(1);
      setProcPlasmaVolFixed(1);
      // new_product->setPlasmaTargetCannotChange(1);
      setProcAMAPorMC(0);
      setProcFixedPlasmaVol(_config.getFFPVolume());
   }
   else
   {
      setProcCollectPlasma(0);
      setProcPlasmaVolFixed(1);
      setProcFixedPlasmaVol(0.0);
      setProcAMAPorMC(0);
   }
   /////////////////////////////////////////////////////////////////////////////


   ///////////////// RBC STUFF //////////////////////////////////////////////////////
   if (rbc_dose > 0.0 && rbc_crit > 0.0)
   {
      float        filterLoss        = 0.0f;
      bool         needTwoFilterLoss = false;
      unsigned int funcBits          = _procState.GetCassetteFunctionBits();

      needTwoFilterLoss = _config.isDrbcAlertOn()
                          && (rbc_dose >= _config.getDrbcThreshold())
                          && mss_rbc;

      // REF number set then go with what it tells us
      if (funcBits != 0)
      {
         bool setIsRas  = CatalogChecker::checkBit(funcBits, CatalogChecker::MSS_RBC);
         bool setIsDrbc = CatalogChecker::checkBit(funcBits, CatalogChecker::DRBC_BAG);

         if (setIsRas)
         {
            DataLog(log_level_predict_info) << "REF given: filter loss is RAS" << endmsg;
            filterLoss = cobeConfig.mssPtfFilterLoss;
         }
         if (setIsDrbc && needTwoFilterLoss)
         {

            DataLog(log_level_predict_info) << "REF given: filter loss is RAS + DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(true);

            filterLoss += cobeConfig.mssPtfFilterLoss;
         }
         else
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is not DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(false);
         }


         // else, we just need to look at the clues
      }
      else
      {

         // one
         if (mss_rbc)
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is RAS" << endmsg;
            filterLoss =  cobeConfig.mssPtfFilterLoss ;
         }

         // two
         if (needTwoFilterLoss && (amap_platelet_size > AMAP_PLT_NONE ))
         {

            DataLog(log_level_predict_info) << "NO REF given: filter loss is RAS + DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(true);

            filterLoss +=  cobeConfig.mssPtfFilterLoss ;
         }
         else
         {
            DataLog(log_level_predict_info) << "NO REF given: filter loss is not DRBC (dose="
                                            << rbc_dose << "; thresh=" << _config.getDrbcThreshold()
                                            << ")" << endmsg;

            setProcPtfIsSplit(false);
         }

      }

      setProcRBCVolume(  (  ( rbc_dose * 100.0f / (rsf) ) / rbc_crit ) +  filterLoss);
      setProcRBCHct(rbc_crit);
      setMSSRbcVolume(mss_rbc ? mss_rbc_volume : 0);
   }
   else
   {
      setProcRBCVolume(0);
      setProcRBCHct(80.0f); // avoid some /0 constructs
      setMSSRbcVolume(0);
      setProcPtfIsSplit(false);
   }
   //////////////////////////////////////////////////////////////////////////////////////////////////

   setProcRBCDose((int)rbc_dose);
   setProcBloodType(blood_type);
   setProcTime(max_procedure_time);

   ///////////////  RAS PAS ////////////////////////////
//    setMSSPlt(mss_plt);
//    setMSSRbc(mss_rbc);
//    setMSSPltPctCarryover( mss_plt_pct_carryover);
//
// setOriginalPltVolume(platelet_volume);
// setOriginalPltYeild(yield);

   // setOriginalPltVolume(0.0f);
   // setOriginalPltYeild(requestedYield);


   DataLog(log_level_predict_info) << "Predict: Created product " << procedure_number
                                   << " (" << designator << ")"
                                   << " " << requestedYield << " " << getOriginalPltVolume() << " " << plasma_volume
                                   << " " << rbc_dose << " " << rbc_crit << " " << max_procedure_time
                                   << " " << blood_type << " ("
                                   << (mss_plt ? "MP/" : "-/")
                                   << (mss_rbc ? "MR/" : "-/")
                                   << ") "
                                   << (mss_plt ? getMSSPltPctCarryover() : 0)
                                   << " "
                                   << (mss_rbc ? mss_rbc_volume : 0)
                                   << " "
                                   << rsf
                                   << endmsg;



   if (_AMAP_PLATELETS_ON)
   {

      DataLog(log_level_predict_info) << "Predict: AMAP PLT Created product "
                                      << procedure_number << "  "
                                      << " plt:(amapp:=" << amap_platelet_size
                                      << " yld=" <<  getDesiredYield()
                                      << " conc=" << getProcPlateletProdCon()
                                      << " volm=" << getProcFixedCollectVol()
                                      << " ) "
                                      <<  endmsg;
   }


   // DataLog(log_level_predict_info) << "Exit: AMAP PLT Created product "  << endmsg;
}



void procedureDetail::SetUnoptimized ()
{
   //  DataLog(log_level_predict_info) << " procedureDetail::SetUnoptimized "   << endmsg;
   setOptimized(0);
   setStatus(UNOPTIMIZED);
   if (getProcPlasmaVolFixed())
   {
      setPlasmaTargetCannotChange(1);
   }
   else
   {
      setPlasmaTargetCannotChange(0);
   }

}

void procedureDetail::SetTargetsCds (ProcRunTargets_CDS& targetsCds)
{
   //  DataLog(log_level_predict_info) << " procedureDetail::SetTargetsCds "   << endmsg;

   ScopedCDSLock targets_lock(targetsCds);

   // Many of the function calls below are virtual.  We therefore don't call
   // them directly on the implicit *this, but explicitly through the this
   // pointer, to maintain polymorphic behavior.  (Note: we call functions for
   // values that are not accessed via virtual functions, because ya never know
   // when someone is going to add a 'virtual' keyword somewhere.)

   targetsCds.PlateletYieldTarget.Set    (this->getYtarg());
   targetsCds.PlateletVolumeTarget.Set   (this->getPredVc());
   targetsCds.PlasmaTarget.Set           (this->getPredVp());
   targetsCds.RbcVolumeTarget.Set        (this->getPredRBCVolume());
   targetsCds.RbcCritTarget.Set          (this->getProcRBCHct() / 100.0f);
   targetsCds.RbcDoseTarget.Set          (this->getPredRBCDose());
   targetsCds.PlateletConcenTarget.Set   (this->getProcPlateletProdCon());
   targetsCds.InfusionRateTarget.Set     (this->getInfusionRate());
   targetsCds.PlateletAmapSizeTarget.Set (this->getAmapPltSize());
   targetsCds.RequiresRepFluid.Set       (this->ReplacementFluidRequired());

   targetsCds.ProcNumber.Set(this->getProcedureNumber());

   // Set Metered Targets
   targetsCds.PltSolutionTarget.Set((float)this->getMSSPltVolume());
   targetsCds.PltPctCarryover.Set((float)this->getMSSPltPctCarryover());
   targetsCds.RBCSolutionTarget.Set((float)this->getMSSRbcVolume());

   DataLog(log_level_proc_info) << "Just set RBCSolutionTarget to ->"
                                << targetsCds.RBCSolutionTarget.Get()
                                << endmsg;
   targetsCds.OriginalVolume.Set(this->getOriginalPltVolume() );
   targetsCds.OriginalYeild.Set(this->getOriginalPltYeild() );

   DataLog(log_level_proc_info) << "Just set SelectedPlateletConcen to "
                                << targetsCds.SelectedPlateletConcen.Get()
                                << endmsg;
   DataLog(log_level_proc_info) << "Just set PltSolutionTarget to " << targetsCds.PltSolutionTarget.Get() << endmsg;
   DataLog(log_level_proc_info) << "Just set PLT PCT Carryover to ->" << targetsCds.PltPctCarryover.Get() << endmsg;
   DataLog(log_level_proc_info) << "Just set OriginalVolume PLT to ->" << targetsCds.OriginalVolume.Get() << endmsg;
   DataLog(log_level_proc_info) << "Just set OriginalYeild PLT to ->" << targetsCds.OriginalYeild.Get() << endmsg;
   DataLog(log_level_proc_info) << "Just set PlateletAmapSizeTarget  to ->"
                                << targetsCds.PlateletAmapSizeTarget.Get()
                                << endmsg;

   // Set the Cassette Options
   targetsCds.MeterPlatelets.Set(this->getMSSPlt());
   targetsCds.MeterRbcs.Set(this->getMSSRbc());
   DataLog(log_level_proc_info) << "Just set RAS to  ->" << targetsCds.MeterRbcs.Get() << endmsg;
   DataLog(log_level_proc_info) << "Just set PAS to  ->" << targetsCds.MeterPlatelets.Get() << endmsg;



}



substates_t procedureDetail::SetTargets ()
{
   if (getStatus() == GOOD_PRODUCT)       // IT5563
   {
      _procState._predictCDS.VinTarget.Set(VinTarget());
   }

   // Create a brand new list of substates
   substates_t info;

   // Forward to private virtual function to fill 'em in
   do_SetTargets(info);

   // .. and return the result

   return info;
}




void procedureDetail::setMyAMAPPltYieldRange (AMAPPLTTYPES prodSize, float& min, float& max, float& conc)
{
   if (prodSize < AMAP_PLT_SINGLE || prodSize > AMAP_PLT_PAS_TRIPLE)
   {
      // DataLog (log_level_predict_error) << "amapp: size " << prodSize << " is not valid." << endmsg;
      min = 0; max = 0; conc = 0;
   }
   else
   {

      min = _config.AMAPPltMinYield(prodSize);
      max = _config.AMAPPltMaxYield(prodSize);

      //
      //
      //   IT13361 limit min to already collected yield if necessary
      //
      if (min < _procState.Yield()) min = _procState.Yield();

      // oh No! amap plt saves final concentration... we need collect Conc
      float finalConc      =  _config.AMAPPltConfigConc(prodSize);
      setMSSPltPctCarryover(_config.AMAPPltCarryover(prodSize));
      float pco            =  ((float)getMSSPltPctCarryover()) / 100.0f;
      float maxTotalvolume =   max / finalConc; // only need min or max for below....
      float maxPasVol      = maxTotalvolume *  ( 1 - pco ) ;

      // get the collect concentration
      conc =  max / (maxTotalvolume - maxPasVol);

      DataLog(log_level_predict_info) << " procedureDetail::setMyAMAPPltYieldRange "
                                      << " pco=" << pco << " collectConc=" << conc << " finalConc=" << finalConc
                                      << endmsg;

   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void procedureDetail::setMyAMAPPltYieldRange (float& min, float& max, float& conc)
{
   DataLog(log_level_predict_info) << " procedureDetail::setMyAMAPPltYieldRange2 "
                                   << " min = " << min << ", max = " << max
                                   << ", conc = " << conc << endmsg;
   setMyAMAPPltYieldRange(getAmapPltSize(), min, max, conc);
}

/* FORMAT HASH e112f69601ed2e0244d86030fbfbfd51 */
