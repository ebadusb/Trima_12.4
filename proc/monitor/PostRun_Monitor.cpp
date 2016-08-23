/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      PostRun_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the monitor template.
 *
 */

#include "PostRun_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "rbcdat.h"
#include "pqinformation.h"
#include "software_cds.h"


// Constants


// Define this class to the object dictionary
DEFINE_OBJ(PostRun_Monitor);

// Constructor
PostRun_Monitor::PostRun_Monitor()
   : aPQMonitor(),
     _plateletsWereCollected(false),
     _plasmaWasCollected(false),
     _rbcsWereCollected(false),
     _VplateletTarget(0.0),
     _VcolPlateletBag(0.0),
     _VcolPltBagWithSoln(0.0),
     _VplasmaTarget(0.0),
     _VcolPlasmaBag(0.0),
     _PLTfactor(0.0),
     _VrbcTarget(0.0),
     _VcolRBCBag(0.0)
{}



// Destructor
PostRun_Monitor::~PostRun_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void PostRun_Monitor::Monitor ()
{
   if (enableMonitoring())
   {
      // Get the Product Quality Data
      _pqData = _pd.PQI().PQdata.Get();

      //
      // Check product volumes, yields, etc.
      //

      if (_plateletsWereCollected)
         SetPlateletFlags();

      if (_plasmaWasCollected)
         SetPlasmaFlags();

      if (_rbcsWereCollected)
      {
         SetRbcFlags();
      }

      // If no platelet product was collected, then zero all flags and set the label
      // otherwise, finish flag checks.
      if (_VcolPlateletBag == 0.0f)
      {
         ClearPlateletFlags();
      }

      // For this test, use rounded volume instead of actual bag volume.
      // If actual is used, a fractional product ( < 1 ml ) will be labeled as leukoreducted
      // but the GUI displays a volume of 0 mls.
      // VcolPlasmaBag was the old var to test against
      const int runSumPlasmaVolume = (int)(_VcolPlasmaBag + 0.5f);
      if (runSumPlasmaVolume == 0)
      {
         ClearPlasmaFlags();
      }

      // RBC flags are complicated now.  There is not one simple check to clear them.
      // All clear logic is inside function.
      ClearRbcFlags();

      //
      // Set flags for any early-run-terminations.
      //
      SetRunEndedEarlyFlags();

      // Save the results
      _pd.MakePQIWritable();
      _pd.PQI().PQdata.Set(_pqData);
      _pd.MakePQIReadable();

      updateStats();

      disable();  // Do this only once.
   }
}



//
// Set platelet flags
//
void PostRun_Monitor::SetPlateletFlags ()
{

   const bool  isMeteredProd     = _pd.RunTargets().MeterPlatelets.Get();
   const float VstoragePltTarget = _pd.RunTargets().PltSolutionTarget.Get();
   const float VstoragePltBag    = _pd.Volumes().VstoragePltBag.Get();
   const float VpltSolutionDelta = fabs((VstoragePltTarget * _pd.Run().PltSSVolumeFactor) - VstoragePltBag);
   const float Kconfig           = _pd.RunTargets().SelectedPlateletConcen.Get();
   const float Yield             = _pd.Volumes().PlateletYield.Get();

   DataLog(_pqLog) << "SetPlateletFlags" << endmsg;

   // Metered Storage Solution flags
   if (isMeteredProd)
   {
      // this flag is for PAS that, due to switching proc our
      // final concentration preclude PAS (IT8424)
      if (_pd.Run().flagMSSPASConcetrationError.Get())
      {
         DataLog(_pqLog) << (int)FINAL_CONC_NOT_ACHIEVED
                         << " Change in products concentration precludes metering" << endmsg;

         LogIt2(FINAL_CONC_NOT_ACHIEVED, VERIFY_PLATELET_YIELD);

         _pqData.verify_platelet_yield_volume_reasons.Put(FINAL_CONC_NOT_ACHIEVED);
      }

      //
      // Didn't meter the correct amount of platelet storage solution
      //    OR we are in salvage case
      if ((VpltSolutionDelta > CobeConfig::data().Vplt1)
          || (_pd.Run().rinsebackVolumeSuccessMss.Get() == false))
      {
         DataLog(_pqLog) << (int)METERED_SOLUTION_VOLUME_ERROR
                         << " VERIFY METERED SOLUTION VOLUME because VstoragePltBag=" << VstoragePltBag
                         << " deviates >= " << CobeConfig::data().Vplt1
                         << " ml from VstoragePltTarget=" << VstoragePltTarget
                         << endmsg;

         LogIt2(METERED_SOLUTION_VOLUME_ERROR, VERIFY_PLATELET_YIELD);

         _pqData.verify_platelet_yield_volume_reasons.Put(METERED_SOLUTION_VOLUME_ERROR);
      }


      if (_pd.Run().flagMSSPASVolumeError.Get() == true)
      {
         DataLog(_pqLog) << (int)METERED_SOLUTION_PRIME_ERROR
                         << " VERIFY METERED SOLUTION VOLUME because MSS Platelet Prime alarmed for Prime delay or Prime too early"
                         << endmsg;

         LogIt2(METERED_SOLUTION_PRIME_ERROR, VERIFY_PLATELET_YIELD);

         _pqData.verify_platelet_yield_volume_reasons.Put(METERED_SOLUTION_PRIME_ERROR);
      }


      if (_pd.PQI().ClampClosureFailedFullFailure
          && (VstoragePltBag > 0.0f)
          )
      {
         DataLog(_pqLog) << (int)CHANNEL_CLAMP_ERROR
                         << " VERIFY PLATELET WBCS because MSS Clamp closure test failed" << endmsg;

         LogIt2(CHANNEL_CLAMP_ERROR, VERIFY_PLATELET_WBCS);

         _pqData.verify_platelet_wbc_reasons.Put(CHANNEL_CLAMP_ERROR);

         // Request the platelet contamination alert.
         _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
      }

      // if we did enter into rinseback but didn't clear enough volume.... WBC flag
      // and if we actually did any metering....
      if ((_pd.Run().rinsebackEnteredMss.Get() == true)
          && (_pd.Run().rinsebackVolumeSuccessMss.Get() == false)
          && (VstoragePltBag > 0.0f))
      {
         DataLog(_pqLog) << (int)RINSEBACK_NOT_COMPLETED
                         << " VERIFY PLATELET WBCS because MSS is running with rinseback not completed (salvage case)" << endmsg;

         LogIt2(RINSEBACK_NOT_COMPLETED, VERIFY_PLATELET_WBCS);

         _pqData.verify_platelet_wbc_reasons.Put(RINSEBACK_NOT_COMPLETED);

         // Request the platelet contamination alert.
         _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
      }


      // if we did NOT enter into rinseback but collected RBCs.... WBC flag (it7670)
      // and if we actually did any metering....
      if ((_pd.Run().rinsebackEnteredMss.Get() == false)
          && (_rbcsWereCollected == true)
          && (VstoragePltBag > 0.0f))
      {
         DataLog(_pqLog) << (int)RINSEBACK_NOT_COMPLETED
                         << " VERIFY PLATELET WBCS because MSS is running with rinseback not completed after collecting RBC's (salvage case)" << endmsg;

         LogIt2(RINSEBACK_NOT_COMPLETED, VERIFY_PLATELET_WBCS);

         _pqData.verify_platelet_wbc_reasons.Put(RINSEBACK_NOT_COMPLETED);

         // Request the platelet contamination alert.
         _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
      }

      const float timeLimit_image = Software_CDS::GetInstance().getMaxMeteringDuration();
      const float timeLimit       = (timeLimit_image) ? timeLimit_image : 0.0f;

      if (timeLimit != 0.0f)
      {

         const float mssDuration =  _pd.Run().MeteringDisconnectTime.Get() - _pd.Run().DonorDisconnectTime.Get();
         DataLog(_pqLog) << "From DonorDisco to end Metering Duration was " << mssDuration << ".  (Limit = " << timeLimit << ")"  << endmsg;

         if (mssDuration >  timeLimit)
         {

            DataLog(_pqLog) << (int)CHANNEL_CLAMP_ERROR << "VERIFY PLATELET WBCS because MSS was run too long after donor disconnect (" << mssDuration << " mins after)" << endmsg;
            LogIt2(CHANNEL_CLAMP_ERROR, VERIFY_PLATELET_WBCS);
            _pqData.verify_platelet_wbc_reasons.Put(CHANNEL_CLAMP_ERROR);

            // Request the platelet contamination alert.
            _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
         }
      }


   }  // end if this is a metered PLT product

   // Platelet Concentration Check (post-metering)
   // Only if configured concentration is within storage limits
   // The use of configured concentration (Kconfig) for both configured and collect implies standard (non-PPC)
   // collections as the collect range and storage range for PPCs do not overlap.
   //
   if (Software_CDS::withinStorageRange(Kconfig, Kconfig))
   {
      float concentrationLowerRange = Software_CDS::getLowConcentrationRange(Kconfig);
      float concentrationUpperRange = Software_CDS::getHighConcentrationRange(Kconfig);
      // Check accrued concentration for within storage limits.

      // Platelet Concentration below storage range
      if (Yield < 1.0E6f * concentrationLowerRange * _VcolPltBagWithSoln)
      {
         DataLog(_pqLog) << (int)PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE
                         << " COUNT PLATELET PRODUCT because PLATELET CONCENTRATION < " << concentrationLowerRange
                         << ".  Final bag volume was " << _VcolPltBagWithSoln << endmsg;

         LogIt(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE);

         _pqData.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE);

         // we should undo any PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE from endRunMonitor in this case since is opposite


         DataLog(_pqLog) <<  " Removing PLATELET CONCENTRATION flag after checking finished products; concentration <= "
                         << concentrationUpperRange
                         << ".  Final bag volume was "
                         << _VcolPltBagWithSoln
                         << endmsg;

         _pqi.RemoveReason(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
         _pqData.platelet_concentration_reasons.Remove(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
      }

      // Platelet Concentration above storage range
      if (Yield > 1.0E6f * concentrationUpperRange * _VcolPltBagWithSoln)
      {
         DataLog(_pqLog) << (int)PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE
                         << " COUNT PLATELET PRODUCT because PLATELET CONCENTRATION > " << concentrationUpperRange
                         << ".  Final bag volume was " << _VcolPltBagWithSoln << endmsg;

         LogIt(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);

         _pqData.platelet_concentration_reasons.Put(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
      }
      else
      {
         // we may need to disable the PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE set in
         // endRun_monitor if metering sends the conc back into range

         DataLog(_pqLog) << " Removing PLATELET CONCENTRATION above storage range flag (if it was applied) after checking finished products; concentration <= "
                         << concentrationUpperRange
                         << ".  Final bag volume was "
                         << _VcolPltBagWithSoln
                         << endmsg;

         _pqi.RemoveReason(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
         _pqData.platelet_concentration_reasons.Remove(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
      }
   }

   _pqData.label_platelets_leukoreduced = (_pqi.GetReasons(_pqData, VERIFY_PLATELET_WBCS) == 0);
}


//
//
// Clear platelet flags if no product collected
//
void PostRun_Monitor::ClearPlateletFlags ()
{
   //
   //   Remove flags for products that we didn't collect
   //
   DataLog(_pqLog) << "Clearing all flags for platelet products" << endmsg;

   // Never label a platelet product we didn't collect!
   _pqData.label_platelets_leukoreduced = false;

   // Never request verification of a platelet product we didn't collect!
   _pqData.verify_platelet_wbc_reasons.ClearAll();
   _pqData.verify_platelet_yield_volume_reasons.ClearAll();
   _pqData.platelet_concentration_reasons.ClearAll();
}



//
// Set plasma flags
//
void PostRun_Monitor::SetPlasmaFlags ()
{
   _pqData.label_plasma_leukoreduced = (_pqi.GetReasons(_pqData, VERIFY_PLASMA_WBCS) == 0);
}



//
// Clear plasma flags if no product collected
//
void PostRun_Monitor::ClearPlasmaFlags ()
{
   DataLog(_pqLog) << "Clearing all flags for plasma products" << endmsg;

   // Never label a plasma product we didn't collect!
   _pqData.label_plasma_leukoreduced = false;

   // Never request verification of a plasma product we didn't collect!
   _pqData.verify_plasma_wbc_reasons.ClearAll();
   _pqData.verify_plasma_volume_reasons.ClearAll();
}



//
// Set Rbc flags
//
void PostRun_Monitor::SetRbcFlags ()
{
   const bool isMeteredProd  = _pd.RunTargets().MeterRbcs.Get();
   const bool doubleTargeted = _pd.Config().isPromptedDrbcSplit(_pd.RunTargets().ProcNumber.Get());

   // If this is an MSS product
   if (isMeteredProd)
   {






      // If this was targeted to be a split product
      if (doubleTargeted)
      {
         const float filterVol              = CobeConfig::data().MssPtfVolume;
         const float VstorageRbcTarget      = (_pd.RunTargets().RBCSolutionTarget / 2);
         const float VstorageRbcBag1        = _pd.Volumes().RBCProduct_A_FinalVss.Get() - filterVol + CobeConfig::data().mssPtfFilterLoss;
         const float VstorageRbcBag2        = _pd.Volumes().RBCProduct_B_FinalVss.Get() - filterVol + CobeConfig::data().mssPtfFilterLoss;

         const float VrbcSolutionDeltaProd1 = fabs(VstorageRbcBag1 - VstorageRbcTarget);
         const float VrbcSolutionDeltaProd2 = fabs(VstorageRbcBag2 - VstorageRbcTarget);

         // Check the delta for RBC product 1
         if ( (VrbcSolutionDeltaProd1 > CobeConfig::data().Vrbc1) ||
              (_pd.Run().rinsebackVolumeSuccessMss.Get() == false) )
         {
            DataLog(_pqLog) << (int)METERED_SOLUTION_VOLUME_ERROR
                            << " VERIFY RBC PROD1 VOLUME vRasBag1=" << VstorageRbcBag1
                            << " delta=" << VrbcSolutionDeltaProd1
                            << " maxdelta= " << CobeConfig::data().Vrbc1
                            << " target=" << VstorageRbcTarget
                            << " success=" << _pd.Run().rinsebackVolumeSuccessMss.Get()
                            << endmsg;

            LogIt2(METERED_SOLUTION_VOLUME_ERROR, VERIFY_RBC_VOL);

            _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_VOLUME_ERROR);
         }

         // Check the delta for RBC product 2
         if ( (VrbcSolutionDeltaProd2 > CobeConfig::data().Vrbc1) ||
              (_pd.Run().rinsebackVolumeSuccessMss.Get() == false) )
         {
            DataLog(_pqLog) << (int)METERED_SOLUTION_VOLUME_ERROR
                            << " VERIFY RBC PROD2 VOLUME vRasBag2=" << VstorageRbcBag2
                            << " delta=" << VrbcSolutionDeltaProd2
                            << " maxdelta= " << CobeConfig::data().Vrbc1
                            << " target=" << VstorageRbcTarget
                            << " success=" << _pd.Run().rinsebackVolumeSuccessMss.Get()
                            << endmsg;

            LogIt2(METERED_SOLUTION_VOLUME_ERROR, VERIFY_RBC_VOL_PROD2);

            _pqData.verify_rbc_volume_reasons_prod2.Put(METERED_SOLUTION_VOLUME_ERROR);
         }



         if (_pd.Run().flagMSSRASVolumeError.Get() == true)
         {
            DataLog(_pqLog) << (int)METERED_SOLUTION_PRIME_ERROR
                            << " VERIFY METERED SOLUTION VOLUME because MSS RBC Prime alarmed for Prime delay"
                            << endmsg;

            LogIt2(METERED_SOLUTION_PRIME_ERROR, VERIFY_RBC_VOL);

            _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_PRIME_ERROR);

            LogIt2(METERED_SOLUTION_PRIME_ERROR, VERIFY_RBC_VOL_PROD2);

            _pqData.verify_rbc_volume_reasons_prod2.Put(METERED_SOLUTION_PRIME_ERROR);
         }




      }
      // Not a split product, but make sure some RBC product was collected
      else if ( _VcolRBCBag > 0.0f )
      {
         const float filterVol         = CobeConfig::data().MssPtfVolume;
         const float VstorageRbcTarget = _pd.RunTargets().RBCSolutionTarget.Get();
         const float VstorageRbcBag    = _pd.Volumes().VstorageRBCBag.Get() - filterVol + CobeConfig::data().mssPtfFilterLoss;
         const float VrbcSolutionDelta = fabs(VstorageRbcTarget - VstorageRbcBag);

         // Check if the delta is too big
         if ( (VrbcSolutionDelta > CobeConfig::data().Vrbc1) ||
              (_pd.Run().rinsebackVolumeSuccessMss.Get() == false) )
         {
            DataLog(_pqLog) << (int)METERED_SOLUTION_VOLUME_ERROR
                            << " VERIFY RBC VOLUME because VstorageRbcBag=" << VstorageRbcBag
                            << " deviates >= " << CobeConfig::data().Vrbc1
                            << "ml from VstorageRbcTarget=" << VstorageRbcTarget
                            << endmsg;

            LogIt2(METERED_SOLUTION_VOLUME_ERROR, VERIFY_RBC_VOL);

            _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_VOLUME_ERROR);
         }




         if (_pd.Run().flagMSSRASVolumeError.Get() == true)
         {
            DataLog(_pqLog) << (int)METERED_SOLUTION_PRIME_ERROR
                            << " VERIFY METERED SOLUTION VOLUME because MSS RBC Prime alarmed for Prime delay"
                            << endmsg;

            LogIt2(METERED_SOLUTION_PRIME_ERROR, VERIFY_RBC_VOL);

            _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_PRIME_ERROR);


         }


      }

      const int MAX_NUMBER_OF_APS_ALERTS = (int)CobeConfig::data().PTFAPSAlertsPerProductMetering;

      ////  If APS occurred on any metering RBC product label for volume,WBC, and quality
      if (_pd.Run().MssApsTooHighRBCProduct1.Get() > 0)
      {
         DataLog(_pqLog) << "APS alarm(s) on RBC product ONE will cause: " << endmsg;

         DataLog(_pqLog) << (int)APS_HIGH << " VERIFY RBC VOLUME because of APS alarm during RAS Delivery to product 1" << endmsg;
         LogIt2(APS_HIGH, VERIFY_RBC_VOL);
         _pqData.verify_rbc_volume_reasons.Put(APS_HIGH);

         // if it maxed out the alerts and failed on an alarm add these flags ( IT9002 )
         if (_pd.Run().MssApsTooHighRBCProduct1.Get() > MAX_NUMBER_OF_APS_ALERTS)
         {
            DataLog(_pqLog) << (int)APS_HIGH << " COUNT RBC WBC because of APS alarm during RAS Delivery to product 1" << endmsg;
            LogIt2(APS_HIGH, VERIFY_RBC_WBCS);
            _pqData.verify_rbc_wbc_reasons.Put(APS_HIGH);

            // removed under IT9188
            // DataLog(_pqLog) << (int)APS_HIGH << " VERIFY RBC QUALITY because of APS HIGH alarm during RAS Delivery to product 1" << endmsg;
            // LogIt2(APS_HIGH, VERIFY_RBC_QUALITY);
            // _pqData.verify_rbc_quality_reasons.Put(APS_HIGH);
         }
      }


      if (_pd.Run().MssApsTooHighRBCProduct2.Get() > 0)
      {
         DataLog(_pqLog) << "APS alarm(s) on RBC product TWO will cause: " << endmsg;

         DataLog(_pqLog) << (int)APS_HIGH << " VERIFY RBC VOLUME because of APS alarm during RAS Delivery to product 2" << endmsg;
         LogIt2(APS_HIGH, VERIFY_RBC_VOL_PROD2);
         _pqData.verify_rbc_volume_reasons_prod2.Put(APS_HIGH);

         // if it maxed out the alerts and failed on an alarm add these flags ( IT9002 )
         if (_pd.Run().MssApsTooHighRBCProduct2.Get() > MAX_NUMBER_OF_APS_ALERTS)
         {
            DataLog(_pqLog) << (int)APS_HIGH << " COUNT RBC WBC because of APS alarm during RAS Delivery to product 2" << endmsg;

            LogIt2(APS_HIGH, VERIFY_RBC_WBCS_PROD2);
            _pqData.verify_rbc_wbc_reasons_prod2.Put(APS_HIGH);

            // removed under IT9188
            // DataLog(_pqLog) << (int)APS_HIGH << " VERIFY RBC QUALITY because of APS HIGH alarm during RAS Delivery to product 2" << endmsg;
            // LogIt2(APS_HIGH, VERIFY_RBC_QUALITY_PROD2);
            // _pqData.verify_rbc_quality_reasons_prod2.Put(APS_HIGH);
         }
      }
   }

   //
   // RBC's are only leukoreduced for RAS kits.
   //
   if (isMeteredProd)
   {
      _pqData.label_rbcs_leukoreduced       = (_pqi.GetReasons(_pqData, VERIFY_RBC_WBCS) == 0);
      _pqData.label_rbcs_leukoreduced_prod2 = (_pqi.GetReasons(_pqData, VERIFY_RBC_WBCS_PROD2) == 0);
   }
   else
   {
      _pqData.label_rbcs_leukoreduced       = false;
      _pqData.label_rbcs_leukoreduced_prod2 = false;
   }
}



//
// Clear Rbc flags if no product collected
//
void PostRun_Monitor::ClearRbcFlags ()
{
   const bool isMeteredProd = _pd.RunTargets().MeterRbcs.Get();
   //
   //   Remove flags for products that we didn't collect
   //
   if (isMeteredProd)   // Potential for 2 different products
   {
      const float VrbcBag1of2 = _pd.Volumes().RBCProduct_A_FinalVolume.Get();
      const float VrbcBag2of2 = _pd.Volumes().RBCProduct_B_FinalVolume.Get();

      if (VrbcBag1of2 == 0.0f)
      {
         DataLog(_pqLog) << "Clearing all flags for dRBC product 1" << endmsg;
         // Never label a RBC product we didn't collect!
         _pqData.label_rbcs_leukoreduced = false;

         // Never request verification of a RBC product we didn't collect!
         _pqData.verify_rbc_volume_reasons.ClearAll();
         _pqData.verify_rbc_wbc_reasons.ClearAll();
         _pqData.verify_rbc_quality_reasons.ClearAll();
      }

      if (VrbcBag2of2 == 0.0f)
      {
         DataLog(_pqLog) << "Clearing all flags for dRBC product 2" << endmsg;
         // Never label a RBC product we didn't collect!
         _pqData.label_rbcs_leukoreduced_prod2 = false;

         // Never request verification of a RBC product we didn't collect!
         _pqData.verify_rbc_volume_reasons_prod2.ClearAll();
         _pqData.verify_rbc_wbc_reasons_prod2.ClearAll();
         _pqData.verify_rbc_quality_reasons_prod2.ClearAll();
      }
   }
   else if (_VcolRBCBag == 0.0f)   // Was not metered/split so it is treated as one big product.
   {
      DataLog(_pqLog) << "Clearing all flags for RBC product" << endmsg;
      // Never label a RBC product we didn't collect!
      _pqData.label_rbcs_leukoreduced = false;

      // Never request verification of a RBC product we didn't collect!
      _pqData.verify_rbc_volume_reasons.ClearAll();
      _pqData.verify_rbc_wbc_reasons.ClearAll();
      _pqData.verify_rbc_quality_reasons.ClearAll();

      // Never label a RBC product we didn't collect!
      _pqData.label_rbcs_leukoreduced_prod2 = false;

      // Never request verification of a RBC product we didn't collect!
      _pqData.verify_rbc_volume_reasons_prod2.ClearAll();
      _pqData.verify_rbc_wbc_reasons_prod2.ClearAll();
      _pqData.verify_rbc_quality_reasons_prod2.ClearAll();
   }
}



//
// Sets run ended early flags
//
void PostRun_Monitor::SetRunEndedEarlyFlags ()
{
   // RunEndedEarly is N/A if the Collect state was never entered
   if (!_pd.Run().EnteredCollect.Get())
   {
      DataLog(_pqLog) << "Collect state not entered; RunEndedEarly is N/A" << endmsg;
      return;
   }

   // Check for all zero is necessary for a PFR following a FATAL ERROR
   const bool allzero = ( (_VplateletTarget == 0.0f) && (_VcolPlateletBag == 0.0f) &&
                          (_VplasmaTarget   == 0.0f) && (_VcolPlasmaBag   == 0.0f) &&
                          (_VrbcTarget      == 0.0f) && (_VcolRBCBag      == 0.0f) );

   //
   // Set flags for any early-run-terminations:
   //

   // ================================================== Platelets
   //
   if (allzero || ( (_pd.TrimaSet().Cassette.Get() != cassette::RBCPLS)                         &&
                    (_VcolPlateletBag > 0.0f)                                                   &&
                    ( (_VplateletTarget - _VcolPlateletBag) > (_VplateletTarget * _PLTfactor) ) &&
                    !_pqData.verify_platelet_yield_volume_reasons.Get(RUN_ENDED_EARLY) ))
   {
      DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                      << " VERIFY PLATELET VOLUME because Run Ended Early with Vcollect=" << _VcolPlateletBag
                      << " which deviates >= " << _PLTfactor
                      << "% from VcolTarget=" << _VplateletTarget
                      << endmsg;

      LogIt(RUN_ENDED_EARLY);

      _pqData.verify_platelet_yield_volume_reasons.Put(RUN_ENDED_EARLY); // Yield & Volume are the same flag
   }

   // ================================================== Plasma
   //
   const float plasmaTarget = plasmaRinsebackEnabled() ? _pd.PlasmaRinseback().VPlasmaTarget : _VplasmaTarget;

   DataLog(_pqLog) << "Using Plasma Rinseback Configuration: " << plasmaRinsebackEnabled()
                   << " / " << plasmaTarget << " / " << _VplasmaTarget << "." << endmsg;

   if (allzero || ( ((plasmaTarget - _VcolPlasmaBag) > CobeConfig::data().Vplasma1) &&
                    (_VcolPlasmaBag > 0.0f)                                         &&
                    !_pqData.verify_plasma_volume_reasons.Get(RUN_ENDED_EARLY) ))
   {
      DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                      << " VERIFY PLASMA VOLUME because Run Ended Early with Vplasma=" << _VcolPlasmaBag
                      << " which deviates >= " << CobeConfig::data().Vplasma1
                      << " from VplasmaTarget=" << plasmaTarget
                      << endmsg;

      LogIt(RUN_ENDED_EARLY);

      _pqData.verify_plasma_volume_reasons.Put(RUN_ENDED_EARLY);
   }

   // ================================================== RBCs
   //
   const bool isRAS = _pd.RunTargets().MeterRbcs.Get();

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   const bool doubleTargeted = _pd.Config().isPromptedDrbcSplit(_pd.RunTargets().ProcNumber.Get());

   // check for deviation in volume for RBC single or double somewhere
   if (isRAS && doubleTargeted)
   {
      const float VrbcBag1of2 = _pd.Volumes().RBCProduct_A_FinalVolume.Get();
      const float VrbcBag2of2 = _pd.Volumes().RBCProduct_B_FinalVolume.Get();

      // bag one is short blame it:
      if (allzero || ( (((_VrbcTarget * 0.5f) - VrbcBag1of2) > CobeConfig::data().Vrbc1) &&
                       !_pqData.verify_rbc_volume_reasons.Get(RUN_ENDED_EARLY)           &&
                       (VrbcBag1of2 > 0.0f) ))
      {
         DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                         << " VERIFY RBC VOLUME of Product 1 because Run Ended Early with Vrbc="
                         << VrbcBag1of2
                         << " which deviates >= "
                         << CobeConfig::data().Vrbc1
                         << " from VrbcTarget="
                         << (_VrbcTarget * 0.5f)
                         << endmsg;

         LogIt(RUN_ENDED_EARLY);
         _pqData.verify_rbc_volume_reasons.Put(RUN_ENDED_EARLY);
      }

      // bag two is short blame it
      if (allzero || ( (((_VrbcTarget * 0.5f) - VrbcBag2of2) > CobeConfig::data().Vrbc1) &&
                       !_pqData.verify_rbc_volume_reasons_prod2.Get(RUN_ENDED_EARLY)     &&
                       (VrbcBag2of2 > 0.0f) ))
      {
         DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                         << " VERIFY RBC VOLUME of Product 2 because Run Ended Early with Vrbc="
                         << VrbcBag2of2
                         << " which deviates >= "
                         << CobeConfig::data().Vrbc1
                         << " from VrbcTarget="
                         << (_VrbcTarget * 0.5f)
                         << endmsg;

         LogIt(RUN_ENDED_EARLY);
         _pqData.verify_rbc_volume_reasons_prod2.Put(RUN_ENDED_EARLY);
      }

   }
   else // single RBC case
   {
      if (allzero || ( ((_VrbcTarget - _VcolRBCBag) > CobeConfig::data().Vrbc1) &&
                       !_pqData.verify_rbc_volume_reasons.Get(RUN_ENDED_EARLY)  &&
                       (_VcolRBCBag > 0.0f) ))
      {
         // single so blame only bag one:
         DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                         << " VERIFY RBC VOLUME of Product because Run Ended Early with Vrbc="
                         << _VcolRBCBag
                         << " which deviates >= "
                         << CobeConfig::data().Vrbc1
                         << " from VrbcTarget="
                         << _VrbcTarget
                         << endmsg;

         LogIt(RUN_ENDED_EARLY);
         _pqData.verify_rbc_volume_reasons.Put(RUN_ENDED_EARLY);
      }
   }

   // ================================================== MSS terminations
   //
   if (_pd.Run().MssUserAbortType.Get() != MSS_USER_ABORT_TYPE_NONE)
   {
      LogIt(METERED_SOLUTION_ENDED_BY_OPERATOR);

      switch (_pd.Run().MssUserAbortType.Get())
      {
         case MSS_USER_ABORT_TYPE_ALL :
         {
            // verify product is collected
            if ( (_VcolRBCBag > 0.0f) &&
                 (_pd.RunTargets().RBCSolutionTarget > CobeConfig::data().MssPtfVolume) )
            {
               DataLog(_pqLog) << (int)METERED_SOLUTION_ENDED_BY_OPERATOR
                               << " VERIFY RBC and PLT VOLUME because operator manually aborted" << endmsg;

               if (_pqData.verify_rbc_volume_reasons.isReason(METERED_SOLUTION_VOLUME_ERROR))
               {
                  LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_RBC_VOL);
                  _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);
               }

               if (_pqData.verify_rbc_volume_reasons_prod2.isReason(METERED_SOLUTION_VOLUME_ERROR))
               {
                  LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_RBC_VOL_PROD2);
                  _pqData.verify_rbc_volume_reasons_prod2.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);
               }
            }

            // verify product is collected
            if ( (_VcolPlateletBag > 0.0f) &&
                 (_pd.RunTargets().PltSolutionTarget.Get() > CobeConfig::data().MssMinPltVolume) )
            {
               LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_PLATELET_YIELD);
               _pqData.verify_platelet_yield_volume_reasons.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);

            }
         }
         break;

         case MSS_USER_ABORT_TYPE_PAS :
         {
            // verify product is collected
            if ( (_VcolPlateletBag > 0.0f) &&
                 (_pd.RunTargets().PltSolutionTarget.Get() > CobeConfig::data().MssMinPltVolume) )
            {
               DataLog(_pqLog) << (int)METERED_SOLUTION_ENDED_BY_OPERATOR
                               << " VERIFY PLT VOLUME because operator manually aborted" << endmsg;

               LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_PLATELET_YIELD);
               _pqData.verify_platelet_yield_volume_reasons.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);
            }
         }
         break;

         case MSS_USER_ABORT_TYPE_RAS :
         {
            // verify product is collected
            if ( (_VcolRBCBag > 0.0f) &&
                 (_pd.RunTargets().RBCSolutionTarget > CobeConfig::data().MssPtfVolume) )
            {
               DataLog(_pqLog) << (int)METERED_SOLUTION_ENDED_BY_OPERATOR
                               << " VERIFY RBC VOLUME because operator manually aborted" << endmsg;

               if (_pqData.verify_rbc_volume_reasons.isReason(METERED_SOLUTION_VOLUME_ERROR))
               {
                  LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_RBC_VOL);
                  _pqData.verify_rbc_volume_reasons.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);
               }

               if (_pqData.verify_rbc_volume_reasons_prod2.isReason(METERED_SOLUTION_VOLUME_ERROR))
               {
                  LogIt2(METERED_SOLUTION_ENDED_BY_OPERATOR, VERIFY_RBC_VOL_PROD2);
                  _pqData.verify_rbc_volume_reasons_prod2.Put(METERED_SOLUTION_ENDED_BY_OPERATOR);
               }
            }
         }
         break;

         default :
            DataLog(_pqLog) << " ERROR on determining MssUserAbortType" << endmsg;
            break;
      }
   }
}



void PostRun_Monitor::updateStats ()
{
   // Log the Product Quality summary
   PQInformation PQA;

   DataLog(log_level_proc_pqi_external)
      << "PRODUCT FLAGS:  PLT=( label " << ( PQA.LabelPlatelets() ? "Y" : "N" )
      << "; WBC "   << ( PQA.NumberOfReasons(VERIFY_PLATELET_WBCS) > 0 ? "Y" : "N" )
      << "; Y/vol " << ( PQA.NumberOfReasons(VERIFY_PLATELET_YIELD) > 0 ? "Y" : "N" )
      << "; conc "  << ( PQA.NumberOfReasons(CONC_OUT_OF_RANGE) > 0 ? "Y" : "N" )

      << " ) PLS=( label " << ( PQA.LabelPlasma() ? "Y" : "N" )
      << "; WBC " << ( PQA.NumberOfReasons(VERIFY_PLASMA_WBCS) > 0 ? "Y" : "N" )
      << "; vol " << ( PQA.NumberOfReasons(VERIFY_PLASMA_VOL) > 0 ? "Y" : "N" )

      << " ) RBC_1=( label " << ( PQA.LabelRbcs()      ? "Y" : "N" )
      << "; WBC "            << ( PQA.NumberOfReasons(VERIFY_RBC_WBCS) > 0 ? "Y" : "N" )
      << "; vol/dose/hct "   << ( PQA.NumberOfReasons(VERIFY_RBC_VOL) > 0 ? "Y" : "N" )
      << "; quality "        << ( PQA.NumberOfReasons(VERIFY_RBC_QUALITY) > 0 ? "Y" : "N" )

      << " ) RBC_2=( label " << ( PQA.LabelRbcsProd2() ? "Y" : "N" )
      << "; WBC "            << ( PQA.NumberOfReasons(VERIFY_RBC_WBCS_PROD2) > 0 ? "Y" : "N" )
      << "; vol/dose/hct "   << ( PQA.NumberOfReasons(VERIFY_RBC_VOL_PROD2) > 0 ? "Y" : "N" )
      << "; quality "        << ( PQA.NumberOfReasons(VERIFY_RBC_QUALITY_PROD2) > 0 ? "Y" : "N" )
      << " )"
      << endmsg;

   // Log what GUI displays
   PQA.LogReasons();
}



// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void PostRun_Monitor::Initialize ()
{}

// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void PostRun_Monitor::enable ()
{
   enableMonitoring(1);

   _plateletsWereCollected = _pd.PQI().PlateletsWereCollected.Get();
   _plasmaWasCollected     = _pd.PQI().PlasmaWasCollected.Get();
   _rbcsWereCollected      = _pd.PQI().RbcWereCollected.Get();

   _VplateletTarget        = _pd.RunTargets().SelectedPlateletVolume.Get();
   _VcolPlateletBag        = _pd.Volumes().PlateletVolume.Get();

   _VplasmaTarget          = _pd.RunTargets().SelectedPlasmaVolume.Get();
   _VcolPlasmaBag          = _pd.Volumes().PlasmaVolume.Get();

   _VrbcTarget             = _pd.RunTargets().SelectedRbcVolume.Get();
   _VcolRBCBag             = _pd.Volumes().RBCVolume.Get();

   _PLTfactor              = CobeConfig::data().Vplt1 / 100.0f;

   // Calculate bag volumes for metered products
   // These vars are used in calculations where metered volume is applicable to the flag.
   _VcolPltBagWithSoln = _VcolPlateletBag + (_pd.RunTargets().MeterPlatelets ? _pd.Volumes().VstoragePltBag : 0);
}



// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void PostRun_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH 0e1991099cc18779b880f2a2c1309802 */
