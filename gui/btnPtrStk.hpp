/*
**************************************************************************
*	Copyright (c) 2008 by CaridianBCT, Inc.  All rights reserved.
*
*	Author: Dan Slauson
*
*	Description: This is included only if compiled for simulator.
*	It's a data structure to keep track of button pointers by button ID.
*	It's managed as an array of lists of button pointers.
*
*	Each button ID gets it's own list of pointers, since there can
*	be multiple copies of the same button.  Whenever a button is
*	constructed, it pushes its pointer onto the back of the list.
*
*	Whenver a button has its allocate_resources() called, its
*	pointer gets moved to the back as well.  That way, we can hopefully
*	always count on having the pointer to the most recently active button
*	at the back of the list.  When a button is destroyed, its pointer
*	gets removed from the list.
*
*	Again, all of this happens only in the simulator, with the purpose
*	of being able to get a pointer to a button from that button's ID,
*	and then being able to get the button's coordinates from that pointer.
*
**************************************************************************
*	Change Log
*	----------
*	$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/btnPtrStk.hpp 1.1 2008/08/22 15:56:48Z dslausb Exp $
*
*	Changes: $Log: btnPtrStk.hpp $
*
**************************************************************************/


#ifndef __BUTTON_POINTER_STACK_HPP__
#define __BUTTON_POINTER_STACK_HPP__

#include <list>

// Forward declaration of button class
class Button_Bitmap;

class BtnPointerStack
{
private:

   // The actual storage data structure: an array of lists
   std::list<Button_Bitmap*> ptrStack[GUI_BUTTON_TOTAL_BUTTONS];

public:

   // Push a pointer onto the back of the list at array location [id]
   // (making sure there aren't multiple copies of the pointer in the
   // list)
   // Called at button construction and allocate_resources()
   void push (GUI_BUTTON_ID id, Button_Bitmap* ptr)
   {
      // Make sure there are no other instances of this pointer
      ptrStack[id].remove(ptr);

      // Put this pointer on the end.
      ptrStack[id].push_back(ptr);

   }

   // Remove the passed pointer from the list at array location [id]
   // (This isn't technically a "pop" because the item may not be at
   // the back of the list, but whatever).
   // Called at button destruction
   void pop (GUI_BUTTON_ID id, Button_Bitmap* ptr)
   {
      // Remove any instances of this pointer
      ptrStack[id].remove(ptr);
   }

   // Return the last item in the list at array location [id]
   // This should, in theory, be the most recently allocated
   // button with this button ID.
   Button_Bitmap* getPtr (GUI_BUTTON_ID id)
   {
      if (ptrStack[id].empty()) return NULL;
      else return ptrStack[id].back();
   }
};


#endif // #ifndef __BUTTON_POINTER_STACK_HPP__

/* FORMAT HASH 606d54d86dd36863985338ad1de682db */
