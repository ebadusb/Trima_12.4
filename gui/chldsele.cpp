/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Terry Wahl

*     Class name:  Child_Screen_Stack_Element

*     File name:   chldsele.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/
#include <vxworks.h>

// BASE CLASS INCLUDES
#include "chldsele.hpp"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR

Child_Screen_Stack_Element::Child_Screen_Stack_Element()
   : alloc_param(),
     child_screen_ptr(NULL),
     next_child_stack_element(NULL)
{}



////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR

Child_Screen_Stack_Element::~Child_Screen_Stack_Element()
{}



////////////////////////////////////////////////////////////////////////////////
// SET_FORWARD_LINK

void Child_Screen_Stack_Element::set_forward_link (Child_Screen_Stack_Element* child_screen_stack_ele_ptr)
{
   // set the forward pointer to the next stack element to the parameter passed.
   next_child_stack_element = child_screen_stack_ele_ptr ;

}     // END of Child_Screen_Stack_Element SET_FORWARD_LINK



////////////////////////////////////////////////////////////////////////////////
// GET_CHILD_ALLOCATION_PARAM

void Child_Screen_Stack_Element::get_child_allocation_param (guistring& alloc_buffer)
{
   // copy the allocation buffer to the caller's buffer
   strcpy (alloc_buffer,  alloc_param);

}     // END of Child_Screen_Stack_Element GET_CHILD_ALLOCATION_PARAM


////////////////////////////////////////////////////////////////////////////////
// SET_CHILD_ALLOCATION_PARAM

void Child_Screen_Stack_Element::set_child_allocation_param (const char* child_param)
{
   // copy the allocation buffer to the stack element's buffer
   alloc_param = child_param;

}     // END of Child_Screen_Stack_Element SET_CHILD_ALLOCATION_PARAM



////////////////////////////////////////////////////////////////////////////////
// SET_CHILD_SCREEN_POINTER

void Child_Screen_Stack_Element::set_child_screen_pointer (Base_Window* child_ptr)
{
   // copy the child screen instance's pointer into the stack element
   child_screen_ptr = child_ptr ;

}     // END of Child_Screen_Stack_Element SET_CHILD_SCREEN_POINTER



////////////////////////////////////////////////////////////////////////////////
// LINK_CHILD_STACK_ELEMENT

void Child_Screen_Stack_Element::link_child_stack_element (Child_Screen_Stack_Element* top_of_stack_ptr)
{
   // Get the top-of-the-stack's forward pointer and make it mine.
   set_forward_link (top_of_stack_ptr -> get_forward_link ()) ;

   // Make the top-of-stack point to me -- we are linked.
   top_of_stack_ptr -> set_forward_link (this) ;


}     // END of Child_Screen_Stack_Element LINK_CHILD_STACK_ELEMENT



////////////////////////////////////////////////////////////////////////////////
// UNLINK_CHILD_STACK_ELEMENT

Child_Screen_Stack_Element* Child_Screen_Stack_Element::unlink_child_stack_element (Child_Screen_Stack_Element* top_of_stack_ptr)
{
   // Get my forward pointer and make it the top-of-stack's forward pointer
   // thus linking around me.
   top_of_stack_ptr -> set_forward_link (get_forward_link ()) ;

   // Make my forward link NULL.
   set_forward_link (NULL) ;

   // Return a pointer to me in case the caller needs a handle to me.
   return (this) ;


}     // END of Child_Screen_Stack_Element UNLINK_CHILD_STACK_ELEMENT

/* FORMAT HASH 30164f7b8c4b0bb00d29cf7025ae7d54 */
