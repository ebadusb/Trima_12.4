/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  ConfirmBoxBase

*     File name:   confirmBoxBase.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/confirmBoxBase.cpp 1.3 2008/07/14 20:27:39Z dslausb Exp dslausb $
$Log: confirmBoxBase.cpp $
Revision 1.3  2008/07/14 20:27:39Z  dslausb
More stringId changes requested by localization
Revision 1.2  2008/07/09 23:50:09Z  dslausb
More string ID standardization as requested by Marita
Revision 1.1  2007/10/18 20:22:42Z  dslausb
Initial revision

*/

#include <vxworks.h>

/** include files **/

#include "confirmBoxBase.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
ConfirmBoxBase::ConfirmBoxBase()
   : Base_Window (BITMAP_NULL),

     _dataKey        (LAST_CONFIG_INDEX_KEY), // a reasonable 'null' value I think

     _btnConfirm     (GUI_BUTTON_CONFIRM_BOX_CONFIRM,
                      0,
                      0,
                      textButtonConfirmBoxConfirm,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME),

     _btnCancel      (GUI_BUTTON_CONFIRM_BOX_CANCEL,
                      0,
                      0,
                      textButtonConfirmBoxCancel,
                      CFG_Look::BUTTON_UP_NAME,
                      CFG_Look::BUTTON_DOWN_NAME),

     _backgroundBitmap(0, 0, BITMAP_NULL),

     _dataWasSet(false)
{}  // END of ConfirmBoxBase CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
ConfirmBoxBase::~ConfirmBoxBase()
{
   ConfirmBoxBase::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void ConfirmBoxBase::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   if (!_dataWasSet)
   {
      DataLog (log_level_gui_error) << "We really can't allocate this screen, since you never set up the data for it." << endmsg;
      return;
   }

   Base_Window::allocate_resources (allocation_parameter);
   _backgroundBitmap.allocate_resources(*this);
   _btnConfirm.allocate_resources(*this);
   _btnCancel.allocate_resources(*this);
   _btnConfirm.set_callback(confirmBoxConfirmCb, (void*)this);
   _btnCancel.set_callback(confirmBoxCancelCb, (void*)this);
}  // END of ConfirmBoxBase ALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void ConfirmBoxBase::deallocate_resources ()
{
   if (!_allocated) return;

   _btnConfirm.deallocate_resources();
   _btnCancel.deallocate_resources();
   _backgroundBitmap.deallocate_resources();

   Base_Window::deallocate_resources ();
}   // END of ConfirmBoxBase DEALLOCATE_RESOURCES


void ConfirmBoxBase::sendConfirmation ()
{
   DataLog (log_level_gui_info) << "Sending confirmation message: " << _dataKey << endmsg;
   ConfirmBoxConfirmMsg confirmMsg(MessageBase::SEND_LOCAL);
   confirmMsg.send(_dataKey);
}

void ConfirmBoxBase::sendCancelation ()
{
   DataLog (log_level_gui_info) << "Sending cancellation message: " << _dataKey << endmsg;
   ConfirmBoxCancelMsg cancelMsg(MessageBase::SEND_LOCAL);
   cancelMsg.send(_dataKey);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void ConfirmBoxBase::confirmBoxCancelCb (void* data)
{
   ((ConfirmBoxBase*)data)->handleCancelBtn();
}

void ConfirmBoxBase::handleCancelBtn ()
{
   sendCancelation();
   removeScreen();
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void ConfirmBoxBase::confirmBoxConfirmCb (void* data)
{
   ((ConfirmBoxBase*)data)->handleConfirmBtn();
}

void ConfirmBoxBase::handleConfirmBtn ()
{
   sendConfirmation();
   removeScreen();
}

void ConfirmBoxBase::setConfirmBoxData (CONFIG_DATA_KEYS dataKey,
                                        BITMAP_ID bgBmpId,
                                        int xPos,
                                        int yPos,
                                        BITMAP_ID upBtnBmpId,
                                        BITMAP_ID downBtnBmpId,
                                        int bottomMargin,
                                        int leftMargin,
                                        int rightMargin)
{
   _dataKey = dataKey;

   // Figure out the dimensions of this box based on
   // those of the background bitmap.
   _backgroundBitmap.set_id(bgBmpId);

   OSRegion  regionVar    = _backgroundBitmap.get_dimensions();
   const int windowHeight = regionVar.height;
   const int windowWidth  = regionVar.width;

   // Set the box's x and y position
   regionVar.x = xPos;
   regionVar.y = yPos;
   set_region(regionVar);

   _btnConfirm.set_up_bitmap(upBtnBmpId);
   _btnConfirm.set_down_bitmap(downBtnBmpId);

   _btnCancel.set_up_bitmap(upBtnBmpId);
   _btnCancel.set_down_bitmap(downBtnBmpId);

   _btnConfirm.get_region(regionVar);
   const int btnHeight   = regionVar.height;
   const int btnWidth    = regionVar.width;
   const int btnY        = windowHeight - (btnHeight + bottomMargin);
   const int confirmBtnX = leftMargin;
   const int cancelBtnX  = windowWidth - (btnWidth + rightMargin);

   _btnConfirm.set_position(confirmBtnX, btnY);
   _btnCancel.set_position(cancelBtnX, btnY);

   _dataWasSet = true;
}

int ConfirmBoxBase::getBtnHeight ()
{
   // It's expected that the two buttons are the same height.
   // This info may be needed for formatting text in child classes.

   OSRegion region;
   _btnConfirm.get_region(region);
   return region.height;
}

void ConfirmBoxBase::removeScreen ()
{
   // Now remove this screen, based on how it was invoked
   if (get_screen_invocation_type() == GUI_SCREEN_INVOKE_STATELESS)
   {
      // Tell the window manager to remove this most recent stateless screen.
      RemoveMostRecentStatelessScreenMsg eventmsg(MessageBase::SEND_LOCAL);
      eventmsg.send(0);
   }
   else // GUI_SCREEN_INVOKE_CHILD
   {
      // Tell the window manager to remove this most recent child screen.
      RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
      eventmsg.send();
   }
}

/* FORMAT HASH e44bbe446fc9eb0e88bd887ad9082536 */
