/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Alarm_Handler

*     File name:   alarmhdl.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for alarmhdl.cpp

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$
*
*/


// Single include define guardian
#ifndef ALARMHDL_HPP
#define ALARMHDL_HPP

// system #includes
#include <string.h>

// message include
#include "message.h"

#include "guistring.h"
#include "iguimsgs.h"

// alarm include
#include "alarms.h"
#include "alarmtb.h"


class Alarm_Handler
{
private:

   guistring alarm_name_buffer;       // buffer for alarm enum conversion
   guistring alarm_type_buffer;       // buffer for alarm type conversion
   guistring alarm_state_buffer;      // buffer for alarm state conversion
   guistring allocation_param_buffer; // buffer for input string manipulation


   // STATELESS_SCREEN_MANAGER
   // This method will send a message to the commutator which in turn will
   // manage the alarm stateless screens.
   void stateless_screen_manager (invocation_method, GUI_SCREEN_ID, const char*);

public:
   // CONSTRUCTOR
   Alarm_Handler() ;

   // DESTRUCTOR
   virtual ~Alarm_Handler() ;

   // START_ALARM_MANAGER
   // This method will allocate the initial router (receive) messages that make the
   // commutator active to requests.
   void start_alarm_manager ();

   // ENABLE_ALARM_SCREEN
   // This method will allocate the alarm screen.
   void enable_alarm_screen ();

   // CLEAR_ALARM_SCREEN
   // This method will allocate the alarm screen.
   void clear_alarm_screen ();

protected:
   Message<GUI_Alarm_struct> Msg_Alarm_Get;
   GuiAlarmTable             _alarmTable;

}; // END of Alarm_Handler Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH dbd48bb118cd3c1642eb711ce0735c40 */
