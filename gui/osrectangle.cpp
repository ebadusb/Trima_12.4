/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/osrectangle.cpp 1.2 2003/02/12 18:35:01Z jl11312 Exp dslausb $
 * $Log: osrectangle.cpp $
 * Revision 1.2  2003/02/12 18:35:01Z  jl11312
 * - corrected several problems with operation of run-related screens
 * - removed old GUI debug hooks
 * - added common code to support interface of run-related screens to proc
 * Revision 1.1  2003/01/06 18:35:24  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include "osgraphics.h"
#include "oswindow.hpp"

OSRectangle::OSRectangle(OSDisplay& display, OSColor color)
   : OSWindowObject(display), _color(color)
{}

OSRectangle::OSRectangle(OSDisplay& display, const OSRegion& region, OSColor color)
   : OSWindowObject(display, region), _color(color)
{}

OSRectangle::~OSRectangle()
{}

void OSRectangle::set_color (OSColor color)
{
   if ( color != _color )
   {
      _color = color;
      if ( _owner )
      {
         _owner->invalidate_obj_region(this);
      }
   }
}

void OSRectangle::draw (UGL_GC_ID gc)
{
   if ( _region.width > 0 && _region.height > 0 )
   {
      UGL_STATUS_MONITOR(uglLineWidthSet(gc, 0)   );
      UGL_STATUS_MONITOR(uglBackgroundColorSet(gc, _color)   );
      UGL_STATUS_MONITOR(uglRectangle(gc, _region.x, _region.y, _region.x + _region.width - 1, _region.y + _region.height - 1)   );
   }
}

/* FORMAT HASH 3511cc4719128ad1a40b7e137e810e79 */
