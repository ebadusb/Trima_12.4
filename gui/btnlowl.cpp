/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Button_LowLeft

*     File name:   btnlowl.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/btnlowl.cpp 1.7 2003/01/06 18:31:00Z jl11312 Exp $
$Log: btnlowl.cpp $
Revision 1.7  2003/01/06 18:31:00Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.6  2002/09/13 17:16:21  sb07663
moved allocation order for vxworks operation
Revision 1.5  2002/08/16 16:29:56  sb07663
added <vxworks.h> include
Revision 1.4  2002/05/07 13:55:42  sb07663
First cut at VxWorks compatibility
Revision 1.3  1996/07/01 19:49:50  tw09453
Update file header

*/

#include <vxworks.h>


// CLASS INCLUDE

#include "btnlowl.hpp"
#include "error.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Button_LowLeft::Button_LowLeft (WINDOW_COLOR button_color) :

   Button_Bitmap (GUI_BUTTON_LOWLEFT, // The button's ID
                  BUTNLOWL_X,         // LowLeft button X coordinates
                  BUTNLOWL_Y,         // LowLeft button Y coordinates
                  textEmpty           // LowLeft button text
                                      // Will set up bitmap in allocate
                                      // Will set down bitmap in allocate
                  )

{

   btn_color = button_color ;    // set the local color value - don't set up bitmaps yet

}     // END of Button_LowLeft CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Button_LowLeft::~Button_LowLeft ()
{
   deallocate_resources ();
}     // END of Button_LowLeft DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES

void Button_LowLeft::allocate_resources (OSWindow& parent)
{

   BITMAP_ID loc_up_btn = BITMAP_BUTTON_SMALL_BOT_TEAL_UP;
   BITMAP_ID loc_dn_btn = BITMAP_BUTTON_SMALL_BOT_TEAL_DOWN;



   // Set up the bitmap ID's for the up and down button states

   switch (btn_color)
   {
      case TEAL :
         loc_up_btn = BITMAP_BUTTON_SMALL_BOT_TEAL_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_BOT_TEAL_DOWN;
         break ;

      case BLUE :
         loc_up_btn = BITMAP_BUTTON_SMALL_BOT_BLUE_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_BOT_BLUE_DOWN;
         break ;

      case MAGENTA :
         loc_up_btn = BITMAP_BUTTON_SMALL_BOT_MAGE_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_BOT_MAGE_DOWN;
         break ;

      case GREY :
         loc_up_btn = BITMAP_BUTTON_SMALL_BOT_GREY_UP;
         loc_dn_btn = BITMAP_BUTTON_SMALL_BOT_GREY_DOWN;
         break ;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid Lower Left button color");

   }    // End of Switch


   // Set up the appropriate up/down bitmaps

   set_up_bitmap (loc_up_btn) ;
   set_down_bitmap (loc_dn_btn) ;

   // Allocate the base button
   Button_Bitmap::allocate_resources (parent);


}     // END of Button_LowLeft ALLOCATE_RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void Button_LowLeft::deallocate_resources ()
{
   Button_Bitmap::deallocate_resources ();    // Just deallocate the base button's resources.

}     // END of Button_LowLeft DEALLOCATE_RESOURCES

/* FORMAT HASH 08786c9a7abea018baf3d187ff0320bf */
