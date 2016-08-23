/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_REPL

*     File name:   cfg_rbc.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.19  2008/07/14 20:26:24Z  dslausb
More stringId changes requested by localization
Revision 1.18  2008/07/07 18:25:19Z  dslausb
More localization string id standardization
Revision 1.17  2008/04/08 17:03:43Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.16  2008/04/03 22:15:33Z  dslausb
IT 5725 - Add config setting for donor volume limit
Revision 1.15  2003/06/24 13:10:39Z  jl11312
- disabled button presses for inactive buttons
Revision 1.14  2003/04/04 17:57:37Z  rm10919
Datalog changes. IT 5818
Revision 1.13  2003/01/30 23:04:44Z  rm10919
Removed listbox references.
Revision 1.12  2002/11/11 05:12:52Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.11  2002/09/26 19:06:03  rm70006
Make some float values ints in config.
Revision 1.10  2002/08/16 16:31:25Z  sb07663
added <vxworks.h> include
Revision 1.9  2002/07/22 19:59:42  rm70006
VXWorks Port
Revision 1.8  2000/10/18 17:09:55Z  sb07663
IT4797: return flow management configuration changes
Revision 1.7  2000/10/08 04:03:35  sb07663
User configuration changes for single stage clinicals
Revision 1.6  2000/10/04 16:43:33  sb07663
Config.dat modifications for new volume removal limits
Revision 1.5  2000/08/14 18:34:32  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.4  2000/02/28 21:54:50  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.3  1999/09/28 22:12:34  TW09453
IT 4375 - teutonic text flow.
Revision 1.2  1999/08/30 21:41:34  TW09453
Convert Printf's to either logerrs, comments or remove for IT 4230
Revision 1.1  1999/05/07 16:13:45  SB07663
Initial revision
Revision 1.28  1999/04/29 21:19:03  SB07663
Allow four digits for volume IT3898; RBC label positioning; storage
warning changes per IT3405
Revision 1.27  1999/04/22 22:57:41  SB07663
Fixed crit key 3 (was typo'd to crit 1)
Revision 1.26  1999/04/16 15:00:33  SB07663
Temp V3.3a versions to allow head rev to build.
Revision 1.25  1999/04/07 22:14:42  SB07663
Initial version (incomplete) for V3.3a
Revision 1.24  1998/06/06 00:27:12  BF02858
- IT3358, DRBC only.  increased upper limit to custom RBC volume
  because the upper limit to the custom RBC dose increased from 275 to 550.
  This change only effects DRBC code.  This will not visiable in the
  non-DRBC 3.2 code.
Revision 1.23  1998/05/29 20:15:30  BF02858
- OK, to make this simple (not doing a varient), I commented out the
  DRBC code.
Revision 1.21  1998/05/29 02:12:05  BF02858
- added DRBC additions to the RBC and Proc Priority screens
Revision 1.19  1998/05/05 16:09:19  BF02858
- added REPLACEMENT FLUID buttons to the CONFIG RBC screen
Revision 1.18  1998/04/16 19:54:37  BF02858
- added rbc ratio for 3.2
- added custom rbc product for DRBC lab testing
Revision 1.17  1997/12/19 18:12:28  BF02858
- per IT 2798, added display of alert message to RBC screen when
  RBC Volume is < 150 or > 250.
Revision 1.16  1997/05/07 19:51:30  Barry_Fulkerson
- commented out all printf's
Revision 1.15  1997/04/17 18:59:59  Barry_Fulkerson
- re-arranged screen to the new calculation look
*/
#include <vxworks.h>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"

// BASE CLASS INCLUDE

#include "cfg_repl.hpp"
static void cb_cfg_min_replace_vol (void* data);
static void cb_cfg_rbc_replace_fluid (void* data);
static void cb_cfg_rbc_fluid_percent (void* data);

/** defines **/

static const int ROW1         = 162;
static const int ROW2         = 262;
static const int ROW3         = 362;

static const int COL1         = 37;
static const int COL2         = 185;
static const int COL3         = 333;
static const int COL4         = 481;

static const int KEYPAD_RBC_X = 250;
static const int KEYPAD_RBC_Y = 200;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR


Screen_CONFIG_REPL::Screen_CONFIG_REPL()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, textCfgScreenLabelRepFluid),

     config_btn_rbc_replace_fluid(GUI_BUTTON_CONFIG_RBC_REPLACE_FLUID,
                                  COL1, ROW1,
                                  textButtonCfgRepFluidRbc,
                                  CFG_Look::BUTTON_UP_NAME,
                                  CFG_Look::BUTTON_DOWN_NAME),

     config_btn_min_replace_vol   (NULL),
     config_btn_min_replace_vol_grey (NULL),
     config_btn_rbc_fluid_percent (NULL),
     config_btn_rbc_fluid_percent_grey (NULL)
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_REPL::~Screen_CONFIG_REPL()
{
   Screen_CONFIG_REPL::deallocate_resources ();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_REPL::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   config_btn_rbc_replace_fluid.allocate_resources (*this) ;
   config_btn_rbc_replace_fluid.set_callback (cb_cfg_rbc_replace_fluid, (void*)this);

   allocate_buttons();

   checkForTimeOnlyConfig();
} // END of Screen_CONFIG_REPL ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIG_REPL::deallocate_resources ()
{
   delete config_btn_min_replace_vol_grey;
   config_btn_min_replace_vol_grey = NULL;

   delete config_btn_rbc_fluid_percent_grey;
   config_btn_rbc_fluid_percent_grey = NULL;

   delete config_btn_min_replace_vol;
   config_btn_min_replace_vol = NULL;

   delete config_btn_rbc_fluid_percent;
   config_btn_rbc_fluid_percent = NULL;

   config_btn_rbc_replace_fluid.deallocate_resources () ;

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();

}  // END of Screen_CONFIG_REPL DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
void Screen_CONFIG_REPL::allocate_buttons ()
{
   if (guiglobs::cdsData.rwConfig.predict.key_rbc_replace_fluid  == CONFIG_YES)
   {
      config_btn_rbc_replace_fluid.setActive();

      //
      //
      //   Allocate the "normal" buttons and deallocate the grey buttons.  We use
      //   the replacement volume button widget pointer as the indicator as to
      //   whether the grey buttons are already allocated or not.
      //
      if (config_btn_min_replace_vol == NULL)
      {
         delete config_btn_min_replace_vol_grey;
         config_btn_min_replace_vol_grey = NULL;

         delete config_btn_rbc_fluid_percent_grey;
         config_btn_rbc_fluid_percent_grey = NULL;

         config_btn_min_replace_vol        = new Config_textabove_button
                                                (COL2,
                                                ROW1,
                                                guiglobs::cdsData.rwConfig.procedure.key_min_replacement_volume,
                                                KEY_MIN_REPLACEMENT_VOLUME,
                                                CFG_Look::BUTTON_UP_NAME,
                                                CFG_Look::BUTTON_DOWN_NAME,
                                                GUI_BUTTON_CONFIG_MIN_REPLACE_VOL,
                                                textButtonLabelCfgTopRepFluidTotCollThreshold,
                                                CFG_Look::FONT_COLOR,
                                                CFG_Look::FONT_ALIGN,
                                                CFG_Look::FONT_SIZE);

         config_btn_min_replace_vol->allocate_resources (*this);
         config_btn_min_replace_vol->set_callback (cb_cfg_min_replace_vol, (void*)this);

         config_btn_rbc_fluid_percent = new Config_textabove_button
                                           (COL3,
                                           ROW1,
                                           guiglobs::cdsData.rwConfig.predict.key_rbc_fluid_percent,
                                           KEY_RBC_FLUID_PERCENT,
                                           CFG_Look::BUTTON_UP_NAME,
                                           CFG_Look::BUTTON_DOWN_NAME,
                                           GUI_BUTTON_CONFIG_RBC_FLUID_PERCENT,
                                           textButtonLabelCfgTopRepFluidPercent,
                                           CFG_Look::FONT_COLOR,
                                           CFG_Look::FONT_ALIGN,
                                           CFG_Look::FONT_SIZE);

         config_btn_rbc_fluid_percent->allocate_resources (*this);
         config_btn_rbc_fluid_percent->set_callback (cb_cfg_rbc_fluid_percent, (void*)this);
      }

   }
   else
   {
      config_btn_rbc_replace_fluid.setInactive();
      //
      //
      //   Allocate the "grey" buttons and deallocate the normal buttons.  We use
      //   the replacement volume button widget pointer as the indicator as to
      //   whether the grey buttons are already allocated or not.
      //
      if (config_btn_min_replace_vol_grey == NULL)
      {
         delete config_btn_min_replace_vol;
         config_btn_min_replace_vol = NULL;

         delete config_btn_rbc_fluid_percent;
         config_btn_rbc_fluid_percent    = NULL;

         config_btn_min_replace_vol_grey = new Config_textabove_button
                                              (COL2,
                                              ROW1,
                                              guiglobs::cdsData.rwConfig.procedure.key_min_replacement_volume,
                                              KEY_INACTIVE,
                                              BITMAP_CONFIG_BUTTON_GRAY,
                                              BITMAP_NULL,
                                              GUI_BUTTON_CONFIG_MIN_REPLACE_VOL,
                                              textButtonLabelCfgTopRepFluidTotCollThreshold,
                                              CFG_Look::FONT_INACTIVE_COLOR,
                                              CFG_Look::FONT_ALIGN,
                                              CFG_Look::FONT_SIZE);

         config_btn_min_replace_vol_grey->allocate_resources (*this);
         config_btn_min_replace_vol_grey->disable_callbacks();

         config_btn_rbc_fluid_percent_grey = new Config_textabove_button
                                                (COL3,
                                                ROW1,
                                                guiglobs::cdsData.rwConfig.predict.key_rbc_fluid_percent,
                                                KEY_INACTIVE,
                                                BITMAP_CONFIG_BUTTON_GRAY,
                                                BITMAP_NULL,
                                                GUI_BUTTON_CONFIG_RBC_FLUID_PERCENT,
                                                textButtonLabelCfgTopRepFluidPercent,
                                                CFG_Look::FONT_INACTIVE_COLOR,
                                                CFG_Look::FONT_ALIGN,
                                                CFG_Look::FONT_SIZE);

         config_btn_rbc_fluid_percent_grey->allocate_resources (*this);
         config_btn_rbc_fluid_percent_grey->disable_callbacks();
      }
   }
}



////////////////////////////////////////////////////////////////////////////////
// callbacks for the cb_cfg_min_replace_vol button
void cb_cfg_min_replace_vol (void* data)
{
   ((Screen_CONFIG_REPL*)data)->process_min_replace_vol();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the config_btn_min_replace_vol button
void Screen_CONFIG_REPL::process_min_replace_vol ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_MIN_REPLACEMENT_VOLUME).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_MIN_REPLACEMENT_VOLUME).Max();

   display_min_max_keypad_limits (min, max, "%.0f");

   config_btn_min_replace_vol->create_keypad_string
      (keypad_string, KEY_MIN_REPLACEMENT_VOLUME, KEYPAD_RBC_X, KEYPAD_RBC_Y, "####", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the config_btn_rbc_replace_fluid buttons
void cb_cfg_rbc_replace_fluid (void* data)
{
   ((Screen_CONFIG_REPL*)data)->process_rbc_replace_fluid();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the config_btn_rbc_replace_fluid button
void Screen_CONFIG_REPL::process_rbc_replace_fluid ()
{
   if (config_btn_rbc_replace_fluid.isActive())
   {
      guiglobs::cdsData.rwConfig.predict.key_rbc_replace_fluid = 1;
   }
   else
   {
      guiglobs::cdsData.rwConfig.predict.key_rbc_replace_fluid = 0;
   }

   allocate_buttons();
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the config_btn_rbc_fluid_percent buttons
void cb_cfg_rbc_fluid_percent (void* data)
{
   ((Screen_CONFIG_REPL*)data)->process_rbc_fluid_percent();
}



//
////////////////////////////////////////////////////////////////////////////////
// Handle the config_btn_rbc_fluid_percent button
void Screen_CONFIG_REPL::process_rbc_fluid_percent ()
{
   static const float min = guiglobs::config_var_list.Var(KEY_RBC_FLUID_PERCENT).Min();
   static const float max = guiglobs::config_var_list.Var(KEY_RBC_FLUID_PERCENT).Max();

   display_min_max_keypad_limits (min, max, "%.0f");

   config_btn_rbc_fluid_percent->create_keypad_string
      (keypad_string, KEY_RBC_FLUID_PERCENT, KEYPAD_RBC_X, KEYPAD_RBC_Y, "###", max, min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_REPL::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   turn_off_min_max_keypad_text();

   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_RBC_FLUID_PERCENT :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }

         config_btn_rbc_fluid_percent->set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.predict.key_rbc_fluid_percent = the_data;
         break;
      }

      case KEY_MIN_REPLACEMENT_VOLUME :
      {
         if ((return_keypad_buffer.keypad_return_value) == -1)
         {
            return;
         }
         config_btn_min_replace_vol->set_text_direct (10, return_keypad_buffer.keypad_return_string);
         int the_data = atoi(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.procedure.key_min_replacement_volume = the_data;
         break;
      }

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find RBC keypad index" << endmsg;
      }

   }   // end of switch

}




void Screen_CONFIG_REPL::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;

      disableConfigBtn(config_btn_rbc_replace_fluid);

      if (config_btn_min_replace_vol) disableConfigBtn(*config_btn_min_replace_vol);
      if (config_btn_min_replace_vol_grey) disableConfigBtn(*config_btn_min_replace_vol_grey);
      if (config_btn_rbc_fluid_percent) disableConfigBtn(*config_btn_rbc_fluid_percent);
      if (config_btn_rbc_fluid_percent_grey) disableConfigBtn(*config_btn_rbc_fluid_percent_grey);
   }
}

/* FORMAT HASH 677368634b8523d45c4af99843c6833e */
