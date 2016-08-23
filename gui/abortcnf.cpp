/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_ABORTCNF

*     File name:   abortcnf.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "abortcnf.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "adjust_manager.hpp"
#include "predict_manager.hpp"

// OTHER INCLUDES
#include "alarms.h"

static const int BUTTON_Y_COORD      = 315 ;
static const int TWO_BUTTON_MARGIN   = 80  ;
static const int TWO_BUTTON_DELTA    = 320 ;
static const int THREE_BUTTON_MARGIN = 20  ;
static const int THREE_BUTTON_DELTA  = 222 ;
static const int FOUR_BUTTON_MARGIN  = 2   ;
static const int FOUR_BUTTON_DELTA   = 161 ;


// GLOBAL FUNCTIONS FOR CALLBACKS
////////////////////////////////////////////////////////////////////////////////
// ABORT_CB_EXIT_BTN
void abort_cb_exit_btn (void* data)
{
   // Actually call the EXIT button routine
   ((Screen_ABORTCNF*)data)->process_exit_button ();
}


////////////////////////////////////////////////////////////////////////////////
// ABORT_CB_RINSEBACK_BTN
void abort_cb_rinseback_btn (void* data)
{
   // Actually call the RINSEBACK button routine
   ((Screen_ABORTCNF*)data)->process_rinseback_button ();
}


////////////////////////////////////////////////////////////////////////////////
// ABORT_CB_DONOTEXIT_BTN
void abort_cb_donotexit_btn (void* data)
{
   // Actually call the DO NOT EXIT button routine
   ((Screen_ABORTCNF*)data)->process_donotexit_button ();
}


////////////////////////////////////////////////////////////////////////////////
// ABORT_CB_DONOTEXIT_BTN
void abort_cb_abortras_btn (void* data)
{
   // Actually call the DO NOT EXIT button routine
   ((Screen_ABORTCNF*)data)->process_partial_abort (END_RAS_ADDITION);
}

////////////////////////////////////////////////////////////////////////////////
// ABORT_CB_DONOTEXIT_BTN
void abort_cb_abortpas_btn (void* data)
{
   // Actually call the DO NOT EXIT button routine
   ((Screen_ABORTCNF*)data)->process_partial_abort (END_PAS_ADDITION);
}




////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_ABORTCNF::Screen_ABORTCNF()
   : Base_Apheresis_Window(GREY, GUI_SCREEN_ABORTCNF),

     text_abort_confirm (textAbortBodyProc),

     bitmap_cancel_text_panel
        (9,
        115,
        BITMAP_PLACARD_ABORTCNF_PANEL),

     _btnDontExit
        (GUI_BUTTON_ABORTCNF_NOEXIT,
        TWO_BUTTON_MARGIN + TWO_BUTTON_DELTA,
        BUTTON_Y_COORD,
        textButtonAbortReturnToPrev,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_DN),

     _btnExit
        (GUI_BUTTON_ABORTCNF_EXIT,
        TWO_BUTTON_MARGIN,
        BUTTON_Y_COORD,
        textEmpty,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_DN),

     _btnRinseback
        (GUI_BUTTON_ABORTCNF_EXIT,
        TWO_BUTTON_MARGIN,
        BUTTON_Y_COORD,
        textButtonAbortRinsebackExit,
        BITMAP_BUTTON_ABORTCNF_RINSEBACK_UP,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_DN),

     _btnAbortRas
        (GUI_BUTTON_ABORTCNF_ABORT_RAS,
        THREE_BUTTON_MARGIN + THREE_BUTTON_DELTA,
        BUTTON_Y_COORD,
        textButtonAbortRasExit,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_DN),


     _btnAbortPas
        (GUI_BUTTON_ABORTCNF_ABORT_PAS,
        THREE_BUTTON_MARGIN + THREE_BUTTON_DELTA,
        BUTTON_Y_COORD,
        textButtonAbortPasExit,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP,
        BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_DN),

     dismiss_flag(FALSE)
{}   // END of Screen_ABORTCNF CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_ABORTCNF::~Screen_ABORTCNF()
{
   Screen_ABORTCNF::deallocate_resources ();
}



void Screen_ABORTCNF::composeTextAndGraphics (int alarmId, CONFIRM_CALL_TYPE abortType, bool isBothPasAndRas = false)
{
   // Explanation cartouche
   bitmap_cancel_text_panel.allocate_resources(*this);

   // Explanation text
   text_abort_confirm.allocate_resources(*this);
   DataLog (log_level_gui_info) << "composeTextAndGraphics alrmid=" << alarmId  << ", abortType="  << abortType << ", isBothPasAndRas=" << isBothPasAndRas << endmsg;


   // Deallocate all buttons.  We'll allocate the appropriate ones later.
   _btnExit.deallocate_resources();
   _btnRinseback.deallocate_resources();
   _btnDontExit.deallocate_resources();
   _btnAbortRas.deallocate_resources();
   _btnAbortPas.deallocate_resources();

   // This vector is populated in the switch statement below.
   vector<Button_Bitmap*> buttons;


   // Get the type of abort that invoked this screen and set the button and
   // information text accordingly.
   switch (abortType)
   {
      case ABORT_NORMAL :
      case ABORT_ALARM :
         buttons.push_back(&_btnExit);
         buttons.push_back(&_btnDontExit);

         text_abort_confirm.set_string_id(textAbortBodyProc);
         _btnExit.set_string_id(textButtonAbortProcExit);
         _btnExit.set_up_bitmap(BITMAP_BUTTON_ABORTCNF_EXIT_RED_UP);
         break;

      case ABORT_RINSEBACK :
      case ABORT_ALARM_RINSEBACK :

         buttons.push_back(&_btnRinseback);
         buttons.push_back(&_btnExit);
         buttons.push_back(&_btnDontExit);

         text_abort_confirm.set_string_id(textAbortBodyRinseback);
         _btnExit.set_string_id(textButtonAbortProcExit);
         _btnExit.set_up_bitmap(BITMAP_BUTTON_ABORTCNF_EXIT_RED_UP);
         break;

      case ABORT_ADDITIVE_SOLUTIONS_ALARM :
      case ABORT_ADDITIVE_SOLUTIONS :

         buttons.push_back(&_btnExit);
         _btnExit.set_string_id(textButtonAbortAasExit);
         _btnExit.set_up_bitmap(BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP);

         if (isBothPasAndRas)
         {
            DataLog (log_level_gui_info) << "isBothPasAndRas." << endmsg;
            switch (getAlarmAasCorrelation(alarmId))
            {
               case AAS_ABORT_ALARM_NOT_ACTUALLY_AN_ALARM :
                  DataLog (log_level_gui_info) << "AAS_ABORT_ALARM_NOT_ACTUALLY_AN_ALARM." << endmsg;
                  buttons.push_back(&_btnAbortPas);
                  buttons.push_back(&_btnAbortRas);
                  text_abort_confirm.set_string_id(textAbortBodyRasOrPas);
                  break;

               case AAS_ABORT_ALARM_NOT_AAS_RELATED :
                  DataLog (log_level_gui_info) << "AAS_ABORT_ALARM_NOT_AAS_RELATED." << endmsg;
                  // If this is an alarm not related to a specific product, we can't
                  // guarantee that ending one product will make sense to resolve the
                  // issue.
                  text_abort_confirm.set_string_id(textAbortBodyAas);
                  break;

               case AAS_ABORT_ALARM_RAS_RELATED :
                  DataLog (log_level_gui_info) << "AAS_ABORT_ALARM_RAS_RELATED." << endmsg;
                  buttons.push_back(&_btnAbortRas);
                  text_abort_confirm.set_string_id(textAbortBodyRas);
                  break;

               case AAS_ABORT_ALARM_PAS_RELATED :
                  DataLog (log_level_gui_info) << "AAS_ABORT_ALARM_PAS_RELATED." << endmsg;
                  buttons.push_back(&_btnAbortPas);
                  text_abort_confirm.set_string_id(textAbortBodyPas);
                  break;
            }
         }
         else
         {
            DataLog (log_level_gui_info) << " not both RAS PAS." << endmsg;
            text_abort_confirm.set_string_id(textAbortBodyAas);
         }

         buttons.push_back(&_btnDontExit);
         break;

      case CONFIRM_SPILLOVER :
         buttons.push_back(&_btnExit);
         buttons.push_back(&_btnDontExit);

         text_abort_confirm.set_string_id(textAbortBodySpillover);
         _btnExit.set_string_id(textButtonAbortSpilloverExit);
         _btnExit.set_up_bitmap(BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP);
         break;

      case CONFIRM_AIR_IN_PLASMA :
         buttons.push_back(&_btnExit);
         buttons.push_back(&_btnDontExit);

         text_abort_confirm.set_string_id(textAbortBodyAirInPlasmaLine);
         _btnExit.set_string_id(textButtonAbortAirInPlasmaLineExit);
         _btnExit.set_up_bitmap(BITMAP_BUTTON_ABORTCNF_EXIT_NOEXIT_UP);
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Unknown abort type requested");
         break;

   }  // End of switch (abortType)

   int xCoords[4] = {0, 0, 0, 0};

   switch (buttons.size())
   {
      case 2 :
         xCoords[0] = TWO_BUTTON_MARGIN;
         xCoords[1] = xCoords[0] + TWO_BUTTON_DELTA;
         break;

      case 3 :
         xCoords[0] = THREE_BUTTON_MARGIN;
         xCoords[1] = xCoords[0] + THREE_BUTTON_DELTA;
         xCoords[2] = xCoords[1] + THREE_BUTTON_DELTA;
         break;

      case 4 :
         xCoords[0] = FOUR_BUTTON_MARGIN;
         xCoords[1] = xCoords[0] + FOUR_BUTTON_DELTA;
         xCoords[2] = xCoords[1] + FOUR_BUTTON_DELTA;
         xCoords[3] = xCoords[2] + FOUR_BUTTON_DELTA;
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Unexpected button size");
         break;
   }

   OSRegion tmpRegion;

   _btnExit.get_region(tmpRegion);
   tmpRegion.x = xCoords[0];

   int btnIndex = 0;

   DataLog (log_level_gui_info) << buttons.size() << " abort buttons:";

   for (vector<Button_Bitmap*>::iterator btnIter = buttons.begin();
        btnIter != buttons.end(); ++btnIter, ++btnIndex)
   {
      guistring btnStr;
      (*btnIter)->get_text_direct(btnStr);
      btnStr.strip_line_breaks();
      DataLog (log_level_gui_info) << " {idx=" << btnIndex << "; id=" << (*btnIter)->get_button_id()
                                   << "; x=" << xCoords[btnIndex] << "; txt=" << btnStr << "}";

      tmpRegion.x = xCoords[btnIndex];
      (*btnIter)->set_region(tmpRegion);
      (*btnIter)->allocate_resources(*this);
   }

   DataLog (log_level_gui_info) << endmsg;
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_ABORTCNF::allocate_resources (const char* allocation_parameter)
{
   int               alarm_id = -1;
   CONFIRM_CALL_TYPE abort_type;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // deallocate all "standard" buttons that are not needed, except for help
   btn_lowleft.deallocate_resources  ();
   btn_goback.deallocate_resources   ();
   btn_continue.deallocate_resources ();
   btn_cancel.deallocate_resources   ();

   // Reset the dismiss flag when the screen is allocated
   dismiss_flag = FALSE;

   // Create the message receiver objects for the two stateless messages of interest
   _msgRemoveMostRecentStatelessScreen.init(Callback<Screen_ABORTCNF>(this, &Screen_ABORTCNF::screen_notify_handler), MessageBase::SNDRCV_RECEIVE_ONLY);
   _msgRemoveAllStatelessScreens.init(Callback<Screen_ABORTCNF>(this, &Screen_ABORTCNF::screen_notify_handler), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Connect the buttons to their "C" callback routines.
   _btnExit.set_callback(abort_cb_exit_btn, (void*)this);
   _btnRinseback.set_callback(abort_cb_rinseback_btn, (void*)this);
   _btnDontExit.set_callback(abort_cb_donotexit_btn, (void*)this);
   _btnAbortRas.set_callback(abort_cb_abortras_btn, (void*)this);
   _btnAbortPas.set_callback(abort_cb_abortpas_btn, (void*)this);

   // Get the type of abort that invoked this screen and set the button and
   // information text accordingly.
   abort_type = return_confirm_type (&alarm_id);

   bool isBothRasAndPas =
      guiglobs::aasRunInfo.isPasProcedure() &&
      guiglobs::aasRunInfo.isRasProcedure();


   composeTextAndGraphics(alarm_id, abort_type, isBothRasAndPas);

   // Now link the status bar/line to this window
   //  use whatever status is currently displayed on
   //  the bar and line.
   link_status_bar_line();

}  // END of Screen_ABORTCNF ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_ABORTCNF::deallocate_resources ()
{
   //
   //
   //   If we got here via a request for spillover, we must clear the
   //   pause condition before continuing.
   //
   int dummyAlarm;
   if (return_confirm_type(&dummyAlarm) == CONFIRM_SPILLOVER)
   {
      clear_spillover_pause();
   }

   // Explanation cartouche
   bitmap_cancel_text_panel.deallocate_resources ();
   // Explanation text
   text_abort_confirm.deallocate_resources ();

   _btnExit.deallocate_resources ();
   _btnDontExit.deallocate_resources ();
   _btnAbortRas.deallocate_resources();
   _btnAbortPas.deallocate_resources();
   _btnRinseback.deallocate_resources();

   _msgRemoveMostRecentStatelessScreen.deregisterMsg();
   _msgRemoveAllStatelessScreens.deregisterMsg();

   guiglobs::apheresis_status_line->deactivate_status_line_type(END_RUN_STATUS_LINE);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}   // END of Screen_ABORTCNF DEALLOCATE_RESOURCES

void Screen_ABORTCNF::removeButtonCallbacks ()
{
   _btnDontExit.remove_callbacks();
   _btnExit.remove_callbacks();
   _btnAbortRas.remove_callbacks();
   _btnAbortPas.remove_callbacks();
   _btnRinseback.remove_callbacks();
}


void Screen_ABORTCNF::dismissScreen (bool allStatelessScreens)
{
   if (allStatelessScreens)
   {
      // Remove myself *and* the underlying alarm screen
      RemoveAllStatelessScreensMsg msg(MessageBase::SEND_LOCAL);
      msg.send(0);
   }
   else if (get_screen_invocation_type() == GUI_SCREEN_INVOKE_CHILD)
   {
      // Remove myself as a child screen
      RemoveMostRecentChildScreenMsg msg(MessageBase::SEND_LOCAL);
      msg.send(0);
   }
   else
   {
      // Remove myself as a stateless screen
      RemoveMostRecentStatelessScreenMsg msg(MessageBase::SEND_LOCAL);
      msg.send(0);
   }
}

////////////////////////////////////////////////////////////////////////////////
// PROCESS_EXIT_BUTTON
void Screen_ABORTCNF::process_exit_button ()
{
   CONFIRM_CALL_TYPE              abort_type;
   int                            alarm_id = -1;

   Message<Alarm_response_struct> Msg_Alarm_Set(MessageBase::SEND_LOCAL);
   RemoveMostRecentChildScreenMsg RemoveMostRecentChildScreen(MessageBase::SEND_LOCAL);
   CancelProcedureMsg             cancel(MessageBase::SEND_LOCAL);
   CancelMssMsg                   cancelMss(MessageBase::SEND_LOCAL);
   Alarm_response_struct          alarm_set_struct;

   DataLog (log_level_gui_info) << "Screen_ABORTCNF, processing exit button" << endmsg;

   // Disable the buttons on this screen
   removeButtonCallbacks();

   // Get the type of abort that invoked this screen
   abort_type = return_confirm_type (&alarm_id);

   // If we did a PFR during MSS and we touched "end run" on the PFR
   // alarm, then we should not attempt to run MSS.
   if (guiglobs::cdsData.run.currentMSSSubState.Get() != INVALID_SUBSTATE)
   {
      DataLog (log_level_gui_info) << "Notifying proc of MSS cancellation." << endmsg;
      RequestCancelMssPfr cancelMssPfr(MessageBase::SEND_LOCAL);
      cancelMssPfr.send();
   }

   // Switch on the abort type and send EXIT messages accordingly
   switch (abort_type)
   {
      case ABORT_ADDITIVE_SOLUTIONS :
      {
         // Send an indication to "Cancel the MSS Procedure" requested by operator
         DataLog(log_level_gui_info) << "Sending CancelMssMsg." << endmsg;
         cancelMss.send(1);
         dismissScreen();
      }
      break;

      case ABORT_ADDITIVE_SOLUTIONS_ALARM :
      {
         // Load the alarm abort structure
         alarm_set_struct.alarm_name     = (ALARM_VALUES)alarm_id;
         alarm_set_struct.alarm_response = ABORT;

         // Actually send the message
         Msg_Alarm_Set.send(alarm_set_struct);

         // Remove alarm status line color and message
         guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

         // stop Metering and goto MSS Disconnect
         MSSEndRunMsg _endRun(MessageBase::SEND_LOCAL);
         _endRun.send(0);

         // remove any alarm screens
         dismissScreen(true);

         // Send an indication to "Cancel the MSS Procedure" requested by operator
         cancelMss.send(0);
         RemoveMostRecentChildScreen.send(0);
      }
      break;

      case ABORT_NORMAL :
      case ABORT_RINSEBACK :

         // Send an indication to "Cancel the Procedure" requested by operator
         cancel.send(0);

         // Dismiss myself and possibly all other stateless screens
         dismissScreen( (get_screen_invocation_type() == GUI_SCREEN_INVOKE_STATELESS) );
         break;

      case ABORT_ALARM_RINSEBACK :
      case ABORT_ALARM :
         // Load the alarm abort structure
         alarm_set_struct.alarm_name     = (ALARM_VALUES)alarm_id;
         alarm_set_struct.alarm_response = ABORT;

         // Actually send the message
         Msg_Alarm_Set.send(alarm_set_struct);

         // Remove alarm status line color and message
         guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

         // Dismiss myself and the alarm screen
         dismissScreen(true);
         break;

      case CONFIRM_SPILLOVER :
         // Send an indication to recover from spillover as requested by
         // operator. [NOTE: we don't worry about attaching or detaching from the
         // AdjustManager, since this is a 'fire-and-forget' message and we don't
         // have any relevant callbacks.
         AdjustManager::sendAdjustment(SPILLOVER, HERE);

         // Dismiss myself
         dismissScreen();
         break;

      case CONFIRM_AIR_IN_PLASMA :
         // Send an indication to "Cancel the Procedure" requested by
         // operator. [NOTE: we don't worry about attaching or detaching from the
         // AdjustManager, since this is a 'fire-and-forget' message and we don't
         // have any relevant callbacks.
         AdjustManager::sendAdjustment(AIR_IN_PLASMA_LINE, HERE);

         // Dismiss myself
         dismissScreen();
         break;

      default :
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Unknown abort type requested");
         break;

   } // End of abort_type switch.


}   // END of PROCESS_EXIT_BUTTON






////////////////////////////////////////////////////////////////////////////////
// PROCESS_RINSEBACK_BUTTON
void Screen_ABORTCNF::process_rinseback_button ()
{
   CONFIRM_CALL_TYPE              abort_type;
   int                            alarm_id = -1;

   Message<Alarm_response_struct> Msg_Alarm_Set(MessageBase::SEND_LOCAL);
   RemoveMostRecentChildScreenMsg RemoveMostRecentChildScreen(MessageBase::SEND_LOCAL);
   RemoveAllStatelessScreensMsg   RemoveAllStatelessScreens(MessageBase::SEND_LOCAL);
   RinsebackRequestMsg            rinseback(MessageBase::SEND_GLOBAL);
   Alarm_response_struct          alarm_set_struct;

   DataLog (log_level_gui_info) << "Screen_ABORTCNF, processing rinseback button" << endmsg;

   // Disable the buttons on this screen
   removeButtonCallbacks();

   // We're going in to rinseback, we don't need to bring up the predict screen
   // again, so clear the flag (IT 13748)
   PredictManager::clear_prediction_screen_requested();

   // Get the type of abort that invoked this screen
   abort_type = return_confirm_type (&alarm_id);

   // Switch on the abort type and send EXIT messages accordingly
   switch (abort_type)
   {
      case ABORT_RINSEBACK :
         // Send an indication to "Rinseback" requested by operator
         rinseback.send(0);

         // Dismiss myself and possibly all other stateless screens
         dismissScreen( (get_screen_invocation_type() == GUI_SCREEN_INVOKE_STATELESS) );
         break;

      case ABORT_ALARM_RINSEBACK :
         // Load the alarm abort structure
         alarm_set_struct.alarm_name     = (ALARM_VALUES)alarm_id;
         alarm_set_struct.alarm_response = ABORT_THRU_RINSEBACK;

         // Actually send the message
         Msg_Alarm_Set.send(alarm_set_struct);

         // Remove alarm status line color and message
         guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

         // Dismiss myself and the alarm screen
         dismissScreen(true);
         break;

      default :
         DataLog (log_level_gui_error) << "Invalid abort type " << abort_type << " for rinseback button." << endmsg;
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Invalid abort type requested");
         break;

   } // End of abort_type switch.


}   // END of PROCESS_RINSEBACK_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_DONOTEXIT_BUTTON
void Screen_ABORTCNF::process_donotexit_button ()
{
   // Disable the buttons on this screen
   removeButtonCallbacks();

   // Dismiss myself
   dismissScreen();
}


void Screen_ABORTCNF::process_partial_abort (PARTIAL_END_RUN_TYPE type)
{
   DataLog (log_level_gui_info) << __FUNCTION__ << "(): endType=" << type << endmsg;

   // Disable the buttons on this screen
   removeButtonCallbacks();

   // Send out the cancellation message
   PartialCancellationMsg partialCancelationMsg(MessageBase::SEND_LOCAL);
   partialCancelationMsg.send(type);

   // Get the type of abort that invoked this screen
   int  alarmNum = NULL_ALARM;
   bool isAlarm  = ( return_confirm_type(&alarmNum) == ABORT_ADDITIVE_SOLUTIONS_ALARM );

   // If this abort is due to an alarm, clear it now (IT-13721)
   if (isAlarm)
   {
      Alarm_response_struct          response = { (ALARM_VALUES)alarmNum, CONTINUE };
      Message<Alarm_response_struct> msg(MessageBase::SEND_GLOBAL);
      msg.send(response);

      guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);
   }

   // Remove myself and possibly the underlying alarm screen
   dismissScreen(isAlarm);
}

////////////////////////////////////////////////////////////////////////////////
// return_confirm_type
Screen_ABORTCNF::CONFIRM_CALL_TYPE Screen_ABORTCNF::return_confirm_type (int* alarm_number)
{
   CONFIRM_CALL_TYPE retcode = ABORT_NORMAL;

   // Determine what kind of ABORT request should be sent out,
   // using a copy of the allocation parameter.
   guistring allocstr(get_allocation_parameter());
   char*     alloc_ptr = allocstr.getBufPtr();

   // Scan for the Abort message type
   char* nextToken         = 0;
   char* type_ptr          = strtok_r(alloc_ptr, "~", &nextToken);
   char* local_integer_ptr = strtok_r(NULL, "~", &nextToken);

   if (type_ptr == NULL)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "Invalid abort request type token");
   }
   else if (strcmp(type_ptr, ABORT_TYPE_USER) == 0)
   {
      // Set the alarm to minus 1 (invalid alarm number) - no alarm should be involved here.
      *alarm_number = -1;

      // Return type of abort
      retcode = ABORT_NORMAL;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_NORMAL." << endmsg;
   }
   else if (strcmp(type_ptr, ABORT_TYPE_RINSEBACK) == 0)
   {
      // Set the alarm to minus 1 (invalid alarm number) - no alarm should be involved here.
      *alarm_number = -1;

      // Return type of abort
      retcode = ABORT_RINSEBACK;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_RINSEBACK." << endmsg;
   }
   else if (strcmp(type_ptr, ABORT_TYPE_ALARM) == 0)
   {
      // Pick out the alarm type from the allocation buffer for the alarm abort structure

      if (local_integer_ptr == NULL)  // Didn't have anticipated alarm type
      {
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid Alarm number passed");
         return ABORT_NORMAL; // to appease static analysis
      }

      // Set the caller's alarm number buffer with the alarm value.
      *alarm_number = atoi(local_integer_ptr) ;

      if ( (*alarm_number == MSS_PLT_SS_EMPTY) ||
           (*alarm_number == APS_TOO_HIGH_ALARM_MSS_WITH_PAS) )
      {
         retcode = ABORT_ADDITIVE_SOLUTIONS_ALARM;
         DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_ADDITIVE_SOLUTIONS_ALARM (alarmNum=" << *alarm_number << ")." << endmsg;
      }
      else
      {
         // Return type of abort
         retcode = ABORT_ALARM;
         DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_ALARM (alarmNum=" << *alarm_number << ")." << endmsg;
      }
   }
   else if (strcmp(type_ptr, ABORT_TYPE_ALARMRINSE) == 0)
   {
      if (local_integer_ptr == NULL)  // Didn't have anticipated alarm type
      {
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid Rinse Alarm number passed");
         return ABORT_NORMAL; // to appease static analysis
      }

      // Set the caller's alarm number buffer with the alarm value.
      *alarm_number = atoi(local_integer_ptr);

      // Return type of abort
      retcode = ABORT_ALARM_RINSEBACK;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_ALARM_RINSEBACK (alarmNum=" << *alarm_number << ")." << endmsg;
   }
   else if (strcmp(type_ptr, CONFIRM_TYPE_SPILLOVER) == 0)
   {
      // Set the alarm to minus 1 (invalid alarm number) - no alarm should be involved here.
      *alarm_number = -1;

      // Return type of abort
      retcode = CONFIRM_SPILLOVER;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type CONFIRM_SPILLOVER." << endmsg;
   }
   else if (strcmp(type_ptr, CONFIRM_TYPE_AIR_IN_PLASMA) == 0)
   {
      // Set the alarm to minus 1 (invalid alarm number) - no alarm should be involved here.
      *alarm_number = -1;

      // Return type of abort
      retcode = CONFIRM_AIR_IN_PLASMA;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type CONFIRM_AIR_IN_PLASMA." << endmsg;
   }
   else if (strcmp(type_ptr, ABORT_TYPE_ADDITIVE_SOLUTIONS) == 0)
   {
      // Set the alarm to minus 1 (invalid alarm number) - no alarm should be involved here.
      *alarm_number = -1;

      // Return type of abort
      retcode = ABORT_ADDITIVE_SOLUTIONS;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_ADDITIVE_SOLUTIONS." << endmsg;
   }
   else if (strcmp(type_ptr, ABORT_TYPE_ADDITIVE_SOLUTIONS_ALARM) == 0)
   {
      if (local_integer_ptr == NULL)  // Didn't have anticipated alarm type
      {
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid AAS Alarm number passed");
         return ABORT_NORMAL; // to appease static analysis
      }

      // Set the caller's alarm number buffer with the alarm value.
      *alarm_number = atoi(local_integer_ptr);

      // Return type of abort
      retcode = ABORT_ADDITIVE_SOLUTIONS_ALARM;

      DataLog (log_level_gui_info) << "Abort screen parsed abort type ABORT_ADDITIVE_SOLUTIONS_ALARM (alarmNum=" << *alarm_number << ")." << endmsg;
   }
   else
   {
      // If we got here, an invalid abort type was sent to ABORTCNF.
      anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
      alarm.setAlarm("Unknown abort type requested");
   }

   return retcode;

}   // END of Screen_ABORTCNF return_confirm_type


//
//
//  This method clears the spillover pause alarm
//
void Screen_ABORTCNF::clear_spillover_pause ()
{
   Alarm_response_struct response;
   response.alarm_name     = PAUSE_BEFORE_SPILLOVER;
   response.alarm_response = CONTINUE;

   Message<Alarm_response_struct> msg(MessageBase::SEND_GLOBAL);
   msg.send(response);
}



////////////////////////////////////////////////////////////////////////////////
// REFRESH_RESOURCES
void Screen_ABORTCNF::refresh_resources ()
{

   if (dismiss_flag == TRUE)
   {
      // Since we've seen stateless screens dismissed while we were allocated,
      // we are being uncovered and we'll leave too.
      process_donotexit_button ();
   }
   else
   {
      // If this isn't a preemptory dismissal of the abortcnf screen, we'd better
      // do our base class' refresh.
      Base_Apheresis_Window::refresh_resources ();
   }

}


void Screen_ABORTCNF::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(END_RUN_STATUS_LINE, textEmptyFormatCfgNumericTextFormatNull);
}


////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_ABORTCNF::screen_notify_handler ()
{
   dismiss_flag = TRUE;
}


Screen_ABORTCNF::AAS_ABORT_ALARM_TYPE Screen_ABORTCNF::getAlarmAasCorrelation (int alarmId)
{
   DataLog (log_level_gui_info) << "getAlarmAasCorrelation(" << alarmId << ")" << endmsg;


   if ( (alarmId <= FIRST_PROCEDURE_ALARM) ||
        (alarmId >= LAST_FATAL_ERROR_ALARM) )
   {
      DataLog (log_level_gui_info) << "Abort decided alarmId(" << alarmId
                                   << ") is unrelated to an AAS product." << endmsg;
      return AAS_ABORT_ALARM_NOT_ACTUALLY_AN_ALARM;
   }
   else
   {
      switch ((ALARM_VALUES)alarmId)
      {
         case CPS_HIGH_DURING_RBC_COLLECT_ALARM :
         case CPS_HIGH_DURING_RBC_COLLECT_ALERT :
         case CPS_PTF_BASELINE_TEST_ALARM :
         case CPS_PTF_BASELINE_TEST_ALERT :
         case CPS_TOO_HIGH_ALARM :
         case CPS_TOO_HIGH_ALERT_NO_CLAMP :
         case CPS_TOO_HIGH_ALERT_ONE_CLAMP :
         case CPS_TOO_HIGH_ALERT_THREE_CLAMP :
         case CPS_TOO_HIGH_ALERT_THREE_CLAMP_PTF :
         case CPS_HIGH_DURING_RBC_COLLECT_PROD2_OK_ALERT :
         case AC_NOT_REMOVED_ALERT :
         case APS_TOO_HIGH_ALERT_MSS :
         case APS_TOO_HIGH_ALARM_MSS_WITH_PAS :
         case APS_TOO_HIGH_ALARM_MSS :
         case MSS_PRIME_DELAY_RBC_ALERT :
         case MSS_PRIME_TOO_SOON_RBC_ALARM :
         case MSS_RBC_SS_EMPTY :
         case MSS_DRBC_SS_EMPTY :

            DataLog (log_level_gui_info) << "Abort decided alarmId(" << alarmId
                                         << ") is related to a RAS product." << endmsg;
            return AAS_ABORT_ALARM_RAS_RELATED;

         case RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP :
         case MSS_PRIME_DELAY_PLT_ALERT :
         case MSS_PRIME_TOO_SOON_PLT_ALERT :
         case MSS_PLT_SS_EMPTY :
         case MSS_MISCONNECTED_2ND_PAS_BAG :
         case CHANNEL_CLAMP_NOT_CLOSED_ADDITION_ALERT : // This version only gets set for PAS, so I think it makes sense here...

            DataLog (log_level_gui_info) << "Abort decided alarmId(" << alarmId
                                         << ") is related to a PAS product." << endmsg;
            return AAS_ABORT_ALARM_PAS_RELATED;

         default :
            DataLog (log_level_gui_info) << "Abort decided alarmId(" << alarmId
                                         << ") is NOT related to a specific AAS product." << endmsg;
            return AAS_ABORT_ALARM_NOT_AAS_RELATED;
      }
   }
}


#if (CPU==SIMNT)  // These functions are simulator-only, and used for unit testing and screenshots.
void Screen_ABORTCNF::cleanSlate ()
{  // todo: after IT 11725, this will likely need some work.  This is a simulator-only function, and I haven't looked into who uses it or for what purpose.
   text_abort_confirm.deallocate_resources ();

   OSRegion btnInitRegion;
   btnInitRegion.y = BUTTON_Y_COORD;
   btnInitRegion.x = TWO_BUTTON_MARGIN;

   _btnExit.set_region(btnInitRegion);

   btnInitRegion.x += TWO_BUTTON_DELTA;

   _btnDontExit.set_region(btnInitRegion);

   btnInitRegion.x = THREE_BUTTON_MARGIN + THREE_BUTTON_DELTA;

   _btnAbortPas.set_region(btnInitRegion);
   _btnAbortRas.set_region(btnInitRegion);

   _btnExit.deallocate_resources ();
   _btnDontExit.deallocate_resources ();
   _btnAbortRas.deallocate_resources();
   _btnAbortPas.deallocate_resources();
}

void Screen_ABORTCNF::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      ABORT_CONFIRM_DISPLAY_TYPE abortType = (ABORT_CONFIRM_DISPLAY_TYPE)traverseInfo.parameters[0];

      DataLog (log_level_sim_gui_side_info) << "Entering Screen_ABORTCNF::handleTraverseMessage("
                                            << abortType << ");" << endmsg;

      cleanSlate();

      int               alarmId            = -1;
      CONFIRM_CALL_TYPE confirmCallType    = ABORT_NORMAL;
      bool              isBothRasPasAndRas = false;

      switch (abortType)
      {
         case ABORT_CONFIRM_NORMAL :
            break;

         case ABORT_CONFIRM_AAS_NORMAL :
            confirmCallType = ABORT_ADDITIVE_SOLUTIONS;
            break;

         case ABORT_CONFIRM_AAS_WITH_END_RAS :
            alarmId            = CPS_TOO_HIGH_ALARM;
            confirmCallType    = ABORT_ADDITIVE_SOLUTIONS_ALARM;
            isBothRasPasAndRas = true;
            break;

         case ABORT_CONFIRM_AAS_WITH_END_PAS :
            alarmId            = RESERVOIR_RETURN_TOO_LONG_ALARM_THREE_CLAMP;
            confirmCallType    = ABORT_ADDITIVE_SOLUTIONS_ALARM;
            isBothRasPasAndRas = true;
            break;

         case ABORT_CONFIRM_AAS_WITH_END_ANY :
            confirmCallType    = ABORT_ADDITIVE_SOLUTIONS;
            isBothRasPasAndRas = true;
            break;

         case ABORT_CONFIRM_RINSEBACK :
            confirmCallType = ABORT_RINSEBACK;
            break;

         case ABORT_CONFIRM_SPILLOVER :
            confirmCallType = CONFIRM_SPILLOVER;
            break;

         case ABORT_CONFIRM_AIR_IN_PLASMA :
            confirmCallType = CONFIRM_AIR_IN_PLASMA;
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Abort Confirm Display Type: " << abortType << endmsg;
            break;
      }

      composeTextAndGraphics(alarmId, confirmCallType, isBothRasPasAndRas);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH a02105cad3e51d13da92765ea695ab8f */
