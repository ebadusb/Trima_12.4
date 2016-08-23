/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PAS_CONNECT_CONFIRM_BOX

*     File name:   pasConnectConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for the PAS connection
        screen.  Because the operator is dealing with several different fluids
        and spikes, we need to do something special to make them confirm they've
        connected the correct fluid to the correct spike.

        This class is derived from the confirmBoxBase class.  It adds extra
        text to that screen, and the background graphic has some specialized
        stuff.


*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef PAS_CONNECT_CONFIRM_BOX_HPP
#define PAS_CONNECT_CONFIRM_BOX_HPP

/** include files **/
#include "confirmBoxBase.hpp"

class Screen_PAS_CONNECT_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text _txtBody;

public:

   // CONSTRUCTOR
   Screen_PAS_CONNECT_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_PAS_CONNECT_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

};

// End of single include define guardian
#endif

/* FORMAT HASH e870c6a409009e6dab639ee5de7ed224 */
