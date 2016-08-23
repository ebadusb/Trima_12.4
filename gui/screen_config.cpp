/************************> FOCUSSED GUI DOMAIN <****************************

 *     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

 *     Author:      Barry Fulkerson

 *     Class name:  Screen_CONFIG

 *     File name:   screen_config.cpp

 *     Contents:    See prototype header descriptions below...

 *     Description:


*************************< FOCUSSED GUI DOMAIN >****************************/
#include <vxworks.h>
#include "screen_config.hpp"
#include "guiglobs.hpp"
#include "cfg_look.h"
#include "an_alarm.h"
#include "statline.hpp"

//
//
// -----------------------------------------------------------------------------
//           Constructor, destructor
//
Screen_Config::Screen_Config(WINDOW_COLOR wcolor,
                             GUI_SCREEN_ID win_ID,
                             TEXT_STRING_ID title,
                             BITMAP_ID bgBmpId)
   : Base_Apheresis_Window(wcolor, win_ID, DisplayOrder_ClearBackground, bgBmpId),
     _keypad(0),
     _listbox(0),
     _title(title),
     _minText(0),
     _maxText(0)
{}



// DESTRUCTOR
Screen_Config::~Screen_Config()
{
   delete _keypad;
   delete _listbox;
   delete _minText;
   delete _maxText;
}



//
//
// -----------------------------------------------------------------------------
//            Resource allocation, deallocation
//
void Screen_Config::allocate_resources (const char* allocation_parameter)
{
   Base_Apheresis_Window::allocate_resources(allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources();
   btn_goback.deallocate_resources();
   btn_cancel.deallocate_resources();

   link_status_line_only();

   // set the text on the continue button
   btn_continue.set_string_id(textButtonLongConfigReturnToMainMenu);

   if (!_keypad)
      _keypad = new Config_Keypad(this, (Config_Keypad::KeypadCallback)&Screen_Config::keypad_return);

   if (!_listbox)
      _listbox = new Config_Listbox(this, (Config_Listbox::ListboxCallback)&Screen_Config::listbox_return);

}



void Screen_Config::deallocate_resources ()
{
   delete _minText;
   _minText = 0;

   delete _maxText;
   _maxText = 0;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   delete _keypad;
   _keypad = 0;

   delete _listbox;
   _listbox = 0;

   Base_Apheresis_Window::deallocate_resources();
}



//
//
// -----------------------------------------------------------------------------
//            Methods invoked by callbacks
//
void Screen_Config::process_continue_button ()
{
   goto_screen(GUI_SCREEN_CONFIG, "GUI_SCREEN_CONFIG");
}


//
//
// -----------------------------------------------------------------------------
//  These have no default implementation and should be overridden in
//  the derived class if keypad or listbox data is needed.
//
void Screen_Config::update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT&)
{}



void Screen_Config::update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT&)
{}



// --- and the hooks for virtuality
//
void Screen_Config::keypad_return (const KEYPAD_RETURN_STRUCT& return_keypad_buffer)
{
   update_screen_with_keypad_data(return_keypad_buffer);
}



void Screen_Config::listbox_return (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer)
{
   update_screen_with_listbox_data(rtn_listbox_buffer);
}



//
//
// -----------------------------------------------------------------------------
const TEXT_STRING_ID Screen_Config::get_text_string (const CONFIG_DATA_KEYS listbox_key,
                                                     const int key)
{
   TEXT_STRING_ID return_id = TEXT_NULL;

   int            index     = 0;
   while ((config_list_table[index].key_name) != LAST_CONFIG_INDEX_KEY)
   {
      if (((config_list_table[index].key_name) == listbox_key) &&
          ((config_list_table[index].list_name) == key))
      {
         return_id = (config_list_table[index].txtid);
         break;
      }

      index++; // keep looping until key_name is found

   }

   return return_id;
}



//
//
// -----------------------------------------------------------------------------
//
//
//   Display / undisplay keypad min and max range info
//
void Screen_Config::display_min_max_keypad_limits (const double min, const double max, const char* format, bool wideDisplay)
{
   guistring button_string;
   guistring min_string;
   guistring max_string;

   // Make sure we're not leaking text here.
   if (_minText)
   {
      delete _minText;
      _minText = NULL;
   }

   if (_maxText)
   {
      delete _maxText;
      _maxText = NULL;
   }
   if (wideDisplay)
   {
      _minText = new Display_Text(textMiscCfgMinValue2);
   }
   else
   {
      _minText = new Display_Text(textMiscCfgMinValue);
   }
   _minText->allocate_resources(*this);

   if (wideDisplay)
   {
      _maxText = new Display_Text(textMiscCfgMaxValue2);
   }
   else
   {
      _maxText = new Display_Text(textMiscCfgMaxValue);
   }
   _maxText->allocate_resources(*this);

   _minText->get_text_direct(min_string);
   sprintf_l10n(button_string, format, min);
   strcat(min_string, button_string);
   _minText->set_text_direct(strlen(min_string), min_string);

   _maxText->get_text_direct(max_string);
   sprintf_l10n(button_string, format, max);
   strcat(max_string, button_string);
   _maxText->set_text_direct(strlen(max_string), max_string);
}



void Screen_Config::turn_off_min_max_keypad_text ()
{
   delete _minText;
   _minText = NULL;

   delete _maxText;
   _maxText = NULL;
}

void Screen_Config::build_keypad_string (guistring& return_str_ptr, CONFIG_DATA_KEYS key_name, short x_pos, short y_pos, const char* format, double max, double min)
{
   build_keypad_string(return_str_ptr, key_name, x_pos, y_pos, format, max, min, guistring() /* empty prePopulateStr */);
}

void Screen_Config::build_keypad_string (guistring& return_str_ptr, CONFIG_DATA_KEYS key_name, short x_pos, short y_pos, const char* format, double max, double min, const guistring& prePopulateStr, TEXT_STRING_ID unitsId)
{
   char temp_str[80] = "";

   return_str_ptr.initialize(); // empty out string to start

   // keypad_name
   sprintf(temp_str, "%d", STANDARD);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_max_value
   sprintf(temp_str, "%f", max);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_min_value
   sprintf(temp_str, "%f", min);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_location_x
   sprintf(temp_str, "%d", x_pos);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_location_y
   sprintf(temp_str, "%d", y_pos);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_units_text_id_1
   sprintf(temp_str, "%d", unitsId);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_format_1
   strcpy(temp_str, format);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   if (prePopulateStr.length() > 0)
      sprintf(temp_str, prePopulateStr);
   else
      sprintf(temp_str, " ");

   // keypad_initial_value_1
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_data_type
   sprintf(temp_str, "%d", key_name);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);

   // keypad_zero_ok : 1 => true
   strcat(return_str_ptr, "1");

   DataLog (log_level_gui_info) << "keypad string thus far: " << return_str_ptr << endmsg;

   // If unitsId was provided, then use it's font size too
   if (unitsId != textEmpty)
   {
      strcat(return_str_ptr, STRING_TOKEN); // end of keypad_zero_ok

      // add null values for the parameters that are N/A
      sprintf(temp_str, "%s%s", BLANK_STRING, STRING_TOKEN);
      strcat(return_str_ptr, temp_str); // keypad_entry_type
      strcat(return_str_ptr, temp_str); // keypad_exponent
      strcat(return_str_ptr, temp_str); // keypad_units_text_id_2
      strcat(return_str_ptr, temp_str); // keypad_format_2
      strcat(return_str_ptr, temp_str); // keypad_initial_value_2
      strcat(return_str_ptr, temp_str); // keypad_hide_input

      // finally, set keypad_use_font_size_from_units = 1 => true
      strcat(return_str_ptr, "1");
      DataLog (log_level_gui_info) << "and with units id: " << return_str_ptr << endmsg;
   }
}



void Screen_Config::build_param_string (guistring& return_str_ptr, CONFIG_DATA_KEYS key_name, short X, short Y)
{
   char temp_str[80] = "";
   return_str_ptr.initialize(); // empty out string to start

   sprintf(return_str_ptr, "%d", key_name);
   strcat(return_str_ptr, STRING_TOKEN);
   sprintf(temp_str, "%d", X);
   strcat(return_str_ptr, temp_str);
   strcat(return_str_ptr, STRING_TOKEN);
   sprintf(temp_str, "%d", Y);
   strcat(return_str_ptr, temp_str);

}

void Screen_Config::disableConfigBtn (Button_Bitmap& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY)
{
   DataLog (log_level_gui_info) << "disableConfigBtn(Button_Bitmap): btnId="
                                << btn.get_button_id() << endmsg;

   btn.disable_callbacks();
   btn.set_disabled(true);
   btn.set_up_bitmap(newUpId);
   btn.set_button_text_color(GREY_LIGHT);
}

void Screen_Config::disableConfigBtn (ConfigButton& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY)
{
   DataLog (log_level_gui_info) << "disableConfigBtn(ConfigButton): btnId="
                                << btn.get_button_id() << endmsg;

   btn.disable_callbacks();
   btn.set_disabled(true);
   btn.set_up_bitmap(newUpId);
   btn.set_button_text_color(GREY_LIGHT);
}

void Screen_Config::disableConfigBtn (Config_textabove_button& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY)
{
   DataLog (log_level_gui_info) << "disableConfigBtn(Config_textabove_button): btnId="
                                << btn.get_button_id() << endmsg;

   btn.disable_callbacks();
   btn.set_disabled(true);
   btn.set_up_bitmap(newUpId);
   btn.set_button_text_color(GREY_LIGHT);
   btn.set_text_color(GREY_LIGHT);
}

void Screen_Config::disableConfigBtn (Config_button_with_text& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY)
{
   DataLog (log_level_gui_info) << "disableConfigBtn(Config_button_with_text): btnId="
                                << btn.get_button_id() << endmsg;

   btn.disable_callbacks();
   btn.set_disabled(true);
   btn.set_up_bitmap(newUpId);
   btn.set_button_text_color(GREY_LIGHT);
   btn.set_text_color(GREY_LIGHT);
}

void Screen_Config::disableConfigBtn (ToggleButton& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY, BITMAP_ID newDownId = BITMAP_CONFIG_BUTTON_GRAY_DOWN)
{
   DataLog (log_level_gui_info) << "disableConfigBtn(ToggleButton): btnId="
                                << btn.get_button_id() << endmsg;

   btn.disable_callbacks();
   btn.set_disabled(true);
   btn.set_up_bitmap(newUpId);
   btn.set_down_bitmap(newDownId);
   btn.set_button_text_color(GREY_LIGHT);
}

void Screen_Config::disableConfigTxtBox (BitmapTextBox& txtBox, BITMAP_ID newBmpId = BITMAP_NULL)
{
   if (newBmpId != BITMAP_NULL) txtBox.setBitmapId(newBmpId);
   txtBox.setTxtColor(GREY_LIGHT);
}

void Screen_Config::disableConfigTxt (Display_Text& txt, TEXT_COLOR disabledColor = GREY_LIGHT)
{
   txt.set_color(disabledColor);
}

void Screen_Config::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (_title);
}

/* FORMAT HASH d7814253894e3f94dbdbafd83f8f278c */
