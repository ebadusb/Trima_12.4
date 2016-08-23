/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CONFIRM_BOX

*     File name:   confirm_box.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/confirm_box.cpp 1.9 2009/04/07 20:31:54Z dslausb Exp dslausb $
$Log: confirm_box.cpp $
Revision 1.9  2009/04/07 20:31:54Z  dslausb
IT 9101 - Use GUI_POOL_MALLOC where possible
Revision 1.8  2008/07/14 20:27:37Z  dslausb
More stringId changes requested by localization
Revision 1.7  2008/07/09 23:50:18Z  dslausb
More string ID standardization as requested by Marita
Revision 1.6  2008/07/07 18:25:28Z  dslausb
More localization string id standardization
Revision 1.5  2007/04/17 17:56:45Z  dslausb
IT 7472 - Added confirm box for RAS connection.
Revision 1.4  2006/11/06 21:03:38Z  dslausb
Reverted from dual guistring classes back to one (no longer necessary since we drastically increased the buffer size in IT 7165)
Revision 1.3  2006/08/17 20:42:47Z  dslausb
- Machine screen no longer has barcode button.  Scanned barcodes automagically go into the barcode field when you're on that screen.
- Confirm single deletion box show the REF number you're confirming deletion for
Revision 1.2  2006/08/03 22:52:48Z  dslausb
Fixed cassette deletion
Revision 1.1  2006/07/31 15:34:00Z  dslausb
Initial revision

*/

#include <vxworks.h>

/** include files **/

#include "confirm_box.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"

/** defines **/
#define  STRING_TOKEN           "~"
#define  __HORIZONTAL_MARGIN__  20
#define  __VERTICAL_MARGIN__    20
#define  __MIN_TEXT_HEIGHT__    100

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CONFIRM_BOX::Screen_CONFIRM_BOX()
   : Base_Window (BITMAP_NULL),

     _confirmText(textEmpty),

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

     _backgroundBitmap(0, 0, BITMAP_NULL)
{
   set_screen_id(GUI_SCREEN_CONFIRM_BOX);
}  // END of Screen_CONFIRM_BOX CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIRM_BOX::~Screen_CONFIRM_BOX()
{
   deallocate_resources ();

}  // END of Screen_CONFIRM_BOX DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   OSRegion       regionVar;
   TEXT_STRING_ID textId;
   BITMAP_ID      bgBmpId, upBtnBmpId, downBtnBmpId;
   bool           showCassetteId;

   int            window_xPos      = 0,
                  window_yPos      = 0,
                  window_width     = 0,
                  window_height    = 0,
                  confirmBtnWidth  = 0,
                  confirmBtnHeight = 0,
                  cancelBtnWidth   = 0,
                  cancelBtnHeight  = 0;

   {
      guistring allocstr(allocation_parameter);
      char*     tokstr    = allocstr.make_buffer();

      char*     nextToken = 0;

      _dataKey       = (CONFIG_DATA_KEYS)atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken));
      bgBmpId        = (BITMAP_ID)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      upBtnBmpId     = (BITMAP_ID)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      downBtnBmpId   = (BITMAP_ID)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      textId         = (TEXT_STRING_ID)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      window_xPos    = atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      window_yPos    = atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      showCassetteId = (bool)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));

      GUI_POOL_FREE(tokstr);
      DataLog (log_level_gui_info) << "Parsed: x position " << window_xPos
                                   << " - y position " << window_yPos
                                   << " - window_width " << window_width
                                   << " - window_height " << window_height
                                   << " - text ID " << textId
                                   << " - data key " << _dataKey
                                   << " - show cassette id " << showCassetteId
                                   << endmsg;
   }

   Base_Window::allocate_resources (allocation_parameter);
   _backgroundBitmap.set_id(bgBmpId);
   _backgroundBitmap.allocate_resources(*this);
   _btnConfirm.set_up_bitmap(upBtnBmpId);
   _btnConfirm.set_down_bitmap(downBtnBmpId);
   _btnConfirm.allocate_resources(*this);
   _btnCancel.set_up_bitmap(upBtnBmpId);
   _btnCancel.set_down_bitmap(downBtnBmpId);
   _btnCancel.allocate_resources(*this);

   _backgroundBitmap.get_dimensions(regionVar);
   window_width  = regionVar.width;
   window_height = regionVar.height;

   _btnConfirm.get_region(regionVar);
   confirmBtnWidth  = regionVar.width;
   confirmBtnHeight = regionVar.height;

   _btnCancel.get_region(regionVar);
   cancelBtnWidth  = regionVar.width;
   cancelBtnHeight = regionVar.height;

   _btnConfirm.deallocate_resources();
   _btnCancel.deallocate_resources();
   _backgroundBitmap.deallocate_resources();
   Base_Window::deallocate_resources();

   // Set up confirm button.
   regionVar.x = __HORIZONTAL_MARGIN__;
   regionVar.y = window_height - ( confirmBtnHeight + __VERTICAL_MARGIN__);
   _btnConfirm.set_position(regionVar.x, regionVar.y);

   // Set up cancel button.
   regionVar.x = window_width - ( cancelBtnWidth + __HORIZONTAL_MARGIN__);
   regionVar.y = window_height - ( cancelBtnHeight + __VERTICAL_MARGIN__);
   _btnCancel.set_position(regionVar.x, regionVar.y);

   regionVar.x      = __HORIZONTAL_MARGIN__;
   regionVar.y      = __VERTICAL_MARGIN__;
   regionVar.width  = window_width - ( 2 * __HORIZONTAL_MARGIN__);
   regionVar.height = window_height - ( confirmBtnHeight + ( 3 * __VERTICAL_MARGIN__));

   _confirmText.set_string_id(textId);
   _confirmText.set_alignment(CENTER);
   _confirmText.set_requested_region(regionVar);

   // Add extra input string to current string, if necessary.
   if (showCassetteId)
   {
      guistring confirmTextString;
      _confirmText.get_text_direct(confirmTextString);

      confirmTextString += "\n\n";
      confirmTextString += guiglobs::cassette_mgr->getCassetteToDelete();

      _confirmText.set_text_direct(confirmTextString.length(), confirmTextString);
   }

   regionVar.x      = window_xPos;
   regionVar.y      = window_yPos;
   regionVar.width  = window_width;
   regionVar.height = window_height;

   set_region(regionVar);
   Base_Window::allocate_resources (allocation_parameter);

   _backgroundBitmap.allocate_resources(*this);
   _confirmText.allocate_resources(*this);
   _btnConfirm.allocate_resources(*this);
   _btnCancel.allocate_resources(*this);

   _btnConfirm.set_callback(confirmBoxConfirmCb, (void*)this);
   _btnCancel.set_callback(confirmBoxCancelCb, (void*)this);
}  // END of Screen_CONFIRM_BOX ALLOCATE_RESOURCES


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _confirmText.deallocate_resources();
   _btnConfirm.deallocate_resources();
   _btnCancel.deallocate_resources();
   _backgroundBitmap.deallocate_resources();

   Base_Window::deallocate_resources ();
}   // END of Screen_CONFIRM_BOX DEALLOCATE_RESOURCES


void Screen_CONFIRM_BOX::sendConfirmation ()
{
   DataLog (log_level_gui_info) << "Sending confirmation message." << endmsg;
   ConfirmBoxConfirmMsg confirmMsg(MessageBase::SEND_LOCAL);
   confirmMsg.send(_dataKey);
}

void Screen_CONFIRM_BOX::sendCancelation ()
{
   DataLog (log_level_gui_info) << "Sending cancelation message." << endmsg;
   ConfirmBoxCancelMsg cancelMsg(MessageBase::SEND_LOCAL);
   cancelMsg.send(_dataKey);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void Screen_CONFIRM_BOX::confirmBoxCancelCb (void* data)
{
   ((Screen_CONFIRM_BOX*)data)->sendCancelation();

   // Tell the window manager to remove the most recent child screen.
   RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
   eventmsg.send();
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void Screen_CONFIRM_BOX::confirmBoxConfirmCb (void* data)
{
   ((Screen_CONFIRM_BOX*)data)->sendConfirmation();

   // Tell the window manager to remove the most recent child screen.
   RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
   eventmsg.send();
}



guistring Screen_CONFIRM_BOX::createAllocationString (CONFIG_DATA_KEYS dataKey,
                                                      BITMAP_ID bgBmpId,
                                                      BITMAP_ID upBtnBmpId,
                                                      BITMAP_ID downBtnBmpId,
                                                      TEXT_STRING_ID textId,
                                                      int xPos,
                                                      int yPos,
                                                      bool showCassetteId)
{
   guistring allocationString;
   allocationString.initialize();

   sprintf(allocationString, "%d~%d~%d~%d~%d~%d~%d~%d",
           dataKey,
           bgBmpId,
           upBtnBmpId,
           downBtnBmpId,
           textId,
           xPos,
           yPos,
           (int)showCassetteId);

   return allocationString;
}

/* FORMAT HASH 3a342096f91d8ddb7450b3de03f04ca7 */
