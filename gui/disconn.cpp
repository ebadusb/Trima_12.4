/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Screen_DISCONN

*     File name:   disconn.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "disconn.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "guiglobs.hpp"

void disconnectConfirmCycleCompleteCallback (void* data)
{
   ((Screen_DISCONN*)data)->confirmCycleCompleteCallback();
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_DISCONN::Screen_DISCONN()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_DISCONN),
     _bmpBgWithMss        (35, 142, BITMAP_AAS_DISCONN),
     _txtAasDisconPanel1  (textMiscAasDisconnect),
     _txtAasDisconPanel2  (textMiscSealDetachAc),
     _txtAasDisconPanel3  (textMiscRemoveAcLine),
     _txtDisconAcBag      (textMiscDisconnAcBag),
     _txtAasDisconDontSeal(textMiscAasDontSealProducts),
     _continueConfirmCycle(this,
                           disconnectConfirmCycleCompleteCallback,
                           Base_Apheresis_Window::btn_continue,
                           GUI_BUTTON_DISCONN_CONFIRM,
                           BUTNCONF_X,
                           BUTNCONF_Y),
     _disconnectPassed(),
     _msgAcClosureVerified()
{ }


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_DISCONN::~Screen_DISCONN()
{
   // If we haven't already deallocated, do it now.
   Screen_DISCONN::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_DISCONN::allocate_resources (const char* allocation_parameter)
{
   Base_Apheresis_Window::allocate_resources(allocation_parameter);

   _disconnectPassed.init(Callback<Screen_DISCONN>(this, &Screen_DISCONN::disconnectPassed));

   _msgAcClosureVerified.init(Callback<Screen_DISCONN>(this, &Screen_DISCONN::gotAcClosureVerifiedMsg));

   DataLog(log_level_gui_info) << "Sending TriggerSetRunTypeMsg Message" << endmsg;
   // Note that we no longer care about a response here, this message
   // just synchronizes with proc to make sure that some variables
   // get set prior to disconnect. Note: isAasProcedure() depends on this!
   TriggerSetRunTypeMsg trigger (MessageBase::SEND_GLOBAL);
   trigger.send();

   // Link the status bar/line to this window
   link_status_bar_line();

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources();
   btn_goback.deallocate_resources();
   btn_cancel.deallocate_resources();

   _bmpBgWithMss.allocate_resources(*this);

   _txtAasDisconPanel1.allocate_resources(*this);
   _txtAasDisconPanel2.allocate_resources(*this);
   _txtAasDisconPanel3.allocate_resources(*this);
   _txtDisconAcBag.allocate_resources(*this);
   setUpTextAndGraphics(isAasProcedure());

   // Set the status bar to reflect that everything is completed except disconnect
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_INFO);
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_LOAD);
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_PREPARE);  // RUNPROC in 2.2 doesn't set this.
   guiglobs::apheresis_status_bar->set_phase_completed(STATUS_BAR_DONATE);

   guiglobs::apheresis_status_bar->set_phase_active(STATUS_BAR_UNLOAD);

    #if (CPU==SIMNT) // So that the status bar is correct in the simulator, no matter where we came from.
   guiglobs::apheresis_status_bar->set_phase(STATUS_BAR_UNLOAD, STATUS_BAR_STATE_RESET);
   guiglobs::apheresis_status_bar->set_phase(STATUS_BAR_SOLUTIONS, STATUS_BAR_STATE_RESET, true);
    #endif

   _continueConfirmCycle.allocate_resources();

} // END of Screen_DISCONN ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_DISCONN::deallocate_resources ()
{
   _continueConfirmCycle.deallocate_resources();

   _msgAcClosureVerified.deregisterMsg();
   _disconnectPassed.deregisterMsg();

   // deallocate all constituent objects first
   //  (if they're already deallocated, it's a NOOP)
   _bmpBgWithMss.deallocate_resources();

   _txtAasDisconPanel1.deallocate_resources();
   _txtAasDisconPanel2.deallocate_resources();
   _txtAasDisconPanel3.deallocate_resources();
   _txtDisconAcBag.deallocate_resources();
   _txtAasDisconDontSeal.deallocate_resources();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONFIRM_DISCONN_BUTTON
void Screen_DISCONN::confirmCycleCompleteCallback ()
{
   _continueConfirmCycle.deallocate_resources();
   DataLog (log_level_gui_info) << "Operator confirms donor is disconnected." << endmsg;

   // Send out the donor disconnected message here.
   DonorDisconnectedMsg DonorDisconnected(MessageBase::SEND_GLOBAL);
   DonorDisconnected.send(0);
}  // END of Screen_DISCONN PROCESS_CONFIRM_DISCONN_BUTTON

void Screen_DISCONN::setUpTextAndGraphics (bool isMss)
{
   if (isMss) _txtAasDisconDontSeal.allocate_resources(*this);
   else _txtAasDisconDontSeal.deallocate_resources();
}

bool Screen_DISCONN::isAasProcedure ()
{
    #if CPU==SIMNT
   return true;
    #else
   return guiglobs::aasRunInfo.isPasProcedure() ||
          (guiglobs::aasRunInfo.isRasProcedure() &&
           !guiglobs::aasRunInfo.rasBlockedDueToCpsAlarms());
    #endif
}

void Screen_DISCONN::disconnectPassed ()
{
   if (isAasProcedure())
   {
      static bool loggedOnce = false;
      if (!loggedOnce)
      {
         loggedOnce = true;
         DataLog (log_level_gui_info) << "Disconnect test passed. MSS to follow." << endmsg;
      }

   }
   else
   {
      DataLog (log_level_gui_info) << "Disconnect test passed." << endmsg;
      goto_screen(GUI_SCREEN_SEAL_AND_DETACH, "GUI_SCREEN_SEAL_AND_DETACH");
   }
}

void Screen_DISCONN::gotAcClosureVerifiedMsg ()
{
   DataLog (log_level_gui_info) << "GUI received AC Closure Verified Message." << endmsg;
   goto_screen(GUI_SCREEN_AAS_TRANSITION_DUMMY, "GUI_SCREEN_AAS_TRANSITION_DUMMY");
}

void Screen_DISCONN::initialize_status_line ()
{
   // Remove any residual messages from ALL status line priority levels
   //  so that odd "exit run" paths will have any status line residual
   //  text cleaned-up.
   guiglobs::apheresis_status_line->reset_status_line ();

   // Set the default status line to reflect the disposable load color
   guiglobs::apheresis_status_line->set_color (MAGENTA);
}

/* FORMAT HASH e70b8082a45c48c281f21b648dbe5b22 */
