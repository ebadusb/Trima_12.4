/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 GAMBRO BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SYSCASSETTE

*     File name:   syscassette.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*
*	Revision 1.45  2009/03/31 15:20:19Z  dslausb
*	IT 9165 - Fix status line for goback
*	Revision 1.44  2009/03/20 15:27:28Z  dslausb
*	- Transparent bitmaps
*	- Stand-alone graphics packages
*	- String.info CRCs
*	- Font mapping for Asian languages
*	Revision 1.43  2009/01/05 22:51:34Z  dslausb
*	IT 8990 - Move RBC initial split screen to after air evac, but before disposables test.
*	Revision 1.42  2008/10/03 16:20:34Z  dslausb
*	Move away from dynamic memory where possible
*	Revision 1.41  2008/10/01 16:07:59Z  dslausb
*	Change CDS objects so they're not pointers
*	Revision 1.40  2008/09/29 15:38:36Z  dslausb
*	DRBC split should only be shown if configured.
*	Revision 1.39  2008/09/09 17:00:20Z  dslausb
*	IT 8481 - Fixed a couple bugs with set load/unload screen transitions
*	Revision 1.38  2008/08/28 17:16:52Z  dslausb
*	Fix inconsistent decimal delimiters
*	Revision 1.37  2008/08/14 19:08:59Z  dslausb
*	IT 8801 - Initialize message pointers to NULL
*	Revision 1.36  2008/07/25 19:05:07Z  dslausb
*	- Overhaul the way procedure and cassette data are seen to be more robust to PFR
*	- Change UGL makefile so that it's not built so frequently
*	Revision 1.35  2008/07/14 20:29:55Z  dslausb
*	More stringId changes requested by localization
*	Revision 1.34  2008/07/09 23:54:37Z  dslausb
*	More string ID standardization as requested by Marita
*	Revision 1.33  2008/07/07 18:28:20Z  dslausb
*	More localization string id standardization
*	Revision 1.32  2008/07/03 20:19:13Z  dslausb
*	Further standardization of string IDs for localization
*	Revision 1.31  2008/05/29 19:59:55Z  dslausb
*	ITs 8507, 8663, and 8665 - Fiddle with set load screens
*	- Fix set load stat line message junk
*	- Add slide clamp instruction for non-RAS drbc sets
*	Revision 1.30  2008/05/09 20:43:08Z  dslausb
*	Moved cassette.h header to source file so reduce dependencies
*	Revision 1.29  2007/11/29 23:47:24Z  dslausb
*	IT 7372 - Moved up filter load screen to be directly after the syscassette screen.
*	Revision 1.28  2007/11/01 16:07:27Z  jheiusb
*	8090,8091 -- fix filtering of Predict on unloaded cassette
*	Revision 1.27  2007/08/28 21:38:49Z  spriusb
*	Fix for auto-split: changing name of REF message to be more generic (no longer VistaRefNumChosenMsg)
*	Revision 1.26  2007/01/16 16:07:12Z  dslausb
*	-- IT 7482 - Add RBC leukoreduction message and flag
*	-- Changed screen transitions for RBC run summary screen
*	-- Changed cassette listbox filtering to allow for PTF and RAS
*	Revision 1.25  2006/11/13 18:36:31Z  dslausb
*	IT 7797 - GUI must know about RBC dose to make decisions about cassette stamp compatibility.
*	Revision 1.25  2006/11/13 17:49:56Z  dslausb
*	IT 7797 - GUI must know about RBC dose to make decisions about cassette stamp compatibility.
*	Revision 1.24  2006/11/06 21:05:04Z  dslausb
*	Slight change to cassette manager member access.
*	Revision 1.23  2006/09/14 16:00:31Z  dslausb
*	IT 7752 - If truncate printed barcodes too long to fit in box.
*	Revision 1.22  2006/09/13 22:36:13Z  dslausb
*	Revision 1.21  2006/09/12 16:19:42Z  dslausb
*	IT 7723 - Fixed glitches in cassette filtering
*	Revision 1.20  2006/09/11 21:08:52Z  dslausb
*	Added message telling Vista what set was chosen.
*	Revision 1.19  2006/09/11 19:25:01Z  dslausb
*	Revision 1.18  2006/08/16 19:07:23Z  dslausb
*	Keypad entry of cassette clears out a scanned cassette.
*	Revision 1.17  2006/08/16 18:42:40Z  dslausb
*	IT 7625 - Made sure you can't get past this screen with a cassette that doesn't match your selected procedure.
*	Revision 1.16  2006/08/09 19:42:45Z  dslausb
*	IT 7682 - Improved two-file cassette functionality
*	Revision 1.15  2006/08/09 16:53:29Z  dslausb
*	IT 7682 - Changed to 2-file cassette entry functionality
*	Revision 1.14  2006/08/03 22:53:08Z  dslausb
*	Fixed cassette deletion
*	Revision 1.13  2006/07/28 23:29:50Z  dslausb
*	Redesigned cassette entry
*	Revision 1.12  2006/07/21 15:45:57Z  dslausb
*	Revision 1.11  2006/07/05 15:22:23Z  dslausb
*	IT 7628 - Clear guiglobs cassette values so they're not reloaded on subsequent entries to the cassette entry screen.
*	Revision 1.10  2006/06/30 19:49:16Z  dslausb
*	IT 7618 - Cassette entry cleared out when bad id is entered.
*	Revision 1.9  2006/06/30 15:08:53Z  dslausb
*	IT 7618 - Cleaned up catalog entry
*	Revision 1.8  2006/06/27 22:53:23Z  dslausb
*	Changes before gui freeze:
*	- Revert to old tyvec picture off to the side for catalog entry
*	- Remove frangible 'bubbles' from PAS connection screen
*	Revision 1.7  2006/05/30 21:29:50Z  dslausb
*	Changed graphic on cassette entry screen.
*	Revision 1.6  2006/05/12 23:15:33Z  dslausb
*	Changed set ID to 5 & 6 digit codes (was 3 & 5).
*	Revision 1.5  2006/04/27 19:46:52Z  dslausb
*	IT 7494 - deallocate resources now deletes keypad callback so other screens aren't trying to use it.
*	Revision 1.4  2006/04/26 21:35:37Z  dslausb
*	IT 7505 - Fixed stuff that was screwing up the status bar for everybody else.
*	Revision 1.3  2006/04/21 20:41:38Z  dslausb
*	IT 7272 - Smoothed out functionality for admin and operator cassette entry.
*	Revision 1.2  2006/04/12 22:16:29Z  dslausb
*	- Changed and deleted some strings for MSS
*	- Fixed a glitch with the status bar on syscassette
*	Revision 1.1  2006/04/11 16:01:40Z  dslausb
*	Initial revision
*
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "syscassette.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "CatalogChecker.h"   // Check CRC on catalog numbers.
#include "set_listbox.hpp"
#include "cassette.h"

#if (CPU==SIMNT)
#include "stringError.h"
#endif

// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "cfg_look.h"

#define SET_LABEL_X 78
#define SET_LABEL_Y 176

void handleEntryButton (void* data);


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSCASSETTE::Screen_SYSCASSETTE()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSCASSETTE),
     _bitmapCassetteEntryButton(GUI_BUTTON_CHOOSE_CASSETTE,
                                380,
                                170,
                                textButtonSelectRef,
                                BITMAP_BUTTON_SYS_SETUP_UP,
                                BITMAP_BUTTON_SETUP_DONOR_DOWN),
     _bitmapCassetteLabel(SET_LABEL_X,
                          SET_LABEL_Y,
                          BITMAP_CASSETTE_LABEL),
     _setIdListboxAllocationString(),
     _setIdKeypadAllocationString(),
     _alert(TEXT_NULL)
{
   initListboxString();
}    // END of Screen_SYSCASSETTE CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SYSCASSETTE::~Screen_SYSCASSETTE()
{
   Screen_SYSCASSETTE::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_SYSCASSETTE::allocate_resources (const char* allocation_parameter)
{
   _msgListboxData.init(Callback<Screen_SYSCASSETTE>(this, &Screen_SYSCASSETTE::listboxCB));
   _msgKeypadData.init(Callback<Screen_SYSCASSETTE>(this, &Screen_SYSCASSETTE::get_keypad_struct_manager), MessageBase::SNDRCV_RECEIVE_ONLY);

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.
   _bitmapCassetteLabel.allocate_resources(*this);
   _bitmapCassetteEntryButton.set_callback (handleEntryButton, (void*)this);
   _bitmapCassetteEntryButton.allocate_resources(*this);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();

   _alert = TEXT_NULL;

   initCalc();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Remove any residual messages
   setStatusLine(textEmpty);
} // END of Screen_SYSCASSETTE ALLOCATE_RESOURCES


void Screen_SYSCASSETTE::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();

   if (guiglobs::screen_status.get_current_screen() != GUI_SCREEN_SYSCASSETTE)
   {
      DataLog (log_level_gui_info) << "No refresh necessary." << endmsg;
      return;
   }

   if (_alert != TEXT_NULL)
   {
      if (_alert != textStatSetIdGoodCode) clearCurrentCassette();

      setStatusLine(_alert);
      _alert = TEXT_NULL;
      return;
   }

   // Check to see if the list has only one option, and if so, make it the default.
   checkCassetteMgr();
   guiglobs::cassette_mgr->checkForSingleOption();

   if (guiglobs::cassette_mgr->HasCurrentScannedCassetteSet())
   {
      guiglobs::cassette_mgr->setScannedAsCurrent();
   }

   _bitmapCassetteLabel.set_id(BITMAP_CASSETTE_LABEL);
   btn_continue.deallocate_resources();

   guistring cassetteRef; // empty by default

   if (guiglobs::cassette_mgr->HasCurrentCassetteSet())
   {
      // The current should only be incompatible in the case where
      // they scanned a REF number and selected an incompatible
      // procedure before they got here.  If that's the case, we'll
      // show it to them and make them pick something else.
      if (!guiglobs::procedure_selected || guiglobs::cassette_mgr->checkCurrentForCompatability() )
      {
         setStatusLine(textStatSetIdGoodCode);
         _bitmapCassetteLabel.set_id(BITMAP_CASSETTE_LABEL_GRAY);
         btn_continue.allocate_resources(*this);
         cassetteRef = guiglobs::cassette_mgr->getCurrentStringID();
      }
      else
      {
         DataLog (log_level_gui_info) << "Current cassette is not compatible with selected procedure" << endmsg;
         setStatusLine(textStatSetIdChosenProcBad);
      }
   }
   else
   {
      setStatusLine(textStatSetIdInstructions);
   }

   // Set the bitmap and text for the cassette entry button
   setCassetteButtonFace(cassetteRef);

}

void Screen_SYSCASSETTE::initialize_status_line ()
{
   // There shouldn't be anything on the status line before entering this screen
   guiglobs::apheresis_status_line->reset_status_line();

   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_SYSCASSETTE::deallocate_resources ()
{
   unlink_status_bar_line();

   _msgKeypadData.deregisterMsg();
   _msgListboxData.deregisterMsg();

   _bitmapCassetteLabel.deallocate_resources();
   _bitmapCassetteEntryButton.deallocate_resources();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}    // END of Screen_SYSCASSETTE DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_SYSCASSETTE::process_cancel_button ()
{
   // Clear the status line when we leave
   guiglobs::apheresis_status_line->reset_status_line();

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}    // END of Screen_SYSCASSETTE PROCESS_CANCEL_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_SYSCASSETTE::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();
   btn_cancel.deallocate_resources();
   btn_continue.deallocate_resources();
   btn_goback.deallocate_resources();

   // Clear the status line when we leave
   guiglobs::apheresis_status_line->reset_status_line();

   if (guiglobs::cassette_mgr->currentSetIsRas())
      goto_screen(GUI_SCREEN_PTF_MOUNT, "GUI_SCREEN_PTF_MOUNT");
   else goto_screen (GUI_SCREEN_SYSDISP, "GUI_SCREEN_SYSDISP");
}    // END of Screen_SYSCASSETTE PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_SYSCASSETTE::process_goback_button ()
{

   // Do screen stuff first... Disk stuff below can be time-intensive,
   // and can leave us sitting on this screen for an awkward amount of
   // time.

   // Clear the status line when we leave
   guiglobs::apheresis_status_line->reset_status_line();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // return to the SYSDONOR screen
   goto_screen (GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");

   checkCassetteMgr();

   int currentRefNum = atoi(guiglobs::cassette_mgr->getCurrentStringID());
   // going back could mean they want the Predict screen again... so we must clean up the
   // CDS function bits so Predict doesnt filter.
   ClearRefNumberMsg clearRefmessage(MessageBase::SEND_LOCAL);
   clearRefmessage.send(currentRefNum) ;

   clearCurrentCassette();

   guiglobs::apheresis_status_bar->set_phase(STATUS_BAR_LOAD, STATUS_BAR_STATE_RESET, true);
}    // END of Screen_SYSCASSETTE PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////


void Screen_SYSCASSETTE::get_keypad_struct_manager ()
{
   const KEYPAD_RETURN_STRUCT& keypad_buffer = _msgKeypadData.getData();
   const guistring&            keypadString  = keypad_buffer.keypad_return_string;
   const int                   string_length = keypadString.length();

   if (string_length == 0)
   {
      return;
   }

   if (string_length != 5 && string_length != 6)
   {
      DataLog (log_level_gui_info) << "Cassette ID length is incorrect.  Must be 5 or 6 digits." << endmsg;
      _alert = textStatCfgMachBadCatNum;
      clearCurrentCassette();
      return;
   }

   if (guiglobs::cassette_mgr->LookupCassette(keypadString))
   {
      DataLog (log_level_gui_info) << "Found cassette " << keypadString
                                   << " in table.  Changing current cassette value."
                                   << endmsg;

      _alert = textStatSetIdGoodCode;
      guiglobs::cassette_mgr->clearScannedCassette();

      if (guiglobs::procedure_selected)
      {
         DataLog (log_level_gui_info) << "A procedure has been selected.  Verifying cassette matches." << endmsg;

         if (guiglobs::cassette_mgr->checkCurrentForCompatability())
         {
            DataLog (log_level_gui_info) << "Cassette is compatible with chosen procedure." << endmsg;
         }
         else
         {
            DataLog (log_level_gui_info) << "Cassette is NOT compatible with chosen procedure." << endmsg;
            clearCurrentCassette();
            _alert = textStatSetIdChosenProcBad;
            return;
         }
      }

      setCassetteButtonFace(keypadString);

      btn_continue.allocate_resources(*this);
      return;
   }

   DataLog (log_level_gui_info) << "Table contains no listing for " << keypadString << ". Setting alert." << endmsg;
   clearCurrentCassette();
   _alert = textStatSetIdBadCode;
}

void Screen_SYSCASSETTE::initCalc ()
{
   _setIdKeypadAllocationString.initialize();
   sprintf(_setIdKeypadAllocationString, "%10d", (int)CASSETTE_CAT_NUM);
}

void Screen_SYSCASSETTE::initListboxString ()
{
   _setIdListboxAllocationString =
      Screen_SET_LISTBOX::createAllocationString(420, true, false);
}

void handleEntryButton (void* data)
{
   ((Screen_SYSCASSETTE*)data)->initListbox();
}

void Screen_SYSCASSETTE::initListbox ()
{
   goto_child_screen(GUI_SCREEN_SET_LISTBOX, _setIdListboxAllocationString);
}

void Screen_SYSCASSETTE::showCalc ()
{
   goto_child_screen(GUI_SCREEN_KEYPAD, _setIdKeypadAllocationString);
}

//
///////////////////////////////////////////////////////////////////////////////
// Here is a function that deals with the status bar.
void Screen_SYSCASSETTE::setStatusLine (TEXT_STRING_ID stringID)
{
   switch (stringID)
   {
      case textStatSetIdGoodCode :
      case textStatCfgMachBadCatNum :
      case textStatSetIdBadCode :
      case textStatSetIdChosenProcBad :
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, stringID);
         break;

      case textEmpty :
      case textStatSetIdInstructions :
      default :
         guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
         guiglobs::apheresis_status_line->set_current_text(stringID);
         break;
   }
}

void Screen_SYSCASSETTE::listboxCB ()
{
   DataLog (log_level_gui_info) << "Listbox Callback." << endmsg;
   Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL callbackVal =
      (Screen_SET_LISTBOX::LISTBOX_CALLBACK_VAL)_msgListboxData.getData();

   switch (callbackVal)
   {
      case Screen_SET_LISTBOX::SELECT :
         _bitmapCassetteEntryButton.set_text_direct(10, guiglobs::cassette_mgr->getCurrentStringID());
         break;

      case Screen_SET_LISTBOX::EMPTY :
         DataLog (log_level_gui_info) << "Listbox empty. Bringing up keypad." << endmsg;
         showCalc();
         break;

      default :
         DataLog (log_level_gui_error) << "Error: bad choice: " << callbackVal << endmsg;
         break;
   }
}

void Screen_SYSCASSETTE::clearCurrentCassette ()
{
   DataLog (log_level_gui_info) << "Clearing chosen cassette." << endmsg;
   _bitmapCassetteEntryButton.set_string_id(textButtonSelectRef);
   _bitmapCassetteEntryButton.set_up_bitmap(BITMAP_BUTTON_SYS_SETUP_UP);
   btn_continue.deallocate_resources();

   checkCassetteMgr();
   guiglobs::cassette_mgr->clearCurrent();
}

void Screen_SYSCASSETTE::checkCassetteMgr ()
{
   if (!guiglobs::cassette_mgr)
   {
      DataLog (log_level_gui_info) << "ERROR: Cassette manager unallocated." << endmsg;
      _FATAL_ERROR(__FILE__, __LINE__, "Unallocated cassette manager object");
   }
}

void Screen_SYSCASSETTE::setCassetteButtonFace (const guistring& cassetteRef)
{
   BITMAP_ID bmid_up = BITMAP_BUTTON_SYS_SETUP_UP;
   BITMAP_ID bmid_dn = BITMAP_BUTTON_SETUP_DONOR_DOWN;

   if (cassetteRef.length() > 0)
   {
      // Look up a potentially reference-coded bitmaps for up and down states
      CFG_Look::lookupSysRefButtonBitMapByCassetteRefNum(cassetteRef, bmid_up, bmid_dn);
   }

   // Apply the up/down bitmap(s) if different from what's already set
   if (bmid_dn != _bitmapCassetteEntryButton.get_down_bitmap_id())
      _bitmapCassetteEntryButton.set_down_bitmap(bmid_dn);
   if (bmid_up != _bitmapCassetteEntryButton.get_up_bitmap_id())
      _bitmapCassetteEntryButton.set_up_bitmap(bmid_up);

   // Set the button's text
   if (cassetteRef.length() > 0)
   {
      // Use the provided reference number
      _bitmapCassetteEntryButton.set_text_direct(cassetteRef.length(), cassetteRef);
   }
   else
   {
      // Use the default string id
      _bitmapCassetteEntryButton.set_string_id(textButtonSelectRef);
   }
}

/* FORMAT HASH 69bb372363fa72c92e42dabd67067a56 */
