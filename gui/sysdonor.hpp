/*******************************************************************************
 *
 * Copyright (c) 2000 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      sysdonor.hpp
 *
 *
 ******************************************************************************/

// Single include define guardian
#ifndef SYSDONOR_HPP
#define SYSDONOR_HPP

#include <vxworks.h> // Needed here for #if CPU==SIMNT.

// CLASS INCLUDE
#include "guiglobs.hpp"

#include "aphwind.hpp"
#include "aphcalbk.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "button_bitmap.h"
#include "states.h"

#include "guiproc.h"
#include "sysdninc.h"
#include "sdevrec.hpp"

void sysdonor_cb_system_setup (void* data);
void sysdonor_cb_donor_info (void* data);


class Screen_SYSDONOR
   : public Base_Apheresis_Window
{
public:
   Screen_SYSDONOR();
   virtual ~Screen_SYSDONOR();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources, it will also effect SYSDONOR state transitions based on
   // the previous screen and current SYSDONOR state.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // REFRESH_RESOURCES
   // This method will update the linkages to any shareable resources such as
   // the status bar and status line and then rerealize those resources. This
   // method checks that this screen is currently allocated before preforming the
   // refresh.
   virtual void refresh_resources ();


   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();


   // NON_DISPLAY_STATE_ADJUSTMENTS
   // This method checks to see what screen we've come from before SYSDONOR.
   // If it is a screen that doesn't change SYSDONOR display state but requires
   // that certain functions be performed, it is done here.
   virtual void non_display_state_adjustments ();


   // POST_CURRENT_SCREEN_DISPLAY_STATE
   // This method calculates the current SYSDONOR display or phase state based
   // upon what current phase we're in and what screen we've come from.
   virtual void post_current_screen_display_state ();


   // NEW_DONOR_DATA_AVAILABILITY
   // This method posts that fact that donor data is now available in the
   // screen status.
   virtual void new_donor_data_availability ();


   // REMOVE_DONOR_DATA_AVAILABILITY
   // This UN-posts the donor data availability in the SYSDONOR status. This
   // is done when we goto the DONORDL screen so that should the donor be
   // rejected, new donor data will repost donor data availability and trigger
   // state change actions.
   virtual void remove_donor_data_availability ();


   // CONNECT_WITH_AP2_INTERFACE
   // This attempts to connect-up to the AP2 agent and determine whether it
   // has connectivity with AP2. If it can't determine whether it's connected
   // or not, GUI cannot continue to process.
   virtual void connect_with_AP2_Interface ();


   // DISABLE_STANDARD_BUTTONS
   // This method will deactivate the four (cancel, continue, goback, lowleft)
   // standard buttons, if they are allocated - this override will also deactivate
   // the Donor Info and System Setup buttons
   virtual void disable_standard_buttons ();
   virtual void enable_standard_buttons ();


   // PROCESS_CONTINUE_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the continue button.
   virtual void process_continue_button ();


   // PROCESS_CANCEL_BUTTON
   // This method is virtual and should be overriden by any derived class using
   // the cancel button.
   virtual void process_cancel_button ();


   // PROCESS_LOWLEFT_BUTTON
   // This method processes any requests to switch to the configuration
   // main screen. This generic button is labeled with the "config" text
   // label.
   virtual void process_lowleft_button ();


   // PROCESS_SYSTEM_SETUP_BUTTON
   // This method will process any button presses from the system setup button.
   //   If the SYSDONOR_STATE is SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE or
   //   SETUP_NOTCOMPLETE_DONOR_COMPLETE, this button is active and will request
   //   switching to screen SYSDISP. This button will be inactive for the other
   //   two states and display a completed checkoff mark.
   void process_system_setup_button ();


   // PROCESS_DONOR_INFO_BUTTON
   // This method will process any button presses from the donor info button.
   //   In all states this button will be "active", and if pressed will request
   //   a switch to screen DONVITAL. If the SYSDONOR_STATE is SETUP_NOTCOMPLETE_DONOR_COMPLETE
   //   or SETUP_COMPLETE_DONOR_COMPLETE, this button will display a completed checkoff mark.
   void process_donor_info_button ();


   // RESET_INTERNAL_SCREEN_STATE
   // This method will reset the SYSDONOR internal screen state.
   virtual void reset_internal_screen_state ();

private:

   Screen_SYSDONOR            (const Screen_SYSDONOR&);  // noncopyable
   Screen_SYSDONOR& operator = (const Screen_SYSDONOR&); // nonassignable


   // STT: This allows the Sysdonor_Action class - state mgmt. actions - to act upon
   //      all the widgets in SYSDONOR.
   friend class Sysdonor_Action;

   // STT: Declare the state management instance
   Sysdonor_Event_Recorder state_management;

   // Remember that a system reset occurred while the screen was NOT being displayed.
   bool system_was_reset;

   bool ap2_donor_data_available;

   // The system setup bitmap button
   Button_Bitmap btn_system_setup;


   // The donor info bitmap button
   Button_Bitmap btn_donor_info;

   // The donor picture bitmap
   Bitmap* bitmap_donor_picture;

   // The borders of the donor picture for button-like appearance
   Bitmap bitmap_donor_top;
   Bitmap bitmap_donor_bottom;
   Bitmap bitmap_donor_left;
   Bitmap bitmap_donor_right;

   // The SYSDONOR screen state
   SYSDONOR_STATE sysdonor_state;
   SYSDONOR_STATE proc_sysdonor_state;


   // Message to initialize the SYSDONOR screen state
   GUISysDonorStateMsg _sysdonor_state_msg;
   void new_sysdonor_state ();


   // Screen Event Handler pointers - for incoming messages
   EnableLoadSystemMsg _msgEnableLoadSystem;
   void enable_load_system ();

   ACPrimeCompletedMsg _msgAcPrimeComplete;
   void ac_prime_complete ();

   DonorDataStatusMsg _msgNewDonorInfo;
   void new_donor_info ();

   // SET_SYSDONOR_STATE
   // This method will set the state of the screen SYSDONOR. The setting
   // of the sysdonor states will determine which disarmed button faces
   // will be displayed by the btn_system_setup button and the btn_donor_info
   // button. It will also determine whether the btn_system_setup button is
   // active or inactive and whether the continue button is visible and active or
   // invisible and inactive.
   //
   // The screen state transition will be determined at screen "allocate" time
   // by interrogating what was the last screen and the current state. The
   // screen states will follow this simplistic state transition diagram:
   //
   //
   //                   GUI_startup
   //                        |
   //                        |          +----- Commutator "reset screen" call
   //                        |          |
   //                        V          V
   //   +--------->SETUP_NOTCOMPLETE_DONOR_NOTCOMPLETE
   //   |                |                       |
   //   |                | LS=sysacatt           | LS=doninfo, donvital
   //   |                |                       |
   //   |                V                       V
   //   |  SETUP_COMPLETE_DONOR_NOTCOMPLETE   SETUP_NOTCOMPLETE_DONOR_COMPLETE
   //   |                |                       |
   //   |                |                       |
   //   |                | LS=doninfo, donvital  | LS=sysacatt
   //   |                |                       |
   //   |                V                       V
   //   |              SETUP_COMPLETE_DONOR_COMPLETE <---+
   //   |                         |  |                   |
   //   |                         |  |                   | LS=doninfo, donvital, donconn
   //   |      LS=runsumm2        |  |                   |
   //   +-------------------------+  +-------------------+
   //
   //
   //
   //       void set_sysdonor_state (SYSDONOR_STATE new_state);

   // There are four asynchronous events that qualify and alter the widget
   //  configurations of the above functional screen states:
   //
   //     1. AC prime complete message
   //     2. Enable the Load System button message
   //     3. AP2 is in control agent interface state
   //     4. AP2 - Donor Data Available agent interface data
   //
   // These asynchronous messages/data and the inherent four functional screen
   //  states resolve to 17 non-trivial screen states that are managed under
   //  the auspices of the Sysdonor_Event_Recorder object above and the state/action
   //  objects defined in the #include file, sysdstab.hpp.


   // GET_SYSDONOR_STATE
   // This method returns the current state of the SYSDONOR screen.
   SYSDONOR_STATE get_sysdonor_state ();


   // LOAD_STATE_MANAGEMENT_TABLES
   // This method actually creates all the state and action entries via the
   // state management object.
   virtual void load_state_management_tables ();


   // Unit testing code
    #if (CPU==SIMNT)
protected:
   virtual void handleTraverseMessage ();
   virtual void unitTestDisplay (bool loadSetDone, bool donorInfoDone, TEXT_STRING_ID statLine = textEmpty);
public:
   virtual bool readyForDonorConnect () { return btn_continue.allocated(); }
    #endif // #if (CPU==SIMNT)
};

#endif

/* FORMAT HASH c6dea9ecb55b35336861a755751da046 */
