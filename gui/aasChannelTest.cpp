/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasChannelTest.cpp
*
* $Header: H:/BCT_Development/Trima5.R/Trima/gui/rcs/aasChannelTest.cpp 1.20 2008/10/03 16:12:44Z dslausb Exp jheiusb $
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE
#include "aasChannelTest.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "guiglobs.hpp"
#include "cassette_mgr.hpp"

////////////////////////////////////////////////////////////////////////////////

Screen_AASCHANNELTEST::Screen_AASCHANNELTEST()
   : // init members base classes
     Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_CHANNEL_TEST),
     _txtAasChannelTest(textMiscChannelTest),
     _bmpChannelTest(365, 145, BITMAP_AAS_CHAN_TEST_WHITE_PAS_A),
     _animationTimer(ANIMATION_PERIOD, Callback<Screen_AASCHANNELTEST>(this, &Screen_AASCHANNELTEST::rotateGraphicsChannelTest), TimerMessage::DISARMED),
     _graphicA(BITMAP_NULL),
     _graphicB(BITMAP_NULL)
{}


////////////////////////////////////////////////////////////////////////////////
Screen_AASCHANNELTEST::~Screen_AASCHANNELTEST()
{
   Screen_AASCHANNELTEST::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASCHANNELTEST::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   guiglobs::aasRunInfo.attach(this);
   guiglobs::aasRunInfo.setMssSetupCompleteCallback(Callback<Screen_AASCHANNELTEST>(this, &Screen_AASCHANNELTEST::gotoConnect));

   // Tell proc that we clamped the channel and we're ready to go.
   MSSSetupMsg _setupMeteredStorage(MessageBase::SEND_LOCAL);
   _setupMeteredStorage.send(0);
   DataLog(log_level_gui_info) << "MSSSetupMsg sent" << endmsg;

   // now deallocate "standard" buttons that are not needed
   btn_continue.deallocate_resources();
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Figure out which flashing graphics to display, and display the first one.
   setupConditionalGraphic();

   // allocate the text holders  (NULL right now)
   _txtAasChannelTest.allocate_resources(*this);


   _msgAcClosureVerified.init(Callback<Screen_AASCHANNELTEST>(this, &Screen_AASCHANNELTEST::gotAcClosureVerifiedMsg));

   // Set up callbacks
   _animationTimer.armTimer(TimerMessage::ARMED);

   btn_cancel.set_string_id(textButtonEndAddition);

}



void Screen_AASCHANNELTEST::gotAcClosureVerifiedMsg ()
{
   DataLog (log_level_gui_info) << "GUI recived AC Closure Verified Message." << endmsg;
   goto_screen(GUI_SCREEN_AAS_TRANSITION_DUMMY, "GUI_SCREEN_AAS_TRANSITION_DUMMY");
}
////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASCHANNELTEST::deallocate_resources ()
{
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   guiglobs::aasRunInfo.detach(this);

   _txtAasChannelTest.deallocate_resources();
   _bmpChannelTest.deallocate_resources();

   _animationTimer.armTimer(TimerMessage::DISARMED);


   _msgAcClosureVerified.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASCHANNELTEST::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}

void Screen_AASCHANNELTEST::rotateGraphicsChannelTest ()
{
   if (_bmpChannelTest.get_id() == _graphicA)
      _bmpChannelTest.set_id(_graphicB);
   else _bmpChannelTest.set_id(_graphicA);
}

void Screen_AASCHANNELTEST::gotoConnect ()
{
   if (guiglobs::aasRunInfo.isRasProcedure())
   {
      DataLog (log_level_gui_info) << "RAS is required.  Transitioning to RAS connection screen." << endmsg;
      goto_screen(GUI_SCREEN_AAS_CONNECT_RAS, "GUI_SCREEN_AAS_CONNECT_RAS");
   }
   else if (guiglobs::aasRunInfo.isPasProcedure())
   {
      DataLog (log_level_gui_info) << "PAS is required.  Transitioning to PAS connection screen." << endmsg;
      goto_screen(GUI_SCREEN_AAS_CONNECT_PAS, "GUI_SCREEN_AAS_CONNECT_PAS");
   }
   else
   {
      DataLog (log_level_gui_error) << "ERROR: Procedure is neither RAS nor PAS. Send the END MSS message." << endmsg;
      MSSEndRunMsg _endRun(MessageBase::SEND_LOCAL);
      _endRun.send(0);
   }
}


void Screen_AASCHANNELTEST::setupConditionalGraphic ()
{
   _graphicA = BITMAP_NULL;
   _graphicB = BITMAP_NULL;

   bool isWhiteStamp = guiglobs::cassette_mgr->currentSetIsWhiteStamp();
   bool pasOn        = guiglobs::cassette_mgr->currentSetIsPas();
   bool rasOn        = guiglobs::cassette_mgr->currentSetIsRas() &&
                       guiglobs::aasRunInfo.isRasProcedure();

   if (isWhiteStamp)
   {
      if (pasOn)
      {
         if (rasOn)
         {
            _graphicA = BITMAP_AAS_CHAN_TEST_WHITE_BOTH_A;
            _graphicB = BITMAP_AAS_CHAN_TEST_WHITE_BOTH_B;
         }
         else
         {
            _graphicA = BITMAP_AAS_CHAN_TEST_WHITE_PAS_A;
            _graphicB = BITMAP_AAS_CHAN_TEST_WHITE_PAS_B;
         }
      }
      else
      {
         if (rasOn)
         {
            _graphicA = BITMAP_AAS_CHAN_TEST_WHITE_RAS_A;
            _graphicB = BITMAP_AAS_CHAN_TEST_WHITE_RAS_B;
         }
         else
         {
            DataLog (log_level_gui_error) << "ERROR: Cassette is neither RAS nor PAS." << endmsg;
            anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
            alarm.setAlarm();
            return;
         }
      }
   }
   else
   {
      _graphicA = BITMAP_AAS_CHAN_TEST_BLACK_A;
      _graphicB = BITMAP_AAS_CHAN_TEST_BLACK_B;
   }

   // If the graphics didn't get set, error.
   if ((_graphicA == BITMAP_NULL) ||
       (_graphicB == BITMAP_NULL))
   {
      DataLog (log_level_gui_error) << "ERROR: No bitmap that works with current set (stamp="
                                    << ((isWhiteStamp) ? "white" : "black") << "; pas=" << pasOn << "; ras=" << rasOn
                                    << ")" << endmsg;
      anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
      alarm.setAlarm();
      return;
   }
   else
   {
      _bmpChannelTest.set_id(_graphicA);
      _bmpChannelTest.allocate_resources(*this);
   }
}

void Screen_AASCHANNELTEST::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatDispTestInProgress);
}

/* FORMAT HASH e3601ed34fcd6d511e465d3a5b30a8e5 */
