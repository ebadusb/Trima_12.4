/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Category_Button_Collection

*     File name:    category_button_collection.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is derived off of the table collection template and contains
         pointers to all the category buttons. It is used to perform "rank"
         changing, accessing the full active button, and to communicate to
         specific buttons when the client has no addressability to the individual
         category button.

*************************< TRIMA GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/category_button_collection.hpp 1.3 2003/11/26 17:12:08Z rm70006 Exp dslausb $
$Log: category_button_collection.hpp $
Revision 1.3  2003/11/26 17:12:08Z  rm70006
IT 6054.  Improve logging.
Revision 1.2  2003/05/12 17:12:38Z  rm70006
IT 5815.

Finish porting barcode stuff.
Revision 1.1  2000/11/06 19:19:00Z  tw09453
Initial revision

*/



// Single include define guardian
#ifndef CATBTNCOLL_HPP
#define CATBTNCOLL_HPP


#include "barcode_object_list.h" // object ID's and addresses

// #include "category_button.hpp"  // include the collection element declaration

#include "tablecol.hpp"  // include the base class declaration

#include "scan_structure.h" // the scan structure declaration

// #include "metadirectory.hpp" // sibling object directory

#include "category_button_parms.h" // enums / manifests etc.


class Category_Button;  // Ensure to the compiler that this REALLY IS a class

class MetaDirectory;



class Category_Button_Collection
   : public Table_Collection<Category_Button>
{
private:

   CATEGORY_RANK  current_rank;

   MetaDirectory* wgt_directory;

   OSWindow*      _parentPtr;

public:
   // CONSTRUCTOR
   Category_Button_Collection(MetaDirectory* wgt_directory_prm);


   // DESTRUCTOR
   virtual ~Category_Button_Collection();


   // RESET_ACTIVE_STATE
   // This searches the collection looking for any full active buttons and sets
   // their state to full inactive
   void reset_active_state ();

   // DELETE_ACTIVE_BARCODE
   // This searches for the category button that owns this category entry and
   // requests that it delete it and display the next available category entry.
   void delete_active_barcode (SCAN_CATEGORY currently_displayed_category, int currently_displayed_category_sequence);

   // setRank
   // This method will switch to the rank of buttons specified in the parameter.
   // It will first unrealize all the buttons in the current rank. It will then
   // realize all the buttons in the newly requested rank.
   void setRank (CATEGORY_RANK new_rank);

   void nextRank ();

   // GET_CURRENT_RANK
   // This returns the current rank of the displayed category buttons.
   CATEGORY_RANK get_current_rank ();

   void allocate_resources (OSWindow& parent);

   void deallocate_resources ();


};  // END of Category_Button_Collection Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH d67f5f8dc03f9f6959cdabd6d3ed8b37 */
