/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaricianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SEAL_AND_DETACH

*     File name:   sealAndDetach.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

***************************************************************************/

#include <vxworks.h>

// BASE CLASS INCLUDE

#include "sealAndDetach.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Screen_SEAL_AND_DETACH::Screen_SEAL_AND_DETACH()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_SEAL_AND_DETACH),

     _bmpSealProducts      (130, 160, BITMAP_PLACARD_POST_DISCONN_SEAL),
     _bmpRemoveDisposables (130, 160, BITMAP_PLACARD_POST_DISCONN_REMOVE),

     _txtSealProducts        (textMiscInstructionSealDetachProd),
     _txtRemoveDisposables   (textMiscInstructionRemoveTubingSet),

     _sealDetachState(SEAL)
{ }


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Screen_SEAL_AND_DETACH::~Screen_SEAL_AND_DETACH()
{
   // If we haven't already deallocated, do it now.
   Screen_SEAL_AND_DETACH::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES ()

void Screen_SEAL_AND_DETACH::allocate_resources (const char* allocation_parameter)
{
   Base_Apheresis_Window::allocate_resources (allocation_parameter);

   // Link the status bar/line to this window
   link_status_bar_line();

   // now deallocate "standard" buttons that are not needed
   btn_lowleft.deallocate_resources ();
   btn_goback.deallocate_resources ();
   btn_cancel.deallocate_resources ();

   _txtSealProducts.allocate_resources(*this);

   // Determine whether or not we are an MSS procedure, because the appearance
   // will be different based on that. Do this last to give proc some time to
   // decide one way or the other.
   transitionToState(SEAL);
} // END of Screen_SEAL_AND_DETACH ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SEAL_AND_DETACH::deallocate_resources ()
{
   guiglobs::aasRunInfo.detach(this);

   // deallocate all constituent objects first
   //  (if they're already deallocated, it's a NOOP)
   _bmpSealProducts.deallocate_resources ();
   _bmpRemoveDisposables.deallocate_resources ();

   _txtSealProducts.deallocate_resources ();
   _txtRemoveDisposables.deallocate_resources ();

   // Now unparent the status bar/line from this window
   unlink_status_bar_line();

   // Delete the screen event notification object
   _msgDisconnectConfirmed.deregisterMsg();

   // deallocate parent's resources
   Base_Apheresis_Window::deallocate_resources ();
}  // END of Screen_SEAL_AND_DETACH DEALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// PROCESS_CONTINUE_BUTTON
void Screen_SEAL_AND_DETACH::process_continue_button ()
{
   switch (_sealDetachState)
   {
      case SEAL :

         btn_continue.deallocate_resources();

         DataLog(log_level_gui_info) << "Continue pressed. Sending TellProcToTransitionFromDisconnect Message" << endmsg;

         _msgDisconnectConfirmed.init(Callback<Screen_SEAL_AND_DETACH>(this, &Screen_SEAL_AND_DETACH::cassetteRaised));

         {
            TellProcToTransitionFromDisconnect transitionToUnload(MessageBase::SEND_GLOBAL);
            transitionToUnload.send(0);
         }

         break;

      case DETACH :
         btn_continue.deallocate_resources();

         _msgDisconnectConfirmed.deregisterMsg();

         DataLog(log_level_gui_info) << "Continue pressed. Sending DisconnectCompleteMsg Message" << endmsg;

         {
            DisconnectCompleteMsg DisconnectComplete(MessageBase::SEND_GLOBAL);
            DisconnectComplete.send(0);
         }

         break;
   }
   // Send out Disconnect Complete message (this should prompt the transition to the
   //  POST_RUN system state).
}  // END of Screen_SEAL_AND_DETACH PROCESS_CONTINUE_BUTTON


void Screen_SEAL_AND_DETACH::cassetteRaised ()
{
   // Process the Procedure-confirmed, donor disconnected message
   // This message is only valid in the DISCONNECT CONFIRM state,
   //  otherwise ignore it.

   btn_continue.allocate_resources(*this);

   DataLog (log_level_gui_info) << "Cassette raise successful." << endmsg;

   transitionToState(DETACH);
}  // END of Screen_DISCONN SCREEN_NOTIFY_HANDLER



void Screen_SEAL_AND_DETACH::transitionToState (SEAL_DETACH_STATE newState)
{
   _sealDetachState = newState;

   switch (_sealDetachState)
   {
      case SEAL :

         _bmpSealProducts.allocate_resources(*this);
         _txtSealProducts.set_color(YELLOW);
         guiglobs::apheresis_status_line->set_current_text(textStatSealAndDetach);

         _bmpRemoveDisposables.deallocate_resources();
         _txtRemoveDisposables.deallocate_resources();

         break;

      case DETACH :

         _bmpRemoveDisposables.allocate_resources(*this);
         _txtRemoveDisposables.allocate_resources(*this);
         _txtSealProducts.set_color(GREY_LIGHT);

         _bmpSealProducts.deallocate_resources();

         break;
   }
}

/* FORMAT HASH 1dfab39fcf6b289bca7ebf7b3ce70050 */
