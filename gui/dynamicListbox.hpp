/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 GAMBRO BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  DynamicListbox

*     File name:   dynamicListbox.hpp

*     Contents:    Header file for dynamic listbox class

*     Description: This is a listbox class that you can use when you're not
        sure up front how many items you will have.  When you construct it,
        you must provide it with:
            - A value from the GUI_SCREEN_ID enumeration so that the screen
                commutator can identify it uniquely
            - A pointer to a DynamicListboxVector, which is a vector of
                DynamicListboxItem pointers.
        Then, on construction, the allocation parameter should contain an
        x-coordinate.  The y-coordinate is dynamic, depending on how many
        items are in the vector.  The listbox is centered vertically.
        If there are more than five items in the vector, the list will have
        up and down arrows for scrolling.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include define guardian
#ifndef _DYNAMIC_LISTBOX_HPP_
#define _DYNAMIC_LISTBOX_HPP_

/** include files **/
#include <vector>


#include "button_bitmap.h"
#include "dynamicListboxItem.hpp"
#include "bwindow.hpp"

class DynamicListbox
   : public Base_Window
{
protected:
   int _numberOfItems,
            _numberOfButtons,
            _yCursor,
            _topItem,
            _itemButtonHeight,
            _scrollButtonHeight,
            _listboxWidth;

   short                 _xPos, _yPos;

   bool                  _mustScroll;

   DynamicListboxVector* _listboxItems;

   Button_Bitmap**       buttons,
   * up_button,
   * up_button_grey,
   * down_button,
   * down_button_grey;

   virtual void updateListboxStrings ();

public:

   // CONSTRUCTOR
   DynamicListbox(GUI_SCREEN_ID screenId, DynamicListboxVector* listboxVector);

   // DESTRUCTOR
   virtual ~DynamicListbox();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   virtual void processUpButton ();
   virtual void processDownButton ();

   virtual void removeListBox (int sendValue);

   // SET_CALLBACK
   // This method establishes the pointer to the C routine that the Photon
   // subsystem will call when the button is pushed then released.
   virtual void set_callback (void (* callback_routine_ptr) (void*, void*, void*),
                              void* callback_data_ptr);

   virtual void set_the_callbacks (int cbNumber);

   static void createAllocationString (guistring& allocString, int xPos); // Note: Object is dynamic in vertical position, feeding it a y parameter makes no sense.
private:
   DynamicListbox();
   DynamicListbox(const DynamicListbox&);
   DynamicListbox& operator = (const DynamicListbox&);

}; // END of DynamicListbox Class Declaration

#endif // End of single include define guardian

/* FORMAT HASH d3bfde022f1fa428ea15728edd3fc98f */
