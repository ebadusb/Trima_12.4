/*
 * Copyright (c) 2003 by Gambro BCT, Inc.  All rights reserved.
 *
 * TITLE:      EndOfRun_Monitor.cpp
 *
 * ABSTRACT:  This is the source file for the End of Run "monitor".
 *
 */

#include "EndOfRun_Monitor.h"
#include "procdata.h"
#include "cobeconfig.h"
#include "rbcdat.h"
#include "software_cds.h"
#include "pqcontrol.h"

// Define this class to the object dictionary
DEFINE_OBJ(EndOfRun_Monitor);

// Constructor
EndOfRun_Monitor::EndOfRun_Monitor()
   : aPQMonitor(),
     _plateletsWereCollected(false),
     _plasmaWasCollected(false),
     _rbcsWereCollected(false),
     _VplateletTarget(0.0),
     _VcolPlateletBag(0.0),
     _VplasmaTarget(0.0),
     _VcolPlasmaBag(0.0),
     _PLTfactor(0.0),
     _VrbcTarget(0.0),
     _VcolRBCBag(0.0),
     _pd()
{}

// Destructor
EndOfRun_Monitor::~EndOfRun_Monitor()
{}

// Monitor function, called AFTER hardware status is received, the values
// are processed by the methods of the state to which the monitor is attached
// in the "states" file and all of its substates (if any),
// and no transition out of the state results.
void EndOfRun_Monitor::Monitor ()
{
   if (enableMonitoring())
   {
      // Process End of Run.
      // Check product volumes, yields, etc.
      if (_plateletsWereCollected)
         SetPlateletFlags();

      if (_plasmaWasCollected)
         SetPlasmaFlags();

      if (_rbcsWereCollected)
         SetRBCFlags();

      // Check PIR
      if (_pd.PQI().wasInPIR.Get() &&
          ( (_pd.PQI().VinTargetEndPIR.Get() - _pd.PQI().VinEndedPIR.Get() ) > 100.0f /*ml*/ ))
      {
         DataLog(_pqLog) << (int)RUN_ENDED_EARLY
                         << " VERIFY PLATELET YIELD because a short PIR occurred"
                         << endmsg;
         LogIt(RUN_ENDED_EARLY);
         _pqi.AddReasonToFlag(RUN_ENDED_EARLY, VERIFY_PLATELET_YIELD);
      }

      // LongDraw
      if (_pd.Run().DrawTooLongInRBCCol.Get() || _pd.Run().DrawTooLongInRBCColProd2.Get())
      {
         if (_plateletsWereCollected)
         {
            LogIt2(LONG_DRAW, VERIFY_PLATELET_WBCS);

            DataLog(_pqLog) << (int)LONG_DRAW << " COUNT PLATELET WBC because a long draw occurred"
                            << endmsg;

            _pqi.AddReasonToFlag(LONG_DRAW, VERIFY_PLATELET_WBCS);    // Per Terry Boucher's e-mail of Fri 12/14/2001 11:48 AM.

            // Request the platelet contamination alert.
            _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

            LogIt2(LONG_DRAW, VERIFY_PLATELET_YIELD);

            DataLog(_pqLog) << (int)LONG_DRAW << " VERIFY PLATELET YIELD/VOLUME because a long draw occurred"
                            << endmsg;

            _pqi.AddReasonToFlag(LONG_DRAW, VERIFY_PLATELET_YIELD);
         }

         if (_plasmaWasCollected)
         {
            LogIt2(LONG_DRAW, VERIFY_PLASMA_VOL);

            DataLog(_pqLog) << (int)LONG_DRAW
                            << " VERIFY PLASMA VOLUME because a long draw occurred"
                            << endmsg;
            _pqi.AddReasonToFlag(LONG_DRAW, VERIFY_PLASMA_VOL);      // Per Terry Boucher's e-mail of Fri 12/14/2001 11:48 AM
         }

         if (_rbcsWereCollected)
         {
            if (_pd.Run().DrawTooLongInRBCCol.Get())
            {
               LogIt2(LONG_DRAW, VERIFY_RBC_VOL);

               DataLog(_pqLog) << (int)LONG_DRAW
                               << " VERIFY RBC VOLUME because a long draw occurred (prod1)"
                               << endmsg;

               _pqi.AddReasonToFlag(LONG_DRAW, VERIFY_RBC_VOL);
            }

            if (_pd.Run().DrawTooLongInRBCColProd2.Get())
            {
               LogIt2(LONG_DRAW, VERIFY_RBC_VOL_PROD2);

               DataLog(_pqLog) << (int)LONG_DRAW
                               << " VERIFY RBC VOLUME because a long draw occurred (prod2)"
                               << endmsg;

               _pqi.AddReasonToFlag(LONG_DRAW, VERIFY_RBC_VOL_PROD2);
            }
         }
      }

      disable();  // Do this only once.
   }
}



void EndOfRun_Monitor::SetPlateletFlags ()
{
   const float ysf      = _pd.Config().Predict.Get().key_ysf;
   const float Ytarg    = _pd.RunTargets().SelectedPlateletYield.Get() * 1.0E11f;
   const float Kconfig  = _pd.RunTargets().SelectedPlateletConcen.Get();
   const float Yr       = _pd.Run().IntegratedPlateletYield.Get();
   const float Yield    = _pd.Volumes().PlateletYield.Get();
   const float Kcollect = 1.0E-6f * Yield / _VcolPlateletBag;

   // The CobeConfig data
   const CobeConfig::CobeConfigStruct& cobeConfig = CobeConfig::data();

   // Low Detected Platelet Concentration
   // Looks for a mismatch between the RBC detector signal and bag yield.
   // IT13478: only if RBC detector is calibrated
   if (_pd.TrimaSet().RBCDetectorCalibrated.Get())
   {
      float A = (Yield > 0.0f) ? (  (Yr / Yield ) * ysf ) : RbcDat::data().YieldCutoffConst;

      if (A < RbcDat::data().YieldCutoffConst)
      {
         A = 1.0E-6f * Yr / _VcolPlateletBag;

         DataLog(_pqLog) << (int)LOW_DETECTED_PLATELET_CONCENTRATION
                         << " VERIFY PLATELET YIELD because detected platelet concentration=" << A << " is too low"
                         << endmsg;

         _pqi.AddReasonToFlag (LOW_DETECTED_PLATELET_CONCENTRATION, VERIFY_PLATELET_YIELD); // Yield & Volume are the same flag

         if (Kconfig <= RbcDat::data().LowConcentrationShutoffConcentration /*2500*/) // Per 6/26/02 Design Review held by T. Boucher, but over objection of S. Butzke.
         {
            DataLog(_pqLog) << (int)LOW_DETECTED_PLATELET_CONCENTRATION
                            << " COUNT PLATELET WBC because detected platelet concentration=" << A << " is too low"
                            << endmsg;

            _pqi.AddReasonToFlag(LOW_DETECTED_PLATELET_CONCENTRATION, VERIFY_PLATELET_WBCS);

            // Request the platelet contamination alert.
            _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");
         }
      }
   }

   // HighPlateletYieldDeviation
   if (fabs(Ytarg - Yield) > 0.5E11f)
   {
      DataLog(_pqLog) << (int)HIGH_PLATELET_YIELD_DEVIATION
                      << " VERIFY PLATELET YIELD because predicted yield=" << Yield
                      << " deviates > .5E11 from target yield=" << Ytarg << endmsg;

      _pqi.PutReason (HIGH_PLATELET_YIELD_DEVIATION);
   }

   // SmallDraw
   if (_pd.Run().DrawTooShortInPltCol.Get())
   {
      if (!_pd.Run().BalanceCyclesDisableWBCPLTFlag.Get()) // IT 9161 if cycles are short by matching amount, no WBC or volume flagging
      {
         DataLog(_pqLog) << (int)SHORT_DRAW << " VERIFY PLATELET YIELD because a short draw occurred" << endmsg;
         _pqi.AddReasonToFlag (SHORT_DRAW, VERIFY_PLATELET_YIELD);

         DataLog(_pqLog) << (int)SHORT_DRAW << " COUNT PLATELET WBC because a short draw occurred" << endmsg;
         _pqi.AddReasonToFlag (SHORT_DRAW, VERIFY_PLATELET_WBCS);

         // Request the platelet contamination alert.
         _pd.RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

      }
   }

   // Platelet Volume Error
   // Looks for a mismatch between predicted target and actual collection
   if (fabs(_VplateletTarget - _VcolPlateletBag) > (_VplateletTarget * _PLTfactor))
   {
      DataLog(_pqLog) << (int)PLATELET_VOLUME_ERROR
                      << " VERIFY PLATELET VOLUME because Vcollect=" << _VcolPlateletBag << " deviates >= "
                      << _PLTfactor * 100.0f << "% from VcolTarget=" << _VplateletTarget << endmsg;

      _pqi.PutReason(PLATELET_VOLUME_ERROR);
   }

   // High Platelet Concentration check
   // Check for WBC contamination in product
   if (Kcollect > cobeConfig.ConcMax)
   {
      DataLog(_pqLog) << (int)HIGH_PLATELET_CONCENTRATION
                      << " COUNT PLATELET PRODUCT because Kplatelet > " << cobeConfig.ConcMax << endmsg;

      _pqi.PutReason(HIGH_PLATELET_CONCENTRATION);
   }

   // Platelet Concentration Check (pre-metering)
   // Only if configured concentration is within storage limits
   // The use of configured concentration (Kconfig) for both configured and collect implies standard (non-PPC)
   // collections as the collect range and storage range for PPCs do not overlap.
   //
   if (Software_CDS::withinStorageRange(Kconfig, Kconfig))
   {
      float concentrationLowerRange = Software_CDS::getLowConcentrationRange(Kconfig);
      float concentrationUpperRange = Software_CDS::getHighConcentrationRange(Kconfig);

      // Check accrued concentration for within storage limits.

      // Platelet Concentration too low
      if (Kcollect < concentrationLowerRange)
      {
         DataLog(_pqLog) << (int)PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE
                         << " COUNT PLATELET PRODUCT because PLATELET CONCENTRATION < " << concentrationLowerRange
                         << ".  Final bag volume was " << _VcolPlateletBag << endmsg;

         _pqi.PutReason(PLATELET_CONCENTRATION_BELOW_STORAGE_RANGE);
      }

      // Platelet Concentration too high
      if (Kcollect > concentrationUpperRange)
      {
         DataLog(_pqLog) << (int)PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE
                         << " COUNT PLATELET PRODUCT because PLATELET CONCENTRATION > " << concentrationUpperRange
                         << ".  Final bag volume was " << _VcolPlateletBag << endmsg;

         _pqi.PutReason(PLATELET_CONCENTRATION_ABOVE_STORAGE_RANGE);
      }
   }

   // Triple PPC flagged as non-LR (IT-13096):
   // - Enabled/disabled via Feature options
   // - Set VERIFY_PLATELET_WBC flag for hyperconcentrated Triple Platelet Product
   //   (yield > 9.0 and concentration > 2100)
   //
   if ( Software_CDS::GetInstance().getFeature(TriplePpcFlag) &&
        (Yield > cobeConfig.TppYieldThreshold * 1.0E11f) &&
        (Kcollect > cobeConfig.PpcConcentrationThreshold) )
   {
      DataLog(_pqLog) << (int)TRIPLE_PPC_AS_NON_LEUKOREDUCED
                      << " COUNT PLATELET PRODUCT because TPP concentration > " << cobeConfig.PpcConcentrationThreshold
                      << " (Yield=" << Yield
                      << " Vcollect=" << _VcolPlateletBag << ")" << endmsg;

      _pqi.AddReasonToFlag(TRIPLE_PPC_AS_NON_LEUKOREDUCED, VERIFY_PLATELET_WBCS);
   }

}



void EndOfRun_Monitor::SetPlasmaFlags ()
{
   const float plasmaTarget = plasmaRinsebackEnabled() ? _pd.PlasmaRinseback().VPlasmaTarget  : _VplasmaTarget;

   // SmallDraw
   if (_pd.Run().DrawTooShortInPlsCol.Get())
   {
      DataLog(_pqLog) << (int)SHORT_DRAW << " VERIFY PLASMA VOLUME because a short draw occurred" << endmsg;

      _pqi.AddReasonToFlag(SHORT_DRAW, VERIFY_PLASMA_VOL);
   }

   DataLog(_pqLog) << "Using Plasma Rinseback Configuration: " << plasmaRinsebackEnabled()
                   << " / " << plasmaTarget << " / " << _VplasmaTarget << "." << endmsg;

   // PlasmaVolumeError
   if (fabs (plasmaTarget - _VcolPlasmaBag) > CobeConfig::data().Vplasma1)
   {
      DataLog(_pqLog) << (int)PLASMA_VOLUME_ERROR
                      << " VERIFY PLASMA VOLUME because Vplasma=" << _VcolPlasmaBag
                      << " deviates >= " << CobeConfig::data().Vplasma1
                      << " from VplasmaTarget=" << plasmaTarget
                      << endmsg;

      _pqi.PutReason(PLASMA_VOLUME_ERROR);
   }
}



void EndOfRun_Monitor::SetRBCFlags ()
{

   ProcData    pd;
   const float VrbcBag1of2 = pd.Volumes().RBCProduct_A_FinalVolume.Get();
   const float VrbcBag2of2 = pd.Volumes().RBCProduct_B_FinalVolume.Get();
   bool        isRAS       = pd.RunTargets().MeterRbcs.Get();

   // IT 8306 - Always determine whether this is prompted split based on configured
   // rbc dose with no scaling or adjustments
   bool doubleTargeted = pd.Config().isPromptedDrbcSplit(pd.RunTargets().ProcNumber.Get());

   // we've detected a deviation in volume for RBC single or double somewhere
   if ( (fabs(_VrbcTarget - _VcolRBCBag) > CobeConfig::data().Vrbc1) &&
        (_VcolRBCBag > 0.0f) )
   {
      if (doubleTargeted && isRAS)
      {
         // bag one is short blame it:
         if ( (fabs((_VrbcTarget * 0.5f) - VrbcBag1of2) > CobeConfig::data().Vrbc1) &&
              (VrbcBag1of2 > 0.0f) )
         {
            DataLog(_pqLog) << (int)RBC_VOLUME_ERROR << " VERIFY RBC VOLUME because VrbcBag1of2="
                            << VrbcBag1of2 << " deviates >= " << CobeConfig::data().Vrbc1
                            << " from VrbcTarget1of2=" << (_VrbcTarget * 0.5f)
                            << endmsg;
            _pqi.PutReason(RBC_VOLUME_ERROR);
         }

         // bag two is short blame it
         if ( (fabs((_VrbcTarget * 0.5f) - VrbcBag2of2) > CobeConfig::data().Vrbc1) &&
              (VrbcBag2of2 > 0.0f) )
         {
            DataLog(_pqLog) << (int)RBC_VOLUME_ERROR << " VERIFY RBC VOLUME because VrbcBag2of2="
                            << VrbcBag2of2 << " deviates >= " << CobeConfig::data().Vrbc1
                            << " from VrbcTarget2of2=" << (_VrbcTarget * 0.5f)
                            << endmsg;
            _pqi.PutReasonRBCprod2(RBC_VOLUME_ERROR);
         }
      }
      else
      {
         // single so blame only bag one:
         DataLog(_pqLog) << (int)RBC_VOLUME_ERROR << " VERIFY RBC VOLUME because Vrbc="
                         << _VcolRBCBag << " deviates >= " << CobeConfig::data().Vrbc1
                         << " from VrbcTarget=" << _VrbcTarget
                         << endmsg;
         _pqi.PutReason(RBC_VOLUME_ERROR);
      }
   }

}



// Initialization, called by the state machine AFTER the Initialization()
// method of the state to which the monitor is attached in the "states" file.
void EndOfRun_Monitor::Initialize ()
{
   // Do nothing
}



// Function to enable monitoring, called by the state machine AFTER the preEnter()
// method of the state to which the monitor is attached in the "states" file.
void EndOfRun_Monitor::enable ()
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
}



// Function to disable monitoring, called by the state machine BEFORE the postExit()
// method of the state to which the monitor is attached in the "states" file.
void EndOfRun_Monitor::disable ()
{
   enableMonitoring(0);
}

/* FORMAT HASH e664f8457b4f905b8d91c1fe29db5ee1 */
