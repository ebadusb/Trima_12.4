// Static Model

//
// Static Model

//
#include <vxworks.h>
#include "substatemap.h"
#include "pqipredictor.h"
#include "verifyflags.h"
#include "pqi_constants.h"
#include "software_cds.h"


PQIPredictor::PQIPredictor()
   : _procRunCDS        (ROLE_RO),
     _procTrimaSetCDS   (ROLE_RO),
     _procDonorCDS      (ROLE_RO),
     _procPQICDS        (ROLE_RO),
     _procVolumesCDS    (ROLE_RO),
     _procHalStatusCDS  (ROLE_RO)
{}

PQIPredictor::~PQIPredictor()
{}

// Returns non-zero (true) if there are NO reasons (stemming from the given Procedure Detail prediction)
// that might preclude labeling the resulting Platelet collection as containing less than 10E6 WBC contaminants.
// Returns 0 (false) if reasons were found that probably will contaminate with WBC in excess of 10E6.
bool PQIPredictor::LabelPlatelets (const procedureDetail& ProcedureDetail) const
{
   bool return_value = true; // Presume "innocent" until proven "guilty".

   // Predicted yield, collect volume, and resulting concentration
   const float desYield = ProcedureDetail.getDesiredYield();
   const float predVc   = ProcedureDetail.getPredVc();
   const float predConc = (predVc > 0.0f ? (desYield * 1.0E5f / predVc) : 0.0f );

   if (
      ( desYield > 0.0f && predVc > 0.0f ) ||    // Only if there is a platelet product
      ( _procVolumesCDS.PlateletVolume > 0.0f )  // or platelets are already collected
      )
   {
      const ProductQualityData& PQLocal = _procPQICDS.PQdata.Get();

      // Check current setting of WBC-in-Platelets flag per IT #5415
      if (PQLocal.verify_platelet_wbc_reasons.Get() > 0)
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Flag already set)" << endmsg;
         return_value = false;
      }

      // Check RBC detector, per #4 of Table CV-1 in TrimaCount-12.doc
      if ( !_procTrimaSetCDS.RBCDetectorCalibrated.Get() )
      {
         if (
            (_procTrimaSetCDS.CassetteState.Get() > ProcTrimaSet_CDS::DRY)  && ( _procHalStatusCDS.CassIsDown() )
            )
         {
            DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                             << ": COUNT WBCS (RBC detector uncalibrated)" << endmsg;
            return_value = false;
         }
      }

      // Check chamber flows, per #5 of Table CV-1 in TrimaCount-12.doc
      // The "No_Collect" substate doesn't need checked.
      // Per IT #5415, don't look at substates earlier than current, so start loop at current substate.
      {
         int j = SubstateMap::alg(_procRunCDS.Substate.Get(), 0);
         if ( j<0 ) j = 0;
         while ( j<ProcedureDetail.getNumOfSubstates() )
         {
            if ( j != Config::__EXTENDED_PCA
                 && j != Config::__NOCOLLECT_STATE    // a.k.a. "No_Collect"
                 && ProcedureDetail.getSubstate(j).T > 0.0f
                 && ProcedureDetail.getSubstate(j).Qch > VERY_SMALL_FLOW_RATE
                 && ProcedureDetail.getSubstate(j).Qch < CobeConfig::data().Qch1 * ProcedureDetail.getSubstate(j).Csf )
            {
               DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                                << ": COUNT WBCS (Qch=" << ProcedureDetail.getSubstate(j).Qch
                                                << " < " << CobeConfig::data().Qch1 * ProcedureDetail.getSubstate(j).Csf
                                                << " state# " << j << endmsg;
               return_value = false;
            }
            j++;
         }
      }

      // Check donor Hct, per #10 of Table CV-2 in TrimaCount-12.doc
      if ( (_procDonorCDS.Hct.Get() * 100.0f) > CobeConfig::data().HCThigh )
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Hct=" << (_procDonorCDS.Hct.Get() * 100.0f)
                                          << " > " << CobeConfig::data().HCThigh << ")" << endmsg;
         return_value = false;
      }

      // Check platelets to be purged, per #11 of Table CV-2 in TrimaCount-12.doc
      if ( ProcedureDetail.getYdpur() > CobeConfig::data().PurgeLossMaxFractionYtarg)
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Ydpur=" << ProcedureDetail.getYdpur()
                                          << " > " << CobeConfig::data().PurgeLossMaxFractionYtarg << ")" << endmsg;
         return_value = false;
      }

      // Check predicted platelet concentration, per #13 of Table CV-2 in TrimaCount-12.doc
      if ( predConc > CobeConfig::data().ConcMax )
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Kplatelet=" << predConc
                                          << " > " << CobeConfig::data().ConcMax << ")" << endmsg;
         return_value = false;
      }

      // Check for spillover having been detected and/or recovery requested, per IT #5522
      if ( _procPQICDS.rbcSpilloverDetection.Get() )
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Spillover previously detected by hardware)" << endmsg;
         return_value = false;
      }
      if ( _procPQICDS.rbcSpilloverRecovery.Get() )
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS (Spillover recovery previously requested by operator)" << endmsg;
         return_value = false;
      }

      // Check for Triple PPC flagged as Non-LR [IT-13096]
      if ( Software_CDS::GetInstance().getFeature(TriplePpcFlag) &&
           (desYield > CobeConfig::data().TppYieldThreshold) &&
           (predConc > CobeConfig::data().PpcConcentrationThreshold) )
      {
         DataLog(log_level_predict_debug) << "procedure " << ProcedureDetail.getProcedureNumber()
                                          << ": COUNT WBCS"
                                          << " (Yield=" << desYield << " > " << CobeConfig::data().TppYieldThreshold
                                          << "  Kplatelet=" << predConc << " > " << CobeConfig::data().PpcConcentrationThreshold
                                          << ")" << endmsg;
         return_value = false;
      }
   }

   return( return_value );

}

/* FORMAT HASH 8b52c35078e187c749122ea7f063403d */
