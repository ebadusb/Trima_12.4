/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SALINE_CONNECT_CONFIRM_BOX

*     File name:   salineConnectConfirmBox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.4  2008/07/14 20:29:23Z  dslausb
More stringId changes requested by localization
Revision 1.3  2008/07/09 23:53:21Z  dslausb
More string ID standardization as requested by Marita
Revision 1.2  2008/06/05 22:41:26Z  dslausb
IT 8673 - Solution connection confirmation boxes
Revision 1.1  2008/06/05 20:56:54Z  dslausb
Initial revision

*/

#include <vxworks.h>

#include "salineConnectConfirmBox.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_SALINE_CONNECT_CONFIRM_BOX::Screen_SALINE_CONNECT_CONFIRM_BOX()
   : ConfirmBoxBase(),
     _txtBody(textConfirmBodySalineConnect)
{
   set_screen_id(GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX);

   // Set up all the data in the parent class.
   setConfirmBoxData(KEY_SALINE_CONNECT_CONFIRM,
                     BITMAP_CONFIRM_BOX_TEAL,
                     178, 185,
                     BITMAP_CONFIG_SUB_UP,
                     BITMAP_CONFIG_SUB_DOWN,
                     20, 23, 30);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_SALINE_CONNECT_CONFIRM_BOX::~Screen_SALINE_CONNECT_CONFIRM_BOX()
{
   Screen_SALINE_CONNECT_CONFIRM_BOX::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_SALINE_CONNECT_CONFIRM_BOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   ConfirmBoxBase::allocate_resources (allocation_parameter);

   _txtBody.allocate_resources(*this);
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SALINE_CONNECT_CONFIRM_BOX::deallocate_resources ()
{
   if (!_allocated) return;

   _txtBody.deallocate_resources();
   ConfirmBoxBase::deallocate_resources ();
}

/* FORMAT HASH a0e10f1c23631363159c43e317b221e3 */
