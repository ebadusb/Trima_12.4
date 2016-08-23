/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_CASSETTE_DELETE_CONFIRM_BOX

*     File name:   cassetteDeleteConfirmBox.hpp

*     Contents:    See prototype header descriptions below...

*     Description: This class represents a confirm box for the deletion of a cassette
        from the setconfig list, the effect of which is to remove a cassette from
        the dropdown menu on the cassette selection screen.

        This class is derived from the confirmBoxBase class.  There are two
        different invocations of this box: the "delete all" confirmation box,
        and the "delete one" confirmation box.  The difference is that the delete
        one version shows the REF of the cassette you're confirming deletion for.


*************************< FOCUSSED GUI DOMAIN >****************************
*/

// Single include define guardian
#ifndef __CONFIRM_BOX_HPP__
#define __CONFIRM_BOX_HPP__

/** include files **/
#include "message.h"
#include "aphwind.hpp"
#include "button_bitmap.h"
#include "confirmBoxBase.hpp"



class Screen_CASSETTE_DELETE_CONFIRM_BOX
   : public ConfirmBoxBase
{
protected:

   Display_Text _confirmText;

public:

   // CONSTRUCTOR
   Screen_CASSETTE_DELETE_CONFIRM_BOX();

   // DESTRUCTOR
   virtual ~Screen_CASSETTE_DELETE_CONFIRM_BOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   static guistring createAllocationString (CONFIG_DATA_KEYS dataKey,
                                            int xPos,
                                            int yPos);
};

#endif

/* FORMAT HASH d7f603e10588ea698bdd2f19f6489cc2 */
