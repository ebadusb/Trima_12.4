/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_POWERUP

*     File name:    powerup.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This screen is derived from the base apheresis class. It displays
         the initial powerup product screen at software initialization.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header$

*/


// Single include define guardian
#ifndef POWERUP_HPP
#define POWERUP_HPP



// CLASS INCLUDE
#include "bwindow.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"


// CLASS DECLARES

class Screen_POWERUP
   : public Base_Window
{
public:
   // CONSTRUCTOR
   Screen_POWERUP();


   // DESTRUCTOR
   virtual ~Screen_POWERUP();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();


   // SCREEN_NOTIFY_HANDLER
   // This method is activated when a "GUI ALIVE" query comes in from the
   // procedure domain. Its entire function is to answer to procedure that
   // the GUI IS alive, thus synchronizing the GUI-PROC dialog.
   virtual void screen_notify_handler ();

   virtual void draw (UGL_GC_ID gc);

private:
   // The full screen power-up graphic bitmap
   Bitmap bitmap_placard_powerup;

   // The text strings for version, rights, product exposition
   Display_Text text_product_exposition;
   Display_Text text_version;
   Display_Text text_rights;

   // Screen Event Handler pointers
   QueryGUIAliveMsg guiAliveMsg;

}; // END of Screen_POWERUP Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 883a47de7e7671d1b7fb6d20ef2cb855 */
