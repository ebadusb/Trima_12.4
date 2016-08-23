/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_DONINFO
*     File name:   doninfo.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the DONINFO screen

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>

#include "doninfo.hpp"

#include "predict_manager.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"

// CALLBACK_BLOOD_TYPE
// This Method sends the blood type to procedure
static void callback_blood_type (void* data);
static void callback_hematocrit (void* data);
static void callback_platelet_precount (void* data);


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_DONINFO::Screen_DONINFO()
   : Base_Apheresis_Window                       (TEAL, GUI_SCREEN_DONINFO),
     current_proc_data                           (guiglobs::proc_status.get_data()),
     hct_set                                     (false),
     tbv_set                                     (false),
     blood_type_set                              (false),
     exiting                                     (false),
     exited                                      (false),
     last_update_sent                            (false),
     waiting_for_donor_data                      (false),
     send_is_valid                               (false),
     wait_is_valid                               (false),
     timeout_occurred                            (false),
     ignore_keypad_message                       (false),
     blood_type_btnface_last_proc                (0),
     donor_blood_type_text_id_to_keypad          (0),
     continue_allocated                          (false),
     hematocrit_value_btnface_last_proc          (0),
     platelet_precount_value_btnface_last_proc   (0),
     hematocrit_value_btnface_last_keypad        (0),
     platelet_precount_value_btnface_last_keypad (0),
     tbv                                         (0),


     btn_donor_blood_type         (GUI_BUTTON_DONOR_BLOOD_TYPE,
                                   BUTN_DONOR_BLOOD_TYPE_X,
                                   BUTN_DONOR_BLOOD_TYPE_Y,
                                   textButtonGiantDonorInfoBloodType,
                                   BITMAP_BUTTON_DONOR_BLOOD_TYPE_UP,
                                   BITMAP_BUTTON_DONOR_BLOOD_TYPE_DN),

     btn_donor_hematocrit         (GUI_BUTTON_DONOR_HEMATOCRIT,
                                   BUTN_DONOR_HEMATOCRIT_X,
                                   BUTN_DONOR_HEMATOCRIT_Y,
                                   textButtonGiantDonorInfoCrit,
                                   BITMAP_BUTTON_DONOR_HEMATOCRIT_UP,
                                   BITMAP_BUTTON_DONOR_HEMATOCRIT_DN),

     btn_donor_platelet_precount  (GUI_BUTTON_DONOR_PLATELET_PRECOUNT,
                                   BUTN_DONOR_PLATELET_PRECOUNT_X,
                                   BUTN_DONOR_PLATELET_PRECOUNT_Y,
                                   textButtonGiantDonorInfoPltPrecount,
                                   BITMAP_BUTTON_DONOR_PLATELET_PRECOUNT_UP,
                                   BITMAP_BUTTON_DONOR_PLATELET_PRECOUNT_DN),

     bitmap_donation_time         (SECONDARY_INLAY_X,
                                   SECONDARY_INLAY_Y,
                                   BITMAP_SECONDARY_INLAY),

     text_hematocrit_units        (textMiscUnitsDonorInfoHematocrit),
     text_platelet_precount_units (textMiscUnitsDonorInfoPltPrecount),
     text_tbv_units               (textMiscUnitsDonorVitalsTbv),
     text_tbv_display             (textMiscInitDonorVitalsTbv),
     text_tbv_label               (textMiscLabelDonorVitalsTbv)

{
   // blood type to text conversion table initialization
   blood_type_strings[0].blood_type           = BT_AB_POS;
   blood_type_strings[0].blood_type_string_id = textButtonKeypadBloodTypeAbPos;
   blood_type_strings[1].blood_type           = BT_AB_NEG;
   blood_type_strings[1].blood_type_string_id = textButtonKeypadBloodTypeAbNeg;
   blood_type_strings[2].blood_type           = BT_B_POS;
   blood_type_strings[2].blood_type_string_id = textButtonKeypadBloodTypeBPos;
   blood_type_strings[3].blood_type           = BT_B_NEG;
   blood_type_strings[3].blood_type_string_id = textButtonKeypadBloodTypeBNeg;
   blood_type_strings[4].blood_type           = BT_A_POS;
   blood_type_strings[4].blood_type_string_id = textButtonKeypadBloodTypeAPos;
   blood_type_strings[5].blood_type           = BT_A_NEG;
   blood_type_strings[5].blood_type_string_id = textButtonKeypadBloodTypeANeg;
   blood_type_strings[6].blood_type           = BT_O_POS;
   blood_type_strings[6].blood_type_string_id = textButtonKeypadBloodTypeOPos;
   blood_type_strings[7].blood_type           = BT_O_NEG;
   blood_type_strings[7].blood_type_string_id = textButtonKeypadBloodTypeONeg;
   blood_type_strings[8].blood_type           = BT_UNKNOWN;
   blood_type_strings[8].blood_type_string_id = TEXT_NULL;

}  // END of Screen_DONINFO CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_DONINFO::~Screen_DONINFO()
{
   cleanup();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_DONINFO::allocate_resources (const char* allocation_parameter)
{
   // DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // Set up callbacks
   enable_callbacks();

   // reset exit-condition state
   exiting                = false;
   exited                 = false;
   last_update_sent       = false;
   waiting_for_donor_data = false;
   send_is_valid          = false;
   wait_is_valid          = false;
   timeout_occurred       = false;
   ignore_keypad_message  = false;    // permit keypad message processing


   // reinitialize the values of the buttonface refresh values explicily
   blood_type_btnface_last_proc                = 0;
   hematocrit_value_btnface_last_proc          = 0.0;
   platelet_precount_value_btnface_last_proc   = 0.0;
   hematocrit_value_btnface_last_keypad        = 0.0;
   platelet_precount_value_btnface_last_keypad = 0.0;
   donor_blood_type_string_last.initialize();

   // initialize the display string compare buffer
   tbv_last.initialize();

   // initialize tbv
   tbv = 0.0;

   // the donor info bitmap buttons
   btn_donor_blood_type.allocate_resources (*this);
   btn_donor_hematocrit.allocate_resources (*this);
   btn_donor_platelet_precount.allocate_resources (*this);

   TEXT_STRING_ID textId = (guiglobs::gui_donor_data_repository.RbcUnitsHct()
                            ? textButtonGiantDonorInfoCrit
                            : textButtonGiantDonorInfoHemoglobin);

   btn_donor_hematocrit.set_string_id(textId);

   // get original text from buttons
   btn_donor_blood_type.set_string_id(textButtonGiantDonorInfoBloodType);
   btn_donor_platelet_precount.set_string_id(textButtonGiantDonorInfoPltPrecount);
   btn_donor_blood_type.get_text_direct(text_blood_type);
   btn_donor_hematocrit.get_text_direct(text_hematocrit);
   btn_donor_platelet_precount.get_text_direct(text_platelet_precount);

   // screen input format TBV text for dissplay
   screen_input_format ();

   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&gui_donor_vitals_struct);

   // set the structures info confirm member with the user not confirmed value
   // and set.
   gui_donor_vitals_struct.cDonorInfoStatus = DONOR_INFO_NOMINAL_COMPLETE;
   donor_vitals_set_struct_manager ();

   // Add the continue button if hct previously set
   if (hct_set == true)
   {

      //  Load, alter the text, set callback CONFIRM HCT (Continue) button.
      btn_continue.set_string_id (textButtonConfirmInfo);
      btn_continue.set_callback (aph_callback_continue, (void*)this);

   }
   else
   {
      btn_continue.deallocate_resources ();
      continue_allocated = false;
   }

   // allocate all non-standard widgets
   textId = (guiglobs::gui_donor_data_repository.RbcUnitsHct()
             ? textMiscUnitsDonorInfoHematocrit
             : textMiscUnitsDonorInfoHemoglobin);

   text_hematocrit_units.set_string_id(textId);

   // set callbacks to the individual button widgets
   btn_donor_blood_type.set_callback (callback_blood_type, (void*)this);
   btn_donor_hematocrit.set_callback (callback_hematocrit, (void*)this);
   btn_donor_platelet_precount.set_callback (callback_platelet_precount, (void*)this);

   btn_donor_blood_type.enable_callbacks();
   btn_donor_hematocrit.enable_callbacks();
   btn_donor_platelet_precount.enable_callbacks();

   // the placard bitmap objects
   bitmap_donation_time.allocate_resources (*this);

   // the display text objects
   text_tbv_display.allocate_resources (*this);
   text_tbv_label.allocate_resources (*this);

   // now get the text that will be used in dynamic screen construction
   get_dynamic_text();

   // Link the status bar/line to this window.  (Note: the apheresis_status_bar
   // pointer *cannot* be null after this, so we don't need to check below.)
   link_status_bar_line();


   // Set the Status Bar color and phase relative to state
   if ((current_proc_data.system_state == BLOOD_PRIME) || (current_proc_data.system_state == BLOOD_RUN))
   {
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_DONATE);
   }
   else
   {
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_INFO);
   }


   // now deallocate "standard" buttons that are not needed based on system
   // state
   if ((current_proc_data.system_state == BLOOD_PRIME) || (current_proc_data.system_state == BLOOD_RUN))
   {
      // go back not needed in run
      btn_goback.deallocate_resources ();

      // set the text on the lowleft button
      btn_lowleft.set_up_bitmap(BITMAP_BUTTON_PREDICT_TEAL_UP);

      // Create the screen timeout timer that will return the user to
      // the appropriate screen
      _timerMsgDonInfoTimeout.armTimer();

   }
   else
   {
      // cancel and lowleft not needed when not in run
      btn_cancel.deallocate_resources ();
      btn_lowleft.deallocate_resources ();
   }

} // END of Screen_DONINFO ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_DONINFO::deallocate_resources ()
{
   cleanup();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}  // END of Screen_DONINFO DEALLOCATE_RESOURCES


void Screen_DONINFO::refresh_resources ()
{
   Base_Apheresis_Window::refresh_resources();
   DataLog(log_level_gui_info) << "Screen_DONINFO: refresh_resources" << endmsg;

   enable_callbacks();
}


void Screen_DONINFO::enable_callbacks ()
{
   _msgKeypadCb.init(Callback<Screen_DONINFO>(this, &Screen_DONINFO::get_keypad_struct_manager));
   _msgAcPrimeComplete.init(Callback<Screen_DONINFO>(this, &Screen_DONINFO::ac_prime_complete_received));


   _timerMsgDonInfoTimeout.init(SCREEN_TIMEOUT_PERIOD, Callback<Screen_DONINFO>(this, &Screen_DONINFO::screen_timer_handler), TimerMessage::DISARMED);

   guiglobs::proc_status.attach(this, 1250);
   guiglobs::proc_status.set_system_state_callback(Callback<Screen_DONINFO>(this, &Screen_DONINFO::system_state_change_notify));
   guiglobs::proc_status.enable_access_pressure_low_warning(true);


   PredictManager::attach(this);
   PredictManager::set_request_complete_callback(Callback<Screen_DONINFO>(this,
                                                                          &Screen_DONINFO::predict_complete_notify));

   PredictManager::set_donor_data_status_callback(Callback<Screen_DONINFO>(this,
                                                                           &Screen_DONINFO::donor_vitals_get_struct_manager));

}

void Screen_DONINFO::disable_callbacks ()
{
   guiglobs::proc_status.detach(this);
   PredictManager::detach(this);

   _msgKeypadCb.deregisterMsg();
   _msgAcPrimeComplete.deregisterMsg();

   _timerMsgDonInfoTimeout.armTimer(TimerMessage::DISARMED);
}

////////////////////////////////////////////////////////////////////////////////
// CLEANUP
void Screen_DONINFO::cleanup ()
{
   DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;
   disable_callbacks();


   // deallocate all non-standard widgets

   // the display text objects
   text_tbv_label.deallocate_resources ();
   text_tbv_display.deallocate_resources ();

   // the placard bitmap objects
   bitmap_donation_time.deallocate_resources ();

   // the donor info bitmap buttons
   btn_donor_blood_type.deallocate_resources ();
   btn_donor_hematocrit.deallocate_resources ();
   btn_donor_platelet_precount.deallocate_resources ();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

}


////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE

// This method will reset the internal screen state
void Screen_DONINFO::reset_internal_screen_state ()
{
   // log the reset for debug purposes
   DataLog(log_level_gui_info) << "GUI DonorInfo Screen INTERNAL RESET" << endmsg;

   // reset all local varibles from text to reals
   screen_initialization ();

}  // END of Screen_DONINFO RESET_INTERNAL_SCREEN_STATE


////////////////////////////////////////////////////////////////////////////////
// FREEZE_SCREEN

// This method will disable all sceen components
void Screen_DONINFO::freeze_screen ()
{
   // DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Disable the rest
   btn_donor_blood_type.disable_callbacks();
   btn_donor_hematocrit.disable_callbacks();
   btn_donor_platelet_precount.disable_callbacks();

}  // END of Screen_DONINO FREEZE_SCREEN


////////////////////////////////////////////////////////////////////////////////
// THAW_SCREEN

// This method will enable all sceen components
void Screen_DONINFO::thaw_screen ()
{
   // DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   // Enable all the standard buttons on this screen
   enable_standard_buttons();

   // Enable the rest
   btn_donor_blood_type.enable_callbacks();
   btn_donor_hematocrit.enable_callbacks();
   btn_donor_platelet_precount.enable_callbacks();

}  // END of Screen_PREDICT THAW_SCREEN


////////////////////////////////////////////////////////////////////////////////
// RESET_DYNAMIC_BUTTON_PARAMETERS

void Screen_DONINFO::reset_dynamic_button_parameters ()
{
   // DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   //////////////////////////////////////////////////////////////////////////
   // values for set flags initialized upon request helps to initialize donor
   // info screen during construction and struct reevaluation

   // initialize buttonface buffers to NULL
   blood_type_string_btnface.initialize();
   hematocrit_string_btnface.initialize();
   platelet_precount_string_btnface.initialize();

   // reinitialize the values of the buttonface strings explicily
   strcpy (blood_type_string_btnface, text_blood_type);
   strcpy (hematocrit_string_btnface, text_hematocrit);
   strcpy (platelet_precount_string_btnface, text_platelet_precount);

}   // End of RESET_DYNAMIC_BUTTON_PARAMETERS


////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION

void Screen_DONINFO::screen_initialization ()
{
   // DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   //////////////////////////////////////////////////////////////////////////
   // values for initialization or reset

   // initialize all donor info entry flags to false and parameters to init
   reset_dynamic_button_parameters ();

   // reset all system variables that define proper screen construction

   // initialize all blood type flag to false separately until included in
   // donor_vitals_get_struct, initialize hematocrit entered to false
   hct_set        = false;
   blood_type_set = false;

   // change the blood type back to original button face text and color
   btn_donor_blood_type.set_string_id(textButtonGiantDonorInfoBloodType);

   // change the hct back to original button face text and color
   const TEXT_STRING_ID textId = (guiglobs::gui_donor_data_repository.RbcUnitsHct()
                                  ? textButtonGiantDonorInfoCrit
                                  : textButtonGiantDonorInfoHemoglobin );

   btn_donor_hematocrit.set_string_id(textId);

   // change the plt precount back to original button face text and color
   btn_donor_platelet_precount.set_string_id(textButtonGiantDonorInfoPltPrecount);

   // end of initialization requirements*************************************
   //////////////////////////////////////////////////////////////////////////

}  // End of SCREEN_INITIALIZATION


////////////////////////////////////////////////////////////////////////////////
// GET_DYNAMIC_TEXT

void Screen_DONINFO::get_dynamic_text ()
{
   // initialize all units and buttonface text buffers
   units_hematocrit.initialize();
   units_platelet_precount.initialize();
   units_tbv.initialize();

   // allocate dynamic text widgets for use
   text_hematocrit_units.allocate_resources(*this);
   text_platelet_precount_units.allocate_resources(*this);
   text_tbv_units.allocate_resources(*this);

   // copy text to local variables
   text_hematocrit_units.get_text_direct(units_hematocrit);
   text_platelet_precount_units.get_text_direct(units_platelet_precount);
   text_tbv_units.get_text_direct(units_tbv);

   // deallocate dynamic text widgets now that they are copied
   text_hematocrit_units.deallocate_resources();
   text_platelet_precount_units.deallocate_resources();
   text_tbv_units.deallocate_resources();


}  // End of GET_DYNAMIC_TEXT


////////////////////////////////////////////////////////////////////////////////
// SCREEN_INPUT_FORMAT

void Screen_DONINFO::screen_input_format ()
{
   format_string convert_to_string;  // class declair for numeric conversion

   // call the conversion utility for tbv
   convert_to_string.convert_number_to_string(tbv_string, "%5.0f", tbv, MAX_LENGTH_TBV_ENTRY);

}


////////////////////////////////////////////////////////////////////////////////
// DONOR_VITALS_SET_STRUCT_MANAGER

void Screen_DONINFO::donor_vitals_set_struct_manager ()
{
   // DataLog(log_level_gui_debug) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   PredictManager::sendDonorVitals(gui_donor_vitals_struct, HERE);

   btn_donor_blood_type.disable_callbacks();
   btn_donor_hematocrit.disable_callbacks();
   btn_donor_platelet_precount.disable_callbacks();

}



////////////////////////////////////////////////////////////////////////////////
// DONOR_VITALS_GET_STRUCT_MANAGER

void Screen_DONINFO::donor_vitals_get_struct_manager ()
{
   if (exited)
      return;

   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__
                               << " got DonorDataStatusmsg " << endmsg;


   // If we're trying to exit (and were just waiting for this processing to
   // complete in other Tasks), continue exiting.
   if (exiting)
   {
      shutdown();
      return;
   }

   // we're waiting...

   if ( PredictManager::waiting())
   {
      DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__
                                  << ", still waiting on predict manager" << endmsg;
      return;
   }


   // Otherwise, carry on.

   format_string convert_to_string;  // class declair for numeric conversion

   btn_donor_blood_type.enable_callbacks();
   btn_donor_hematocrit.enable_callbacks();
   btn_donor_platelet_precount.enable_callbacks();

   // initialize all donor info entry flags to false and parameters to init
   reset_dynamic_button_parameters ();

   // update the local tbv value
   // tbv = gui_donor_vitals_struct.fSafetyDonorTBV;
   tbv = guiglobs::donor_tbv();

   // screen input format prepares the tbv text string for display
   screen_input_format ();

   // initialize display buffer for reconstruction
   tbv_display.initialize();

   // concatinate the tbv units and update text
   strcpy(tbv_display, tbv_string);
   strcat(tbv_display, units_tbv);

   // reset display text
   text_tbv_display.set_text_direct(strlen(tbv_display), tbv_display);

   // reset the last value
   strcpy(tbv_last, tbv_string);

   // if the blood type has been entered, refresh
   if ((gui_donor_vitals_struct.DonorBloodType) > (int)BT_UNKNOWN)
   {
      // if we have a valid blood type for display check against last blood type entered
      if ((int)(gui_donor_vitals_struct.DonorBloodType) != blood_type_btnface_last_proc)
      {
         int index = 0; // index to traverse blood type string table

         // use the blood type table to access donor blood type based on enumeration
         while (blood_type_strings[index].blood_type_string_id != TEXT_NULL)
         {
            if (
               ((int)(blood_type_strings[index].blood_type)) ==
               ((int)(gui_donor_vitals_struct.DonorBloodType))
               )
            {
               text_donor_blood_type_string_id.set_string_id(blood_type_strings[index].blood_type_string_id);
               donor_blood_type_text_id_to_keypad = (int)blood_type_strings[index].blood_type_string_id;
            }
            index += 1;
         }

         // convert text id to text
         guistring donor_blood_type_string;
         text_donor_blood_type_string_id.get_text_direct(donor_blood_type_string);

         // initialize buffers to NULL, then build the buttonface explicitly
         blood_type_string_btnface.initialize();
         strcpy  (blood_type_string_btnface, text_blood_type);
         strcat  (blood_type_string_btnface, "\n"); // provide spacing between values
         strcat  (blood_type_string_btnface, donor_blood_type_string);

         // change the button face text and color
         btn_donor_blood_type.set_text_direct(strlen(blood_type_string_btnface), blood_type_string_btnface);
         btn_donor_blood_type.set_button_text_color(GREY_LIGHT);

         // set refresh gating value
         blood_type_btnface_last_proc = (int)(gui_donor_vitals_struct.DonorBloodType);

      }

      // blood type set flag goes here when and if needed

   }
   // else if the blood type has been cleared entered, refresh
   else if ((gui_donor_vitals_struct.DonorBloodType) == (int)BT_UNKNOWN)
   {
      // if we have a valid blood type for display check against last blood type entered
      if (blood_type_btnface_last_proc != 0)
      {

         // initialize buttonface buffers to NULL
         blood_type_string_btnface.initialize();

         // change the blood type back to original button face text and color
         btn_donor_blood_type.set_string_id(textButtonGiantDonorInfoBloodType);

         // set refresh gating value
         blood_type_btnface_last_proc = 0;

         // set id to zero if no blood type selected
         donor_blood_type_text_id_to_keypad = 0;

      }
   }


   if ((gui_donor_vitals_struct.fDonorHct) > 0.0)
   {
      if ((gui_donor_vitals_struct.fDonorHct) != hematocrit_value_btnface_last_proc)
      {
         // call the conversion utility for hct
         guistring donor_hematocrit_string;

         if (guiglobs::gui_donor_data_repository.RbcUnitsHct())
         {
            convert_to_string.convert_number_to_string(donor_hematocrit_string,
                                                       "%2.0f",
                                                       (gui_donor_vitals_struct.fDonorHct),
                                                       MAX_LENGTH_HEMATOCRIT_ENTRY);
         }
         else
         {
            convert_to_string.convert_number_to_string(donor_hematocrit_string,
                                                       "%2.1f",
                                                       (gui_donor_vitals_struct.fDonorHct / HCT_TO_HGB),
                                                       MAX_LENGTH_HEMOGLOBIN_ENTRY);
         }

         // build buttonface string
         strcat (hematocrit_string_btnface, "\n");
         strcat (hematocrit_string_btnface, donor_hematocrit_string);
         strcat (hematocrit_string_btnface, units_hematocrit);

         // change the button face text and color
         btn_donor_hematocrit.set_text_direct(strlen(hematocrit_string_btnface), hematocrit_string_btnface);
         btn_donor_hematocrit.set_button_text_color(GREY_LIGHT);

         // set refresh gating value
         hematocrit_value_btnface_last_proc = gui_donor_vitals_struct.fDonorHct;

         // Add the continue button by sending the reset struct for proc to see
         if (!continue_allocated)
         {
            continue_allocated = true;
            //  Load, alter the text, set callback CONFIRM HCT (Continue) button.
            btn_continue.allocate_resources (*this) ;
            btn_continue.set_string_id (textButtonConfirmInfo);
            btn_continue.set_callback (aph_callback_continue, (void*)this);
         }
      }

      // set hct set flag to true
      hct_set = true;
   }

   // extract donor weight plt precount from struct and if > 0, ... display it
   if ((gui_donor_vitals_struct.fDonorPltPrecount) > 0.0)
   {
      if ((gui_donor_vitals_struct.fDonorPltPrecount) != platelet_precount_value_btnface_last_proc)
      {
         // call the conversion utility for platelet precount
         guistring donor_platelet_precount_string;
         convert_to_string.convert_number_to_string(donor_platelet_precount_string,
                                                    "%3.0f",
                                                    (gui_donor_vitals_struct.fDonorPltPrecount),
                                                    MAX_LENGTH_PLATELET_PRECOUNT_ENTRY);

         // build buttonface string
         strcat (platelet_precount_string_btnface, "\n"); // provide spacing between values
         strcat (platelet_precount_string_btnface, donor_platelet_precount_string);
         strcat (platelet_precount_string_btnface, units_platelet_precount);

         // change the button face text and color
         btn_donor_platelet_precount.set_text_direct(strlen(platelet_precount_string_btnface), platelet_precount_string_btnface);
         btn_donor_platelet_precount.set_button_text_color(GREY_LIGHT);

         // set refresh gating value
         platelet_precount_value_btnface_last_proc = gui_donor_vitals_struct.fDonorPltPrecount;

      }

      // platelet precount is now set
      guiglobs::donor_platelet_precount_entered = true;

   }

}  // End of DONOR_VITALS_GET_STRUCT_MANAGER



////////////////////////////////////////////////////////////////////////////////
// PROCEDURE_PREDICT_STRUCT_MANAGER
void Screen_DONINFO::predict_complete_notify ()
{
   if (exited) return;


   if (exiting)
   {
      wait_is_valid = false;

      DataLog(log_level_gui_info) << "Screen_DONINFO , notified of predict complete, continuing shutdown" << endmsg;
      shutdown();
      return;
   }

   donor_vitals_get_struct_manager();


}


////////////////////////////////////////////////////////////////////////////////
// GET_KEYPAD_STRUCT_MANAGER

void Screen_DONINFO::get_keypad_struct_manager ()
{
   const KEYPAD_RETURN_STRUCT& keypad_buffer = _msgKeypadCb.getData();

   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);


   // disallow any keypad message processing that may be pending if this flag is TRUE
   if (ignore_keypad_message == true)
   {
      return;
   }

   // Ditto for exit conditions
   if (exiting || exited) return;

   // provide response to proc based on keypad type and return value
   switch (keypad_buffer.keypad_type)
   {
      case (BLOOD_TYPE) :
      {

         // set the structures blood type member with the return value from keypad
         // and set.
         if (((keypad_buffer.keypad_return_value) == 0) &&
             (strcmp((keypad_buffer.keypad_return_string), donor_blood_type_string_last)))
         {
            // initialize the blood type entries to reflect no rbc collected
            gui_donor_vitals_struct.DonorBloodType = BT_UNKNOWN;
            donor_blood_type_text_id_to_keypad     = 0;

            // initialize the blood type string conversion buffer
            guistring donor_blood_type_string;
            donor_blood_type_string.initialize();

            int index = 0;               // index to traverse blood type string table

            // use the blood type table to access donor blood type based on string comparison
            while (blood_type_strings[index].blood_type_string_id != TEXT_NULL)
            {

               // obtain text id then string for return comparison and subsequent forwarding
               // to procedure
               text_donor_blood_type_string_id.set_string_id(blood_type_strings[index].blood_type_string_id);
               text_donor_blood_type_string_id.get_text_direct(donor_blood_type_string);

               // if text string from text database is the same as keypad return value capture
               // corresponding enumerated blood type
               if (!(strcmp(donor_blood_type_string, (keypad_buffer.keypad_return_string))))
               {
                  gui_donor_vitals_struct.DonorBloodType = blood_type_strings[index].blood_type;
                  donor_blood_type_text_id_to_keypad     = (int)blood_type_strings[index].blood_type_string_id;
               }

               index += 1;

            }

            // submit the entry to proc for optimization
            donor_vitals_set_struct_manager ();

            // set refresh gating value
            strcpy(donor_blood_type_string_last, (keypad_buffer.keypad_return_string));

         }

         // if the return value is entry cleared then submit no RBC collected to procedure
         if ((keypad_buffer.keypad_return_value) == -1)
         {
            // set the blood type entry to no rbc collected
            gui_donor_vitals_struct.DonorBloodType = BT_UNKNOWN;

            // reset the text id for transmission to 0
            donor_blood_type_text_id_to_keypad = 0;

            // submit the entry to proc for optimization
            donor_vitals_set_struct_manager ();

         }

      }

      break;

      case (HEMATOCRIT) :
      {
         // set the structures hct member with the return value from keypad
         // and set.
         if (((keypad_buffer.keypad_return_value) >= MIN_ALLOWABLE_HCT) &&
             ((keypad_buffer.keypad_return_value) <= MAX_ALLOWABLE_HCT) &&
             ((keypad_buffer.keypad_return_value) != hematocrit_value_btnface_last_keypad))
         {
            // set the structures weight member with the return value from keypad
            // and set.
            gui_donor_vitals_struct.fDonorHct = keypad_buffer.keypad_return_value;
            donor_vitals_set_struct_manager ();

            // set refresh gating value
            hematocrit_value_btnface_last_keypad = keypad_buffer.keypad_return_value;
         }
      }
      break;

      case (HEMOGLOBIN) :
      {
         // set the structures hct member with the return value from keypad
         // and set.
         if (((keypad_buffer.keypad_return_value_float) >= MIN_ALLOWABLE_HGB) &&
             ((keypad_buffer.keypad_return_value_float) <= MAX_ALLOWABLE_HGB) &&
             ((keypad_buffer.keypad_return_value_float) != hematocrit_value_btnface_last_keypad))
         {
            // set the structures weight member with the return value from keypad
            // and set.
            gui_donor_vitals_struct.fDonorHct = keypad_buffer.keypad_return_value_float * HCT_TO_HGB;

            donor_vitals_set_struct_manager ();

            // set refresh gating value
            hematocrit_value_btnface_last_keypad = keypad_buffer.keypad_return_value_float;
         }
      }
      break;


      case (PLATELET_PRECOUNT) :
      {

         // set the structures precount member with the return value from keypad
         // and set.
         if (((keypad_buffer.keypad_return_value) >= MIN_ALLOWABLE_PRECOUNT) &&
             ((keypad_buffer.keypad_return_value) <= MAX_ALLOWABLE_PRECOUNT) &&
             ((keypad_buffer.keypad_return_value) != platelet_precount_value_btnface_last_keypad))
         {

            // set the structures weight member with the return value from keypad
            // and set.
            gui_donor_vitals_struct.fDonorPltPrecount = keypad_buffer.keypad_return_value;
            donor_vitals_set_struct_manager ();

            // set refresh gating value
            platelet_precount_value_btnface_last_keypad = keypad_buffer.keypad_return_value;

         }
      }
      break;

      default :
         break;
   }  // End of switch (keypad_buffer.keypad_type)

}  // End of GET_KEYPAD_STRUCT_MANAGER


//////////////////////////////////////////////////////////////////////
//
// SHUTDOWN
//
// Implements the logic for the various shutdown states.
//
// Note that some of this code is redundant; we no longer have to detect invalid
// procedures and switch to the predict screen, since the run screen does
// that now), but those changes are extensive and as I write this we're late in
// a development cycle.  This can be cleaed up later (with caution, see the
// extensive comments below). --DGS
//
void Screen_DONINFO::shutdown ()
{

   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__
                               << (exiting                ? " exiting; "                        : "")
                               << (timeout_occurred       ? " timed out; "                      : "")
                               << (last_update_sent       ? " sent last update;"                : "")
                               << (send_is_valid          ? " send is_valid query;"             : "")
                               << (wait_is_valid          ? " waiting on is_valid query;"       : "")
                               << (waiting_for_donor_data ? " waiting for donor data complete;" : "")
                               << (PredictManager::no_product() ? " NO VALID PRODUCTS"          : "")
                               << endmsg;

   // There are several cases where we want to transition to the predict screen;
   // set up the invocation parameter for later use...
   guistring predict_call_type;
   sprintf(predict_call_type, "%d", (int)DONINFO_DELTA);

   // freeze the screen until new product predict is received
   freeze_screen ();

   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);


   // disallow any keypad message processing that may be pending
   ignore_keypad_message = true;

   /// First, make sure there are no outstanding donor vitals transactions
   if ( waiting_for_donor_data )
   {
      // we'll come back here when the donor data arrives
      DataLog(log_level_gui_debug) << "Screen_DONINFO, waiting for donor data" << endmsg;
      return;
   }

   // If we're in rinseback, we can immediately transition to the run screen,
   // since predict no longer has anything useful to say
   if ( guiglobs::proc_status.get_data().system_state == BLOOD_RINSEBACK)
   {
      DataLog(log_level_gui_info) << "Screen_DONINFO, shutdown in rinseback; go straight to run screen."
                                  << endmsg;
      disable_callbacks();
      goto_screen(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      return;
   }


   // If the screen timed out, we need to throw away the operator's changes, and
   // just head back to the run screen (IT 12223), or the predict screen if
   // there are no valid products(13240).
   //
   // We have several complicated behaviors here.  First, IT 12223, demands that
   // we head 'silently' back to the run screen, even if the current procedure
   // is invalid; presumably it's invalid for reasons other than the user's
   // input, and we shouldn't interrupt the run.  On that basis, we clear the
   // 'predict screen requested' flag to prevent the run screen from heading
   // right back to the predict screen.
   //
   // On the other hand, IT 13240, if user made changes that invalidated *all*
   // procedures, and hit the 'goback' button from the predict screen to get
   // here, we *must not* timeout silently, but need to go back to the predict
   // screen.  In this case, we go to the predict screen directly.
   if (timeout_occurred)
   {
      DataLog(log_level_gui_debug) << "Screen_DONINFO, exiting from timeout; "
                                   << "don't update with user's changes (IT 12223),"
                                   << " go back to run screen"
                                   << endmsg;

      PredictManager::clear_prediction_screen_requested();
      disable_callbacks();
      timeout_occurred = false;    // reset for next time (probably redundant)

      // if no products, time out to predict screen
      if (PredictManager::no_product())
         goto_screen(GUI_SCREEN_PREDICT, predict_call_type);
      else                      // otherwise, time out to run screen
         goto_screen(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      return;
   }

   // If we get here, we didn't time out, so we want to make the operator's
   // changes take effect; send one final donor vitals message out before
   // exiting.
   if (!last_update_sent)
   {
      DataLog(log_level_gui_debug) << "Screen_DONINFO, sending last update" << endmsg;
      gui_donor_vitals_struct.cDonorInfoStatus = DONOR_INFO_OPERATOR_COMFIRMATION;
      // Update donor vitals.
      donor_vitals_set_struct_manager ();
      last_update_sent = true;

      // ... we'll come back here when this transaction is complete
      return;
   }


   // If we're waiting on a Predict IS_TRANSACTION_VALID, keep waiting until
   // that transaction is complete (at which point we'll come back here)
   if ( wait_is_valid )
   {
      DataLog(log_level_gui_debug) << "Screen_DONINFO, waiting for is valid" << endmsg;
      return;
   }


   // Now: do we go to the predict screen or the run screen?

   // First: if we're not in the procedure, we know we're going to the predict screen

   bool in_run = (guiglobs::proc_status.get_data().system_state == BLOOD_RUN) ||
                 (guiglobs::proc_status.get_data().system_state == BLOOD_PRIME);

   if (!in_run)
      PredictManager::set_prediction_screen_requested();

   // If we don't already know that we're going to the prediction screen, we
   // need to fire an IS_PROCEDURE_VALID request to find out.  If we need to do
   // that, proceed:
   if (send_is_valid &&  (!PredictManager::prediction_screen_requested()))
   {
      DataLog(log_level_gui_debug) << "Screen_DONINFO, sending is valid" << endmsg;

      send_is_valid = false;
      wait_is_valid = true;
      PredictManager::sendPredictRequest(IS_PROCEDURE_VALID, HERE);

      // and we'll come back where when we're done.
      return;
   }

   // If we get there, we are no longer waiting for anything, and are clear to
   // exit.  Immediately stop processing incoming messages.
   exited = true;
   disable_callbacks();
   // DataLog(log_level_gui_info) << "Screen_DONINFO, exited set!" << endmsg;


   // Where do we go from here?  If the user explicitly requested the predict
   // screen, or if someone out there wants it, go there.
   if ( PredictManager::prediction_screen_requested() )
   {
      DataLog(log_level_gui_debug) << "DONINFO, off to predict screen" << endmsg;


      // user desires to goto the PREDICT screen
      goto_screen(GUI_SCREEN_PREDICT, predict_call_type);
      return;
   }

   DataLog(log_level_gui_debug) << "DONINFO, off to run screen" << endmsg;

   // user desires back to goto the RUNPROC screen
   goto_screen(GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");

}



////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER

void Screen_DONINFO::screen_timer_handler ()
{
   if (exited) return;

   if (current_proc_data.system_state == BLOOD_PRIME || current_proc_data.system_state == BLOOD_RUN)
   {
      DataLog(log_level_gui_info) << "Screen_DONINFO: screen timeout during run"
                                  << endmsg;
      timeout_occurred = true;
      exiting          = true;
      // we know we're going to go back to the run screen
      send_is_valid    = false;
      shutdown();
   }
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_DONINFO::process_cancel_button ()
{
   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);

   // add delete specific widgets depending on current system state
   if (guiglobs::proc_status.rinsebackIsAnOption())
      goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_RINSEBACK);
   else goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_USER);
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_DONINFO::process_goback_button ()
{
   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);

   disable_callbacks();
   // Request to move to the RUNPROC screen
   goto_screen (GUI_SCREEN_DONVITAL, "GUI_SCREEN_DONVITAL");


}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_BLOOD_TYPE_BUTTON

void Screen_DONINFO::process_blood_type_button ()
{

   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);

   // initialize the allocation buffer
   blood_type_allocation_string.initialize();

   // convert keypad type into corresponding string value
   sprintf(blood_type_allocation_string, "%10d", (int)BLOOD_TYPE);
   sprintf(donor_blood_type_string_to_keypad, "%10d", donor_blood_type_text_id_to_keypad);

   strncat(blood_type_allocation_string, "~ ~ ~ ~ ~ ~ ~", strlen("~ ~ ~ ~ ~ ~ ~"));

   strncat(blood_type_allocation_string, donor_blood_type_string_to_keypad, strlen(donor_blood_type_string_to_keypad));

   // goto the keypad with the appropriate units
   if (guiglobs::apheresis_status_line)
      guiglobs::apheresis_status_line->reparent_status_line(*this);

   goto_child_screen(GUI_SCREEN_KEYPAD, blood_type_allocation_string);

}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_HEMATOCRIT_BUTTON

void Screen_DONINFO::process_hematocrit_button ()
{
   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);

   // initialize the allocation buffer
   hematocrit_allocation_string.initialize();

   // convert keypad type into corresponding string value
   if (guiglobs::gui_donor_data_repository.RbcUnitsHct())
      sprintf(hematocrit_allocation_string, "%10d", (int)HEMATOCRIT);
   else
      sprintf(hematocrit_allocation_string, "%10d", (int)HEMOGLOBIN);

   // goto the keypad with the appropriate units
   if (guiglobs::apheresis_status_line)
      guiglobs::apheresis_status_line->reparent_status_line(*this);

   goto_child_screen(GUI_SCREEN_KEYPAD, hematocrit_allocation_string);

}  // END of Screen_DONINFO PROCESS_HEMATOCRIT_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_PLATELET_PRECOUNT_BUTTON

void Screen_DONINFO::process_platelet_precount_button ()
{
   // reset the screen timeout
   _timerMsgDonInfoTimeout.interval(SCREEN_TIMEOUT_PERIOD);

   // initialize the allocation buffer
   plt_precount_allocation_string.initialize();

   // convert keypad type into corresponding string value
   sprintf(plt_precount_allocation_string, "%10d", (int)PLATELET_PRECOUNT);

   // goto the keypad with the appropriate units
   if (guiglobs::apheresis_status_line)
      guiglobs::apheresis_status_line->reparent_status_line(*this);

   goto_child_screen(GUI_SCREEN_KEYPAD, plt_precount_allocation_string);

}  // END of Screen_DONINFO PROCESS_PLATELET_PRECOUNT_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON

void Screen_DONINFO::process_lowleft_button ()
{
   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   PredictManager::set_prediction_screen_requested();
   exiting       = true;
   send_is_valid = false;       // we don't care if the current procedure is valid
   shutdown();

}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_DONINFO::process_continue_button ()
{
   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__ << endmsg;

   exiting       = true;
   send_is_valid = true;
   shutdown();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_BLOOD_TYPE

void callback_blood_type (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad callback data." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_DONINFO*)data) -> process_blood_type_button ();
}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_HEMATOCRIT

void callback_hematocrit (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad callback data." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_DONINFO*)data) -> process_hematocrit_button ();

}


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_PLATELET_PRECOUNT

void callback_platelet_precount (void* data)
{
   if (!data)
   {
      DataLog (log_level_gui_error) << "Bad callback data." << endmsg;
      return;
   }

   // Actually call the process button routine for the system setup button
   ((Screen_DONINFO*)data) -> process_platelet_precount_button ();

}



void Screen_DONINFO::system_state_change_notify ()
{
   if (exited) return;
   DataLog(log_level_gui_info) << "Screen_DONINFO " << __FUNCTION__
                               << " system state callback " << endmsg;

   if (current_proc_data.system_state == BLOOD_RINSEBACK)
   {
      // Request to move to the RUNPROC screen
      disable_callbacks();
      goto_screen (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
   }
}

void Screen_DONINFO::ac_prime_complete_received ()
{
   if (exited) return;
   // only dismiss ATTENTION_STATUS_LINE if it is textStatAcPriming
   if (guiglobs::apheresis_status_line)
   {
      // Remove AC priming message
      guiglobs::apheresis_status_line->deactivate_status_line_text_type(textStatAcPriming, ATTENTION_STATUS_LINE);

   }
}

/* FORMAT HASH f3feacf70774984cf49d66463d7e5b04 */
