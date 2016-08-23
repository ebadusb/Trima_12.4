/*******************************************************************************
 *
 * Copyright (c) 2003 by Cobe BCT, Inc.  All rights reserved.
 *
 * TITLE:      bio_signatures.cpp
 *             biometric signatures
 *
 * AUTHOR:     Regis McGarry
 *
 * ABSTRACT:   This file provides utility functions for the processing of biometric
 *             signatures.
 *
 * HISTORY:
 *
 ******************************************************************************/


#include <vxWorks.h>
#include <sys/stat.h>
#include <time.h>
#include <usrLib.h>

#include "bio_mgr.h"
#include "random_file_storage.hpp"
#include "pin_file.h"         // Get Pin File functions.
#include "trima_datalog.h"    // Datalog stuff
#include "pfrmsg.h"           // Get PFR Message enums
#include "cfg_ids.h"          // Get tracking values for audit level
#include "guiglobs.hpp"       // Gui globals
#include "adjust.h"           // Get Adjustments Enum
#include "display_text.h"     // To find alarm strings.
#include "text_utils.h"       // Gui Text Utilities

#define MAX_AUTH_EVENTS 300





//
// LogItems
//
void AdjustmentContainer::LogItems ()
{
   GuiTextUtils    guiTextUtils;
   TriggerDataType data;

   if (drawAdjustment)
   {
      if (drawUp > drawDown)
      {
         data.value = ACCESS_PRESSURE_UP;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(ACCESS_PRESSURE_UP));
      }
      else
      {
         data.value = ACCESS_PRESSURE_DOWN;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(ACCESS_PRESSURE_DOWN));
      }

      drawAdjustment = false;
      drawUp         = 0;
      drawDown       = 0;
   }

   if (returnAdjustment)
   {
      if (returnUp > returnDown)
      {
         data.value = RETURN_PRESSURE_UP;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(RETURN_PRESSURE_UP));
      }
      else
      {
         data.value = RETURN_PRESSURE_DOWN;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(RETURN_PRESSURE_DOWN));
      }

      returnAdjustment = false;
      returnUp         = 0;
      returnDown       = 0;
   }

   if (tinglingAdjustment)
   {
      if (tinglingUp > tinglingDown)
      {
         data.value = TINGLING_UP;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(TINGLING_UP));
      }
      else
      {
         data.value = TINGLING_DOWN;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(TINGLING_DOWN));
      }

      tinglingAdjustment = false;
      tinglingUp         = 0;
      tinglingDown       = 0;
   }

   if (clumpingAdjustment)
   {
      if (clumpingUp > clumpingDown)
      {
         data.value = CLUMPING_UP;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(CLUMPING_UP));
      }
      else
      {
         data.value = CLUMPING_DOWN;
         guiglobs::bioMgr().LogEvent(TRIGGER_ADJUSTMENT, data, guiTextUtils.BuildAdjustmentString(CLUMPING_DOWN));
      }

      clumpingAdjustment = false;
      clumpingUp         = 0;
      clumpingDown       = 0;
   }
}



//
// Default constructor
//
BioSignaturesMgr::BioSignaturesMgr()
   : _pinAddMsg(),
     _pfrMsg(),
     _configChangedMsg(),
     _alarmResponseMsg(),
     _procedureSelectedMsg(),
     _adjustMsg(),
     _donorVitalsChangeMsg(),
     _biometricPfrFile(NULL),
     _cantPfr(false),
     _standard_time(true),
     time_text(),
     _guiCds(ROLE_RW),
     bio_gender(DONOR_SEX_UNKNOWN),
     bio_height(0),
     bio_weight(0),
     bio_hematocrit(0),
     bio_platelet(0),
     bio_bloodtype(BT_UNKNOWN)
{}

//
// Default destructor
//
BioSignaturesMgr::~BioSignaturesMgr()
{
   delete _biometricPfrFile;
}


//
// Initialize
//
void BioSignaturesMgr::Initialize ()
{
   // Init the storage file for the alarms.
   struct stat fileStat;

   if (stat(CFR11_FILE_NAME, &fileStat) != OK)
   {
      // File doesn't exist, so open a new one
      _biometricPfrFile = new Random_file_storage(CFR11_FILE_NAME,
                                                  MAX_AUTH_EVENTS,
                                                  sizeof(CFR11_PFR_Storage_Type),
                                                  sizeof(CFR11_Key_Type));
   }
   else
   {
      // Else open the existing one
      _biometricPfrFile = new Random_file_storage(CFR11_FILE_NAME);
   }

   // Open the file storage
   if (_biometricPfrFile->open_file() != 0)
   {
      DataLog_Critical critical;

      DataLog(critical) << "CFR11 Alarm File " << CFR11_FILE_NAME << " failed to open." << endmsg;

      anAlarmMsg alarm(CFR11_LOG_FILE_ERROR);
      alarm.setAlarm("Can't open CFR11 log file");

      return;
   }

   if (_biometricPfrFile->return_record_size() != sizeof(CFR11_PFR_Storage_Type))
   {
      DataLog(log_level_gui_cfr11_error) << "CFR11 file storage size is " << _biometricPfrFile->return_record_size()
                                         << ", should be " << sizeof(CFR11_PFR_Storage_Type) << ".  Erasing file..." << endmsg;

      _biometricPfrFile->close_file();
      delete _biometricPfrFile;

      _biometricPfrFile = new Random_file_storage(CFR11_FILE_NAME,
                                                  MAX_AUTH_EVENTS,
                                                  sizeof(CFR11_PFR_Storage_Type),
                                                  sizeof(CFR11_Key_Type));
      _biometricPfrFile->open_file();
   }

   // Initialize messages.
   // Do here as opposed to in the constructor because message system isn't started when this
   // object is created.
   _pinAddMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandlePinAddMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _pfrMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandlePfrToOccurMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _configChangedMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleConfigChangedMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _erasePinFileMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleErasePinFileMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _numberOfNewPinsMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleNumberOfNewPinsMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
   _writePinFileMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleWritePinFileMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Turn on functionality based on config setting.
   SetAuditLevel();

   _standard_time = guiglobs::cdsData.config.LangUnit.Get().key_time_format;
}  // End BioSignatureMgr::Initialize;



//
// HandlePinAddMsg
//
void BioSignaturesMgr::HandlePinAddMsg ()
{
   const PinSignatureListMsg msg = _pinAddMsg.getData();

   // Add new entries to PinDat, deleting some if necessary) and refresh the pin file.

   // add the new pins.
   for (int i = 0; i < msg.numberOfSignatures; i++)
      PinDat::addPin(msg.signatureList[i].badge, msg.signatureList[i].pin);


   DataLog(log_level_gui_cfr11_info) << msg.numberOfSignatures << " pin's added.  New pin list size is " << PinDat::size() << endmsg;
}  //  End BioSignaturesMgr::HandlePinAddMsg;


//
// HandleErasePinFileMsg
//
void BioSignaturesMgr::HandleErasePinFileMsg ()
{
   DataLog(log_level_gui_cfr11_info) << "Erasing PIN file." << endmsg;

   // Wipe out all of the pins
   PinDat::clear();

   // Now, update the pin file so we are in sync.  No need to log on erasing
   PinDat::update();
}



//
// HandleNumberOfNewPinsMsg
//
void BioSignaturesMgr::HandleNumberOfNewPinsMsg ()
{
   const int numberOfNewPins = _numberOfNewPinsMsg.getData();
   const int listSize        = PinDat::size();

   DataLog(log_level_gui_cfr11_info) << "Adding " << numberOfNewPins << " new PINS." << endmsg;

   // If more than max size pin's, peel off a few
   if (listSize + numberOfNewPins > BIO_MAX_PIN_LIST)
   {
      const int numberToDelete = listSize + numberOfNewPins - BIO_MAX_PIN_LIST;

      DataLog(log_level_gui_cfr11_info) << "PIN file currently has " << listSize
                                        << " entries.  Need to add " << numberOfNewPins << ". Deleting "
                                        << numberToDelete << " pin's." << endmsg;

      for (int i = 0; i < numberToDelete; i++)
      {
         PinDat::erase(PinDat::begin());
      }
   }
}



//
// HandleWritePinFileMsg
//
void BioSignaturesMgr::HandleWritePinFileMsg ()
{
   DataLog(log_level_gui_cfr11_info) << "PIN transfer complete.  Writing file." << endmsg;

   // Now, update the pin file so we are in sync.
   PinDat::update();
}


//
// HandleAirInPlasmLineMsg
//
void BioSignaturesMgr::HandleAirInPlasmLineMsg ()
{
   Display_Text    text;
   guistring       list_text;
   TriggerDataType data;

   data.value = AIR_IN_PLASMA_LINE;

   DataLog(log_level_gui_cfr11_info) << "Add air in plasma line to CFR11 list." << endmsg;

   text.set_string_id(textButtonAdjustAirInPlasma);
   text.get_text_direct(list_text);
   list_text.strip_line_breaks();

   // Log the event
   LogEvent(TRIGGER_AIR_IN_PLASMA_LINE, data, list_text);
}  //  End BioSignaturesMgr::HandleAlarmResponseMsg;


//
// HandleSpilloverMsg
//
void BioSignaturesMgr::HandleSpilloverMsg ()
{
   Display_Text    text;
   guistring       list_text;
   TriggerDataType data;

   data.value = SPILLOVER;

   DataLog(log_level_gui_cfr11_info) << "Add spillover button to CFR11 list." << endmsg;

   text.set_string_id(textButtonAdjustSpillover);
   text.get_text_direct(list_text);
   list_text.strip_line_breaks();

   // Log the event
   LogEvent(TRIGGER_SPILLOVER, data, list_text);
}  //  End BioSignaturesMgr::HandleAlarmResponseMsg;


//
// HandleAlarmResponseMsg
//
void BioSignaturesMgr::HandleAlarmResponseMsg ()
{
   Display_Text                text;
   guistring                   list_text;
   GuiAlarmTable               alarmTable;

   const Alarm_response_struct msg       = _alarmResponseMsg.getData();

   const ALARM_TABLE*          alarmItem = alarmTable.GetEntry(msg.alarm_name);

   // If no alarm found, then not displayable.  Ignore.
   if (alarmItem == 0)
   {
      return;
   }

   // Don't log transitions to screens that are screens themselves or have no status bar text
   if ((alarmItem->goto_screen_gui != GUI_SCREEN_NULL_SCREEN) ||
       (alarmItem->alarm_status_line_text == textEmpty))
   {
      return;
   }

   //
   //  only use msg sent from the alarm screen
   //   alarm_response = ABORT | ABORT_THRU_RINSEBACK | CONTINUE | UNLOAD
   //
   if (msg.alarm_response != CONFIRM)
   {
      DataLog(log_level_gui_cfr11_info) << "Received Alarm Msg " << msg.alarm_name << ", response " << msg.alarm_response << "." << endmsg;

      TriggerDataType data;
      data.value = msg.alarm_name;

      text.set_string_id(alarmItem->alarm_status_line_text);
      text.get_text_direct(list_text);

      // Log the event
      LogEvent(TRIGGER_ALARM, data, list_text);
   }
}  //  End BioSignaturesMgr::HandleAlarmResponseMsg;


//
// HandlePfrToOccurMsg
//
void BioSignaturesMgr::HandlePfrToOccurMsg ()
{
   if (_biometricPfrFile == NULL)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "PFR Restore happened before BioSignatureMgr initialized.");
   }

   const PFR_Status pfrStatus = (PFR_Status)_pfrMsg.getData();

   DataLog(log_level_gui_cfr11_info) << "PFR Status " << pfrStatus << endmsg;

   if (pfrStatus == NoRestore)
   {
      // Free all records from the last run.
      _biometricPfrFile->free_all_records();
      _biometricPfrFile->commit_update();
   }
   else
   {
      // Restore the GUI List
      _biometricPfrFile->rewind_cursor();

      CFR11_PFR_Storage_Type pfrItem;
      CFR11_Log_Type         logItem;
      CFR11_Key_Type         key;
      int                    count = 0;

      for (;; )
      {
         // Get the record from the PFR file
         const int recordNumber = _biometricPfrFile->read_next((REC_TYPE*)&pfrItem, (KEY_TYPE*)&key);
         taskDelay(0); // Make sure other tasks get a chance to interupt (avoid priority inversion)

         if (recordNumber < 0)
         {
            DataLog(log_level_gui_cfr11_info) << "Restored " << count << " CFR 11 Items." << endmsg;
            break;
         }

         count++;
         logItem = pfrItem.logItem;
         DataLog(log_level_gui_cfr11_info) << "Read item " << logItem << ".  Key = " << key << ", RN = " << recordNumber << endmsg;

         // Stuff the gui cfr list with the item.
         CFR11_Tracking_List listItem;
         listItem.key      = recordNumber;
         listItem.btn_text = pfrItem.logString;
         listItem.trigger  = pfrItem.logItem.trigger;

         guiglobs::cfr_list.push_back(listItem);
      }

      DataLog (log_level_gui_cfr11_info) << "PFR added " << count << " items to CFR11 list." << endmsg;
      dumpCfrList(__FILE__, __LINE__);
   }
}  //  End BioSignaturesMgr::HandlePfrToOccurMsg;



//
// HandleConfigChangedMsg
//
void BioSignaturesMgr::HandleConfigChangedMsg ()
{
   // Set the audit level
   SetAuditLevel();
}



//
// HandleAdjustmentMsg
//
void BioSignaturesMgr::HandleAdjustmentMsg ()
{
   const AdjustmentPayload           payload(_adjustMsg.getData());
   const troubleshooting_adjustments adjustment = payload._enum;

   // Don't log this action.
   if (adjustment == SALINE_BOLUS)
   {
      return;
   }

   DataLog(log_level_gui_cfr11_info) << "Received Adjustment Msg " << adjustment << "." << endmsg;

   switch (adjustment)
   {
      case ACCESS_PRESSURE_UP :
         _adjustmentContainer.drawAdjustment = true;
         _adjustmentContainer.drawUp++;
         break;

      case ACCESS_PRESSURE_DOWN :
         _adjustmentContainer.drawAdjustment = true;
         _adjustmentContainer.drawDown++;
         break;

      case RETURN_PRESSURE_UP :
         _adjustmentContainer.returnAdjustment = true;
         _adjustmentContainer.returnUp++;
         break;

      case RETURN_PRESSURE_DOWN :
         _adjustmentContainer.returnAdjustment = true;
         _adjustmentContainer.returnDown++;
         break;

      case TINGLING_UP :
         _adjustmentContainer.tinglingAdjustment = true;
         _adjustmentContainer.tinglingUp++;
         break;

      case TINGLING_DOWN :
         _adjustmentContainer.tinglingAdjustment = true;
         _adjustmentContainer.tinglingDown++;
         break;

      case CLUMPING_UP :
         _adjustmentContainer.clumpingAdjustment = true;
         _adjustmentContainer.clumpingUp++;
         break;

      case CLUMPING_DOWN :
         _adjustmentContainer.clumpingAdjustment = true;
         _adjustmentContainer.clumpingDown++;
         break;

      case ADJUSTMENT_SCREEN_CLOSED :
         // Log the event
         _adjustmentContainer.LogItems();
         break;

      case SPILLOVER :
         HandleSpilloverMsg();
         break;

      case AIR_IN_PLASMA_LINE :
         HandleAirInPlasmLineMsg();
         break;

      default :
         DataLog(log_level_gui_cfr11_error) << "Unexpected Adjustment Choice " << adjustment << endmsg;
         return;
   }
}


//
// HandleDonorVitalsChangeMsg
//
void BioSignaturesMgr::HandleDonorVitalsChangeMsg ()
{
   const SDonorVitals msg = _donorVitalsChangeMsg.getData();

   DataLog(log_level_gui_cfr11_info) << "Received Donor Vitals Changed Msg " << (int)msg.cDonorInfoStatus << endmsg;

   TriggerDataType data;

   Display_Text    displayText;
   guistring       list_text;

   // Get the comma delimiter
   guistring comma_delimiter;
   displayText.set_string_id(textMiscCfrCommaDelim);
   displayText.get_text_direct(comma_delimiter);

   CDonor donor(msg);

   // Only log when donor info has been entered by operator or when adjustments are made
   switch (msg.cDonorInfoStatus)
   {
      case DONOR_INFO_OPERATOR_COMFIRMATION :
      {
         //
         // First, log the Donor Vitals A (Gender, Height, Weight)
         //
         data.donor_vitals_a.gender = donor.Gender();
         data.donor_vitals_a.height = donor.HeightInCm();
         data.donor_vitals_a.weight = donor.WeightInKg();

         // Build the informational text string for the operator
         BuildDonorVitalsAMsg(msg, list_text);

         DataLog(log_level_gui_cfr11_info) << "Donor Info Operator Confirmation("
                                           << donor.Gender() << ", " << donor.HeightInCm() << ", "
                                           << donor.WeightInKg() << ").  " << list_text << endmsg;

         // Log the event
         if (list_text.length() != 0)
         {
            LogEvent(TRIGGER_DONOR_VITALS_A_CHANGE, data, list_text);
         }

         //
         // Next, log the Donor Vitals B (Blood Type, Hematocrit, Platelet Count)
         //
         data.donor_vitals_b.bloodType     = donor.BloodType();
         data.donor_vitals_b.hematocrit    = donor.Hct();
         data.donor_vitals_b.plateletCount = donor.PreCnt();

         // Build the informational text string for the operator
         BuildDonorVitalsBMsg(msg, list_text);

         DataLog(log_level_gui_cfr11_info) << "Donor Info Operator Confirmation("
                                           << donor.BloodType() << ", " << donor.Hct() << ", "
                                           << donor.PreCnt() << ").  " << list_text << endmsg;

         // Log the event
         if (list_text.length() != 0)
         {
            LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, list_text);
         }

         break;
      }

      case DONOR_VITAL_OPERATOR_COMFIRMATION :
         //
         // Next, log the Donor Vitals B (Blood Type, Hematocrit, Platelet Count)
         //
      {
         data.donor_vitals_b.bloodType     = donor.BloodType();
         data.donor_vitals_b.hematocrit    = donor.Hct();
         data.donor_vitals_b.plateletCount = donor.PreCnt();

         // Build the informational text string for the operator
         BuildDonorVitalsBMsg(msg, list_text);

         DataLog(log_level_gui_cfr11_info) << "Donor Info Operator Confirmation("
                                           << donor.BloodType() << ", " << donor.Hct() << ", "
                                           << donor.PreCnt() << ").  " << list_text << endmsg;

         // Log the event
         if (list_text.length() != 0)
         {
            LogEvent(TRIGGER_DONOR_VITALS_B_CHANGE, data, list_text);
         }

         break;
      }

      case DONOR_DOWNLOAD_OPERATOR_COMMIT :
      {
         bio_gender     = (int)msg.cDonorSex;
         bio_height     = msg.fDonorHeight;
         bio_weight     = msg.fDonorWeight;

         bio_hematocrit = msg.fDonorHct;
         bio_platelet   = msg.fDonorPltPrecount;
         bio_bloodtype  = msg.DonorBloodType;
      }
      break;

      default :
         break;  // Don't do anything
   }
}



//
// HandleProcedureSelectedMsg
//
void BioSignaturesMgr::HandleProcedureSelectedMsg ()
{
   guistring    list_text;
   Display_Text new_procedure(textMiscCfrActionNewProcedureSelected);

   DataLog(log_level_gui_cfr11_info) << "New Procedure " << _procedureSelectedMsg
                                     << " selected.  Last procedure was " << _guiCds.cfr11ProcedureNumber << endmsg;

   // Only log changes.
   if (_procedureSelectedMsg != _guiCds.cfr11ProcedureNumber)
   {
      // Log the event
      TriggerDataType data;
      data.value = _procedureSelectedMsg;
      _guiCds.cfr11ProcedureNumber.Set(_procedureSelectedMsg);

      new_procedure.get_text_direct(list_text);

      LogEvent(TRIGGER_PROCEDURE_SELECTED, data, list_text);
   }
}


//
// BuildDonorVitalsAMsg
//
void BioSignaturesMgr::BuildDonorVitalsAMsg (const SDonorVitals& vitals, guistring& msg)
{
   GuiTextUtils guiTextUtils;

   Display_Text displayText(textEmpty);

   guistring    info_string;
   guistring    guiString;

   guistring    gender_label;
   guistring    height_label;
   guistring    weight_label;

   // Get the comma delimiter
   guistring comma_delimiter;
   displayText.set_string_id(textMiscCfrCommaDelim);
   displayText.get_text_direct(comma_delimiter);

   if ((bio_gender == DONOR_SEX_UNKNOWN) || (bio_gender != (int)vitals.cDonorSex))
   {
      // Build the donor's gender
      guiTextUtils.BuildDonorGenderString(vitals, guiString);

      displayText.set_string_id(textMiscCfrDonInfoGender);
      displayText.get_text_direct(gender_label);
      gender_label += guiString;

      // Always the first part of string so no need to check for use of comma delimeter.
      info_string = gender_label;

      // Set the gender to the new value.
      bio_gender = (int)vitals.cDonorSex;
   }

   if ((bio_height == 0) || (bio_height != vitals.fDonorHeight))
   {
      // Build the Height String
      guiTextUtils.BuildHeightString(vitals, guiString);

      displayText.set_string_id(textMiscCfrDonInfoHeight);
      displayText.get_text_direct(height_label);

      height_label += guiString;

      // Do we need a comma delimeter?
      if (info_string.length() != 0)
      {
         info_string += comma_delimiter;
         info_string += height_label;
      }
      else
      {
         info_string = height_label;
      }

      // Set the height to the new value.
      bio_height = vitals.fDonorHeight;
   }

   if ((bio_weight == 0) || (bio_weight != vitals.fDonorWeight))
   {
      // Build the Weight String
      guiTextUtils.BuildWeightString(vitals, guiString);

      displayText.set_string_id(textMiscCfrDonInfoWeight);
      displayText.get_text_direct(weight_label);
      weight_label += guiString;

      // Do we need a comma delimeter?
      if (info_string.length() != 0)
      {
         info_string += comma_delimiter;
         info_string += weight_label;
      }
      else
      {
         info_string = weight_label;
      }

      // Set the weight to the new value.
      bio_weight = vitals.fDonorWeight;
   }

   msg = info_string;
}



//
// BuildDonorVitalsBMsg
//
void BioSignaturesMgr::BuildDonorVitalsBMsg (const SDonorVitals& vitals, guistring& msg)
{
   GuiTextUtils textUtils;

   Display_Text displayText;

   guistring    blood_type_label;
   guistring    hematocrit_label;
   guistring    platelet_count_label;
   guistring    guiString;
   guistring    vitals_string;

   // Get the comma delimiter
   guistring comma_delimiter;
   displayText.set_string_id(textMiscCfrCommaDelim);
   displayText.get_text_direct(comma_delimiter);

   CDonor donor(vitals);


   //
   // Get the Blood Type String
   //

//   if ((bio_bloodtype == BT_UNKNOWN) || (bio_bloodtype != vitals.DonorBloodType))
   if (bio_bloodtype != vitals.DonorBloodType)
   {
      const TEXT_STRING_ID btStringId = textUtils.GetBloodType(vitals.DonorBloodType);

      if (btStringId == TEXT_NULL)
      {
         _FATAL_ERROR (__FILE__, __LINE__, "Unable to determine the blood type from donor data.");
      }

      //
      // Format the Blood Type Text
      //
      displayText.set_string_id(textDataCfrBloodType);
      displayText.get_text_direct(blood_type_label);

      // Do string lookup to get the text label
      displayText.set_string_id(btStringId);
      displayText.get_text_direct(guiString);

      blood_type_label += guiString;

      vitals_string     = blood_type_label;

      bio_bloodtype     = vitals.DonorBloodType;
   }

   //
   // Format the Hematocrit text
   //
   if (bio_hematocrit != vitals.fDonorHct)
   {
      const bool usingHgb = donor.RbcUnitsHgb();
      displayText.set_string_id(usingHgb ? textMiscCfrDonInfoHgb : textMiscCfrDonInfoCrit);
      displayText.get_text_direct(hematocrit_label);

      if (donor.RbcUnitsHgb())
         sprintf_l10n(guiString, "%.1f ", donor.Hgb());
      else
         sprintf(guiString, "%d", (int)donor.Hct());

      hematocrit_label += guiString;

      displayText.set_string_id(usingHgb ? textDataUnitsCfrHgb : textDataUnitsCrit);
      displayText.get_text_direct(guiString);
      hematocrit_label += guiString;

      // Do we need a comma delimeter?
      if (vitals_string.length() != 0)
      {
         vitals_string += comma_delimiter;
         vitals_string += hematocrit_label;
      }
      else
      {
         vitals_string = hematocrit_label;
      }

      // Set the bloodtype to the new value.
      bio_hematocrit = vitals.fDonorHct;
   }

   //
   // Format the Platelet Count text
   //
   if (bio_platelet != vitals.fDonorPltPrecount)
   {
      displayText.set_string_id(textMiscCfrDonInfoPltPrecount);
      displayText.get_text_direct(platelet_count_label);
      sprintf(guiString, "%d", (int)vitals.fDonorPltPrecount);
      platelet_count_label += guiString;

      displayText.set_string_id(textDataUnitsCfrPltPrecount);
      displayText.get_text_direct(guiString);
      platelet_count_label += guiString;

      // Do we need a comma delimeter?
      if (vitals_string.length() != 0)
      {
         vitals_string += comma_delimiter;
         vitals_string += platelet_count_label;
      }
      else
      {
         vitals_string = platelet_count_label;
      }

      // Set the weight to the new value
      bio_platelet = vitals.fDonorPltPrecount;
   }
   //
   // Copy the string.
   //
   msg = vitals_string;
}



//
// SetAuditLevel
//
void BioSignaturesMgr::SetAuditLevel ()
{
   const int auditLevel = guiglobs::cdsData.config.Procedure.Get().key_audit_tracking;

   DataLog(log_level_gui_cfr11_info) << "CFR 11 Tracking set to " << auditLevel << "." << endmsg;

   // Deregister for all tracking event messages
   _alarmResponseMsg.deregisterMsg();
   _procedureSelectedMsg.deregisterMsg();
   _adjustMsg.deregisterMsg();
   _donorVitalsChangeMsg.deregisterMsg();

   switch (auditLevel)
   {
      case TRACKING_OFF :
         break;

      case TRACKING_LEVEL_3 :
         // Setup for all alerts
         _alarmResponseMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleAlarmResponseMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      case TRACKING_LEVEL_2 :
         // Setup for all adjustments
         _adjustMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleAdjustmentMsg), MessageBase::SNDRCV_RECEIVE_ONLY);

      case TRACKING_LEVEL_1 :
         // Setup for all donor tracking changes
         _procedureSelectedMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleProcedureSelectedMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
         _donorVitalsChangeMsg.init(Callback<BioSignaturesMgr>(this, &BioSignaturesMgr::HandleDonorVitalsChangeMsg), MessageBase::SNDRCV_RECEIVE_ONLY);
         break;

      default :
         // Generate an alarm
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm("Bad Tracking Level choice");
         break;
   }
}



//
// PinValid
//
BioSignaturesMgr::PinValidType BioSignaturesMgr::PinValid (const char* badge, const char* pin)
{
   PinValidType status;

   DataLog(log_level_gui_cfr11_info) << "Validating badge[" << badge << "], pin[" << pin << "]." << endmsg;

   PIN_LIST_TYPE::iterator i = PinDat::find(badge);

   if (i == PinDat::end())
   {
      status = BIO_NO_MATCH;
   }
   else
   {
      const string storedPin = i->second;

      if (strcmp(storedPin.c_str(), pin) == 0)
      {
         status = BIO_PIN_VALID;
      }
      else
      {
         status = BIO_PIN_INVALID;
      }
   }

   // Update the log

   return status;
}



//
// AddPin
//
void BioSignaturesMgr::AddPin (const char* badge, const char* pin)
{
   // If the pin file is full, remove one to make room for one.
   if (PinDat::size() >= BIO_MAX_PIN_LIST)
   {
      // Erase one.
      PinDat::erase(PinDat::begin());
   }

   PIN_LIST_TYPE::iterator iter = PinDat::find(badge);

   // Badge not found, add it.
   if (iter == PinDat::end())
   {
      // Add the new pin
      PinDat::addPin(badge, pin);
   }
   else
   {
      // Change the existing one.
      (*iter).second = pin;
   }

   DataLog(log_level_gui_cfr11_info) << "badge " << badge << " added.  New pin list size is " << PinDat::size() << endmsg;

   // Now, update the pin file so we are in sync.
   PinDat::update();
}



//
// UpdateItem
//
void BioSignaturesMgr::UpdateItem (const char* badge, AuthorizationStatusType status, BiometricSignatureType deviceId, long key, const void* authData)
{
   CFR11_PFR_Storage_Type pfrItem;
   CFR11_Log_Type&        log_item = pfrItem.logItem;
   CFR11_Key_Type         pfrFileKey;


   // Retrieve the event record.
   const RECORD_NUMBER_TYPE recordNumber = _biometricPfrFile->read_direct(key, (REC_TYPE*)&pfrItem, (KEY_TYPE*)&pfrFileKey);

   if ((recordNumber == IO_ERR) || (recordNumber != key))
   {
      // Something bad happened.  Log Error.
      DataLog(log_level_gui_cfr11_error) << "bad key " << key << " in UpdateItem." << endmsg;

      // TBD.  Is this an alarm case?
      return;
   }

   log_item.status = status;
   strncpy(log_item.badge, badge, BARCODE_ID_SIZE);
   log_item.device = deviceId;

   DataLog(log_level_gui_cfr11_info) << "Auth Event Updated " << log_item << endmsg;

   switch (status)
   {
      case BIO_AUTH_SUCCESS :
         // Remove event from file.
         if (!_biometricPfrFile->delete_record(key))
         {
            // Log error.
            DataLog(log_level_gui_cfr11_error) << "Failed to delete biometric record " << key << ", item " << log_item << endmsg;
            return;
         }

         break;

      case BIO_AUTH_FAILED :
      case BIO_AUTH_UNKNOWN :
      {
         switch (log_item.device)
         {
            case BIO_DEVICE_PIN :
               RequestPinSignature(badge, (const char*)authData, key);
               break;

            default :
               DataLog(log_level_gui_cfr11_error) << "Failed to request Pin Signature for " << key << ", item " << log_item << endmsg;
               return;
         }
      }
      break;

      default :
         DataLog(log_level_gui_cfr11_error) << "Got unexpected enum " << status << ".  " << log_item << endmsg;
         break;
   }

   // Log binary entry (for Vista)
   datalog_WriteBinaryRecord(log_handle_vista_binary, VISTA_RecordType, VISTA_AuditTrailSubType, &log_item, sizeof(log_item));
}



//
// LogEvent
//
int BioSignaturesMgr::LogEvent (ComplianceTriggerType trigger, const TriggerDataType& data, const guistring& list_text)
{
   // make local copy of the string for modification
   guistring eventString = list_text;

   if (eventString.length() == 0)
   {
      DataLog (log_level_gui_cfr11_error) << "ERROR: list text is empty for trigger " << trigger  << endmsg;
      return 0;
   }

   CFR11_PFR_Storage_Type pfr_item;
   CFR11_Log_Type&        log_item = pfr_item.logItem;
   RECORD_NUMBER_TYPE     recordNumber;

   // Save the event in our list.
   log_item.timestamp = time(0);
   log_item.trigger   = trigger;
   log_item.data      = data;

   // Bump the sequence number
   _guiCds.cfr11SequenceNumber = _guiCds.cfr11SequenceNumber + 1;
   log_item.sequenceNumber     = _guiCds.cfr11SequenceNumber;

   add_time_of_day (eventString);

   // Copy over the gui string (for possible PFR restore)
   strncpy(pfr_item.logString, eventString, CFR11_LOG_STRING_SIZE);

#if (CPU!=SIMNT)

   // Write out the record to the pfr file
   recordNumber = _biometricPfrFile->write_direct ((REC_TYPE*)&pfr_item, (KEY_TYPE*)&log_item.sequenceNumber, true);

   // If we have a malformed write, bring it down
   if (recordNumber == IO_ERR)
   {
      DataLog_Critical critical;

      DataLog(critical) << "Write to log file failed on item " << log_item << endmsg;

      anAlarmMsg alarm(CFR11_LOG_FILE_ERROR);
      alarm.setAlarm("CFR11 Log Write Failed");

      return -1;
   }
   else
   {
      // Log ascii entry
      DataLog(log_level_gui_cfr11_info) << "Logged sn/rn " << log_item.sequenceNumber << "/" << recordNumber << " "
                                        << log_item << "." << endmsg;

      // Forward the event to the Vista interface
      Message<CFR11_Log_Type> msg(MessageBase::SEND_LOCAL);
      msg.send(log_item);
   }
#endif

   // Store the item in the cfr list for the CFR screen.
   CFR11_Tracking_List list_item;

   list_item.btn_text = eventString;
   list_item.key      = recordNumber;
   list_item.trigger  = trigger;

   guiglobs::cfr_list.push_back(list_item);

   DataLog(log_level_gui_cfr11_info) << "Added (RN " << list_item.key << ", " << list_item.btn_text << ") to GUI list." << endmsg;
   dumpCfrList(__FILE__, __LINE__);

   // Generate an alarm msg to proc to manage our compliance screen.
   anAlarmMsg alarm(CFR11_TRACKING_ALARM);
   alarm.setAlarm("CFR Part 11 Compliance Screen Alarm");

   return log_item.sequenceNumber;
}  // End of BioSignature::LOG_EVENT



//
// add_time_time_of_day
//
void BioSignaturesMgr::add_time_of_day (guistring& list_text)
{
   // static char time_of_day[20];
   // memset(time_of_day, 0, sizeof(time_of_day));

   guistring    time_of_day;

   time_t       current_time = time(NULL);

   char         hours[3];
   char         minutes[3];
   char         seconds[3];

   Display_Text text;

   guistring    resultString;

   guistring    units_time_delimiter;
   guistring    units_time_am;
   guistring    units_time_pm;
   guistring    units_time;
   guistring    time_event_separator;

   //
   //  Set text elements for current_time.
   //
   text.set_string_id(textDataUnitsCfrAm);
   text.get_text_direct(units_time_am);

   text.set_string_id(textDataUnitsCfrPm);
   text.get_text_direct(units_time_pm);

   text.set_string_id(textMiscCfrTimeDelim);
   text.get_text_direct(units_time_delimiter);

   text.set_string_id(textMiscCfrTimeEventSeparator);
   text.get_text_direct(time_event_separator);

   //
   //  Set the time elements for the current_time.
   //

   strftime(hours, 3, "%H", localtime(&current_time));

   if (_standard_time)
   {
      int hour = atoi(hours);
      strcat(units_time, ( hour < 12 ) ? units_time_am : units_time_pm);

      strftime(hours, 3, "%I", localtime(&current_time));
   }

   // replace default first zero char if present
   if (hours[0] == '0')
   {
      hours[0] = hours[1];
      hours[1] = '\0';
   }

   strftime(minutes, 3, "%M", localtime(&current_time));
   strftime(seconds, 3, "%S", localtime(&current_time));

   //
   // Put it all together.
   //
   strcat(time_of_day, hours);
   strcat(time_of_day, units_time_delimiter);
   strcat(time_of_day, minutes);
   strcat(time_of_day, units_time);
   strcat(time_of_day, time_event_separator);


   resultString  = time_of_day;
   resultString += list_text;

   // The final string.
   list_text = resultString;
}  //  End BioSignature::ADD_TIME_OF_DAY


//
//
//
void BioSignaturesMgr::RequestPinSignature (const char* badge, const char* pin, int sequenceNumber)
{
   Message<PinSignatureRequestMsg> requestMsg(MessageBase::SEND_LOCAL);
   PinSignatureRequestMsg          request;

   request.scanSequenceNumber = sequenceNumber;
   request.biometricDeviceId  = BIO_DEVICE_PIN;
   strncpy(request.badge, badge, BARCODE_ID_SIZE);
   strncpy(request.pin, pin, PIN_SIZE);

   DataLog(log_level_gui_cfr11_info) << "Requesting signature for (RN "
                                     << request.scanSequenceNumber << ", Dev " << request.biometricDeviceId << ", Badge "
                                     << request.badge << ", Pin " << request.pin << ")." << endmsg;

   requestMsg.send(request);
}


//
//
//
void BioSignaturesMgr::invoke_cfr11_screen ()
{
   // Generate an alarm msg to proc to manage our compliance screen.
   anAlarmMsg alarm(CFR11_TRACKING_ALARM);
   alarm.setAlarm("CFR Part 11 Compliance Screen Alarm invocation");

}

/* FORMAT HASH f5ae8f20f1e1c3afcd290dab279bad90 */
