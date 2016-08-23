/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PLASMA

*     File name:   cfg_plas.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$
*/


#include <vxworks.h>

/** include files **/

// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "an_alarm.h"

// BASE CLASS INCLUDE
#include "cfg_plas.hpp"

/** defines **/


// Define column locations for plasma 1 - 6 anchors
#define PLASMA_COLUMN_1 180
#define PLASMA_COLUMN_2 480

#define PLASMA_ROW_SPACING 60

// Define bottom section anchors
#define BOTTOM_SECTION_COLUMN_1 110
#define BOTTOM_SECTION_COLUMN_2 332

#define BOTTOM_SECTION_SEPARATOR_Y 295


///////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIG_PLASMA::Screen_CONFIG_PLASMA()
   : Screen_Config(TEAL, GUI_SCREEN_CONFIG_PLASMA, textCfgScreenLabelPls),

// Configure Plasma buttons 1 - 6
     btn_plasma_volume_1  (PLASMA_COLUMN_1,
                           CFG_Look::FIRST_Y,
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[0],
                           KEY_PLS_VOLUME_1,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_1,
                           textButtonLabelCfgLeftPlsVol1,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     btn_plasma_volume_2  (PLASMA_COLUMN_1,
                           CFG_Look::FIRST_Y + PLASMA_ROW_SPACING,
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[1],
                           KEY_PLS_VOLUME_2,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_2,
                           textButtonLabelCfgLeftPlsVol2,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     btn_plasma_volume_3  (PLASMA_COLUMN_1,
                           CFG_Look::FIRST_Y + (2 * PLASMA_ROW_SPACING),
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[2],
                           KEY_PLS_VOLUME_3,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_3,
                           textButtonLabelCfgLeftPlsVol3,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     btn_plasma_volume_4  (PLASMA_COLUMN_2,
                           CFG_Look::FIRST_Y,
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[3],
                           KEY_PLS_VOLUME_4,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_4,
                           textButtonLabelCfgLeftPlsVol4,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     btn_plasma_volume_5  (PLASMA_COLUMN_2,
                           CFG_Look::FIRST_Y + PLASMA_ROW_SPACING,
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[4],
                           KEY_PLS_VOLUME_5,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_5,
                           textButtonLabelCfgLeftPlsVol5,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     btn_plasma_volume_6  (PLASMA_COLUMN_2,
                           CFG_Look::FIRST_Y + (2 * PLASMA_ROW_SPACING),
                           guiglobs::cdsData.rwConfig.templates.key_pls_volume[5],
                           KEY_PLS_VOLUME_6,
                           CFG_Look::BUTTON_UP_NAME,
                           CFG_Look::BUTTON_DOWN_NAME,
                           GUI_BUTTON_CONFIG_PLS_VOLUME_6,
                           textButtonLabelCfgLeftPlsVol6,
                           CFG_Look::FONT_COLOR,
                           CFG_Look::FONT_ALIGN,
                           CFG_Look::FONT_SIZE),

     _horizontalLine(0, BOTTOM_SECTION_SEPARATOR_Y, BITMAP_CFGPLASMA_HORIZ_LINE),

     btn_amap_min (PLASMA_COLUMN_1,
                   BOTTOM_SECTION_SEPARATOR_Y + 10,
                   guiglobs::cdsData.rwConfig.predict.key_pls_amap_min,
                   KEY_PLASMA_AMAP_MINIMUM,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_PLS_AMAP_MIN,
                   textButtonLabelCfgLeftPlsAmapMin,
                   CFG_Look::FONT_COLOR,
                   CFG_Look::FONT_ALIGN,
                   CFG_Look::FONT_SIZE),

     btn_amap_max (PLASMA_COLUMN_1,
                   BOTTOM_SECTION_SEPARATOR_Y + 10 + PLASMA_ROW_SPACING,
                   guiglobs::cdsData.rwConfig.predict.key_pls_amap_max,
                   KEY_PLASMA_AMAP_MAXIMUM,
                   CFG_Look::BUTTON_UP_NAME,
                   CFG_Look::BUTTON_DOWN_NAME,
                   GUI_BUTTON_CONFIG_PLS_AMAP_MAX,
                   textButtonLabelCfgLeftPlsAmapMax,
                   CFG_Look::FONT_COLOR,
                   CFG_Look::FONT_ALIGN,
                   CFG_Look::FONT_SIZE),

     btn_male_only (PLASMA_COLUMN_2,
                    BOTTOM_SECTION_SEPARATOR_Y + 10,
                    guiglobs::cdsData.rwConfig.predict.key_male_only_plasma,
                    KEY_MALE_ONLY_PLASMA,
                    CFG_Look::BUTTON_UP_NAME,
                    CFG_Look::BUTTON_DOWN_NAME,
                    GUI_BUTTON_CONFIG_MALE_ONLY_PLASMA,
                    textButtonLabelCfgLeftPlsTrali,
                    CFG_Look::FONT_COLOR,
                    CFG_Look::FONT_ALIGN,
                    CFG_Look::FONT_SIZE),

     btn_ffp_volume (PLASMA_COLUMN_2,
                     BOTTOM_SECTION_SEPARATOR_Y + 10 + PLASMA_ROW_SPACING,
                     guiglobs::cdsData.rwConfig.predict.key_ffp_volume,
                     KEY_FFP_VOLUME,
                     CFG_Look::BUTTON_UP_NAME,
                     CFG_Look::BUTTON_DOWN_NAME,
                     GUI_BUTTON_CONFIG_FFP_VOLUME,
                     textButtonLabelCfgLeftFfpVolume,
                     CFG_Look::FONT_COLOR,
                     CFG_Look::FONT_ALIGN,
                     CFG_Look::FONT_SIZE),


// Init range constants
     pls_volume_1_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_1).Min()),
     pls_volume_1_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_1).Max()),

     pls_volume_2_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_2).Min()),
     pls_volume_2_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_2).Max()),

     pls_volume_3_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_3).Min()),
     pls_volume_3_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_3).Max()),

     pls_volume_4_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_4).Min()),
     pls_volume_4_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_4).Max()),

     pls_volume_5_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_5).Min()),
     pls_volume_5_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_5).Max()),

     pls_volume_6_min(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_6).Min()),
     pls_volume_6_max(guiglobs::config_var_list.Var(KEY_PLS_VOLUME_6).Max()),

     pls_amap_min_min(guiglobs::config_var_list.Var(KEY_PLASMA_AMAP_MINIMUM).Min()),
     pls_amap_min_max(guiglobs::config_var_list.Var(KEY_PLASMA_AMAP_MINIMUM).Max()),

     pls_amap_max_min(guiglobs::config_var_list.Var(KEY_PLASMA_AMAP_MAXIMUM).Min()),
     pls_amap_max_max(guiglobs::config_var_list.Var(KEY_PLASMA_AMAP_MAXIMUM).Max()),

     pls_ffp_volume_min(guiglobs::config_var_list.Var(KEY_FFP_VOLUME).Min()),
     pls_ffp_volume_max(guiglobs::config_var_list.Var(KEY_FFP_VOLUME).Max())
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIG_PLASMA::~Screen_CONFIG_PLASMA()
{
   Screen_CONFIG_PLASMA::deallocate_resources ();
}


#define ALLOCATE_BUTTON(button, callback) button.allocate_resources(*this); button.set_callback(callback, (void*)this);

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_CONFIG_PLASMA::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Screen_Config::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   // Allocate the buttons
   ALLOCATE_BUTTON(btn_plasma_volume_1, cb_pls_volume_1);
   ALLOCATE_BUTTON(btn_plasma_volume_2, cb_pls_volume_2);
   ALLOCATE_BUTTON(btn_plasma_volume_3, cb_pls_volume_3);
   ALLOCATE_BUTTON(btn_plasma_volume_4, cb_pls_volume_4);
   ALLOCATE_BUTTON(btn_plasma_volume_5, cb_pls_volume_5);
   ALLOCATE_BUTTON(btn_plasma_volume_6, cb_pls_volume_6);

   ALLOCATE_BUTTON(btn_amap_min, cb_pls_amap_min);
   ALLOCATE_BUTTON(btn_amap_max, cb_pls_amap_max);
   ALLOCATE_BUTTON(btn_male_only, cb_pls_male_only_plasma);
   ALLOCATE_BUTTON(btn_ffp_volume, cb_pls_ffp_volume);

   _horizontalLine.allocate_resources(*this);

   checkForTimeOnlyConfig();
}  // END of Screen_CONFIG_PLASMA ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_CONFIG_PLASMA::deallocate_resources ()
{
   // deallocate all constituent objects first
   btn_plasma_volume_1.deallocate_resources ();
   btn_plasma_volume_2.deallocate_resources ();
   btn_plasma_volume_3.deallocate_resources ();
   btn_plasma_volume_4.deallocate_resources ();
   btn_plasma_volume_5.deallocate_resources ();
   btn_plasma_volume_6.deallocate_resources ();

   btn_amap_min.deallocate_resources ();
   btn_amap_min.deallocate_resources ();

   btn_male_only.deallocate_resources();
   btn_ffp_volume.deallocate_resources();

   _horizontalLine.deallocate_resources();

   // deallocate parent's resources
   Screen_Config::deallocate_resources ();
}   // END of Screen_CONFIG_PLASMA DEALLOCATE_RESOURCES


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_1 button

void cb_pls_volume_1 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_1();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_1 button

void Screen_CONFIG_PLASMA::process_pls_volume_1 ()
{
   display_min_max_keypad_limits(pls_volume_1_min, pls_volume_1_max, "%.0f");
   btn_plasma_volume_1.create_keypad_string(keypad_string, KEY_PLS_VOLUME_1, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", pls_volume_1_max, pls_volume_1_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_2 button

void cb_pls_volume_2 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_2();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_2 button

void Screen_CONFIG_PLASMA::process_pls_volume_2 ()
{
   display_min_max_keypad_limits(pls_volume_2_min, pls_volume_2_max, "%.0f");
   btn_plasma_volume_2.create_keypad_string(keypad_string, KEY_PLS_VOLUME_2, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", pls_volume_2_max, pls_volume_2_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_3 button

void cb_pls_volume_3 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_3();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_3 button

void Screen_CONFIG_PLASMA::process_pls_volume_3 ()
{
   display_min_max_keypad_limits(pls_volume_3_min, pls_volume_3_max, "%.0f");
   btn_plasma_volume_3.create_keypad_string(keypad_string, KEY_PLS_VOLUME_3, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y, "####", pls_volume_3_max, pls_volume_3_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_4 button

void cb_pls_volume_4 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_4();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_4 button

void Screen_CONFIG_PLASMA::process_pls_volume_4 ()
{
   display_min_max_keypad_limits(pls_volume_4_min, pls_volume_4_max, "%.0f");
   btn_plasma_volume_4.create_keypad_string(keypad_string, KEY_PLS_VOLUME_4, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y, "####", pls_volume_4_max, pls_volume_4_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_5 button

void cb_pls_volume_5 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_5();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_5 button

void Screen_CONFIG_PLASMA::process_pls_volume_5 ()
{
   display_min_max_keypad_limits(pls_volume_5_min, pls_volume_5_max, "%.0f");
   btn_plasma_volume_5.create_keypad_string(keypad_string, KEY_PLS_VOLUME_5, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y, "####", pls_volume_5_max, pls_volume_5_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the pls_volume_6 button

void cb_pls_volume_6 (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_volume_6();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the pls_volume_6 button

void Screen_CONFIG_PLASMA::process_pls_volume_6 ()
{
   display_min_max_keypad_limits(pls_volume_6_min, pls_volume_6_max, "%.0f");
   btn_plasma_volume_6.create_keypad_string(keypad_string, KEY_PLS_VOLUME_6, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y, "####", pls_volume_6_max, pls_volume_6_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the AMAP minimum volume buttons

void cb_pls_amap_min (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_amap_min();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the AMAP minimum volume button

void Screen_CONFIG_PLASMA::process_pls_amap_min ()
{
   display_min_max_keypad_limits(pls_amap_min_min, pls_amap_min_max, "%.0f");

   btn_amap_min.create_keypad_string
      (keypad_string, KEY_PLASMA_AMAP_MINIMUM, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF, "####", pls_amap_min_max, pls_amap_min_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}


//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the AMAP maximum volume buttons

void cb_pls_amap_max (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_amap_max();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the AMAP minimum volume button

void Screen_CONFIG_PLASMA::process_pls_amap_max ()
{
   display_min_max_keypad_limits(pls_amap_max_min, pls_amap_max_max, "%.0f");

   btn_amap_max.create_keypad_string
      (keypad_string, KEY_PLASMA_AMAP_MAXIMUM, CFG_Look::KEYPAD_X_2, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF, "####", pls_amap_max_max, pls_amap_max_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}



//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the male-only plasma button

void cb_pls_male_only_plasma (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_male_only();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the male-only plasma button

void Screen_CONFIG_PLASMA::process_pls_male_only ()
{
   btn_male_only.create_param_string (keypad_string, KEY_MALE_ONLY_PLASMA, 260, 145);

   goto_child_screen(GUI_SCREEN_LISTBOX, keypad_string);
}

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the ffp volume button

void cb_pls_ffp_volume (void* data)
{
   ((Screen_CONFIG_PLASMA*)data)->process_pls_ffp_volume();
}

//
////////////////////////////////////////////////////////////////////////////////
// Handle the ffp volume button

void Screen_CONFIG_PLASMA::process_pls_ffp_volume ()
{
   display_min_max_keypad_limits(pls_ffp_volume_min, pls_ffp_volume_max, "%.0f");

   btn_ffp_volume.create_keypad_string (keypad_string, KEY_FFP_VOLUME, CFG_Look::KEYPAD_X, CFG_Look::FIRST_Y + CFG_Look::Y_DIFF, "####", pls_ffp_volume_max, pls_ffp_volume_min);

   goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Message handler for KEYPADS
void Screen_CONFIG_PLASMA::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   turn_off_min_max_keypad_text();

   switch (return_keypad_buffer.config_data_type)
   {
      case KEY_PLS_VOLUME_1 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_1.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[0] = the_data;

         break;
      }

      case KEY_PLS_VOLUME_2 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_2.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[1] = the_data;

         break;
      }

      case KEY_PLS_VOLUME_3 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_3.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[2] = the_data;
         break;
      }

      case KEY_PLS_VOLUME_4 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_4.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[3] = the_data;

         break;
      }

      case KEY_PLS_VOLUME_5 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_5.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[4] = the_data;

         break;
      }

      case KEY_PLS_VOLUME_6 :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_plasma_volume_6.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         float the_data = atof(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.templates.key_pls_volume[5] = the_data;
         break;
      }

      case KEY_PLASMA_AMAP_MINIMUM :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_amap_min.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         int the_data = atoi(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.predict.key_pls_amap_min = the_data;
         break;
      }

      case KEY_PLASMA_AMAP_MAXIMUM :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_amap_max.set_text_direct (10, return_keypad_buffer.keypad_return_string);
         int the_data = atoi(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.predict.key_pls_amap_max = the_data;
         break;
      }

      case KEY_FFP_VOLUME :
      {
         if (return_keypad_buffer.keypad_return_value == -1)
         {
            return;
         }

         btn_ffp_volume.set_text_direct(10, return_keypad_buffer.keypad_return_string);
         int the_data = atoi(return_keypad_buffer.keypad_return_string);
         guiglobs::cdsData.rwConfig.predict.key_ffp_volume = the_data;
         break;
      }

      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "could not find PLASMA keypad index" << endmsg;
      }
   }
}


//
///////////////////////////////////////////////////////////////////////////////
// Message handler for LISTBOXS
void Screen_CONFIG_PLASMA::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   btn_male_only.display_the_text_string (CONFIG_DATA_LISTBOX_STRINGS(rtn_listbox_buffer.key_value));
   guiglobs::cdsData.rwConfig.predict.key_male_only_plasma = int(rtn_listbox_buffer.key_value);
} // end of update_screen_with_date method


void Screen_CONFIG_PLASMA::checkForTimeOnlyConfig ()
{
   if (guiglobs::timeOnlyConfigMode)
   {
      DataLog (log_level_gui_info) << "Buttons should be disabled." << endmsg;
      disableConfigBtn(btn_plasma_volume_1);
      disableConfigBtn(btn_plasma_volume_2);
      disableConfigBtn(btn_plasma_volume_3);
      disableConfigBtn(btn_plasma_volume_4);
      disableConfigBtn(btn_plasma_volume_5);
      disableConfigBtn(btn_plasma_volume_6);
      disableConfigBtn(btn_amap_min);
      disableConfigBtn(btn_amap_max);
      disableConfigBtn(btn_male_only);
      disableConfigBtn(btn_ffp_volume);
   }
}

/* FORMAT HASH bb6ac1be93dc7c0940f6a62bc4f8a54e */
