/**
 * Copyright (C) 2013 Terumo BCT, Inc.  All rights reserved.
 *
 *  Author:      Mat Ramey
 *
 *  Class name:  Screen_PAS_BARCODE_CONFIRM_BOX
 *
 *  File name:   pasBarcodeConfirmBox.cpp
 *
 *  Description: See details in companion header file
 *
 */

#include <vxworks.h>

/** include files **/

#include "pasBarcodeConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_PAS_BARCODE_CONFIRM_BOX::Screen_PAS_BARCODE_CONFIRM_BOX()
   : ConfirmBoxBase(),
     _pasBagBarcodeMsg(Callback<Screen_PAS_BARCODE_CONFIRM_BOX>(this, &Screen_PAS_BARCODE_CONFIRM_BOX::handlePasBagBarcodeMsg),
                       MessageBase::SNDRCV_RECEIVE_ONLY),
     _txtBody(textConfirmPasBarcode),
     _barcodeIsValid(false)
{
   set_screen_id(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX);

   // Change the text of Cancel button to be End Addition
   _btnCancel.set_string_id(textButtonAlarmEndAddition);
#if (CPU==SIMNT)
   _screenTraverseMessage = NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_PAS_BARCODE_CONFIRM_BOX::~Screen_PAS_BARCODE_CONFIRM_BOX()
{
   Screen_PAS_BARCODE_CONFIRM_BOX::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_PAS_BARCODE_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   DataLog(log_level_gui_info) << "allocating: " << allocation_parameter << endmsg;

   // Check for the case where we switch to the invalid bag scan case
   if ( strcmp(allocParamForBadBarcode(), allocation_parameter) == 0 )
   {
      setConfirmBoxData(KEY_PAS_CONNECT_CONFIRM,
                        BITMAP_CONFIRM_BOX_RED_LARGE,
                        130, 120,
                        BITMAP_BTN_RED_UP,
                        BITMAP_BTN_RED_DOWN,
                        35, 40, 40);

      _txtBody.set_string_id(textConfirmPasBarcodeInvalid);
   }
   else // nominal case
   {
      setConfirmBoxData(KEY_PAS_CONNECT_CONFIRM,
                        BITMAP_CONFIRM_BOX_BURGUNDY_LARGE,
                        130, 120,
                        BITMAP_BTN_RED_UP,
                        BITMAP_BTN_RED_DOWN,
                        35, 40, 40);

      _txtBody.set_string_id(textConfirmPasBarcode);
   }

   // Now allocate the base class elements
   ConfirmBoxBase::allocate_resources(allocation_parameter);
   _txtBody.allocate_resources(*this);

   // turn off the Confirm button (it's not used)
   _btnConfirm.deallocate_resources();
#if (CPU==SIMNT)
   _screenTraverseMessage = new SimntTraverseMsg(Callback<Screen_PAS_BARCODE_CONFIRM_BOX>(this, &Screen_PAS_BARCODE_CONFIRM_BOX::handleTraverseMessage), MessageBase::SNDRCV_RECEIVE_ONLY);
#endif
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_PAS_BARCODE_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
#if CPU==SIMNT
   if (_screenTraverseMessage)
   {
      delete _screenTraverseMessage;
      _screenTraverseMessage = NULL;
   }
#endif /* if CPU==SIMNT */
}


void Screen_PAS_BARCODE_CONFIRM_BOX::handleCancelBtn ()
{
   sendCancelMsg(false); // false => End Addition
   removeScreen();
}


void Screen_PAS_BARCODE_CONFIRM_BOX::sendCancelMsg (bool isBadBarcode)
{
   DataLog (log_level_gui_info) << "Sending cancellation message: isBadBarcode=" << isBadBarcode << endmsg;
   ConfirmBoxCancelMsg cancelMsg(MessageBase::SEND_LOCAL);
   cancelMsg.send(isBadBarcode);
}


void Screen_PAS_BARCODE_CONFIRM_BOX::handlePasBagBarcodeMsg ()
{
   // This is a special-case handler for recognizing the barcode from a PAS bag
   //
   PAS_BAG_BARCODE_MSG msg = _pasBagBarcodeMsg.getData();

   // Terminate the string
   msg.pasBagString[msg.pasBagLength] = 0;

   if (_barcodeIsValid)
   {
      // Ignore this one as we've already scanned a valid PAS bag
      DataLog(log_level_gui_info) << "Already got a valid PAS bag barcode. Ignoring: " << msg.pasBagString << endmsg;
   }
   else if (isValidPasBagBarcode(msg.pasBagString))
   {
      // Got a good barcode. Proceed as though a Confirm button was pressed
      DataLog(log_level_gui_info) << "Got VALID PAS bag barcode: " << msg.pasBagString << endmsg;
      _barcodeIsValid = true;
      handleConfirmBtn();
   }
   else
   {
      // No go. Send cancel message indicating bad scan and remove myself
      DataLog(log_level_gui_info) << "Got INVALID PAS bag barcode: " << msg.pasBagString << endmsg;
      sendCancelMsg(true);
      removeScreen();
   }
}

bool Screen_PAS_BARCODE_CONFIRM_BOX::isValidPasBagBarcode (const char* barcode)
{
   // Current needs dictate a hard-coded set of numbers, so this is a very
   // simplistic implementation.

   static const char* TheValidSet[2] =
   {
      "0110314537408503",  // NDC barcode string for B-Braun Isoplate bag
      "0100309429601127",  // NDC barcode string for Intersol PAS 3 (excluding leading char)
   };

   // Note: for Fenwal, just check for the *substring*; the label we
   // tested with contained a non-printable leading character (^A).
   //
   const bool isValid( (strcmp(barcode, TheValidSet[0]) == 0) ||
                       (strstr(barcode, TheValidSet[1]) != NULL) );
   return isValid;
}

#if (CPU==SIMNT)

void Screen_PAS_BARCODE_CONFIRM_BOX::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();
   if (traverseInfo.type == CHANGE_LABELS )
   {
      setConfirmBoxData(KEY_PAS_CONNECT_CONFIRM,
                        BITMAP_CONFIRM_BOX_RED_LARGE,
                        130, 120,
                        BITMAP_BTN_RED_UP,
                        BITMAP_BTN_RED_DOWN,
                        35, 40, 40);

      _txtBody.set_string_id(textConfirmPasBarcodeInvalid);
   }
   else if (traverseInfo.type == UNIVERSAL )
   {
      setConfirmBoxData(KEY_PAS_CONNECT_CONFIRM,
                        BITMAP_CONFIRM_BOX_BURGUNDY_LARGE,
                        130, 120,
                        BITMAP_BTN_RED_UP,
                        BITMAP_BTN_RED_DOWN,
                        35, 40, 40);

      _txtBody.set_string_id(textConfirmPasBarcode);
   }
}
#endif

/* FORMAT HASH f60a9d6ed744adef3a23c3259a74739b */
