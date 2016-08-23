/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Allocation_array

*     File name:   Allocation_array.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/allocation_array.cpp 1.5 2002/12/13 15:02:55Z rm70006 Exp rm70006 $
$Log: allocation_array.cpp $
Revision 1.5  2002/12/13 15:02:55Z  rm70006
Change allocation status to use char * instead of void *.  T2.2 compiler complained about the conversion.
Revision 1.4  2002/08/16 16:29:51Z  sb07663
added <vxworks.h> include
Revision 1.3  2002/07/05 21:13:51  sb07663
changes for new crc signature
Revision 1.2  2002/06/05 14:32:45  sb07663
Modifications for compatibility with new Vx messaging system
Revision 1.1  2000/11/06 19:31:47  tw09453
Initial revision
*/

#include <vxworks.h>

#define DEBUG 1

// CLASS INCLUDE
#include "allocation_array.hpp"
#include "crcgen.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Allocation_array::Allocation_array(long number_of_elements,
                                   long allocation_status_field_size,
                                   long user_key_field_size) :

   num_of_elements(number_of_elements),
   allocation_status_size(allocation_status_field_size),
   user_key_size(user_key_field_size)
{
   // Compute the size of a single array element
   element_size = allocation_status_field_size + user_key_field_size ;

   // Compute the total size (in bytes) of the array memory.
   array_size = num_of_elements * element_size ;

   // Allocate and base the array memory
   array_memory_ptr = new unsigned char [array_size] ;

}   // END of Allocation_array CONSTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Allocation_array::~Allocation_array()
{
   if (array_memory_ptr != NULL)
   {
      // attempt to free the memory
      delete [] array_memory_ptr ;
      array_memory_ptr = NULL ;
   }
}   // END of Allocation_array DESTRUCTOR


////////////////////////////////////////////////////////////////////////////////
// INITIALIZE_ALLOCATION_STATUS
void Allocation_array::initialize_allocation_status (const char* allocation_status)
{
   for (long i = 0; i < num_of_elements; i++)
   {
      set_allocation_status (i, allocation_status) ;
   }
}    // END of Allocation_array INITIALIZE_ALLOCATION_STATUS


////////////////////////////////////////////////////////////////////////////////
// SET_ALLOCATION_STATUS
long Allocation_array::set_allocation_status (long index, const char* allocation_status)
{
   unsigned char* element_address = check_and_return_element_address (index) ;

   if (element_address == NULL)
   {
      return(ARR_FAILURE) ;
   }

   memmove (element_address, allocation_status,  allocation_status_size) ;

   return(ARR_SUCCESS);

}   // END of Allocation_array SET_ALLOCATION_STATUS



////////////////////////////////////////////////////////////////////////////////
// GET_ALLOCATION_STATUS

long Allocation_array::get_allocation_status (long index, char* allocation_status)
{
   unsigned char* element_address = check_and_return_element_address (index) ;

   if (element_address == NULL)
   {
      return(ARR_FAILURE) ;
   }

   memmove (allocation_status, element_address, allocation_status_size) ;

   return(ARR_SUCCESS);

}   // END of Allocation_array GET_ALLOCATION_STATUS



////////////////////////////////////////////////////////////////////////////////
// SET_USER_KEY

long Allocation_array::set_user_key (long index, long keybuff_length, const unsigned char* keybuff)
{
   unsigned char* element_address = check_and_return_element_address (index) ;

   if (element_address == NULL)
   {
      return(ARR_FAILURE) ;
   }

   if (keybuff_length > user_key_size)
   {
      return(ARR_FAILURE) ;
   }

   long min_copy = (keybuff_length < user_key_size) ? keybuff_length : user_key_size ;

   // Clean the whole key
   memset  ((element_address + allocation_status_size), '\0', user_key_size) ;

   // Copy in as much as will fit of the user's key
   memmove ((element_address + allocation_status_size), keybuff, min_copy) ;

   return(ARR_SUCCESS);

}   // END of Allocation_array SET_USER_KEY



////////////////////////////////////////////////////////////////////////////////
// GET_USER_KEY

long Allocation_array::get_user_key (long index, long keybuff_length, unsigned char* keybuff)
{
   unsigned char* element_address = check_and_return_element_address (index) ;

   if (element_address == NULL)
   {
      return(ARR_FAILURE) ;
   }

   long min_copy = (keybuff_length < user_key_size) ? keybuff_length : user_key_size ;

   memmove (keybuff, (element_address + allocation_status_size), min_copy) ;

   return(ARR_SUCCESS);

}   // END of Allocation_array GET_USER_KEY



////////////////////////////////////////////////////////////////////////////////
// GET_SERIALIZED_ARRAY_POINTER

void Allocation_array::get_serialized_array_pointer (unsigned long* array_length, unsigned char** array_pointer)
{
   // Return the total length of the array (in bytes).
   *array_length = array_size ;

   // Return the pointer to the array
   *array_pointer = array_memory_ptr ;

}   // END of Allocation_array GET_SERIALIZED_ARRAY_POINTER



////////////////////////////////////////////////////////////////////////////////
// LOAD_ARRAY_FROM_FILE

void Allocation_array::load_array_from_file (const unsigned char* serialized_array_pointer)
{
   // Copy over the caller's serialized array into the array memory space
   memmove (array_memory_ptr, serialized_array_pointer, array_size) ;

}   // END of Allocation_array LOAD_ARRAY_FROM_FILE



////////////////////////////////////////////////////////////////////////////////
// CHECK_AND_RETURN_ELEMENT_ADDRESS

unsigned char* Allocation_array::check_and_return_element_address (long index)
{
   // Make sure the index is within the array limits (bounds=  0 : (num_of_elements-1))
   if (!((index >= 0) && (index < num_of_elements)))
   {
      // The index is out of bounds
      return(NULL) ;
   }

   unsigned char* this_element_ptr = array_memory_ptr + (index * element_size) ;

   return(this_element_ptr) ;

}   // END of Allocation_array CHECK_AND_RETURN_ELEMENT_ADDRESS



////////////////////////////////////////////////////////////////////////////////
// GET_ARRAY_CRC

unsigned long Allocation_array::get_array_crc ()
{
   unsigned long result = 0;
   crcgen32(&result, array_memory_ptr, array_size);
   return result;

}   // END of Allocation_array GET_ARRAY_CRC

/* FORMAT HASH d19f90b95b67b59ad70faf7187ae823d */
