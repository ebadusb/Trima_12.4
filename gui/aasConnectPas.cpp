/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasConnectPas.cpp
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "aasConnectPas.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "math.h"
#include "procvolumes_cds.h"
#include "pasBarcodeConfirmBox.hpp"

// FOR OVERRIDE BUTTONS
#include "screen_config.hpp"

#if (CPU==SIMNT) // Include simulator unit testing messages
#include "guiUnitTestMessageDefs.h"
#endif

#define BAG_VOL_BUTT_X 526
#define BAG_VOL_BUTT_Y 238

#define CONNNECT_PAS_REMINDER_DELAY 600000 // 600.0 seconds - 10 minutes

void handle_plt_bagvol (void* data);


////////////////////////////////////////////////////////////////////////////////

Screen_AASCONNECTPAS::Screen_AASCONNECTPAS()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_CONNECT_PAS),
     _bmpConnectPas(15, 155, BITMAP_MSS_CONNECT_PLT),
     _btnPasBagVol (GUI_BUTTON_MSS_BAG_VOL_PLAT,
                    BAG_VOL_BUTT_X,
                    BAG_VOL_BUTT_Y,
                    textEmpty,
                    BITMAP_BTN_NARROW_RED_UP,
                    BITMAP_BTN_NARROW_RED_DOWN),
     _txtFirstPanelConnect(textMiscConnectPas),
     _txtSecondPanelFrang(textMiscBreakPasLineBreakFrang),
     _txtThirdPanelUnclamp(textMiscBreakPasLineFrang),
     _txtBagVolLbl(textButtonOverrideDefaultPasBagVol),
     _txtNumBagsLbl(textMiscPasNumBags),
     _txtConnectPasOnly(textMiscConnectPasOnly),
     _minText(textMiscPasConnectMinBagVol),
     _maxText(textMiscPasConnectMaxBagVol),
     _timerReminder(CONNNECT_PAS_REMINDER_DELAY,
                    Callback<Screen_AASCONNECTPAS>(this, &Screen_AASCONNECTPAS::reminder_timer_handler),
                    TimerMessage::DISARMED),
     _msgKeypadReturn(),
     _connectionConfirmed(),
     _connectionCanceled(),
     _msgCancelMssAll(),
     _msgCancelMssPartial(),
     _isPasBarcodeFeatureOn(false),
     _isPasBarcodePending(false)
{}


////////////////////////////////////////////////////////////////////////////////
Screen_AASCONNECTPAS::~Screen_AASCONNECTPAS()
{
   Screen_AASCONNECTPAS::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASCONNECTPAS::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();
   btn_cancel.set_string_id(textButtonEndAddition);

   // Now link the status bar/line to this window
   link_status_bar_line();

   // allocate the text holders  (NULL right now)
   _txtFirstPanelConnect.allocate_resources(*this);
   _txtSecondPanelFrang.allocate_resources(*this);
   _txtThirdPanelUnclamp.allocate_resources(*this);
   _txtBagVolLbl.allocate_resources(*this);
   _txtNumBagsLbl.allocate_resources(*this);
   _txtConnectPasOnly.allocate_resources(*this);

   _bmpConnectPas.allocate_resources(*this);
   _btnPasBagVol.allocate_resources(*this);

   // Set up callbacks
   _msgKeypadReturn.init(Callback<Screen_AASCONNECTPAS>(this, &Screen_AASCONNECTPAS::update_screen_with_keypad_data), MessageBase::SNDRCV_RECEIVE_ONLY);

   choosePasBagVolOverride((bool)guiglobs::cdsData.config.Procedure.Get().key_override_pas_bag_vol);

   ProcVolumes_CDS cds_volumes(ROLE_RO);
   setNumBags(cds_volumes.VpltBagCapacity.Get());

   // Start the reminder timer
   _timerReminder.armTimer(TimerMessage::ARMED);

   // Check for the PAS Bag Barcode Scan feature
   _isPasBarcodeFeatureOn = Software_CDS::GetInstance().getFeature(ConfirmPasBagBarcodeScan);
   if (_isPasBarcodeFeatureOn)
   {
      _isPasBarcodePending = true;
      _msgCancelMssAll.init(Callback<Screen_AASCONNECTPAS>(this,  &Screen_AASCONNECTPAS::hdlCancelMssAll));
      _msgCancelMssPartial.init(Callback<Screen_AASCONNECTPAS>(this,  &Screen_AASCONNECTPAS::hdlCancelMssPartial));
      DataLog(log_level_gui_info) << "ConfirmPasBagBarcodeScan is On: barcodePending=" << _isPasBarcodePending << endmsg;
   }
}

void Screen_AASCONNECTPAS::choosePasBagVolOverride (bool overrideEnabled)
{
   if (overrideEnabled)
   {
      // OVERRIDE OPTION IS TURNED ON
      _txtBagVolLbl.set_color(YELLOW);
      _btnPasBagVol.set_callback (handle_plt_bagvol, (void*)this);
      _btnPasBagVol.set_disabled(false);
      _btnPasBagVol.set_button_text_color(YELLOW);
      _btnPasBagVol.set_up_bitmap(BITMAP_BTN_NARROW_RED_UP);
   }
   else
   {
      // OVERRIDE OPTION IS TURNED OFF
      _txtBagVolLbl.set_color(GREY_LIGHT);
      _btnPasBagVol.set_disabled(true);
      _btnPasBagVol.set_button_text_color(GREY_LIGHT);
      _btnPasBagVol.set_up_bitmap(BITMAP_CONFIG_SUB_NARROW_GRAY);
   }
}

void Screen_AASCONNECTPAS::createConfirmCallbacks ()
{
   if (_isPasBarcodeFeatureOn)
   {
      _connectionConfirmed.init(Callback<Screen_AASCONNECTPAS>(this, &Screen_AASCONNECTPAS::barcodeConfirmed));
      _connectionCanceled.init(Callback<Screen_AASCONNECTPAS>(this,  &Screen_AASCONNECTPAS::barcodeCanceledOrInvalid));
   }
   else
   {
      _connectionConfirmed.init(Callback<Screen_AASCONNECTPAS>(this, &Screen_AASCONNECTPAS::confirmCB));
      _connectionCanceled.init(Callback<Screen_AASCONNECTPAS>(this,  &Screen_AASCONNECTPAS::cancelCB));
   }
}

void Screen_AASCONNECTPAS::destroyConfirmCallbacks ()
{
   _connectionConfirmed.deregisterMsg();
   _connectionCanceled.deregisterMsg();
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASCONNECTPAS::deallocate_resources ()
{
   // Shut off the reminder timer
   _timerReminder.armTimer(TimerMessage::DISARMED);

   // Stop the audible alert
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_CLEAR);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   destroyConfirmCallbacks();
   _msgCancelMssAll.deregisterMsg();
   _msgCancelMssPartial.deregisterMsg();

   // clean up the text lines
   _txtFirstPanelConnect.deallocate_resources();
   _txtSecondPanelFrang.deallocate_resources();
   _txtThirdPanelUnclamp.deallocate_resources();
   _txtBagVolLbl.deallocate_resources();
   _txtNumBagsLbl.deallocate_resources();
   _txtConnectPasOnly.deallocate_resources();
   _btnPasBagVol.deallocate_resources();
   _bmpConnectPas.deallocate_resources();

   _minText.deallocate_resources();
   _maxText.deallocate_resources();
   _msgKeypadReturn.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCONNECTPAS::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCONNECTPAS::process_continue_button ()
{
   if (_isPasBarcodeFeatureOn)
   {
      gotoNextScreen();
   }
   else
   {
      callConfirmBox();
   }
}


void Screen_AASCONNECTPAS::callConfirmBox ()
{
   DataLog (log_level_gui_info) << "Putting up an PAS confirmation message." << endmsg;

   btn_continue.deallocate_resources();
   createConfirmCallbacks();

   goto_child_screen(GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX, "PAS CONNECT CONFIRMATION BOX");
}

void Screen_AASCONNECTPAS::confirmCB ()
{
   DataLog (log_level_gui_info) << "PAS Confirm Callback." << endmsg;
   destroyConfirmCallbacks();
   gotoNextScreen();
}

void Screen_AASCONNECTPAS::cancelCB ()
{
   DataLog (log_level_gui_info) << "PAS Cancel Callback." << endmsg;
   destroyConfirmCallbacks();
   btn_continue.allocate_resources(*this);
}


void Screen_AASCONNECTPAS::callConfirmBarcodeBox ()
{
   DataLog(log_level_gui_info) << "Putting up PAS BARCODE confirmation box." << endmsg;

   btn_continue.deallocate_resources();
   createConfirmCallbacks();

   goto_stateless_screen(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX, "PAS BARCODE CONFIRMATION BOX");
}

void Screen_AASCONNECTPAS::callInvalidBarcodeBox ()
{
   DataLog(log_level_gui_info) << "Putting up INVALID PAS BARCODE box." << endmsg;

   goto_stateless_screen(GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX,
                         Screen_PAS_BARCODE_CONFIRM_BOX::allocParamForBadBarcode());
}

void Screen_AASCONNECTPAS::barcodeConfirmed ()
{
   DataLog (log_level_gui_info) << "PAS Barcode Confirmed Callback." << endmsg;
   destroyConfirmCallbacks();
   btn_continue.allocate_resources(*this);
   _isPasBarcodePending = false;
}

void Screen_AASCONNECTPAS::barcodeCanceledOrInvalid ()
{
   // The canceled message value indicates either:
   // 0 : the operator selected End Addition, or
   // 1 : an invalid barcode was scanned
   //
   if ( _connectionCanceled.getData() == 0 )
   {
      DataLog(log_level_gui_info) << "PAS Barcode was Canceled." << endmsg;
      destroyConfirmCallbacks();
      process_cancel_button();  // End the addition
   }
   else
   {
      DataLog(log_level_gui_info) << "PAS Barcode was Invalid." << endmsg;
      callInvalidBarcodeBox();  // Prompt the operator again
   }
}

void Screen_AASCONNECTPAS::hdlCancelMssAll ()
{
   _isPasBarcodePending = false;
}

void Screen_AASCONNECTPAS::hdlCancelMssPartial ()
{
   if (END_PAS_ADDITION == _msgCancelMssPartial.getData() )
   {
      _isPasBarcodePending = false;
   }
}

void Screen_AASCONNECTPAS::gotoNextScreen ()
{
   DataLog (log_level_gui_mss_info) << "PAS connect complete." << endmsg;
   MSSConnectedMsg _connectionsCompleted(MessageBase::SEND_LOCAL);
   _connectionsCompleted.send(0);

   DataLog (log_level_gui_mss_info) << "Transition to AAS prime screen." << endmsg;
   goto_screen(GUI_SCREEN_AAS_PRIME, "GUI_SCREEN_AAS_PRIME");
}

///////////////////////////////////////////////////////////////////////////////
// Stuff for dealing with keypad data.
///////////////////////////////////////////////////////////////////////////////

void handle_plt_bagvol (void* data)
{
   DataLog(log_level_gui_info) << "Override default bag volume pressed." << endmsg;
   float     min, max;
   guistring keypad_string;
   keypad_string.initialize();

   min = guiglobs::config_var_list.Var(KEY_PLT_DEF_BAG_VOL).Min();
   max = guiglobs::config_var_list.Var(KEY_PLT_DEF_BAG_VOL).Max();

   ((Screen_AASCONNECTPAS*)data)->displayKeypadLimits(min, max, "%.0f");

   Screen_Config::build_keypad_string (keypad_string,
                                       KEY_PLT_DEF_BAG_VOL,     // unused
                                       410,
                                       130,
                                       "####",
                                       max,
                                       min);

   guiglobs::apheresis_status_line->unparent_status_line (guiglobs::apheresis_status_line->get_owning_screen());

   Base_Apheresis_Window::goto_child_screen(GUI_SCREEN_KEYPAD, keypad_string);
}

void Screen_AASCONNECTPAS::displayKeypadLimits (const float min, const float max, const char* format)
{
   guistring button_string;
   guistring min_string;
   guistring max_string;

   _minText.set_string_id(textMiscPasConnectMinBagVol);
   _minText.allocate_resources(*this);
   _minText.get_text_direct(min_string);

   _maxText.set_string_id(textMiscPasConnectMaxBagVol);
   _maxText.allocate_resources(*this);
   _maxText.get_text_direct(max_string);

   sprintf_l10n(button_string, format, min);
   strcat(min_string, button_string);
   _minText.set_text_direct(strlen(min_string), min_string);

   sprintf_l10n(button_string, format, max);
   strcat(max_string, button_string);
   _maxText.set_text_direct(strlen(max_string), max_string);
}

void Screen_AASCONNECTPAS::turnOffKeypadLimits ()
{
   _minText.deallocate_resources();
   _maxText.deallocate_resources();
}

void Screen_AASCONNECTPAS::update_screen_with_keypad_data ()
{
   const KEYPAD_RETURN_STRUCT& keypad_buffer = _msgKeypadReturn.getData();
   float bagVolMsg = keypad_buffer.keypad_return_value;
   turnOffKeypadLimits();

   if (bagVolMsg != -1)
   {
      DataLog(log_level_gui_info) << "Received keypad data: " << bagVolMsg << endmsg;

      MSSWriteBagVolPLT writePltMssVol(MessageBase::SEND_LOCAL);
      writePltMssVol.send((int)bagVolMsg);

      setNumBags(bagVolMsg);
   }
   else
   {
      DataLog(log_level_gui_error) << "Keypad returned -1, meaning that no value was returned." << endmsg;
   }
}


////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Screen_AASCONNECTPAS::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();

   // Open the confirmation box to prompt for a PAS bag barcode scan?
   //
   // This refresh will get called after allocate_resources() and also
   // whenever operator has selected a button from the End Addition screen.
   //
   if ( _isPasBarcodeFeatureOn &&
        _isPasBarcodePending &&
        (GUI_SCREEN_PAS_BARCODE_CONFIRM_BOX != guiglobs::screen_status.get_current_displayed_screen()) )
   {
      callConfirmBarcodeBox();
   }
}

// This is the function that actually updates both the number of bags
// label and the bag volume button on the PLT connect screen.
void Screen_AASCONNECTPAS::setNumBags (float defBagVol)
{
   float adjustedBagVol = defBagVol * guiglobs::rasBagEmptyPercentage;
   float mssNeeded      = guiglobs::aasRunInfo.getPasTotalVolNeeded();

   char  defBagVolString[10];
   int   defBagVolStringSize = sprintf(defBagVolString, "%.0f", defBagVol);
   _btnPasBagVol.set_text_direct(defBagVolStringSize, defBagVolString);

   int numBags = 0;
   if (adjustedBagVol != 0) numBags = (int)ceil(mssNeeded / adjustedBagVol);
   if (numBags < 1) numBags = 1;  // IT 7940 - If we're here at all, we need at least one bag.  It's common sense, people.

   if (!guiglobs::setIntDataItem("dataItemNumPasBagsNeeded", numBags))
      _FATAL_ERROR(__FILE__, __LINE__, "Unable to access num pas bags variable");

    #if (CPU==SIMNT)
   _btnPasBagVol.set_text_direct(5, "250");
    #endif
}

void Screen_AASCONNECTPAS::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatConnectPas);
}

#if (CPU==SIMNT)
void Screen_AASCONNECTPAS::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      PAS_CONNECT_DISPLAY_TYPES pasConnectDisplayType = (PAS_CONNECT_DISPLAY_TYPES)traverseInfo.parameters[0];

      DataLog (log_level_sim_gui_side_info) << "Connect PAS traversal message recieved (displayType=" << pasConnectDisplayType << ")" << endmsg;

      switch (pasConnectDisplayType)
      {
         case PAS_CONNECT_TYPE_OVERRIDE_ON : choosePasBagVolOverride(true); break;
         case PAS_CONNECT_TYPE_OVERRIDE_OFF : choosePasBagVolOverride(false); break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown PAS Connect Display Type: " << pasConnectDisplayType << endmsg;
            return;
      }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

////////////////////////////////////////////////////////////////////////////////
// REMINDER_TIMER_HANDLER

void Screen_AASCONNECTPAS::reminder_timer_handler ()

{
   DataLog(log_level_gui_info) << "Sounding the PAS connection reminder. " << endmsg;

   // Sound the reminder
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_ONCE);

   // Start the timer again
   _timerReminder.armTimer(TimerMessage::ARMED);

}   // END of REMINDER_TIMER_HANDLER

/* FORMAT HASH 63bd9defb05b1e6e39dd2775e88230c3 */
