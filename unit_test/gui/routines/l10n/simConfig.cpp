/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simConfig.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *******************************************************************************/

#include <vxWorks.h>
#include <math.h>

#include "simConfig.hpp"

// From common
#include "trima_datalog.h"

// From GUI
#include "gui.h"
#include "guiglobs.hpp"
#include "statline.hpp"

// From unit_test
#include "simScreenCapture.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "scrncomm.hpp"
#include "simActionQueue.hpp"
#include "simUtilities.hpp"
#include "simMsgSys.hpp"
#include "set_listbox.hpp"
#include "screen_config.hpp"
#include "string_screen_mapper.hpp"

#include "software_cds.h"

#define PLT_BUTTON_YIELD(row) ((row==0) ? (GUI_BUTTON_CONFIG_PLT_YIELD_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PLT_YIELD_2) : (GUI_BUTTON_CONFIG_PLT_YIELD_3)))
#define PP_BUTTON_TIME(row) ((row==0) ? (GUI_BUTTON_CONFIG_PP_TIME_1) : ((row==1) ? (GUI_BUTTON_CONFIG_PP_TIME_2) : (GUI_BUTTON_CONFIG_PP_TIME_3)))

void SimConfig::utilWalkThrough ()
{
   SimMsgSys msg;
   resetSoftwareConfiguration();
   walkThroughConfig();
}

void SimConfig::walkThroughConfig ()
{
   SimDelay::waitForScreen(GUI_SCREEN_SYSDONOR);
   if (!SimUtilities::checkScreen(GUI_SCREEN_SYSDONOR)) return;

   SimScreenCapture::autoNavOn(false);

   SimScreenTouch::touch(GUI_BUTTON_LOWLEFT, GUI_SCREEN_SYSDONOR);

   // Enter Password, then take a picture
   SimScreenTouch::keypadSequence("789852e");

   guiglobs::apheresis_status_line->deactivate_status_line_type(END_RUN_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
   SimUtilities::setStatLine(textCfgScreenLabelMainMenu, DEFAULT_STATUS_LINE);
   // this is not supposed to be pruned, forcing to capture
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG, "cfgMain");

   walkThroughConfigAutomatedProcessesPlatelets();
   walkThroughConfigMachine();
   walkThroughConfigLanguageUnit();
   walkThroughConfigProcedure();
   walkThroughConfigPlasma();
   walkThroughConfigRedBloodCells();
   walkThroughConfigDonor();
   walkThroughConfigDonorVolumeLimits();
   walkThroughConfigReplacementFluid();
   walkThroughConfigProcedurePriority();
   walkThroughConfigSoftwareOptions();
   walkThroughConfigAmapPlatelets();
   // walkThroughConfigProcedurePriorityAmapPlt();
   
   //walkThroughAirOutMitigation();

   SimUtilities::setStatLine(textStatConfigExitWait);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG, "cfgExiting");
   SimUtilities::setStatLine();

   // Don't actually hit the continue button so our changes don't take effect.
   SimUtilities::setSystemState(STARTUP_TESTS);
}

void SimConfig::walkThroughConfigAutomatedProcessesPlatelets ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);

   // Set up initial values and take a picture
   // Turn Split Notification on.
   SimUtilities::toggleSplitNotif(true);
   // Turn off PLT MSS.
   SimUtilities::toggleMasterPas(false);

   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "cfgAutoProcPasOff");

   // Exit Mss Config Screen, Go to Platelet Config Screen, and take a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLATELET, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PLATELET, "cfgPltPasOff");

   // Exit Platelet Config Screen and go back into the MSS Config Screen
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);

   // Turn on MSS and take a picture.
   SimUtilities::toggleMasterPas(true);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "cfgAutoProcPasOn");

   // Exit Mss Config Screen and go back to the Platelet Config Screen
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLATELET, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PLATELET, "cfgPltPasOn");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_1, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PLATELET, "cfgPltProd1PasOff");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_PROD_1, GUI_SCREEN_CONFIG_PLATELET);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_YSF, GUI_SCREEN_CONFIG_PLATELET);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgPltScalingFactor");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MALE_ONLY_PLT, GUI_SCREEN_CONFIG_PLATELET);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgPltDonorsAllowed");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_3, GUI_SCREEN_LISTBOX);

   for (unsigned int buttonIndex = 0, step = 3, limit = NUM_YIELD_VOLUME; buttonIndex < limit; )
   {
      unsigned int blockNum = int(std::ceil(float(buttonIndex) / step));
      SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PLATELET, "cfgPltSet%d", blockNum);

      for (unsigned int rowNum = step - Min(step, limit - blockNum * step); rowNum < step; ++rowNum, ++buttonIndex)
      {
         SimScreenTouch::touch(PLT_BUTTON_YIELD(rowNum), GUI_SCREEN_CONFIG_PLATELET);

         SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgPltIndicator%d", buttonIndex + 1);
         SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
      }

      for (unsigned int scrollDownCount = 0; scrollDownCount < step && scrollDownCount < (limit - blockNum * step); ++scrollDownCount)
      {
         SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_PLATELET);
      }
   }

   SimUtilities::setStatLine(textStatCfgPltConcRange);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PLATELET, "cfgPltConcRange");
   SimUtilities::setStatLine();

   // Exit Platelet Screen
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLATELET);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   setCompPlasma();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "cfgPltPasOnCompPlasma");
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

   // reset configuration
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   resetSoftwareConfiguration();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

}

void SimConfig::walkThroughConfigMachine ()
{
   SimDelay::waitForScreen(GUI_SCREEN_CONFIG);
   if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG)) return;

   SimUtilities::loadRefCodes();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMach");

   Base_Window* cfgMachScrnPtr = guiglobs::scrnComm->getScreenInstancePointer(GUI_SCREEN_CONFIG);

   // Bringing up the date format dropdown and taking a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DATE_FORMAT, GUI_SCREEN_CONFIG_MACHINE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgMachDateFormat");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   // Bringing up set date calc and taking a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_SET_DATE, GUI_SCREEN_CONFIG_MACHINE);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgMachSetDate");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   // Bringing up time format dropdown and taking a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_TIME_FORMAT, GUI_SCREEN_CONFIG_MACHINE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgMachTimeFormat");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   // Bringing up set time calc and taking a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_SET_TIME, GUI_SCREEN_CONFIG_MACHINE);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgMachSetTime");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   // Bring up audio select dropdown and take a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_SET_AUDIO, GUI_SCREEN_CONFIG_MACHINE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgMachSetAudio");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   // Bring up REF number calc and take picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_SET_ID, GUI_SCREEN_CONFIG_MACHINE);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgMachSetIdCalc");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   // Bring up tubing set code calc and take a picture.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_SET_CODE, GUI_SCREEN_CONFIG_MACHINE);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgMachSetCodeCalc");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   // Bring up "delete one" box.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_REMOVE_SET, GUI_SCREEN_CONFIG_MACHINE);
   SimScreenTouch::touch(GUI_BUTTON_CASSETTE_LISTBOX_2, GUI_SCREEN_SET_LISTBOX);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIRM_BOX, "cfgMachConfirmDeleteOneSet");
   SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_CONFIRM_BOX);

   // Bring up "delete all" box.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MACHINE_REMOVE_SET, GUI_SCREEN_CONFIG_MACHINE);
   SimScreenCapture::screenshot(GUI_SCREEN_SET_LISTBOX, "cfgMachRemoveSetListbox");

   {
      bool doThisTheHardWay = false;

      if (guiglobs::scrnComm)
      {
         Base_Window* setListboxPointer = guiglobs::scrnComm->getCurrentChildScreen(GUI_SCREEN_SET_LISTBOX);

         if (setListboxPointer)
         {
            int numTimes = 0;  // Make sure we don't do this more than 10 times.
            while ((!((Screen_SET_LISTBOX*)setListboxPointer)->isBottomOfList()) && numTimes++ < 10)
               SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_SET_LISTBOX);
         }
         else
         {
            DataLog (log_level_gui_error) << "Can't get a good set listbox pointer." << endmsg;
            doThisTheHardWay = true;
         }
      }
      else
      {
         DataLog (log_level_gui_error) << "Screen commutator pointer is bogus." << endmsg;
         doThisTheHardWay = true;
      }

      if (doThisTheHardWay)
      {
         DataLog (log_level_gui_error) << "We're going to have to do this the hard way, I guess." << endmsg;

         for (int i = 0; i<7; ++i)
            SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_SET_LISTBOX);
      }
   }

   SimScreenCapture::screenshot(GUI_SCREEN_SET_LISTBOX, SHOT_OPTS_NO_PRUNING, "cfgMachRemoveSetListbox2");
   SimScreenTouch::touch(GUI_BUTTON_CASSETTE_LISTBOX_5, GUI_SCREEN_SET_LISTBOX);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIRM_BOX, "cfgMachConfirmDeleteAll");
   SimScreenTouch::touch(GUI_BUTTON_CONFIRM_BOX_CANCEL, GUI_SCREEN_CONFIRM_BOX);

   SimUtilities::setStatLine(textStatMaxEntriesExceeded);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachMaxEntriesExceeded");

   SimUtilities::setStatLine(textStatCfgMachBadPair);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachInvalidEntry");

   SimUtilities::setStatLine(textStatCfgMachSuccess);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachSuccess");

   SimUtilities::setStatLine(textStatCfgMachBadBarcode);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachInvalidEntry");

   SimUtilities::setStatLine(textStatCfgMachBadCatNum);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachBadCatNum");

   SimUtilities::setStatLine(textStatCfgMachBadSetCode);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_MACHINE, "cfgMachBadSetCode");

   SimUtilities::setStatLine();

   // Exit out.
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_MACHINE);
}

void SimConfig::walkThroughConfigLanguageUnit ()
{
   // ENTER LANGUAGE / UNIT SCREEN
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LANGUAGE, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_LANGUAGE, "cfgLangUnit");

   // HEIGHT
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_HEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgLangUnitHeight");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   // WEIGHT
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_WEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgLangUnitWeight");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   // DECIMAL DELIMITER
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DECIMAL_DELIMITER, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgLangUnitDelim");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_LISTBOX);

   // Hematocrit or Hemoglobin Units
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_CRIT_OR_GLOB, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgLangUnitRbcs");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_LISTBOX);

   // Note that if you uncomment out the language listbox stuff up
   // above, you'll need to get rid of the following line, because we'll
   // have left this screen automatically.
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_LANGUAGE);
}

void SimConfig::walkThroughConfigProcedure ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PROCEDURE, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PROCEDURE, "cfgProc");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PROCEDURE_TIME, GUI_SCREEN_CONFIG_PROCEDURE);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgProcMaxTime");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RETURN_PRESSURE, GUI_SCREEN_CONFIG_PROCEDURE);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgProcRetPressLim");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DRAW_PRESSURE, GUI_SCREEN_CONFIG_PROCEDURE);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgProcDrawPressLim");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_CUSTOM_RATIO, GUI_SCREEN_CONFIG_PROCEDURE);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgProcInletAcRatio");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_RATIO, GUI_SCREEN_CONFIG_PROCEDURE);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgProcInletAcRatioRbc");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);

   //SimScreenTouch::touch(GUI_BUTTON_CONFIG_AUDIT_TRACKING, GUI_SCREEN_CONFIG_PROCEDURE);
   //SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgProcPlasmaOnlyACRatio");
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PROCEDURE);
}

void SimConfig::walkThroughConfigPlasma ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLASMA, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot("cfgplasma_main");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLS_VOLUME_1, GUI_SCREEN_CONFIG_PLASMA);

   SimScreenCapture::screenshot("cfgplasma_volume");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLS_AMAP_MIN, GUI_SCREEN_CONFIG_PLASMA);

   SimScreenCapture::screenshot("cfgplasma_amap_minimum");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLS_AMAP_MAX, GUI_SCREEN_CONFIG_PLASMA);

   SimScreenCapture::screenshot("cfgplasma_amap_maximum");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MALE_ONLY_PLASMA, GUI_SCREEN_CONFIG_PLASMA);
   SimScreenCapture::screenshot("cfgplasma_donors_allowed");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_3, GUI_SCREEN_LISTBOX);

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLASMA);
}

void SimConfig::walkThroughConfigRedBloodCells ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);
   SimUtilities::toggleMasterRas(false);

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_RBC, "cfgRbcRasOff");

   // Toggle the alert to the Off setting.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DRBC_ALERT, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_RBC, "cfgRbcDrbcOff");

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_RBC);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);
   SimUtilities::toggleMasterRas(true);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC, GUI_SCREEN_CONFIG);
   // # DRBC SPLIT NOTIFICATION
   // Now bring up the calculator.
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DRBC_ALERT, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcDrbcCalc");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_1, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator1");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_2, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator2");

   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_3, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator3");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   //down 3 products GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_RBC);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_RBC);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, "cfgRbcNextThreeProducts");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_1, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator4");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_2, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator5");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_DOSE_3, GUI_SCREEN_CONFIG_RBC);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgRbcIndicator6");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);

   SimUtilities::setStatLine(textStatDoseRangeError);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_RBC, "cfgRbcDoseRangeError");

   SimUtilities::setStatLine();

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_RBC);
}

void SimConfig::walkThroughConfigDonor ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_DONOR, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_DONOR, "cfgDonor");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_MAX_DRAW_FLOW, GUI_SCREEN_CONFIG_DONOR);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgDonorMaxDrawFlow");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_3, GUI_SCREEN_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_INLET_FLOW_RAMP, GUI_SCREEN_CONFIG_DONOR);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgDonorInitialDrawRamp");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_DONOR);
}

void SimConfig::walkThroughConfigDonorVolumeLimits ()
{
   SimMsgSys msgSys;

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_VOLUME_LIMITS, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimJap");

   //turn off japanese features
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_VOLUME_LIMITS);
   turnOffJapaneseFeatures();
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_VOLUME_LIMITS, GUI_SCREEN_CONFIG);

   // First let's take a picture with everything disabled.
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_ML_PER_KG, false);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_TBV, false);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_WEIGHT, false);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_DRBC_BODY_VOL, false);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimAllOff");

   // Now try to exit (error: a volume limit must be selected)
   // A.G. changelist 55558 does not require error anymore
   //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_VOLUME_LIMITS);
   //SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimError");

   // Enable everything.
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_ML_PER_KG, true);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_TBV, true);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_WEIGHT, true);
   SimUtilities::toggleDonorVolumeLimit(VOLUME_REMOVAL_DRBC_BODY_VOL, true);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimAllOn");

   //LB to KG
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_VOLUME_LIMITS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LANGUAGE, GUI_SCREEN_CONFIG);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_WEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_LANGUAGE);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_VOLUME_LIMITS, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimKg");


   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_VOLUME_LIMITS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LANGUAGE, GUI_SCREEN_CONFIG);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_WEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_LANGUAGE);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_VOLUME_LIMITS, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "cfgVolLimLb");

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_VOLUME_LIMITS);
   turnOnJapaneseFeatures();
}

void SimConfig::turnOnJapaneseFeatures()
{
   SimDelay::waitForScreen(GUI_SCREEN_CONFIG);
   if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG)) return;

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimDelay::milliseconds(1000);

   /*0YG1B - KXJV8 - 0X0X*/
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_1, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_B, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_K, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_J, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_V, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
}

void SimConfig::turnOffJapaneseFeatures()
{
   SimDelay::waitForScreen(GUI_SCREEN_CONFIG);
   if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG)) return;

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimDelay::milliseconds(1000);

   /*0YI8C - MFMI0 - 4G8X*/
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_I, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_C, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_M, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_F, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_M, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_I, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
}

void SimConfig::walkThroughConfigReplacementFluid ()
{

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_REPLACEMENT_FLUID, GUI_SCREEN_CONFIG);
   if (guiglobs::cdsData.rwConfig.predict.key_rbc_replace_fluid)        // make sure replacement fluid is off
      SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_REPLACE_FLUID, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID);

   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, "cfgRepFluidOff");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_RBC_REPLACE_FLUID, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, "cfgRepFluidOn");
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID);
}

void SimConfig::walkThroughConfigProcedurePriority ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PRIORITY, GUI_SCREEN_CONFIG);

   for (unsigned int buttonIndex = 0, step = 3, limit = NUM_OFFLINE_PROCEDURES; buttonIndex < limit; )
   {
      unsigned int blockNum = int(std::ceil(float(buttonIndex) / step));

      SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PRIORITY, "cfgPrioBlock%d", blockNum);

      for (unsigned int rowNum = step - Min(step, limit - blockNum * step); rowNum < step; ++rowNum, ++buttonIndex)
      {
         SimScreenTouch::touch(PP_BUTTON_TIME(rowNum), GUI_SCREEN_CONFIG_PRIORITY);

         SimScreenCapture::screenshot(GUI_SCREEN_KEYPAD, "cfgPrioTime%d", buttonIndex + 1);
         SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
      }

      for (unsigned int scrollDownCount = 0; scrollDownCount < step && scrollDownCount < (limit - blockNum * step); ++scrollDownCount)
      {
         SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_PRIORITY);
      }
   }

   // # PLASMA
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PP_PLASMA_1, GUI_SCREEN_CONFIG_PRIORITY);

   // limit -> there are 6 (NUM_PLASMA_VOLUME) + NONE + AMAP + MAX CONCURRENT + FFP
   for (
      unsigned int buttonIndex = 0, step = 4, limit = NUM_PLASMA_VOLUME + 4 - step, blockNum = 0;
      SimScreenCapture::screenshot(GUI_SCREEN_PLAS_LISTBOX, "cfgPrioPlasma%d", blockNum), buttonIndex <= limit;
      buttonIndex += step, ++blockNum
      )
   {
      for (unsigned int scrollDownCount = 0; scrollDownCount < step && scrollDownCount < (limit - blockNum * step); ++scrollDownCount)
      {
         SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_PLAS_LISTBOX);
      }
   }

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_PLAS_LISTBOX);

   // # RBC
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PP_RBC_1, GUI_SCREEN_CONFIG_PRIORITY);
   SimScreenCapture::screenshot(GUI_SCREEN_RBC_LISTBOX, "cfgPrioRbc");

   //down 3 rbcs
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenCapture::screenshot(GUI_SCREEN_RBC_LISTBOX, "cfgPrioRbcNextProducts");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON, GUI_SCREEN_RBC_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_RBC_LISTBOX);

   // # PLATELETS
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PP_PLATELETS_1, GUI_SCREEN_CONFIG_PRIORITY);

   for (
      unsigned int buttonIndex = 0, step = 4, limit = NUM_YIELD_VOLUME + 1 - step, blockNum = 0;
      SimScreenCapture::screenshot(GUI_SCREEN_PLT_LISTBOX, "cfgPrioPlt%d", blockNum), buttonIndex <= limit;
      buttonIndex += step, ++blockNum
      )
   {
      for (unsigned int scrollDownCount = 0; scrollDownCount < step && scrollDownCount < (limit - blockNum * step); ++scrollDownCount)
      {
         SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_PLT_LISTBOX);
      }
   }

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_PLT_LISTBOX);

   // # BLOOD TYPE
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PP_TYPE_1, GUI_SCREEN_CONFIG_PRIORITY);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgPrioBloodType1");

   for (int j = 4; j>0; --j) SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_LISTBOX);

   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgPrioBloodType2");

   for (int j = 4; j>0; --j) SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_LISTBOX);

   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgPrioBloodType3");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_LISTBOX);
   SimScreenCapture::screenshot(GUI_SCREEN_LISTBOX, "cfgPrioBloodType4");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_LISTBOX);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PRIORITY);
}

void SimConfig::walkThroughConfigProcedurePriorityAmapPlt ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PRIORITY, GUI_SCREEN_CONFIG);
   // # PLATELETS Only
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PRIORITY, "cfgPrioAmapPlt");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PP_PLATELETS_1, GUI_SCREEN_CONFIG_PRIORITY);

   SimScreenCapture::screenshot(GUI_SCREEN_PLT_LISTBOX, "cfgPrioAmapPltList");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_PLT_LISTBOX);
   SimDelay::waitForScreen(GUI_SCREEN_CONFIG_PRIORITY);

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PRIORITY);
}

void SimConfig::walkThroughConfigSoftwareOptions ()
{
   SimMsgSys msg;

	SimDelay::waitForScreen(GUI_SCREEN_CONFIG);
   if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG)) return;

   string_screen_mapper::AddConcatMap(textCfgSoftOptEmptyItem,
                                      textCfgSoftOptEmptyItem);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "cfgSoftwareOptions1");

   SimDelay::milliseconds(1000);

   SimUtilities::setStatLine(textSoftOptInvalidKeyUser);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "cfgSoftwareOptionsInvalidUser");
   SimUtilities::setStatLine();

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimDelay::milliseconds(1000);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions2");

   /*invalid key*/
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_M, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions_InvalidKey");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CLR, GUI_SCREEN_KEYBOARD);

   /*invalid serial 0Z0B4 - ZB4U0 - GIUX0 - X0X */
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_B, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_B, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_I, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions_InvalidSerial");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CLR, GUI_SCREEN_KEYBOARD);

   /* Invalid site 0Z641 - GY2ZC - G0X0X - 0X8 */
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_6, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_1, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_C, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions_InvalidSite");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CLR, GUI_SCREEN_KEYBOARD);

   /* Invalid customer 0Z456 - 0ZH25 - AX2X0 - Z03 */
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_5, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_6, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_5, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_A, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_3, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions_InvalidCustomer");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CLR, GUI_SCREEN_KEYBOARD);

   /* Invalid COUNTRY 0Z2Z8 - 2ZCGG - S08GX - 0X */
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_C, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_S, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions_InvalidCountry");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CLR, GUI_SCREEN_KEYBOARD);

   /* everything on */
   /*0YP98 - 0GBFL - RYUVU - P*/
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_P, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_9, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_B, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_F, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_L, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_R, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_V, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_P, GUI_SCREEN_KEYBOARD);

   SimScreenCapture::screenshot(GUI_SCREEN_KEYBOARD, "cfgSoftwareOptions3");
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);

   SimDelay::seconds(1);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS,
                                "cfgSoftwareOptions4");
   /* shift 9 items down*/
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "cfgSoftwareOptions5");

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_DN, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "cfgSoftwareOptions6");

   // turn TWO on
   /*
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_A, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_1, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Q, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_6, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_T, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_D, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_W, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_5, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_F, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_V, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   // close keyboard
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimDelay::milliseconds(1000);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "cfgSoftwareOptions7");
   */
   resetSoftwareConfiguration();

   // return to main config
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
}

void SimConfig::walkThroughConfigAmapPlatelets ()
{
   // TURN ON THE AMAP PLATELET
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   setAmapPlatelets();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   // open platelet
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "cfgMSSAMAPPlatelet");
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

   SimDelay::seconds(2);
   // amap platelet
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLATELET, GUI_SCREEN_CONFIG);
   SimScreenCapture::screenshot(GUI_SCREEN_CFG_PLT_AMAP, "cfgPltAmap");

   // concentration
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_FINAL_CONC_1, GUI_SCREEN_CFG_PLT_AMAP);
   SimScreenTouch::keypadSequence("8999e");
   SimDelay::waitForScreen(GUI_SCREEN_CFG_PLT_AMAP);
   SimScreenCapture::screenshot(GUI_SCREEN_CFG_PLT_AMAP, "cfgPrioAmapPltConcOutRange");
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLT_FINAL_CONC_1, GUI_SCREEN_CFG_PLT_AMAP);
   SimScreenTouch::keypadSequence("1400e");
   SimDelay::waitForScreen(GUI_SCREEN_CFG_PLT_AMAP);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CFG_PLT_AMAP);


   SimDelay::seconds(2);
   walkThroughConfigProcedurePriorityAmapPlt();

   // reset configuration
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   SimDelay::waitForScreen(GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
   resetSoftwareConfiguration();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);
}

void SimConfig::walkThroughAirOutMitigation ()
{
   // TURN ON THE AMAP PLATELET
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   setAirOutMitigation();
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PROCEDURE, GUI_SCREEN_CONFIG);
   SimDelay::seconds(1);
   SimScreenCapture::screenshot(GUI_SCREEN_CONFIG_PROCEDURE, "cfgProcAirOutMitigation");
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PROCEDURE);
}

// 0YK4G - HT680 - X40H4
void SimConfig::setShowPlasmaVolume ()
{
   if (Software_CDS::GetInstance().getFeature(ShowPlasmaVolume))
      return;

   Software_CDS& sw = Software_CDS::getRWCds();
   sw.SetFeature(24, true, AUTH_R_N_D, true);

   /*
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_K, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_T, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_6, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimDelay::milliseconds(1000);
   */
}


void SimConfig::setAmapPlatelets ()
{
   Software_CDS& sw = Software_CDS::getRWCds();
   sw.SetFeature(11, true, AUTH_R_N_D, true);

   /*
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_U, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimDelay::milliseconds(1000);
   */
}

// 0YC4G - 0IZ80 - X4AG
void SimConfig::setCompPlasma ()
{
   Software_CDS& sw = Software_CDS::getRWCds();
   sw.SetFeature(12, true, AUTH_R_N_D, true);
   /*
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD,
                         GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Y, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_C, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_I, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_8, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_4, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_A, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_G, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimDelay::milliseconds(1000);
   */
}

void SimConfig::setAirOutMitigation ()
{
   if (Software_CDS::GetInstance().getFeature(AiroutMitigation))
      return;

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION_KEYBOARD,
                         GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_Z, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_C, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_2, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_H, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_R, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_0, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_1, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_J, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_9, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_N, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_P, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_W, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_S, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_W, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_W, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_X, GUI_SCREEN_KEYBOARD);
   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_T, GUI_SCREEN_KEYBOARD);

   SimScreenTouch::touch(GUI_BUTTON_KEYBOARD_CR, GUI_SCREEN_KEYBOARD);
   SimDelay::milliseconds(1000);

}

void SimConfig::resetSoftwareConfiguration ()
{
   SimDelay::milliseconds(1000);

   Software_CDS& sw = Software_CDS::getRWCds();
   //enable_screenshot_button
   sw.SetFeature(0, false, AUTH_R_N_D, true);

   //aps_high_speed_log_enable
   sw.SetFeature(1, false, AUTH_R_N_D, true);

   //cps_high_speed_log_enable
   sw.SetFeature(2, false, AUTH_R_N_D, true);

   //not in the list
   //sw.SetFeature(3, true, AUTH_R_N_D, true);

   //select_screen_display_platelet_volume
   sw.SetFeature(4, true, AUTH_R_N_D, true); //verified

   //allow_adjust_flow_rates_on_procedure_select
   sw.SetFeature(5, true, AUTH_R_N_D, true);//verified

   //confirm_ac_connection
   sw.SetFeature(6, true, AUTH_R_N_D, true); //verified, false on txt(?)

   //not in the list
   //sw.SetFeature(7, true, AUTH_R_N_D, true);

   //display_residual_volumes
   sw.SetFeature(8, false, AUTH_R_N_D, true); //verified, true on txt(?)

   //postcount_platelet_mobilization_algorithm
   sw.SetFeature(9, false, AUTH_R_N_D, true); //verified, true on txt(?)

   //airout_mitigation
   sw.SetFeature(10, false, AUTH_R_N_D, true); //verified, true on txt(?)

   //amap_platelets_available
   sw.SetFeature(11, false, AUTH_R_N_D, true);

   //complementary_plasma_available
   sw.SetFeature(12, false, AUTH_R_N_D, true);

   //not in the list
   //sw.SetFeature(13, false, AUTH_R_N_D, true);

   //cfr11_pin_logging
   sw.SetFeature(14, false, AUTH_R_N_D, true);

   //service_mode_periodic_log
   sw.SetFeature(15, true, AUTH_R_N_D, true); //verified

   //service_mode_periodic_log
   sw.SetFeature(16, false, AUTH_R_N_D, true);

   //not int the list
   //sw.SetFeature(17, false, AUTH_R_N_D, true);

   //not in the list
   //sw.SetFeature(18, true, AUTH_R_N_D, true);

   //not in the list 
   //sw.SetFeature(19, true, AUTH_R_N_D, true);

   //special_black_stamp_needle
   sw.SetFeature(20, true, AUTH_R_N_D, true);//verified, false on txt(?)

   //plo_flag
   sw.SetFeature(21, true, AUTH_R_N_D, true);

   //plopca_flag
   sw.SetFeature(22, true, AUTH_R_N_D, true);

   //obsolete
   //sw.SetFeature(23, true, AUTH_R_N_D, true);

   //show_plasma_volume
   sw.SetFeature(24, true, AUTH_R_N_D, true);

   //extra_plasma_no_ac
   sw.SetFeature(25, true, AUTH_R_N_D, true);

   //japan_features
   sw.SetFeature(26, true, AUTH_R_N_D, true); //verified

   //allow_auto_conf_plasma_procs
   sw.SetFeature(27, true, AUTH_R_N_D, true);

   //procedure_confirmation_popup
   sw.SetFeature(28, true, AUTH_R_N_D, true);

   //confirm_pas_bag_barcode_scan
   sw.SetFeature(29, true, AUTH_R_N_D, true);

   //pir_preferred
   sw.SetFeature(30, false, AUTH_R_N_D, true);

   //pir_preferred
   sw.SetFeature(31, true, AUTH_R_N_D, true);

}

void SimConfig::configureIgnoredStrings ()
{
   if (string_screen_mapper::populateIgnoredStrings())
      DataLog (log_level_sim_info) << "Found ignored_strings.dat. Strings in the file will be ignored when determining success." << endmsg;
   else
      DataLog (log_level_sim_info) << "WARNING: Simulator may not succeed. \"ignored_strings.dat\" not found." << endmsg;
}

void SimConfig::gotoConfig ()
{
   SimDelay::waitForScreen(GUI_SCREEN_SYSDONOR);
   if (!SimUtilities::checkScreen(GUI_SCREEN_SYSDONOR))
      return;

   SimScreenCapture::autoNavOn(false);

   SimScreenTouch::touch(GUI_BUTTON_LOWLEFT, GUI_SCREEN_SYSDONOR);

   // Enter Password, then take a picture
   SimScreenTouch::keypadSequence("789852e");
}

void SimConfig::exitConfig ()
{
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG);
}

void SimConfig::setMinAmapPlasma (const char* stringNum)
{
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLASMA, GUI_SCREEN_CONFIG);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_PLS_AMAP_MIN, GUI_SCREEN_CONFIG_PLASMA);

   SimScreenTouch::keypadSequence("200");
   SimScreenTouch::touch(GUI_BUTTON_KPAD_ENTER, GUI_SCREEN_KEYPAD);
   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_PLASMA);
}

void SimConfig::setupPrerunSimulatorConfig ()
{
   gotoConfig();
   setMinAmapPlasma("100");
   //SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   resetSoftwareConfiguration();
   //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LANGUAGE, GUI_SCREEN_CONFIG);
   //height
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_HEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);
   //weight
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_WEIGHT, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_2, GUI_SCREEN_LISTBOX);
	//hct
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_CRIT_OR_GLOB, GUI_SCREEN_CONFIG_LANGUAGE);
   SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_1, GUI_SCREEN_LISTBOX);

   SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_LANGUAGE);
   exitConfig();
}

void SimConfig::setupRunSimulatorConfig ()
{
   gotoConfig();
   setMinAmapPlasma("200");
   //SimScreenTouch::touch(GUI_BUTTON_CONFIG_SOFT_OPTION, GUI_SCREEN_CONFIG);
   resetSoftwareConfiguration();
   //SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS);

   exitConfig();
}

/* FORMAT HASH b2da3912f079450eb9db0f0b195628a3 */
