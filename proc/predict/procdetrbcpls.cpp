/*
 * Copyright (c) 2000 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      procdetrbcpls.cpp
 *
 */

#include <vxworks.h>
#include <math.h>
#include "procdetrbcpls.h"
#include "substatemap.h"
#include "residualAlgorithm.h"

#include "CatalogChecker.h"
#include "cfg_ids.h"

static const float MAXPLASMAHCT     = 0.60f;  // maximum allowable crit to collect plasma
static const float DEFAULTDRBCSPLIT = 275.0f; // default DRBC split volume if split is off in config

procedureDetailRBCPls::procedureDetailRBCPls()
   : procedureDetail(),
     _hct(0.0f),
     _PTF(false),
     _RBCSetup1Time(0.0f),
     _RBCSetup1Vin(0.0f),
     _RBCSetup1Vac(0.0f),
     _RBCSetup2Time(0.0f),
     _RBCSetup2Vin(0.0f),
     _RBCSetup2Vac(0.0f),
     _RBCSetup1DoubleTime(0.0f),
     _RBCSetup1DoubleVin(0.0f),
     _RBCSetup1DoubleVac(0.0f),
     _RBCSetup2DoubleTime(0.0f),
     _RBCSetup2DoubleVin(0.0f),
     _RBCSetup2DoubleVac(0.0f),
     _lowestIRDamping(1.0f),
     _disableProd1Done(false)

{
   _numOfSubstates      = Predict_CDS::NumOfRBCPlsSubstates;
   _recommendedCassette = cassette::RBCPLS;
}

//
// -----------------------------------------------------------------------------
//
procedureDetailRBCPls::~procedureDetailRBCPls()
{}

//
// -----------------------------------------------------------------------------
//
INTERNAL_PREDICT_RESULTS procedureDetailRBCPls::PreCheckProcedure ()
{


   // DataLog(log_level_predict_debug) << "procedureDetailRBCPls: PreCheckProcedure";
   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   float configRbcDose = _config.getRbcDose(getProcedureNumber());
   float drbcThreshold = _config.getDrbcThreshold();
   bool  splitOn       = (bool)_config.isDrbcAlertOn();


   if ( splitOn && ((configRbcDose > DEFAULTDRBCSPLIT)  && (configRbcDose < drbcThreshold)))
   {

      DataLog(log_level_predict_info) <<  "Violated Dose GT Split threshold;   configDose->"
                                      << configRbcDose
                                      << " threshold->"
                                      <<  drbcThreshold
                                      <<  endmsg;
      return RBC_BAG_VOLUME_VIOLATION;
   }

   // IT 5725 Does the donor qualify if DRBC volume limit is on?
   bool  drbcTbvLimitOn = (  _config.getTbvVolLimitSetting()
                             & (int)VOLUME_REMOVAL_DRBC_BODY_VOL) != 0;
   float drbcTbvLimit   = _config.getDrbcTbvLimit();
   if (drbcTbvLimitOn &&
       ((_procState.Tbv() / 1000.0f) < drbcTbvLimit) &&
       ( configRbcDose > DEFAULTDRBCSPLIT )
       )
   {
      DataLog(log_level_predict_info) <<   "Donor TBV(" <<  _procState.Tbv() / 1000.0f << ")less than"
                                      <<   " DRBC Volume limit(" <<  drbcTbvLimit << ")"
                                      <<  endmsg;
      return DRBC_VOL_LIMIT_VIOLATION;
   }

   return GOOD_PRODUCT;
}

//
// -----------------------------------------------------------------------------
//
INTERNAL_PREDICT_RESULTS procedureDetailRBCPls::PostCheckProcedure ()
{
   //
   //
   //   IT11766 disqualify DRBC plus plasma if configured
   //
   /*
   const bool disallow = Software_CDS::GetInstance().getFeature(DisallowDRBCPlusPlasma); // V6.4 feature does not yet exist
   if (disallow)
   {
      float configRbcDose = _config.getRbcDose(getProcedureNumber());
      float drbcThreshold = ( _config.isDrbcAlertOn() ? (float)_config.getDrbcThreshold() : DEFAULTDRBCSPLIT );
      if ((configRbcDose >= drbcThreshold) && (getPredVp() > 0.0f))
         return DRBC_WITH_PLASMA;
   }
   */

   return GOOD_PRODUCT;
}
//
// -----------------------------------------------------------------------------
//

bool procedureDetailRBCPls::secondPartofPTFDouble () const
{
   float targetDose =  getPredRBCDose();

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = _config.isPromptedDrbcSplit(getProcedureNumber());


   if (doubleTargeted &&
       (  // either we passed the split point or failed dueto a CPS high alarm
          ((_procState.RbcDose() * 2.0f) >= targetDose )
          || _procState.PtfRbcProd1Disabled()

       )
       // and we're doing RAS
       && getMSSRbc()
       )
   {

      return true;
   }
   else
   {
      return false;
   }

}
//
// -----------------------------------------------------------------------------
//
bool procedureDetailRBCPls::doingPTFDouble () const
{
   // float targetDose    =  getPredRBCDose();

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = _config.isPromptedDrbcSplit(getProcedureNumber());

   return doubleTargeted && getMSSRbc();
}



//
// -----------------------------------------------------------------------------
//

int procedureDetailRBCPls::RBCState () const
{
   return Config::__RP_RBC;
}

int procedureDetailRBCPls::RBCDoubleState () const
{
   return Config::__RP_RBC_DOUBLE;
}

//
// -----------------------------------------------------------------------------
//
//
//   For RBC/Plasma sets, we can collect plasma up to 60 crit.  We only need to check
//   the plasma only substates since the RBC collection substates will lower the donor crit.
//
bool procedureDetailRBCPls::PlasmaHctOK () const
{
   if (substate[Config::__RP_PLASMA_ONLY].Hend > 0.0f) // we have an answer
   {
      DataLog(log_level_predict_info) << "procedure " << getProcedureNumber()
                                      << " hct at end of plasma collection is " << substate[Config::__RP_PLASMA_ONLY].Hend << endmsg;
      if (substate[Config::__RP_PLASMA_ONLY].Hend > MAXPLASMAHCT)
      {
         return false;
      }
   }
   return true;
}
//
// -----------------------------------------------------------------------------
//

// -----------------------------------------------------------------------------
//
void procedureDetailRBCPls::LogTargets () const
{
   DataLog(log_level_predict_debug) << "Doing RAS Double? ->"
                                    << doingPTFDouble()
                                    << "; in second Part of RAS Double? -> "
                                    << secondPartofPTFDouble()
                                    << endmsg;



   DataLog(log_level_predict_debug) << "substate T EndVin AccumVac Qind/Qinr Qp Qc Qac Qr AccumVp Vr HypoD Vsvn Hend" << endmsg;
   DataLog(log_level_predict_debug).flags(std::ios::showpoint | std::ios::fixed);
   DataLog(log_level_predict_debug).precision(3);

   for (int subndx = 0; subndx < getNumOfSubstates(); subndx++)
   {
      if (subndx == 0 || substate[subndx].T > 0.0f)
      {
         DataLog(log_level_predict_debug) << SubstateMap::algStr(subndx, 1) << " "
                                          << substate[subndx].T << " "
                                          << substate[subndx].EndVin << " "
                                          << substate[subndx].AccumVac << " "
                                          << substate[subndx].Qininstd << "/" << substate[subndx].Qininstr << " "
                                          << substate[subndx].Qpinstd << " "
                                          << substate[subndx].Qcinstd << " "
                                          << substate[subndx].Qacinst << " "
                                          << substate[subndx].Qrinst << " "
                                          << substate[subndx].AccumVp << " "
                                          << substate[subndx].Vr << " "
                                          << substate[subndx].HypovolemiaDistance << " "
                                          << substate[subndx].Vsvn << " "
                                          << substate[subndx].Hend
                                          << endmsg;
      }
   }
}

//
// -----------------------------------------------------------------------------
//
void procedureDetailRBCPls::SetConstants ()
{
   // DataLog(log_level_predict_debug) << "procedureDetailRBCPls: SetConstants";

   _extracorporealVolume = cobeConfig.RBCPlsExtracorporealVolume;

   // lookup the base residual based on settype and Rinseback selection
   _residualRBCVolume = ResidualAlgorithm::getBaseResidual(_recommendedCassette,
                                                           _config.getPlasmaRinseback(),
                                                           _config.getSalineRinseback());


   _primeDuration       = cobeConfig.RbcPlasmaPrimeTime;
   _vinPrime            = cobeConfig.RbcPlasmaPrimeVolume;
   _hct                 = _procState.Hct();

   _HypoMargin          = 0.0f; // IT9379, 11220 no longer include hypovolemia margins in predict

   _PTF                 = getMSSRbc();

   _RBCSetup1Time       = 0.0f;
   _RBCSetup1Vin        = 0.0f;
   _RBCSetup1Vac        = 0.0f;
   _RBCSetup2Time       = 0.0f;
   _RBCSetup2Vin        = 0.0f;
   _RBCSetup2Vac        = 0.0f;
   _RBCSetup1DoubleTime = 0.0f;
   _RBCSetup1DoubleVin  = 0.0f;
   _RBCSetup1DoubleVac  = 0.0f;
   _RBCSetup2DoubleTime = 0.0f;
   _RBCSetup2DoubleVin  = 0.0f;
   _RBCSetup2DoubleVac  = 0.0f;

   memset(&_rbcsubstate, 0, sizeof(IntermediateResults));

}


//
//
// -----------------------------------------------------------------------------
//
void procedureDetailRBCPls::do_SetTargets (substates_t& info)
{
   // DataLog(log_level_predict_debug) << "procedureDetailRBCPls: SetTargets";

   // NOTE: we should really lock _predictCDS to improve performance across all
   // the writes below... we don't, because I happen to know that it's already
   // been locked by the calling function (predict.cpp).  A debatable choice. --DGS


   //////////////////////////////////////////////////////////////////////////////////////////////
   // Csf
   for (int subst = 0; subst<_numOfSubstates; subst++)
   {
      info[subst].Csf = cobeConfig.CsfMax;
      info[subst].Rpm = substate[subst].Rpm;
   }
   // Purge, Pir stuff - should be zeroed out
   _procState._predictCDS.VinPurge.Set(0.0f);
   _procState._predictCDS.VinDeltaPurge.Set(0.0f);
   _procState._predictCDS.VinPIRStart.Set(0.0f);
   _procState._predictCDS.MaxPIRQc.Set(0.0f);
   _procState._predictCDS.Ydch.Set(0.0f);
   _procState._predictCDS.YdchPIR.Set(0.0f);

   if (secondPartofPTFDouble())
      _procState._predictCDS.RBCVsvn.Set(substate[RBCDoubleState()].Vsvn);
   else
      _procState._predictCDS.RBCVsvn.Set(substate[RBCState()].Vsvn);

   // IT 8954
   _procState.setPIRLockin(0.0f);
   _procState.setQcmr(0.0f);
   _procState.setLastCalcQchMax(0.0f);
   /////////////////////////////////////////////////////////////////////////////////////////////

   // ------------------------------------------------------------------------------------
   //   Nocollect
   //
   info[Config::__NOCOLLECT_STATE].Vtarget       = 0.0f;
   info[Config::__NOCOLLECT_STATE].VtargetType   = VTarget::NOCOLLECT;
   info[Config::__NOCOLLECT_STATE].OrVtarget     = 0.0f;
   info[Config::__NOCOLLECT_STATE].OrVtargetType = VTarget::NOCOLLECT;
   /// VALVE POSITIONS
   info[Config::__NOCOLLECT_STATE].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__NOCOLLECT_STATE].PlasmaValve   = HAL_VALVE_RETURN;
   info[Config::__NOCOLLECT_STATE].RBCValve      = HAL_VALVE_RETURN;


   // ------------------------------------------------------------------------------------
   //
   //  Channel setup
   //
   if (substate[Config::__RP_CHANNEL_SETUP].T > 0.0f)
   {
      info[Config::__RP_CHANNEL_SETUP].Vtarget     = substate[Config::__RP_CHANNEL_SETUP].EndVin;
      info[Config::__RP_CHANNEL_SETUP].VtargetType = VTarget::VIN;
   }
   else
   {
      info[Config::__RP_CHANNEL_SETUP].Vtarget     = 0.0f;
      info[Config::__RP_CHANNEL_SETUP].VtargetType = VTarget::NOCOLLECT;
   }

   /// VALVE POSITIONS
   info[Config::__RP_CHANNEL_SETUP].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__RP_CHANNEL_SETUP].PlasmaValve   = HAL_VALVE_RETURN;
   info[Config::__RP_CHANNEL_SETUP].RBCValve      = HAL_VALVE_RETURN;


   info[Config::__RP_CHANNEL_SETUP].VtargetType   = VTarget::VIN;
   info[Config::__RP_CHANNEL_SETUP].OrVtarget     = 0.0f;
   info[Config::__RP_CHANNEL_SETUP].OrVtargetType = VTarget::NOCOLLECT;
   info[Config::__RP_CHANNEL_SETUP].T             = substate[Config::__RP_CHANNEL_SETUP].T;
   info[Config::__RP_CHANNEL_SETUP].EndVin        = substate[Config::__RP_CHANNEL_SETUP].EndVin;

   // ------------------------------------------------------------------------------------
   //
   //   Plasma only
   //
   /// VALVE POSITIONS

   info[Config::__RP_PLASMA_ONLY].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__RP_PLASMA_ONLY].PlasmaValve   = HAL_VALVE_COLLECT;
   info[Config::__RP_PLASMA_ONLY].RBCValve      = HAL_VALVE_RETURN;

   info[Config::__RP_PLASMA_ONLY].Vtarget       = _PredVp;
   info[Config::__RP_PLASMA_ONLY].VtargetType   = VTarget::VPBAG;
   info[Config::__RP_PLASMA_ONLY].OrVtarget     = 0.0f;
   info[Config::__RP_PLASMA_ONLY].OrVtargetType = VTarget::NOCOLLECT;
   info[Config::__RP_PLASMA_ONLY].T             = substate[Config::__RP_PLASMA_ONLY].T;
   info[Config::__RP_PLASMA_ONLY].EndVin        = substate[Config::__RP_PLASMA_ONLY].EndVin;
   // ------------------------------------------------------------------------------------
   //
   //   RBC Setup 1
   //
   if (substate[Config::__RP_RBC_SETUP_1].T > 0.0f)
   {
      info[Config::__RP_RBC_SETUP_1].VtargetType = VTarget::VIN;
      info[Config::__RP_RBC_SETUP_1].Vtarget     = substate[Config::__RP_RBC_SETUP_1].EndVin;
   }
   else
   {
      info[Config::__RP_RBC_SETUP_1].VtargetType = VTarget::NOCOLLECT;
      info[Config::__RP_RBC_SETUP_1].Vtarget     = 0.0f;
   }

   /// VALVE POSITIONS
   info[Config::__RP_RBC_SETUP_1].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__RP_RBC_SETUP_1].PlasmaValve   = HAL_VALVE_RETURN;
   info[Config::__RP_RBC_SETUP_1].RBCValve      = HAL_VALVE_RETURN;


   info[Config::__RP_RBC_SETUP_1].OrVtarget     = 0.0f;
   info[Config::__RP_RBC_SETUP_1].OrVtargetType = VTarget::NOCOLLECT;
   info[Config::__RP_RBC_SETUP_1].T             = substate[Config::__RP_RBC_SETUP_1].T;
   info[Config::__RP_RBC_SETUP_1].EndVin        = substate[Config::__RP_RBC_SETUP_1].EndVin;

   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1.VtargetType = " << VTarget::VIN << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1.EndVin = " << substate[Config::__RP_RBC_SETUP_1].EndVin << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1.T           = " << substate[Config::__RP_RBC_SETUP_1].T << endmsg;

   // ------------------------------------------------------------------------------------
   //
   //   RBC Setup 2
   //
   if (substate[Config::__RP_RBC_SETUP_2].T > 0.0f)
   {
      info[Config::__RP_RBC_SETUP_2].VtargetType = VTarget::VIN;
      info[Config::__RP_RBC_SETUP_2].Vtarget     = substate[Config::__RP_RBC_SETUP_2].EndVin;
   }
   else
   {
      info[Config::__RP_RBC_SETUP_2].VtargetType = VTarget::NOCOLLECT;
      info[Config::__RP_RBC_SETUP_2].Vtarget     = 0.0f;
   }

   /// VALVE POSITIONS
   info[Config::__RP_RBC_SETUP_2].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__RP_RBC_SETUP_2].PlasmaValve   = HAL_VALVE_RETURN;
   info[Config::__RP_RBC_SETUP_2].RBCValve      = HAL_VALVE_RETURN;


   info[Config::__RP_RBC_SETUP_2].OrVtarget     = 0.0f;
   info[Config::__RP_RBC_SETUP_2].OrVtargetType = VTarget::NOCOLLECT;
   info[Config::__RP_RBC_SETUP_2].T             = substate[Config::__RP_RBC_SETUP_2].T;
   info[Config::__RP_RBC_SETUP_2].EndVin        = substate[Config::__RP_RBC_SETUP_2].EndVin;


   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2.VtargetType = " << VTarget::VIN << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2.EndVin = " << substate[Config::__RP_RBC_SETUP_2].EndVin << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2.T           = " << substate[Config::__RP_RBC_SETUP_2].T << endmsg;

   // ------------------------------------------------------------------------------------
   //
   //   RBC collection
   //

   /// VALVE POSITIONS
   info[Config::__RP_RBC].PlateletValve = HAL_VALVE_RETURN;
   info[Config::__RP_RBC].PlasmaValve   = HAL_VALVE_RETURN;
   info[Config::__RP_RBC].RBCValve      = HAL_VALVE_COLLECT;



   // correct the volume for being half way done.
   if (doingPTFDouble())
   {
      info[Config::__RP_RBC].Vtarget = getPredRBCVolume() / 2.0f;
      if ( _procState.PtfRbcProd1Disabled() )
      {

         DataLog(log_level_predict_info) << "RBC1 Vtarg stopped because of CPS alarm @ " <<  _procState.RbcVolume() << "mL" << endmsg;
         info[Config::__RP_RBC].Vtarget =  _procState.RbcVolume();
      }

   }
   else
   {
      info[Config::__RP_RBC].Vtarget = getPredRBCVolume();
   }

   info[Config::__RP_RBC].VtargetType   = VTarget::VRBCBAG;
   info[Config::__RP_RBC].OrVtarget     = 0.0f;
   info[Config::__RP_RBC].OrVtargetType = VTarget::NOCOLLECT;
   info[Config::__RP_RBC].T             = substate[Config::__RP_RBC].T;
   info[Config::__RP_RBC].EndVin        = substate[Config::__RP_RBC].EndVin;


   // DataLog(log_level_predict_info) << "__RP_RBC.VtargetType = " << VTarget::VRBCBAG << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC.EndVin      = " << substate[Config::__RP_RBC].EndVin << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC.T           = " << substate[Config::__RP_RBC].T << endmsg;
   // DataLog(log_level_predict_info) << "__RP_RBC.Vtarget     = " << info[Config::__RP_RBC].Vtarget << endmsg;
   //////////////////////////////////////////////////////////////////////////////////////////
   ////  PTF RAS DOUBLES ONLY ///////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////
   if (doingPTFDouble())
   {

      // ------------------------------------------------------------------------------------
      //
      //   RBC Setup Double 1
      //
      if (substate[Config::__RP_RBC_SETUP_1_DOUBLE].T > 0.0f)
      {
         info[Config::__RP_RBC_SETUP_1_DOUBLE].VtargetType = VTarget::VIN;
         info[Config::__RP_RBC_SETUP_1_DOUBLE].Vtarget     = substate[Config::__RP_RBC_SETUP_1_DOUBLE].EndVin;
      }
      else
      {
         info[Config::__RP_RBC_SETUP_1_DOUBLE].VtargetType = VTarget::NOCOLLECT;
         info[Config::__RP_RBC_SETUP_1_DOUBLE].Vtarget     = 0.0f;
      }

      info[Config::__RP_RBC_SETUP_1_DOUBLE].OrVtarget     = 0.0f;
      info[Config::__RP_RBC_SETUP_1_DOUBLE].OrVtargetType = VTarget::NOCOLLECT;

      info[Config::__RP_RBC_SETUP_1_DOUBLE].PlateletValve = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_SETUP_1_DOUBLE].PlasmaValve   = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_SETUP_1_DOUBLE].RBCValve      = HAL_VALVE_RETURN;

      info[Config::__RP_RBC_SETUP_1_DOUBLE].T             = substate[Config::__RP_RBC_SETUP_1_DOUBLE].T;
      info[Config::__RP_RBC_SETUP_1_DOUBLE].EndVin        = substate[Config::__RP_RBC_SETUP_1_DOUBLE].EndVin;
      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1_DOUBLE.VtargetType = " << VTarget::VIN << endmsg;
      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1_DOUBLE.EndVin = "      << substate[Config::__RP_RBC_SETUP_1_DOUBLE].EndVin << endmsg;
      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_1_DOUBLE.T           = " << substate[Config::__RP_RBC_SETUP_1_DOUBLE].T << endmsg;
      // ------------------------------------------------------------------------------------
      //
      //   RBC Setup Double 2
      //
      if (substate[Config::__RP_RBC_SETUP_2_DOUBLE].T > 0.0f)
      {
         info[Config::__RP_RBC_SETUP_2_DOUBLE].VtargetType = VTarget::VIN;
         info[Config::__RP_RBC_SETUP_2_DOUBLE].Vtarget     = substate[Config::__RP_RBC_SETUP_2_DOUBLE].EndVin;
      }
      else
      {
         info[Config::__RP_RBC_SETUP_2_DOUBLE].VtargetType = VTarget::NOCOLLECT;
         info[Config::__RP_RBC_SETUP_2_DOUBLE].Vtarget     = 0.0f;
      }

      info[Config::__RP_RBC_SETUP_2_DOUBLE].OrVtarget     = 0.0f;
      info[Config::__RP_RBC_SETUP_2_DOUBLE].OrVtargetType = VTarget::NOCOLLECT;

      info[Config::__RP_RBC_SETUP_2_DOUBLE].PlateletValve = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_SETUP_2_DOUBLE].PlasmaValve   = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_SETUP_2_DOUBLE].RBCValve      = HAL_VALVE_RETURN;

      info[Config::__RP_RBC_SETUP_2_DOUBLE].T             = substate[Config::__RP_RBC_SETUP_2_DOUBLE].T;
      info[Config::__RP_RBC_SETUP_2_DOUBLE].EndVin        = substate[Config::__RP_RBC_SETUP_2_DOUBLE].EndVin;

      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2_DOUBLE.VtargetType = " << VTarget::VIN << endmsg;
      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2_DOUBLE.EndVin = " << substate[Config::__RP_RBC_SETUP_2_DOUBLE].EndVin << endmsg;
      // DataLog(log_level_predict_info) << "__RP_RBC_SETUP_2_DOUBLE.T           = " << substate[Config::__RP_RBC_SETUP_2_DOUBLE].T << endmsg;
      // ------------------------------------------------------------------------------------
      //   RBC collection Double
      //
      info[Config::__RP_RBC_DOUBLE].Vtarget = getPredRBCVolume();

      if ( _procState.PtfRbcProd1Disabled() )
      {

         DataLog(log_level_predict_info) << "RBC2 Vtarg adjusted (because of CPS alarm on RBC1) to "
                                         << getPredRBCVolume() / 2.0f + _procState.RbcProduct_A_FinalVolume()
                                         << "mL" << endmsg;

         info[Config::__RP_RBC_DOUBLE].Vtarget = getPredRBCVolume() / 2.0f + _procState.RbcProduct_A_FinalVolume();

         setPredRBCDose(info[Config::__RP_RBC_DOUBLE].Vtarget * getProcRBCHct() / 100.0f);
         setPredRBCVolume(info[Config::__RP_RBC_DOUBLE].Vtarget);

      }

      info[Config::__RP_RBC_DOUBLE].VtargetType   = VTarget::VRBCBAG;
      info[Config::__RP_RBC_DOUBLE].OrVtarget     = 0.0f;
      info[Config::__RP_RBC_DOUBLE].OrVtargetType = VTarget::NOCOLLECT;

      info[Config::__RP_RBC_DOUBLE].PlateletValve = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_DOUBLE].PlasmaValve   = HAL_VALVE_RETURN;
      info[Config::__RP_RBC_DOUBLE].RBCValve      = HAL_VALVE_COLLECT;

      info[Config::__RP_RBC_DOUBLE].T             = substate[Config::__RP_RBC_DOUBLE].T;
      info[Config::__RP_RBC_DOUBLE].EndVin        = substate[Config::__RP_RBC_DOUBLE].EndVin;

      DataLog(log_level_predict_info) << "__RP_RBC_DOUBLE.VtargetType = " << VTarget::VRBCBAG << endmsg;
      DataLog(log_level_predict_info) << "__RP_RBC_DOUBLE.EndVin      = " << substate[Config::__RP_RBC_DOUBLE].EndVin << endmsg;
      DataLog(log_level_predict_info) << "__RP_RBC_DOUBLE.T           = " << substate[Config::__RP_RBC_DOUBLE].T << endmsg;
      DataLog(log_level_predict_info) << "__RP_RBC_DOUBLE.Vtarget     = " << info[Config::__RP_RBC_DOUBLE].Vtarget << endmsg;


   }
}


//
//
// -----------------------------------------------------------------------------
//
float procedureDetailRBCPls::instantaneousInfusion (int substateindex) const
{
   const IntermediateResults& info = substate[substateindex];

   if (info.Tcn == 0.0f)
      return 0.0f; // substate is unused

   float IR       = 0.0f;
   float ACinDraw = info.Qacinst * info.Td;
   float acfactor = 1.0f;
   float TBV      = _procState.Tbv() / 1000.0f;

   if (info.Vsvn > 0.0f)   // divide by zero protection, Vsvn should always be valid
   {
      acfactor  = 1.0f - cobeConfig.RecirculateSnDrawVol / info.Vsvn;
      ACinDraw *= acfactor;
   }

   //
   //
   //   Use current facb instead of average to allow for dynamic hct changes
   //
   float ACinProduct = 0.0f;

   if (substateindex == Config::__RP_PLASMA_ONLY)
   {
      ACinProduct = info.Qpinstd * info.Td * info.Facb;
      IR          =  (ACinDraw - ACinProduct) / info.Tcn / TBV;
   }
   else if ( (substateindex == Config::__RP_RBC)
             || (substateindex == Config::__RP_RBC_DOUBLE) )
   {

      ACinProduct = (info.Qininstd - info.Qpinstd) * info.Td * info.Facb * (1.0f - (getPredRBCCrit() / 100.0f) );

      IR          =  (ACinDraw - ACinProduct) / info.Tcn / TBV;
   }
   else
   {  // all nocollection states
      IR = ACinDraw / info.Tcn / TBV;
   }

   return IR;
}
//
//
// -----------------------------------------------------------------------------
//
int procedureDetailRBCPls::AlgSubstate (const State_names procsubstate) const
{
   return SubstateMap::alg(procsubstate, 1);
}

//
//
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
//    Note: all calculations contained herein are directly from the V5 code,
//    which was in turn generated from the spreadsheet created by Bob Langley.
//    For details on how the calculations were developed please see the
//    RBC/Plasma algorithm documentation.
//
void procedureDetailRBCPls::generatesubstates ()
{


   DataLog(log_level_predict_info) << "----generatesubstates----- #" << getnumOfTimes()  << endmsg;

   setconvergenceCtr(0);
   setnumOfTimes(getnumOfTimes() + 1);
   Calculate_Flows::_numCalculations = 0;

   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   float TBV = _procState.Tbv();

   //
   //
   //   "general" calculations (V5 r.cpp equivalents)
   //
   float qininstdmax = MIN(MIN(cobeConfig.TbvQinMaxCoefficient * _config.getTbvQinmaxScaleConst() * TBV, cobeConfig.RbcPlasmaQinmaxr), _config.getMaxInstQin());


///////////////////MERGE QUESTIONS///////////////////////////////////////////////
//   if(_procState.getOperatorAdjQinmax() != 0.0f)
//      qininstdmax = MIN( qininstdmax, _procState.getOperatorAdjQinmax() );
///////////////////////////////////////////////////////////////////////////////

   _procState.setQinistdMax(qininstdmax);

   //
   //
   //   "toplevel" calculations (V5 r.cpp equivalents)
   //
   _I = MIN(static_cast<float>(0.93f * _config.getReferenceACRate() / (1.0f - exp((double)(-_PredTp / 37.0f))))
            , getMaxACRate() );


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

   //
   //
   //
   //     Actual substate flow generation
   //
   const int startingsubstate = AlgSubstate(_procState.SubState() );

   setPlasmaLimitedByHct(false);
   const float timeLeftForPlasma =  _procState.getMaxTime()
                                   - substate[Config::__RP_CHANNEL_SETUP].AccumTime
                                   - _rbcsubstate.T
                                   - _RBCSetup1Time
                                   - _RBCSetup2Time
                                   - _RBCSetup1DoubleTime   // will be zero if not a double
                                   - _RBCSetup2DoubleTime;  // will be zero if not a double


   AccumulateVolumes(); // to set previous substate end points, if any

   switch (startingsubstate)
   {
      default :
         InterfaceSetup();
      // and fall through
      case 2 : // Config::__RP_PLASMA_ONLY:
         PlasmaOnly(timeLeftForPlasma);
      // and fall through
      case 3 : // Config::__RP_RBC_SETUP_1
         RBCSetup1();
      // and fall through
      case 4 : // Config::__RP_RBC_SETUP_2
         RBCSetup2();
      // and fall through
      case 5 : // Config::__RP_RBC:
      {

         RBCCollection();
         if (!doingPTFDouble())
         {
            break;
         }
      }
      case 6 : // Config::__RP_RBC_SETUP_1
         RBCSetup1Double();
      // and fall through
      case 7 : // Config::__RP_RBC_SETUP_2
         RBCSetup2Double();
      // and fall through
      case 8 : // Config::__RP_RBC:
         RBCCollectionDouble();
         break;
   }

   //
   //
   //     Flag if we have exceeded hypovolemia
   //
   // use the correct collect substate to make your determination:
   if (doingPTFDouble())
   {

      hypovolemic(substate[Config::__RP_RBC_DOUBLE].HypovolemiaDistance < getHypoMargin());
      hypovolemiaDistance(substate[Config::__RP_RBC_DOUBLE].HypovolemiaDistance);

   }
   else
   {

      hypovolemic(substate[Config::__RP_RBC].HypovolemiaDistance < getHypoMargin());
      hypovolemiaDistance(substate[Config::__RP_RBC].HypovolemiaDistance);
   }





   //
   //
   //    If AMAP pushed us over our time limit then we need to reduce the
   //    plasma collection to fit within time constraints.  Note that due
   //    to dynamic hct changes we can't just arbitrarily shorten plasma
   //    collection or the RBC flows and times would be incorrect.
   //
   if (_PredTp > (_procState.getMaxTime() + 0.1f))
   {
      if (getProcCollectPlasma()                                                          // collecting plasma
          && (!getPlasmaTargetCannotChange())                                             //  and its either MC or amap
          && (substate[Config::__RP_PLASMA_ONLY].T > 0.0f)                                // and we are collecting during extended plasma
          && (_PredTp - substate[Config::__RP_PLASMA_ONLY].T) <  _procState.getMaxTime()) // and reducing plasma would allow procedure
      {
         if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(1);
      }
   }

   //
   //
   //  for AMAP+RBC collections, ensure that we have collected the proper amount
   //  of plasma.
   //
   if ((getPredRBCDose() > 0.0f) && getProcCollectPlasma() && (!getPlasmaTargetCannotChange()))
   {
      //
      //
      //    This check is to ensure we have not collected too much plasma
      //
      if (hypovolemic() && (substate[Config::__RP_PLASMA_ONLY].Vp > 0.0f))
      {
         if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(2); // too much collected
      }

      //
      //
      //  This check is to ensure we have collected all the AMAP plasma that we can.  The 10ml margin
      //  indicates how close we need to get and is chosen to match the behavior of V5.1.
      //
      if (hypovolemiaDistance() > (getHypoMargin() + 10.0f))
      {
         if (_PredTp < (_procState.getMaxTime() - 0.1f))
         {
            if (_Vptarg > substate[Config::__RP_PLASMA_ONLY].AccumVp) // note assumes AMAP requested volumes always include AC
            {
               if (_numOfTimes < (MAXITERATIONS - 1)) setconvergenceCtr(3); // not enough
            }
         }
      }
   }
}


//
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
//   "interface setup" calculations.  Flows are determined from hct at start of
//   substate (this substate only lasts a fraction of a minute so no dynamic flow
//   changes will occur).
//
void procedureDetailRBCPls::InterfaceSetup ()
{

   IntermediateResults& currentsubstate = substate[Config::__RP_CHANNEL_SETUP];

   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   const float TBV           =  _procState.Tbv();
   const float TBVec         = TBV - cobeConfig.RBCPlsExtracorporealVolume;
   const float ratio         = _config.getACRatio();
   const float initialCrit   =  _procState.InitialHct();
   const float currentvbolus = _procState.VSalineBolus();

   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;


   currentsubstate.EndVin = getVinPrime() + cobeConfig.VinInterfaceSetup;
   currentsubstate.Vsvn   = cobeConfig.ReservoirVolume;
   float dynamichct = ( TBVec * initialCrit) / ( TBVec + currentvbolus + cobeConfig.ACPrimeVolume + currentsubstate.EndVin / ratio);

   currentsubstate.Hend = ((initialCrit) * (TBV - getExtracorporealVolume())) / (TBV - getExtracorporealVolume() + cobeConfig.ACPrimeVolume + currentsubstate.EndVin / ratio);

   float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

   if (QnTBVMax > _config.getMaxNeedleRetFlow())
      QnTBVMax = _config.getMaxNeedleRetFlow();

   Calc_No_collection instflows;

   instflows.Initialize(dynamichct,
                        _procState.PreCount(),
                        TBV,
                        _I,
                        ratio,
                        45.0f,    // IT 11825 limit to 45
                        cobeConfig.MaxInstantQreturn,
                        QnTBVMax);

   if (instflows.calculate(&currentsubstate))
      return; // flow error

   currentsubstate.T       = cobeConfig.VinInterfaceSetup / currentsubstate.Qininstd; // assumed no return cycle in this substate
   currentsubstate.Qpinstd = 0.0f;
   currentsubstate.Qpinstr = 0.0f;
   currentsubstate.Qcinstd = 0.0f;
   currentsubstate.Qcinstr = 0.0f;
   AccumulateVolumes();

   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              _procState.PlasmaVolume(),  _procState.AcInPlasma(),
                                                                                              _procState.RbcVolume(), _procState.AcInRbc(),
                                                                                              currentsubstate.AccumVrep + currentvbolus, currentsubstate.AccumVac);

}

//
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
//   rbc setup 1 calculations
//
void procedureDetailRBCPls::RBCSetup1 ()
{

   const IntermediateResults& plasmasubstate   = substate[Config::__RP_PLASMA_ONLY];
   const IntermediateResults* previoussubstate = 0;


   IntermediateResults& currentsubstate = substate[Config::__RP_RBC_SETUP_1];
   memset(&currentsubstate, 0, sizeof(IntermediateResults));

   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   //
   //  Our starting conditions
   //
   const float currentvbolus = _procState.VSalineBolus();

   // Set initial values.
   _RBCSetup1Time = 0.0f;
   _RBCSetup1Vin  = 0.0f;
   _RBCSetup1Vac  = 0.0f;

   // Determine the correct previous substate
   previoussubstate = (plasmasubstate.T > 0) ? &substate[Config::__RP_PLASMA_ONLY] : &substate[Config::__RP_CHANNEL_SETUP];

   float endingDonorHct = previoussubstate->Hend;   // In case no setup

   //
   //   Calculate predicted values for setup if press through filtration is active
   //   and we have a RBC target and we haven't collected red cells (past the substate).
   //
   if ( _PTF && (_procState.RbcVolume() == 0.0f) && (_Vrtarg > 0.0f) )
   {
      Calc_Rbcpl_RBC_Setup setupflows;

      // RBC Setup constants
      const float currentRBCSetupVolume = _procState.RbcSetup1Volume();

      const float rbcSetup1Hrbc         = cobeConfig.PtfRbcPlsSetup1Hrbc / 100.0f;
      const float rbcSetup1Volume       = (cobeConfig.PtfRbcPlsSetup1Volume > 0.0f) ? cobeConfig.PtfRbcPlsSetup1Volume : currentRBCSetupVolume;

      const float TBV                   =   _procState.Tbv();
      const float TBVec                 = TBV - cobeConfig.RBCPlsExtracorporealVolume;
      const float initialCrit           = _procState.InitialHct() ;
      const float ratio                 =  _config.getACRatioDuringRBC() ;

      // Calculate Max Needle flow
      float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

      if (QnTBVMax > _config.getMaxNeedleRetFlow())
         QnTBVMax = _config.getMaxNeedleRetFlow();

      // Calculate substate flows
      setupflows.Initialize(_hct, _procState.PreCount(), TBV, _I, ratio, _procState.getQinistdMax(), cobeConfig.MaxInstantQreturn, QnTBVMax, rbcSetup1Hrbc);
      setupflows.calculate(&currentsubstate);

      _RBCSetup1Time = (rbcSetup1Volume - currentRBCSetupVolume) / currentsubstate.Qininstd;
      _RBCSetup1Vin  = rbcSetup1Volume - currentRBCSetupVolume;
      _RBCSetup1Vac  = _RBCSetup1Vin / ratio;

      endingDonorHct = (TBVec * initialCrit) / (TBVec - previoussubstate->AccumVp - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + _RBCSetup1Vac);
   }

   currentsubstate.EndVin = previoussubstate->EndVin + _RBCSetup1Vin;
   currentsubstate.T      = _RBCSetup1Time;
   currentsubstate.Vac    = _RBCSetup1Vac;
   currentsubstate.Hend   = endingDonorHct;

   AccumulateVolumes();
   currentsubstate.HypovolemiaDistance =  _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                               _procState.VAccumOverdraw(),
                                                                                               0.0f, 0.0f, // collect volumes
                                                                                               previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                               currentsubstate.Vr, currentsubstate.Vacr,
                                                                                               previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep, /* previoussubstate->AccumVac+ */ currentsubstate.Vac);
}



//
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
//   rbc setup 2 calculations
//
void procedureDetailRBCPls::RBCSetup2 ()
{

   const IntermediateResults& plasmasubstate    = substate[Config::__RP_PLASMA_ONLY];
   const IntermediateResults& rbcsetup1substate = substate[Config::__RP_RBC_SETUP_1];
   const IntermediateResults* previoussubstate  = 0;

   IntermediateResults&       currentsubstate   = substate[Config::__RP_RBC_SETUP_2];
   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   //
   //  Our starting conditions
   //
   const float currentvbolus = _procState.VSalineBolus();

   // Set initial values
   _RBCSetup2Time = 0.0f;
   _RBCSetup2Vin  = 0.0f;
   _RBCSetup2Vac  = 0.0f;

   // Determine the correct previous substate
   if (rbcsetup1substate.T > 0)  // Can be 0 if turned off in cobe config
   {
      previoussubstate = &substate[Config::__RP_RBC_SETUP_1];
   }
   else if (plasmasubstate.T > 0)
   {
      previoussubstate = &substate[Config::__RP_PLASMA_ONLY];
   }
   else
   {
      previoussubstate = &substate[Config::__RP_CHANNEL_SETUP];
   }


   float endingDonorHct = previoussubstate->Hend;  // In case no setup

   //
   //   Calculate predicted values for setup if press through filtration is active
   //   and we have a RBC target and we haven't collected red cells (past the substate).
   //
   if ( _PTF && (_procState.RbcVolume() == 0.0f) && (_Vrtarg > 0.0f) )
   {
      Calc_Rbcpl_RBC_Setup setupflows;


      const float ratio     =  _config.getACRatioDuringRBC();
      const float scaledHin =  (endingDonorHct * (1.0f - 1.0f / ratio)) * cobeConfig.PtfHctScalingFactor;

      // RBC Setup constants
      const float currentRBCSetupVolume =  _procState.RbcSetup2Volume();

      const float rbcSetup2Hrbc         =  scaledHin; // cobeConfig.PtfRbcPlsSetup2Hrbc / 100.0f;
      const float rbcSetup2Volume       = (cobeConfig.PtfRbcPlsSetup2Volume > 0.0f) ? cobeConfig.PtfRbcPlsSetup2Volume : currentRBCSetupVolume;

      const float TBV                   =  _procState.Tbv();
      const float TBVec                 = TBV - cobeConfig.RBCPlsExtracorporealVolume;
      const float initialCrit           =  _procState.InitialHct();

      // Calculate Max Needle flow
      float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

      if (QnTBVMax > _config.getMaxNeedleRetFlow())
         QnTBVMax = _config.getMaxNeedleRetFlow();

      // Calculate substate flows
      setupflows.Initialize(_hct,
                            _procState.PreCount(),
                            _procState.Tbv(),
                            _I,
                            ratio,
                            _procState.getQinistdMax(),
                            cobeConfig.MaxInstantQreturn,
                            QnTBVMax,
                            rbcSetup2Hrbc);

      setupflows.calculate(&currentsubstate);

      _RBCSetup2Time = (rbcSetup2Volume - currentRBCSetupVolume) / currentsubstate.Qininstd;
      _RBCSetup2Vin  = rbcSetup2Volume - currentRBCSetupVolume;

      _RBCSetup2Vac  = _RBCSetup2Vin / ratio;

      endingDonorHct = (TBVec * initialCrit) / (TBVec - previoussubstate->AccumVp - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + _RBCSetup2Vac);
   }

   currentsubstate.EndVin = previoussubstate->EndVin + _RBCSetup2Vin;
   currentsubstate.T      = _RBCSetup2Time;
   currentsubstate.Vac    = _RBCSetup2Vac;
   currentsubstate.Hend   = endingDonorHct;

   AccumulateVolumes();
   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                              currentsubstate.Vr, currentsubstate.Vacr,
                                                                                              previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep, /* previoussubstate->AccumVac+ */ currentsubstate.Vac);
}


//
// ---------------------------------------------------------------------------------------
//   RBC collection calculations
//
// Inputs:
// previoussubstate Hend, EndVin
//
void procedureDetailRBCPls::RBCCollection ()
{

   // DataLog(log_level_predict_info) << "====RBCCollection=== " <<  endmsg;
   IntermediateResults&       currentsubstate  = substate[Config::__RP_RBC];

   const IntermediateResults* previoussubstate = &substate[Config::__RP_PLASMA_ONLY];

   // If PTF and collecting RBCs, set correct substate
   if (_PTF && (_Vrtarg > 0.0f))
   {
      previoussubstate = &substate[Config::__RP_RBC_SETUP_2];
   }

   //
   //  Our starting conditions
   //
   const float currentvr     = _procState.RbcVolume();
   const float currentvacr   = _procState.AcInRbc();
   float       currentdose   = _procState.RbcDose(); // also used as accumulator
   const float currentvbolus =  _procState.VSalineBolus();


   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   const float TBV             =  _procState.Tbv();
   const float TBVec           = TBV - cobeConfig.RBCPlsExtracorporealVolume;
   const float ratio           = _config.getACRatioDuringRBC();
   const float collectCrit     = getPredRBCCrit() / 100.0f;
   float       doseneeded      = getPredRBCDose();
   const int   replaceRequired = ReplacementFluidRequired();
   const float replacePercent  = _config.getFluidBal() / 100.0f;


   // collect 1/2 the dose here and the other half in  RBCCollectionDouble
   if (doingPTFDouble())
   {
      doseneeded = doseneeded / 2.0f;
   }


   // end this now!
   if (_procState.PtfRbcProd1Disabled())
   {

      DataLog(log_level_predict_info) << "Disable RBC collect  curr dose->" <<  currentdose << endmsg;
      doseneeded = currentdose;
   }

   const float initialCrit =  _procState.InitialHct();



   ////////////////////////////////////////////////////////////////////////////////////////
   //
   //  New black stamp limiting of Qn (9227)
   //

   float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

   float ResCrit  = _procState.ResHct() * 100.0f;
   if   (ResCrit > 80.0f) ResCrit = 80.0f;

   float A_val = cobeConfig.QnCoeffThirdOrder  * ResCrit * ResCrit * ResCrit;
   float B_val = cobeConfig.QnCoeffSecondOrder * ResCrit * ResCrit;
   float C_val = cobeConfig.QnCoeffFirstOrder  * ResCrit;

   float QnCap = A_val + B_val + C_val + cobeConfig.QnConst;

   if (QnCap > _procState.AdjQrpCap() )
      QnCap = _procState.AdjQrpCap();

   if (QnTBVMax > QnCap )
   {
      DataLog(log_level_predict_info) << "Needle flow capped at " << QnCap  << " ;calculated to be ->" << QnTBVMax << endmsg;
      QnTBVMax = QnCap;
   }
   ////////////////////////////////////////////////////////////////////////////////////////

   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   // dynamic donor crit.
   // Initialize to previous substate hct or to current calculated hct (best guess vs current reality).
   float hct  = ( (AlgSubstate(_procState.SubState()) < Config::__RP_RBC) ? previoussubstate->Hend : _procState.Hct() );

   float facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));
   currentsubstate.EndVin = previoussubstate->EndVin; // in case no rbcs collected


   Calc_Rbcpl_rbc_collection     non_ptf_rbc_flows;
   Calc_Rbcpl_ptf_rbc_collection ptf_rbc_flows;
   Calc_Rbcpl_rbc_collection*    rbcflows = NULL;

   // assume non PTF
   rbcflows = &non_ptf_rbc_flows;
   float HCT_Delta = 0.0f;  // no fudging for non PTF


   // Use PTF flows if using PTF sets.
   if (_PTF)
   {
      rbcflows  = &ptf_rbc_flows;
      HCT_Delta = cobeConfig.RbcHctDelta;
   }

   memset(&_rbcsubstate, 0, sizeof(IntermediateResults));
   _rbcsubstate.Csf = cobeConfig.CsfMax;
   _rbcsubstate.Rpm = cobeConfig.RPMCollect;

   float Qrbcmax = _procState.getQinistdMax();

   // not limited unless PTF active
   if (_PTF && (Qrbcmax > cobeConfig.PtfQrbc) )
      Qrbcmax = cobeConfig.PtfQrbc;

   int   firstpass    = 1;
   int   rbcSatisfied = (currentdose >= doseneeded);
   float newI         = _I;

   while (!rbcSatisfied)
   {
      //
      //   Calculate flows for this draw cycle
      //


      float lastI  = newI;

      float dratio = 1.0f;
      do
      {

         rbcflows->Initialize(hct,
                              _procState.PreCount(),
                              TBV,
                              newI,
                              ratio,
                              _procState.getQinistdMax(),
                              cobeConfig.MaxInstantQreturn,
                              QnTBVMax,
                              replaceRequired,
                              replacePercent,
                              collectCrit,
                              Qrbcmax,
                              HCT_Delta);

         if (rbcflows->calculate(&_rbcsubstate))
            return;  // flow error



         if (_PTF)
         {

            lastI = newI;

            ///////////////////////////////////////////////////////////
            //////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////
            // float Control_IR    = instantaneousInfusion(parent,substateindex);
            ///////////////////////////////////////////////////////////////////

            float ACinDraw = _rbcsubstate.Qacinst * _rbcsubstate.Td;

            float acfactor = 1.0f;
            float TBV      =  _procState.Tbv() / 1000.0f;



            if (_rbcsubstate.Vsvn > 0.0f)     // divide by zero protection, Vsvn should always be valid
            {
               acfactor  = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstate.Vsvn;
               ACinDraw *= acfactor;
            }

            //
            //
            //   Use current facb instead of average to allow for dynamic hct changes
            //
            float ACinProduct = 0.0f;

            ACinProduct = (_rbcsubstate.Qininstd - _rbcsubstate.Qpinstd) * _rbcsubstate.Td * _rbcsubstate.Facb * (1.0f - (getPredRBCCrit() / 100.0f) );
            float Control_IR = (ACinDraw - ACinProduct) / _rbcsubstate.Tcn / TBV;


            ///////////////////////////////////////////////////////////////////
            // float Saftey_IR     = instantaneousInfusion(parent,substateindex,true);  // do safety like calc
            ///////////////////////////////////////////////////////////////////

            ACinDraw = _rbcsubstate.Qacinst * _rbcsubstate.Td;
            if (_rbcsubstate.Vsvn > 0.0f)     // divide by zero protection, Vsvn should always be valid
            {
               acfactor  = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstate.Vsvn;
               ACinDraw *= acfactor;
            }


            //
            //
            //   Use current facb instead of average to allow for dynamic hct changes
            //
            float calcCrit = getPredRBCCrit() / 100.0f + cobeConfig.RbcHctDelta;    // getRBCCalculatedCrit();

            // if this is a pre prediction then we have to guess at the crit fudge effect
            // we will use the control crit plus the CC fudge delta
            if (   _procState.RbcProduct_A_FinalVolume()==0.0f
                   && _procState.RbcProduct_B_FinalVolume()==0.0f)
            {
               calcCrit = getPredRBCCrit() / 100.0f + cobeConfig.RbcHctDelta;
            }

            ACinProduct = (_rbcsubstate.Qininstd - _rbcsubstate.Qpinstd) * _rbcsubstate.Td * _rbcsubstate.Facb * (1.0f - (calcCrit) );
            float Saftey_IR = (ACinDraw - ACinProduct) / _rbcsubstate.Tcn / TBV;


            // DataLog(log_level_predict_info) << "  Saftey_like_IR = " << Saftey_IR << endmsg;
            ///////////////////////////////////////////////////////////////////


            float       MaxACRate    = getMaxACRate();
            const float adjustedRate =   _procState.AdjIrCap();
            //
            // DataLog(log_level_predict_info) << "  Control_IR= " << Control_IR << endmsg;
            // DataLog(log_level_predict_info) << "     $Controling to a set max rate of = " << MaxACRate << endmsg;
            // DataLog(log_level_predict_info) << "     $Tingling Adjusted Rate = " << adjustedRate << endmsg;

            // see if we need to use teh adjusted Rate
            if (MaxACRate > adjustedRate)
            {
               MaxACRate =  adjustedRate;
            }

            if (Saftey_IR != 0.0f)
            {
               // take effect once safety reaches ~1.2
               if (Saftey_IR >= MaxACRate )
               {
                  dratio = (MaxACRate / Saftey_IR);
               }

            }
            else
            {

               dratio = 1.0f;
            }


            ///////////////////////////
            //  just some sanity checks.....
            // never speed up
            if (dratio > 1.0f)
            {
               dratio = 1.0f;
            }

            if (Saftey_IR >= MaxACRate)
            {
               newI =  dratio * Control_IR;
            }

            // DataLog(log_level_predict_info) << "      IR Damping Factor= " << dratio << endmsg;


            /////////////////////////////////////////////////////////
            ////////////////////////////////////////////////////////////
            // newI = IRDampingfactor(&parent,Config::__RP_RBC);
            // DataLog(log_level_predict_info) << "       lastI ->" <<  lastI << endmsg;
            // DataLog(log_level_predict_info) << "       newI ->" <<  newI << endmsg;
         }

      } while ( _PTF &&  ( lastI > newI ) );


      //
      //  First time through - these are the commanded flows for the moment.  Save
      //  these in the official repository.
      //
      if (firstpass)
      {

         // DataLog(log_level_predict_info) << " Infusion Rate currently used in CollectDbl ->" <<  newI << endmsg;

         firstpass              = 0;
         currentsubstate        = _rbcsubstate;
         currentsubstate.EndVin = previoussubstate->EndVin;
      }

      //
      //  Calculate rbcs collected during this cycle and apply to our target
      //
      //
      float dcf       = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstate.Vsvn;
      float dose      = (_rbcsubstate.Qininstd - _rbcsubstate.Qpinstd) * collectCrit * _rbcsubstate.Td;
      float pctneeded = 1.0f;



      //
      //    Add collected to totals
      //
      if ((currentdose + dose) >= doseneeded)
      {
         rbcSatisfied = 1;
         pctneeded    = (doseneeded - currentdose) / dose;

         if (pctneeded < 0.0f)
            pctneeded = 0.0f;
      }

      currentdose            += dose * pctneeded;
      currentsubstate.T      += (_rbcsubstate.Td + _rbcsubstate.Tr) * pctneeded;
      currentsubstate.Vr     += (dose / collectCrit) * pctneeded;
      currentsubstate.Vacr   += (dose / collectCrit) * facb * (1.0f - collectCrit);
      currentsubstate.Vrep   += ((_rbcsubstate.Qcinstd * _rbcsubstate.Td) * pctneeded);
      currentsubstate.Vac    += (_rbcsubstate.Qininstd * _rbcsubstate.Td / ratio) * dcf * pctneeded;
      currentsubstate.EndVin += _rbcsubstate.Qininstd * _rbcsubstate.Td * pctneeded;

      //
      //
      //  recalculate dynamic donor hct for next pass through
      //
      hct  = ( ( TBVec * initialCrit - currentdose) / (TBVec - previoussubstate->AccumVp - currentvr - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + currentsubstate.Vac) );
      facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));
   }

   currentsubstate.Hend  = hct;
   currentsubstate.Vr   += currentvr; // by convention since no "accumVr" exists
   currentsubstate.Vacr += currentvacr;

   //
   //
   //  Save off hypo information for next pass (if needed)
   //
   _rbcsubstate.Vr   = currentsubstate.Vr;
   _rbcsubstate.Vacr = currentsubstate.Vacr;
   _rbcsubstate.Vrep = currentsubstate.Vrep;
   _rbcsubstate.Vac  = currentsubstate.Vac;
   _rbcsubstate.T    = currentsubstate.T;


   if (_procState.PtfRbcProd1Disabled())
   {
      DataLog(log_level_predict_info) << "Override  set to Current Vin _rbcsubstate.EndVin " <<  _procState.Vin() << endmsg;
      _rbcsubstate.EndVin =  _procState.Vin();
   }

   AccumulateVolumes();

   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                              currentsubstate.Vr, currentsubstate.Vacr,
                                                                                              previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep, previoussubstate->AccumVac + currentsubstate.Vac);

}



//
// ---------------------------------------------------------------------------------------
// DOUBLE
// ---------------------------------------------------------------------------------------
//   rbc setup 1 calculations
//
void procedureDetailRBCPls::RBCSetup1Double ()
{

   if ( !doingPTFDouble() ) return;  // only for doubles


   const IntermediateResults* previoussubstate = 0;

   IntermediateResults&       currentsubstate  = substate[Config::__RP_RBC_SETUP_1_DOUBLE];
   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   //
   //  Our starting conditions
   //
   const float currentvbolus =  _procState.VSalineBolus();

   // Set initial values.
   _RBCSetup1DoubleTime = 0.0f;
   _RBCSetup1DoubleVin  = 0.0f;
   _RBCSetup1DoubleVac  = 0.0f;

   // Determine the correct previous substate
   previoussubstate =  &substate[Config::__RP_RBC];

   float endingDonorHct = previoussubstate->Hend;

   if ( doingPTFDouble() )
   {
      Calc_Rbcpl_RBC_Setup setupflows;

      // RBC Setup constants
      const float currentRBCSetupVolume = _procState.RbcSetup1DoubleVolume();

      const float rbcSetup1Hrbc         = cobeConfig.PtfRbcPlsSetup1BHrbc / 100.0f;
      const float rbcSetup1Volume       = (cobeConfig.PtfRbcPlsSetup1BVolume > 0.0f) ? cobeConfig.PtfRbcPlsSetup1BVolume : currentRBCSetupVolume;
      const float TBV                   =  _procState.Tbv();
      const float TBVec                 = TBV - cobeConfig.RBCPlsExtracorporealVolume;
      const float initialCrit           = previoussubstate->Hend;
      const float ratio                 = _config.getACRatioDuringRBC();

      // Calculate Max Needle flow
      float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

      if (QnTBVMax > _config.getMaxNeedleRetFlow())
         QnTBVMax = _config.getMaxNeedleRetFlow();

      // Calculate substate flows
      setupflows.Initialize(_hct,
                            _procState.PreCount(),
                            TBV,
                            _I,
                            ratio,
                            _procState.getQinistdMax(),
                            cobeConfig.MaxInstantQreturn,
                            QnTBVMax,
                            rbcSetup1Hrbc);

      setupflows.calculate(&currentsubstate);

      _RBCSetup1DoubleTime = (rbcSetup1Volume - currentRBCSetupVolume) / currentsubstate.Qininstd;
      _RBCSetup1DoubleVin  = rbcSetup1Volume - currentRBCSetupVolume;
      _RBCSetup1DoubleVac  = _RBCSetup1DoubleVin / ratio;

      endingDonorHct       = (TBVec * initialCrit) / (TBVec - previoussubstate->AccumVp - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + _RBCSetup1DoubleVac);
   }

   currentsubstate.EndVin = previoussubstate->EndVin + _RBCSetup1DoubleVin;
   currentsubstate.T      = _RBCSetup1DoubleTime;
   currentsubstate.Vac    = _RBCSetup1DoubleVac;
   currentsubstate.Hend   = endingDonorHct;

   AccumulateVolumes();
   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                              currentsubstate.Vr, currentsubstate.Vacr,
                                                                                              previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep, /* previoussubstate->AccumVac+ */ currentsubstate.Vac);
}



//
// ---------------------------------------------------------------------------------------
// DOUBLE
// ---------------------------------------------------------------------------------------
//   rbc setup 2 calculations
//
void procedureDetailRBCPls::RBCSetup2Double ()
{


   if ( !doingPTFDouble() ) return; // only for doubles

   const IntermediateResults& rbcsetup1substate = substate[Config::__RP_RBC_SETUP_1_DOUBLE];
   const IntermediateResults* previoussubstate  = 0;

   IntermediateResults&       currentsubstate   = substate[Config::__RP_RBC_SETUP_2_DOUBLE];
   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   //
   //  Our starting conditions
   //
   const float currentvbolus =  _procState.VSalineBolus();

   // Set initial values
   _RBCSetup2DoubleTime = 0.0f;
   _RBCSetup2DoubleVin  = 0.0f;
   _RBCSetup2DoubleVac  = 0.0f;


   float endingDonorHct;  // In case no setup
   float lastHend;
   // Determine the correct previous substate
   if (rbcsetup1substate.T > 0)  // Can be 0 if turned off in cobe config
   {
      previoussubstate = &substate[Config::__RP_RBC_SETUP_1_DOUBLE];
      endingDonorHct   = previoussubstate->Hend; // In case of setup1
   }
   else
   {
      previoussubstate = &substate[Config::__RP_RBC];
      endingDonorHct   = previoussubstate->Hend; // In case of setup1
   }
   lastHend = endingDonorHct;


   //
   //   Calculate predicted values for setup if press through filtration is active
   //   and we have a RBC target and we haven't collected red cells (past the substate).
   //
   if ( doingPTFDouble() )
   {
      Calc_Rbcpl_RBC_Setup setupflows;


      const float ratio     = _config.getACRatioDuringRBC();

      const float scaledHin =  (previoussubstate->Hend * (1.0f - 1.0f / ratio)) * cobeConfig.PtfHctScalingFactor;

      // RBC Setup constants
      const float currentRBCSetupVolume = _procState.RbcSetup2DoubleVolume();

      const float rbcSetup2Hrbc         = scaledHin;
      const float rbcSetup2Volume       = (cobeConfig.PtfRbcPlsSetup2BVolume > 0.0f) ? cobeConfig.PtfRbcPlsSetup2BVolume : currentRBCSetupVolume;

      const float TBV                   =  _procState.Tbv();
      const float TBVec                 = TBV - cobeConfig.RBCPlsExtracorporealVolume;
      const float initialCrit           = lastHend;

      // Calculate Max Needle flow
      float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;

      if (QnTBVMax > _config.getMaxNeedleRetFlow())
         QnTBVMax = _config.getMaxNeedleRetFlow();

      // Calculate substate flows
      setupflows.Initialize(_hct,
                            _procState.PreCount(),
                            _procState.Tbv(),
                            _I,
                            ratio,
                            _procState.getQinistdMax(),
                            cobeConfig.MaxInstantQreturn,
                            QnTBVMax,
                            rbcSetup2Hrbc);

      setupflows.calculate(&currentsubstate);

      _RBCSetup2DoubleTime = (rbcSetup2Volume - currentRBCSetupVolume) / currentsubstate.Qininstd;
      _RBCSetup2DoubleVin  = rbcSetup2Volume - currentRBCSetupVolume;

      _RBCSetup2DoubleVac  = _RBCSetup2DoubleVin / ratio;

      endingDonorHct       = (TBVec * initialCrit) / (TBVec - previoussubstate->AccumVp - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + _RBCSetup2DoubleVac);
   }

   currentsubstate.EndVin = previoussubstate->EndVin + _RBCSetup2DoubleVin;
   currentsubstate.T      = _RBCSetup2DoubleTime;
   currentsubstate.Vac    = _RBCSetup2DoubleVac;
   currentsubstate.Hend   = endingDonorHct;

   AccumulateVolumes();
   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                              currentsubstate.Vr, currentsubstate.Vacr,
                                                                                              previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep, /* previoussubstate->AccumVac+ */ currentsubstate.Vac);
}


//
// DOUBLE
// ---------------------------------------------------------------------------------------
//   RBC collection calculations
// ---------------------------------------------------------------------------------------
//
void procedureDetailRBCPls::RBCCollectionDouble ()
{

   // DataLog(log_level_predict_info) << "   ====RBCCollectionDouble=== " <<  endmsg;
   if ( !doingPTFDouble() ) return; // only for PTF doubles


   IntermediateResults&       currentsubstate  = substate[Config::__RP_RBC_DOUBLE];
   const IntermediateResults* firsthalfRBC     = &substate[Config::__RP_RBC];
   const IntermediateResults* previoussubstate = &substate[Config::__RP_RBC_SETUP_2_DOUBLE];



   //
   //  Our starting conditions
   //
   float currentvr =   _procState.RbcDoubleVolume();
   if (( _procState.RbcDoubleVolume() < getPredRBCVolume() / 2.0f)
       && !(_procState.PtfRbcProd1Disabled())
       )
   {
      currentvr = getPredRBCVolume() / 2.0f;
   }

   float DoseThisProduct = _procState.RbcDoubleDose();

   float currentdose     =  _procState.RbcDose();
   if ( (_procState.RbcDose() < getPredRBCDose() / 2.0f)
        && !(_procState.PtfRbcProd1Disabled())
        )
   {
      currentdose =  getPredRBCDose() / 2.0f ;
   }


   const float currentvacr   = _procState.AcInRbc();
   const float currentvbolus =  _procState.VSalineBolus();


   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   const float TBVml           =  _procState.Tbv();
   const float TBVec           = TBVml - cobeConfig.RBCPlsExtracorporealVolume;
   const float ratio           = _config.getACRatioDuringRBC();
   const float collectCrit     = getPredRBCCrit() / 100.0f;
   float       doseneeded      = getPredRBCDose();
   const int   replaceRequired = ReplacementFluidRequired();
   const float replacePercent  = _config.getFluidBal() / 100.0f;



   if (_procState.PtfRbcProd1Disabled())
   {
      doseneeded = getPredRBCDose() / 2.0f + _procState.RbcDose();
   }

   // Pick up where collect A left off!
   const float initialCrit = firsthalfRBC->Hend;



   ////////////////////////////////////////////////////////////////////////////////////////
   //
   //  New black stamp limiting of Qn (9227)
   //

   float QnTBVMax = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBVml;

   float ResCrit  = _procState.ResHct() * 100.0f;
   if   (ResCrit > 80.0f) ResCrit = 80.0f;

   float A_val = cobeConfig.QnCoeffThirdOrder  * ResCrit * ResCrit * ResCrit;
   float B_val = cobeConfig.QnCoeffSecondOrder * ResCrit * ResCrit;
   float C_val = cobeConfig.QnCoeffFirstOrder  * ResCrit;

   float QnCap = A_val + B_val + C_val + cobeConfig.QnConst;

   if (QnCap > _procState.AdjQrpCap() )
      QnCap = _procState.AdjQrpCap();

   if (QnTBVMax > QnCap)
   {
      DataLog(log_level_predict_info) << "Needle flow capped at " << QnCap  << " ;calculated to be ->" << QnTBVMax << endmsg;
      QnTBVMax = QnCap;
   }

   ////////////////////////////////////////////////////////////////////////////////////////


   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   // dynamic donor crit.
   // Initialize to previous substate hct or to current calculated hct (best guess vs current reality).
   float hct  = ( (AlgSubstate(_procState.SubState()) < Config::__RP_RBC_DOUBLE) ? initialCrit : _procState.Hct() );

   float facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));
   currentsubstate.EndVin = previoussubstate->EndVin; // in case no rbcs collected

   Calc_Rbcpl_ptf_rbc_collection ptf_rbc_flows;


   memset(&_rbcsubstateDouble, 0, sizeof(IntermediateResults));
   _rbcsubstateDouble.Csf = cobeConfig.CsfMax;
   _rbcsubstateDouble.Rpm = cobeConfig.RPMCollect;

   float Qrbcmax = _procState.getQinistdMax();

   // not limited unless PTF active
   if (_PTF && (Qrbcmax > cobeConfig.PtfQrbc) )
      Qrbcmax = cobeConfig.PtfQrbc;

   int         firstpass    = 1;
   int         rbcSatisfied = (currentdose >= doseneeded);

   const float HCT_Delta    = cobeConfig.RbcHctDelta;
   float       newI         = _I;



   while (!rbcSatisfied)
   {

      //
      //   Calculate flows for this draw cycle
      float lastI  = newI;

      float dratio = 1.0f;

      do
      {

         ptf_rbc_flows.Initialize(hct,
                                  _procState.PreCount(),
                                  TBVml,
                                  newI,
                                  ratio,
                                  _procState.getQinistdMax(),
                                  cobeConfig.MaxInstantQreturn,
                                  QnTBVMax,
                                  replaceRequired,
                                  replacePercent,
                                  collectCrit,
                                  Qrbcmax,
                                  HCT_Delta);


         if (ptf_rbc_flows.calculate(&_rbcsubstateDouble))
            return;    // flow error

         lastI = newI;

         ///////////////////////////////////////////////////////////
         //////////////////////////////////////////////////////////
         ///////////////////////////////////////////////////////////////////
         // float Control_IR    = instantaneousInfusion(parent,substateindex);
         ///////////////////////////////////////////////////////////////////

         float ACinDraw = _rbcsubstateDouble.Qacinst * _rbcsubstateDouble.Td;
         float acfactor = 1.0f;
         float TBV_L    =  _procState.Tbv() / 1000.0f;

         if (_rbcsubstateDouble.Vsvn > 0.0f)        // divide by zero protection, Vsvn should always be valid
         {
            acfactor  = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstateDouble.Vsvn;
            ACinDraw *= acfactor;
         }


         //
         //
         //   Use current facb instead of average to allow for dynamic hct changes
         //
         float ACinProduct = 0.0f;

         ACinProduct = (_rbcsubstateDouble.Qininstd - _rbcsubstateDouble.Qpinstd) * _rbcsubstateDouble.Td * _rbcsubstateDouble.Facb * (1.0f - (getPredRBCCrit() / 100.0f) );
         float Control_IR = (ACinDraw - ACinProduct) / _rbcsubstateDouble.Tcn / TBV_L;

         ///////////////////////////////////////////////////////////////////
         // float Saftey_IR     = instantaneousInfusion(parent,substateindex,true);  // do safety like calc
         ///////////////////////////////////////////////////////////////////

         ACinDraw = _rbcsubstateDouble.Qacinst * _rbcsubstateDouble.Td;
         if (_rbcsubstateDouble.Vsvn > 0.0f)        // divide by zero protection, Vsvn should always be valid
         {
            acfactor  = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstateDouble.Vsvn;
            ACinDraw *= acfactor;
         }

         //
         //
         //   Use current facb instead of average to allow for dynamic hct changes
         //
         float calcCrit = getPredRBCCrit() / 100.0f + cobeConfig.RbcHctDelta;    // getRBCCalculatedCrit();

         // if this is a pre prediction then we have to guess at the crit fudge effect
         // we will use the control crit plus the CC fudge delta
         if (   _procState.RbcProduct_A_FinalVolume()==0.0f
                && _procState.RbcProduct_B_FinalVolume()==0.0f)
         {
            calcCrit = getPredRBCCrit() / 100.0f + cobeConfig.RbcHctDelta;
         }

         ACinProduct = (_rbcsubstateDouble.Qininstd - _rbcsubstateDouble.Qpinstd) * _rbcsubstateDouble.Td * _rbcsubstateDouble.Facb * (1.0f - (calcCrit) );
         float Saftey_IR = (ACinDraw - ACinProduct) / _rbcsubstateDouble.Tcn / TBV_L;



         // DataLog(log_level_predict_info) << "  Saftey_like_IR = " << Saftey_IR << endmsg;
         ///////////////////////////////////////////////////////////////////


         float       MaxACRate    = getMaxACRate();
         const float adjustedRate = _procState.AdjIrCap();

         // DataLog(log_level_predict_info) << "  Control_IR= " << Control_IR << endmsg;
         // DataLog(log_level_predict_info) << "     $Controling to a set max rate of = " << MaxACRate << endmsg;
         // DataLog(log_level_predict_info) << "     $Tingling Adjusted Rate = " << adjustedRate << endmsg;

         // see if we need to use teh adjusted Rate
         if (MaxACRate > adjustedRate)
         {
            MaxACRate =  adjustedRate;
         }

         if (Saftey_IR != 0.0f)
         {
            // take effect once safety reaches ~1.2
            if (Saftey_IR >= MaxACRate )
            {
               dratio = (MaxACRate / Saftey_IR);
            }

         }
         else
         {

            dratio = 1.0f;
         }


         ///////////////////////////
         //  just some sanity checks.....
         // never speed up
         if (dratio > 1.0f)
         {
            dratio = 1.0f;
         }

         if (Saftey_IR >= MaxACRate)
         {
            newI =  dratio * Control_IR;
         }

         // DataLog(log_level_predict_info) << "      IR Damping Factor= " << dratio << endmsg;


         /////////////////////////////////////////////////////////
         ////////////////////////////////////////////////////////////
         // newI = IRDampingfactor(&parent,Config::__RP_RBC);
         // DataLog(log_level_predict_info) << "       lastI ->" <<  lastI << endmsg;
         // DataLog(log_level_predict_info) << "       newI ->" <<  newI << endmsg;



      } while (lastI > newI);



      //
      //  First time through - these are the commanded flows for the moment.  Save
      //  these in the official repository.
      //
      if (firstpass)
      {
         // DataLog(log_level_predict_info) << " Infusion Rate currently used in CollectDouble ->" <<  newI << endmsg;
         firstpass              = 0;
         currentsubstate        = _rbcsubstateDouble;
         currentsubstate.EndVin = previoussubstate->EndVin;
      }

      //
      //  Calculate rbcs collected during this cycle and apply to our target
      //
      float dcf       = 1.0f - cobeConfig.RecirculateSnDrawVol / _rbcsubstateDouble.Vsvn;
      float dose      = (_rbcsubstateDouble.Qininstd - _rbcsubstateDouble.Qpinstd) * collectCrit * _rbcsubstateDouble.Td;
      float pctneeded = 1.0f;



      //
      //    Add collected to totals
      //
      if ((currentdose + dose) >= doseneeded)
      {
         rbcSatisfied = 1;
         pctneeded    = (doseneeded - currentdose) / dose;

         if (pctneeded < 0.0f)
            pctneeded = 0.0f;
      }

      currentdose       += dose * pctneeded;
      currentsubstate.T += (_rbcsubstateDouble.Td + _rbcsubstateDouble.Tr) * pctneeded;

      DoseThisProduct   += dose * pctneeded;


      currentsubstate.Vr     += (dose / collectCrit) * pctneeded;
      currentsubstate.Vacr   += (dose / collectCrit) * facb * (1.0f - collectCrit);
      currentsubstate.Vrep   += ((_rbcsubstateDouble.Qcinstd * _rbcsubstateDouble.Td) * pctneeded);
      currentsubstate.Vac    += (_rbcsubstateDouble.Qininstd * _rbcsubstateDouble.Td / ratio) * dcf * pctneeded;
      currentsubstate.EndVin += _rbcsubstateDouble.Qininstd * _rbcsubstateDouble.Td * pctneeded;

      // DataLog(log_level_predict_info) << "  EndVin += " <<  currentsubstate.EndVin << endmsg;
      //
      //  recalculate dynamic donor hct for next pass through
      //

      hct  = ( ( TBVec * initialCrit - DoseThisProduct) / (TBVec - previoussubstate->AccumVp - currentvr - currentsubstate.Vr + previoussubstate->AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate->AccumVac + currentsubstate.Vac) );
      facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));

   }



   currentsubstate.Hend  = hct;
   currentsubstate.Vr   += currentvr; // by convention since no "accumVr" exists
   currentsubstate.Vacr += currentvacr;
   currentsubstate.Vacr += firsthalfRBC->Vacr; // SPECIAL add in first products AC IT8400

   //
   //
   //  Save off hypo information for next pass (if needed)
   //
   _rbcsubstateDouble.Vr   = currentsubstate.Vr    ;
   _rbcsubstateDouble.Vacr = currentsubstate.Vacr  ;
   _rbcsubstateDouble.Vrep = currentsubstate.Vrep ;
   _rbcsubstateDouble.Vac  = currentsubstate.Vac   ;
   _rbcsubstateDouble.T    = currentsubstate.T ;


   AccumulateVolumes();

   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              previoussubstate->AccumVp, previoussubstate->AccumVacp,
                                                                                              currentsubstate.Vr, currentsubstate.Vacr,
                                                                                              previoussubstate->AccumVrep + currentvbolus + currentsubstate.Vrep,
                                                                                              previoussubstate->AccumVac + currentsubstate.Vac);

}
////////////////////////////////////////////////////////////////////////////////////////
//
// ---------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------
//   plasma collection calculations
//
void procedureDetailRBCPls::PlasmaOnly (const float maxendtime)
{

   const IntermediateResults& previoussubstate = substate[Config::__RP_CHANNEL_SETUP];
   IntermediateResults&       currentsubstate  = substate[Config::__RP_PLASMA_ONLY];

   //
   //  Our starting conditions
   //
   const float currentvp     = _procState.PlasmaVolume();
   const float currentacinvp = _procState.AcInPlasma();
   const float currentvbolus =  _procState.VSalineBolus();

   //
   //
   //   Some pseudo-constants to avoid repeated function calls
   //
   const float TBV   =  _procState.Tbv();
   const float TBVec = TBV - cobeConfig.RBCPlsExtracorporealVolume;

   //
   //
   //   Note ratio for this substate specifically set in configuration
   //
   const float ratio           = _config.getACRatioDuringPlasmaOnly();

   const float initialCrit     = _procState.InitialHct();
   float       QnTBVMax        = _config.getTbvQnretmaxScaleConst() * cobeConfig.TbvQnretMaxCoefficient * TBV;
   if (QnTBVMax > _config.getMaxNeedleRetFlow()) QnTBVMax = _config.getMaxNeedleRetFlow();
   const int   replaceRequired = ReplacementFluidRequired();
   const float replacePercent  = _config.getFluidBal() / 100.0f;
   const int   amapPlasma      = (getProcCollectPlasma() && (!getPlasmaTargetCannotChange()));


   memset(&currentsubstate, 0, sizeof(IntermediateResults));
   currentsubstate.Csf = cobeConfig.CsfMax;
   currentsubstate.Rpm = cobeConfig.RPMCollect;

   float hct  = (AlgSubstate(_procState.SubState()) < Config::__RP_PLASMA_ONLY) ? previoussubstate.Hend : _procState.Hct();
   float facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));
   currentsubstate.EndVin = previoussubstate.EndVin; // in case no plasma collected

   Calc_Rbcpl_plasma_collection plasmaflows;
   IntermediateResults          plsubstate;
   memset(&plsubstate, 0, sizeof(IntermediateResults));
   plsubstate.Csf = cobeConfig.CsfMax;
   plsubstate.Rpm = cobeConfig.RPMCollect;

   int  firstpass       = 1;
   bool plasmaSatisfied = false;
   if (getProcPlasmaProductVol() > 0.0f)   // we are running to product volume not including AC
   {
      plasmaSatisfied = ((currentvp - currentacinvp) > _Vptarg);
   }
   else
   {
      plasmaSatisfied = (currentvp >= _Vptarg);
   }

   while (!plasmaSatisfied)
   {
      //
      //   Calculate flows for this draw cycle
      //
      plasmaflows.Initialize(hct,
                             _procState.PreCount(),
                             TBV,
                             _I,
                             ratio,
                             _procState.getQinistdMax(),
                             cobeConfig.MaxInstantQreturn,
                             QnTBVMax,
                             replaceRequired,
                             replacePercent);

      if (plasmaflows.calculate(&plsubstate))
         return; // flow error

      //
      //
      //  First time through - these are the commanded flows for the moment.  Save
      //  these in the official repository.
      //
      if (firstpass)
      {
         firstpass              = 0;
         currentsubstate        = plsubstate;
         currentsubstate.EndVin = previoussubstate.EndVin;
      }

      //
      //
      //  Calculate plasma collected during this cycle and apply to our target
      //
      float pctneeded = 1.0f;
      float dcf       = 1.0f - cobeConfig.RecirculateSnDrawVol / plsubstate.Vsvn;
      float vp        = plsubstate.Qpinstd * plsubstate.Td;
      float acinvp    = vp * facb;

      //
      //
      //   AMAP can end either on max plasma collected, hypovolemia limit, or time.
      //
      if (amapPlasma)
      {
         //
         //
         //   Check to see if we have exceeded hypvolemia limits.  If so reduce vp collected
         //   this cycle until within constraints.
         //
         float delta = 0.0f;
         while (1)
         {
            float vrepnow   = previoussubstate.AccumVrep + currentsubstate.Vrep + currentvbolus + (plsubstate.Qcinstd * plsubstate.Td) * pctneeded;
            float vacnow    = previoussubstate.AccumVac + currentsubstate.Vac + (plsubstate.Qininstd * plsubstate.Td / ratio) * dcf * pctneeded;
            float remaining = 0.0f;
            // IT 8604
            if (doingPTFDouble())
            {
               remaining = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                _procState.VAccumOverdraw(),
                                                                                0.0f, 0.0f, // collect volumes
                                                                                currentvp + currentsubstate.Vp + vp * pctneeded, currentacinvp + currentsubstate.Vacp + acinvp * pctneeded,
                                                                                _rbcsubstateDouble.Vr, _rbcsubstateDouble.Vacr,
                                                                                vrepnow + _rbcsubstateDouble.Vrep, vacnow + _rbcsubstateDouble.Vac);

            }
            else
            {
               remaining = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                _procState.VAccumOverdraw(),
                                                                                0.0f, 0.0f, // collect volumes
                                                                                currentvp + currentsubstate.Vp + vp * pctneeded, currentacinvp + currentsubstate.Vacp + acinvp * pctneeded,
                                                                                _rbcsubstate.Vr, _rbcsubstate.Vacr,
                                                                                vrepnow + _rbcsubstate.Vrep, vacnow + _rbcsubstate.Vac);
            }
            if (remaining > getHypoMargin()) break;
            delta    += getHypoMargin() - remaining + 0.1f;
            pctneeded = (vp - delta) / vp;
            if (pctneeded < 0.0f) break; // we can't get any more plasma this iteration
         }


         //
         //
         //   Check AMAP time constraint
         //
         if ((currentsubstate.T + (plsubstate.Td + plsubstate.Tr) * pctneeded) > maxendtime)
         {
            float timepctneeded = (maxendtime - currentsubstate.T) / (plsubstate.Td + plsubstate.Tr);

            if (timepctneeded < pctneeded)
               pctneeded = timepctneeded; // in case hypo is more constraining
         }

      }

      //
      //
      //   both fixed and amap plasma volumes can end only on collected volume
      //   currentvp, currentacinvp: starting volumes entering substate
      //   currentsubstate.Vp, currensubstate.Vacp: volumes accrued since entry
      //   vp, acinvp: volumes accrued this cycle
      //
      float fixedpctneeded = 1.0f;
      if (getProcPlasmaProductVol() > 0.0f)   // we are running to product volume not including AC
      {
         fixedpctneeded = (_Vptarg - currentvp + currentacinvp - currentsubstate.Vp + currentsubstate.Vacp) / (vp - acinvp);
      }
      else
      {
         fixedpctneeded = (_Vptarg - currentvp - currentsubstate.Vp) / vp;
      }
      if (fixedpctneeded < pctneeded) pctneeded = fixedpctneeded; // in case AMAP is more constraining


      //
      //
      //  recalculate dynamic hct for next pass through
      //
      float prevhct = hct; // before this cycle
      hct  = ( TBVec * initialCrit) / ( TBVec - currentvp - currentsubstate.Vp + previoussubstate.AccumVrep + currentsubstate.Vrep + currentvbolus + previoussubstate.AccumVac + currentsubstate.Vac);
      facb = (1.0f / ((ratio - 1.0f) * (1.0f - hct) + 1.0f));

      //
      //
      //    Terminate collection if we have hit maximum crit.
      //
      if (hct >= MAXPLASMAHCT)
      {
         pctneeded = 0.0f;    // ignore this cycle
         hct       = prevhct; // restore last cycle crit value
         setPlasmaLimitedByHct(true);
      }

      //
      //

      //    Add collected plasma to totals
      //
      if (pctneeded < 1.0f)
      {
         plasmaSatisfied = 1;

         if (pctneeded < 0.0f)
            pctneeded = 0.0f;
      }

      currentsubstate.T      += (plsubstate.Td + plsubstate.Tr) * pctneeded;
      currentsubstate.Vp     += vp * pctneeded;
      currentsubstate.Vacp   += vp * pctneeded * facb;
      currentsubstate.Vrep   += ((plsubstate.Qcinstd * plsubstate.Td) * pctneeded);
      currentsubstate.Vac    += (plsubstate.Qininstd * plsubstate.Td / ratio) * dcf * pctneeded;
      currentsubstate.EndVin += plsubstate.Qininstd * plsubstate.Td * pctneeded;

   }

   currentsubstate.Hend = hct;

   AccumulateVolumes();
   currentsubstate.HypovolemiaDistance = _procState._hypovolemia.VolumeRemainingToHypovolemia(0,
                                                                                              _procState.VAccumOverdraw(),
                                                                                              0.0f, 0.0f, // collect volumes
                                                                                              currentsubstate.AccumVp, currentsubstate.AccumVacp,
                                                                                              0.0f, 0.0f,
                                                                                              currentsubstate.AccumVrep + currentvbolus, currentsubstate.AccumVac);
}
//
//
// -----------------------------------------------------------------------------
//
int procedureDetailRBCPls::ReplacementFluidRequired () const
{
   int required = 0;
   if (!CatalogChecker::checkBit(_procState.GetCassetteFunctionBits(), CatalogChecker::NO_SALINE_BOLUS))   // this set has a replace line
   {
      if (_config.getFluidBalYN() && (getProcFixedCollectVol() == 0.0f))   // Do we need to check for replacement fluid
      {
         if (getProcCollectPlasma() && !(getProcPlasmaVolFixed()))   // AMAP or MC
         {
            required = 1;
         }
         else
         {
            if ((getProcFixedPlasmaVol() + getProcRBCVolume()) >= _config.getFluidBalActiveVol())
            {
               required = 1;
            }
         }
      }
   }
   return required;
}

#ifdef __WIN32__
void procedureDetailRBCPls::ResetProcDet ()
{
   memset(&substate, 0, sizeof(substate));

   _PredVp        = 0.0f;
   _PredVc        = 0.0f;
   _PredVac       = 0.0f;
   _PredVrep      = 0.0f;
   _PredTp        = 0.0f;
   _PredRBCCrit   = 0.0f;
   _PredRBCVolume = 0.0f;
   _PredRBCDose   = 0.0f;

}
#endif

/* FORMAT HASH ff2611b5da1d76cb97e06476683d781e */
