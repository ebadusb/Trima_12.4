/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SYSDONOR

*     File name:   sysdonor.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************/

#include <vxworks.h>

#include "time.h"

#include "xif_cds.h"       // Gets Vista Interface status


// BASE CLASS INCLUDE

#include "sysdonor.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "guiact.hpp"
#include "sdaction.hpp"


#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSDONOR::Screen_SYSDONOR()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSDONOR),

     btn_system_setup (GUI_BUTTON_LOADSYS,
                       BUTNSETUP_X,
                       BUTNSETUP_Y,
                       textButtonGiantLoadSystem,
                       BITMAP_BUTTON_SYS_SETUP_UP,
                       BITMAP_BUTTON_SETUP_DONOR_DOWN),

     btn_donor_info   (GUI_BUTTON_DONORINFO,
                       BUTNINFO_X,
                       BUTNINFO_Y,
                       textButtonDonorInfo,
                       BITMAP_BUTTON_DONOR_INFO_UP,
                       BITMAP_BUTTON_SETUP_DONOR_DOWN),

     bitmap_donor_picture (0),

     bitmap_donor_top      (BITMAP_TOP_UP_X,
                            BITMAP_TOP_UP_Y,
                            BITMAP_TOP_UP),

     bitmap_donor_bottom   (BITMAP_BOTTOM_UP_X,
                            BITMAP_BOTTOM_UP_Y,
                            BITMAP_BOTTOM_UP),

     bitmap_donor_left     (BITMAP_LEFT_UP_X,
                            BITMAP_LEFT_UP_Y,
                            BITMAP_LEFT_UP),

     bitmap_donor_right    (BITMAP_RIGHT_UP_X,
                            BITMAP_RIGHT_UP_Y,
                            BITMAP_RIGHT_UP),

     system_was_reset         (false),
     ap2_donor_data_available (false),

     sysdonor_state      (SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE),
     proc_sysdonor_state (SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE),

     _sysdonor_state_msg(Callback<Screen_SYSDONOR>(this, &Screen_SYSDONOR::new_sysdonor_state),
                         MessageBase::SNDRCV_RECEIVE_ONLY)

{

   // STT: Load up the state management tables (a virtual function, so make sure
   // we call the right one).
   Screen_SYSDONOR::load_state_management_tables();

}   // END of Screen_SYSDONOR CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_SYSDONOR::~Screen_SYSDONOR()
{
   //  Note allocation and deallocation are unique for sysdonor so child objects
   //  are only deleted upon destruction.
   //
   // Remove the Screen Event Messages
   _msgAcPrimeComplete.deregisterMsg();
   _msgNewDonorInfo.deregisterMsg();
   _msgEnableLoadSystem.deregisterMsg();

   // deallocate all constituent objects first
   btn_system_setup.deallocate_resources ();
   btn_donor_info.deallocate_resources ();

   delete bitmap_donor_picture;
   bitmap_donor_picture = 0;

   bitmap_donor_top.deallocate_resources ();
   bitmap_donor_bottom.deallocate_resources ();
   bitmap_donor_left.deallocate_resources ();
   bitmap_donor_right.deallocate_resources ();

   // if not allocated, this acts as a noop.
   btn_cancel.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.deallocate_resources ();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}   // END of Screen_SYSDONOR DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
//
//   Allocate and deallocate have special meanings for the sysdonor screen.  To
//   keep backwards compatibility with the Photon code, and all of the legacy
//   statemanager/action implementation, this screen is always "allocated".
//   Subsequent calls to allocate / deallocate do not actually add or remove
//   constituents, but only set visibility.
//
void Screen_SYSDONOR::allocate_resources (const char* allocation_parameter)
{
   bool first_allocation = !_allocated;

   // add screen to window manager if needed
   if ( first_allocation )
   {
      Base_Apheresis_Window::allocate_resources (allocation_parameter);
   }

   // Remove the goback allocation
   btn_goback.deallocate_resources();


   // Link the status bar/line to this window
   link_status_bar_line();

   if (first_allocation)
   {
      // Set up the receive messages (from Procedure) that enable the
      //  System Load and Begin Run buttons.  Only do once.
      _msgEnableLoadSystem.init(Callback<Screen_SYSDONOR>(this, &Screen_SYSDONOR::enable_load_system), MessageBase::SNDRCV_RECEIVE_ONLY);
      _msgAcPrimeComplete.init(Callback<Screen_SYSDONOR>(this, &Screen_SYSDONOR::ac_prime_complete), MessageBase::SNDRCV_RECEIVE_ONLY);
      _msgNewDonorInfo.init(Callback<Screen_SYSDONOR>(this, &Screen_SYSDONOR::new_donor_info), MessageBase::SNDRCV_RECEIVE_ONLY);

      // Connect with the AP2 Interface Agent
      connect_with_AP2_Interface();

      enable_standard_buttons();
   }

   // If the GUI was reset while SYSDONOR was NOT visible, don't recalculate the screen display state
   // or perform the niceties of returning from a non-state-affecting screen, just reset this flag.
   // Otherwise: perform these housekeeping functions.
   // Added: guiglobs::currentlyInPreConnectDonorDataCheck means that the operator just backed out of the
   // confirm screen, which has absolutely no affect on the state of this screen.
   if (system_was_reset || guiglobs::currentlyInPreConnectDonorDataCheck)
   {
      if (guiglobs::currentlyInPreConnectDonorDataCheck)
      {
         // This screen no longer cares about it, and the condition is
         // no longer true, so set bool to false
         guiglobs::currentlyInPreConnectDonorDataCheck = false;

         // We disabled these buttons when we left this screen.  Enable them again.
         enable_standard_buttons();
      }

      if (system_was_reset)
         system_was_reset = false; // just reset the reset-while-not-displayed-flag
   }
   else
   {
      // Check for adjustments that must be made when coming from a
      //  screen that doesn't affect the display state
      non_display_state_adjustments();


      // Determine the current SYSDONOR display state at this allocation
      post_current_screen_display_state();


      // Get the ID of the previous screen from the screen status object
      const GUI_SCREEN_ID previous_screen = guiglobs::screen_status.get_previous_screen();

      // Check to see if we came from the DONORDL screen.  If so, then the donor was rejected.
      if (previous_screen == GUI_SCREEN_CONFIRM_DON_INFO)
      {
         remove_donor_data_availability();
      }
   }

   // Now assert the current state on the screen
   state_management.unconditional_state_check();
} // END of Screen_SYSDONOR ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// NON_DISPLAY_STATE_ADJUSTMENTS
void Screen_SYSDONOR::non_display_state_adjustments ()
{
   // These screens don't affect the "display state" of the SYSDONOR
   // screen (in terms of major task completed, system load or donor data).
   // They do, however, require some different kind of house (screen) cleaning based
   // on their unique context.

   // Get the ID of the previous screen from the screen status object
   GUI_SCREEN_ID previous_screen = guiglobs::screen_status.get_previous_screen();

   switch (previous_screen)
   {
      case GUI_SCREEN_CONFIRM_DON_INFO :

         // If it was DONOR Download, we must assume that this was a donor reject
         //  and we need to display the status line message that we're waiting
         //  for new (improved) donor data to be transmitted from AP2.
#if (CPU != SIMNT) // This line screws things up for screenshot scripts, so turn it off for SIMNT
         guiglobs::apheresis_status_line->set_current_text (textStatAwaitingAp2Xmit);
#endif

         // We must also turn off "active" donor data status bar indication
         //  if we haven't completed either leg of preparation
         if (sysdonor_state == SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE)
         {
            guiglobs::apheresis_status_bar->reset_all_phases();
         }

         break;

      case GUI_SCREEN_CONFIG :

         // Check if this screen is being displayed after returning from the
         //  config screen. This return should reset the status line
         //  in case of any residual messages from config.
         guiglobs::apheresis_status_line->reset_status_line();

         // If we just came back from the config screen, enable button if setup not complete.
         if (sysdonor_state == SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE)
         {
            btn_system_setup.enable_callbacks();
         }

         break;

      case GUI_SCREEN_DONCONN :

         // Although, we no longer travel from Donor Connected Screen back to
         //  this screen, the old Sysdonor code had contingent code to make
         //  sure that should this happen, the config button would be gone. This
         //  is here for completeness with the past.

         btn_lowleft.deallocate_resources();

         break;

      case GUI_SCREEN_SYSDISP :

         // We must also turn off "active" system load status bar indication
         //  if we haven't completed either leg of preparation

         if (sysdonor_state == SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE)
         {
            guiglobs::apheresis_status_bar->reset_all_phases ();
            // Get rid of any attention messages (but not alarm messages)
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

            break;
         }

         // If we return from the sysdisp side after having completed donor info, then
         // just make sure that no messages (except alarm) are displayed on the status line.

         if (sysdonor_state == SETUP_NOTCOMPLETE_DONOR_COMPLETE)
         {
            // Get rid of any attention messages (but not alarm messages)
            guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
            // Set the current default status line to blank.
            guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);

            break;
         }

         break;

      default :
         break; // no action here.
   }  // End of Switch statement

}  // END of Screen_SYSDONOR NON_DISPLAY_STATE_ADJUSTMENTS



////////////////////////////////////////////////////////////////////////////////
// NEW_DONOR_DATA_AVAILABILITY
void Screen_SYSDONOR::new_donor_data_availability ()
{
   ap2_donor_data_available = true;

   // Post that new donor data has arrived - but don't do state transitioning
   //  if the screen is not allocated.
   state_management.post_event_nocheck (SD_AP2_DONOR_DATA_AVAILABILITY, SD_TRUE);

   // If the screen is allocated, try state transitioning
   if (_allocated)
   {
      state_management.unconditional_state_check();
   }

}  // END of Screen_SYSDONOR NEW_DONOR_DATA_AVAILABILITY



////////////////////////////////////////////////////////////////////////////////
// CONNECT_WITH_AP2_INTERFACE
void Screen_SYSDONOR::connect_with_AP2_Interface ()
{
   // Ensure that this routine is only called once.
   static bool           connect_with_VIP       = true;
   static int            MAX_WAIT_FOR_INTERFACE = 10;

   const struct timespec waitTime               = {1, 0}; // One second delay

   if (connect_with_VIP)
   {
      connect_with_VIP = false;
   }
   else
   {
      return;
   }

#if (CPU != SIMNT)

   // Attempt to find out whether AP2 is in control or not.
   // Wait for either:
   //  (1) up to MAX_WAIT_FOR_INTERFACE seconds establishing this fact... otherwise terminate.
   //  (2) Broadcast cycle to be complete or Vista to connect.
   for (int i = 0; i < MAX_WAIT_FOR_INTERFACE; i++)
   {
      if (guiglobs::cdsData.xif.broadcastComplete.Get() || guiglobs::cdsData.xif.vistaConnected.Get())
         break;

      DataLog(log_level_gui_info) << " AP2-waiting for vip response (" << i << ")" << endmsg;
      nanosleep (&waitTime, 0);
   }
#endif
   if (guiglobs::cdsData.xif.vistaInControl.Get())
   {
      DataLog(log_level_gui_info) << "Vista-In Control" << endmsg;

      // register that ap2 IS in control in the screen state
      state_management.post_event_nocheck (SD_AP2_IN_CONTROL, SD_TRUE);
   }
   else
   {
      DataLog(log_level_gui_info) << "AP2-NOT In Control" << endmsg;
   }
}  // END of Screen_SYSDONOR CONNECT_WITH_AP2_INTERFACE



////////////////////////////////////////////////////////////////////////////////
// REMOVE_DONOR_DATA_AVAILABILITY
void Screen_SYSDONOR::remove_donor_data_availability ()
{
   ap2_donor_data_available = 0;

   // Post that donor data is not available - don't check for state transitions
   //  this will be used in the Sysdonor_Action class when transferring control
   //  to the Donor Download screen.
   state_management.post_event_nocheck (SD_AP2_DONOR_DATA_AVAILABILITY, SD_FALSE);

   // Send a message to Vista telling them that the donor was rejected.
   VistaDonorRejectedMsg reject(MessageBase::SEND_GLOBAL);
   reject.send(0);

   // Set the flag that the donor was rejected (undownloaded).
   // This is done here to solve IT 5894.
   External_IF_CDS _xifCds(ROLE_RW);
   _xifCds.donorDownloaded.Set(false);

   // Set the internal flag that the procedure was (un)selected
   guiglobs::procedure_selected = false;

   Message<ProcedureInformationStruct> clearProcedureMsg(MessageBase::SEND_LOCAL);
   ProcedureInformationStruct          nullProc;
   memset(&nullProc, 0, sizeof(nullProc));
   clearProcedureMsg.send(nullProc);

   DataLog(log_level_gui_info) << "Clearing procedure information" << endmsg;

}  // END of Screen_SYSDONOR REMOVE_DONOR_DATA_AVAILABILITY



////////////////////////////////////////////////////////////////////////////////
// POST_CURRENT_SCREEN_DISPLAY_STATE
void Screen_SYSDONOR::post_current_screen_display_state ()
{
   SYSDONOR_STATE next_state;


   struct SYSDONOR_STD
   {
      SYSDONOR_STATE current_sysdonor_state;
      GUI_SCREEN_ID  last_screen;
      SYSDONOR_STATE next_sysdonor_state;
   }


   const sysdon_std[] =
   {  //      current sysdonor state               prev screen          next state
      {SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE,  GUI_SCREEN_SYSACATT,       SETUP_COMPLETE_DONOR_NOTCOMPLETE},
      {SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE,  GUI_SCREEN_PREDICT,        SETUP_NOTCOMPLETE_DONOR_COMPLETE},
      {SETUP_COMPLETE_DONOR_NOTCOMPLETE,     GUI_SCREEN_PREDICT,        SETUP_COMPLETE_DONOR_COMPLETE},
      {SETUP_NOTCOMPLETE_DONOR_COMPLETE,     GUI_SCREEN_SYSACATT,       SETUP_COMPLETE_DONOR_COMPLETE},
      {SETUP_COMPLETE_DONOR_COMPLETE,        GUI_SCREEN_PREDICT,        SETUP_COMPLETE_DONOR_COMPLETE},
      {SETUP_COMPLETE_DONOR_COMPLETE,        GUI_SCREEN_DONCONN,        SETUP_COMPLETE_DONOR_COMPLETE},
      {SETUP_COMPLETE_DONOR_COMPLETE,        GUI_SCREEN_RUNSUMM2,       SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE},
      {SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE,  GUI_SCREEN_POWERUP,        SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE},
      {SETUP_COMPLETE_DONOR_NOTCOMPLETE,     GUI_SCREEN_POWERUP,        SETUP_COMPLETE_DONOR_NOTCOMPLETE},
      {SETUP_COMPLETE_DONOR_COMPLETE,        GUI_SCREEN_POWERUP,        SETUP_COMPLETE_DONOR_COMPLETE},
      {SETUP_COMPLETE_DONOR_COMPLETE,        GUI_SCREEN_CONFIRM_DON_INFO,       SETUP_COMPLETE_DONOR_NOTCOMPLETE},
      {SETUP_NOTCOMPLETE_DONOR_COMPLETE,     GUI_SCREEN_CONFIRM_DON_INFO,       SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE}
   };

   static int max_sysdon_states = sizeof(sysdon_std) / sizeof (SYSDONOR_STD);

   // Get the ID of the previous screen from the screen status object
   GUI_SCREEN_ID previous_screen = guiglobs::screen_status.get_previous_screen();

   // Initialize the next state to an invalid one to detect a "no table match" condition.
   next_state = SYSDONOR_INVALID_STATE;

   // Walk through the STD table trying to match on current state and last screen - once found,
   // we'll assign the next sysdonor state. If we do not find a valid state-change condition,
   // we leave the screen alone.
   for (int i = 0; i < max_sysdon_states; i++)
   {
      if ((sysdon_std[i].current_sysdonor_state == sysdonor_state) &&
          (sysdon_std[i].last_screen ==  previous_screen))
      {
         next_state = sysdon_std[i].next_sysdonor_state;
      }
   }

   // If we found a valid state change, post the state change.
   if (next_state != SYSDONOR_INVALID_STATE)
   {
      sysdonor_state = next_state;
      state_management.post_event_nocheck (SD_SCREEN_DISPLAY_STATE, sysdonor_state);
   }
}  // END of Screen_SYSDONOR POST_CURRENT_SCREEN_DISPLAY_STATE



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SYSDONOR::deallocate_resources ()
{
   // Unparent the status bar/line from this window
   unlink_status_bar_line();

   detach();
}   // END of Screen_SYSDONOR DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// LOAD_STATE_MANAGEMENT_TABLES
void Screen_SYSDONOR::load_state_management_tables ()
{
   // This file contains all the state and action object "news" (allocations)
   // and linkage calls into their respective collection objects. It's big and
   // boring and It needn't be seen in-line here.
   #include "sysdstab.hpp"

}   // END of Screen_SYSDONOR LOAD_STATE_MANAGEMENT_TABLES



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_SYSDONOR::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   guiglobs::currentlyInPreConnectDonorDataCheck = true;

   goto_screen(GUI_SCREEN_CONFIRM_DON_INFO, "GUI_SCREEN_CONFIRM_DON_INFO");
}   // END of Screen_SYSDONOR PROCESS_CONTINUE_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Screen_SYSDONOR::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}   // END of Screen_SYSDONOR PROCESS_CANCEL_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON
void Screen_SYSDONOR::process_lowleft_button ()
{

   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Switch to the initial configuration screen since the lower-
   //  left standard button has transmogrified into the configuration
   //  button on the SYSDONOR screen.
   goto_screen (GUI_SCREEN_CONFIG, "GUI_SCREEN_CONFIG");

}   // END of Screen_SYSDONOR PROCESS_LOWLEFT_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_SYSTEM_SETUP_BUTTON
void Screen_SYSDONOR::process_system_setup_button ()
{
   // goto the load disposables screen
   goto_screen (GUI_SCREEN_SYSCASSETTE, "GUI_SCREEN_SYSCASSETTE");
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_DONOR_INFO_BUTTON
void Screen_SYSDONOR::process_donor_info_button ()
{
   if (ap2_donor_data_available)
   {
      // now goto the donordl screen
      goto_screen (GUI_SCREEN_CONFIRM_DON_INFO, "AP2 Donor Download Screen");
   }
   else
   {
      // Display the DONOR VITALS screen
      goto_screen (GUI_SCREEN_DONVITAL, "GUI_SCREEN_DONVITAL");
   }
}   // END of Screen_SYSDONOR PROCESS_DONOR_INFO_BUTTON



////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE
void Screen_SYSDONOR::reset_internal_screen_state ()
{
   // Have the state method reset to the initial state
   sysdonor_state = proc_sysdonor_state;

   // Reset the state managers internal state
   state_management.post_event_nocheck (SD_DISPLAY_LOAD_SYSTEM, SD_FALSE);
   state_management.post_event_nocheck (SD_ENABLE_BEGIN_RUN, SD_FALSE);
   state_management.post_event_nocheck (SD_AP2_DONOR_DATA_AVAILABILITY, SD_FALSE);
   state_management.post_event_nocheck (SD_SCREEN_DISPLAY_STATE, sysdonor_state);

   // Purposely DO NOT change the AP2 in control mode - this can only occur via a reboot.
   // state_management.post_event_nocheck (SD_AP2_IN_CONTROL, SD_FALSE);

   // Note:  Code used to use _allocated to determine if the screen was realized or not.  That doesn't work anymore
   // since the screen is always allocated and deallocate resources doesn't delete the objects anymore.  The new
   // and safer equivalent is to see if we are linked to the status bar.  This reproduces the same intent as
   // before.
   if (Base_Apheresis_Window::get_status_bar_is_linked())
   {
      // Force a review of the screen state if it's currently displayed.
      state_management.unconditional_state_check();
   }
   else
   {
      // Remember that a GUI reset occurred while SYSDONOR was unallocated
      system_was_reset = true; // this will be used at the next screen allocation to decide whether or not to calculate the screen display state.
   }

}   // END of Screen_SYSDONOR RESET_INTERNAL_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// DISABLE_STANDARD_BUTTONS
void Screen_SYSDONOR::disable_standard_buttons ()
{
   // First disable all the standard buttons
   Base_Apheresis_Window::disable_standard_buttons();

   btn_system_setup.disable_callbacks();
   btn_donor_info.disable_callbacks();
}   // END of Screen_SYSDONOR DISABLE_STANDARD_BUTTONS



void Screen_SYSDONOR::enable_standard_buttons ()
{
   // First disable all the standard buttons
   Base_Apheresis_Window::enable_standard_buttons();

   btn_system_setup.enable_callbacks();
   btn_donor_info.enable_callbacks();
}   // END of Screen_SYSDONOR DISABLE_STANDARD_BUTTONS



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_SYSDONOR::enable_load_system ()
{
   // Let Procedure know that GUI got the message.
   ConfirmEnableLoadSystemMsg confirm(MessageBase::SEND_GLOBAL);
   confirm.send(0);

   // Post this new state component - but don't check for transition
   state_management.post_event_nocheck (SD_DISPLAY_LOAD_SYSTEM, SD_TRUE);

   External_IF_CDS _xifCds(ROLE_RW);

   // Notify TRAP that the system state has changed
   _xifCds.loadSetReady.Set(true);

   // If the screen is allocated, try state transitioning
   if (_allocated)
   {
      state_management.unconditional_state_check();
   }
}



void Screen_SYSDONOR::ac_prime_complete ()
{
   // only dismiss ATTENTION_STATUS_LINE if it is textStatAcPriming
   if (guiglobs::apheresis_status_line)
   {
      // Remove AC priming message
      guiglobs::apheresis_status_line->deactivate_status_line_text_type(textStatAcPriming, ATTENTION_STATUS_LINE);
   }

   // Post this new state component - but don't check for transition
   state_management.post_event_nocheck (SD_ENABLE_BEGIN_RUN, SD_TRUE);

   // If the screen is allocated, try state transitioning
   if (_allocated)
   {
      state_management.unconditional_state_check();
   }
}



void Screen_SYSDONOR::new_donor_info ()
{
   const int from = _msgNewDonorInfo.getData();

   if (from == RECEIVED_FROM_AP2AGENT)
   {
      new_donor_data_availability();
   }
}   // END of Screen_SYSDISP SCREEN_NOTIFY_HANDLER



////////////////////////////////////////////////////////////////////////////////
// GET_SYSDONOR_STATE
SYSDONOR_STATE Screen_SYSDONOR::get_sysdonor_state ()
{
   return sysdonor_state;
}   // END of Screen_SYSDONOR GET_SYSDONOR_STATE



////////////////////// PHOTON C CALLBACK RELAY ROUTINES ////////////////////////


////////////////////////////////////////////////////////////////////////////////
// SYSDONOR_CB_SYSTEM_SETUP
void sysdonor_cb_system_setup (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_SYSDONOR*)data)->process_system_setup_button();

}   // END of SYSDONOR_CB_SYSTEM_SETUP



////////////////////////////////////////////////////////////////////////////////
// SYSDONOR_CB_DONOR_INFO
void sysdonor_cb_donor_info (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_SYSDONOR*)data)->process_donor_info_button();

}   // END of SYSDONOR_CB_DONOR_INFO



//
// new_sysdonor_state
//
void Screen_SYSDONOR::new_sysdonor_state ()
{
   const int state = _sysdonor_state_msg.getData();

   DataLog(log_level_gui_info) << state << " Sys Donor state message" << endmsg;

   if (state >= SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE && state < SYSDONOR_INVALID_STATE)
   {
      sysdonor_state      = (SYSDONOR_STATE)state;
      proc_sysdonor_state = (SYSDONOR_STATE)state;

      DataLog(log_level_gui_info) << sysdonor_state <<  " Changing screen state" << endmsg;
   }
}



//
// refresh_resources
//
void Screen_SYSDONOR::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();

   // Determine the current SYSDONOR display state at this allocation
   // post_current_screen_display_state();

   // Now assert the current state on the screen
   state_management.unconditional_state_check();
}

void Screen_SYSDONOR::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_color(TEAL);
}


#if (CPU==SIMNT)

void Screen_SYSDONOR::unitTestDisplay (bool loadSetDone, bool donorInfoDone, TEXT_STRING_ID statLine = textEmpty)
{
   DataLog (log_level_gui_info) << "unitTestDisplay(" << loadSetDone << "; "
                                << donorInfoDone << "; " << statLine << ");" << endmsg;

   btn_lowleft.set_string_id(textButtonConfig);
   btn_continue.set_string_id(textButtonLongBeginDonorPrep);
   btn_donor_info.set_string_id (textButtonDonorInfo);

   btn_cancel.deallocate_resources();
   btn_continue.deallocate_resources();
   btn_lowleft.deallocate_resources();

   if (statLine == textEmpty)
   {
      guiglobs::apheresis_status_line->deactivate_status_line_type(DEFAULT_STATUS_LINE);
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   }
   else guiglobs::apheresis_status_line->set_current_status_line_text_by_type(ATTENTION_STATUS_LINE, statLine);

   if (loadSetDone)
   {
      btn_system_setup.set_up_bitmap (BITMAP_BUTTON_SYS_SETUP_UP_CHKED);
      btn_system_setup.set_string_id (textButtonGiantLoadSystem);
      btn_system_setup.set_button_text_color(GREY_LIGHT);

      if (donorInfoDone)
      {
         btn_cancel.allocate_resources(*this);
         btn_continue.allocate_resources(*this);
         btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP_CHKED);
         btn_donor_info.set_button_text_color(GREY_LIGHT);
      }
      else
      {
         btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP);
         btn_donor_info.set_button_text_color(YELLOW);
      }
   }
   else
   {
      btn_system_setup.set_up_bitmap (BITMAP_BUTTON_SYS_SETUP_UP);
      btn_system_setup.set_string_id (textButtonGiantLoadSystem);
      btn_system_setup.set_button_text_color(YELLOW);

      if (donorInfoDone)
      {
         btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP_CHKED);
         btn_donor_info.set_button_text_color(GREY_LIGHT);
      }
      else
      {
         btn_lowleft.allocate_resources(*this);
         btn_donor_info.set_up_bitmap (BITMAP_BUTTON_DONOR_INFO_UP);
         btn_donor_info.set_button_text_color(YELLOW);
      }
   }
}


void Screen_SYSDONOR::handleTraverseMessage ()
{
   // Note that the way the current GUI is written, this screen never gets deallocated.
   // That means that this class will listen for the unit testing message whether it's
   // currently displayed or not.
   //
   // For this reason, you probably shouldn't handle the universal message, because
   // it'll get called more often than you may mave intended. You've been warned.

   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == SYSDONOR_UNIVERSAL)
   {
      if (guiglobs::screen_status.get_current_displayed_screen() == GUI_SCREEN_SYSDONOR)
      {
         bool           loadSetDone        = false;
         bool           donorInfoDone      = false;
         bool           vistaConnectedDone = false;
         TEXT_STRING_ID statLine           = textEmpty;
         State_names    systemState        = STARTUP_TESTS;

         DataLog (log_level_sim_gui_side_info) << "Two button screen got display type " << (SYSDONOR_DISPLAY_TYPES)traverseInfo.parameters[0] << endmsg;

         switch ((SYSDONOR_DISPLAY_TYPES)traverseInfo.parameters[0])
         {
            case SYSDONOR_DISPLAY_INITIAL : break;
            case SYSDONOR_DISPLAY_DONINFO_DONE : donorInfoDone        = true; break;
            case SYSDONOR_DISPLAY_LOADSET_DONE : loadSetDone          = true; systemState = AC_PRIME; break;
            case SYSDONOR_DISPLAY_LOADSET_DONE_AC_PRIME : loadSetDone = true; statLine = textStatAcPriming; systemState = AC_PRIME; break;
            case SYSDONOR_DISPLAY_BOTH_DONE : loadSetDone             = true; donorInfoDone = true; systemState = AC_PRIME; break;
            case SYSDONOR_DISPLAY_VIP_DOWNLOADING : statLine          = textStatAwaitingAp2Xmit; break;
            default :
               DataLog (log_level_sim_gui_side_error) << "Unrecognized code for sysdonor." << endmsg;
               break;
         }

         unitTestDisplay(loadSetDone, donorInfoDone, statLine);
         guiglobs::apheresis_status_bar->setState(systemState);
      }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH 39db8ef630def05b2809b0617b7675a4 */
