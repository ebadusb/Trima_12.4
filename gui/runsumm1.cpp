/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_RUNSUMM1

*     File name:   runsumm1.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>

// SYSTEM INCLUDE
#include <time.h>

// BASE CLASS INCLUDE

#include "runsumm1.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"
#include "bio_mgr.h"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "runsumGlobs.hpp"
#include "xif_cds.h"
#include "units.h"
#include "trima_info.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_RUNSUMM1::Screen_RUNSUMM1()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_RUNSUMM1),

     _usingHemoglobinUnits(false),

// Main data boxes
     _bmpTxtBoxen  (183, 144, BITMAP_PLACARD_RUNSUMM1),

// Extra data boxes for configurable data
     _bmpExtraBoxLeftTop(183, 326, BITMAP_RUNSUMM1_EXTRA_BOX),
     _bmpExtraBoxLeftBottom(183, 369, BITMAP_RUNSUMM1_EXTRA_BOX),
     _bmpExtraBoxRightTop(504, 326, BITMAP_RUNSUMM1_EXTRA_BOX),
     _bmpExtraBoxRightBottom(504, 369, BITMAP_RUNSUMM1_EXTRA_BOX),

// Text
     _txtScreenLabel                   (textMiscLabelRs1Procedural),
     _txtDlogNumber                      (textMiscLabelRs1Dlog),

// Column 1 Data
     _dataTotalAcUsed(textDataLabelRs1AcUsed,
                      textEmptyFormatRs1DatTotAcUsed,
                      textDataUnitsRs1AcUsed),

     _dataEndOfRunTime(textDataLabelRs1EndOfRunTime,
                       textEmptyFormatRs1DatEndOfRunTime,
                       textDataUnitsRs1TimeAm,
                       textDataUnitsRs1TimePm),

     _dataPostPltCount(textDataLabelRs1PltPostCount,
                       textEmptyFormatRs1DatPostPltCount,
                       textDataUnitsRs1X1000),

     _dataTbvProcessed(textDataLabelRs1TbvProcessed,
                       textEmptyFormatRs1DatTbvProcessed,
                       textDataUnitsRs1VolMl),

     _dataAasStartTime(textDataLabelRs1AasStartTime,
                       textEmptyFormatRs1DatAasStartTime,
                       textDataUnitsRs1TimeAasAm,
                       textDataUnitsRs1TimeAasPm),

     _dataAvgInletFlow(textDataLabelRs1AvgInletFlow,
                       textEmptyFormatRs1DatAvgInletFlow,
                       textDataUnitsRs1AvgInletFlow),


// Column 2 Data
     _dataActualAcToDonor(textDataLabelRs1AcToDonor,
                          textEmptyFormatRs1DatAcToDonor,
                          textDataUnitsRs1AcToDonor),

     _dataLengthOfRun(textDataLabelRs1DurationOfRun,
                      textEmptyFormatRs1DatLengthOfRun,
                      textDataUnitsRs1MinLength),

     _dataPostCrit(textDataLabelRs1PostHct,
                   textEmptyFormatRs1DatPostHct,
                   textDataUnitsRs1Percent),

     _dataSalineUsed(textDataLabelRs1SalineUsed,
                     textEmptyFormatRs1DatSalineUsed,
                     textDataUnitsRs1SalineUsed),

     _dataPrbcResidualLoss(textDataLabelRs1PrbcResidualLoss,
                           textEmptyFormatRs1DatPrbcResidualLoss,
                           textDataUnitsRs1PrbcResidualLoss),

     _dataPlasmaResidualLoss(textDataLabelRs1PlasRedidualLoss,
                             textEmptyFormatRs1DatPlasResidualLoss,
                             textDataUnitsRs1PlasResidualLoss),

     _dataSalineRinsebackStatus(textDataLabelRs1SalineRinseback,
                                textDataDatRs1RinsebackIncomplete),

     _dataPlasmaRinsebackStatus(textDataLabelRs1PlasmaRinseback,
                                textDataDatRs1RinsebackIncomplete),

     _runComplete(false),
     _showAvgInletFlow(Software_CDS::GetInstance().getFeature(AllowAdjustFlowRatesOnProcedureSelect)),
     _showResidualVol(Software_CDS::GetInstance().getFeature(DisplayResidualVolumes))
{
   _dataEndOfRunTime.setDashesForZeros();
   _dataAvgInletFlow.setDashesForZeros();
}   // END of Screen_RUNSUMM1 CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_RUNSUMM1::~Screen_RUNSUMM1()
{
   // If we haven't already deallocated, do it now.
   Screen_RUNSUMM1::deallocate_resources ();
}   // END of Screen_RUNSUMM1 DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()
void Screen_RUNSUMM1::allocate_resources (const char* allocation_parameter)
{
   // _usingHemoglobinUnits = (guiglobs::gui_donor_data_repository.RbcUnitsHgb());
   _usingHemoglobinUnits = (guiglobs::cdsData.config.LangUnit.Get().key_crit_or_glob);

   // refresh cached features
   _showAvgInletFlow = Software_CDS::GetInstance().getFeature(AllowAdjustFlowRatesOnProcedureSelect);
   _showResidualVol  = Software_CDS::GetInstance().getFeature(DisplayResidualVolumes);

   // Activate the receive message object to receive the runsumm1 data
   _msgRunSummaryStats.init(Callback<Screen_RUNSUMM1>(this, &Screen_RUNSUMM1::processRunsumData), MessageBase::SNDRCV_RECEIVE_ONLY);

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   if (_usingHemoglobinUnits)
   {
#if (CPU==SIMNT)
      _dataPostCrit.updateLabelStringId(textDataLabelRs1PostHct);
#endif
      _dataPostCrit.updateStringIds(textDataLabelRs1PostHgb,
                                    textEmptyFormatRs1DatPostHct,
                                    textDataUnitsRs1GPerDl);
   }
   else
   {
#if (CPU==SIMNT)
      _dataPostCrit.updateLabelStringId(textDataLabelRs1PostHct);
#endif
      _dataPostCrit.updateStringIds(textDataLabelRs1PostHct,
                                    textEmptyFormatRs1DatPostHct,
                                    textDataUnitsRs1Percent);
   }

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front. Only get the parent widget
   //  once to reduce overhead.

   // the runsumm1 data panel
   _bmpTxtBoxen.allocate_resources (*this);
   _txtScreenLabel.allocate_resources(*this);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_cancel.deallocate_resources ();

   // deallocate continue/goback until message received
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Send a message to request the runsumm1 data
   DataLog (log_level_gui_info) << "Sending runsum stats request message." << endmsg;

   RunsumGlobs::requestRunsumStats();

   // Check if any barcode activity has taken place. If so, raise the barcode screen for review.
   if ( (guiglobs::barcode_repo->total_scans() > 0) && (guiglobs::cdsData.xif.vistaConnected.Get() || guiglobs::cdsData.xif.vistaHasConnected.Get() ))
   {
      guiglobs::barcode_repo->invoke_barcode_screen ();
   }

   //  Check to see if anything in cfr11 list, if so, raise cfr11 screen to review list.
   if ((guiglobs::screen_status.get_current_system_state() == POST_RUN) &&
       (!guiglobs::cfr_list.empty()))
   {
      guiglobs::bioMgr().invoke_cfr11_screen();
   }

   // Initialize state flag (we'll set it when we get the message from proc.
   _runComplete = false;
}  // END of Screen_RUNSUMM1 ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_RUNSUMM1::deallocate_resources ()
{
   _blinkCollection.stopBlink();

   // Deallocate bitmaps
   _bmpTxtBoxen.deallocate_resources();
   _bmpExtraBoxLeftTop.deallocate_resources();
   _bmpExtraBoxLeftBottom.deallocate_resources();
   _bmpExtraBoxRightTop.deallocate_resources();
   _bmpExtraBoxRightBottom.deallocate_resources();

   // Deallocate data items
   _dataTotalAcUsed.deallocate_resources();
   _dataEndOfRunTime.deallocate_resources();
   _dataLengthOfRun.deallocate_resources();
   _dataPostPltCount.deallocate_resources();
   _dataActualAcToDonor.deallocate_resources();
   _dataPostCrit.deallocate_resources();
   _dataTbvProcessed.deallocate_resources();
   _dataSalineUsed.deallocate_resources();
   _dataAvgInletFlow.deallocate_resources();
   _dataAasStartTime.deallocate_resources();
   _dataPrbcResidualLoss.deallocate_resources();
   _dataPlasmaResidualLoss.deallocate_resources();

   _dataSalineRinsebackStatus.deallocate_resources();
   _dataPlasmaRinsebackStatus.deallocate_resources();

   // Deallocate text items
   _txtDlogNumber.deallocate_resources();
   _txtScreenLabel.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Deallocate the Runsumm1 receive message object
   _msgRunSummaryStats.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

}   // END of Screen_RUNSUMM1 DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_RUNSUMM1::process_continue_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();
   if (checkForRunCompleteAnomoly()) return;

   const RUN_SUMMARY_STRUCT& runSummaryData = _msgRunSummaryStats.getData();
   RunsumGlobs::gotoNextRunSummaryScreen(runSummaryData);
}   // END of Screen_RUNSUMM1 PROCESS_CONTINUE_BUTTON

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON
void Screen_RUNSUMM1::process_goback_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   switch (guiglobs::screen_status.get_current_system_state())
   {
      case BLOOD_RINSEBACK : // If we're in rinseback, go back to runproc screen.
         goto_screen (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
         break;

      case METERED_STORAGE : // If we're in metered storage, go back to mssBegin.
         goto_screen (GUI_SCREEN_AAS_ADDITION, __RUNSUM_TO_MSS_ADDITION_ALLOC_PARAM__);
         break;

      default :
         DataLog (log_level_gui_error) << "Runsum shouldn't have goback option if we're not "
                                       << "in either rinseback or metered storage state." << endmsg;
         break;
   }
}   // END of Screen_RUNSUMM2 PROCESS_GOBACK_BUTTON



////////////////////////////////////////////////////////////////////////////////
// PROCESS_RUNSUMM1_DATA
void Screen_RUNSUMM1::processRunsumData ()
{
   guiglobs::dont_connect_icon.deallocate_resources();

   const RUN_SUMMARY_STRUCT& runSummaryData = _msgRunSummaryStats.getData();

   _dataTotalAcUsed.setData(runSummaryData.rs1_total_AC_used);

   if (guiglobs::screen_status.get_current_system_state() == BLOOD_RINSEBACK)
   {
      struct tm temp_time = {0, 0, 0, 0, 0, 0, 0, 0};
      temp_time.tm_hour = -1;
      _dataEndOfRunTime.setData(temp_time);
   }
   else
   {
      _dataEndOfRunTime.setData(runSummaryData.rs1_end_of_run_time);
   }

   _dataLengthOfRun.setData(runSummaryData.rs1_length_of_run);
   _dataPostPltCount.setData(runSummaryData.rs1_post_platelet_count);
   _dataActualAcToDonor.setData(runSummaryData.rs1_actual_AC_to_donor);
   _dataTbvProcessed.setData(runSummaryData.rs1_total_blood_vol_processed);
   _dataSalineUsed.setData(runSummaryData.replacement_fluid_volume);
   _dataAasStartTime.setData(runSummaryData.rs1_start_of_aas_time);
   _dataPrbcResidualLoss.setData(runSummaryData.rs1_residual_loss_prbc);
   _dataPlasmaResidualLoss.setData(runSummaryData.rs1_residual_loss_plasma);
   _dataPlasmaRinsebackStatus.setData(runSummaryData.plasma_rinseback_status);
   _dataSalineRinsebackStatus.setData(runSummaryData.saline_rinseback_status);

   if (_usingHemoglobinUnits)
      _dataPostCrit.setData( (runSummaryData.rs1_post_hct / HCT_TO_HGB), 1);
   else
      _dataPostCrit.setData((int)runSummaryData.rs1_post_hct);

   _dataAvgInletFlow.setData(runSummaryData.rs1_last_adjust_flow_rate);

   bool setIsWhiteStamp      = guiglobs::cassette_mgr->currentSetIsWhiteStamp();

   bool currentlyInRinseback = guiglobs::screen_status.get_current_system_state() == BLOOD_RINSEBACK;

   // Plasma rinseback is white-stamp only
   bool plasRinsebackEnabled = setIsWhiteStamp && guiglobs::cdsData.config.Procedure.Get().key_plasma_rinseback
                               && (!currentlyInRinseback); // We don't want to see this yet if we're not in rinseback.

   // Saline rinseback is black-stamp only
   bool salineRinsebackEnabled = (!setIsWhiteStamp) && guiglobs::cdsData.config.Procedure.Get().key_saline_rinseback
                                 && (!currentlyInRinseback); // We don't want to see this yet if we're not in rinseback.

   DataLog (log_level_gui_info) << "Runsumm1 scrn data: ("
                                << "setIsWhiteStamp=" << setIsWhiteStamp
                                << "; currentlyInRinseback=" << currentlyInRinseback
                                << "; plasRinsebackEnabled=" << plasRinsebackEnabled
                                << "; salineRinsebackEnabled=" << salineRinsebackEnabled
                                << ")" << endmsg;

   selectScreenLayout(runSummaryData.meteringStarted,
                      _showAvgInletFlow,
                      _showResidualVol,
                      plasRinsebackEnabled,
                      salineRinsebackEnabled);

   // Todo: make this conditional on rinseback state later on.
   initBlinkCollection();

   // Handle screen modifications needed on first receipt of the run summary data
   if (!btn_continue.allocated())
   {
      // allocate "next page" button
      btn_continue.allocate_resources(*this);
      btn_continue.set_callback  (aph_callback_continue, (void*)this);
      btn_continue.set_string_id (textButtonLongNextPage);

      if (guiglobs::screen_status.get_current_system_state() == POST_RUN)
      {
         setStatLineAndBar();
         _runComplete = true;
      }
   }

   if (((!runSummaryData.run_complete) ||
        (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE)) &&
       (!btn_goback.allocated())
       )
   {
      btn_goback.allocate_resources(*this);
      btn_goback.set_callback (aph_callback_goback, (void*)this);
   }

#if (CPU!=SIMNT) // We don't really want to see this on screenshots.
   ///////////////////////////////////////////////////////////////////////////////////////
   // If this is a clinical build, then show the datalog number.
   ///////////////////////////////////////////////////////////////////////////////////////
   const bool isClinicalOrTestSoftware = TrimaInfo::isClinicalBuild() || TrimaInfo::isTestBuild();

   if (isClinicalOrTestSoftware)
   {
      const int sequenceNum = atoi(TrimaInfo::sequenceNumber());

      if (sequenceNum >= 0)
      {
         guistring tempString;
         tempString.initialize();

         _txtDlogNumber.set_string_id(textMiscLabelRs1Dlog);
         _txtDlogNumber.get_text_direct(tempString);
         sprintf (tempString, "%s%d", tempString.getBufPtr(), sequenceNum);
         _txtDlogNumber.set_text_direct(tempString.length(), tempString);
         _txtDlogNumber.allocate_resources(*this);
      }
      else
         DataLog (log_level_gui_error) << "Unable to determine dlog sequence number ("
                                       << sequenceNum << ")." << endmsg;
   }
#endif // #if (CPU!=SIMNT)

   // Debug only
   // outputData();

   RunsumGlobs::completeRunsumStatsProcessing();
}  // END of Screen_RUNSUMM1 PROCESS_RUNSUMM1_DATA



//
//
//
void Screen_RUNSUMM1::refresh_resources ()
{
   // Call base class.  In this case, make sure you reparent the status bar.
   Base_Apheresis_Window::refresh_resources();
   if (checkForRunCompleteAnomoly()) return;

   if (_runComplete)
   {
      setStatLineAndBar();
   }
}



//
//
//
void Screen_RUNSUMM1::setStatLineAndBar ()
{
   // Set the status bar to reflect end of run
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_INFO);
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_LOAD);
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_PREPARE); // RUNPROC in 2.2 doesn't set this.
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_DONATE);
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_SOLUTIONS);
   guiglobs::apheresis_status_bar->set_phase_active(STATUS_BAR_UNLOAD);

   // Set the status line to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (MAGENTA);

   // Remove any outstanding attention messages remaining from
   //  an "abort run" screen change.
   guiglobs::apheresis_status_line->deactivate_status_line_type(ATTENTION_STATUS_LINE);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textStatEndOfRun);

}

//
// Check for Run Complete in Rinseback Timing Anomoly
// -----------------------------
// IT 7802 documents a situation where the operator was able to transition
// to this screen just as the run was completing, so the screen allocates
// as if we're still performing rinseback, but when the procstatus object
// fired its "run ended" callback, we weren't on this screen yet, so it didn't
// transition back like it normally would.
// The result is that when continue is touched, the system freezes up when
// attempting to transition to runsum2, because that transition is not valid
// once the run is complete.  So this function checks for that.
//
bool Screen_RUNSUMM1::checkForRunCompleteAnomoly ()
{
   if (guiglobs::proc_status.get_data().run_complete
       && guiglobs::proc_status.get_data().system_state == BLOOD_RINSEBACK)
   {
      DataLog(log_level_gui_info) << "Detected an IT7802-type situation.  Transitioning back to procedure screen." << endmsg;
      goto_screen (GUI_SCREEN_RUNPROC, "GUI_SCREEN_RUNPROC");
      return true;
   }
   return false;
}

void Screen_RUNSUMM1::selectScreenLayout (bool showAasStartTime, bool showAvgInletFlow, bool showResidualLoss, bool plasRinsebackOn, bool salineRinsebackOn)
{
   DataLog (log_level_gui_info) << "RS1 screen layout: (showAasStartTime="
                                << showAasStartTime << " showAvgInletFlow=" << showAvgInletFlow
                                << " showResidualLoss=" << showResidualLoss << " plasRinsebackOn="
                                << plasRinsebackOn << " salineRinsebackOn=" << salineRinsebackOn
                                << ")" << endmsg;

   if (showResidualLoss)
   {
      // Deallocate rinseback status messages
      _dataSalineRinsebackStatus.deallocate_resources();
      _dataPlasmaRinsebackStatus.deallocate_resources();

      // Show extra residual loss boxes
      _bmpExtraBoxRightTop.allocate_resources(*this);
      _bmpExtraBoxRightBottom.allocate_resources(*this);

      // Show residual loss data
      _dataPrbcResidualLoss.allocate_resources(*this);
      _dataPlasmaResidualLoss.allocate_resources(*this);
   }
   else
   {
      _bmpExtraBoxRightBottom.deallocate_resources();
      _dataPrbcResidualLoss.deallocate_resources();
      _dataPlasmaResidualLoss.deallocate_resources();

      if (plasRinsebackOn || salineRinsebackOn)
      {
         _bmpExtraBoxRightTop.allocate_resources(*this);

         if (plasRinsebackOn)
         {
            _dataSalineRinsebackStatus.deallocate_resources();
            _dataPlasmaRinsebackStatus.allocate_resources(*this);
         }

         if (salineRinsebackOn)
         {
            _dataPlasmaRinsebackStatus.deallocate_resources();
            _dataSalineRinsebackStatus.allocate_resources(*this);
         }
      }
      else
      {
         _bmpExtraBoxRightTop.deallocate_resources();
         _dataPlasmaRinsebackStatus.deallocate_resources();
         _dataSalineRinsebackStatus.deallocate_resources();
      }
   }

   if (showAasStartTime)
   {
      // Show top left extra box for AAS start time
      _bmpExtraBoxLeftTop.allocate_resources(*this);

      // Show AAS start time data
      _dataAasStartTime.allocate_resources(*this);
   }
   else
   {
      _bmpExtraBoxLeftTop.deallocate_resources();
      _dataAasStartTime.deallocate_resources();
   }

   if (showAvgInletFlow)
   {
      // Y-coordinate depends on whether or not we're
      // also showing AAS start time.
      if (showAasStartTime)
      {
         _bmpExtraBoxLeftBottom.allocate_resources(*this);

#if (CPU==SIMNT)
         _dataAvgInletFlow.updateLabelStringId(textDataLabelRs1AvgInletFlow);
#endif
         // Make sure text is aligned correctly
         _dataAvgInletFlow.updateLocations(textDataLabelRs1AvgInletFlow,
                                           textEmptyFormatRs1DatAvgInletFlow,
                                           textDataUnitsRs1AvgInletFlow);
      }
      else
      {
         _bmpExtraBoxLeftTop.allocate_resources(*this);
         _bmpExtraBoxLeftBottom.deallocate_resources();
#if (CPU==SIMNT)
         _dataAvgInletFlow.updateLabelStringId(textDataLabelRs1AvgInletFlowAlt);
#endif
         // Make sure text is aligned correctly
         _dataAvgInletFlow.updateLocations(textDataLabelRs1AvgInletFlowAlt,
                                           textEmptyFormatRs1DatAvgInletFlowAlt,
                                           textDataUnitsRs1AvgInletFlowAlt);
      }

      _dataAvgInletFlow.allocate_resources(*this);
   }
   else
   {
      _bmpExtraBoxLeftBottom.deallocate_resources();
      if (!showAasStartTime) _bmpExtraBoxLeftTop.deallocate_resources();
      _dataAvgInletFlow.deallocate_resources();
   }

#if (CPU==SIMNT)
   if (_usingHemoglobinUnits)
   {
      _dataPostCrit.updateLabelStringId(textDataLabelRs1PostHct);
      _dataPostCrit.updateStringIds(textDataLabelRs1PostHgb, textEmptyFormatRs1DatPostHct, textDataUnitsRs1GPerDl);
   }
   else
   {
      _dataPostCrit.updateLabelStringId(textDataLabelRs1PostHct);
      _dataPostCrit.updateStringIds(textDataLabelRs1PostHct, textEmptyFormatRs1DatPostHct, textDataUnitsRs1Percent);
   }
#endif
   // Allocate the rest of the text
   _dataTotalAcUsed.allocate_resources(*this);
   _dataEndOfRunTime.allocate_resources(*this);
   _dataLengthOfRun.allocate_resources(*this);
   _dataPostPltCount.allocate_resources(*this);
   _dataActualAcToDonor.allocate_resources(*this);
   _dataTbvProcessed.allocate_resources(*this);
   _dataSalineUsed.allocate_resources(*this);
   _dataPostCrit.allocate_resources(*this);
}


void Screen_RUNSUMM1::initBlinkCollection ()
{
   State_names currentState     = guiglobs::screen_status.get_current_system_state();

   bool        inRinseback      = currentState == BLOOD_RINSEBACK;
   bool        inMeteredStorage = currentState == METERED_STORAGE;

   bool        addedStuff       = false;

   if (inRinseback)
   {
      _blinkCollection.addObject(&_dataEndOfRunTime);
      _blinkCollection.addObject(&_dataActualAcToDonor);
      _blinkCollection.addObject(&_dataPostCrit);

      if (_showResidualVol)
      {
         _blinkCollection.addObject(&_dataPrbcResidualLoss);
         _blinkCollection.addObject(&_dataPlasmaResidualLoss);
      }

      if (guiglobs::isSalineRinseback())
      {
         _blinkCollection.addObject(&_dataSalineUsed);
      }

      addedStuff = true;
   }

   if (inMeteredStorage && _showResidualVol)
   {
      _blinkCollection.addObject(&_dataPrbcResidualLoss);
      _blinkCollection.addObject(&_dataPlasmaResidualLoss);

      addedStuff = true;
   }

   if (addedStuff) _blinkCollection.startBlink();
}

void Screen_RUNSUMM1::outputData ()
{
   DataLog (log_level_gui_info) << "Run Summary 1 (below):" << endmsg;
   DataLog (log_level_gui_info) << _dataTotalAcUsed;
   DataLog (log_level_gui_info) << _dataEndOfRunTime;
   DataLog (log_level_gui_info) << _dataPostPltCount;
   DataLog (log_level_gui_info) << _dataTbvProcessed;
   DataLog (log_level_gui_info) << _dataAasStartTime;
   DataLog (log_level_gui_info) << _dataAvgInletFlow;
   DataLog (log_level_gui_info) << _dataActualAcToDonor;
   DataLog (log_level_gui_info) << _dataLengthOfRun;
   DataLog (log_level_gui_info) << _dataPostCrit;
   DataLog (log_level_gui_info) << _dataSalineUsed;
   DataLog (log_level_gui_info) << _dataSalineRinsebackStatus;
   DataLog (log_level_gui_info) << _dataPlasmaRinsebackStatus;
   DataLog (log_level_gui_info) << _dataPrbcResidualLoss;
   DataLog (log_level_gui_info) << _dataPlasmaResidualLoss;
}


#if (CPU==SIMNT)
void Screen_RUNSUMM1::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      RS1_DISPLAY_TYPE rs1DisplayType   = (RS1_DISPLAY_TYPE)traverseInfo.parameters[0];
      bool
                       showAasStartTime = false,
         showAvgInletFlow               = false,
         showResidualLoss               = false,
         plasmaRinsebackOn              = false,
         salineRinsebackOn              = false;

      DataLog (log_level_sim_gui_side_info) << "Recieved SIMNT screen change msg with change value " << rs1DisplayType << "." << endmsg;

      switch (rs1DisplayType)
      {
         case RS1_DISPLAY_TYPE_NO_FRILLS : break;
         case RS1_DISPLAY_TYPE_RESIDUALS : showResidualLoss         = true; break;
         case RS1_DISPLAY_TYPE_PLASMA_RINSEBACK : plasmaRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_SALINE_RINSEBACK : salineRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME : showAasStartTime          = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_PLASMA_RINSEBACK : showAasStartTime           = true; plasmaRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_SALINE_RINSEBACK : showAasStartTime           = true; salineRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AVG_INLET : showAvgInletFlow                               = true; break;
         case RS1_DISPLAY_TYPE_AVG_INLET_AND_PLASMA_RINSEBACK : showAvgInletFlow          = true; plasmaRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AVG_INLET_AND_SALINE_RINSEBACK : showAvgInletFlow          = true; salineRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_RESIDUALS_AND_AAS_TIME : showResidualLoss                  = true; showAasStartTime = true; break;
         case RS1_DISPLAY_TYPE_RESIDUALS_AND_INLET : showResidualLoss                     = true; showAvgInletFlow = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_INLET : showAasStartTime                      = true; showAvgInletFlow = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_INLET_AND_PLASMA_RINSEBACK : showAasStartTime = true; showAvgInletFlow = true; plasmaRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_INLET_AND_SALINE_RINSEBACK : showAasStartTime = true; showAvgInletFlow = true; salineRinsebackOn = true; break;
         case RS1_DISPLAY_TYPE_AAS_TIME_AND_INLET_AND_RESIDUALS : showResidualLoss        = true; showAasStartTime = true; showAvgInletFlow = true; break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown RS1 Display Type: " << rs1DisplayType << endmsg;
            return;
      }
      _usingHemoglobinUnits = !_usingHemoglobinUnits;

      selectScreenLayout(showAasStartTime, showAvgInletFlow, showResidualLoss, plasmaRinsebackOn, salineRinsebackOn);
   }
   else if (traverseInfo.type == RUNSUMM1_SETUP)
   {
      selectScreenLayout((bool)traverseInfo.parameters[0],
                         (bool)traverseInfo.parameters[1],
                         (bool)traverseInfo.parameters[2],
                         (bool)traverseInfo.parameters[3],
                         (bool)traverseInfo.parameters[4]);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)


//////////////////////////////////////
// ResidualStatusDataField member functions
//////////////////////////////////////
ResidualStatusDataField::ResidualStatusDataField(TEXT_STRING_ID labelTxtId,
                                                 TEXT_STRING_ID dataTxtId)
   : TextDataField(labelTxtId, dataTxtId)
{}

void ResidualStatusDataField::setData (SPECIAL_RINSEBACK_STATUS status)
{
   TEXT_STRING_ID txtId = textDataDatRs1RinsebackIncomplete;

   switch (status)
   {
      case SPECIAL_RINSEBACK_STATUS_INCOMPLETE : break;
      case SPECIAL_RINSEBACK_STATUS_COMPLETE : txtId = textDataDatRs1RinsebackComplete; break;
      default :
         DataLog (log_level_gui_error) << "Error: unknown rinseback data ("
                                       << status << ")" << endmsg;
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();
   }


   _txtData.set_string_id(txtId);
}
//////////////////////////////////////
// End ResidualStatusDataField member functions
//////////////////////////////////////

/* FORMAT HASH 8f3ded27ed5dda858a44b11d02d77fe4 */
