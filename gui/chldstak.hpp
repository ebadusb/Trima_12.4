/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Child_Screen_Stack

*     File name:    chldstak.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the child screen stack on which each active instance
         is placed until either a new Apheresis procedure screen is invoked or the
         child screens are singlely or en masse, removed. This class is also used
         to implement the "stateless" screen stack which is not dependent on system
         state or the underlying procedure screen.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.5  2003/01/06 18:31:06Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.4  2002/11/11 05:24:45  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.3  2002/05/07 13:55:48  sb07663
First cut at VxWorks compatibility
Revision 1.2  1996/07/01 20:20:45  tw09453
Update the file header

*/


// Single include define guardian
#ifndef CHLDSTAK_HPP
#define CHLDSTAK_HPP


// CLASS INCLUDES

#include "chldsele.hpp"


// This structure is used to communicate data between the user of the
// child screen stack and the stack class. Pushing data: fill this
// structure with the items when calling the push routine. Popping data:
// pass a pointer to this structure when calling the pop or peek routine.

struct CHILD_STACK_INFO
{
   // This contains the pointer to the child screen
   Base_Window* child_screen_ptr;

   // This contains the associated allocation parameter for
   //  the above screen.
   guistring alloc_param;
};




class Child_Screen_Stack
{


private:
   // This stack element is used as the "head of stack" anchor element. Its
   // forward element will point to the top of stack element, if one exists.
   // If none exist, the "head of stack" will have a forward pointer of NULL.
   Child_Screen_Stack_Element head_of_stack;


   // This counter keeps track of the number of elements currently on the
   // child screen stack. It is incremented on a push, decremented on a pop,
   // and set to zero when the stack is "cleared".
   int child_stack_count;


public:

   // CONSTRUCTOR
   Child_Screen_Stack() ;


   // DESTRUCTOR
   virtual ~Child_Screen_Stack() ;


   // GET_STACK_COUNT
   // Return the number of screens on the stack
   int get_stack_count () const { return child_stack_count; }


   // PEEK_CHILD_SCREEN
   // This method copies the value of the child screen pointer and
   // its associated allocation parameter found on the top of the stack
   // to the caller's child_stack_info structure..
   // It does not remove or add any elements to the stack however.
   void peek_child_screen (CHILD_STACK_INFO* peek_target);


   // POP_CHILD_SCREEN
   // This method copies the values of the child screen pointer and
   // its associated allocation parameter found on the top of the stack
   // to the caller's child_stack_info structure.
   // It then removes and C++ deallocates the top child stack element.
   void pop_child_screen (CHILD_STACK_INFO* pop_target);


   // PUSH_CHILD_SCREEN
   // This method C++ allocates a stack element and copies the child screen
   // pointer and allocation parameter into the stack element. It then
   // makes the element the top of the stack.
   void push_child_screen (CHILD_STACK_INFO* push_source);


   // CLEAR_CHILD_SCREEN_STACK
   // This method removes each stack element in turn. For each child stack
   // element it performs the following:
   // (1): it "GUI deallocates" the child screen.
   // (2): it C++ "deletes" the (dynamically allocated) child screen itself.
   // (3): it C++ "deletes" the child stack element.
   void clear_child_screen_stack ();


};  // END of Child_Screen_Stack Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 20d5e26aa00fa9e8c52c7409c2bd0032 */
