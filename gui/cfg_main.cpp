/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG

*     File name:   cfg_main.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>

/** include files **/
#include <string.h>

// EXTERNAL REFERENCES
#include "guiglobs.hpp"

// BASE CLASS INCLUDE
#include "cfg_main.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "screen_config.hpp"
#include "cfg_rbc.hpp"
#include "trima_info.h"

// play sounds hack
#include "sounds.hpp"

// play alarms hack
#include "cassette_dat.h"


/** defines **/
const int FIRST_X  = 10;
const int SECOND_X = FIRST_X + 210;
const int THIRD_X  = SECOND_X + 210;
const int FIRST_Y  = 115;
const int Y_DIFF   = 75;


//
///////////////////////////////////////////////////////////////////
//
static void cb_config_machine (void* data);
static void cb_config_language (void* data);
static void cb_config_plasma (void* data);
static void cb_config_platelet (void* data);
static void cb_config_rbc (void* data);
static void cb_config_procedure (void* data);
static void cb_config_donor (void* data);
static void cb_config_priority (void* data);
static void cb_config_replacement_fluid (void* data);
static void cb_config_volume_limits (void* data);
static void cb_config_mss (void* data);
static void cb_config_softwareOption (void* data);

///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG::Screen_CONFIG()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_CONFIG, DisplayOrder_ClearBackground),
     text_screen_title     (textCfgScreenLabelMainMenu),

// Allocate the buttons
     _cfgBtnLang(GUI_BUTTON_CONFIG_LANGUAGE,
                 FIRST_X,
                 FIRST_Y,
                 textButtonCfgMainLang,
                 BITMAP_CONFIG_MAIN_UP,
                 BITMAP_CONFIG_MAIN_DOWN,
                 false, false),

     _cfgBtnPlatelet(GUI_BUTTON_CONFIG_PLATELET,
                     SECOND_X,
                     FIRST_Y,
                     textButtonCfgMainPlt,
                     BITMAP_CONFIG_MAIN_UP,
                     BITMAP_CONFIG_MAIN_DOWN,
                     false, false),

     _cfgBtnDonor(GUI_BUTTON_CONFIG_DONOR,
                  THIRD_X,
                  FIRST_Y,
                  textButtonCfgMainDonor,
                  BITMAP_CONFIG_MAIN_UP,
                  BITMAP_CONFIG_MAIN_DOWN,
                  false, false),

     _cfgBtnMachine(GUI_BUTTON_CONFIG_MACHINE,
                    FIRST_X,
                    FIRST_Y + Y_DIFF,
                    textButtonCfgMainMach,
                    BITMAP_CONFIG_MAIN_UP,
                    BITMAP_CONFIG_MAIN_DOWN,
                    true, false),


     _cfgBtnPlasma(GUI_BUTTON_CONFIG_PLASMA,
                   SECOND_X,
                   FIRST_Y + Y_DIFF,
                   textButtonCfgMainPls,
                   BITMAP_CONFIG_MAIN_UP,
                   BITMAP_CONFIG_MAIN_DOWN,
                   false, false),

     _cfgBtnVolLimits(GUI_BUTTON_CONFIG_VOLUME_LIMITS,
                      THIRD_X,
                      FIRST_Y + Y_DIFF,
                      textButtonCfgMainVolLim,
                      BITMAP_CONFIG_MAIN_UP,
                      BITMAP_CONFIG_MAIN_DOWN,
                      false, false),

     _cfgBtnProcedure(GUI_BUTTON_CONFIG_PROCEDURE,
                      FIRST_X,
                      FIRST_Y + Y_DIFF + Y_DIFF,
                      textButtonCfgMainProc,
                      BITMAP_CONFIG_MAIN_UP,
                      BITMAP_CONFIG_MAIN_DOWN,
                      false, false),

     _cfgBtnRbc(GUI_BUTTON_CONFIG_RBC,
                SECOND_X,
                FIRST_Y + Y_DIFF + Y_DIFF,
                textButtonCfgMainRbc,
                BITMAP_CONFIG_MAIN_UP,
                BITMAP_CONFIG_MAIN_DOWN,
                false, false),

     _cfgBtnRepFluid(GUI_BUTTON_CONFIG_REPLACEMENT_FLUID,
                     THIRD_X,
                     FIRST_Y + Y_DIFF + Y_DIFF,
                     textButtonCfgMainRepFluid,
                     BITMAP_CONFIG_MAIN_UP,
                     BITMAP_CONFIG_MAIN_DOWN,
                     false, false),

     _cfgBtnAutoProc(GUI_BUTTON_CONFIG_METERED_STORAGE_SOLUTION,
                     FIRST_X,
                     FIRST_Y + (3 * Y_DIFF),
                     textButtonCfgMainAutoProc,
                     BITMAP_CONFIG_MAIN_UP,
                     BITMAP_CONFIG_MAIN_DOWN,
                     false, false),

     _cfgBtnSwOpt(GUI_BUTTON_CONFIG_SOFT_OPTION,
                  SECOND_X,
                  FIRST_Y + (3 * Y_DIFF),
                  textButtonCfgMainSoftOpt,
                  BITMAP_CONFIG_MAIN_UP,
                  BITMAP_CONFIG_MAIN_DOWN,
                  false, false),

     _cfgBtnProcPrio(GUI_BUTTON_CONFIG_PRIORITY,
                     THIRD_X,
                     FIRST_Y + (3 * Y_DIFF),
                     textButtonCfgMainPrio,
                     BITMAP_CONFIG_MAIN_UP,
                     BITMAP_CONFIG_MAIN_DOWN,
                     false, false)

{}



///////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_CONFIG::~Screen_CONFIG()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_CONFIG::deallocate_resources ();
}  // END of Screen_CONFIG DESTRUCTOR



///////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG::allocate_resources (const char* allocation_parameter)
{
   guiglobs::timeOnlyConfigMode = bool( Software_CDS::GetInstance().getTimeOnlyConfigMode() );

   DataLog (log_level_gui_info) << "TimeOnlyConfigMode=" << guiglobs::timeOnlyConfigMode << endmsg;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   _cfgBtnMachine.allocate_resources(*this);
   _cfgBtnLang.allocate_resources(*this);
   _cfgBtnPlasma.allocate_resources(*this);
   _cfgBtnPlatelet.allocate_resources(*this);
   _cfgBtnProcedure.allocate_resources(*this);
   _cfgBtnDonor.allocate_resources(*this);
   _cfgBtnVolLimits.allocate_resources(*this);
   _cfgBtnRbc.allocate_resources(*this);
   _cfgBtnAutoProc.allocate_resources(*this);
   _cfgBtnProcPrio.allocate_resources(*this);
   _cfgBtnRepFluid.allocate_resources(*this);
   _cfgBtnSwOpt.allocate_resources(*this);

   if ((!guiglobs::timeOnlyConfigMode) &&
       guiglobs::screen_status.get_previous_screen() == GUI_SCREEN_SYSDONOR)
   {
      // If we are coming from SYSDONR, we need to get the password first, so
      // make sure an operator can't press any other buttons before we can bring
      // the keypad to the front of the display
      //
      _cfgBtnMachine.disable_callbacks();
      _cfgBtnLang.disable_callbacks();
      _cfgBtnPlasma.disable_callbacks();
      _cfgBtnPlatelet.disable_callbacks();
      _cfgBtnProcedure.disable_callbacks();
      _cfgBtnDonor.disable_callbacks();
      _cfgBtnVolLimits.disable_callbacks();
      _cfgBtnRbc.disable_callbacks();
      _cfgBtnProcPrio.disable_callbacks();
      _cfgBtnRepFluid.disable_callbacks();
      _cfgBtnAutoProc.disable_callbacks();
      _cfgBtnSwOpt.disable_callbacks();
   }

   // Point the non-standard buttons to their C callback routines
   _cfgBtnMachine.set_callback (cb_config_machine, (void*)this);
   _cfgBtnLang.set_callback (cb_config_language, (void*)this);
   _cfgBtnPlasma.set_callback (cb_config_plasma, (void*)this);
   _cfgBtnPlatelet.set_callback (cb_config_platelet, (void*)this);
   _cfgBtnProcedure.set_callback (cb_config_procedure, (void*)this);
   _cfgBtnRbc.set_callback (cb_config_rbc, (void*)this);
   _cfgBtnDonor.set_callback (cb_config_donor, (void*)this);
   _cfgBtnVolLimits.set_callback (cb_config_volume_limits, (void*)this);
   _cfgBtnProcPrio.set_callback (cb_config_priority, (void*)this);
   _cfgBtnRepFluid.set_callback (cb_config_replacement_fluid, (void*)this);
   _cfgBtnAutoProc.set_callback (cb_config_mss, (void*)this);
   _cfgBtnSwOpt.set_callback(cb_config_softwareOption, (void*)this);

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_lowleft.deallocate_resources ();

   // set text on the continue button
   btn_continue.set_string_id (textButtonLongConfigExit);

   link_status_line_only();

   // Display the keypad for a password entry into the configuration system
   // coming from the System Donor screen
   if (guiglobs::screen_status.get_previous_screen() == GUI_SCREEN_SYSDONOR)
   {
      // Refresh the "working copy" of the Trima configuration.
      guiglobs::cdsData.rwConfig.refresh();

      if (!guiglobs::timeOnlyConfigMode)
      {
         _msgKeypadData.init(Callback<Screen_CONFIG>(this, &Screen_CONFIG::respond_to_keypad_return), MessageBase::SNDRCV_RECEIVE_ONLY);

         char keypadAllocString[80] = "";

         sprintf(keypadAllocString,
                 "%d%s%f%s%f%s%d%s%d%s%d%s%s",
                 STANDARD,
                 STRING_TOKEN,
                 999999999.0f,
                 STRING_TOKEN,
                 0.0f,
                 STRING_TOKEN,
                 224,
                 STRING_TOKEN,
                 110,
                 STRING_TOKEN,
                 textEmpty,
                 STRING_TOKEN,
                 "########"
                 );

         goto_child_screen (GUI_SCREEN_KEYPAD, keypadAllocString);
      }
   }

} // END of Screen_CONFIG ALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG::deallocate_resources ()
{
   // Delete the screen event notification object
   _msgKeypadData.deregisterMsg();

   _cfgBtnMachine.deallocate_resources();
   _cfgBtnLang.deallocate_resources();
   _cfgBtnPlasma.deallocate_resources();
   _cfgBtnPlatelet.deallocate_resources();
   _cfgBtnProcedure.deallocate_resources();
   _cfgBtnDonor.deallocate_resources();
   _cfgBtnVolLimits.deallocate_resources();
   _cfgBtnRbc.deallocate_resources();
   _cfgBtnAutoProc.deallocate_resources();
   _cfgBtnProcPrio.deallocate_resources();
   _cfgBtnRepFluid.deallocate_resources();
   _cfgBtnSwOpt.deallocate_resources();

   //
   //  remove status line set from continue button.  Note this code is after the screen
   //  transition to ensure that the status line stays up until the config screen is
   //  really gone.
   //
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}  // END of Screen_CONFIG DEALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_CONFIG::process_continue_button ()
{
   // Set status line
   guiglobs::apheresis_status_line->reparent_status_line(*this);
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatConfigExitWait);

   // Freeze the continue button so we don't get multiple presses.
   disable_standard_buttons ();

   // Make sure that the DRBC split toggle bit is set correctly in CDS before writing.
   Screen_CONFIG_RBC::getInstancePtr()->verifyDrbcBitReflectsPtfToggles();

   // if Air to donor mitigation is off, key_air_removal will be off
   if (!Software_CDS::GetInstance().getFeature(AiroutMitigation))
   {
      guiglobs::cdsData.rwConfig.procedure.key_air_removal = false;
   }
   // Save the "working copy" of the config to the Config CDS and commit the change to file.
   guiglobs::cdsData.rwConfig.write();

   guiglobs::initStatBar();

   AdminCassetteDat::updateIfChanged();

   DataLog(log_level_gui_info) << "Exiting Screen - config" << endmsg;

   // Used to occur when ConfigChanged message was received.
   goto_screen(GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
}  // END of Screen_CONFIG PROCESS_CONTINUE_BUTTON



///////////////////////////////////////////////////////////////////////////////
// Machine button
void cb_config_machine (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_CONFIG*)data)->process_config_machine_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_machine_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_MACHINE, "GUI_SCREEN_CONFIG_MACHINE");
}



///////////////////////////////////////////////////////////////////////////////
// language button
void cb_config_language (void* data)
{
   ((Screen_CONFIG*)data)->process_config_language_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_language_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_LANGUAGE, "GUI_SCREEN_CONFIG_LANGUAGE");
}



///////////////////////////////////////////////////////////////////////////////
// plasma button
void cb_config_plasma (void* data)
{
   ((Screen_CONFIG*)data)->process_config_plasma_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_plasma_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_PLASMA, "GUI_SCREEN_CONFIG_PLASMA");
}



///////////////////////////////////////////////////////////////////////////////
// platelet button
void cb_config_platelet (void* data)
{
   ((Screen_CONFIG*)data)->process_config_platelet_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_platelet_button ()
{
   const bool amappOn = guiglobs::amapPlatelets();

   DataLog (log_level_gui_info) << "AMAPP is " << amappOn << endmsg;

   if (amappOn)
      goto_screen (GUI_SCREEN_CFG_PLT_AMAP, "GUI_SCREEN_CFG_PLT_AMAP");
   else
      goto_screen (GUI_SCREEN_CONFIG_PLATELET, "GUI_SCREEN_CONFIG_PLATELET");
}



///////////////////////////////////////////////////////////////////////////////
// rbc button
void cb_config_rbc (void* data)
{
   ((Screen_CONFIG*)data)->process_config_rbc_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_rbc_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_RBC, "GUI_SCREEN_CONFIG_RBC");
}



///////////////////////////////////////////////////////////////////////////////
// donor button
void cb_config_donor (void* data)
{
   ((Screen_CONFIG*)data)->process_config_donor_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_donor_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_DONOR, "GUI_SCREEN_CONFIG_DONR");
}



///////////////////////////////////////////////////////////////////////////////
// donor button
void cb_config_volume_limits (void* data)
{
   ((Screen_CONFIG*)data)->process_config_volume_limits_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_volume_limits_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_VOLUME_LIMITS, "GUI_SCREEN_CONFIG_VOLUME_LIMITS");
}



///////////////////////////////////////////////////////////////////////////////
// procedure button
void cb_config_procedure (void* data)
{
   ((Screen_CONFIG*)data)->process_config_procedure_button();
}



///////////////////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_procedure_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_PROCEDURE, "GUI_SCREEN_CONFIG_PROCEDURE");
}



//
///////////////////////////////////////////////////////////////////
// priority button
void cb_config_priority (void* data)
{
   ((Screen_CONFIG*)data)->process_config_priority_button();
}



//
///////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_priority_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_PRIORITY, "GUI_SCREEN_CONFIG_PRIORITY");
}



//
///////////////////////////////////////////////////////////////////
// priority button
void cb_config_replacement_fluid (void* data)
{
   ((Screen_CONFIG*)data)->process_replacement_fluid_button();
}



//
///////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_replacement_fluid_button ()
{
   goto_screen (GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, "GUI_SCREEN_CONFIG_REPLACEMENT_FLUID");
}

///////////////////////////////////////////////////////////////////
// MSS button
void cb_config_mss (void* data)
{
   ((Screen_CONFIG*)data)->process_config_mss_button();
}

///////////////////////////////////////////////////////////////////
//
void Screen_CONFIG::process_config_mss_button ()
{
   // uncomment after clinical
   goto_screen (GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "GUI_SCREEN_CONFIG_METERED_SOLUTIONS");
}

///////////////////////////////////////////////////////////////////
// Sotfware option
void cb_config_softwareOption (void* data)
{
   ((Screen_CONFIG*)data)->process_config_swOpt_button();
}



void Screen_CONFIG::process_config_swOpt_button ()
{
   // uncomment after clinical
   goto_screen (GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS, "GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS");
}

//
///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
//
// Reads the password from the configuration file.  If the user's keypad entry DOES NOT match
// the password from the config file then we will switch back to the main configuration screen
// otherwise the user is left in the MAIN CONFIGURATION screen.
//
void Screen_CONFIG::respond_to_keypad_return (void)
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_error) << "ERROR: Shouldn't get a keypad when in timeOnlyConfigMode" << endmsg;
      return;
   }

   KEYPAD_RETURN_STRUCT return_keypad_buffer = _msgKeypadData.getData();

   float                password             = guiglobs::cdsData.rwConfig.machine.key_pword;
   float                the_data             = (float)atof(return_keypad_buffer.keypad_return_string);

   if (TrimaInfo::isTestBuild())
   {
      const int testHack = int( the_data );

      DataLog(log_level_gui_info) << "TestBuild: testHack=" << testHack << endmsg;

      // Play sounds hack
      if ((testHack / 100) == 999)
      {
         SOUND_CODE sound = ALARM_SOUND;
         sound = SOUND_CODE(testHack - 99900);

         DataLog(log_level_gui_info) << "Playing sound " << sound << endmsg;

         // pause 2 seconds to get the recorder ready
         taskDelay(sysClkRateGet() * 2);

         guiglobs::button_audio_feedback.create_feedback_sound (sound, SOUND_ONCE);
      }

      // Play alarms hack
      if ((testHack / 100000) == 9)
      {
         ALARM_VALUES alarm;
         alarm = ALARM_VALUES(testHack - 900000);

         DataLog(log_level_gui_info) << "Playing alarm " << alarm << endmsg;

         anAlarmMsg AlarmMsg(alarm);
         AlarmMsg.setAlarm();
      }
   }

   if (the_data == password)
   {
      _cfgBtnMachine.enable_callbacks();
      _cfgBtnLang.enable_callbacks();
      _cfgBtnPlasma.enable_callbacks();
      _cfgBtnPlatelet.enable_callbacks();
      _cfgBtnProcedure.enable_callbacks();
      _cfgBtnDonor.enable_callbacks();
      _cfgBtnVolLimits.enable_callbacks();
      _cfgBtnRbc.enable_callbacks();
      _cfgBtnProcPrio.enable_callbacks();
      _cfgBtnRepFluid.enable_callbacks();
      _cfgBtnAutoProc.enable_callbacks();
      _cfgBtnSwOpt.enable_callbacks();
      return;
   }
   else
   {
      if (!TrimaInfo::isTestBuild())
      {
         DataLog(log_level_gui_error) << "Incorrect password " << return_keypad_buffer.keypad_return_string << endmsg;
      }

      goto_screen (GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
   }
}


void Screen_CONFIG::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textCfgScreenLabelMainMenu);
}

/* FORMAT HASH 5ae005f9e7c96a6df6935d31c65cdd7f */
