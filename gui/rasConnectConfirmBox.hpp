/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_RAS_CONNECT_CONFIRM_BOX

*     File name:   rasConnectConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for the RAS connection
        screen.  Because the RAS spike is in the same place and uses the same
        pump as AC, it is necessary to take special action to ensure that
        the operator connects the correct bag to the correct spike.

        This class is derived from the confirmBoxBase class.  It adds extra
        text to that screen, and the background graphic has some specialized
        stuff.


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.1  2007/10/18 20:23:00Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef RAS_CONNECT_CONFIRM_BOX_HPP__
#define RAS_CONNECT_CONFIRM_BOX_HPP__

/** include files **/
#include "confirmBoxBase.hpp"

class Screen_RAS_CONNECT_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text _txtBody;

public:

   // CONSTRUCTOR
   Screen_RAS_CONNECT_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_RAS_CONNECT_CONFIRM_BOX();

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

/* FORMAT HASH 0237a5286aa51f5214eea9b7c2644be0 */
