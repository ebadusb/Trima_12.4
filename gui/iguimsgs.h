/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   N/A

*     File name:    guimsgs.h

*     Contents:     See prototype header descriptions below...

*     Description:

         Various structure definitions for structure messages between
         Proc and GUI and intra-GUI messaging.


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.14  2002/11/11 06:30:35Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.13  2002/06/05 14:32:50  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.12  2000/06/22 21:37:42  ms10234
Move the Run Summary structure to the trima common project
Revision 1.11  1999/05/06 17:04:37  MS10234
Added flags to run summary message for product counting and
verification.
Revision 1.10  1997/05/30 15:43:57  TM02109
Change the platelet, plasma, rbc volumes to float so that GUI can
handle these in the same manner in the post run screens as they
are handled in the other screens.
Revision 1.9  1997/02/20 01:26:24  TM02109
Modified to add new post_hct and count message type variables
and population for the post_hct in ss10-1
Revision 1.8  1996/10/09 23:37:47  Barry_Fulkerson
- removed the GUI_LISTBOX_STRUC.  Now this structure is
  spec'd in the cfg_kstr.hpp file
Revision 1.7  1996/09/28 22:32:00  Barry_Fulkerson
GUI Configuration additions for 2.3
Revision 1.6  1996/07/08 20:21:23  tw09453
Update file header

*/

#ifndef IGUIMSGS_H
#define IGUIMSGS_H

#include "gui.h"
#include "guistring.h"

////////////////////////////////////////////////////////////////////////////////
// GUI to GUI Message Structures

// Structure used to communicate to the GUI commutator for switching both
// procedure and child screens.

enum invocation_method
{
   DISPLAY_AS_CHILD,
   INVOKE_SCREEN,
   DISPLAY_AS_STATELESS,
   SWAP_CHILD_SCREEN,
   SWAP_STATELESS_SCREEN,
   SWAP_CHILD_WITH_SCREEN,
   SWAP_STATELESS_WITH_SCREEN,
};

struct SCREEN_INVOKE_STRUCT
{
   invocation_method screen_invocation;
   GUI_SCREEN_ID     screen_id;
   guistring         alloc_param;

};

#endif

/* FORMAT HASH 885cdf673128eb67fe7ffea20f95f6cf */
