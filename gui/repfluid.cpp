/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_REPFLUID

*     File name:   repfluid.cpp

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "repfluid.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "predict_manager.hpp"

static void repfluid_cb_cancel   (void* data);
static void repfluid_cb_continue (void* data);
static void cbMuteSound          (void* data);



////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_REPFLUID::Screen_REPFLUID()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_REPFLUID),

     _bmpConnectGraphic (311, 131, BITMAP_PLACARD_REPLACE_FLUID_CONNECT),
     _bmpBackground (10, 115, BITMAP_CARTOUCHE_REPLACE_FLUID),

     _txtBodyNormal   (textAlarmBodyWithGraphicSaline),
     _txtBodyBolus    (textAlarmBodyWithGraphicSalineBolus),
     _txtBodyNewBag   (textAlarmBodyWithGraphicSalineEmpty),
     _txtBodyRinseback(textAlarmBodyWithGraphicSalineRinsebackConnect),
     _txtConnectOnlySaline    (textMiscConnectSalineOnly),
     _txtNaclBagLabel (textMiscSalineConnBagLabel),

     _btnRfContinue   (GUI_BUTTON_REPFLUID_CONTINUE,
                       486, 115,
                       textButtonContinueSalineConnect,
                       BITMAP_BUTTON_CONFIRM_REPLACE_FLUID_UP,
                       BITMAP_BUTTON_CONFIRM_REPLACE_FLUID_DN),

     _btnRfCancel     (GUI_BUTTON_REPFLUID_CANCEL,
                       486, 407,
                       textButtonCancelSalineConnect,
                       BITMAP_BUTTON_CANCEL_REPLACE_FLUID_UP,
                       BITMAP_BUTTON_CANCEL_REPLACE_FLUID_DN),

     _rfInvocation(INVOCATION_NORMAL),

     alarm_state   (0),
     current_alarm (NULL_ALARM),

     _muteTimeout(MUTE_PERIOD, Callback<Screen_REPFLUID>(this, &Screen_REPFLUID::startBeepingSound), TimerMessage::DISARMED)

{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_REPFLUID::~Screen_REPFLUID()
{
   if (_allocated)
   {
      // If we haven't already deallocated, do it now.
      // deallocate_resources is virtual, so be sure to call the right one!
      Screen_REPFLUID::deallocate_resources ();
   }
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_REPFLUID::allocate_resources (const char* allocation_parameter)
{
   // Set the invocation mode of the screen: normal or bolus
   if (strcmp(allocation_parameter, "bolus") == 0)
   {
      _rfInvocation = INVOCATION_BOLUS;
      current_alarm = REPLACEMENT_FLUID_BOLUS_ALARM;
   }
   else if (strcmp(allocation_parameter, "newbag") == 0)
   {
      _rfInvocation = INVOCATION_NEW_BAG;
      current_alarm = REPLACEMENT_FLUID_NEW_BAG_ALARM;
   }
   else if (strcmp(allocation_parameter, "rinseback") == 0)
   {
      _rfInvocation = INVOCATION_RINSEBACK;
      current_alarm = REPLACEMENT_FLUID_ALARM_RINSEBACK;
   }
   else
   {
      _rfInvocation = INVOCATION_NORMAL;
      current_alarm = REPLACEMENT_FLUID_ALARM;
   }

   guiglobs::current_displayed_alarm = current_alarm;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // Allocate the bitmaps
   _bmpBackground.allocate_resources(*this);
   _bmpConnectGraphic.allocate_resources(*this);


   // Allocate (conditionally) the text prompt
   switch (_rfInvocation)
   {
      case INVOCATION_NORMAL :
         _txtBodyNormal.allocate_resources(*this);
         _txtBodyBolus.deallocate_resources();
         _txtBodyNewBag.deallocate_resources();
         _txtBodyRinseback.deallocate_resources();
         break;

      case INVOCATION_BOLUS :
         _txtBodyBolus.allocate_resources(*this);
         _txtBodyNormal.deallocate_resources();
         _txtBodyNewBag.deallocate_resources();
         _txtBodyRinseback.deallocate_resources();
         break;

      case INVOCATION_NEW_BAG :
         _txtBodyNewBag.allocate_resources(*this);
         _txtBodyNormal.deallocate_resources();
         _txtBodyBolus.deallocate_resources();
         _txtBodyRinseback.deallocate_resources();
         break;

      case INVOCATION_RINSEBACK :
         _txtBodyRinseback.allocate_resources(*this);
         _txtBodyNewBag.deallocate_resources();
         _txtBodyNormal.deallocate_resources();
         _txtBodyBolus.deallocate_resources();
         break;

      default :
         DataLog (log_level_gui_error) << "Unknown replacement fluid display type " << _rfInvocation << endmsg;
         anAlarmMsg unexpectedChoiceAlarm(GUI_UNEXPECTED_CHOICE);
         unexpectedChoiceAlarm.setAlarm();
         break;
   }

   _txtConnectOnlySaline.allocate_resources(*this);
   _txtNaclBagLabel.allocate_resources(*this);

   if (guiglobs::cdsData.config.LangUnit.Get().key_decimal_delimiter == DECIMAL_DELIMIT)
   {
      guistring delimString;
      _txtNaclBagLabel.get_text_direct(delimString);
      delimString.swapOutDelimiter();
      _txtNaclBagLabel.set_text_direct(delimString.length(), delimString);
   }

   // Allocate and set callback for continue button.
   _btnRfContinue.set_callback (repfluid_cb_continue, (void*)this);
   _btnRfContinue.allocate_resources(*this);

   // Allocate and set callback for cancel button
   // (unless this is rinseback, in which case cancel doesn't make sense).
   if (guiglobs::screen_status.get_current_system_state() != BLOOD_RINSEBACK)
   {
      _btnRfCancel.set_callback   (repfluid_cb_cancel, (void*)this);
      _btnRfCancel.allocate_resources(*this);
   }

   // Allocate a scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChange.init(Callback<Screen_REPFLUID>(this, &Screen_REPFLUID::alarm_state_change), MessageBase::SNDRCV_RECEIVE_ONLY);

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // send the alarm confirm message after allocation is complete
   alarm_set_struct.alarm_name     = current_alarm;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

   // Don't beep for the initial connection.
   if (guiglobs::screen_status.get_current_system_state() >= BLOOD_PRIME)
   {
      btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
      btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
      btn_lowleft.set_callback (cbMuteSound, (void*)this);

      btn_lowleft.allocate_resources(*this);

      // start the alarm sound
      startBeepingSound();
   }
}  // END of Screen_REPFLUID ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_REPFLUID::deallocate_resources ()
{
   // We may have allocated a child screen confirm box here.  Make sure that's gone.
   RemoveAllChildScreensMsg removeChildScreenMsg(MessageBase::SEND_LOCAL);
   removeChildScreenMsg.send();

   _txtBodyNormal.deallocate_resources();
   _txtBodyBolus.deallocate_resources();
   _txtBodyNewBag.deallocate_resources();
   _txtBodyRinseback.deallocate_resources();
   _txtConnectOnlySaline.deallocate_resources();
   _txtNaclBagLabel.deallocate_resources();

   // Deallocate the bitmaps
   _bmpBackground.deallocate_resources();
   _bmpConnectGraphic.deallocate_resources();


   // Deallocate both buttons
   _btnRfContinue.deallocate_resources();
   _btnRfCancel.deallocate_resources();

   // Deallocate the state change message pointer
   _msgAlarmStateChange.deregisterMsg();

   _muteTimeout.armTimer(TimerMessage::DISARMED);

   destroyConfirmCallbacks();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   guiglobs::current_displayed_alarm = NULL_ALARM;

   // send the alarm confirm message after deallocation is complete
   alarm_set_struct.alarm_name     = NULL_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

}   // END of Screen_REPFLUID DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_REPFLUID::alarm_state_change ()
{
   long message_data = _msgAlarmStateChange.getData();

   if ((message_data == (long)LATCHED))
   {
      // set alarm state
      alarm_state = LATCHED;

   }
   else if ((message_data == (long)ACTIVE))
   {
      // set alarm state
      alarm_state = ACTIVE;

   }  // End of else if (message..

   // Confirm to Proc that we got the alarm message.
   alarm_set_struct.alarm_name     = current_alarm;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

}



////////////////////////////////////////////////////////////////////////////////
// ALARM_SET_STRUCT_MANAGER
void Screen_REPFLUID::alarm_set_struct_manager ()
{
   // struct message declare for outgoing donor vitals information
   Message<Alarm_response_struct> Msg_Alarm_Set_ptr(MessageBase::SEND_GLOBAL);

   // Actually send the message
   Msg_Alarm_Set_ptr.send(alarm_set_struct);

}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CANCEL_BUTTON
void Screen_REPFLUID::process_RF_cancel_button ()
{
   guistring allocation_parameter_string;

   // Notify Proc that the operator didn't connect RF
   RepFluidConnectStatusMsg notconnected(MessageBase::SEND_GLOBAL);
   notconnected.send(RF_NOT_CONNECTED);

   // If the invocation occurred when the Adjust screen "bolus" button was pushed...
   if (_rfInvocation == INVOCATION_BOLUS)
   {

      // set and send alarm response
      alarm_set_struct.alarm_name     = current_alarm;
      alarm_set_struct.alarm_response = CONTINUE;
      alarm_set_struct_manager ();

      // clear the alarm initialization variable so we can reallocate the same
      // alarm in succession after it has been dismissed
      guiglobs::gui_alarm_manager.clear_alarm_screen ();

      // Remove all alarm screens and return to either the adjust or the run screen.
      RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
      removeall.send(0);

   }
   // Otherwise this was invoked during setup or a procedure change during run...
   // This is OK for either the first bag, or a new bag.
   else
   {
      // No need to request a reprediction; the predict screen takes care of
      // that now.

      // set invocation allocation parameter
      sprintf(allocation_parameter_string, "%d", (int)REPFLUID_NO_DELTA);

      // call the procedure selection screen, because the op. didn't want to
      //  connect RF for this procedure... let 'em select another procedure.
      goto_stateless_screen (GUI_SCREEN_PREDICT, allocation_parameter_string);
   }

   // Reset the statusline
   guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);


}   // END of Screen_REPFLUID PROCESS_RF_CANCEL_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CONTINUE_BUTTON
void Screen_REPFLUID::process_RF_continue_button ()
{
   callConfirmBox();
}

void Screen_REPFLUID::callConfirmBox ()
{
   createConfirmCallbacks();

   DataLog (log_level_gui_info) << "Putting up an saline confirmation message." << endmsg;
   goto_child_screen(GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX, CHILD_OF_STATELESS);
}

void Screen_REPFLUID::confirmCB ()
{
   DataLog (log_level_gui_info) << "Saline Confirm Callback." << endmsg;
   destroyConfirmCallbacks();
   confirmConnection();
}

void Screen_REPFLUID::cancelCB ()
{
   DataLog (log_level_gui_info) << "Saline Cancel Callback." << endmsg;
   destroyConfirmCallbacks();
}

void Screen_REPFLUID::createConfirmCallbacks ()
{
   _msgConnectionConfirmed.init(Callback<Screen_REPFLUID>(this, &Screen_REPFLUID::confirmCB));
   _msgConnectionCanceled.init(Callback<Screen_REPFLUID>(this, &Screen_REPFLUID::cancelCB));
}

void Screen_REPFLUID::destroyConfirmCallbacks ()
{
   _msgConnectionConfirmed.deregisterMsg();
   _msgConnectionCanceled.deregisterMsg();
}

void Screen_REPFLUID::confirmConnection ()
{

   // Notify Proc that the operator connected RF
   RepFluidConnectStatusMsg connected(MessageBase::SEND_GLOBAL);
   connected.send(RF_CONNECTED);


   // set and send alarm response
   alarm_set_struct.alarm_name     = current_alarm;
   alarm_set_struct.alarm_response = CONTINUE;
   alarm_set_struct_manager ();

   // Reset the statusline
   guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // Remove all alarm screens and return to either the adjust or the run screen.
   RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
   removeall.send(0);


}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Screen_REPFLUID::process_cancel_button ()
{
   char alarm_allocation_string[MAXSIZE_BWINDOW_PARAM];  // newly constructed alarm data buffer
   char alarm_enumeration_buffer[20] = "";

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", current_alarm);

   // create the string that will be parsed by the ABORTCNF screen using
   // components recieved and then created via the incoming alarm structure
   strncpy(alarm_allocation_string, ABORT_TYPE_ALARM, 5);
   alarm_allocation_string[5] = '\0';
   strncat(alarm_allocation_string, "~", 1);
   strncat(alarm_allocation_string, alarm_enumeration_buffer, MAXSIZE_BWINDOW_PARAM - 7);

   // assure buffer maintains its size
   alarm_allocation_string[MAXSIZE_BWINDOW_PARAM - 1] = '\0';

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << "GUI cancel received " << (int)ABORT << " " << alarm_allocation_string << endmsg;

   // goto ABORTCNF to give user opportunity to confirm choice
   goto_stateless_screen (GUI_SCREEN_ABORTCNF, alarm_allocation_string);

}    // END of Screen_REPFLUID PROCESS_CANCEL_BUTTON


void Screen_REPFLUID::startBeepingSound ()
{
   // call sound method to stop alarm "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_CONTINUOUS);

   // Change the bitmap to the one with sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
}


// PROCESS_MUTE_BUTTON
void Screen_REPFLUID::muteBeepingSound ()
{
   // Destroy and recreate the timer that will drive the restart the alarm
   // sounds.
   _muteTimeout.armTimer(TimerMessage::ARMED);

   // Change the bitmap to one with no sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_DISABLE_SOUND);
}


void Screen_REPFLUID::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatSalineConnect);
}


////////////////////////////////////////////////////////////////////////////////
// REPFLUID_CB_CONTINUE
void repfluid_cb_continue (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_REPFLUID*)data) -> process_RF_continue_button ();
}



////////////////////////////////////////////////////////////////////////////////
// REPFLUID_CB_CANCEL
void repfluid_cb_cancel (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_REPFLUID*)data) -> process_RF_cancel_button ();
}

void cbMuteSound (void* data)
{
   // call sound method to stop alert "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALARM_SOUND,
                                                          SOUND_CLEAR);

   // call sound method to stop alert "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND,
                                                          SOUND_CLEAR);

   // Actually call the process button routine for the system setup button
   ((Screen_REPFLUID*)data) -> muteBeepingSound ();

}

/* FORMAT HASH 062e42a5050e93fc55add0941666043e */
