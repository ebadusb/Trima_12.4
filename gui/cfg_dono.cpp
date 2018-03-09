/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_DONOR

*     File name:   cfg_dono.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_dono.cpp 1.34 2008/07/14 20:25:36Z dslausb Exp dslausb $
*/
#include <vxworks.h>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"


// BASE CLASS INCLUDE
#include "cfg_dono.hpp"

static void cb_cfg_ac_rate (void* data);
static void cb_cfg_post_crit (void* data);
static void cb_cfg_post_plat (void* data);
static void cb_cfg_max_draw_flow (void* data);
static void cb_cfg_inlet_flow_ramp (void* data);
static void cb_cfg_inlet_management (void* data);
static void cb_cfg_return_management (void* data);
static void cb_cfg_autoflow (void* data);


/** defines **/


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG_DONOR::Screen_CONFIG_DONOR()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_DONOR, textCfgScreenLabelDonor),


     config_btn_ac_rate           (CFG_Look::FIRST_X,
                                   CFG_Look::FIRST_Y,
                                   guiglobs::cdsData.rwConfig.procedure.key_ac_rate,
                                   KEY_AC_RATE,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_DONOR_AC_RATE,
                                   textButtonLabelCfgLeftDonAcRate,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),


     config_btn_post_crit         (CFG_Look::FIRST_X,
                                   CFG_Look::FIRST_Y + CFG_Look::Y_DIFF,
                                   guiglobs::cdsData.rwConfig.procedure.key_post_crit,
                                   KEY_POST_CRIT,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_DONOR_POST_CRIT,
                                   textButtonLabelCfgLeftDonPostCrit,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),

     config_btn_post_plat         (CFG_Look::FIRST_X,
                                   CFG_Look::FIRST_Y + CFG_Look::Y_DIFF + CFG_Look::Y_DIFF,
                                   guiglobs::cdsData.rwConfig.procedure.key_post_plat,
                                   KEY_POST_PLAT,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_DONOR_POST_PLAT,
                                   textButtonLabelCfgLeftDonPostPlat,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),

     config_btn_max_draw_flow(CFG_Look::SECOND_X,
                              CFG_Look::FIRST_Y,
                              guiglobs::cdsData.rwConfig.procedure.key_max_draw_flow,
                              KEY_MAX_DRAW_FLOW,
                              CFG_Look::BUTTON_UP_NAME,
                              CFG_Look::BUTTON_DOWN_NAME,
                              GUI_BUTTON_CONFIG_MAX_DRAW_FLOW,
                              textButtonLabelCfgLeftDonMaxDrawFlow,
                              CFG_Look::FONT_COLOR,
                              CFG_Look::FONT_ALIGN,
                              CFG_Look::FONT_SIZE),

     config_btn_inlet_flow_ramp   (CFG_Look::SECOND_X,
                                   CFG_Look::FIRST_Y + CFG_Look::Y_DIFF,
                                   guiglobs::cdsData.rwConfig.predict.key_inlet_flow_ramp,
                                   KEY_INLET_FLOW_RAMP,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_INLET_FLOW_RAMP,
                                   textButtonLabelCfgLeftDonInletFlowRamp,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),

     config_btn_inlet_management  (CFG_Look::SECOND_X,
                                   CFG_Look::FIRST_Y + (2 * CFG_Look::Y_DIFF),
                                   guiglobs::cdsData.rwConfig.predict.key_inlet_management,
                                   KEY_INLET_MANAGEMENT,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_INLET_MANAGEMENT,
                                   textButtonLabelCfgLeftDonInletMgmt,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),

     config_btn_return_management (CFG_Look::SECOND_X,
                                   CFG_Look::FIRST_Y + (3 * CFG_Look::Y_DIFF),
                                   guiglobs::cdsData.rwConfig.predict.key_return_management,
                                   KEY_RETURN_MANAGEMENT,
                                   CFG_Look::BUTTON_UP_NAME,
                                   CFG_Look::BUTTON_DOWN_NAME,
                                   GUI_BUTTON_CONFIG_RETURN_MANAGEMENT,
                                   textButtonLabelCfgLeftDonReturnMgmt,
                                   CFG_Look::FONT_COLOR,
                                   CFG_Look::FONT_ALIGN,
                                   CFG_Look::FONT_SIZE),


     config_btn_autoflow         (CFG_Look::FIRST_X,
                                  CFG_Look::FIRST_Y + (3 * CFG_Look::Y_DIFF),
                                  guiglobs::cdsData.rwConfig.procedure.key_autoflow,
                                  KEY_AUTOFLOW,
                                  CFG_Look::BUTTON_UP_NAME,
                                  CFG_Look::BUTTON_DOWN_NAME,
                                  GUI_BUTTON_CFG_AUTOFLOW),


     config_lbl_autoflow(JPHtextAutoFlowCfgBtn),
     _isAfToggledOff(false),

     _key_post_crit(guiglobs::config_var_list.Var(KEY_POST_CRIT)),
     _key_post_plat(guiglobs::config_var_list.Var(KEY_POST_PLAT)),
     _featureChanged(Callback<Screen_CONFIG_DONOR>(this,&Screen_CONFIG_DONOR::FeatureChangedNotification))

{
   _isAFEnabledOnBoot = Software_CDS::GetInstance().getFeature(AutoFlowFlag);
   if (_isAFEnabledOnBoot && guiglobs::cdsData.rwConfig.predict.key_inlet_flow_ramp != 1)
   {
     // Ensure that Inlet Draw Ramp is forced to True
     guiglobs::cdsData.rwConfig.predict.key_inlet_flow_ramp = 1;
     guiglobs::cdsData.rwConfig.write();
   }
}
// END of Screen_CONFIG_donor CONSTRUCTOR


void Screen_CONFIG_DONOR::dealloc_widgets ()
{
   deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_DONOR::~Screen_CONFIG_DONOR()
{
   dealloc_widgets();
}  // END of Screen_CONFIG_DONOR DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_DONOR::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.  The following buttons
   // are always allocated.

   config_btn_ac_rate.allocate_resources (*this);
   config_btn_ac_rate.set_callback (cb_cfg_ac_rate, (void*)this);

   config_btn_post_crit.allocate_resources (*this);
   config_btn_post_crit.set_callback (cb_cfg_post_crit, (void*)this);

   //
   //
   //  If we are using the "old" postcount algorithm then indicate this by using
   //  the special bitmap.
   //
   if (Software_CDS::GetInstance().getFeature(PostcountPlateletMobilizationAlgorithm))
   {
      DataLog(log_level_gui_info) << "Using normal bitmap for postcount button" << endmsg;
      config_btn_post_plat.set_up_bitmap(CFG_Look::BUTTON_UP_NAME); // this statement needed in case we switch languages in the same boot
   }
   else
   {
      DataLog(log_level_gui_info) << "Using special bitmap for postcount button" << endmsg;
      config_btn_post_plat.set_up_bitmap(BITMAP_CONFIG_POSTCOUNT_UP);
   }
   config_btn_post_plat.allocate_resources (*this);
   config_btn_post_plat.set_callback (cb_cfg_post_plat, (void*)this);

   config_btn_max_draw_flow.allocate_resources (*this);
   config_btn_max_draw_flow.set_callback (cb_cfg_max_draw_flow, (void*)this);

   config_btn_inlet_management.allocate_resources (*this);
   config_btn_inlet_management.set_callback (cb_cfg_inlet_management, (void*)this);

   config_btn_return_management.allocate_resources (*this);
   config_btn_return_management.set_callback (cb_cfg_return_management, (void*)this);

   config_btn_inlet_flow_ramp.allocate_resources (*this);
   config_btn_inlet_flow_ramp.set_callback (cb_cfg_inlet_flow_ramp, (void*)this);

   //////////////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////
   //    Autoflow on/off button isnt available in 6.4 but the initial code is here.
   //
   // ///////////////////////////////////////////////////////////////////////////////
   // config_lbl_autoflow.allocate_resources (*this);
   // config_btn_autoflow.allocate_resources (*this);
   // config_btn_autoflow.set_callback(cb_cfg_autoflow, (void*)this);
   // chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_autoflow,      config_btn_autoflow);
   // ///////////////////////////////////////////////////////////////////////////////

   checkForTimeOnlyConfig();

   // If AF is enabled, Inlet Draw Flow ramp is forced to Yes, user cannot select an option.
   if (Software_CDS::GetInstance().getFeature(AutoFlowFlag))
   {
      disableInitialDrawRampButton();
   }
   else if (_isAfToggledOff)
   {
      enableInitialDrawRampButton();
      _isAfToggledOff = false;
   }
} // END of Screen_CONFIG_donor ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_DONOR::deallocate_resources ()
{
   // deallocate all constituent objects first
   config_btn_ac_rate.deallocate_resources();
   config_btn_post_crit.deallocate_resources();
   config_btn_post_plat.deallocate_resources();
   config_btn_max_draw_flow.deallocate_resources();
   config_btn_inlet_management.deallocate_resources();
   config_btn_return_management.deallocate_resources();
   config_btn_inlet_flow_ramp.deallocate_resources();
   config_btn_autoflow.deallocate_resources();
   config_lbl_autoflow.deallocate_resources();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();

}  // END of Screen_CONFIG_DONOR DEALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// callbacks for the ac_rate buttons
void cb_cfg_ac_rate (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_ac_rate();
}



void Screen_CONFIG_DONOR::process_ac_rate ()
{
   config_btn_ac_rate.create_param_string (keypad_string, KEY_AC_RATE, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y);
   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the post_crit buttons
void cb_cfg_post_crit (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_post_crit();
}



void Screen_CONFIG_DONOR::process_post_crit ()
{
   static const float min = _key_post_crit.Min();
   static const float max = _key_post_crit.Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   config_btn_post_crit.create_keypad_string
      (keypad_string, KEY_POST_CRIT, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "##", _key_post_crit.Max(), _key_post_crit.Min());

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the post_plat buttons
void cb_cfg_post_plat (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_post_plat();
}



void Screen_CONFIG_DONOR::process_post_plat ()
{
   static const float min = _key_post_plat.Min();
   static const float max = _key_post_plat.Max();

   display_min_max_keypad_limits(min, max, "%.0f");

   config_btn_post_plat.create_keypad_string
      (keypad_string, KEY_POST_PLAT, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "######", _key_post_plat.Max(), _key_post_plat.Min());

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



////////////////////////////////////////////////////////////////////////////////
// callbacks for the config_btn_max_draw_flow buttons
void cb_cfg_max_draw_flow (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_max_draw_flow();
}



void Screen_CONFIG_DONOR::process_max_draw_flow ()
{
   config_btn_max_draw_flow.create_param_string
      (keypad_string, KEY_MAX_DRAW_FLOW, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}



////////////////////////////////////////////////////////////////////////////////
void cb_cfg_inlet_flow_ramp (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_inlet_flow_ramp();
}



void Screen_CONFIG_DONOR::process_inlet_flow_ramp ()
{
   config_btn_inlet_flow_ramp.create_param_string
      (keypad_string, KEY_INLET_FLOW_RAMP, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}



////////////////////////////////////////////////////////////////////////////////
void cb_cfg_inlet_management (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_inlet_management();
}



void Screen_CONFIG_DONOR::process_inlet_management ()
{
   config_btn_inlet_management.create_param_string
      (keypad_string, KEY_INLET_MANAGEMENT, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}



////////////////////////////////////////////////////////////////////////////////
void cb_cfg_return_management (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_return_management();
}



void Screen_CONFIG_DONOR::process_return_management ()
{
   config_btn_return_management.create_param_string
      (keypad_string, KEY_RETURN_MANAGEMENT, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}



void cb_cfg_autoflow (void* data)
{
   ((Screen_CONFIG_DONOR*)data)->process_autoflow();
}
void Screen_CONFIG_DONOR::process_autoflow ()
{


   guiglobs::cdsData.rwConfig.procedure.key_autoflow
      = (bool)(!guiglobs::cdsData.rwConfig.procedure.key_autoflow);

   chooseToggleButtonAppearance(guiglobs::cdsData.rwConfig.procedure.key_autoflow,
                                config_btn_autoflow);


}


void Screen_CONFIG_DONOR::chooseToggleButtonAppearance (bool isOn, ConfigButton& btn,
                                                        TEXT_STRING_ID onTxt = textListboxItemCfgOn,
                                                        TEXT_STRING_ID offTxt = textListboxItemCfgOff,
                                                        BITMAP_ID onBmp = CFG_Look::BUTTON_DOWN_NAME,
                                                        BITMAP_ID offBmp = CFG_Look::BUTTON_UP_NAME)
{


   config_btn_autoflow.set_button_text_color(YELLOW);

   config_btn_autoflow.set_up_bitmap((guiglobs::cdsData.rwConfig.procedure.key_autoflow) ?
                                     CFG_Look::BUTTON_DOWN_NAME : CFG_Look::BUTTON_UP_NAME);



   btn.set_up_bitmap((isOn) ? onBmp : offBmp);
   btn.set_string_id((isOn) ? onTxt : offTxt);



}



///////////////////////////////////////////////////////////////////////////////
//
// Message handler for KEYPADS
void Screen_CONFIG_DONOR::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   turn_off_min_max_keypad_text();

   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_POST_CRIT :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         config_btn_post_crit.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = (float)atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_post_crit = the_data;
         break;
      }

      case KEY_POST_PLAT :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         config_btn_post_plat.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = (float)atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_post_plat = the_data;
         break;
      }

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find DONOR keypad index upon return from keypad" << endmsg;
   }

}



//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXES
void Screen_CONFIG_DONOR::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   switch (rtn_listbox_buffer.key_name)
   {
      case KEY_AC_RATE :
         sprintf(keypad_string, "%d", rtn_listbox_buffer.key_value);
         config_btn_ac_rate.set_text_direct (10, keypad_string);
         guiglobs::cdsData.rwConfig.procedure.key_ac_rate = rtn_listbox_buffer.key_value;
         break;

      case KEY_INLET_MANAGEMENT :
         sprintf(keypad_string, "%d", rtn_listbox_buffer.key_value);
         config_btn_inlet_management.set_text_direct (10, keypad_string);
         guiglobs::cdsData.rwConfig.predict.key_inlet_management = rtn_listbox_buffer.key_value;
         break;

      case KEY_RETURN_MANAGEMENT :
         sprintf(keypad_string, "%d", rtn_listbox_buffer.key_value);
         config_btn_return_management.set_text_direct (10, keypad_string);
         guiglobs::cdsData.rwConfig.predict.key_return_management = rtn_listbox_buffer.key_value;
         break;

      case KEY_MAX_DRAW_FLOW :
         config_btn_max_draw_flow.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.procedure.key_max_draw_flow = rtn_listbox_buffer.key_value;
         break;

      case KEY_INLET_FLOW_RAMP :
         config_btn_inlet_flow_ramp.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.predict.key_inlet_flow_ramp = rtn_listbox_buffer.key_value;
         break;

      case KEY_AUTOFLOW :
         config_btn_autoflow.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
         guiglobs::cdsData.rwConfig.procedure.key_autoflow = int(rtn_listbox_buffer.key_value);
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "Could not find the DONOR return key upon a return from the button list box" << endmsg;
   } // end of switch

} // end of update_screen_with_date method


void Screen_CONFIG_DONOR::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;
      disableConfigBtn(config_btn_ac_rate);
      disableConfigBtn(config_btn_post_crit);
      disableConfigBtn(config_btn_post_plat);
      disableConfigBtn(config_btn_max_draw_flow);
      disableConfigBtn(config_btn_inlet_flow_ramp);
      disableConfigBtn(config_btn_inlet_management);
      disableConfigBtn(config_btn_return_management);
   }
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   disableInitialDrawRampButton ()
///
///   Disables the initial Draw Ramp button, user cannot select a value
///
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////

void Screen_CONFIG_DONOR::disableInitialDrawRampButton ()
{
   disableConfigBtn(config_btn_inlet_flow_ramp);
   config_btn_inlet_flow_ramp.set_text_color(YELLOW);
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   enableInitialDrawRampButton ()
///
///   The Initial Draw Ramp Button is enabled, user can select an option
///
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////

void Screen_CONFIG_DONOR::enableInitialDrawRampButton ()
{
   DataLog (log_level_gui_info) << "enableConfigBtn(Config_button_with_text): btnId="
                                   << config_btn_inlet_flow_ramp.get_button_id() << endmsg;
   config_btn_inlet_flow_ramp.enable_callbacks();
   config_btn_inlet_flow_ramp.set_disabled(false);
   config_btn_inlet_flow_ramp.set_up_bitmap(CFG_Look::BUTTON_UP_NAME);
   config_btn_inlet_flow_ramp.set_button_text_color(YELLOW);
   config_btn_inlet_flow_ramp.set_text_color(YELLOW);
}

////////////////////////////////////////////////////////////////////////////////
///   Function Name:
///   FeatureChangedNotification ()
///
///   If features have been changed using Feature key, check if AF has been toggled.
///   If AF is turned OFF, set the flag.
///   If AF is turned ON, set the value of Inlet Draw Ramp to 1
///
///   @param None
///   @return void
////////////////////////////////////////////////////////////////////////////////

void Screen_CONFIG_DONOR::FeatureChangedNotification()
{
   bool isAfEnabled = Software_CDS::GetInstance().getFeature(AutoFlowFlag);

   if (_isAFEnabledOnBoot && !isAfEnabled)
   {
      _isAfToggledOff = true;
   }

   if (isAfEnabled)
   {
      guiglobs::cdsData.rwConfig.predict.key_inlet_flow_ramp = 1;
      guiglobs::cdsData.rwConfig.write();
   }
}

////////////////////////////////////////////////////////////////////////////

/* FORMAT HASH f8f490f311ac6ed6a5930b9e2ef91654 */
