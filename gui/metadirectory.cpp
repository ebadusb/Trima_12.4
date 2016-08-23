/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  MetaDirectory

*     File name:   metadirectory.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< TRIMA GUI DOMAIN >****************************

Revision 1.2  2002/04/22 18:04:50  sb07663
Changes for initial VxWorks port
Revision 1.1  2000/11/06 19:19:27  tw09453
Initial revision

*/

#include <vxworks.h>

// EXTERNAL REFERENCES

#include "gui.h"
#include "metadirectory.hpp"
#include "obj_name_entry.hpp"
#include "trima_datalog.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

MetaDirectory::MetaDirectory()
   : number_of_unregistered(0)
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

MetaDirectory::~MetaDirectory() {}



////////////////////////////////////////////////////////////////////////////////
// GET_OBJECT_PTR

void* MetaDirectory::get_object_ptr (BC_OBJECT_NAME object)
{

   Object_Name_Entry* struct_ptr = NULL;

   // Set the collection cursor to the top of the collection
   set_cursor_to_top () ;

   do
   {
      // Get the first entry of the collection
      struct_ptr = get_next_entry () ;

      // See if this is registered and the right object.
      if (struct_ptr != NULL)
      {
         if ((struct_ptr -> object_registered == true) && (struct_ptr -> object_name == object))
         {
            return ((void*)(struct_ptr -> object_ptr)) ;
         }

      }

   } while (struct_ptr != NULL) ;


   // Didn't find it -- veeeeeeeeeery bad news

   _FATAL_ERROR (__FILE__, __LINE__, "MetaDirectory: Unable to find the pointer to an object reference");

   return (NULL) ;    // for the compiler.

}



////////////////////////////////////////////////////////////////////////////////
// REGISTER_OBJECT

void MetaDirectory::register_object (BC_OBJECT_NAME object, void* object_pointer)
{


   Object_Name_Entry* struct_ptr         = NULL ;
   int                found_unregistered = false ;


   if (number_of_unregistered > 0)    // we have some dead entries we can re-use
   {
      // Set the collection cursor to the top of the collection
      set_cursor_to_top () ;

      do
      {
         // Get the first entry of the collection
         struct_ptr = get_next_entry () ;

         // See if this is registered and the right object.
         if (struct_ptr != NULL)
         {
            if (struct_ptr -> object_registered == false)
            {
               number_of_unregistered-- ;
               found_unregistered = true ;
               break;       // out of the do-while
            }

         }

      } while (struct_ptr != NULL) ;

   }    // End of: "if (number_of_unregistered > 0)



   if (found_unregistered == false)
   {
      // Create the object structure
      struct_ptr = new Object_Name_Entry ;

      // Add this to the table
      add_table_entry (struct_ptr) ;
   }

   // Initialize the entry and load up the caller's parameters
   struct_ptr -> object_registered = true ;
   struct_ptr -> object_name       = object ;
   struct_ptr -> object_ptr        = object_pointer ;


}     // END of MetaDirectory register_object





////////////////////////////////////////////////////////////////////////////////
// UNREGISTER_OBJECT

void MetaDirectory::unregister_object (BC_OBJECT_NAME object)
{
   Object_Name_Entry* struct_ptr = NULL;

   // Set the collection cursor to the top of the collection
   set_cursor_to_top () ;

   do
   {
      // Get the first entry of the collection
      struct_ptr = get_next_entry () ;

      // See if this is registered and the right object.
      if (struct_ptr != NULL)
      {
         if ((struct_ptr -> object_registered) && (struct_ptr -> object_name == object))
         {
            struct_ptr -> object_registered = false ;
            number_of_unregistered++ ;
         }

      }

   } while (struct_ptr != NULL) ;


}

/* FORMAT HASH 87a0a70c53c3681d6ddf1c763b112790 */
