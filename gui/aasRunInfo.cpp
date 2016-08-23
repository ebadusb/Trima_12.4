#include "aasRunInfo.hpp"
#include "aphwind.hpp" // So I can call "goto_screen".
#include "guiglobs.hpp"

AasRunInfo::AasRunInfo() :

    #if CPU==SIMNT
    #define __AAS_RUN_INFO_SIMNT_CONSTRUCTOR_EXTRAS__
    #include "../unit_test/gui/h/simAasRunInfo.hpp"
    #undef __AAS_RUN_INFO_SIMNT_CONSTRUCTOR_EXTRAS__
    #endif

// Callback Pointers
   _attachedObj(NULL),
   _aasVolAddedCallback(),
   _mssSetupCompleteCallback(),

   _readyForRASAddition(false),
   _readyForPASAddition(false),

   _mssSetupComplete(false),

   _isRasProcedureCheckCount(-1),
   _isPasProcedureCheckCount(-1),
   _targetedAsSplitCheckCount(-1),

   _oldIsRasProcedure(false),
   _oldIsPasProcedure(false),
   _oldTargetedAsSplit(false)
{}

AasRunInfo::~AasRunInfo()
{
   _msgAasAdded.deregisterMsg();
   _msgCancelAas.deregisterMsg();
   _msgAasComplete.deregisterMsg();
   _msgPfrForceTransition.deregisterMsg();
   _msgReadyForPASAddition.deregisterMsg();
   _msgReadyForRASAddition.deregisterMsg();
   _msgPartialCancelation.deregisterMsg();
}

#define __AAS_MSG_INIT__(MSG_CLASS_NAME, MSG_OBJ_NAME, CALLBACK_FUNC_NAME)  \
   MSG_OBJ_NAME.init(Callback<AasRunInfo>(this, &AasRunInfo::CALLBACK_FUNC_NAME), MessageBase::SNDRCV_RECEIVE_ONLY)

void AasRunInfo::init ()
{
   __AAS_MSG_INIT__(AasAdditionVolumeUpdate, _msgAasAdded,          aasAdditionVolumeUpdate);
   __AAS_MSG_INIT__(MSSCompletionMsg,      _msgAasComplete,        aasComplete);
   __AAS_MSG_INIT__(CancelMssMsg,          _msgCancelAas,          cancelAas);
   __AAS_MSG_INIT__(ProcToGuiAasPfrMsg,    _msgPfrForceTransition, makePfrScreenTransition);
   __AAS_MSG_INIT__(MSSPASAdditionMsg,     _msgReadyForPASAddition,    receivedPASAddition);
   __AAS_MSG_INIT__(MSSRASAdditionMsg,     _msgReadyForRASAddition,    receivedRASAddition);
   __AAS_MSG_INIT__(PartialCancellationMsg, _msgPartialCancelation, handlePartialCancelation);
   __AAS_MSG_INIT__(MSSSetupCompleteMsg,   _msgMssSetupComplete,   handleMssSetupComplete);
}

void AasRunInfo::detach (void* obj)
{
   if ( obj == _attachedObj )
   {
      CallbackBase null_callback;

      _aasVolAddedCallback      = null_callback;
      _mssSetupCompleteCallback = null_callback;

      _attachedObj              = NULL;
   }
}

void AasRunInfo::setAasAddedCallback (const CallbackBase& callback)
{
   _aasVolAddedCallback = callback;
}

void AasRunInfo::setMssSetupCompleteCallback (const CallbackBase& callback)
{
   _mssSetupCompleteCallback = callback;
}

GUI_SCREEN_ID AasRunInfo::convertPfrEnumToScreenId (AAS_PFR_SCREEN pfrValue)
{
   switch (pfrValue)
   {
      case AAS_PFR_UNLOAD :        return GUI_SCREEN_NULL_SCREEN; // Do nothing; Handled by screen transition table;
      case AAS_PFR_CLAMPCHANNEL :  return GUI_SCREEN_AAS_CLAMP_CHANNEL;
      case AAS_PFR_CHANNELTEST :   return GUI_SCREEN_AAS_CHANNEL_TEST;
      case AAS_PFR_PASCONNECT :    return GUI_SCREEN_AAS_CONNECT_PAS;
      case AAS_PFR_RASCONNECT :    return GUI_SCREEN_AAS_CONNECT_RAS;
      case AAS_PFR_REDIRECT_PAS :  return GUI_SCREEN_AAS_PAS_SPLIT;
      case AAS_PFR_PRIME :         return GUI_SCREEN_AAS_PRIME;
      case AAS_PFR_ADDITION :      return GUI_SCREEN_AAS_ADDITION;

      default :
         DataLog (log_level_gui_mss_error) << "ERROR: Requested screen choice unknown.  Canceling AAS." << endmsg;
         cancelAas();
         return GUI_SCREEN_NULL_SCREEN;
   }
}

void AasRunInfo::makePfrScreenTransition ()
{
   AAS_PFR_SCREEN scrnTran = (AAS_PFR_SCREEN)_msgPfrForceTransition.getData();

   DataLog (log_level_gui_mss_info) << "Making PFR screen transition: " << scrnTran << endmsg;

   GUI_SCREEN_ID nextScreen = convertPfrEnumToScreenId(scrnTran);

   if (nextScreen != GUI_SCREEN_NULL_SCREEN)
   {
      DataLog (log_level_gui_mss_info) << "Got PFR message requesting screen transition. Value="
                                       << scrnTran << ", New Screen=" << nextScreen << endmsg;
      Base_Apheresis_Window::goto_screen(nextScreen, "AAS_PFR_TRANSITION");
   }
   else DataLog (log_level_gui_mss_info) << "Got PFR message, it requires no transition." << endmsg;
}

void AasRunInfo::aasComplete (void)
{
   if (_msgAasComplete)
   {
      END_MSS_MESSAGE_SENDER sender = (END_MSS_MESSAGE_SENDER)_msgAasComplete.getData();

      DataLog (log_level_gui_mss_info) << "Received word that AAS has completed from "
                                       << ((sender==END_MSS_MESSAGE_SENDER_PAS) ? "PAS" : ((sender==END_MSS_MESSAGE_SENDER_RAS) ? "RAS" : "unknown"))
                                       << " substate." << endmsg;
   }

   guiglobs::button_audio_feedback.create_feedback_sound (END_AAS_SOUND, SOUND_IMMEDIATE);
}

void AasRunInfo::handlePartialCancelation (void)
{
   PARTIAL_END_RUN_TYPE partialAbortType = (PARTIAL_END_RUN_TYPE)_msgPartialCancelation.getData();

   DataLog (log_level_gui_mss_info) << "AasRunInfo received partial abort message. (Type: "
                                    << partialAbortType << ")" << endmsg;

   if (guiglobs::screen_status.get_current_system_state() != METERED_STORAGE)
   {
      DataLog (log_level_gui_mss_info) << "Do nothing with mss abort info (we're not currently in metered storage)." << endmsg;
      return;
   }

   GUI_SCREEN_ID currentScreenId = guiglobs::screen_status.get_current_screen();

   if (partialAbortType == END_PAS_ADDITION)
   {
      if (currentScreenId == GUI_SCREEN_AAS_CONNECT_PAS)
      {
         DataLog (log_level_gui_mss_info) << "PAS aborted. Send connections complete message and transition to prime." << endmsg;
         MSSConnectedMsg connectionsCompleted(MessageBase::SEND_LOCAL);
         connectionsCompleted.send();
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_PRIME, "GUI_SCREEN_AAS_PRIME");
      }
      else if (currentScreenId == GUI_SCREEN_AAS_PRIME && _readyForRASAddition)
      {
         DataLog (log_level_gui_mss_info) << "PAS aborted, RASAddition is ready. Go directly to GUI_SCREEN_AAS_ADDITION screen." << endmsg;
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
      }
   }

   if (partialAbortType == END_RAS_ADDITION)
   {
      if (currentScreenId == GUI_SCREEN_AAS_CONNECT_RAS)
      {
         DataLog (log_level_gui_mss_info) << "RAS aborted. Transition to PAS connect screen." << endmsg;
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_CONNECT_PAS, "GUI_SCREEN_AAS_CONNECT_PAS");
      }
   }
}

void AasRunInfo::handleMssSetupComplete ()
{
   _mssSetupComplete = true;
   _mssSetupCompleteCallback();
}

void AasRunInfo::aasAdditionVolumeUpdate (void)
{
   // Let's use this as the impetus for updating the meter, but
   // we'll still draw the data from CDS to be more robust to PFR.
   _aasVolAddedCallback();
}

void AasRunInfo::cancelAas (void)
{
   DataLog (log_level_gui_mss_info) << "AasRunInfo Sending Aas Cancel Message" << endmsg;
   MSSEndRunMsg _endRun(MessageBase::SEND_LOCAL);
   _endRun.send(0);
}




void AasRunInfo::receivedRASAddition (void)
{
   _readyForRASAddition = true;
   gotoAddition();
}


void AasRunInfo::receivedPASAddition (void)
{
   _readyForPASAddition = true;
   gotoAddition();
}



void AasRunInfo::gotoAddition (void)
{
   GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_screen() ;
   DataLog(log_level_gui_mss_info) << "Receive MSSPASAdditionMsg or MSSRASAdditionMsg from Proc"
                                   << " current Screen: " << currentScreen << endmsg;

   if ( isRasProcedure() && isPasProcedure() )
   {
      if (_readyForPASAddition && !_readyForRASAddition)
      {
         // if we want to see the split and the current screen is not split.. display
         if (   guiglobs::cdsData.config.Procedure.Get().key_plt_mss_split_notif
                && currentScreen != GUI_SCREEN_AAS_PAS_SPLIT)
         {
            DataLog(log_level_gui_mss_info) << "Transitioning to split screen. (PAS && RAS)" << endmsg;
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_PAS_SPLIT, "GUI_SCREEN_AAS_PAS_SPLIT");
         }
      }
      else if (_readyForRASAddition && !_readyForPASAddition)
      {

         // if we want to see the split and the current screen is not split.. display
         if (   guiglobs::cdsData.config.Procedure.Get().key_plt_mss_split_notif )
         {
            DataLog(log_level_gui_mss_info) << "waiting for PAS to init Split screen. (PAS && RAS)" << endmsg;
         }
         else
         {
            DataLog (log_level_gui_mss_info) << "Transition to AAS addition. (PAS && RAS)" << endmsg;
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
         }
      }
      else if (_readyForRASAddition && _readyForPASAddition)
      {
         if (currentScreen != GUI_SCREEN_AAS_ADDITION)
         {
            // if we want to see the split and the current screen is not split.. display
            if (   guiglobs::cdsData.config.Procedure.Get().key_plt_mss_split_notif
                   && currentScreen != GUI_SCREEN_AAS_PAS_SPLIT)
            {
               DataLog(log_level_gui_mss_info) << "Transitioning to split screen. (PAS && RAS)" << endmsg;
               Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_PAS_SPLIT, "GUI_SCREEN_AAS_PAS_SPLIT");
            }
            else
            {

               DataLog (log_level_gui_mss_info) << "Transition to AAS addition. (PAS && RAS)" << endmsg;
               Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
            }
         }      // else we are already in addition dont go back to split...
      }
      else
      {

         DataLog(log_level_gui_mss_error) << "ERROR: Received a Metering Addition notice without being in that state.(PAS && RAS)" << endmsg;
         // i have no clue what you've done here to get into this situation!
      }

   }
   else if ( isRasProcedure() && _readyForRASAddition )
   {
      if (currentScreen == GUI_SCREEN_AAS_PRIME)
      {
         DataLog (log_level_gui_mss_info) << "Transition to AAS addition. (RAS only)" << endmsg;
         Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
      }

   }
   else if ( isPasProcedure() && _readyForPASAddition)
   {
      if (currentScreen == GUI_SCREEN_AAS_PRIME)
      {
         if (guiglobs::cdsData.config.Procedure.Get().key_plt_mss_split_notif)
         {
            DataLog(log_level_gui_mss_info) << "Transitioning to split screen. (PAS only)" << endmsg;
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_PAS_SPLIT, "GUI_SCREEN_AAS_PAS_SPLIT");
         }
         else
         {
            DataLog (log_level_gui_mss_info) << "Transition to AAS addition. (PAS only)" << endmsg;
            Base_Apheresis_Window::goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
         }
      }
   }
   else      // no metering we should not be here
   {
      DataLog(log_level_gui_mss_error) << "ERROR: Received a Metering Addition notice without being in that state." << endmsg;
   }

}






void AasRunInfo::attach (void* obj)
{
   if ( _attachedObj )
   {
      detach(_attachedObj);
   }

   _attachedObj = obj;

   init();
}

#if CPU==SIMNT
#define __AAS_RUN_INFO_SIMNT_FUNCTION_IMPLEMENTATIONS__
#include "../unit_test/gui/h/simAasRunInfo.hpp"
#undef __AAS_RUN_INFO_SIMNT_FUNCTION_IMPLEMENTATIONS__
#else

// These are the real functions.
float AasRunInfo::getPasNeeded ()
{
   if (!isPasProcedure()) return 0.0f;

   float pasMultFactor = guiglobs::cdsData.run.PltSSVolumeFactor.Get();
   if (pasMultFactor <= 0.0f) pasMultFactor = 1.0f;

   return guiglobs::cdsData.targets.PltSolutionTarget.Get() * pasMultFactor;
}

// the total vol needed function here includes
// prime volume.  This is mostly for calculating
// number of PAS bags needed.
float AasRunInfo::getPasTotalVolNeeded ()
{
   float returnVol = getPasNeeded();

   if (returnVol <= 0.0f) return 0.0f;


   if (!guiglobs::cdsData.run.rinsebackVolumeSuccessMss.Get())
   {
      returnVol += guiglobs::pasNoRinsebackPenalty;
   }
   else
   {
      returnVol += guiglobs::pasPrimeVol;
   }

   returnVol /= guiglobs::pasVolBiasPercent;

   return returnVol;
}

float AasRunInfo::getRasNeeded ()
{
   if (!isRasProcedure()) return 0.0f;

   float targetRasVol = guiglobs::cdsData.targets.RBCSolutionTarget.Get();

   if (productTargetedAsDrbcPtf())
   {
      bool prod1_Ok = (!guiglobs::cdsData.run.PTFRBCprod1Disabled.Get()) &&
                      (guiglobs::cdsData.volumes.RBCProduct_A_FinalVolume.Get() > 1.0f);

      bool prod2_Ok = (!guiglobs::cdsData.run.PTFRBCprod2Disabled.Get()) &&
                      (guiglobs::cdsData.volumes.RBCProduct_B_FinalVolume.Get() > 1.0f);

      if (prod1_Ok == prod2_Ok) return targetRasVol + (2.0f * (guiglobs::ptfFilterVol - guiglobs::PtfFilterLoss));
      else return (targetRasVol / 2.0f) + guiglobs::ptfFilterVol - guiglobs::PtfFilterLoss;
   }

   return targetRasVol + guiglobs::ptfFilterVol - guiglobs::PtfFilterLoss;
}

float AasRunInfo::getAasNeeded ()
{
   // The nominal case. If one has been canceled, zero is returned
   float volume = getPasNeeded() + getRasNeeded();

   // Check for case where one of the two additions gets canceled so that
   // progress bar will be adjusted properly to account for what's accumulated
   const bool rasCanceled = guiglobs::cdsData.run.cancelJustRas.Get();
   const bool pasCanceled = guiglobs::cdsData.run.cancelJustPas.Get();

   // When RAS has been canceled, add its volume accumulated thus far
   if (rasCanceled)
      volume += guiglobs::cdsData.volumes.VstorageRBCBag.Get();

   // Likewise for PAS
   if (pasCanceled)
      volume += guiglobs::cdsData.volumes.VstoragePltBag.Get();

   return volume;
}

bool AasRunInfo::isRasProcedure (bool ignoreAlarms = false)
{  // Emulate the similar function from proc/interface/MssCommon.cpp
   State_names currentState          = guiglobs::screen_status.get_current_system_state();

   bool        blockedDueToCPSAlarms = rasBlockedDueToCpsAlarms();


   // certain events during Metering can invalidate RAS (i.e. AC disconnect failure)
   bool rasStillOk =
      // This variable should be ignored (always true) if prior to donor disconnect,
      // since proc doesn't set MeterRbcs_OK until then.
      (currentState < DONOR_DISCONNECT) ||
      // If we're at or after donor disconnect, check this variable.
      guiglobs::cdsData.run.MeterRbcs_OK.Get();

   bool collectionOk =
      // Either it's too early to care about collect
      // volumes and such...
      ((currentState <= BLOOD_RUN) ||
       // OR, make sure we collected some platelet volume
       // and completed first cycle.
       (((guiglobs::cdsData.volumes.RBCVolume.Get() > guiglobs::ptfFilterVol) &&
         guiglobs::cdsData.run.FirstCycleComplete.Get()))) &&
      // Also, make sure we're not blocked due to CPS alarms.
      !blockedDueToCPSAlarms;

   bool returnVal =
      guiglobs::cassette_mgr->currentSetIsRas() &&
      guiglobs::cdsData.targets.MeterRbcs.Get() &&
      guiglobs::cdsData.targets.RBCSolutionTarget.Get() > 0.0f &&
      collectionOk && rasStillOk &&
      ( ( (!guiglobs::cdsData.run.cancelMSS.Get()) &&
          (!guiglobs::cdsData.run.cancelJustRas.Get())) ||
        ignoreAlarms);
#if 0  // Debug logging only
       // Only log every tenth one, or anytime the return value changes.
   if ((++_isRasProcedureCheckCount % 10 == 0) || ( _oldIsRasProcedure != returnVal ))
   {
      _oldIsRasProcedure = returnVal;

      DataLog (log_level_gui_mss_debug) << "isRasProcedure=" << returnVal
                                        << " (state=" << currentState
                                        << "; rbcVol=" << guiglobs::cdsData.volumes.RBCVolume.Get()
                                        << "; 1stCycle=" << guiglobs::cdsData.run.FirstCycleComplete.Get()
                                        << "; setIsRas=" << guiglobs::cassette_mgr->currentSetIsRas()
                                        << "; isRas=" << guiglobs::cdsData.targets.MeterRbcs.Get()
                                        << "; rasTarg=" << guiglobs::cdsData.targets.RBCSolutionTarget.Get()
                                        << "; ignoreAlarms=" << ignoreAlarms
                                        << "; cancelMss=" << guiglobs::cdsData.run.cancelMSS.Get()
                                        << "; cancelRas=" << guiglobs::cdsData.run.cancelJustRas.Get()
                                        << "; blockedDueToCPSAlarms=" << blockedDueToCPSAlarms
                                        << "; rasStillOk=" << rasStillOk
                                        << ")" << endmsg;
   }
#endif
   return returnVal;
}

bool AasRunInfo::isPasProcedure (bool ignoreAlarms = false)
{  // Emulate the similar function from proc/interface/MssCommon.cpp
   State_names currentState = guiglobs::screen_status.get_current_system_state();

   // certain events during Metering can invalidate PAS
   bool pasStillOk =
      // This variable should be ignored (always true) if prior to donor disconnect,
      // since proc doesn't set MeterPlatelets_OK until then.
      (currentState < DONOR_DISCONNECT) ||
      // If we're at or after donor disconnect, check this variable.
      guiglobs::cdsData.run.MeterPlatelets_OK.Get();

   bool collectionOk =
      // Either it's too early to care about collect
      // volumes and such...
      ((currentState <= BLOOD_RUN) ||
       // OR, make sure we collected some platelet volume
       // and completed first cycle.
       (((guiglobs::cdsData.volumes.PlateletVolume.Get() > 0.0) &&
         (guiglobs::cdsData.volumes.PlateletVolume.Get() + guiglobs::cdsData.targets.PltSolutionTarget.Get() <= 1200.0f) &&
         guiglobs::cdsData.run.FirstCycleComplete.Get())));

   bool returnVal =
      guiglobs::cassette_mgr->currentSetIsPas() &&
      guiglobs::cdsData.targets.MeterPlatelets.Get() &&
      guiglobs::cdsData.targets.PltSolutionTarget.Get() >= 1.0f &&
      collectionOk && pasStillOk &&
      ( ((!guiglobs::cdsData.run.cancelMSS.Get()) && (!guiglobs::cdsData.run.cancelJustPas.Get())) || ignoreAlarms);

#if 0  // Debug logging only
       // Only log every tenth one, or anytime the return value changes.
   if ((++_isPasProcedureCheckCount % 10 == 0) || ( _oldIsPasProcedure != returnVal ))
   {
      _oldIsPasProcedure = returnVal;

      DataLog (log_level_gui_mss_debug) << "isPasProcedure(state=" << currentState
                                        << "; pltVol=" << guiglobs::cdsData.volumes.PlateletVolume.Get()
                                        << "; 1stCycle=" << guiglobs::cdsData.run.FirstCycleComplete.Get()
                                        << "; setIsPas=" << guiglobs::cassette_mgr->currentSetIsPas()
                                        << "; isPas=" << guiglobs::cdsData.targets.MeterPlatelets.Get()
                                        << "; pasTarg=" << guiglobs::cdsData.targets.PltSolutionTarget.Get()
                                        << "; ignoreAlarms=" << ignoreAlarms
                                        << "; cancelMss=" << guiglobs::cdsData.run.cancelMSS.Get()
                                        << "; cancelPas=" << guiglobs::cdsData.run.cancelJustPas.Get()
                                        << "; pasStillOk=" << pasStillOk
                                        << ")" << endmsg;
   }
#endif

   return returnVal;
}

float AasRunInfo::getAasAdded ()
{
   return guiglobs::cdsData.volumes.VstoragePltBag.Get() +
          guiglobs::cdsData.volumes.VstorageRBCBag.Get();
}

bool AasRunInfo::readyForAddition () { return (_readyForPASAddition || _readyForRASAddition); }

bool AasRunInfo::productTargetedAsDrbcPtf ()
{
   bool  setIsDrbc       = guiglobs::cassette_mgr->currentSetIsDrbc();
   bool  setIsRas        = guiglobs::cassette_mgr->currentSetIsRas();
   float targetedRas     = guiglobs::cdsData.targets.RBCSolutionTarget.Get();
   bool  isPromptedSplit = guiglobs::cdsData.config.isPromptedDrbcSplit(guiglobs::cdsData.targets.ProcNumber.Get());

   bool  targetedAsSplit =
      isPromptedSplit &&
      setIsDrbc &&
      setIsRas &&
      (targetedRas > 0);

   // Only log every tenth one, or anytime the return value changes.
   if ((++_targetedAsSplitCheckCount % 10 == 0) || ( _oldTargetedAsSplit != targetedAsSplit ))
   {
      _oldTargetedAsSplit = targetedAsSplit;

      DataLog (log_level_gui_mss_debug) << "Testing targeted as split: Cassette(drbc="
                                        << setIsDrbc << "; ras=" << setIsRas << "); Procedure(targetedRas="
                                        << targetedRas << "; promptedSplit=" << isPromptedSplit << ") RESULT: " << targetedAsSplit << endmsg;
   }

   return targetedAsSplit;
}

bool AasRunInfo::rasBlockedDueToCpsAlarms ()
{
   bool  targetedAsDouble = productTargetedAsDrbcPtf();

   bool  rbc1Invalid      = guiglobs::cdsData.run.PTFRBCprod1Disabled.Get();
   bool  rbc2Invalid      = guiglobs::cdsData.run.PTFRBCprod2Disabled.Get();

   float rbc1Collected    = guiglobs::cdsData.volumes.RBCProduct_A_FinalVolume.Get() > 0.0f;
   float rbc2Collected    = guiglobs::cdsData.volumes.RBCProduct_B_FinalVolume.Get() > 0.0f;

   bool  returnVal        = false;

   if (guiglobs::screen_status.get_current_system_state() == BLOOD_PRIME)
   {
      returnVal = false;
   }
   else if (guiglobs::screen_status.get_current_system_state() == BLOOD_RUN)
   {
      // If we're still collecting...
      if (targetedAsDouble)
         returnVal =
            (rbc1Invalid || !rbc1Collected) &&
            rbc2Invalid;
      else returnVal = rbc1Invalid;
   }
   else
   {
      if (targetedAsDouble)
         returnVal =
            (rbc1Invalid || !rbc1Collected) &&
            (rbc2Invalid || !rbc2Collected);
      else returnVal = rbc1Invalid;
   }

   // If we're blocking RAS due to CPS alarms, log it.
   if (returnVal)
      DataLog (log_level_gui_mss_debug) << "RAS is blocked due to CPS alarms (PtfSplit="
                                        << targetedAsDouble << ") - (RBC1:Col="
                                        << rbc1Collected << ";Val=" << !rbc1Invalid << ") - (RBC2:Col="
                                        << rbc2Collected << ";Val=" << !rbc2Invalid << ");" << endmsg;

   return returnVal;
}


bool AasRunInfo::mssSetupComplete () { return _mssSetupComplete; }

#undef __AAS_MSG_INIT__

#endif

/* FORMAT HASH a0b6c84eaee91eaf03639f45ab340846 */
