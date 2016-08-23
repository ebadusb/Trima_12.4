/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 */

#ifndef _OSGRAPHICS_INCLUDE
#define _OSGRAPHICS_INCLUDE

//
// This file contains the classes used to wrap the underlying graphics
// windowing system.  The intent is to limit dependencies on this graphics
// system to this header file and to the implementation files associated
// with it. All interaction with the graphics system from the application
// should be through the publically accessible functions and data defined
// in this file.
//
#include <vxWorks.h>

#include <list>

#include <../src/ugl/driver/font/agfa/itype/object.h>
#include <ugl/ugl.h>
#include <ugl/uglwin.h>

#include "bitmap_info.h"

#include "callback.h"
#include "trima_datalog.h"
#include "styling_record.h"
#include "language_hdr.h"

#include "ugl_monitor.hpp"

enum { MAX_FONTS = 256      };

typedef UGL_UINT32 WIN_ATTRIB;

//
// Font management
//
typedef UGL_FONT_ID OSFontId;

//
// Bitmap management
//
typedef UGL_DDB_ID OSBitmapId;

//
// The OSDisplay class handles the underlying UGL graphics context
// and event loop.  Currently, only one instance of the OSDisplay
// class should be created by the application.  The intent of making
// this a instantiable class is to provide a framework for future
// support for other types of displays such as printers.
//
//
//


class OSWindow;
class OSDisplay
{
private:
   UGL_DEVICE_ID        _uglDisplay;
   UGL_EVENT_SERVICE_ID _uglEventService;
   WIN_APP_ID           _uglApp;
   UGL_GC_ID            _uglGc;
   WIN_ID               _uglRootWindow;
   UGL_FONT_DRIVER_ID   _uglFontDriver;
   UGL_ORD              _option;

   short                _width;
   short                _height;

   OSFontId             _font[MAX_FONTS];

   list<OSWindow*>      _windowList;

   struct BitmapStatus
   {
      OSBitmapId   _uglId;
      unsigned int _loadCount;
   };

   BitmapStatus _bitmapStatus[BITMAP_ID_COUNT];
   bool         _cursorDisplayed;

   void drawRootWindow ();

public:
   OSDisplay(const CallbackBase& startCallback, const CallbackBase& wakeupCallback, unsigned int idleTimeoutMSec);
   virtual ~OSDisplay();

   //
   // The event handler must be called by the application to process
   // graphics input events and to update the display.  The flush
   // function can be called to flush pending graphics output to the
   // display.
   //
   void flush ();

   //
   // Color management
   //
   void set_color_entry (OSColor index, const OSPaletteEntry& entry);

   //
   // Font management
   //
   void     initFonts ();
   OSFontId get_fontWithFontId (const int FontIndex, const unsigned char pixelSize, const unsigned char weight = 0);
   OSFontId get_font (const char* familyName, int fontIndex, unsigned char pixelSize, unsigned char weight = 0);

   void cleanupFonts (); // Call when language changes to clean up unused fonts

   void delete_font (OSFontId font);

   //
   // Bitmap management
   //
   OSBitmapId load_bitmap (BITMAP_ID guiId, bool& transparency);
   void       unload_bitmap (BITMAP_ID guiId);

   //
   // The top left of the display is fixed at 0,0.  The following
   // functions report the width and height of the display (in pixels).
   //
   short width () const { return _width; }
   short height () const { return _height; }

   //
   // The OSDisplay class maintains a list of active windows for use in
   // the flush() function.
   //
   void add_window (OSWindow* window) { _windowList.push_front(window); }
   void remove_window (OSWindow* window) { _windowList.remove(window); }

   //
   // These functions are declared public to allow access by other
   // classes defined in the osgraphics interface.  They should not
   // be used by application code.
   //
   UGL_DEVICE_ID display () const { return _uglDisplay; }
   UGL_GC_ID     gc () const { return _uglGc; }
   WIN_APP_ID    app () const { return _uglApp; }
   WIN_ID        rootWindow () const { return _uglRootWindow; }

   //
   // These functions get and set the cursor position.
   // Only used in Service Mode.
   //
   void displayCursor ();
   void setCursorPos (int x, int y);
   void getCursorPos (int& x, int& y);

private:
   void cursor_init ();

   // The boolean return value is true if this bitmap has a transparency mask
   bool load_bitmap_from_compressed_data (BITMAP_ID guiId);
   void load_bitmap_from_file (BITMAP_ID guiId);
};


class OSWindowObject
{
   friend class OSWindow;

public:
   enum ClippingType { Clipped, Transparency, NotClipped };

   OSWindowObject(OSDisplay& display);
   OSWindowObject(OSDisplay& display, const OSRegion& region);
   virtual ~OSWindowObject();

   virtual void    set_position (short x, short y);
   virtual void    set_region (const OSRegion& newRegion);
   const OSRegion& get_region () const { return _region; }

   short          top () const { return _region.y; }
   short          left () const { return _region.x; }
   unsigned short width () const { return _region.width; }
   unsigned short height () const { return _region.height; }

   virtual void set_visible (bool newVisible);
   bool         is_visible () const { return _visible; }

   virtual void move_to_back ();
   virtual void move_to_front ();

protected:
   virtual void pre_draw () { }
   virtual void draw (UGL_GC_ID gc) = 0;

private:
   void                 initialize_data ();
   virtual ClippingType clip_siblings () const = 0;

protected:
   OSRegion   _region;
   bool       _visible;

   OSDisplay& _display;
   OSWindow*  _owner;
};

class OSRectangle
   : public OSWindowObject
{
public:
   OSRectangle(OSDisplay& display, OSColor color = 0);
   OSRectangle(OSDisplay& display, const OSRegion& region, OSColor color = 0);
   virtual ~OSRectangle();

   void    set_color (OSColor color);
   OSColor get_color () { return _color; }

protected:
   virtual void draw (UGL_GC_ID gc);

private:
   virtual ClippingType clip_siblings () const { return Clipped; }

private:
   OSColor _color;
};



void dumpOsColor (DataLog_Stream& os, OSColor colorIndex);


#endif /* #ifndef _OSGRAPHICS_INCLUDE */

/* FORMAT HASH 5a1910be9940ad2070f92807a4ff0a33 */
