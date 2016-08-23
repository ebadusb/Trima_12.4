/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_RUNSUMM2

*     File name:   runsumm2.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/

#include <vxworks.h>

// SYSTEM INCLUDE
#include <time.h>

// BASE CLASS INCLUDE
#include "runsumm2.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "frmtdec.hpp"



// EXTERNAL REFERENCES
#include "guiglobs.hpp"
#include "runsumGlobs.hpp"
#include "rebootConfirmBox.hpp"


// Callback for Japanese configuration of "next prodcedure" button
static void cbAltResetBtn (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_RUNSUMM2::Screen_RUNSUMM2()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_RUNSUMM2),

     _bmpPanel  (PLAC_RUNSUMM2_X,
                 125,
                 BITMAP_PLACARD_RUNSUMM2),

     _bmpPanelDrbcVersion (PLAC_RUNSUMM2_X,
                           125,
                           BITMAP_PLACARD_RUNSUMM2_RBC_SPLIT),

     _bmpPanelCompPlsVersion (PLAC_RUNSUMM2_X + 13,
                              117,
                              BITMAP_PLACARD_RUNSUMM2_COMPPLS),

     _btnAltReset(NULL),

// The display label objects
     _txtSectionLblPlt   (textMiscLabelRs2PltProd),
     _txtSectionLblPlas  (textMiscLabelRs2PlasProd),
     _txtSectionLblRbc   (textMiscLabelRs2RbcProd),
     _txtSectionLblRbc2  (textDataLabelRbc2SplitLayout),

     _dataPltProdVol     (textDataLabelRs2PltVol,
                          textEmptyFormatRs2DatPltVol,
                          textDataUnitsRs2PltVol),

     _dataPltYield       (textDataLabelRs2PltYield,
                          textEmptyFormatRs2DatPltYield,
                          textDataUnitsRs2PltYield,
                          textDataUnitsRs2PltYieldExp),

     _dataPltAcVol       (textDataLabelRs2PltAc,
                          textEmptyFormatRs2DatPltAc,
                          textDataUnitsRs2PltAc),

     _dataPltAasVol      (textDataLabelRs2PasVol,
                          textEmptyFormatRs2DatPasVol,
                          textDataUnitsRs2PasVol),

     _dataPlasProdVol    (textDataLabelRs2PlasVol,
                          textEmptyFormatRs2DatPlsVol,
                          textDataUnitsRs2PlasVol),

     _dataPlasAcVol      (textDataLabelRs2PlasAc,
                          textEmptyFormatRs2DatPlsAc,
                          textDataUnitsRs2PlasAc),

     _dataRbcProdVol     (textDataLabelRs2RbcVol,
                          textEmptyFormatRs2DatRbcVol,
                          textDataUnitsRs2RbcVol),

     _dataRbcAcVol       (textDataLabelRs2RbcAc,
                          textEmptyFormatRbcAc,
                          textDataUnitsRs2RbcAc),

     _dataRbcAasVol      (textDataLabelRs2RasVol,
                          textEmptyFormatRs2DatRasVol,
                          textDataUnitsRs2RasVol),

     _dataRbc2ProdVol    (textDataLabelRs2Rbc2Vol,
                          textEmptyFormatRs2DatRbc2Vol,
                          textDataUnitsRs2Rbc2Vol),

     _dataRbc2AcVol      (textDataLabelRs2Rbc2Ac,
                          textEmptyFormatRs2DatRbc2Ac,
                          textDataUnitsRs2Rbc2Ac),

     _dataRbc2AasVol     (textDataLabelRs2Ras2Vol,
                          textEmptyFormatRs2DatRas,
                          textDataUnitsRs2Ras2Vol),


/////////////////////////////////////////////////
//  Complementary Plasma section
/////////////////////////////////////////////////
//
     _txtSectionLblPltForComp(),


     _dataPltProdVolComp     (CMP2textDataLabelRs2PltVol,
                              CMP2textEmptyFormatRs2DatPltVol,
                              CMP2textDataUnitsRs2PltVol),


     _dataPltYieldForComp (CMP2textDataLabelRs2PltYield,
                           CMP2textEmptyFormatRs2DatPltYield,
                           CMP2textDataUnitsRs2PltYield,
                           CMP2textDataUnitsRs2PltYieldExp),

     _dataPltAcVolForComp (CMP2textDataLabelRs2PltAc,
                           CMP2textEmptyFormatRs2DatPltAc,
                           CMP2textDataUnitsRs2PltAc),

     _dataPltAasVolForComp(CMP2textDataLabelRs2PasVol,
                           CMP2textEmptyFormatRs2DatPasVol,
                           CMP2textDataUnitsRs2PasVol),






//
     _txtSectionLblPlasForComp(TEXT_PLS_LABEL_COMP_PLS),


     _dataPlasProdVolForComp  (CMP2textDataLabelRs2PlasVol,
                               CMP2textEmptyFormatRs2DatPlsVol,
                               CMP2textDataUnitsRs2PlasVol),

     _dataCompPls        (TEXT_COMP_PLS_ERS_LABEL,
                          TEXT_COMP_PLS_ERS_VOL_DATA_EMPTY,
                          TEXT_COMP_PLS_ERS_VOL_DATA_UNIT),


     _dataPlasAcVolForComp  (CMP2textDataLabelRs2PlasAc,
                             CMP2textEmptyFormatRs2DatPlsAc,
                             CMP2textDataUnitsRs2PlasAc),


//

     _txtSectionLblRbcForComp(TEXT_RBC_LABEL_COMP_PLS),


     _dataRbcProdVolForComp(CMP2textDataLabelRs2RbcVol,
                            CMP2textEmptyFormatRs2DatRbcVol,
                            CMP2textDataUnitsRs2RbcVol),

     _dataRbcAcVolForComp(CMP2textDataLabelRs2RbcAc,
                          CMP2textEmptyFormatRs2DatRbcAc,
                          CMP2textDataUnitsRs2RbcAc),

     _dataRbcAasVolForComp(CMP2textDataLabelRs2RasVol,
                           CMP2textEmptyFormatRs2DatRasVol,
                           CMP2textDataUnitsRs2RasVol),

     _bmpCompPls_Icon(525, 272, BITMAP_RUNSUMM2_COMP_PLS_ICON),


     _showCompPlasmaIcon(false),
     _COMP_PLS_ERS(false)

{
   _dataPltProdVol.setDashesForZeros();
   _dataPltYield.setDashesForZeros();
   _dataPltAcVol.setDashesForZeros();
   _dataPltAasVol.setDashesForZeros();
   _dataPlasProdVol.setDashesForZeros();
   _dataPlasAcVol.setDashesForZeros();
   _dataRbcProdVol.setDashesForZeros();
   _dataRbcAcVol.setDashesForZeros();
   _dataRbcAasVol.setDashesForZeros();
   _dataRbc2ProdVol.setDashesForZeros();
   _dataRbc2AcVol.setDashesForZeros();
   _dataRbc2AasVol.setDashesForZeros();

   _dataCompPls.setDashesForZeros();
   _dataPltAcVolForComp.setDashesForZeros();
   _dataPltYieldForComp.setDashesForZeros();

   _dataPlasProdVolForComp.setDashesForZeros();
   _dataPlasAcVolForComp.setDashesForZeros();
   _dataPltAasVolForComp.setDashesForZeros();
   _dataPltProdVolComp.setDashesForZeros();


   _dataRbcProdVolForComp.setDashesForZeros();
   _dataRbcAcVolForComp.setDashesForZeros();
   _dataRbcAasVolForComp.setDashesForZeros();
}   // END of Screen_RUNSUMM2 CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_RUNSUMM2::~Screen_RUNSUMM2()
{
   Screen_RUNSUMM2::deallocate_resources ();
}   // END of Screen_RUNSUMM2 DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_RUNSUMM2::allocate_resources (const char* allocation_parameter)
{
   if ( guiglobs::complementaryPlasmaEnabled() && guiglobs::cassette_mgr->currentSetIsPas() )
   {
      DataLog (log_level_gui_info) << "ERS complementaryPlasmaEnabled" << endmsg;
      _COMP_PLS_ERS = true;
      _txtSectionLblPltForComp.set_string_id(TEXT_PLT_LABEL_COMP_PLS);
   }
   else
   {
      DataLog (log_level_gui_info) << "ERS complementaryPlasmaDisabled" << endmsg;
      _COMP_PLS_ERS = false;
   }

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front. Only get the parent widget
   //  once to reduce overhead.

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_cancel.deallocate_resources ();

   //
   //  Disable continue until we get run summary data
   //
   btn_continue.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   guiglobs::apheresis_status_line->set_current_text (textStatEndOfRun);

   // Activate the receive message object to receive the runsumm1 data
   _msgPtrRunStats.init(Callback<Screen_RUNSUMM2>(this, &Screen_RUNSUMM2::processRunStats), MessageBase::SNDRCV_RECEIVE_ONLY);

   // Send a message to request the runsumm1 data
   RunsumGlobs::requestRunsumStats();

   chooseScreenLayout(guiglobs::aasRunInfo.productTargetedAsDrbcPtf());

}  // END of Screen_RUNSUMM2 ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_RUNSUMM2::deallocate_resources ()
{
   // deallocate all constituent objects first

   // the display text label objects

   _blinkCollection.stopBlink();
   destroyConfirmCallbacks();

   _txtSectionLblPlt.deallocate_resources();
   _txtSectionLblPlas.deallocate_resources();
   _txtSectionLblRbc.deallocate_resources();
   _txtSectionLblRbc2.deallocate_resources();
   _txtSectionLblPlasForComp.deallocate_resources();
   _txtSectionLblPltForComp.deallocate_resources();
   _txtSectionLblRbcForComp.deallocate_resources();

   _dataPltProdVol.deallocate_resources();
   _dataPltProdVolComp.deallocate_resources();
   _dataPltYield.deallocate_resources();
   _dataPltAcVol.deallocate_resources();
   _dataPltAasVol.deallocate_resources();

   _dataPlasProdVol.deallocate_resources();
   _dataPlasAcVol.deallocate_resources();

   _dataRbcProdVol.deallocate_resources();
   _dataRbcAcVol.deallocate_resources();
   _dataRbcAasVol.deallocate_resources();

   _dataRbc2ProdVol.deallocate_resources();
   _dataRbc2AcVol.deallocate_resources();
   _dataRbc2AasVol.deallocate_resources();
   _dataCompPls.deallocate_resources();
   _dataPlasProdVolForComp.deallocate_resources();
   _dataPlasAcVolForComp.deallocate_resources();
   _dataPltAasVolForComp.deallocate_resources();
   _dataPltAcVolForComp.deallocate_resources();
   _dataPltYieldForComp.deallocate_resources();

   if (_showCompPlasmaIcon && _COMP_PLS_ERS)
      _bmpCompPls_Icon.deallocate_resources();


   // the runsumm2 data panel
   _bmpPanel.deallocate_resources();
   _bmpPanelDrbcVersion.deallocate_resources();
   _bmpPanelCompPlsVersion.deallocate_resources();



   _dataRbcProdVolForComp.deallocate_resources();
   _dataRbcAcVolForComp.deallocate_resources();
   _dataRbcAasVolForComp.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Deallocate the Runsumm2 receive message object
   if (_btnAltReset)
   {
      delete _btnAltReset;
      _btnAltReset = NULL;
   }

   _msgPtrRunStats.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();


}   // END of Screen_RUNSUMM2 DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_RUNSUMM2::process_continue_button ()
{
   callConfirmBox();
}

void Screen_RUNSUMM2::callConfirmBox ()
{
   btn_continue.deallocate_resources();

   createConfirmCallbacks();

   DataLog (log_level_gui_info) << "Putting up a restart confirmation message." << endmsg;
   goto_child_screen(GUI_SCREEN_REBOOT_CONFIRM_BOX, "RESTART CONFIRMATION BOX");
}

void Screen_RUNSUMM2::confirmCB ()
{
   destroyConfirmCallbacks();

   // Disable all the standard buttons on this screen
   disable_standard_buttons();

    #if CPU!=SIMNT
   // Clear and Shutdown the barcode repository
   if (guiglobs::barcode_repo)  // This should always be a valid pointer here, but check it just in case.
   {
      guiglobs::barcode_repo -> clean_category_storage();
      guiglobs::barcode_repo -> shut_down_repository();
   }
    #endif /* if CPU!=SIMNT */

   goto_screen(GUI_SCREEN_POWERDOWN, "GUI_SCREEN_POWERDOWN");
}   // END of Screen_RUNSUMM2 PROCESS_CONTINUE_BUTTON

void Screen_RUNSUMM2::cancelCB ()
{
   const bool alternateNextProcedureButton = Software_CDS::GetInstance().getFeature(JapanFeatures);

   destroyConfirmCallbacks();

   if (alternateNextProcedureButton && _btnAltReset) // null test just in case
   {
      _btnAltReset->allocate_resources(*this);  // IT10629
   }
   else
   {
      btn_continue.allocate_resources(*this);
   }
}

void Screen_RUNSUMM2::createConfirmCallbacks ()
{
   _msgRebootConfirmed.init(Callback<Screen_RUNSUMM2>(this, &Screen_RUNSUMM2::confirmCB));
   _msgRebootCanceled.init(Callback<Screen_RUNSUMM2>(this, &Screen_RUNSUMM2::cancelCB));
}

void Screen_RUNSUMM2::destroyConfirmCallbacks ()
{
   _msgRebootConfirmed.deregisterMsg();
   _msgRebootCanceled.deregisterMsg();
}

////////////////////////////////////////////////////////////////////////////////
// PROCESS_GOBACK_BUTTON

void Screen_RUNSUMM2::process_goback_button ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   bool shouldShowSalvage                    = false;

   const RUN_SUMMARY_STRUCT& runSummaryStats = _msgPtrRunStats.getData();
   shouldShowSalvage = RunsumGlobs::shouldShowAasSalvage(runSummaryStats);

   // Go back to the first run summary screen, which is offline
   // addition if that's necessary, and run summary 1 otherwise.
   if (shouldShowSalvage)
      goto_screen (GUI_SCREEN_AAS_RUN_SUMMARY, "GUI_SCREEN_AAS_RUN_SUMMARY");
   else
      goto_screen (GUI_SCREEN_RUNSUMM1, "GUI_SCREEN_RUNSUMM1");
}   // END of Screen_RUNSUMM2 PROCESS_GOBACK_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_RUNSUMM2_DATA
void Screen_RUNSUMM2::processRunStats ()
{
   guiglobs::dont_connect_icon.deallocate_resources();

   const RUN_SUMMARY_STRUCT& runSummaryStats              = _msgPtrRunStats.getData();
   const bool                alternateNextProcedureButton = Software_CDS::GetInstance().getFeature(JapanFeatures);

   DataLog (log_level_gui_info) << "Receive ProcData Stats " << endmsg;

   // Decide whether to display the version with two RBC products and no PLT product
   setData(runSummaryStats);

   // Put up a next procedure button (or the japanese replacement for it) iff
   //      1) The run is complete.
   //      2) We're not still in metered storage, and
   //      3) The button is not already allocated.
   if ( (runSummaryStats.run_complete)
        && (!btn_continue.allocated())
        && guiglobs::screen_status.get_current_system_state() != METERED_STORAGE)
   {
      if (alternateNextProcedureButton)
      {
         //
         //  Create replacement for Japanese configuration - yellow btn w/ "reset" text.
         //
         _btnAltReset = new Button_Bitmap (GUI_BUTTON_RUNSUMMARY2_RESET_MACHINE,       // The button's ID.
                                           BUTNCONT_X,                                 // Continue button X coordinates.
                                           BUTNCONT_Y,                                 // Continue button Y coordinates.
                                           textButtonLongResetMachine,                 // Reset button text.
                                           BITMAP_BUTTON_RESET_MACHINE_UP,             // Set yellow up bitmap.
                                           BITMAP_BUTTON_RESET_MACHINE_DOWN);          // Set down bitmap.

         _btnAltReset->allocate_resources (*this);
         _btnAltReset->set_callback (aph_callback_continue, (void*)this);

      }
      else
      {
         // Change the message on the continue to say "next procedure".
         btn_continue.allocate_resources(*this);
         btn_continue.set_callback  (aph_callback_continue, (void*)this);
         btn_continue.set_string_id (textButtonLongNextProcedure);
      }
   }

   State_names currentSystemState = guiglobs::screen_status.get_current_system_state();

   bool        flashPasStats      =
      guiglobs::aasRunInfo.isPasProcedure()
      && (currentSystemState == BLOOD_RINSEBACK || currentSystemState == METERED_STORAGE);

   bool flashRasStats =
      guiglobs::aasRunInfo.isRasProcedure()
      && (currentSystemState == BLOOD_RINSEBACK || currentSystemState == METERED_STORAGE);

   bool flashPlasmaStats =
      runSummaryStats.plasma_rinseback_active
      && currentSystemState == BLOOD_RINSEBACK;

   chooseBlinkingText(flashPasStats, flashRasStats, flashPlasmaStats, runSummaryStats.drbcAutoSplit);

   outputData();

   RunsumGlobs::completeRunsumStatsProcessing();
}


void Screen_RUNSUMM2::chooseBlinkingText (bool flashPasStats, bool flashRasStats, bool flashPlasmaStats, bool isSplitLayout)
{
   if (flashPasStats || flashRasStats || flashPlasmaStats)
   {
      DataLog (log_level_gui_info) << "Flashing text: (PAS:" << flashPasStats
                                   << ", RAS:" << flashRasStats << ", Plasma:" << flashPlasmaStats
                                   << ")" << endmsg;


      if ( _COMP_PLS_ERS )
      {

         if (flashPasStats)
         {
            _blinkCollection.addObject(&_dataPltProdVolComp);
            _blinkCollection.addObject(&_dataPltAasVolForComp);
            _blinkCollection.addObject(&_dataCompPls);
         }

         if (flashRasStats)
         {
            _blinkCollection.addObject(&_dataRbcProdVolForComp);
            _blinkCollection.addObject(&_dataRbcAasVolForComp);

            // drbc Split layout and Comp Plsm are mutually exclusive:
//                 if (isSplitLayout)
//                 {
//                     _blinkCollection.addObject(&_dataRbc2ProdVolForComp);
//                     _blinkCollection.addObject(&_dataRbc2AasVolForComp);
//                 }
         }

         if (flashPlasmaStats)
         {
            _blinkCollection.addObject(&_dataPlasProdVol);
            _blinkCollection.addObject(&_dataPlasAcVol);
         }

      }
      else
      {


         if (flashPasStats)
         {
            _blinkCollection.addObject(&_dataPltProdVol);
            _blinkCollection.addObject(&_dataPltAasVol);
         }

         if (flashRasStats)
         {
            _blinkCollection.addObject(&_dataRbcProdVol);
            _blinkCollection.addObject(&_dataRbcAasVol);

            if (isSplitLayout)
            {
               _blinkCollection.addObject(&_dataRbc2ProdVol);
               _blinkCollection.addObject(&_dataRbc2AasVol);
            }
         }

         if (flashPlasmaStats)
         {
            _blinkCollection.addObject(&_dataPlasProdVol);
            _blinkCollection.addObject(&_dataPlasAcVol);
         }

      }

      _blinkCollection.startBlink();
   }
   else
   {
      DataLog (log_level_gui_info) << "No flashing text required." << endmsg;
   }
}

void Screen_RUNSUMM2::chooseScreenLayout (bool splitDrbcLayout)
{
   // Non-conditional text objects
   _txtSectionLblPlas.allocate_resources (*this);
   _txtSectionLblRbc.allocate_resources (*this);

   _dataPlasProdVol.allocate_resources (*this);
   _dataPlasAcVol.allocate_resources (*this);

   _dataRbcProdVol.allocate_resources (*this);
   _dataRbcAcVol.allocate_resources (*this);
   _dataRbcAasVol.allocate_resources (*this);

   if (splitDrbcLayout)
   {
      DataLog(log_level_gui_info) << "Choosing DRBC string layout" << endmsg;

      _txtSectionLblPlt.deallocate_resources();

      _dataPltProdVol.deallocate_resources();
      _dataPltProdVolComp.deallocate_resources();
      _dataPltYield.deallocate_resources();
      _dataPltAcVol.deallocate_resources();
      _dataPltAasVol.deallocate_resources();

      _txtSectionLblRbc2.allocate_resources(*this);

      _dataRbc2ProdVol.allocate_resources(*this);
      _dataRbc2AcVol.allocate_resources(*this);
      _dataRbc2AasVol.allocate_resources(*this);

      _bmpPanelDrbcVersion.allocate_resources(*this);
      _bmpPanel.deallocate_resources();

      // Set conditional text and styling
      _txtSectionLblPlas.set_string_id(textMiscLabelRs2PlasProdSplitLayout);
      _txtSectionLblRbc.set_string_id(textDataLabelRs2RbcSplitLayout);

      _dataPlasProdVol.updateStringIds(textDataLabelRs2PlasVolSplitLayout,
                                       textEmptyFormatRs2DatPlsVolSplitLayout,
                                       textDataUnitsRs2PlasVolSplitLayout);

      _dataPlasAcVol.updateStringIds(textDataLabelRs2PlasAcSplitLayout,
                                     textEmptyFormatRs2DatPlsAcSplitLayout,
                                     textDataUnitsRs2PlasAcSplitLayout);

      _dataRbcProdVol.updateStringIds(textDataLabelRs2RbcVolSplitLayout,
                                      textEmptyFormatRs2DatRbcVolSplitLayout,
                                      textDataUnitsRs2RbcVolSplitLayout);

      _dataRbcAcVol.updateStringIds(textDataLabelRs2RbcAcSplitLayout,
                                    textEmptyFormatRs2DatRbcAcSplitLayout,
                                    textDataUnitsRs2RbcAcSplitLayout);

      _dataRbcAasVol.updateStringIds(textDataLabelRs2RasVolSplitLayout,
                                     textEmptyFormatRs2DatRbcRasSplitLayout,
                                     textDataUnitsRs2RasVolSplitLayout);

      _dataRbc2AasVol.updateStringIds(textDataLabelRs2Ras2Vol,
                                      textEmptyFormatRs2DatRas,
                                      textDataUnitsRs2Ras2Vol);

   }
   else if (_COMP_PLS_ERS)
   {

      DataLog(log_level_gui_info) << "Choosing COMP_PLS SRBC string layout" << endmsg;

      _bmpPanelDrbcVersion.deallocate_resources();
      _bmpPanel.deallocate_resources();
      _dataRbc2ProdVol.deallocate_resources();
      _dataRbc2AcVol.deallocate_resources();
      _dataRbc2AasVol.deallocate_resources();
      _dataPlasProdVol.deallocate_resources();
      _dataPlasAcVol.deallocate_resources();
      _dataPltProdVol.deallocate_resources();
      _txtSectionLblRbc2.deallocate_resources();
      _txtSectionLblPlas.deallocate_resources();
      _txtSectionLblPlt.deallocate_resources();
      _dataPltYield.deallocate_resources();
      _dataPltAcVol.deallocate_resources();
      _dataPltAasVol.deallocate_resources();


      _txtSectionLblPlasForComp.allocate_resources(*this);
      _txtSectionLblPltForComp.allocate_resources(*this);
      _txtSectionLblRbcForComp.allocate_resources(*this);




      _dataPlasProdVolForComp.allocate_resources(*this);
      _dataPlasProdVolForComp.updateStringIds(CMP2textDataLabelRs2PlasVol,
                                              CMP2textEmptyFormatRs2DatPlsVol,
                                              CMP2textDataUnitsRs2PlasVol);

      _dataCompPls.allocate_resources(*this);
      _dataCompPls.updateStringIds (TEXT_COMP_PLS_ERS_LABEL,
                                    TEXT_COMP_PLS_ERS_VOL_DATA_EMPTY,
                                    TEXT_COMP_PLS_ERS_VOL_DATA_UNIT);

      _dataPlasAcVolForComp.allocate_resources(*this);
      _dataPlasAcVolForComp.updateStringIds(CMP2textDataLabelRs2PlasAc,
                                            CMP2textEmptyFormatRs2DatPlsAc,
                                            CMP2textDataUnitsRs2PlasAc);



      _dataPltProdVolComp.allocate_resources(*this);
      _dataPltYieldForComp.allocate_resources(*this);
      _dataPltAcVolForComp.allocate_resources(*this);
      _dataPltAasVolForComp.allocate_resources(*this);
      _dataPltAasVolForComp.updateStringIds(CMP2textDataLabelRs2PasVol,
                                            CMP2textEmptyFormatRs2DatPasVol,
                                            CMP2textDataUnitsRs2PasVol);






      _bmpPanelCompPlsVersion.allocate_resources(*this);


      _txtSectionLblRbc.deallocate_resources();
      _dataRbcProdVol.deallocate_resources();
      _dataRbcAcVol.deallocate_resources();
      _dataRbcAasVol.deallocate_resources();

      // Set conditional text and styling

      _txtSectionLblRbcForComp.set_string_id(TEXT_RBC_LABEL_COMP_PLS);

      _dataRbcProdVolForComp.allocate_resources(*this);
      _dataRbcProdVolForComp.updateStringIds(CMP2textDataLabelRs2RbcVol,
                                             CMP2textEmptyFormatRs2DatRbcVol,
                                             CMP2textDataUnitsRs2RbcVol);


      _dataRbcAcVolForComp.allocate_resources(*this);
      _dataRbcAcVolForComp.updateStringIds(CMP2textDataLabelRs2RbcAc,
                                           CMP2textEmptyFormatRs2DatRbcAc,
                                           CMP2textDataUnitsRs2RbcAc);



      _dataRbcAasVolForComp.allocate_resources(*this);
      _dataRbcAasVolForComp.updateStringIds(CMP2textDataLabelRs2RasVol,
                                            CMP2textEmptyFormatRs2DatRasVol,
                                            CMP2textDataUnitsRs2RasVol);

      if (_showCompPlasmaIcon)
         _bmpCompPls_Icon.allocate_resources(*this);
   }
   else
   {
      DataLog(log_level_gui_info) << "Choosing SRBC string layout" << endmsg;

      _txtSectionLblRbc2.deallocate_resources();
      _dataRbc2ProdVol.deallocate_resources();
      _dataRbc2AcVol.deallocate_resources();
      _dataRbc2AasVol.deallocate_resources();

      _txtSectionLblPlt.allocate_resources(*this);

      _dataPltProdVol.allocate_resources(*this);
      _dataPltYield.allocate_resources(*this);
      _dataPltAcVol.allocate_resources(*this);
      _dataPltAasVol.allocate_resources(*this);

      _bmpPanelDrbcVersion.deallocate_resources();
      _bmpPanel.allocate_resources(*this);

      // Set conditional text and styling
      _txtSectionLblPlas.set_string_id(textMiscLabelRs2PlasProd);
      _txtSectionLblRbc.set_string_id(textMiscLabelRs2RbcProd);

      _dataPlasProdVol.updateStringIds(textDataLabelRs2PlasVol,
                                       textEmptyFormatRs2DatPlsVol,
                                       textDataUnitsRs2PlasVol);

      _dataPlasAcVol.updateStringIds(textDataLabelRs2PlasAc,
                                     textEmptyFormatRs2DatPlsAc,
                                     textDataUnitsRs2PlasAc);

      _dataRbcProdVol.updateStringIds(textDataLabelRs2RbcVol,
                                      textEmptyFormatRs2DatRbcVol,
                                      textDataUnitsRs2RbcVol);

      _dataRbcAcVol.updateStringIds(textDataLabelRs2RbcAc,
                                    textEmptyFormatRbcAc,
                                    textDataUnitsRs2RbcAc);

      _dataRbcAasVol.updateStringIds(textDataLabelRs2RasVol,
                                     textEmptyFormatRs2DatRasVol,
                                     textDataUnitsRs2RasVol);


   }
}

void Screen_RUNSUMM2::setData (const RUN_SUMMARY_STRUCT& runSummaryStats)
{
   _dataPltProdVol.setData(runSummaryStats.rs2_platelet_volume);
   _dataPltYield.setData(((float)runSummaryStats.rs2_yield_of_platelets / 10.0f), 1);
   _dataPltAcVol.setData(runSummaryStats.rs2_vol_AC_in_platelets);
   _dataPltAasVol.setData(runSummaryStats.rs2_SS_plat_vol);

   _dataPlasProdVol.setData(runSummaryStats.rs2_plasma_volume);
   //
   //
   //  IT10221 rounding fix for AC in plasma display
   //
   if (runSummaryStats.rs2_plasma_volume < 1)
   {
      _dataPlasAcVol.setData(0);
   }
   else
   {
      _dataPlasAcVol.setData(runSummaryStats.rs2_vol_AC_in_plasma);
   }

   _dataRbcProdVol.setData(runSummaryStats.rs2_RBC_volume);
   _dataRbcAcVol.setData(runSummaryStats.rs2_vol_AC_in_RBC);
   _dataRbcAasVol.setData(runSummaryStats.rs2_SS_rbc_vol);


   _dataRbc2ProdVol.setData(runSummaryStats.rs2_RBC_volume_2);
   _dataRbc2AcVol.setData(runSummaryStats.rs2_vol_AC_in_RBC_2);
   _dataRbc2AasVol.setData(runSummaryStats.rs2_SS_rbc_vol_2);


   _dataCompPls.setData(runSummaryStats.rs2_comp_pls_volm);
   _dataPlasProdVolForComp.setData(runSummaryStats.rs2_plasma_volume);
   _dataPlasAcVolForComp.setData(runSummaryStats.rs2_vol_AC_in_plasma);
   _dataPltAasVolForComp.setData(runSummaryStats.rs2_SS_plat_vol);
   _dataPltAcVolForComp.setData(runSummaryStats.rs2_vol_AC_in_platelets);
   _dataPltYieldForComp.setData(((float)runSummaryStats.rs2_yield_of_platelets / 10.0f), 1);
   _dataPltProdVolComp.setData(runSummaryStats.rs2_platelet_volume);

   _dataRbcProdVolForComp.setData(runSummaryStats.rs2_RBC_volume);
   _dataRbcAcVolForComp.setData(runSummaryStats.rs2_vol_AC_in_RBC);
   _dataRbcAasVolForComp.setData(runSummaryStats.rs2_SS_rbc_vol);


   DataLog (log_level_gui_info) <<  "rs2_plasma_volume "  << runSummaryStats.rs2_plasma_volume << endmsg;
   DataLog (log_level_gui_info) <<  "rs2_vol_AC_in_plasma "  << runSummaryStats.rs2_vol_AC_in_plasma << endmsg;
   DataLog (log_level_gui_info) <<  "rs2_comp_pls_volm "  << runSummaryStats.rs2_comp_pls_volm << endmsg;

   if (_COMP_PLS_ERS && (runSummaryStats.rs2_comp_pls_volm > 0))
   {
      _showCompPlasmaIcon = runSummaryStats.rs2_comp_pls_icon;    // Proc sends it is false if feature is disabled

      if (_showCompPlasmaIcon)
         _bmpCompPls_Icon.allocate_resources(*this);
   }
   else
   {
      DataLog (log_level_gui_info) << "CompPls: no volume to show. use std layout" << endmsg;
   }

}


////////////////////////////////////////////////////////////////////////////////////////////////////////
void Screen_RUNSUMM2::outputData ()
{
   bool splitDrbcLayout = guiglobs::aasRunInfo.productTargetedAsDrbcPtf();

   if (!splitDrbcLayout)
   {
      DataLog (log_level_gui_info) << " (PLTVOL) " << _dataPltProdVol;
      DataLog (log_level_gui_info) << " (PLTYLD) " << _dataPltYield;
      DataLog (log_level_gui_info) << " (PLTAC) " << _dataPltAcVol;
      DataLog (log_level_gui_info) << " (PLTAAS) " << _dataPltAasVol;
   }

   DataLog (log_level_gui_info) << " (PLASVOL) " << _dataPlasProdVol;

   if (_COMP_PLS_ERS)
   {
      DataLog (log_level_gui_info) << " (CPPLAS) " << _dataCompPls;
      DataLog (log_level_gui_info) << " (CPPLTYLD) " << _dataPltYieldForComp;
      DataLog (log_level_gui_info) << " (CPPLTVOL) " << _dataPltProdVolComp;
      DataLog (log_level_gui_info) << " (CPPLTAC) " << _dataPltAcVolForComp;
      DataLog (log_level_gui_info) << " (CPPLTAAS) " << _dataPltAasVolForComp;
      DataLog (log_level_gui_info) << " (CPPLAS) " << _dataCompPls;
      DataLog (log_level_gui_info) << " (CPPLASVOL) " << _dataPlasProdVolForComp;
      DataLog (log_level_gui_info) << " (CPPLASAC) " << _dataPlasAcVolForComp;
      DataLog (log_level_gui_info) << " (CPRBCVOL) " << _dataRbcProdVolForComp;
      DataLog (log_level_gui_info) << " (CPRBCAC) " << _dataRbcAcVolForComp;
      DataLog (log_level_gui_info) << " (CPRBCAAS) " << _dataRbcAasVolForComp;
   }

   DataLog (log_level_gui_info) << " (PLASAC) " << _dataPlasAcVol;

   if (splitDrbcLayout)
   {
      DataLog (log_level_gui_info) << " (RBC1VOL) " << _dataRbcProdVol;
      DataLog (log_level_gui_info) << " (RBC1AC) " << _dataRbcAcVol;
      DataLog (log_level_gui_info) << " (RBC1AAS) " << _dataRbcAasVol;

      DataLog (log_level_gui_info) << " (RBC2VOL) " << _dataRbc2ProdVol;
      DataLog (log_level_gui_info) << " (RBC2AC) " << _dataRbc2AcVol;
      DataLog (log_level_gui_info) << " (RBC2AAS) " << _dataRbc2AasVol;
   }
   else
   {
      DataLog (log_level_gui_info) << " (RBCVOL) " << _dataRbcProdVol;
      DataLog (log_level_gui_info) << " (RBCAC) " << _dataRbcAcVol;
      DataLog (log_level_gui_info) << " (RBCAAS) " << _dataRbcAasVol;
   }
   DataLog (log_level_gui_info) << endmsg;
}

#if CPU==SIMNT
void Screen_RUNSUMM2::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      RUN_SUMMARY_STRUCT& runSummaryStats = const_cast<RUN_SUMMARY_STRUCT&>(_msgPtrRunStats.getData());



      RS2_DISPLAY_TYPE runsumm2DisplayType = (RS2_DISPLAY_TYPE)traverseInfo.parameters[0];
      int              complementaryPlasma = (int)traverseInfo.parameters[1];
      if (complementaryPlasma == 0xC03)
         _COMP_PLS_ERS = true;
      else
         _COMP_PLS_ERS = false;

      bool isDrbcRun = false;

      switch (runsumm2DisplayType)
      {
         case RS2_DRBC :
            isDrbcRun = true;
            break;
         case RS2_SRBC :
            isDrbcRun = false;
            _txtSectionLblPltForComp.set_string_id(TEXT_PLT_LABEL_COMP_PLS);
            break;
         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Runsumm 2 Display Type: " << runsumm2DisplayType << endmsg;
            return;
      }

      chooseScreenLayout(isDrbcRun);
      runSummaryStats.drbcAutoSplit = isDrbcRun;
      processRunStats();
   }
   else if (traverseInfo.type == CHANGE_NEXT_PROCEDURE_BUTTON)
   {
      const bool alternateNextProcedureButton = Software_CDS::GetInstance().getFeature(JapanFeatures);

      // alternate_next_procedure_button always on now
      // guiglobs::cdsData.config.SWOptionSet("alternate_next_procedure_button", !alternateNextProcedureButton);
      // alternateNextProcedureButton = !alternateNextProcedureButton;

      DataLog(log_level_sim_gui_side_info) << "Changing Next Procedure Button to " << ((alternateNextProcedureButton) ? "Japanese" : "normal" ) << endmsg;

      if (btn_continue.allocated())
         btn_continue.deallocate_resources();

      if (alternateNextProcedureButton)
      {
         //
         //  Create replacement for Japanese configuration - yellow btn w/ "reset" text.
         //
         _btnAltReset = new Button_Bitmap (GUI_BUTTON_RUNSUMMARY2_RESET_MACHINE, // The button's ID.
                                           BUTNCONT_X,                           // Continue button X coordinates.
                                           BUTNCONT_Y,                           // Continue button Y coordinates.
                                           textButtonLongResetMachine,           // Reset button text.
                                           BITMAP_BUTTON_RESET_MACHINE_UP,       // Set yellow up bitmap.
                                           BITMAP_BUTTON_RESET_MACHINE_DOWN);    // Set down bitmap.

         _btnAltReset->allocate_resources (*this);
         _btnAltReset->set_callback (aph_callback_continue, (void*)this);

      }
      else
      {
         // Change the message on the continue to say "next procedure".
         btn_continue.allocate_resources(*this);
         btn_continue.set_callback  (aph_callback_continue, (void*)this);
         btn_continue.set_string_id (textButtonLongNextProcedure);
      }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif /* if CPU==SIMNT */

/* FORMAT HASH 5ae8a921a08ffd2211eab1ffb2e3410a */
