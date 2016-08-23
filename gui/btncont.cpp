/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Button_Continue

*     File name:   btncont.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>

// CLASS INCLUDE

#include "btncont.hpp"
#include "error.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Button_Continue::Button_Continue (WINDOW_COLOR button_color, bool extraLogging = false) :

   Button_Bitmap (GUI_BUTTON_CONTINUE,    // The button's ID
                  BUTNCONT_X,             // Continue button X coordinates
                  BUTNCONT_Y,             // Continue button Y coordinates
                  textButtonLongContinue, // Continue button text
                  BITMAP_NULL,            // Will set up bitmap in allocate
                  BITMAP_NULL,            // Will set down bitmap in allocate
                  extraLogging
                  )

{
   btn_color = button_color; // set the local color value - don't set up bitmaps yet

}  // END of Button_Continue CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Button_Continue::~Button_Continue ()
{
   deallocate_resources ();
}  // END of Button_Continue DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Button_Continue::allocate_resources (OSWindow& parent)
{

   BITMAP_ID loc_up_btn = BITMAP_BUTTON_LARGE_BOT_TEAL_UP;
   BITMAP_ID loc_dn_btn = BITMAP_BUTTON_LARGE_BOT_TEAL_DOWN;


   // Set up the bitmap ID's for the up and down button states

   switch (btn_color)
   {
      case TEAL :
         loc_up_btn = BITMAP_BUTTON_LARGE_BOT_TEAL_UP;
         loc_dn_btn = BITMAP_BUTTON_LARGE_BOT_TEAL_DOWN;
         break;

      case BLUE :
         loc_up_btn = BITMAP_BUTTON_LARGE_BOT_BLUE_UP;
         loc_dn_btn = BITMAP_BUTTON_LARGE_BOT_BLUE_DOWN;
         break;

      case MAGENTA :
         loc_up_btn = BITMAP_BUTTON_LARGE_BOT_MAGE_UP;
         loc_dn_btn = BITMAP_BUTTON_LARGE_BOT_MAGE_DOWN;
         break;

      case GREY :
         loc_up_btn = BITMAP_BUTTON_LARGE_BOT_GREY_UP;
         loc_dn_btn = BITMAP_BUTTON_LARGE_BOT_GREY_DOWN;
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid CONTINUE button color");

   } // End of Switch


   // Set up the appropriate up/down bitmaps in the base button after allocation.

   set_up_bitmap (loc_up_btn);
   set_down_bitmap (loc_dn_btn);

   // Allocate the base button

   Button_Bitmap::allocate_resources (parent);

}  // END of Button_Continue ALLOCATE_RESOURCES



////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Button_Continue::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources (); // Just deallocate the base button's resources.

}  // END of Button_Continue DEALLOCATE_RESOURCES

/* FORMAT HASH 24c33e7e9b28cb1ba5add18d7220f9ee */
