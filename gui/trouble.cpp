/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_TROUBLE

*     File name:   trouble.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the ADJUST screen

*************************< FOCUSSED GUI DOMAIN >****************************/

#include <vxworks.h>
#include <math.h>

// header file include
#include "trouble.hpp"
#include "guiglobs.hpp"

#include "an_alarm.h"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "guipalette.h"

#include "predict_manager.hpp"
#include "adjust_manager.hpp"

// define meter standards
static const int METER_DRAW_X     = 35;
static const int METER_DRAW_Y     = 255;
static const int METER_DRAW_W     = 94;
static const int METER_DRAW_H     = 115;

static const int METER_RETURN_X   = 138;
static const int METER_RETURN_Y   = 255;
static const int METER_RETURN_W   = 94;
static const int METER_RETURN_H   = 115;

static const int METER_TINGLING_X = 258;
static const int METER_TINGLING_Y = 255;
static const int METER_TINGLING_W = 94;
static const int METER_TINGLING_H = 115;

static const int METER_CLUMPING_X = 383;
static const int METER_CLUMPING_Y = 255;
static const int METER_CLUMPING_W = 94;
static const int METER_CLUMPING_H = 115;

// callbacks for operator buttons
static void callback_spillover (void* data);
static void callback_air_in_plasma_line (void* data);
static void callback_saline_bolus (void* data);
static void callback_adjust_draw_up (void* data);
static void callback_adjust_draw_down (void* data);
static void callback_adjust_return_up (void* data);
static void callback_adjust_return_down (void* data);
static void callback_adjust_ac_reaction_up (void* data);
static void callback_adjust_ac_reaction_down (void* data);
static void callback_adjust_clumping_up (void* data);
static void callback_adjust_clumping_down (void* data);



int Screen_TROUBLE::tot_instance_cnt = 0;

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_TROUBLE::Screen_TROUBLE()
   : Base_Apheresis_Window (BLUE, GUI_SCREEN_TROUBLE),

     current_proc_data(guiglobs::proc_status.get_data()),

     _exiting           (false),
     _exited            (false),
     _predict_requested (false),
     _send_is_valid     (false),
     _wait_is_valid     (false),

     previous_remaining_proc_time (-1),
     timeout_occurred             (false),

     my_instance_cnt(++tot_instance_cnt),

     btn_draw_up                 (GUI_BUTTON_ADJUST_DRAW_UP,
                                  BUTN_DRAW_INCR_X,
                                  BUTN_DRAW_INCR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_DRAW_INCR_UP,
                                  BITMAP_BUTTON_DRAW_INCR_DN),

     btn_draw_down               (GUI_BUTTON_ADJUST_DRAW_DN,
                                  BUTN_DRAW_DECR_X,
                                  BUTN_DRAW_DECR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_DRAW_DECR_UP,
                                  BITMAP_BUTTON_DRAW_DECR_DN),

     btn_return_up               (GUI_BUTTON_ADJUST_RETURN_UP,
                                  BUTN_RETURN_INCR_X,
                                  BUTN_RETURN_INCR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_RETURN_INCR_UP,
                                  BITMAP_BUTTON_RETURN_INCR_DN),

     btn_return_down             (GUI_BUTTON_ADJUST_RETURN_DN,
                                  BUTN_RETURN_DECR_X,
                                  BUTN_RETURN_DECR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_RETURN_DECR_UP,
                                  BITMAP_BUTTON_RETURN_DECR_DN),

     btn_ac_reaction_up          (GUI_BUTTON_ADJUST_AC_UP,
                                  BUTN_AC_REACTION_INCR_X,
                                  BUTN_AC_REACTION_INCR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_AC_REACTION_INCR_UP,
                                  BITMAP_BUTTON_AC_REACTION_INCR_DN),

     btn_ac_reaction_down        (GUI_BUTTON_ADJUST_AC_DN,
                                  BUTN_AC_REACTION_DECR_X,
                                  BUTN_AC_REACTION_DECR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_AC_REACTION_DECR_UP,
                                  BITMAP_BUTTON_AC_REACTION_DECR_DN),

     btn_clumping_up             (GUI_BUTTON_ADJUST_CLUMP_UP,
                                  BUTN_CLUMPING_INCR_X,
                                  BUTN_CLUMPING_INCR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_CLUMPING_INCR_UP,
                                  BITMAP_BUTTON_CLUMPING_INCR_DN),

     btn_clumping_down           (GUI_BUTTON_ADJUST_CLUMP_DN,
                                  BUTN_CLUMPING_DECR_X,
                                  BUTN_CLUMPING_DECR_Y,
                                  textEmpty,
                                  BITMAP_BUTTON_CLUMPING_DECR_UP,
                                  BITMAP_BUTTON_CLUMPING_DECR_DN),

     btn_spillover               (GUI_BUTTON_ADJUST_SPILLOVER,
                                  BUTN_SPILLOVER_X,
                                  BUTN_SPILLOVER_Y,
                                  textButtonAdjustSpillover,
                                  BITMAP_BUTTON_SPILLOVER_UP,
                                  BITMAP_BUTTON_SPILLOVER_DN,
                                  false, false),

     btn_spillover_grey          (GUI_BUTTON_ADJUST_SPILLOVER,
                                  BUTN_SPILLOVER_X,
                                  BUTN_SPILLOVER_Y,
                                  textButtonAdjustSpillover,
                                  BITMAP_BUTTON_SPILLOVER_GREY,
                                  BITMAP_NULL,
                                  false, false),

     btn_air_in_plasma           (GUI_BUTTON_ADJUST_AIR_PLASMA,
                                  BUTN_AIR_IN_PLASMA_X,
                                  BUTN_AIR_IN_PLASMA_Y,
                                  textButtonAdjustAirInPlasma,
                                  BITMAP_BUTTON_AIR_IN_PLASMA_UP,
                                  BITMAP_BUTTON_AIR_IN_PLASMA_DN,
                                  false, false),

     btn_air_in_plasma_grey      (GUI_BUTTON_ADJUST_AIR_PLASMA,
                                  BUTN_AIR_IN_PLASMA_X,
                                  BUTN_AIR_IN_PLASMA_Y,
                                  textButtonAdjustAirInPlasma,
                                  BITMAP_BUTTON_AIR_IN_PLASMA_GREY,
                                  BITMAP_NULL,
                                  false, false),

     btn_saline_bolus            (GUI_BUTTON_ADJUST_SALINE_BOLUS,
                                  BUTN_SALINE_BOLUS_X,
                                  BUTN_SALINE_BOLUS_Y,
                                  textButtonAdjustSalineBolus,
                                  BITMAP_BUTTON_SALINE_BOLUS_UP,
                                  BITMAP_BUTTON_SALINE_BOLUS_DN,
                                  false, false),

     btn_saline_bolus_grey       (GUI_BUTTON_ADJUST_SALINE_BOLUS,
                                  BUTN_SALINE_BOLUS_X,
                                  BUTN_SALINE_BOLUS_Y,
                                  textButtonAdjustSalineBolus,
                                  BITMAP_BUTTON_SALINE_BOLUS_GREY,
                                  BITMAP_NULL,
                                  false, false),


     bitmap_donation_time        (SECONDARY_INLAY_X,
                                  SECONDARY_INLAY_Y,
                                  BITMAP_SECONDARY_INLAY),

     bitmap_draw_return          (DRAW_RETURN_X,
                                  DRAW_RETURN_Y,
                                  BITMAP_DRAW_RETURN),

     bitmap_ac_reaction          (AC_REACTION_X,
                                  AC_REACTION_Y,
                                  BITMAP_AC_REACTION),

     bitmap_clumping             (CLUMPING_X,
                                  CLUMPING_Y,
                                  BITMAP_CLUMPING),

     text_donation_time_display   (textMiscAdjustTimeInit),
     text_donation_time_remaining (textMiscAdjustDonationTimeRemaining),
     text_draw_label              (textMiscAdjustMeterLabelDraw),
     text_return_label            (textMiscAdjustMeterLabelReturn),
     text_ac_reaction_label       (textMiscAdjustMeterLabelAcReaction),
     text_clumping_label          (textMiscAdjustMeterLabelClumping),



     _screenTimeout(SCREEN_TIMEOUT_PERIOD, Callback<Screen_TROUBLE>(this,
                                                                    &Screen_TROUBLE::screen_timeout),
                    TimerMessage::DISARMED)

{

   DataLog(log_level_gui_info) << "Screen_TROUBLE: constructor, this=" << this
                               << " my_cnt=" << my_instance_cnt
                               << " tot_cnt=" << tot_instance_cnt << endmsg;

   // screen initialization will refresh all relevant screen allocation or
   // construction parameters for display text and system states.  However, don't call
   // reset_internal_screen_state, since it's virtual and we're in a ctor; call the nonvirtual
   // function that reset_internal_screen_state forwards to.  --DGS
   screen_initialization ();
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_TROUBLE::~Screen_TROUBLE()
{
   tot_instance_cnt--;

   DataLog(log_level_gui_info) << "Screen_TROUBLE: destructor, this=" << this
                               << " my_cnt=" << my_instance_cnt
                               << " tot_cnt=" << tot_instance_cnt
                               << endmsg;

   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_TROUBLE::deallocate_resources();
}





////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_TROUBLE::allocate_resources (const char* allocation_parameter)
{
   DataLog(log_level_gui_info) << "Screen_TROUBLE: allocate, this=" << this << endmsg;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // reset all screen parameters to prepare for refresh from procedure
   // status run echo below
   screen_initialization ();

   enable_callbacks();
   guiglobs::proc_status.enable_access_pressure_low_warning(true);


   // Create the screen timeout timer that will return the user to
   // the appropriate screen
   timeout_occurred = false;
   timeout_reset_manager();

   // now deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();

   // If this screen was allocated statelessly, get rid of the end run
   // button.  Because of the screen priority, it displays beneath this
   // one, which is both wierd and kind of memory-intensive.
   if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS)
      btn_cancel.deallocate_resources();

   // set the callback on the standard buttons
   btn_continue.set_callback (aph_callback_continue, (void*)this);

   // now allocate "non-standard" widgets that are needed not based on
   // system state

   // the placard bitmap objects
   bitmap_donation_time.allocate_resources (*this);

   // the display text objects
   text_donation_time_remaining.allocate_resources (*this);
   text_donation_time_display.allocate_resources (*this);

   // the placard bitmap objects
   bitmap_draw_return.allocate_resources (*this);
   bitmap_clumping.allocate_resources (*this);
   bitmap_ac_reaction.allocate_resources (*this);

   // the display text objects
   text_draw_label.allocate_resources (*this);
   text_return_label.allocate_resources (*this);
   text_ac_reaction_label.allocate_resources (*this);
   text_clumping_label.allocate_resources (*this);

   // now get the text that will be used in dynamic screen construction
   get_dynamic_text();

   // Link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to donate
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_DONATE);

   meter_draw.Initialize(*this, METER_DRAW_X, METER_DRAW_Y, METER_DRAW_H, METER_DRAW_W, guipalette::meter_yellow);
   meter_return.Initialize(*this, METER_RETURN_X, METER_RETURN_Y, METER_RETURN_H, METER_RETURN_W, guipalette::meter_yellow);
   meter_tingling.Initialize(*this, METER_TINGLING_X, METER_TINGLING_Y, METER_TINGLING_H, METER_TINGLING_W, guipalette::meter_yellow);
   meter_clumping.Initialize(*this, METER_CLUMPING_X, METER_CLUMPING_Y, METER_CLUMPING_H, METER_CLUMPING_W, guipalette::meter_yellow);


   // now deallocate "standard" buttons that are not needed based on system
   // state
   if (current_proc_data.system_state != BLOOD_RINSEBACK)
   {
      // set the text on the lowleft button
      btn_lowleft.set_up_bitmap (BITMAP_BUTTON_PREDICT_BLUE_UP);
      // btn_lowleft.set_up_bitmap (BITMAP_BUTTON_SELECT_PRODUCT_BLUE);

      btn_lowleft.set_callback (aph_callback_lowleft, (void*)this);
   }
   else btn_lowleft.deallocate_resources ();


   //////  Uncomment the following to log GUI task stack usage.
   // TASK_DESC taskDescriptor;
   // taskInfoGet (taskIdSelf(), &taskDescriptor);

   // DataLog(log_level_gui_info) << "DGS:: winApp current stack margin is " << taskDescriptor.td_stackMargin
   //                             << " high water mark is " << taskDescriptor.td_stackHigh
   //                             << " size is " << taskDescriptor.td_stackSize << endmsg;

   updateScreenData();

} // END of Screen_TROUBLE ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
//
// Here's the actual functionality of deallocate_resources; we need a nonvirtual function so that we
// can call it from the dtor.
void Screen_TROUBLE::deallocate_resources ()
{
   DataLog(log_level_gui_info) << "Screen_TROUBLE: deallocate, this=" << this << endmsg;

   disable_callbacks();

   //   Turn off the meter objects
   //
   meter_draw.Active(0);
   meter_return.Active(0);
   meter_tingling.Active(0);
   meter_clumping.Active(0);

   _screenTimeout.armTimer(TimerMessage::DISARMED);

   // deallocate all non-standard widgets

   // the display text objects
   text_draw_label.deallocate_resources ();
   text_return_label.deallocate_resources ();
   text_ac_reaction_label.deallocate_resources ();
   text_clumping_label.deallocate_resources ();
   text_donation_time_remaining.deallocate_resources ();
   text_donation_time_display.deallocate_resources ();

   // the placard bitmap objects
   bitmap_donation_time.deallocate_resources ();
   bitmap_draw_return.deallocate_resources ();
   bitmap_ac_reaction.deallocate_resources ();
   bitmap_clumping.deallocate_resources ();

   // the donor info bitmap buttons
   btn_draw_up.deallocate_resources ();
   btn_draw_down.deallocate_resources ();
   btn_return_up.deallocate_resources ();
   btn_return_down.deallocate_resources ();
   btn_ac_reaction_up.deallocate_resources ();
   btn_ac_reaction_down.deallocate_resources ();
   btn_clumping_up.deallocate_resources ();
   btn_clumping_down.deallocate_resources ();
   btn_spillover.deallocate_resources ();
   btn_spillover_grey.deallocate_resources();
   btn_air_in_plasma.deallocate_resources ();
   btn_air_in_plasma_grey.deallocate_resources ();
   btn_saline_bolus.deallocate_resources ();
   btn_saline_bolus_grey.deallocate_resources ();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}  // END of Screen_TROUBLE DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
void Screen_TROUBLE::refresh_resources ()
{
   // Do what the base class does...
   Base_Apheresis_Window::refresh_resources();

   DataLog(log_level_gui_info) << "Screen_TROUBLE: refresh, this=" << this << endmsg;

   // plus, turn our callbacks back on.  This is problematic, since we may have
   // been preempted by the "Pause screen's trouble screen" instance(s).


   // restart our timeout timer from scratch
   timeout_reset_manager();
   enable_callbacks();


   updateScreenData();
}




////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE
// This method will reset the internal screen state
void Screen_TROUBLE::reset_internal_screen_state ()
{
   // screen initialization will refresh all relevant screen allocation or
   // construction parameters for display text and system states
   screen_initialization ();
}



////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION
void Screen_TROUBLE::screen_initialization ()
{
   //////////////////////////////////////////////////////////////////////////
   // values for initialization or reset
   // initialize pressure status and return pump values to blank, pending
   // reciept of first procedure status message

   // initialize all blood type flag to false separately until included in
   // donor_vitals_get_struct

   _exiting           = false;
   _exited            = false;
   _predict_requested = false;
   _send_is_valid     = false;
   _wait_is_valid     = false;

   AdjustManager::reset();

   // initialize donation time
   previous_remaining_proc_time = -1;

}  // End of SCREEN_INITIALZATION



////////////////////////////////////////////////////////////////////////////////
// GET_DYNAMIC_TEXT
void Screen_TROUBLE::get_dynamic_text ()
{
   Display_Text text;

   text.set_string_id(textDataUnitsAdjustTime);
   text.get_text_direct(units_time);

}  // End of GET_DYNAMIC_TEXT



////////////////////////////////////////////////////////////////////////////////
// FREEZE_SCREEN
// This method will disable all sceen components
void Screen_TROUBLE::freeze_screen ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   btn_draw_up          .disable_callbacks();
   btn_draw_down        .disable_callbacks();
   btn_return_up        .disable_callbacks();
   btn_return_down      .disable_callbacks();
   btn_ac_reaction_up   .disable_callbacks();
   btn_ac_reaction_down .disable_callbacks();
   btn_clumping_up      .disable_callbacks();
   btn_clumping_down    .disable_callbacks();
   btn_spillover        .disable_callbacks();
   btn_air_in_plasma    .disable_callbacks();
   btn_saline_bolus     .disable_callbacks();

}



////////////////////////////////////////////////////////////////////////////////
// THAW_SCREEN
// This method will disable all screen components
void Screen_TROUBLE::thaw_screen ()
{
   // Enable all the standard buttons on this screen
   enable_standard_buttons();

   btn_draw_up          .enable_callbacks();
   btn_draw_down        .enable_callbacks();
   btn_return_up        .enable_callbacks();
   btn_return_down      .enable_callbacks();
   btn_ac_reaction_up   .enable_callbacks();
   btn_ac_reaction_down .enable_callbacks();
   btn_clumping_up      .enable_callbacks();
   btn_clumping_down    .enable_callbacks();
   btn_spillover        .enable_callbacks();
   btn_air_in_plasma    .enable_callbacks();
   btn_saline_bolus     .enable_callbacks();

}



////////////////////////////////////////////////////////////////////////////////
// SCREEN_EXIT
void Screen_TROUBLE::screen_exit (GUI_SCREEN_ID gui_screen_transition,
                                  const char* allocation_param_transition)
{
   DataLog(log_level_gui_info) << "Screen_TROUBLE, exiting" << endmsg;

   disable_callbacks();

   // Send an indication to "goto screen" which is the transition screen or if
   // invoked as a stateless screen remove myself and display any other
   // active stateless screen.
   if (gui_screen_transition == GUI_SCREEN_ABORTCNF)
   {
      if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATIC)
      {
         // user desires back to goto the ABORTCNF screen as child
         goto_child_screen(gui_screen_transition, allocation_param_transition);
      }
      else if (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS)
      {
         // user desires back to goto the ABORTCNF screen as stateless
         goto_stateless_screen(gui_screen_transition, allocation_param_transition);
      }
      else
      {
         _FATAL_ERROR (__FILE__, __LINE__, "Unknown screen invocation type");
      }

   }
   else if ((gui_screen_transition == GUI_SCREEN_RUNPROC) &&
            (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS))
   {
      // user desires to remove stateless screen
      RemoveMostRecentStatelessScreenMsg req(MessageBase::SEND_LOCAL);
      req.send(0);
   }
   else if ((gui_screen_transition == GUI_SCREEN_PREDICT) &&
            (get_screen_invocation_type () == GUI_SCREEN_INVOKE_STATELESS))
   {
      // user desires back to goto the RUNPROC screen no repredict necessary
      // or goto the PREDICT screen for repredict but as stateless.
      swap_stateless_screen(gui_screen_transition, allocation_param_transition);
   }
   else
   {
      // user desires back to goto the RUNPROC screen no repredict necessary
      // or goto the PREDICT screen for repredict.
      goto_screen(gui_screen_transition, allocation_param_transition);
   }

}  // End of SCREEN_EXIT



////////////////////////////////////////////////////////////////////////////////
// TIMEOUT_RESET_MANAGER
void Screen_TROUBLE::timeout_reset_manager ()
{
   // Restart the timer
   _screenTimeout.armTimer(TimerMessage::ARMED);
}



////////////////////////////////////////////////////////////////////////////////
// PROCEDURE_STATUS_STRUCT_MANAGER
void Screen_TROUBLE::proc_status_notify ()
{
   if (_exited) return;

   // update time display if necessary.  Note, the second term is a wrapper
   // around an actual CDS that calls Get() on our behalf, which is why we don't
   // need it.
   int remaining_time = (int)ceil(guiglobs::cdsData.targets.ProcTimeTarget.Get()) - (int)(guiglobs::cdsData.time.ProcRunTime());

   if ( remaining_time < 0 ||
        ( current_proc_data.system_state != BLOOD_PRIME && current_proc_data.system_state != BLOOD_RUN ))
   {
      remaining_time = 0;
   }

   if (remaining_time != previous_remaining_proc_time)
   {
      char time_display[20];

      sprintf(time_display, "%d", remaining_time);
      strcat(time_display, units_time);
      text_donation_time_display.set_text_direct(strlen(time_display), time_display);

      previous_remaining_proc_time = remaining_time;
   }
}



////////////////////////////////////////////////////////////////////////////////
//
//  This is the callback that gets hit when a predict request is fulfilled.
//
void Screen_TROUBLE::predict_complete_notify ()
{
   if (_exited) return;

   // if we were waiting for a response to our IS_PROCEDURE_VALID message, we just got it:
   _wait_is_valid = false;

   if (_exiting)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, predict request complete. Continuing to exit."
                                  << endmsg;
      shutdown();
      return;
   }

   // in principle we shouldn't get here if we're still waiting on a ProcedureAdjustment message to
   // be acked.  However, it might happen for all kinds of other reasons, so:
   if ( AdjustManager::waiting() )
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, predict complete notify but waiting on adjustment acks"
                                  << endmsg;
      return;
      // we're waiting to come back here on a ProcAdjStatus message
   }

   // Otherwise... update and go back to our usual business

   DataLog(log_level_gui_info) << "Screen_TROUBLE, predict request complete. Continuing, update screen data"
                               << endmsg;

   updateScreenData();

}  // End of PROCEDURE_PREDICT_STRUCT_MANAGER



int Screen_TROUBLE::display_operator_button (BUTTON_VISIBILITY_STATUS status,
                                             Button_Bitmap& btn,
                                             Button_Bitmap& btn_grey)
{
   int do_callback = 0;
   switch (status)
   {
      case BUTTON_INVISIBLE :
         btn_grey.deallocate_resources();
         btn.deallocate_resources();
         break;
      case BUTTON_GREY :
         btn.deallocate_resources();
         if (!btn_grey.allocated())
         {
            btn_grey.allocate_resources (*this);
            btn_grey.set_button_text_color(GREY_LIGHT);
            btn_grey.disable_callbacks();
         }
         break;
      case BUTTON_VISIBLE :
         if (!btn.allocated())
         {
            btn.allocate_resources (*this);
            do_callback = 1;
         }
         break;
   }
   return do_callback;
}



void Screen_TROUBLE::update_operator_buttons ()
{
   ProcAdjust_CDS&                adjustmentCds = guiglobs::cdsData.adjustments;

   const BUTTON_VISIBILITY_STATUS salineBolus   = adjustmentCds.SalineBolus.Get();
   const BUTTON_VISIBILITY_STATUS airblock      = adjustmentCds.AirBlock.Get();
   const BUTTON_VISIBILITY_STATUS spillover     = adjustmentCds.Spillover.Get();

   DataLog(log_level_proc_info) << "Adjustment button status: "
                                << "Saline Bolus <" << salineBolus << "> "
                                << "Air Block <" << airblock << "> "
                                << "Spillover <" << spillover << ">" << endmsg;

   if (display_operator_button(spillover, btn_spillover, btn_spillover_grey))
   {
      btn_spillover.set_callback(callback_spillover, (void*)this);
   }

   if (display_operator_button(airblock, btn_air_in_plasma, btn_air_in_plasma_grey))
   {
      btn_air_in_plasma.set_callback(callback_air_in_plasma_line, (void*)this);
   }

   if (display_operator_button(salineBolus, btn_saline_bolus, btn_saline_bolus_grey))
   {
      btn_saline_bolus.set_callback(callback_saline_bolus, (void*)this);
   }
}



void Screen_TROUBLE::adjustment_complete_notify ()
{
   DataLog(log_level_gui_info) << "Screen_TROUBLE:: adjustment completed"
                               << endmsg;

   if (_exited)
      return;

   if (_exiting)
   {
      shutdown();
      return;
   }
   updateScreenData();
}


void Screen_TROUBLE::updateScreenData ()
{
   if (_exited) return;
   if (_exiting)
   {
      shutdown();
      return;
   }

   DataLog(log_level_gui_info) << "Screen_TROUBLE:: updateScreenData." << endmsg;

   ProcAdjust_CDS& adjustmentCds = guiglobs::cdsData.adjustments;

   if (!_allocated)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE is UNALLOCATED!" << endmsg;
      return;
   }

   // if the screen is allocated use the information given
   // in the message.

   update_operator_buttons();

   //
   //
   // ------------------------------ Draw adjustment
   //
   const bool drawMeterActive = adjustmentCds.Draw.Active.Get();

   meter_draw.Active(drawMeterActive);

   if (!drawMeterActive)
   {
      btn_draw_up.deallocate_resources();
      btn_draw_down.deallocate_resources();
   }
   else
   {
      // add the draw up button back
      if (!btn_draw_up.allocated())
      {
         btn_draw_up.allocate_resources(*this);
         btn_draw_up.set_callback (callback_adjust_draw_up, (void*)this);
      }

      // add the draw down button back
      if (!btn_draw_down.allocated())
      {
         btn_draw_down.allocate_resources(*this);
         btn_draw_down.set_callback (callback_adjust_draw_down, (void*)this);
      }
   }

   //
   // ------------------------------ return adjustment
   //
   const bool returnMeterActive = adjustmentCds.Return.Active.Get();

   meter_return.Active(returnMeterActive);

   if (!returnMeterActive)
   {
      btn_return_up.deallocate_resources();
      btn_return_down.deallocate_resources();
   }
   else
   {
      // add the return button back
      if (!btn_return_up.allocated())
      {
         btn_return_up.allocate_resources(*this);
         btn_return_up.set_callback (callback_adjust_return_up, (void*)this);
      }

      // add the return button back
      if (!btn_return_down.allocated())
      {
         btn_return_down.allocate_resources(*this);
         btn_return_down.set_callback (callback_adjust_return_down, (void*)this);
      }
   }

   //
   // ------------------------------ tingling adjustment
   //
   const bool tinglingMeterActive = adjustmentCds.Tingling.Active.Get();

   meter_tingling.Active(tinglingMeterActive);

   if (!tinglingMeterActive)
   {
      btn_ac_reaction_up.deallocate_resources();
      btn_ac_reaction_down.deallocate_resources();
   }
   else
   {
      // add the ac reaction button back
      if (!btn_ac_reaction_up.allocated())
      {
         btn_ac_reaction_up.allocate_resources(*this);
         btn_ac_reaction_up.set_callback (callback_adjust_ac_reaction_up, (void*)this);
      }

      // add the ac reaction button back
      if (!btn_ac_reaction_down.allocated())
      {
         btn_ac_reaction_down.allocate_resources(*this);
         btn_ac_reaction_down.set_callback (callback_adjust_ac_reaction_down, (void*)this);
      }
   }

   //
   // ------------------------------ clumping adjustment
   //
   const bool clumpingMeterActive = adjustmentCds.Clumping.Active.Get();

   meter_clumping.Active(clumpingMeterActive);

   if (!clumpingMeterActive)
   {
      btn_clumping_up.deallocate_resources();
      btn_clumping_down.deallocate_resources();
   }
   else
   {
      // add the clumping button back
      if (!btn_clumping_up.allocated())
      {
         btn_clumping_up.allocate_resources(*this);
         btn_clumping_up.set_callback (callback_adjust_clumping_up, (void*)this);
      }

      // add the clumping button back
      if (!btn_clumping_down.allocated())
      {
         btn_clumping_down.allocate_resources(*this);
         btn_clumping_down.set_callback (callback_adjust_clumping_down, (void*)this);
      }
   }

   meter_draw.Update(adjustmentCds.Draw.Minimum.Get(),
                     adjustmentCds.Draw.Maximum.Get(),
                     adjustmentCds.Draw.CurrentValue.Get(),
                     adjustmentCds.Draw.CurrentCap.Get());

   meter_return.Update(adjustmentCds.Return.Minimum.Get(),
                       adjustmentCds.Return.Maximum.Get(),
                       adjustmentCds.Return.CurrentValue.Get(),
                       adjustmentCds.Return.CurrentCap.Get());

   meter_tingling.Update(adjustmentCds.Tingling.Minimum.Get(),
                         adjustmentCds.Tingling.Maximum.Get(),
                         adjustmentCds.Tingling.CurrentValue.Get(),
                         adjustmentCds.Tingling.CurrentCap.Get());

   meter_clumping.Update(adjustmentCds.Clumping.Minimum.Get(),
                         adjustmentCds.Clumping.Maximum.Get(),
                         adjustmentCds.Clumping.CurrentValue.Get(),
                         adjustmentCds.Clumping.CurrentCap.Get());


   // don't thaw the screen if we're waiting on a transaction
   if ( (!AdjustManager::waiting()) && (!PredictManager::waiting()) )
      thaw_screen();
}  // END of Screen_TROUBLE GET_ADJUST_SCALE_VALUES



////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER
void Screen_TROUBLE::screen_timeout ()
{
   DataLog(log_level_gui_info) << "Screen_TROUBLE: Time out occurred in Adjust Screen"
                               << " this=" << this
                               << " my_cnt=" << my_instance_cnt
                               << " tot_cnt=" << tot_instance_cnt
                               << " my_ptr=" << this
                               << " comm_ptr=" << guiglobs::screen_status.get_current_displayed_screen_ptr()
                               << endmsg;

   timeout_occurred = true;

   // Don't simulate the button press unless I am the most recent (top/current) instance
   // i.e., don't do this if we've been pre-empted by subsequent trouble screens
   if ( my_instance_cnt == tot_instance_cnt &&
        guiglobs::screen_status.get_current_displayed_screen_ptr() == this)
   {
      process_continue_button ();
   }
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON
void Screen_TROUBLE::process_cancel_button ()
{
   // reset the screen timeout
   timeout_reset_manager ();

   if ( guiglobs::proc_status.rinsebackIsAnOption() )
      goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_RINSEBACK);
   else
      goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_DRAW_UP_BUTTON
void Screen_TROUBLE::process_draw_up_button ()
{
   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(ACCESS_PRESSURE_UP, HERE);

}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_DRAW_DOWN_BUTTON
void Screen_TROUBLE::process_draw_down_button ()
{

   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(ACCESS_PRESSURE_DOWN, HERE);

}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RETURN_UP_BUTTON
void Screen_TROUBLE::process_return_up_button ()
{
   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(RETURN_PRESSURE_UP, HERE);

}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RETURN_DOWN_BUTTON
void Screen_TROUBLE::process_return_down_button ()
{
   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(RETURN_PRESSURE_DOWN, HERE);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_AC_REACTION_UP_BUTTON
void Screen_TROUBLE::process_ac_reaction_up_button ()
{
   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(TINGLING_UP, HERE);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_AC_REACTION_DOWN_BUTTON
void Screen_TROUBLE::process_ac_reaction_down_button ()
{

   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(TINGLING_DOWN, HERE);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLUMPING_UP_BUTTON
void Screen_TROUBLE::process_clumping_up_button ()
{
   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(CLUMPING_UP, HERE);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CLUMPING_DOWN_BUTTON
void Screen_TROUBLE::process_clumping_down_button ()
{

   // freeze all adjust widgets until response recieved
   freeze_screen();

   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of decision to change procedure
   AdjustManager::sendAdjustment(CLUMPING_DOWN, HERE);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON
void Screen_TROUBLE::process_lowleft_button ()
{
   // we're leaving the screen as soon as all our messages and requests have been completed.  The
   // user doesn't get to interact with the screen any more...
   freeze_screen ();

   // reset the screen timeout
   timeout_reset_manager ();

   // set system flags: exit is requested, and we want to go to the predict screen
   _exiting       = true;
   _send_is_valid = _wait_is_valid = false; // we don't care if the current procedure is valid

   // We need to go to the prediction screen
   _predict_requested = true;

   // and go into shutdown
   shutdown();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_TROUBLE::process_continue_button ()
{
   // We're leaving, the user no longer gets to interact with the screen
   freeze_screen ();

   _exiting = true;

   // exit is requested, and if PredictManager doesn't already think we need to go
   // to the predict screen, find out whether we need to or not:

   if (!PredictManager::prediction_screen_requested() )
   {
      _send_is_valid = true;
      _wait_is_valid = false;    // we're not waiting yet.
   }

   // else: we should check that those arent' set.

   // reset the screen timeout
   timeout_reset_manager ();


   // inform bio mgr of decision to exit the screen
   AdjustManager::sendAdjustment(ADJUSTMENT_SCREEN_CLOSED, HERE);

   shutdown();
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_SPILLOVER_BUTTON
void Screen_TROUBLE::process_spillover_button ()
{
   // reset the screen timeout
   timeout_reset_manager ();

   //   Pause pumps while the operator is thinking it over
   //
   anAlarmMsg pause(PAUSE_BEFORE_SPILLOVER);
   pause.setAlarm();

   // exit the screen based on invocation type
   screen_exit (GUI_SCREEN_ABORTCNF, CONFIRM_TYPE_SPILLOVER);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_AIR_IN_PLASMA_LINE_BUTTON
void Screen_TROUBLE::process_air_in_plasma_line_button ()
{
   // reset the screen timeout
   timeout_reset_manager ();

   // exit the screen based on invocation type
   screen_exit (GUI_SCREEN_ABORTCNF, CONFIRM_TYPE_AIR_IN_PLASMA);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_SALINE_BOLUS_BUTTON
void Screen_TROUBLE::process_saline_bolus_button ()
{
   // reset the screen timeout
   timeout_reset_manager ();

   // inform proc of request
   AdjustManager::sendAdjustment(SALINE_BOLUS, HERE);

   // exit the screen by pretending the continue button was pressed
   process_continue_button();
}



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_TROUBLE::system_state_notify ()
{
   // add delete specific widgets depending on current system state
   if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      // lowleft not needed when not in run
      btn_lowleft.deallocate_resources ();
   }
}



void Screen_TROUBLE::shutdown ()
{
   //
   // Note that some of this code is redundant; we no longer have to detect invalid
   // procedures and switch to the predict screen, since the run screen does
   // that now), but those changes are extensive and as I write this we're late in
   // a development cycle.  This can be cleaed up later (with caution, see the
   // extensive comments below). --DGS


   DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown."
                               << ( AdjustManager::waiting() ? " Waiting for adjustments" : "")
                               << ( _exiting ? ", exiting " : "")
                               << ( _send_is_valid ? ", send is_valid" : "")
                               << ( _wait_is_valid ? ", waiting for is_valid" : "")
                               << endmsg;



   // if not waiting on an exit, this is meaningless (we must have come here from a callback
   // triggered by external sources)
   if (!_exiting)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown but no exit requested." << endmsg;
      return;
   }

   // If we're still waiting on ProcedureAdjustment messages, we shouldn't come here.  However,
   // ProcAdjStatus messages run around the system a lot, so it might happen:
   if ( AdjustManager::waiting() )
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown but waiting for adjustments" << endmsg;
      return;
   }


   // If, by some odd chance, the PredictManager is also busy, same deal:
   if (PredictManager::waiting() )
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown but waiting for Predict" << endmsg;
      return;
   }


   // If we're in rinseback, we can immediately transition:
   if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown in rinseback; don't need to ask if procedure is valid"
                                  << endmsg;
      _send_is_valid = _wait_is_valid = false; // probably redundant
   }


   // With those cases out of the way, we can get down to our 'exit state machine'.

   // DataLog (log_level_gui_error) << "Screen_TROUBLE: predict manager state: "
   //                               << (PredictManager::prediction_screen_requested()
   //                                   ? " prediction screen requested;" : "")
   //                               << (PredictManager::procedures_are_available()
   //                                   ? " procedures are available" : "")
   //                               << endmsg;


   // If we're going to send a IS_PROC_VALID request and wait for an answer, do it here.
   // If we're waiting to send the IS_PROC_VALID message, do so and exit (wait for the
   // predict_complete_notify callback to be triggered and then come back here.
   if (_send_is_valid)
   {
      DataLog (log_level_gui_error) << "Screen_TROUBLE: shutdown; firing IS_PROCEDURE_VALID"
                                    << endmsg;
      PredictManager::sendPredictRequest(IS_PROCEDURE_VALID, HERE);
      _send_is_valid = false;
      _wait_is_valid = true;
      // and exit.  We'll come back here at least once, when we get the response.
      return;
   }

   // If we get here somehow and we're still waiting to find out if the current procedure is valid,
   // keep waiting.  (Presumably, we'll come back here *again*.  This is belt-and-suspenders stuff)
   if (_wait_is_valid)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown; waiting for IS_VALID."
                                  << endmsg;
      return;
   }

   //  NOTE: if we get here, anything that still needs a callback has been handled; from this point
   //  on, we don't want to respond to any messages.  We can turn off the callbacks, but we still
   //  may (and frequently do have) messages in the queue.  Set the state that says, 'no, really, no
   //  more callbacks and I mean it'
   _exited = true;
   DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown set _exited, disabling all callbacks." << endmsg;
   disable_callbacks();


   // If we're in rinseback, we can immediately transition:
   if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      DataLog(log_level_gui_info) << "Screen_TROUBLE, shutdown in rinseback; go straight to run screen."
                                  << endmsg;
      // exit the screen based on invocation type
      screen_exit (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      return;
   }


   // If Predict or the user has asked to go to the predict screen, do so.
   if ( _predict_requested || PredictManager::prediction_screen_requested() )
   {
      // Some user feedback
      if (current_proc_data.system_state == BLOOD_RUN)
      {
         //
         //   IT4181: only give beep feedback if timeout and proceeding
         //   to the prediction screen.
         //
         if (timeout_occurred)
         {
            // call sound method to start alert "beep"
            guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND,
                                                                   SOUND_IMMEDIATE);
         }
      }

      // set invocation allocation parameter
      guistring predict_call_type;
      if(guiglobs::predictScreenFromAfDqAlarm == true)
      {
         sprintf(predict_call_type, "%d", (int)AUTO_FLOW_TIMEOUT);
      }
      else
      {
         sprintf(predict_call_type, "%d", (int)ADJUST_DELTA);
      }

      // exit the screen based on invocation type
      screen_exit (GUI_SCREEN_PREDICT, predict_call_type);
      return;
   }

   // If we get here, the current procedure is still valid and we go to the run screen.

   DataLog(log_level_gui_info) << "Screen_TROUBLE, completing continue_button exit to runscreen" << endmsg;

   // exit the screen based on invocation type
   screen_exit (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
}


void Screen_TROUBLE::enable_callbacks ()
{

   DataLog(log_level_gui_info) << "Screen_TROUBLE: enable_callbacks" << endmsg;

   // Hook up to the adjust manager (duplicate attaches are safe)
   AdjustManager::attach(this);


   // Register with the Predict manager for complete predict transactions
   PredictManager::attach(this);
   PredictManager::set_request_complete_callback(Callback<Screen_TROUBLE>(this,
                                                                          &Screen_TROUBLE::predict_complete_notify));


   // Procedure Status horribleness
   guiglobs::proc_status.attach(this, 1250);
   guiglobs::proc_status.set_system_state_callback(Callback<Screen_TROUBLE>(this, &Screen_TROUBLE::system_state_notify));
   guiglobs::proc_status.set_proc_status_callback(Callback<Screen_TROUBLE>(this, &Screen_TROUBLE::proc_status_notify));
}


void Screen_TROUBLE::disable_callbacks ()
{

   DataLog(log_level_gui_info) << "Screen_TROUBLE: disable_callbacks" << endmsg;

   guiglobs::proc_status.detach(this);
   PredictManager::detach(this);
   AdjustManager::detach(this);
}





////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_DRAW_UP
void callback_adjust_draw_up (void* data)
{
   ((Screen_TROUBLE*)data)->process_draw_up_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_DRAW_DOWN
void callback_adjust_draw_down (void* data)
{
   ((Screen_TROUBLE*)data)->process_draw_down_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_RETURN_UP
void callback_adjust_return_up (void* data)
{
   ((Screen_TROUBLE*)data)->process_return_up_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_RETURN_DOWN
void callback_adjust_return_down (void* data)
{
   ((Screen_TROUBLE*)data)->process_return_down_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_AC_REACTION_UP
void callback_adjust_ac_reaction_up (void* data)
{
   ((Screen_TROUBLE*)data)->process_ac_reaction_up_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_AC_REACTION_DOWN
void callback_adjust_ac_reaction_down (void* data)
{
   ((Screen_TROUBLE*)data)->process_ac_reaction_down_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_CLUMPING_UP
void callback_adjust_clumping_up (void* data)
{
   ((Screen_TROUBLE*)data)->process_clumping_up_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_ADJUST_CLUMPING_DOWN
void callback_adjust_clumping_down (void* data)
{
   ((Screen_TROUBLE*)data)->process_clumping_down_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_SPILLOVER
void callback_spillover (void* data)
{
   ((Screen_TROUBLE*)data)->process_spillover_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_AIR_IN_PLASMA_LINE
void callback_air_in_plasma_line (void* data)
{
   ((Screen_TROUBLE*)data)->process_air_in_plasma_line_button ();
}



////////////////////////////////////////////////////////////////////////////////
// CALLBACK_SALINE_BOLUS
void callback_saline_bolus (void* data)
{
   ((Screen_TROUBLE*)data)->process_saline_bolus_button ();
}

/* FORMAT HASH 51b52e94c5d7a6e6eba53bb4e086feee */
