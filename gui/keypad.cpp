/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_KEYPAD

*     File name:   keypad.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the dynamically allocated KEYPAD
                   screen
*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/keypad.cpp 1.74 2009/04/07 20:40:22Z dslausb Exp dslausb $
$Log: keypad.cpp $
Revision 1.74  2009/04/07 20:40:22Z  dslausb
IT 9101 - Where possible GUI_POOL_MALLOC
Revision 1.73  2008/10/13 20:25:11Z  dslausb
IT 8629 - Fix strange time button behaviour in stateless version of predict.
Revision 1.72  2008/09/12 18:08:21Z  dslausb
Fix CFR11 Reboot issue
Revision 1.71  2008/08/28 17:16:40Z  dslausb
Fix inconsistent decimal delimiters
Revision 1.70  2008/07/09 23:50:47Z  dslausb
More string ID standardization as requested by Marita
Revision 1.69  2008/07/09 20:35:31Z  dslausb
Fix a minor keypad init bug
Revision 1.68  2008/07/07 18:26:13Z  dslausb
More localization string id standardization
Revision 1.67  2008/07/03 20:17:26Z  dslausb
Further standardization of string IDs for localization
Revision 1.66  2008/07/02 00:38:59Z  dslausb
IT 8575 - Clean up some class-scoped variables that should be local in order to clear up some keypad issues
Revision 1.65  2008/07/01 22:50:14Z  dslausb
Fiddle with a bunch of poorly scoped variables in order to fix a keypad issue
Revision 1.64  2008/06/27 23:57:23Z  dslausb
IT 8733 - Fixed text layout problem in inlet flow rates keypad
Revision 1.63  2008/02/18 20:18:09Z  dslausb
Updates for string.info files
Revision 1.62  2007/08/21 18:56:32Z  dslausb
Changes to DRBC split config functionality
Revision 1.61  2007/08/10 22:25:16Z  dslausb
RBC config screen changes for DRBC auto split
Revision 1.60  2003/11/24 19:24:51Z  rm70006
IT 6624.  Remove unneeded debug logging.
Revision 1.59  2003/11/14 20:08:43Z  rm70006
IT 6360.  Add capability for security keypad for CFR 11.
Revision 1.58  2003/08/14 14:59:48Z  rm70006
IT 5841.  Add docked keypad capability.
Revision 1.57  2003/02/26 17:21:22Z  rm10919
IT 5890 - modified behavior of keypad to show all format chars in display at all times.
Revision 1.56  2003/01/06 18:31:14Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.55  2002/12/05 14:00:27  jl11312
- removed calls to ZafApplication::Flush, since they were causing recursive calls through the Zinc event handler
Revision 1.54  2002/11/27 20:24:15  jl11312
- reduced flickering on pop up
Revision 1.53  2002/11/25 18:38:30  jl11312
- fixed some problems with buttons, display text, and config screens
Revision 1.52  2002/11/22 16:43:52Z  sb07663
Updates to fix strtok anomalies
Revision 1.51  2002/11/11 06:31:12Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.49  2002/09/27 17:03:14  sb07663
fixed allocation problem if no alloc string given
Revision 1.48  2002/09/24 14:23:12  sb07663
keypad positioning changes
Revision 1.47  2002/09/17 15:40:56  sb07663
Derived off of base_window to avoid allocate/deallocate of unneeded buttons
Revision 1.46  2002/08/16 16:34:33  sb07663
added <vxworks.h> include
Revision 1.45  2002/08/06 18:58:26  ms10234
Changed the int messages to end with "Msg"
Revision 1.44  2002/06/05 14:32:51  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.43  2002/05/23 14:30:15  sb07663
Modifications for working bitmaps and buttons under Zinc
Revision 1.42  2002/05/07 13:58:10  sb07663
First cut at VxWorks compatibility
Revision 1.41  2001/06/20 14:13:25  sb07663
IT2397: include "text value out of range" for donor vitals and
donor information keypad entries
Revision 1.40  2000/06/06 19:35:27  ms10234
Messages were moved from the common project to the trima project
Revision 1.39  1997/05/29 15:52:16  MC03015
Removal of height calculation bug
Revision 1.38  1997/05/29 00:14:13  MC03015
Modification of max and min management to foat
Revision 1.37  1997/05/28 00:54:28  MC03015
Modification of return value logic to allow for floats return
Revision 1.36  1997/05/27 16:42:09  MC03015
Modifications to allow for YSF float value and a redefinition of text entry max and min
Revision 1.35  1997/05/22 00:44:26  MC03015
Modification of string management, placement in frmtstr
Revision 1.34  1997/05/17 01:53:48  MC03015
Debugging of blood type keypad
Revision 1.33  1997/05/09 06:56:41  MC03015
Blood type keypad modifications
Revision 1.32  1997/03/29 00:45:12  MC03015
Addition of blood type transmission
Revision 1.31  1997/03/15 06:23:18  MC03015
Removal of debug statements
Revision 1.30  1997/03/15 02:27:05  MC03015
Addition of new keypad table and parsing logic
Revision 1.29  1996/12/21 10:10:38  mc03015
Modified string buffer for return management
Revision 1.28  1996/12/20 09:02:32  mc03015
Modified return value after clear to -1
Revision 1.27  1996/12/15 04:34:29  mc03015
No substantial modifications
Revision 1.26  1996/10/29 09:46:02  MC03015
Modifications for screen init and refresh
Revision 1.25  1996/10/25 02:01:06  MC03015
Addition of return values for enter button press with no values entered
Revision 1.24  1996/10/17 09:12:09  MC03015
Removal of common header guitemp.h
Revision 1.23  1996/10/15 00:14:56  MC03015
Additional formatting and exclusive keystroke processing added for beta 2.3
Revision 1.22  1996/10/11 20:18:30  MC03015
Modifications to HEIGHT keypad entries
Revision 1.21  1996/10/10 02:12:18  Barry_Fulkerson
- changed config enums to point to the enum list in cfg_keys.h
Revision 1.20  1996/10/07 11:26:58  MC03015
Keypad modifications for PREDICT and debug
Revision 1.19  1996/09/24 22:18:06  MC03015
Changes for Beta 2.3, specifically text entry name modifications and additions.
Revision 1.14  1996/08/21 01:16:17  MC03015
beta 2.3 modifications for KEYPAD size only
Revision 1.13  1996/07/10 18:33:25  MC03015
addition of new header (log  and header info), maigic number removal, and string function review
*/

#include <vxworks.h>
#include "keypad.hpp"
#include "statline.hpp"


// CALLBACKs
static void callback_clear (void* data);
static void callback_9 (void* data);
static void callback_8 (void* data);
static void callback_7 (void* data);
static void callback_6 (void* data);
static void callback_5 (void* data);
static void callback_4 (void* data);
static void callback_3 (void* data);
static void callback_2 (void* data);
static void callback_1 (void* data);
static void callback_0 (void* data);
static void callback_enter (void* data);



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_KEYPAD::Screen_KEYPAD()
   : Base_Window (BITMAP_NULL),
     status_line_displayed(false),
     cleared(true),
     max_length_user_entry1(0),
     max_length_user_entry2(0),
     keypad_display_1_finished(false),
     keypad_display_2_finished(false),
     useInitVal1(false),
     useInitVal2(false),
     keypad_value_1(0.0),
     keypad_value_2(0.0),
     user_entry_buffer_1(),
     user_entry_buffer_2(),
     keypad_initialized_display_1(),
     keypad_initialized_display_2(),
     keypad_display_1(),
     keypad_display_2(),
     format_buffer(),

     keypad_name(NULL_KEYPAD),
     keypad_max_value(0.0),
     keypad_min_value(0.0),
     keypad_location_x(0),
     keypad_location_y(0),
     keypad_units_text_id_1(TEXT_NULL),
     keypad_units_text_id_2(TEXT_NULL),
     keypad_format_1(),
     keypad_format_2(),
     keypad_initial_value_1(0.0),
     keypad_initial_value_2(0.0),
     keypad_exponent(0),
     keypad_zero_ok(false),
     keypad_entry_type(NULL_ENTRY_TYPE),
     keypad_data_type(KEY_INACTIVE),
     keypad_button_clear_text_id(TEXT_NULL),
     keypad_button_0_text_id(TEXT_NULL),
     keypad_button_1_text_id(TEXT_NULL),
     keypad_button_2_text_id(TEXT_NULL),
     keypad_button_3_text_id(TEXT_NULL),
     keypad_button_4_text_id(TEXT_NULL),
     keypad_button_5_text_id(TEXT_NULL),
     keypad_button_6_text_id(TEXT_NULL),
     keypad_button_7_text_id(TEXT_NULL),
     keypad_button_8_text_id(TEXT_NULL),
     keypad_button_9_text_id(TEXT_NULL),
     keypad_button_enter_text_id(TEXT_NULL),
     keypad_hide_input(0),
     keypad_units_1_font_size(0),
     keypad_use_font_size_from_units(false),

     btn_clear     (GUI_BUTTON_KPAD_CLEAR,
                    BUTN_KPAD_CLEAR_X,
                    BUTN_KPAD_CLEAR_Y,
                    textEmpty,
                    BITMAP_BUTTON_KPAD_CLEAR_UP,
                    BITMAP_BUTTON_KPAD_CLEAR_DN),

     btn_9       (GUI_BUTTON_KPAD_9,
                  BUTN_KPAD_9_X,
                  BUTN_KPAD_9_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_9_UP,
                  BITMAP_BUTTON_KPAD_9_DN),

     btn_8       (GUI_BUTTON_KPAD_8,
                  BUTN_KPAD_8_X,
                  BUTN_KPAD_8_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_8_UP,
                  BITMAP_BUTTON_KPAD_8_DN),

     btn_7       (GUI_BUTTON_KPAD_7,
                  BUTN_KPAD_7_X,
                  BUTN_KPAD_7_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_7_UP,
                  BITMAP_BUTTON_KPAD_7_DN),

     btn_6       (GUI_BUTTON_KPAD_6,
                  BUTN_KPAD_6_X,
                  BUTN_KPAD_6_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_6_UP,
                  BITMAP_BUTTON_KPAD_6_DN),

     btn_5       (GUI_BUTTON_KPAD_5,
                  BUTN_KPAD_5_X,
                  BUTN_KPAD_5_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_5_UP,
                  BITMAP_BUTTON_KPAD_5_DN),

     btn_4       (GUI_BUTTON_KPAD_4,
                  BUTN_KPAD_4_X,
                  BUTN_KPAD_4_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_4_UP,
                  BITMAP_BUTTON_KPAD_4_DN),

     btn_3       (GUI_BUTTON_KPAD_3,
                  BUTN_KPAD_3_X,
                  BUTN_KPAD_3_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_3_UP,
                  BITMAP_BUTTON_KPAD_3_DN),

     btn_2       (GUI_BUTTON_KPAD_2,
                  BUTN_KPAD_2_X,
                  BUTN_KPAD_2_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_2_UP,
                  BITMAP_BUTTON_KPAD_2_DN),

     btn_1       (GUI_BUTTON_KPAD_1,
                  BUTN_KPAD_1_X,
                  BUTN_KPAD_1_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_1_UP,
                  BITMAP_BUTTON_KPAD_1_DN),

     btn_0       (GUI_BUTTON_KPAD_0,
                  BUTN_KPAD_0_X,
                  BUTN_KPAD_0_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_0_UP,
                  BITMAP_BUTTON_KPAD_0_DN),

     btn_enter   (GUI_BUTTON_KPAD_ENTER,
                  BUTN_KPAD_ENTER_X,
                  BUTN_KPAD_ENTER_Y,
                  textEmpty,
                  BITMAP_BUTTON_KPAD_ENTER_UP,
                  BITMAP_BUTTON_KPAD_ENTER_DN),

     text_user_entry_1 (textEmptyFormatKeypadUsrEntryInit),
     text_user_entry_2 (textEmptyFormatKeypadUsrEntryInit),
     text_exponent     (textKeypadUnitsExponentInit)
{
   Base_Window::set_screen_id(GUI_SCREEN_KEYPAD);
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_KEYPAD::~Screen_KEYPAD()
{
   Screen_KEYPAD::deallocate_resources();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_KEYPAD::allocate_resources (const char* allocation_parameter)
{
   // determine keypad location and units and buttonface text by parsing
   // allocation parameter
   decode_allocation_param (allocation_parameter);

   // set background bitmap for keypad to null
   set_background_bitmap(BITMAP_KEYPAD_BODY);

   //
   // establish location of the keypad.  This must be done before allocating
   // base window resources or the keypad will show up at (0,0).
   //
   set_position(keypad_location_x, keypad_location_y);
   Base_Window::allocate_resources(allocation_parameter);

   // Allocate the resources for the keypad buttons
   btn_clear.allocate_resources (*this);
   btn_9.allocate_resources (*this);
   btn_8.allocate_resources (*this);
   btn_7.allocate_resources (*this);
   btn_6.allocate_resources (*this);
   btn_5.allocate_resources (*this);
   btn_4.allocate_resources (*this);
   btn_3.allocate_resources (*this);
   btn_2.allocate_resources (*this);
   btn_1.allocate_resources (*this);
   btn_0.allocate_resources (*this);
   btn_enter.allocate_resources (*this);

   // Set the callbacks for the keypad buttons
   btn_clear.set_callback (callback_clear, (void*)this);
   btn_9.set_callback (callback_9, (void*)this);
   btn_8.set_callback (callback_8, (void*)this);
   btn_7.set_callback (callback_7, (void*)this);
   btn_6.set_callback (callback_6, (void*)this);
   btn_5.set_callback (callback_5, (void*)this);
   btn_4.set_callback (callback_4, (void*)this);
   btn_3.set_callback (callback_3, (void*)this);
   btn_2.set_callback (callback_2, (void*)this);
   btn_1.set_callback (callback_1, (void*)this);
   btn_0.set_callback (callback_0, (void*)this);
   btn_enter.set_callback (callback_enter, (void*)this);

   // initialize keypad location etc...
   screen_initialization ();

   // initialize keypad components, units, calling button and build button widgets etc...
   component_initialization ();
}  // END of Screen_KEYPAD ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_KEYPAD::deallocate_resources ()

{
   if (!_allocated)
      return;

   // Allocate the resources for the Donor Info button
   btn_clear.deallocate_resources ();
   btn_9.deallocate_resources ();
   btn_8.deallocate_resources ();
   btn_7.deallocate_resources ();
   btn_6.deallocate_resources ();
   btn_5.deallocate_resources ();
   btn_4.deallocate_resources ();
   btn_3.deallocate_resources ();
   btn_2.deallocate_resources ();
   btn_1.deallocate_resources ();
   btn_0.deallocate_resources ();
   btn_enter.deallocate_resources ();

   text_user_entry_1.deallocate_resources ();
   text_user_entry_2.deallocate_resources ();
   text_exponent.deallocate_resources ();

   if (status_line_displayed) guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // deallocate parent's resources
   Base_Window::deallocate_resources();

}   // END of Screen_KEYPAD DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DECODE_ALLOCATION_PARAM
void Screen_KEYPAD::decode_allocation_param (const char* param)
{
   useInitVal1 = false;
   useInitVal2 = false;

   // GUISTRING placeholders for allocation tokens.
   guistring
      keypad_max_value_alloc,
      keypad_min_value_alloc,
      keypad_location_x_alloc,
      keypad_location_y_alloc,
      keypad_units_text_id_1_alloc,
      keypad_units_text_id_2_alloc,
      keypad_format_1_alloc,
      keypad_format_2_alloc,
      keypad_initial_value_1_alloc,
      keypad_initial_value_2_alloc,
      keypad_exponent_alloc,
      keypad_zero_ok_alloc,
      keypad_entry_type_alloc,
      keypad_data_type_alloc,
      keypad_hide_input_alloc,
      keypad_use_font_size_from_units_alloc;

   // reset the local format buffers to length zero for copy
   keypad_format_1.initialize();
   keypad_format_2.initialize();

   // copy allocation string into buffer for parsing
   guistring paramcopy(param);
   char*     tokstr    = paramcopy.make_buffer();
   char*     nextToken = 0;

   // use the predefined string token to extract data elements from allocation parameter
   // if provided
   keypad_name = (KEYPAD_IDS)(atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken)));

   // parse the remainder of the string, if it exists
   keypad_max_value_alloc                = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_min_value_alloc                = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_location_x_alloc               = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_location_y_alloc               = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_units_text_id_1_alloc          = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_format_1_alloc                 = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_initial_value_1_alloc          = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_data_type_alloc                = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_zero_ok_alloc                  = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_entry_type_alloc               = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_exponent_alloc                 = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_units_text_id_2_alloc          = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_format_2_alloc                 = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_initial_value_2_alloc          = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_hide_input_alloc               = strtok_r(NULL, STRING_TOKEN, &nextToken);
   keypad_use_font_size_from_units_alloc = strtok_r(NULL, STRING_TOKEN, &nextToken);
   GUI_POOL_FREE(tokstr);

   int keypad_table_pointer = NULL_KEYPAD;

   // determine the appropriate table entry to extract keypad parameters
   for (int index = 0; index < NULL_KEYPAD; index++)
   {
      if ((keypads[index].keypad_name) == keypad_name)
      {
         keypad_table_pointer = index;
         break;
      }
   }

   // abort if the keypad is not found
   if (keypad_table_pointer == (int)NULL_KEYPAD)
   {
      //// DEFAULT PROCESSING ////
      _FATAL_ERROR (__FILE__, __LINE__, "Unknown keypad type requested");
      return;
   }

   // extract all table data to initialize keypad child screen
   keypad_name                 = keypads[keypad_table_pointer].keypad_name;
   keypad_max_value            = keypads[keypad_table_pointer].keypad_max_value;
   keypad_min_value            = keypads[keypad_table_pointer].keypad_min_value;
   keypad_location_x           = keypads[keypad_table_pointer].keypad_location_x;
   keypad_location_y           = keypads[keypad_table_pointer].keypad_location_y;
   keypad_units_text_id_1      = keypads[keypad_table_pointer].keypad_units_text_id_1;
   keypad_units_text_id_2      = keypads[keypad_table_pointer].keypad_units_text_id_2;
   keypad_format_1             = keypads[keypad_table_pointer].keypad_format_1;
   keypad_format_2             = keypads[keypad_table_pointer].keypad_format_2;
   keypad_initial_value_1      = keypads[keypad_table_pointer].keypad_initial_value_1;
   keypad_initial_value_2      = keypads[keypad_table_pointer].keypad_initial_value_2;
   keypad_exponent             = keypads[keypad_table_pointer].keypad_exponent;
   keypad_zero_ok              = keypads[keypad_table_pointer].keypad_zero_ok;
   keypad_entry_type           = keypads[keypad_table_pointer].keypad_entry_type;
   keypad_data_type            = keypads[keypad_table_pointer].keypad_data_type;
   keypad_button_clear_text_id = keypads[keypad_table_pointer].keypad_button_clear_text_id;
   keypad_button_9_text_id     = keypads[keypad_table_pointer].keypad_button_9_text_id;
   keypad_button_8_text_id     = keypads[keypad_table_pointer].keypad_button_8_text_id;
   keypad_button_7_text_id     = keypads[keypad_table_pointer].keypad_button_7_text_id;
   keypad_button_6_text_id     = keypads[keypad_table_pointer].keypad_button_6_text_id;
   keypad_button_5_text_id     = keypads[keypad_table_pointer].keypad_button_5_text_id;
   keypad_button_4_text_id     = keypads[keypad_table_pointer].keypad_button_4_text_id;
   keypad_button_3_text_id     = keypads[keypad_table_pointer].keypad_button_3_text_id;
   keypad_button_2_text_id     = keypads[keypad_table_pointer].keypad_button_2_text_id;
   keypad_button_1_text_id     = keypads[keypad_table_pointer].keypad_button_1_text_id;
   keypad_button_0_text_id     = keypads[keypad_table_pointer].keypad_button_0_text_id;
   keypad_button_enter_text_id = keypads[keypad_table_pointer].keypad_button_enter_text_id;
   keypad_hide_input           = keypads[keypad_table_pointer].keypad_hide_input;

   // check all allocation parameters passed in to override currently defined
   // defaults
   if ((keypad_max_value_alloc.length() > 0) && (strcmp(keypad_max_value_alloc, CHILD_OF_STATELESS) != 0))
   {
      if (strcmp(keypad_max_value_alloc, BLANK_STRING))
      {
         keypad_max_value = atof(keypad_max_value_alloc);
      }
   }

   if (keypad_min_value_alloc.length() > 0)
   {
      if (strcmp(keypad_min_value_alloc, BLANK_STRING))
      {
         keypad_min_value = atof(keypad_min_value_alloc);
      }
   }

   if (keypad_location_x_alloc.length() > 0)
   {
      if (strcmp(keypad_location_x_alloc, BLANK_STRING))
      {
         keypad_location_x = (short)(atoi(keypad_location_x_alloc));
      }
   }

   if (keypad_location_y_alloc.length() > 0)
   {
      if (strcmp(keypad_location_y_alloc, BLANK_STRING))
      {
         keypad_location_y = (short)(atoi(keypad_location_y_alloc));
      }
   }

   if (keypad_units_text_id_1_alloc.length() > 0)
   {
      if (strcmp(keypad_units_text_id_1_alloc, BLANK_STRING))
      {
         keypad_units_text_id_1 = (TEXT_STRING_ID)(atoi(keypad_units_text_id_1_alloc));
      }
   }

   if (keypad_units_text_id_2_alloc.length() > 0)
   {
      if (strcmp(keypad_units_text_id_2_alloc, BLANK_STRING))
      {
         keypad_units_text_id_2 = (TEXT_STRING_ID)(atoi(keypad_units_text_id_2_alloc));
      }
   }

   if (keypad_format_1_alloc.length() > 0)
   {
      if (strcmp(keypad_format_1_alloc, BLANK_STRING))
      {
         strcpy(keypad_format_1, keypad_format_1_alloc);
      }
   }

   if (keypad_format_2_alloc.length() > 0)
   {
      if (strcmp(keypad_format_2_alloc, BLANK_STRING))
      {
         strcpy(keypad_format_2, keypad_format_2_alloc);
      }
   }

   if (keypad_initial_value_1_alloc.length() > 0)
   {
      if (strcmp(keypad_initial_value_1_alloc, BLANK_STRING))
      {
         useInitVal1            = true;
         keypad_initial_value_1 = atof(keypad_initial_value_1_alloc);
      }
   }

   if (keypad_initial_value_2_alloc.length() > 0)
   {
      if (strcmp(keypad_initial_value_2_alloc, BLANK_STRING))
      {
         useInitVal2            = true;
         keypad_initial_value_2 = atof(keypad_initial_value_2_alloc);
      }
   }

   if (keypad_exponent_alloc.length() > 0)
   {
      if (strcmp(keypad_exponent_alloc, BLANK_STRING))
      {
         keypad_exponent = (int)(atoi(keypad_exponent_alloc));
      }
   }

   if (keypad_zero_ok_alloc.length() > 0)
   {
      if (strcmp(keypad_zero_ok_alloc, BLANK_STRING))
      {
         keypad_zero_ok = (bool)(atoi(keypad_zero_ok_alloc));
      }
   }

   if (keypad_entry_type_alloc.length() > 0)
   {
      if (strcmp(keypad_entry_type_alloc, BLANK_STRING))
      {
         keypad_entry_type = (KEYPAD_ENTRY_DIRECTION)(atoi(keypad_entry_type_alloc));
      }
   }

   if (keypad_data_type_alloc.length() > 0)
   {
      if (strcmp(keypad_data_type_alloc, BLANK_STRING))
      {
         keypad_data_type = (CONFIG_DATA_KEYS)(atoi(keypad_data_type_alloc));
      }
   }

   if (keypad_hide_input_alloc.length() > 0)
   {
      if (strcmp(keypad_hide_input_alloc, BLANK_STRING))
      {
         keypad_hide_input = (CONFIG_DATA_KEYS)(atoi(keypad_hide_input_alloc));
      }
   }

   if (keypad_use_font_size_from_units_alloc.length() > 0)
   {
      if (strcmp(keypad_use_font_size_from_units_alloc, BLANK_STRING))
      {
         keypad_use_font_size_from_units = (bool)(atoi(keypad_use_font_size_from_units_alloc));
      }
   }
}   // END of Screen_KEYPAD DECODE_ALLOCATION_PARAM



////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION
void Screen_KEYPAD::screen_initialization ()
{
   //////////////////////////////////////////////////////////////////////////
   // values for initialization

   // initialize keypad status
   // these are determined at construction because dynamic, so initialized
   // every time,(i.e. allocated = constructed), therefore no screen
   // initialization outside of allocation

   // allocate the resources for user entry text
   text_user_entry_1.allocate_resources (*this);
   text_user_entry_2.allocate_resources (*this);

   if (keypad_use_font_size_from_units)
   {
      Display_Text text_units_1(keypad_units_text_id_1);
      keypad_units_1_font_size = text_units_1.getFontSize();
      text_user_entry_1.setFontSize(keypad_units_1_font_size);
      text_user_entry_2.setFontSize(keypad_units_1_font_size);
   }

   // establish location of the user entry text
   text_user_entry_1.set_requested_region(OSRegion(KEYPAD_TEXT1_POS_X, KEYPAD_TEXT1_POS_Y, KEYPAD_TEXT1_SIZE_W, KEYPAD_TEXT1_SIZE_H));
   text_user_entry_2.set_requested_region(OSRegion(KEYPAD_TEXT2_POS_X, KEYPAD_TEXT2_POS_Y, KEYPAD_TEXT2_SIZE_W, KEYPAD_TEXT2_SIZE_H));

   // initialize all strings used by the keypad
   string_initialization ();

   // end of initialization requirements*************************************
   //////////////////////////////////////////////////////////////////////////

}   // End of SCREEN_INITIALIZATION


////////////////////////////////////////////////////////////////////////////////
// COMPONENT_INITIALIZATION

void Screen_KEYPAD::component_initialization ()

{

   //////////////////////////////////////////////////////////////////////////
   // values for initialization

   // initialize keypad elements
   // these are determined at construction because dynamic, so initialized
   // every time,(i.e. allocated = constructed), therefore no screen
   // initialization outside of allocation

   // prepare the non-units portion of the display by parsing the format
   // statement, get the units and position text by doing a virtual clear.
   reset_display ();

   // set the button face texts, if no text exists deactivate the button
   btn_clear.set_string_id(keypad_button_clear_text_id);

   if (keypad_button_9_text_id != TEXT_NULL)
   {
      btn_9.set_string_id(keypad_button_9_text_id);
   }
   else
   {
      btn_9.deallocate_resources ();
   }

   if (keypad_button_8_text_id != TEXT_NULL)
   {
      btn_8.set_string_id(keypad_button_8_text_id);
   }
   else
   {
      btn_8.deallocate_resources ();
   }

   if (keypad_button_7_text_id != TEXT_NULL)
   {
      btn_7.set_string_id(keypad_button_7_text_id);
   }
   else
   {
      btn_7.deallocate_resources ();
   }

   if (keypad_button_6_text_id != TEXT_NULL)
   {
      btn_6.set_string_id(keypad_button_6_text_id);
   }
   else
   {
      btn_6.deallocate_resources ();
   }

   if (keypad_button_5_text_id != TEXT_NULL)
   {
      btn_5.set_string_id(keypad_button_5_text_id);
   }
   else
   {
      btn_5.deallocate_resources ();
   }

   if (keypad_button_4_text_id != TEXT_NULL)
   {
      btn_4.set_string_id(keypad_button_4_text_id);
   }
   else
   {
      btn_4.deallocate_resources ();
   }

   if (keypad_button_3_text_id != TEXT_NULL)
   {
      btn_3.set_string_id(keypad_button_3_text_id);
   }
   else
   {
      btn_3.deallocate_resources ();
   }

   if (keypad_button_2_text_id != TEXT_NULL)
   {
      btn_2.set_string_id(keypad_button_2_text_id);
   }
   else
   {
      btn_2.deallocate_resources ();
   }

   if (keypad_button_1_text_id != TEXT_NULL)
   {
      btn_1.set_string_id(keypad_button_1_text_id);
   }
   else
   {
      btn_1.deallocate_resources ();
   }

   if (keypad_button_0_text_id != TEXT_NULL)
   {
      btn_0.set_string_id(keypad_button_0_text_id);
   }
   else
   {
      btn_0.deallocate_resources ();
   }

   btn_enter.set_string_id(keypad_button_enter_text_id);

   // process values passed in for display initialization
   display_initialization_values();

   // end of initialization requirements*************************************
   //////////////////////////////////////////////////////////////////////////
   status_line_displayed = false;

}   // End of COMPONENT_INITIALIZATION


////////////////////////////////////////////////////////////////////////////////
// STRING_INITIALIZATION

void Screen_KEYPAD::string_initialization ()

{

   //////////////////////////////////////////////////////////////////////////
   // values for string initialization

   // reset the buffers to length zero to execute clear
   keypad_display_1.initialize();
   keypad_display_2.initialize();
   user_entry_buffer_1.initialize();
   user_entry_buffer_2.initialize();
   keypad_initialized_display_1.initialize();
   keypad_initialized_display_2.initialize();
   format_buffer.initialize();

   // end of string initialization requirements******************************
   //////////////////////////////////////////////////////////////////////////

}   // End of STRING_INITIALIZATION


////////////////////////////////////////////////////////////////////////////////
// DISPLAY_INITIALIZATION_VALUES

void Screen_KEYPAD::display_initialization_values ()

{
   if (useInitVal2)
   {
      // convert current entry
      sprintf(user_entry_buffer_2, "%d", (int)keypad_initial_value_2);

      // update display with NULL last user entry
      display_filter ("");
   }

   if (useInitVal1)
   {
      // convert current entry
      sprintf(user_entry_buffer_1, "%d", (int)keypad_initial_value_1);

      // update display with NULL last user entry
      display_filter ("");
   }
}   // End of DISPLAY_INITIALIZATION_VALUES


////////////////////////////////////////////////////////////////////////////////
// FORMAT_DISPLAY

void Screen_KEYPAD::format_display ()

{

   //////////////////////////////////////////////////////////////////////////
   // values for formatted display initialization

   int format_index = 0;                   // index to traverse format buffer

   // reset the user maximum length entry
   max_length_user_entry1 = 0;
   max_length_user_entry2 = 0;

   // reset the user entry buffer to length zero
   format_buffer.initialize();

   // copy format string into buffer for processing
   strcpy(format_buffer, keypad_format_1);

   // traverse the buffer to obtain length information and to convert into
   // display format
   keypad_initialized_display_1.initialize();

   while (format_buffer[format_index] != '\0')
   {

      // if formatting character, change to blank, increment entry length allowed
      if (format_buffer[format_index] == NUMBER_FORMAT_CHAR)
      {
         keypad_initialized_display_1 += BLANK_CHAR;
         max_length_user_entry1       += 1;
      }
      else  // else copy into dispaly buffer
      {
         keypad_initialized_display_1 += format_buffer[format_index];
      }

      format_index += 1;

   }

   // index to traverse format buffer
   format_index = 0;

   // reset the user entry buffer to length zero
   format_buffer.initialize();

   // copy format string into buffer for processing
   strcpy(format_buffer, keypad_format_2);

   // traverse the buffer to obtain length information and to convert into
   // display format
   keypad_initialized_display_2.initialize();
   while (format_buffer[format_index] != '\0')
   {

      // if formatting character, change to blank, increment entry length allowed
      if (format_buffer[format_index] == NUMBER_FORMAT_CHAR)
      {
         keypad_initialized_display_2 += BLANK_CHAR;
         max_length_user_entry2       += 1;
      }
      else  // else copy into display buffer
      {
         keypad_initialized_display_2 += format_buffer[format_index];
      }

      format_index += 1;

   }

   // end of initialization requirements*************************************
   //////////////////////////////////////////////////////////////////////////

}   // End of FORMAT_DISPLAY



////////////////////////////////////////////////////////////////////////////////
// SET_EXPONENT
void Screen_KEYPAD::set_exponent ()
{
   // reset displays of exponent to NULL for rebuild
   guistring keypad_exponent_text;

   // prepare exponent for display in text
   sprintf(keypad_exponent_text, "%d", keypad_exponent);

   // allocate and set the exponent text
   text_exponent.allocate_resources (*this);
   text_exponent.set_text_direct(keypad_exponent_text.length(), keypad_exponent_text);

   // define the dimensions for the exponent
   text_exponent.set_requested_region(OSRegion(KEYPAD_TEXT_EXP_POS_X, KEYPAD_TEXT_EXP_POS_Y, KEYPAD_TEXT_EXP_SIZE_W, KEYPAD_TEXT_EXP_SIZE_H));

   // if we have a exponent we need to move the user entry text to the left
   text_user_entry_1.set_requested_region(OSRegion(KEYPAD_TEXT1_POS_X, KEYPAD_TEXT1_POS_Y, KEYPAD_TEXT1_SIZE_W - 14, KEYPAD_TEXT1_SIZE_H));

}   // End of SET_EXPONENT



////////////////////////////////////////////////////////////////////////////////
// RESET_DISPLAY
void Screen_KEYPAD::reset_display ()
{
   // the display is virtually cleared
   cleared = true;

   // initialize the flags that check for completion of individual user
   // keypad entries
   keypad_display_1_finished = false;
   keypad_display_2_finished = false;

   guistring keypad_units_1, keypad_units_2;

   // reset all string buffers to length zero to clear
   string_initialization ();

   // if the exponent is not 0 display
   if (keypad_exponent > 0)
   {
      set_exponent ();
   }

   // copy format string into display buffers and capture maximum allowable
   // string length.
   format_display ();

   // now display the results of the reset
   Display_Text text_units_1(keypad_units_text_id_1), text_units_2(keypad_units_text_id_2);

   // obtain the first units for concatenation
   text_units_1.get_text_direct(keypad_units_1);
   // obtain the second units for concatenation
   text_units_2.get_text_direct(keypad_units_2);

   // concatenate the units to the initialized keypad entry
   strcat(keypad_initialized_display_1, keypad_units_1);

   keypad_initialized_display_1.swapOutDelimiter();

   // display the units now with or without exponents
   text_user_entry_1.set_text_direct(keypad_initialized_display_1.length(), keypad_initialized_display_1);

   // concatenate the units to the initialized keypad entry
   strcat(keypad_initialized_display_2, keypad_units_2);

   keypad_initialized_display_2.swapOutDelimiter();

   // display the units now with or without exponents
   text_user_entry_2.set_text_direct(keypad_initialized_display_2.length(), keypad_initialized_display_2);

}   // END of Screen_KEYPAD RESET_DISPLAY



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_FILTER
void Screen_KEYPAD::display_filter (const char* display_request)
{
   guistring last_user_entry;  // last user entry parameter
   // for manipulation
   // reset displays of last user entry to NULL for copy
   last_user_entry.initialize();

   // copy in parameter per function cal
   strcpy(last_user_entry, display_request);

   // apply pre-display filtering based on unique keypad type
   switch (keypad_name)
   {
      case STANDARD_FLOAT :
      {
         user_entry_buffer_1.initialize();

         if (keypad_initial_value_1 != 0.0)
         {
            Display_Text tmpTxt((TEXT_STRING_ID)keypad_initial_value_1);
            tmpTxt.get_text_direct(last_user_entry);

            // reset the initial value to 0 so the initial value is used only
            // once and upon allocation of the keypad
            keypad_initial_value_1 = 0.0;

         }
      }
      break;

      case HEIGHT_US :
      {
         if (keypad_display_2_finished == true)
         {
            if ((user_entry_buffer_1[0] != ONE) && (user_entry_buffer_1[0] != '\0'))
            {
               keypad_display_1_finished = true;
            }
            else if ((user_entry_buffer_1[0] == ONE) &&
                     (last_user_entry[0] != ONE)     &&
                     (last_user_entry[0] != ZERO)    &&
                     (last_user_entry[0] != '\0'))
            {
               keypad_display_1_finished = true;
            }
         }
      }
      break;

      case BLOOD_TYPE :
      {

         // clear the user entry buffer, which is the default placement for
         // initialization of variables
         user_entry_buffer_1.initialize();

         if (keypad_initial_value_1 != 0.0)
         {
            Display_Text tmpTxt((TEXT_STRING_ID)((int)keypad_initial_value_1));
            tmpTxt.get_text_direct(last_user_entry);

            // reset the initial value to 0 so the initial value is used only
            // once and upon allocation of the keypad
            keypad_initial_value_1 = 0.0;

         }

      }
      break;

      default :
         break;
   }  // End of switch (keypad...

   // add user entry to buffer and modify the display if within bounds of
   // maximum entry length
   if (((strlen(user_entry_buffer_2) + strlen(last_user_entry)) <= max_length_user_entry2) &&
       (max_length_user_entry2 != 0))
   {
      // update the display as appropriate
      update_display (last_user_entry, user_entry_buffer_2, keypad_display_2, keypad_initialized_display_2);

      // set the display text now
      text_user_entry_2.set_text_direct(keypad_display_2.length(), keypad_display_2);

      // remove all blanks for string conversion if data has been entered by user
      if (strlen(user_entry_buffer_2) > 0)
      {
         keypad_display_2.strip_blanks();
      }

      // capture user input for return
      keypad_value_2 = atof(keypad_display_2);
   }
   else
   {
      // finished with entry two
      keypad_display_2_finished = true;
   }       // End of else if ((strlen(...

   // add user entry to buffer and modify the display if within bounds of
   // maximum entry length
   if ( ((strlen(user_entry_buffer_1) + strlen(last_user_entry)) <= max_length_user_entry1) &&
        (!keypad_display_1_finished) &&
        (keypad_display_2_finished)
        )
   {
      // update the display as appropriate
      update_display (last_user_entry, user_entry_buffer_1, keypad_display_1, keypad_initialized_display_1);

      if (keypad_hide_input)
      {
         // Replace each key with a '*'
         guistring hiddenText;
         hiddenText.initialize();

         for (int i = 0; i < user_entry_buffer_1.length(); i++)
         {
            hiddenText += "*";
         }

         text_user_entry_1.set_text_direct(hiddenText.length(), hiddenText);
      }
      else
      {
         // set the display text now
         text_user_entry_1.set_text_direct(keypad_display_1.length(), keypad_display_1);
      }

      // remove all blanks for string conversion if data has been entered by user
      if (strlen(user_entry_buffer_1) > 0)
      {
         keypad_display_1.strip_blanks();
      }

      // capture user input for return (make sure we have a useable decimal delimiter first).
      guistring tmpStr = keypad_display_1;
      tmpStr.swapOutDelimiter(guistring::COMMA_TO_PERIOD);
      keypad_value_1 = atof(tmpStr);

   }  // End of else if ((strlen(...

   // apply post-display filtering based on unique keypad type
   if (keypad_name == HEIGHT_US)
   {
      guistring tmpStr1 = keypad_display_1;
      guistring tmpStr2 = keypad_display_2;
      tmpStr1.swapOutDelimiter(guistring::COMMA_TO_PERIOD);
      tmpStr2.swapOutDelimiter(guistring::COMMA_TO_PERIOD);

      // calculate return value
      keypad_value_1 = atof(tmpStr1) + (12.0) * atof(tmpStr2);
   }  // End of switch (keypad...
}



////////////////////////////////////////////////////////////////////////////////
// UPDATE_DISPLAY
void Screen_KEYPAD::update_display (guistring& last_user_entry,
                                    guistring& user_entry_buffer,
                                    guistring& keypad_display,
                                    guistring& keypad_initialized_display)
{
   int display_index    = 0;
   int user_entry_index = 0;

   // set the cleared flag to false, i.e. data entered
   cleared = false;

   // set the display string to NULL in preparation of copy
   keypad_display.initialize();

   // append the user entry to the current entry and update
   strncpy(keypad_display, keypad_initialized_display, strlen(keypad_initialized_display));

   // append the user entry to the current entry and update
   strncat(user_entry_buffer, last_user_entry, strlen(last_user_entry));

   // reverse strings for concatenation if right to left
   if (keypad_entry_type == RIGHTTOLEFT)
   {
      keypad_display.reverse();
      user_entry_buffer.reverse();
   }

   guistring tempdisplay; // = keypad_display;

   // the buffer to obtain length information and to convert into
   // display format
   while ((user_entry_buffer[user_entry_index] != '\0') && (keypad_display[display_index] != '\0'))
   {

      // if blank, add character increment
      if (keypad_display[display_index] == BLANK_CHAR)
      {
         tempdisplay      += user_entry_buffer[user_entry_index];
         user_entry_index += 1;
      }
      else
      {
         tempdisplay += keypad_display[display_index];
      }

      display_index++;

   }
   //
   //  Add format or unit characters to string
   //
   while (display_index < (strlen(keypad_initialized_display)))
   {
      tempdisplay += keypad_display[display_index];
      display_index++;
   }

   //
   //  Now copy display string back to original
   //
   keypad_display = tempdisplay;

   // re-reverse strings for concatenation if implementation was right to left
   if (keypad_entry_type == RIGHTTOLEFT)
   {
      keypad_display.reverse();
      user_entry_buffer.reverse();
   }

}   // END of Screen_KEYPAD UPDATE_DISPLAY



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLEAR_BUTTON
void Screen_KEYPAD::process_clear_button ()
{
   // update the display by refreshing the units
   reset_display();

   // update display with NULL last user entry
   display_filter ("");

   // the display is virtually cleared
   cleared = true;

}   // END of Screen_KEYPAD PROCESS_CLEAR_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_9
void Screen_KEYPAD::process_button_9 ()
{
   // reset buffer
   guistring button_text;

   // copy entry into string buffer
   btn_9.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_9


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_8

void Screen_KEYPAD::process_button_8 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_8.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_8


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_7

void Screen_KEYPAD::process_button_7 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_7.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_7


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_6

void Screen_KEYPAD::process_button_6 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_6.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_6


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_5

void Screen_KEYPAD::process_button_5 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_5.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_5


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_4

void Screen_KEYPAD::process_button_4 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_4.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_4


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_3

void Screen_KEYPAD::process_button_3 ()

{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_3.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_3


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_2

void Screen_KEYPAD::process_button_2 ()
{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_2.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_2


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_1
void Screen_KEYPAD::process_button_1 ()
{
   guistring button_text;

   // reset buffer
   button_text.initialize();

   // copy entry into string buffer
   btn_1.get_text_direct(button_text);

   // submit for update
   display_filter(button_text);

}   // END of Screen_KEYPAD PROCESS_BUTTON_1



////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_0
void Screen_KEYPAD::process_button_0 ()
{
   // only place a zero in the string if its not the first entry
   if (!cleared || keypad_zero_ok)
   {
      guistring button_text;

      // reset buffer
      button_text.initialize();

      // copy entry into string buffer
      btn_0.get_text_direct(button_text);

      // submit for update
      display_filter(button_text);
   }

}   // END of Screen_KEYPAD PROCESS_BUTTON_0



////////////////////////////////////////////////////////////////////////////////
// PROCESS_ENTER_BUTTON
void Screen_KEYPAD::process_enter_button ()
{
   KEYPAD_RETURN_STRUCT           keypad_struct;
   Message<KEYPAD_RETURN_STRUCT>  Msg_Keypad_Set(MessageBase::SEND_LOCAL);
   RemoveMostRecentChildScreenMsg scrnmsg(MessageBase::SEND_LOCAL);

   //
   //
   // Remove status line in case it is displayed
   //
   if (status_line_displayed)
   {
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
      status_line_displayed = false;
   }

   // if it is a standard numeric keypad, then check max and min and return
   if ( (keypad_value_1 >= keypad_min_value) &&
        (keypad_value_1 <= keypad_max_value) &&
        (!cleared)
        )
   {
      // Deal with decimal delimiters
      keypad_struct.keypad_type               = keypad_name;
      keypad_struct.config_data_type          = keypad_data_type;
      keypad_struct.keypad_return_string      = keypad_display_1;
      keypad_struct.keypad_return_string.swapOutDelimiter(guistring::COMMA_TO_PERIOD);
      keypad_struct.keypad_return_value       = (int)keypad_value_1;
      keypad_struct.keypad_return_value_float = (float)keypad_value_1;

      // send value back to active screen
      Msg_Keypad_Set.send(keypad_struct);

      // remove most recent child screen
      scrnmsg.send(0);
   }
   // if it is a non-numeric keypad then max and min are equal and not
   // cleared and equal to default.
   else if ((TEXT_KEYPAD == (int)keypad_min_value) &&
            (TEXT_KEYPAD == (int)keypad_max_value) &&
            (!cleared))
   {

      // populate the structure with the keypad display and "zero" values
      keypad_struct.keypad_type               = keypad_name;
      keypad_struct.config_data_type          = keypad_data_type;
      keypad_struct.keypad_return_string      = keypad_display_1;
      keypad_struct.keypad_return_value       = 0;
      keypad_struct.keypad_return_value_float = 0.0;

      // send value back to active screen
      Msg_Keypad_Set.send(keypad_struct);

      // remove most recent child screen
      scrnmsg.send(0);
   }
   // if it is either a non-numeric or numeric keypad and entry is cleared
   // then return nothing.
   else if (cleared)
   {
      // populate the structure with a "null" display string and "null" values
      // (= -1 for a non-numeric keypad; okay since all keypad entries are
      // positive integers)
      keypad_struct.keypad_type               = keypad_name;
      keypad_struct.config_data_type          = keypad_data_type;
      keypad_struct.keypad_return_string.initialize();
      keypad_struct.keypad_return_value       = -1;
      keypad_struct.keypad_return_value_float = -1.0;

      // send value back to active screen
      Msg_Keypad_Set.send(keypad_struct);

      // remove most recent child screen
      scrnmsg.send(0);
   }
   // if it doesn't match any of the above criteria, clear entry field
   else
   {
      process_clear_button();
      //
      //
      //  Put up the status line (assumes proper parent)
      //
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatValueOutOfRange);
      status_line_displayed = true;
   }

}   // END of Screen_KEYPAD PROCESS_ENTER_BUTTON



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_CLEAR
void callback_clear (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_clear_button ();

}   // END of CALLBACK_CLEAR



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_9
void callback_9 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_9 ();

}   // END of CALLBACK_9



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_8
void callback_8 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_8 ();

}   // END of CALLBACK_8



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_7
void callback_7 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_7 ();

}   // END of CALLBACK_7



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_6
void callback_6 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_6 ();

}   // END of CALLBACK_6


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_5
void callback_5 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_5 ();

}   // END of CALLBACK_5



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_4
void callback_4 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_4 ();

}   // END of CALLBACK_4



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_3
void callback_3 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_3 ();

}   // END of CALLBACK_3



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_2
void callback_2 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_2 ();

}   // END of CALLBACK_2



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_1
void callback_1 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_1 ();
}   // END of CALLBACK_1



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_0
void callback_0 (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_button_0 ();
}   // END of CALLBACK_0



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ENTER
void callback_enter (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_KEYPAD*)data)->process_enter_button ();
}  // END of CALLBACK_ENTER



//
// Docked Keypad
//


//
//
//
void Docked_Keypad::allocate_resources (OSWindow& parent, const char* format_string)
{
   // Attach the window to the parent
   attach(&parent);

   Screen_KEYPAD::allocate_resources(format_string);
}



//
//
//
void Docked_Keypad::deallocate_resources ()
{
   Screen_KEYPAD::deallocate_resources();
}

/* FORMAT HASH 3a98a9dec373162af5e65b7338a0ad9d */
