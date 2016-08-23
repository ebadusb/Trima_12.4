/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Button_Bitmap

*     File name:   btngobk.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/


#include <vxworks.h>

// CLASS INCLUDE

#include "btngobk.hpp"
#include "error.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Button_Goback::Button_Goback (WINDOW_COLOR button_color) :

   Button_Bitmap (GUI_BUTTON_GOBACK, // The button's ID
                  BUTNGOBK_X,        //  button X coordinates
                  BUTNGOBK_Y,        //  button Y coordinates
                  textEmpty          //  button text is null for goback
                                     // Will set up bitmap in allocate
                                     // Will set down bitmap in allocate
                  )

{

   btn_color = button_color;    // set the local color value
                                // - don't set up bitmaps yet

}     // END of Button_Goback CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Button_Goback::~Button_Goback ()
{
   deallocate_resources ();

}     // END of Button_Goback DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Button_Goback::allocate_resources (OSWindow& parent)
{

   BITMAP_ID loc_up_btn = BITMAP_BUTTON_GOBACK_TEAL_UP;
   BITMAP_ID loc_dn_btn = BITMAP_BUTTON_GOBACK_TEAL_DOWN;



   // Set up the bitmap ID's for the up and down button states

   switch (btn_color)
   {
      case TEAL :
         loc_up_btn = BITMAP_BUTTON_GOBACK_TEAL_UP;
         loc_dn_btn = BITMAP_BUTTON_GOBACK_TEAL_DOWN;
         break ;

      case BLUE :
         loc_up_btn = BITMAP_BUTTON_GOBACK_BLUE_UP;
         loc_dn_btn = BITMAP_BUTTON_GOBACK_BLUE_DOWN;
         break ;

      case MAGENTA :
         loc_up_btn = BITMAP_BUTTON_GOBACK_MAGE_UP;
         loc_dn_btn = BITMAP_BUTTON_GOBACK_MAGE_DOWN;
         break ;

      case GREY :
         loc_up_btn = BITMAP_BUTTON_GOBACK_GREY_UP;
         loc_dn_btn = BITMAP_BUTTON_GOBACK_GREY_DOWN;
         break ;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid GOBACK button color");

   }    // End of Switch


   // Set up the appropriate up/down bitmaps in the base button after allocation.

   set_up_bitmap (loc_up_btn) ;
   set_down_bitmap (loc_dn_btn) ;


   // Allocate the base button

   Button_Bitmap::allocate_resources (parent);


}     // END of Button_Goback ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Button_Goback::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources ();    // Just deallocate the base button's resources.

}     // END of Button_Goback DEALLOCATE_RESOURCES

/* FORMAT HASH 8bde6feea03a67926cf5663064c5bfe1 */
