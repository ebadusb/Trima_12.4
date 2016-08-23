/*
 * Copyright (c) 2010 by CaridianBCT, Inc.  All rights reserved.
 *
 * $Header: //depot/main/embedded/Trima/Main/proc/monitor/PlasmaLineOcclusion.cpp#1 $
 *
 * This monitor is designed to detect a plasma line occlusion during the EPCA substate.
 * A Red/Green baseline is established during midrun and this baseline is checked against the
 * active Red/Green during EPCA.  If the Red/Green exceed the baseline, then a flag is generated.
 *
 * This monitor basically starts turned off and listens for the substate change message.  If we ever make
 * it to Midrun, we activate the monitor and wait for the chance to collect data.  Once collected, we
 * deactivate again and wait for EPCA to process the signal data.
 *
 * The monitor is turned on/off as a feature via FEATURE_ID::PloFlag
 *
 *
 */


#include "PlasmaLineOcclusion.h"
#include "cobeconfig.h"
#include "software_cds.h"


// Cobe Config constants
const float        MIDRUN_BASELINE_START_VOL   = CobeConfig::data().PLOebsmin;
const unsigned int MIDRUN_BASELINE_MAX_SAMPLES = CobeConfig::data().PLOebsmax;
const unsigned int MIDRUN_BASELINE_MIN_SAMPLES = CobeConfig::data().PLOesammin;
const unsigned int RED_REFLECTANCE_TRIGGER     = CobeConfig::data().Rdropthresh;
const unsigned int GREEN_REFLECTANCE_TRIGGER   = CobeConfig::data().Gdropthresh;
const float        EPCA_MIN_COLLECT_VOLUME     = CobeConfig::data().CVedmin;
const float        EPCA_MAX_COLLECT_VOLUME     = CobeConfig::data().CVedmax;

static const char* NO_RECOVERY                 = RecoveryTypes::RecoveryStr(RecoveryTypes::None);



ProcData* PlasmaLineOcclusion_Monitor::_pd = 0;

DEFINE_OBJ(PlasmaLineOcclusion_Monitor);

PlasmaLineOcclusion_Monitor::PlasmaLineOcclusion_Monitor()
   : _numberOfSamples(0),
     _redReflectanceRaw(0),
     _redReflectanceAverage(-1.0),
     _redReflectanceTrigger(0),
     _greenReflectanceRaw(0),
     _greenReflectanceAverage(-1.0),
     _greenReflectanceTrigger(0),
     _epcaCollectVolume(0),
     _dataCollectorTimer(),
     _midrunStartPltVol(0),
     _redCriteriaTriggered(false),
     _greenCriteriaTriggered(false),
     _flagTriggered(false),
     _baselineEstablished(false),
     _featureEnabled(Software_CDS::GetInstance().getFeature(PloFlag))
{
   if ( !_pd )
      _pd = new ProcData;
}



PlasmaLineOcclusion_Monitor::~PlasmaLineOcclusion_Monitor()
{
   delete _pd;
   _pd = NULL;
}



//
//  This method is called once when the state machine gets initialized (before any set is loaded)
//
void PlasmaLineOcclusion_Monitor::Initialize ()
{
   // If flag is turned on, initialize the monitor
   if (_featureEnabled)
   {
      // Initialize timers
      _dataCollectorTimer.notifier(Callback<PlasmaLineOcclusion_Monitor>(this, &PlasmaLineOcclusion_Monitor::Process6SecondData) );
      _dataCollectorTimer.interval(6000);      // 6 second timer.
      _dataCollectorTimer.inactivate();

      // Register for the state change message
      _substateChangeMsg.init(Callback<PlasmaLineOcclusion_Monitor>(this, &PlasmaLineOcclusion_Monitor::HandleSubstateChangeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      // Register for the PFR message
      _pfrStatusMsg.init(Callback<PlasmaLineOcclusion_Monitor>(this, &PlasmaLineOcclusion_Monitor::HandlePfrStatusMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      DataLog(log_level_proc_pqi_info) << "PLOebsmin="   << MIDRUN_BASELINE_START_VOL << endmsg;
      DataLog(log_level_proc_pqi_info) << "PLOebsmax="   << MIDRUN_BASELINE_MAX_SAMPLES << endmsg;
      DataLog(log_level_proc_pqi_info) << "PLOesamin="   << MIDRUN_BASELINE_MIN_SAMPLES << endmsg;
      DataLog(log_level_proc_pqi_info) << "Rdropthresh=" << RED_REFLECTANCE_TRIGGER << endmsg;
      DataLog(log_level_proc_pqi_info) << "Gdropthresh=" << GREEN_REFLECTANCE_TRIGGER << endmsg;
      DataLog(log_level_proc_pqi_info) << "CVedmin="     << EPCA_MIN_COLLECT_VOLUME << endmsg;
      DataLog(log_level_proc_pqi_info) << "CVedmax="     << EPCA_MAX_COLLECT_VOLUME << endmsg;

      DataLog(log_level_proc_pqi_info) << "PLO monitor initialized" << endmsg;
   }
   else
   {
      DataLog(log_level_proc_pqi_info) << "PLO monitor disabled" << endmsg;
      MonitorBase::disable();
   }

   DataLog(log_level_proc_pqi_info).flags(ios::showpoint | ios::fixed);
   DataLog(log_level_proc_pqi_info).precision(2);
}



//
// Function called by the state machine once upon entering the Collect state
// so everything checked here had better already happened.
//
void PlasmaLineOcclusion_Monitor::enable ()
{
   // Check for a change to my feature flag and re-initialize if necessary
   if (Software_CDS::GetInstance().getFeature(PloFlag) != _featureEnabled)
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
void PlasmaLineOcclusion_Monitor::disable ()
{
   // Only log if we are actually changing state
   if (_EnableMonitoring)
   {
      DataLog(log_level_proc_pqi_info) << "PLO monitor disabled" << endmsg;
   }

   _dataCollectorTimer.inactivate();  // Turn off just in case we get here from some odd transition
   MonitorBase::disable();
}



//
// Calculate Baseline
//
void PlasmaLineOcclusion_Monitor::CalculateBaseline (float currentVc)
{
   _baselineEstablished = true;
   _dataCollectorTimer.inactivate();  // Turn off data collection

   DataLog(log_level_proc_pqi_info) << "PLO data collection stopped at Vc " << currentVc << endmsg;

   _redReflectanceAverage   = _redReflectanceRaw / _numberOfSamples;
   _greenReflectanceAverage = _greenReflectanceRaw / _numberOfSamples;

   DataLog(log_level_proc_pqi_info) << "Average Red / Green reflectance is " << _redReflectanceAverage
                                    << " / " << _greenReflectanceAverage << " on " << _numberOfSamples << " samples " << endmsg;

   _redReflectanceTrigger   = _redReflectanceAverage - RED_REFLECTANCE_TRIGGER;
   _greenReflectanceTrigger = _greenReflectanceAverage - GREEN_REFLECTANCE_TRIGGER;

   DataLog(log_level_proc_pqi_info) << "PLO Red / Green triggers are set to "
                                    << _redReflectanceTrigger << " / " << _greenReflectanceTrigger << endmsg;

   DataLog(log_level_proc_pqi_info) << "PLO waiting for EPCA." << endmsg;
}



//
//  This method is called once per Proc cycle.
//  Only enabled if we hit Midrun
//
void PlasmaLineOcclusion_Monitor::Monitor ()
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
      case SS_MIDRUN :
      {
         const float currentVc  = _pd->Volumes().Vc.Get();
         const float triggerVol = MIDRUN_BASELINE_START_VOL * _pd->Predict().Qc(true, SS_MIDRUN);

         // If we are outside of the collection window, stop collecting data
         if ( _dataCollectorTimer.activated() &&
              (_numberOfSamples >= MIDRUN_BASELINE_MAX_SAMPLES)
              )
         {
            CalculateBaseline(currentVc);

            // Done collecting during Midrun.  Wait until EPCA to reactivate.
            disable();
         }
         // else if we are in the window for collecting the red green data, then
         //    activate the collector to establish the baseline red and green averages
         else if ( !_dataCollectorTimer.activated()                  &&
                   (currentVc - _midrunStartPltVol >  triggerVol)    &&
                   (_numberOfSamples <= MIDRUN_BASELINE_MIN_SAMPLES)      // prevent re-arming
                   )
         {
            DataLog(log_level_proc_pqi_info) << "PLO data collection started at Vc " << currentVc << endmsg;
            _dataCollectorTimer.activate();
         }

         break;
      }

      // Check the current Red / Green against the trigger criteria
      case SS_EXTENDED_PCA :
      {
         float conc = _pd->RunTargets().SelectedPlateletConcen.Get();
         if (Software_CDS::aboveStorageRange(conc, conc))
         {
            DataLog(log_level_proc_pqi_info) << "Target concentration exceeds limit.  Deactivating PLO monitor." << endmsg;

            Deactivate();
         }

         const float pltBagVolumeDelta = _pd->Volumes().Vc.Get() - _epcaCollectVolume;

         if (pltBagVolumeDelta > EPCA_MAX_COLLECT_VOLUME)
         {
            DataLog(log_level_proc_pqi_info) << "EPCA collect volume exceeds max threshold. Ending EPCA PLO ending monitor" << endmsg;

            Deactivate();
         }
         // Don't check if we are paused or in a recovery or don't have enough samples
         else if ( !_flagTriggered                                  &&
                   !nowPaused()                                     &&
                   (_numberOfSamples > MIDRUN_BASELINE_MIN_SAMPLES) &&
                   (_pd->RecoveryName() == NO_RECOVERY)             &&
                   (pltBagVolumeDelta >= EPCA_MIN_COLLECT_VOLUME)   &&
                   (pltBagVolumeDelta <= EPCA_MAX_COLLECT_VOLUME)
                   )
         {
            // Check the current RBC detector values for trigger criteria
            const int redReflectance   = _pd->Status().RedValue();
            const int greenReflectance = _pd->Status().GreenValue();

            // Red criteria
            if ( !_redCriteriaTriggered && (redReflectance <= _redReflectanceTrigger) )
            {
               DataLog(log_level_proc_pqi_info) << "PLO red criteria triggered" << endmsg;

               DataLog(log_level_proc_pqi_info) << "Red Reflectance value was " << redReflectance
                                                << ".  Baseline was " << _redReflectanceTrigger
                                                << ".  Trigger volume was " << pltBagVolumeDelta << endmsg;

               _redCriteriaTriggered = true;
            }

            // Green criteria
            if ( !_greenCriteriaTriggered && (greenReflectance <= _greenReflectanceTrigger) )
            {
               DataLog(log_level_proc_pqi_info) << "PLO green criteria triggered" << endmsg;

               DataLog(log_level_proc_pqi_info) << "Green Reflectance value was " << greenReflectance
                                                << ".  Baseline was " << _greenReflectanceTrigger
                                                << ".  Trigger volume was " << pltBagVolumeDelta << endmsg;

               _greenCriteriaTriggered = true;
            }

            if (_redCriteriaTriggered && _greenCriteriaTriggered)
            {
               DataLog(log_level_proc_pqi_info) << "PLO criteria triggered" << endmsg;

               // Throw the flag
               _sout << "COUNT PLATELET PRODUCT because WBC contamination detected";
               LogIt(PLATELET_CONTAMINANT_DETECTION);
               _pqi.PutReason(PLATELET_CONTAMINANT_DETECTION);

               // Request the platelet contamination alert
               _pd->RequestWbcAlarm(__FILE__, __LINE__, "WBC event alarm is needed");

               // Can't disable the flag because I may have to take it away.
               _flagTriggered = true;
            }
         }

         break;
      }

      default :
         // If we are activated and for some reason left Midrun before min samples are reached, deactivate monitor
         if (_dataCollectorTimer.activated())
         {
            DataLog(log_level_proc_pqi_info) << "Deactivating monitor, min number of samples not met.  Samples = " << _numberOfSamples << endmsg;

            Deactivate();

            // Set the average and triggers to the max value so they won't get triggered.
            _redReflectanceAverage   = 4095;
            _greenReflectanceAverage = 4095;

            _redReflectanceAverage   = 4095;
            _greenReflectanceAverage = 4095;
         }
         break;
   }
}



//
// Activated by the _dataCollectorTimer, this function grabs the red / green data every
// 6 seconds and calculates the averages
//
void PlasmaLineOcclusion_Monitor::Process6SecondData ()
{
   static bool collecting = false;  // flag used for logging

   // Reset the timer to get notified in 6 seconds again...
   _dataCollectorTimer.init();

   // Don't collect data if we are paused or in a recovery
   if (nowPaused() || (_pd->RecoveryName() != NO_RECOVERY) )
   {
      if (collecting)
      {
         DataLog(log_level_proc_pqi_info) << "PLO stopped collecting data during a paused state.  Number of samples are " << _numberOfSamples << endmsg;
         collecting = false;
      }
   }
   else
   {
      // Collect the raw data
      _redReflectanceRaw   += _pd->Status().RedValue();
      _greenReflectanceRaw += _pd->Status().GreenValue();

      _numberOfSamples     += 1;

      if (!collecting)
      {
         DataLog(log_level_proc_pqi_info) << "PLO started collecting data.  Number of samples are " << _numberOfSamples << endmsg;
         collecting = true;
      }
   }
}



//
//  Enable monitor when we have gone into Midrun and the other criteria have been satisfied.
//
void PlasmaLineOcclusion_Monitor::HandleSubstateChangeMsg ()
{
   const State_names substate = (State_names)_substateChangeMsg.getData();

   switch (substate)
   {
      case SS_MIDRUN :
      {
         DataLog(log_level_proc_pqi_info) << "PLO monitor sees substate changed to " << SubstateMap::ssStr(substate) << endmsg;

         // This monitor is only valid if we are collecting platelets and
         // have a calibrated RBC detector.  White stamp cassette and Midrun state proves that.
         if (_pd->TrimaSet().RBCDetectorCalibrated.Get())
         {
            // Capture the starting Platelet bag volume for later comparison.
            _midrunStartPltVol = _pd->Volumes().Vc.Get();

            // Turn on the monitor.
            MonitorBase::enableMonitoring(1);

            const float Qc = _pd->Predict().Qc(true, SS_MIDRUN);

            DataLog(log_level_proc_pqi_info) << "PLO monitor enabled at Vc " << _midrunStartPltVol
                                             << ". Qc is " << Qc << ".  Baseline establishment target is "
                                             << _midrunStartPltVol + (MIDRUN_BASELINE_START_VOL * Qc)
                                             << " mls." << endmsg;
         }
         else
         {
            DataLog(log_level_proc_pqi_info) << "PLO monitor deactivated due to incompatible cassette, RBC detector, or Platelet concentration issue" << endmsg;

            Deactivate();
         }
      }

      break;

      case SS_EXTENDED_PCA :
      {
         const float currentVc = _pd->Volumes().Vc.Get();

         DataLog(log_level_proc_pqi_info) << "PLO monitor sees substate changed to " << SubstateMap::ssStr(substate)
                                          << " at Vc " << currentVc << endmsg;

         // if we collected enough samples, reactivate monitor
         if (_numberOfSamples >= MIDRUN_BASELINE_MIN_SAMPLES)
         {
            // We can entry EPCA after a Midrun shorter than the max but greater
            // than the min.  If baseline is not established, then do it now.
            if (!_baselineEstablished)
               CalculateBaseline(currentVc);

            _epcaCollectVolume = currentVc;

            // Turn on the monitor
            MonitorBase::enableMonitoring(1);
            DataLog(log_level_proc_pqi_info) << "PLO monitor enabled for EPCA." << endmsg;
         }

         // If we got here and the timer is stil active, turn it off
         if (_dataCollectorTimer.activated())
         {
            _dataCollectorTimer.inactivate();  // Turn off just in case we get here from some odd transition
            DataLog(log_level_proc_pqi_error) << "Data timer active during EPCA.  Turning off.  Number of samples="
                                              << _numberOfSamples << endmsg;

         }
      }

      break;

      // These states could be executed.  If we get here, make sure we are turned off (like in the case of a procedure change)
      case SS_PIR_PLASMA :
      case SS_PIR_NOPLASMA :
      {
         DataLog(log_level_proc_pqi_info) << "PLO monitor sees substate changed to "
                                          << SubstateMap::ssStr(substate) << ".  Disabling monitor." << endmsg;

         disable();

         // We can entry PIR after a Midrun shorter than the max but greater
         // than the min.  If baseline is not established, then do it now.
         if (!_baselineEstablished && (_numberOfSamples >= MIDRUN_BASELINE_MIN_SAMPLES) )
         {
            const float currentVc = _pd->Volumes().Vc.Get();
            CalculateBaseline(currentVc);
         }
      }

      break;

      default :
         if (substate > SS_EXTENDED_PCA)
         {
            DataLog(log_level_proc_pqi_info) << "PLO monitor deactivated for rest of run.  Past EPCA state." << endmsg;

            Deactivate();
         }

         break;
   }
}

void PlasmaLineOcclusion_Monitor::HandlePfrStatusMsg ()
{
   const PFR_Status msg = (PFR_Status)_pfrStatusMsg.getData();

   DataLog(log_level_proc_pqi_info) << "PFR Status is " << msg << endmsg;

   if (msg == RestorePFRData)
   {
      DataLog(log_level_proc_pqi_info) << "PLO monitor disabled.  PFR run." << endmsg;

      Deactivate();
   }
}



void PlasmaLineOcclusion_Monitor::Deactivate ()
{
   // Don't need to worry about any more state changes.
   _substateChangeMsg.deregisterMsg();

   disable();
}

/* FORMAT HASH c99e90dc6d257dbc1db2f48a82a9df09 */
