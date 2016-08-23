/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Base_Window

*     File name:    bwindow.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class is the base class for all screens used in the Focussed GUI
         domain.  It manages the window background bitmap, the screen ID, the
         screen invocation type ID.  This is the base class that the Screen
         Commutator manages -- all the particular screens polymorphically derive
         from this object.

*************************< FOCUSSED GUI DOMAIN >****************************/


// Single include guardian define
#ifndef BWINDOW_HPP
#define BWINDOW_HPP

#include <vxworks.h> // Needed for CPU compiler directives.
#include <dirent.h>
#include <sys/stat.h>

// Focussed #INCLUDES

#include "oswindow.hpp"
#include "bitmap.h"
#include "gui.h"
#include "guistring.h"
#include "ostime.hpp"

#if CPU == SIMNT
#include "display_text.h"
#endif // #if CPU == SIMNT

class Base_Window
   : public OSWindow
{
public:
   enum DisplayOrderType
   {
      DisplayOrder_Normal, DisplayOrder_ClearBackground
   };

   // If empty, filename will be updated with the generated filename
   static void screenshot (guistring& filename);

protected:

   static int     _snapNumber;
   static rawTime _snapTime;
   static void screenshotName (guistring& filename);

   BITMAP_ID backgrnd_bitmap_id;
   Bitmap    background_bitmap;

#if CPU == SIMNT
public:
   void setScreenshotName (const char* name);
   void removeScreenshotName ();
   void setScreenshotInfo (const char* info);
   void removeScreenshotInfo ();
protected:
   Display_Text _txtScreenshotName;
   Display_Text _txtScreenshotInfo;
#endif /* if CPU == SIMNT */

   // Flag designating how current screen was invoked.
   GUI_SCREEN_INVOCATION screen_invocation_type;


   // Logical ID of the current Window
   GUI_SCREEN_ID window_ID;

   // Repository of the MAXSIZE_BWINDOW_PARAM char (MAX) parameter passed at
   // window allocation.
   guistring allocation_parameter;


   // SET_ALLOCATION_PARAMETER
   // This method stores the allocation parameter string passed by the commutator
   // when it allocates this screen.
   void set_allocation_parameter (const char* alloc_param);


   DisplayOrderType _displayOrder;
   bool             _allocated;


public:

   // CONSTRUCTOR
   Base_Window(BITMAP_ID background = BITMAP_DEFAULT_BACKGROUND, DisplayOrderType displayOrder = DisplayOrder_Normal);


   // DESTRUCTOR
   virtual ~Base_Window ();

   // ALLOCATE_RESOURCES
   // This method will allocate a widget and attach it under the parent
   // (container) widget passed as a parameter. If the filename was specified
   // at constructor time, the bitmap will be allocated and attached to the
   // widget.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method will delete the photon widget and any other manageable
   // resources.
   virtual void deallocate_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method is virtual and should be overriden by any derived classes that
   // must have their internal states reset.
   virtual void reset_internal_screen_state ();


   // SET_SCREEN_ID
   // This method will set the logical ID of the current screen. [Note: this fn,
   // and the following, non-virtual for many reasons -- getters and setters are
   // generally not good candidates for polymorphism. --DGS]
   void set_screen_id (GUI_SCREEN_ID screen_id);


   // GET_SCREEN_ID
   // This method will return the logical ID of the current screen
   GUI_SCREEN_ID get_screen_id () const { return window_ID; }


   // SET_SCREEN_INVOCATION_TYPE
   // This method will set the invocation type of the current screen
   virtual void set_screen_invocation_type (GUI_SCREEN_INVOCATION invocation_type);


   // GET_SCREEN_INVOCATION_TYPE
   // This method will return the invocation type of the current screen
   virtual GUI_SCREEN_INVOCATION get_screen_invocation_type ();


   // SET_BACKGROUND_BITMAP
   // This method loads the background bitmap and links it to the base window
   // widget for subsequent display.
   virtual void set_background_bitmap (BITMAP_ID background);


   // GET_ALLOCATION_PARAMETER
   // This method returns the allocation parameter
   const guistring& get_allocation_parameter ();
   virtual int      allocated () { return _allocated; }


   // REFRESH_RESOURCES
   // This method exists for overrides in derived classes
   virtual void refresh_resources ();

#if CPU==SIMNT
// Keep track of all the Base_Windows created, so we can get the
// coordinates of the center of each button given the button ID
protected:
   static Base_Window* array[GUI_SCREEN_TOTAL_SCREENS];
public:
   static Base_Window* get_window_from_id (GUI_SCREEN_ID id)    { return array[id]; }
#endif /* if CPU==SIMNT */

}; // END of Base_Window Class Declaration


// End of single include guardian define
#endif

/* FORMAT HASH e62655c3e5e0583aeb4965ac7a05f1e5 */
