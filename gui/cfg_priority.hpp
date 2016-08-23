/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_CONFIG_PRIORITY

*     File name:   cfg_priority.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************


$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/cfg_priority.hpp 1.28 2008/04/08 17:03:17Z dslausb Exp dslausb $
$Log: cfg_priority.hpp $
Revision 1.28  2008/04/08 17:03:17Z  dslausb
IT 8451 - Implement Time-Only Config Mode
Revision 1.27  2006/08/10 18:20:26Z  dslausb
IT 7660 - Hyper icon on plt buttons in procedure priority
Revision 1.26  2006/06/29 16:10:30Z  dslausb
IT 7614 - Cleaned up procedure priority screen (took off yield, volume, and dose labels)
Revision 1.25  2006/06/13 17:42:24Z  dslausb
IT 7503 - Added procedure information to PLT, Plas, and RBC buttons in the config procedure priority screen.
Revision 1.24  2006/06/05 22:48:53Z  dslausb
Removed Fixed Volume
Revision 1.23  2006/04/17 20:51:38Z  dslausb
Procedure Priority screen plt buttons now have MSS icons if procedure is MSS.
Revision 1.22  2002/11/11 05:21:49Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.21  2002/09/06 21:50:05  sb07663
Fixed invalid reference initializations
Revision 1.20  2002/07/19 19:52:42  rm70006
VXWorks Port
Revision 1.19  2000/08/14 18:34:18Z  sb07663
IT4564: consolidation of config.dat parameter tables
Revision 1.18  2000/02/28 21:54:39  SB07663
IT4612: initial checkin for single stage user configuration
Revision 1.17  1999/04/16 14:59:38  SB07663
V3.3a versions for new screen definitions
Revision 1.16  1999/04/07 22:13:23  SB07663
Initial version (incomplete) for V3.3a
Revision 1.15  1998/10/14 14:51:05  BF02858
- IT 3605.  Added 'height' amount to widget call.  To keep the labels
 from 'moving' downward when the down button was pressed.
Revision 1.14  1997/06/02 20:12:54  Barry_Fulkerson
- changed the allocation of the up/down buttons so that the up button
  is grayed out when at the top of the list and the down button is grayed
  out when at the bottom of the list
Revision 1.13  1997/03/15 00:02:21  Barry_Fulkerson
- reduced label size so text box doesn't run off screen
Revision 1.12  1997/03/13 22:35:12  Barry_Fulkerson
- changed reference from min/max string to min/max text id's for easier translations
Revision 1.11  1997/03/13 19:11:49  Barry_Fulkerson
- changed width for new procedure #
Revision 1.10  1997/03/13 03:50:42  Barry_Fulkerson
- display the procedure for the button pressed, per IT 1412
Revision 1.9  1997/03/10 16:02:39  Barry_Fulkerson
- moved scroll buttons to align with PROCEDURE labels, per Julie C's
  recommendation.
Revision 1.8  1997/03/07 17:57:10  Barry_Fulkerson
Revision 1.7  1997/03/06 04:50:52  Barry_Fulkerson
- change #defines so not to conflict with changes in cfg_look.h
Revision 1.6  1997/03/05 19:28:29  Barry_Fulkerson
- added methods for displaying textual limit info when time button is pressed
Revision 1.5  1997/03/05 01:59:33  Barry_Fulkerson
- added keypad limit labeling to the Priority screen
Revision 1.4  1997/03/04 20:50:00  Barry_Fulkerson
- added new code to extend data from 5 to 15 procedures in the Procedure Priority screen
Revision 1.4  1997/02/28 02:44:26  Barry_Fulkerson
- TEMP checkin of PRIORITY screen, so that procedure selection is available
   only 3 procedures are displayed because the up/down scroll is not yet implemented
  THIS IS NOT FINAL!!!!
Revision 1.2  1997/02/26 03:17:54  Barry_Fulkerson
- add Header & Log key words for revision control
*/

// Single include define guardian
#ifndef SYSCONFIG_PRIORITY_HPP
#define SYSCONFIG_PRIORITY_HPP


// CLASS INCLUDE
#include "screen_config.hpp"

#define  MAX_ROWS       3
#define  MAX_PROCEDURES 20

//
///////////////////////////////////////////////////////////////////
//

class Screen_CONFIG_PRIORITY
   : public Screen_Config
{

public:

   typedef enum {INACTIVE, PLATELET, PLASMA, RBC, BLOOD_TYPE, TIME} BUTTON_TYPE;

   typedef struct
   {
      int                     x_pos;
      int                     y_pos;
      void*                   button_ptr;
      Button_Bitmap*          bitmap;
      Screen_CONFIG_PRIORITY* parent;
      BUTTON_TYPE             btn_type;
      int                     row;
   } BTN_INFO;

protected:

   // The display text objects
   Display_Text   plate_label;
   Display_Text   plasm_label;
   Display_Text   rbc_label;
   Display_Text   blood_type_label;
   Display_Text   time_label;
   Display_Text   procedure_indicator;

   Button_Bitmap* up_button;
   Button_Bitmap  gray_up_button;
   Button_Bitmap* down_button;
   Button_Bitmap  gray_dn_button;

   int            _top_procedure;
   BUTTON_TYPE    _callback_type;
   int            _callback_procedure;

   struct DATA_INFO
   {
      TEXT_STRING_ID   procedure_label;
      CONFIG_DATA_KEYS platelet_key;
      int*             platelet_value;
      CONFIG_DATA_KEYS plasma_key;
      int*             plasma_value;
      CONFIG_DATA_KEYS rbc_key;
      int*             rbc_value;
      CONFIG_DATA_KEYS blood_type_key;
      int*             blood_type_value;
      CONFIG_DATA_KEYS time_key;
      float*           time_value;
   } data_info[MAX_PROCEDURES];

   struct ROW_INFO
   {
      BTN_INFO     procedure_row_box;
      BTN_INFO     procedure_row_line;

      BTN_INFO     platelet_button;
      Display_Text yieldValue;
      Display_Text yieldx10;
      Display_Text yieldtoThe11th;

      BTN_INFO     plasma_button;
      Display_Text volValue;
      Display_Text volUnits;

      BTN_INFO     rbc_button;
      Display_Text doseValue;
      Display_Text doseUnits;

      BTN_INFO     blood_type_button;
      BTN_INFO     time_button;
   } row_info[MAX_ROWS];

protected:

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void label_the_buttons ();

public:

   // CONSTRUCTOR
   Screen_CONFIG_PRIORITY();

   // DESTRUCTOR
   virtual ~Screen_CONFIG_PRIORITY();

   //
   //
   //   Methods invoked by callbacks (public so they can be used by non-class
   //   callback functions)
   //
   virtual void process_up_button ();
   virtual void process_down_button ();
   virtual void set_indicator (const int row);
   virtual void awaiting_callback (const BUTTON_TYPE type, const int row);
   virtual void updatePltDataField (const int row);
   virtual void updatePlasDataField (const int row);
   virtual void updateRbcDataField (const int row);

   //  Overridden from base class
   virtual void update_screen_with_keypad_data (const KEYPAD_RETURN_STRUCT& return_keypad_buffer);
   virtual void update_screen_with_listbox_data (const GUI_LISTBOX_STRUCT& rtn_listbox_buffer);

   void checkForTimeOnlyConfig ();
};

// End of single include define guardian
#endif
//
///////////////////////////////////////////////////////////////////
// end of cfg_priority.hpp file

/* FORMAT HASH a31ff78fba71da25780c9ee924143676 */
