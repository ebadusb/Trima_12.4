/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995 - 1998 COBE BCT, Inc. All rights reserved

*     Author:      Michael Cobb

*     Class name:  Screen_CONFIRM_DON_INFO

*     File name:   confirmDonInfo.cpp

*     Contents:    Implementation of class methods

*     Description: Class representation of the confirm donor info screen

*     Note: This file is ported, with a few changes, from donordl.cpp.  The screen name
       has changed as well.  That's because this is now being used not just for vista
*	   downloads, but also for confirming donor data before connecting the donor.
       See IT 8446 for more details.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>

// header file include
#include "confirmDonInfo.hpp"
#include "frmtdate.hpp"
#include "math.h"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "trimamessages.h"
#include "predict_manager.hpp"
#include "guiglobs.hpp"
#include "software_cds.h"

#include "mathdefs.hpp"

#if (CPU==SIMNT)

Screen_CONFIRM_DON_INFO::CONFIRM_DON_INFO_SCREEN_VERSION Screen_CONFIRM_DON_INFO::_versionToShow = Screen_CONFIRM_DON_INFO::CONFIRM_DON_INFO_VERSION_MALE;
#endif // #if (CPU==SIMNT)

static void cb_gotoDonInfo (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_CONFIRM_DON_INFO::Screen_CONFIRM_DON_INFO()
   : Base_Apheresis_Window (TEAL, GUI_SCREEN_CONFIRM_DON_INFO),

     donor_data_confirmed(false),

     _txtDonorName           (textEmptyFormatDonDlDonorNameInit),
     _txtDonorDob            (textEmptyFormatDonDlDobInit),
     _txtDonorGender         (textEmptyFormatDonDlGenderInit),
     _txtDonorHeight         (textEmptyFormatDonDlHeightInit),
     _txtDonorWeight         (textEmptyFormatDonDlWeightInit),
     _txtDonorTbv            (textDataInitDonDlTbv),
     _txtDonorBloodType      (textEmptyFormatDonDlBloodTypeInit),
     _txtDonorHct            (textEmptyFormatDonDlHctInit),
     _txtDonorPltPrecount    (textMiscDonDlDataPltYield),
     _txtDonorNameAndDobLbl  (textDataLabelDonDlNameAndDob),
     _txtDonorGenderLbl      (textDataLabelDonDlGender),
     _txtDonorHeightLbl      (textDataLabelDonDlHeight),
     _txtDonorWeightLbl      (textDataLabelDonDlWeight),
     _txtDonorBloodTypeLbl   (textDataLabelDonDlBloodType),
     _txtDonorHctLbl         (textDataLabelDonDlHct),
     _txtDonorPltPrecountLbl (textDataLabelDonDlPltPrecount),
     _txtDonorTbvLbl         (textDataLabelDonDlTbv),

     _bmpNameAndDobBox       (SECONDARY_INLAY_X,
                              SECONDARY_INLAY_Y,
                              BITMAP_SECONDARY_INLAY),

     _bmpDonorStats          (BITMAP_DONOR_STATS_X,
                              BITMAP_DONOR_STATS_Y,
                              BITMAP_DONOR_STATS),


     _bmpPtrDonorPhoto(NULL),


     _btnDonorInfo (GUI_BUTTON_DON_CONF_REVITALS,
                    BITMAP_DONOR_IMAGE_X,
                    BITMAP_DONOR_IMAGE_Y,
                    textEmpty,
                    BITMAP_DONOR_IMAGE_DEFAULT_MALE_BTN,
                    BITMAP_DONOR_IMAGE_DEFAULT_BTN_DOWN),


     donor_bitmap_id (BITMAP_NULL),
     _usingHgbUnits  (false),

     _exiting        (false),
     _goback         (false),

     _unexpectedErrorAlarm(GUI_UNEXPECTED_CHOICE)
{
   // reset all system states that define proper screen allocation
   Screen_CONFIRM_DON_INFO::reset_internal_screen_state ();
}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_CONFIRM_DON_INFO::~Screen_CONFIRM_DON_INFO()
{
   // If we haven't already deallocated, do it now.
   Screen_CONFIRM_DON_INFO::deallocate_resources ();
   delete _bmpPtrDonorPhoto;
}   // END of Screen_CONFIRM_DON_INFO DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_CONFIRM_DON_INFO::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   _usingHgbUnits = guiglobs::gui_donor_data_repository.RbcUnitsHgb();

   const TEXT_STRING_ID textId = (_usingHgbUnits ? textDataLabelDonDlHgb : textDataLabelDonDlHct);

   _txtDonorHctLbl.set_string_id(textId);

   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&gui_donor_vitals_struct);

   btn_cancel.deallocate_resources ();

   if (guiglobs::currentlyInPreConnectDonorDataCheck)
   {
      btn_lowleft.deallocate_resources();
   }
   else
   {
      // Properly set the status fields to ensure nobody thinks that this has been committed yet.
      gui_donor_vitals_struct.cDonorVitalsStatus   = DONOR_VITAL_INCOMPLETE;
      gui_donor_vitals_struct.cDonorInfoStatus     = DONOR_INFO_DEFAULT;
      gui_donor_vitals_struct.cDonorDownloadStatus = DONOR_DOWNLOAD_ALL_AP2AGENT_VALUES;

      // now deallocate "standard" buttons that are not needed
      btn_goback.deallocate_resources ();

      // set the reject bitmap on lowleft -- don't activate until screen is settled
      btn_lowleft.set_up_bitmap(BITMAP_BUTTON_DONOR_DELTA_DN);
      btn_lowleft.disable_callbacks();

      // Set the Status Bar: donor info active
      guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_INFO);
   }

   // set the continue button with the proper text -- don't activate until screen is settled
   btn_continue.set_string_id (textButtonConfirmInfo);
   btn_continue.disable_callbacks ();

   // start from back to front on bitmap display widgets
   _bmpDonorStats.allocate_resources(*this);

   // allocate all the text objects - dynamic objects first, having first "cleaned" them.

   // Allocate resources for the donor name widget BEFORE trying to auto-fit it!
   _txtDonorName.allocate_resources(*this);
   // _txtDonorName.set_auto_fit_text(1); This doesn't appear to do anything.
   _txtDonorName.set_string_id(textEmptyFormatDonDlDonorNameInit);

   _txtDonorDob.set_string_id(textEmptyFormatDonDlDobInit);
   _txtDonorDob.allocate_resources(*this);

   _txtDonorGender.set_string_id(textEmptyFormatDonDlGenderInit);
   _txtDonorGender.allocate_resources(*this);

   _txtDonorHeight.set_string_id(textEmptyFormatDonDlHeightInit);
   _txtDonorHeight.allocate_resources(*this);

   _txtDonorWeight.set_string_id(textEmptyFormatDonDlWeightInit);
   _txtDonorWeight.allocate_resources(*this);

   _txtDonorTbv.set_string_id(textDataInitDonDlTbv);
   _txtDonorTbv.allocate_resources(*this);

   _txtDonorBloodType.set_string_id(textEmptyFormatDonDlBloodTypeInit);
   _txtDonorBloodType.allocate_resources(*this);

   _txtDonorHct.set_string_id(textEmptyFormatDonDlHctInit);
   _txtDonorHct.allocate_resources(*this);

   _txtDonorPltPrecount.set_string_id(textMiscDonDlDataPltYield);
   _txtDonorPltPrecount.allocate_resources(*this);


   _txtDonorGenderLbl      .allocate_resources(*this);
   _txtDonorHeightLbl      .allocate_resources(*this);
   _txtDonorWeightLbl      .allocate_resources(*this);
   _txtDonorBloodTypeLbl   .allocate_resources(*this);
   _txtDonorHctLbl         .allocate_resources(*this);
   _txtDonorPltPrecountLbl .allocate_resources(*this);
   _txtDonorTbvLbl         .allocate_resources(*this);

   // Link the status bar/line to this window
   link_status_bar_line();

   // Reset exiting-state variables
   _exiting = false;
   _goback  = false;

   // Hook up to PredictManager, set callbacks
   PredictManager::attach(this);
   PredictManager::set_request_complete_callback(
      Callback<Screen_CONFIRM_DON_INFO>(this,
                                        &Screen_CONFIRM_DON_INFO::screen_notify_handler));

   // submit initial values to Proc for the go-around -- we'll pick up this
   //  message from Safety when Donor Data notifies us of its arrival.
   PredictManager::sendDonorVitals(gui_donor_vitals_struct, HERE);

   // Set handler for receiving a "donor data status" message. NOTE: we
   // intentionally do not use the PredictManager here,because this screen uses
   // the return value of the DonorStatusInfo message, which is not currently
   // supported by PredictManager; and it really isn't needed, since this screen
   // has no coordination issues with other screens (... at the moment). --DGS

   // if the RBC cal set the WBC alarm flag then request the alarm at this time
   if (guiglobs::cdsData.run.wbcAlarmPending.Get())
   {
      RequestGenericWbcAlarm requestGenericWbcAlarm(MessageBase::SEND_GLOBAL);
      requestGenericWbcAlarm.send();
   }

   #if CPU==SIMNT
   // Go ahead set the graphical stuff.
   // display_donor_download_data();
   #endif
}  // END of Screen_CONFIRM_DON_INFO ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_CONFIRM_DON_INFO::deallocate_resources ()
{
   // deallocate all objects
   _bmpNameAndDobBox.deallocate_resources ();

   _btnDonorInfo.deallocate_resources();

   _bmpDonorStats.deallocate_resources ();

   // deallocate all the text objects
   _txtDonorName.deallocate_resources ();
   _txtDonorDob.deallocate_resources ();
   _txtDonorGender.deallocate_resources ();
   _txtDonorHeight.deallocate_resources ();
   _txtDonorWeight.deallocate_resources ();
   _txtDonorTbv.deallocate_resources ();
   _txtDonorBloodType.deallocate_resources ();
   _txtDonorHct.deallocate_resources ();
   _txtDonorPltPrecount.deallocate_resources ();
   _txtDonorNameAndDobLbl.deallocate_resources ();
   _txtDonorGenderLbl.deallocate_resources ();
   _txtDonorHeightLbl.deallocate_resources ();
   _txtDonorWeightLbl.deallocate_resources ();
   _txtDonorBloodTypeLbl.deallocate_resources ();
   _txtDonorHctLbl.deallocate_resources ();
   _txtDonorPltPrecountLbl.deallocate_resources ();
   _txtDonorTbvLbl.deallocate_resources ();

   #if CPU==SIMNT
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);
   #endif

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Unregister for the Donor Update Msg
   donorDataUpdateMsg.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}   // END of Screen_CONFIRM_DON_INFO DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// RESET_INTERNAL_SCREEN_STATE
// This method will reset the internal screen state
void Screen_CONFIRM_DON_INFO::reset_internal_screen_state ()
{
   // reset all local varibles from text to reals
   screen_initialization ();

}   // END of Screen_CONFIRM_DON_INFO RESET_INTERNAL_SCREEN_STATE



////////////////////////////////////////////////////////////////////////////////
// SCREEN_INITIALIZATION
void Screen_CONFIRM_DON_INFO::screen_initialization ()
{
   //////////////////////////////////////////////////////////////////////////
   // values for initialization or reset

   // user has yet to confirm data
   if (!guiglobs::currentlyInPreConnectDonorDataCheck)
      donor_data_confirmed = false;

   // end of initialization requirements*************************************
   //////////////////////////////////////////////////////////////////////////

}   // End of SCREEN_INITIALIZATION

////////////////////////////////////////////////////////////////////////////////
// DONOR_VITALS_GET_STRUCT_MANAGER
void Screen_CONFIRM_DON_INFO::donor_vitals_get_struct_manager ()
{
   // copy donor vitals information over to local struct for resubmission and display purposes
   guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&gui_donor_vitals_struct);

   // format the screen for the operator to commit or reject
   display_donor_download_data();

    #if CPU==SIMNT
   // Toggle from male to female or vice versa.
   toggleVersionToShow();
    #endif
}   // End of DONOR_VITALS_GET_STRUCT_MANAGER


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON
void Screen_CONFIRM_DON_INFO::process_goback_button ()
{
   // Let's freeze the screen from any other button pushes
   disable_standard_buttons ();

   // Remove the notification message, so we won't have flashes of rejected messages from safety
   // nor will we race with AP2Agent as it gets another donor vitals set -- if this screen
   // were to get this notification before this message is deallocated, we'd Fatal_Error.
   donorDataUpdateMsg.deregisterMsg();

   // Remove alarm status line color and message if present
   guiglobs::apheresis_status_line->deactivate_status_line_type(ALERT_STATUS_LINE);

   // Actually, don't do this here, because sysdonor wants to know about it.
   // So, let him take care of it.
   // guiglobs::currentlyInPreConnectDonorDataCheck = false;

   // go back to the sysdonor screen
   goto_screen(GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");
}


////////////////////////////////////////////////////////////////////////////////
// PROCESS_LOWLEFT_BUTTON
void Screen_CONFIRM_DON_INFO::process_lowleft_button ()
{
   // Let's freeze the screen from any other button pushes
   disable_standard_buttons ();

   // Remove the notification message, so we won't have flashes of rejected messages from safety
   // nor will we race with AP2Agent as it gets another donor vitals set -- if this screen
   // were to get this notification before this message is deallocated, we'd Fatal_Error.
   donorDataUpdateMsg.deregisterMsg();

   // reset the screen prior to reject
   screen_initialization ();

   // Remove alarm status line color and message if present
   guiglobs::apheresis_status_line->deactivate_status_line_type(ALERT_STATUS_LINE);

   // set the operator reject value and send the struct
   gui_donor_vitals_struct.cDonorDownloadStatus =  DONOR_DOWNLOAD_OPERATOR_REJECT;

   // resubmit the donor vitals struct with reject value set
   PredictManager::sendDonorVitals(gui_donor_vitals_struct, HERE);

   // We'll exit to the sysdonor screen when the donor vitals transaction completes
   _goback  = true;
   _exiting = true;

   DataLog (log_level_gui_info) << "ConfirmDonorInfoScreen: exiting (goback)" << endmsg;

   guiglobs::currentlyInPreConnectDonorDataCheck = false;
}




////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_CONFIRM_DON_INFO::process_continue_button ()
{
   // Let's freeze the screen from any other button pushes
   disable_standard_buttons ();

   // Remove alarm status line color and message if present
   guiglobs::apheresis_status_line->deactivate_status_line_type(ALERT_STATUS_LINE);


   if (guiglobs::currentlyInPreConnectDonorDataCheck)
   {
      // If we confirm here, go on to connect the operator
      // Screen change will be handled by resulting state transition
      // NOTE:  This is currently ONLY used by the simulator.  Is this ripe for deleting?
      GUIRequestBeginRunMsg request(MessageBase::SEND_LOCAL);
      request.send(0);
   }
   else
   {
      // Remove the notification message, so we won't have flashes of confirmed messages from safety
      donorDataUpdateMsg.deregisterMsg();

      // set the operator confirmation value and send the struct
      gui_donor_vitals_struct.cDonorDownloadStatus = DONOR_DOWNLOAD_OPERATOR_COMMIT;
      gui_donor_vitals_struct.cDonorInfoStatus     = DONOR_INFO_OPERATOR_COMFIRMATION;

      // resubmit the donor vitals struct with confirmation value set
      PredictManager::sendDonorVitals(gui_donor_vitals_struct, HERE);

      // Display optimization in progress message
      guiglobs::apheresis_status_line->set_current_status_line_text_by_type (ATTENTION_STATUS_LINE, textStatOptInProgress);

      // if the user confirms data, the user can never go back without
      // reconfirmation
      donor_data_confirmed = true;

      // goto_screen(GUI_SCREEN_PREDICT, "GUI_SCREEN_PREDICT");
      // We'll go to the predict screen when the donor vitals transaction
      // completes
      _exiting = true;
      _goback  = false;
      DataLog (log_level_gui_info) << "ConfirmDonorInfoScreen: exiting (continue)" << endmsg;
   }

   guiglobs::currentlyInPreConnectDonorDataCheck = false;
}   // END of Screen_CONFIRM_DON_INFO PROCESS_CONTINUE_BUTTON


void Screen_CONFIRM_DON_INFO::shutdown ()
{
   PredictManager::detach(this);

   DataLog (log_level_gui_info) << "ConfirmDonorInfoScreen: finishing exit process, "
                                << (_goback
       ? "going back to sys donor screen"
       : "proceeding to predict screen" )
                                << endmsg;

   if (_goback)
      // go back to the sysdonor screen
      goto_screen(GUI_SCREEN_SYSDONOR, "GUI_SCREEN_SYSDONOR");

   else
      // we go to the prediction screen
      goto_screen(GUI_SCREEN_PREDICT, "GUI_SCREEN_PREDICT");

}


////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER
void Screen_CONFIRM_DON_INFO::screen_notify_handler ()
{
   if (_exiting)
   {
      DataLog (log_level_gui_info) << "ConfirmDonorInfoScreen: synchronization complete, continuing to exit"
                                   << endmsg;

      shutdown();
      return;
   }

   switch (PredictManager::donor_data_source())
   {
      case RECEIVED_FROM_SAFETY :
         // Update Donor Data
         donor_vitals_get_struct_manager();
         break;

      case RECEIVED_FROM_AP2AGENT :
         _unexpectedErrorAlarm.setAlarm("Received new donor data from AP2Agent while processing current donor data.");
         break;

      default :
         _unexpectedErrorAlarm.setAlarm("Unknown donor data update request.");
         break;
   }

}   // END of Screen_CONFIRM_DON_INFO SCREEN_NOTIFY_HANDLER



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_DONOR_DOWNLOAD_DATA
void Screen_CONFIRM_DON_INFO::display_donor_download_data ()
{
   const bool haveDonorName = strlen(gui_donor_vitals_struct.cDonorName) > 0;
   const bool haveDonorDob  = strlen(gui_donor_vitals_struct.cDonorDateofBirth) > 0;

   // Display the Donor's name -- !!! need to work on this in terms of donor Language type text flow via the donor name "language" parameter stored in donordat !!!
   //
   //
   //   !!!!!!!!  DO NOT LOG DONOR NAME OR DOB !!!!!!  This information must not be available
   //   in Trima-saved files in order to protect donor confidentiality!
   //
   //

   if (haveDonorName || haveDonorDob)
   {
      // Don't allocate the label and bitmap for name and DOB unless we have data for at least one of those.
      _bmpNameAndDobBox.allocate_resources(*this);
      _txtDonorNameAndDobLbl.allocate_resources(*this);

      if (haveDonorName)
      {
         // Reload Donor name string info from style record.
         _txtDonorName.set_string_id(textEmptyFormatDonDlDonorNameInit);

         // Fit the name into the text instance's allotted width by successively reducing the font point size.
         _txtDonorName.set_text_direct(strlen(gui_donor_vitals_struct.cDonorName), gui_donor_vitals_struct.cDonorName);
      }

      if (haveDonorDob)
      {
         char birth_buffer[20];
         strcpy (birth_buffer, gui_donor_vitals_struct.cDonorDateofBirth);

         // format the birthday
         Format_date fmtdate;
         fmtdate.format_date(birth_buffer);

         _txtDonorDob.set_text_direct(strlen(birth_buffer), birth_buffer);
      }
   }

   // Display the donor's gender
   switch (gui_donor_vitals_struct.cDonorSex)
   {
      case DONOR_SEX_IS_MALE :
         _txtDonorGender.set_string_id(textMiscDonDlDataGenderMale);
         break;

      case DONOR_SEX_IS_FEMALE :
         _txtDonorGender.set_string_id(textMiscDonDlDataGenderFemale);
         break;

      default :
         DataLog(log_level_gui_error) << "Bad donor info " << gui_donor_vitals_struct << endmsg;
         _unexpectedErrorAlarm.setAlarm("Unable to determine the gender.");
         break;
   }


   // Display the donor's height in it's proper units

   // This block allows all these variables to be removed after use.
   {
      guistring    height_string;
      guistring    str_feet;
      guistring    str_inches;
      guistring    str_cm;

      int          ht_feet;
      int          ht_inches;
      int          ht_cm;

      Display_Text text_portal(textEmpty);

      guistring    get_text_buffer;

      switch (guiglobs::cdsData.config.LangUnit.Get().key_height)
      {
         case FT_HEIGHT :
         {
            int heightInInches = guiglobs::gui_donor_data_repository.heightInInches();
            ht_feet   =      heightInInches / 12;
            ht_inches =    heightInInches % 12;

            sprintf (str_feet, "%d", ht_feet);
            sprintf (str_inches, " %d", ht_inches);
            text_portal.set_string_id(textDataUnitsDonDlFeet);
            text_portal.get_text_direct (get_text_buffer);

            // put the feet in the buffer
            strcpy (height_string, str_feet);

            // add translated feet units
            text_portal.set_string_id(textDataUnitsDonDlFeet);
            text_portal.get_text_direct (get_text_buffer);
            strcat (height_string, get_text_buffer);

            // put the inches in the buffer
            strcat (height_string, str_inches);

            // add the translated inches units
            text_portal.set_string_id(textDataUnitsDonDlInches);
            text_portal.get_text_direct (get_text_buffer);
            strcat (height_string, get_text_buffer);

            // put this height aggregation into the display text
            _txtDonorHeight.set_text_direct(strlen(height_string), height_string);

            // we're done; let's leave
            break;
         }


         case CENTI_HEIGHT :
         {
            ht_cm = guiglobs::gui_donor_data_repository.heightInCm();

            sprintf (str_cm, "%d", ht_cm);

            text_portal.set_string_id(textDataUnitsDonDlCm);
            text_portal.get_text_direct (get_text_buffer);

            // put the centimeters in the buffer
            strcpy (height_string, str_cm);

            // add the units
            strcat (height_string, get_text_buffer);

            // put this height aggregation into the display text
            _txtDonorHeight.set_text_direct(strlen(height_string), height_string);

            // we're done; let's leave
            break;
         }

         default :
            DataLog(log_level_gui_error) << "Bad donor info " << gui_donor_vitals_struct << endmsg;
            _unexpectedErrorAlarm.setAlarm("Unable to determine the height units type.");
            break;

      }  // end of switch (guiglobs::cdsData.config.LangUnit.Get().key_height)

   } // pop height stuff off of the stack




   // Display the donor's weight in it's proper units

   // This block allows all these variables to be removed after use.
   {
      guistring    weight_string;

      guistring    str_pounds;
      guistring    str_kilos;

      int          wt_pounds;
      int          wt_kilos;

      Display_Text text_portal(textEmpty);

      guistring    get_text_buffer;


      switch (guiglobs::cdsData.config.LangUnit.Get().key_weight)
      {
         case LB_WEIGHT :
         {
            wt_pounds = int(round(guiglobs::gui_donor_data_repository.weightInLb()));

            sprintf (str_pounds, "%d", wt_pounds);
            text_portal.set_string_id(textDataUnitsDonDlLbs);
            text_portal.get_text_direct (get_text_buffer);

            // put the pounds in the buffer
            strcpy (weight_string, str_pounds);

            // add translated lbs units
            strcat (weight_string, get_text_buffer);

            // put this weight aggregation into the display text
            _txtDonorWeight.set_text_direct(strlen(weight_string), weight_string);

            // we're done; let's leave
            break;
         }

         case KG_WEIGHT :
         {
            wt_kilos = int(round(guiglobs::gui_donor_data_repository.weightInKg()));

            sprintf (str_kilos, "%d", wt_kilos);
            text_portal.set_string_id(textDataUnitsDonDlKg);
            text_portal.get_text_direct(get_text_buffer);

            // put the kilos in the buffer
            strcpy (weight_string, str_kilos);

            // add translated kg units
            strcat (weight_string, get_text_buffer);

            // put this weight aggregation into the display text
            _txtDonorWeight.set_text_direct(strlen(weight_string), weight_string);

            // we're done; let's leave
            break;
         }

         default :
            DataLog(log_level_gui_error) << "Bad donor info " << gui_donor_vitals_struct << endmsg;
            _unexpectedErrorAlarm.setAlarm("Unable to determine the weight units type.");
            break;

      }  // end of switch (guiglobs::cdsData.config.LangUnit.Get().key_weight)


   } // pop weight stuff off of the stack




   // Display the donor's blood type

   {
      Display_Text text_portal(textEmpty);

      guistring    get_text_buffer;

      // Find a corresponding text string for the blood type and load it into our factotum text object
      switch (gui_donor_vitals_struct.DonorBloodType)
      {
         case BT_O_NEG :
            text_portal.set_string_id(textMiscBloodTypeONeg);
            break;

         case BT_O_POS :
            text_portal.set_string_id(textMiscBloodTypeOPos);
            break;

         case BT_AB_NEG :
            text_portal.set_string_id(textMiscBloodTypeAbNeg);
            break;

         case BT_AB_POS :
            text_portal.set_string_id(textMiscBloodTypeAbPos);
            break;

         case BT_A_NEG :
            text_portal.set_string_id(textMiscBloodTypeANeg);
            break;

         case BT_A_POS :
            text_portal.set_string_id(textMiscBloodTypeAPos);
            break;

         case BT_B_NEG :
            text_portal.set_string_id(textMiscBloodTypeBNeg);
            break;

         case BT_B_POS :
            text_portal.set_string_id(textMiscBloodTypeBPos);
            break;

         case BT_UNKNOWN :
            text_portal.set_string_id(textEmptyFormatDonDlBloodTypeInit);
            break;

         default :
            DataLog(log_level_gui_error) << "Bad donor info " << gui_donor_vitals_struct << endmsg;
            _unexpectedErrorAlarm.setAlarm("Unable to determine the blood type.");
            break;

      }  // End of "switch (gui_donor_vitals_struct.DonorBloodType)

      // Extract the bloodtype text string from the factotum text object
      text_portal.get_text_direct(get_text_buffer);

      // Put the text string into the screen specific text object for blood type
      _txtDonorBloodType.set_string_id(text_portal.getStringId());
      _txtDonorBloodType.set_text_direct(strlen(get_text_buffer), get_text_buffer);

   } // pop bloodtype stuff off of the stack





   // Display the donor's hematocrit

   {
      guistring            displayString, tmpTxtBuffer;

      const TEXT_STRING_ID textId = (_usingHgbUnits ? textDataUnitsDonDlHemoglobin : textDataUnitsDonDlHct);

      Display_Text         text_portal(textId);

      if (_usingHgbUnits)
         sprintf_l10n (displayString, "%.1f ", guiglobs::gui_donor_data_repository.Hgb());
      else
         sprintf (displayString, "%d", int(guiglobs::gui_donor_data_repository.Hct() + 0.5));

      // Extract the percentage text string from the factotum text object
      text_portal.get_text_direct(tmpTxtBuffer);

      // Concatenate the hematocrit and the percentage sign
      strcat(displayString, tmpTxtBuffer);

      // Put the text string into the screen specific text object for hematocrit
      _txtDonorHct.set_text_direct(strlen(displayString), displayString);

   } // pop hematocrit stuff off of the stack




   // Display the donor's platelet precount

   {
      int          int_ppc;
      guistring    str_ppc;

      Display_Text text_portal(textEmpty);

      guistring    get_text_buffer;


      // Convert to a rounded-up integer
      int_ppc = int(gui_donor_vitals_struct.fDonorPltPrecount + 0.5);

      // Make it a string
      sprintf (str_ppc, "%d", int_ppc);

      // Get the units label
      text_portal.set_string_id(textDataUnitsDonDlPltPrecount);

      // Extract the units label text string from the factotum text object
      text_portal.get_text_direct(get_text_buffer);

      // Concatenate the platelet precount and its units label
      strcat(str_ppc, get_text_buffer);

      // Put the text string into the screen specific text object for platelet precount
      _txtDonorPltPrecount.set_text_direct(strlen(str_ppc), str_ppc);

   }  // pop platelet precount stuff off of the stack





   // Display the donor's TBV

   {
      int          int_tbv;
      guistring    str_tbv;

      Display_Text text_portal(textEmpty);

      guistring    get_text_buffer;


      // Convert to a rounded-up integer
      int_tbv = int(guiglobs::donor_tbv() + 0.5);

      // Make it a string
      sprintf (str_tbv, "%d", int_tbv);

      // Get the units label
      text_portal.set_string_id(textDataUnitsDonDlTbv);

      // Extract the units label text string from the factotum text object
      text_portal.get_text_direct(get_text_buffer);

      // Concatenate the TBV and its units label
      strcat(str_tbv, get_text_buffer);

      // Log the Ogawa TBV value
      if (Software_CDS::GetInstance().getFeature(JapanFeatures))
      {
         DataLog (log_level_gui_info) << "Ogawa TBV: " << str_tbv << endmsg;
      }

      // Put the text string into the screen specific text object for TBV
      _txtDonorTbv.set_text_direct(strlen(str_tbv), str_tbv);

   }  // pop TBV stuff off of the stack

   _btnDonorInfo.set_up_bitmap((gui_donor_vitals_struct.cDonorSex == DONOR_SEX_IS_FEMALE) ? BITMAP_DONOR_IMAGE_DEFAULT_FEMALE_BTN : BITMAP_DONOR_IMAGE_DEFAULT_MALE_BTN);
   _btnDonorInfo.allocate_resources(*this);

   BITMAP_ID donorImage = guiglobs::gui_donor_data_repository.GetBitmapId();

   if (donorImage != BITMAP_NULL)
   {
      _bmpPtrDonorPhoto = new Bitmap(0, 0, donorImage);
      _bmpPtrDonorPhoto->allocate_resources(_btnDonorInfo);

      OSRegion btnRegion, bmpRegion;
      _btnDonorInfo.get_region(btnRegion);
      bmpRegion   = _bmpPtrDonorPhoto->get_dimensions();

      bmpRegion.x = (btnRegion.width - bmpRegion.width) / 2;
      bmpRegion.y = (btnRegion.height - bmpRegion.height) / 2;

      if (bmpRegion.x < 0)
         bmpRegion.x = abs(bmpRegion.x);

      if (bmpRegion.y < 0)
         bmpRegion.y = abs(bmpRegion.y);

      _bmpPtrDonorPhoto->set_position(bmpRegion.x, bmpRegion.y);
   }

   // Don't do this if the Donor came from Vista
   if (guiglobs::currentlyInPreConnectDonorDataCheck && !guiglobs::gui_donor_data_repository.isFromVista())
   {
      _btnDonorInfo.set_callback (cb_gotoDonInfo, (void*)this);
      _btnDonorInfo.enable_callbacks();
   }
   else
      _btnDonorInfo.disable_callbacks();

   // Make the continue button and the lowerleft (reject) button active -- they've been deactivated until now.
   if (!guiglobs::currentlyInPreConnectDonorDataCheck)
      btn_lowleft.enable_callbacks();

   btn_continue.enable_callbacks ();
}   // END of Screen_CONFIRM_DON_INFO DISPLAY_DONOR_DOWNLOAD_DATA

//
////////////////////////////////////////////////////////////////////////////////
// callbacks for the remove cassette button
//
void cb_gotoDonInfo (void* data)
{
   ((Screen_CONFIRM_DON_INFO*)data)->gotoDonInfo();
}

void Screen_CONFIRM_DON_INFO::gotoDonInfo ()
{
   goto_screen(GUI_SCREEN_DONVITAL, "DONVITAL from confirm screen");
}


#if CPU==SIMNT
void Screen_CONFIRM_DON_INFO::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   _usingHgbUnits = !_usingHgbUnits;
   const TEXT_STRING_ID textId = (_usingHgbUnits ? textDataLabelDonDlHgb : textDataLabelDonDlHct);
   _txtDonorHctLbl.set_string_id(textId);

   if (traverseInfo.type == UNIVERSAL)
   {
      CONFIRM_DON_INFO_DISPLAY_TYPE confirmDonInfoDisplayType = (CONFIRM_DON_INFO_DISPLAY_TYPE)traverseInfo.parameters[0];

      guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&gui_donor_vitals_struct);

      DataLog (log_level_sim_gui_side_info) << "Confirm don info got display type " << confirmDonInfoDisplayType << " to base class." << endmsg;

      switch (confirmDonInfoDisplayType)
      {
         case CONFIRM_DON_INFO_VERSION_MALE :
            gui_donor_vitals_struct.cDonorSex = DONOR_SEX_IS_MALE;
            break;
         case CONFIRM_DON_INFO_VERSION_FEMALE :
            gui_donor_vitals_struct.cDonorSex = DONOR_SEX_IS_FEMALE;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_OP :
            gui_donor_vitals_struct.DonorBloodType = BT_O_POS;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_ON :
            gui_donor_vitals_struct.DonorBloodType = BT_O_NEG;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_AP :
            gui_donor_vitals_struct.DonorBloodType = BT_A_POS;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_AN :
            gui_donor_vitals_struct.DonorBloodType = BT_A_NEG;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_BP :
            gui_donor_vitals_struct.DonorBloodType = BT_B_POS;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_BN :
            gui_donor_vitals_struct.DonorBloodType = BT_B_NEG;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_ABP :
            gui_donor_vitals_struct.DonorBloodType = BT_AB_POS;
            break;
         case CONFIRM_DON_INFO_BLOOD_TYPE_ABN :
            gui_donor_vitals_struct.DonorBloodType = BT_AB_NEG;
            break;
         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Confirm Donor Info Display Type: " << confirmDonInfoDisplayType << endmsg;
            return;
      }

      display_donor_download_data();
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 84b4f2653fc5f97e82ea93db19e74662 */
