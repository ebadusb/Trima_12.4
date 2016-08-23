/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      fullRoutine.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT

// From common
#include "trima_datalog.h"

// From GUI
#include "gui.h"
#include "guiglobs.hpp"
#include "sealAndDetach.hpp"
#include "pasBarcodeConfirmBox.hpp"
// From unit_test
#include "fullRoutine.hpp"
#include "string_screen_mapper.hpp"

#include "simScreenCapture.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "scrncomm.hpp"
#include "simActionQueue.hpp"
#include "simConfig.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

/*********************************************************************************************
                                    Constructor
**********************************************************************************************/

FullScreenshotRoutine::FullScreenshotRoutine()
   : SimRoutineBase()
{}

/*********************************************************************************************
                                    Destructor
**********************************************************************************************/

FullScreenshotRoutine::~FullScreenshotRoutine()
{
   DataLog (log_level_sim_main_routine_error) << "FullScreenshotRoutine destructor.  That's bad." << endmsg;
}

/*********************************************************************************************
                                    Utility Functions
**********************************************************************************************/

void FullScreenshotRoutine::populateActionQueue ()
{
	initConcatStrings();
   _actionQueue.add("RequiredConfig",      &SimConfig::setupPrerunSimulatorConfig);
   _actionQueue.add("Config Comp pls effect Predict", &configCompPlsOptionPredictEffect);
   _actionQueue.add("Config Amap plt effect Predict", &configShowPlasmaPredictEffect);

   //this is the one that causes exception, GOT FIXED
   _actionQueue.add("Config Amap plt effect Predict", &configAmapPltOptionPredictEffect);
   _actionQueue.add("RequiredConfig",      &SimConfig::setupRunSimulatorConfig);
   _actionQueue.add("Config Comp pls effect Run Sum", &walkThroughCompPlasmaRunSum);
   _actionQueue.add("Run Screens ",        &getRunScreens, false);
   _actionQueue.add("Config",              &SimConfig::walkThroughConfig);
   _actionQueue.add("Safety Alarms",       &SimAlarms::walkThroughSafetyAlarms);
   _actionQueue.add("Control Alarms",      &SimAlarms::walkThroughControlAlarms);
   _actionQueue.add("Abort Screens",       &SimUtilities::walkThroughAbortScreens);
   _actionQueue.add("Replacement Fluid",   &SimUtilities::walkThroughReplacementFluidAlarm);
   _actionQueue.add("DRBC Split",          &SimUtilities::walkThroughDrbcSplitScreen);
   _actionQueue.add("Barcode",             &SimUtilities::walkThroughBarcodeScreens);
   _actionQueue.add("Cfr11 1",             &SimUtilities::walkThroughCFR11Screens);
   _actionQueue.add("Power Down",          &SimUtilities::walkThroughPowerDownScreens);
   _actionQueue.add("Splash",              &SimUtilities::walkThroughSplashScreen);
}

/*********************************************************************************************
                                    Message Handlers
**********************************************************************************************/

// This is the switch statement that handles screen changes.  This is the primary method for
// progressing the screenshot routine along.  Whenver a new static screen gets allocated, this
// function gets called, and you can take action depending on the new screen.
// If adding new stuff, it's preferable to try to keem this function very clean so that you can
// easily infer navigation just by glancing at it.  If you're adding more than a couple lines to
// any particular case, consider putting it in a separate function and calling that instead.
// Also, to the extent possible, try to keep them in the order that the routine will visit them.
void FullScreenshotRoutine::dealWithScreenChange ()
{
   if ( !SimUtilities::autoNavOn() ) return;   // Don't do anything if we're not taking screenshots.

   // Let's wait a second to make sure all screen allocation stuff takes place.
   SimDelay::seconds(1);

   GUI_SCREEN_ID newScreen = guiglobs::screen_status.get_current_displayed_screen();
   GUI_SCREEN_ID oldScreen = guiglobs::screen_status.get_previous_displayed_screen();

   // Increment and return the number of times this particular screen has been displayed.
   int scrnCount = SimUtilities::screenCounter.count(newScreen);

   DataLog (log_level_sim_main_routine_info) << "Screen Change message received.  Transition from "
                                             << oldScreen << " to " << newScreen << "; scrnCount=" << scrnCount << endmsg;

   switch (newScreen)
   {
      case GUI_SCREEN_SYSCASSETTE :
         snapCassetteScreen();
         SimUtilities::changeScreens(GUI_SCREEN_PTF_MOUNT, "--REF select to RBC Setup--");
         break;

      case GUI_SCREEN_PTF_MOUNT :
         SimUtilities::cycleThroughPtfLoadTypes();
         SimUtilities::changeScreens(GUI_SCREEN_SYSDISP, "--RBC Setup to SysDisp--");
         break;

      case GUI_SCREEN_SYSDISP :
         SimUtilities::cycleThroughSysDispTypes();
         SimUtilities::changeScreens(GUI_SCREEN_SYSCLAMP, "--SysDisp to SysClamp--");
         break;

      case GUI_SCREEN_SYSCLAMP :
         SimUtilities::cycleThroughSysClampDisplayTypes();
         SimUtilities::changeScreens(GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, "--SysClamp to jP nO AIR--");
         break;

      case GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG :
         SimUtilities::cycleThroughJpClampDisplayTypes();
         SimUtilities::changeScreens(GUI_SCREEN_PAS_INIT_SPLIT, "--SysClamp to PasSplit--");
         break;

      case GUI_SCREEN_PAS_INIT_SPLIT :
         SimUtilities::cycleThroughPasInitSplitTypes();
         SimUtilities::changeScreens(GUI_SCREEN_RBC_SPLIT_INIT, "--PasSplit to RbcSplitInit--");
         break;

      case GUI_SCREEN_RBC_SPLIT_INIT :
         SimScreenCapture::screenshot(GUI_SCREEN_RBC_SPLIT_INIT, "rbcSplitInit");
         SimUtilities::setSystemState(DISPOSABLE_TEST);
         break;

      case GUI_SCREEN_SYSTEST :
         SimScreenCapture::screenshot(GUI_SCREEN_SYSTEST, "dispTest");
         SimUtilities::setStatLine(textStatDispTestComplete);
         SimScreenCapture::screenshot(GUI_SCREEN_SYSTEST, "dispTestComplete");
         SimUtilities::changeScreens(GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION, "--SysTest to JpBloodDiversion--");
         break;

      case GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION :
         SimScreenCapture::screenshot(GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION, SHOT_OPTS_LBL_JAPAN_ONLY, "jpClampDiversion");
         SimUtilities::changeScreens(GUI_SCREEN_CLAMP_CROSSOVER, "--JpBloodDiversion to ClampCrossover--");
         break;

      case GUI_SCREEN_CLAMP_CROSSOVER :
         // need to deactivate status line if present
         guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_CLAMP_CROSSOVER, "clampCrossover");
         SimUtilities::setSystemState(AC_CONNECTED);
         break;

      case GUI_SCREEN_SYSACATT :
         snapAcConnectScreen();
         SimUtilities::setSystemState(AC_PRIME);
         break;

      case GUI_SCREEN_SYSDONOR :
         // This function will take care of getting you to the next screen.
         sysDonorNav();
         break;

      case GUI_SCREEN_DONVITAL :
         snapDonVitals();
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONVITAL);
         break;

      case GUI_SCREEN_DONINFO :
         snapDonInfo();
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONINFO);
         break;

      case GUI_SCREEN_PREDICT :
         snapPredict();
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_PREDICT);
         if ( guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_PREDICT_CONFIRM_BOX)
            SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CONFIRM, GUI_SCREEN_PREDICT_CONFIRM_BOX);
         break;

      case GUI_SCREEN_CONFIRM_DON_INFO :
         SimUtilities::cycleThroughConfirmDonInfoDisplayTypes();
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, newScreen);
         break;

      case GUI_SCREEN_DONCONN :
         SimUtilities::sendProcedureData(SimUtilities::PRIME, SimUtilities::PLT | SimUtilities::PLAS);
         SimUtilities::cycleThroughDonConnTypes();
         // works with start draw
         SimScreenTouch::touch(GUI_BUTTON_DON_CONN_START_DRAW, newScreen);
         break;

      case GUI_SCREEN_TROUBLE :
         SimUtilities::sendAdjustData(true);
         SimScreenCapture::screenshot(GUI_SCREEN_TROUBLE, "adjustPltPlsRbc");
         SimUtilities::sendAdjustData(false);
         SimScreenCapture::screenshot(GUI_SCREEN_TROUBLE, "disconnDrbc");
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_TROUBLE);            // Continue
         break;

      case GUI_SCREEN_DISCONN :
         SimScreenCapture::screenshot(GUI_SCREEN_DISCONN, "disconn");
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DISCONN);            // Continue
         SimScreenCapture::screenshot(GUI_SCREEN_DISCONN, "disconnConfirm");
         SimUtilities::changeScreens(GUI_SCREEN_SEAL_AND_DETACH, "--disconn to Seal&Detach --");

      case GUI_SCREEN_SEAL_AND_DETACH :
         SimScreenCapture::screenshot(GUI_SCREEN_SEAL_AND_DETACH, "seal");
         ((Screen_SEAL_AND_DETACH*)guiglobs::scrnComm->getScreenInstancePointer(GUI_SCREEN_SEAL_AND_DETACH))->transitionToState(Screen_SEAL_AND_DETACH::DETACH);
         SimDelay::seconds(1);
         SimScreenCapture::screenshot(GUI_SCREEN_SEAL_AND_DETACH, "detach");
         SimUtilities::setSystemState(METERED_STORAGE);
         SimUtilities::changeScreens(GUI_SCREEN_AAS_CLAMP_CHANNEL, "--seal&Detach to aasClamp --");
         break;

      case GUI_SCREEN_AAS_CLAMP_CHANNEL :
         SimUtilities::cycleThroughChannelClampDisplayTypes();
         SimUtilities::changeScreens(GUI_SCREEN_AAS_CHANNEL_TEST, "--aasClamp to aasChanTest --");
         break;

      case GUI_SCREEN_AAS_CHANNEL_TEST :
         SimScreenCapture::screenshot(GUI_SCREEN_AAS_CHANNEL_TEST, "aasChanTest");
         SimUtilities::changeScreens(GUI_SCREEN_AAS_CONNECT_RAS, "--aasChanTest to connectRas --");
         break;

      case GUI_SCREEN_AAS_CONNECT_RAS :
         SimUtilities::cycleThroughRasConnectDisplayTypes();
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_AAS_CONNECT_RAS);
         SimScreenCapture::screenshot(GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX, "rasConnectConfirm");
         SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX);
         SimUtilities::changeScreens(GUI_SCREEN_AAS_CONNECT_PAS, "--connectRas to connectPas --");
         break;

	  case GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX:
         SimScreenCapture::screenshot(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX, "pasBarcodeConfirm");
         SimUtilities::sendTraverseMsg(CHANGE_LABELS, 0);
         SimScreenCapture::screenshot(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX, "pasInvalidBarcodeConfirm");
         SimUtilities::sendTraverseMsg(UNIVERSAL, 0);
         SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX);

         SimScreenTouch::touch(GUI_BUTTON_ABORTCNF_EXIT, GUI_SCREEN_ABORTCNF);
         //SimUtilities::changeScreens(GUI_SCREEN_AAS_CONNECT_PAS, "--connectRas to connectPas --");
      //case GUI_SCREEN_AAS_CONNECT_PAS :
         SimUtilities::sendTraverseMsg(UNIVERSAL, PAS_CONNECT_TYPE_OVERRIDE_OFF);
         SimScreenCapture::screenshot(GUI_SCREEN_AAS_CONNECT_PAS, "pasConnectOverrideOff");

         SimUtilities::sendTraverseMsg(UNIVERSAL, PAS_CONNECT_TYPE_OVERRIDE_ON);
         SimScreenTouch::touch(GUI_BUTTON_MSS_BAG_VOL_PLAT, GUI_SCREEN_AAS_CONNECT_PAS);
         SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "pasConnectOverrideOn");
         SimScreenTouch::keypadSequence("e");

         //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_AAS_CONNECT_PAS);
         //SimScreenCapture::screenshot(GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX, "pasConnectConfirm");
         //SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX);
         SimUtilities::changeScreens(GUI_SCREEN_AAS_PRIME, "--connectPas to aasPrime --");
         break;

      case GUI_SCREEN_AAS_PRIME :
         SimScreenCapture::screenshot(GUI_SCREEN_AAS_PRIME, "aasPrime");
         SimUtilities::changeScreens(GUI_SCREEN_AAS_PAS_SPLIT, "--aasPrime to pasSplit --");
         break;

      case GUI_SCREEN_AAS_PAS_SPLIT :
         SimScreenCapture::screenshot(GUI_SCREEN_AAS_PAS_SPLIT, "pasSplit");
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_AAS_PAS_SPLIT);
         SimUtilities::changeScreens(GUI_SCREEN_AAS_ADDITION, "--pasSplit to aasAddition --");
         break;

      case GUI_SCREEN_AAS_ADDITION :
         guiglobs::apheresis_status_line->set_color (MAGENTA);
         SimUtilities::setAasVolume(35);
         SimScreenCapture::screenshot(GUI_SCREEN_AAS_ADDITION, "aasAddition");
         SimUtilities::setSystemState(METERED_STORAGE_DISCONNECT);
         break;

      case GUI_SCREEN_AAS_RUN_SUMMARY :
         guiglobs::apheresis_status_line->set_color (MAGENTA);
         switch (scrnCount)
         {
            case 1 :
               SimUtilities::cycleThroughAasRunsumDisplayTypes();
               SimUtilities::changeScreens(GUI_SCREEN_MSS_END, "--aasRunSummary to mssSealProducts --");
               break;
            default :
               SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM1, "--aasRunSummary to runSumm1 --");
               break;
         }
         break;

      case GUI_SCREEN_MSS_END :
         guiglobs::apheresis_status_line->set_color (MAGENTA);
         SimUtilities::cycleThroughMssEndDisplayTypes();
         SimUtilities::setSystemState(POST_RUN);
         break;

      case GUI_SCREEN_RUNSUMM1 :
      case GUI_SCREEN_RUNSUM_PLT :
      case GUI_SCREEN_RUNSUM_PLASMA :
      case GUI_SCREEN_RUNSUM_RBC :
      case GUI_SCREEN_RUNSUM_RBC_PROD1 :
      case GUI_SCREEN_RUNSUM_RBC_PROD2 :
      case GUI_SCREEN_RUNSUMM2 :
         // See function dealWithRunSumProcessed();
         DataLog (log_level_sim_main_routine_info) << "Run summary screens handled after stats processed." << endmsg;
         break;

      case GUI_SCREEN_RUNPROC :
         // See function dealWithRunSumProcessed();
         DataLog (log_level_sim_main_routine_info) << "Run summary screens handled after proc status processed." << endmsg;
         SimUtilities::sendProcedureData(SimUtilities::PRIME, SimUtilities::PLT | SimUtilities::PLAS);
         break;

      default :
         DataLog (log_level_sim_main_routine_error) << "Unhandled screen transition." << endmsg;
         break;
   }
}

//
// For the run summary screens, when the screen gets allocated,
// it requests a run summar struct.  The screen display really
// isn't complete until the screen recieves and processes that
// data, even though it's allocate_resources() function has been
// called.  This function deals with the run summary screen after
// it's had a chance to process the info.
//
void FullScreenshotRoutine::dealWithRunSumProcessed ()
{
   if ( !SimUtilities::autoNavOn() ) return;   // Don't do anything if we're not taking screenshots.

   guiglobs::apheresis_status_line->deactivate_status_line_type(END_RUN_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
   guiglobs::apheresis_status_line->set_color (MAGENTA);
   // Let's wait a second to make sure all processing has taken place.
   SimDelay::seconds(1);
   GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_screen();

   // Get the number of times this particular screen has been displayed.
   int scrnCount           = SimUtilities::screenCounter.getCount(currentScreen);
   int runsumResponseCount = SimUtilities::runsumResponseCounter.count(currentScreen);

   DataLog (log_level_sim_main_routine_info) << "Run summary processed message recieved from screen "
                                             << currentScreen << "; scrnCount=" << scrnCount << endmsg;

   switch (currentScreen)
   {
      case GUI_SCREEN_RUNSUMM1 :
         SimUtilities::setStatLine(textStatEndOfRun, END_RUN_STATUS_LINE);
         SimUtilities::cycleThroughRs1DisplayTypes();
         SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_PLT, "--runSumm1 to runsumPlt --");
         break;

      case GUI_SCREEN_RUNSUM_PLT :
         SimUtilities::setStatLine(textStatEndOfRunPlt, END_RUN_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runSumPlt");
         SimUtilities::cycleThroughRunsumLabels("runSumPlt");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_PLT);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runsumPltVerify1a");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_PLT);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runsumPltVerify2a");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_3, GUI_SCREEN_RUNSUM_PLT);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runsumPltVerify3");

         SimUtilities::sendTraverseMsg(CHANGE_REASONS);

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_PLT);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runsumPltVerify1b");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_PLT);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "runsumPltVerify2b");

         SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_PLASMA, "--runsumPlt to runsumPlas --");
         break;

      case GUI_SCREEN_RUNSUM_PLASMA :
         SimUtilities::setStatLine(textStatEndOfRunPlasma, END_RUN_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLASMA, SHOT_OPTS_LBL_INCORRECT, "runSumPlas");
         SimUtilities::cycleThroughRunsumLabels("runSumPlas");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_PLASMA);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLASMA, "runSumPlasVerify1");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_PLASMA);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLASMA, "runSumPlasVerify2");

         SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_RBC, "--runsumPlas to runsumRbc --");
         break;

      case GUI_SCREEN_RUNSUM_RBC :
         SimUtilities::setStatLine(textStatEndOfRunRbc, END_RUN_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC, "runsumRbc");
         SimUtilities::cycleThroughRunsumLabels("runsumRbc");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_RBC);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC, "runsumRbcVerify1");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_RBC);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC, "runsumRbcVerify2");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_3, GUI_SCREEN_RUNSUM_RBC);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC, "runsumRbcVerify3");

         SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_RBC_PROD1, "--runsumRbc to runsumRbc1 --");
         break;

      case GUI_SCREEN_RUNSUM_RBC_PROD1 :
         SimUtilities::setStatLine(textStatRbc1Runsum, END_RUN_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD1, "runsumRbc1");
         SimUtilities::cycleThroughRunsumLabels("runsumRbc1");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_RBC_PROD1);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD1, "runsumRbc1Verify1");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_RBC_PROD1);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD1, "runsumRbc1Verify2");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_3, GUI_SCREEN_RUNSUM_RBC_PROD1);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD1, "runsumRbc1Verify3");

         SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_RBC_PROD2, "--runsumRbc1 to runsumRbc2 --");
         break;

      case GUI_SCREEN_RUNSUM_RBC_PROD2 :
         SimUtilities::setStatLine(textStatRbc2Runsum, END_RUN_STATUS_LINE);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD2, "runsumRbc2");
         SimUtilities::cycleThroughRunsumLabels("runsumRbc2");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_RBC_PROD2);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD2, "runsumRbc1Verify2");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_2, GUI_SCREEN_RUNSUM_RBC_PROD2);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD2, "runsumRbc1Verify2");

         SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_3, GUI_SCREEN_RUNSUM_RBC_PROD2);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_RBC_PROD2, "runsumRbc1Verify2");

         SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM2, "--runsumRbc2 to runsum2 --");
         break;

      case GUI_SCREEN_RUNSUMM2 :
         SimUtilities::setStatLine(textStatEndOfRun, END_RUN_STATUS_LINE);
         guiglobs::apheresis_status_line->set_color (MAGENTA);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUMM2, "runsum2");

         // This traverse message toggles the Japanese features.
         SimUtilities::sendTraverseMsg(CHANGE_NEXT_PROCEDURE_BUTTON);
         SimScreenCapture::screenshot(GUI_SCREEN_RUNSUMM2, SHOT_OPTS_LBL_JAPAN_ONLY, "runsum2Jp");
         SimUtilities::sendTraverseMsg(CHANGE_NEXT_PROCEDURE_BUTTON);

         switch (runsumResponseCount)
         {
            case 1 :
               SimUtilities::cycleThroughRs2DisplayTypes();
               SimUtilities::sendProcedureData(SimUtilities::RINSEBACK, SimUtilities::DRBC);
               SimUtilities::UnitSystem::toggle();
               SimUtilities::sendRunStats();
               break;

            case 2 :
               SimUtilities::UnitSystem::toggle();
               SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_RUNSUMM2);
               SimScreenCapture::screenshot(GUI_SCREEN_REBOOT_CONFIRM_BOX, "runsum2ConfirRestart");
               SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_REBOOT_CONFIRM_BOX);
               break;

            default :
               SimUtilities::setSystemState(STARTUP_TESTS);
               DataLog (log_level_sim_main_routine_info) << "Done with full run stuff." << endmsg;
               _actionQueue.nextRoutine();
               break;
         }

         break;

      default :
         DataLog (log_level_sim_main_routine_error) << "Unhandled run summary screen " << currentScreen << endmsg;
         break;
   }
}



void FullScreenshotRoutine::dealWithProcStatusProcessed ()
{
   if ( !SimUtilities::autoNavOn() ) return;   // Don't do anything if we're not taking screenshots.

   // Let's wait a second to make sure all processing has taken place.
   SimDelay::seconds(1);
   GUI_SCREEN_ID currentScreen = guiglobs::screen_status.get_current_screen();

   // Get the number of times this particular screen has been displayed.
   int scrnCount           = SimUtilities::screenCounter.getCount(currentScreen);
   int runsumResponseCount = SimUtilities::procStatusResponseCounter.count(currentScreen);

   const ProcedureStatus::ProcSnapshot& currentProcData = guiglobs::proc_status.get_data();
   const State_names                    currentState    = guiglobs::screen_status.get_current_system_state();

   DataLog (log_level_sim_main_routine_info) << "Proc status processed message recieved from screen "
                                             << currentScreen << "; scrnCount=" << scrnCount << endmsg;

   switch (scrnCount)
   {
      case 1 :

         if (currentState == BLOOD_PRIME)
         {
            if (currentProcData.estimated_platelets > 0.0)
            {
               SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcPrimePlt");
               SimUtilities::sendProcedureData(SimUtilities::PRIME, SimUtilities::DRBC);
            }
            else
            {
               SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcPrimeDrbc");
               SimUtilities::setSystemState(BLOOD_RUN);
               SimUtilities::sendProcedureData(SimUtilities::DRAW, SimUtilities::PLT | SimUtilities::PLAS);
            }
         }
         else if (currentState == BLOOD_RUN)
         {
            if (currentProcData.pressure < 0)
            {
               if (currentProcData.estimated_platelets > 0.0)
               {
                  SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcDrawPlt");
                  SimUtilities::sendProcedureData(SimUtilities::DRAW, SimUtilities::DRBC);
               }
               else
               {
                  SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcDrawDrbc");
                  SimUtilities::sendProcedureData(SimUtilities::RETURN, SimUtilities::PLT | SimUtilities::PLAS);
               }
            }
            else
            {
               if (currentProcData.estimated_platelets > 0.0)
               {
                  SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcReturnPlt");
                  SimUtilities::sendProcedureData(SimUtilities::RETURN, SimUtilities::DRBC);
               }
               else
               {
                  SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcReturnDrbc");
                  SimUtilities::setStatLine(textStatLrsPurge, DEFAULT_STATUS_LINE);

                  SimScreenCapture::screenshot("runProcLrsPurge");

                  SimUtilities::setStatLine(textStatRecoveryProcedure, DEFAULT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcRecovery");

                  SimUtilities::setStatLine(textStatAdjustingPltConc, DEFAULT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcAdjustingConc");

                  SimUtilities::setStatLine(textStatSalinePrime, DEFAULT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcSalinePrime");

                  SimUtilities::setStatLine(textStatUnscheduledPurge, DEFAULT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcUnscheduledPurge");

                  SimUtilities::setStatLine(textStatApsLow, ALERT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcApsLow");

                  SimUtilities::setStatLine(textStatAlarmPreWatchdogAlarm, ALERT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcWatchDogPrewarning");

                  SimUtilities::setStatLine(textStatAlarmCpsHighRbcCollect, ALERT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcCpsHighRbcCollect");

                  SimUtilities::setStatLine(textStatAlarmCpsHighAasP2Ok, ALERT_STATUS_LINE);
                  SimScreenCapture::screenshot("runProcCpsHighAasP2Ok");

                  SimUtilities::showAdjustScreen();
               }
            }
         }
         break;

      case 2 :
         if (currentState == BLOOD_RUN)
         {
            SimUtilities::setStatLine();
            SimUtilities::setSystemState(BLOOD_RINSEBACK);
            SimUtilities::sendProcedureData(SimUtilities::RINSEBACK, SimUtilities::PLT | SimUtilities::PLAS);
         }
         else if (BLOOD_RINSEBACK)
         {
            if (currentProcData.estimated_platelets > 0.0)
            {
               SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcRinsebackPlt");
               SimUtilities::sendProcedureData(SimUtilities::RINSEBACK, SimUtilities::DRBC);
            }
            else
            {
               SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcRinsebackDrbc");

               SimUtilities::setRunComplete();
               SimScreenCapture::screenshot(GUI_SCREEN_RUNPROC, "runProcComplete");

               SimUtilities::setSystemState(DONOR_DISCONNECT);
            }
         }
         break;
   }
}

void FullScreenshotRoutine::initAutoChange ()
{
   SimMsgSys msgSys;

   DataLog (log_level_sim_main_routine_info) << "Procedure init Autochange" << endmsg;

   // Turn on the screenshots.
   SimScreenCapture::autoNavOn(true);

   SimUtilities::init();

   SimScreenCapture::screenshot(GUI_SCREEN_RUNSUM_PLT, "test");
   SimDelay::seconds(1);
}

void FullScreenshotRoutine::getRunScreens ()
{
   SimMsgSys msgSys;

   DataLog (log_level_sim_main_routine_info) << "Procedure starting." << endmsg;

   // Turn on the screenshots.
   SimScreenCapture::autoNavOn(true);

   SimUtilities::init();

   // Make sure we're in the right state and both buttons
   // are displayed.
   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();
   SimDelay::milliseconds(250);

   SimUtilities::cycleThroughSysDonorDisplayTypes();

   DataLog (log_level_sim_main_routine_info) << "Swap stat bar" << endmsg;
   guiglobs::swapStatBar();
   SimDelay::seconds(1);

   SimScreenCapture::screenshot(GUI_SCREEN_SYSDONOR, "twoButtonAltStatBar");
   SimDelay::seconds(1);

   DataLog (log_level_sim_main_routine_info) << "Swap stat bar back" << endmsg;
   guiglobs::initStatBar();

   guiglobs::apheresis_status_bar->setState(STARTUP_TESTS);

   SimDelay::seconds(1);

   SimUtilities::changeScreens(GUI_SCREEN_SYSCASSETTE, "--Two-Button to SysCassette--");
}

void FullScreenshotRoutine::go ()
{
   startRoutineLoop(new FullScreenshotRoutine);
}

void FullScreenshotRoutine::snapCassetteScreen ()
{
   SimMsgSys msgSys;  // In case we want to use this from the command line.

   if (!SimUtilities::checkScreen(GUI_SCREEN_SYSCASSETTE))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   DataLog (log_level_sim_main_routine_info) << "Delete all sets from the short list" << endmsg;
   guiglobs::cassette_mgr->DeleteAllCassettes();
   // needs extra removal of 'attention status'
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   SimUtilities::setStatLine(textStatSetIdInstructions);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "refSelectStart");

   DataLog (log_level_sim_main_routine_info) << "Get all the status lines for set selection" << endmsg;

   SimUtilities::setStatLine(textStatSetIdGoodCode);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "goodRef");

   SimUtilities::setStatLine(textStatCfgMachBadCatNum);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "badRef");

   SimUtilities::setStatLine(textStatSetIdChosenProcBad);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "refMismatch");

   SimUtilities::setStatLine(textStatSetIdBadCode);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "invalidRef");

   SimUtilities::setStatLine(textStatCfgMachBadSetCode);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSCASSETTE, "badSetCode");

   DataLog (log_level_sim_main_routine_info) << "Add the cassettes back in" << endmsg;
   SimUtilities::loadRefCodes();                                                                                               SimDelay::seconds(1);
}

void FullScreenshotRoutine::snapAcConnectScreen ()
{
   SimMsgSys msgSys;  // In case we want to use this from the command line.

   if (!SimUtilities::checkScreen(GUI_SCREEN_SYSACATT))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   // Turn on the AC confirm screen feature
   SimUtilities::sendTraverseMsg(SET_SHOW_AC_ATTACH_CONFIRM, 1);

   SimScreenCapture::screenshot(GUI_SCREEN_SYSACATT, "acConnect");


   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_SYSACATT);
   SimDelay::seconds(3);
   if (guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_AC_CONNECT_CONFIRM_BOX)
   {
      SimScreenCapture::screenshot(GUI_SCREEN_AC_CONNECT_CONFIRM_BOX, "acConnectConfirm");
      SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_AC_CONNECT_CONFIRM_BOX);
   }

   // show the luer connection
   SimUtilities::sendTraverseMsg(SET_SHOW_AC_ATTACH_CONFIRM, 2);
   SimScreenCapture::screenshot(GUI_SCREEN_SYSACATT, "acConnectLuer");

   // Turn on the AC confirm screen feature
   SimUtilities::sendTraverseMsg(SET_SHOW_AC_ATTACH_CONFIRM, 0);
}


void FullScreenshotRoutine::snapDonInfo ()
{
   if (!SimUtilities::checkScreen(GUI_SCREEN_DONINFO))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   // bool bUnitsHemoglobin = guiglobs::gui_donor_data_repository.RbcUnitsHgb();

   bool bUnitHCT = guiglobs::gui_donor_data_repository.RbcUnitsHct();
   SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoInitial");
   SimScreenTouch::touch(GUI_BUTTON_DONOR_BLOOD_TYPE, GUI_SCREEN_DONINFO);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donInfoBloodTypeKeypad");
   SimScreenTouch::keypadSequence("0e");
   SimDelay::seconds(3);
   SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoBloodType");

   SimScreenTouch::touch(GUI_BUTTON_DONOR_HEMATOCRIT, GUI_SCREEN_DONINFO);
   if ( true == bUnitHCT )
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donInfoHct");
      SimScreenTouch::keypadSequence("38e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoHct");
   }
   else
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donInfoHgb");
      SimScreenTouch::keypadSequence("150e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoHgb");
   }

   SimScreenTouch::touch(GUI_BUTTON_DONOR_PLATELET_PRECOUNT, GUI_SCREEN_DONINFO);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donInfoPrecountKeypad");
   SimScreenTouch::keypadSequence("250e");
   SimDelay::seconds(3);
   SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoPrecount");

   // "optimization in progress" message too fast for a snapshot.
   // Let's fake it here
   SimUtilities::setStatLine(textStatOptInProgress);
   SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoOpt");
   SimUtilities::setStatLine();  // Resetting status line
}

void FullScreenshotRoutine::snapDonVitals ()
{
   if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }
   bool bUnitsWeightLb       = guiglobs::gui_donor_data_repository.IsWeightInLb();
   bool bUnitsHeightInInches = guiglobs::gui_donor_data_repository.IsHeightInIn();
   //this is already generated in passDonVitals
   //SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, "donVitalsInitial");
   SimScreenTouch::touch(GUI_BUTTON_DONOR_GENDER, GUI_SCREEN_DONVITAL);
   SimDelay::seconds(7);

   SimScreenTouch::touch(GUI_BUTTON_DONOR_HEIGHT, GUI_SCREEN_DONVITAL);
   if ( true == bUnitsHeightInInches )
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donVitalsFtKeypad");
      SimScreenTouch::keypadSequence("60e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, "donVitalsFt");
   }
   else
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donVitalsCmKeypad");
      SimScreenTouch::keypadSequence("180e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, "donVitalsCm");
   }

   SimScreenTouch::touch(GUI_BUTTON_DONOR_WEIGHT, GUI_SCREEN_DONVITAL);
   if ( true == bUnitsWeightLb )
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donVitalsLbsKeypad");
      SimScreenTouch::keypadSequence("250e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, "donVitalsLb");
   }
   else
   {
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "donVitalsKgKeypad");
      SimScreenTouch::keypadSequence("120e");
      SimDelay::seconds(3);
      SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, "donVitalsKg");
   }
}


void FullScreenshotRoutine::snapPredict ()
{
   SimDelay::seconds(10);  // Predict needs a good long time to chew on this.

   if (!SimUtilities::checkScreen(GUI_SCREEN_PREDICT))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   DataLog (log_level_sim_main_routine_info) << "snapPredict(): iteration=" << SimUtilities::screenCounter.getCount(GUI_SCREEN_PREDICT) << endmsg;

   if (SimUtilities::UnitSystem::units() == SimUtilities::UnitSystem::UNITS_ENGLISH)
   {
      DataLog (log_level_sim_main_routine_info) << "Predict units are ENGLISH" << endmsg;

      SimUtilities::sendTraverseMsg(SET_SHOW_PLT_VOLUME, false);

      SimDelay::seconds(5);   // Predict needs a good long time to chew on this.

      // Set the flow adjust features
      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN, "predictInitial");

      SimScreenTouch::touch(GUI_BUTTON_PRED_DONOR_TIME, GUI_SCREEN_PREDICT);
      SimDelay::seconds(5);   // Wait for keypad to appear

      SimScreenTouch::keypadSequence("1e");
      SimDelay::seconds(5);
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN, "predictTimeRange");

      SimScreenTouch::keypadSequence("10e");
      SimDelay::seconds(5);
      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN, "predictNoProduct");

      SimScreenTouch::touch(GUI_BUTTON_PRED_DONOR_TIME, GUI_SCREEN_PREDICT);
      SimDelay::seconds(5);   // Wait for keypad to appear

      SimScreenTouch::keypadSequence("120");
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN, "predictTimeKeypad");
      SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
      SimDelay::seconds(10);
      // Make sure we choose a procedure with platelets, plasma and rbc.

      {
         const ProductOptionStruct& current = guiglobs::cdsData.procedures.getProcedure(CURR);
         const ProductOptionStruct& next    = guiglobs::cdsData.procedures.getProcedure(NEXT);
         int counter = 15;

         while (((current.estimated_plasma == 0.0)  ||  (current.estimated_platelet_volume == 0.0)  ||  (current.estimated_rbcs == 0.0))
                && ((next.estimated_plasma >  0.0)  ||     (next.estimated_platelet_volume >  0.0)  ||     (next.estimated_rbcs >  0.0))
                && (--counter > 0))
         // while ((we don't like the current procedure)  &&  (there are more procedures to check))
         {
            SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
         }
      }
      // SimUtilities::toggleAmapPlatelet();
      SimDelay::seconds(10);
   }
   else
   {
      DataLog (log_level_sim_main_routine_info) << "Predict units are METRIC" << endmsg;

      SimUtilities::sendTraverseMsg(SET_SHOW_FLOW_ADJUST, true);
      SimDelay::seconds(2);

      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_JAPAN_ONLY, "predictSelectMetric");

      bool enabled = !Software_CDS::GetInstance().getFeature(AllowAdjustFlowRatesOnProcedureSelect);
      Software_CDS::GetInstance().SetFeature(AllowAdjustFlowRatesOnProcedureSelect, enabled, AUTH_R_N_D, true);
      SimDelay::seconds(2);
      SimScreenTouch::touch(GUI_BUTTON_PRED_FLOW_RATE, GUI_SCREEN_PREDICT);
      SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, SHOT_OPTS_LBL_JAPAN_ONLY, "predictAdjustFlow");
      SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
      Software_CDS::GetInstance().SetFeature(AllowAdjustFlowRatesOnProcedureSelect, !enabled, AUTH_R_N_D, true);
      SimDelay::seconds(2);
      SimUtilities::sendTraverseMsg(SET_SHOW_FLOW_ADJUST, false);
      SimDelay::seconds(2);

      SimUtilities::sendTraverseMsg(SET_SHOW_PLT_VOLUME, true);
      SimDelay::seconds(2);
      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_JAPAN_ONLY, "predictShowPlatelet");

      SimUtilities::sendTraverseMsg(SET_SHOW_PLT_VOLUME, false);
      SimDelay::seconds(2);

      SimUtilities::sendTraverseMsg(SET_SHOW_PLASMA, true);
      SimDelay::seconds(2);
      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_JAPAN_ONLY, "predictShowPlasma");

      SimUtilities::sendTraverseMsg(SET_SHOW_PLASMA, false);
      SimDelay::seconds(2);

      SimUtilities::toggleAmapPlatelet();
      SimUtilities::sendTraverseMsg(SET_SHOW_AMAP_PLT, true);
      SimDelay::seconds(2);
      SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_JAPAN_ONLY, "predictShowPlateletAmap");

      SimUtilities::sendTraverseMsg(SET_SHOW_AMAP_PLT, false);
      SimDelay::seconds(2);

      SimUtilities::sendTraverseMsg(SET_SHOW_FLOW_ADJUST, true);
      SimDelay::seconds(2);

      SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_PREDICT);
      SimDelay::seconds(2);
	  
      if ( guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_PREDICT_CONFIRM_BOX)
	  {
		  SimScreenCapture::screenshot(GUI_SCREEN_PREDICT_CONFIRM_BOX, SHOT_OPTS_LBL_JAPAN_ONLY, "predictConfirm");
		  SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_PREDICT_CONFIRM_BOX);
	  }

      SimUtilities::UnitSystem::toggle();
      SimUtilities::toggleAmapPlatelet();
   }

   DataLog (log_level_sim_main_routine_info) << "Done snapping predict iteration=" << SimUtilities::screenCounter.getCount(GUI_SCREEN_PREDICT) << endmsg;
}

void FullScreenshotRoutine::sysDonorNav ()
{
   if (!SimUtilities::checkScreen(GUI_SCREEN_SYSDONOR))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   switch (SimUtilities::screenCounter.getCount(GUI_SCREEN_SYSDONOR))
   {
      case 1 :
         SimUtilities::changeScreens(GUI_SCREEN_DONVITAL, "--Two-Button to DonVitals A --");
         break;

      case 2 :
         SimUtilities::UnitSystem::toggle();
         SimUtilities::changeScreens(GUI_SCREEN_DONVITAL, "--Two-Button to DonVitals B --");
         break;

      case 3 :
         SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_SYSDONOR);
         break;

      default :
         DataLog (log_level_sim_main_routine_error) << "Unhandled screen count." << endmsg;
         break;
   }
}

void FullScreenshotRoutine::goFullRoutine ()
{
   SimMsgSys msgSys;
   SimConfig::configureIgnoredStrings();
   SimScreenCapture::useCategory();
   SimConfig::resetSoftwareConfiguration();
   go();
}

void FullScreenshotRoutine::configCompPlsOptionPredictEffect ()
{
   SimUtilities::autoNavOn(false);

   SimUtilities::setSystemState(DISPOSABLE_TEST);

   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();


   SimConfig::gotoConfig();
   //SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::setCompPlasma();
   //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   /*
   Software_CDS& sw = Software_CDS::getRWCds();
   sw.SetFeature(ComplementaryPlasmaAvailable, false, true);
   */
   // turn on pas/ split
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);

   // Set up initial values and take a picture
   // Turn Split Notification on.
   SimUtilities::toggleSplitNotif(true);
   // Turn off PLT MSS.
   SimUtilities::toggleMasterPas(true);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_COMPLEMENTARY_PLASMA, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

   // platelet product
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLATELET, GUI_SCREEN_CONFIG);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_1, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_2, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_3, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLATELET);

   SimConfig::exitConfig();

   SimScreenTouch::touch(GUI_BUTTON_DONORINFO, GUI_SCREEN_SYSDONOR);
   SimDelay::seconds(2);

   if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL)) return;

   passDonVitals();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONVITAL);
   passDonorInfo();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONINFO);
   passPredictCompPlasma();

   SimUtilities::setSystemState(DISPOSABLE_TEST);
   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();

   SimConfig::gotoConfig();

   // RESET platelet product
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLATELET, GUI_SCREEN_CONFIG);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_1, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_2, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_3, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLATELET);

   //SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::resetSoftwareConfiguration();
   //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   // turn off pas/ split
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);
   // Set up initial values and take a picture
   // Turn Split Notification on.
   SimUtilities::toggleSplitNotif(false);
   // Turn off PLT MSS.
   SimUtilities::toggleMasterPas(false);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

   SimConfig::exitConfig();
}

void FullScreenshotRoutine::configAmapPltOptionPredictEffect ()
{
   SimUtilities::autoNavOn(false);

   SimUtilities::setSystemState(DISPOSABLE_TEST);

   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();

   SimConfig::setAmapPlatelets();
   /*
   SimConfig::gotoConfig();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::setAmapPlatelets();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimConfig::exitConfig();
   */

   SimScreenTouch::touch(GUI_BUTTON_DONORINFO, GUI_SCREEN_SYSDONOR);
   SimDelay::seconds(2);

   if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL)) return;

   passDonVitals();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONVITAL);
   passDonorInfo();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONINFO);
   passPredictAmapPlt();
   SimUtilities::setSystemState(DISPOSABLE_TEST);
   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();

   SimConfig::resetSoftwareConfiguration();
   /*
   SimConfig::gotoConfig();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::resetSoftwareConfiguration();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimConfig::exitConfig();
   */
}

void FullScreenshotRoutine::configShowPlasmaPredictEffect ()
{
   SimUtilities::autoNavOn(false);

   SimUtilities::setSystemState(DISPOSABLE_TEST);

   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();

   SimConfig::setShowPlasmaVolume();
	/*
   SimConfig::gotoConfig();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::setShowPlasmaVolume();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimConfig::exitConfig();
   */

   SimScreenTouch::touch(GUI_BUTTON_DONORINFO, GUI_SCREEN_SYSDONOR);
   SimDelay::seconds(2);

   if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL)) return;

   passDonVitals();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONVITAL);
   passDonorInfo();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONINFO);
   passPredictShowPlasma();
   SimUtilities::setSystemState(DISPOSABLE_TEST);
   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();

   SimConfig::resetSoftwareConfiguration();
   /*
   SimConfig::gotoConfig();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimConfig::resetSoftwareConfiguration();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimConfig::exitConfig();
   */
}


void FullScreenshotRoutine::passDonVitals ()
{
   SimDelay::seconds(2);
   if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   static bool firstPass = true;
   if ( firstPass )
   {
      SimScreenCapture::screenshot(GUI_SCREEN_DONVITAL, SHOT_OPTS_NO_PRUNING, "donVitalsInitial");
      firstPass = false;
   }

   bool bUnitsWeightLb       = guiglobs::gui_donor_data_repository.IsWeightInLb();
   bool bUnitsHeightInInches = guiglobs::gui_donor_data_repository.IsHeightInIn();
   SimScreenTouch::touch(GUI_BUTTON_DONOR_GENDER, GUI_SCREEN_DONVITAL);

   SimScreenTouch::touch(GUI_BUTTON_DONOR_HEIGHT, GUI_SCREEN_DONVITAL);
   if ( true == bUnitsHeightInInches )
      SimScreenTouch::keypadSequence("60e");
   else
      SimScreenTouch::keypadSequence("180e");

   SimScreenTouch::touch(GUI_BUTTON_DONOR_WEIGHT, GUI_SCREEN_DONVITAL);
   if ( true == bUnitsWeightLb )
      SimScreenTouch::keypadSequence("250e");
   else
      SimScreenTouch::keypadSequence("120e");
}

void FullScreenshotRoutine::passDonorInfo ()
{
   SimDelay::seconds(2);
   if (!SimUtilities::checkScreen(GUI_SCREEN_DONINFO))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   // bool bUnitsHemoglobin = guiglobs::gui_donor_data_repository.RbcUnitsHgb();

   bool bUnitHCT = guiglobs::gui_donor_data_repository.RbcUnitsHct();
   SimScreenTouch::touch(GUI_BUTTON_DONOR_BLOOD_TYPE, GUI_SCREEN_DONINFO);
   SimScreenTouch::keypadSequence("0e");

   SimScreenTouch::touch(GUI_BUTTON_DONOR_HEMATOCRIT, GUI_SCREEN_DONINFO);
   if ( true == bUnitHCT )
      SimScreenTouch::keypadSequence("45e");
   else
      SimScreenTouch::keypadSequence("150e");

   SimScreenTouch::touch(GUI_BUTTON_DONOR_PLATELET_PRECOUNT, GUI_SCREEN_DONINFO);
   SimScreenTouch::keypadSequence("400e");

   // "optimization in progress" message too fast for a snapshot.
   // Let's fake it here
   SimUtilities::setStatLine(textStatOptInProgress);
   // SimScreenCapture::screenshot(GUI_SCREEN_DONINFO, "donInfoOpt");
   SimUtilities::setStatLine();  // Resetting status line
}

void FullScreenshotRoutine::passPredictAmapPlt ()
{
   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   if (!SimUtilities::checkScreen(GUI_SCREEN_PREDICT))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   DataLog (log_level_sim_main_routine_info) << "snapPredict(): iteration=" << SimUtilities::screenCounter.getCount(GUI_SCREEN_PREDICT) << endmsg;

   DataLog (log_level_sim_main_routine_info) << "Predict units are ENGLISH" << endmsg;

   SimUtilities::sendTraverseMsg(UNIVERSAL);

   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   // Set the flow adjust features
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN, "predictPlateletAmap1");

   /*
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictPlateletAmap2");
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictPlateletAmap3");
   */
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_PREDICT);

}

void FullScreenshotRoutine::passPredictCompPlasma ()
{
   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   if (!SimUtilities::checkScreen(GUI_SCREEN_PREDICT))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }
   SimUtilities::sendTraverseMsg(UNIVERSAL);
   // Set the flow adjust features
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictCompPlasma1");

   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictCompPlasma2");

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_PREDICT);

   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX, SHOT_OPTS_NO_PRUNING, "predictCompPlasma3");
}

void FullScreenshotRoutine::passPredictShowPlasma ()
{
   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   if (!SimUtilities::checkScreen(GUI_SCREEN_PREDICT))
   {
      DataLog (log_level_sim_main_routine_error) << "Incorrect screen." << endmsg;
      return;
   }

   DataLog (log_level_sim_main_routine_info) << "snapPredict(): iteration=" << SimUtilities::screenCounter.getCount(GUI_SCREEN_PREDICT) << endmsg;

   DataLog (log_level_sim_main_routine_info) << "Predict units are ENGLISH" << endmsg;

   SimUtilities::sendTraverseMsg(UNIVERSAL);

   SimDelay::seconds(2);  // Predict needs a good long time to chew on this.

   // Set the flow adjust features
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, "predictShowPlasma1");


   /*
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PREDICT_MORE_OR_LESS, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictShowPlasma2");
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);
   SimScreenCapture::screenshot(GUI_SCREEN_PREDICT, SHOT_OPTS_NO_PRUNING, "predictShowPlasma2");
   */
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_PREDICT);

}


void FullScreenshotRoutine::walkThroughCompPlasmaRunSum ()
{
   SimUtilities::autoNavOn(false);
   SimMsgSys tmpMsgSys;
   SimUtilities::setSystemState(POST_RUN);
   SimUtilities::changeScreens(GUI_SCREEN_AAS_RUN_SUMMARY, "Run summary");
   // SimScreenCapture::screenshot( GUI_SCREEN_AAS_RUN_SUMMARY, "runSumAAsCompPlasma");
   SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM1, "Run summary");
   // SimScreenCapture::screenshot( GUI_SCREEN_RUNSUMM1, "runSum1CompPlasma");
   SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_PLT, "Run summary");
   // SimScreenCapture::screenshot( GUI_SCREEN_RUNSUM_PLT, "runSumPltCompPlasma");
   SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_PLASMA, "Run summary");
   // SimScreenCapture::screenshot( GUI_SCREEN_RUNSUM_PLASMA, "runSumPlsCompPlasma");
   SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM2, "Run summary");
   RS2_DISPLAY_TYPE type = RS2_DRBC;
   SimUtilities::sendTraverseMsg(UNIVERSAL, type, 0xC03e);
   SimScreenCapture::screenshot(GUI_SCREEN_RUNSUMM2, "runSum2CompPlasmaDRBC");
   type = RS2_SRBC;

   SimUtilities::sendProcedureData(SimUtilities::RINSEBACK, SimUtilities::DRBC);
   SimUtilities::sendTraverseMsg(UNIVERSAL, type, 0xC03);
   SimScreenCapture::screenshot(GUI_SCREEN_RUNSUMM2, "runSum2CompPlasmaSRBC");
   SimUtilities::setSystemState(STARTUP_TESTS);
   SimUtilities::enableLoadBtn();
}

void FullScreenshotRoutine::initConcatStrings()
{
	/*keypads*/
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsLbs);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsKg);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsInches);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsHemoglobin);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsFlow);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsFeet);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsCm);
   string_screen_mapper::AddConcatMap(textEmptyFormatKeypadUsrEntryInit,textKeypadUnitsTime);

   /*donorInfo*/
   string_screen_mapper::AddConcatMap(textButtonGiantDonorInfoCrit,textMiscUnitsDonorInfoHemoglobin);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorInfoCrit,textMiscUnitsDonorInfoHematocrit);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorInfoPltPrecount,textMiscUnitsDonorInfoPltPrecount);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlHeightInit,textDataUnitsDonDlCm);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlHeightInit,textDataUnitsDonDlFeet);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlHeightInit,textDataUnitsDonDlInches);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlHctInit,textDataUnitsDonDlHemoglobin);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlHctInit,textDataUnitsDonDlHct);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlWeightInit,textDataUnitsDonDlKg);
   string_screen_mapper::AddConcatMap(textEmptyFormatDonDlWeightInit,textDataUnitsDonDlLbs);
   string_screen_mapper::AddConcatMap(textDataInitDonDlTbv,textDataUnitsDonDlTbv);
   
   /*donor vitals*/
   string_screen_mapper::AddConcatMap(textMiscInitDonorVitalsTbvSet,textMiscUnitsDonorVitalsTbv);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorVitalsWeightLbSet,textMiscUnitsDonorVitalsWeightLbs);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorVitalsHeightFtSet,textMiscUnitsDonorVitalsHeightFeet);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorVitalsHeightFtSet,textMiscUnitsDonorVitalsHeightInches);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorVitalsHeightCmSet,textMiscUnitsDonorVitalsHeightCm);
   string_screen_mapper::AddConcatMap(textButtonGiantDonorVitalsWeightKgSet,textMiscUnitsDonorVitalsWeightKg);
   
   string_screen_mapper::AddConcatMap(textMiscInitDonorVitalsTbvSet,textMiscUnitsDonorVitalsTbv);
 
   /* runproc */
   string_screen_mapper::AddConcatMap(textEmptyFormatRunDonationTimeLeftInit,textDataUnitsRunProcTime);
   string_screen_mapper::AddConcatMap(textEmptyFormatRunDonationTimeLeftInit,textDataUnitsRunProcTimeAm);
   string_screen_mapper::AddConcatMap(textEmptyFormatRunDonationTimeLeftInit,textDataUnitsRunProcTimePm);

   /* predict */
   string_screen_mapper::AddConcatMap(textEmptyFormatPredCurrPltVolLabelInit,textDataUnitsMl);
   string_screen_mapper::AddConcatMap(textEmptyFormatPredCurrTimeLabelInit,textDataUnitsPredDuration);
   string_screen_mapper::AddConcatMap(textEmptyFormatPredCurrFlowRateLabelInit,textDataUnitsPredFlowRate);

   /* trouble screen*/
   string_screen_mapper::AddConcatMap(textMiscAdjustTimeInit,textDataUnitsAdjustTime);

   /* runsum */
   string_screen_mapper::AddConcatMap(textEmptyFormatRs1DatAvgInletFlow,textDataUnitsRs1AvgInletFlowAlt);
}


#endif /* CPU == SIMNT */

/* FORMAT HASH a45631c533a6a80bd8af56899ec1e38b */
