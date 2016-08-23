/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2007 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_REBOOT_CONFIRM_BOX

*     File name:   rebootConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.2  2008/07/09 23:52:25Z  dslausb
More string ID standardization as requested by Marita
Revision 1.1  2008/02/18 23:43:49Z  dslausb
Initial revision
Initial revision

*/

#include <vxworks.h>

/** include files **/

#include "rebootConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_REBOOT_CONFIRM_BOX::Screen_REBOOT_CONFIRM_BOX()
   : ConfirmBoxBase(),

     _txtBody(textConfirmBodyRestart)
{
   set_screen_id(GUI_SCREEN_REBOOT_CONFIRM_BOX);

   // Set up all the data in the parent class.
   setConfirmBoxData(KEY_REBOOT_CONFIRM,
                     BITMAP_REBOOT_CONFIRM_BOX,
                     180, 167,
                     BITMAP_BTN_RED_UP,
                     BITMAP_BTN_RED_DOWN,
                     20, 23, 30);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_REBOOT_CONFIRM_BOX::~Screen_REBOOT_CONFIRM_BOX()
{
   Screen_REBOOT_CONFIRM_BOX::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_REBOOT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtBody.allocate_resources(*this);
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_REBOOT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
}

/* FORMAT HASH ed286bc1f9affb5954b90677a42006d3 */
