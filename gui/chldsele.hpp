/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Child_Screen_Stack_Element

*     File name:    chldsele.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the elements on the child screen stack or stateless
         screen stack each of which represent an active child/stateless screen
         overlayed on the current apheresis dialog screen.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.4  2002/11/11 05:24:19  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.3  2002/05/07 13:55:47  sb07663
First cut at VxWorks compatibility
Revision 1.2  1996/07/01 20:20:38  tw09453
Update the file header

*/


// Single include define guardian
#ifndef CHLDSELE_HPP
#define CHLDSELE_HPP

#include "bwindow.hpp"
#include "guistring.h"

class Child_Screen_Stack_Element
{
private:

   // This is storage for the allocation parameter string passed to the child window
   // at allocation time.
   guistring alloc_param;

   // This is the Base Window pointer to the child screen. All child screens are
   // derived from the Base Window.
   Base_Window* child_screen_ptr ;

   // This is the forward link to a more senior child stack element on the stack
   Child_Screen_Stack_Element* next_child_stack_element ;

public:

   // CONSTRUCTOR
   Child_Screen_Stack_Element() ;


   // DESTRUCTOR
   virtual ~Child_Screen_Stack_Element() ;


   // GET_CHILD_ALLOCATION_PARAM
   // This method copies the allocation parameter to the buffer
   // location passed as an argument.
   void get_child_allocation_param (guistring& alloc_buffer) ;


   // GET_CHILD_SCREEN_POINTER
   // This method returns a pointer to the screen represented by this
   // stack element.
   Base_Window* get_child_screen_pointer () { return child_screen_ptr; }

   // GET_FORWARD_LINK
   // This method returns a pointer to the next (more senior) element in the
   // child stack. If this is the end of the stack, it returns a NULL pointer.
   Child_Screen_Stack_Element* get_forward_link () { return next_child_stack_element; }


   // SET_CHILD_ALLOCATION_PARAM
   // This method copies the child screen allocation parameter into the stack
   // element. This allocation parameter will be re-used by the caller to
   // "re-allocate" the screen.
   void set_child_allocation_param (const char* child_param) ;


   // SET_CHILD_SCREEN_POINTER
   // This method saves a copy of the pointer to the child screen in the
   // child screen stack element.
   void set_child_screen_pointer (Base_Window* child_screen_ptr) ;


   // SET_FORWARD_LINK
   // This method will save a copy of a pointer to child screen stack element
   // in this child screen stack element.
   void set_forward_link (Child_Screen_Stack_Element* child_screen_stack_ele_ptr) ;


   // LINK_CHILD_STACK_ELEMENT
   // This method links this instance under the instance passed as a parameter.
   // The instance passed as a parameter will typically be the fixed
   // "head of chain" instance.
   void link_child_stack_element (Child_Screen_Stack_Element* top_of_stack_ptr) ;


   // UNLINK_CHILD_STACK_ELEMENT
   // This method unlinks this instance from the child screen stack and returns
   // a pointer to this instance.
   Child_Screen_Stack_Element* unlink_child_stack_element (Child_Screen_Stack_Element* top_of_stack_ptr);

};  // END of Child_Screen_Stack_Element Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH ab949533c040d82a9217665ec853f613 */
