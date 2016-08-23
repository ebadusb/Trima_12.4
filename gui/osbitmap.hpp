/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 */

#ifndef OSBITMAP_INCLUDE
#define OSBITMAP_INCLUDE

#include "osgraphics.h"

class OSBitmap
   : public OSWindowObject
{
public:
   OSBitmap(OSDisplay& display);
   OSBitmap(OSDisplay& display, BITMAP_ID guiId);
   virtual ~OSBitmap(void);

   virtual void set_region (const OSRegion& newRegion);

   void set_bitmap (BITMAP_ID guiId);
   void copy_to_display (void);

protected:
   virtual void draw (UGL_GC_ID gc);
   bool _usesTransparency;

private:
   void                 initialize_data (void);
   virtual ClippingType clip_siblings (void) const;

private:
   BITMAP_ID      _guiId;
   OSBitmapId     _uglId;
   unsigned short _width;
   unsigned short _height;
};

#endif

/* FORMAT HASH 0c7c1254fe773c9e4c60780699184e8c */
