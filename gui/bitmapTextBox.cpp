/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/bitmapTextBox.cpp 1.3 2008/02/19 23:41:43Z dslausb Exp dslausb $
 * $Log: bitmapTextBox.cpp $
 * Revision 1.3  2008/02/19 23:41:43Z  dslausb
 * First batch of layout fixes for string.info files
 * Revision 1.2  2008/01/23 17:26:38Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.1  2007/03/16 16:27:12Z  dslausb
 * New text box class (used in plt config - they had been using a disabled button)
 *
 */

#include <vxworks.h>
#include "bitmapTextBox.h"
#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

BitmapTextBox::BitmapTextBox(short x,
                             short y,
                             TEXT_STRING_ID txtId,
                             BITMAP_ID bmpId
                             )
   : OSWindow(*GuiMain::getDisplay(), OSRegion(x, y, 0, 0)),
     _margin(0),
     _bitmap(0, 0, bmpId),
     _text(txtId),
     _allocated(false)
{} // END of BitmapTextBox CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

BitmapTextBox::~BitmapTextBox()
{
   if (_allocated) deallocate_resources ();

} // END of BitmapTextBox DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void BitmapTextBox::allocate_resources (OSWindow& parent)
{
   if (_allocated) return;

   _allocated = true;

   //
   //  Size window to loaded bitmap size
   //
   OSRegion bitmapreg, windowreg;
   bitmapreg = _bitmap.get_dimensions();
   get_region(windowreg);

   windowreg.width  = bitmapreg.width;
   windowreg.height = bitmapreg.height;
   set_region(windowreg);

   forceTxtDimensions();

   _bitmap.allocate_resources(*this);
   _text.allocate_resources(*this);

   //
   // Add this window to the parent
   //
   attach(&parent);
}  // END of BitmapTextBox ALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void BitmapTextBox::deallocate_resources ()
{
   if (!_allocated) return;

   _allocated = false;

   detach();

   // Deallocate the base objects
   //  _text.deallocate_resources ();
   _bitmap.deallocate_resources();
   _text.deallocate_resources();
}  // END of BitmapTextBox DEALLOCATE_RESOURCES

void BitmapTextBox::forceTxtDimensions (void)
{
   //
   // Force button text to button dimensions, overriding any coordinate information
   // in the language table.
   //
   OSRegion btnRegion, txtRegion;
   get_region(btnRegion);
   _text.get_requested_region(txtRegion);

   //
   // Set left/right to account for button margins
   //
   txtRegion.x     = _margin;
   txtRegion.width = btnRegion.width - 2 * _margin;

   // Only force vertical stuff if something doesn't make sense.
   if (txtRegion.height > (btnRegion.height - 2 * _margin) ||
       txtRegion.height == 0 ||
       txtRegion.y > (btnRegion.height - _margin ))
   {
      txtRegion.y      = _margin;
      txtRegion.height = btnRegion.height - 2 * _margin;
   }

   _text.set_requested_region(txtRegion);
}

////////////////////////////////////////////////////////////////////////////////
// SET_UP_BITMAP

void BitmapTextBox::setBitmapId (BITMAP_ID up_bitmap)
{
   // Can't set up bitmap unless widget has been allocated and non-null bitmap spec'ed
   if (up_bitmap == BITMAP_NULL)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "Invalid up bitmap button load");
   }

   _bitmap.set_id(up_bitmap);

}  // END of BitmapTextBox SET_UP_BITMAP

////////////////////////////////////////////////////////////////////////////////
// SET_BUTTON_TEXT_COLOR

void BitmapTextBox::setTxtColor (TEXT_COLOR textcolor)
{
   // Set the text color
   _text.set_color(textcolor);

}  // END of BitmapTextBox SET_BUTTON_TEXT_COLOR

///////////////////////////////////////////////////////////////////////////////
///SEARCH_LANGUAGE_TABLE

void BitmapTextBox::setStringId (TEXT_STRING_ID textid)
{
   _text.set_string_id (textid);
   forceTxtDimensions();
}  // END of BitmapTextBox SEARCH_LANGUAGE_TABLE

///////////////////////////////////////////////////////////////////////////////
///SET_TEXT_DIRECT

void BitmapTextBox::setTxtDirect (const int direct_len, const char* direct_text)
{
   _text.set_text_direct (direct_len, direct_text);

}  // END of BitmapTextBox SET_TEXT_DIRECT

///////////////////////////////////////////////////////////////////////////////
///GET_TEXT_DIRECT

void BitmapTextBox::getTxtDirect (guistring& buffer_ptr)
{
   _text.get_text_direct (buffer_ptr);

} // END of BitmapTextBox GET_TEXT_DIRECT

///////////////////////////////////////////////////////////////////////////////
// SET_BTN_TEXT_MARGIN

void BitmapTextBox::setMargin (int text_margin)
{
   if (text_margin >= 0)
   {
      _margin = text_margin;
   }
   else
   {
      _margin = 0;
   }

   forceTxtDimensions();

} // END of BitmapTextBox SET_BTN_TEXT_MARGIN

// -------------------------- Event processing -------------------------

/* FORMAT HASH b36413b3d197576d1289f6334b4e1b83 */
