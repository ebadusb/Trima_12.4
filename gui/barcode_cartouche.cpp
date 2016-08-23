/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class names:   Barcode_Cartouche, Barcode_Input_Cartouche, Barcode_Category_Cartouche

*     File name:   barcode_cartouche.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/barcode_cartouche.cpp 1.22 2009/04/07 20:28:52Z dslausb Exp dslausb $
$Log: barcode_cartouche.cpp $
Revision 1.22  2009/04/07 20:28:52Z  dslausb
IT 9101 - Use GUI_POOL_MALLOC where possible
Revision 1.21  2009/03/17 22:31:32Z  dslausb
IT 8919 - Fix overflowing barcode buffers
Revision 1.20  2008/10/17 17:19:22Z  dslausb
IT 8593 - Fix barcode screen string allocation
Revision 1.19  2008/10/14 22:38:22Z  dslausb
IT 8593 - Make barcode reader work
Revision 1.18  2008/10/10 16:36:49Z  dslausb
Change layout of text field on barcode screen.
Revision 1.17  2008/07/07 18:23:49Z  dslausb
More localization string id standardization
Revision 1.16  2008/07/03 20:16:40Z  dslausb
Further standardization of string IDs for localization
Revision 1.15  2008/06/20 19:52:32Z  dslausb
Tweak barcode screen for localization
Revision 1.14  2008/02/18 20:16:27Z  dslausb
Updates for string.info files
Revision 1.13  2003/09/16 20:24:42Z  rm70006
IT 5961.  Add filter for unprintable characters.
Revision 1.12  2003/08/05 20:25:34Z  rm70006
IT 6241.  Fixed bug where status bitmap wasn't being drawn properly.
Revision 1.11  2003/08/05 16:07:41Z  rm70006
IT 6191.  Debug barcode concatenation problem.
Revision 1.10  2003/05/12 17:12:07Z  rm70006
IT 5815.

Finish porting barcode stuff.
Revision 1.9  2003/05/01 12:47:32Z  sb07663
IT5849: new barcode feedback
Revision 1.8  2003/01/06 18:30:19Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.7  2002/12/05 14:00:20  jl11312
- removed calls to ZafApplication::Flush, since they were causing recursive calls through the Zinc event handler
Revision 1.6  2002/11/11 04:59:07  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.5  2002/10/29 17:14:46  rm10919
Put in Display_text class changes.
Revision 1.4  2002/08/15 22:14:18Z  sb07663
Compiling Vx version
Revision 1.3  2001/12/07 21:21:16  rm70006
IT 5151.
Add barcode feedback processing.
Revision 1.2  2001/05/03 13:24:32  rm70006
Added barcode code from V4.2.  IT 5083.
Revision 1.1  2000/11/06 19:19:51  tw09453
Initial revision
*/
#include <vxworks.h>


#define DEBUG 1


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

// Other GUI includes
#include "barcode_cartouche.hpp"

#include "metadirectory.hpp"

#include "category_button_collection.hpp"

#include "statline.hpp"

//////////////////////////////////// Barcode_Cartouche ////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Barcode_Cartouche::Barcode_Cartouche (MetaDirectory* wgt_directory,
                                      int cons_x_coord,
                                      int cons_y_coord,
                                      int text_x_coord,
                                      int text_y_coord,
                                      TEXT_STRING_ID strId
                                      )
   : text_scan_buffer(strId),
     bitmap_vista_feedback(cons_x_coord + 5, cons_y_coord + 5, BITMAP_NULL),
     widget_directory(wgt_directory),
     scan_length(0)
{
   memset(scan_hold, 0, sizeof(scan_hold));
   memset (scan_symbology, 0, MAX_SYMB_LENGTH);
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Barcode_Cartouche::~Barcode_Cartouche ()

{}  // End of destructor



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Barcode_Cartouche::allocate_resources (OSWindow& parent)
{
   // Display the text buffer next
   bitmap_vista_feedback.allocate_resources (parent);
   text_scan_buffer.allocate_resources (parent);

   // Display init some data members
   strcpy (scan_hold, "");

   scan_length = 0;
}  // End of allocate_resources




////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Barcode_Cartouche::deallocate_resources ()
{
   // Display the text buffer next
   text_scan_buffer.deallocate_resources ();
   bitmap_vista_feedback.deallocate_resources();

}  // End of deallocate_resources




////////////////////////////////////////////////////////////////////////////////
// CLEAR_DISPLAY
void Barcode_Cartouche::clear_display ()
{
   // Avoid gratuitous clears to minimize the widget flashing
   if (scan_length > 0)
   {

      // This resets the text without changing the coordinates, styling, etc.
      // Get the text out of the barcode scan factotum
      Display_Text temp_disp_buffer;
      guistring    temp_buff;

      temp_disp_buffer.set_string_id (textEmptyFormatBarcodeScan);

      int temp_len = temp_disp_buffer.get_text_length ();
      temp_disp_buffer.get_text_direct(temp_buff);

      // Reset the class data member, text string
      temp_buff.strip_unprintable_chars();
      text_scan_buffer.set_text_direct(temp_len, temp_buff);

      strcpy (scan_hold, "");

      scan_length = 0;

      // Remove the bitmap image.
      bitmap_vista_feedback.set_visible(false);
   }
}  // End of clear_display



////////////////////////////////////////////////////////////////////////////////
// RETURN_DISPLAY_DATA_AND_CLEAR
void Barcode_Cartouche::return_display_data_and_clear (SCAN_STRUCTURE* scan_info)
{
   // Copy the scan string
   return_display_data(scan_info);

   // Copy over the last symbology character
   memmove (scan_info->scan_symbology_char, scan_symbology, MAX_SYMB_LENGTH);

   // Clear the display buffer
   clear_display ();

   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);

   // Reset all data on this side
   memset(scan_symbology, 0, MAX_SYMB_LENGTH);
   scan_length = 0;
}  // End of return_display_data_and_clear



////////////////////////////////////////////////////////////////////////////////
// RETURN_DISPLAY_DATA
void Barcode_Cartouche::return_display_data (SCAN_STRUCTURE* scan_info)
{
   scan_info->scan_strlen = (scan_length < MAX_SCAN_LENGTH) ? scan_length : MAX_SCAN_LENGTH - 1;

   // Copy over the data
   if (scan_info->scan_strlen > 0)
   {
      // Return the text itself
      memmove (scan_info->scan_string, scan_hold, scan_info->scan_strlen);
   }

   // Ensure properly terminated string
   scan_info->scan_string[scan_info->scan_strlen] = '\0';

}  // End of return_display_data



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_BARCODE
void Barcode_Cartouche::display_barcode (char* scan_data, int scan_data_length)
{
   // Make sure we have positive dimensions
   if (scan_data_length < 1)
   {
      clear_display ();
      return;
   }

   // Ensure we don't overrun the display
   if (scan_data_length > MAX_SCAN_LENGTH)
   {
      // Clear display and any status messages
      clear_display ();

      // Announce to the operator that there's a buffer overrun
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE, textStatBarcodeScanOverrun);
   }
   else
   {
      // Move this into the hold area -- before we alter it (below)
      memmove (scan_hold, scan_data, scan_data_length);
      scan_length = scan_data_length;

      // Make sure that the string is null-terminated.
      scan_hold[scan_data_length] = '\0';

      int total_number_of_delimiters = 0;

      // Determine how many seqments are now in the hold buffer.
      for (int i = 0; i < scan_data_length; i++)
      {
         if (scan_data[i] == SEGMENT_DELIM)
         {
            total_number_of_delimiters++;
            scan_data[i] = ' ';   // provide an opportunity to "flow" a bunch of text in display
         }
      }

      if (total_number_of_delimiters > 1)
      {
         // Set the font size to tiny
         text_scan_buffer.set_font_size (TINY_FONT_SIZE);
      }
      else
      {
         // Set the font to the original (we need to have a getter in the disptxt object.!!!!)
         text_scan_buffer.set_font_size (NORM_FONT_SIZE);
      }

      // Move the scan data into a guistring,
      // make sure it's zero-delimited by truncating
      // it, then strip the unprintable characters.
      guistring barcodeString(scan_data);
      barcodeString.truncate(scan_data_length);
      barcodeString.strip_unprintable_chars();

      // Display the text string or part of it
      text_scan_buffer.set_text_direct (barcodeString.length(), barcodeString);
   }

}  // End of display_barcode




////////////////////////////////////////////////////////////////////////////////
// CONCATENATE_AND_DISPLAY_BARCODE
int Barcode_Cartouche::concatenate_and_display_barcode (char* scan_data, int scan_data_length)
{
   char scan_in_dup[2 * MAX_SCAN_LENGTH];
   char scan_hold_dup[2 * MAX_SCAN_LENGTH];

   int  max_new_data;
   bool dup_string = false;

   int  last_delim_offset;

   // Make sure we have positive dimensions, if not, clear what we have and return
   if (scan_data_length < 1)
   {
      clear_display ();
      return FAILURE_CART;
   }

   // See if this string is already in the scan_hold buffer

   // get maximum input string length to work with
   max_new_data = scan_data_length <= MAX_SCAN_LENGTH ? scan_data_length : MAX_SCAN_LENGTH;

   // Put input string and hold string in local buffers
   memmove (scan_in_dup, scan_data, max_new_data);
   memmove (scan_hold_dup, scan_hold, scan_length);

   // Add the segment delimiter to the input string
   scan_in_dup[max_new_data] = SEGMENT_DELIM;

   // Add the segment delimiter to the hold string, if it will fit --
   //   otherwise we'll fail on buffer overrun in display_barcode anyway
   if (scan_length <= MAX_SCAN_LENGTH)
   {
      scan_hold_dup[scan_length] = SEGMENT_DELIM;
   }

   // Replace any zeroes in the input string
   for (int i = 0; i < max_new_data; i++)
   {
      if (scan_in_dup[i] == '\0')
      {
         scan_in_dup[i] = '\xff';
      }
   }

   scan_in_dup[max_new_data + 1] = '\0'; // cauterize in prep. for strstr

   // Replace any zeroes in the hold string
   for (int i = 0; i < scan_length; i++)
   {
      if (scan_hold_dup[i] == '\0')
      {
         scan_hold_dup[i] = '\xff';
      }
   }

   scan_hold_dup[scan_length + 1] = '\0'; // cauterize in prep. for strstr

   // See if the input segment exists in the barcode scan hold buffer
   //   (containing the concatenation of all the previous barcode scans which
   //    are delimited by the SEGMENT_DELIM character)
   if (strstr(scan_hold_dup, scan_in_dup) != NULL)
   {
      dup_string = true; // We have a substring duplicate scan -- let the caller know on return
   }

   // Create the concatenation buffer and concatenate the existing and the string passed in.
   char* concat_string = (char*)GUI_POOL_MALLOC((scan_length + scan_data_length + 1) * sizeof(char));
   memmove (concat_string, scan_hold, scan_length);

   const char seg_del = SEGMENT_DELIM;

   if (scan_length > 0) // don't put the delimiter character after a "null" hold buffer
   {
      // There's something in the hold buffer so delimit it with the delimiter character
      memmove (concat_string + scan_length, (void*)&seg_del, 1);
      last_delim_offset = sizeof(SEGMENT_DELIM);
   }
   else
   {
      last_delim_offset = 0;
   }

   memmove (concat_string + scan_length + last_delim_offset, scan_data, scan_data_length);

   // Call barcode_display to do the display. if it's too big, display_barcode will handle the error.
   display_barcode (concat_string, (scan_length + scan_data_length + last_delim_offset));

   // Free concat_string buffer
   GUI_POOL_FREE(concat_string);

   // Was too long and got cleared
   if (scan_length == 0)
   {
      return TOO_LONG;
   }

   // Wasn't too long, but may be a duplicate scan
   if (dup_string)
   {
      return ALREADY_CONCATENATED;
   }
   else
   {
      return SUCCESS_CART;
   }
}  // End of concatenate_and_display_barcode



//////////////////////////////////// Barcode_Input_Cartouche ////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Barcode_Input_Cartouche::Barcode_Input_Cartouche (MetaDirectory* wgt_directory,
                                                  int cons_x_coord,
                                                  int cons_y_coord
                                                  )
   : Barcode_Cartouche (wgt_directory,
                        cons_x_coord,
                        cons_y_coord,
                        0, // Text Offset
                        0,
                        textEmptyFormatBarcodeScan), // Text Offset
     scan_notify(NULL)
{
   widget_directory->register_object (CARTOUCHE_INPUT, (void*)this);

}  // End of CONSTRUCTOR




////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Barcode_Input_Cartouche::~Barcode_Input_Cartouche ()
{
   delete scan_notify;
   widget_directory->unregister_object (CARTOUCHE_INPUT);
}  // End of DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Barcode_Input_Cartouche::allocate_resources (OSWindow& parent)
{
   // Allocate all for the parent class
   Barcode_Cartouche::allocate_resources (parent);

   // Allocate the notify message for new scans
   scan_notify = new BarcodeScannedMsg(Callback<Barcode_Input_Cartouche>(this, &Barcode_Input_Cartouche::barcode_notification_handler), MessageBase::SNDRCV_RECEIVE_ONLY);
}  // End of allocate_resources



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Barcode_Input_Cartouche::deallocate_resources ()
{
   // Deallocate the notify message for new scans
   delete scan_notify;
   scan_notify = NULL;

   // Deallocate all for the parent class
   Barcode_Cartouche::deallocate_resources ();
}  // End of deallocate_resources



////////////////////////////////////////////////////////////////////////////////
// CLEAR_DISPLAY
void Barcode_Input_Cartouche::clear_display ()
{
   // Clear via the base class
   Barcode_Cartouche::clear_display ();

   // Clear any message in the status line -- any clear of the input cartouche warrants a clean status line
   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);

}  // End of clear_display



////////////////////////////////////////////////////////////////////////////////
// BARCODE_NOTIFICATION_HANDLER
void Barcode_Input_Cartouche::barcode_notification_handler ()
{
   SCAN_STRUCTURE input_scan;
   int            FIFO_rc;
   int            scan_count = 0;

   do
   {
      // Read the next message in the FIFO
      FIFO_rc = guiglobs::barcode_repo->get_next_barcode_scan_from_fifo (&input_scan);

      if (FIFO_rc == SUCCESS_REPO)
      {
         if (input_scan.status == BC_STATUS_BAD_SCAN)
         {
            // With a bad scan, essentially require that the operator re-scan everything.
            guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE, textStatBarcodeBadScan);

            // Don't look at the rest of the scans for this item
            guiglobs::barcode_repo->clear_fifo();

            // Clear any existing scans for this item
            Barcode_Cartouche::clear_display();
         }
         else
         {
            // Concatenate and display the message
            int concat_rc = concatenate_and_display_barcode (input_scan.scan_string, input_scan.scan_strlen);

            // Count 'em
            scan_count++;

            do
            {
               if (concat_rc == TOO_LONG)
               {
                  break;
               }

               if (concat_rc == ALREADY_CONCATENATED)
               {
                  // Raise notification if a duplicate segment was detected
                  guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE,
                                                                                         textStatBarcodeSegDupe);
                  break;
               }

               if (concat_rc == SUCCESS_CART)
               {
                  // Remove any masking attention messages
                  guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);

                  // Raise notification of a new barcode and that it should be assigned
                  guiglobs::apheresis_status_line->set_current_text (textStatBarcodeMoveInput);

                  break;
               }

            } while (this - this);

            // Save off this segment's symbology character
            memmove (scan_symbology, input_scan.scan_symbology_char, MAX_SYMB_LENGTH);
         } // End of ELSE: if (input_scan.status == BC_STATUS_BAD_SCAN)

      } // End of "if (FIFO_rc ==

   } while (FIFO_rc == SUCCESS_REPO);

   // none were waiting in the FIFO, must have been an explicit invocation of the screen
   if (scan_count == 0 && scan_length == 0)
   {
      // Since no scans were made, suggest that they review the existing barcodes
      guiglobs::apheresis_status_line->set_current_text (textStatBarcodeReview);
   }
}



//////////////////////////////////// Barcode_Category_Cartouche ////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Barcode_Category_Cartouche::Barcode_Category_Cartouche (MetaDirectory* wgt_directory,
                                                        int cons_x_coord,
                                                        int cons_y_coord)
   : Barcode_Cartouche (wgt_directory,
                        cons_x_coord,
                        cons_y_coord,
                        30, // Text Offset
                        30,
                        textEmptyFormatBarcodeScanBottom), // Text Offset
     currently_displayed_category(BC_CAT_NO_ASSIGNMENT),
     currently_displayed_category_sequence (-1000)
{
   widget_directory->register_object (CARTOUCHE_CATEGORY, (void*)this);
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Barcode_Category_Cartouche::~Barcode_Category_Cartouche ()
{
   widget_directory->unregister_object (CARTOUCHE_CATEGORY);
}  // End of DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// RESET_CURRENTLY_DISPLAYED_BARCODE
void Barcode_Category_Cartouche::reset_currently_displayed_barcode ()
{
   if (currently_displayed_category != BC_CAT_NO_ASSIGNMENT)
   {
      // Get addressability to the category collection and delete the stored category
      Category_Button_Collection* btn_coll = (Category_Button_Collection*)widget_directory->get_object_ptr (CATEGORY_BTN_COLLECTION);

      // Ask the category button collection to remove this category item from the repository
      if (btn_coll)
         btn_coll->delete_active_barcode (currently_displayed_category, currently_displayed_category_sequence);
   }

}  // End of reset_currently_displayed_barcode



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_BARCODE_CATEGORY
int Barcode_Category_Cartouche::display_barcode_category (char* scan_data,
                                                          int scan_data_length,
                                                          SCAN_CATEGORY current_category,
                                                          int current_category_sequence)
{
   SCAN_STRUCTURE data_buffer;
   BITMAP_ID      feedback = BITMAP_NULL;

   // Save off the category ID and sequence number of this item
   currently_displayed_category          =  current_category;
   currently_displayed_category_sequence = current_category_sequence;

   // Display the scan data
   display_barcode (scan_data, scan_data_length);

   // Process the Vista response.
   int read_rc = guiglobs::barcode_repo->read_category_item (current_category, current_category_sequence, &data_buffer);

   if (guiglobs::cdsData.xif.vistaConnected.Get())
   {
      if (read_rc == SUCCESS_REPO)
      {
         if (data_buffer.status == BC_STATUS_GOOD_SCAN)
         {
            if (data_buffer.vista_response == BC_VISTA_INVALID)
            {
               feedback = BITMAP_BARCODE_VISTA_INVALID;
            }
            else if (data_buffer.vista_response == BC_VISTA_PENDING)
            {
               feedback = BITMAP_BARCODE_VISTA_PENDING;
            }
            else if (data_buffer.vista_response == BC_VISTA_INCORRECT_CATEGORY)
            {
               feedback = BITMAP_BARCODE_VISTA_INCORRECT_CATEGORY;
            }
            else if (data_buffer.vista_response == BC_VISTA_INVALID_DATA)
            {
               feedback = BITMAP_BARCODE_VISTA_INVALID_DATA;
            }
            else
               feedback = BITMAP_NULL;
         }
      }

      if (feedback == BITMAP_NULL)
      {
         bitmap_vista_feedback.set_visible(false);
      }
      else
      {
         bitmap_vista_feedback.set_visible(true);
         bitmap_vista_feedback.set_id(feedback);
         bitmap_vista_feedback.move_to_front();
      }
   }
   else
   {
      DataLog (log_level_gui_info) << "Vista not connected.  Show no vista feedback icon." << endmsg;
      bitmap_vista_feedback.set_visible(false);
   }

   // Return OK
   return SUCCESS_CART;
}  // End of display_barcode_category



////////////////////////////////////////////////////////////////////////////////
// CLEAR_DISPLAY
void Barcode_Category_Cartouche::clear_display ()
{
   // Ensure that the category and sequence numbers are in a null state
   currently_displayed_category          =  BC_CAT_NO_ASSIGNMENT;
   currently_displayed_category_sequence = -1;

   // Call our parent's clear routine now.
   Barcode_Cartouche::clear_display ();
} // End of clear_display

/* FORMAT HASH 3043b112d146e77d262b686322a3504d */
