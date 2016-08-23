/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header$
 *
 */

#include <vxworks.h>
#include "button_bitmap.h"
#include "guiglobs.hpp"
#include "guiproc.h"

#define XTRA_LOGGING if (_extraLogging) DataLog (log_level_gui_info)

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Button_Bitmap::Button_Bitmap(GUI_BUTTON_ID btn_id,
                             short x,
                             short y,
                             TEXT_STRING_ID text_id,
                             BITMAP_ID btn_up_bitmap_name,
                             BITMAP_ID btn_dn_bitmap_name,
                             bool extraLogging = false,
                             bool overrideStylingRecord = true
                             )
   : OSWindow(*GuiMain::getDisplay(), OSRegion(x, y, 0, 0)),
     btn_logical_id(btn_id),
     btn_text_margin(0),
     btn_up_bitmap(0, 0, btn_up_bitmap_name, extraLogging),
     btn_dn_bitmap(0, 0, btn_dn_bitmap_name, extraLogging),
     btn_text(text_id),
     callback_routine_ptr(NULL),
     callback_data(NULL),
     last_click_x(0),
     last_click_y(0),
     _btnEnabled(true),
     _callbackEnabled(false),
     _allocated(false),
     _extraLogging(extraLogging),
     _overrideStylingRecord(overrideStylingRecord)
{
   // sanity check our ID...
   if ( (btn_id < 0) || (btn_id >= GUI_BUTTON_INVALID) )
      _FATAL_ERROR(__FILE__, __LINE__, "Button_Bitmap created with invalid ID");


    #if CPU == SIMNT
   btnPointers.push(btn_id, this);
    #endif /* if CPU == SIMNT */
} // END of Button_Bitmap CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Button_Bitmap::~Button_Bitmap()
{
   if (_allocated) deallocate_resources ();

    #if CPU == SIMNT
   btnPointers.pop(btn_logical_id, this);
    #endif /* if CPU == SIMNT */
   _checkDestruction = 0xDEADBEEF;
} // END of Button_Bitmap DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Button_Bitmap::allocate_resources (OSWindow& parent)
{
   XTRA_LOGGING << "Allocating resources for button " << btn_logical_id << "." << endmsg;

    #if CPU == SIMNT
   // Just in case there are multiple versions of this button, let's
   // point to this one.
   btnPointers.push(btn_logical_id, this);
    #endif /* if CPU == SIMNT */

   if (_allocated)
   {
      XTRA_LOGGING << "Resources already allocated for button "
                   << btn_logical_id << ". Returning." << endmsg;
      return;
   }

   _allocated = true;

   btn_dn_bitmap.allocate_resources(*this); // behind up bitmap
   btn_up_bitmap.allocate_resources(*this);

   _btnEnabled      = true;
   _callbackEnabled = (callback_routine_ptr != NULL);

   //
   //  Size window to loaded bitmap size
   //
   OSRegion bitmapreg, windowreg;
   bitmapreg = btn_up_bitmap.get_dimensions();
   get_region(windowreg);

   windowreg.width  = bitmapreg.width;
   windowreg.height = bitmapreg.height;
   set_region(windowreg);

   if (_overrideStylingRecord) force_text_dimensions();
   btn_text.allocate_resources(*this);

   // Attempt to mitigate non- or partially-rendered button anomaly (IT-13739, -10668)
   if (!btn_up_bitmap.get_visible() && !btn_text.is_visible())
   {
      // Up bitmap and button text are not visible. Fix and force a refresh.
      DataLog(log_level_gui_info) << "Fixing visibility for: " << get_button_id()
                                  << " dn=" << btn_dn_bitmap.get_visible()
                                  << " up=" << btn_up_bitmap.get_visible()
                                  << " txt=" << btn_text.is_visible()
                                  << endmsg;
      btn_dn_bitmap.set_visible(true);
      btn_up_bitmap.set_visible(true);
      btn_text.set_visible(true);
   }

   //
   // Add this window to the parent
   //
   btnAttach(&parent);

   XTRA_LOGGING << "Resource allocation complete for button " << btn_logical_id << "." << endmsg;
}  // END of Button_Bitmap ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Button_Bitmap::deallocate_resources ()
{
   XTRA_LOGGING << "Deallocating resources for button " << btn_logical_id << "." << endmsg;

   if (!_allocated)
   {
      XTRA_LOGGING << "Resources already deallocated for button "
                   << btn_logical_id << ". Returning." << endmsg;
      return;
   }

   _allocated = false;
   remove_callbacks();
   btnDetach();

   // Deallocate the base objects
   btn_up_bitmap.deallocate_resources();
   btn_dn_bitmap.deallocate_resources();
   btn_text.deallocate_resources();

   XTRA_LOGGING << "Resource deallocation complete for button " << btn_logical_id << "." << endmsg;
}  // END of Button_Bitmap DEALLOCATE_RESOURCES



void Button_Bitmap::btnAttach (OSWindow* parent)
{
   WIN_ATTRIB winAttrib = WIN_ATTRIB_NO_INPUT | WIN_ATTRIB_VISIBLE;
   attach(parent, winAttrib);

   UGL_STATUS_MONITOR (winCbAdd(_uglWinId, MSG_PTR_BTN1_DOWN, 0, &OSWindow::uglPointerCallback, UGL_NULL) );
   UGL_STATUS_MONITOR (winCbAdd(_uglWinId, MSG_PTR_BTN1_UP,   0, &OSWindow::uglPointerCallback, UGL_NULL) );
}

void Button_Bitmap::btnDetach ()
{
   // IT 10088. Note order. Call detach() only after deregistering the uglPointerCallbacks
   // Note: we remove twice because we added the same callback twice up un
   // btnAttach.  This code is very deeply confusing if you don't notice
   // that. --DGS
   UGL_STATUS_MONITOR(winCbRemove(_uglWinId, &OSWindow::uglPointerCallback) );
   UGL_STATUS_MONITOR(winCbRemove(_uglWinId, &OSWindow::uglPointerCallback) );
   detach();
}


void Button_Bitmap::force_text_dimensions ()
{
   //
   // Force button text to button dimensions, overriding any coordinate information
   // in the language table.
   //
   OSRegion button_dim, text_dim;
   get_region(button_dim);
   btn_text.get_requested_region(text_dim);

   //
   // Set left/right to account for button margins
   //
   text_dim.x     = btn_text_margin;
   text_dim.width = button_dim.width - 2 * btn_text_margin;

   // Only force vertical stuff if something doesn't make sense.
   if ( (text_dim.height > (button_dim.height - 2 * btn_text_margin)) ||
        (text_dim.height == 0) ||
        (text_dim.y > (button_dim.height - btn_text_margin)) )
   {
      text_dim.y      = btn_text_margin;
      text_dim.height = button_dim.height - 2 * btn_text_margin;
   }

   btn_text.set_requested_region(text_dim);
}

////////////////////////////////////////////////////////////////////////////////
// SET_UP_BITMAP

void Button_Bitmap::set_up_bitmap (BITMAP_ID up_bitmap)
{
   // Can't set up bitmap unless widget has been allocated and non-null bitmap spec'ed
   if (up_bitmap == BITMAP_NULL)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "Invalid up bitmap button load");
   }

   btn_up_bitmap.set_id(up_bitmap);

   // Set the region size based on this bitmap's size.
   OSRegion bitmapreg, windowreg;
   bitmapreg = btn_up_bitmap.get_dimensions();
   get_region(windowreg);

   if (windowreg.width < bitmapreg.width)
      windowreg.width = bitmapreg.width;

   if (windowreg.height < bitmapreg.height)
      windowreg.height = bitmapreg.height;

   set_region(windowreg);
}  // END of Button_Bitmap SET_UP_BITMAP

////////////////////////////////////////////////////////////////////////////////
// SET_DOWN_BITMAP

void Button_Bitmap::set_down_bitmap (BITMAP_ID dn_bitmap)
{
   btn_dn_bitmap.set_id(dn_bitmap);

   // Set the region size based on this bitmap's size.
   OSRegion bitmapreg, windowreg;
   bitmapreg = btn_dn_bitmap.get_dimensions();
   get_region(windowreg);

   if (windowreg.width < bitmapreg.width)
      windowreg.width = bitmapreg.width;

   if (windowreg.height < bitmapreg.height)
      windowreg.height = bitmapreg.height;

   set_region(windowreg);


}  // END of Button_Bitmap SET_DOWN_BITMAP

////////////////////////////////////////////////////////////////////////////////
// SET_BUTTON_TEXT_COLOR

void Button_Bitmap::set_button_text_color (TEXT_COLOR textcolor)
{
   // Set the text color
   btn_text.set_color(textcolor);

}  // END of Button_Bitmap SET_BUTTON_TEXT_COLOR

///////////////////////////////////////////////////////////////////////////////
///SEARCH_LANGUAGE_TABLE

void Button_Bitmap::set_string_id (TEXT_STRING_ID textid)
{
   btn_text.set_string_id (textid);
   if (_overrideStylingRecord) force_text_dimensions();
}  // END of Button_Bitmap SEARCH_LANGUAGE_TABLE

///////////////////////////////////////////////////////////////////////////////
///SET_TEXT_DIRECT
void Button_Bitmap::set_text_direct (TrimaTextItem* textItem)
{
   btn_text.setText(textItem);
}

void Button_Bitmap::set_text_direct (int direct_len, const char* direct_text)
{
   btn_text.set_text_direct (direct_len, direct_text);

}  // END of Button_Bitmap SET_TEXT_DIRECT

///////////////////////////////////////////////////////////////////////////////
///SET_TEXT_LANGUAGE
void Button_Bitmap::set_text_language (CONFIG_DATA_LISTBOX_STRINGS config_language)
{
   btn_text.set_text_language(config_language);

}  // END of Button_Bitmap SET_TEXT_LANGUAGE

///////////////////////////////////////////////////////////////////////////////
///GET_TEXT_DIRECT

void Button_Bitmap::get_text_direct (guistring& buffer_ptr)
{
   btn_text.get_text_direct (buffer_ptr);

} // END of Button_Bitmap GET_TEXT_DIRECT

///////////////////////////////////////////////////////////////////////////////
// SET_BTN_TEXT_MARGIN

void Button_Bitmap::set_btn_text_margin (int text_margin)
{
   if (text_margin >= 0)
   {
      btn_text_margin = text_margin;
   }
   else
   {
      btn_text_margin = 0;
   }

   if (_overrideStylingRecord) force_text_dimensions();

} // END of Button_Bitmap SET_BTN_TEXT_MARGIN

///////////////////////////////////////////////////////////////////////////////
///PROCESS_BUTTON_ARM
void Button_Bitmap::process_button_arm ()
{
   if (!_btnEnabled) return;

   // Create an audible feedback tone on button press
   guiglobs::button_audio_feedback.create_feedback_sound (BUTTON_CLICK_SOUND, SOUND_IMMEDIATE);

   // Hide the text on the button and the up bitmap
   btn_text.set_visible(false);
   btn_up_bitmap.set_visible(false);

} // END of Button_Bitmap PROCESS_BUTTON_ARM

///////////////////////////////////////////////////////////////////////////////
///PROCESS_BUTTON_DISARM
void Button_Bitmap::process_button_disarm ()
{
   if (!_btnEnabled) return;

   /////////////////////////
   // First, take care of the graphical elements.
   btn_up_bitmap.set_visible(true);
   btn_text.set_visible(true);

   // Then, send out a button message to broadcast this event to whoever cares about it.
   log_button_push();

   ////////////////////////
   // Now, call the callback routine if necessary.
   if (callback_routine_ptr && _callbackEnabled)
      callback_routine_ptr(callback_data);
}



///////////////////////////////////////////////////////////////////////////////
///GET_BUTTON_ID
GUI_BUTTON_ID Button_Bitmap::get_button_id ()
{
   return(btn_logical_id);

}  // END of Button_Bitmap GET_BUTTON_ID

// -------------------------- Event processing -------------------------

void Button_Bitmap::set_callback (CallbackFunc callback_routine, void* data_ptr)
{
   callback_routine_ptr = callback_routine;
   callback_data        = data_ptr;
   _callbackEnabled     = (callback_routine_ptr != NULL);
}

void Button_Bitmap::remove_callbacks ()
{
   _callbackEnabled = false;
}

void Button_Bitmap::enable_callbacks ()
{
   _btnEnabled = true;
}

void Button_Bitmap::disable_callbacks ()
{
   _btnEnabled = false;
}

void Button_Bitmap::pointerEvent (const PointerEvent& event)
{
   if (event.eventType == PointerEvent::ButtonPress)
   {
      last_click_x = event.x;
      last_click_y = event.y;
      process_button_arm();
   }
   else if (event.eventType == PointerEvent::ButtonRelease)
   {
      process_button_disarm();
   }
}

void Button_Bitmap::setFont (int newFontIndex, int newFontSize)
{
   btn_text.setFont(newFontIndex, newFontSize);
}


void Button_Bitmap::log_button_push ()
{
   const GUI_SCREEN_ID  current_screen = guiglobs::screen_status.get_current_displayed_screen();
   const GUI_BUTTON_ID  current_button = get_button_id();

   BUTTON_PRESS_MESSAGE btnData;

   // Log the button push event.
   DataLog(log_level_gui_info) << "GUI button pushed.  Screen " << current_screen
                               << ", Button " << current_button << endmsg;

   // Button info
   btnData.BUTTON_ID   = current_button;
   btnData.BUTTON_NAME = btnIdToStr(current_button);

   // Screen info
   btnData.SCREEN_ID   = current_screen;
   btnData.SCREEN_NAME = scrnIdToStr(current_screen);

   // Create an instance of the button message
   Message<BUTTON_PRESS_MESSAGE> btnmsg(MessageBase::SEND_LOCAL);

   // Send the message with the Screen/Button combined ID.
   btnmsg.send(btnData);
}



DataLog_Stream& operator << (DataLog_Stream& os, Button_Bitmap& btn)
{
   return os << "[Btn: id=" << btn.btn_logical_id << "; region=" << btn._region
             << "; allocated=" << btn._allocated << "; upBmp=" << btn.btn_up_bitmap
             << "; downBmp=" << btn.btn_dn_bitmap << "; txt=" << btn.btn_text << "]";
}

#if CPU == SIMNT
BtnPointerStack Button_Bitmap::btnPointers;

Button_Bitmap* Button_Bitmap::get_button_from_id (GUI_BUTTON_ID id)
{
   return btnPointers.getPtr(id);
}

#endif /* if CPU == SIMNT */

#undef XTRA_LOGGING

/* FORMAT HASH 838005b590da04d1e483bffeff2b7f97 */
