/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_SYSTEST

*     File name:   systest.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************
****************************************************************************/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "systest.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"
#include "software_cds.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SYSTEST::Screen_SYSTEST()
   : Base_Apheresis_Window(TEAL, GUI_SCREEN_SYSTEST),

     _bmpDispTstA (334, 150, BITMAP_PLACARD_DISP_TESTA),
     _bmpDispTstB (334, 150, BITMAP_PLACARD_DISP_TESTB),

     _txtDispTst (textMiscTubingSetTest),
     _txtDontConnectAc (textMiscSysDispDontConnectAc),

     _bitmapState(BITMAP_STATE_A),
     _a2donorMitigation(guiglobs::airToDonorMitigationEnabled()),

     _animationTimer(ANIMATION_PERIOD, Callback<Screen_SYSTEST>(this, &Screen_SYSTEST::animateGraphics), TimerMessage::DISARMED),
     _halfSecondDelay(COMPLETION_NOTICE_PERIOD, Callback<Screen_SYSTEST>(this, &Screen_SYSTEST::delayCallback), TimerMessage::DISARMED)
{}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SYSTEST::~Screen_SYSTEST()
{
   Screen_SYSTEST::deallocate_resources();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_SYSTEST::allocate_resources (const char* allocation_parameter)
{
   // refresh cached feature
   _a2donorMitigation = guiglobs::airToDonorMitigationEnabled();

   DataLog (log_level_gui_info) << "ARM=" << _a2donorMitigation
                                << " JapanWF=" << Software_CDS::isJapaneseWorkflow()
                                << endmsg;

   // allocate parent's resources first
   Base_Apheresis_Window::allocate_resources (allocation_parameter);


   // then allocate constituent objects with this window as parent widget
   //  layer allocation from Z-back to Z-front.
   _bmpDispTstA.allocate_resources(*this) ;
   _bmpDispTstB.allocate_resources(*this) ;
   _txtDispTst.allocate_resources(*this) ;

   if (Software_CDS::isJapaneseWorkflow())
      _txtDontConnectAc.allocate_resources(*this);
   else
      _txtDontConnectAc.deallocate_resources();

   // now deallocate all of the "standard" buttons (none are needed)
   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();
   btn_continue.deallocate_resources ();
   btn_lowleft.deallocate_resources ();

   // Now link the status bar/line to this window
   link_status_bar_line();

   // Set the status bar to reflect load in progress
   guiglobs::apheresis_status_bar->set_phase_active (STATUS_BAR_LOAD);

   // Set the "B" bitmap to defer realization (via the window realization)
   //  until realized explicitly itself. This is to prevent both A and B
   //  bitmaps from being realized on top of each other at window realization.
   _bmpDispTstA.set_visible(true);
   _bmpDispTstB.set_visible(false);

   // Initialize received messages
   _msgTestCompleted.init(Callback<Screen_SYSTEST>(this, &Screen_SYSTEST::testCompleted), MessageBase::SNDRCV_RECEIVE_ONLY);
   _msgShowClampCrossover.init(Callback<Screen_SYSTEST>(this, &Screen_SYSTEST::showClampCrossover), MessageBase::SNDRCV_RECEIVE_ONLY);

   _msgShowClampFromARM.init(Callback<Screen_SYSTEST>(this, &Screen_SYSTEST::showClampDisplay), MessageBase::SNDRCV_RECEIVE_ONLY);


   // Create the animation timer that will drive the alternate display
   //  of the machine test.
   _animationTimer.armTimer(TimerMessage::ARMED);

   // Set the half second timer pointer to NULL to prevent dual allocations
   //  if multiple test completed messages occur.
}   // END of Screen_SYSTEST ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Screen_SYSTEST::deallocate_resources ()
{
   // deallocate all constituent objects first
   _bmpDispTstA.deallocate_resources() ;
   _bmpDispTstB.deallocate_resources() ;
   _txtDispTst.deallocate_resources() ;
   _txtDontConnectAc.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Remove the timers at now, if, for some reason,
   //  they haven't been killed during the processing.
   _halfSecondDelay.armTimer(TimerMessage::DISARMED);
   _animationTimer.armTimer(TimerMessage::DISARMED);

   _msgTestCompleted.deregisterMsg();
   _msgShowClampCrossover.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}    // END of Screen_SYSTEST DEALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DISPLAY_ALTERNATE_SYSTEST_GRAPHICS

void Screen_SYSTEST::animateGraphics ()
{
   if (_bitmapState == BITMAP_STATE_A)
   {
      _bmpDispTstB.set_visible(true);
      _bmpDispTstA.set_visible(false);

      // indicate the "B" state is in effect
      _bitmapState = BITMAP_STATE_B;

   }
   else
   {
      _bmpDispTstA.set_visible(true);
      _bmpDispTstB.set_visible(false);

      // indicate the "B" state is in effect
      _bitmapState = BITMAP_STATE_A;
   }

}    // END of Screen_SYSTEST DISPLAY_ALTERNATE_SYSTEST_GRAPHICS



////////////////////////////////////////////////////////////////////////////////
// SCREEN_NOTIFY_HANDLER

void Screen_SYSTEST::testCompleted ()
{

   if (!_a2donorMitigation)
   {
      DataLog (log_level_gui_info) << "ARM off disable PLT clamp pause" << endmsg;
      // not ARM so short circuit screen pause in the state machine
      FinishedClampFromARMMsg FinishShowClamp(MessageBase::SEND_LOCAL);
      FinishShowClamp.send(0);
   }

   _animationTimer.armTimer(TimerMessage::DISARMED);

   // Notify the operator of test completion by placing message on the
   //  status line.
   guiglobs::apheresis_status_line->set_current_text (textStatDispTestComplete) ;


   // Create a delay before acknowledging test completion.
   //  Let the timer manager actually transmit the end of test indication.
   _halfSecondDelay.armTimer(TimerMessage::ARMED);

}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void Screen_SYSTEST::showClampCrossover ()
{
   // Goto the clamp crossover screen.  We'll come back here when we're done.
   goto_screen(GUI_SCREEN_CLAMP_CROSSOVER, "GUI_SCREEN_CLAMP_CROSSOVER");
}


void Screen_SYSTEST::showClampDisplay ()
{
   DataLog (log_level_gui_info) << " ShowClampFromARMmsg received" << endmsg;

   const bool airRemoval = guiglobs::cdsData.config.Procedure.Get().key_air_removal;

   // When air removal is on (which implies ARM is ON), then check for any
   // final clamping instructions since the evacuation has completed
   //
   if ( airRemoval && guiglobs::shouldShowClampPlasmaAirBag() )
   {
      DataLog (log_level_gui_info) << "AirEvac on: shouldShowClampPlasmaAirBag" << endmsg;
      showClampPlasmaAirBag();
   }
   else if ( airRemoval && guiglobs::shouldShowClampPas() )
   {
      DataLog (log_level_gui_info) << "AirEvac on: shouldShowClampPas" << endmsg;
      showClampPLTbag();
   }
   else if ( airRemoval && guiglobs::shouldShowRbcSplitInit() )
   {
      DataLog (log_level_gui_info) << "AirEvac on: shouldShowRbcSplitInit" << endmsg;
      showClampRBCbag();
   }
   else
   {
      // no screens to show jump to finish clamps
      DataLog (log_level_gui_info) << "Sending FinishedClampFromARMMsg message" << endmsg;
      FinishedClampFromARMMsg FinishShowClamp(MessageBase::SEND_LOCAL);
      FinishShowClamp.send(0);
   }

}

void Screen_SYSTEST::showClampPlasmaAirBag ()
{
   DataLog (log_level_gui_info) << "Show Clamp Plasma Air Bag screen" << endmsg;
   goto_screen(GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG, "GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG");

}

void Screen_SYSTEST::showClampPLTbag ()
{
   DataLog (log_level_gui_info) << "Show PAS clamp screen" << endmsg;
   goto_screen(GUI_SCREEN_PAS_INIT_SPLIT, "GUI_SCREEN_PAS_INIT_SPLIT");

}

void Screen_SYSTEST::showClampRBCbag ()
{
   DataLog (log_level_gui_info) << "Show dRBC clamp screen" << endmsg;
   goto_screen(GUI_SCREEN_RBC_SPLIT_INIT, "GUI_SCREEN_RBC_SPLIT_INIT");

}

////////////////////////////////////////////////////////////////////////////////
// SCREEN_TIMER_HANDLER

void Screen_SYSTEST::delayCallback ()
{
   // Stop the halfsec timer so it can't pop again before
   // SYSACATT is called. (deallocate will remove the timer object).
   _halfSecondDelay.armTimer(TimerMessage::DISARMED);

   // Send Procedure a message confirming that from the GUI
   // perspective the test is completed.
   AckTestCompletedMsg complete(MessageBase::SEND_GLOBAL);
   complete.send(0);
}

void Screen_SYSTEST::initialize_status_line ()
{
   // Set the status line to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (TEAL);

   // Set the in-progress message
   guiglobs::apheresis_status_line->set_current_text (textStatDispTestInProgress) ;
}

/* FORMAT HASH f7ef0b683a27c78bc0c49e0a69af0951 */
