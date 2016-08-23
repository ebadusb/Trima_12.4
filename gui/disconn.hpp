/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_DISCONN

*     File name:    disconn.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

        This module creates and displays the donor disconnect screen.  It directs
        the operator to seal and detach the products; disconnect the donor; and
        remove and dispose of the blood tubing set.  It communicates with the
        Procedure by way of the operator's pressing of the continue key.  Each
        pressing of the key causes the current graphic to grey out and the subsequent
        graphic to appear.  The pressing of the continue key during the donor
        disconnect causes a "confirmation button" to appear which only stays
        visible for n seconds.  If the operator doesn't press the button within
        this time period, the confirmation key is removed from the screen.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef DISCONN_HPP
#define DISCONN_HPP



// CLASS INCLUDE


#include "aphwind.hpp"
#include "contConfCyc.hpp"
#include "trimamessages.h"

// CLASS DECLARES


class Screen_DISCONN
   : public Base_Apheresis_Window
{

private:

   Bitmap       _bmpBgWithMss; // The MSS version of the disconnect graphic

   Display_Text _txtAasDisconPanel1,
                _txtAasDisconPanel2,
                _txtAasDisconPanel3,
                _txtDisconAcBag,
                _txtAasDisconDontSeal;      // Tell the operator not to seal product lines (mss only)

   ContinueConfirmCycle _continueConfirmCycle;

   // Set up text and graphics
   void setUpTextAndGraphics (bool isMss);

   TellGuiThatDisconnectPassed _disconnectPassed;
   VerifyAcClosureCompleteMsg  _msgAcClosureVerified;

   void disconnectPassed ();
   bool isAasProcedure ();

public:

   // CONSTRUCTOR
   Screen_DISCONN();


   // DESTRUCTOR
   virtual ~Screen_DISCONN();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // CONFIRM_CYCLE_COMPLETE_CALLBACK
   // This method will send the donor disconnected message to procedure and wait
   // to receive a system state change to POST_RUN. Once it receives this message,
   // it will grey-out the disconnect graphic, display the remove graphic,
   // remove the confirm_disconn button and change the disconn state to DSC_STATE_UNLOAD.
   void confirmCycleCompleteCallback ();

   void gotAcClosureVerifiedMsg ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

}; // END of Screen_DISCONN Class Declaration

#endif /* ifndef DISCONN_HPP */

/* FORMAT HASH 05b88e4e047f59972cc09356d129d6c8 */
