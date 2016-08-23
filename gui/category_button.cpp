/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class names:   Category_Button

*     File name:   category_button.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>

// Other GUI includes
#include "category_button.hpp"
#include "category_button_collection.hpp"
#include "metadirectory.hpp"
#include "guiglobs.hpp"
#include "statline.hpp"


Audio_Feedback Category_Button::_feedbackSound;

void printActionString (DataLog_Stream& os, long action)
{
   if (action == Category_Button::BC_ACTION_UNDEFINED)
      os << "UNDEFINED";
   else if (action == Category_Button::BC_ACTION_DO_NOTHING)
      os << "DO_NOTHING";
   else
   {
      bool firstOne = true;

      if (action & Category_Button::BC_ACTION_FULL_FACE)
      {
         if (firstOne)
            firstOne = false;
         else
            os << " | ";

         os << Category_Button::BC_ACTION_FULL_FACE;
      }

      if (action & Category_Button::BC_ACTION_EMPTY_FACE)
      {
         if (firstOne)
            firstOne = false;
         else
            os << " | ";

         os << Category_Button::BC_ACTION_EMPTY_FACE;
      }

      if (action & Category_Button::BC_ACTION_ADD_TEXT_WRITE_CART)
      {
         if (firstOne)
            firstOne = false;
         else
            os << " | ";

         os << Category_Button::BC_ACTION_ADD_TEXT_WRITE_CART;
      }

      if (action & Category_Button::BC_ACTION_DELETE_TEXT)
      {
         if (firstOne)
            firstOne = false;
         else
            os << " | ";

         os << Category_Button::BC_ACTION_DELETE_TEXT;
      }

      if (action & Category_Button::BC_ACTION_ERASE_CART)
      {
         if (firstOne)
            firstOne = false;
         else
            os << " | ";

         os << Category_Button::BC_ACTION_ERASE_CART;
      }
   }
}


DataLog_Stream& operator << (DataLog_Stream& os, Category_Button::CATEGORY_BUTTON_STATE state)
{
   switch (state)
   {
      case Category_Button::CBS_EMPTY :
         os << "EMPTY";          break;
      case Category_Button::CBS_FULL_INACTIVE :
         os << "FULL_INACTIVE";  break;
      case Category_Button::CBS_FULL_ACTIVE :
         os << "FULL_ACTIVE";    break;
      case Category_Button::CBS_UNKNOWN :
         os << "UNKNOWN";        break;
      default :
         os << "UNDEFINED";      break;
   }

   return os << "(" << (int)state << ")";
}


DataLog_Stream& operator << (DataLog_Stream& os, Category_Button::BARCODE_CAT_BTN_ACTIONS action)
{
   switch (action)
   {
      case Category_Button::BC_ACTION_DO_NOTHING :
         os << "DO_NOTHING";             break;
      case Category_Button::BC_ACTION_FULL_FACE :
         os << "FULL_FACE";              break;
      case Category_Button::BC_ACTION_EMPTY_FACE :
         os << "EMPTY_FACE";             break;
      case Category_Button::BC_ACTION_ADD_TEXT_WRITE_CART :
         os << "ADD_TEXT_WRITE_CART";    break;
      case Category_Button::BC_ACTION_DELETE_TEXT :
         os << "DELETE_TEXT";            break;
      case Category_Button::BC_ACTION_ERASE_CART :
         os << "ERASE_CART";             break;
      default : os << "UNDEFINED";         break;
   }

   return os << "(" << (int)action << ")";
}

DataLog_Stream& operator << (DataLog_Stream& os, const Category_Button::BTN_STATES& state)
{
   os << "{current=" << state.tab_current_state
      << "; requested=" << state.tab_requested_state
      << "; catsExist=" << state.tab_categories_exist
      << "; resultant=" << state.tab_resultant_state
      << "; action=(" << state.tab_processing_section_id << ")=";

   printActionString(os, state.tab_processing_section_id);

   return os << "}";
}


////////////////////////////////////////////////////////////////////////////////
// CATEGORY_BTN_CALLBACK

static void category_btn_callback (void* button_instance)
{
   // Call the button's press routine
   ((Category_Button*)button_instance)->process_button_press ();

}   // END of CATEGORY_BTN_CALLBACK



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Category_Button::Category_Button (MetaDirectory* wgt_directory,
                                  SCAN_CATEGORY category_prm,
                                  CATEGORY_RANK rank_prm,
                                  GUI_BUTTON_ID btn_id,
                                  short row,
                                  short col,
                                  BITMAP_ID btn_up_bitmap_name_empty,
                                  BITMAP_ID btn_up_bitmap_name_full,
                                  BITMAP_ID btn_dn_bitmap_name_empty,
                                  BITMAP_ID btn_dn_bitmap_name_full,
                                  int max_entries = UNLIMITED_SCANS,
                                  bool allowDeletions = true,
                                  TEXT_STRING_ID text_id = textEmpty,
                                  TEXT_COLOR emptyTextColor = LAST_TEXT_COLOR,
                                  TEXT_COLOR fullTextColor = LAST_TEXT_COLOR)
   : Button_Bitmap(btn_id, col, row,
                   textEmptyFormatCatButton,
                   btn_up_bitmap_name_empty,
                   btn_dn_bitmap_name_empty,
                   false, false),

     _btnLblTxt(text_id),
     _bmpVistaFeedback(78, 57, BITMAP_NULL),
     _metaDirectoryPtr (wgt_directory),
     _rank(rank_prm),
     _category(category_prm),
     _catBtnState(CBS_EMPTY),
     _bmpIdUpEmpty(btn_up_bitmap_name_empty),
     _bmpIdUpFull  (btn_up_bitmap_name_full),
     _bmpIdDownEmpty(btn_dn_bitmap_name_empty),
     _bmpIdDownFull  (btn_dn_bitmap_name_full),
     _maxScansAllowed(max_entries),
     _lastDisplayedSeqnum(0),
     _allowDeletions (allowDeletions),
     _highWaterSequence(0),
     _txtColorEmpty(emptyTextColor),
     _txtColorFull(fullTextColor),
     _lblId(text_id),
     _barcode_msg(Callback<Category_Button>(this, &Category_Button::vista_barcode_feedback_notify),
                  MessageBase::SNDRCV_RECEIVE_ONLY)

{
   DataLog (log_level_gui_info) << "Constructed button " << btn_id << " with colors: [empty="
                                << _txtColorEmpty << "(" << emptyTextColor << "); full=" << _txtColorFull << "("
                                << fullTextColor << ")]" << endmsg;
}  // End of constructor



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Category_Button::~Category_Button ()
{
   Category_Button::deallocate_resources();
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Category_Button::allocate_resources (OSWindow& parent)
{
   int category_total      = 0;
   int category_high_water = 0;

   // Don't allocate if the widget is already allocated
   if (allocated())
      return;

   // Set the margins in the button - they'll take place when the button is allocated and dimension'ed
   set_btn_text_margin (CAT_BTN_MARGIN);

   // Actually allocate the button
   Button_Bitmap::allocate_resources(parent);

   // Allocated the status bitmap (invisible to start)
   _bmpVistaFeedback.allocate_resources(*this);
   _bmpVistaFeedback.set_visible(false);

   // First find out the high water mark in this _category
   guiglobs::barcode_repo->return_category_count (_category, &category_total, &category_high_water);

   if (category_total > 0)
   {
      set_up_bitmap (_bmpIdUpFull);
      set_down_bitmap (_bmpIdDownFull);
      _catBtnState = CBS_FULL_INACTIVE;
   }
   else _catBtnState = CBS_EMPTY;

   // Set the callback
   set_callback (category_btn_callback, (void*)this);

   if (guiglobs::cdsData.xif.vistaConnected.Get())
   {
      // Determine the highest feedback rating
      BITMAP_ID feedback = get_vista_summary_status(category_high_water);

      // display the status bitmap.
      display_vista_status_symbol (feedback);
   }

   if (_lblId != textEmpty && _lblId != TEXT_NULL)
   {
      _btnLblTxt.set_string_id(_lblId);

      if (guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == DECIMAL_DELIMIT)
      {
         guistring delimString;
         _btnLblTxt.get_text_direct(delimString);
         delimString.swapOutDelimiter();
         _btnLblTxt.set_text_direct(delimString.length(), delimString);
      }

      _btnLblTxt.allocate_resources(*this);
   }
}  // End of allocate_resources




////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Category_Button::deallocate_resources ()
{
   _bmpVistaFeedback.deallocate_resources();
   _btnLblTxt.deallocate_resources();
   Button_Bitmap::deallocate_resources();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_BUTTON_PRESS
void Category_Button::process_button_press ()
{
   SCAN_STRUCTURE        data_buffer;
   int                   category_total      = 0;
   int                   category_high_water = 0;
   int                   read_rc             = 0;
   CATEGORY_BUTTON_STATE button_state        = CBS_UNKNOWN;


   Barcode_Input_Cartouche* cart_input = (Barcode_Input_Cartouche*)_metaDirectoryPtr->get_object_ptr (CARTOUCHE_INPUT);

   // Reset the status line coming into this button press.
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);


   // Get the current highwater mark sequence for this _category
   guiglobs::barcode_repo->return_category_count (_category,
                                                  &category_total,
                                                  &category_high_water);

   // If there is a scan entry but we're full up for this _category, raise warning and return.
   cart_input->return_display_data (&data_buffer);

   if (data_buffer.scan_strlen > 0 && category_total >= _maxScansAllowed)
   {
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE,
                                                                             textStatBarcode2ManyScans);
      return;
   }

   // Get the historical high water mark
   _highWaterSequence = guiglobs::barcode_repo->return_historical_high_watermark (_category);

   // Determine if there is a new unassigned scan waiting in the input cartouche -- if so,
   // we need to store it; reset any other full_active buttons; set ourself as full active.
   cart_input->return_display_data_and_clear (&data_buffer);

   // Capture the current button state
   button_state = get_button_state();

   if (_category == BC_CAT_TUBING_SET)
   {
      if (data_buffer.scan_strlen > 8
          && data_buffer.scan_strlen < MAX_SCAN_LENGTH)
      {
         DataLog (log_level_gui_barcode_info) << "Looking up barcode ("
                                              << data_buffer.scan_string << ")." << endmsg;
         guiglobs::cassette_mgr->lookupBarcode(data_buffer);
      }
      else
         DataLog (log_level_gui_barcode_info) << "String length "
                                              << data_buffer.scan_strlen << " is out of range." << endmsg;
   }

   // We have an input scan to store -- doesn't matter what the
   // current state is, it's going to turn into a FULL_ACTIVE state.
   if (data_buffer.scan_strlen > 0)
   {
      // Put our _category ID in the structure
      data_buffer.scan_category = _category;

      // Reset status
      data_buffer.status = BC_STATUS_GOOD_SCAN;

      // Default Vista status to Pending
      data_buffer.vista_response = BC_VISTA_PENDING;

      // Add the sequence number to the key
      _highWaterSequence++;

      // Add the current time to the structure
      time(&data_buffer.scan_time);

      // Write out the _category item
      guiglobs::barcode_repo->write_category_item (_category, _highWaterSequence, &data_buffer);

      // Set the button to CBS_FULL_ACTIVE
      set_button_state (CBS_FULL_ACTIVE, _highWaterSequence);

      // Clear any message in the status line
      guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
      guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);


      // Stop processing
      return;
   }


   if (button_state == CBS_FULL_INACTIVE || button_state == CBS_FULL_ACTIVE)
   {
      int cat_search_index;

      // First find out the high water mark in this _category
      guiglobs::barcode_repo->return_category_count (_category,
                                                     &category_total,
                                                     &category_high_water);

      // Determine what the next "candidate" _category item sequence number should be.
      //  (remember, we may have deleted "holes" in the sequence)
      if (_lastDisplayedSeqnum >= category_high_water)
      {
         cat_search_index = 0;
      }
      else
      {
         cat_search_index = _lastDisplayedSeqnum + 1;
      }


      for (; cat_search_index <= category_high_water; cat_search_index++)
      {
         read_rc = guiglobs::barcode_repo->read_category_item (_category, cat_search_index, &data_buffer);
         if (read_rc == SUCCESS_REPO)
         {
            // Set the button to CBS_FULL_ACTIVE with the new found _category item
            set_button_state (CBS_FULL_ACTIVE, cat_search_index);

            return;
         }

      }   // End of "for (int i = current_disp...

   }  // End of "if (button_state == CBS_FULL_INACTIVE || but...


   // If the button was empty and there was no input scan, it just presses and NOTHING happens.


}  // End of process_button_press


////////////////////////////////////////////////////////////////////////////////
// SET_BUTTON_STATE
void Category_Button::set_button_state (CATEGORY_BUTTON_STATE s_requested_state, int sequence_number)
{
   int                   category_total      = 0;
   int                   category_high_water = 0;
   CATEGORY_BUTTON_STATE s_resultant_state   = CBS_UNKNOWN;
   int                   table_dex           = 0;
   int                   have_stored_scans   = FALSE;
   long                  action_bits         = 0;
   int                   read_rc             = FAILURE_REPO;

   // Get _category cartouche addressability -- we'll probably need it below.
   Barcode_Category_Cartouche* cart_category = (Barcode_Category_Cartouche*)_metaDirectoryPtr->get_object_ptr (CARTOUCHE_CATEGORY);

   // Discover the number of _category scans stored behind this button
   guiglobs::barcode_repo->return_category_count (_category,
                                                  &category_total,
                                                  &category_high_water);

   // Set the bool indicating whether we have categories stored or not.
   have_stored_scans = (category_total > 0);


   // Search the state table to find the operating state
   table_dex   = 0;
   action_bits = BC_ACTION_UNDEFINED;

   do
   {
      if ((btnStates[table_dex].tab_current_state   == _catBtnState) &&
          (btnStates[table_dex].tab_requested_state == s_requested_state) &&
          (btnStates[table_dex].tab_categories_exist == have_stored_scans))
      {
         // Save off the resultant state on this match
         s_resultant_state = btnStates[table_dex].tab_resultant_state;

         // Save off the processing ID
         action_bits = btnStates[table_dex].tab_processing_section_id;

         // Leave the loop
         break;
      }

      table_dex++;

   } while (btnStates[table_dex].tab_current_state != CBS_UNKNOWN);

   DataLog (log_level_gui_info) << "set_button_state(" << s_requested_state
                                << "; " << sequence_number << "): curState=" << _catBtnState
                                << "; haveStoredScans=" << have_stored_scans << "; tableState="
                                << btnStates[table_dex] << endmsg;

   // todo: add some logging here.

   // If we didn't get a hit in the table, a software problem exists
   if (action_bits == BC_ACTION_UNDEFINED)
   {
      char state_info[200];
      sprintf (state_info, "BARCODE SCREEN: Category button state inconsistency. Current state = %d, Requested state = %d Number of entries = %d",
               _catBtnState,
               s_requested_state,
               category_total
               );

      _FATAL_ERROR (__FILE__, __LINE__, state_info);
   }

   if (action_bits == BC_ACTION_DO_NOTHING)
   {
      _catBtnState = s_resultant_state; // we may change state here, although currently not happening in the state table.
      return;                           // nothing to do here
   }

   // Now perform the actions necessary for this state change

   /***/
   // Write the current scan to the _category cartouche and label the button
   //  (basically this is done for any button with scans and who is being pressed)
   if (action_bits & BC_ACTION_ADD_TEXT_WRITE_CART)
   {
      SCAN_STRUCTURE data_buffer;
      int            loc_sequence = sequence_number;

      // If this button is not the active full button, then reset anyone else who is.
      if (get_button_state () != CBS_FULL_ACTIVE)
      {
         Category_Button_Collection* btn_coll = (Category_Button_Collection*)_metaDirectoryPtr->get_object_ptr (CATEGORY_BTN_COLLECTION);
         btn_coll->reset_active_state ();
      }

      // Erase the _category info in the _category cartouche
      cart_category->clear_display();

      for (; loc_sequence <= category_high_water; loc_sequence++)
      {
         read_rc = guiglobs::barcode_repo->read_category_item (_category, loc_sequence, &data_buffer);

         if (read_rc == SUCCESS_REPO)
         {
            cart_category->display_barcode_category (data_buffer.scan_string,
                                                     data_buffer.scan_strlen,
                                                     _category,
                                                     loc_sequence);

            _lastDisplayedSeqnum = loc_sequence;
            break;
         }
         else
         {
            if (read_rc == EOF)
            {
               break;      // never found it just leave it blank
            }
            else
            {
               _FATAL_ERROR (__FILE__, __LINE__, "BARCODE SCREEN: Category inconsistency while displaying scan info.");
            }
         }
      }   // End of " for (int i = loc_sequence; loc_sequence <= ca...

      // Write the _category scan sequence number on the button face
      if (read_rc == SUCCESS_REPO)
      {
         guistring seq_number;
         guistring continue_lit;
         guistring cr_lit;

         // Display text for bringing in the "more categories stored" dingbat
         Display_Text temp_text;


         sprintf (seq_number, "%d", loc_sequence);

         if (_lastDisplayedSeqnum < category_high_water)
         {
            temp_text.set_string_id (textMiscBarcodeMoreCategoriesElipses);
            temp_text.get_text_direct (continue_lit);
            strcat (seq_number, continue_lit);
         }

         temp_text.set_string_id (textMiscBarcodeVerticalSpacer);    // push down text
         temp_text.get_text_direct (cr_lit);
         strcat (cr_lit, seq_number);

         set_text_direct (strlen(cr_lit), cr_lit);
      }

   }  // End of " if ((actions_bit & M_WRITECART) == M_WRITECART)...

   /***/
   // Set the button bitmap to an active face
   if (action_bits & BC_ACTION_FULL_FACE)
   {
      set_up_bitmap   (_bmpIdUpFull);
      set_down_bitmap (_bmpIdDownFull);
      if (_txtColorFull != LAST_TEXT_COLOR) _btnLblTxt.set_color(_txtColorFull);
   }

   /***/
   // Erase any scan info. displayed in the _category cartouche
   if (action_bits & BC_ACTION_ERASE_CART)
   {
      // Erase cartouche
      cart_category->clear_display();
   }

   /***/
   // Set the button bitmap to an INactive face
   if (action_bits & BC_ACTION_EMPTY_FACE)
   {
      set_up_bitmap   (_bmpIdUpEmpty);
      set_down_bitmap (_bmpIdDownEmpty);
      if (_txtColorEmpty != LAST_TEXT_COLOR) _btnLblTxt.set_color(_txtColorEmpty);
   }

   /***/
   if (action_bits & BC_ACTION_DELETE_TEXT)
   {
      // Set the button label to null
      set_text_direct (1, " ");
   }

   // change state as the very last thing.
   _catBtnState = s_resultant_state;

   if (guiglobs::cdsData.xif.vistaConnected.Get())
   {
      // Determine the highest feedback rating
      BITMAP_ID feedback = get_vista_summary_status(category_high_water);

      // display the status bitmap.
      display_vista_status_symbol (feedback);
   }
}  // End of set button state



BITMAP_ID Category_Button::get_vista_summary_status (int category_high_water) const
{
   // Search the item list attached to this button _category for buttons
   // that haven't been acknowledged from Vista.
   // Display the appropriate icon on the button.
   BITMAP_ID      feedback = BITMAP_NULL;
   SCAN_STRUCTURE data_buffer;
   int            read_rc  = FAILURE_REPO;

   for (int i = 1; i <= category_high_water; i++)
   {
      read_rc = guiglobs::barcode_repo->read_category_item (_category, i, &data_buffer);

      if (read_rc == SUCCESS_REPO)
      {
         if (data_buffer.status == BC_STATUS_GOOD_SCAN)
         {
            if (data_buffer.vista_response == BC_VISTA_INVALID)
            {
               feedback = BITMAP_BARCODE_VISTA_INVALID;
               break;
            }
            else if (data_buffer.vista_response == BC_VISTA_INCORRECT_CATEGORY)
            {
               feedback = BITMAP_BARCODE_VISTA_INCORRECT_CATEGORY;
               break;
            }
            else if (data_buffer.vista_response == BC_VISTA_INVALID_DATA)
            {
               feedback = BITMAP_BARCODE_VISTA_INVALID_DATA;
               break;
            }
            else if (data_buffer.vista_response == BC_VISTA_PENDING)
            {
               feedback = BITMAP_BARCODE_VISTA_PENDING;
               continue;
            }
         }
      }
   }

   if (read_rc == SUCCESS_REPO)
   {
      DataLog(log_level_gui_barcode_info) << "Setting button to " << feedback << ".  CAT("
                                          << data_buffer.scan_category << "), SEQ(" << data_buffer.scan_sequence
                                          << "), ST(" << data_buffer.status << ")" << endmsg;
   }

   return feedback;
}



void Category_Button::display_vista_status_symbol (BITMAP_ID bitmap)
{
   if (bitmap == BITMAP_NULL)
   {
      _bmpVistaFeedback.set_visible(false);
   }
   else
   {
      // put up a pending bitmap.  This will change if Vista responds.
      _bmpVistaFeedback.set_id(bitmap);
      _bmpVistaFeedback.move_to_front();
      _bmpVistaFeedback.set_visible(true);
      DataLog(log_level_gui_barcode_info) << "Setting bitmap" << endmsg;
   }
}




////////////////////////////////////////////////////////////////////////////////
// DELETE_ACTIVE_BARCODE
void Category_Button::delete_active_barcode (SCAN_CATEGORY prm_category, int category_sequence)
{

   SCAN_STRUCTURE data_buffer;
   int            category_total      = 0;
   int            category_high_water = 0;
   int            read_rc             = 0;





   if (prm_category == _category)
   {
      if (!_allowDeletions)    // NOT allowed to delete items
      {
         // Put up a message and refresh button and cartouche
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE,
                                                                                textStatBarcodeDeletionDisallowed);
      }
      else
      {
         // if we get a bad return code, assume it is already deleted or non-existent, so ignore return.
         guiglobs::barcode_repo->delete_category_item (prm_category, category_sequence);
      }


      // Discover the number of _category scans remaining behind this button
      guiglobs::barcode_repo->return_category_count (_category,
                                                     &category_total,
                                                     &category_high_water);

      // If there are no _category items left, set the button to empty. If there is at least
      // one _category item left, display the first in the collection.
      if (category_total <= 0)
      {
         set_button_state (CBS_EMPTY, -1);
         return;
      }
      else    // we have at least one scan in this _category -- display the first
      {
         for (int i = 0; i <= category_high_water; i++)
         {
            read_rc = guiglobs::barcode_repo->read_category_item (_category, i, &data_buffer);
            if (read_rc == SUCCESS_REPO)
            {
               // Set the button to CBS_FULL_ACTIVE with the new found _category item
               set_button_state (CBS_FULL_ACTIVE, i);
               return;
            }

         }    // End of "for (int i=0; i <= category_high_water; i++)

      }   // End of: "else  //we have at least one scan

   }  // End of: "if (prm_category == _category)

}  // End of delete_active_barcode


// NOTIFY
void Category_Button::vista_barcode_feedback_notify ()
{
   const VISTA_BARCODE_FEEDBACK& msg = _barcode_msg.getData();

   if (msg.scan_category != _category)
   {
      // Category didn't match this instance.  Ignore message.
      return;
   }

   SCAN_STRUCTURE data_buffer;
   int            status = 0;

   status = guiglobs::barcode_repo->read_category_item (msg.scan_category, msg.scan_sequence, &data_buffer);

   if (data_buffer.status == BC_STATUS_DELETED)
   {
      // item was previously deleted.  no need to process.
      return;
   }

   DataLog(log_level_gui_barcode_info) << "Processing vista barcode msg.  CAT("
                                       << msg.scan_category
                                       << "), SEQ(" << msg.scan_sequence
                                       << "), VR(" << msg.vista_status << ")" << endmsg;

   if (status != SUCCESS_REPO)
   {
      DataLog(log_level_gui_barcode_info) << "BARCODE SCREEN: can't match vista response." << endmsg;
      return;
   }

   // Log Event.
   DataLog(log_level_gui_barcode_info) << "Setting button status icon to " << msg.vista_status
                                       << ". CAT(" << data_buffer.scan_category
                                       << "), SEQ(" << data_buffer.scan_sequence
                                       << "), ST(" << data_buffer.status
                                       << "), VR(" << data_buffer.vista_response << ")" << endmsg;

   // Upate the barcode repository
   data_buffer.vista_response = msg.vista_status;
   status                     = guiglobs::barcode_repo->update_category_item (msg.scan_category, msg.scan_sequence, &data_buffer);

   // The barcode screen may not be allocated when the Vista feedback comes.
   // If not, then do nothing.  The changes will appear when the screen is
   // next rendered.
   if (!allocated())
   {
      DataLog(log_level_gui_barcode_info) << "Widget not realized.  Exiting" << endmsg;
      return;
   }

   // Get _category cartouche addressability -- we'll probably need it below.
   Barcode_Category_Cartouche* cart_category = (Barcode_Category_Cartouche*)_metaDirectoryPtr->get_object_ptr (CARTOUCHE_CATEGORY);

   switch (msg.vista_status)
   {
      case BC_VISTA_OK :
         // Put up a null bitmap.
         _bmpVistaFeedback.set_visible(false);

         cart_category->display_barcode_category (data_buffer.scan_string,
                                                  data_buffer.scan_strlen,
                                                  _category,
                                                  data_buffer.scan_sequence);

         break;

      case BC_VISTA_INVALID :
         // put up a rejected bitmap

         _bmpVistaFeedback.set_id (BITMAP_BARCODE_VISTA_INVALID);
         _bmpVistaFeedback.set_visible(true);

         cart_category->display_barcode_category (data_buffer.scan_string,
                                                  data_buffer.scan_strlen,
                                                  _category,
                                                  data_buffer.scan_sequence);

         // Set the status line with the feedback if applicable.
         if (msg.vista_response_length > 0)
         {
            DataLog(log_level_gui_barcode_external) << "Setting status line with Vista barcode feedback. C="
                                                    << msg.scan_category << " S=" << msg.scan_sequence << " R=" << msg.vista_response
                                                    << " L=" << msg.vista_response_length << endmsg;

            guiglobs::apheresis_status_line->set_current_text_direct(msg.vista_response);
         }

         // Generate the bad barcode _feedbackSound
         _feedbackSound.create_feedback_sound (BARCODE_INVALID, SOUND_ONCE);
         break;

      case BC_VISTA_INCORRECT_CATEGORY :
         // put up a rejected bitmap

         _bmpVistaFeedback.set_id (BITMAP_BARCODE_VISTA_INCORRECT_CATEGORY);
         _bmpVistaFeedback.set_visible(true);

         cart_category->display_barcode_category (data_buffer.scan_string,
                                                  data_buffer.scan_strlen,
                                                  _category,
                                                  data_buffer.scan_sequence);

         // Set the status line with the feedback if applicable.
         if (msg.vista_response_length > 0)
         {
            DataLog(log_level_gui_barcode_external) << "Setting status line with Vista barcode feedback. C="
                                                    << msg.scan_category << " S=" << msg.scan_sequence << " R=" << msg.vista_response
                                                    << " L=" << msg.vista_response_length << endmsg;

            guiglobs::apheresis_status_line->set_current_text_direct(msg.vista_response);
         }

         // Generate the bad barcode _feedbackSound
         _feedbackSound.create_feedback_sound (BARCODE_INVALID, SOUND_ONCE);
         break;

      case BC_VISTA_INVALID_DATA :
         // put up a rejected bitmap

         _bmpVistaFeedback.set_id (BITMAP_BARCODE_VISTA_INVALID_DATA);
         _bmpVistaFeedback.set_visible(true);

         cart_category->display_barcode_category (data_buffer.scan_string,
                                                  data_buffer.scan_strlen,
                                                  _category,
                                                  data_buffer.scan_sequence);

         // Set the status line with the feedback if applicable.
         if (msg.vista_response_length > 0)
         {
            DataLog(log_level_gui_barcode_external) << "Setting status line with Vista barcode feedback. C="
                                                    << msg.scan_category << " S=" << msg.scan_sequence << " R=" << msg.vista_response
                                                    << " L=" << msg.vista_response_length << endmsg;

            guiglobs::apheresis_status_line->set_current_text_direct(msg.vista_response);
         }

         // Generate the bad barcode _feedbackSound
         _feedbackSound.create_feedback_sound (BARCODE_INVALID, SOUND_ONCE);
         break;

      default :
         DataLog(log_level_gui_barcode_error) << data_buffer.vista_response << " BARCODE_SCREEN: Illegal Vista Response" << endmsg;
   }
}

// Define the state table
Category_Button::BTN_STATES Category_Button::btnStates [] =
{  // current            requested          has scans   resultant               proc id

   {CBS_EMPTY,         CBS_EMPTY,          false,      CBS_EMPTY,              BC_ACTION_DO_NOTHING},
   {CBS_EMPTY,         CBS_EMPTY,          true,       CBS_FULL_INACTIVE,      BC_ACTION_FULL_FACE},

   {CBS_EMPTY,         CBS_FULL_INACTIVE,  false,      CBS_EMPTY,              BC_ACTION_DO_NOTHING},
   {CBS_EMPTY,         CBS_FULL_INACTIVE,  true,       CBS_FULL_INACTIVE,      BC_ACTION_FULL_FACE},

   {CBS_EMPTY,         CBS_FULL_ACTIVE,    false,      CBS_EMPTY,              BC_ACTION_DO_NOTHING},
   {CBS_EMPTY,         CBS_FULL_ACTIVE,    true,       CBS_FULL_ACTIVE,        BC_ACTION_FULL_FACE | BC_ACTION_ADD_TEXT_WRITE_CART},


   {CBS_FULL_INACTIVE, CBS_EMPTY,          false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE},
   {CBS_FULL_INACTIVE, CBS_EMPTY,          true,       CBS_FULL_INACTIVE,      BC_ACTION_DO_NOTHING},

   {CBS_FULL_INACTIVE, CBS_FULL_INACTIVE,  false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE},
   {CBS_FULL_INACTIVE, CBS_FULL_INACTIVE,  true,       CBS_FULL_INACTIVE,      BC_ACTION_DO_NOTHING},

   {CBS_FULL_INACTIVE, CBS_FULL_ACTIVE,    false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE},
   {CBS_FULL_INACTIVE, CBS_FULL_ACTIVE,    true,       CBS_FULL_ACTIVE,        BC_ACTION_ADD_TEXT_WRITE_CART},



   {CBS_FULL_ACTIVE,   CBS_EMPTY,          false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE | BC_ACTION_DELETE_TEXT | BC_ACTION_ERASE_CART},
   {CBS_FULL_ACTIVE,   CBS_EMPTY,          true,       CBS_FULL_INACTIVE,      BC_ACTION_DELETE_TEXT | BC_ACTION_ERASE_CART},

   {CBS_FULL_ACTIVE,   CBS_FULL_INACTIVE,  false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE | BC_ACTION_DELETE_TEXT | BC_ACTION_ERASE_CART},
   {CBS_FULL_ACTIVE,   CBS_FULL_INACTIVE,  true,       CBS_FULL_INACTIVE,      BC_ACTION_DELETE_TEXT | BC_ACTION_ERASE_CART},

   {CBS_FULL_ACTIVE,   CBS_FULL_ACTIVE,    false,      CBS_EMPTY,              BC_ACTION_EMPTY_FACE | BC_ACTION_DELETE_TEXT | BC_ACTION_ERASE_CART},
   {CBS_FULL_ACTIVE,   CBS_FULL_ACTIVE,    true,       CBS_FULL_ACTIVE,        BC_ACTION_ADD_TEXT_WRITE_CART},

   // Table end entry
   {CBS_UNKNOWN,       CBS_UNKNOWN,        false,      CBS_UNKNOWN,            BC_ACTION_DO_NOTHING}
};

/* FORMAT HASH 2662bbf770a88ae192c4a4756a4ef41a */
