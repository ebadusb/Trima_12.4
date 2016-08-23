/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SALINE_CONNECT_CONFIRM_BOX

*     File name:   salineConnectConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for the saline connection
        screen.  Because the operator is dealing with several different fluids
        and spikes, we need to do something special to make them confirm they've
        connected the correct fluid to the correct spike.

        This class is derived from the confirmBoxBase class.  It adds extra
        text to that screen, and the background graphic has some specialized
        stuff.


*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef SALINE_CONNECT_CONFIRM_BOX_HPP
#define SALINE_CONNECT_CONFIRM_BOX_HPP

#include "confirmBoxBase.hpp"

class Screen_SALINE_CONNECT_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text _txtBody;

public:

   Screen_SALINE_CONNECT_CONFIRM_BOX();

   virtual ~Screen_SALINE_CONNECT_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

};

#endif

/* FORMAT HASH bed9851d3c462d84a7c84e6d3fec9d00 */
