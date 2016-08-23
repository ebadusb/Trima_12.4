/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_REBOOT_CONFIRM_BOX

*     File name:   rebootConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for a system reboot.
        Prior to IT 8388, the operator the continue button on the final
        run summary screen said "next procedure", and caused the system
        to reboot in preparation for the next procedure.

        The problem was that operators tended to get "happy fingers", touching
        continue repeatedly to get through the screens, and if they pressed
        continue one too many times, they rebooted and missed their run summary.
        This screen functions as an obstacle to that happening.


*************************< FOCUSSED GUI DOMAIN >****************************

Initial revision

*/

// Single include define guardian
#ifndef REBOOT_CONFIRM_BOX_HPP__
#define REBOOT_CONFIRM_BOX_HPP__

/** include files **/
#include "confirmBoxBase.hpp"

class Screen_REBOOT_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text _txtBody;

public:

   // CONSTRUCTOR
   Screen_REBOOT_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_REBOOT_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

}; // END of Screen_REBOOT_CONFIRM_BOX Class Declaration

// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of rebootConfirmBox.hpp file

/* FORMAT HASH 44410fa6103d83babba8c4ee15a00f6f */
