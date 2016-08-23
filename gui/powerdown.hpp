/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 Gambro BCT, Inc. All rights reserved

*     Author:       Dan Slauson

*     Class name:   Screen_POWERDOWN

*     File name:    powerdown.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This screen is derived from the base window class. It displays
         the final powerdown screen just before reboot.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/powerdown.hpp 1.2 2008/08/15 20:36:34Z mereusb Exp $
$Log: powerdown.hpp $
Revision 1.2  2008/08/15 20:36:34Z  mereusb
IT 8107: Change for screenshot scripts.
Revision 1.1  2008/03/13 16:01:59  dslausb
Initial revision

*/


// Single include define guardian
#ifndef __POWERDOWN_HPP__
#define __POWERDOWN_HPP__



// CLASS INCLUDE

#include "bwindow.hpp"
#include "display_text.h"

#if CPU==SIMNT
#include "guiUnitTestMessageDefs.h"
#endif /* if CPU==SIMNT */

// CLASS DECLARES
class Screen_POWERDOWN
   : public Base_Window
{

private:

   // The text strings for version, rights, product exposition
   Display_Text _txtMsg ;
   OSRectangle  _background;

public:


   // CONSTRUCTOR
   Screen_POWERDOWN();


   // DESTRUCTOR
   virtual ~Screen_POWERDOWN();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   void watchdogFailure ();
    #if CPU==SIMNT
protected:
   SimntTraverseMsg* _screenTraverseMessage;
   virtual void handleTraverseMessage ();
    #endif /* if CPU==SIMNT */
};  // END of Screen_POWERDOWN Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 81d5615270e7583d63d791795f658a4d */
