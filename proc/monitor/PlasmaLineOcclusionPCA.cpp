/*
 * Copyright (c) 2011 by CaridianBCT, Inc.  All rights reserved.
 *
 * This monitor is designed to detect a plasma line occlusion during the PCA substate.
 * A Red/Green trigger is established during PCA and this trigger is checked against the
 * active Red during PCA.  If the Red exceed the trigger, then a flag is generated.
 *
 * This monitor basically starts turned off and listens for the substate change message.
 * If we ever make it to PCA, we activate the monitor, establish the trigger criteria and wait
 * for the red reflectance value to hit the trigger value.
 *
 * The monitor is turned on/off as a feature via FEATURE_ID::PloPcaFlag
 *
 */


#include "PlasmaLineOcclusionPCA.h"
#include "cobeconfig.h"
#include "software_cds.h"

// Cobe Config constants
const float        PLOPCA_MIN_COLLECT_VOL       = CobeConfig::data().CVPdMin_PLO_PCA;
const float        PLOPCA_MAX_COLLECT_VOL       = CobeConfig::data().CVPdMax_PLO_PCA;
const float        RED_REFLECTANCE_DROP         = CobeConfig::data().RDropThresh_PLO_PCA;
const float        PLASMA_TARGET_VOLUME_MINIMUM = CobeConfig::data().PlasmaTargetMinimum_PLO_PCA;

static const char* NO_RECOVERY                  = RecoveryTypes::RecoveryStr(RecoveryTypes::None);



ProcData* PlasmaLineOcclusionPCA_Monitor::_pd = 0;

DEFINE_OBJ(PlasmaLineOcclusionPCA_Monitor);

PlasmaLineOcclusionPCA_Monitor::PlasmaLineOcclusionPCA_Monitor()
   : _pcaEndPltVolWindow (0.0f),
     _pcaStartPltVolWindow (0.0f),
     _redReflectanceTrigger (0.0f),
     _featureEnabled(Software_CDS::GetInstance().getFeature(PloPcaFlag))
{
   if ( !_pd )
      _pd = new ProcData;
}



PlasmaLineOcclusionPCA_Monitor::~PlasmaLineOcclusionPCA_Monitor()
{
   delete _pd;
   _pd = NULL;
}



//
//  This method is called once when the state machine gets initialized (before any set is loaded)
//
void PlasmaLineOcclusionPCA_Monitor::Initialize ()
{
   // If my feature flag is turned on, initialize the monitor
   if (_featureEnabled)
   {
      // Register for the state change message
      _substateChangeMsg.init(Callback<PlasmaLineOcclusionPCA_Monitor>(this, &PlasmaLineOcclusionPCA_Monitor::HandleSubstateChangeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      // Register for the PFR message
      _pfrStatusMsg.init(Callback<PlasmaLineOcclusionPCA_Monitor>(this, &PlasmaLineOcclusionPCA_Monitor::HandlePfrStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      // logged for development
      /*
      DataLog(log_level_proc_pqi_info) << "Rdropthresh = " << RED_REFLECTANCE_DROP << endmsg;
      DataLog(log_level_proc_pqi_info) << "CVpdmin = "     << PLOPCA_MIN_COLLECT_VOL << endmsg;
      DataLog(log_level_proc_pqi_info) << "CVpdmax = "     << PLOPCA_MAX_COLLECT_VOL << endmsg;
      */

      DataLog(log_level_proc_pqi_info) << "PLOPLS monitor initialized" << endmsg;
   }
   else
   {
      DataLog(log_level_proc_pqi_info) << "PLOPLS monitor disabled" << endmsg;
      Deactivate();
   }

   DataLog(log_level_proc_pqi_info).flags(ios::showpoint | ios::fixed);
   DataLog(log_level_proc_pqi_info).precision(2);
}



//
// Function called by the state machine once upon entering the Collect state
// so everything checked here had better already happened.
//
void PlasmaLineOcclusionPCA_Monitor::enable ()
{
   // Check for a change to my feature flag and re-initialize if necessary
   if (Software_CDS::GetInstance().getFeature(PloPcaFlag) != _featureEnabled)
   {
      _featureEnabled = !_featureEnabled;
      Initialize();
   }

   // Monitor doesn't do anything until we get to Midrun.
   // Wait for the state change to Midrun and then start monitoring
   MonitorBase::disable();
}



//
// Function called once upon exiting the state
//
void PlasmaLineOcclusionPCA_Monitor::disable ()
{
   // Only log if we are actually changing state
   if (_EnableMonitoring)
   {
      DataLog(log_level_proc_pqi_info) << "PLOPLS monitor disabled" << endmsg;
   }

   MonitorBase::disable();
}



//
//  Enable monitor when we have gone into Midrun and the other criteria have been satisfied.
//
void PlasmaLineOcclusionPCA_Monitor::HandleSubstateChangeMsg ()
{
   const State_names substate = (State_names)_substateChangeMsg.getData();

   switch (substate)
   {
      case SS_PCA :
      {
         DataLog(log_level_proc_pqi_info) << "PLOPCA monitor sees substate changed to " << SubstateMap::ssStr(substate) << endmsg;

         // This monitor is only valid if we are collecting platelets and
         // have a calibrated RBC detector.  White stamp cassette and Midrun state proves that.
         if (_pd->TrimaSet().RBCDetectorCalibrated.Get())
         {
            // Determine the End Vc Window and the Red Reflectance trigger values
            const float currentVc = _pd->Volumes().Vc.Get();
            _pcaEndPltVolWindow    = currentVc + PLOPCA_MAX_COLLECT_VOL;
            _pcaStartPltVolWindow  = currentVc + PLOPCA_MIN_COLLECT_VOL;
            _redReflectanceTrigger = _pd->Status().RedValue() - RED_REFLECTANCE_DROP;

            // Turn on the monitor.
            MonitorBase::enableMonitoring(1);

            DataLog(log_level_proc_pqi_info) << "PLOPCA monitor enabled at Vc " << _pcaStartPltVolWindow
                                             << ". Monitor window ends at Vc " << _pcaEndPltVolWindow
                                             << " mls.  Red reflectance trigger is " << _redReflectanceTrigger << " (Vc now " << currentVc << ")" << endmsg;
         }
         else
         {
            DataLog(log_level_proc_pqi_info) << "PLOPCA monitor deactivated due to incompatible cassette, RBC detector, or Platelet concentration issue" << endmsg;

            Deactivate();
         }
      }
      break;

      default :
         if (substate > SS_PCA)
         {
            DataLog(log_level_proc_pqi_info) << "PLOPCA monitor deactivated for rest of run.  Past PCA state." << endmsg;

            Deactivate();
         }
         break;
   }
}



//
// HandlePfrStatusMsg
// This callback is invoked when we get the PFR Status message.
// The monitor is turned off on a PFR.
//
void PlasmaLineOcclusionPCA_Monitor::HandlePfrStatusMsg ()
{
   const PFR_Status msg = (PFR_Status)_pfrStatusMsg.getData();

   DataLog(log_level_proc_pqi_info) << "PFR Status is " << msg << endmsg;

   if (msg == RestorePFRData)
   {
      DataLog(log_level_proc_pqi_info) << "PLOPCA monitor disabled.  PFR run." << endmsg;

      Deactivate();
   }
}



//
// Deactivate
//
void PlasmaLineOcclusionPCA_Monitor::Deactivate ()
{
   // Don't need to worry about any more state changes.
   _substateChangeMsg.deregisterMsg();

   disable();
}



//
//  This method is called once per Proc cycle.
//  Only enabled if we hit Midrun
//
void PlasmaLineOcclusionPCA_Monitor::Monitor ()
{
   // Don't do anything if we are turned off.
   if (!enableMonitoring())
   {
      return;
   }

   const State_names currentSubstate = _pd->Run().Substate.Get();

   switch (currentSubstate)
   {
      // Activate or deactivate the data collection timers
      case SS_PCA :
      {
         // End monitoring if we are collecting a hyper-concentrate
         const float conc = _pd->RunTargets().SelectedPlateletConcen.Get();
         if (Software_CDS::aboveStorageRange(conc, conc))
         {
            DataLog(log_level_proc_pqi_info) << "Target concentration exceeds limit.  Deactivating PLOPCA monitor." << endmsg;

            Deactivate();
         }

         // End monitoring if we have collected plasma volumes below or at the minimum.
         // Doing this check in the PCA state is a good place because the main purpose of this
         // exclusion is primarily designed to look at PlateletPlasma state
         const float plasmaVolume = _pd->Volumes().PlasmaVolume.Get();

         if (plasmaVolume <= PLASMA_TARGET_VOLUME_MINIMUM)
         {
            DataLog(log_level_proc_pqi_info) << "Plasma volume of " << plasmaVolume
                                             << " exceeds limit of " << PLASMA_TARGET_VOLUME_MINIMUM
                                             << ".  Deactivating PLOPCA monitor." << endmsg;

            Deactivate();
         }

         const float currentVc = _pd->Volumes().Vc.Get();

         // End monitoring if we went past the monitor window
         if (currentVc > _pcaEndPltVolWindow)
         {
            DataLog(log_level_proc_pqi_info) << "Vc of " << currentVc << " exceeds max PCA Vc of "
                                             <<  _pcaEndPltVolWindow << ".  Deactivating PLOPCA monitor." << endmsg;

            Deactivate();
         }

         static bool logit = true;
         if (logit && (currentVc > _pcaStartPltVolWindow))
         {
            logit = false;
            DataLog(log_level_proc_pqi_info) << "Vc = " << currentVc << endmsg;
         }


         const int redReflectance = _pd->Status().RedValue();

         if (  currentVc >= _pcaStartPltVolWindow
               && currentVc <= _pcaEndPltVolWindow)
         {
            // Don't check if we are paused or in a recovery
            if (
               !nowPaused()                         &&
               (_pd->RecoveryName() == NO_RECOVERY) &&
               ( (redReflectance <= _redReflectanceTrigger) )
               )
            {
               DataLog(log_level_proc_pqi_info) << "PLOPCA red criteria triggered at Vc " << currentVc << endmsg;

               DataLog(log_level_proc_pqi_info) << "Red Reflectance value was " << redReflectance
                                                << ".  Trigger value was " << _redReflectanceTrigger << endmsg;

               // Throw the flag
               _sout << "COUNT PLATELET PRODUCT because WBC contamination detected";
               LogIt(PLATELET_CONTAMINANT_DETECTION);
               _pqi.PutReason(PLATELET_CONTAMINANT_DETECTION);

               // Request the platelet contamination alert
               _pd->RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

               // Only need to set the flag once.
               Deactivate();
            }
         }
         break;
      }

      default :
      {
         // If we are activated and for some reason left PCA before we triggered, deactivate monitor
         if (currentSubstate > SS_PCA)
         {
            DataLog(log_level_proc_pqi_info) << "Deactivating monitor, Substate ended or max Vc exceeded." << endmsg;

            Deactivate();
         }

         break;
      }
   }
}

/* FORMAT HASH 7d3dcd1b652c12217cbdb747af192fe9 */
