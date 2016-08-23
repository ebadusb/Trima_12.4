/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Button_Bitmap

*     File name:   btnbitm.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>


// CLASS INCLUDE

#include "btncanc.hpp"
#include "error.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Button_Cancel::Button_Cancel (WINDOW_COLOR button_color) :

   Button_Bitmap (GUI_BUTTON_CANCEL, // The button's ID.
                  BUTNCANC_X,        // Cancel button X coordinates
                  BUTNCANC_Y,        // Cancel button Y coordinates
                  textButtonCancel   // Cancel button text
                                     // Will set up bitmap in allocate
                                     // Will set down bitmap in allocate
                  )

{

   btn_color = button_color ;    // set the local color value - don't set up bitmaps yet

}     // END of Button_Cancel CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Button_Cancel::~Button_Cancel ()
{
   deallocate_resources ();
}     // END of Button_Cancel DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Button_Cancel::allocate_resources (OSWindow& parent)
{

   BITMAP_ID loc_up_btn = BITMAP_BUTTON_SMALL_TOP_TEAL_UP;
   BITMAP_ID loc_dn_btn = BITMAP_BUTTON_SMALL_TOP_TEAL_DOWN;


   // Set up the bitmap ID's for the up and down button states

   switch (btn_color)
   {
      case TEAL :
         loc_up_btn = BITMAP_BUTTON_SMALL_TOP_TEAL_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_TOP_TEAL_DOWN;
         break ;

      case BLUE :
         loc_up_btn = BITMAP_BUTTON_SMALL_TOP_BLUE_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_TOP_BLUE_DOWN;
         break ;

      case MAGENTA :
         loc_up_btn = BITMAP_BUTTON_SMALL_TOP_MAGE_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_TOP_MAGE_DOWN;
         break ;

      case GREY :
         loc_up_btn = BITMAP_BUTTON_SMALL_TOP_GREY_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_TOP_GREY_DOWN;
         break ;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid CANCEL button color");

   }    // End of Switch


   // Set up the appropriate up/down bitmaps in the base button after allocation.

   set_up_bitmap (loc_up_btn) ;
   set_down_bitmap (loc_dn_btn) ;

   // Allocate the base button

   Button_Bitmap::allocate_resources (parent);


}     // END of Button_Cancel ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Button_Cancel::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources ();    // Just deallocate the base button's resources.

}     // END of Button_Cancel DEALLOCATE_RESOURCES

/* FORMAT HASH 704f7982951255e3d50df0195d74f192 */
