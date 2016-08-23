/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Button_Cancel

*     File name:    btncanc.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

          This class defines the standard cancel button found on most of the
          apheresis screens.  It will control the setting of bitmaps (for different
          color motifs) and the position of the button on the screen.

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef BTNCANC_HPP
#define BTNCANC_HPP



// CLASS INCLUDE


#include "button_bitmap.h" // Parent class


class Button_Cancel
   : public Button_Bitmap
{

private:

   WINDOW_COLOR btn_color ;

public:



   // CONSTRUCTOR
   Button_Cancel (WINDOW_COLOR btncolor = TEAL);


   // DESTRUCTOR
   virtual ~Button_Cancel ();


   // ALLOCATE_RESOURCES
   // This method allocates the Button bitmap resources and establishes the up
   // and down bitmaps.
   void allocate_resources (OSWindow& widget_parent);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources ();


}; // END of Button_Cancel Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH e9250aabc16e1103aaf2faac794fce75 */
