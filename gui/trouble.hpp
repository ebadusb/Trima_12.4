/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_TROUBLE

*     File name:   trouble.hpp

*     Contents:    Definition of class methods and attributes

*     Description: Class header file for trouble.cpp

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef TROUBLE_HPP
#define TROUBLE_HPP


// GUI global element include
#include "guiglobs.hpp"

// GUI class includes
#include "aphwind.hpp"
#include "display_text.h"
#include "gui.h"
#include "scrnstat.hpp"
#include "troublemeter.h"
#include "procadjust_cds.h"

// string definitions and methods
#include "frmtstr.hpp"

// state definitions
#include "states.h"
#include "predict_msgs.h"

// GUI <-> procedure screen include
#include "guiproc.h"

// GUI <-> safety screen include
#include "safproc.h"

// error logging include
#include "error.h"

// sound include
#include "sounds.hpp"

// math include
#include <math.h>


class Screen_TROUBLE
   : public Base_Apheresis_Window

{

public:

   Screen_TROUBLE();

   virtual ~Screen_TROUBLE();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void refresh_resources ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the TROUBLE internal screen state
   virtual void reset_internal_screen_state ();


   // PROCESS_CANCEL_BUTTON
   // This method is called from the C callback stub and executes "exit"
   virtual void process_cancel_button ();


   // PROCESS_DRAW_UP_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_draw_up_button ();


   // PROCESS_DRAW_DOWN_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_draw_down_button ();


   // PROCESS_RETURN_UP_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_return_up_button ();


   // PROCESS_RETURN_DOWN_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_return_down_button ();


   // PROCESS_AC_REACTION_UP_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_ac_reaction_up_button ();


   // PROCESS_AC_REACTION_DOWN_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_ac_reaction_down_button ();


   // PROCESS_CLUMPING_UP_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_clumping_up_button ();


   // PROCESS_CLUMPING_DOWN_BUTTON
   // This method is called from the C callback stub and executes
   // any procedure adjustment request
   void process_clumping_down_button ();


   // PROCESS_CONTINUE_BUTTON
   // This method is called from the C callback stub and executes "continue"
   virtual void process_continue_button ();


   // PROCESS_LOWLEFT_BUTTON
   // This method is called from the C callback stub and executes "predict"
   virtual void process_lowleft_button ();


   // PROCESS_SPILLOVER_BUTTON
   // This method is called from the C callback stub and executes "spillover"
   void process_spillover_button ();


   // PROCESS_AIR_IN_PLASMA_LINE_BUTTON
   // This method is called from the C callback stub and executes "air in plsama line"
   void process_air_in_plasma_line_button ();

   // PROCESS_SALINE_BOLUS_BUTTON
   // This method is called from the C callback stub and executes "saline bolus"
   void process_saline_bolus_button ();

   void adjustment_complete_notify (); // callback when adjustment message
                                       // processing is complete in Proc and Predict

   void updateScreenData ();     // Read the CDS and update the screen; also the
                                 // callback for adjustment status message
                                 // receipt by the AdjustManager.



private:
   // ATTRIBUTES

   const ProcedureStatus::ProcSnapshot& current_proc_data;

   // Exit procedure state machine variables

   bool       _exiting;           // true when either continue or predict button pressed; we are exiting
   bool       _exited;            // true when we are on our way out; stop responding to callbacks
   bool       _predict_requested; // true when someone has requested the predict screen
   bool       _send_is_valid;     // true when we need to send a IS_PROCEDURE_VALID to predict
   bool       _wait_is_valid;     // true when we're waiting for response to IS_PROCEDURE_VALID

   int        previous_remaining_proc_time;

   bool       timeout_occurred; // set to 1 if timeout happens

   int        my_instance_cnt;   // instance count
   static int tot_instance_cnt;  // total instance count

   // character buffers for units
   guistring units_time;     // local storage for time units

   // declare all screen components

   // bitmapped buttons
   Button_Bitmap btn_draw_up;
   Button_Bitmap btn_draw_down;
   Button_Bitmap btn_return_up;
   Button_Bitmap btn_return_down;
   Button_Bitmap btn_ac_reaction_up;
   Button_Bitmap btn_ac_reaction_down;
   Button_Bitmap btn_clumping_up;
   Button_Bitmap btn_clumping_down;
   Button_Bitmap btn_spillover;
   Button_Bitmap btn_spillover_grey;
   Button_Bitmap btn_air_in_plasma;
   Button_Bitmap btn_air_in_plasma_grey;
   Button_Bitmap btn_saline_bolus;
   Button_Bitmap btn_saline_bolus_grey;

   // bitmaps
   Bitmap bitmap_donation_time;
   Bitmap bitmap_draw_return;
   Bitmap bitmap_ac_reaction;
   Bitmap bitmap_clumping;

   // text entries
   Display_Text text_donation_time_display;
   Display_Text text_donation_time_remaining;
   Display_Text text_draw_label;
   Display_Text text_return_label;
   Display_Text text_ac_reaction_label;
   Display_Text text_clumping_label;

   // Screen Timer Handler pointers
   TimerMessage _screenTimeout;
   void screen_timeout ();

   // photon widget declarations
   TroubleMeter meter_draw;
   TroubleMeter meter_return;
   TroubleMeter meter_tingling;
   TroubleMeter meter_clumping;

   // SCREEN_INITIALZATION
   // This method initializes screen components
   void screen_initialization ();


   // GET_DYNAMIC_TEXT
   // This method obtains all dynamic text elements from the text database
   // for manipulation during screen allocation.
   void get_dynamic_text ();


   // FREEZE_SCREEN
   // This method disables all screen objects.
   void freeze_screen ();


   // THAW_SCREEN
   // This method enables all screen objects.
   void thaw_screen ();


   // SCREEN_EXIT
   // This method manages all screen exits
   void screen_exit (GUI_SCREEN_ID, const char*);


   // SCREEN_INPUT_FORMAT
   // This method formats and outputs all data to be displayed on the screen
   void screen_input_format ();


   // TIMEOUT_RESET_MANAGER
   // This method manages all triggered timers
   void timeout_reset_manager ();



protected:

   //  These methods update the operator buttons for recoveries
   void update_operator_buttons ();
   int  display_operator_button (BUTTON_VISIBILITY_STATUS status,
                                 Button_Bitmap& btn,
                                 Button_Bitmap& btn_grey);




//////////////// MESSAGING DECLARATIONS AND SUPPORTING POINTERS ////////////////
////////////////////////////////////////////////////////////////////////////////

protected:
   void proc_status_notify ();
   void system_state_notify ();

   void predict_complete_notify ();

   // these are convenience functions to make it easier to turn callbacks on and off.
   void enable_callbacks ();
   void disable_callbacks ();

   void shutdown ();

};

#endif

/* FORMAT HASH b37af68d14050ada5b925714d704a051 */
