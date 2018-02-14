/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SYSACATT

*     File name:   sysacatt.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
*/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "sysacatt.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"
#include "HALSTATUS_CDS.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSACATT::Screen_SYSACATT()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSACATT),

     _bmpAcAttach (PLAC_ACATT_X,
                   PLAC_ACATT_Y,
                   BITMAP_PLACARD_AC_ATTACH),
     _bmpAcLuerAttach (PLAC_ACATT_X - 30,
                       PLAC_ACATT_Y,
                       BITMAP_PLACARD_AC_LUER_ATTACH),
     _bmpSensorImage (418, 156, BITMAP_AC_ATTACH_SENSOR),
     _bmpLuerSensorImage (476, 156, BITMAP_AC_ATTACH_LUER_SENSOR),

     _txtAcConnect (textMiscInstructionDispConnectAc),
     _txtAcSqueeze (textMiscInstructionDispSqueezeAc),
     _txtAcLoad    (textMiscInstructionDispLoadAc),
     _txtAcBag1    (textMiscAcConnBag1),
     _txtAcBag2    (textMiscAcConnBag2),

     _txtAcLuerConnect (textMiscInstructionDispConnectAcLuer),
     _txtAcLuerBreak   (textMiscInstructionDispBreakAcLuer),
     _txtAcLuerSqueeze (textMiscInstructionDispSqueezeAcLuer),
     _txtAcLuerLoad    (textMiscInstructionDispLoadAcLuer),
     _txtAcLuerBag1    (textMiscAcLuerConnBag1),
     _txtAcLuerBag2    (textMiscAcLuerConnBag2),
     _txtAcLuerBag3    (textMiscAcLuerConnBag3),

     _txtNoStorSol (textMiscConnectAcOnly),
     _acImageFlipped (false)
{}   // END of Screen_SYSACATT CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SYSACATT::~Screen_SYSACATT()
{
   Screen_SYSACATT::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_SYSACATT::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // then allocate constituent objects with this window as parent widget
   // layer allocation from Z-back to Z-front.

   updateScreedData(guiglobs::cassette_mgr->currentSetHasAcLuer());

   _txtNoStorSol.allocate_resources(*this);

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources ();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);
   //Set the blinking period to 1.5 second
   _acBlinkingTimer.init(1500, Callback<Screen_SYSACATT>(this, &Screen_SYSACATT::TimeoutHandler), TimerMessage::ARMED);
   TimeoutHandler();
}   // END of Screen_SYSACATT ALLOCATE_RESOURCES

void Screen_SYSACATT::updateScreedData (bool hasAcLuer)
{
   // set the elements that depend on luer vs. spike connector
   _txtAcLoad.deallocate_resources();
   _txtAcSqueeze.deallocate_resources();
   _txtAcConnect.deallocate_resources();
   _txtAcBag1.deallocate_resources();
   _txtAcBag2.deallocate_resources();

   _txtAcLuerBreak.deallocate_resources();
   _txtAcLuerConnect.deallocate_resources();
   _txtAcLuerSqueeze.deallocate_resources();
   _txtAcLuerLoad.deallocate_resources();
   _txtAcLuerBag1.deallocate_resources();
   _txtAcLuerBag2.deallocate_resources();
   _txtAcLuerBag3.deallocate_resources();

   _bmpAcLuerAttach.deallocate_resources();
   _bmpAcAttach.deallocate_resources();
   _bmpSensorImage.deallocate_resources();
   _bmpLuerSensorImage.deallocate_resources();

   if ( hasAcLuer)
   {
      // AC bag has luer connector w. frangible
      DataLog (log_level_gui_info) << "Kit has AC bag with luer & frangible." << endmsg;
      _bmpAcLuerAttach.allocate_resources(*this);
      _bmpLuerSensorImage.allocate_resources(*this);
      _txtAcLuerBreak.allocate_resources(*this);
      _txtAcLuerConnect.allocate_resources(*this);
      _txtAcLuerSqueeze.allocate_resources(*this);
      _txtAcLuerLoad.allocate_resources(*this);
      _txtAcLuerBag1.allocate_resources(*this);
      _txtAcLuerBag2.allocate_resources(*this);
      _txtAcLuerBag3.allocate_resources(*this);
   }
   else
   {
      // AC bag uses a spike connector
      DataLog (log_level_gui_info) << "Kit has AC bag with spike." << endmsg;
      _bmpAcAttach.allocate_resources(*this);
      _bmpSensorImage.allocate_resources(*this);
      _txtAcConnect.allocate_resources(*this);
      _txtAcSqueeze.allocate_resources(*this);
      _txtAcLoad.allocate_resources(*this);
      _txtAcBag1.allocate_resources(*this);
      _txtAcBag2.allocate_resources(*this);
   }
}

////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_SYSACATT::deallocate_resources ()
{
   destroyConfirmCallbacks();

   // deallocate all constituent objects first
   _txtAcLoad.deallocate_resources();
   _txtAcSqueeze.deallocate_resources();
   _txtAcConnect.deallocate_resources();
   _txtNoStorSol.deallocate_resources();
   _txtAcBag1.deallocate_resources();
   _txtAcBag2.deallocate_resources();

   _txtAcLuerBreak.deallocate_resources();
   _txtAcLuerConnect.deallocate_resources();
   _txtAcLuerSqueeze.deallocate_resources();
   _txtAcLuerLoad.deallocate_resources();
   _txtAcLuerBag1.deallocate_resources();
   _txtAcLuerBag2.deallocate_resources();
   _txtAcLuerBag3.deallocate_resources();

   _bmpAcLuerAttach.deallocate_resources();
   _bmpAcAttach.deallocate_resources();
   _bmpSensorImage.deallocate_resources();
   _bmpLuerSensorImage.deallocate_resources();
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
   _acBlinkingTimer.armTimer(TimerMessage::DISARMED);

}     // END of Screen_SYSACATT DEALLOCATE_RESOURCES


void Screen_SYSACATT::createConfirmCallbacks ()
{
   _msgAcConnectConfirmed.init(Callback<Screen_SYSACATT>(this, &Screen_SYSACATT::confirmCB));
   _msgAcConnectCanceled.init(Callback<Screen_SYSACATT>(this, &Screen_SYSACATT::cancelCB));
}

void Screen_SYSACATT::destroyConfirmCallbacks ()
{
   _msgAcConnectConfirmed.deregisterMsg();
   _msgAcConnectCanceled.deregisterMsg();
}

void Screen_SYSACATT::callConfirmBox ()
{
   btn_continue.deallocate_resources();

   createConfirmCallbacks();

   DataLog (log_level_gui_info) << "Putting up an AC confirmation message." << endmsg;
   goto_child_screen(GUI_SCREEN_AC_CONNECT_CONFIRM_BOX, "AC CONNECT CONFIRMATION BOX");
}

void Screen_SYSACATT::confirmCB ()
{
   DataLog (log_level_gui_info) << "AC Confirm Callback." << endmsg;
   destroyConfirmCallbacks();
   confirmAcAttached();
}

void Screen_SYSACATT::cancelCB ()
{
   DataLog (log_level_gui_info) << "AC Cancel Callback." << endmsg;
   destroyConfirmCallbacks();
   btn_continue.allocate_resources(*this);
}



////////////////////////////////////////////////////////////////////////////////
// PROCESS_CANCEL_BUTTON

void Screen_SYSACATT::process_cancel_button ()
{

   // Call aphwind's default abort confirm messaging
   Base_Apheresis_Window::process_cancel_button();

}    // END of Screen_SYSACATT PROCESS_CANCEL_BUTTON


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON

void Screen_SYSACATT::process_continue_button ()
{
   if ( Software_CDS::GetInstance().getFeature(ConfirmAcConnection) )
   {
      callConfirmBox();
   }
   else
   {
      confirmAcAttached();
   }
}

////////////////////////////////////////////////////////////////////////////////
// confirmAcAttached

void Screen_SYSACATT::confirmAcAttached ()
{

   // Disable all the standard buttons on this screen
   disable_standard_buttons();

   // Display a status line message that AC is priming
   guiglobs::apheresis_status_line->set_current_status_line_text_by_type
      (ATTENTION_STATUS_LINE, textStatAcPriming);

   // Notify Procedure that the AC is attached and then wait for
   // a system state change to AC Prime to deallocate.
   ACConnectedMsg connected(MessageBase::SEND_GLOBAL);
   connected.send(0);

}    // END of PROCESS_CONTINUE_BUTTON

void Screen_SYSACATT::initialize_status_line ()
{
   // Remove any residual messages
   //  Set the status line to reflect the disposable load color
   guiglobs::apheresis_status_line->reset_status_line ();
   guiglobs::apheresis_status_line->set_color (TEAL);
}

#if CPU==SIMNT
void Screen_SYSACATT::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == SET_SHOW_AC_ATTACH_CONFIRM)
   {
      int parameter = (int)traverseInfo.parameters[0];
      if ( parameter == 1 )
      {
         guiglobs::cdsData.config.SWOptionSet("confirm_ac_connection", true);
         updateScreedData(false);
         // _showConfirmBox has been eliminated; uses getFeature(ConfirmAcConnection) directly
      }
      if ( parameter == 2 )
      {
         guiglobs::cdsData.config.SWOptionSet("confirm_ac_connection", false);
         updateScreedData(true);
      }

      DataLog(log_level_sim_gui_side_info) << "Screen_SYSACATT parameter is " << parameter << endmsg;
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif /* if CPU==SIMNT */

void Screen_SYSACATT::TimeoutHandler ()
{
   HalStatus_CDS status(ROLE_RO);
   //If AC has AC leur then use leur sensor image
   if (guiglobs::cassette_mgr->currentSetHasAcLuer())
   {
      //If fluid is detected then set the green image else either use orange image or yellow image
      if(status.ACDetectFluid())
      {
         _bmpLuerSensorImage.set_id(BITMAP_AC_ATTACH_LUER_SENSOR_GREEN);
      }
      else
      {
         if (_acImageFlipped)
         {
            _bmpLuerSensorImage.set_id(BITMAP_AC_ATTACH_LUER_SENSOR);
         }
         else
         {
            _bmpLuerSensorImage.set_id(BITMAP_AC_ATTACH_LUER_SENSOR_ORANGE);
         }
      }
   }
   else
   {
      //If fluid is detected then set the green image else either use orange image or yellow image
      if (status.ACDetectFluid())
      {
         _bmpSensorImage.set_id(BITMAP_AC_ATTACH_SENSOR_GREEN);
      }
      else
      {
         if (_acImageFlipped)
         {
            _bmpSensorImage.set_id(BITMAP_AC_ATTACH_SENSOR_ORANGE);
         }
         else
         {
            _bmpSensorImage.set_id(BITMAP_AC_ATTACH_SENSOR);
         }
      }
   }
   _acImageFlipped = !_acImageFlipped;
}

/* FORMAT HASH 7fada088e5869d501b4cf5e9e5bc622d */
