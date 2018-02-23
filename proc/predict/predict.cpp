/****************************************************************************
 *
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      predict.cpp
 *             Procedure prediction/optimization
 *
 ***************************************************************************/


#include "predict.h"

#include <cmath>

#include "procdet.h"
#include "procdetpltplsrbc.h"
#include "procdetrbcpls.h"
#include "procdetfactory.h"

#include "pqipredictor.h"
#include "trima_datalog_levels.h"
#include "mathdefs.hpp"

#include "scoped_cds_lock.hpp"
#include "software_cds.h"

/////////////////////////////////////////////////////////////////////////////
// Constructor - setup messaging environment
Predict::Predict()
   : _procedures(ProcedureList::Instance()),
     _concentrationWarningGiven(false),
     _productGivenWarning(-999),
     _maxDesignator(_procedures.MaxDesignator()),
     _blockTargets(false),
     _currentPltMss(0),

     _config(Config::Instance()),
     _procState(ProcDataState::Instance()),
     _TargetsCDS(ProcRunTargets_CDS(ROLE_RW)),

     _AMAP_PLATELETS_ON(false),         // feature driven
     _AMAP_PLATELETS_PROCESSING(false), // looping and stepping over plt sizes
     _COMP_PLS_ON(false),               // feature driven
     _stepSize(0.0f),
     _maxAMAPPyield(0.0f),

     _selectedProc (_procedures.Selected()),
     _curProc      (_procedures.Current()),

     cobeConfig(CobeConfig::data()),

     _amapPltItermediatePredict(false)

{
   // DataLog(log_level_predict_debug) << "Predict Constructed" << endmsg;
}

////////////////////////////////////////////////////////////////////////////
// Destructor -
Predict::~Predict()
{
   cleanup();

}


//////////////////////////////////////////////////////////////////////////////
// Initialization routines called by Standard Main
ALARM_VALUES Predict::Init ()
{
   // DataLog(log_level_predict_debug) << " Predict::Init()" << endmsg;
#if (CPU==SIMNT) && !defined(__WIN32__)
   datalog_SetTaskOutputOptions(DATALOG_CURRENT_TASK, DataLog_LogEnabled, DataLog_ConsoleEnabled);
#endif

   // Init local variables:
   _curProc      = NULL;
   _selectedProc = 0;

   //////////////////////////////////////////////////////////////////////////////
   // jph

   if (Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable))
   {
      DataLog(log_level_predict_debug) << "amapp: AMAP PLT feature is ON" << endmsg;
      _stepSize          = cobeConfig.YieldStepSize;
      _AMAP_PLATELETS_ON = true;
   }
   else
   {
      DataLog(log_level_predict_debug) << "amapp: AMAP PLT feature is OFF" << endmsg;
      _AMAP_PLATELETS_ON = false;
   }
   //////////////////////////////////////////////////////////////////////////////
   //

   if (Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable))
   {
      DataLog(log_level_predict_debug) << "Complementary PLS feature is  ON" << endmsg;
      _COMP_PLS_ON = true;
   }
   else
   {
      DataLog(log_level_predict_debug) << "Complementary PLS feature is OFF" << endmsg;
      _COMP_PLS_ON = false;
   }

   /////////////////////////////////////////////////////////////////////////////

   // setPIRQchMax(cobeConfig.CollectMaximumFlow);

   return NULL_ALARM;
}



void Predict::cleanup ()
{
   _procedures.clear();

   _curProc      = NULL;
   _selectedProc = NULL;
}


void Predict::FillInConfig ()
{
   _config.readConfigDat();
   doRepredictForAdjustments();
}


//
// -----------------------------------------------------------------------------
//  This code assumes indices have been validated by external configuration
//  read/write code.
//
void Predict::FillInProcedureConfig ()
{
   // DataLog(log_level_predict_debug) << " Predict::FillInProcedureConfig()" << endmsg;
#ifndef __PREDICTROBUSTNESS__
   cleanup();
#endif


   if ( _config.isCompPlasmaOn() )
   {
      DataLog(log_level_predict_debug) << "compPls is config enabled" << endmsg;
   }
   else
   {

      DataLog(log_level_predict_debug) << "compPls is config disabled" << endmsg;
   }


   //////////////////////////////////////////////////////////////////////////////////
   int i = 0;
   for ( i = 0; i < _config.getNumProcedures(); i++)
   {
      const ProcedureDescription& def = _config.getProcedureDescription(i);

      // ProcedureDescriptions count from 0, procedure numbers count from 1...
      procedureDetail* new_product = create_procedure(def, i + 1);
      _procedures.insert(new_product);
   }

   //   Add in region specific procedure options
   if (Software_CDS::GetInstance().getFeature(AllowAutoConfPlasmaProcs))
   {
      FillInRegionConfig(i + 1);
   }
}


procedureDetail* Predict::create_procedure (const ProcedureDescription& def,
                                            int procedure_number) const
{
   ///// METERED STORAGE SETUP //////////////////////////////////////////////////////
   // use these to hold the state of the master switch and the procedure switch combinations
   bool  MssPLTSwitch      = false;
   bool  MssRBCSwitch      = false;

   float PltSSTargetVolume = 0;

   // do the master/procedure switch "and"  to get the status
   MssPLTSwitch = (def.plt_mss && _config.isPltMeteringOn());

   // PTF needs both masters (PTF and RBC) and the procedure RBC and PTF switch checked
   MssRBCSwitch = (def.rbc_mss && _config.isRbcMeteringOn());

   // get the % Plasma carryover SS volm
//       percent_co = (float)def.pct_carryover/100.0f;
//       PltSSTargetVolume = floor((int)round((def.platelet_volume * ((1-percent_co) / percent_co))));
//
   float plt_volm   =  def.platelet_volume;
   float percent_co = (float)def.pct_carryover / 100.0f;

   // Is this an AMAP Platelets procedure?  Yes, if AMAPP is turned on, *and*
   // the procedure has AMAP configured.  (Procedures can have both AMAP and
   // non-AMAP configurations, and change behavior when AMAPP is flipped, which
   // is why we can't just check the procedure.) --DGS
   bool isAmapPlt = _AMAP_PLATELETS_ON && ( def.amap_platelet_size != AMAP_PLT_NONE );

   if (percent_co == 0.0f || plt_volm == 0.0f || isAmapPlt)    // we figure it out later if amap plt is on
   {
      PltSSTargetVolume = 0.0f;
   }
   else
   {
      PltSSTargetVolume = round(plt_volm * (  (1 - percent_co) / percent_co)  );
   }

   PIRType          pir_t       = PIR_STD;

   procedureDetail* new_product = NULL;



   if (isAmapPlt) // Handle the AMAP Platelet case
   {
      float amapmin, amapmax, amapconc = -1.0f;

      // for AMAP platelets, we need to compute the concentration differently.
      // This is a duplication of code from procdet.cpp, setMyAMAPPltYieldRange;
      // duplicated because we're close to ECO and even halfway-decent
      // refactoring of this code is therefore out-of-scope.

      amapmin = _config.AMAPPltMinYield(def.amap_platelet_size);
      amapmax = _config.AMAPPltMaxYield(def.amap_platelet_size);

      float finalConc      = _config.AMAPPltConfigConc(def.amap_platelet_size);
      float carryover      = _config.AMAPPltCarryover(def.amap_platelet_size);
      float pco            = carryover / 100.0f;
      float maxTotalvolume = amapmax / finalConc; // only need min or max for below....
      float maxPasVol      = maxTotalvolume *  ( 1 - pco ) ;

      // get the collect concentration
      amapconc    =  amapmax / (maxTotalvolume - maxPasVol);

      pir_t       = Software_CDS::isPPC(amapconc) ?  PIR_PPC : PIR_STD;

      new_product = ProcDetFactory::CreateNewProcedure(_procState.CassetteType(),
                                                       true,
                                                       pir_t);

      // and yes, this repeats the previous calculcation, with some side
      // effects, see comment above.
      new_product->setMyAMAPPltYieldRange(def.amap_platelet_size, amapmin, amapmax, amapconc) ;

      new_product->initializeAmapPlt(
         amapmax,                   // current yield -- assume max at initial
         def.amap_platelet_size,
         def.plasma_volume,                        // ml
         def.rbc_dose,                             // ml
         def.rbc_crit,                             // percent, e.g. 80
         _config.getRSF(),
         def.max_procedure_time,                  // minutes
         def.blood_type,                          // or'd enums
         def.designator,                          // primary, secondary, etc.
         _config.isPltMeteringOn(),               // metered plt storage
         MssRBCSwitch,                            // metered rbc storage
         def.rbc_mss_volume,                      // rbc metered volume
         procedure_number);


   }
   else // Handle the Non-AMAP Platelet case
   {
      bool isCollectingPlatelets = def.platelet_volume > 0.0f;
      if (isCollectingPlatelets)
      {
         float conc = (100000.0f * def.yield / def.platelet_volume);
         pir_t = Software_CDS::isPPC(conc) ?  PIR_PPC : PIR_STD;
      }

      new_product = ProcDetFactory::CreateNewProcedure(_procState.CassetteType(),
                                                       isCollectingPlatelets,
                                                       pir_t);
      new_product->initialize(
         def.yield,              // *1e11, e.g. 3.5
         def.platelet_volume,    // ml
         def.plasma_volume,      // ml
         0.0f,                   // plasma product includes AC
         def.rbc_dose,           // ml
         def.rbc_crit,           // percent, e.g. 80
         _config.getRSF(),
         def.max_procedure_time,              // minutes
         def.blood_type,                      // or'd enums
         def.designator,                      // primary, secondary, etc.
         MssPLTSwitch,                        // metered plt storage
         PltSSTargetVolume,                   // plt metered volume
         def.pct_carryover,
         MssRBCSwitch,                         // metered rbc storage
         def.rbc_mss_volume,                   // rbc metered volume
         procedure_number);

   }
   return new_product;
}



void Predict::FillInRegionConfig (int procindex)
{
   DataLog(log_level_predict_debug) << " Predict::FillInRegionConfig()" << endmsg;


   PIRType          pir_t       = PIR_STD;
   procedureDetail* new_product = 0;

   //
   //
   //   Add in FFP procedure
   //
   // NOTE: new_product is a non-owning pointer; the newly created procedure
   // objects are owned by the _procedures list.
   new_product = ProcDetFactory::CreateNewProcedure(_procState.CassetteType(),
                                                    false,
                                                    pir_t); // new procedureDetailPltPlsRBC;
   new_product->initialize(0.0f,                            // no yield
                           0.0f,                            // platelet volume
                           PLASMA_FFP_FLAG,                 // _config.getFFPVolume(), // plasma ml
                           0.0f,                            // plasma product includes AC
                           0.0f,                            // rbc dose
                           0.0f,                            // rbc crit
                           _config.getRSF(),
                           _config.getMaxProcedureTime(),
                           BT_ALL,
                           1,                      // designator
                           false,                  // metered plt storage
                           0,                      // plt metered volume
                           0,                      // percent carryover
                           false,                  // metered rbc storage
                           0,                      // rbc metered volume
                           procindex++);
   new_product->setCannotBeDisqualifiedAsDuplicate(true);
   _procedures.insert(new_product);

   //
   //
   //   Add in AMAP plasma procedure
   //
   new_product = ProcDetFactory::CreateNewProcedure(_procState.CassetteType(),
                                                    false,
                                                    pir_t); // new procedureDetailPltPlsRBC;
   new_product->initialize(0.0f,                            // no yield
                           0.0f,                            // platelet volume
                           PLASMA_AMAP_FLAG,                // plasma ml
                           0.0f,                            // plasma product includes AC
                           0.0f,                            // rbc dose
                           0.0f,                            // rbc crit
                           _config.getRSF(),
                           _config.getMaxProcedureTime(),
                           BT_ALL,
                           1,                      // designator
                           false,                  // metered plt storage
                           0,                      // plt metered volume
                           0,                      // percent carryover
                           false,                  // metered rbc storage
                           0,                      // rbc metered volume
                           procindex++);
   new_product->setCannotBeDisqualifiedAsDuplicate(true);
   _procedures.insert(new_product);

   //
   //
   //   Now add in plasma-only procedures.  Start at the configured plasma volume limit and work
   //   backwards in 50ml increments.  Note in the case where the volumes do not include AC this
   //   is subverting the meaning of the AMAP configurations, which in config definitions always
   //   include AC.
   //
   //
   int        plasmavolume = Software_CDS::GetInstance().getValueFeature(PlasmaLadderMax);
   int        plasmamin    = Software_CDS::GetInstance().getValueFeature(PlasmaLadderMin);
   const bool extra_plasma_procedures_do_not_include_ac = Software_CDS::GetInstance().getFeature(ExtraPlasmaNoAc);
   while (plasmavolume >= plasmamin)
   {

      new_product = ProcDetFactory::CreateNewProcedure(_procState.CassetteType(),
                                                       false,
                                                       pir_t);     // new procedureDetailPltPlsRBC;

      int plasma_product_volume = 0;
      if (extra_plasma_procedures_do_not_include_ac)
      {
         plasma_product_volume = plasmavolume;
      }
      new_product->initialize(0.0f,                         // no yield
                              0.0f,                         // platelet volume
                              (float)plasmavolume,          // plasma ml
                              (float)plasma_product_volume, // override in case this product does not include AC
                              0.0f,                         // rbc dose
                              0.0f,                         // rbc crit
                              _config.getRSF(),
                              _config.getMaxProcedureTime(),
                              BT_ALL,
                              1,                         // designator
                              false,                     // metered plt storage
                              0,                         // plt metered volume
                              0,                         // percent carryover
                              false,                     // metered rbc storage
                              0,                         // rbc metered volume
                              procindex++);
      new_product->setCannotBeDisqualifiedAsDuplicate(true);
      _procedures.insert(new_product);

      plasmavolume -= 50;
   }

}


void Predict::SetAllUnoptimized ()
{
   DataLog(log_level_predict_debug) << " Predict::SetAllUnoptimized()" << endmsg;
   checkCassetteTypes();

   _procedures.setAllUnoptimized();

}

void Predict::setPredictTargets ()
{

   DataLog(log_level_predict_debug) << " Predict::setPredictTargets()" << endmsg;
   //
   // Send all the run targets to Proc land
   //
   if (_selectedProc)
   {
      // The Predict CDS object may be written to more than once in this block;
      // lock it here to ensure that no other tasks manage to read it in an
      // inconsistent state.  (IT 13212, although this probably isn't the root
      // cause of that issue --DGS)
      ScopedCDSLock lock(_procState._predictCDS);

      substates_t   info;
      _TargetsCDS.ProcTimeTarget.Set(_selectedProc->getPredTp());    // always set, see IT6706

      if (!_blockTargets)
      {
         DataLog(log_level_predict_debug) << "Optimization target update is not blocked" << endmsg;

         _selectedProc->SetTargetsCds(_TargetsCDS);
         _selectedProc->setNoCollectFirstReturn();
         info = _selectedProc->SetTargets();

         // Begin IT 7192
         if (_curProc)
         {
            _currentDisplayedValues.estimated_platelets       = _curProc->getYtarg();
            _currentDisplayedValues.estimated_platelet_volume = _curProc->getPredVc();
            _currentDisplayedValues.estimated_plasma          = _curProc->getPredVp();
            int lastsubstate = _curProc->getNumOfSubstates() - 1;        // zero-based index
            _currentDisplayedValues.estimated_plasma_no_ac    = _curProc->getPredVp() - _curProc->getSubstate(lastsubstate).AccumVacp;
            _currentDisplayedValues.estimated_rbcs            = _curProc->getPredRBCVolume();
            _currentDisplayedValues.estimated_rbc_dose        = _curProc->getPredRBCDose();
            _currentDisplayedValues.procedure_time            = _curProc->getPredTp();
         }
         // End IT 7192


      }
      else
      {
         DataLog(log_level_predict_debug) << "Optimization target update is blocked" << endmsg;
         _procState._predictCDS.SubstateInfo.Get(&info);
      }

      // 10227 adjusments need this to happen even if blocked
      _selectedProc->copySubstateInfoToCds(info);

      // CDS lock goes out of scope here, unlocking the Predict_CDS.
   }
}

int Predict::doRepredictForAdjustments (bool isAfAdjustment)
{
   DataLog(log_level_predict_debug) << "Predict::doRepredictForAdjustments()" << endmsg;
   const int repredict = _config.AdjustConfig(_procState.AdjQinCap(),
                                              _procState.AdjQrpCap(),
                                              _procState.AdjIrCap(),
                                              _procState.AdjRatioCap(),
                                              isAfAdjustment );
   if ( repredict == -1)
   {
      DataLog(log_level_predict_error) << "Illegal AC Rate " << _config.getKeyACRate() << endmsg;
#ifndef __WIN32__
      anAlarmMsg alarm(CONFIGDAT_VALUE_OUT_OF_RANGE);
      alarm.setAlarm();
#endif
   }

   DataLog(log_level_predict_debug).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_predict_debug).precision(3);
   DataLog(log_level_predict_debug) << "Adjustment caps:Qin Qrp IR R: "
                                    << _procState.AdjQinCap() << " "
                                    << _procState.AdjQrpCap() << " "
                                    << _procState.AdjIrCap() << " "
                                    << _procState.AdjRatioCap()
                                    << endmsg;
   return repredict;

}


void Predict::UpdateProcData ()
{
   //   DataLog(log_level_predict_debug) << " Predict::UpdateProcData()" << endmsg;
   _procState.Update();

   if (_AMAP_PLATELETS_ON)
      calcMaxAMAPplateletYield();

   _currentPltMss = _TargetsCDS.MeterPlatelets.Get();
}

void Predict::checkCassetteTypes ()
{
   //
   // check cassette type
   //
   cassette::cassetteType currentCassette = _procState.CassetteType();
   DataLog(log_level_predict_debug) << " Predict::checkCassetteTypes(), current cassette = "
                                    << currentCassette << endmsg;

   for (iterator iter = _procedures.begin(); iter != _procedures.end(); ++iter)
   {
      procedureDetail*       procedure           = *iter;

      cassette::cassetteType recommendedCassette =
         ProcDetFactory::calcRecommendedCassette(currentCassette, *procedure);


      cassette::cassetteType requiredCassette = procedure->getRecommendedCassette();

      // Bad naming here... 'required' is the cassette that the procedure thinks
      // is needs.  'recommended' is the cassette that's actually loaded, or
      // 'required' if it hasn't been loaded yet.


      //   If the procedure doesn't match the desired cassette then
      //   replace it with the appropriate one
      if (recommendedCassette != requiredCassette)
      {
         DataLog(log_level_predict_debug) << "Predict: For procedure "
                                          << procedure->getProcedureNumber()
                                          << ", cassette (loaded) " << recommendedCassette
                                          << " != " << requiredCassette
                                          << " (recommended), REPLACING PROCEDURE."
                                          << endmsg;


         procedureDetail* newguy = NULL;
         if (recommendedCassette == cassette::PLTPLSRBC)
         {
            PIRType pir_t = PIR_STD;
            if (procedure->getProcFixedCollectVol() > 0.0f)
            {
               float conc = (100000.0f * procedure->getDesiredYield() / procedure->getProcFixedCollectVol());
               if ( Software_CDS::isPPC(conc) )
                  pir_t = PIR_PPC;
               else
                  pir_t = PIR_STD;
            }
            newguy = new procedureDetailPltPlsRBC(pir_t);
         }
         else
         {
            newguy = new procedureDetailRBCPls () ;
         }
         newguy->copy(*procedure);
         _procedures.replace(newguy);

      }
      else
      {
         DataLog(log_level_predict_debug) << "Predict: For procedure "
                                          << procedure->getProcedureNumber()
                                          << ", cassette (loaded) " << recommendedCassette
                                          << " matches; no change required."
                                          << endmsg;
      }
   } // end loop
}


INTERNAL_PREDICT_RESULTS Predict::PreCheck (const procedureDetail& pd)
{
   DataLog(log_level_predict_debug) << " Predict::PreCheck()" << endmsg;
   if (MeteringStatusLocked(pd))
   {
      return PRODUCT_METERING_STATUS_LOCKED;
   }
   return GOOD_PRODUCT;
}

INTERNAL_PREDICT_RESULTS Predict::PostCheck (const procedureDetail& pd)
{
   DataLog(log_level_predict_debug) << " Predict::PostCheck()" << endmsg;
   if ((!_procState.getOptimizingSelectedProc()) && checkIfDuplicate(pd))
   {
      return DUPLICATE_PRODUCT;
   }
   return pd.getStatus();
}

void Predict::predictProcedure (procedureDetail& pd)
{

   //  DataLog(log_level_predict_debug) << " Predict::predictProcedure()" << endmsg;
   _procState._hypovolemia.initialize(pd.getRecommendedCassette());
   // DataLog(log_level_predict_info) << " Predict::predictProcedure() cassette is " << pd.getRecommendedCassette() << endmsg;

   //
   //   Check to see if we are optimizing the current procedure.  We need
   //   this information to properly determine hypovolemia constraints.
   //
   _procState.setOptimizingSelectedProc ( (_selectedProc && (_selectedProc->getProcedureNumber() == pd.getProcedureNumber()) && (_procState.ProcRunTime() > 0.0f)) );


   //////////////////////////////////////////////////////////////////////////////
   // jph   figure AMAP platelet  for each proc.
   if (pd.getPltAmap() && !_AMAP_PLATELETS_PROCESSING )
   {

      // DataLog(log_level_predict_info) << "amapp: Process AMAP Plt procedures to find limits and possibilities:" << endmsg;
      // DataLog(log_level_predict_info) << "amapp: procedure " << pd.getProcedureNumber() << endmsg;
      predictPltAmap(pd);
   }
   else
   {
      // DataLog(log_level_predict_info) << "amapp: amapp processing finished " << pd.getProcedureNumber() << endmsg;
      //////////////////////////////////////////////////////////////////////////////

      // Predict level
      pd.setStatus(PreCheck(pd));

      if (_procState.CassetteState() >= ProcTrimaSet_CDS::RINSEBACK)
      {
         pd.setStatus(GOOD_PRODUCT);
         //  Set targets to current values (IT6687)
         if (_selectedProc)
         {
            _selectedProc->setPredTp(_procState.ProcRunTime());
            _selectedProc->setYtarg(_procState.Yield() );
            _selectedProc->setPredVc(_procState.CollectVolume());
            _selectedProc->setMSSPlt(_currentPltMss);
            _selectedProc->setPredVp(_procState.PlasmaVolume());
            _selectedProc->setPredRBCVolume(_procState.RbcVolume());
            _selectedProc->setPredRBCDose(_procState.RbcDose());
            _selectedProc->setInfusionRate(static_cast<float>( 0.93f * _config.getReferenceACRate() / (1.0f - exp((double)(-_procState.ProcRunTime() / 37.0f)))) );

            if (_selectedProc->getInfusionRate()> _config.getACRate())
               _selectedProc->setInfusionRate(_config.getACRate() );
         }
      }
      else if ((pd.getStatus() == GOOD_PRODUCT) || _procState.getOptimizingSelectedProc())
      {
         pd.Optimize();
      }

      pd.setStatus(PostCheck(pd));

      if (pd.getStatus() != INCORRECT_DESIGNATOR)
         pd.setOptimized(1);

      LogProcedure(pd);
      // SCOTTS HACK
      // taskDelay(150);
   }

   _procState.setPredictRequest(CLEAR_ALL);
}

void Predict::LogProcedure (procedureDetail& pd)
{
   logConvergenceTrail(pd);

   DataLog(log_level_predict_info).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_predict_info).precision(3);

   if (_COMP_PLS_ON && _config.isCompPlasmaOn())
   {


      if (_amapPltItermediatePredict && pd.getStatus() == GOOD_PRODUCT)
      {

         DataLog(log_level_predict_external) << "procedure " << pd.getProcedureNumber()
                                             << " reports status Intermediate(" << pd.getStatus() << ")"
                                             << " (" << pd.getYtarg()
                                             << " " << pd.getPredVc()
                                             << " " << pd.getPredVp()
                                             << " " << pd.getPredRBCDose() << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume()
                                             << " " << pd.getPredTp()
                                             << " " << (( (pd.getPredVp() > 0.0f) && pd.getMSSPlt() ) ? " CompPls=" : "")
                                             << (( (pd.getPredVp() > 0.0f) && pd.getMSSPlt() ) ? ( (pd.getPredVp() > pd.getMSSPltVolume()) ?  pd.getMSSPltVolume()  : pd.getPredVp()  ) : 0)
                                             << ") "
                                             << (pd.getMSSPlt() ? "MP/" : "-/")
                                             << (pd.getMSSRbc() ? "MR/" : "-/")
                                             << ") "
                                             << (pd.getMSSPlt() ? pd.getMSSPltPctCarryover() : 0)
                                             << " "
                                             << (pd.getMSSRbc() ? pd.getMSSRbcVolume() : 0)
                                             << endmsg;


      }
      else
      {

         DataLog(log_level_predict_external) << "procedure " << pd.getProcedureNumber()

                                             << " reports status " << pd.getStatus()

                                             << " (" << pd.getYtarg()
                                             << " " << pd.getPredVc()
                                             << " " << pd.getPredVp()
                                             << " " << pd.getPredRBCDose() << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume()
                                             << " " << pd.getPredTp()
                                             << " " << (( (pd.getPredVp() > 0.0f) && pd.getMSSPlt() ) ? " CompPls=" : "")
                                             << (( (pd.getPredVp() > 0.0f) && pd.getMSSPlt() ) ? ( (pd.getPredVp() > pd.getMSSPltVolume()) ?  pd.getMSSPltVolume()  : pd.getPredVp()  ) : 0)
                                             << ") "
                                             << (pd.getMSSPlt() ? "MP/" : "-/")
                                             << (pd.getMSSRbc() ? "MR/" : "-/")
                                             << ") "
                                             << (pd.getMSSPlt() ? pd.getMSSPltPctCarryover() : 0)
                                             << " "
                                             << (pd.getMSSRbc() ? pd.getMSSRbcVolume() : 0)
                                             << endmsg;
      }





   }
   else if (_amapPltItermediatePredict && pd.getStatus() == GOOD_PRODUCT )
   {
      LogProcedureIntermediateAmapPlt(pd);
   }
   else
   {
      DataLog(log_level_predict_external) << "procedure " << pd.getProcedureNumber()

                                          << " reports status " << pd.getStatus()

                                          << " (" << pd.getYtarg()
                                          << " " << pd.getPredVc()
                                          << " " << pd.getPredVp()
                                          << " " << pd.getPredRBCDose() << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume()
                                          << " " << pd.getPredTp()
                                          << ") "
                                          << (pd.getMSSPlt() ? "MP/" : "-/")
                                          << (pd.getMSSRbc() ? "MR/" : "-/")
                                          << ") "
                                          << (pd.getMSSPlt() ? pd.getMSSPltPctCarryover() : 0)
                                          << " "
                                          << (pd.getMSSRbc() ? pd.getMSSRbcVolume() : 0)
                                          << endmsg;
   }

   if (pd.getconvergenceCtr() > 0)
      DataLog(log_level_predict_info) << "Convergence Ctr " << pd.getconvergenceCtr() << endmsg;

   if (pd.getCannotBeDisqualifiedAsDuplicate())
      DataLog(log_level_predict_info) << "procedure " << pd.getProcedureNumber() <<
         " cannot be disqualified as a duplicate." << endmsg;

   if (pd.getProcPlasmaProductVol() > 0.0f)
      DataLog(log_level_predict_info) << "procedure " << pd.getProcedureNumber() <<
         " targeted to plasma volume without AC of " << pd.getProcPlasmaProductVol() << " mL." << endmsg;

   if (pd.getPlasmaLimitedByHct())
      DataLog(log_level_predict_info) << "procedure " << pd.getProcedureNumber() <<
         " plasma collection was limited by maximum hematocrit." << endmsg;

#if CPU != SIMNT
   if (_procState.getOptimizingSelectedProc() && pd.getStatus() != PRODUCT_ACHIEVED)
#endif
   pd.LogTargets();
}

void Predict::LogProcedureIntermediateAmapPlt (procedureDetail& pd)
{
   DataLog(log_level_predict_external) << "procedure " << pd.getProcedureNumber()
                                       << " reports status Intermediate(" << pd.getStatus() << ")"
                                       << " (" << pd.getYtarg()
                                       << " " << pd.getPredVc()
                                       << " " << pd.getPredVp()
                                       << " " << pd.getPredRBCDose() << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume()
                                       << " " << pd.getPredTp()
                                       << ") "
                                       << (pd.getMSSPlt() ? "MP/" : "-/")
                                       << (pd.getMSSRbc() ? "MR/" : "-/")
                                       << ") "
                                       << (pd.getMSSPlt() ? pd.getMSSPltPctCarryover() : 0)
                                       << " "
                                       << (pd.getMSSRbc() ? pd.getMSSRbcVolume() : 0)
                                       << endmsg;
}

void Predict::logConvergenceTrail (const procedureDetail& pd) const
{
   char trail_log[100];         // I'm not happy about hard coding this, but this is the way it used
                                // to be (when it was being unnecessarily 'newed' up in the ctor). --DGS
   // DataLog(log_level_predict_debug) << " Predict::logConvergenceTrail()" << endmsg;
   int nTimes = pd.getnumOfTimes();
   if (nTimes > 1)
   {
      // strlen "procedure xx "
      sprintf(trail_log, "%s %2d %s:", "procedure", pd.getProcedureNumber(), "convergence trail");
      for (int i = 0; i< nTimes; i++ )
         sprintf(trail_log + strlen(trail_log), "%d ",  pd.getConvergenceTrail(i));
      DataLog(log_level_predict_debug) << trail_log << endmsg;
   }
}

bool Predict::checkIfDuplicate (const procedureDetail& proc)
{
   //  DataLog(log_level_predict_debug) << " Predict::checkIfDuplicate()" << endmsg;
   static const double YIELD_MARGIN             = 0.095; // platelets
   static const float  CONCENTRATION_MARGIN     = 100.0f;
   static const double PLASMA_MARGIN            = 10.0;
   static const double RBC_MARGIN               = 5.0;
   static const float  TIME_MARGIN              = 2.0;
   static const float  PERCENT_CARRYOVER_MARGIN = 2.0; // percent
   static const float  RBC_SS_VOLUME_MARGIN     = 10.0;

   //
   //
   //  Shortcircuit the check if the procedure has been flagged as being
   //  always required in the procedure priority list
   //
   if (proc.getCannotBeDisqualifiedAsDuplicate()) return 0;

   const_iterator iter;

   bool           isDuplicate = false; // innocent until proven guilty

   for (iter = _procedures.begin(); iter != _procedures.end(); iter++)
   {
      int currentProcNum = proc.getProcedureNumber();
      int iterProcNum    = (*iter)->getProcedureNumber();
      //
      // check to see if done checking for duplicates
      //
      if (currentProcNum == iterProcNum) break;     // all done

      //
      // If not a valid procedure, skip check
      //
      if ((*iter)->getStatus() != GOOD_PRODUCT) continue;

      bool currentIsPas = proc.getMSSPlt();
      bool currentIsRas = proc.getMSSRbc();
      bool iterIsPas    = (*iter)->getMSSPlt();
      bool iterIsRas    = (*iter)->getMSSRbc();

      bool aasMatchup   = (currentIsPas == iterIsPas) && (currentIsRas == iterIsRas);

      DataLog (log_level_predict_debug) << "Comparing procedure " << currentProcNum
                                        << " (RAS:" << currentIsRas << ";PAS:" << currentIsPas << ") with procedure "
                                        << iterProcNum << " (RAS:" << iterIsRas << ";PAS:" << iterIsPas << ") -> AAS matchup = "
                                        << aasMatchup << "." << endmsg;

      if (aasMatchup)    // Only compare procedures if AAS matches up.
      {
         // These bools determine whether we want to match up the AAS volumes
         bool compareRasVol    = currentIsRas && iterIsRas;
         bool comparePasVol    = currentIsPas && iterIsPas;

         bool timeMatchup      = (fabs((*iter)->getPredTp() - proc.getPredTp()) < TIME_MARGIN);
         bool rbcVolMatchup    = (fabs((*iter)->getPredRBCVolume() - proc.getPredRBCVolume()) < RBC_MARGIN);
         bool plasmaVolMatchup = (fabs((*iter)->getPredVp() - proc.getPredVp()) < PLASMA_MARGIN);
         bool pltYieldMatchup  = (fabs((*iter)->getYtarg() - proc.getYtarg()) < YIELD_MARGIN);
         bool pltConcMatchup   = (fabs((*iter)->getProcPlateletProdCon() - proc.getProcPlateletProdCon()) < CONCENTRATION_MARGIN);
         bool pasVolMatchup    = ((!comparePasVol) || (abs((*iter)->getMSSPltPctCarryover() - proc.getMSSPltPctCarryover())  < PERCENT_CARRYOVER_MARGIN ));
         bool rasVolMatchup    = ((!compareRasVol) || (abs((*iter)->getMSSRbcVolume() - proc.getMSSRbcVolume())  < RBC_SS_VOLUME_MARGIN ));

         bool completeMatchup  =        timeMatchup
                                 && rbcVolMatchup
                                 && plasmaVolMatchup
                                 && pltYieldMatchup
                                 && pltConcMatchup
                                 && pasVolMatchup
                                 && rasVolMatchup;

         if (completeMatchup)
         {
            DataLog (log_level_predict_debug) << "Procedures " << currentProcNum << " and "
                                              << iterProcNum << " match perfectly. " << endmsg;

            isDuplicate = true;
            break;
         }
         else
         {
            DataLog (log_level_predict_debug) << "Procedures " << currentProcNum << " and "
                                              << iterProcNum << " are different (";

            if (!timeMatchup)      { DataLog (log_level_predict_debug) << "time "     ; }
            if (!rbcVolMatchup)    { DataLog (log_level_predict_debug) << "rbcVol "   ; }
            if (!plasmaVolMatchup) { DataLog (log_level_predict_debug) << "plasVol "  ; }
            if (!pltYieldMatchup)  { DataLog (log_level_predict_debug) << "pltYield " ; }
            if (!pltConcMatchup)   { DataLog (log_level_predict_debug) << "pltConc "  ; }
            if (!pasVolMatchup)    { DataLog (log_level_predict_debug) << "pasVol "   ; }
            if (!rasVolMatchup)    { DataLog (log_level_predict_debug) << "rasVol "   ; }

            DataLog (log_level_predict_debug) << ")" << endmsg;
         }
      }
   }    // end for loop

   return isDuplicate;
}



// Talk to jerry
bool Predict::MeteringStatusLocked (const procedureDetail& pd)
{
   //  DataLog(log_level_predict_debug) << " Predict::MeteringStatusLocked()" << endmsg;
   //
   //
   //  Filter out non-pas plt  procedures if we have started a pas plt collection
   //

   if (_selectedProc)  // if we have made a selection
   {


      if ( _config.isPltMeteringOn() )
      {


         if (_procState.Yield() > 0.05)
         {

            if (CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT))
            {


               if (_selectedProc->getMSSPlt() && !pd.getMSSPlt() && pd.getProcPlateletProdCon() > 0.0f)       // & ITS A plt COLLECTION
               {


                  DataLog(log_level_proc_info) << "PAS to nonPAS procedure disQualified:"
                                               <<  "setIsPas"  <<    CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT)
                                               <<  "pd.getMSSPlt()"  << pd.getMSSPlt()
                                               <<  "_selectedProc->getMSSPlt()"  << _selectedProc->getMSSPlt()
                                               <<  " pd.getProcPlateletProdCon()"  <<  pd.getProcPlateletProdCon()
                                               <<  " _ProcVolumesCDS->PlateletYield.Get()"  <<  _procState.Yield()
                                               << endmsg;


                  return true;
               }
            }

            if (CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT))
            {
               if (!_selectedProc->getMSSPlt() && pd.getMSSPlt())
               {


                  DataLog(log_level_proc_info) << "nonPAS to PAS procedure disQualified:"
                                               <<  "setIsPas"  << CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT)
                                               <<  "pd.getMSSPlt()"  << pd.getMSSPlt()
                                               <<  "_selectedProc->getMSSPlt()"  << _selectedProc->getMSSPlt()
                                               <<  " pd.getProcPlateletProdCon()"  <<  pd.getProcPlateletProdCon()
                                               <<  " _ProcVolumesCDS->PlateletYield.Get()"  <<  _procState.Yield()
                                               << endmsg;


                  return true;
               }

            }

         }

      }

   }
   return false;
}



#ifndef __WIN32__

// Utility function to set all target values to zero; used by the predict task
// to reset targets when a donor is rejected.
void Predict::reset_targets ()
{
   _TargetsCDS.ProcTimeTarget.Set(0.0f);
   _TargetsCDS.PlateletYieldTarget.Set(0.0f);
   _TargetsCDS.PlateletVolumeTarget.Set(0.0f);
   _TargetsCDS.PlasmaTarget.Set(0.0f);
   _TargetsCDS.RbcVolumeTarget.Set(0.0f);
   _TargetsCDS.RbcCritTarget.Set(0.0f);
   _TargetsCDS.RbcDoseTarget.Set(0.0f);
   _TargetsCDS.PlateletConcenTarget.Set(0.0f);
   _TargetsCDS.InfusionRateTarget.Set(0.0f);
   _TargetsCDS.RequiresRepFluid.Set(0);
   _TargetsCDS.ProcNumber.Set(-1);
   _TargetsCDS.PlateletConcenTarget.Set(AMAP_PLT_NONE);

   _TargetsCDS.SelectedPlateletYield.Set(0.0f);
   _TargetsCDS.SelectedPlateletVolume.Set(0.0f);
   _TargetsCDS.SelectedPlateletConcen.Set(0.0f);
   _TargetsCDS.SelectedPlasmaVolume.Set(0.0f);
   _TargetsCDS.SelectedRbcVolume.Set(0.0f);
}


void Predict::process_adjustment (bool isAfAdjustment)
{
   DataLog(log_level_predict_debug) << "Predict::Adjustment" << endmsg;
   doRepredictForAdjustments(isAfAdjustment);
   _blockTargets = true;
   SetAllUnoptimized();

   if (_selectedProc)     // should always be valid
      predictProcedure(*_selectedProc);
}


void Predict::ProcData_Repredict_Only ()
{
   DataLog(log_level_predict_debug) << " REPREDICT_ONLY" << endmsg;
   if (_selectedProc && (_selectedProc->AlgSubstate(_procState.SubState()) > 0))
   {
      _selectedProc->setOptimized(0);
      _selectedProc->setStatus(UNOPTIMIZED);
      predictProcedure(*_selectedProc);
   }

   //
   //
   //  IT5477: Since we now allow deletion of the selected procedure ensure
   //  that we are always have a "selected" procedure when the system is
   //  actually processing.
   //
   if ((!_selectedProc) && (_procState.ProcRunTime() > 0.0f))
   {
      anAlarmMsg noselected(PREDICT_NO_SELECTED_PROCEDURE);
      noselected.setAlarm();
   }
}


void Predict::reset_PIR_flags ()
{
   for (iterator iter = _procedures.begin(); iter != _procedures.end(); iter++)
   {
      if ((*iter)->GetPIRRequired())
      {
         DataLog(log_level_predict_debug) << "Predict: Reset PIR flag for Product ("
                                          << (*iter)->getProcedureNumber()
                                          << ")" << endmsg;
         (*iter)->SetPIRRequired(false);
      }
   }
}

void Predict::ProcData_Donor_Info_Changed ()
{
   reset_PIR_flags();

   SetAllUnoptimized();


   //////////////////////////////////////////////////////////////////////////////
   // jph re-calc the limits of amap p yield
   if (_AMAP_PLATELETS_ON)
   {
      DataLog(log_level_predict_debug) << "AMAPP: Recalc the Max AMAP Plat yield...." << endmsg;
      calcMaxAMAPplateletYield();
   }

   //////////////////////////////////////////////////////////////////////////////
   //   Reoptimize current procedure using new donor parameters.  If this
   //   hasn't caused a procedure list change then we can transition back
   //   to the run screen.
   //
   _blockTargets = true;
   if (_config.isAutoFlowOn() == true)
   {
      UpdateAutoFlowTimerMsg updateTime(MessageBase::SEND_LOCAL);
      updateTime.send(4);
      DataLog(log_level_predict_debug) << "sending Autoflow time update"   << endmsg;
   }
}



void Predict::FeatureChangedReceived ()
{
   // check for AMAP Platelets feature
   if (Software_CDS::GetInstance().getFeature(AmapPlateletsAvailable) != _AMAP_PLATELETS_ON)
   {
      // toggle the setting
      _AMAP_PLATELETS_ON = !_AMAP_PLATELETS_ON;

      if ( _AMAP_PLATELETS_ON )
      {
         DataLog(log_level_predict_debug) << "amapp: AMAP PLT is now ON" << endmsg;
         _stepSize = cobeConfig.YieldStepSize;
      }
      else
      {
         DataLog(log_level_predict_debug) << "amapp: AMAP PLT is now OFF" << endmsg;
         _stepSize = 0.0;
      }
   }

   // check for Complementary Plasma
   if (Software_CDS::GetInstance().getFeature(ComplementaryPlasmaAvailable) != _COMP_PLS_ON)
   {
      // toggle the setting
      _COMP_PLS_ON = !_COMP_PLS_ON;

      if (_COMP_PLS_ON)
      {
         DataLog(log_level_predict_debug) << "Complementary PLS is now ON" << endmsg;
      }
      else
      {
         DataLog(log_level_predict_debug) << "Complementary PLS is now OFF" << endmsg;
      }
   }

   // On exit from Config screen, ConfigurationReceived() will trigger adjustments
}


void Predict::select ()
{
   // DataLog(log_level_predict_debug) << " Predict::select()" << endmsg;
   _blockTargets = false;
   _selectedProc = _curProc;


   // IT 8908
   //    if(_selectedProc->getRecommendedCassette() == cassette::RBCPLS)
   //    {
   //          substates_t info;
   //          _selectedProc->restoreSubstateInfoFromCds(info);
   //     }


   // Remember if we were metering PLT.
   if (_TargetsCDS.MeterPlatelets.Get()
       && _config.isPltMeteringOn()
       && CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT)
       &&(_procState.Yield() > 0.05 )  // why 0.05?  cause thats when GUI/predict puts up a value
       )
   {

      DataLog(log_level_proc_info) << "Remember PAS metering"  << endmsg;

      // only set if we didn't have an original
      if (_TargetsCDS.PlateletConcenTarget.Get() < 1.0f)
      {
         _selectedProc->setMSSPltPctCarryover((int)_TargetsCDS.PltPctCarryover.Get() );
         DataLog(log_level_proc_info) << "Carry the mss flag : carryover = "
                                      << _selectedProc->getMSSPltPctCarryover()
                                      << endmsg;
      }

      // if metering is off carryover the PPCO
      if (!_selectedProc->getMSSPlt())
      {
         _selectedProc->setMSSPltPctCarryover(_TargetsCDS.PltPctCarryover.Get() );
         DataLog(log_level_proc_info) << "Carry the mss PPCO : carryover = "
                                      << _selectedProc->getMSSPltPctCarryover()
                                      << endmsg;
      }

      _selectedProc->setMSSPltVolume(_TargetsCDS.PltSolutionTarget.Get() ) ;
      _selectedProc->setOriginalPltVolume(_TargetsCDS.OriginalVolume.Get() ) ;
      _selectedProc->setOriginalPltYeild(_TargetsCDS.OriginalYeild.Get()) ;
      _selectedProc->setMSSPlt(true);

      DataLog(log_level_proc_info) << "Carry Over PAS info :"
                                   << "PPCO -> " << _TargetsCDS.PltPctCarryover.Get()
                                   << "_TargetsCDS.PlateletConcenTarget ->" << _TargetsCDS.PlateletConcenTarget.Get()
                                   << "Original SS volm ->" << _TargetsCDS.PltSolutionTarget.Get()
                                   << "Original Plt volm ->" << _TargetsCDS.OriginalVolume.Get()
                                   << "Original Plt Yeild ->" << _TargetsCDS.OriginalYeild.Get()
                                   << "_selectedProc->setMSSPltPctCarryover ->" << _selectedProc->getMSSPltPctCarryover()
                                   << "_selectedProc->getProcPlateletProdCon ->" << _selectedProc->getProcPlateletProdCon()
                                   << endmsg;
   }

   setPredictTargets();

   // IT 12332: these fields in the target CDS must be updated when, and only
   // when, the operator selects a procedure.  [This is part of the "contract
   // with the operator" --DGS]
   _TargetsCDS.SelectedPlateletYield.Set(_currentDisplayedValues.estimated_platelets);
   _TargetsCDS.SelectedPlateletVolume.Set(_currentDisplayedValues.estimated_platelet_volume);
   _TargetsCDS.SelectedPlateletConcen.Set(_selectedProc->getProcPlateletProdCon());
   _TargetsCDS.SelectedPlasmaVolume.Set(_currentDisplayedValues.estimated_plasma);
   _TargetsCDS.SelectedRbcVolume.Set(_currentDisplayedValues.estimated_rbcs);


   // --- Check for concentration out of tolerance condition ---
   //
   //  If this product does not match concentration within our 5%
   //  volume specification then we need to assert a warning (IT 2273)
   //
   // (Note from IT 13576: we warn even if the predicted platelet yeild is zero,
   // on the basis that this is a particularly odd situation and we want to make
   // sure the operator knows about it.)

   // We only warn about a procedure once; if we haven't warned about this one,
   // reset the flag.
   if (_selectedProc->getProcedureNumber() != _productGivenWarning)
   {
      _concentrationWarningGiven = false;
   }

   float pltErr = 0.0f; // also for non-platelet collections
   if (_selectedProc->getProcFixedCollectVol() > 0.0f)
   {
      pltErr = (float)fabs(_selectedProc->getProcFixedCollectVol() - _selectedProc->getPredVc()) / _selectedProc->getProcFixedCollectVol();
   }

   if (/*IT 8005 - No concentration warning for PAS*/ (!_selectedProc->getMSSPlt()) &&
                                                      (pltErr > Config::__MaxConcentrationError) &&
                                                      (!_concentrationWarningGiven))
   {
      anAlarmMsg concAlarm(CONCENTRATION_OUT_OF_TOLERANCE);
      concAlarm.setAlarm();
      _concentrationWarningGiven = true;
      _productGivenWarning       = _selectedProc->getProcedureNumber();
   }

   if (_COMP_PLS_ON  && _config.isCompPlasmaOn())
   {


      //
      // Log the selected procedure
      //
      DataLog(log_level_predict_external) << "Predict: selected product " << _selectedProc->getProcedureNumber()
                                          << " (" << _selectedProc->getYtarg()
                                          << " " << _selectedProc->getPredVc()
                                          << " " << _selectedProc->getPredVp()
                                          << " " << _selectedProc->getPredRBCDose() << "@" << _selectedProc->getPredRBCCrit() << "=" << _selectedProc->getPredRBCVolume()
                                          << " " << _selectedProc->getPredTp()
                                          << " " << (  ( (_selectedProc->getPredVp() > 0.0f) && _selectedProc->getMSSPlt() ) ? " CompPls=" : "")
                                          << (( (_selectedProc->getPredVp() > 0.0f) && _selectedProc->getMSSPlt() ) ? ( (_selectedProc->getPredVp() > _selectedProc->getMSSPltVolume()) ?  _selectedProc->getMSSPltVolume()  : _selectedProc->getPredVp()  ) : 0)
                                          << ") ("
                                          << (_selectedProc->getMSSPlt() ? "MP/" : "-/")
                                          << (_selectedProc->getMSSRbc() ? "MR/" : "-/")
                                          << ") "
                                          << (_selectedProc->getMSSPlt() ? _selectedProc->getMSSPltPctCarryover() : 0)
                                          << " "
                                          << (_selectedProc->getMSSRbc() ? _selectedProc->getMSSRbcVolume() : 0)
         // << (( (_selectedProc->getPredVp() > 0.0f) && _selectedProc->getMSSPlt() ) ?    _selectedProc->getMSSPltVolume() : 0)
         // << (( (_selectedProc->getPredVp() > 0.0f) && _selectedProc->getMSSPlt() ) ? ( (_selectedProc->getPredVp() > _selectedProc->getMSSPltVolume()) ?  _selectedProc->getMSSPltVolume()  : _selectedProc->getPredVp()  ) : 0)
         // << (( (pd.getPredVp() > 0.0f) && pd.getMSSPlt() ) ? ( (pd.getPredVp() > pd.getMSSPltVolume()) ?  pd.getMSSPltVolume()  : pd.getPredVp()  ) : 0)

                                          << endmsg;
   }
   else
   {


      //
      // Log the selected procedure
      //
      DataLog(log_level_predict_external) << "Predict: selected product " << _selectedProc->getProcedureNumber()
                                          << " (" << _selectedProc->getYtarg()
                                          << " " << _selectedProc->getPredVc()
                                          << " " << _selectedProc->getPredVp()
                                          << " " << _selectedProc->getPredRBCDose() << "@" << _selectedProc->getPredRBCCrit() << "=" << _selectedProc->getPredRBCVolume()
                                          << " " << _selectedProc->getPredTp()
                                          << ") ("
                                          << (_selectedProc->getMSSPlt() ? "MP/" : "-/")
                                          << (_selectedProc->getMSSRbc() ? "MR/" : "-/")
                                          << ") "
                                          << (_selectedProc->getMSSPlt() ? _selectedProc->getMSSPltPctCarryover() : 0)
                                          << " "
                                          << (_selectedProc->getMSSRbc() ? _selectedProc->getMSSRbcVolume() : 0)
                                          << " "
                                          << endmsg;

   }


//    DataLog(log_level_predict_info) << "Predict: volume removals:"
//                                    << " Platelet: " << _selectedProc->getPredVc() << " - "
//                                                     << _selectedProc->substate[_selectedProc->getNumOfSubstates()-1].AccumVacc
//                                    << "; Plasma: " << _selectedProc->getPredVp() << " - "
//                                                    << _selectedProc->substate[_selectedProc->getNumOfSubstates()-1].AccumVacp
//                                    << endmsg;
//



   int   lastsubstate = _selectedProc->getNumOfSubstates() - 1; // zero-based index
   float totalremoved = _selectedProc->getPredVc() -  _selectedProc->getSubstate(lastsubstate).AccumVacc
                        + _selectedProc->getPredVp() - _selectedProc->getSubstate(lastsubstate).AccumVacp;
   DataLog(log_level_predict_info) << "Predict: volume removals:"
                                   << " Platelet: " << _selectedProc->getPredVc() << " - "
                                   << _selectedProc->getSubstate(lastsubstate).AccumVacc
                                   << "; Plasma: " << _selectedProc->getPredVp() << " - "
                                   << _selectedProc->getSubstate(lastsubstate).AccumVacp
                                   << "; total: " << totalremoved
                                   << endmsg;


   _selectedProc->LogTargets();

}


ProductOptionStruct Predict::FillProcInfo (const procedureDetail& pd) const
{
   //  DataLog(log_level_predict_debug) << " Predict::FillProcInfo()" << endmsg;
   static PQIPredictor pqi;

   ProductOptionStruct procInfo;
   procInfo.is_collecting_platelets    = pd.getProcFixedCollectVol() > 0;
   procInfo.estimated_platelets        = pd.getYtarg();
   procInfo.estimated_platelet_volume  = pd.getPredVc();
   procInfo.estimated_plasma           = pd.getPredVp();
   int lastsubstate = pd.getNumOfSubstates() - 1; // zero-based index
   procInfo.estimated_plasma_no_ac     = pd.getPredVp() - pd.getSubstate(lastsubstate).AccumVacp;
   procInfo.estimated_rbcs             = pd.getPredRBCVolume();
   procInfo.estimated_rbc_dose         = pd.getPredRBCDose();
   procInfo.procedure_time             = pd.getPredTp();
   procInfo.replacement_fluid_required = pd.ReplacementFluidRequired();
   procInfo.count_icon_needed          = !pqi.LabelPlatelets(pd);
   procInfo.metered_platelet           = (pd.getMSSPlt() && _config.isPltMeteringOn());
   procInfo.plateletsAmapSize          = pd.getAmapPltSize();
   DataLog(log_level_predict_info) << "AMAP: plateletsAmapSize  " << procInfo.plateletsAmapSize <<  endmsg;


   // if we started with a metered product make sure it can be listed as unmetered
   if (  _TargetsCDS.MeterPlatelets.Get()
         && _config.isPltMeteringOn()
         && (CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::MSS_PLT))
         &&(_procState.Yield() > 0.05 )  // why 0.05?  cause thats when GUI/predict puts up a value
         )
   {
      procInfo.metered_platelet = true;
   }



   procInfo.metered_rbc = (pd.getMSSRbc() && _config.isRbcMeteringOn() );
   procInfo.ptf_rbc     = (pd.getMSSRbc() && _config.isRbcMeteringOn() );
   procInfo.ptf_split   = pd.getProcPtfIsSplit();
   /*
     (     pd.getPTFRbc()
     && pd.getMSSRbc()
     && _ConfigCDS->Procedure.Get().key_ptf_rbc_on
     && _ConfigCDS->Procedure.Get().key_mss_rbc_on);
   */
   if (  _TargetsCDS.MeterRbcs.Get()
         && _config.isRbcMeteringOn()
         &&(_procState.RbcVolume() > 0.0 )
         )
   {
      procInfo.metered_rbc = true;
      procInfo.ptf_rbc     = true;
   }



   //
   //
   //  IT6849: subtract off rinseback plasma if applicable
   //
   if ( (pd.getRecommendedCassette() == cassette::PLTPLSRBC) &&                     // white set
        _config.getPlasmaRinseback() &&                                             // with plasma rinseback enabled
        (pd.getPredVp() >= cobeConfig.RinsebackVplrb - Config::__plasmaTolerance) ) // and we have enough plasma
   {
      const float facp = (1.0f / ((_config.getACRatio() - 1.0f) * (1.0f - _procState.Hct()))); // note assumes white stamp ratio
      const float facb = facp / (1.0f + facp);
      const float plasma_in_rinseback_plasma = cobeConfig.RinsebackVplrb * (1.0f - facb);

      procInfo.estimated_plasma       -= cobeConfig.RinsebackVplrb;
      procInfo.estimated_plasma_no_ac -= plasma_in_rinseback_plasma;

      // IT11401
      // dont display less that one on gui it displays 0 mL plasma
      if (procInfo.estimated_plasma < 1.0f)
         procInfo.estimated_plasma = 0.0f;
      if (procInfo.estimated_plasma_no_ac < 1.0f)
         procInfo.estimated_plasma_no_ac = 0.0f;
      DataLog(log_level_predict_info) << "Predict: procedure " << pd.getProcedureNumber()
                                      << " subtracting off (" << cobeConfig.RinsebackVplrb << " " << plasma_in_rinseback_plasma
                                      << ") for plasma rinseback; reporting " << procInfo.estimated_plasma << " "
                                      << procInfo.estimated_plasma_no_ac << endmsg;



   }
   procInfo.estimated_qin_max             = pd.getMaxQin();
   procInfo.estimated_vac                 = pd.getPredVac();
   procInfo.estimated_tbv_processed       = pd.getSubstate(pd.getNumOfSubstates() - 1).EndVin - pd.getPredVac();
   procInfo.estimated_post_platelet_count = (int)_procState._postcount.postcount(_procState.PreCount(),
                                                                                 _procState.Tbv(),
                                                                                 procInfo.estimated_platelets * 1.0e11f);


   ////////////////////////////////////////////////////////////////////////////////////////////////////
   ////  COMP PLS STUFF to GUI
   ////////////////////////////////////////////////////////////////////////////////////////////////////

   // _ConfigCDS->Predict.Get().key_complementary_plasma
   // _COMP_PLS_ON

   // send complementary plasma numbers.
   procInfo.comp_pls_entire = false;
   procInfo.comp_pls_volume = 0.0f;
   float percentPlasma = 0.0f;
   float pasVolume     = 0.0f;

   if (_COMP_PLS_ON
       &&   _config.isCompPlasmaOn()
       && (_procState.CassetteType() != cassette::RBCPLS) )
   {
      if (procInfo.metered_platelet && (pd.getMSSPltPctCarryover() > 0) )
      {

         percentPlasma = float(pd.getMSSPltPctCarryover()) / 100.0f;
         pasVolume     = floor(pd.getProcFixedCollectVol() * (( 1 - percentPlasma ) / percentPlasma));



         // IT11705 check against actual collect volm if we ended plt collect early
         if (   (_procState.SubState() > SS_EXTENDED_PCA)
                &&  _procState.FirstCycleComplete()  )
         {

            DataLog(log_level_predict_info) << "compPls: PLT collect done. using actual PLT volume volm=" << procInfo.estimated_platelet_volume << endmsg;
            pasVolume = floor(_procState.CollectVolume() * (( 1 - percentPlasma ) / percentPlasma));
         }

      }

      if (procInfo.estimated_plasma > 0.0f)
      {
         if (procInfo.estimated_plasma <= pasVolume)
         {
            procInfo.comp_pls_entire = true;
         }

         if (procInfo.estimated_plasma > pasVolume)
            procInfo.comp_pls_volume = pasVolume;
         else
            procInfo.comp_pls_volume = procInfo.estimated_plasma;
      }

      procInfo.comp_pls_PAS_volume = pasVolume;


      DataLog(log_level_predict_info) << "compPls: predict est PLT volm=" << procInfo.estimated_platelet_volume
                                      << ",  est Plasma volm= " <<  procInfo.estimated_plasma
                                      << ",  est PAS volm= " <<  pasVolume
                                      << ",   PLT conf volm= " <<  pd.getProcFixedCollectVol()
                                      << ",   PLT actual volm= " << _procState.CollectVolume()
                                      << ",  PLS%CO volm= " <<  pd.getMSSPltPctCarryover()
                                      << ",  Entire Prod Complnty= " <<  procInfo.comp_pls_entire << endmsg;

      // DataLog(log_level_predict_info) << "compPls: est PLT volm=" << procInfo.estimated_platelet_volume << "  est pls volm= " <<  procInfo.estimated_plasma << endmsg;
      // DataLog(log_level_predict_info) << "compPls: pas volm =" << pasVolume << "  ppco= " << pd.getMSSPltPctCarryover() << endmsg;
   }

   //
   ///////////////////////////////////////////////////////////////////////////////////////////////////



   procInfo.validProcedure  = true;

   procInfo.procedure_index = pd.getProcedureNumber();

   return procInfo;
}


void Predict::ModifyProduct (const ModifyProductStruct& mod)
{
   //  DataLog(log_level_predict_debug) << " Predict::ModifyProduct" << endmsg;
   //  ModifyProductStruct mod = m_ModifyProductMsg.getData();

   DataLog (log_level_predict_debug) << "Predict got ModifyProductStruct message: "
                                     << mod << endmsg;

   //
   //   Check for donor changes and proc state
   //
   UpdateProcData();

   //
   //
   //   Since this may be a PFR check cassette types first to ensure that
   //   the proper procedureDetails are created.
   //
   checkCassetteTypes();

   procedureDetail* pD    = _procedures[mod.product_number]; // getProcedure(mod.product_number);
   int              valid = 0;                               // guilty until proven innocent

   switch (mod.action)
   {
      case SET_PRODUCT_STATUS :
         if (pD)
         {
            pD->setStatus((INTERNAL_PREDICT_RESULTS)mod.aux_data);
            DataLog(log_level_predict_info) << "Predict: modify request, product " << mod.product_number
                                            << " forced to status " << mod.aux_data << endmsg;
            valid = 1;
         }
         else
         {
            DataLog(log_level_predict_error) << "Predict: cannot set product status: product number invalid->"
                                             << mod.product_number << endmsg;
         }
         break;


      case SET_PRODUCT_AS_CURRENT :
         if (pD)
         {
            //
            //
            //  If a product is forced to be the current product then
            //  we assume that this is outside of the operator's control
            //  (e.g. powerfail) and therefore we should not put up the
            //  alert for concentration out of tolerance.
            //
            _selectedProc              = _curProc = pD;
            _concentrationWarningGiven = true;
            _productGivenWarning       = _curProc->getProcedureNumber();
            select();

            DataLog(log_level_predict_info) << "Predict: modify request, product " << mod.product_number
                                            << " set as current product" << endmsg;
            valid = 1;
         }
         else
         {
            DataLog(log_level_predict_error) << "Predict: cannot set product as current: product number invalid->"
                                             << mod.product_number << endmsg;
         }
         break;
      case REOPTIMIZE_PRODUCT :
         if (pD)
         {
            SetAllUnoptimized();
            if (pD->getRecommendedCassette() == cassette::RBCPLS)
            {
               substates_t info;
               pD->restoreSubstateInfoFromCds(info);
            }

            predictProcedure(*pD);
            if (pD == _selectedProc) setPredictTargets();
            DataLog(log_level_predict_info) << "Predict: modify request, product " << mod.product_number
                                            << " reoptimized, new status " << pD->getStatus() << endmsg;
            valid = 1;
         }
         else
         {
            DataLog(log_level_predict_error) << "Predict: cannot reoptimize: product number invalid->"
                                             << mod.product_number << endmsg;
         }
         break;


      case DELETE_ALL_PRODUCTS :
         if (_selectedProc && pD && (pD->AlgSubstate(_procState.SubState()) > 0))
         {
            DataLog(log_level_predict_error) << "Predict: cannot delete all products, a product is currently selected"
                                             << endmsg;
         }
         else
         {
            cleanup();
            valid = 1;
         }
         break;


      default :
         break;

   }

   if (!valid)
   {
      anAlarmMsg badModify(PREDICT_INVALID_MODIFY_REQUEST);
      badModify.setAlarm();
      DataLog(log_level_predict_error) << "Predict: bad modify request " << mod.action
                                       << " product " << mod.product_number
                                       << " auxdata " << mod.aux_data
                                       << endmsg;
   }
}


#endif  // end VxWorks only stuff...


/////////////////////////////////////////////////////////////////////////////
//
void Predict::logPredictData (procedureDetail& pd) const
{
   //  DataLog(log_level_predict_debug) << " Predict::logPredictData()" << endmsg;
   logConvergenceTrail(pd);
   DataLog(log_level_predict_info).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_predict_info).precision(3);



// JPH FIX LOGGING HERE:
//
//        DataLog(log_level_predict_info) << "procedure " << pd.getProcedureNumber()
//             << " reports status " << pd.getStatus() << " plt:(vol=" << pd.getPredVc()
//             << " yldTarg=" << pd.getDesiredYield() << " maxYld=" << _maxAMAPPyield
//             << " amap=" << pd.getAmapPltSize() << " conc=" << pd.getProcPlateletProdCon()
//             << ") plasma:(" << pd.getPredVp() << ") rbc:(" << pd.getPredRBCDose()
//             << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume() << ")" << " duration=" << pd.getPredTp()
//             << endmsg;

   //     DataLog(log_level_predict_info) << "procedure " << pd.getProcedureNumber()
   //   << " reports status " << pd.getStatus()
   // << " (" << pd.toplevel.Ytarg
   // << " " << pd.getPredVc()
   // << " " << pd.getPredVp()
   //  << " " << pd.getPredRBCDose() << "@" << pd.getPredRBCCrit() << "=" << pd.getPredRBCVolume()
   // << " " << pd.getPredTp()
   //   << ")" << endmsg;



// #if CPU!=SIMNT
//     if (_OptimizingSelectedProc)
// #endif
//     pd.LogTargets();
}
/////////////////////////////////////////////////////////////////////////////
void Predict::predictPltAmap (procedureDetail& pd)
{
   DataLog(log_level_predict_debug) << " Predict::predictPltAmap()" << endmsg;
   if (!_AMAP_PLATELETS_ON) return;


   pd.setAMAPPLTinvalid(false);
   _AMAP_PLATELETS_PROCESSING = true;

   int          procNumber       = pd.getProcedureNumber();
   float        maxPossibleYield = _maxAMAPPyield;
   AMAPPLTTYPES PltProdSize      = pd.getAmapPltSize(); // single, double, triple
   float        minYield         = -1.0f;
   float        maxYield         = -1.0f;
   float        amapConc         = -1.0f;
   float        currentYield     = -1.0f;


   pd.setMyAMAPPltYieldRange(minYield, maxYield, amapConc) ;

   pd.setProcPlateletProdCon(amapConc);

   if (maxPossibleYield < maxYield && maxPossibleYield > minYield)
   {
      maxYield = maxPossibleYield;
   }

   if (maxPossibleYield < minYield)
   {
      DataLog(log_level_predict_info) << "amapp: cant create procedure. The Max Possible Yield is below products minimum yield :"
                                      << ",  procNum = " << procNumber
                                      << ",  maxPossibleYield = " << maxPossibleYield
                                      << ",  PltProdSize = " << PltProdSize
                                      << ",  minYield = " << minYield
                                      << ",  maxYield = " << maxYield
                                      << ",  amapConc = " << amapConc
                                      << ",  currentYield = " << currentYield
                                      << ",  theSetDesiredYield = " << pd.getDesiredYield()
                                      << endmsg;


      pd.setStatus(YIELD_NOT_ACHIEVED);
      pd.setDesiredYield(0.0f) ;
      pd.setAMAPPLTinvalid(true);
   }

   if (!pd.getAMAPPLTinvalid())
   {
      // set this to be intermediate predict
      _amapPltItermediatePredict = true;

      // IT118861
      pd.setYtarg(0);
      pd.setPredVc(0);
      pd.setPredVp(0);
      pd.setPredRBCDose(0);
      pd.setPredRBCVolume(0);
      pd.setPredTp(0);

      ///////////////////////////////////////////////////////////////////////////
      // find the jumpin point for our search to speed things up.
      float startingYield =  maxYield;

      ////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////
      AMAP_PLT_STATUS status1 = AMAP_PLT_UNK,
                      status2 = AMAP_PLT_UNK,
                      status3 = AMAP_PLT_UNK,
                      status4 = AMAP_PLT_UNK,
                      status5 = AMAP_PLT_UNK;

      float sampleJumps       = fabs(startingYield - minYield) / 4.0f;
      int   intBuffer         = (int)(sampleJumps * 10.0f + 0.50001f);
      sampleJumps = (intBuffer / 10.0f);
      // DataLog(log_level_predict_info) << "amapp: sampleJumps = " << sampleJumps << endmsg;

      bool EndSearch = false;

      status1 = checkAMAPplt(pd, startingYield);
      DataLog(log_level_predict_info)
         << "amapp: First check = "
         << (int)status1
         << " at yield "
         << startingYield
         << endmsg;

      // if the first time fails for an reason that lowering Yield cannot fix, then...
      if (status1 == AMAP_PLT_FINISHED)
      {
         pd.setStatus(YIELD_NOT_ACHIEVED);
         pd.setDesiredYield(0.0f) ;
         pd.setAMAPPLTinvalid(true);
      }


      if ( status1 != AMAP_PLT_GOOD && !pd.getAMAPPLTinvalid() )
      {

         float start2 =  (startingYield - sampleJumps);
         if (  start2 < minYield  )
            start2 = minYield;
         status2 = checkAMAPplt(pd,  start2);
         DataLog(log_level_predict_info)
            << "amapp: second check = "
            << (int)status2
            << " at yield "
            << start2
            << endmsg;

         if (status2 != AMAP_PLT_GOOD)
         {

            float start3 =  (startingYield - sampleJumps * 2.0f);
            if (  start3 < minYield  )
               start3 = minYield;
            status3 = checkAMAPplt(pd, start3);
            DataLog(log_level_predict_info)
               << "amapp: third check = "
               << (int)status3
               << " at yield "
               << start3
               << endmsg;



            if (status3 != AMAP_PLT_GOOD)
            {
               float start4 =  (startingYield - sampleJumps * 3.0f);
               if (  start4 < minYield  )
                  start4 = minYield;
               status4 = checkAMAPplt(pd, start4);
               DataLog(log_level_predict_info)
                  << "amapp: fourth check = "
                  << (int)status4
                  << " at yield "
                  << start4
                  << endmsg;


               if (status4 != AMAP_PLT_GOOD)
               {
                  status5 = checkAMAPplt(pd,  minYield);
                  DataLog(log_level_predict_info)
                     << "amapp: fifth (minYield) check = "
                     << (int)status5
                     << " at yield "
                     << minYield
                     << endmsg;

                  if (status5 != AMAP_PLT_GOOD)
                  {
                     // no fit
                     DataLog(log_level_predict_info) << "amapp: cant create procedure. The Max Possible Yield is below products minimum yield :"
                                                     << ",  procNum = " << procNumber
                                                     << ",  maxPossibleYield = " << maxPossibleYield
                                                     << ",  PltProdSize = " << PltProdSize
                                                     << ",  minYield = " << minYield
                                                     << ",  maxYield = " << maxYield
                                                     << ",  amapConc = " << amapConc
                                                     << ",  currentYield = " << currentYield
                                                     << ",  theSetDesiredYield = " << pd.getDesiredYield()
                                                     << endmsg;

                     pd.setStatus(YIELD_NOT_ACHIEVED);
                     pd.setDesiredYield(0.0f) ;
                     pd.setAMAPPLTinvalid(true);

                  }
                  else
                  {

                     startingYield = start4 - _stepSize;
                     DataLog(log_level_predict_info) << "amapp: fifth good starting check at  = " << startingYield << endmsg;
                  }
               }
               else                    // status4 == GOOD
               {
                  startingYield = start3 - _stepSize;
                  DataLog(log_level_predict_info) << "amapp: fourth good starting check at  = " << startingYield << endmsg;
               }
            }
            else                 // statu3 == GOOD
            {
               startingYield = start2 - _stepSize;
               DataLog(log_level_predict_info) << "amapp: third good starting check at  = " << startingYield << endmsg;
            }
         }
         else              // status2 == GOOD
         {
            startingYield = startingYield - _stepSize;
            DataLog(log_level_predict_info) << "amapp: second good starting check at  = " << startingYield << endmsg;
         }
      }
      else
      {

         currentYield = startingYield;
         EndSearch    = true;
      }


      if (startingYield < minYield)
         startingYield = minYield;
      ///////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////
      // starting place high
      // currentYield = maxYield;  change to found point.....

      if  ( !pd.getAMAPPLTinvalid() )
      {

         if ( !EndSearch )
         {

            currentYield = startingYield;
            if (currentYield > maxYield)
               currentYield = maxYield;

            AMAP_PLT_STATUS status = AMAP_PLT_BAD;          // assume bad

            // top down to find best  (add some delta to overcome the floating point noise....)
            while ( (currentYield + 0.01f >= minYield) )
            {

               status = checkAMAPplt(pd, currentYield);
               if (    status == AMAP_PLT_GOOD
                       || status == AMAP_PLT_FINISHED
                       )
                  break;

               currentYield -= _stepSize;
               DataLog(log_level_predict_info) << "amapp: amap yield decreased to:"  <<  currentYield  << "(min Yield = " << minYield << ")" << endmsg;

            }             // end while loop

         }

      }

      // set this to be intermediate predict
      _amapPltItermediatePredict = false;
      if ( pd.getStatus() == GOOD_PRODUCT )
         LogProcedure(pd);

      //
      // 10211 -- need to keep the volume
      //
      if (_selectedProc && currentYield > 0.0f)
      {
         if (currentYield != _TargetsCDS.SelectedPlateletYield.Get() )
         {
            DataLog(log_level_predict_info) << "amapp: Current Yield changed"  <<  currentYield  << "( was  = " << _TargetsCDS.SelectedPlateletYield.Get() << ")" << endmsg;

            _currentDisplayedValues.estimated_platelets       = _TargetsCDS.PlateletYieldTarget.Get();
            _currentDisplayedValues.estimated_platelet_volume = _TargetsCDS.PlateletVolumeTarget.Get();

            setPredictTargets();

            // force these because they are not selected via an operator
            _TargetsCDS.SelectedPlateletYield.Set(_TargetsCDS.PlateletYieldTarget.Get() );
            _TargetsCDS.SelectedPlateletVolume.Set(_TargetsCDS.PlateletVolumeTarget.Get());
            //   _TargetsCDS.SelectedPlateletConcen.Set( _selectedProc.getProcPlateletProdCon());

         }
      }

   }    // end if (!pd.getAMAPPLTinvalid())


   _AMAP_PLATELETS_PROCESSING = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Predict::AMAP_PLT_STATUS Predict::checkAMAPplt (procedureDetail& pd, float yield)
{
   DataLog(log_level_predict_info) << " Predict::checkAMAPplt "   << endmsg;
   Predict::AMAP_PLT_STATUS status = AMAP_PLT_BAD;

   // const ProductListArray &defs = _ConfigCDS->ProcedureList.Procedure.Get();

   // use this some how :

   int procArrayIndex             = pd.getProcedureNumber() - 1;
   int procNumber                 = pd.getProcedureNumber();
   const ProcedureDescription def = _config.getProcedureDescription(procArrayIndex);

   ///// METERED STORAGE SETUP //////////////////////////////////////////////////////
   // use these to hold the state of the master switch and the procedure switch combinations
   bool MssRBCSwitch = false;

   // PTF needs both masters (PTF and RBC) and the procedure RBC and PTF switch checked
   MssRBCSwitch = (def.rbc_mss && _config.isRbcMeteringOn());

   /////////////////////////////////////////////////////////////////////////////////////////////////


   pd.initializeAmapPlt(
      yield,
      def.amap_platelet_size,
      (pd.getPlasmaTargetCannotChange()) ? pd.getProcFixedPlasmaVol() : def.plasma_volume,
      def.rbc_dose,                   // ml
      def.rbc_crit,                   // percent, e.g. 80
      _config.getRSF(),
      def.max_procedure_time,        // minutes
      def.blood_type,                // or'd enums
      def.designator,                // primary, secondary, etc.
      _config.isPltMeteringOn(),     // metered plt storage
      MssRBCSwitch,                  // metered rbc storage
      def.rbc_mss_volume,            // rbc metered volume
      procNumber);


   INTERNAL_PREDICT_RESULTS precheckStatus = pd.GeneralPreCheck();
   // DataLog(log_level_predict_info) << "amapp: Precheck  status :"  <<  (int)precheckStatus << endmsg;


   if (precheckStatus != GOOD_PRODUCT)
   {

      DataLog(log_level_predict_info) << "amapp: Precheck failed :"  <<  (int)precheckStatus << endmsg;
      //  logPredictData(pd);


      if (
         precheckStatus == DUPLICATE_PRODUCT
         ||  precheckStatus == INTERNAL_ERROR
         ||  precheckStatus == INVALID_PRODUCT_COMBINATION
         ||  precheckStatus == PRODUCT_ACHIEVED
         ||  precheckStatus == BLOOD_TYPE_MISMATCH
         ||  precheckStatus == CASSETTE_TYPE_MISMATCH
         ||  precheckStatus == INCORRECT_DESIGNATOR
         ||  precheckStatus == DONOR_INFO_OUT_OF_RANGE
         )
      {
         DataLog(log_level_predict_info) << "amapp: Quit because can't optimize out of this condition :"  <<  (int)pd.getStatus() << endmsg;
         // logPredictData(pd);
         // break;
         status = AMAP_PLT_FINISHED;
      }

   }

   predictProcedure(pd);
   // DataLog(log_level_predict_info) << "amapp: predict status is :"  <<  pd.getStatus() << endmsg;


   if ( pd.getStatus() == GOOD_PRODUCT)
   {

      DataLog(log_level_predict_info) << "amapp: Quit on the good status :"  <<  (int)pd.getStatus() << endmsg;
      logPredictData(pd);
      status = AMAP_PLT_GOOD;
      // break;
   }

   if (
      pd.getStatus() == DUPLICATE_PRODUCT
      ||  pd.getStatus() == INTERNAL_ERROR
      ||  pd.getStatus() == INVALID_PRODUCT_COMBINATION
      ||  pd.getStatus() == PRODUCT_ACHIEVED
      ||  pd.getStatus() == BLOOD_TYPE_MISMATCH
      ||  pd.getStatus() == CASSETTE_TYPE_MISMATCH
      ||  pd.getStatus() == INCORRECT_DESIGNATOR
      ||  pd.getStatus() == DONOR_INFO_OUT_OF_RANGE
      )
   {
      DataLog(log_level_predict_info) << "amapp: Quit because cant optimize out of this condition :"  <<  (int)pd.getStatus() << endmsg;
      logPredictData(pd);
      // break;

      status = AMAP_PLT_FINISHED;
   }

   return status;
}


// jph    calcMaxAMAPplateletYield
///////////////////////////////////////////////////////////////////////////////
void Predict::calcMaxAMAPplateletYield ()
{
   DataLog(log_level_predict_info) << " Predict::calcMaxAMAPplateletYield "   << endmsg;
   // determine the max possible yield in an AMAPP situation....
   //
   // maxYield = TBV * Cpre * Fppta
   //
   // Fppta = (1-Cpomin/Cpre/1000.0) * Fm

   _AMAP_PLATELETS_PROCESSING = false; // makes us reprocess all the AMAP Plts to insure they are in scope yet.
   float TBV    =   _procState.Tbv();
   float Cpre   =   _procState.PreCount();
   float Fm     =    cobeConfig.PlateletMobilizationFactor;
   float Cpomin = _config.getMinPostCount() / 1000.0f;

   float Fptta  = ( 1.0f - (Cpomin / Cpre) ) / Fm;
   if (Fptta < 0.0f)
   {
      // DataLog(log_level_predict_info) << "amapp: Fptta underrun ="  << Fptta << endmsg;
      Fptta = 0.0f;
   }
   if (Fptta > 1.0f)
   {
      // DataLog(log_level_predict_info) << "amapp: Fptta overrun ="  << Fptta << endmsg;
      Fptta = 1.0f;
   }

   _maxAMAPPyield =  (TBV * Cpre * Fptta) / 1e5f;


   // round to closest tenth, otherwise we get wierd result due to rounding later...
   int temp =  (int)(_maxAMAPPyield * 10.0f + 0.50001f);
   _maxAMAPPyield = temp / 10.0;


   DataLog(log_level_predict_info) << "amapp:  Calc AMAPP max possible yield ="
                                   << _maxAMAPPyield
                                   << " { TBV= "
                                   << TBV
                                   << " Cpre= "
                                   << Cpre
                                   << " Cpomin= "
                                   << Cpomin
                                   << " Fm= "
                                   << Fm  << " }" << endmsg;
}


// void Predict::setMyAMAPPltYieldRange(AMAPPLTTYPES prodSize, float &min, float &max, float &conc)
// {
//     if (prodSize < AMAP_PLT_SINGLE || prodSize > AMAP_PLT_TRIPLE)
//     {
//        // DataLog (log_level_predict_error) << "amapp: size " << prodSize << " is not valid." << endmsg;
//         min = 0; max = 0; conc = 0;
//     }
//     else
//     {
//         min = _ConfigCDS->Templates.Get().key_plt_amap_min_yield[prodSize-1];
//         max = _ConfigCDS->Templates.Get().key_plt_amap_max_yield[prodSize-1];
//         conc = _ConfigCDS->Templates.Get().key_plt_amap_conc[prodSize-1];
//     }
// }
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////
// void Predict::setMyAMAPPltYieldRange(const procedureDetail& pd, float &min, float &max, float &conc)
// {
//     setMyAMAPPltYieldRange(pd.getAmapPltSize(), min, max, conc);
// }
//
//

/* FORMAT HASH d2e6dc5fb6f6d04cf84e934d6cc048b5 */
