/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_RUNPROC

*     File name:   runproc.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for runproc.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#ifndef RUNPROC_HPP
#define RUNPROC_HPP

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"
#include "frmtdec.hpp"
#include "discmetr.hpp"
#include "trimamessages.h"
#include "troublemeter.h"

// string definitions and methods
#include "frmtstr.hpp"

// state definitions
#include "states.h"
// keypad include
#include "keypad.h"

#include "procedure_status.h"

// CLASS DECLARES

class Screen_RUNPROC
   : public Base_Apheresis_Window
{
public:
   enum TimeDisplayType
   {
      DisplayTimeOfDay,
      DisplayMinutes
   };

   enum PRES_METER_TYPE
   {
      STANDARD_PRESSURE_METER,
      STICK_PRESSURE_METER,
      NO_PRESSURE_METER_ALLOCATED
   };

   struct TIMESTRUCT
   {
      int start_proc_time;
      int elapsed_proc_time;
      int total_proc_time;
      int remaining_proc_time;
      int finish_proc_time;
      int time_of_day;
      int day_of_month;

      TIMESTRUCT(TIMESTRUCT& cpyTimeStrct)
         : start_proc_time(cpyTimeStrct.start_proc_time),
           elapsed_proc_time(cpyTimeStrct.elapsed_proc_time),
           total_proc_time(cpyTimeStrct.total_proc_time),
           remaining_proc_time(cpyTimeStrct.remaining_proc_time),
           finish_proc_time(cpyTimeStrct.finish_proc_time),
           time_of_day(cpyTimeStrct.time_of_day),
           day_of_month(cpyTimeStrct.day_of_month)
      {}

      TIMESTRUCT()
         : start_proc_time(0),
           elapsed_proc_time(0),
           total_proc_time(0),
           remaining_proc_time(0),
           finish_proc_time(0),
           time_of_day(0),
           day_of_month(0)
      {}
   };

private:

   // noncopyable, non-assignable
   Screen_RUNPROC            (const Screen_RUNPROC&);  // not defined
   Screen_RUNPROC& operator = (const Screen_RUNPROC&); // not defined

   TIMESTRUCT _currentTime;
   TIMESTRUCT _previousTime;

   void updateTimeStruct ();

   ShowTimeoutPredictScreen _invokePredictScreenForAutoflow;

   //
   // State information for display.  State data which is subject to change during
   // the run has a current and previous value to allow performing display updates
   // only upon change.
   //
   const ProcedureStatus::ProcSnapshot& current_proc_data;
   ProcedureStatus::ProcSnapshot        previous_proc_data;

   bool                   standard_time;         // true for standard (AM/PM), false for military time
   static TimeDisplayType start_finish_display;  // display type for time display

   enum { MaxPressureIndicatorPos = 10 }; // maximum indicator position on standard pressure meter
   int current_pressure_indicator_pos;    // indicator position (bar position) for pressure meter
   int previous_pressure_indicator_pos;

   // The donor vitals bitmap buttons
   Button_Bitmap btn_time_current;
   Button_Bitmap btn_time_min;

   // The timebar, pressure, and collect placard bitmap objects
   Bitmap bitmap_timebar;
   Bitmap bitmap_pressure;
   Bitmap bitmap_platelet;
   Bitmap bitmap_plasma;
   Bitmap bitmap_pas_icon;
   Bitmap bitmap_rbc;
   Bitmap bitmap_ras_icon;
   Bitmap bitmap_comppls_icon;
   Bitmap bitmap_squeeze;
   Bitmap bitmap_dontSeal;

   // storage for language specific strings used in building display text
   guistring units_platelet_estimate;
   guistring units_plasma_estimate;
   guistring units_rbc_estimate;
   guistring units_platelet_current;
   guistring units_plasma_current;
   guistring units_rbc_current;
   guistring units_time;
   guistring units_time_am;
   guistring units_time_pm;
   guistring units_time_delim;

   // The display text objects
   Display_Text text_pressure_label;
   Display_Text text_platelets_label;
   Display_Text text_plasma_label;
   Display_Text text_rbcs_label;
   Display_Text text_platelets_est;
   Display_Text text_platelets_exp_est;
   Display_Text text_plasma_est;
   Display_Text text_rbcs_est;
   Display_Text text_platelets_cur;
   Display_Text text_platelets_exp_cur;
   Display_Text text_plasma_cur;
   Display_Text text_rbcs_cur;
   Display_Text text_draw_return;
   Display_Text text_donation_time_left;
   Display_Text text_donation_time_mid;
   Display_Text text_donation_time_right;
   Display_Text text_date;

   // meter widgets
   OSRectangle*    pressure_indicator;
   PRES_METER_TYPE allocated_meter;

   TroubleMeter*   meter_procedure_time;
   TroubleMeter*   meter_platelets;
   TroubleMeter*   meter_plasma;
   TroubleMeter*   meter_rbcs;

   // Discrete pressure meter used for "stick" feedback
   Discrete_Meter stick_meter ;

   // SCREEN_INITIALZATION
   // This method initializes screen components
   void screen_initialization ();


   // GET_DYNAMIC_TEXT
   // This method obtains all dynamic text elements from the text database
   // for manipulation during screen allocation.
   void get_dynamic_text ();


   // BUILD_METER
   // This method builds all meter widgets
   TroubleMeter* build_meter (OSColor color, TroubleMeter::MeterType type, short x, short y, short height, short width);


   // BUILD_METERS
   // This method defines and controls the construction of all the meter widgets
   void build_all_meters ();

protected:
   void setup_proc_status ();

   void system_state_notify ();
   void run_complete_notify ();

   void pressure_notify ();
   void update_standard_pressure_meter ();
   void update_stick_pressure_meter ();

   void proc_status_notify ();
   void display_time (bool force_update);
   void display_product_meters ();
   void display_state ();

   void display_time_of_day (Display_Text& text, int time_since_midnight);
   void display_minutes (Display_Text& text, int minutes);
   void display_date (Display_Text& text);

   // This function decides whether or not to display the special RAS/PAS bars
   void chooseAasBars ();

   void HandlePredictPopUp ();

   // Upon coming up, we need to check to see if the predict screen has been
   // requested, and if so, to transition to it.
   void TransitionToPredict ();

public:

   Screen_RUNPROC();

   virtual ~Screen_RUNPROC();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);
   virtual void refresh_resources ();


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the RUNPROC internal screen state
   virtual void reset_internal_screen_state ();


   // SELECT_TIME_DISPLAY
   // Set time display type
   void select_time_display (TimeDisplayType type);


   // PROCESS_CANCEL_BUTTON
   // This method is called from the C callback stub and executes "exit"
   virtual void process_cancel_button ();


   // PROCESS_LOWLEFT_BUTTON
   // This method is called from the C callback stub and executes "adjust"
   virtual void process_lowleft_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // the callback for either "continue" or the "donor info" button.
   virtual void process_continue_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the virtual C callback stub and executes
   // the callback for "run complete".
   void process_run_complete_button ();


   // PROCESS_DISPLAY_RUN_SUMMARY_BUTTON
   // This method is called from the virtual C callback stub and executes
   // the callback for "display run summary".
   void process_display_run_summary_button ();


   // ALLOCATE_PRESSURE_METERS
   // This method will allocate the pressure meter and any attendant
   // objects requested by the parameter. Since the two pressure meters
   // are mutually exclusive, the allocation of one of the meters will
   // cause the deallocation of the other.
   void allocate_pressure_meters (PRES_METER_TYPE type) ;


   // DEALLOCATE_PRESSURE_METERS
   // This method will deallocate the pressure meter and any attendant
   // objects requested by the parameter.
   void deallocate_pressure_meters (PRES_METER_TYPE type) ;

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

}; // END of Screen_RUNPROC Class Declaration

// Here's a function to output a timestruct to a dlog stream
// DataLog_Stream & operator << (DataLog_Stream &os, Screen_RUNPROC::TIMESTRUCT timeStruct);

#endif /* ifndef RUNPROC_HPP */

/* FORMAT HASH 1db3240a9767ba537d8772750b22c168 */
