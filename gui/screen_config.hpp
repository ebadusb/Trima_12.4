/************************> FOCUSSED GUI DOMAIN <****************************

 *     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

 *     Author:      Barry Fulkerson

 *     Class name:  Screen_CONFIG

 *     File name:   screen_config.hpp

 *     Contents:    See prototype header descriptions below...

 *     Description:


*************************< FOCUSSED GUI DOMAIN >****************************/


// Single include define guardian
#ifndef SCREEN_CONFIG_HPP
#define SCREEN_CONFIG_HPP

#include "aphwind.hpp"
#include "config_keypad.hpp"
#include "config_listbox.hpp"
#include "operator_config.h"
#include "cfg_look.h"
#include "guistring.h"
#include "cfg_textabove_button.h"
#include "toggleButton.hpp"
#include "bitmapTextBox.h"

#define CHOOSE_DISABLED_BITMAP(__SETTING__) ((__SETTING__) ? BITMAP_CONFIG_BUTTON_GRAY_DOWN : BITMAP_CONFIG_BUTTON_GRAY)

//
///////////////////////////////////////////////////////////////////
//

class Screen_Config
   : public Base_Apheresis_Window
{
private:
   Screen_Config();                                  // force use of nondefault constructor
   Screen_Config(const Screen_Config&);              // disallowed
   Screen_Config& operator = (const Screen_Config&); // disallowed

protected:

   // CONSTRUCTOR
   Screen_Config(WINDOW_COLOR wcolor,
                 GUI_SCREEN_ID win_ID,
                 TEXT_STRING_ID title,
                 BITMAP_ID bgBmpId = BITMAP_DEFAULT_BACKGROUND);

   // DESTRUCTOR
   virtual ~Screen_Config();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   //
   //
   //   Methods invoked by callbacks
   //
   virtual void process_continue_button ();

   //
   //
   //  These have no default implementation and should be overridden in
   //  the derived class if keypad or listbox data is needed.
   //
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   //
   //
   //   Miscellaneous helper functions for derived classes
   //
   virtual const TEXT_STRING_ID get_text_string (const CONFIG_DATA_KEYS listbox_key,
                                                 const int key);

public:
   //
   //
   //   Display / undisplay keypad min and max range info
   //
   virtual void display_min_max_keypad_limits (const double min, const double max, const char* format, bool wideDisplay = false);
   virtual void turn_off_min_max_keypad_text ();

   static void build_keypad_string (guistring& return_str_ptr, CONFIG_DATA_KEYS key_name, short x_pos, short y_pos, const char* format, double max, double min);
   static void build_keypad_string (guistring& return_str_ptr, CONFIG_DATA_KEYS key_name, short x_pos, short y_pos, const char* format, double max, double min, const guistring& prePopulateStr, TEXT_STRING_ID unitsId = textEmpty);
   static void build_param_string  (guistring& return_string,  CONFIG_DATA_KEYS key_name, short X, short Y);

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

protected:

   //
   //
   //  These methods are invoked by the notify methods of the keypad and
   //  listbox objects.  Note nonvirtuality since we pass 'this' to the objects.
   //
   void keypad_return (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   void listbox_return (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   guistring keypad_string;

protected:
   Config_Keypad*  _keypad;   // pointer to ensure not instantiated until needed
   Config_Listbox* _listbox;  // pointer to ensure not instantiated until needed
   TEXT_STRING_ID  _title;
   Display_Text*   _minText;
   Display_Text*   _maxText;

   // Copy of the current configuration.
   static Operator_Config operator_config;

   static void disableConfigBtn (Button_Bitmap& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY);
   static void disableConfigBtn (ConfigButton& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY);
   static void disableConfigBtn (Config_textabove_button& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY);
   static void disableConfigBtn (Config_button_with_text& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY);
   static void disableConfigBtn (ToggleButton& btn, BITMAP_ID newUpId = BITMAP_CONFIG_BUTTON_GRAY, BITMAP_ID newDownId = BITMAP_CONFIG_BUTTON_GRAY_DOWN);
   static void disableConfigTxt (Display_Text& txt, TEXT_COLOR disabledColor = GREY_LIGHT);
   static void disableConfigTxtBox (BitmapTextBox& txtBox, BITMAP_ID newBmpId = BITMAP_NULL);
};

#endif

/* FORMAT HASH 1e0a5ecdbbb83822ab07d9730ad0a8a0 */
