/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  OipAlarmBase

*     File name:   oipAlarmBase.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************


*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "oipAlarmBase.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

void oipCbMuteSound (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

OipAlarmBase::OipAlarmBase(WINDOW_COLOR wcolor, GUI_SCREEN_ID win_ID)
   : Base_Apheresis_Window (wcolor, win_ID),
     _alrmState            (CLEARED),
     _msgMuteTimeout       (MUTE_PERIOD, Callback<OipAlarmBase> (
                               this, &OipAlarmBase::startBeepingSound), TimerMessage::DISARMED),
     _alarmId              (NULL_ALARM),
     _isBeepingAlarm       (false)

{
   _soundCode = ALERT_SOUND;
   _soundMode = SOUND_CONTINUOUS;

}   // END of OipAlarmBase CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
OipAlarmBase::~OipAlarmBase()
{
   if (_allocated)
   {
      // If we haven't already deallocated, do it now.
      OipAlarmBase::deallocate_resources ();
   }
}   // END of OipAlarmBase DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void OipAlarmBase::allocate_resources (const char* allocation_parameter)
{
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   setUpOipInfo(allocation_parameter);

   // Allocate a scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChange.init(Callback<OipAlarmBase>(this, &OipAlarmBase::alrmStateChange), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Now link the status bar/line to this window
   link_status_bar_line();

   guiglobs::current_displayed_alarm = _alarmId;

   // send the alarm confirm message after allocation is complete
   _alrmResponseStruct.alarm_name     = _alarmId;
   _alrmResponseStruct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_continue.deallocate_resources ();

   if (_isBeepingAlarm)
   {
      // set the bell/ bitmap on the mute button and add callback
      btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
      btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
      btn_lowleft.set_callback (oipCbMuteSound, (void*)this);

      // start the alarm sound
      startBeepingSound();

   }
   else
   {
      btn_lowleft.deallocate_resources();
   }
}  // END of OipAlarmBase ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void OipAlarmBase::deallocate_resources ()
{
   // Mute
   muteBeepingSound();

   // Deallocate the state change message pointer
   _msgMuteTimeout.armTimer(TimerMessage::DISARMED);
   _msgAlarmStateChange.deregisterMsg();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   guiglobs::current_displayed_alarm = NULL_ALARM;

   // send the alarm confirm message after deallocation is complete
   _alrmResponseStruct.alarm_name     = NULL_ALARM;
   _alrmResponseStruct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();
}   // END of OipAlarmBase DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void OipAlarmBase::alrmStateChange ()
{
   long message_data = _msgAlarmStateChange.getData();

   if ((message_data == (long)LATCHED))
   {
      // set alarm state
      _alrmState = LATCHED;

   }
   else if ((message_data == (long)ACTIVE))
   {
      // set alarm state
      _alrmState = ACTIVE;

   }  // End of else if (message..

   // Confirm to Proc that we got the alarm message.
   _alrmResponseStruct.alarm_name     = (ALARM_VALUES)_alarmId;
   _alrmResponseStruct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();
}   // END of OipAlarmBase SCREEN_NOTIFY_HANDLER



////////////////////////////////////////////////////////////////////////////////
// ALARM_SET_STRUCT_MANAGER
void OipAlarmBase::alarm_set_struct_manager ()
{
   // struct message declare for outgoing donor vitals information
   Message<Alarm_response_struct> Msg_Alarm_Set_ptr(MessageBase::SEND_GLOBAL);

   // Actually send the message
   Msg_Alarm_Set_ptr.send(_alrmResponseStruct);

}   // End of ALARM_SET_STRUCT_MANAGER




////////////////////////////////////////////////////////////////////////////////
// PROCESS_RF_CONTINUE_BUTTON
void OipAlarmBase::continueOip ()
{
   // set and send alarm response
   _alrmResponseStruct.alarm_name     = _alarmId;
   _alrmResponseStruct.alarm_response = CONTINUE;
   alarm_set_struct_manager ();

   // Reset the statusline
   guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // Remove all alarm screens and return to either the adjust or the run screen.
   RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
   removeall.send(0);
}   // END of OipAlarmBase PROCESS_RF_CONTINUE_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void OipAlarmBase::cancelOip ()
{
   char alarm_allocation_string[MAXSIZE_BWINDOW_PARAM];  // newly constructed alarm data buffer
   char alarm_enumeration_buffer[20] = "";

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", _alarmId);

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

}    // END of OipAlarmBase PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void OipAlarmBase::rinsebackOip ()
{
   char      alarm_enumeration_buffer[20] = "";
   guistring alarm_allocation_string;  // newly constructed alarm data buffer

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", _alarmId);

   // create the string that will be parsed by the ABORTCNF screen using
   // components recieved and then created via the incoming alarm structure
   strncpy(alarm_allocation_string, ABORT_TYPE_ALARMRINSE, 5);
   strncat(alarm_allocation_string, "~", 1);
   strncat(alarm_allocation_string, alarm_enumeration_buffer, MAXSIZE_BWINDOW_PARAM - 7);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << (int)ABORT_THRU_RINSEBACK << " " << alarm_allocation_string << endmsg;

   // goto ABORTCNF to give user opportunity to confirm choice
   goto_stateless_screen (GUI_SCREEN_ABORTCNF, alarm_allocation_string);
}    // END of OipAlarmBase PROCESS_CANCEL_BUTTON


void OipAlarmBase::startBeepingSound ()
{
   DataLog (log_level_gui_info) << "***###***### Beeping Sound Started ###***###***"
                                << " Sound Code " <<  SoundCodeStrings[_soundCode]
                                << " Sound Mode " << _soundMode << endmsg;
   // call sound method to stop alarm "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (_soundCode, _soundMode);

   // Change the bitmap to the one with sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
}   // END of Screen_ALARM SCREEN_TIMER_HANDLER


// PROCESS_MUTE_BUTTON
void OipAlarmBase::muteBeepingSound ()
{
   guiglobs::button_audio_feedback.create_feedback_sound (_soundCode,
                                                          SOUND_CLEAR);
   // Destroy and recreate the timer that will drive the restart the alarm
   // sounds.
   _msgMuteTimeout.armTimer(TimerMessage::ARMED);

   // Change the bitmap to one with no sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_DISABLE_SOUND);


}   // END of Screen_ALARM PROCESS_MUTE_BUTTON


void oipCbMuteSound (void* data)
{
   OipAlarmBase* oipAlarmBase = (OipAlarmBase*)data;

   // call sound method to stop alert "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALARM_SOUND,
                                                          SOUND_CLEAR);

   // call sound method to stop alert "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND,
                                                          SOUND_CLEAR);


   // Actually call the process button routine for the system setup button
   ((OipAlarmBase*)data) -> muteBeepingSound ();

}   // END of CALLBACK_MUTE_SOUND

/* FORMAT HASH bacd24b47501c33b369a40473b5af3a3 */
