/*
 * Copyright (c) 1995 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      Offline_optimizer.hpp
 *             Focussed system prediction functions - wrapper for
 *             offline optimization.
 *
 * AUTHOR:     R.E. Edwards
 *
 * ABSTRACT:   This file encapsulates the optimization algorithm
 *
 * DOCUMENTS
 * Requirements:
 * Test:
 *
 * EXAMPLE:
 *
 */

#include "stdafx.h"
#include "ap2 predictionater.h"
extern CAP2predictionaterApp theApp;
#include "offline_optimizer.h"
#include "substatemap.h"
#include "units.h"
#include "states.h"

#include <cmath>
//-----------------------------------------------------------------------------
//                 Constructor, destructor
//
Offline_optimizer::Offline_optimizer()
{
}

Offline_optimizer::~Offline_optimizer()
{
}



//-----------------------------------------------------------------------------
//                         Initialization
//
void Offline_optimizer::initialize()
{
    m_predict.Init();
   m_predict.FillInConfig();
   m_predict.FillInProcedureConfig();
}

//-----------------------------------------------------------------------------
//                        Donor parameter setup
//
void Offline_optimizer::donor(const float height,
                              const float weight,
                              const float TBV,
                              const float hematocrit,
                              const float adjustedcrit,
                              const float precount,
                              const BLOODTYPE blood_type)
{
   ProcDonor_CDS donorcds(ROLE_RW);
   ProcRun_CDS runcds(ROLE_RW);

   donorcds.TBV.Set(TBV);
   donorcds.PreCnt.Set(precount);
   donorcds.Hct.Set(hematocrit);
   donorcds.Weight.Set(weight);
   donorcds.BloodType.Set(blood_type);
   donorcds.SampleVolume.Set(20.0f);

   runcds.AdjustedHct.Set(adjustedcrit);

   m_predict.FillInDonor();
   calculateBMI(height,weight);
}

void Offline_optimizer::calculateBMI(float height,float weight)
{
   //
   // check for height in inches, convert to meters
   //
   if ( cfg_main_input::userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_IN )
   {
      height /= 39.370079;
   }

   //
   // check for height in cm, convert to meters
   //
   if ( cfg_main_input::userconfig->LangUnit.Get().key_height == HEIGHT_UNITS_CM )
   {
      height /= 100.0;
   }

   //
   // check for weight in lbs, convert to kg
   //
   if ( cfg_main_input::userconfig->LangUnit.Get().key_weight == WEIGHT_UNITS_LB )
   {
      weight /= 2.2046226;
   }

   ProcDonor_CDS donorcds(ROLE_RW);
   donorcds.BMI.Set( weight / ( height*height ) );
}

//-----------------------------------------------------------------------------
//
void Offline_optimizer::setInitialConditions(const Initial_conditions &conditions)
{

   ProcVolumes_CDS volumescds(ROLE_RW);
   volumescds.Vin.Set(conditions.m_vin);
   volumescds.Vac.Set(conditions.m_vac);
   volumescds.PlasmaVolume.Set(conditions.m_plasma_bag_volume);
   volumescds.VacPlasmaBag.Set(conditions.m_plasma_bag_ac);
   volumescds.PlateletVolume.Set(conditions.m_collect_bag_volume);
   volumescds.VacPlateletBag.Set(conditions.m_collect_bag_ac);
   volumescds.RBCVolume.Set(conditions.m_rbc_bag_volume);
   volumescds.VacRBCBag.Set(conditions.m_rbc_bag_ac);
   volumescds.RBCDose.Set(conditions.m_rbc_bag_dose);
   volumescds.VReplacement.Set(conditions.m_vrep);
   volumescds.VSalineBolus.Set(conditions.m_vbolus);
   volumescds.PlateletYield.Set(conditions.m_yield);
   volumescds.TimeOfVincv.Set(conditions.m_tcv);
   volumescds.Vincv.Set(conditions.m_vincv);
   volumescds.VinEPCAStart.Set(conditions.m_epca_start);

   ProcTime_CDS timecds(ROLE_RW);
   timespec proctime;
	float f_runtime = conditions.m_procedure_run_time*60.0f;
	//crazy there is a roundoff error of 1.0 when converting
	//from float to long.
	int temp = (int) (f_runtime + 0.001f);
   proctime.tv_sec = temp;//static_cast<>(f_runtime);
   proctime.tv_nsec = 0;
   timecds.tmProcRun.Set(proctime);

   ProcTrimaSet_CDS setcds(ROLE_RW);
   setcds.Cassette.Set(cassette::cassetteType(conditions.m_cassette));
   setcds.RBCDetectorCalibrated.Set(1);

   // RKM
   //setcds.RBCPTFSet.Set(conditions.m_ptf);

   ProcRun_CDS runcds(ROLE_RW);
   //Sriram
   int substate = conditions.m_substate_int;
   if ( substate == 0) {
	   substate = SS_CHANNEL_SETUP;
   }
   runcds.Substate.Set(static_cast<State_names>(substate));//state_name);
   runcds.Qchmin.Set(conditions.m_minimum_qch);
   runcds.Qchcurrent.Set(conditions.m_current_qch);
   runcds.NumberWBCPurges.Set(conditions.m_purges);

   // RKM
   //runcds.PTFQrbc.Set(30.0f);
   //runcds.PTFQrbc.Set(conditions.m_ptf_qrbc);

   m_predict.UpdateProcData();

	// conditions.m_qac = 0.0f;
	// conditions.m_qcol = 0;
	// conditions.m_qin = 0.0f;
	// conditions.m_qp = 0.0f;
	// conditions.m_qrep = 0.0f;
	// conditions.m_qrp = 0.0f;
	// conditions.m_rpm = 0.0f;
	// conditions.m_vcol = 0.0f;
	// conditions.m_vp = 0.0f;
	// conditions.m_vrp = 0.0f;

   ProcAdjust_CDS adjustcds(ROLE_RW);
   adjustcds.Draw.CurrentCap.Set(conditions.m_qin_adjustment);
   adjustcds.Return.CurrentCap.Set(conditions.m_qrp_adjustment);
   adjustcds.Tingling.CurrentCap.Set(conditions.m_ir_adjustment);
   adjustcds.Clumping.CurrentCap.Set(conditions.m_ratio_adjustment);
   m_predict.doRepredictForAdjustments();

}


//-----------------------------------------------------------------------------
//                   Optimization variants
//
void Offline_optimizer::optimize()
{
   m_predict.SetUnoptimized();
   int productindex = 1;
   procedureDetail *pd = getProduct(productindex);
   while (pd) {
      m_predict.predictProcedure(*pd);
      pd = getProduct(++productindex);
   }
}

const INTERNAL_PREDICT_RESULTS Offline_optimizer::optimize(const int productindex)
{
   m_predict.SetUnoptimized();
   procedureDetail *pd = getProduct(productindex);
   if (pd) {
      m_predict.predictProcedure(*pd);
      return pd->getStatus();
   }
   return UNOPTIMIZED;
}

procedureDetail *Offline_optimizer::getProduct(int procindex)
{
   procedureDetail *detail = m_predict.getProcedure(procindex);
   return detail;
}

//-----------------------------------------------------------------------------

void Offline_optimizer::description(const INTERNAL_PREDICT_RESULTS result,
                                    CString &string)
{
   static const int MAXRESULT = 26;

   string.Empty();
   int result_as_int = (int)result;
   if ((result_as_int >= 0) && (result_as_int <= MAXRESULT)) {

      //
      //
      //  ----- WARNING ----- This code assumes that the string table contains
      //  enumerations in the order specified in the predict.h enumeration!
      //
      long id = IDS_OPTIMIZER_RESULTS_0;
      id += result_as_int;
      string.LoadString(id);
   } 
}

