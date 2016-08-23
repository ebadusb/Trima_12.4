/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Alarm_Handler

*     File name:   alarmhdl.cpp

*     Contents:    Imlementation of class methods

*     Description: Class responsible for receiving alarm indications and
                   dispanding the dynamically allocated alarm screen

$Header$

***************************************************************************/

// header file include
#include <vxWorks.h>

#include "alarmhdl.hpp"

// GUI global element include
#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Alarm_Handler::Alarm_Handler()
{}  // END of Alarm_Handler CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Alarm_Handler::~Alarm_Handler()
{}  // END of Alarm_Handler DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// START_ALARM_MANAGER
void Alarm_Handler::start_alarm_manager ()
{
   // Create the message handler for the alarm message
   Msg_Alarm_Get.init(Callback<Alarm_Handler>(this, &Alarm_Handler::enable_alarm_screen), MessageBase::SNDRCV_RECEIVE_ONLY);
}  // END of Alarm_Handler START_ALARM_MANAGER



////////////////////////////////////////////////////////////////////////////////
// STATELESS_SCREEN_MANAGER
void Alarm_Handler::stateless_screen_manager (invocation_method invocation,
                                              GUI_SCREEN_ID new_screen,
                                              const char* screen_param)
{
   SCREEN_INVOKE_STRUCT          stateless_screen_struct;
   Message<SCREEN_INVOKE_STRUCT> Msg_Stateless_Screen_Set(MessageBase::SEND_LOCAL);

   // Set up parameters to display the stateless screen
   stateless_screen_struct.screen_invocation = invocation;
   stateless_screen_struct.screen_id         = new_screen;

   // Copy in the parameter string (ensure no overrun and cauterize the end)
   stateless_screen_struct.alloc_param.initialize();

   if (strlen(screen_param) > 0)
   {
      strncpy (stateless_screen_struct.alloc_param,
               screen_param,
               MAXSIZE_BWINDOW_PARAM - 1);
   }

   // Actually send the message
   Msg_Stateless_Screen_Set.send(stateless_screen_struct);

}  // END of Alarm_Handler STATELESS_SCREEN_MANAGER



////////////////////////////////////////////////////////////////////////////////
// ENABLE_ALARM_SCREEN
void Alarm_Handler::enable_alarm_screen ()

{
   ALARM_VALUES     alarm_name;              // store incoming alarm enumeration
   ALARM_TYPES      alarm_type;              // store incoming alarm type
   ALARM_STATES     alarm_state;

   GUI_Alarm_struct alarm_msg_buffer = Msg_Alarm_Get.getData();

   // reset allocation buffer
   allocation_param_buffer.initialize();

   char*         OIP_parameter_string_ptr = 0;
   GUI_SCREEN_ID OIP_screen_ID            = GUI_SCREEN_NULL_SCREEN;

   // extract alarm name and type for recasting
   alarm_name  = alarm_msg_buffer.alarm_name;
   alarm_type  = alarm_msg_buffer.alarm_type;
   alarm_state = alarm_msg_buffer.alarm_state;

   // only send the state change if we are already displayed
   if ( alarm_name == guiglobs::current_displayed_alarm )
   {
      AlarmStateChangeMsg stateChangeMsg(MessageBase::SEND_LOCAL);
      stateChangeMsg.send(alarm_state);
      return;
   }

   if (alarm_name == NULL_ALARM)
   {
      DataLog(log_level_gui_error) << "NULL alarm received.  Probable bad GUI/Alarm Manager handshake" << endmsg;
      _FATAL_ERROR (__FILE__, __LINE__, "bad gui/alarm manager handshake");
   }
   else if ( (alarm_name > NULL_ALARM) || (alarm_name < 0) )
   {
      DataLog(log_level_gui_error) << "Bad Alarm ID " << (int)alarm_name << endmsg;
      _FATAL_ERROR (__FILE__, __LINE__, "alarm table overrun");
   }

   const ALARM_TABLE* alarmItem = _alarmTable.GetEntry(alarm_name);

   if ( alarmItem == 0 )
   {
      // If we didn't find the alarm, then log the failure as a Safety T1
      alarmItem = _alarmTable.GetEntry(SAFETY_T1_FAILURE);

      if ( alarmItem == 0 )
      {
         DataLog(log_level_gui_error) << "Bad Alarm ID " << (int)alarm_name << endmsg;
         _FATAL_ERROR (__FILE__, __LINE__, "Alarm not found in GUI alarm table");
      }
      else
      {
         OIP_parameter_string_ptr = alarmItem->goto_screen_literal;
         OIP_screen_ID            = alarmItem->goto_screen_gui;
      }
   }
   else
   {
      OIP_parameter_string_ptr = alarmItem->goto_screen_literal;
      OIP_screen_ID            = alarmItem->goto_screen_gui;
   }

   // If the table defines an alternate screen ID, then this is an OIP alarm.
   if ( OIP_screen_ID > GUI_SCREEN_NULL_SCREEN &&
        OIP_screen_ID < GUI_SCREEN_TOTAL_SCREENS &&
        OIP_screen_ID != GUI_SCREEN_ALARM )
   {
      // Call the unique screen for this type of alarm (a non-standard alarm)
      stateless_screen_manager (SWAP_STATELESS_WITH_SCREEN, OIP_screen_ID, OIP_parameter_string_ptr);
   }
   else
   {
      sprintf(allocation_param_buffer, "%d~%d~%d~", alarm_name, alarm_type, alarm_state);

      // goto the alarm screen with the appropriate data string
      guiglobs::stack_screens_to_process++;
      stateless_screen_manager (SWAP_STATELESS_WITH_SCREEN, GUI_SCREEN_ALARM, allocation_param_buffer);
   }
}   // END of Alarm_Handler ENABLE_ALARM_SCREEN



////////////////////////////////////////////////////////////////////////////////
// CLEAR_ALARM_SCREEN
void Alarm_Handler::clear_alarm_screen ()
{}   // END of Alarm_Handler CLEAR_ALARM_SCREEN

/* FORMAT HASH ddd77cc2807c795d960956a112408944 */
