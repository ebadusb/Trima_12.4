/*
 *	Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * Revision 1.7  2009/03/20 15:27:08Z  dslausb
 * - Transparent bitmaps
 * - Stand-alone graphics packages
 * - String.info CRCs
 * - Font mapping for Asian languages
 * Revision 1.6  2008/01/23 17:27:46Z  dslausb
 * IT 8435 - VxWorks 5.5 Checkin
 * Revision 1.5  2003/10/30 17:06:02Z  rm70006
 * IT 6501.  Add missing code to change language on donor download (config change).
 * Revision 1.4  2003/09/22 21:17:55Z  rm10919
 * Change text drawing for simulator so background color for text is correct.
 * Revision 1.3  2003/02/12 18:35:01Z  jl11312
 * - corrected several problems with operation of run-related screens
 * - removed old GUI debug hooks
 * - added common code to support interface of run-related screens to proc
 * Revision 1.2  2003/02/03 14:50:32  jl11312
 * - corrected update problem when new text object is added to a screen which has already been drawn
 * Revision 1.1  2003/01/06 18:35:25  jl11312
 * Initial revision
 *
 */

#include <vxWorks.h>
#include <ugl/winWindow.h>
#include "oswindow.hpp"

OSWindowObject::OSWindowObject(OSDisplay& display)
   : _display(display)
{
   initialize_data();
}

OSWindowObject::OSWindowObject(OSDisplay& display, const OSRegion& region)
   : _display(display)
{
   initialize_data();
   _region = region;
}

void OSWindowObject::initialize_data (void)
{
   _owner   = NULL;
   _visible = true;
}

OSWindowObject::~OSWindowObject()
{
   if ( _owner )
   {
      _owner->delete_object(this);
      _owner = NULL;
   }
}

void OSWindowObject::set_position (short x, short y)
{
   if ( x != _region.x ||
        y != _region.y )
   {
      OSRegion newRegion(x, y, _region.width, _region.height);
      OSWindowObject::set_region(newRegion);
   }
}

void OSWindowObject::set_region (const OSRegion& newRegion)
{
   if ( newRegion.x != _region.x ||
        newRegion.y != _region.y ||
        newRegion.width != _region.width ||
        newRegion.height != _region.height )
   {
      if ( _owner )
      {
         _owner->set_object_region(this, newRegion);
      }
      else
      {
         _region = newRegion;
      }
   }
}

void OSWindowObject::set_visible (bool newVisible)
{
   if ( _visible != newVisible )
   {
      if ( _owner )
      {
         _owner->set_object_visible(this, newVisible);
      }
      else
      {
         _visible = newVisible;
      }
   }
}

void OSWindowObject::move_to_back (void)
{
   if ( _owner )
   {
      _owner->move_object_to_back(this);
   }
}

void OSWindowObject::move_to_front (void)
{
   if ( _owner )
   {
      _owner->move_object_to_front(this);
   }
}

OSWindow::OSWindow(OSDisplay& display)
   : _display(display),
     _checkDestruction(0xCAFEBABE)
{
   initialize_data();
}

OSWindow::OSWindow(OSDisplay& display, const OSRegion& region)
   : _display(display),
     _checkDestruction(0xCAFEBABE)
{
   initialize_data();
   _region = region;
}

void OSWindow::initialize_data (void)
{
   _uglWinId         = UGL_NULL_ID;
   _parent           = NULL;
   _disabled         = false;
   _activeDrawRegion = UGL_NULL_ID;
}

OSWindow::~OSWindow()
{
   winDataSet(_uglWinId, UGL_NULL, UGL_NULL, 0);

   detach();

   list<OSWindowObject*>::iterator objIter;
   for ( objIter = _clippedObjects.begin(); objIter != _clippedObjects.end(); ++objIter )
   {
      if (*objIter) (*objIter)->_owner = NULL;
   }

   for (objIter = _transparencyObjects.begin(); objIter != _transparencyObjects.end(); ++objIter)
   {
      if (*objIter) (*objIter)->_owner = NULL;
   }

   for ( objIter = _nonClippedObjects.begin(); objIter != _nonClippedObjects.end(); ++objIter )
   {
      if (*objIter) (*objIter)->_owner = NULL;
   }
   _checkDestruction = 0xDEADBEEF;
}

void OSWindow::set_position (short x, short y)
{
   if ( x != _region.x ||
        y != _region.y )
   {
      OSRegion newRegion(x, y, _region.width, _region.height);
      OSWindow::set_region(newRegion);
   }
}

void OSWindow::set_region (const OSRegion& newRegion)
{
   _region = newRegion;
   if ( _uglWinId != UGL_NULL_ID )
   {
      const UGL_RECT& rect = _region.convert_to_ugl_rect();
      UGL_STATUS_MONITOR(winRectSet(_uglWinId, &rect));
   }
}

void OSWindow::attach (OSWindow* window, WIN_ATTRIB winAttrib /* = WIN_ATTRIB_VISIBLE*/)
{
   if ( _parent != window ||
        _uglWinId == UGL_NULL_ID)
   {
      //
      // Make sure this window has detached from any previous parent
      //
      detach();

      //
      // Create the underlying UGL window
      //
      void* data = (void*)this;
      _uglWinId = UGL_NULL_ID_MONITOR(winCreate(_display.app(), UGL_NULL, winAttrib,
                                                _region.x, _region.y, _region.width,
                                                _region.height, (void*)&data, sizeof(void*), UGL_NULL));

      // if (_uglWinId == UGL_NULL_ID)
      //    _FATAL_ERROR(__FILE__, __LINE__, "UGL ERROR: failure in winCreate");

      UGL_STATUS_MONITOR(winCbAdd(_uglWinId, MSG_DRAW, 0, OSWindow::uglDrawCallback, UGL_NULL)   );

      //
      // Attach all of this window's children
      //
      if ( !_childWindows.empty() )
      {
         list<OSWindow*>::iterator childIter = _childWindows.end();
         do
         {
            OSWindow* child = *--childIter;
            if (child && (child->_uglWinId != UGL_NULL_ID) )
            {
               UGL_STATUS_MONITOR(winAttach(child->_uglWinId, _uglWinId, UGL_NULL_ID)   );
            }
         } while ( childIter != _childWindows.begin() );
      }

      //
      // Notify parent that this window has been attached
      //

      // UGL_WINDOW_ID rootWindow = _display.rootWindow();
      _parent = window;
      if ( _parent )
      {
         _parent->add_child_window(this);
         if (_parent->_uglWinId != UGL_NULL_ID)
         {
            UGL_STATUS_MONITOR(winAttach(_uglWinId, _parent->_uglWinId, UGL_NULL_ID)   );
         }
      }
      else
      {
         UGL_STATUS_MONITOR(winAttach(_uglWinId, _display.rootWindow(), UGL_NULL_ID));
      }

      //
      // Notify display of new window
      //
      _display.add_window(this);
   }
}

void OSWindow::detach (void)
{
   if ( _uglWinId != UGL_NULL_ID )
   {
      if (winPointerGrabGet(NULL) == _uglWinId)
      {
         UGL_STATUS_MONITOR(winPointerUngrab (_uglWinId));
      }

      UGL_STATUS_MONITOR(winCbRemove(_uglWinId, OSWindow::uglDrawCallback));
      //
      // Detach all of this window's children
      //
      list<OSWindow*>::iterator childIter;
      for ( childIter = _childWindows.begin(); childIter!=_childWindows.end(); ++childIter )
      {
         OSWindow* child = *childIter;
         if (child && (child->_uglWinId != UGL_NULL_ID) )
         {
            UGL_STATUS_MONITOR(winDetach(child->_uglWinId)   );
         }
      }

      //
      // Notify display that window is no longer displayed
      //
      _display.remove_window(this);

      //
      // Notify parent that this window is being detached
      //
      if ( _parent )
      {
         _parent->delete_child_window(this);
         _parent = NULL;
      }

      //
      // Detach the underlying UGL window from its parent and then delete
      // the UGL window
      //
      if ( winParent(_uglWinId) != UGL_NULL_ID )
      {
         UGL_STATUS_MONITOR(winDetach(_uglWinId)   );
      }

      UGL_STATUS_MONITOR(winDestroy(_uglWinId)   );

      _uglWinId = UGL_NULL_ID;
   }
}

void OSWindow::add_object (OSWindowObject* obj)
{
   if (obj)
   {
      obj->_owner = this;
      invalidate_obj_region(obj);
   }
}


void OSWindow::add_object_to_front (OSWindowObject* obj)
{
   if (obj)
   {
      switch ( obj->clip_siblings() )
      {
         case OSWindowObject::Clipped :
            _clippedObjects.push_front(obj);
            break;

         case OSWindowObject::Transparency :
            _transparencyObjects.push_front(obj);
            break;

         case OSWindowObject::NotClipped :
            _nonClippedObjects.push_front(obj);
            break;
      }

      add_object(obj);
   }
}


void OSWindow::add_object_to_back (OSWindowObject* obj)
{
   if (obj)
   {
      switch ( obj->clip_siblings() )
      {
         case OSWindowObject::Clipped :
            _clippedObjects.push_back(obj);
            break;

         case OSWindowObject::Transparency :
            _transparencyObjects.push_back(obj);
            break;

         case OSWindowObject::NotClipped :
            _nonClippedObjects.push_back(obj);
            break;
      }

      add_object(obj);
   }
}

void OSWindow::delete_object (OSWindowObject* obj)
{
   if (obj && obj->_owner && (obj->_owner == this) )
   {
      //
      // Can't check clip_siblings method since delete_object is
      // called from OSWindowObject destructor, and clip_siblings()
      // is defined only by classes derived from OSWindowObject.  So
      // here we just attempt to remove the object from both lists.
      //
      _clippedObjects.remove(obj);
      _transparencyObjects.remove(obj);
      _nonClippedObjects.remove(obj);

      invalidate_obj_region(obj);
      obj->_owner = NULL;
   }
}

void OSWindow::move_object_to_front (OSWindowObject* obj)
{
   //
   // Order is important only for clipped objects
   //
   if (obj)
   {
      if ( obj->clip_siblings() == OSWindowObject::Clipped &&
           _clippedObjects.front() != obj )
      {
         _clippedObjects.remove(obj);
         _clippedObjects.push_front(obj);
         invalidate_obj_region(obj);
      }
      else if ( obj->clip_siblings() == OSWindowObject::Transparency
                && _transparencyObjects.front() != obj)
      {
         _transparencyObjects.remove(obj);
         _transparencyObjects.push_front(obj);

         invalidate_obj_region(obj);
      }
   }
}

void OSWindow::move_object_to_back (OSWindowObject* obj)
{
   //
   // Order is important only for clipped objects
   //
   if (obj)
   {
      if (obj->clip_siblings() == OSWindowObject::Clipped
          && _clippedObjects.back() != obj)
      {
         _clippedObjects.remove(obj);
         _clippedObjects.push_back(obj);
         invalidate_obj_region(obj);
      }
      else if ( obj->clip_siblings() == OSWindowObject::Transparency
                && _transparencyObjects.front() != obj)
      {
         _transparencyObjects.remove(obj);
         _transparencyObjects.push_back(obj);

         invalidate_obj_region(obj);
      }
   }
}

void OSWindow::set_object_region (OSWindowObject* obj, const OSRegion& newRegion)
{
   //
   // Since we may be resizing/moving the object, we need to invalidate
   // both the old object position and the new position.
   //
   if (obj)
   {
      invalidate_obj_region(obj);
      obj->_region = newRegion;
      invalidate_obj_region(obj);
   }
}

void OSWindow::set_object_visible (OSWindowObject* obj, bool newVisible)
{
   if ( obj && obj->_visible != newVisible )
   {
      //
      // Force _visible to true for call to invalidate_obj_region to insure
      // we really invalidate the region.
      //
      obj->_visible = true;
      invalidate_obj_region(obj);

      obj->_visible = newVisible;
   }
}

void OSWindow::setWindowVisibility (bool newVisible)
{
   if (newVisible != winVisibleGet(_uglWinId))
   {
      const UGL_RECT& rect = _region.convert_to_ugl_rect();

      UGL_STATUS_MONITOR(winVisibleSet(_uglWinId, newVisible)   );
      UGL_STATUS_MONITOR(winRectInvalidate(_uglWinId, &rect)   );
   }
}

void OSWindow::invalidateWindow (void)
{
   UGL_STATUS_MONITOR(winRectInvalidate(_uglWinId, NULL));
}


void OSWindow::invalidate_obj_region (OSWindowObject* obj)
{
   if ( _uglWinId != UGL_NULL_ID && obj &&
        obj->is_visible() )
   {
      UGL_RECT rect = obj->_region.convert_to_ugl_rect();

      //
      // If we are not currently drawing the window, we can simply
      // invalidate the region.  Otherwise, we must include the region
      // within the drawing area, since simply invalidating the region
      // will not automatically add it to the active clip region.
      //
      if ( _activeDrawRegion == UGL_NULL_ID )
      {
         //
         // Make sure that we invalidate at least one pixel.  Some
         // objects are not sized until first rendered, and initial
         // have a zero size region.  Expanding the region if
         // necessary insures that the parent window will be redrawn
         // and the object will get a chance to compute its region.
         //
         if ( rect.left >= rect.right ) rect.right = rect.left + 1;
         if ( rect.top >= rect.bottom ) rect.bottom = rect.top + 1;
         UGL_STATUS_MONITOR(winRectInvalidate(_uglWinId, &rect));

      }
      else
      {
         UGL_STATUS_MONITOR(uglRegionRectInclude(_activeDrawRegion, &rect)  );
      }
   }
}

bool OSWindow::disabled (void)
{
   if (_disabled) return true;

   //
   // The window is considered disabled if the window itself
   // or any of its parents are disabled.
   //
   bool      disabledFlag = _disabled;
   OSWindow* parent       = _parent;

   while ( disabledFlag == false &&
           parent )
   {
      disabledFlag = parent->_disabled;
      parent       = parent->_parent;
   }

   return disabledFlag;
}

void OSWindow::add_child_window (OSWindow* child)
{
   _childWindows.push_front(child);
}

void OSWindow::delete_child_window (OSWindow* child)
{
   _childWindows.remove(child);
}

void OSWindow::draw (UGL_GC_ID gc)
{
   if ( _uglWinId != UGL_NULL_ID)
   {
      _activeDrawRegion = uglRegionCreate();
      if (_activeDrawRegion == UGL_NULL)
         _FATAL_ERROR(__FILE__, __LINE__, "UGL ERROR: Failed to create region");


#if CPU==SIMNT
      // If in simulator, draw ALL VISIBLE windows, since the screenshot script is counting
      // on you to do so.  If you change this, it WILL BREAK THE STRING/SCREEN MAPPING!!!!
      UGL_STATUS_MONITOR(uglRegionUnion(_activeDrawRegion, winVisibleRegionGet(_uglWinId), _activeDrawRegion));
#else
      UGL_STATUS_MONITOR(uglRegionUnion(_activeDrawRegion, winDirtyRegionGet(_uglWinId), _activeDrawRegion));
#endif  /* if CPU=SIMNT */

      pre_draw_objects();

      if ( !uglRegionIsEmpty(_activeDrawRegion) )
      {
         draw_objects(gc);
      }

      UGL_STATUS_MONITOR(uglRegionDestroy(_activeDrawRegion)   );

      _activeDrawRegion = UGL_NULL_ID;
   }
}

void OSWindow::pre_draw_objects (void)
{
   list<OSWindowObject*>::iterator objIter;
   for ( objIter = _clippedObjects.begin(); objIter != _clippedObjects.end(); ++objIter )
   {
      OSWindowObject* obj = *objIter;
      if ( obj->_visible )
      {
         obj->pre_draw();
      }
   }


   for (objIter = _transparencyObjects.begin(); objIter != _transparencyObjects.end(); ++objIter)
   {
      OSWindowObject* obj = *objIter;
      if (obj && obj->_visible)
      {
         obj->pre_draw();
      }
   }


   for ( objIter = _nonClippedObjects.begin(); objIter != _nonClippedObjects.end(); ++objIter )
   {
      OSWindowObject* obj = *objIter;
      if ( obj->_visible )
      {
         obj->pre_draw();
      }
   }
}

void OSWindow::draw_objects (UGL_GC_ID gc)
{
   UGL_REGION_ID clippedDrawRegion = UGL_NULL_RESULT_MONITOR(uglRegionCreate());
   // if (clippedDrawRegion == UGL_NULL)
   //    _FATAL_ERROR(__FILE__, __LINE__, "Failed to create region");
   UGL_STATUS_MONITOR(uglRegionCopy(_activeDrawRegion, clippedDrawRegion)   );

   winDrawStart(_uglWinId, gc, false);
   UGL_STATUS_MONITOR(uglClipRegionSet(gc, clippedDrawRegion)   );

// #if CPU == SIMNT
//    UGL_STATUS_MONITOR(   uglRectangle(gc, 0, 0, _display.width(), _display.height())   );
// #endif  //  if CPU=SIMNT  */

   list<OSWindowObject*>::iterator objIter;
   for ( objIter = _clippedObjects.begin(); objIter != _clippedObjects.end(); ++objIter )
   {
      OSWindowObject* obj = *objIter;
      if ( obj && obj->_visible )
      {
         obj->draw(gc);

         const UGL_RECT& objRect = obj->_region.convert_to_ugl_rect();
         UGL_STATUS_MONITOR(uglRegionRectExclude(clippedDrawRegion, &objRect) );
         if ( uglRegionIsEmpty(clippedDrawRegion) )
         {
            break;
         }

         UGL_STATUS_MONITOR(uglClipRegionSet(gc, clippedDrawRegion)   );
      }
   }

   UGL_STATUS_MONITOR(uglClipRegionSet(gc, _activeDrawRegion)   );

   for (objIter = _transparencyObjects.begin(); objIter != _transparencyObjects.end(); ++objIter)
   {
      OSWindowObject* obj = *objIter;
      if (obj && obj->_visible)
      {
         obj->draw(gc);

         const UGL_RECT& objRect = obj->_region.convert_to_ugl_rect();
         UGL_STATUS_MONITOR(uglRegionCopy(_activeDrawRegion, clippedDrawRegion)   );
         UGL_STATUS_MONITOR(uglRegionClipToRect(clippedDrawRegion, &objRect)   );
         if (!uglRegionIsEmpty(clippedDrawRegion))
         {
            obj->draw(gc);
         }
      }
   }

   for ( objIter = _nonClippedObjects.begin(); objIter != _nonClippedObjects.end(); ++objIter )
   {
      OSWindowObject* obj = *objIter;

      if (!obj)
      {
         DataLog (log_level_gui_error) << "NULL object pointer." << endmsg;
         continue;
      }

      if ( obj->_visible )
      {
         const UGL_RECT& objRect = obj->_region.convert_to_ugl_rect();

         UGL_STATUS_MONITOR(uglRegionCopy(_activeDrawRegion, clippedDrawRegion)   );
         UGL_STATUS_MONITOR(uglRegionClipToRect(clippedDrawRegion, &objRect)   );
         if ( !uglRegionIsEmpty(clippedDrawRegion) ) obj->draw(gc);
      }
   }

   UGL_STATUS_MONITOR(winDrawEnd(_uglWinId, gc, true)   );

   UGL_STATUS_MONITOR(uglRegionDestroy(clippedDrawRegion)   );
}

void OSWindow::pointerEvent (const PointerEvent& event) { }

OSWindow* OSWindow::getParentPointer () { return _parent; }

UGL_STATUS OSWindow::uglDrawCallback (WIN_ID /*id*/, WIN_MSG* pMsg, void* pData, void* /*pParam*/)
{
   OSWindow* w = *(OSWindow**)pData;
   if (w && pMsg)
   {
      w->draw(pMsg->data.draw.gcId);
      return UGL_STATUS_FINISHED;
   }
   else
   {
      DataLog (log_level_gui_info) << "uglDrawCallback() has a NULL pointer." << endmsg;
      return UGL_STATUS_ERROR;
   }
}
unsigned long OSWindow::getCheckDestroyed ()
{
   return _checkDestruction;
}
UGL_STATUS OSWindow::uglPointerCallback (WIN_ID id, WIN_MSG* pMsg, void* pData, void* /*pParam*/)
{
   UGL_WINDOW_ID windowId = id;
   OSWindow*     window   = *(OSWindow**)pData;

   if (window->getCheckDestroyed() != 0xCAFEBABE)
   {
      DataLog(log_level_gui_error) << "OSWindow: pointer callback on window after destructor invoked. Check Value: "
                                   << window->getCheckDestroyed() << endmsg;
   }

   if ((windowId != UGL_NULL) && window && pMsg)
   {
      // We are only interested in left button press or release events
      if ((pMsg->data.ptr.buttonChange & 0x01) == 0) return UGL_STATUS_FINISHED;

      OSWindow::PointerEvent ptEvent;
      if ((pMsg->data.ptr.buttonState & 0x01) != 0)
      {
         // Left button is pressed.  Grab the pointer to insure
         // we get the release event as well.
         winPointerGrab (windowId);
         ptEvent.eventType = OSWindow::PointerEvent::ButtonPress;
      }
      else
      {
         winPointerUngrab (windowId);
         ptEvent.eventType = OSWindow::PointerEvent::ButtonRelease;
      }

      ptEvent.x = pMsg->data.ptr.position.x;
      ptEvent.y = pMsg->data.ptr.position.y;

      if (!window->disabled()) window->pointerEvent(ptEvent);
   }

   return UGL_STATUS_FINISHED;
}

/* FORMAT HASH 9b6e24b914ce87087cd13350e1aa17d3 */
