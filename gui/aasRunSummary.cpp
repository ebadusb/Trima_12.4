/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Class name:  Screen_AAS_RUN_SUMMARY

*     File name:   aasRunSummary.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>


// BASE CLASS INCLUDE
#include "aasRunSummary.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "runsumGlobs.hpp"
#include "CatalogChecker.h"

#if (CPU==SIMNT) // Include unit testing definitions if compiled for simulator.
#include "guiUnitTestMessageDefs.h"
#endif

static const int INITIAL_Y_COORD = 305;
static const int BMP_X_COORD     = 480;
static const int Y_DELTA         = 33;

OfflineMeteringItem::OfflineMeteringItem(const string& logName,
                                         TEXT_STRING_ID lblId,
                                         TEXT_STRING_ID dataId,
                                         TEXT_STRING_ID unitId)
   : _logName(logName),
     _haveSetOriginalRegions(false),
     _txtLbl(lblId),
     _txtData(dataId),
     _txtUnit(unitId),
     _dataBox(BMP_X_COORD, INITIAL_Y_COORD, BITMAP_AAS_SUMM_DATA_BOX)
{}

OfflineMeteringItem::~OfflineMeteringItem()
{
   deallocate_resources();
}

void OfflineMeteringItem::allocate_resources (OSWindow& parent, int data)
{
   char salvVolString[10];
   int  salvVolStringSize = 0;

   salvVolStringSize = sprintf(salvVolString, "%d", data);
   _txtData.set_text_direct(salvVolStringSize, salvVolString);

   OSRegion lblRegion;
   OSRegion dataRegion;
   OSRegion unitRegion;

   _txtLbl.allocate_resources(parent);
   _txtLbl.get_requested_region(lblRegion);
   _txtData.allocate_resources(parent);
   _txtData.get_requested_region(dataRegion);
   _txtUnit.allocate_resources(parent);
   _txtUnit.get_requested_region(unitRegion);

   _dataBox.allocate_resources(parent);

   if (!_haveSetOriginalRegions)
   {
      _origLblRegion          = lblRegion;
      _origDataRegion         = dataRegion;
      _origUnitRegion         = unitRegion;

      _haveSetOriginalRegions = true;
   }
}

void OfflineMeteringItem::deallocate_resources ()
{
   _txtLbl.deallocate_resources();
   _txtData.deallocate_resources();
   _txtUnit.deallocate_resources();
   _dataBox.deallocate_resources();
}

void OfflineMeteringItem::bumpYcoord (int yCoordDelta)
{
   // Don't bother if we got a zero here.
   if (yCoordDelta == 0)
   {
      DataLog (log_level_gui_info) << "No need to bump y-coord for " << _logName << " zero pixels." << endmsg;
      return;
   }

   OSRegion region;

   _txtLbl.get_requested_region(region);
   region.y += yCoordDelta;
   _txtLbl.set_requested_region(region);

   _txtData.get_requested_region(region);
   region.y += yCoordDelta;
   _txtData.set_requested_region(region);

   _txtUnit.get_requested_region(region);
   region.y += yCoordDelta;
   _txtUnit.set_requested_region(region);

   region    = _dataBox.get_dimensions();
   region.y += yCoordDelta;
   _dataBox.set_position(BMP_X_COORD, region.y);
}

void OfflineMeteringItem::init ()
{
   if (_haveSetOriginalRegions)
   {
      _txtLbl.set_requested_region(_origLblRegion);
      _txtData.set_requested_region(_origDataRegion);
      _txtUnit.set_requested_region(_origUnitRegion);
      _dataBox.set_position(BMP_X_COORD, INITIAL_Y_COORD);
   }
}

//  BITMAP_RUNSUMM1_SINGLE_BOX
////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_AAS_RUN_SUMMARY::Screen_AAS_RUN_SUMMARY()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_RUN_SUMMARY),

     _pasMeteringItem("PAS salvage volume",
                      textDataLabelPasOffline,
                      textEmptyFormatOfflinePasData,
                      textDataUnitsPasOffline),

     _rasMeteringItem1("RAS salvage volume (product 1)",
                       textDataLabelRas1Offline,
                       textEmptyFormatOfflineRas1Data,
                       textDataUnitsRas1Offline),

     _rasMeteringItem2("RAS salvage volume (product 2)",
                       textDataLabelRas2Offline,
                       textEmptyFormatOfflineRas2Data,
                       textDataUnitsRas2Offline),

     _txtDataLbl(textDataLabelAasRunsumm),
     _txtMainInstructions(textMiscOfflineInstructionsNoSealInstructions),

     _bmpTextAreaTop(230, 120, BITMAP_AAS_SUMM_TEXT_AREA),
     _bmpTextAreaBottom(230, 273, BITMAP_AAS_SUMM_TEXT_AREA),
     _bmpInstructionalGraphic(20, 120, BITMAP_AAS_SUMM_SEAL_DRBC_ABOVE_FILTER),
     _pasSalvageVol(0),
     _rasSalvageVol1(0),
     _rasSalvageVol2(0),
     _haveBeenRas(false),
     _pass(0),
     _passes(1)
{}   // END of Screen_AAS_RUN_SUMMARY CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_AAS_RUN_SUMMARY::~Screen_AAS_RUN_SUMMARY()
{
   // If we haven't already deallocated, do it now.
   if (_allocated)
   {
      Screen_AAS_RUN_SUMMARY::deallocate_resources();
   }
}  // END of Screen_AAS_RUN_SUMMARY DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_AAS_RUN_SUMMARY::allocate_resources (const char* allocation_parameter)
{
   // Note: This is now the default screen for the POST_RUN system state.
   // So, before we do anything here, make sure we're supposed to be on this screen.
   // To do this, we should request a run summary from proc, and then not do anything
   // until it is recieved.
   _msgRunStats.init(Callback<Screen_AAS_RUN_SUMMARY>(this, &Screen_AAS_RUN_SUMMARY::processRunsummData), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Send a message to request the runsumm1 data
   DataLog (log_level_gui_info) << "Sending runsum stats request message." << endmsg;
   RunsumGlobs::requestRunsumStats();
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_AAS_RUN_SUMMARY::deallocate_resources ()
{
   _msgRunStats.deregisterMsg();

   _pasMeteringItem.deallocate_resources();
   _rasMeteringItem1.deallocate_resources();
   _rasMeteringItem2.deallocate_resources();

   _txtMainInstructions.deallocate_resources();
   _txtDataLbl.deallocate_resources();

   _bmpTextAreaTop.deallocate_resources();
   _bmpTextAreaBottom.deallocate_resources();
   _bmpInstructionalGraphic.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources();
}   // END of Screen_AAS_RUN_SUMMARY DEALLOCATE_RESOURCES

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_AAS_RUN_SUMMARY::process_continue_button ()
{
   if (_passes == 2 && _pass == 0)
   {
      _pass = 1;

      const bool setIsDrbc = guiglobs::cassette_mgr->currentSetIsDrbc();
      const bool setIsPtf  = guiglobs::cassette_mgr->currentSetIsRas();

      selectGraphicAndInstruction (setIsPtf, setIsDrbc);
   }
   else
   {
      goto_screen(GUI_SCREEN_MSS_END, "GUI_SCREEN_MSS_END");
   }
}   // End of PROCESS_CONTINUE_BUTTON

void Screen_AAS_RUN_SUMMARY::processRunsummData ()
{
   const RUN_SUMMARY_STRUCT& runSummary = _msgRunStats.getData();

   bool haveSalvageVolToShow            = RunsumGlobs::shouldShowAasSalvage(runSummary);
   bool setIsPtf = guiglobs::cassette_mgr->currentSetIsRas();
   bool collectedRbcs                   = runSummary.rs2_RBC_volume > 0 || runSummary.rs2_RBC_volume_2 > 0;
   bool mustGiveSealInstructions        = setIsPtf && collectedRbcs;
   bool isPostRun = (guiglobs::screen_status.get_current_system_state() == POST_RUN);

   DataLog (log_level_gui_info) << "AAS Run Summary recieved struct: ("
                                << "salvVol=" << haveSalvageVolToShow << "; "
                                << "setIsPtf=" << setIsPtf << "; "
                                << "collectedRbcs=" << collectedRbcs << "; "
                                << "sealInstruct=" << mustGiveSealInstructions << "; "
                                << "isPostRun=" << isPostRun << ")" << endmsg;

   // If we're in post run and we don't have any salvage volume to show, then proceed
   // to the first run summary screen.
   if (isPostRun)
   {
      goto_screen(GUI_SCREEN_RUNSUMM1, "GUI_SCREEN_RUNSUMM1");
      DataLog (log_level_gui_info) << "No data to show on post run aas runsummary screen. Transition to Run Summary 1" << endmsg;
      return;
   }

   // If we are NOT in post run (i.e. we're still in MSS Disconnect), and we have neither salvage volume
   // nor seal instructions to show, then proceed to the MSS_END screen.
   if (!haveSalvageVolToShow && !mustGiveSealInstructions)
   {
      goto_screen(GUI_SCREEN_MSS_END, "GUI_SCREEN_MSS_END");
      DataLog (log_level_gui_info) << "No data to show on disconnect aas runsummary screen. Transition to Mss End" << endmsg;
      return;
   }
   _pasSalvageVol  = RunsumGlobs::getPasSalvageVol(runSummary);
   _rasSalvageVol1 = RunsumGlobs::getRasSalvageVolProd1(runSummary);
   _rasSalvageVol2 = RunsumGlobs::getRasSalvageVolProd2(runSummary);
   _haveBeenRas    = (_rasSalvageVol1 > 0 || _rasSalvageVol2 > 0 || guiglobs::aasRunInfo.isRasProcedure());

   const bool setIsDrbc           = guiglobs::cassette_mgr->currentSetIsDrbc();
   const bool targetedAsAutoSplit = guiglobs::aasRunInfo.productTargetedAsDrbcPtf();


   DataLog (log_level_gui_info)
      <<  "setIsPtf="  << setIsPtf
      <<  " setIsDrbc="  << setIsDrbc
      <<  " rasSalvageVolProd1="  << _rasSalvageVol1
      <<  " rasSalvageVolProd2="  << _rasSalvageVol2
      <<  " pasSalvageVol="  << _pasSalvageVol
      <<  " haveBeenRas="  << _haveBeenRas
      << endmsg;

   if ((_pasSalvageVol > 0) && _haveBeenRas)
   {
      _passes = 2;
   }

   if ((_passes == 1) && !_pasSalvageVol && !_haveBeenRas) // Successful PAS only run
   {
      goto_screen(GUI_SCREEN_MSS_END, "GUI_SCREEN_MSS_END");
      DataLog (log_level_gui_info) << "No data to show on disconnect aas runsummary screen. Transition to Mss End" << endmsg;
      return;
   }

   // We passed the tests.  Time to start allocating resources.
   DataLog (log_level_gui_info) << "Show aas runsummary screen." << endmsg;

   // Now that we're sure this is where we're supposed to be, go ahead
   // and call all of the stuff that's normally called during allocate_resources().

   // allocate parent's resources first
   // Because this is called based on a message that GUI requests, there is the potential
   // that this could get called twice in certain corner cases.  This is a fatal error in
   // the base class.  So, we must make sure that this doesn't happen.  See IT 9287.
   if (Base_Apheresis_Window::allocated())
      DataLog (log_level_gui_error) << "Cannot allocate aasRunSummary screen more than once." << endmsg;
   else
      Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // Deallocate "standard" buttons that are not needed.
   btn_goback.deallocate_resources();
   btn_cancel.deallocate_resources();
   btn_lowleft.deallocate_resources();

   // Link the status bar/line to this window
   link_status_bar_line();

   setStatLineAndBar();

   selectGraphicAndInstruction(setIsPtf, setIsDrbc);
   RunsumGlobs::completeRunsumStatsProcessing();
}


void Screen_AAS_RUN_SUMMARY::setStatLineAndBar ()
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


void Screen_AAS_RUN_SUMMARY::selectRASGraphicAndInstruction (bool setIsPtf, bool setIsDrbc)
{
   if (_rasSalvageVol1 == 0 && _rasSalvageVol2 == 0) // RAS succeeded
   {
      if (setIsPtf)
      {
         if (setIsDrbc)
         {
            _txtMainInstructions.set_string_id(textMiscOfflineInstructionsDrbcAbove);
            _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_SEAL_DRBC_ABOVE_FILTER);

            DataLog(log_level_gui_info) << "Show DRBC RAS seal ABOVE filter" << endmsg;
         }
         else
         {
            _txtMainInstructions.set_string_id(textMiscOfflineInstructionsSrbcAbove);
            _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_SEAL_SRBC_ABOVE_FILTER);

            DataLog(log_level_gui_info) << "Show SRBC RAS seal ABOVE filter" << endmsg;
         }
      }
   }
   else  // RAS failed or terminated
   {
      if (setIsPtf)
      {
         if (setIsDrbc)
         {
            _txtMainInstructions.set_string_id(textMiscOfflineInstructionsDrbcBelow);
            _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_SEAL_DRBC_BELOW_FILTER);

            DataLog(log_level_gui_info) << "Show DRBC DRBC RAS seal BELOW filter" << endmsg;
         }
         else
         {
            _txtMainInstructions.set_string_id(textMiscOfflineInstructionsSrbcBelow);
            _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_SEAL_SRBC_BELOW_FILTER);

            DataLog(log_level_gui_info) << "Show SRBC SRBC RAS seal BELOW filter" << endmsg;
         }
      }
   }
}

// This function can be called once or twice in METERED_STORAGE_DISCONNECT state, depends on
// configuration and conditions. It won't be called in POST_RUN state. It is called by
// processRunsummData() function directly. but when it's called the second time for displaying
// the second screen, it's called by process_continue_button().function.
//
void Screen_AAS_RUN_SUMMARY::selectGraphicAndInstruction (bool setIsPtf, bool setIsDrbc)
{
   if (!_bmpTextAreaTop.allocated())
      _bmpTextAreaTop.allocate_resources(*this);

   const bool targetedAsAutoSplit = guiglobs::aasRunInfo.productTargetedAsDrbcPtf();

   if (_passes == 1) // this function is called one time in METERED_STORAGE_DISCONNECT stste
   {
      if (_pasSalvageVol > 0)  // PAS failed or terminated in PAS only procedures (1 case)
      {
         _txtMainInstructions.set_string_id(textMiscOfflineInstructionsNoSealInstructions);
         _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_NO_RAS);

         showOfflineAdditionData(_pasSalvageVol, _rasSalvageVol1, _rasSalvageVol2, targetedAsAutoSplit);
      }
      else
      {
         if (_haveBeenRas)   // _pasSalvageVol == 0, RAS succeeded or failed in RAS only or PAS+RAS procedures (4 cases)
         {
            selectRASGraphicAndInstruction(setIsPtf, setIsDrbc);

            showOfflineAdditionData(_pasSalvageVol, _rasSalvageVol1, _rasSalvageVol2, targetedAsAutoSplit);
         }
      }
   }
   else  // this function needs to be called two times in METERED_STORAGE_DISCONNECT stste
         // passes == 2, in PAS+RAS procedures, only PAS or both PAS and RAS failed, terminated (2 cases)
   {
      if (_pass == 0)  // the first call, only check and update PAS status
      {
         _txtMainInstructions.set_string_id(textMiscOfflineInstructionsNoSealInstructions);
         _bmpInstructionalGraphic.set_id(BITMAP_AAS_SUMM_NO_RAS);

         showOfflineAdditionData(_pasSalvageVol, 0, 0, false);
      }
      else   // this second call, _pass == 1, Only check and update RAS status
      {
         selectRASGraphicAndInstruction(setIsPtf, setIsDrbc);

         showOfflineAdditionData(0, _rasSalvageVol1, _rasSalvageVol2, targetedAsAutoSplit);
      }
   }

   if (!_bmpInstructionalGraphic.allocated())
      _bmpInstructionalGraphic.allocate_resources(*this);

   if (!_txtMainInstructions.allocated())
      _txtMainInstructions.allocate_resources(*this);

   DataLog(log_level_gui_info) << "Displayed AAS Graphics And Instruction: "
                               << " Instruction" << ":" << _bmpInstructionalGraphic.get_id() << ":"
                               << " Main Instr" << ":" << _txtMainInstructions.getStringId() << ":" << endmsg;

}


void Screen_AAS_RUN_SUMMARY::showOfflineAdditionData (int pasSalvageVol,
                                                      int rasSalvageVolProd1,
                                                      int rasSalvageVolProd2,
                                                      bool targetedAsAutoSplit)
{
   const bool haveSalvageVolToShow = (pasSalvageVol > 0)      ||
                                     (rasSalvageVolProd1 > 0) ||
                                     (rasSalvageVolProd2 > 0);

   if (haveSalvageVolToShow)
   {
      int yCoordAdjustment = 0;

      _bmpTextAreaBottom.allocate_resources(*this);
      _txtDataLbl.allocate_resources(*this);

      // Reset all the y coordinates.
      _pasMeteringItem.init();
      _rasMeteringItem1.init();
      _rasMeteringItem2.init();

      if (pasSalvageVol > 0)
      {
         // Do not edit this logging without talking to somebody from outcome review.
         DataLog (log_level_gui_external) << "Show PAS salvage volume: " << pasSalvageVol << endmsg;
         _pasMeteringItem.allocate_resources(*this, pasSalvageVol);
         yCoordAdjustment += Y_DELTA;
      }
      else
      {
         // Do not edit this logging without talking to somebody from outcome review.
         DataLog (log_level_gui_external) << "No PAS salvage volume to display." << endmsg;
      }

      if (_pass == 1)
         _pasMeteringItem.deallocate_resources();

      if (targetedAsAutoSplit)
      {
         DataLog (log_level_gui_info) << "RBC product was targeted as a split." << endmsg;

         if (rasSalvageVolProd1 > 0)
         {
            // Do not edit this logging without talking to somebody from outcome review.
            DataLog (log_level_gui_external) << "Show RAS salvage volume for RBC1: " << rasSalvageVolProd1 << endmsg;

            _rasMeteringItem1.changeLblId(textDataLabelRas1Offline);
            _rasMeteringItem1.allocate_resources(*this, rasSalvageVolProd1);
            _rasMeteringItem1.bumpYcoord(yCoordAdjustment);
            yCoordAdjustment += Y_DELTA;
         }
         else
         {
            // Do not edit this logging without talking to somebody from outcome review.
            DataLog (log_level_gui_external) << "No RAS salvage volume to display for RBC1." << endmsg;
         }


         if (rasSalvageVolProd2 > 0)
         {
            // Do not edit this logging without talking to somebody from outcome review.
            DataLog (log_level_gui_external) << "Show RAS salvage volume for RBC2: "
                                             << rasSalvageVolProd2 << endmsg;

            _rasMeteringItem2.allocate_resources(*this, rasSalvageVolProd2);
            _rasMeteringItem2.bumpYcoord(yCoordAdjustment);
         }
         else
         {
            // Do not edit this logging without talking to somebody from outcome review.
            DataLog (log_level_gui_external) << "No RAS salvage volume to display for RBC2." << endmsg;
         }
      }
      else if (rasSalvageVolProd1 > 0)
      {
         DataLog (log_level_gui_info) << "RBC product was targeted as a single." << endmsg;

         // Do not edit this logging without talking to somebody from outcome review.
         DataLog (log_level_gui_external) << "Show RAS salvage volume: " << rasSalvageVolProd1 << endmsg;

         // We didn't target this product as a split, so the label should read
         // as if it's a single product, not the first of two products.
         _rasMeteringItem1.changeLblId(textDataLabelRasOffline);
         _rasMeteringItem1.allocate_resources(*this, rasSalvageVolProd1);
         _rasMeteringItem1.bumpYcoord(yCoordAdjustment);
      }
      else
      {
         // Do not edit this logging without talking to somebody from outcome review.
         DataLog (log_level_gui_external) << "No RAS salvage volume to display." << endmsg;
      }
   }
   else
   {
      _bmpTextAreaBottom.deallocate_resources();
      _txtDataLbl.deallocate_resources();

      // Reset all the y coordinates.
      _pasMeteringItem.deallocate_resources();
      _rasMeteringItem1.deallocate_resources();
      _rasMeteringItem2.deallocate_resources();
   }
}

// This function is for unit testing this screen.  Send it a value from ENUMERATED_AAS_RUNSUMM_DISPLAY_TYPE
// Using the int message SimntGuiSwitchSalvageScreens in order to swap screen display types.
// (See function cycleThroughAasRunsumDisplayTypes() in unit_test/gui/screenshotGrabber.cpp for an example
// of how this is used).  -D.S.
#if (CPU==SIMNT)
void Screen_AAS_RUN_SUMMARY::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      ENUMERATED_AAS_RUNSUMM_DISPLAY_TYPE aasRunsummType = (ENUMERATED_AAS_RUNSUMM_DISPLAY_TYPE)traverseInfo.parameters[0];

      DataLog (log_level_sim_gui_side_info) << "Swapping aasRunSummary screen to type " << aasRunsummType << endmsg;

      // Initialize the data fields for the same reason.
      _pasMeteringItem.init();
      _rasMeteringItem1.init();
      _rasMeteringItem2.init();

      // First, deallocate the graphical elements to start with a clean slate.
      _pasMeteringItem.deallocate_resources();
      _rasMeteringItem1.deallocate_resources();
      _rasMeteringItem2.deallocate_resources();
      _txtMainInstructions.deallocate_resources();
      _txtDataLbl.deallocate_resources();
      _bmpTextAreaTop.deallocate_resources();
      _bmpTextAreaBottom.deallocate_resources();
      _bmpInstructionalGraphic.deallocate_resources();

      // Initialize these with common data, then change 'em
      // in the switch statement if necessary.
      bool  isPostRun          = true;
      bool  setIsPtf           = true;
      bool  setIsDrbc          = true;
      float rasSalvageVolProd1 = 100.0f;
      float rasSalvageVolProd2 = 100.0f;
      float pasSalvageVol      = 75.0f;

      // Some display-specific changaroos.
      switch (aasRunsummType)
      {
         case AAS_RUNSUM_PTF_SRBC_SALVAGE_BOTH :
            setIsDrbc          = false;
            rasSalvageVolProd2 = 0.0f;
            break;

         case AAS_RUNSUM_PTF_DRBC_SALVAGE_RAS :
            pasSalvageVol = 0.0f;
            break;

         case AAS_RUNSUM_PTF_SRBC_SALVAGE_RAS :
            setIsDrbc          = false;
            rasSalvageVolProd2 = 0.0f;
            pasSalvageVol      = 0.0f;
            break;

         case AAS_RUNSUM_PTF_SRBC_SALVAGE_PAS :
            setIsDrbc          = false;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;

         case AAS_RUNSUM_NON_PTF_SALVAGE_PAS :
            setIsPtf           = false;
            setIsDrbc          = false;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;

         case AAS_RUNSUM_PTF_DRBC_NON_SALVAGE :
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            pasSalvageVol      = 0.0f;
            break;

         case AAS_RUNSUM_PTF_SRBC_NON_SALVAGE :
            setIsDrbc          = false;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            pasSalvageVol      = 0.0f;
            break;
         case AAS_RUNSUM_INST_DRBC_ABOVE :
            isPostRun          = false;
            pasSalvageVol      = 0.0f;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;
         case AAS_RUNSUM_INST_DRBC_BELOW :
            isPostRun     = false;
            pasSalvageVol = 0.0f;
            break;
         case AAS_RUNSUM_INST_SRBC_BELOW :
            isPostRun = false;
            setIsDrbc = false;
            break;
         case AAS_RUNSUM_INST_SRBC_ABOVE_WITH_PAS :
            isPostRun          = false;
            setIsDrbc          = false;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;
         case AAS_RUNSUM_INST_SRBC_ABOVE :
            isPostRun          = false;
            setIsDrbc          = false;
            pasSalvageVol      = 0.0f;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;
         case AAS_RUNSUM_INST_NO_SEAL :
            isPostRun          = false;
            setIsDrbc          = false;
            setIsPtf           = false;
            pasSalvageVol      = 0.0f;
            rasSalvageVolProd1 = 0.0f;
            rasSalvageVolProd2 = 0.0f;
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown AAS Run Summary Display Type: " << aasRunsummType << endmsg;
            return;
      }

      selectGraphicAndInstruction(setIsPtf, setIsDrbc);
      showOfflineAdditionData(pasSalvageVol, rasSalvageVolProd1, rasSalvageVolProd2, setIsDrbc);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH d9efe3179b775f68f393b4b426e5f24e */
