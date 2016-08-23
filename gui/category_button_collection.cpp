/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Category_Button_Collection

*     File name:   category_button_collection.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/category_button_collection.cpp 1.5 2008/10/14 22:27:33Z dslausb Exp dslausb $
$Log: category_button_collection.cpp $
Revision 1.5  2008/10/14 22:27:33Z  dslausb
IT 8593 - Get buttons on barcode screen working
Revision 1.4  2003/11/26 17:12:02Z  rm70006
IT 6054.  Improve logging.
Revision 1.3  2003/01/06 18:31:02Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.2  2002/08/15 22:14:23  sb07663
Compiling Vx version
Revision 1.1  2000/11/06 19:18:50  tw09453
Initial revision

*/

#include <vxworks.h>

// EXTERNAL REFERENCES

#include "gui.h"

#include "guiglobs.hpp"

#include "category_button.hpp"

#include "category_button_collection.hpp"

#include "metadirectory.hpp" // sibling object directory


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Category_Button_Collection::Category_Button_Collection(MetaDirectory* wgt_directory_prm)
   : wgt_directory(wgt_directory_prm),
     current_rank(FIRST_RANK),
     _parentPtr(NULL)
{
   wgt_directory->register_object (CATEGORY_BTN_COLLECTION, (void*)this);
}   // END of Category_Button_Collection CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Category_Button_Collection::~Category_Button_Collection()
{
   wgt_directory -> unregister_object (CATEGORY_BTN_COLLECTION);

}   // END of Category_Button_Collection DESTRUCTOR


void Category_Button_Collection::allocate_resources (OSWindow& parent)
{
   _parentPtr = &parent;
   setRank(FIRST_RANK);
}

void Category_Button_Collection::deallocate_resources ()
{
   Category_Button* catbtn_ptr = NULL;

   _parentPtr = NULL;

   // Set the collection cursor to the top of the collection
   set_cursor_to_top ();

   do
   {
      // Get the first entry of the collection
      catbtn_ptr = get_next_entry ();

      // See if we're past the end of the collection
      if (catbtn_ptr != NULL)
         catbtn_ptr->deallocate_resources();

   } while (catbtn_ptr != NULL);
}



////////////////////////////////////////////////////////////////////////////////
// RESET_ACTIVE_STATE
void Category_Button_Collection::reset_active_state ()
{
   Category_Button* struct_ptr = NULL;

   // Set the collection cursor to the top of the collection
   set_cursor_to_top ();

   do
   {
      // Get the first entry of the collection
      struct_ptr = get_next_entry ();

      // See if this is an full and active category button
      if (struct_ptr != NULL)
      {
         if (struct_ptr->get_button_state () == Category_Button::CBS_FULL_ACTIVE)
         {
            struct_ptr->set_button_state (Category_Button::CBS_FULL_INACTIVE, -1);
         }
      }
   } while (struct_ptr != NULL);
}   // END of Category_Button_Collection  reset_active_state



////////////////////////////////////////////////////////////////////////////////
// DELETE_ACTIVE_BARCODE
void Category_Button_Collection::delete_active_barcode (SCAN_CATEGORY currently_displayed_category, int currently_displayed_category_sequence)
{
   Category_Button* struct_ptr = NULL;

   // Set the collection cursor to the top of the collection
   set_cursor_to_top ();

   do
   {
      // Get the first entry of the collection
      struct_ptr = get_next_entry ();

      // See if this is an full and active category button
      if (struct_ptr != NULL)
      {
         if ( (struct_ptr -> get_category_id () == currently_displayed_category) &&
              (struct_ptr -> get_rank_number  () == current_rank) )
         {
            struct_ptr -> delete_active_barcode (currently_displayed_category,
                                                 currently_displayed_category_sequence);
         }

      }

   } while (struct_ptr != NULL);
}   // END of Category_Button_Collection delete_active_barcode


void Category_Button_Collection::nextRank ()
{
   CATEGORY_RANK nextRank;

   switch (current_rank)
   {
      case RANK_1 :
         nextRank = RANK_2;
         break;

      default :
         nextRank = RANK_1;
         break;
   }

   setRank(nextRank);
}


////////////////////////////////////////////////////////////////////////////////
// setRank
//
void Category_Button_Collection::setRank (CATEGORY_RANK new_rank)
{
   Category_Button* struct_ptr = NULL;

   if (new_rank > LAST_RANK || new_rank < FIRST_RANK)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "BARCODE SCREEN: Request to switch to invalid category button rank");
   }

   // Reset any/all buttons that are full/actives to just full.
   reset_active_state ();

   // Set the collection cursor to the top of the collection
   set_cursor_to_top ();

   do
   {
      // Get the first entry of the collection
      struct_ptr = get_next_entry ();

      // See if this is an full and active category button
      if (struct_ptr != NULL &&  _parentPtr != NULL)
      {
         if (struct_ptr -> get_rank_number() == new_rank)
            struct_ptr->allocate_resources(*_parentPtr);
         else struct_ptr->deallocate_resources();
      }
      else DataLog (log_level_gui_info) << "Got a NULL pointer: struct_ptr=" << struct_ptr << "; _parentPtr=" << _parentPtr << endmsg;

   } while (struct_ptr != NULL);

   // Record this as the new rank
   current_rank = new_rank;
}   // END of Category_Button_Collection setRank



////////////////////////////////////////////////////////////////////////////////
// GET_CURRENT_RANK
CATEGORY_RANK Category_Button_Collection::get_current_rank ()
{
   return current_rank;
}   // END of Category_Button_Collection get_current_rank

/* FORMAT HASH 056b445925f143a033d04838dbce270b */
