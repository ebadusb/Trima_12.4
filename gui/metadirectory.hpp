/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   MetaDirectory

*     File name:    metadirectory.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This is derived off of the table collection template and contains
         structures that identify singleton objects and contain pointers to
         those objects. Various peer objects that don't have lexical scope
         or aggregation can interact by registering their ID's and addresses
         with the MetaDirectory. Others who know of them but don't have
         addressability can query about their address in order to invoke
         their methods.

*************************< TRIMA GUI DOMAIN >****************************
*/



// Single include define guardian
#ifndef METADIRECTORY_HPP
#define METADIRECTORY_HPP


#include "barcode_object_list.h" // object ID's and addresses

#include "tablecol.hpp"  // include the base class declaration



// Structure containing the object ID and its pointer
//
class Object_Name_Entry ;


class MetaDirectory
   : public Table_Collection<Object_Name_Entry>
{
private:

   int number_of_unregistered ;


protected:



public:

   MetaDirectory() ;

   virtual ~MetaDirectory() ;


   // REGISTER_OBJECT
   // This creates a registration entry in the table and marks the entry as active
   void register_object   (BC_OBJECT_NAME object, void* object_pointer) ;

   // UNREGISTER_OBJECT
   // This marks the entry as unregistered (logical delete)
   void unregister_object (BC_OBJECT_NAME object) ;


   // GET_OBJECT_PTR
   // Searches the table for the structure that matches this object ID and is actively registered.
   void* get_object_ptr (BC_OBJECT_NAME object) ;



};

#endif

/* FORMAT HASH 90ac9a55fe92933e9dc366a1b31614f5 */
