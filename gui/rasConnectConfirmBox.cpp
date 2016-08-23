/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_RAS_CONNECT_CONFIRM_BOX

*     File name:   rasConnectConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.5  2008/07/14 20:28:43Z  dslausb
More stringId changes requested by localization
Revision 1.4  2008/07/09 23:52:14Z  dslausb
More string ID standardization as requested by Marita
Revision 1.3  2008/05/15 22:26:25Z  dslausb
IT 8463 - Move open crossover instruction to clamp channel screen
Revision 1.2  2007/10/25 22:10:12Z  dslausb
IT 8263 - Revert RAS confirmation, and edit RAS connection.
Revision 1.1  2007/10/18 20:22:54Z  dslausb
Initial revision

*/

#include <vxworks.h>

/** include files **/

#include "rasConnectConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_RAS_CONNECT_CONFIRM_BOX::Screen_RAS_CONNECT_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _txtBody(textConfirmBodyRasConnect)
{
   set_screen_id(GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX);

   // Set up all the data in the parent class.
   setConfirmBoxData(KEY_RAS_CONNECT_CONFIRM,
                     BITMAP_CONFIRM_BOX_BURGUNDY,
                     207, 167,
                     BITMAP_BTN_RED_UP,
                     BITMAP_BTN_RED_DOWN,
                     20, 23, 30);
}  // END of Screen_RAS_CONNECT_CONFIRM_BOX CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_RAS_CONNECT_CONFIRM_BOX::~Screen_RAS_CONNECT_CONFIRM_BOX()
{
   Screen_RAS_CONNECT_CONFIRM_BOX::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_RAS_CONNECT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtBody.allocate_resources(*this);
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_RAS_CONNECT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
}

/* FORMAT HASH 97ae9e64d574d60ac89ceec07355e615 */
