/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Caridian BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PAS_EMPTY

*     File name:   pasEmpty.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "pasEmpty.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "pasBarcodeConfirmBox.hpp"

static void pasEmptyCbCancelBtn (void* data);
static void pasEmptyCbContinueBtn (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//

Screen_PAS_EMPTY::Screen_PAS_EMPTY()
   : OipAlarmBase(TEAL, GUI_SCREEN_PAS_EMPTY),

     _bmpTxtCartouche (10, 115, BITMAP_PAS_EMPTY_ALARM_BG),

     _txtPasEmptyInstruction (textMiscPasEmpty),

     _btnContinue   (GUI_BUTTON_PAS_EMPTY_CONTINUE,
                     486, 115,
                     textButtonLongContinue,
                     BITMAP_BUTTON_CONTINUE_UP,
                     BITMAP_BUTTON_CONTINUE_DN),

     _btnCancel     (GUI_BUTTON_PAS_EMPTY_CANCEL,
                     486, 407,
                     textButtonAlarmEndAddition,
                     BITMAP_BUTTON_EXIT_UP,
                     BITMAP_BUTTON_EXIT_DN),

     _pasRemainingData(textDataLabelPasVolRemaining,
                       textEmptyFormatPasEmptyData,
                       textDataUnitsPasVolRemaining),

     _pasVolMsg(Callback<Screen_PAS_EMPTY>(this, &Screen_PAS_EMPTY::gotPasRemainingMsg)),

     _connectionConfirmed(),
     _connectionCanceled(),
     _msgCancelMssAll(Callback<Screen_PAS_EMPTY>(this,  &Screen_PAS_EMPTY::hdlCancelMssAll)),
     _msgCancelMssPartial(Callback<Screen_PAS_EMPTY>(this,  &Screen_PAS_EMPTY::hdlCancelMssPartial)),
     _isPasBarcodeFeatureOn(false),
     _isPasBarcodePending(false),
     _isAdditionEnded(false)
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_PAS_EMPTY::~Screen_PAS_EMPTY()
{
   if (_allocated)
   {
      // If we haven't already deallocated, do it now.
      Screen_PAS_EMPTY::deallocate_resources();
   }
}


// Implements parent interface; called by parent's allocate_resources()
void Screen_PAS_EMPTY::setUpOipInfo (const char* allocation_parameter)
{
   // IT-13701 made MSS_PLT_SS_EMPTY_RED_GREEN obsolete
   _alarmId        = MSS_PLT_SS_EMPTY;
   _isBeepingAlarm = false;
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_PAS_EMPTY::allocate_resources (const char* allocation_parameter)
{
   OipAlarmBase::allocate_resources(allocation_parameter);

   // Allocate the bitmaps
   _bmpTxtCartouche.allocate_resources(*this);
   _txtPasEmptyInstruction.allocate_resources(*this);

   // Allocate both buttons
   _btnContinue.allocate_resources(*this);
   _btnCancel.allocate_resources(*this);
   _pasRemainingData.allocate_resources(*this);

   // Link the buttons to their "C" callback stubs
   _btnContinue.set_callback (pasEmptyCbContinueBtn, (void*)this);
   _btnCancel.set_callback   (pasEmptyCbCancelBtn, (void*)this);

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);

   Gui2ProcRequestRemainingPasVolMsg volMsg(MessageBase::SEND_GLOBAL);
   volMsg.send();

   // Check for the PAS Bag Barcode Scan feature
   _isPasBarcodeFeatureOn = Software_CDS::GetInstance().getFeature(ConfirmPasBagBarcodeScan);
   if (_isPasBarcodeFeatureOn)
   {
      _isPasBarcodePending = true;
      DataLog(log_level_gui_info) << "ConfirmPasBagBarcodeScan is ON" << endmsg;
   }
   else
   {
      DataLog(log_level_gui_info) << "ConfirmPasBagBarcodeScan is OFF" << endmsg;
   }
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_PAS_EMPTY::deallocate_resources ()
{
   // Remove alarm status line color and message
   guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

   _bmpTxtCartouche.deallocate_resources();

   _txtPasEmptyInstruction.deallocate_resources();

   _btnContinue.deallocate_resources();
   _btnCancel.deallocate_resources();
   _pasRemainingData.deallocate_resources();

   // deallocate parent's resources
   OipAlarmBase::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Screen_PAS_EMPTY::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();

   // Note: this refresh is invoked after allocate_resources() and
   // whenever operator chooses a button on the End Addition screen.

   // When user has chosen to end PAS or ALL addition, implicitly dismiss
   // the alarm by emulating a final Continue button press.
   //
   if ( _isAdditionEnded )
   {
      continueOip();
   }
   //
   // Else, open the confirmation box to prompt for a PAS bag barcode scan?
   //
   else if ( _isPasBarcodeFeatureOn && _isPasBarcodePending &&
             (GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX != guiglobs::screen_status.get_current_displayed_screen()) )
   {
      callConfirmBarcodeBox();
   }
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Screen_PAS_EMPTY::processCancelBtn ()
{
   cancelOip();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_PAS_EMPTY::processContinueBtn ()
{
   if (_isPasBarcodeFeatureOn)
   {
      // Continue button doesn't get enabled until barcode has been confirmed
      continueOip();
   }
   else // pre-feature behavior
   {
      // Here, Continue triggers the confirmation prompt
      callConfirmBox();
   }
}


void Screen_PAS_EMPTY::gotPasRemainingMsg ()
{
   int volData = _pasVolMsg.getData();
   DataLog (log_level_gui_info) << "GUI received volume data " << volData << " from proc." << endmsg;

   _pasRemainingData.setData(volData);
}


void Screen_PAS_EMPTY::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ALARM_STATUS_LINE, textStatPasEmpty);
}


void Screen_PAS_EMPTY::callConfirmBox ()
{
   DataLog (log_level_gui_info) << "Putting up PAS confirmation message." << endmsg;

   _btnContinue.deallocate_resources();  // Continue OFF
   createConfirmCallbacks();

   goto_stateless_screen(GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX, "PAS CONNECT CONFIRMATION BOX");
}

void Screen_PAS_EMPTY::connectionConfirmed ()
{
   DataLog (log_level_gui_info) << "PAS Confirmed Callback." << endmsg;
   destroyConfirmCallbacks();
   continueOip();
}

void Screen_PAS_EMPTY::connectionCanceled ()
{
   DataLog (log_level_gui_info) << "PAS Canceled Callback." << endmsg;
   destroyConfirmCallbacks();
   _btnContinue.allocate_resources(*this);  // Continue ON
}

void Screen_PAS_EMPTY::callConfirmBarcodeBox ()
{
   DataLog(log_level_gui_info) << "Putting up PAS BARCODE confirmation box." << endmsg;

   _btnContinue.deallocate_resources();  // Continue OFF
   createConfirmCallbacks();

   goto_stateless_screen(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX, "PAS BARCODE CONFIRMATION BOX");
}

void Screen_PAS_EMPTY::callInvalidBarcodeBox ()
{
   DataLog(log_level_gui_info) << "Putting up INVALID PAS BARCODE box." << endmsg;

   goto_stateless_screen(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX,
                         Screen_PAS_BARCODE_CONFIRM_BOX::allocParamForBadBarcode());
}

void Screen_PAS_EMPTY::barcodeConfirmed ()
{
   DataLog (log_level_gui_info) << "PAS Barcode Confirmed Callback." << endmsg;
   destroyConfirmCallbacks();
   _btnContinue.allocate_resources(*this);  // Continue ON
   _isPasBarcodePending = false;
}

void Screen_PAS_EMPTY::barcodeCanceledOrInvalid ()
{
   // The canceled message value indicates either:
   // 0 : the operator selected End Addition, or
   // 1 : an invalid barcode was scanned
   //
   if ( _connectionCanceled.getData() == 0 )
   {
      DataLog(log_level_gui_info) << "PAS Barcode was Canceled." << endmsg;
      destroyConfirmCallbacks();
      processCancelBtn();       // End the addition
   }
   else
   {
      DataLog(log_level_gui_info) << "PAS Barcode was Invalid." << endmsg;
      callInvalidBarcodeBox();  // Prompt the operator again
   }
}

void Screen_PAS_EMPTY::hdlCancelMssAll ()
{
   DataLog(log_level_gui_info) << "End all MSS Addition" << endmsg;
   _isAdditionEnded = true;
}

void Screen_PAS_EMPTY::hdlCancelMssPartial ()
{
   if (END_PAS_ADDITION == _msgCancelMssPartial.getData() )
   {
      DataLog(log_level_gui_info) << "End PAS Addition" << endmsg;
      _isAdditionEnded = true;
   }
}


void Screen_PAS_EMPTY::createConfirmCallbacks ()
{
   if (_isPasBarcodeFeatureOn)
   {
      _connectionConfirmed.init(Callback<Screen_PAS_EMPTY>(this, &Screen_PAS_EMPTY::barcodeConfirmed));
      _connectionCanceled.init(Callback<Screen_PAS_EMPTY>(this, &Screen_PAS_EMPTY::barcodeCanceledOrInvalid));
   }
   else
   {
      _connectionConfirmed.init(Callback<Screen_PAS_EMPTY>(this, &Screen_PAS_EMPTY::connectionConfirmed));
      _connectionCanceled.init(Callback<Screen_PAS_EMPTY>(this, &Screen_PAS_EMPTY::connectionCanceled));
   }
}

void Screen_PAS_EMPTY::destroyConfirmCallbacks ()
{
   _connectionConfirmed.deregisterMsg();
   _connectionCanceled.deregisterMsg();
}


////////////////////////////////////////////////////////////////////////////////
// CONTINUE BUTTON CALLBACK
void pasEmptyCbContinueBtn (void* data)
{
   // Actually call the screen's Continue method
   ((Screen_PAS_EMPTY*)data)->processContinueBtn();
}


////////////////////////////////////////////////////////////////////////////////
// CANCEL BUTTON CALLBACK
void pasEmptyCbCancelBtn (void* data)
{
   // Actually call the screen's Cancel (End Addition) method
   ((Screen_PAS_EMPTY*)data)->processCancelBtn ();
}

/* FORMAT HASH a3653df821d5e3bcd471d119a74897bc */
