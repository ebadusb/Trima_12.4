/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   OipAlarmBase

*     File name:    oipAlarmBase.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

    This class is intended to represent the base for an OIP alarm.  It's
    not used by all of 'em yet, but it's meant to have all the alarm stuff
    functioning to that it could be, in theory, and derived classes would
    only have to worry about the text and graphics and custom buttons, etc.


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.4  2008/09/15 20:52:20Z  dslausb
DRBC split alarm needs rinseback button
Revision 1.3  2008/08/10 16:42:33Z  ssunusb
IT8044 - Alternate Sound for DRBC Split notification
Revision 1.2  2008/06/11 18:11:37Z  dslausb
IT 8686 - New "pas vol remaining" functionality on the PAS empty screen
Revision 1.1  2007/04/05 22:56:16Z  dslausb
Initial revision
*/



// Single include define guardian
#ifndef __OIP_ALARM_BASE_HPP__
#define __OIP_ALARM_BASE_HPP__



// CLASS INCLUDE


#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "timermessage.h"
#include "sounds.hpp"
// alarm include
#include "alarms.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// proc includes
#include "predict_msgs.h"
#include "trimamessages.h"


// CLASS DECLARES

class OipAlarmBase
   : public Base_Apheresis_Window
{

private:

   // Structure with which to respond to PROC
   Alarm_response_struct _alrmResponseStruct;

   // Current state of the alarm as communicated to GUI by PROC
   int _alrmState;

   // Screen Event Handler pointers
   AlarmStateChangeMsg _msgAlarmStateChange;
   void alrmStateChange ();

   // Screen Timer Handler pointers
   TimerMessage _msgMuteTimeout ;

protected:

   // This is all stuff that needs to get set up in the setUpOipInfo function in derived classes.
   ALARM_VALUES _alarmId;

   // Sound Code defaults to ALERT_SOUND and Mode to SOUND_CONTINUOUS, but can be overridden in the derived class.
   SOUND_CODE _soundCode;
   SOUND_MODE _soundMode;

   bool       _isBeepingAlarm;

   virtual void cancelOip ();

   virtual void continueOip ();

   virtual void rinsebackOip ();

   void startBeepingSound ();

   // This function is virtual and must be overridden for subclasses to compile.
   // In order for them to work correctly, you should set the data for _alarmId
   // and _isBeepingAlarm in there somewhere.
   virtual void setUpOipInfo (const char* allocation_parameter) = 0;

public:
   // CONSTRUCTOR
   OipAlarmBase(WINDOW_COLOR wcolor, GUI_SCREEN_ID win_ID);


   // DESTRUCTOR
   virtual ~OipAlarmBase();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // ALARM_SET_STRUCT_MANAGER
   // This method sets the command codes into a structure and sends it to the Proc
   // handler.
   void alarm_set_struct_manager ();

   void muteBeepingSound ();
}; // END of OipAlarmBase Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH b029bdb3fb5b444a8d0094f736b89562 */
