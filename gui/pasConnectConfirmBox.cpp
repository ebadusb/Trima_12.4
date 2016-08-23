/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_PAS_CONNECT_CONFIRM_BOX

*     File name:   pasConnectConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.3  2008/07/14 20:28:18Z  dslausb
More stringId changes requested by localization
Revision 1.2  2008/07/09 23:51:05Z  dslausb
More string ID standardization as requested by Marita
Revision 1.1  2008/04/24 22:14:49Z  dslausb
Initial revision

*/

#include <vxworks.h>

/** include files **/

#include "pasConnectConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_PAS_CONNECT_CONFIRM_BOX::Screen_PAS_CONNECT_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _txtBody(textConfirmBodyPasConnect)
{
   set_screen_id(GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX);

   // Set up all the data in the parent class.
   setConfirmBoxData(KEY_PAS_CONNECT_CONFIRM,
                     BITMAP_CONFIRM_BOX_BURGUNDY,
                     180, 167,
                     BITMAP_BTN_RED_UP,
                     BITMAP_BTN_RED_DOWN,
                     20, 23, 30);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_PAS_CONNECT_CONFIRM_BOX::~Screen_PAS_CONNECT_CONFIRM_BOX()
{
   Screen_PAS_CONNECT_CONFIRM_BOX::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_PAS_CONNECT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtBody.allocate_resources(*this);
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_PAS_CONNECT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
}

/* FORMAT HASH a3a918b3bb00c9b7a67e2ffd50b80232 */
