#include "predictConfig.h"
#include "substatemap.h"
#include <cstdio>
#include "predict_types.h"

const float Config::__plasmaTolerance       = 0.4f;          // ml
const float Config::__MaxConcentrationError = 0.05f;         // allowable concentration error

const int   Config::__NOCOLLECT_STATE       = SubstateMap::alg(NOCOLLECT_STATE, 0);
const int   Config::__CHANNEL_SETUP         = SubstateMap::alg(SS_CHANNEL_SETUP, 0);
const int   Config::__PREPLATELET_PLASMA    = SubstateMap::alg(SS_PREPLATELET_PLASMA, 0);
const int   Config::__PREPLATELET_NOPLASMA  = SubstateMap::alg(SS_PREPLATELET_NOPLASMA, 0);
const int   Config::__PLATELET_PLASMA       = SubstateMap::alg(SS_PLATELET_PLASMA, 0);
const int   Config::__PCA                   = SubstateMap::alg(SS_PCA, 0);
const int   Config::__MIDRUN                = SubstateMap::alg(SS_MIDRUN, 0);
const int   Config::__PIR_PLASMA            = SubstateMap::alg(SS_PIR_PLASMA, 0);
const int   Config::__PIR_NOPLASMA          = SubstateMap::alg(SS_PIR_NOPLASMA, 0);
const int   Config::__EXTENDED_PCA          = SubstateMap::alg(SS_EXTENDED_PCA, 0);
const int   Config::__EXTENDED_PLASMA       = SubstateMap::alg(SS_EXTENDED_PLASMA, 0);
const int   Config::__RBC                   = SubstateMap::alg(SS_RBC, 0);
const int   Config::__RBC_SETUP_1           = SubstateMap::alg(SS_RBC_PTF_SETUP_1, 0);
const int   Config::__RBC_SETUP_2           = SubstateMap::alg(SS_RBC_PTF_SETUP_2, 0);
const int   Config::__RBC_PRIME             = SubstateMap::alg(SS_RBC_PTF_PRIME, 0);
const int   Config::__RP_CHANNEL_SETUP      = SubstateMap::alg(SS_RP_CHANNEL_SETUP, 1);
const int   Config::__RP_PLASMA_ONLY        = SubstateMap::alg(SS_RP_PLASMA_ONLY, 1);
const int   Config::__RP_RBC_SETUP_1        = SubstateMap::alg(SS_RP_RBC_PTF_SETUP_1, 1);
const int   Config::__RP_RBC_SETUP_2        = SubstateMap::alg(SS_RP_RBC_PTF_SETUP_2, 1);
const int   Config::__RP_RBC                = SubstateMap::alg(SS_RP_RBC, 1);

const int   Config::__RP_RBC_SETUP_1_DOUBLE = SubstateMap::alg(SS_RP_RBC_PTF_SETUP_1_DOUBLE, 1);
const int   Config::__RP_RBC_SETUP_2_DOUBLE = SubstateMap::alg(SS_RP_RBC_PTF_SETUP_2_DOUBLE, 1);
const int   Config::__RP_RBC_DOUBLE         = SubstateMap::alg(SS_RP_RBC_DOUBLE, 1);

Config::Config()
   : _pConfigCds(ROLE_RO),
     _cc(CobeConfig::data()),
     _ACRatio(0.0f),
     _ACRatioDuringRBC(0.0f),
     _ACRatioDuringPlasmaOnly(0.0f),
     _CqInlet(0.0f),
     _CqReturn(0.0f),
     _ACRate(0.0f),
     _ReferenceACRate(0.0f),
     _YSF(0.0f),
     _YSF_CV(0.0f),
     _FluidBalYN(0),
     _FluidBalActiveVol(0.0f),
     _FluidBal(0.0f),
     _TbvQinmaxScaleConst(0.0f),
     _TbvQnretmaxScaleConst(0.0f),
     _RSF(0.0f),
     _AMAPVolume(0),
     _AMAPMinVolume(0),
     _AMAPMaxVolume(0),
     _InletFlowRamp(0),
     _MaxInstQin(0.0f),
     _MaxNeedleRetFlow(0.0f),
     _PlasmaRinseback(0),
     _SalineRinseback(0),
     _CertifiedYield(0),
     _Ytarg_CL(0),
     _MinPostCount(0.0f),
     _MinPostHct(0.0f),
     _MaxProcedureTime(0),
     _FirstTimeQinCapDown(false)
{}

Config::~Config() {}

void Config::readConfigDat ()
{
   // Grab references to the CDS data, so we don't have to keep calling Get() and incurring a
   // semaphone transaction. (CDS Get() returns a local copy of the data, so we're not holding on to
   // references to anything that's going to change out from under us.
   const PredictionConfig& pred = _pConfigCds.Predict.Get();
   const ProcedureConfig&  proc = _pConfigCds.Procedure.Get();

   _CertifiedYield = pred.key_guaranteed_yield;
   _InletFlowRamp  = pred.key_inlet_flow_ramp;

   _Ytarg_CL       = pred.key_confidence_level;
   _YSF_CV         = pred.key_coefficient_of_variation;


   _MinPostHct   = proc.key_post_crit;
   _MinPostCount = proc.key_post_plat;

   // check for other rinseback types
   _PlasmaRinseback   = proc.key_plasma_rinseback;
   _SalineRinseback   = proc.key_saline_rinseback;

   _FluidBalYN        = pred.key_rbc_replace_fluid;

   _FluidBalActiveVol = proc.key_min_replacement_volume;

   _FluidBal          = pred.key_rbc_fluid_percent;


   _ACRatioDuringPlasmaOnly = proc.key_plasma_ratio;

   switch (pred.key_rbc_ratio)
   {
      case 0 :
         _ACRatioDuringRBC = 8.14f;
         break;
      case 1 :
         _ACRatioDuringRBC = 11.0f;
         break;
      case 2 :
         _ACRatioDuringRBC = 13.7f;
         break;
      default :
         _ACRatioDuringRBC = 0.00f;
         break;
   }


   switch (pred.key_inlet_management)
   {
      case 1 :
         _CqInlet = 0.66f;
         break;
      case 2 :
         _CqInlet = 0.72f;
         break;
      case 3 :
         _CqInlet = 0.78f;
         break;
      case 4 :
         _CqInlet = 0.85f;
         break;
      case 5 :
         _CqInlet = 0.92f;
         break;
      case 6 :
      default :
         _CqInlet = 1.00f;
         break;
   }

   switch (pred.key_return_management)
   {
      case 1 :
         _CqReturn = 0.50f;
         break;
      case 2 :
         _CqReturn = 0.57f;
         break;
      case 3 :
         _CqReturn = 0.66f;
         break;
      case 4 :
         _CqReturn = 0.76f;
         break;
      case 5 :
         _CqReturn = 0.87f;
         break;
      case 6 :
      default :
         _CqReturn = 1.00f;
         break;
   }

   // Reset these values to fit the Cq constants
   _TbvQinmaxScaleConst   = getCqInlet();
   _TbvQnretmaxScaleConst = getCqReturn();

   _MaxProcedureTime      = proc.key_proc_time;

   // From Procedure Screen
   _AMAPVolume    = pred.key_pls_amap_min;
   _AMAPMaxVolume = pred.key_pls_amap_max;
   _YSF           = pred.key_ysf;
   _RSF           = pred.key_rsf;

   // Get instantaneous limits as modified by adjustments
   _MaxInstQin       = 0.0f;
   _MaxNeedleRetFlow = 0.0f;
   _ACRate           = 0.0f;
   _ACRatio          = 0.0f;
}

//
//
// -----------------------------------------------------------------------------
//   Adjustment values encapsulated here to allow multiple usages including offline
//
float Config::getConfigMaxQin () const
{
   float limit;
   switch (_pConfigCds.Procedure.Get().key_max_draw_flow)
   {
      case 0 :
      default :
         limit = _cc.QinFlowLimitSlow;
         break;
      case 1 :
         limit = _cc.QinFlowLimitMedium;
         break;
      case 2 :
         limit = _cc.QinFlowLimitFast;
         break;
   }
   return limit;
}


bool Config::isAutoFlowOn () const
{
   return _pConfigCds.Procedure.Get().key_autoflow;
}

float Config::getConfigACRatio () const
{
   return _pConfigCds.Procedure.Get().key_custom_ratio;
}

int Config::isPltMeteringOn () const
{
   return _pConfigCds.Procedure.Get().key_mss_plt_on;
}

int Config::isRbcMeteringOn () const
{
   return _pConfigCds.Procedure.Get().key_mss_rbc_on;
}
int Config::isCompPlasmaOn () const
{
   return _pConfigCds.Predict.Get().key_complementary_plasma;
}

int Config::getNumProcedures () const
{
#ifdef __PREDICTROBUSTNESS__
   return 1;
#else
   return _pConfigCds.ProcedureList.numberOfProcedures.Get();
#endif
}


ProcedureDescription Config::getProcedureDescription (int procedureNum) const
{
   const ProductListArray& defs = _pConfigCds.ProcedureList.Procedure.Get();
   ProcedureDescription    def  = defs[procedureNum];
   return def;
}

int Config::isDrbcAlertOn () const
{
   return _pConfigCds.Procedure.Get().key_drbc_alert;
}

int Config::getDrbcThreshold () const
{
   return _pConfigCds.Procedure.Get().key_drbc_threshold;
}
int Config::getKeyACRate () const
{
   return _pConfigCds.Procedure.Get().key_ac_rate;
}

int Config::getPlasmaGenderOption () const
{
   return _pConfigCds.Predict.Get().key_male_only_plasma;
}

int Config::getPltGenderOption () const
{
   return _pConfigCds.Predict.Get().key_male_only_plt;
}

bool Config::isPromptedDrbcSplit (int procedureNum) const
{
   return _pConfigCds.isPromptedDrbcSplit(procedureNum);
}

int Config::getWeightUnits () const
{
   return _pConfigCds.LangUnit.Get().key_weight;
}

float Config::getFFPVolume () const
{
   return _pConfigCds.Predict.Get().key_ffp_volume;
}

float Config::getRbcDose (int procedureNum) const
{
   return _pConfigCds.ProcedureList.Procedure.Get()[procedureNum - 1].rbc_dose;
}

float Config::getDrbcTbvLimit () const
{
   return _pConfigCds.Procedure.Get().key_drbc_body_vol;
}

int Config::getTbvVolLimitSetting () const
{
   return _pConfigCds.Procedure.Get().key_tbv_vol_setting;
}

float Config::getConfigMaxACRate () const
{
   float limit = 0.0f;
   switch (_pConfigCds.Procedure.Get().key_ac_rate)
   {
      case 1 :
      default :
         limit = 1.0f;
         break;
      case 2 :
      case 3 :
         limit = 1.1f;
         break;
      case 4 :
      case 5 :
      case 6 :
         limit = 1.2f;
         break;
   }
   return limit;
}

//
//
// -----------------------------------------------------------------------------
//
int Config::AdjustConfig (float QinCap, float QrpCap, float IrCap, float RatioCap)
{
   int repredict = 0;
   float limit;
   //
   //
   //   Test instantaneous Qin limit and see if limit has changed
   //
   if (_FirstTimeQinCapDown == false && QinCap == _cc.QinLimitMax)
   {
      limit = MIN(getConfigMaxQin(), QinCap);
      if (_MaxInstQin != limit)
      {
         _MaxInstQin = limit;
         repredict   = 1;
      }
   }
   else if (QinCap <= _cc.QinLimitMax)
   {
      limit = QinCap;
      _FirstTimeQinCapDown = true;
      if (_MaxInstQin != limit)
      {
         _MaxInstQin = limit;
         repredict   = 1;
      }
   }
   //
   //
   //  Max instantaneous Qn flow
   //
   limit = MIN(_cc.QneedleLimitMax, QrpCap);
   if (_MaxNeedleRetFlow != limit )
   {
      _MaxNeedleRetFlow = limit;
      repredict         = 1;
   }

   //
   //
   //   Current instantaneous infusion rate
   //
   limit = getConfigMaxACRate();
   switch (getKeyACRate())
   {
      case 1 :
         _ReferenceACRate = 0.90f;
         break;
      case 2 :
         _ReferenceACRate = 0.95f;
         break;
      case 3 :
         _ReferenceACRate = 0.99f;
         break;
      case 4 :
         _ReferenceACRate = 1.01f;
         break;
      case 5 :
         _ReferenceACRate = 1.05f;
         break;
      case 6 :
         _ReferenceACRate = 1.10f;
         break;
      default :
         _ReferenceACRate = 0.00f;
         return -1; // Raise alarm.
   }

   if (limit > IrCap)
      limit = IrCap;

   if (_ACRate != limit)
   {
      _ACRate   = limit;
      repredict = 1;
   }

   //   Current AC ratio
   limit = MIN(getConfigACRatio(), RatioCap);
   if (_ACRatio != limit)
   {
      _ACRatio  = limit;
      repredict = 1;
   }

   return repredict;
}

bool Config::PirAlways () const
{
   // return Software_CDS::GetInstance().always_recover_platelet_inventory;
   //
   return true;  // always on for 6.3 sp0;  later spirals may be CC dependant.
}


//////////////////////////////////////////////////////////////////////
//
// Meyers singleton -- no pointers or dynamic memory.  Guaranteed to be
// constructed before first use.  This code is *not* thread safe.  It's
// reentrancy is somewhat debatable.  Neither issue is a problem as long as
// Config is only used in the Predict Task.  To make this fully reentrant, see
// the locking mechanism used in trima_common_reentrant/alarmcontainer.cpp.
//
Config& Config::Instance ()
{
   static Config instance;
   return instance;

}



float Config::AMAPPltConfigConc (AMAPPLTTYPES prodSize)
{
   return   (_pConfigCds.Templates.Get().key_plt_amap_conc[ prodSize - 1 ]); // there are six of these
}


float Config::AMAPPltMinYield (AMAPPLTTYPES prodSize)
{
   int index = ((prodSize - 1) % NUM_PLT_AMAP_PAS) + 1;
   return   (_pConfigCds.Templates.Get().key_plt_amap_min_yield[ index - 1 ]); // there are only three of these
}

float Config::AMAPPltMaxYield (AMAPPLTTYPES prodSize)
{
   int index = ((prodSize - 1) % NUM_PLT_AMAP_PAS) + 1;
   return   (_pConfigCds.Templates.Get().key_plt_amap_max_yield[ index - 1 ]); // there are only three of these
}

int Config::AMAPPltCarryover (AMAPPLTTYPES prodSize)
{
   if (prodSize >= AMAP_PLT_PAS_SINGLE)
   {
      int index = ((prodSize - 1) % NUM_PLT_AMAP_PAS) + 1;
      return (_pConfigCds.Templates.Get().key_plt_amap_pas_pct_carryover[ index - 1 ]); // there are only three of these
   }
   return 100; // all non-pas products at 100%
}


#ifdef __WIN32__
bool Config::InitializeCds (std::string path)
{
   return _pConfigCds.ReadConfig(path.c_str() );
}
#endif

/* FORMAT HASH 95dfb7119f737e9749a0feb398f13501 */
