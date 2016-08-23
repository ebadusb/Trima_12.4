/*
 * Copyright (c) 2012 by Terumo BCT, Inc.  All rights reserved.
 *
 * Author:  Regis McGarry
 *
 * Description:
 *  This file contains common GUI containers for displaying text.  They are broken
 * out here so they can be used by the language file reader which is separate and
 * outside of the GUI process.
 *
 */


#ifndef _STYLING_RECORD_
#define _STYLING_RECORD_

#include "ugl/ugltypes.h"
#include "datalog.h"

typedef unsigned char OSColor;

//
// Color management - the graphics interface provides support for a palette of
// up to 256 colors.  All color references from the application are in terms
// of the indices used in the OSSetColorEntry function.
//
enum { OSPaletteSize = 256  };


struct OSPaletteEntry
{
   unsigned char red;
   unsigned char green;
   unsigned char blue;
};

class OSRegion
{
public:
   short x, y;     // top left corner pixel coordinates
   short width;    // width in pixels
   short height;   // height in pixels

   OSRegion()
      : x(0), y(0), width(0), height(0) {}
   OSRegion(short argX, short argY, short argWidth, short argHeight)
      : x      (argX),
        y      (argY),
        width  (argWidth),
        height (argHeight)
   {}

   OSRegion(const OSRegion& copyRegion)
      : x      (copyRegion.x),
        y      (copyRegion.y),
        width  (copyRegion.width),
        height (copyRegion.height)
   {}

   void horiz_shift (short horizShift) { x += horizShift; }
   void vert_shift  (short vertShift) { y += vertShift; }

   UGL_RECT convert_to_ugl_rect () const
   {
      UGL_RECT rect;
      rect.left   = x;
      rect.top    = y;
      rect.right  = x + width - 1;
      rect.bottom = y + height - 1;
      return rect;
   }
};


DataLog_Stream& operator << (DataLog_Stream& os, const OSRegion& region);
DataLog_Stream& operator << (DataLog_Stream& os, const UGL_RECT& rect);


struct StylingRecord
{
   OSColor      color;          // color of the text as applied to whole string
   unsigned int attributes;     // attributes from the attribute enums above
   OSRegion     region;         // placement in window or _owner relative
   int          fontIndex;      // font index used, (probably the lang default, unless font mapped)
   int          fontSize;       // size of font most of the time

   StylingRecord()
      : color(0), attributes(0), fontIndex(0), fontSize(0) {}
};


#endif

/* FORMAT HASH 7539f57d5eaa037854d1a3ea711de64a */
