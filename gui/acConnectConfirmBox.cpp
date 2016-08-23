/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_AC_CONNECT_CONFIRM_BOX

*     File name:   acConnectConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.3  2008/07/14 20:23:51Z  dslausb
More stringId changes requested by localization
Revision 1.2  2008/07/09 23:49:02Z  dslausb
More string ID standardization as requested by Marita
Revision 1.1  2008/06/05 22:40:06Z  dslausb
Initial revision

*/

#include "acConnectConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_AC_CONNECT_CONFIRM_BOX::Screen_AC_CONNECT_CONFIRM_BOX()
   : ConfirmBoxBase(),
     _txtBody(textConfirmBodyAcConnect)
{
   set_screen_id(GUI_SCREEN_AC_CONNECT_CONFIRM_BOX);

   // Set up all the data in the parent class.
   setConfirmBoxData(KEY_AC_CONNECT_CONFIRM,
                     BITMAP_CONFIRM_BOX_TEAL,
                     182, 171,
                     BITMAP_CONFIG_SUB_UP,
                     BITMAP_CONFIG_SUB_DOWN,
                     20, 23, 30);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_AC_CONNECT_CONFIRM_BOX::~Screen_AC_CONNECT_CONFIRM_BOX()
{
   Screen_AC_CONNECT_CONFIRM_BOX::deallocate_resources ();

}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_AC_CONNECT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtBody.allocate_resources(*this);
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_AC_CONNECT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
}

/* FORMAT HASH cb58e409f314d4272270945a54b94551 */
