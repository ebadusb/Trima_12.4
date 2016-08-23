/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasConnectRas.cpp
*
*
* $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasConnectRas.cpp 1.27 2009/06/29 16:55:46Z dslausb Exp dslausb $
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "aasConnectRas.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "math.h"
#include "rasConnectConfirmBox.hpp"

// FOR OVERRIDE BUTTONS
#include "screen_config.hpp"

#if (CPU==SIMNT)
#include "guiUnitTestMessageDefs.h"
#endif

#define CONNNECT_RAS_REMINDER_DELAY 600000 // 600.0 seconds - 10 minutes


////////////////////////////////////////////////////////////////////////////////

Screen_AASCONNECTRAS::Screen_AASCONNECTRAS()
   : // init members base classes
     Base_Apheresis_Window           (MAGENTA, GUI_SCREEN_AAS_CONNECT_RAS),

     _txtPanel1Srbc(textMiscPanelLabelRasConnect),
     _txtPanel1Drbc(textMiscPanelLabelRasConnectDrbc),
     _txtPanel1DrbcOneBag(textMiscPanelLabelRasConnectDrbc1Bag),

     _txtPanel2Srbc(textMiscPanelLabelRasBagFrang),
     _txtPanel2Drbc(textMiscPanelLabelRasBagFrangDrbc),

     _txtPanel3Srbc(textMiscPanelLabelRasLineFrang),
     _txtPanel3Drbc(textMiscPanelLabelRasLineFrangDrbc),
     _txtPanel3DrbcOneBag(textMiscPanelLabelRasLineFrangDrbcOneBag),

     _txtConnectRasOnly              (textMiscConnectRasOnly),
     _txtNumBags                     (textMiscRasNumBags),

     _bmpConnectRas                  (15, 155, BITMAP_MSS_CONNECT_RBC),
     _bmpConnectRasDrbc              (15, 155, BITMAP_MSS_CONNECT_DRBC),
     _bmpConnectRasDrbcOneBag        (15, 155, BITMAP_MSS_CONNECT_DRBC_ONE_BAG),

     _connectionConfirmed(),
     _connectionCanceled(),
     _timerReminder(CONNNECT_RAS_REMINDER_DELAY, Callback<Screen_AASCONNECTRAS>(this, &Screen_AASCONNECTRAS::reminder_timer_handler), TimerMessage::DISARMED)


{ }


////////////////////////////////////////////////////////////////////////////////
Screen_AASCONNECTRAS::~Screen_AASCONNECTRAS()
{
   Screen_AASCONNECTRAS::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASCONNECTRAS::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   guiglobs::aasRunInfo.attach(this);
   guiglobs::aasRunInfo.setMssSetupCompleteCallback(Callback<Screen_AASCONNECTRAS>(this, &Screen_AASCONNECTRAS::procReadyForConnect));

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();

   // Don't allow the operator to continue until we know proc is ready.
   if (guiglobs::aasRunInfo.mssSetupComplete())
   {
      DataLog (log_level_gui_info) << "Proc is ready for connect." << endmsg;
      btn_continue.allocate_resources(*this);        // put continue button back
   }
   else btn_continue.deallocate_resources();

   btn_cancel.set_string_id(textButtonEndAddition);

   // Now link the status bar/line to this window
   link_status_bar_line();

   pickTextAndGraphics(setNumBags(), guiglobs::cassette_mgr->currentSetIsDrbc());

   _txtConnectRasOnly.allocate_resources(*this);
   _txtNumBags.allocate_resources(*this);

   // Start the reminder timer
   _timerReminder.armTimer(TimerMessage::ARMED);


}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASCONNECTRAS::deallocate_resources ()
{
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   destroyConfirmCallbacks();

   guiglobs::aasRunInfo.detach(this);

   // Dynamic text
   deallocateTextAndGraphics();

   // Static text
   _txtConnectRasOnly.deallocate_resources();
   _txtNumBags.deallocate_resources();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();

   // Shut off the reminder timer
   _timerReminder.armTimer(TimerMessage::DISARMED);

   // Stop the audible alert
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_CLEAR);



}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCONNECTRAS::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCONNECTRAS::process_continue_button ()
{
   callConfirmBox();
}   // END of Screen_MSSCLAMP PROCESS_CONTINUE_BUTTON


void Screen_AASCONNECTRAS::callConfirmBox ()
{
   btn_continue.deallocate_resources();

   createConfirmCallbacks();

   DataLog (log_level_gui_info) << "Putting up an RBC confirmation message." << endmsg;
   goto_child_screen(GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX, "RAS CONNECT CONFIRMATION BOX");
}

void Screen_AASCONNECTRAS::confirmCB ()
{
   DataLog (log_level_gui_info) << "Confirm Callback." << endmsg;
   destroyConfirmCallbacks();
   gotoNextScreen();
}

void Screen_AASCONNECTRAS::cancelCB ()
{
   DataLog (log_level_gui_info) << "Confirm Callback." << endmsg;
   destroyConfirmCallbacks();
   btn_continue.allocate_resources(*this);
}

void Screen_AASCONNECTRAS::createConfirmCallbacks ()
{
   DataLog (log_level_gui_info) << "Creating confirmation callbacks." << endmsg;
   _connectionConfirmed.init(Callback<Screen_AASCONNECTRAS>(this, &Screen_AASCONNECTRAS::confirmCB));
   _connectionCanceled.init(Callback<Screen_AASCONNECTRAS>(this, &Screen_AASCONNECTRAS::cancelCB));
}

void Screen_AASCONNECTRAS::destroyConfirmCallbacks ()
{
   _connectionConfirmed.deregisterMsg();
   _connectionCanceled.deregisterMsg();
}

void Screen_AASCONNECTRAS::gotoNextScreen ()
{
   if (guiglobs::aasRunInfo.isPasProcedure())
   {
      DataLog (log_level_gui_info) << "Leaving RAS connect. PAS connect must still be shown." << endmsg;
      RasConnectedPasToBeConnected rasConnectComplete(MessageBase::SEND_LOCAL);
      rasConnectComplete.send();
      goto_screen(GUI_SCREEN_AAS_CONNECT_PAS, "GUI_SCREEN_AAS_CONNECT_PAS");
   }
   else
   {
      DataLog (log_level_gui_info) << "Leaving RAS connect. PAS connect is unnecessary. Transition to Prime." << endmsg;
      MSSConnectedMsg connectionsCompleted(MessageBase::SEND_LOCAL);
      connectionsCompleted.send();
      goto_screen(GUI_SCREEN_AAS_PRIME, "GUI_SCREEN_AAS_PRIME");
   }
}

// Function allows arguments to specify set type in the simulator.
void Screen_AASCONNECTRAS::pickTextAndGraphics (int numBags, int setIsDrbc)
{
   DataLog (log_level_gui_info) << "Pick text and graphic: numBags="
                                << numBags << "; setIsDrbc=" << setIsDrbc << endmsg;

   deallocateTextAndGraphics();

   if (setIsDrbc)
   {
      if (numBags > 1)
      {
         _bmpConnectRasDrbc.allocate_resources(*this);
         _txtPanel1Drbc.allocate_resources(*this);
         _txtPanel2Drbc.allocate_resources(*this);
         _txtPanel3Drbc.allocate_resources(*this);
      }
      else
      {
         _bmpConnectRasDrbcOneBag.allocate_resources(*this);
         _txtPanel1DrbcOneBag.allocate_resources(*this);
         _txtPanel2Srbc.allocate_resources(*this);
         _txtPanel3DrbcOneBag.allocate_resources(*this);
      }
   }
   else
   {
      _bmpConnectRas.allocate_resources(*this);
      _txtPanel1Srbc.allocate_resources(*this);
      _txtPanel2Srbc.allocate_resources(*this);
      _txtPanel3Srbc.allocate_resources(*this);
   }
}


void Screen_AASCONNECTRAS::deallocateTextAndGraphics ()
{
   _txtPanel1Srbc.deallocate_resources();
   _txtPanel1Drbc.deallocate_resources();
   _txtPanel1DrbcOneBag.deallocate_resources();
   _txtPanel2Srbc.deallocate_resources();
   _txtPanel2Drbc.deallocate_resources();
   _txtPanel3Srbc.deallocate_resources();
   _txtPanel3Drbc.deallocate_resources();
   _txtPanel3DrbcOneBag.deallocate_resources();

   _bmpConnectRas.deallocate_resources();
   _bmpConnectRasDrbc.deallocate_resources();
   _bmpConnectRasDrbcOneBag.deallocate_resources();
}



// This is the function that actually updates the number of PAS bags needed in the text.
int Screen_AASCONNECTRAS::setNumBags ()
{
   int   numBags      = 0;
   float totalRasVol  = guiglobs::aasRunInfo.getRasNeeded();
   float singleBagVol = (guiglobs::cdsData.config.Procedure.Get().key_rbc_def_bag_vol * guiglobs::rasBagEmptyPercentage);

   if (singleBagVol > 0)                                  // Make sure we're not dividing by zero.
      numBags = (int)(ceil(totalRasVol / singleBagVol));  // Set the integer number of bags.

   // If we're here at all, we need at least one bag.
   if (numBags < 1) numBags = 1;

   DataLog (log_level_gui_info) << "We need " << numBags << " bags of RAS. (totalRasVol="
                                << totalRasVol << "; adjustedBagVol=" << singleBagVol << ")" << endmsg;

   setNumBags(numBags);

   return numBags;
}

int Screen_AASCONNECTRAS::setNumBags (int numBags)
{
   if (!guiglobs::setIntDataItem("dataItemNumRasBagsNeeded", numBags))
      _FATAL_ERROR(__FILE__, __LINE__, "Unable to access num ras bags variable");

   return numBags;
}

void Screen_AASCONNECTRAS::procReadyForConnect (void)
{
   DataLog (log_level_gui_info) << "Recieved proc ready for connect message." << endmsg;
   btn_continue.allocate_resources(*this);
}

void Screen_AASCONNECTRAS::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatConnectRas);
}


////////////////////////////////////////////////////////////////////////////////
// REMINDER_TIMER_HANDLER

void Screen_AASCONNECTRAS::reminder_timer_handler ()

{
   // Sound the reminder
   guiglobs::button_audio_feedback.create_feedback_sound (ATTENTION1_SOUND, SOUND_ONCE);

   // Start the timer again
   _timerReminder.armTimer(TimerMessage::ARMED);

}   // END of REMINDER_TIMER_HANDLER

////////////////////////////////////////////////////////////////////////////////

#if (CPU==SIMNT)

void Screen_AASCONNECTRAS::handleTraverseMessage ()
{
   TRAVERSE_INFO traverseInfo = _screenTraverseMessage->getData();

   if (traverseInfo.type == UNIVERSAL)
   {
      RAS_CONNECT_DISPLAY_TYPE rasType = (RAS_CONNECT_DISPLAY_TYPE)traverseInfo.parameters[0];

      switch (rasType)
      {
         case RAS_CONNECT_TYPE_TWO_BAG_SET_TWO_BAGS_NEEDED :
            DataLog (log_level_sim_gui_side_info) << "Recieved message calls for a two bag set with two bags of RAS." << endmsg;
            pickTextAndGraphics(2, 1);
            setNumBags(2);
            break;

         case RAS_CONNECT_TYPE_TWO_BAG_SET_ONE_BAG_NEEDED :
            DataLog (log_level_sim_gui_side_info) << "Recieved message calls for a two bag set with one bag of RAS." << endmsg;
            pickTextAndGraphics(1, 1);
            setNumBags(1);
            break;

         case RAS_CONNECT_TYPE_ONE_BAG_SET :
            DataLog (log_level_sim_gui_side_info) << "Recieved message calls for a one bag set." << endmsg;
            pickTextAndGraphics(1, 0);
            setNumBags(1);
            break;

         default :
            DataLog (log_level_sim_gui_side_error) << "Malformed message received.  Unknown RAS Connect Display Type: " << rasType << endmsg;
            return;
      }
   }
   else
   {
      DataLog (log_level_sim_gui_side_info) << "Passing traverse type " << traverseInfo.type << " to base class." << endmsg;
      // If we didn't get one of our messages, call the super's version to see if it can handle it.
      Base_Apheresis_Window::handleTraverseMessage();
   }
}
#endif // #if (CPU==SIMNT)

/* FORMAT HASH 8ab271c2e3482e1b9b2fac349e6097b2 */
