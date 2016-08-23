/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 GambroBCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_SEAL_AND_DETACH

*     File name:    sealAndDetach.hpp

*     Contents:     See prototype header descriptions below...

*	Description:
    This is the screen that follows the donor disconnect screen for a
*   non-AAS procedure.  It instructs the operator to seal the product
    lines, the cassette raises, and the screen then instructs the
*	operator to unload the tubing set.

*************************< FOCUSSED GUI DOMAIN >***************************
***************************************************************************/

// Single include define guardian
#ifndef SEAL_AND_DETACH_HPP__
#define SEAL_AND_DETACH_HPP__

// CLASS INCLUDE

#include <vxWorks.h> // Needed for CPU compiler directives
#include "aphwind.hpp"
#include "aphcalbk.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "trimamessages.h"

// CLASS DECLARES


class Screen_SEAL_AND_DETACH
   : public Base_Apheresis_Window
{

public:

   enum SEAL_DETACH_STATE { SEAL, DETACH };

private:

   SEAL_DETACH_STATE _sealDetachState;

   Bitmap            _bmpSealProducts,      // The disconnect donor bitmap
                     _bmpRemoveDisposables; // The remove tubing set bitmap

   Display_Text _txtSealProducts,        // Labeling for the disconnect bitmap
                _txtRemoveDisposables;   // Labeling for the seal detach bitmap

   // Confirm donor disconnect message object
   ConfirmDonorDisconnectedMsg _msgDisconnectConfirmed;

   void cassetteRaised ();

public:

   // CONSTRUCTOR
   Screen_SEAL_AND_DETACH();


   // DESTRUCTOR
   virtual ~Screen_SEAL_AND_DETACH();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // PROCESS_CONTINUE_BUTTON
   // This method will grey-out and realize the various graphics. In the case
   // of the disconnect state, it will realize the confirm_disconnect button
   // with a 10 second time-out.
   virtual void process_continue_button ();

   void transitionToState (SEAL_DETACH_STATE newState);

};

#endif /* ifndef __SEAL_AND_DETACH_HPP__ */

/* FORMAT HASH a034e30368231ec7861fe147c1ab61bf */
