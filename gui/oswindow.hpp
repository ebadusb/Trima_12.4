/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * The OSWindow class provides support for a rectangular window.  This window
 * can contain bitmaps, display text, and/or other (child) windows.
 */

#ifndef _OSWINDOW_INCLUDE
#define _OSWINDOW_INCLUDE

#include <vxWorks.h>

#include <list>

#include "osgraphics.h"

class OSWindow
{
public:
   OSWindow(OSDisplay& display);
   OSWindow(OSDisplay& display, const OSRegion& region);
   virtual ~OSWindow();

   UGL_WINDOW_ID winIdGet () const { return _uglWinId; }
   void          set_position (short x, short y);
   void          set_region (const OSRegion& newRegion);
   void          get_region (OSRegion& region) const { region = _region; }

   short          top () const { return _region.y; }
   short          left () const { return _region.x; }
   unsigned short width () const { return _region.width; }
   unsigned short height () const { return _region.height; }

   void attach (OSWindow* window, WIN_ATTRIB winAttrib = WIN_ATTRIB_VISIBLE);
   void detach ();

   void add_object_to_front   (OSWindowObject* obj);
   void add_object_to_back    (OSWindowObject* obj);
   void delete_object         (OSWindowObject* obj);
   void move_object_to_front  (OSWindowObject* obj);
   void move_object_to_back   (OSWindowObject* obj);
   void set_object_region     (OSWindowObject* obj, const OSRegion& newRegion);
   void set_object_visible    (OSWindowObject* obj, bool newVisible);
   void invalidate_obj_region (OSWindowObject* obj);

   void setWindowVisibility (bool newVisible);
   bool getWindowVisibility (){ return winVisibleGet(_uglWinId); }

   void OSWindow::invalidateWindow ();

   virtual void set_disabled (bool newDisabled) { _disabled = newDisabled; }
   virtual bool disabled ();

   virtual void  draw (UGL_GC_ID gc);
   unsigned long getCheckDestroyed ();
   struct PointerEvent
   {
      enum EventType { ButtonPress, ButtonRelease };
      EventType eventType;
      short     x, y;
   };

   virtual void pointerEvent (const PointerEvent& event);

   static UGL_STATUS uglDrawCallback (WIN_ID id, WIN_MSG* pMsg, void* pData, void* pParam);
   static UGL_STATUS uglPointerCallback (WIN_ID id, WIN_MSG* pMsg, void* pData, void* pParam);

private:
   OSWindow();
   OSWindow(const OSWindow& obj);
   OSWindow& operator = (OSWindow& obj);

   void initialize_data     ();
   void add_object          (OSWindowObject* obj);
   void add_child_window    (OSWindow* child);
   void delete_child_window (OSWindow* child);
   void pre_draw_objects    ();
   void draw_objects        (UGL_GC_ID gc);

protected:
   UGL_WINDOW_ID _uglWinId;
   OSDisplay&    _display;
   OSWindow* getParentPointer ();

   OSRegion      _region;

   unsigned long _checkDestruction;
private:

   bool            _disabled;

   UGL_REGION_ID   _activeDrawRegion;

   OSWindow*       _parent;
   list<OSWindow*> _childWindows;

   //
   // Window objects for each window are separated into two lists.  The first
   // list contains window objects for which clip_siblings() is true (e.g.
   // bitmaps).  The list is sorted by z-order, with the first element at the
   // top of the z-order.  These objects must be drawn in order, with clipping
   // applied to insure overlapping objects are drawn correctly.
   //
   // The second list contains window objects for which clip_siblings() is false
   // (e.g. text).  These objects are drawn after all objects in the
   // clippedObjects list are drawn, and so effectively stay at the top of
   // the z-order.
   //
   // In addition to z-order, each child window object has a visible flag used
   // to determine if the object should be drawn.
   //
   list<OSWindowObject*> _clippedObjects;
   list<OSWindowObject*> _transparencyObjects;
   list<OSWindowObject*> _nonClippedObjects;

};



#endif

/* FORMAT HASH 54a66d9575eafab481df8857dddb582b */
