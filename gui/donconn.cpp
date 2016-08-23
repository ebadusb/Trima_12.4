/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_DONCONN

*     File name:   donconn.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>

// BASE CLASS INCLUDE
#include "donconn.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES
#include "guiglobs.hpp"

#if CPU==SIMNT
#include "guiUnitTestMessageDefs.h"
#endif // #if CPU==SIMNT

////////////////////////////////////////////////////////////////////////////////
// callbacks for the HEIGHT buttons
void cbProcessStartDrawBtn (void* data)
{
   ((Screen_DONCONN*)data)->processStartDrawBtn();
}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_DONCONN::Screen_DONCONN() :

   Base_Apheresis_Window(BLUE, GUI_SCREEN_DONCONN, DisplayOrder_Normal, BITMAP_BACKGROUND_NO_CONTINUE_OUTLINE),

   _stickMeter (BITMAP_ACCMETER_FRAME, // meter frame bitmap id
                10,                    // x offset of frame within the screen
                172,                   // y offset of frame within the screen
                3,                     // x offset of meter within the frame
                3,                     // y offset of meter within the frame
                200,                   // meter height in pixels
                10,                    // total positive tics
                10,                    // total negative tics
                150.0,                 // positive pressure value limit
                -150.0,                // negative pressure value limit
                1,                     // DO enable the display of the last pressure value
                                       // positive pressure indicator bitmaps //
                BITMAP_ACCMETER_GREEN_NOT_ACTIVE,
                BITMAP_ACCMETER_GREEN_CURR_ACTIVE,
                BITMAP_ACCMETER_GREEN_LAST_ACTIVE,
                BITMAP_ACCMETER_GREEN_OFFSCALE_ACTIVE,
                // negative pressure indicator bitmaps //
                BITMAP_ACCMETER_YELLOW_NOT_ACTIVE,
                BITMAP_ACCMETER_YELLOW_CURR_ACTIVE,
                BITMAP_ACCMETER_YELLOW_LAST_ACTIVE,
                BITMAP_ACCMETER_YELLOW_OFFSCALE_ACTIVE
                ),

   _bmpDiversionJp(PLAC_DONCONN_X, PLAC_DONCONN_Y, BITMAP_PLACARD_DONOR_CONNECT_DIVERSION_NO_SEAL),
   _bmpDiversion(PLAC_DONCONN_X, PLAC_DONCONN_Y, BITMAP_PLACARD_DONOR_CONNECT_DIVERSION),
   _bmpNoDiversionJp(PLAC_DONCONN_X, PLAC_DONCONN_Y, BITMAP_PLACARD_DONOR_CONNECT_NO_DIVERSION_NO_SEAL),
   _bmpNoDiversion(PLAC_DONCONN_X, PLAC_DONCONN_Y, BITMAP_PLACARD_DONOR_CONNECT_NO_DIVERSION),

   _btnStartDraw (GUI_BUTTON_DON_CONN_START_DRAW,
                  205, 383,
                  textButtonLongStartDraw,
                  BITMAP_DONOR_CONNECT_START_DRAW_BTN_UP,
                  BITMAP_DONOR_CONNECT_START_DRAW_BTN_DOWN),

   _txtPanelOneNoDiversion     (textMiscDonConnNoDiversion),
   _txtPanelOneDiversion       (textMiscInstructionDonConnCloseConnect),
   _txtPanelTwo                (textMiscInstructionDonConnOpen),
   _txtPanelThreeNoDiversion   (textMiscInstructionDonConnCloseSeal),
   _txtPanelThreeDiversion     (textMiscInstructionDonConnCloseSealOpen),
   _txtPanelThreeNoDiversionJp (textMiscInstructionDonConnClose),
   _txtPanelThreeDiversionJp   (textMiscInstructionDonConnCloseOpen),
   _currentProcData(guiglobs::proc_status.get_data() )
{}  // END of Screen_DONCONN CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_DONCONN::~Screen_DONCONN()
{
   Screen_DONCONN::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_DONCONN::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.

   chooseDynamicTextAndGraphics((bool)guiglobs::cdsData.config.Procedure.Get().key_blood_diversion,
                                Software_CDS::isJapaneseWorkflow());

   _txtPanelTwo.allocate_resources(*this);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources ();
   btn_continue.deallocate_resources ();

   // Allocate the start draw button
   _btnStartDraw.allocate_resources(*this);

   // allocate the stick, pressure meter and initialize the corresponding message
   _stickMeter.allocate_resources(*this) ;

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_LOAD);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_INFO);
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_PREPARE);

   // Send a Donor Committed message to Vista.
   // This implies that the donor will not be "rejected" and that a run
   // summary will be generated.
   VistaDonorCommittedMsg committed(MessageBase::SEND_GLOBAL);
   committed.send(0);

   guiglobs::proc_status.attach(this, 1250);
   guiglobs::proc_status.set_pressure_callback(Callback<Screen_DONCONN>(this, &Screen_DONCONN::manage_stick_pressure_data));

   _btnStartDraw.set_callback(cbProcessStartDrawBtn, (void*)this);
} // END of Screen_DONCONN ALLOCATE_RESOURCES


void Screen_DONCONN::deallocateDynamicTextAndGraphics ()
{
   _txtPanelOneNoDiversion.deallocate_resources();
   _txtPanelOneDiversion.deallocate_resources();
   _txtPanelThreeNoDiversion.deallocate_resources();
   _txtPanelThreeDiversion.deallocate_resources();
   _txtPanelThreeNoDiversionJp.deallocate_resources();
   _txtPanelThreeDiversionJp.deallocate_resources();

   _bmpDiversionJp.deallocate_resources();
   _bmpDiversion.deallocate_resources();
   _bmpNoDiversionJp.deallocate_resources();
   _bmpNoDiversion.deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// chooseDynamicTextAndGraphics

void Screen_DONCONN::chooseDynamicTextAndGraphics (bool bloodDiversionOn, bool dontShowSealSampleBag)
{
   deallocateDynamicTextAndGraphics();

   if (dontShowSealSampleBag)
   {
      _txtPanelOneNoDiversion.allocate_resources(*this);

      if (bloodDiversionOn)
      {
         _bmpDiversionJp.allocate_resources(*this);
         _txtPanelThreeDiversionJp.allocate_resources(*this);
      }
      else
      {
         _bmpNoDiversionJp.allocate_resources(*this);
         _txtPanelThreeNoDiversionJp.allocate_resources(*this);
      }
   }
   else
   {
      if (bloodDiversionOn)
      {
         _bmpDiversion.allocate_resources(*this);
         _txtPanelOneDiversion.allocate_resources(*this);
         _txtPanelThreeDiversion.allocate_resources(*this);
      }
      else
      {
         _bmpNoDiversion.allocate_resources(*this);
         _txtPanelOneNoDiversion.allocate_resources(*this);
         _txtPanelThreeNoDiversion.allocate_resources(*this);
      }
   }
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_DONCONN::deallocate_resources ()
{
   guiglobs::proc_status.detach(this);

   deallocateDynamicTextAndGraphics();
   _txtPanelTwo.deallocate_resources();

   _stickMeter.deallocate_resources() ;

   // Set the status bar to reflect that preparation is completed
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_PREPARE);

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}  // END of Screen_DONCONN DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_DONCONN::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}  // END of Screen_DONCONN PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_START_DRAW_BUTTON
void Screen_DONCONN::processStartDrawBtn ()
{
   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Tell the Procedure that the Donor is connected

   // NOTE: the GUIRequestStartDraw message followed this message in the
   // past. It has been removed (as a cause for a race condition with
   // Router and Proc -- causing both GUI and Proc to wait forever)
   // as a fix for IT 2388 and IT 2287 lockups.

   DonorConnectedMsg connected(MessageBase::SEND_GLOBAL);
   connected.send(0);
}  // END of Screen_DONCONN PROCESS_START_DRAW_BUTTON



////////////////////////////////////////////////////////////////////////////////
// MANAGE_STICK_PRESSURE_DATA
void Screen_DONCONN::manage_stick_pressure_data ()
{
   // Set the meter limits, if they've changed
   if (_currentProcData.inlet_pressure_max != (int)_stickMeter.get_positive_pressure_value_limit())
   {
      _stickMeter.set_positive_pressure_value_limit(_currentProcData.inlet_pressure_max);
   }

   if (_currentProcData.inlet_pressure_min != (int)_stickMeter.get_negative_pressure_value_limit())
   {
      _stickMeter.set_negative_pressure_value_limit(_currentProcData.inlet_pressure_min);
   }

   // Now set the meter value unconditionally. The meter will detect "no change" conditions
   // and avoid flicker.
   _stickMeter.set_meter_value(_currentProcData.pressure);
}

void Screen_DONCONN::initialize_status_line ()
{
   // Set the status to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (BLUE);

   // Remove any residual messages
   guiglobs::apheresis_status_line->set_current_text (textEmpty) ;
}

#if (CPU==SIMNT)
void Screen_DONCONN::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      DON_CONN_DISPLAY_TYPES donConnDisplayType = (DON_CONN_DISPLAY_TYPES)traverseInfo.parameters[0];

      bool                   diversion          = false, dontShowSeal = false;

      DataLog (log_level_sim_gui_side_info) << "Donor connect screen shows display type " << donConnDisplayType << endmsg;

      switch (donConnDisplayType)
      {
         case DON_CONN_DIVERSION_OFF : break;
         case DON_CONN_DIVERSION_ON : diversion        = true; break;
         case DON_CONN_DIVERSION_OFF_JP : dontShowSeal = true; break;
         case DON_CONN_DIVERSION_ON_JP : diversion     = true; dontShowSeal = true; break;
         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown Donnor Connection Display Type: " << donConnDisplayType << endmsg;
            return;
      }

      chooseDynamicTextAndGraphics(diversion, dontShowSeal);
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif

/* FORMAT HASH 270cb6e4e649a2b10d6730295bc24762 */
