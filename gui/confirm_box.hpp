/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CONFIRM_BOX

*     File name:   confirm_box.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/confirm_box.hpp 1.3 2006/08/17 20:42:52Z dslausb Exp dslausb $
$Log: confirm_box.hpp $
Revision 1.3  2006/08/17 20:42:52Z  dslausb
- Machine screen no longer has barcode button.  Scanned barcodes automagically go into the barcode field when you're on that screen.
- Confirm single deletion box show the REF number you're confirming deletion for
Revision 1.2  2006/08/03 22:52:53Z  dslausb
Fixed cassette deletion
Revision 1.1  2006/07/31 15:33:53Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef __CONFIRM_BOX_HPP__
#define __CONFIRM_BOX_HPP__

/** include files **/
#include "message.h"
#include "aphwind.hpp"
#include "button_bitmap.h"



class Screen_CONFIRM_BOX
   : public Base_Window
{
protected:

   static void confirmBoxCancelCb (void* data);
   static void confirmBoxConfirmCb (void* data);

   CONFIG_DATA_KEYS _dataKey;

   Display_Text     _confirmText;

   Button_Bitmap    _btnConfirm, _btnCancel;

   Bitmap           _backgroundBitmap;

public:

   // CONSTRUCTOR
   Screen_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_CONFIRM_BOX();

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

   static guistring createAllocationString (CONFIG_DATA_KEYS dataKey,
                                            BITMAP_ID bgBmpId,
                                            BITMAP_ID upBtnBmpId,
                                            BITMAP_ID downBtnBmpId,
                                            TEXT_STRING_ID textId,
                                            int xPos,
                                            int yPos,
                                            bool showCassetteId);
}; // END of Screen_CONFIRM_BOX Class Declaration

// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of confirm_box.hpp file

/* FORMAT HASH 0e91895f7ba3fc94640e6d62fd0a4e88 */
