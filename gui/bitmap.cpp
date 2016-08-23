/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Bitmap

*     File name:   bitmap.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/


#include <vxworks.h>

#include "bitmap.h"
#include "guiglobs.hpp"

#define XTRA_LOGGING if (_extraLogging) DataLog (log_level_gui_info)

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Bitmap::Bitmap (int cons_x_coord,
                int cons_y_coord,
                BITMAP_ID id,
                bool extraLogging = false)
   : _xcoord(cons_x_coord),
     _ycoord(cons_y_coord),
     _id(id),
     _bitmap(NULL),
     _visible(true),
     _extraLogging(extraLogging)
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Bitmap::~Bitmap ()
{
   Bitmap::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Bitmap::allocate_resources (OSWindow& parent)
{
   if (_id ==  BITMAP_NULL) XTRA_LOGGING << "Attempting to allocate a BITMAP_NULL bitmap" << endmsg;

   if (!_bitmap)
   {
      XTRA_LOGGING << "Allocating resources for bitmap " << _id << "." << endmsg;

      _bitmap = new OSBitmap(*GuiMain::getDisplay(), _id);
      _bitmap->set_visible(_visible);
      parent.add_object_to_front(_bitmap);
      _bitmap->set_position(_xcoord, _ycoord);
   }
   else XTRA_LOGGING << "Bitmap " << _id << " already allocated." << endmsg;
}   // END of Bitmap ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Bitmap::deallocate_resources ()
{
   if (_id ==  BITMAP_NULL) XTRA_LOGGING << "Attempting to deallocate a BITMAP_NULL bitmap" << endmsg;

   if (_bitmap)
   {
      XTRA_LOGGING << "Dellocating resources for bitmap " << _id << "." << endmsg;
      delete _bitmap;
      _bitmap = NULL;
   }
   else XTRA_LOGGING << "Bitmap " << _id << " already deallocated." << endmsg;
}   // END of Bitmap DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// SET_ID

void Bitmap::set_id (BITMAP_ID id)
{
   if (_id != id)
   {
      _id = id;
      if (_bitmap) _bitmap->set_bitmap(_id);
   }
}


OSRegion Bitmap::get_dimensions () const
{
   OSRegion region(_xcoord, _ycoord, 0, 0);

   if (_id != BITMAP_NULL)
   {
      region.width  = bitmap_data_table[_id].width;
      region.height = bitmap_data_table[_id].height;
   }

   return region;
}

void Bitmap::set_position (short x, short y)
{
   _xcoord = x;
   _ycoord = y;

   if (_bitmap) _bitmap->set_position(_xcoord, _ycoord);
}

void Bitmap::set_visible (bool newVisible)
{
   _visible = newVisible;
   if (_bitmap)
   {
      XTRA_LOGGING << "Setting _visible to " << newVisible << " for bitmap " << _id << "." << endmsg;
      _bitmap->set_visible(_visible);
   }
}

void Bitmap::move_to_front (void)
{
   if (_bitmap)
   {
      XTRA_LOGGING << "Moving bitmap " << _id << " to front." << endmsg;
      _bitmap->move_to_front();
   }
}

void Bitmap::move_to_back (void)
{
   if (_bitmap)
   {
      XTRA_LOGGING << "Moving bitmap " << _id << " to back." << endmsg;
      _bitmap->move_to_back();
   }
}


DataLog_Stream& Bitmap::print (DataLog_Stream& os) const
{
   return os << "[bmp: id=" << _id << "; region=" << get_dimensions()
             << "; allocated=" << allocated() << "; visible=" << _visible << "]";
}

#undef XTRA_LOGGING

/* FORMAT HASH 453f1a8b789e5e970f4bee3a1a92dd16 */
