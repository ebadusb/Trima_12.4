/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_RUNPROC

*     File name:   runproc.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the RUNPROC screen


*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>
#include <math.h>

#include "guiglobs.hpp"
#include "guipalette.h"
#include "runproc.hpp"
#include "runsumGlobs.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "predict_manager.hpp"

static const int METER_PRESSURE_W       = 94;
static const int METER_PRESSURE_H       = 11;

static const int METER_TIME_PROCEDURE_X = 313;
static const int METER_TIME_PROCEDURE_Y = 114;
static const int METER_TIME_PROCEDURE_W = 272;
static const int METER_TIME_PROCEDURE_H = 21;

static const int METER_PLATELET_X       = 213;
static const int METER_PLATELET_Y       = 239;
static const int METER_PLATELET_W       = 113;
static const int METER_PLATELET_H       = 140;

static const int METER_PLASMA_X         = 340;
static const int METER_PLASMA_Y         = 239;
static const int METER_PLASMA_W         = 113;
static const int METER_PLASMA_H         = 140;

static const int METER_RBC_X            = 467;
static const int METER_RBC_Y            = 239;
static const int METER_RBC_W            = 113;
static const int METER_RBC_H            = 140;

#if CPU==SIMNT
static const int FORCE_REDRAW = 1;
#else
static const int FORCE_REDRAW = 0;
#endif // #if CPU==SIMNT

static void callback_display_time_hours (void* data);
static void callback_display_time_min (void* data);
static void callback_run_complete (void* data);
static void callback_display_run_summary (void* data);

// Macro forumula for getting the percent change
static const double CHANGE_THRESHOLD = .05;

inline
bool SIGNIFICANT_CHANGE (double A, double B)
{
   return ( fabs (A - B) / MIN (fabs(A), fabs(B))) > CHANGE_THRESHOLD;
}


Screen_RUNPROC::TimeDisplayType Screen_RUNPROC::start_finish_display = Screen_RUNPROC::DisplayTimeOfDay;  // display type for time display

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_RUNPROC::Screen_RUNPROC()
   : Base_Apheresis_Window (BLUE, GUI_SCREEN_RUNPROC),

     standard_time                   (true), // true for AM/PM, a reasonable default
     current_pressure_indicator_pos  (0),
     previous_pressure_indicator_pos (0),
     allocated_meter                 (NO_PRESSURE_METER_ALLOCATED),

     btn_time_current    (GUI_BUTTON_RUN_CURRENT_TIME,
                          BUTN_CURRENT_TIME_X,
                          BUTN_CURRENT_TIME_Y,
                          textEmpty,
                          BITMAP_BUTTON_CURRENT_TIME_DES,
                          BITMAP_BUTTON_CURRENT_TIME_SEL),

     btn_time_min        (GUI_BUTTON_RUN_TIME_MIN,
                          BUTN_TIME_MIN_X,
                          BUTN_TIME_MIN_Y,
                          textEmpty,
                          BITMAP_BUTTON_TIME_MIN_DES,
                          BITMAP_BUTTON_TIME_MIN_SEL),

     bitmap_timebar          (TIMEBAR_X,
                              TIMEBAR_Y,
                              BITMAP_TIMEBAR),

     bitmap_pressure         (PRESSURE_X,
                              PRESSURE_Y,
                              BITMAP_PRESSURE),

     bitmap_platelet         (PLATELET_X,
                              PLATELET_Y,
                              BITMAP_PLATELET),

     bitmap_plasma           (PLASMA_X,
                              PLASMA_Y,
                              BITMAP_PLASMA),

     bitmap_pas_icon      (289, 244, BITMAP_MASKED_PAS_ICON),

     bitmap_comppls_icon  (417, 244, BITMAP_MASKED_COMP_PLS_ICON),

     bitmap_rbc              (RBC_X,
                              RBC_Y,
                              BITMAP_RBC),

     bitmap_ras_icon      (544, 244, BITMAP_MASKED_RAS_ICON),

     bitmap_squeeze          (SQUEEZE_X,
                              SQUEEZE_Y,
                              BITMAP_SQUEEZE),

     bitmap_dontSeal        (64, 105, BITMAP_DONT_SEAL_ICON),


     text_pressure_label           (textMiscRunProcPressureLabel),

     text_platelets_label          (textMiscRunProcProductLabelPlt),

     text_plasma_label             (textMiscRunProcProductLabelPlasma),

     text_rbcs_label               (textMiscRunProcProductLabelRbc),

     text_platelets_est            (textMiscRunProcInitPltEst),

     text_platelets_exp_est        (textMiscRunProcInitPltEstExponent),

     text_plasma_est               (textDataInitRunProcEstPlas),

     text_rbcs_est                 (textDataInitRunProcEstRbc),

     text_platelets_cur            (textMiscRunProcInitPltCurrent),

     text_platelets_exp_cur        (textMiscRunProcInitPltCurrentExponent),

     text_plasma_cur               (textDataInitRunProcCurrentPlas),

     text_rbcs_cur                 (textDataInitRunProcCurrentRbc),

     text_draw_return              (textEmptyFormatRunDrawReturnInit),

     text_donation_time_left       (textEmptyFormatRunDonationTimeLeftInit),

     text_donation_time_mid        (textEmptyFormatRunDonationTimeMidInit),

     text_donation_time_right      (textEmptyFormatRunDonationTimeRightInit),

     text_date (textMiscRunProcDate),

     stick_meter (BITMAP_ACCMETER_FRAME_PROC,        // meter frame bitmap id

                  PRESSURE_X,                   // x offset of frame within the screen
                  PRESSURE_Y,                   // y offset of frame within the screen

                  9,                            // x offset of meter within the frame
                  12,                           // y offset of meter within the frame

                  200,                          // meter height in pixels

                  10,                           // total positive tics
                  10,                           // total negative tics

                  150.0,                        // positive pressure value limit
                  -150.0,                       // negative pressure value limit

                  1,                            // DO enable the display of the last pressure value

                  // positive pressure indicator bitmaps //
                  BITMAP_ACCMETER_GREEN_NOT_ACTIVE,
                  BITMAP_ACCMETER_GREEN_CURR_ACTIVE,
                  BITMAP_ACCMETER_GREEN_LAST_ACTIVE,
                  BITMAP_ACCMETER_GREEN_OFFSCALE_ACTIVE,

                  // negative pressure indicator bitmaps //
                  BITMAP_ACCMETER_YELLOW_NOT_ACTIVE,
                  BITMAP_ACCMETER_YELLOW_CURR_ACTIVE,
                  BITMAP_ACCMETER_YELLOW_LAST_ACTIVE,
                  BITMAP_ACCMETER_YELLOW_OFFSCALE_ACTIVE
                  ),

     pressure_indicator   (NULL),
     meter_procedure_time (NULL),
     meter_platelets      (NULL),
     meter_plasma         (NULL),
     meter_rbcs           (NULL),
     current_proc_data    (guiglobs::proc_status.get_data() )
{}


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_RUNPROC::~Screen_RUNPROC()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_RUNPROC::deallocate_resources ();

   // comment -- it looks like this is properly deleted elsewhere, but
   // this is not completely clear.
   delete pressure_indicator;

}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_RUNPROC::allocate_resources (const char* allocation_parameter)
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;


   guiglobs::run_summary_display_active = false;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // obtain time type
   standard_time = guiglobs::cdsData.config.LangUnit.Get().key_time_format;

   // deallocate "standard" buttons that are not needed
   btn_goback.deallocate_resources ();

   // lower left button valid in BLOOD_RUN and BLOOD_RINSEBACK only
   if (current_proc_data.system_state == BLOOD_PRIME)
   {
      btn_lowleft.deallocate_resources ();
   }

   btn_lowleft.set_up_bitmap(BITMAP_BUTTON_ADJUST);

   bool runCompleteHandled = false;

   // handle button changes needed for rinseback
   if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      // if run complete remove exit run and troubleshoot buttons and change
      // face of the continue button
      if (current_proc_data.run_complete)
      {
         DataLog (log_level_gui_info) << "System state is BLOOD_RINSEBACK, run is complete." << endmsg;
         // remove adjust and "exit run" buttons from RUNPROC screen
         btn_cancel.deallocate_resources();
         btn_lowleft.deallocate_resources();

         // Add "run complete" button to RUNPROC screen
         btn_continue.set_up_bitmap (BITMAP_BUTTON_LARGE_BOT_BLUE_UP);
         btn_continue.set_string_id (textButtonLongContinue);
         btn_continue.set_callback (callback_run_complete, (void*)this);

         runCompleteHandled = true;
      }
      else
      {
         DataLog (log_level_gui_info) << "System state is BLOOD_RINSEBACK, run is not complete, allow viewing of run summary" << endmsg;
         btn_continue.set_up_bitmap (BITMAP_BUTTON_LARGE_BOT_BLUE_UP);
         btn_continue.set_string_id (textButtonLongNextPage);
         btn_continue.set_callback (callback_display_run_summary, (void*)this);
      }
   }
   else
   {
      // modify text in allocated buttons
      btn_continue.set_string_id (textEmpty);

      // modify bitmaps in allocated buttons
      btn_continue.set_up_bitmap(BITMAP_BUTTON_DONOR_INFO);
   }

   // Allocate the resources for the donor vitals buttons
   btn_time_current.allocate_resources (*this);
   btn_time_min.allocate_resources (*this);

   // set callbacks to the individual button widgets
   btn_time_current.set_callback (callback_display_time_hours, (void*)this);
   btn_time_min.set_callback (callback_display_time_min, (void*)this);

   // allocate date object
   text_date.allocate_resources(*this);

   // allocate background bitmap objects
   bitmap_timebar.allocate_resources(*this);
   bitmap_platelet.allocate_resources(*this);
   bitmap_plasma.allocate_resources(*this);
   bitmap_rbc.allocate_resources(*this);

   // now add the text
   text_platelets_label.allocate_resources (*this);
   text_plasma_label.allocate_resources (*this);
   text_rbcs_label.allocate_resources (*this);
   text_platelets_est.allocate_resources (*this);
   text_platelets_exp_est.allocate_resources (*this);
   text_plasma_est.allocate_resources (*this);
   text_rbcs_est.allocate_resources (*this);
   text_platelets_cur.allocate_resources (*this);
   text_platelets_exp_cur.allocate_resources (*this);
   text_plasma_cur.allocate_resources (*this);
   text_rbcs_cur.allocate_resources (*this);
   text_donation_time_mid.allocate_resources (*this);

   // left and right times valid in BLOOD_PRIME and BLOOD_RUN only
   if ((current_proc_data.system_state == BLOOD_PRIME) || (current_proc_data.system_state == BLOOD_RUN))
   {
      text_donation_time_left.allocate_resources (*this);
      text_donation_time_right.allocate_resources (*this);
   }

   // now get the text that will be used in dynamic screen construction
   get_dynamic_text();

   // if you have the start and finished time displayed, update
   if (start_finish_display == DisplayTimeOfDay)
   {
      // set the minute button to fade
      btn_time_min.set_up_bitmap (BITMAP_BUTTON_TIME_MIN_DES);

      // set the current button to active
      btn_time_current.set_up_bitmap (BITMAP_BUTTON_CURRENT_TIME_SEL);

   }
   else
   {
      // set the minute button to active
      btn_time_min.set_up_bitmap (BITMAP_BUTTON_TIME_MIN_SEL);

      // set the current button to fade
      btn_time_current.set_up_bitmap (BITMAP_BUTTON_CURRENT_TIME_DES);
   }

   // now initialize the meter widgets
   build_all_meters();

   // Link the status bar/line to this window
   link_status_bar_line();

   // set the squeeze bitmap
   bitmap_squeeze.allocate_resources (*this);
   bitmap_squeeze.set_visible(false);

   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (BLUE);

   // Set the Status Bar: status donate
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_LOAD);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_INFO);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_PREPARE);
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_DONATE);

   // screen initialization will refresh all relevant screen allocation or
   // construction parameters for display text and system states
   reset_internal_screen_state ();
   setup_proc_status();

   system_state_notify();

   // Cover race condition where proc status changes from tests in beginning of allocate_resources
   // to the call to setup_proc_status.  See IT 6959.
   if (current_proc_data.run_complete && !runCompleteHandled)
   {
      run_complete_notify();
   }

   if ((guiglobs::screen_status.get_current_system_state() == BLOOD_RINSEBACK) &&
       (guiglobs::aasRunInfo.isPasProcedure() ||
        guiglobs::aasRunInfo.isRasProcedure() ||
        guiglobs::isSpecialRinseback()))
      bitmap_dontSeal.allocate_resources(*this);
   else
      bitmap_dontSeal.deallocate_resources();


   const bool AUTOFLOW_ON = (bool)(guiglobs::cdsData.config.Procedure.Get().key_autoflow);
   if (AUTOFLOW_ON == true)
   {
      _invokePredictScreenForAutoflow.init(Callback<Screen_RUNPROC>(this, &Screen_RUNPROC::HandlePredictPopUp), MessageBase::SNDRCV_RECEIVE_ONLY);
   }


   // Transition directly to predict screen if necessary
   TransitionToPredict();

} // END of Screen_RUNPROC ALLOCATE_RESOURCES



void Screen_RUNPROC::HandlePredictPopUp ()
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;

   const bool AUTOFLOW_ON = (bool)(guiglobs::cdsData.config.Procedure.Get().key_autoflow);
   if (AUTOFLOW_ON == true)
   {
      DataLog (log_level_gui_info) << " Received ShowTimeoutPredictScreen message." << endmsg;

      guistring allocation_parameter_string;

      // guiglobs::button_audio_feedback.create_feedback_sound (ALERT_SOUND, SOUND_IMMEDIATE);

      GuiToProcPredictRequestMsg predictRequestMsg(MessageBase::SEND_LOCAL);
      predictRequestMsg.send(REPREDICT_ALL);


      // set invocation allocation parameter
      sprintf(allocation_parameter_string, "%d", (int)AUTO_FLOW_TIMEOUT);

      // call the procedure selection screen,
      goto_stateless_screen (GUI_SCREEN_PREDICT, allocation_parameter_string);
   }

}


void Screen_RUNPROC::TransitionToPredict ()
{
   // Under some circumetances, if the current procedure is invalid, we need to
   // go to the predict screen and stop the pumps.  We do this here, because if
   // deallocate_resources gets called, we need to make sure that they were all
   // properly allocated in the first place.  IT 13240, with refinements from
   // 13748.


   if (current_proc_data.system_state > BLOOD_RUN)
   {
      DataLog(log_level_gui_info) << "Screen_RUNPROC: Transition to Predict?  No, because the run is finished."
                                  << endmsg;
      return;
   }

   if ( PredictManager::prediction_screen_requested())
   {
      DataLog(log_level_gui_info) << "Screen_RUNPROC: Transition to Predict? Predict Screen requested, "
                                  << "and the run isn't done, so Yes."
                                  << endmsg;

      // This method of invoking the predict screen should disable the goback button.
      goto_screen(GUI_SCREEN_PREDICT, "CPS_DISABLES_RBCS");
   }
}

void Screen_RUNPROC::refresh_resources ()
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;

   Base_Apheresis_Window::refresh_resources();

   setup_proc_status();

   chooseAasBars();

   // Put back the last status line
   guiglobs::proc_status.refresh_status_line();

   if (meter_procedure_time)
   {
      // Refresh the time bar
      meter_procedure_time->Update(0, _currentTime.total_proc_time, _currentTime.elapsed_proc_time, 0, FORCE_REDRAW);
   }

   // Transition directly to predict screen if necessary
   TransitionToPredict();
}

void Screen_RUNPROC::initialize_status_line ()
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;

   if (current_proc_data.system_state == BLOOD_RINSEBACK && current_proc_data.run_complete)
   {
      // Remove rinseback in progress message and show run complete
      guiglobs::apheresis_status_line->set_current_text (textStatRunComplete);
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   }
}


void Screen_RUNPROC::setup_proc_status ()
{
   guiglobs::proc_status.attach(this, 1250);
   guiglobs::proc_status.set_system_state_callback(Callback<Screen_RUNPROC>(this, &Screen_RUNPROC::system_state_notify));
   guiglobs::proc_status.set_pressure_callback(Callback<Screen_RUNPROC>(this, &Screen_RUNPROC::pressure_notify));
   guiglobs::proc_status.set_proc_status_callback(Callback<Screen_RUNPROC>(this, &Screen_RUNPROC::proc_status_notify));
   guiglobs::proc_status.set_run_complete_callback(Callback<Screen_RUNPROC>(this, &Screen_RUNPROC::run_complete_notify));
   guiglobs::proc_status.set_access_pressure_low_bitmap(bitmap_squeeze);
   guiglobs::proc_status.enable_access_pressure_low_warning(true);

   // if (guiglobs::proc_status.predict_pause_active())
   // {
   //    if (guiglobs::screen_status.get_current_system_state() == BLOOD_RINSEBACK)
   //    {
   //       DataLog (log_level_gui_info) << "Predict pause is still active during rinseback. Sending rinseback request message." << endmsg;

   //         // Sending this message is inteneded to clear the predict pause.
   //       RinsebackRequestMsg askForRinseback(MessageBase::SEND_GLOBAL);
   //       askForRinseback.send();
   //    }
   //    else
   //    {
   //       char  allocation_param[10];
   //       sprintf(allocation_param, "%d", guiglobs::proc_status.get_last_predict_screen_type());

   //       DataLog(log_level_gui_info) << "predict pause active - forcing transition to predict screen" << endmsg;
   //       swap_stateless_screen(GUI_SCREEN_PREDICT, allocation_param);
   //    }
   // }
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_RUNPROC::deallocate_resources ()
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;

   guiglobs::proc_status.detach(this);

   // deallocate text objects
   text_platelets_label.deallocate_resources();
   text_plasma_label.deallocate_resources();
   text_rbcs_label.deallocate_resources();
   text_platelets_est.deallocate_resources();
   text_platelets_exp_est.deallocate_resources();
   text_plasma_est.deallocate_resources();
   text_rbcs_est.deallocate_resources();
   text_platelets_cur.deallocate_resources();
   text_platelets_exp_cur.deallocate_resources();
   text_plasma_cur.deallocate_resources();
   text_rbcs_cur.deallocate_resources();
   text_donation_time_left.deallocate_resources();
   text_donation_time_mid.deallocate_resources();
   text_donation_time_right.deallocate_resources();
   text_date.deallocate_resources();

   // deallocate background bitmap objects
   bitmap_timebar.deallocate_resources();
   bitmap_platelet.deallocate_resources();
   bitmap_plasma.deallocate_resources();
   bitmap_rbc.deallocate_resources();
   bitmap_squeeze.deallocate_resources ();
   bitmap_dontSeal.deallocate_resources();
   bitmap_pas_icon.deallocate_resources();
   bitmap_ras_icon.deallocate_resources();
   bitmap_comppls_icon.deallocate_resources();

   // deallocate the pressure meters
   deallocate_pressure_meters (STANDARD_PRESSURE_METER) ;
   deallocate_pressure_meters (STICK_PRESSURE_METER) ;

   delete meter_procedure_time;
   meter_procedure_time = NULL ;

   delete meter_platelets;
   meter_platelets = NULL ;

   delete meter_plasma;
   meter_plasma = NULL ;

   delete meter_rbcs;
   meter_rbcs = NULL ;

   // deallocate non-standard button objects
   btn_time_current.deallocate_resources();
   btn_time_min.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}  // END of Screen_RUNPROC DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE

// This method will reset the internal screen state
void Screen_RUNPROC::reset_internal_screen_state ()
{
   DataLog (log_level_gui_info) << "Screen_RUNPROC: " << __FUNCTION__ << ':' << __LINE__ << endmsg;

   // screen initialization will refresh all relevant screen allocation or
   // construction parameters for display text and system states
   screen_initialization ();
}


////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION

void Screen_RUNPROC::screen_initialization ()
{
   allocated_meter = NO_PRESSURE_METER_ALLOCATED;

   memset(&previous_proc_data, -1, sizeof(previous_proc_data));

}


////////////////////////////////////////////////////////////////////////////////
// GET_DYNAMIC_TEXT
void Screen_RUNPROC::get_dynamic_text ()
{
   Display_Text text;

   text.set_string_id(textDataUnitsRunProcEstPltYield);
   text.get_text_direct(units_platelet_estimate);

   text.set_string_id(textDataUnitsMl);
   text.get_text_direct(units_plasma_estimate);

   text.set_string_id(textDataUnitsMl);
   text.get_text_direct(units_rbc_estimate);

   text.set_string_id(textDataUnitsRunProcCurrentPltYield);
   text.get_text_direct(units_platelet_current);

   text.set_string_id(textDataUnitsMl);
   text.get_text_direct(units_plasma_current);

   text.set_string_id(textDataUnitsMl);
   text.get_text_direct(units_rbc_current);

   text.set_string_id(textDataUnitsRunProcTime);
   text.get_text_direct(units_time);

   text.set_string_id(textDataUnitsRunProcTimeAm);
   text.get_text_direct(units_time_am);

   text.set_string_id(textDataUnitsRunProcTimePm);
   text.get_text_direct(units_time_pm);

   text.set_string_id(textDataUnitsRunProcTimeDelim);
   text.get_text_direct(units_time_delim);
}  // End of GET_DYNAMIC_TEXT


////////////////////////////////////////////////////////////////////////////////
// BUILD_METER

TroubleMeter* Screen_RUNPROC::build_meter (OSColor color, TroubleMeter::MeterType type, short x, short y, short height, short width)
{
   TroubleMeter* meter = new TroubleMeter;

   meter->Initialize(*this, x, y, height, width, color, TroubleMeter::CapDisabled, type);
   meter->Active(1);

   return meter;
}


////////////////////////////////////////////////////////////////////////////////
// BUILD_METERS

void Screen_RUNPROC::build_all_meters ()
{
   // time bar valid in BLOOD_PRIME and BLOOD_RUN only
   if (current_proc_data.system_state != BLOOD_RINSEBACK)
   {
      meter_procedure_time = build_meter(guipalette::meter_yellow, TroubleMeter::Horizontal,
                                         METER_TIME_PROCEDURE_X, METER_TIME_PROCEDURE_Y,
                                         METER_TIME_PROCEDURE_H, METER_TIME_PROCEDURE_W);
   }

   meter_platelets = build_meter(guipalette::meter_yellow, TroubleMeter::Vertical,
                                 METER_PLATELET_X, METER_PLATELET_Y,
                                 METER_PLATELET_H, METER_PLATELET_W);

   meter_plasma = build_meter(guipalette::meter_yellow, TroubleMeter::Vertical,
                              METER_PLASMA_X, METER_PLASMA_Y,
                              METER_PLASMA_H, METER_PLASMA_W);

   meter_rbcs = build_meter(guipalette::meter_yellow, TroubleMeter::Vertical,
                            METER_RBC_X, METER_RBC_Y,
                            METER_RBC_H, METER_RBC_W);

}  // End of BUILD_METERS

// Here's a function to output a timestruct to a datalog stream.
DataLog_Stream& operator << (DataLog_Stream& os, const Screen_RUNPROC::TIMESTRUCT& timeStruct)
{
   return os
          << "(curTime=" << timeStruct.time_of_day
          << "; curDay=" << timeStruct.day_of_month
          << "; start=" << timeStruct.start_proc_time
          << "; elapsed=" << timeStruct.elapsed_proc_time
          << "; remaining=" << timeStruct.remaining_proc_time
          << "; finish=" << timeStruct.finish_proc_time
          << "; total=" << timeStruct.total_proc_time
          << ")";
}

void Screen_RUNPROC::updateTimeStruct ()
{
   _previousTime = _currentTime;

   time_t    tm = time(0);
   struct tm local_tm;
   localtime_r(&tm, &local_tm);

   _currentTime.time_of_day  = 60 * local_tm.tm_hour + local_tm.tm_min;
   _currentTime.day_of_month = local_tm.tm_mday;

 #if CPU==SIMNT
   float       PROCEDURE_DURATION = 39;

   State_names currState          = guiglobs::screen_status.get_current_system_state();
   int         timeElapsed        = 0;
   if (currState < BLOOD_RUN) timeElapsed = (int)(.05 * PROCEDURE_DURATION);
   else if (currState > BLOOD_RUN)
      timeElapsed = (int)PROCEDURE_DURATION;
   else timeElapsed = (int)(.6 * PROCEDURE_DURATION);

   _currentTime.start_proc_time     = _currentTime.time_of_day - timeElapsed;
   _currentTime.elapsed_proc_time   = timeElapsed;
   _currentTime.total_proc_time     = (int)PROCEDURE_DURATION;
   _currentTime.remaining_proc_time = (int)PROCEDURE_DURATION - timeElapsed;
   _currentTime.finish_proc_time    = (int)_currentTime.start_proc_time + (int)PROCEDURE_DURATION;

 #else
   _currentTime.start_proc_time   = (int)guiglobs::cdsData.run.procedureStartTime.Get();
   _currentTime.elapsed_proc_time = (int)guiglobs::cdsData.time.ProcRunTime();
   _currentTime.total_proc_time   = (int)ceil(guiglobs::cdsData.targets.ProcTimeTarget.Get());

   int delta_Proc_Time = _currentTime.total_proc_time - _currentTime.elapsed_proc_time;

   if (delta_Proc_Time < 0)
      delta_Proc_Time = 0;

   _currentTime.remaining_proc_time =  delta_Proc_Time;
   _currentTime.finish_proc_time    =  _currentTime.time_of_day + delta_Proc_Time ;

 #endif


   // If one of the time items changed more than 5%, log it.
   if (SIGNIFICANT_CHANGE(_currentTime.elapsed_proc_time,   _previousTime.elapsed_proc_time) ||
       SIGNIFICANT_CHANGE(_currentTime.total_proc_time,      _previousTime.total_proc_time)      ||
       SIGNIFICANT_CHANGE(_currentTime.remaining_proc_time,  _previousTime.remaining_proc_time)  ||
       SIGNIFICANT_CHANGE(_currentTime.finish_proc_time,     _previousTime.finish_proc_time))
      DataLog (log_level_gui_info) << "Timestruct: " << _currentTime << endmsg;
}
////////////////////////////////////////////////////////////////////////////////
// DISPLAY_TIME

void Screen_RUNPROC::display_time (bool force_update)
{
   updateTimeStruct();

   // for states other than blood prime and blood run, we display only the center time
   // otherwise, we display the full meter
   bool display_meter = ((current_proc_data.system_state == BLOOD_PRIME) || (current_proc_data.system_state == BLOOD_RUN));

   // left and right times valid in BLOOD_PRIME and BLOOD_RUN only
   if (display_meter)
   {
      text_donation_time_left.allocate_resources (*this);
      text_donation_time_right.allocate_resources (*this);
   }

   // if display time in hours/minutes is selected
   if (start_finish_display == DisplayTimeOfDay)
   {
      if (display_meter &&
          (force_update || _currentTime.start_proc_time != _previousTime.start_proc_time))
      {
         display_time_of_day(text_donation_time_left, _currentTime.start_proc_time);
      }

      if (display_meter &&
          (force_update || _currentTime.finish_proc_time != _previousTime.finish_proc_time))
      {
         display_time_of_day(text_donation_time_right, _currentTime.finish_proc_time);
      }

      if (force_update || _currentTime.time_of_day != _previousTime.time_of_day)
      {
         display_time_of_day(text_donation_time_mid, _currentTime.time_of_day);
      }
   }
   else
   {
      if (force_update || (_currentTime.total_proc_time != _previousTime.total_proc_time))
      {
         if (display_meter)
            display_minutes(text_donation_time_mid, _currentTime.total_proc_time);
         else
            display_minutes(text_donation_time_mid, _currentTime.elapsed_proc_time);
      }

      if (display_meter &&
          (force_update || _currentTime.remaining_proc_time != _previousTime.remaining_proc_time))
      {
         display_minutes(text_donation_time_right, _currentTime.remaining_proc_time);
      }

      if (display_meter &&
          (force_update || _currentTime.elapsed_proc_time != _previousTime.elapsed_proc_time ))
      {
         display_minutes(text_donation_time_left, _currentTime.elapsed_proc_time);
      }
   }

   if (meter_procedure_time)
   {
      bool previous_display_meter = ((previous_proc_data.system_state == BLOOD_PRIME) || (previous_proc_data.system_state == BLOOD_RUN));
      if (display_meter &&
          (_currentTime.elapsed_proc_time != _previousTime.elapsed_proc_time ||
           _currentTime.total_proc_time != _previousTime.total_proc_time))
      {
         // update the meter with the new current procedure time and total procedure time
         meter_procedure_time->Update(0, _currentTime.total_proc_time, _currentTime.elapsed_proc_time, 0, FORCE_REDRAW);
      }
      else if (previous_display_meter &&
               !display_meter)
      {
         meter_procedure_time->Update(0, 100, 0, 0, FORCE_REDRAW);
      }
   }

   if (_currentTime.day_of_month != _previousTime.day_of_month)
   {
      display_date(text_date);
   }

}  // End of DISPLAY_TIME


void Screen_RUNPROC::display_time_of_day (Display_Text& text, int time_since_midnight)
{
   int  hours   = (time_since_midnight / 60) % 24;
   int  minutes = time_since_midnight % 60;

   char hour_string[3];
   char minute_string[3];
   char time_string[20];

   sprintf(minute_string, "%02d", minutes);
   if (standard_time)
   {
      int std_hours = hours % 12;
      sprintf(hour_string, "%d", (std_hours == 0) ? 12 : std_hours);
   }
   else
   {
      sprintf(hour_string, "%d", hours);
   }

   strcpy(time_string, hour_string);
   strcat(time_string, units_time_delim);
   strcat(time_string, minute_string);
   if (standard_time)
   {
      strcat(time_string, (hours < 12) ? units_time_am : units_time_pm);
   }

   text.deallocate_resources();
   text.set_text_direct(strlen(time_string), time_string);
   text.allocate_resources(*this);
}

void Screen_RUNPROC::display_minutes (Display_Text& text, int minutes)
{
   char time_string[20];

   sprintf(time_string, "%d", minutes);
   strcat(time_string, units_time);

   text.deallocate_resources();
   text.set_text_direct(strlen(time_string), time_string);
   text.allocate_resources(*this);
}

void Screen_RUNPROC::display_date (Display_Text& text)
{
   static char date_string[20];

   time_t      currentdate = time(0);
   struct tm   local_tm;

   localtime_r(&currentdate, &local_tm);
   switch (guiglobs::cdsData.config.LangUnit.Get().key_date_format)
   {
      case MMDDYY_DATE :
         strftime(date_string, 20, "%m/%d/%Y", &local_tm);
         break;

      case DDMMYY_DATE :
         strftime(date_string, 20, "%d/%m/%Y", &local_tm);
         break;

      default :
         strftime(date_string, 20, "%Y/%m/%d", &local_tm);
         break;
   }

   // replace default first zero char if present -> removed (IT 7575)
   // if (date_string[0] == '0') date_string[0] = ' ';
   text.set_text_direct(strlen(date_string), date_string);
}

void Screen_RUNPROC::display_product_meters ()
{
   guistring display_buffer;

   if (current_proc_data.estimated_platelets != previous_proc_data.estimated_platelets)
   {
      sprintf_l10n(display_buffer, "%.1f", (float)current_proc_data.estimated_platelets / 10.0f);
      strcat(display_buffer, units_platelet_estimate);
      text_platelets_est.set_text_direct(strlen(display_buffer), display_buffer);
   }

   if (current_proc_data.collected_platelets != previous_proc_data.collected_platelets)
   {
      sprintf_l10n(display_buffer, "%.1f", (float)current_proc_data.collected_platelets / 10.0f);
      strcat(display_buffer, units_platelet_current);
      text_platelets_cur.set_text_direct(strlen(display_buffer), display_buffer);
   }

   if (current_proc_data.estimated_plasma != previous_proc_data.estimated_plasma)
   {
      sprintf(display_buffer, "%d", current_proc_data.estimated_plasma);
      strcat(display_buffer, units_plasma_estimate);
      text_plasma_est.set_text_direct(strlen(display_buffer), display_buffer);

   }

   if (current_proc_data.collected_plasma != previous_proc_data.collected_plasma)
   {
      sprintf(display_buffer, "%d", current_proc_data.collected_plasma);
      strcat(display_buffer, units_plasma_current);
      text_plasma_cur.set_text_direct(strlen(display_buffer), display_buffer);

   }

   if (current_proc_data.estimated_rbcs != previous_proc_data.estimated_rbcs)
   {
      sprintf(display_buffer, "%d", current_proc_data.estimated_rbcs);
      strcat(display_buffer, units_rbc_estimate);
      text_rbcs_est.set_text_direct(strlen(display_buffer), display_buffer);

   }

   if (current_proc_data.collected_rbcs != previous_proc_data.collected_rbcs)
   {
      sprintf(display_buffer, "%d", current_proc_data.collected_rbcs);
      strcat(display_buffer, units_rbc_current);
      text_rbcs_cur.set_text_direct(strlen(display_buffer), display_buffer);

   }

   if (current_proc_data.estimated_platelets != previous_proc_data.estimated_platelets ||
       current_proc_data.collected_platelets != previous_proc_data.collected_platelets)
   {
      meter_platelets->Update(0, current_proc_data.estimated_platelets, current_proc_data.collected_platelets, 0, FORCE_REDRAW);
   }

   if (current_proc_data.estimated_plasma != previous_proc_data.estimated_plasma ||
       current_proc_data.collected_plasma != previous_proc_data.collected_plasma)
   {
      meter_plasma->Update(0, current_proc_data.estimated_plasma, current_proc_data.collected_plasma, 0, FORCE_REDRAW);
   }

   if (current_proc_data.estimated_rbcs != previous_proc_data.estimated_rbcs ||
       current_proc_data.collected_rbcs != previous_proc_data.collected_rbcs)
   {
      meter_rbcs->Update(0, current_proc_data.estimated_rbcs, current_proc_data.collected_rbcs, 0, FORCE_REDRAW);
   }

   previous_proc_data.estimated_platelets = current_proc_data.estimated_platelets;
   previous_proc_data.estimated_plasma    = current_proc_data.estimated_plasma;
   previous_proc_data.estimated_rbcs      = current_proc_data.estimated_rbcs;

   previous_proc_data.collected_platelets = current_proc_data.collected_platelets;
   previous_proc_data.collected_plasma    = current_proc_data.collected_plasma;
   previous_proc_data.collected_rbcs      = current_proc_data.collected_rbcs;
}



void Screen_RUNPROC::display_state ()
{
   if (!current_proc_data.run_complete &&
       current_proc_data.return_pump != previous_proc_data.return_pump)
   {
      text_draw_return.set_string_id((current_proc_data.return_pump == RETURN_PUMP_CYCLE_DRAW) ?
                                     textMiscRunProcStatusDraw : textMiscRunProcStatusReturn);
   }
}


////////////////////////////////////////////////////////////////////////////////
// SELECT_TIME_DISPLAY

void Screen_RUNPROC::select_time_display (TimeDisplayType type)
{
   start_finish_display = type;

   if (type == DisplayTimeOfDay)
   {
      // set the minute button to fade
      btn_time_min.set_up_bitmap (BITMAP_BUTTON_TIME_MIN_DES);

      // set the current button to active
      btn_time_current.set_up_bitmap (BITMAP_BUTTON_CURRENT_TIME_SEL);
   }
   else
   {
      // set the minute button to active
      btn_time_min.set_up_bitmap (BITMAP_BUTTON_TIME_MIN_SEL);

      // set the current button to fade
      btn_time_current.set_up_bitmap (BITMAP_BUTTON_CURRENT_TIME_DES);
   }

   // update the current on screen time for user
   display_time (true);

   // update the time for the ten min autoflow alrm
   const bool AUTOFLOW_ON = (bool)(guiglobs::cdsData.config.Procedure.Get().key_autoflow);
   if (AUTOFLOW_ON == true)
   {
      UpdateAutoFlowTimerMsg updateTime(MessageBase::SEND_LOCAL);
      updateTime.send(5);
   }
}  // END of Screen_RUNPROC SELECT_TIME_DISPLAY


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_RUNPROC::process_cancel_button ()
{
   if (guiglobs::proc_status.rinsebackIsAnOption())
      goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_RINSEBACK);
   else
      goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER);
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON

void Screen_RUNPROC::process_lowleft_button ()
{
   // user desires to change procedure, goto the TROUBLE screen
   goto_screen(GUI_SCREEN_TROUBLE, "GUI_SCREEN_TROUBLE");
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_RUNPROC::process_continue_button ()
{
   // switch to donor info screen
   goto_screen (GUI_SCREEN_DONINFO, "GUI_SCREEN_DONINFO");
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_RUN_COMPLETE_BUTTON

void Screen_RUNPROC::process_run_complete_button ()
{
   // let procedure know that the user confirms end of run
   AckRunCompleteMsg complete(MessageBase::SEND_GLOBAL);
   complete.send(0);
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_DISPLAY_RUN_SUMMARY_BUTTON

void Screen_RUNPROC::process_display_run_summary_button ()
{
   // switch to run summary screen
   guiglobs::run_summary_display_active = true;
   RunsumGlobs::gotoNextRunSummaryScreen();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_DISPLAY_TIME_HOURS

void callback_display_time_hours (void* data)
{
   // Actually call the process button routine for the system time setup button
   ((Screen_RUNPROC*)data) -> select_time_display(Screen_RUNPROC::DisplayTimeOfDay);
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_DISPLAY_TIME_MIN

void callback_display_time_min (void* data)
{
   // Actually call the process button routine for the system time setup button
   ((Screen_RUNPROC*)data) -> select_time_display(Screen_RUNPROC::DisplayMinutes);
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_RUN_COMPLETE

void callback_run_complete (void* data)
{
   // Actually call the process button routine for the run complete button
   ((Screen_RUNPROC*)data) -> process_run_complete_button ();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_RUN_COMPLETE

void callback_display_run_summary (void* data)
{
   // Actually call the process button routine for the display run summary button
   ((Screen_RUNPROC*)data) -> process_display_run_summary_button ();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_PRESSURE_METERS

void Screen_RUNPROC::allocate_pressure_meters (PRES_METER_TYPE type)
{
   if (type == STANDARD_PRESSURE_METER)
   {
      if (allocated_meter != STANDARD_PRESSURE_METER)
      {
         allocated_meter = STANDARD_PRESSURE_METER;

         // deallocate the "other"
         deallocate_pressure_meters (STICK_PRESSURE_METER) ; // there can only be ONE.

         // allocate the background bitmap for the pressure meter
         bitmap_pressure.allocate_resources(*this);

         // text labels
         text_pressure_label.allocate_resources (*this);

         // "draw" / "return" label
         text_draw_return.allocate_resources (*this);

         if (!pressure_indicator)
            pressure_indicator = new OSRectangle(*GuiMain::getDisplay());

         update_standard_pressure_meter();
         add_object_to_front(pressure_indicator);
      }
   }
   else if (type == STICK_PRESSURE_METER)
   {
      if (allocated_meter != STICK_PRESSURE_METER)
      {
         allocated_meter = STICK_PRESSURE_METER;

         // deallocate the "other"
         deallocate_pressure_meters (STANDARD_PRESSURE_METER) ; // there can only be ONE.

         // allocate the stick, pressure meter
         stick_meter.allocate_resources(*this) ;
      }
   }
}  // End of ALLOCATE_PRESSURE_METERS



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_PRESSURE_METERS

void Screen_RUNPROC::deallocate_pressure_meters (PRES_METER_TYPE type)
{
   if (type == STANDARD_PRESSURE_METER)
   {
      text_draw_return.deallocate_resources ();
      text_pressure_label.deallocate_resources();
      bitmap_pressure.deallocate_resources();

      if (pressure_indicator)
      {
         delete_object(pressure_indicator);
      }
   }
   else
   {
      stick_meter.deallocate_resources() ;
   }

}  // End of DEALLOCATE_PRESSURE_METERS


void Screen_RUNPROC::system_state_notify ()
{
   // add delete specific widgets depending on current system state
   if (current_proc_data.system_state == BLOOD_RUN)
   {
      // Add "adjustments" button to RUNPROC screen
      // if not allocated
      if (!btn_lowleft.allocated())
      {
         btn_lowleft.allocate_resources(*this);
         btn_lowleft.set_up_bitmap(BITMAP_BUTTON_ADJUST);
         btn_lowleft.set_callback (aph_callback_lowleft, (void*)this);
      }

   }
   else if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      btn_continue.set_up_bitmap (BITMAP_BUTTON_LARGE_BOT_BLUE_UP);
      btn_continue.set_string_id (textButtonLongNextPage);
      btn_continue.set_callback (callback_display_run_summary, (void*)this);

      // left and right times valid in BLOOD_RUN and only
      text_donation_time_left.deallocate_resources ();
      text_donation_time_right.deallocate_resources ();

      if (guiglobs::screen_status.get_current_system_state() == BLOOD_RINSEBACK &&
          (guiglobs::aasRunInfo.isPasProcedure() ||
           guiglobs::aasRunInfo.isRasProcedure() ||
           guiglobs::isSpecialRinseback()))
         bitmap_dontSeal.allocate_resources(*this);
      else bitmap_dontSeal.deallocate_resources();
   }

   display_time(true);
}

void Screen_RUNPROC::pressure_notify ()
{
   if (current_proc_data.use_stick_meter &&
       allocated_meter != STICK_PRESSURE_METER)
   {
      allocate_pressure_meters(STICK_PRESSURE_METER);
   }
   else if (!current_proc_data.use_stick_meter &&
            allocated_meter != STANDARD_PRESSURE_METER)
   {
      allocate_pressure_meters(STANDARD_PRESSURE_METER);
   }

   if (allocated_meter == STANDARD_PRESSURE_METER)
   {
      update_standard_pressure_meter();
   }
   else
   {
      update_stick_pressure_meter();
   }
}

void Screen_RUNPROC::proc_status_notify ()
{
   chooseAasBars();
   display_time(false);
   display_product_meters();
   display_state();
   previous_proc_data = current_proc_data;

   if (guiglobs::apheresis_status_line->get_current_status_line_type() != ATTENTION_STATUS_LINE ||
       guiglobs::apheresis_status_line->get_current_status_line_text_by_type(ATTENTION_STATUS_LINE) == textStatOptInProgress)
      // Remove optimization in progress message
      guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // DataLog (log_level_gui_info) << "Processed proc status data." << endmsg;

#if CPU==SIMNT
   SimntGuiProcessedProcStatus processedProcStatus(MessageBase::SEND_LOCAL);
   processedProcStatus.send();
#endif // #if CPU==SIMNT
}

void Screen_RUNPROC::run_complete_notify ()
{
   // remove adjust and "exit run" buttons from RUNPROC screen
   btn_cancel.deallocate_resources();
   btn_lowleft.deallocate_resources();

   // Set the text near the pressure indicator to blank
   text_draw_return.set_text_direct(strlen(BLANK_STRING), BLANK_STRING);

   // Remove rinseback in progress message
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // Add "run complete" button to status line
   guiglobs::apheresis_status_line->set_current_text (textStatRunComplete);

   // remove "continue" button to RUNPROC screen for run complete
   btn_continue.deallocate_resources();

   // Add "continue" button to RUNPROC screen
   btn_continue.allocate_resources(*this);
   btn_continue.set_up_bitmap (BITMAP_BUTTON_LARGE_BOT_BLUE_UP);
   btn_continue.set_string_id (textButtonLongContinue);
   btn_continue.set_callback (callback_run_complete, (void*)this);
}

void Screen_RUNPROC::update_stick_pressure_meter ()
{
   const float maxPressure = current_proc_data.inlet_pressure_max;
   const float minPressure = current_proc_data.inlet_pressure_min;

   // Set the meter limits, if they've changed
   if (maxPressure != (int)stick_meter.get_positive_pressure_value_limit())
   {
      stick_meter.set_positive_pressure_value_limit(maxPressure);
   }

   if (minPressure != (int)stick_meter.get_negative_pressure_value_limit())
   {
      stick_meter.set_negative_pressure_value_limit(minPressure);
   }

   // Now set the meter value unconditionally. The meter will detect "no change" conditions
   // and avoid flicker.
   stick_meter.set_meter_value(current_proc_data.pressure);
}

void Screen_RUNPROC::update_standard_pressure_meter ()
{
   OSColor indicator_color =  guipalette::meter_red;

   // i'll leave this commented out code in incase we want to adjust the color transitions again.
//   static OSColor currentColor =  guiglobs::meter_red;

   const float maxPressure = current_proc_data.inlet_pressure_max;
   const float minPressure = current_proc_data.inlet_pressure_min;

   if (current_proc_data.pressure >= maxPressure)
   {

//      if (currentColor != guiglobs::meter_red)
//             DataLog (log_level_gui_info) << "Red bar @ " << current_proc_data.pressure << " (min/max " << minPressure << "/" <<  maxPressure << ")" << endmsg;

//      currentColor =  guiglobs::meter_red;
      current_pressure_indicator_pos = 0;
   }
   else if (current_proc_data.pressure <= minPressure)
   {

      //      if (currentColor != guiglobs::meter_red)
      //            DataLog (log_level_gui_info) << "Red bar @ " << current_proc_data.pressure << " (min/max " << minPressure << "/" <<  maxPressure << ")" << endmsg;



      //     currentColor =  guiglobs::meter_red;
      current_pressure_indicator_pos = MaxPressureIndicatorPos;
   }
   else
   {
      float pressure_difference;
      float fractional_pos;
      //
      // Divide the positive side of the pressure meter ...
      if (current_proc_data.pressure >= 0)
      {
         pressure_difference            = maxPressure;
         fractional_pos                 = ( maxPressure - 1 - current_proc_data.pressure ) / pressure_difference;
         current_pressure_indicator_pos = ( (int)(fractional_pos * ( MaxPressureIndicatorPos - 1 ) ) / 2 ) + 1;
      }
      //
      // Divide the negative side of the pressure meter ...
      else
      {
         pressure_difference            = minPressure + 1;
         fractional_pos                 = (minPressure + 1 - current_proc_data.pressure) / pressure_difference;
         current_pressure_indicator_pos = MaxPressureIndicatorPos - ( ( (int)(fractional_pos * ( MaxPressureIndicatorPos - 1 ) ) / 2 ) + 1 );
      }

      if (current_pressure_indicator_pos <= 2 ||
          current_pressure_indicator_pos >= MaxPressureIndicatorPos - 2)
      {

//          if (currentColor != guiglobs::meter_yellow)
//                DataLog (log_level_gui_info) << "Yellow bar @ " << current_proc_data.pressure << " (min/max " << minPressure << "/" <<  maxPressure << ")" << endmsg;

//         currentColor = guiglobs::meter_yellow;
         indicator_color = guipalette::meter_yellow;
      }
      else
      {
//         if (currentColor != guiglobs::meter_green)
//             DataLog (log_level_gui_info) << "Green bar @ " << current_proc_data.pressure << " (min/max " << minPressure << "/" <<  maxPressure << ")" << endmsg;

//         currentColor = guiglobs::meter_green;
         indicator_color = guipalette::meter_green;
      }
   }


   if (current_pressure_indicator_pos != previous_pressure_indicator_pos)
   {
      OSRegion region(PRESSURE_X + 6, 0, METER_PRESSURE_W, METER_PRESSURE_H);
      region.y = PRESSURE_Y + 55 + 13 * current_pressure_indicator_pos;
      pressure_indicator->set_region(region);
      pressure_indicator->set_color(indicator_color);
      previous_pressure_indicator_pos = current_pressure_indicator_pos;
   }
}

// This function decides whether or not to display the special RAS/PAS bars
void Screen_RUNPROC::chooseAasBars ()
{
   const bool rasOn  = guiglobs::aasRunInfo.isRasProcedure();
   const bool pasOn  = guiglobs::aasRunInfo.isPasProcedure();
   const bool compOn = guiglobs::complementaryPlasmaEnabled() &&  guiglobs::showCompPlsSymbol();

   if (rasOn)
      bitmap_ras_icon.allocate_resources(*this);
   else
      bitmap_ras_icon.deallocate_resources();

   if (pasOn)
      bitmap_pas_icon.allocate_resources(*this);
   else
      bitmap_pas_icon.deallocate_resources();


   if (compOn)
      bitmap_comppls_icon.allocate_resources(*this);
   else
      bitmap_comppls_icon.deallocate_resources();

   /*if (guiglobs::cdsData.config.Procedure.Get().key_plasma_rinseback)
           bitmap_plasma_rinseback_icon.allocate_resources(*this);
   else
           bitmap_plasma_rinseback_icon.deallocate_resources();*/

}

/* FORMAT HASH fda0945a1bd0a9f07e62509fee81f2fc */
