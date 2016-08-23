/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  ConfirmBoxBase

*     File name:   confirmBoxBase.hpp

*     Contents:    See prototype header descriptions below...

*     Description:  This class provides a base class for confirmation boxes.  It's
        not meant to be instantiated directly, it's meant to be derived from in
        child classes.  See rasConnectConfirmBox and cassetteDeleteConfirmBox for
        examples of this.

        This class provides the underlying functionality for a child screen with
        two buttons: confirm or cancel, similar to a dialog box in Windows.  Parent
        classes must implement handlers to listen for the continue and confirm
        messages, if action needs to be taken.

        This class also needs a certain amount of setup data which may or may not
        be available at construction time, so it has a function called setConfirmBoxData()
        that must be called, and is generally called in the constructor if possible,
        or at the beginning of the allocate_resources() function.


*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef __CONFIRM_BOX_BASE_HPP__
#define __CONFIRM_BOX_BASE_HPP__

/** include files **/
#include "message.h"
#include "aphwind.hpp"
#include "button_bitmap.h"



class ConfirmBoxBase
   : public Base_Window
{
protected:

   bool _dataWasSet;

   static void confirmBoxCancelCb (void* data);
   static void confirmBoxConfirmCb (void* data);

   // These are virtual so that inherited classes can override their functionality.
   virtual void handleCancelBtn ();
   virtual void handleConfirmBtn ();

   CONFIG_DATA_KEYS _dataKey;

   Button_Bitmap    _btnConfirm, _btnCancel;
   Bitmap           _backgroundBitmap;

   // This function is meant to be called by the derived class prior to calling this class's allocation
   // function.  It takes the window's config data key, background bitmap, the window's
   // x and y coordinates, 2 bitmaps for up and down on the buttons (it's assumed that both buttons
   // use the same up and down bitmaps), and x and y coordinates for the both buttons.
   // Anything else must be done in an inherited class.
   void setConfirmBoxData (CONFIG_DATA_KEYS dataKey,
                           BITMAP_ID bgBmpId,
                           int xPos,
                           int yPos,
                           BITMAP_ID upBtnBmpId,
                           BITMAP_ID downBtnBmpId,
                           int bottomMargin,
                           int leftMargin,
                           int rightMargin);

   int getBtnHeight ();

   void removeScreen ();
public:

   // CONSTRUCTOR
   ConfirmBoxBase();

   // DESTRUCTOR
   virtual ~ConfirmBoxBase();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void sendConfirmation ();
   virtual void sendCancelation ();
}; // END of ConfirmBoxBase Class Declaration

// End of single include define guardian
#endif

/* FORMAT HASH 89542ffae7c27bef5c12b9219d7e449f */
