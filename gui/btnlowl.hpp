/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Button_LowLeft

*     File name:    btnlowl.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class defines the standard LowLeft button found on most of the
         apheresis screens.  It will control the setting of bitmaps (for different
         color motifs) and the position of the button on the screen.


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/btnlowl.hpp 1.3 2002/05/07 13:55:42 sb07663 Exp $
$Log: btnlowl.hpp $
Revision 1.3  2002/05/07 13:55:42  sb07663
First cut at VxWorks compatibility
Revision 1.2  1996/07/01 19:49:54  tw09453
Update file header

*/


// Single include define guardian
#ifndef BTNLOWL_HPP
#define BTNLOWL_HPP



// CLASS INCLUDE


#include "button_bitmap.h" // Parent class


class Button_LowLeft
   : public Button_Bitmap
{

private:

   WINDOW_COLOR btn_color ;

public:



   // CONSTRUCTOR
   Button_LowLeft (WINDOW_COLOR btncolor = TEAL);


   // DESTRUCTOR
   virtual ~Button_LowLeft ();


   // ALLOCATE_RESOURCES
   // This method allocates the Button bitmap resources and establishes the up
   // and down bitmaps.
   void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources ();


}; // END of Button_LowLeft Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 3bd6c1bd045218efc384fdf19f972762 */
