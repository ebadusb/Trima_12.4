/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_ALARM

*     File name:   alarm.hpp

*************************< FOCUSSED GUI DOMAIN >****************************/

// Single include define guardian
#ifndef ALARM_HPP
#define ALARM_HPP

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"

// string definitions and methods
#include "frmtstr.hpp"

// state definitions
#include "states.h"

// message include
#include "timermessage.h"
#include "message.h"

// alarm include
#include "alarms.h"

// alarm handler include
#include "alarmhdl.hpp"

// alarm table include
#include "alarmtb.h"

// .. and the *other* alarm table
#include "alarmcontainer.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// GUI <-> safety screen include
#include "safproc.h"

// sound include
#include "sounds.hpp"

#include "bitmapTextBox.h"
#include "trimamessages.h"



// "C" callback stubs for redirection to set callback method

// CALLBACK_CONTINUE
// This Method requests a continuation of the procedure
void callback_continue_procedure (void* data);

// CALLBACK_RINSEBACK
// This Method requests a change to the rinseback state
void callback_goto_rinseback (void* data);

// CALLBACK_END_METEREDSTORAGE
// This Method requests a jump to the end of metered storage procedure
void callback_goto_endMeteredStorage (void* data);


// CALLBACK_ABORT
// This Method executes the abort alarm responce
void callback_abort_procedure (void* data);


// CALLBACK_UNLOAD
// This Method executes the unload cassette alarm responce
void callback_unload (void* data);


// CALLBACK_ADJUST
// This Method executes the adjust procedure alarm responce
void callback_adjust (void* data);


// CALLBACK_MORE_INFO
// This Method executes the more info request
void callback_more_info (void* data);


// CALLBACK_BUTTON_SOUND
// This Method gives our buttons its "beep"
void callback_button_sound (void* data);


// CALLBACK_MUTE_SOUND
// This Method mutes the alarm "beep"
void callback_mute_sound (void* data);


// CLASS DECLARES

class Screen_ALARM
   : public Base_Apheresis_Window
{
private:

   // ATTRIBUTES

   long                  current_state;      // refrencing state value - enum
   long                  current_substate;   // procedure substate - enum

   bool                  inMeteredStorage;     // set to true if we are currently doing metered storage for RBC||PLT

   ALARM_VALUES          alarm_enumeration;    // store incoming alarm enumeration
   ALARM_TYPES           alarm_type;           // store incoming alarm type
   ALARM_STATES          alarm_state;          // store incoming alarm state

   GuiAlarmTable         _alarmTable;
   const ALARM_TABLE*    _alarmItem;
   AlarmTable            _procAlarmsTable; // various alarm tables are confusing, this is the one in alarmcontainers.h

   bool                  four_button_suite; // local state variable to determine number of buttons

   int                   rinseback_status_request; // flag to screen origin of status message

   guistring             alarm_enumeration_buffer; // buffer for alarm enum conversion

   ALARM_VALUES          alarm_name;

   TEXT_STRING_ID        alarm_status_line_text;
   TEXT_STRING_ID        alarm_body_text;
   BITMAP_ID             alarm_bitmap;
   char*                 goto_screen_literal;
   GUI_SCREEN_ID         goto_screen_gui;
   unsigned long         alarm_opts;

   Alarm_response_struct alarm_set_struct;

   // Screen Event Handler pointers
   AlarmStateChangeMsg _msgAlarmStateChange;

   // Message monitored for Cassette position changes.
   CassettePositionStatusMsg _cassStatMsg;

   // Screen Timer Handler pointers
   TimerMessage _timerMuteTimeout ;

   // status line storage for recreation of calling screen status line
   // WINDOW_COLOR    calling_status_line_color;
   // TEXT_STRING_ID  calling_status_line_text;

   // The alarm screen bitmap buttons
   Button_Bitmap btn_alarm_continue;
   Button_Bitmap btn_alarm_rinseback;
   Button_Bitmap btn_alarm_exit;
   Button_Bitmap btn_alarm_adjust;
   Button_Bitmap btn_alarm_unload;

   BitmapTextBox bitmap_continue_button_disabled;

   // The placard bitmap objects
   Bitmap bitmap_text_background;
   Bitmap bitmap_text_bitmap_background;
   Bitmap bitmap_alarm_foreground;

   // The display text objects
   // Display_Text  text_continue_button_disabled;
   Display_Text text_and_bitmap_display;
   Display_Text text_only_display;
   Display_Text text_status_line_display;
   Display_Text text_version;
   Display_Text text_alarm_number ;

   // DECODE_ALLOCATION_PARAM
   // determine alarm screen display parameters using lookup table
   void decode_allocation_param ();


   // ALARM_SET_STRUCT_MANAGER
   // This method manages all structs recieved via messaging
   void alarm_set_struct_manager ();


   // RECONSTRUCT_ALARM_SCREEN
   // This method builds all bitmaped alarm screenes per alarms table
   void reconstruct_alarm_screen ();


   // FOUR_BUTTON_SCREEN_ADJUSTMENT
   // This method does some button jiggery-pokery
   void four_button_suite_adjustment ();

protected:

   void alarm_state_change ();
   void checkRinsebackOk (ALARM_VALUES alrmVal);
   void screen_timer_handler ();
   void mute_timer_handler ();
   //
   // Callback for Cassette position change messages
   //
   void cassetteChange ();

public:


   // CONSTRUCTOR
   Screen_ALARM();


   // DESTRUCTOR
   virtual ~Screen_ALARM();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char*);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_GOBACK_BUTTON
   // This method is called from the vitual C callback stub and executes
   // "go back"
   virtual void process_goback_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the C callback stub and executes "continue"
   void process_continue_procedure_button ();

   // Special callbacks for alarms with special button adjustments
   void process_continue_to_2nd_RBC_product ();

   // PROCESS_RINSEBACK_BUTTON
   // This method is called from the C callback stub and executes "rinseback"
   void process_rinseback_button ();


   // PROCESS_ABORT_BUTTON
   // This method is called from the C callback stub and executes "abort"
   void process_abort_button ();


   // PROCESS_UNLOAD_BUTTON
   // This method is called from the C callback stub and executes "unload"
   void process_unload_button ();


   // PROCESS_ADJUST_BUTTON
   // This method is called from the C callback stub and executes "adjust"
   void process_adjust_button ();


   // PROCESS_MORE_INFO_BUTTON
   // This method is called from the C callback stub and executes "abort"
   void process_more_info_button ();


   // PROCESS_MUTE_BUTTON
   // This method is called from the C callback stub and executes "mute"
   void process_mute_button ();


   // END METERD STORAGE
   void process_end_metered_storage ();


   // GetEnum
   ALARM_VALUES getEnum () const {return alarm_enumeration; }

};

#endif

/* FORMAT HASH a7a19f49f6c2b15ab1a737ef446ba0b3 */
