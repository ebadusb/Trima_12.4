/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2003 Gambro BCT, Inc. All rights reserved

*     Class name:  Screen_CFR11

*     File name:   CFR11_screen.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>


// BASE CLASS INCLUDE
#include "CFR11_screen.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES
#include "guiglobs.hpp"



static void cb_enter_later_button (void* data);
static void cb_clear_badge_button (void* data);

static void cb_up_button (void* data);
static void cb_down_button (void* data);

static void cb_list_button_1 (void* data);
static void cb_list_button_2 (void* data);
static void cb_list_button_3 (void* data);
static void cb_list_button_4 (void* data);

const int UP_BUTTON_X       = 215;
const int UP_BUTTON_Y       = 215;

const int DOWN_BUTTON_X     = 215;
const int DOWN_BUTTON_Y     = 310;

const int UP_BUTTON_Y_ALT   = 185;

const int DOWN_BUTTON_Y_ALT = 385;

// Security Calc~Max Val~Min Val~X Coord~Y Coord~Units~Format String
const char KEYPAD_CONFIG[] = "11~999999~0~224~180~~######";

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_CFR11::Screen_CFR11()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_CFR11),

     operator_id_label(textMiscCfrOpIdLabel),
     operator_badge_id(textEmptyFormatCfrOpId),
     text_continue_button_disabled(textButtonAlarmDisabledContinue),
     text_time_of_day(textEmptyFormatCfrTimeOfDay),

     operator_id_cartouche(70,
                           135,
                           BITMAP_CFR11_OP_ID_CARTOUCHE),

     list_cartouche(59,
                    180,
                    BITMAP_CFR11_LIST_CARTOUCHE),


     enter_later_button(GUI_BUTTON_CFR_SELECTED,
                        66,
                        385,
                        textButtonLongCfrEnterLater,
                        BITMAP_CFR11_BUTTON_ACCEPT_UP,
                        BITMAP_CFR11_BUTTON_ACCEPT_DN),

     clear_badge_button(GUI_BUTTON_CFR_CLEAR_BADGE,
                        477,
                        136,
                        textButtonCfrClear,
                        BITMAP_CFR11_BUTTON_CLEAR_UP,
                        BITMAP_CFR11_BUTTON_CLEAR_DN),

     up_button(NULL),
     grey_up_button(NULL),
     down_button(NULL),
     grey_down_button(NULL),

     current_selection(NULL),
     list_btn_1(NULL),
     list_btn_2(NULL),
     list_btn_3(NULL),
     list_btn_4(NULL),

     _statusLine(textEmpty),
     _statusLineType(NULL_STATUS_LINE),

     _badgeBarcodeMsg(Callback<Screen_CFR11>        (this, &Screen_CFR11::HandleBadgeBarcodeMsg),         MessageBase::SNDRCV_RECEIVE_ONLY),
     _keypadMsg(Callback<Screen_CFR11>              (this, &Screen_CFR11::HandleKeypadMsg),               MessageBase::SNDRCV_RECEIVE_ONLY),
     _pinSignatureResponseMsg(Callback<Screen_CFR11>(this, &Screen_CFR11::HandlePinSignatureResponseMsg), MessageBase::SNDRCV_RECEIVE_ONLY)

{
   _badge.erase();
}   // END of Screen_CFR11 CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CFR11::~Screen_CFR11()
{
   // If we haven't already deallocated, do it now.
   if (_allocated)
   {
      // deallocate_resources is virtual, so be sure to call the right one!
      Screen_CFR11::deallocate_resources();
   }
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_CFR11::allocate_resources (const char* allocation_parameter)
{
   // Register for the alarm state change message
   _alarmStateChangeMsg.init(Callback<Screen_CFR11>(this, &Screen_CFR11::HandleAlarmStateChangeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   guiglobs::current_displayed_alarm = CFR11_TRACKING_ALARM;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // Allocate the bitmaps
   list_cartouche.allocate_resources (*this);
   operator_id_cartouche.allocate_resources (*this);

   // Allocate the operator id label
   operator_id_label.allocate_resources (*this);
   operator_badge_id.allocate_resources (*this);

   if (!_badge.empty())
   {
      operator_badge_id.set_text_direct(_badge.size(), _badge.c_str());
   }

   // Allocate the buttons
   enter_later_button.allocate_resources (*this);
   clear_badge_button.allocate_resources (*this);

   time_of_day_display();

   text_time_of_day.allocate_resources(*this);

   if (!IsPostRun())
   {
      up_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_UP,
                                       UP_BUTTON_X,
                                       UP_BUTTON_Y,
                                       TEXT_NULL,
                                       BITMAP_CFR11_BUTTON_SCROLL_UP,
                                       BITMAP_CFR11_BUTTON_SCROLL_PUSH);

      grey_up_button = new Bitmap (UP_BUTTON_X,
                                   UP_BUTTON_Y,
                                   BITMAP_CFR11_BUTTON_SCROLL_UP_GREY);

      down_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_DN,
                                         DOWN_BUTTON_X,
                                         DOWN_BUTTON_Y,
                                         TEXT_NULL,
                                         BITMAP_CFR11_BUTTON_SCROLL_DN,
                                         BITMAP_CFR11_BUTTON_SCROLL_PUSH);


      grey_down_button = new Bitmap (DOWN_BUTTON_X,
                                     DOWN_BUTTON_Y,
                                     BITMAP_CFR11_BUTTON_SCROLL_DN_GREY);

      list_btn_1 = new Discrete_Button (GUI_BUTTON_CFR_LIST_1,
                                        70,
                                        263,
                                        TEXT_NULL,
                                        BITMAP_CFR11_CURRENT_SELECTION,
                                        BITMAP_CFR11_CURRENT_SELECTION);

      list_btn_1->allocate_resources(*this);
      list_btn_1->set_callback(cb_list_button_1, (void*)this);
   }
   else
   {
      up_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_UP,
                                       UP_BUTTON_X,
                                       UP_BUTTON_Y_ALT,
                                       TEXT_NULL,
                                       BITMAP_CFR11_BUTTON_SCROLL_UP,
                                       BITMAP_CFR11_BUTTON_SCROLL_PUSH);

      grey_up_button = new Bitmap (UP_BUTTON_X,
                                   UP_BUTTON_Y_ALT,
                                   BITMAP_CFR11_BUTTON_SCROLL_UP_GREY);

      down_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_DN,
                                         DOWN_BUTTON_X,
                                         DOWN_BUTTON_Y_ALT,
                                         TEXT_NULL,
                                         BITMAP_CFR11_BUTTON_SCROLL_DN,
                                         BITMAP_CFR11_BUTTON_SCROLL_PUSH);


      grey_down_button = new Bitmap (DOWN_BUTTON_X,
                                     DOWN_BUTTON_Y_ALT,
                                     BITMAP_CFR11_BUTTON_SCROLL_DN_GREY);

      allocate_list_buttons();
   }

   grey_up_button->allocate_resources (*this);
   grey_down_button->allocate_resources (*this);

   //
   // Set button iterator to end of list and set end of list iterator.
   //
   list_iter = guiglobs::cfr_list.end();
   end_iter  = --list_iter;

   refresh_list();

   // Link the buttons to their "C" callback stubs
   enter_later_button.set_callback (cb_enter_later_button, (void*)this);
   clear_badge_button.set_callback (cb_clear_badge_button, (void*)this);

   // Deallocate "standard" buttons that are not needed.
   btn_goback.deallocate_resources();
   btn_cancel.deallocate_resources();
   btn_lowleft.deallocate_resources();

   // Change text on continue button to "accept"
   btn_continue.set_string_id (textButtonCfrSelected);

   // Link the status bar/line to this window
   link_status_bar_line();

   _statusLine     = textStatAuthEvents;
   _statusLineType = DEFAULT_STATUS_LINE;

   // send the alarm confirm message after deallocation is complete
   Alarm_response_struct alarm_set_struct;
   alarm_set_struct.alarm_name     = CFR11_TRACKING_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   Message<Alarm_response_struct> alarmRspns(MessageBase::SEND_LOCAL);
   alarmRspns.send(alarm_set_struct);

}  // END of Screen_CFR11 ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CFR11::deallocate_resources ()
{
   // Deallocate the bitmaps
   list_cartouche.deallocate_resources();
   operator_id_cartouche.deallocate_resources();

   // Deallocate the display_text objects
   operator_id_label.deallocate_resources();
   operator_badge_id.deallocate_resources();
   text_continue_button_disabled.deallocate_resources();
   text_time_of_day.deallocate_resources();

   // Deallocate the buttons
   enter_later_button.deallocate_resources();
   clear_badge_button.deallocate_resources();

   delete grey_up_button;
   grey_up_button = NULL;

   delete grey_down_button;
   grey_down_button = NULL;

   delete current_selection;
   current_selection = NULL;

   delete list_btn_1;
   list_btn_1 = NULL;

   delete list_btn_2;
   list_btn_2 = NULL;

   delete list_btn_3;
   list_btn_3 = NULL;

   delete list_btn_4;
   list_btn_4 = NULL;

   delete up_button;
   up_button = NULL;

   delete down_button;
   down_button = NULL;

   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources();

   // Deallocate message handlers
   _badgeBarcodeMsg.deregisterMsg();
   _keypadMsg.deregisterMsg();
   _pinSignatureResponseMsg.deregisterMsg();
   _alarmStateChangeMsg.deregisterMsg();

   guiglobs::current_displayed_alarm = NULL_ALARM;

   // send the alarm confirm message after deallocation is complete
   Alarm_response_struct alarm_set_struct;
   alarm_set_struct.alarm_name     = NULL_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   Message<Alarm_response_struct> alarmRspns(MessageBase::SEND_LOCAL);
   alarmRspns.send(alarm_set_struct);

}   // END of Screen_CFR11 DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_LIST_BUTTONS
void Screen_CFR11::allocate_list_buttons ()
{
   delete list_btn_1;
   list_btn_1 = NULL;

   delete list_btn_2;
   list_btn_2 = NULL;

   delete list_btn_3;
   list_btn_3 = NULL;

   delete list_btn_4;
   list_btn_4 = NULL;

   if (guiglobs::cfr_list.size() >= 1)
   {
      list_btn_1 = new Discrete_Button(GUI_BUTTON_CFR_LIST_1,
                                       70,
                                       220,
                                       TEXT_NULL,
                                       BITMAP_CFR11_BUTTON_LIST_UP,
                                       BITMAP_CFR11_BUTTON_LIST_DN);

      list_btn_1->allocate_resources(*this);
      list_btn_1->set_callback(cb_list_button_1, (void*)this);
   }

   if (guiglobs::cfr_list.size() >= 2)
   {
      list_btn_2 = new Discrete_Button(GUI_BUTTON_CFR_LIST_2,
                                       70,
                                       260,
                                       TEXT_NULL,
                                       BITMAP_CFR11_BUTTON_LIST_UP,
                                       BITMAP_CFR11_BUTTON_LIST_DN);


      list_btn_2->allocate_resources(*this);
      list_btn_2->set_callback(cb_list_button_2, (void*)this);
   }

   if (guiglobs::cfr_list.size() >= 3)
   {
      list_btn_3 = new Discrete_Button(GUI_BUTTON_CFR_LIST_3,
                                       70,
                                       300,
                                       TEXT_NULL,
                                       BITMAP_CFR11_BUTTON_LIST_UP,
                                       BITMAP_CFR11_BUTTON_LIST_DN);


      list_btn_3->allocate_resources(*this);
      list_btn_3->set_callback(cb_list_button_3, (void*)this);
   }

   if (guiglobs::cfr_list.size() >= 4)
   {
      list_btn_4 = new Discrete_Button (GUI_BUTTON_CFR_LIST_4,
                                        70,
                                        340,
                                        TEXT_NULL,
                                        BITMAP_CFR11_BUTTON_LIST_UP,
                                        BITMAP_CFR11_BUTTON_LIST_DN);

      list_btn_4->allocate_resources(*this);
      list_btn_4->set_callback(cb_list_button_4, (void*)this);
   }

}  // END of Screen_CFR11 ALLOCATE_LIST_BUTTONS

////////////////////////////////////////////////////////////////////////////////
// REFRESH_LIST
void Screen_CFR11::refresh_list ()
{
   guistring          list_string;

   CFR_LIST::iterator current_iter = list_iter;

   if (list_btn_4 != NULL)
   {
      btn_four_iter = current_iter--;
   }
   if (list_btn_3 != NULL)
   {
      btn_three_iter = current_iter--;
   }
   if (list_btn_2 != NULL)
   {
      btn_two_iter = current_iter--;
   }
   if (list_btn_1 != NULL)
   {
      btn_one_iter = current_iter;
   }

   delete up_button;
   up_button = NULL;

   delete down_button;
   down_button = NULL;

   const int LIST_SIZE = guiglobs::cfr_list.size();

   if (!IsPostRun())
   {
      //
      // Do we need the up arrow button?
      //
      if ((LIST_SIZE > MAX_NUMBER_OF_BUTTONS) &&
          (list_iter != guiglobs::cfr_list.begin())
          )
      {
         up_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_UP,
                                          UP_BUTTON_X,
                                          UP_BUTTON_Y,
                                          TEXT_NULL,
                                          BITMAP_CFR11_BUTTON_SCROLL_UP,
                                          BITMAP_CFR11_BUTTON_SCROLL_PUSH);

         up_button->allocate_resources(*this);
         up_button->set_callback (cb_up_button, (void*)this);
      }

      //
      // Do we need the down arrow button?
      //
      if ((LIST_SIZE > MAX_NUMBER_OF_BUTTONS) &&
          (list_iter != end_iter)
          )
      {
         down_button = new Discrete_Button (GUI_BUTTON_CFR_SCROLL_DN,
                                            DOWN_BUTTON_X,
                                            DOWN_BUTTON_Y,
                                            TEXT_NULL,
                                            BITMAP_CFR11_BUTTON_SCROLL_DN,
                                            BITMAP_CFR11_BUTTON_SCROLL_PUSH);

         down_button->allocate_resources(*this);
         down_button->set_callback(cb_down_button, (void*)this);
      }


      // I'm pretty sure this can't happen, but the reasons *why* it can't
      // happen are deeply obscure... so a little defensive coding is in order.
      if (!list_btn_1)
      {
         DataLog (log_level_gui_cfr11_info) << "ERROR: list_btn_1 is NULL" << endmsg;
         _FATAL_ERROR (__FILE__, __LINE__, "No action entry for a valid state entry");
      }

      //
      // Set the state of the button
      //
      if (LIST_SIZE == 0)
      {
         list_btn_1->setInactive();
         ExitScreen();
      }
      else if ( list_iter != guiglobs::cfr_list.end() )
      {
         CFR11_Tracking_List& item = *list_iter;

#ifdef SIMNT
         list_btn_1->set_string_id(textMiscCfrActionAcReactionUp);
#endif
         list_btn_1->set_text_direct(item.btn_text.length(), item.btn_text);

         // Clear any previous selections.
         // We may have more than one if, while in this screen, another alarm comes up.
         ClearSelections();

         // The current list_iter is always selected.
         list_btn_1->setActive();
         item.selected = true;

         // If we just confirmed donor vitals using Vista in the STARTUP_TESTS state,
         // then make sure all unconfirmed donor vitals items are selected.
         if (guiglobs::screen_status.get_current_system_state() == STARTUP_TESTS &&
             (item.trigger == TRIGGER_DONOR_VITALS_A_CHANGE ||
              item.trigger == TRIGGER_DONOR_VITALS_B_CHANGE))
         {
            for (CFR_LIST::iterator dvCheckIter = guiglobs::cfr_list.begin();
                 dvCheckIter != guiglobs::cfr_list.end(); dvCheckIter++)
            {
               if ( !dvCheckIter->selected &&
                    ( dvCheckIter->trigger == TRIGGER_DONOR_VITALS_A_CHANGE ||
                      dvCheckIter->trigger == TRIGGER_DONOR_VITALS_B_CHANGE)
                    )
               {
                  dvCheckIter->selected = true;
                  DataLog (log_level_gui_cfr11_info) << "Set item with key "
                                                     << dvCheckIter->key << " to selected." << endmsg;
               }
            }

            DataLog (log_level_gui_cfr11_info) << "Checked all donor confirm keys." << endmsg;
            dumpCfrList(__FILE__, __LINE__);
         }
      }
   }
   else  // Post Run Screen
   {
      // If list is empty, then exit screen
      if (LIST_SIZE == 0)
      {
         ExitScreen();
         return;
      }

      //
      // Do we need the up arrow button?
      //
      if ((LIST_SIZE > MAX_NUMBER_OF_BUTTONS) &&
          (btn_one_iter != guiglobs::cfr_list.begin()))
      {
         up_button = new Button_Bitmap (GUI_BUTTON_CFR_SCROLL_UP,
                                        215,
                                        185,
                                        TEXT_NULL,
                                        BITMAP_CFR11_BUTTON_SCROLL_UP,
                                        BITMAP_CFR11_BUTTON_SCROLL_PUSH);
         up_button->allocate_resources(*this);
         up_button->set_callback (cb_up_button, (void*)this);
      }

      //
      // Do we need the down arrow button?
      //
      if ((LIST_SIZE > MAX_NUMBER_OF_BUTTONS) && (btn_four_iter != end_iter))
      {
         down_button = new Button_Bitmap(GUI_BUTTON_CFR_SCROLL_DN,
                                         215,
                                         385,
                                         TEXT_NULL,
                                         BITMAP_CFR11_BUTTON_SCROLL_DN,
                                         BITMAP_CFR11_BUTTON_SCROLL_PUSH);

         down_button->allocate_resources(*this);
         down_button->set_callback(cb_down_button, (void*)this);
      }


      if ((!guiglobs::cfr_list.empty()) &&
          (btn_one_iter != guiglobs::cfr_list.end()) &&
          (list_btn_1 != NULL))
      {
         list_string = (*btn_one_iter).btn_text;

         list_btn_1->set_text_direct(strlen(list_string), list_string);

         if ((*btn_one_iter).selected)
         {
            list_btn_1->setActive();

         }
         else
         {
            list_btn_1->setInactive();
         }
      }

      if ((!guiglobs::cfr_list.empty()) &&
          (btn_two_iter != guiglobs::cfr_list.end()) &&
          (list_btn_2 != NULL))
      {
         list_string = (*btn_two_iter).btn_text;

         list_btn_2->set_text_direct(strlen(list_string), list_string);

         if ((*btn_two_iter).selected)
         {
            list_btn_2->setActive();

         }
         else
         {
            list_btn_2->setInactive();
         }
      }

      if ((!guiglobs::cfr_list.empty()) &&
          (btn_three_iter != guiglobs::cfr_list.end()) &&
          (list_btn_3 != NULL))
      {
         list_string = (*btn_three_iter).btn_text;

         list_btn_3->set_text_direct(strlen(list_string), list_string);

         if ((*btn_three_iter).selected)
         {
            list_btn_3->setActive();

         }
         else
         {
            list_btn_3->setInactive();
         }
      }

      if ((!guiglobs::cfr_list.empty()) &&
          (btn_four_iter != guiglobs::cfr_list.end()) &&
          (list_btn_4 != NULL))
      {
         list_string = (*btn_four_iter).btn_text;

         list_btn_4->set_text_direct(strlen(list_string), list_string);

         if ((*btn_four_iter).selected)
         {
            list_btn_4->setActive();

         }
         else
         {
            list_btn_4->setInactive();
         }
      }

   }
}   // END of Screen_CFR11 REFRESH_LIST



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_CFR11::process_continue_button ()
{
   // Enforce screen workflow.
   // 1.  Must have booped badge.
   // 2.  Must have selected item(s).

   // Check for badge boop.
   if (_badge.empty())
   {
      _statusLine     = textStatMissingBadgeBoop;
      _statusLineType = ATTENTION_STATUS_LINE;

      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

      return;
   }

   // Check for item selection
   if (NoEventSelected())
   {
      _statusLine     = textStatNoItemSelected;
      _statusLineType = ATTENTION_STATUS_LINE;

      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

      return;
   }

   DisableButtons(true);

   // Bring up the keypad to get the pin.
   _dockedKeypad.allocate_resources(*this, KEYPAD_CONFIG);

   _statusLine     = textStatEnterPin;
   _statusLineType = DEFAULT_STATUS_LINE;

   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

   // Thread of control goes to keypad handler.....

}   // End of PROCESS_CONTINUE_BUTTON



////////////////////////////////////////////////////////////////////////////////
// CB_CLEAR_BADGE_BUTTON
void cb_clear_badge_button (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data)->process_clear_badge_button();
}   // END of cb_clear_badge_button



////////////////////////////////////////////////////////////////////////////////
// CB_ENTER_LATER_BUTTON
void cb_enter_later_button (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data)->process_enter_later_button();
}   // END of cb_enter_later_button



//
// Exit the screen
//
void Screen_CFR11::ExitScreen ()
{
   // Clear the alarm with the alarm manager
   Message<Alarm_response_struct> clearAlarmMsg(MessageBase::SEND_GLOBAL);
   Alarm_response_struct          clearAlarm;

   // Remove this alarm screen
   RemoveAllStatelessScreensMsg removeall(MessageBase::SEND_LOCAL);
   removeall.send(0);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   clearAlarm.alarm_name     = CFR11_TRACKING_ALARM;
   clearAlarm.alarm_response = CONTINUE;

   clearAlarmMsg.send(clearAlarm);

   // Clear any message in the status line
   _statusLine     = textEmpty;
   _statusLineType = DEFAULT_STATUS_LINE;

   guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);

   // Unselect all buttons
   ClearSelections();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_ENTER_LATER_BUTTON
void Screen_CFR11::process_enter_later_button ()
{
   ExitScreen();
}   // End of PROCESS_ENTER_LATER_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLEAR_BADGE_BUTTON
void Screen_CFR11::process_clear_badge_button ()
{
   // Clear text
   operator_badge_id.set_text_direct(1, " ");
   _badge.erase();
}   // End of PROCESS_SELECTED_BUTTON


////////////////////////////////////////////////////////////////////////////////
// CB_UP_BUTTON
void cb_up_button (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data)->process_up_button();

}   // END of cb_up_bUtton

////////////////////////////////////////////////////////////////////////////////
// PROCESS_UP_BUTTON
void Screen_CFR11::process_up_button ()
{
   // Unselect the current item.
   if (!IsPostRun())
   {
      (*list_iter).selected = false;
   }

   --list_iter;
   refresh_list();

}   // End of PROCESS_UP_BUTTON

////////////////////////////////////////////////////////////////////////////////
// CB_DOWN_BUTTON
void cb_down_button (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data)->process_down_button();
}   // END of cb_down_button

////////////////////////////////////////////////////////////////////////////////
// PROCESS_DOWN_BUTTON
void Screen_CFR11::process_down_button ()
{
   // Unselect the current item.

   if (!IsPostRun())
   {
      (*list_iter).selected = false;
   }

   ++list_iter;
   refresh_list();
}   // End of PROCESS_DOWN_BUTTON

////////////////////////////////////////////////////////////////////////////////
// CB_LIST_BUTTON_1
void cb_list_button_1 (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data)->process_list_btn_1();
}   // END of cb_list_button_1

////////////////////////////////////////////////////////////////////////////////
// CB_LIST_BUTTON_2
void cb_list_button_2 (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data) -> process_list_btn_2();

}   // END of cb_list_button_2

////////////////////////////////////////////////////////////////////////////////
// CB_LIST_BUTTON_3
void cb_list_button_3 (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data) -> process_list_btn_3();

}   // END of cb_list_button_3

////////////////////////////////////////////////////////////////////////////////
// CB_LIST_BUTTON_4
void cb_list_button_4 (void* data)
{
   // Call the class method on callback
   ((Screen_CFR11*)data) -> process_list_btn_4();

}   // END of cb_list_button_4

////////////////////////////////////////////////////////////////////////////////
// PROCESS_LIST_BTN_1
void Screen_CFR11::process_list_btn_1 ()
{
   //
   // toggle selected flag for list element on list btn one.
   //
   if (IsPostRun())
   {
      (*btn_one_iter).selected = !(*btn_one_iter).selected;
   }
   else
   {
      (*list_iter).selected = !(*list_iter).selected;
   }

}   // End of PROCESS_LIST_BTN_1

////////////////////////////////////////////////////////////////////////////////
// PROCESS_LIST_BTN_2
void Screen_CFR11::process_list_btn_2 ()
{
   //
   // toggle selected flag for list element on list btn two.
   //
   (*btn_two_iter).selected = !(*btn_two_iter).selected;

}   // End of PROCESS_LIST_BTN_2

////////////////////////////////////////////////////////////////////////////////
// PROCESS_LIST_BTN_3
void Screen_CFR11::process_list_btn_3 ()
{
   //
   // toggle selected flag for list element on list btn three.
   //
   (*btn_three_iter).selected = !(*btn_three_iter).selected;

}   // End of PROCESS_LIST_BTN_3

////////////////////////////////////////////////////////////////////////////////
// PROCESS_LIST_BTN_4
void Screen_CFR11::process_list_btn_4 ()
{
   //
   // toggle selected flag for list element on list btn four.
   //
   (*btn_four_iter).selected = !(*btn_four_iter).selected;

}   // End of PROCESS_LIST_BTN_4

//
// This method puts the operator badge barcode in the textbox
//
void Screen_CFR11::HandleBadgeBarcodeMsg ()
{
   BADGE_BARCODE_MSG msg = _badgeBarcodeMsg.getData();

   // Terminate the string
   msg.badgeString[msg.badgeLength] = 0;

   DataLog(log_level_gui_cfr11_info) << "Got Badge Barcode " << msg.badgeString << "(" << msg.badgeLength << ")" << endmsg;

   operator_badge_id.set_text_direct(msg.badgeLength, msg.badgeString);

   // Save badge string
   _badge = msg.badgeString;
}

//
// Keypad Return
//
void Screen_CFR11::HandleKeypadMsg ()
{
   bool                       exitScreen = false;
   AuthorizationStatusType    bioStatus;

   const KEYPAD_RETURN_STRUCT msg = _keypadMsg.getData();

   _dockedKeypad.deallocate_resources();

   // the keypad is gone so remove the enter pin status line
   guiglobs::apheresis_status_line->deactivate_status_line_type (DEFAULT_STATUS_LINE);

   DisableButtons(false);

   // Validate the Pin.
   const BioSignaturesMgr::PinValidType pinStatus = guiglobs::bioMgr().PinValid(_badge.c_str(), msg.keypad_return_string);

   // Only continue if the PIN was valid.
   switch (pinStatus)
   {
      case BioSignaturesMgr::BIO_PIN_VALID :
         DataLog(log_level_gui_cfr11_info) << "good pin for badge " << _badge.c_str() << endmsg;

         _statusLine     = textStatIdVerified;
         _statusLineType = DEFAULT_STATUS_LINE;

         guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

         exitScreen = true;
         bioStatus  = BIO_AUTH_SUCCESS;
         break;

      case BioSignaturesMgr::BIO_PIN_INVALID :
         DataLog(log_level_gui_cfr11_info) << "bad pin for badge " << _badge.c_str() << endmsg;

         _statusLine     = textStatWaitingForVista;
         _statusLineType = ATTENTION_STATUS_LINE;

         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

         bioStatus = BIO_AUTH_FAILED;
         break;

      case BioSignaturesMgr::BIO_NO_MATCH :
         DataLog(log_level_gui_cfr11_info) << "no match for badge " << _badge.c_str() << endmsg;

         _statusLine     = textStatWaitingForVista;
         _statusLineType = ATTENTION_STATUS_LINE;

         guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

         bioStatus = BIO_AUTH_UNKNOWN;
         break;

      default :
      {
         DataLog(log_level_gui_cfr11_error) << "Bad status " << pinStatus << " for badge " << _badge.c_str() << endmsg;
         anAlarmMsg alarmMsg(GUI_UNEXPECTED_CHOICE);
         alarmMsg.setAlarm("Bad PIN status");
         return;
      }
   }

   DataLog(log_level_gui_cfr11_info) << "updating " << guiglobs::cfr_list.size() << " items in GUI CFR List." << endmsg;

   CFR_LIST::iterator iter = guiglobs::cfr_list.begin();

   // Erase the selected items
   for (;; )
   {
      // Ensure that iter isn't null
      if ((iter == 0) || (iter == guiglobs::cfr_list.end()))
      {
         DataLog(log_level_gui_cfr11_info) << "exiting update loop" << endmsg;
         break;
      }

      DataLog(log_level_gui_cfr11_info) << "looking at RN " << (*iter).key
                                        << ", selected = " << (*iter).selected
                                        << ", status = " << (*iter).authStatus << endmsg;

      if ((*iter).selected)
      {
         DataLog(log_level_gui_cfr11_info) << "operator selected RN " << (*iter).key << endmsg;

         guiglobs::bioMgr().UpdateItem(_badge.c_str(), bioStatus, BIO_DEVICE_PIN, (*iter).key, msg.keypad_return_string);

         // Remove it from the list if the pin entry was successful
         switch (pinStatus)
         {
            case BioSignaturesMgr::BIO_PIN_VALID :
               DataLog(log_level_gui_cfr11_info) << "pin valid for RN " << (*iter).key << endmsg;
               DataLog(log_level_gui_cfr11_info) << "HandleKeypadMsg erased list item " << (*iter).key << endmsg;
               guiglobs::cfr_list.erase(iter);
               iter = guiglobs::cfr_list.begin();
               break;

            case BioSignaturesMgr::BIO_PIN_INVALID :
               DataLog(log_level_gui_cfr11_info) << "pin invalid for RN " << (*iter).key << endmsg;

               (*iter).authStatus = BIO_AUTH_FAILED;
               iter++;
               break;

            case BioSignaturesMgr::BIO_NO_MATCH :
               DataLog(log_level_gui_cfr11_info) << "no match/unknown for RN " << (*iter).key << endmsg;

               (*iter).authStatus = BIO_AUTH_UNKNOWN;
               iter++;
               break;

            default :
               DataLog(log_level_gui_cfr11_info) << "unexpected pin status " << pinStatus << " for RN " << (*iter).key << endmsg;
               iter++;
         }
      }
      else
      {
         iter++;
      }
   }

   if (IsPostRun())
   {
      allocate_list_buttons();
   }

   list_iter = guiglobs::cfr_list.end();
   end_iter  = --list_iter;

   // refresh the list.
   refresh_list();

   if (guiglobs::cfr_list.empty())
   {
      ExitScreen();
   }

   DataLog(log_level_gui_cfr11_info) << "End of handle keypad message" << endmsg;
   dumpCfrList(__FILE__, __LINE__);
}

//
// HandlePinSignatureResponseMsg
//
void Screen_CFR11::HandlePinSignatureResponseMsg ()
{
   bool                    foundItem = false;
   PinSignatureResponseMsg msg       = _pinSignatureResponseMsg.getData();
   AuthorizationStatusType status    = BIO_AUTH_NONE;

   DataLog(log_level_gui_cfr11_info) << "Got Pin Signature Response Msg (RN "
                                     << msg.scanSequenceNumber << ", " << msg.action << ", Badge " << msg.signature.badge
                                     << ", Pin " << msg.signature.pin << ")." << endmsg;

   DataLog(log_level_gui_cfr11_info) << "List size is " << guiglobs::cfr_list.size() << endmsg;

   // not waiting for Vista anymore so clear that from the status line
   guiglobs::apheresis_status_line->deactivate_status_line_text_type(textStatWaitingForVista, ATTENTION_STATUS_LINE);

   // Update the on-screen list.
   CFR_LIST::iterator iter = guiglobs::cfr_list.begin();

   while (iter != guiglobs::cfr_list.end())
   {
      DataLog(log_level_gui_cfr11_info) << "Checking item (RN " << (*iter).key << ", Selected " << (*iter).selected
                                        << ", " << (*iter).authStatus << ")" << endmsg;

      if ((*iter).key == msg.scanSequenceNumber)
      {
         DataLog(log_level_gui_cfr11_info) << "Found item (RN " << (*iter).key << ", Selected " << (*iter).selected
                                           << ", " << (*iter).authStatus << ")" << endmsg;

         foundItem = true;

         switch (msg.action)
         {
            case BIO_SIGNATURE_VALID_ADD :
               // Do special stuff for new badge then continue with BIO_SIGNATURE_VALID case
               // There isn't supposed to be a break; after this case

               DataLog(log_level_gui_cfr11_info) << "Badge " << msg.signature.badge << ", pin " << msg.signature.pin << " added. " << endmsg;

               // Add the new entry to the pin file.
               guiglobs::bioMgr().AddPin(msg.signature.badge, msg.signature.pin);

            case BIO_SIGNATURE_VALID :
               DataLog(log_level_gui_cfr11_info) << "RN " << msg.scanSequenceNumber << " erased. " << endmsg;

               _statusLine     = textStatIdVerified;
               _statusLineType = DEFAULT_STATUS_LINE;

               guiglobs::apheresis_status_line->deactivate_status_line_type (ATTENTION_STATUS_LINE);
               guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);

               // Erase the entry
               // If it is a donor info line, erase the other one too
               if (((*iter).trigger == TRIGGER_DONOR_VITALS_A_CHANGE ||
                    (*iter).trigger == TRIGGER_DONOR_VITALS_B_CHANGE))
               {
                  CFR_LIST::iterator dummyiter = guiglobs::cfr_list.begin();
                  while (dummyiter != guiglobs::cfr_list.end())
                  {
                     if (((*dummyiter).trigger == TRIGGER_DONOR_VITALS_A_CHANGE ||
                          (*dummyiter).trigger == TRIGGER_DONOR_VITALS_B_CHANGE))
                     {
                        guiglobs::cfr_list.erase(dummyiter);
                        DataLog (log_level_gui_cfr11_info) << "HandlePinSignatureResponseMsg erased list item " << (*dummyiter).key << endmsg;
                        dummyiter = guiglobs::cfr_list.begin();
                     }
                     else
                     {
                        dummyiter++;
                     }
                  }
               }
               else
               {
                  // Erase the entry
                  guiglobs::cfr_list.erase(iter);
                  DataLog (log_level_gui_cfr11_info) << "HandlePinSignatureResponseMsg erased list item " << (*iter).key << endmsg;
               }

               status = BIO_AUTH_SUCCESS;
               break;

            case BIO_SIGNATURE_INVALID :
               (*iter).authStatus = BIO_AUTH_FAILED;
               status             = BIO_AUTH_FAILED;

               DataLog(log_level_gui_cfr11_info) << "Badge " << msg.signature.badge << " invalid. " << endmsg;

               _statusLine     = textStatIdNotVerified;
               _statusLineType = ATTENTION_STATUS_LINE;

               guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);
               break;
         }
      }

      if (foundItem)
         break;
      else
         ++iter;
   }

   if (foundItem)
   {
      // Only update valid cases.
      // Invalid cases have already been logged.
      if (status != BIO_AUTH_FAILED)
      {
         guiglobs::bioMgr().UpdateItem(msg.signature.badge, status, BIO_DEVICE_PIN, msg.scanSequenceNumber, msg.signature.pin);
      }

      if (IsPostRun())
      {
         allocate_list_buttons();
      }

      list_iter = guiglobs::cfr_list.end();
      end_iter  = --list_iter;

      refresh_list();
   }
   else
   {
      DataLog(log_level_gui_cfr11_error) << "Badge " << msg.signature.badge
                                         << " sequence number " << msg.scanSequenceNumber << " not found." << endmsg;
   }

   DataLog(log_level_gui_cfr11_info) << "End of handle pin signature response message " << endmsg;
   dumpCfrList(__FILE__, __LINE__);
}



//
// NoEventSelected
// Returns true if no item in the list is selected
//
bool Screen_CFR11::NoEventSelected ()
{
   for (CFR_LIST::iterator iter = guiglobs::cfr_list.begin(); iter != guiglobs::cfr_list.end(); ++iter)
   {
      if ((*iter).selected)
      {
         return false;
      }
   }

   return true;
}



//
// IsEventSelected
// Returns true if an item in the list is selected
//
bool Screen_CFR11::IsEventSelected ()
{
   for (CFR_LIST::iterator iter = guiglobs::cfr_list.begin(); iter != guiglobs::cfr_list.end(); ++iter)
   {
      if ((*iter).selected)
      {
         return true;
      }
   }

   return false;
}



//
// IsPostRun
// Returns true if procedure has completed.
//
bool Screen_CFR11::IsPostRun ()
{
   return (guiglobs::screen_status.get_current_system_state() == POST_RUN);
}



//
// ClearSelections
// Unselects all items in list.
//
void Screen_CFR11::ClearSelections ()
{
   for (CFR_LIST::iterator iter = guiglobs::cfr_list.begin(); iter != guiglobs::cfr_list.end(); ++iter)
   {
      (*iter).selected = false;
   }
}



//
// DisableButtons
// Disables common buttons while authentication is taking place
//
void Screen_CFR11::DisableButtons (bool action)
{
   enter_later_button.set_disabled(action);
   clear_badge_button.set_disabled(action);
   btn_continue.set_disabled(action);

   list_btn_1->set_disabled(false);

   if (IsPostRun())
   {
      if (list_btn_2 != NULL)
         list_btn_2->set_disabled(false);

      if (list_btn_3 != NULL)
         list_btn_3->set_disabled(false);

      if (list_btn_4 != NULL)
         list_btn_4->set_disabled(false);
   }
}



//
// time_time_of_day_display
//
void Screen_CFR11::time_of_day_display ()
{
   static char time_of_day[20];
   memset(time_of_day, 0, sizeof(time_of_day));

   time_t       current_time = time(NULL);
   struct tm    local_tm;

   char         hours[3];
   char         minutes[3];
   char         seconds[3];

   Display_Text text;

   guistring    units_time_delimiter;
   guistring    units_time_am;
   guistring    units_time_pm;
   guistring    units_time; // To put it all together!

   //
   //  Set text elements for current_time.
   //
   text.set_string_id(textDataUnitsCfrAm);
   text.get_text_direct(units_time_am);

   text.set_string_id(textDataUnitsCfrPm);
   text.get_text_direct(units_time_pm);

   text.set_string_id(textMiscCfrTimeDelim);
   text.get_text_direct(units_time_delimiter);

   //
   //  Set the time elements for the current_time.
   //

   strftime(hours, 3, "%H", localtime(&current_time));

   const bool standard_time = (guiglobs::cdsData.config.LangUnit.Get().key_time_format == STANDARD_TIME_FORMAT);

   if (standard_time)
   {
      int hour = atoi(hours);
      strcat(units_time, ( hour < 12 ) ? units_time_am : units_time_pm);

      strftime(hours, 3, "%I", localtime(&current_time));
   }

   // replace default first zero char if present
   if (hours[0] == '0')
   {
      hours[0] = ' ';
   }

   strftime(minutes, 3, "%M", localtime(&current_time));
   strftime(seconds, 3, "%S", localtime(&current_time));

   //
   // Put it all together.
   //
   strcat(time_of_day, hours);
   strcat(time_of_day, units_time_delimiter);
   strcat(time_of_day, minutes);
   strcat(time_of_day, units_time);

   text_time_of_day.set_text_direct(sizeof(time_of_day), time_of_day);
}  //  End Screen_CFR11::TIME_OF_DAY_DISPLAY



//
// UdateAllSelectedItems
// Only call this routine on the response from sherpa.  Otherwise, the UpdateItem call may not work.
//
void Screen_CFR11::UpdateAllSelectedItems (const char* badge, AuthorizationStatusType status, BiometricSignatureType deviceId, int sequenceNumber, const void* authData)
{
   // Update the on-screen list.
   for (CFR_LIST::iterator iter = guiglobs::cfr_list.begin(); iter != guiglobs::cfr_list.end(); ++iter)
   {
      DataLog(log_level_gui_cfr11_info) << "Checking item (RN " << (*iter).key
                                        << ", Selected " << (*iter).selected << ", " << (*iter).authStatus << ") for update..." << endmsg;

      if ((*iter).selected)
      {
         DataLog(log_level_gui_cfr11_info) << "Updating item (RN " << (*iter).key << ")" << endmsg;

         // Only update valid cases.
         // Invalid cases have already been logged.
         if (status != BIO_AUTH_FAILED)
         {
            guiglobs::bioMgr().UpdateItem(badge, status, deviceId, sequenceNumber, authData);
         }
      }
   }
}



//
// EraseAllSelectedItems
//
void Screen_CFR11::EraseAllSelectedItems ()
{
   // Update the on-screen list.
   CFR_LIST::iterator iter = guiglobs::cfr_list.begin();

   while (iter != guiglobs::cfr_list.end())
   {
      DataLog(log_level_gui_cfr11_info) << "Checking item (" << (*iter).key << ", " << (int)(*iter).selected
                                        << ", " << (*iter).authStatus << ") for erasing..." << endmsg;

      if ((*iter).selected)
      {
         DataLog(log_level_gui_cfr11_info) << "Erasing item (" << (*iter).key << ", " << (int)(*iter).selected
                                           << ", " << (*iter).authStatus << ")" << endmsg;

         // Erase the entry
         guiglobs::cfr_list.erase(iter);

         iter = guiglobs::cfr_list.begin();
      }
      else
         ++iter;
   }
}



//
// HandleAlarmStateChangeMsg
//
void Screen_CFR11::HandleAlarmStateChangeMsg ()
{
   DataLog(log_level_gui_cfr11_info) << "Received alarm state change msg" << endmsg;

   // send the alarm confirm message after deallocation is complete
   Alarm_response_struct alarm_set_struct;
   alarm_set_struct.alarm_name     = CFR11_TRACKING_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   Message<Alarm_response_struct> alarmRspns(MessageBase::SEND_LOCAL);
   alarmRspns.send(alarm_set_struct);

   // Refresh the screen
   refresh_list();
}



//
// refresh_resources
//
void Screen_CFR11::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();

   if (_statusLine != textEmpty)
   {
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);
   }
}

void Screen_CFR11::initialize_status_line ()
{
   _statusLine     = textStatAuthEvents;
   _statusLineType = DEFAULT_STATUS_LINE;

   guiglobs::apheresis_status_line->set_current_status_line_text_by_type(_statusLineType, _statusLine);
}

/* FORMAT HASH f02cfad8d842238e309f9bce9c4b8b8c */
