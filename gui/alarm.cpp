/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_ALARM

*     File name:   alarm.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the ALARM screen

*************************< FOCUSSED GUI DOMAIN >****************************/

#include <vxworks.h>


// header file include
#include "alarm.hpp"
#include "trimamessages.h"
#include "statline.hpp"
#include "guiglobs.hpp"
#include "trima_info.h"
#include "predict_manager.hpp"
#include "scrncomm.hpp"
#include "generic_positions.hpp"


static const int BITMAP_BUTTON_ADJUST_Y = 188;
static const int BITMAP_BUTTON_UNLOAD_Y = 261;


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_ALARM::Screen_ALARM()
   : Base_Apheresis_Window (GREY, GUI_SCREEN_ALARM),

     current_state            (0),
     current_substate         (0),
     inMeteredStorage         (false),
     alarm_enumeration        (ALARM_VALUES(0)),
     alarm_type               (ALARM_TYPES (0)),
     alarm_state              (ALARM_STATES(0)),
     _alarmTable              (),
     _alarmItem               (NULL),
     _procAlarmsTable         (),
     four_button_suite        (false),
     rinseback_status_request (0),
     alarm_name               (ALARM_VALUES(0)),
     alarm_status_line_text   (TEXT_STRING_ID(0)),
     alarm_body_text          (TEXT_STRING_ID(0)),
     alarm_bitmap             (BITMAP_ID(0)),
     goto_screen_literal      (NULL),
     goto_screen_gui          (GUI_SCREEN_ID(0)),
     alarm_opts               (0),


     btn_alarm_continue (GUI_BUTTON_ALARM_CONTINUE,
                         BITMAP_BUTTON_GENERIC_X,
                         BITMAP_BUTTON_CONTINUE_Y,
                         textButtonLongContinue,
                         BITMAP_BUTTON_CONTINUE_UP,
                         BITMAP_BUTTON_CONTINUE_DN),

     btn_alarm_rinseback(GUI_BUTTON_ALARM_RINSEBACK,
                         BITMAP_BUTTON_GENERIC_X,
                         BITMAP_BUTTON_RINSEBACK_Y,
                         textButtonAlarmRinseback,
                         BITMAP_BUTTON_RINSEBACK_UP,
                         BITMAP_BUTTON_RINSEBACK_DN),

     btn_alarm_exit     (GUI_BUTTON_ALARM_EXIT,
                         BITMAP_BUTTON_GENERIC_X,
                         BITMAP_BUTTON_EXIT_Y,
                         textButtonAlarmExit,
                         BITMAP_BUTTON_EXIT_UP,
                         BITMAP_BUTTON_EXIT_DN),

     btn_alarm_unload   (GUI_BUTTON_ALARM_UNLOAD,
                         BITMAP_BUTTON_GENERIC_X,
                         BITMAP_BUTTON_UNLOAD_Y,
                         textButtonAlarmUnload,
                         BITMAP_BUTTON_UNLOAD_UP,
                         BITMAP_BUTTON_UNLOAD_DN),

     btn_alarm_adjust   (GUI_BUTTON_ALARM_ADJUST,
                         BITMAP_BUTTON_GENERIC_X,
                         BITMAP_BUTTON_ADJUST_Y,
                         textButtonAlarmAdjust,
                         BITMAP_BUTTON_ADJUST_UP,
                         BITMAP_BUTTON_ADJUST_DN),

     bitmap_continue_button_disabled (BITMAP_BUTTON_GENERIC_X,
                                      BITMAP_BUTTON_CONTINUE_Y,
                                      textButtonLongContinue,
                                      BITMAP_BUTTON_CONTINUE_DISABLED),

     bitmap_text_background          (10, 115, BITMAP_ALARM_TEXT_ONLY_BACKGROUND),

     bitmap_text_bitmap_background   (10, 115, BITMAP_ALARM_TEXT_AND_BITMAP_BACKGROUND),

     bitmap_alarm_foreground         (BITMAP_ALARM_BITMAP_BACKGROUND_X,
                                      BITMAP_ALARM_BITMAP_BACKGROUND_Y,
                                      BITMAP_ALARM_BITMAP_BACKGROUND),

// text_continue_button_disabled   (textButtonAlarmDisabledContinue),

     text_and_bitmap_display         (textEmptyFormatAlarmBmpDisplay),

     text_only_display               (textEmptyFormatAlarmNoBmpDisplay),

     text_version                    (textEmptyFormatAlarmVersionLocation),

     text_alarm_number               (textEmptyFormatAlarmNumberData),

     _timerMuteTimeout(MUTE_PERIOD, Callback<Screen_ALARM>(this, &Screen_ALARM::mute_timer_handler), TimerMessage::DISARMED)

{
   // initialize four button suite message to false
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_ALARM::~Screen_ALARM()
{
   if (_allocated)
   {
      // deallocate_resources is virtual, so be sure to call the right one!
      Screen_ALARM::deallocate_resources();
   }
}



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_ALARM::allocate_resources (const char* allocation_parameter)
{
   _alarmItem = NULL;         // initialize alarm table pointer

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // log alarm reciept
   DataLog(log_level_gui_info) << "alarm screen allocate " << allocation_parameter << endmsg;

   // Allocate a scrnevnt handler for receiving a "State Change" message.
   _msgAlarmStateChange.init(Callback<Screen_ALARM>(this, &Screen_ALARM::alarm_state_change), MessageBase::SNDRCV_RECEIVE_ONLY);
   _cassStatMsg.init(Callback<Screen_ALARM>(this, &Screen_ALARM::cassetteChange), MessageBase::SNDRCV_RECEIVE_ONLY);

   // switch to appropriate button set depending on state
   current_state    = guiglobs::screen_status.get_current_system_state ();
   current_substate = guiglobs::screen_status.get_current_substate ();

   // are we in metered storage? set this true or false.
   inMeteredStorage = (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE ||
                       guiglobs::screen_status.get_current_system_state() == METERED_STORAGE_DISCONNECT);

   // determine alarm parameters using lookup table
   decode_allocation_param();

   guiglobs::current_displayed_alarm = alarm_enumeration;

   _alarmItem = _alarmTable.GetEntry(alarm_enumeration);

   if (_alarmItem == NULL)
   {
      // If we didn't find the alarm, then log the failure as a Safety T1
      _alarmItem = _alarmTable.GetEntry(SAFETY_T1_FAILURE);

      if (_alarmItem == NULL)
      {
         DataLog(log_level_gui_error) << "Bad Alarm ID " << (int)alarm_enumeration << endmsg;
         _FATAL_ERROR (__FILE__, __LINE__, "Proc alarm not found in GUI alarm table");
         return;
      }
   }

   // If the alarm table says this alarm can cause platelet contamination, and
   // we intend to collect platelets, show the magnifying glass on the alarm screen.
   if ( ((_alarmItem->alarm_name == GENERIC_PLT_WBC_CONTAMINATION_ALERT) ||
         (_alarmItem->alarm_name == GENERIC_PLT_WBC_CONTAMINATION_OBTRUSIVE_ALERT))
#if (CPU!=SIMNT)  // If we're in the simulator, we don't want to worry about the targeted product, we want to see the icon anyway.
        && (guiglobs::cdsData.targets.PlateletVolumeTarget > 0.0f)
#endif // #if (CPU!=SIMNT)
        )
   {
      btn_alarm_continue.set_up_bitmap(BITMAP_BUTTON_ALARM_CONTINUE_UP_WBC_FLAG);
   }

   // set up screen based on the alarm type
   switch (alarm_type)
   {
      ////////////////////// OPERATOR_ATTENTION ////////////////////////////////////////////////
      case  OPERATOR_ATTENTION :
      {

         // allocate the continue button
         btn_alarm_continue.allocate_resources(*this);
         btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);

         // set rinseback status to true -- for Warnings or Op. Attentions per I.T. 4606

         if (!(_alarmItem->alarm_options & ALARM_OPT_UNOBTRUSIVE_ALERT))
            checkRinsebackOk(_alarmItem->alarm_name);


         if (( current_state != POST_RUN || alarm_type != OPERATOR_ATTENTION ) &&
             ( current_state != DONOR_DISCONNECT || !guiglobs::donor_disconnect_confirmed ) &&
             !(_alarmItem->alarm_options & ALARM_OPT_UNOBTRUSIVE_ALERT))
         {
            // allocate the exit button for all alarm types except A2 and
            // all non-ooperator attention alarms prior to post run
            // or all alarms in donor disconnect
            // and for LOW_PLATELET_CONCENTRATION_ALERT IT5665
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_abort_procedure, (void*)this);
         }

         if ( inMeteredStorage )
         {
            btn_alarm_exit.set_string_id (textButtonAlarmEndAddition);
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_goto_endMeteredStorage, (void*)this);

            btn_alarm_continue.set_string_id (textButtonLongContinue);
            btn_alarm_continue.allocate_resources(*this);
            btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);
         }


         // special case of an Operator action that has a sound... IT 11547
         if ( _alarmItem->alarm_name == SPILLOVER_DETECTED
              // /* add more here ;) */
              )
         {
            // set the bell/ bitmap on the mute button and add callback
            btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
            btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
            btn_lowleft.set_callback (callback_mute_sound, (void*)this);

         }
         else         // most operator actions dont sound so dealloc the sound button

         {     // modify standard buttons
               // allocate standard buttons based on alarm type
               // no sound management for an operator attention
            btn_lowleft.deallocate_resources ();
         }

      }
      break;

      ////////////////////// R1  R1  R1  R1 ////////////////////////////////////////////////
      case R1 :
      {
         // NOTE : always allocate disabled continue button under the active button
         bitmap_continue_button_disabled.allocate_resources(*this);
         // text_continue_button_disabled.allocate_resources(*this);

         // establish new location of the continue button text
         // OSRegion    text_dimension(BITMAP_BUTTON_GENERIC_X, BITMAP_BUTTON_CONTINUE_Y, 115, 40);
         // text_continue_button_disabled.set_requested_region(text_dimension);

         // request rinseback status to determine if rinseback is appropriate
         checkRinsebackOk(_alarmItem->alarm_name);

         // ALL four button alarm specific information is managed here,
         // ALL corresponding states are managed here and in four button suite adjustment
         // the corresponding states are used to filter button name an callback info
         // if in access pressure alert, pause or stop alert and in R1 alter position of continue button
         four_button_suite =
            (_alarmItem->alarm_options & ALARM_OPT_4_BTN_SUITE) ||

            // Any disposable test alarm
            (current_state == DISPOSABLE_TEST) ||

            // or if motor hardware alarm during cassette load (IT4801)
            (current_state == STARTUP_TESTS && (_alarmItem->alarm_options & ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST));

         // set the bell/ bitmap on the mute button and add callback
         btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
         btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
         btn_lowleft.set_callback (callback_mute_sound, (void*)this);


         if (( current_state != POST_RUN || alarm_type != OPERATOR_ATTENTION ) &&
             (( current_state != DONOR_DISCONNECT && guiglobs::screen_status.get_current_screen() != GUI_SCREEN_DISCONN) || !guiglobs::donor_disconnect_confirmed ) &&
             !(_alarmItem->alarm_options & ALARM_OPT_UNOBTRUSIVE_ALERT))
         {
            // allocate the exit button for all alarm types except A2 and
            // all non-ooperator attention alarms prior to post run
            // or all alarms in donor disconnect
            // and for LOW_PLATELET_CONCENTRATION_ALERT IT5665
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_abort_procedure, (void*)this);
         }

         if ( inMeteredStorage )
         {
            btn_alarm_exit.set_string_id (textButtonAlarmEndAddition);
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_goto_endMeteredStorage, (void*)this);

            btn_alarm_continue.set_string_id (textButtonLongContinue);
            btn_alarm_continue.allocate_resources(*this);
            btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);
         }


      }
      break;

      ////////////////////// R2  R2  R2  R2 ////////////////////////////////////////////////
      case R2 :
      {
         // If we're in the rinseback state, or we're in PFR and rinseback is OK
         if (current_state == BLOOD_RINSEBACK ||
             (_alarmItem->alarm_name == POWER_FAIL_RINSEBACK))
         {
            // the continue button in this instance allows the user to continue with
            // rinseback
            btn_alarm_continue.allocate_resources(*this);
            btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);

            // text is always on top, so the disabled continue button text
            // must be deallocated.
            // text_continue_button_disabled.deallocate_resources();

         }        // else offer rinseback
         else
            checkRinsebackOk(_alarmItem->alarm_name);

         // set the bell/ bitmap on the mute button and add callback
         btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
         btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
         btn_lowleft.set_callback (callback_mute_sound, (void*)this);

         if ( inMeteredStorage ) btn_alarm_exit.set_string_id (textButtonAlarmEndAddition);

         if (( current_state != POST_RUN || alarm_type != OPERATOR_ATTENTION ) &&
             ( current_state != DONOR_DISCONNECT || !guiglobs::donor_disconnect_confirmed ) &&
             !(_alarmItem->alarm_options & ALARM_OPT_UNOBTRUSIVE_ALERT))
         {
            // allocate the exit button for all alarm types except A2 and
            // all non-ooperator attention alarms prior to post run
            // or all alarms in donor disconnect
            // and for LOW_PLATELET_CONCENTRATION_ALERT IT5665
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_abort_procedure, (void*)this);
         }
      }
      break;

      ////////////////////// A1  A1  A1  A1 ////////////////////////////////////////////////
      case A1 :
      {

         // set the bell/ bitmap on the mute button and add callback
         btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
         btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
         btn_lowleft.set_callback (callback_mute_sound, (void*)this);

         if (( current_state != POST_RUN || alarm_type != OPERATOR_ATTENTION ) &&
             ( current_state != DONOR_DISCONNECT || !guiglobs::donor_disconnect_confirmed ) &&
             !(_alarmItem->alarm_options & ALARM_OPT_UNOBTRUSIVE_ALERT))
         {
            // allocate the exit button for all alarm types except A2 and
            // all non-ooperator attention alarms prior to post run
            // or all alarms in donor disconnect
            // and for LOW_PLATELET_CONCENTRATION_ALERT IT5665
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_abort_procedure, (void*)this);
         }


         // Metered Storage alarms and alerts fall in here
         // overide the exit button
         if (inMeteredStorage)
         {
            btn_alarm_exit.set_string_id (textButtonAlarmEndAddition);
            btn_alarm_exit.allocate_resources(*this);
            btn_alarm_exit.set_callback (callback_goto_endMeteredStorage, (void*)this);
         }


      }
      break;

      ////////////////////// A2  A2  A2  A2 ////////////////////////////////////////////////
      case A2 :
      {
         // set the bell/ bitmap on the mute button and add callback
         btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);
         btn_lowleft.set_down_bitmap (BITMAP_BUTTON_SOUND_DN);
         btn_lowleft.set_callback (callback_mute_sound, (void*)this);
      }
      break;

      ////////////////////// default error  ////////////////////////////////////////////////
      default :
      {
         DataLog(log_level_gui_error) << "Bad Alarm type " << (int)alarm_type << endmsg;
         _FATAL_ERROR (__FILE__, __LINE__, "unknown alarm_type enum");
      }
      break;

         /////////////////////////////////////////////////////////////////////////////////////////////
   }   // end switch

   if (_alarmItem->alarm_options & ALARM_OPT_DISALLOW_END_RUN)

   {
      // Fix for IT8075: disconnect test alarms and alerts should not have an
      // End Run button since the procedure is finished and the donor disconnected,
      // so deallocate it
      btn_alarm_exit.deallocate_resources();
      btn_cancel.deallocate_resources();
   }

   // if in four button suite add fourth button and relocate others
   if (four_button_suite)
      four_button_suite_adjustment();

   if (alarm_type == A1 ||
       alarm_type == A2 ||
       (_alarmItem->alarm_options & ALARM_OPT_SHOW_ALARM_NUM))
   {
      // If this alarm is an A1 or A2 (old term. E1, E2), display the alarm
      //  enumeration near the lower right corner of the screen.
      sprintf(alarm_enumeration_buffer, "%u", (unsigned int)alarm_enumeration);
      text_alarm_number.set_text_direct (strlen(alarm_enumeration_buffer), alarm_enumeration_buffer);
      text_alarm_number.allocate_resources(*this);
   }
   else text_alarm_number.deallocate_resources();

   // now temporarily deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_continue.deallocate_resources ();

   // Link the status bar/line to this window
   link_status_bar_line();

   // build the screen
   reconstruct_alarm_screen ();

   // send the alarm confirm message after allocation is complete
   alarm_set_struct.alarm_name     = alarm_enumeration;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();


    #if (CPU!=SIMNT) // This extra text is no good for screenshots.
   ///////////////////////////////////////////////////////////////////////////////////////
   // look for the Clinical or Test build files to show some non-standard stuff on the gui
   ///////////////////////////////////////////////////////////////////////////////////////
   if (TrimaInfo::isClinicalBuild() || TrimaInfo::isTestBuild())
   {
      // Extract the Version ID from the text object -- only allow 1/5 of the concat string to be used
      guistring text_version_string;
      text_version.set_text_via_id(textMiscTrimaVersion);
      text_version.get_text_direct (text_version_string);

      if (TrimaInfo::isClinicalBuild())
         strcat (text_version_string, "  Clinical ");

      strcat (text_version_string, TrimaInfo::buildNumber());

      if (TrimaInfo::isTestBuild())
      {
         // Build the string with the Trima, Safety CRC's and the build number
         strcat (text_version_string, "  C=");
         strcat (text_version_string, TrimaInfo::trimaCrc());
         strcat (text_version_string, " Datalog=");
         strcat (text_version_string, TrimaInfo::logFileNameFull());
      }

      // Put the Version ID plus the various CRC's and build ID back into the version object
      text_version.set_text_direct(text_version_string.length(), text_version_string);

      // allocate the text version string id
      text_version.allocate_resources(*this) ;
   }
#endif // #if (CPU!=SIMNT)

#if 0
   // This is a hack to get all the buttons to show up.  I just use it for
   // making layout changes, but it's not compiled because of the compiler
   // directive above.  It's useful sometimes, though, so I'll leave it here.
   /**/ //////////////////////////////////////////////////////////////////
   /**/ btn_alarm_continue.allocate_resources(*this);
   /**/ btn_alarm_rinseback.allocate_resources(*this);
   /**/ btn_alarm_exit.allocate_resources(*this);
   /**/ btn_alarm_adjust.allocate_resources(*this);
   /**/ btn_alarm_unload.allocate_resources(*this);
   /**/
   /**/ OSRegion btnRegion;
   /**/
   /**/ btn_alarm_continue.get_region(btnRegion);
   /**/ DataLog (log_level_gui_info) << "Continue Region: " << btnRegion << endmsg;
   /**/ btn_alarm_rinseback.get_region(btnRegion);
   /**/ DataLog (log_level_gui_info) << "Rinseback Region: " << btnRegion << endmsg;
   /**/ btn_alarm_exit.get_region(btnRegion);
   /**/ DataLog (log_level_gui_info) << "Exit Region: " << btnRegion << endmsg;
   /**/ btn_alarm_adjust.get_region(btnRegion);
   /**/ DataLog (log_level_gui_info) << "Adjust Region: " << btnRegion << endmsg;
   /**/ btn_alarm_unload.get_region(btnRegion);
   /**/ DataLog (log_level_gui_info) << "Unload Region: " << btnRegion << endmsg;
   /**/ //////////////////////////////////////////////////////////////////
#endif // #if 0

   // Log which buttons are currently displayed on this alarm.
   DataLog (log_level_gui_info) << "Alarm buttons displayed: "
                                << "[cont="     << btn_alarm_continue.allocated()
                                << "; rinse="   << btn_alarm_rinseback.allocated()
                                << "; endrun="  << btn_alarm_exit.allocated()
                                << "; adjust="  << btn_alarm_adjust.allocated()
                                << "; unload="  << btn_alarm_unload.allocated()
                                << "]" << endmsg;

}  // END of Screen_ALARM ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_ALARM::deallocate_resources ()

{
   guiglobs::current_displayed_alarm = NULL_ALARM;

   _msgAlarmStateChange.deregisterMsg();
   _cassStatMsg.deregisterMsg();

   _timerMuteTimeout.armTimer(TimerMessage::DISARMED);

   // deallocate all non-standard buttons
   btn_alarm_continue.deallocate_resources();
   btn_alarm_rinseback.deallocate_resources();
   btn_alarm_exit.deallocate_resources();
   btn_alarm_adjust.deallocate_resources();
   btn_alarm_unload.deallocate_resources();
   btn_continue.deallocate_resources();
   text_and_bitmap_display.deallocate_resources();
   text_only_display.deallocate_resources();
   // text_continue_button_disabled.deallocate_resources();
   text_alarm_number.deallocate_resources();
   bitmap_alarm_foreground.deallocate_resources();
   bitmap_text_background.deallocate_resources();
   bitmap_text_bitmap_background.deallocate_resources();
   bitmap_continue_button_disabled.deallocate_resources();

   text_version.deallocate_resources  () ;

   // Remove alarm status line color and message
   if (guiglobs::apheresis_status_line) guiglobs::apheresis_status_line->deactivate_status_line_type(ALARM_STATUS_LINE);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   // send the alarm confirm message after deallocation is complete
   alarm_set_struct.alarm_name     = NULL_ALARM;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();


}   // END of Screen_ALARM DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DECODE_ALLOCATION_PARAM
void Screen_ALARM::decode_allocation_param ()
{
   // copy allocation string into buffer for parsing
   guistring allocation_param_buffer(get_allocation_parameter());
   char*     strtokstr = allocation_param_buffer.make_buffer();

   char*     nextToken = 0;

   // use the predetermined string token to extract the three data elements
   alarm_enumeration = (ALARM_VALUES)atoi(strtok_r(strtokstr, "~", &nextToken));
   alarm_type        = (ALARM_TYPES)atoi(strtok_r(NULL, "~", &nextToken));
   alarm_state       = (ALARM_STATES)atoi(strtok_r(NULL, "~", &nextToken));
   GUI_POOL_FREE(strtokstr);
}   // END of Screen_ALARM DECODE_ALLOCATION_PARAM



////////////////////////////////////////////////////////////////////////////////
// ALARM_SET_STRUCT_MANAGER
void Screen_ALARM::alarm_set_struct_manager ()
{
   Message<Alarm_response_struct> alarm(MessageBase::SEND_LOCAL);
   alarm.send(alarm_set_struct);
}  // End of ALARM_SET_STRUCT_MANAGER



////////////////////////////////////////////////////////////////////////////////
// RECONSTRUCT_ALARM_SCREEN
void Screen_ALARM::reconstruct_alarm_screen ()
{
   // deallocate all widgets based on parameters passed in

   // assume for now text and bitmaps will always change between screens
   // NOTE : leave the disabled continue button up for now, all R1's
   text_and_bitmap_display.deallocate_resources();
   text_only_display.deallocate_resources();
   bitmap_alarm_foreground.deallocate_resources();
   bitmap_text_background.deallocate_resources();
   bitmap_text_bitmap_background.deallocate_resources();

   // deallocate the continue (more info) button and the goback button
   btn_goback.deallocate_resources();
   btn_continue.deallocate_resources();

   // extract all table data to initialize alarm screen
   if (_alarmItem)
   {
      alarm_name             = _alarmItem->alarm_name;
      alarm_body_text        = _alarmItem->alarm_body_text;
      alarm_status_line_text = _alarmItem->alarm_status_line_text;
      alarm_bitmap           = _alarmItem->alarm_bitmap;
      alarm_opts             = _alarmItem->alarm_options;
      goto_screen_literal    = _alarmItem->goto_screen_literal;
      goto_screen_gui        = _alarmItem->goto_screen_gui;
   }
   else
   {
      goto_screen_literal = NULL;
      DataLog (log_level_gui_error) << "Alarm pointer is NULL." << endmsg;
   }

   // reallocate all resources as necessary

   // set the background
   // set the appropriate bitmap backgrounds based on state and alarm
   // display type
   if (alarm_bitmap > BITMAP_NULL && alarm_bitmap < BITMAP_ID_COUNT)
   {
      bitmap_text_background.deallocate_resources();
      bitmap_text_bitmap_background.allocate_resources(*this);

        #define ALARM_GRAPHIC_SPACE_X 258
        #define ALARM_GRAPHIC_SPACE_Y 115
        #define ALARM_GRAPHIC_SPACE_W 219
        #define ALARM_GRAPHIC_SPACE_H 304

      {   // Center the graphic within the allotted space.
         OSRegion bmpRegion;

         bitmap_alarm_foreground.set_id(alarm_bitmap);
         bitmap_alarm_foreground.allocate_resources(*this);
         bmpRegion = bitmap_alarm_foreground.get_dimensions();

         DataLog (log_level_gui_info) << "Graphic region for alarm " << alarm_name
                                      << " is " << bmpRegion << endmsg;

         if (bmpRegion.width > ALARM_GRAPHIC_SPACE_W)
         {
            DataLog (log_level_gui_error) << "Alarm graphic " << alarm_bitmap
                                          << " width " << bmpRegion.width << " exceeds allotted "
                                          << ALARM_GRAPHIC_SPACE_W << endmsg;
            bmpRegion.x = ALARM_GRAPHIC_SPACE_X;
         }
         else bmpRegion.x = ALARM_GRAPHIC_SPACE_X + ((ALARM_GRAPHIC_SPACE_W - bmpRegion.width) / 2);

         if (bmpRegion.height > ALARM_GRAPHIC_SPACE_H)
         {
            DataLog (log_level_gui_error) << "Alarm graphic " << alarm_bitmap
                                          << " height " << bmpRegion.height << " exceeds allotted "
                                          << ALARM_GRAPHIC_SPACE_H << endmsg;
            bmpRegion.y = ALARM_GRAPHIC_SPACE_Y;
         }
         else bmpRegion.y = ALARM_GRAPHIC_SPACE_Y + ((ALARM_GRAPHIC_SPACE_H - bmpRegion.height) / 2);

         bitmap_alarm_foreground.set_position(bmpRegion.x, bmpRegion.y);
      }

      text_and_bitmap_display.set_string_id(alarm_body_text);
      text_and_bitmap_display.allocate_resources(*this);
   }
   else
   {
      bitmap_text_bitmap_background.deallocate_resources();
      bitmap_alarm_foreground.deallocate_resources();
      bitmap_text_background.allocate_resources(*this);
      text_only_display.set_string_id(alarm_body_text);
      text_only_display.allocate_resources(*this);
   }

   // if non-active, display the continue button
   if ((alarm_state == ACTIVE) &&
       (alarm_type == R1))
   {
      btn_alarm_continue.allocate_resources(*this);
      btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);

      // text is always on top, so the disabled continue button text
      // must be deallocated.
      // text_continue_button_disabled.deallocate_resources();

      // if in four button suite add fourth button and relocate others
      if (four_button_suite)
      {
         four_button_suite_adjustment ();
      }

   }

   if (alarm_opts & ALARM_OPT_UNLOAD_BTN_ONLY)
   {
      // If their only option is to unload, go ahead and take away
      // the continue button.
      btn_alarm_continue.deallocate_resources();
      // text_continue_button_disabled.deallocate_resources();
      bitmap_continue_button_disabled.deallocate_resources();
   }

   // set the more info button based multiple screen property
   // if the alarm position is the first of many or in the middle allocate
   // the more info button
   if (_alarmTable.PeekNextEntry() &&
       _alarmTable.PeekNextEntry()->alarm_name == alarm_name)
   {
      btn_continue.set_string_id (textButtonLongAlarmMoreInfo);
      btn_continue.allocate_resources(*this);
      btn_continue.set_callback (callback_more_info, (void*)this);
   }

   if (_alarmTable.PeekPreviousEntry() &&
       _alarmTable.PeekPreviousEntry()->alarm_name == alarm_name)
   {
      btn_goback.allocate_resources(*this);
      btn_goback.set_up_bitmap(BITMAP_BUTTON_GOBACK_UP);
      btn_goback.set_callback (aph_callback_goback, (void*)this);
   }

   // Display alarm status line message
   if (guiglobs::apheresis_status_line) guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ALARM_STATUS_LINE, alarm_status_line_text);

}   // End of RECONSTRUCT_ALARM_SCREEN

////////////////////////////////////////////////////////////////////////////////
// FOUR_BUTTON_SUITE_ADUSTMENT
void Screen_ALARM::four_button_suite_adjustment ()
{
   if (
      (alarm_opts & ALARM_OPT_UNLOAD_BTN_ONLY) ||
      (current_state == DISPOSABLE_TEST) ||
      ((alarm_enumeration==EVACUATION_PRESSURE_ALERT) || (alarm_enumeration==EVACUATION_PRESSURE_ALERT_JP) ) ||
      (
         ( (alarm_enumeration==MOTOR_FAILED_TO_MAINTAIN_SPEED) || (alarm_enumeration==MOTOR_INTEGRATION_ERROR)
           || (alarm_enumeration==MOTOR_VOLUME_ERROR) || (alarm_enumeration==MOTOR_DISABLED) ) &&
         (current_state == STARTUP_TESTS)
      ) ||
      (
         (alarm_enumeration==RBC_DETECTOR_CAL_FAILURE) &&
         (current_state == STARTUP_TESTS)
      )
      )
   {
      // allocate and display extra alarm button for unload
      btn_alarm_unload.allocate_resources(*this);
      btn_alarm_unload.set_callback (callback_unload, (void*)this);

      //
      //  Since we have allocated the unload button, deallocate the 'end run'
      //  button to force the operator to make the right choice.
      //
      btn_alarm_exit.deallocate_resources();
   }

   if ((!(alarm_opts & ALARM_OPT_DISALLOW_ADJUST)) &&
       ((current_state == BLOOD_RUN) || (current_state == BLOOD_RINSEBACK)))
   {
      // allocate and display extra alarm button for adjust
      btn_alarm_adjust.allocate_resources(*this);
      btn_alarm_adjust.set_callback (callback_adjust, (void*)this);
   }

}   // End of FOUR_BUTTON_SUITE_ADUSTMENT


////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER

void Screen_ALARM::screen_timer_handler ()

{

   // Get the sound for this alarm and start it
   SOUND_CODE sound = _procAlarmsTable[alarm_enumeration].getSound();

   guiglobs::button_audio_feedback.create_feedback_sound (sound, SOUND_CONTINUOUS);

   // Change the bitmap to the one with sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_ACTIVATE_SOUND);

}


////////////////////////////////////////////////////////////////////////////////
// MUTE_TIMER_HANDLER

void Screen_ALARM::mute_timer_handler ()

{
   // Get the sound for this alarm and start it
   SOUND_CODE sound = _procAlarmsTable[alarm_enumeration].getSound();

   // Beep once to remind them they muted an alarm
   guiglobs::button_audio_feedback.create_feedback_sound (sound, SOUND_ONCE);

   // Restart the timer so it beeps again
   _timerMuteTimeout.armTimer(TimerMessage::ARMED);

}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON
void Screen_ALARM::process_goback_button ()
{
   // decrement the table index and reassert the screen components
   _alarmItem = _alarmTable.GetPreviousEntry();
   reconstruct_alarm_screen();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_PROCEDURE_BUTTON
void Screen_ALARM::process_continue_procedure_button ()
{

   // set and send alarm response
   alarm_set_struct.alarm_name     = alarm_enumeration;
   alarm_set_struct.alarm_response = CONTINUE;
   alarm_set_struct_manager ();

   DataLog(log_level_gui_error) << "Screen_ALARM, CONTINUE: stateless count "
                                << guiglobs::scrnComm->stateless_screen_stack_count()
                                << ",  with goto_string_literal = " << goto_screen_literal
                                << " and goto_screen_gui = " << goto_screen_gui
                                << ", predict screen wanted: "
                                << PredictManager::prediction_screen_requested()
                                << endmsg;


   if (goto_screen_literal && (goto_screen_gui != GUI_SCREEN_NULL_SCREEN))
   {

      DataLog(log_level_gui_info) << "Alarm_SCREEN, exiting with goto_string_literal = " << goto_screen_literal
                                  << " and goto_screen_gui = " << goto_screen_gui << endmsg;

      // clear the alarm initialization variable so we can reallocate the same
      // alarm in succession after it has been dismissed
      guiglobs::gui_alarm_manager.clear_alarm_screen ();

      // switch to appropriate button set depending on info provided
      goto_screen (goto_screen_gui, goto_screen_literal);
   }

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << alarm_set_struct.alarm_response << " alarm continue request" << endmsg;


   // IT 13761: if it's a 10 minute pause alarm, we can't continue, so make sure
   // nobody tries to bring the predict screen back up.    Actually, this is
   // true for all A1 and A2 alarms, and R2's can only go to rinseback, in all
   // these cases, tell RunProc (and all other screens) not to bring up the
   // predict screen.
   if ( (alarm_type == A2) || (alarm_type == A1) || (alarm_type == R2) )
   {
      DataLog(log_level_gui_info) << "Screen_ALARM: alarm is of type " << alarm_type
                                  << ", so there's no reason to go back to the predict screen; clearing "
                                  << "the prediction_screen_requested flag." << endmsg;
      PredictManager::clear_prediction_screen_requested();
   }


   // Send an indication to "remove the last stateless screen" which is
   // THIS screen, so the alarm screen leaves and the appropriate
   // procedure screen is uncovered.
   RemoveMostRecentStatelessScreenMsg scrnmsg(MessageBase::SEND_LOCAL);
   scrnmsg.send(0);


   // if the CPS_PTF_PRIME_TEST is raised then we need to put up the
   // proc selection screen for the operator IT8160.
   if ( ( CPS_PTF_PRIME_TEST == alarm_set_struct.alarm_name) &&
        (current_state == BLOOD_RUN) )  // <-- IT 9971
   {
      // No need to request a reprediction; the predict screen takes care of
      // that now.
      DataLog(log_level_gui_info) << "Alarm_SCREEN, exiting with CPS_PTF_PRIME_TEST; "
                                  << "bringing up stateless Predict screen" << endmsg;

      // We're going to the predict screen anyway, so clear the 'predict screen
      // required' flag (this prevents *other* screens from sending us to a
      // stateless predict screen when we're already there.) IT 17438
      PredictManager::clear_prediction_screen_requested();

      // set invocation allocation parameter
      guistring allocation_parameter_string;

      sprintf(allocation_parameter_string, "%d", (int)CPS_DISABLES_RBCS);

      // call the procedure selection screen, because the op. didn't want to
      //  connect RF for this procedure... let 'em select another procedure.
      goto_stateless_screen (GUI_SCREEN_PREDICT, allocation_parameter_string);

   }


   const bool AUTOFLOW_ON = (bool)(guiglobs::cdsData.config.Procedure.Get().key_autoflow);
   if (AUTOFLOW_ON == true)
   {
      // not for the auto aps pauses.. that causes to much time test.
      if (APS_PAUSE != alarm_enumeration)
      {
         // update the time for the ten min autoflow alrm
         UpdateAutoFlowTimerMsg updateTime(MessageBase::SEND_LOCAL);
         updateTime.send(2);

         DataLog(log_level_gui_info) << " set Autoflow time update in response to alarm Continue btn (alarm = "
                                     << alarm_enumeration << " )" << endmsg;
      }
   }
}   // END of Screen_ALARM PROCESS_CONTINUE_PROCEDURE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_RINSEBACK_BUTTON

void Screen_ALARM::process_rinseback_button ()
{

   guistring alarm_allocation_string;   // newly constructed alarm data buffer

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", alarm_enumeration);

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

}   // END of Screen_ALARM PROCESS_RINSEBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_ABORT_BUTTON

void Screen_ALARM::process_abort_button ()

{

   guistring alarm_allocation_string;   // newly constructed alarm data buffer

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", alarm_enumeration);

   // create the string that will be parsed by the ABORTCNF screen using
   // components recieved and then created via the incoming alarm structure
   strncpy(alarm_allocation_string, ABORT_TYPE_ALARM, 5);
   strncat(alarm_allocation_string, "~", 1);
   strncat(alarm_allocation_string, alarm_enumeration_buffer, MAXSIZE_BWINDOW_PARAM - 7);

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << "alarm abort received " << (int)ABORT << " " << alarm_allocation_string << endmsg;

   // goto ABORTCNF to give user opportunity to confirm choice
   goto_stateless_screen (GUI_SCREEN_ABORTCNF, alarm_allocation_string);

}   // END of Screen_ALARM PROCESS_ABORT_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_UNLOAD_BUTTON

void Screen_ALARM::process_unload_button ()

{
   // set and send alarm response
   alarm_set_struct.alarm_name     = alarm_enumeration;
   alarm_set_struct.alarm_response = UNLOAD;
   alarm_set_struct_manager ();

   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << (int)alarm_set_struct.alarm_response << " alarm unload request" << endmsg;

   // Freeze the current screen, wait until cassette is raised. Remove the current screen and go to the
   // Screen_SYSCASSETTE screen. (Screen_ALARM::cassetteChange() will update the GUI.)
   btn_alarm_unload.set_up_bitmap(BITMAP_BUTTON_UNLOAD_DN);

   btn_alarm_unload.disable_callbacks();
   btn_alarm_continue.disable_callbacks();
   btn_lowleft.disable_callbacks();

}   // END of Screen_ALARM PROCESS_UNLOAD_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_ADJUST_BUTTON
void Screen_ALARM::process_adjust_button ()
{
   guistring alarm_allocation_string;  // newly constructed alarm data buffer

   // initialize the allocation buffer
   alarm_allocation_string.initialize();

   // Send an indication to "go to stateless screen" which is the adjust screen
   // in this case.
   goto_stateless_screen (GUI_SCREEN_TROUBLE, alarm_allocation_string);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_MORE_INFO_BUTTON
void Screen_ALARM::process_more_info_button ()
{
   // increment the table index and reassert the screen components
   _alarmItem = _alarmTable.GetNextEntry();
   reconstruct_alarm_screen();
}   // END of Screen_ALARM PROCESS_MORE_INFO_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_MUTE_BUTTON
void Screen_ALARM::process_mute_button ()
{
   // Destroy and recreate the timer that will drive the restart the alarm
   // sounds.
   _timerMuteTimeout.armTimer(TimerMessage::ARMED);

   // Change the bitmap to one with no sound waves
   btn_lowleft.set_up_bitmap (BITMAP_BUTTON_DISABLE_SOUND);


}   // END of Screen_ALARM PROCESS_MUTE_BUTTON

//////////////////////////////////////////////////////////////////
// PROCESS THE ENDRUN of METERED STORAGE
void Screen_ALARM::process_end_metered_storage ()
{

   guistring alarm_allocation_string;   // newly constructed alarm data buffer

   // convert the integer element to text for string construction
   sprintf(alarm_enumeration_buffer, "%d", alarm_enumeration);


   // create the string that will be parsed by the ABORTCNF screen using
   // components recieved and then created via the incoming alarm structure
   strncpy(alarm_allocation_string, ABORT_TYPE_ADDITIVE_SOLUTIONS_ALARM, 9);

   strncat(alarm_allocation_string, "~", 1);

   strncat(alarm_allocation_string, alarm_enumeration_buffer, MAXSIZE_BWINDOW_PARAM - 7);


   // clear the alarm initialization variable so we can reallocate the same
   // alarm in succession after it has been dismissed
   guiglobs::gui_alarm_manager.clear_alarm_screen ();

   // log alarm response
   DataLog(log_level_gui_info) << "alarm abort received " << (int)ABORT_TYPE_ADDITIVE_SOLUTIONS_ALARM << " " << alarm_allocation_string << endmsg;

   // goto ABORTCNF to give user opportunity to confirm choice
   goto_stateless_screen (GUI_SCREEN_ABORTCNF, alarm_allocation_string);
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_CONTINUE

void callback_continue_procedure (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_ALARM*)data) -> process_continue_procedure_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_RINSEBACK

void callback_goto_rinseback (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_ALARM*)data) -> process_rinseback_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ABORT

void callback_abort_procedure (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_ALARM*)data) -> process_abort_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_UNLOAD

void callback_unload (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system unload button
   ((Screen_ALARM*)data) -> process_unload_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST

void callback_adjust (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system unload button
   ((Screen_ALARM*)data) -> process_adjust_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_MORE_INFO

void callback_more_info (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_ALARM*)data) -> process_more_info_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_BUTTON_SOUND

void callback_button_sound (void*)
{

   // call sound method to create "beep"
   guiglobs::button_audio_feedback.create_feedback_sound (BUTTON_CLICK_SOUND,
                                                          SOUND_IMMEDIATE);

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_MUTE_SOUND

void callback_mute_sound (void* data)
{
   static AlarmTable alarmTable;

   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // interpret the void* as a pointer to a Screen_ALARM object
   Screen_ALARM& screenref = *static_cast<Screen_ALARM*>(data);

   // Get the alarm enum, look up it's sound, and turn that sound off
   SOUND_CODE sound = alarmTable[screenref.getEnum()].getSound();

   guiglobs::button_audio_feedback.create_feedback_sound (sound, SOUND_CLEAR);

   // call sound method to stop alarm "beep"
//    guiglobs::button_audio_feedback.create_feedback_sound (ALARM_SOUND,
//                                                           SOUND_CLEAR);

   // call sound method to stop alert "beep"
//    guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND,
//                                                           SOUND_CLEAR);

   // Actually call the process button routine for the system setup button
   screenref.process_mute_button();

}   // END of CALLBACK_MUTE_SOUND

////////////////////////////////////////////////////////////////////////////////////

void callback_goto_endMeteredStorage (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "NULL data pointer." << endmsg;
      return;
   }

   // Actually call the
   ((Screen_ALARM*)data) -> process_end_metered_storage ();
}

////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER

void Screen_ALARM::alarm_state_change ()
{
   ALARM_STATES message_data = (ALARM_STATES)_msgAlarmStateChange.getData();

   if ((message_data == LATCHED) && (alarm_type == R1))
   {
      // set alarm state
      alarm_state = LATCHED;

      btn_alarm_continue.deallocate_resources();

      // the disabled continue button text
      // must be  re-allocated.
      // text_continue_button_disabled.allocate_resources(*this);

   }   // End of if (message..
   else if ((message_data == ACTIVE) && (alarm_type == R1))
   {
      // set alarm state
      alarm_state = ACTIVE;

      // allocate continue button if not allocated buttons
      btn_alarm_continue.allocate_resources(*this);
      btn_alarm_continue.set_callback (callback_continue_procedure, (void*)this);

      // text is always on top, so the disabled continue button text
      // must be deallocated.
      // text_continue_button_disabled.deallocate_resources();

      // if in four button suite add fourth button and relocate others
      if (four_button_suite)
      {
         four_button_suite_adjustment();
      }

   }   // End of else if (message..

   alarm_set_struct.alarm_name     = alarm_enumeration;
   alarm_set_struct.alarm_response = CONFIRM;
   alarm_set_struct_manager ();

}

void Screen_ALARM::checkRinsebackOk (ALARM_VALUES alrmVal)
{
   if ( guiglobs::proc_status.rinsebackIsAnOption(alrmVal) )
   {
      // if rinseback is ok allocate the button
      btn_alarm_rinseback.allocate_resources(*this);
      btn_alarm_rinseback.set_callback (callback_goto_rinseback, (void*)this);

      // if in four button suite add fourth button and relocate others
      if (four_button_suite)
      {
         four_button_suite_adjustment();
      }
   }
   else
      btn_alarm_rinseback.deallocate_resources();
}

// Cassette is raised after the operator presses the 'unload' button
void Screen_ALARM::cassetteChange ()
{
   if (_cassStatMsg.getData() == CASSETTE_RAISED)
   {
      // Send an indication to "remove the last stateless screen" which is
      // THIS screen, so the alarm screen leaves and the appropriate
      // procedure screen is uncovered.
      RemoveMostRecentStatelessScreenMsg removescreen(MessageBase::SEND_LOCAL);
      removescreen.send(0);

      goto_screen(GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSDONOR");
   }
}

/* FORMAT HASH 82d479612977df7b22989f2cf4f03761 */
