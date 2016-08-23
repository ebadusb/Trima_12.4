/*
* Copyright (c) 2005 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasPrime.cpp
*
* $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/aasPrime.cpp 1.12 2008/10/01 16:06:02Z dslausb Exp dslausb $
*
*/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "aasPrime.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"


////////////////////////////////////////////////////////////////////////////////

Screen_AASPRIME::Screen_AASPRIME()
   : // init members base classes
     Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_PRIME),
     _animationTimer(ANIMATION_PERIOD, Callback<Screen_AASPRIME>(this, &Screen_AASPRIME::rotatePrimeGraphics), TimerMessage::DISARMED),
     _txtAasPrime(textMiscAasPrime),
     _bmpAasPrime(365, 145, BITMAP_NULL),
     _graphicA(BITMAP_NULL),
     _graphicB(BITMAP_NULL)

{}


////////////////////////////////////////////////////////////////////////////////
Screen_AASPRIME::~Screen_AASPRIME()
{
   // deallocate_resources is virtual, so be sure to call the right one!
   Screen_AASPRIME::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void Screen_AASPRIME::allocate_resources (const char* allocation_parameter)
{
   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources();
   btn_continue.deallocate_resources();      // no continue for prime thru addition
   btn_cancel.set_string_id(textButtonEndAddition);

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Figure out which flashing graphics to display, and display the first one.
   setupConditionalGraphic();

   // allocate the text holders  (NULL right now)
   _txtAasPrime.allocate_resources(*this);

   // Set up callbacks
   _animationTimer.armTimer(TimerMessage::ARMED);
   _msgVacPurgeComplete.init(Callback<Screen_AASPRIME>(this, &Screen_AASPRIME::showSplitIfNecessary));
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_AASPRIME::deallocate_resources ()
{
   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   _animationTimer.armTimer(TimerMessage::DISARMED);
   _msgVacPurgeComplete.deregisterMsg();

   // clean up the text lines
   _txtAasPrime.deallocate_resources();
   _bmpAasPrime.deallocate_resources();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////

void Screen_AASPRIME::process_cancel_button ()
{
   goto_child_screen (GUI_SCREEN_ABORTCNF, ABORT_TYPE_ADDITIVE_SOLUTIONS);
}

void Screen_AASPRIME::rotatePrimeGraphics ()
{
   if (_bmpAasPrime.get_id() == _graphicA)
      _bmpAasPrime.set_id(_graphicB);
   else _bmpAasPrime.set_id(_graphicA);
}

void Screen_AASPRIME::showSplitIfNecessary ()
{
   if (guiglobs::aasRunInfo.isPasProcedure())
   {
      if (guiglobs::cdsData.config.Procedure.Get().key_plt_mss_split_notif)
      {
         DataLog(log_level_gui_info) << "Transitioning to split screen." << endmsg;
         goto_screen(GUI_SCREEN_AAS_PAS_SPLIT, "GUI_SCREEN_AAS_PAS_SPLIT");
      }
      else
      {
         DataLog(log_level_gui_info) << "Sending PasReady message and transitioning to addition screen." << endmsg;

/*******************************************************************************************************************************
*            // dont send an automatic transition message if the pas split screen is not configured to come up                 *
*            // we use this time secretly to collect RG data for the pas addition.  the state has a timer for its transition.  *
*                                                                                                                              *
*            // MSSReadyPltAdditionMsg  _readyForPLT(MessageBase::SEND_GLOBAL);                                                *
*            // _readyForPLT.send(0);                                                                                          *
*******************************************************************************************************************************/

         goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
      }
   }
   else
   {
      DataLog(log_level_gui_info) << "Transitioning to addition screen." << endmsg;
      goto_screen(GUI_SCREEN_AAS_ADDITION, "GUI_SCREEN_AAS_ADDITION");
   }
}

void Screen_AASPRIME::setupConditionalGraphic ()
{
   _graphicA = BITMAP_NULL;
   _graphicB = BITMAP_NULL;

   bool stampIsWhite = guiglobs::cassette_mgr->currentSetIsWhiteStamp();


   bool pasOn =  guiglobs::cassette_mgr->currentSetIsPas()
                && guiglobs::aasRunInfo.isPasProcedure(); // IT 12062: filter out none PAS runs

   bool rasOn =  guiglobs::cassette_mgr->currentSetIsRas()
                && guiglobs::aasRunInfo.isRasProcedure(); // IT 11933: filters out CPS ras shutdowns etc.



   if (stampIsWhite)
   {
      if (pasOn && rasOn)
      {
         _graphicA = BITMAP_AAS_PRIME_WHITE_BOTH_A;
         _graphicB = BITMAP_AAS_PRIME_WHITE_BOTH_B;
      }

      if ((!pasOn) && rasOn)
      {
         _graphicA = BITMAP_AAS_PRIME_WHITE_RAS_A;
         _graphicB = BITMAP_AAS_PRIME_WHITE_RAS_B;
      }

      if (pasOn && (!rasOn))
      {
         _graphicA = BITMAP_AAS_PRIME_WHITE_PAS_A;
         _graphicB = BITMAP_AAS_PRIME_WHITE_PAS_B;
      }
   }
   else
   {
      _graphicA = BITMAP_AAS_PRIME_BLACK_A;
      _graphicB = BITMAP_AAS_PRIME_BLACK_B;
   }

   // If the graphics didn't get set, error.
   if ((_graphicA == BITMAP_NULL) ||
       (_graphicB == BITMAP_NULL))
   {
      DataLog (log_level_gui_error) << "ERROR: No bitmap that works with current set." << endmsg;
      anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
      alarm.setAlarm();
   }
   else
   {
      _bmpAasPrime.set_id(_graphicA);
      _bmpAasPrime.allocate_resources(*this);
   }
}

void Screen_AASPRIME::initialize_status_line ()
{
   guiglobs::apheresis_status_line->set_current_text (textStatAasActivePrime);
}

/* FORMAT HASH 6008510fde8e05120fe6ba100dc224fa */
