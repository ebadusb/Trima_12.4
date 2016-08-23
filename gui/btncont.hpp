/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Button_Continue

*     File name:    btncont.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class defines the standard continue button found on
         most of the apheresis screens. It will control the setting
         of bitmaps (for different color motifs) and the position
         of the button on the screen.


*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef BTNCONT_HPP
#define BTNCONT_HPP



// CLASS INCLUDE


#include "button_bitmap.h" // Parent class


class Button_Continue
   : public Button_Bitmap
{

private:
   WINDOW_COLOR btn_color;

public:
   // CONSTRUCTOR
   Button_Continue (WINDOW_COLOR btncolor = TEAL, bool extraLogging = false);


   // DESTRUCTOR
   virtual ~Button_Continue ();


   // ALLOCATE_RESOURCES
   // This method allocates the Button bitmap resources and establishes the up
   // and down bitmaps.
   void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources ();

}; // END of Button_Continue Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 0b5f1f91d0db9ce02908e227fb777ce3 */
