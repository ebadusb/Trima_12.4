/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_DONVITAL

*     File name:   donvital.cpp

*     Contents:    Implementation of class methods

*     Description: Class representaion of the DONVITAL screen

*     Documents:
          Requirements:    I:\....
          Select OMT:      I:\....
          Test:            I:\....

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/donvital.cpp 1.92 2009/07/30 20:14:28Z dslausb Exp dslausb $

*/

#include <vxworks.h>

// "C" callback stubs for redirection to set callback method
static void callback_gender (void* data);
static void callback_height (void* data);
static void callback_weight (void* data);


// header file include
#include "donvital.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "predict_manager.hpp"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_DONVITAL::Screen_DONVITAL()
   : Base_Apheresis_Window (TEAL,
                            GUI_SCREEN_DONVITAL),

     btn_donor_gender  (GUI_BUTTON_DONOR_GENDER,
                        BUTN_DONOR_GENDER_X,
                        BUTN_DONOR_GENDER_Y,
                        TEXT_NULL,
                        BITMAP_BUTTON_DONOR_GENDER_INIT,
                        BITMAP_BUTTON_DONOR_GENDER_BUTTON_DOWN),

     btn_donor_height      (GUI_BUTTON_DONOR_HEIGHT,
                            BUTN_DONOR_HEIGHT_X,
                            BUTN_DONOR_HEIGHT_Y,
                            textButtonGiantDonorVitalsHeight,
                            BITMAP_BUTTON_DONOR_HEIGHT_UP,
                            BITMAP_BUTTON_DONOR_HEIGHT_DN),

     btn_donor_weight      (GUI_BUTTON_DONOR_WEIGHT,
                            BUTN_DONOR_WEIGHT_X,
                            BUTN_DONOR_WEIGHT_Y,
                            textButtonGiantDonorVitalsWeight,
                            BITMAP_BUTTON_DONOR_WEIGHT_UP,
                            BITMAP_BUTTON_DONOR_WEIGHT_DN),

     bitmap_tbv            (SECONDARY_INLAY_X,
                            SECONDARY_INLAY_Y,
                            BITMAP_SECONDARY_INLAY),

     text_tbv              (textMiscLabelDonorVitalsTbv),

     text_donor_tbv        (textMiscInitDonorVitalsTbv)
{
   screen_initialization ();
}     // END of Screen_DONVITAL CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_DONVITAL::~Screen_DONVITAL()

{
   // If we haven't already deallocated, do it now.
   cleanup ();
}     // END of Screen_DONVITAL DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Screen_DONVITAL::allocate_resources (const char* allocation_parameter)

{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // now deallocate "standard" buttons that are not needed
   btn_cancel.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.deallocate_resources ();

   // Allocate the resources for the donor vitals buttons
   btn_donor_gender.allocate_resources (*this);
   btn_donor_height.allocate_resources (*this);
   btn_donor_weight.allocate_resources (*this);

   // get original height/weight text from buttons
   btn_donor_height.set_string_id(textButtonGiantDonorVitalsHeight);
   btn_donor_weight.set_string_id(textButtonGiantDonorVitalsWeight);

   {
      // definition of the safety structure
      SDonorVitals donVitalsStruct;

      // copy donor vitals information over to local struct for resubmission and display purposes
      guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);

      // reset the user tbv confirmed flag upon screen entry
      donVitalsStruct.cDonorVitalsStatus = DONOR_VITAL_NOMINAL_COMPLETE;
      donVitalsStruct.cDonorInfoStatus   = DONOR_INFO_NOMINAL_COMPLETE;

      // submit initial values to donor vitals struct, then retest realization
      // of tbv, finished button, and other button text values using the
      // struct
      donor_vitals_set_struct_manager(donVitalsStruct);
   }

   // if user has already confirmed tbv once do not build go back button
   if (tbv_confirmed)
      btn_goback.deallocate_resources ();

   // set callbacks to the individual button widgets
   btn_donor_gender.set_callback (callback_gender, (void*)this);
   btn_donor_height.set_callback (callback_height, (void*)this);
   btn_donor_weight.set_callback (callback_weight, (void*)this);

   btn_donor_gender.enable_callbacks();
   btn_donor_height.enable_callbacks();
   btn_donor_weight.enable_callbacks();

   // start from back to front on text display widgets
   bitmap_tbv.allocate_resources(*this) ;

   // now add the text to the allocated tbv frame
   text_tbv.allocate_resources(*this) ;
   text_donor_tbv.allocate_resources(*this) ;

   // Link the status bar/line to this window
   link_status_bar_line();

   // Set the Status Bar: donor info active
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_INFO);

   //
   //  NOTE: messages allocated last since updates during allocation are bad.
   //
   // Activate the receive message object to receive the keypad data
   _msgKeypadData.init(Callback<Screen_DONVITAL>(this, &Screen_DONVITAL::get_keypad_struct_manager), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Allocate a scrnevnt handler for receiving a "AC complete" message.
   _msgAcPrimeComplete.init(Callback<Screen_DONVITAL>(this, &Screen_DONVITAL::ac_prime_completed), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Allocate a scrnevnt handler for receiving a "donor data status" message.
   PredictManager::attach(this);
   PredictManager::set_request_complete_callback(
      Callback<Screen_DONVITAL>(this,
                                &Screen_DONVITAL::donor_vitals_get_struct_manager)
      );

}    // END of Screen_DONVITAL ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_DONVITAL::deallocate_resources ()
{
   cleanup();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}     // END of Screen_DONVITAL DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// CLEANUP
void Screen_DONVITAL::cleanup ()
{
   _msgKeypadData.deregisterMsg();
   _msgAcPrimeComplete.deregisterMsg();

   PredictManager::detach(this);

   // deallocate all objects
   btn_donor_gender.deallocate_resources ();
   btn_donor_height.deallocate_resources ();
   btn_donor_weight.deallocate_resources ();
   bitmap_tbv.deallocate_resources ();
   text_tbv.deallocate_resources ();
   text_donor_tbv.deallocate_resources ();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

}


////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE

// This method will reset the internal screen state
void Screen_DONVITAL::reset_internal_screen_state ()
{
   // reset all local varibles from text to reals
   screen_initialization ();

}      // END of Screen_DONVITAL RESET_INTERNAL_SCREEN_STATE


////////////////////////////////////////////////////////////////////////////////
// RESET_DYNAMIC_BUTTON_PARAMETERS

void Screen_DONVITAL::reset_dynamic_button_parameters ()

{
   //////////////////////////////////////////////////////////////////////////
   // initialize buttonface buffers to NULL

   // values for set flags initialized upon request helps to initialize donor
   // vitals screen during construction and struct reevaluation
   sex_set    = false;
   height_set = false;
   weight_set = false;
}        // End of RESET_DYNAMIC_BUTTON_PARAMETERS


////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION

void Screen_DONVITAL::screen_initialization ()
{
   //////////////////////////////////////////////////////////////////////////
   // values for initialization or reset

   // initialize all donor vitals entry flags to false and parameters to init
   reset_dynamic_button_parameters ();

   // user has yet to confirm tbv
   tbv_confirmed = FALSE;

   // change the gender button back to original button face text and color
   btn_donor_gender.set_up_bitmap(BITMAP_BUTTON_DONOR_GENDER_INIT);

   // change the height back to original button face text and color
   btn_donor_height.set_string_id(textButtonGiantDonorVitalsHeight);
   // change the weight back to original button face text and color
   btn_donor_weight.set_string_id(textButtonGiantDonorVitalsWeight);
}      // End of SCREEN_INITIALIZATION

////////////////////////////////////////////////////////////////////////////////
// DONOR_VITALS_SET_STRUCT_MANAGER

void Screen_DONVITAL::donor_vitals_set_struct_manager (SDonorVitals& donVitalsStruct)

{
   PredictManager::sendDonorVitals(donVitalsStruct, HERE);

   btn_donor_gender.disable_callbacks();
   btn_donor_height.disable_callbacks();
   btn_donor_weight.disable_callbacks();
}      // End of DONOR_VITALS_SET_STRUCT_MANAGER


////////////////////////////////////////////////////////////////////////////////
// DONOR_VITALS_GET_STRUCT_MANAGER

void Screen_DONVITAL::donor_vitals_get_struct_manager ()

{
   btn_donor_gender.enable_callbacks();
   btn_donor_height.enable_callbacks();
   btn_donor_weight.enable_callbacks();

   // reset parameters for tbv, finished button, and other button property
   // values before using the struct to test and reset them
   // initialize all donor vitals entry flags to false
   reset_dynamic_button_parameters ();

   // extract male/female value from struct if set, and adjust male/female
   // buttons appropriately
   if (guiglobs::gui_donor_data_repository.male())
   {
      // set the male icon to selected
      btn_donor_gender.set_up_bitmap (BITMAP_BUTTON_DONOR_MALE_SEL);

      // set the "sex_set" flag to true
      sex_set = true;
   }
   else if (guiglobs::gui_donor_data_repository.female())
   {
      // set the female icon to selected
      btn_donor_gender.set_up_bitmap (BITMAP_BUTTON_DONOR_FEMALE_SEL);

      // set the "sex_set" flag to true
      sex_set = true;
   }
   else
   {
      // Initial image since this hasn't been set yet.
      btn_donor_gender.set_up_bitmap (BITMAP_BUTTON_DONOR_GENDER_INIT);
      sex_set = false;
   }

   // extract donor weight from struct and if > 0, ... display it
   if ((guiglobs::gui_donor_data_repository.height()) > 0.0)
   {
      switch (guiglobs::cdsData.config.LangUnit.Get().key_height)
      {
         case FT_HEIGHT :
            btn_donor_height.deallocate_resources();
            btn_donor_height.set_string_id(textButtonGiantDonorVitalsHeightFtSet);
            btn_donor_height.allocate_resources(*this);
            btn_donor_height.set_button_text_color(GREY_LIGHT);
            height_set = true;
            break;

         case CENTI_HEIGHT :
            btn_donor_height.deallocate_resources();
            btn_donor_height.set_string_id(textButtonGiantDonorVitalsHeightCmSet);
            btn_donor_height.allocate_resources(*this);
            btn_donor_height.set_button_text_color(GREY_LIGHT);
            height_set = true;
            break;

         default :
            DataLog (log_level_gui_error) << "Unrecognized height setting." << endmsg;
            break;
      }
   }
   else
      DataLog (log_level_gui_info) << "Height has not yet been set." << endmsg;

   // extract donor weight from struct and if > 0, ... display it
   if (guiglobs::gui_donor_data_repository.weight() > 0.0)
   {
      switch (guiglobs::cdsData.config.LangUnit.Get().key_weight)
      {
         case LB_WEIGHT :
            btn_donor_weight.deallocate_resources();
            btn_donor_weight.set_string_id(textButtonGiantDonorVitalsWeightLbSet);
            btn_donor_weight.allocate_resources(*this);
            btn_donor_weight.set_button_text_color(GREY_LIGHT);
            weight_set = true;
            break;

         case KG_WEIGHT :
            btn_donor_weight.deallocate_resources();
            btn_donor_weight.set_string_id(textButtonGiantDonorVitalsWeightKgSet);
            btn_donor_weight.allocate_resources(*this);
            btn_donor_weight.set_button_text_color(GREY_LIGHT);
            weight_set = true;
            break;

         default :
            DataLog (log_level_gui_error) << "Unrecognized weight setting." << endmsg;
            break;
      }
   }
   else
      DataLog (log_level_gui_info) << "Weight has not yet been set." << endmsg;

   // if all three entries are confirmed, dispaly TBV and "finished" button
   // DonorVitalsComplete will take the place of this decision structure
   if (sex_set && height_set && weight_set)
   {
      //  Load, alter the text, set callback
      //  on the CONFIRM TBV (Continue) button if not yet allocated.
      if (!btn_continue.allocated())
      {
         btn_continue.allocate_resources (*this) ;
         btn_continue.set_string_id (textButtonConfirmInfo);
         btn_continue.set_callback (aph_callback_continue, (void*)this);
      }
   }

   text_donor_tbv.deallocate_resources();
   text_donor_tbv.set_string_id(textMiscInitDonorVitalsTbvSet);
   text_donor_tbv.allocate_resources(*this);

}      // End of DONOR_VITALS_GET_STRUCT_MANAGER


////////////////////////////////////////////////////////////////////////////////
// GET_KEYPAD_STRUCT_MANAGER

void Screen_DONVITAL::get_keypad_struct_manager ()
{
   bool somethingChanged                     = false;
   const KEYPAD_RETURN_STRUCT& keypad_buffer = _msgKeypadData.getData();

   SDonorVitals                donVitalsStruct;

   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);


   switch (keypad_buffer.keypad_type)
   {
      case (HEIGHT_US) :
      {

         // set the structures time member with the return value from keypad
         // and set.
         if (keypad_buffer.keypad_return_value >= MIN_ALLOWABLE_HEIGHT_IN &&
             keypad_buffer.keypad_return_value <= MAX_ALLOWABLE_HEIGHT_IN)
         {
            somethingChanged             = true;
            donVitalsStruct.fDonorHeight = keypad_buffer.keypad_return_value;
         }
      }
      break;

      case (HEIGHT_CM) :
      {

         // set the structures height member with the return value from keypad
         // and set.
         if (keypad_buffer.keypad_return_value >= MIN_ALLOWABLE_HEIGHT_CM &&
             keypad_buffer.keypad_return_value <= MAX_ALLOWABLE_HEIGHT_CM)
         {
            somethingChanged             = true;
            donVitalsStruct.fDonorHeight = keypad_buffer.keypad_return_value;
         }
      }
      break;

      case (WEIGHT_LB) :
      {

         // set the structures weight member with the return value from keypad
         // and set.
         if (keypad_buffer.keypad_return_value >= MIN_ALLOWABLE_WEIGHT_LB &&
             keypad_buffer.keypad_return_value <= MAX_ALLOWABLE_WEIGHT_LB)
         {
            somethingChanged             = true;
            donVitalsStruct.fDonorWeight = keypad_buffer.keypad_return_value;
         }
      }
      break;

      case (WEIGHT_KG) :
      {
         // set the structures time member with the return value from keypad
         // and set.
         if (keypad_buffer.keypad_return_value >= MIN_ALLOWABLE_WEIGHT_KG &&
             keypad_buffer.keypad_return_value <= MAX_ALLOWABLE_WEIGHT_KG)
         {
            somethingChanged             = true;
            donVitalsStruct.fDonorWeight = keypad_buffer.keypad_return_value;
         }
      }
      break;

      default :
         break;
   }      // End of switch (keypad_buffer.keypad_type)

   if (somethingChanged)
      donor_vitals_set_struct_manager(donVitalsStruct);
}      // End of GET_KEYPAD_STRUCT_MANAGER


////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_DONVITAL::process_goback_button ()

{
   // reset the common SDonorVitals structure in DonorDat so that any
   // config changes in SYSDONOR don't conflict with the current
   // SDonorVitals content.
   guiglobs::gui_donor_data_repository.donor_vitals_struct_initialization ();

   // reset the screen prior to goback
   screen_initialization ();

   guiglobs::apheresis_status_bar->set_phase(STATUS_BAR_INFO, STATUS_BAR_STATE_RESET, true);

   // go back to the sysdonor screen
   goto_screen(GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");

}     // END of Screen_DONVITAL PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_GENDER_BUTTON

void Screen_DONVITAL::process_gender_button ()

{
   // definition of the safety structure
   SDonorVitals donVitalsStruct;

   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);

   // on first click, set gender based on which side of button clicked.
   // After that, toggle between male and female
   if (donVitalsStruct.cDonorSex == DONOR_SEX_UNKNOWN)
   {
      if (btn_donor_gender.getLastClickX() < btn_donor_gender.width() / 2)
      {
         donVitalsStruct.cDonorSex = DONOR_SEX_IS_FEMALE;
      }
      else
      {
         donVitalsStruct.cDonorSex = DONOR_SEX_IS_MALE;
      }
   }
   else if (donVitalsStruct.cDonorSex == DONOR_SEX_IS_MALE)
   {
      donVitalsStruct.cDonorSex = DONOR_SEX_IS_FEMALE;
   }
   else
   {
      donVitalsStruct.cDonorSex = DONOR_SEX_IS_MALE;
   }

   // submit initial values to donor vitals struct, then retest realization
   // of tbv, finished button, and other button text values using the
   // struct
   donor_vitals_set_struct_manager(donVitalsStruct);
}     // END of Screen_DONVITAL PROCESS_GENDER_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_HEIGHT_BUTTON

void Screen_DONVITAL::process_height_button ()

{
   // Generate the keypad allocation string
   guistring keypadAllocStrHeight;
   sprintf(keypadAllocStrHeight, "%d", (int)((guiglobs::cdsData.config.LangUnit.Get().key_height == FT_HEIGHT) ? HEIGHT_US : HEIGHT_CM));

   // goto the keypad with the appropriate units
   guiglobs::apheresis_status_line->reparent_status_line(*this);
   goto_child_screen(GUI_SCREEN_KEYPAD, keypadAllocStrHeight);
}     // END of Screen_DONVITAL PROCESS_HEIGHT_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_WEIGHT_BUTTON

void Screen_DONVITAL::process_weight_button ()
{
   // Generate the keypad allocation string
   guistring keypadAllocStrWeight;
   sprintf(keypadAllocStrWeight, "%d", (int)((guiglobs::cdsData.config.LangUnit.Get().key_weight == LB_WEIGHT) ? WEIGHT_LB : WEIGHT_KG));

   // goto the keypad with the appropriate units
   guiglobs::apheresis_status_line->reparent_status_line(*this);
   goto_child_screen(GUI_SCREEN_KEYPAD, keypadAllocStrWeight);
}     // END of Screen_DONVITAL PROCESS_WEIGHT_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_DONVITAL::process_continue_button ()
{

   // If the goback button had been allocated prior to an internal state reset
   // remove it now.
   btn_goback.deallocate_resources();

   // if the user confirms tbv, the user can never go back without
   // reconfirmation
   tbv_confirmed = TRUE;

   // definition of the safety structure
   SDonorVitals donVitalsStruct;

   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct (&donVitalsStruct);

   donVitalsStruct.cDonorVitalsStatus = DONOR_VITAL_OPERATOR_COMFIRMATION;

   // submit initial values to donor vitals struct, then retest realization
   // of tbv, finished button, and other button text values using the
   // struct
   donor_vitals_set_struct_manager(donVitalsStruct);

   goto_screen(GUI_SCREEN_DONINFO, "GUI_SCREEN_DONINFO");

}     // END of Screen_DONVITAL PROCESS_CONTINUE_BUTTON


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_GENDER

void callback_gender (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_DONVITAL*)data) -> process_gender_button ();

}      // END of CALLBACK_GENDER


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_HEIGHT

void callback_height (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_DONVITAL*)data) -> process_height_button ();

}      // END of CALLBACK_HEIGHT


////////////////////////////////////////////////////////////////////////////////
// CALLBACK_WEIGHT

void callback_weight (void* data)
{
   // Actually call the process button routine for the system setup button
   ((Screen_DONVITAL*)data) -> process_weight_button ();

}      // END of CALLBACK_WEIGHT


////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER

void Screen_DONVITAL::ac_prime_completed ()
{
   // only dismiss ATTENTION_STATUS_LINE if it is textStatAcPriming
   if (guiglobs::apheresis_status_line)
   {
      // Remove AC priming message
      guiglobs::apheresis_status_line->deactivate_status_line_text_type(textStatAcPriming, ATTENTION_STATUS_LINE);
   }
}

/* FORMAT HASH a16811a92bad301daaae9afeba047d72 */
