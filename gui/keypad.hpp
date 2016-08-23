/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_KEYPAD

*     File name:   keypad.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for keypad.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/keypad.hpp 1.48 2008/07/01 22:50:19Z dslausb Exp dslausb $
$Log: keypad.hpp $
Revision 1.48  2008/07/01 22:50:19Z  dslausb
Fiddle with a bunch of poorly scoped variables in order to fix a keypad issue
Revision 1.48  2008/07/01 22:33:12Z  dslausb
IT 8575 - Clean up some class-scoped variables that should be local in order to clear up some keypad issues
Revision 1.47  2008/06/27 23:57:29Z  dslausb
IT 8733 - Fixed text layout problem in inlet flow rates keypad
Revision 1.46  2008/02/18 20:18:14Z  dslausb
Updates for string.info files
Revision 1.45  2003/11/14 20:08:58Z  rm70006
IT 6360.  Add capability for security keypad for CFR 11.
Revision 1.44  2003/08/14 14:56:03Z  rm70006
IT 5841.  Add ability for docked keypad.
Revision 1.43  2003/07/18 20:03:49Z  jl11312
- updated power response defines
- removed references to safproc.hpp
Revision 1.42  2003/01/06 18:31:14Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.41  2002/11/22 16:43:53  sb07663
Updates to fix strtok anomalies
Revision 1.40  2002/11/11 06:33:38Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.39  2002/09/24 14:23:21  sb07663
keypad positioning changes
Revision 1.38  2002/09/17 15:40:56  sb07663
Derived off of base_window to avoid allocate/deallocate of unneeded buttons
Revision 1.37  2002/06/05 14:32:51  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.36  2002/05/07 13:58:10  sb07663
First cut at VxWorks compatibility
Revision 1.35  2001/06/20 14:13:27  sb07663
IT2397: include "text value out of range" for donor vitals and
donor information keypad entries
Revision 1.34  2000/03/14 16:08:22  MS10234
Changed include files to remove paths, and case sensitive file
names.  (All files will be specified in lower case)  The makefile was
updated with appropriate dependencies
Revision 1.33  2000/02/28 23:21:05  SB07663
IT4604: include directive changes for safproc.hpp
Revision 1.32  1997/05/29 00:14:36  MC03015
Modification of max and min management to float
Revision 1.31  1997/05/27 16:42:13  MC03015
Modifications to allow for YSF float value and a redefinition of text entry max and min
Revision 1.30  1997/05/22 00:44:30  MC03015
Modification of string management, placement in frmtstr
Revision 1.29  1997/05/17 01:53:51  MC03015
Debugging of blood type keypad
Revision 1.28  1997/05/09 06:56:44  MC03015
Blood type keypad modifications
Revision 1.27  1997/03/15 02:27:08  MC03015
Addition of new keypad table and parsing logic
Revision 1.26  1996/12/21 10:10:41  mc03015
Modified string buffer for return management
Revision 1.25  1996/12/15 04:34:36  mc03015
No substantial modifications
Revision 1.24  1996/10/17 09:12:24  MC03015
Removal of common header guitemp.h
Revision 1.23  1996/10/15 00:15:05  MC03015
Additional formatting and exclusive keystroke processing added for beta 2.3
Revision 1.22  1996/10/11 20:18:37  MC03015
Modifications to HEIGHT keypad entries
Revision 1.21  1996/10/10 02:12:20  Barry_Fulkerson
- changed config enums to point to the enum list in cfg_keys.h
Revision 1.20  1996/10/07 11:27:02  MC03015
Keypad modifications for PREDICT and debug
Revision 1.19  1996/09/24 22:18:12  MC03015
Changes for Beta 2.3, specifically text entry name modifications and additions.
Revision 1.14  1996/07/10 18:33:27  MC03015
addition of new header (log  and header info), maigic number removal, and string function review
*/

// Single include define guardian
#ifndef KEYPAD_HPP
#define KEYPAD_HPP

// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"

// string definitions and methods
#include "frmtstr.hpp"

// message include
#include "message.h"

// keypad includes
#include "keypad.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// GUI <-> safety screen include
#include "safproc.h"

#include "bwindow.hpp"
#include "button_bitmap.h"


extern KEYPAD_TABLE keypads[];

class Screen_KEYPAD;

// CLASS DECLARES

class Screen_KEYPAD
   : public Base_Window
{
private:
   // ATTRIBUTES
   bool      status_line_displayed;
   bool      cleared;                   // flag - user cleared entry
   int       max_length_user_entry1;    // maximum length of the user entry 1
   int       max_length_user_entry2;    // maximum length of the user entry 2
   bool      keypad_display_1_finished; // done with entry one
   bool      keypad_display_2_finished; // done with entry two

   bool      useInitVal1;
   bool      useInitVal2;

   double    keypad_value_1;               // value returned upon pressing enter
   double    keypad_value_2;               // value stored as a result of entry 2
   guistring user_entry_buffer_1;          // buffer for user numeric entry
   guistring user_entry_buffer_2;          // buffer for user numeric entry
   guistring keypad_initialized_display_1; // buffer for initialized display
   guistring keypad_initialized_display_2; // buffer for initialized display
   guistring keypad_display_1;             // buffer for display manipulation
   guistring keypad_display_2;             // buffer for display manipulation
   guistring format_buffer;                // buffer for format capture

   // the predefined elements to extract data elements from the keypad table
   KEYPAD_IDS             keypad_name;
   double                 keypad_max_value;
   double                 keypad_min_value;
   short                  keypad_location_x;
   short                  keypad_location_y;
   TEXT_STRING_ID         keypad_units_text_id_1;
   TEXT_STRING_ID         keypad_units_text_id_2;
   guistring              keypad_format_1;
   guistring              keypad_format_2;
   double                 keypad_initial_value_1;
   double                 keypad_initial_value_2;
   int                    keypad_exponent;
   bool                   keypad_zero_ok;
   KEYPAD_ENTRY_DIRECTION keypad_entry_type;
   CONFIG_DATA_KEYS       keypad_data_type;
   TEXT_STRING_ID         keypad_button_clear_text_id;
   TEXT_STRING_ID         keypad_button_0_text_id;
   TEXT_STRING_ID         keypad_button_1_text_id;
   TEXT_STRING_ID         keypad_button_2_text_id;
   TEXT_STRING_ID         keypad_button_3_text_id;
   TEXT_STRING_ID         keypad_button_4_text_id;
   TEXT_STRING_ID         keypad_button_5_text_id;
   TEXT_STRING_ID         keypad_button_6_text_id;
   TEXT_STRING_ID         keypad_button_7_text_id;
   TEXT_STRING_ID         keypad_button_8_text_id;
   TEXT_STRING_ID         keypad_button_9_text_id;
   TEXT_STRING_ID         keypad_button_enter_text_id;
   int                    keypad_hide_input;
   int                    keypad_units_1_font_size;
   bool                   keypad_use_font_size_from_units;

   // the keypad bitmap buttons
   Button_Bitmap btn_clear;
   Button_Bitmap btn_9;
   Button_Bitmap btn_8;
   Button_Bitmap btn_7;
   Button_Bitmap btn_6;
   Button_Bitmap btn_5;
   Button_Bitmap btn_4;
   Button_Bitmap btn_3;
   Button_Bitmap btn_2;
   Button_Bitmap btn_1;
   Button_Bitmap btn_0;
   Button_Bitmap btn_enter;

   // the display text objects
   Display_Text text_user_entry_1;
   Display_Text text_user_entry_2;
   Display_Text text_exponent;

   // DECODE_ALLOCATION_PARAM
   // determine keypad location and units using lookup table
   void decode_allocation_param (const char* param);

   // SCREEN_INITIALZATION
   // This method initializes the screen
   void screen_initialization ();


   // COMPONENT_INITIALZATION
   // This method initializes the screen components
   void component_initialization ();


   // STRING_INITIALIZATION
   // This method initializes the string components
   void string_initialization ();


   // DISPLAY_INITIALIZATION_VALUES
   // This method initializes the display upon entry only
   void display_initialization_values ();


   // FORMAT_DISPLAY
   // This method updates the keypad display with units and format
   // statements initialized to clear
   void format_display ();


   // SET_EXPONENT
   // This method sets the keypad exponent display if necessary
   void set_exponent ();


   // RESET_DISPLAY
   // This method resets the keypad display
   void reset_display ();


   // DISPLAY_FILTER
   // This method filters the keypad display entries
   void display_filter (const char*);


   // UPDATE_DISPLAY
   // This method updates the keypad display
   void update_display (guistring&, guistring&, guistring&, guistring&);


public:
   // CONSTRUCTOR
   Screen_KEYPAD();


   // DESTRUCTOR
   virtual ~Screen_KEYPAD();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char*);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CLEAR_BUTTON
   // This method is called from the C callback stub and executes "continue"
   void process_clear_button ();


   // PROCESS_BUTTON_9
   // This method is called from the C callback stub and executes "continue"
   void process_button_9 ();


   // PROCESS_BUTTON_8
   // This method is called from the C callback stub and executes "continue"
   void process_button_8 ();


   // PROCESS_BUTTON_7
   // This method is called from the C callback stub and executes "continue"
   void process_button_7 ();


   // PROCESS_BUTTON_6
   // This method is called from the C callback stub and executes "continue"
   void process_button_6 ();


   // PROCESS_BUTTON_5
   // This method is called from the C callback stub and executes "continue"
   void process_button_5 ();


   // PROCESS_BUTTON_4
   // This method is called from the C callback stub and executes "continue"
   void process_button_4 ();


   // PROCESS_BUTTON_3
   // This method is called from the C callback stub and executes "continue"
   void process_button_3 ();


   // PROCESS_BUTTON_2
   // This method is called from the C callback stub and executes "continue"
   void process_button_2 ();


   // PROCESS_BUTTON_1
   // This method is called from the C callback stub and executes "continue"
   void process_button_1 ();


   // PROCESS_BUTTON_0
   // This method is called from the C callback stub and executes "continue"
   void process_button_0 ();


   // PROCESS_ENTER_BUTTON
   // This method is called from the C callback stub and executes "continue"
   void process_enter_button ();

}; // END of Screen_KEYPAD Class Declaration



class Docked_Keypad
   : public Screen_KEYPAD
{
public:
   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (OSWindow& parent, const char* format_string);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

};



// End of single include define guardian
#endif

/* FORMAT HASH 654fc7c35d2e6c54eb845a08e4a34eaf */
