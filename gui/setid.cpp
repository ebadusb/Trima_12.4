/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SETID

*     File name:   setid.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "setid.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "CatalogChecker.h"
#include "guiglobs.hpp"

// EXTERNAL REFERENCES
#define BITMAP_BUTTON_UNLOAD_SET_SELECTION_X 486
#define BITMAP_BUTTON_UNLOAD_SET_SELECTION_Y 157
#define BITMAP_BUTTON_USE_SET_SELECTION_X 485
#define BITMAP_BUTTON_USE_SET_SELECTION_Y 320

#define BITMAP_CARTOUCHE_SET_ID_X 12
#define BITMAP_CARTOUCHE_SET_ID_Y 115

static void _cbSetIdContinue (void* data);
static void _cbSetIdUnload   (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SETID::Screen_SETID()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SETID),

     _bmpTxtCartouche    (BITMAP_CARTOUCHE_SET_ID_X,
                          BITMAP_CARTOUCHE_SET_ID_Y,
                          BITMAP_CARTOUCHE_SET_ID),

     _txtBody            (textMiscSetIdOperatorPrompt),
     _txtRefNum          (textEmptyFormatSetIdRef),
     _txtAlrmNum         (textEmptyFormatAlarmNumberData),

     _btnUseSet           (GUI_BUTTON_SETID_PLT_PLS_RBC,
                           BITMAP_BUTTON_USE_SET_SELECTION_X,
                           BITMAP_BUTTON_USE_SET_SELECTION_Y,
                           textButtonLongContinue,
                           BITMAP_BUTTON_CONTINUE_UP,
                           BITMAP_BUTTON_CONTINUE_DN),

     _btnUnload        (GUI_BUTTON_SETID_LOAD_NEW,
                        BITMAP_BUTTON_UNLOAD_SET_SELECTION_X,
                        BITMAP_BUTTON_UNLOAD_SET_SELECTION_Y,
                        textButtonAlarmUnloadSet,
                        BITMAP_BUTTON_UNLOAD_UP,
                        BITMAP_BUTTON_UNLOAD_DN),
     _alarmState(CLEARED),

     _stateOfBtnLockout(0),
     _currentAlarm(NULL_ALARM)


{
   guistring tmpStr;
   int       tmpStrLen = sprintf(tmpStr, "%u", (unsigned int)SET_IDENTIFICATION_ALARM);
   _txtAlrmNum.set_text_direct(tmpStrLen, tmpStr);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_SETID::~Screen_SETID()
{
   if (_allocated)
   {
      Screen_SETID::deallocate_resources();
   }
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_SETID::allocate_resources (const char* allocation_parameter)
{
   // establish the alarm "name" we're running under.
   _currentAlarm                     = SET_IDENTIFICATION_ALARM;

   guiglobs::current_displayed_alarm = _currentAlarm;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // Allocate the bitmaps
   _bmpTxtCartouche.allocate_resources(*this);

   // Allocate the text prompt
   _txtBody.allocate_resources(*this);
   _txtRefNum.allocate_resources(*this);
   _txtAlrmNum.allocate_resources(*this);

   // display the cat number
   #if (CPU==SIMNT)
   const char* catalogString;
   if (guiglobs::cassette_mgr->HasCurrentCassetteSet())
      catalogString = guiglobs::cassette_mgr->getCurrentStringID();
   else
      catalogString = "80419";
   #else
   const char* catalogString = guiglobs::cassette_mgr->getCurrentStringID();
   #endif

   // Put the catalog number on the button.
   int str_len = strlen(catalogString);
   _txtRefNum.set_text_direct(str_len, catalogString);

   // Allocate the selection buttons
   _btnUseSet.allocate_resources(*this);
   _btnUnload.allocate_resources(*this);

   // Link the buttons to their "C" callback stubs
   _btnUseSet.set_callback (_cbSetIdContinue, (void*)this);
   _btnUnload.set_callback (_cbSetIdUnload,      (void*)this);

   // Allocate a scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChanged.init(Callback<Screen_SETID>(this, &Screen_SETID::alarmStateChange), MessageBase::SNDRCV_RECEIVE_ONLY);

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.deallocate_resources ();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Initialize the button lockout flag
   _stateOfBtnLockout = 0;

   // send the alarm confirm message after allocation is complete
   _alarmSetStruct.alarm_name     = _currentAlarm;
   _alarmSetStruct.alarm_response = CONFIRM;
   alarmSetStructManager ();
}  // END of Screen_SETID ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SETID::deallocate_resources ()
{
   // Deallocate the bitmaps
   _bmpTxtCartouche.deallocate_resources();

   // Deallocate the text prompt
   _txtBody.deallocate_resources();
   _txtRefNum.deallocate_resources();
   _txtAlrmNum.deallocate_resources();

   // Deallocate the selection buttons
   _btnUseSet.deallocate_resources();
   _btnUnload.deallocate_resources();

   // Deallocate the scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChanged.deregisterMsg();

   // Reset the "disposable lowering" status line ATTENTION message
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   guiglobs::current_displayed_alarm = NULL_ALARM;

   // send the alarm confirm message after deallocation is complete
   _alarmSetStruct.alarm_name     = NULL_ALARM;
   _alarmSetStruct.alarm_response = CONFIRM;
   alarmSetStructManager ();

}   // END of Screen_SETID DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// ALARM STATE CHANGE
void Screen_SETID::alarmStateChange ()
{
   const long msgData = _msgAlarmStateChanged.getData();

   if ((msgData == (long)LATCHED))
   {
      // set alarm state
      _alarmState = LATCHED;

   }
   else if ((msgData == (long)ACTIVE))
   {
      // set alarm state
      _alarmState = ACTIVE;

   }  // End of else if (message..

   // Confirm to Proc that we got the alarm message.
   _alarmSetStruct.alarm_name     = _currentAlarm;
   _alarmSetStruct.alarm_response = CONFIRM;
   alarmSetStructManager ();

}   // END of Screen_SETID SCREEN_NOTIFY_HANDLER

////////////////////////////////////////////////////////////////////////////////
// ALARM_SET_STRUCT_MANAGER
void Screen_SETID::alarmSetStructManager ()
{
   // struct message declare for outgoing donor vitals information
   Message<Alarm_response_struct> msgAlarmSet(MessageBase::SEND_LOCAL);

   // Actually send the message
   msgAlarmSet.send(_alarmSetStruct);

}   // End of ALARM_SET_STRUCT_MANAGER

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_SETID::processContinueButton ()
{
   DataLog(log_level_gui_info) << "Operator Continues from SetId alarm screen" << endmsg;

   if (raceLockout ())
   {
      return;
   }

   // the stamp bit is set to 1 if white or 0 if black
   if (guiglobs::cassette_mgr->currentSetIsWhiteStamp())
      setIdExit (cassette::PLTPLSRBC);
   else setIdExit (cassette::RBCPLS);

   // get rid of the alarm screen ... shooo
   RemoveMostRecentStatelessScreenMsg removescreen(MessageBase::SEND_LOCAL);
   removescreen.send(0);
}   // END of Screen_SETID PROCESS_PLT_PLS_RBC_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS CANCEL BUTTON
void Screen_SETID::process_cancel_button ()
{
   DataLog(log_level_gui_info) << "Operator endRun from SetId alarm screen" << endmsg;
   guistring alrmAllocString;  // newly constructed alarm data buffer
   guistring alrmEnumBuffer;

   if (raceLockout ()) return;

   // Lock the screen from other button presses
   freezeScreen ();

   // convert the integer element to text for string construction
   sprintf(alrmEnumBuffer, "%d", _currentAlarm);

   // create the string that will be parsed by the ABORTCNF screen using
   // components recieved and then created via the incoming alarm structure
   strncpy(alrmAllocString, ABORT_TYPE_ALARM, 5);
   strncat(alrmAllocString, "~", 1);
   strncat(alrmAllocString, alrmEnumBuffer, MAXSIZE_BWINDOW_PARAM - 7);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << "GUI endRun received" << (int)ABORT << " " << alrmAllocString << endmsg;

   // goto ABORTCNF to give user opportunity to confirm choice
   goto_stateless_screen (GUI_SCREEN_ABORTCNF, alrmAllocString);
}    // END of

////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOAD_NEWSET_BUTTON
void Screen_SETID::processUnloadButton ()
{
   DataLog(log_level_gui_info) << "Operator Unloading set from SetId alarm screen" << endmsg;

   if (raceLockout ()) return;

   Alarm_response_struct alarm_set_struct;

   // set and send alarm response
   alarm_set_struct.alarm_name     = SET_IDENTIFICATION_ALARM;
   alarm_set_struct.alarm_response = UNLOAD;

   Message<Alarm_response_struct> alarm(MessageBase::SEND_LOCAL);
   alarm.send(alarm_set_struct);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // Clear the current set from the cassette manager.
   guiglobs::cassette_mgr->clearCurrent();

   // log alarm response
   DataLog(log_level_gui_info) << (int)alarm_set_struct.alarm_response << " alarm unload request" << endmsg;

   // Send an indication to "remove the last stateless screen" which is
   // THIS screen, so the alarm screen leaves and the appropriate
   // procedure screen is uncovered.
   RemoveMostRecentStatelessScreenMsg removescreen(MessageBase::SEND_LOCAL);
   removescreen.send(0);
}   // END of Screen_SETID PROCESS_LOAD_NEWSET_BUTTON

////////////////////////////////////////////////////////////////////////////////
// SET_IDENTIFICATION_EXIT
void Screen_SETID::setIdExit (cassette::cassetteType setID)
{
   // Lock the screen from other button presses
   freezeScreen ();

   // Notify Proc that the operator picked the platelet plasma set.
   OperatorSetIdentificationMsg identMsg(MessageBase::SEND_GLOBAL);
   identMsg.send((long)setID);

   // set and send alarm response
   _alarmSetStruct.alarm_name     = _currentAlarm;
   _alarmSetStruct.alarm_response = CONTINUE;
   alarmSetStructManager ();

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed

   guiglobs::gui_alarm_manager.clear_alarm_screen ();
}   // END of Screen_SETID SET_IDENTIFICATION_EXIT



////////////////////////////////////////////////////////////////////////////////
// FREEZE_SCREEN
void Screen_SETID::freezeScreen ()
{
   // Disable the rest
   _btnUseSet.disable_callbacks();
   _btnUnload.disable_callbacks();
   btn_cancel.disable_callbacks();
}   // END of Screen_SETID FREEZE_SCREEN



////////////////////////////////////////////////////////////////////////////////
// THAW_SCREEN
void Screen_SETID::thawScreen ()
{
   // Enable all the standard buttons on this screen -- aphwindow (parent) method
   // Enable the rest
   _btnUseSet.enable_callbacks();
   _btnUnload.enable_callbacks();
   btn_cancel.enable_callbacks();
}   // END of Screen_SETID THAW_SCREEN



////////////////////////////////////////////////////////////////////////////////
// BUTTON_RACE_LOCKOUT
int Screen_SETID::raceLockout ()
{
   // Zero return means that no button has called this lockout routine yet -- this caller is the first.
   // One return means that someone has been through this logic already and therefore this button is second
   //     (or later) and should not process.

   // The semaphore, _stateOfBtnLockout, is reset during allocate (above) -- in other words only
   //     one button press per allocation is allowed.

   // N.B. all GUI logic is single threaded, therefore this code won't be interrupted by other
   //     messages.
   if (_stateOfBtnLockout == 0)
   {
      _stateOfBtnLockout = 1;
      return 0;
   }
   else return 1;
}   // END of Screen_SETID BUTTON_RACE_LOCKOUT

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Screen_SETID::refresh_resources ()
{
   // First, call the base class' refresh resources for status line/bar stuff
   Base_Apheresis_Window::refresh_resources ();

   // Now unlock the button semaphore so that the buttons may be pushed.
   _stateOfBtnLockout = 0;

   // Thaw the screen
   thawScreen ();
}    // END of Screen_SETID REFRESH_RESOURCES

void Screen_SETID::initialize_status_line ()
{
   // Set the status line to indicate that the disposable is lowering
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, textStatSetId);
}


////////////////////////////////////////////////////////////////////////////////
// SETID_CB_PLT_PLS_RBC
void _cbSetIdContinue (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_SETID*)data) -> processContinueButton ();
}   // END of SETID_CB_PLT_PLS_RBC


////////////////////////////////////////////////////////////////////////////////
// SETID_CB_NEWSET
void _cbSetIdUnload (void* data)
{
   // Actually call the continue method within REFLUID screen
   ((Screen_SETID*)data) -> processUnloadButton ();
}   // END of SETID_CB_NEWSET

/* FORMAT HASH 3ccb6a2cb05a8feb5926da218a4a584b */
