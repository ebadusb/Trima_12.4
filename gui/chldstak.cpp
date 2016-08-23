/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Child_Screen_Stack

*     File name:   chldstak.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: K:/BCT_Development/vxWorks/Trima/gui/rcs/chldstak.cpp 1.9 2003/01/06 18:31:06Z jl11312 Exp jl11312 $
$Log: chldstak.cpp $
Revision 1.9  2003/01/06 18:31:06Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.8  2002/12/05 14:00:24  jl11312
- removed calls to ZafApplication::Flush, since they were causing recursive calls through the Zinc event handler
Revision 1.7  2002/11/25 18:38:28  jl11312
- fixed some problems with buttons, display text, and config screens
Revision 1.6  2002/11/11 05:14:46Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.5  2002/08/16 16:31:27  sb07663
added <vxworks.h> include
Revision 1.4  2002/05/07 13:55:48  sb07663
First cut at VxWorks compatibility
Revision 1.3  1997/06/12 22:24:17  tw09453
PtFlush after the screen deallocate in order to serialize Photon
processing BEFORE the C++ delete of the screen. Possible race
condition between C++ memory accounting for Photon resources
and the freeing of those resources.
Revision 1.2  1996/07/01 20:20:41  tw09453
Update the file header

*/
#include <vxworks.h>


// BASE CLASS INCLUDES
#include "chldstak.hpp"
#include "chldsele.hpp"


// EXTERNAL REFERENCES
#include "guiglobs.hpp"

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Child_Screen_Stack::Child_Screen_Stack()
{

   // Label the head of stack anchor element for dump identification
   head_of_stack.set_child_allocation_param ("CHILD STACK ANCHOR ELEMENT");

   // Initialize the stack count to zero.
   child_stack_count = 0;

}    // END of Child_Screen_Stack CONSTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Child_Screen_Stack::~Child_Screen_Stack()
{
   // Ensure that no screens or stack elements are left hanging (delete 'em)
   clear_child_screen_stack ();

}    // END of Child_Screen_Stack DESTRUCTOR



////////////////////////////////////////////////////////////////////////////////
// PEEK_CHILD_SCREEN

void Child_Screen_Stack::peek_child_screen (CHILD_STACK_INFO* peek_target)
{
   // determine if there is an item on the stack to peek at
   Child_Screen_Stack_Element* child_ptr = head_of_stack.get_forward_link() ;

   if (child_ptr != NULL)
   {
      // copy the child screen ptr to the caller's structure
      (peek_target -> child_screen_ptr) = (child_ptr -> get_child_screen_pointer ()) ;

      // copy the child screen allocation to the caller's structure
      child_ptr -> get_child_allocation_param (peek_target -> alloc_param) ;
   }
   else    // if the PEEK occurs on an empty stack
   {
      // make the child screen pointer NULL
      (peek_target -> child_screen_ptr) = NULL ;

      // Null out the allocation parameter
      (peek_target -> alloc_param).initialize() ;
   }

}    // END of Child_Screen_Stack PEEK_CHILD_SCREEN



////////////////////////////////////////////////////////////////////////////////
// POP_CHILD_SCREEN

void Child_Screen_Stack::pop_child_screen (CHILD_STACK_INFO* pop_target)
{
   // Use peek to copy the info to the caller
   peek_child_screen (pop_target);

   // get the address of the top of stack element (NULL == stack is empty)
   Child_Screen_Stack_Element* child_ptr = head_of_stack.get_forward_link() ;

   // If it exists, remove the top of stack element,
   if (child_ptr != NULL)
   {
      // unlink the top of stack child element from the head.
      child_ptr -> unlink_child_stack_element (&head_of_stack);
      child_stack_count -= 1;

      // give the memory back to the HEAP.
      delete child_ptr;
      child_ptr = NULL;
   }

}    // END of Child_Screen_Stack POP_CHILD_SCREEN




////////////////////////////////////////////////////////////////////////////////
// PUSH_CHILD_SCREEN

void Child_Screen_Stack::push_child_screen (CHILD_STACK_INFO* push_source)
{
   // allocate a stack element and copy the info from push_source into it
   Child_Screen_Stack_Element* child_ptr = new Child_Screen_Stack_Element ;

   // copy in the allocation parameter string
   child_ptr -> set_child_allocation_param (push_source -> alloc_param);

   // copy in the child screen pointer
   child_ptr -> set_child_screen_pointer (push_source -> child_screen_ptr);

   // link (push) the newly populated child stack element to the head of stack
   child_ptr -> link_child_stack_element (&head_of_stack) ;
   child_stack_count += 1;

}    // END of Child_Screen_Stack PUSH_CHILD_SCREEN




////////////////////////////////////////////////////////////////////////////////
// CLEAR_CHILD_SCREEN_STACK

void Child_Screen_Stack::clear_child_screen_stack ()
{

   // create a stack info instance to hold child THE screen pointer.
   // this pointer will be used to C++ deallocate the screen.
   CHILD_STACK_INFO local_info ;

   while (head_of_stack.get_forward_link() != NULL)
   {
      // return the pointer to the child screen and C++ deallocate the stack
      // element.
      pop_child_screen (&local_info) ;

      if (local_info.child_screen_ptr != NULL)
      {
         // Give the child screen a chance to clean up any resources
         local_info.child_screen_ptr -> deallocate_resources ();

         // Return the child screen object to the great HEAP.
         delete local_info.child_screen_ptr;
         local_info.child_screen_ptr = NULL;
      }
   }

   child_stack_count = 0;

}    // END of Child_Screen_Stack CLEAR_CHILD_SCREEN_STACK

/* FORMAT HASH 0d25f611b9e640ab9a3a2f53f9e92227 */
