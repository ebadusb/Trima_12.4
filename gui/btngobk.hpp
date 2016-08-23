/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Button_Goback

*     File name:    btngobk.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class defines the standard goback button found on most of the
         apheresis screens.  It will control the setting of bitmaps (for different
         color motifs) and the position of the button on the screen.

*************************< FOCUSSED GUI DOMAIN >***************************/



// Single include define guardian
#ifndef BTNGOBK_HPP
#define BTNGOBK_HPP



// CLASS INCLUDE


#include "button_bitmap.h" // Parent class


class Button_Goback
   : public Button_Bitmap
{

private:

   WINDOW_COLOR btn_color ;

public:



   // CONSTRUCTOR
   Button_Goback (WINDOW_COLOR btncolor = TEAL) ;


   // DESTRUCTOR
   virtual ~Button_Goback ();


   // ALLOCATE_RESOURCES
   // This method allocates the Button bitmap resources and establishes the up
   // and down bitmaps.
   void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources ();


}; // END of Button_Goback Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 2278b49f8deb7b9cf7bec6735bbf50ab */
