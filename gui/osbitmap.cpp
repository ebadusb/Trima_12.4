/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/osbitmap.cpp 1.4 2008/01/23 17:27:30Z dslausb Exp dslausb $
 * $Log: osbitmap.cpp $
 * Revision 1.4  2008/01/23 17:27:30Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.3  2007/01/22 20:12:02Z  dslausb
 * Added extra logging for disconnect screen, encapsulated continue/confirm cycle functionality into a new class.
 * Revision 1.2  2003/08/25 19:49:12Z  jl11312
 * - modified configuration screens to clear background before button display (IT 6137)
 * Revision 1.1  2003/01/06 18:35:22Z  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include "osbitmap.hpp"
#include "trima_datalog.h"
#include "oswindow.hpp"

OSBitmap::OSBitmap(OSDisplay& display)
   : OSWindowObject(display),
     _guiId(BITMAP_NULL),
     _uglId(NULL),
     _width(0),
     _height(0)
{
   initialize_data();
}

OSBitmap::OSBitmap(OSDisplay& display, BITMAP_ID guiId)
   : OSWindowObject(display)
{
   initialize_data();
   set_bitmap(guiId);
}

void OSBitmap::initialize_data (void)
{
   _guiId            = BITMAP_NULL;
   _uglId            = UGL_NULL_ID;
   _width            = 0;
   _height           = 0;
   _usesTransparency = false;
}

OSBitmap::~OSBitmap()
{
   if ( _owner )
   {
      _owner->delete_object(this);
      _owner = NULL;
   }

   _uglId = UGL_NULL_ID;
   if ( _guiId != BITMAP_NULL )
   {
      _usesTransparency = false;
      _display.unload_bitmap(_guiId);
      _guiId            = BITMAP_NULL;
   }
}

OSWindowObject::ClippingType OSBitmap::clip_siblings (void) const
{
   if ( _usesTransparency )
   {
      return Transparency;
   }
   else return Clipped;
}

void OSBitmap::set_region (const OSRegion& newRegion)
{
   OSRegion clippedRegion = newRegion;
   clippedRegion.width  = _width;
   clippedRegion.height = _height;
   OSWindowObject::set_region(clippedRegion);
}

void OSBitmap::set_bitmap (BITMAP_ID id)
{
   if ( _guiId != BITMAP_NULL )
   {
      _usesTransparency = false;
      _display.unload_bitmap(_guiId);
   }

   _guiId = id;
   if ( _guiId != BITMAP_NULL )
   {
      _uglId = _display.load_bitmap(_guiId, _usesTransparency);

      UGL_SIZE width, height;

      UGL_STATUS_MONITOR(uglBitmapSizeGet(_uglId, &width, &height)   );

      _width  = width;
      _height = height;
   }
   else
   {
      _uglId  = UGL_NULL_ID;
      _width  = 0;
      _height = 0;
   }

   OSRegion newRegion = _region;
   newRegion.width  = _width;
   newRegion.height = _height;
   OSWindowObject::set_region(newRegion);

   //
   // The explicit call to invalidate the bitmap's region is needed
   // in case we changed to an identically sized bitmap (if the old and
   // new regions are the same, OSWindowObject::set_region simply returns
   // without forcing a redraw).
   //
   if ( _owner )
   {
      _owner->invalidate_obj_region(this);
   }
}

void OSBitmap::copy_to_display (void)
{
   if ( _uglId != UGL_NULL_ID )
   {
      UGL_GC_ID gc = uglGcCreate(_display.display());

      UGL_STATUS_MONITOR(uglRasterModeSet(gc, UGL_RASTER_OP_COPY)   );

      UGL_STATUS_MONITOR(uglBitmapBlt(gc, _uglId, 0, 0, _width - 1, _height - 1, UGL_DISPLAY_ID, _region.x, _region.y)   );

      UGL_STATUS_MONITOR(uglGcDestroy(gc)   );
   }
}

void OSBitmap::draw (UGL_GC_ID gc)
{
   if ( _uglId != UGL_NULL_ID )
   {
      UGL_STATUS_MONITOR(uglBitmapBlt(gc, _uglId, 0, 0, _width - 1, _height - 1, UGL_DEFAULT_ID, _region.x, _region.y)   );
   }
}

/* FORMAT HASH ffcc974d766b06fb45372d1166119134 */
