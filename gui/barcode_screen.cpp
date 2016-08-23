/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_BARCODE

*     File name:   barcode_screen.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/barcode_screen.cpp 1.20 2008/10/14 22:27:13Z dslausb Exp dslausb $

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "barcode_screen.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"


static void barcode_cb_rank_toggle (void* data);
static void barcode_cb_input_clear (void* data);
static void barcode_cb_category_clear (void* data);

// Saved input
static SCAN_STRUCTURE saved_input;


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_BARCODE::Screen_BARCODE()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_BARCODE),

     category_placard (CARTANCHOR_X, 122, BITMAP_PLACARD_BARCODE),

     cartouche_input_label (textMiscBarcodeInputLabel),

     btn_clear_input_cartouche (GUI_BUTTON_BARCODE_INPUT_CLEAR,
                                466, 124, textButtonBarcodeInputClear,
                                BITMAP_BUTTON_BARCODE_INPUT_CLEAR_UP,
                                BITMAP_BUTTON_BARCODE_INPUT_CLEAR_DN),

     btn_clear_category_cartouche(GUI_BUTTON_BARCODE_CATEGORY_CLEAR,
                                  466, 372, textButtonBarcodeCategoryClear,
                                  BITMAP_BUTTON_BARCODE_CATEGORY_CLEAR_UP,
                                  BITMAP_BUTTON_BARCODE_CATEGORY_CLEAR_DN),


     btn_category_rank_switch (GUI_BUTTON_BARCODE_RANK_SWITCH,
                               BTN_X_COORD(5),
                               BTN_Y_COORD(1.5),
                               textButtonCategoryRankSwitch,
                               BITMAP_BUTTON_BARCODE_RANK_SWITCH_UP,
                               BITMAP_BUTTON_BARCODE_RANK_SWITCH_DN),

     alarm_state(CLEARED)
{
   // The category collection object
   cat_collection = new Category_Button_Collection(&barcode_metadirectory);

   // The category cartouche
   cartouche_category = new Barcode_Category_Cartouche (&barcode_metadirectory, 70, 379);

   // The input cartouche
   cartouche_input = new Barcode_Input_Cartouche (&barcode_metadirectory, 70, 131);


   // Create all the Category Buttons

   ////////
   // RANK 1
   ////////

   //  NOTE category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_NOTE,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_NOTE,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(1),
                                                         BITMAP_BUTTON_BARCODE_CAT_NOTE_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_NOTE_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN));

   //  BLOOD TUBING SET category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_TUBING_SET,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_BLOOD_TUBING_SET,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(2),
                                                         BITMAP_BUTTON_BARCODE_CAT_BLOOD_TS_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_BLOOD_TS_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN));

   //  AC BAG category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_AC,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_AC,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(3),
                                                         BITMAP_BUTTON_BARCODE_CAT_AC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_AC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeAc,
                                                         NAVY_BLUE,
                                                         GREY_DARK));

   //  REPLACEMENT FLUID category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_REPLACEMENT_FLUID,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_REPLACEMENT_FLUID,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(4),
                                                         BITMAP_BUTTON_BARCODE_CAT_REPLACEMENT_FLUID_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_REPLACEMENT_FLUID_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeNacl,
                                                         NAVY_BLUE,
                                                         GREY_DARK));

   //  DONATION ID category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_DONATION_ID,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_DONATION_ID,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(1),
                                                         BITMAP_BUTTON_BARCODE_CAT_DONATION_ID_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_DONATION_ID_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         1));  // only one donation ID per donation at a time

   //  OPERATOR category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_OPERATOR,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_OPERATOR,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(2),
                                                         BITMAP_BUTTON_BARCODE_CAT_OPERATOR_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_OPERATOR_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS, // could have several logging in and out for one run
                                                         false));

   //  RBC STORAGE SOLUTION category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_RBC_STORAGE_SOL,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_RBC_STORAGE_SOL,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(3),
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_SOL_UP_RBC_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_SOL_UP_RBC_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN));

   //  PLT STORAGE SOLUTION category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_PLT_STORAGE_SOL,
                                                         RANK_1,
                                                         GUI_BUTTON_BARCODE_CAT_PLT_STORAGE_SOL,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(4),
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_SOL_UP_PLT_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_SOL_UP_PLT_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN));

   ////////
   // RANK 2
   ////////

   //  DONOR category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_DONOR,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_DONOR,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(1),
                                                         BITMAP_BUTTON_BARCODE_CAT_DONOR_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_DONOR_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         1));  // only one donor on a machine at a time

   //  SAMPLE BAG category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_SAMPLE_BAG,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_SAMPLE_BAG,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(2),
                                                         BITMAP_BUTTON_BARCODE_CAT_SAMPLE_BAG_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_SAMPLE_BAG_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         1)); // should only have one sample bag per set

   //  DATA category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_DATA,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_DATA,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(3),
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_DATA_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_STORAGE_DATA_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN));

   //  Miscellaneous 2 category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_MISC1,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_MISC1,
                                                         BTN_Y_COORD(1),
                                                         BTN_X_COORD(4),
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeMisc1,
                                                         YELLOW,
                                                         GREY_LIGHT));

   //  Miscellaneous 3 category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_MISC2,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_MISC2,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(1),
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeMisc2,
                                                         YELLOW,
                                                         GREY_LIGHT));

   //  Miscellaneous 4 category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_MISC3,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_MISC3,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(2),
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeMisc3,
                                                         YELLOW,
                                                         GREY_LIGHT));

   //  Miscellaneous 4 category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_MISC4,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_MISC4,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(3),
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeMisc4,
                                                         YELLOW,
                                                         GREY_LIGHT));

   //  Miscellaneous 4 category
   cat_collection->add_table_entry (new Category_Button (&barcode_metadirectory,
                                                         BC_CAT_MISC5,
                                                         RANK_2,
                                                         GUI_BUTTON_BARCODE_CAT_MISC5,
                                                         BTN_Y_COORD(2),
                                                         BTN_X_COORD(4),
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_EMPTY,
                                                         BITMAP_BUTTON_BARCODE_CAT_MISC_UP_FULL,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_EMPTY_DN,
                                                         BITMAP_BUTTON_BARCODE_CAT_GENERIC_FULL_DN,
                                                         UNLIMITED_SCANS,
                                                         true,
                                                         textButtonBarcodeMisc5,
                                                         YELLOW,
                                                         GREY_LIGHT));
}   // END of Screen_BARCODE CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_BARCODE::~Screen_BARCODE()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_BARCODE::deallocate_resources ();


   // Delete the various complex objects
   delete cat_collection; // this will cause all the elements in the collection to be deleted as well
   delete cartouche_category;
   delete cartouche_input;

}   // END of Screen_BARCODE DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_BARCODE::allocate_resources (const char* allocation_parameter)
{
   guiglobs::current_displayed_alarm = BARCODE_INFO_ALARM;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // Allocate the bitmaps
   category_placard.allocate_resources (*this);

   // Allocate the input cartouche label
   cartouche_input_label.allocate_resources (*this);


   // Allocate the input cartouche
   cartouche_input->allocate_resources (*this);
   // Have input cartouche drain the Repository FiFo now.
   cartouche_input->barcode_notification_handler();  // Sack of Rome

   SCAN_STRUCTURE input_data;
   cartouche_input->return_display_data (&input_data);

   // Restore the last scan if it wasn't used
   if (saved_input.scan_strlen > 0 && input_data.scan_strlen == 0)
   {
      cartouche_input->display_barcode (saved_input.scan_string, saved_input.scan_strlen);
      saved_input.scan_strlen = 0;
   }

   // Allocate the category cartouche
   cartouche_category->allocate_resources (*this);

   // Allocate the buttons
   btn_category_rank_switch.allocate_resources (*this);
   btn_clear_category_cartouche.allocate_resources (*this);
   btn_clear_input_cartouche.allocate_resources (*this);


   // Link the buttons to their "C" callback stubs
   btn_category_rank_switch.set_callback (barcode_cb_rank_toggle, (void*)this);
   btn_clear_input_cartouche.set_callback (barcode_cb_input_clear, (void*)this);
   btn_clear_category_cartouche.set_callback (barcode_cb_category_clear, (void*)this);


   // Allocate a scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChange.init(Callback<Screen_BARCODE>(this, &Screen_BARCODE::alarm_state_change), MessageBase::SNDRCV_RECEIVE_ONLY);


   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_lowleft.deallocate_resources ();


   // Create and allocate the complex widgets / collections

   // Allocate all the category buttons and set them as unrealized and deferred

   cat_collection->allocate_resources(*this);

   // Now link the status bar/line to this window
   link_status_bar_line();


   // Set the prompt line messages
   // guiglobs::apheresis_status_line->set_current_text (textStatBarcodeMoveInput);  ??? let cartouche decide the message


   // send the alarm confirm message after allocation is complete
   alarm_set_struct.alarm_name     = BARCODE_INFO_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager();

}  // END of Screen_BARCODE ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_BARCODE::deallocate_resources ()
{
   // Deallocate all the category buttons
   cat_collection->deallocate_resources();

   // Deallocate the bitmaps
   category_placard.deallocate_resources();


   // Deallocate the buttons
   btn_category_rank_switch.deallocate_resources();
   btn_clear_category_cartouche.deallocate_resources();
   btn_clear_input_cartouche.deallocate_resources();

   // Save the input data if it hasn't been used yet
   cartouche_input->return_display_data (&saved_input);

   // Deallocate the input data
   cartouche_input->deallocate_resources();

   // Deallocate the category data
   cartouche_category->deallocate_resources();

   // Deallocate the input cartouche label
   cartouche_input_label.deallocate_resources ();

   _msgAlarmStateChange.deregisterMsg();

   // send the alarm confirm message after allocation is complete
   alarm_set_struct.alarm_name     = NULL_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

   guiglobs::current_displayed_alarm = NULL_ALARM;

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}   // END of Screen_BARCODE DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_BARCODE::alarm_state_change ()
{
   long message_data = _msgAlarmStateChange.getData();

   if ((message_data == (long)LATCHED))
   {
      // set alarm state
      alarm_state = LATCHED;

   }
   else if ((message_data == (long)ACTIVE))
   {
      // set alarm state
      alarm_state = ACTIVE;

   }  // End of else if (message..

   // Confirm to Proc that we got the alarm message.
   alarm_set_struct.alarm_name     = (ALARM_VALUES)BARCODE_INFO_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();


}   // END of Screen_BARCODE SCREEN_NOTIFY_HANDLER



////////////////////////////////////////////////////////////////////////////////
// ALARM_SET_STRUCT_MANAGER
void Screen_BARCODE::alarm_set_struct_manager ()
{

   // struct message declare for outgoing donor vitals information
   Message<Alarm_response_struct> Msg_Alarm_Set(MessageBase::SEND_GLOBAL);

   // Actually send the message
   Msg_Alarm_Set.send(alarm_set_struct);

}   // End of ALARM_SET_STRUCT_MANAGER



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_BARCODE::process_continue_button ()
{
   // Remove this alarm screen
   RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
   removeall.send(0);

   // set and send alarm response
   alarm_set_struct.alarm_name     = BARCODE_INFO_ALARM;
   alarm_set_struct.alarm_response = CONTINUE;
   alarm_set_struct_manager ();

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // Clear any message in the status line -- any clear of the input cartouche warrants a clean status line
   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);


}   // End of PROCESS_CONTINUE_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CATEGORY_RANK_SWITCH
void Screen_BARCODE::process_category_rank_switch ()
{
   CATEGORY_RANK current_rank;
   CATEGORY_RANK next_rank;

   // Clear the display in the category cartouche
   cartouche_category->clear_display ();

   cat_collection->nextRank();
}   // End of PROCESS_CATEGORY_RANK_SWITCH



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLEAR_INPUT_CARTOUCHE
void Screen_BARCODE::process_clear_input_cartouche ()
{
   // Clear the cartouche
   cartouche_input->clear_display();

}   // End of PROCESS_CLEAR_INPUT_CARTOUCHE



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLEAR_CATEGORY_CARTOUCHE
void Screen_BARCODE::process_clear_category_cartouche ()
{
   cartouche_category->reset_currently_displayed_barcode ();

}   // End of PROCESS_CLEAR_CATEGORY_CARTOUCHE



///////////////////////////////CALLBACK STUBS///////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// BARCODE_CB_RANK_TOGGLE
void barcode_cb_rank_toggle (void* data)
{
   // Call the class method on callback
   ((Screen_BARCODE*)data)->process_category_rank_switch ();

}   // END of barcode_cb_rank_toggle



////////////////////////////////////////////////////////////////////////////////
// BARCODE_CB_INPUT_CLEAR
void barcode_cb_input_clear (void* data)
{
   // Call the class method on callback
   ((Screen_BARCODE*)data)->process_clear_input_cartouche ();

}   // END of barcode_cb_input_clear



////////////////////////////////////////////////////////////////////////////////
// BARCODE_CB_CATEGORY_CLEAR
void barcode_cb_category_clear (void* data)
{
   // Call the class method on callback
   ((Screen_BARCODE*)data)->process_clear_category_cartouche ();

}   // END of barcode_cb_category_clear

/* FORMAT HASH 441ed92b577ab5d72552134dec31515d */
