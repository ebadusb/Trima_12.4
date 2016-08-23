/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2000 Gambro BCT Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Allocation_array

*     File name:    allocation_array.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This contains the class declaration for the allocation array object
         that allows variable length array elements that are used as user
         keys in the random_file_storage class.

*************************< FOCUSSED GUI DOMAIN >***************************/


// Single include define guardian
#ifndef ALLOCATION_ARRAY_HPP
#define ALLOCATION_ARRAY_HPP

// SYSTEM INCLUDES

#include <string.h>
#include <unistd.h>

// TRIMA INCLUDES

#define  ARR_SUCCESS 1
#define  ARR_FAILURE 0


class Allocation_array
{

private:

   // Unimplemented methods - no null constructor; copies; assignments
   Allocation_array ();
   Allocation_array(Allocation_array const&);
   Allocation_array& operator = (Allocation_array const&);


   // Points to the big malloc of characters that will be treated as an array
   unsigned char* array_memory_ptr;

   // Array size in bytes
   unsigned long array_size;

   // Number of array elements represented by this array
   long num_of_elements;

   // Size of the allocation status field (found in each array element)
   long allocation_status_size;

   // Size of the user_key field (found in each array element)
   long user_key_size;

   // Single element size
   long element_size;



   // CHECK_AND_RETURN_ELEMENT_ADDRESS
   // This method will calculate the actual memory address of the array element
   // referenced by the index. If the index is outside of array bounds, this
   // method returns a NULL.
   //
   unsigned char* check_and_return_element_address (long index);



protected:



public:

   // CONSTRUCTOR
   Allocation_array (long number_of_elements, long allocation_status_field_size, long user_key_field_size);


   // DESTRUCTOR
   virtual ~Allocation_array ();

   // INITIALIZE_ALLOCATION_STATUS
   // This method will set every allocation status field to the value passed
   // by the caller.
   void initialize_allocation_status (const char* allocation_status);


   // SET_ALLOCATION_STATUS
   // This method will set the allocation status in a specific array element.
   // If the index is out of bounds, the routine returns a zero,
   // otherwise it returns a one.
   //
   long set_allocation_status (long index, const char* allocation_status);


   // GET_ALLOCATION_STATUS
   // This method will return the allocation status contained within a virtual array element.
   // The allocation status is passed to the caller. If the index is out of bounds, the routine
   // returns a zero, otherwise it returns a one.
   //
   long get_allocation_status (long index, char* allocation_status);


   // SET_USER_KEY
   // This method will copy the key found within the caller's buffer to an array element upto a length of keybuff_length.
   // The array element index is passed to the caller. If the index is out of bounds, the routine
   // returns a zero, otherwise it returns a one.
   //
   long set_user_key (long index, long keybuff_length, const unsigned char* keybuff);


   // GET_USER_KEY
   // This method will copy the key found within an array element to the caller's buffer upto a length of keybuff_length.
   // The array element index is passed to the caller. If the index is out of bounds, the routine
   // returns a zero, otherwise it returns a one.
   //
   long get_user_key (long index, long keybuff_length, unsigned char* keybuff);


   // GET_SERIALIZED_ARRAY_POINTER
   // This method will return the pointer the virtualized array and the length of the array
   //
   void get_serialized_array_pointer (unsigned long* array_length, unsigned char** array_pointer);


   // LOAD_ARRAY_FROM_FILE
   // This method will copy whatever array representation the caller points to into the
   // the virtual array buffer. This must be a well-formed array buffer saved to and
   // retrieved from a file.
   //
   void load_array_from_file (const unsigned char* serialized_array_pointer);


   // GET_ARRAY_CRC
   // This method will return the a CRC calculated on the value of the array at
   // the time of this call.
   unsigned long get_array_crc ();


};


// End of single include define guardian
#endif

/* FORMAT HASH 913c5f451d2ac6a243be989123d31dba */
