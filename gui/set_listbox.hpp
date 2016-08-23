/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SET_LISTBOX

*     File name:   set_listbox.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.2  2006/08/03 22:53:03Z  dslausb
Fixed cassette deletion
Revision 1.1  2006/07/28 23:27:33Z  dslausb
Initial revision

*/

// Single include define guardian
#ifndef SET_LISTBOX_HPP
#define SET_LISTBOX_HPP

#include <vxworks.h> // Needed for SIMNT compiler directives in header

/** include files **/
#include "bwindow.hpp"
#include "button_bitmap.h"
#include <vector>


class Screen_SET_LISTBOX
   : public Base_Window
{
protected:
   int _numOfSets,
            _numOfButtons,
            _yCursor,
            _topItem,
            _itemButtonHeight,
            _scrollButtonHeight,
            _listboxWidth;

   short _xPos, _yPos;
   bool  _mustScroll,
         _filterForProcedure,
         _isDeleteListbox;

   vector<guistring*> cassettes;

   Button_Bitmap**    buttons,
   * up_button,
   * up_button_grey,
   * down_button,
   * down_button_grey;

   static void setCassetteToDelete (const char* deleteSet);

   void removeListBox (int sendValue);
   void setButtonFace (Button_Bitmap& button, const guistring& cassetteRef);

public:

   enum LISTBOX_CALLBACK_VAL
   {
      EMPTY,
      CANCEL,
      DELETE_ONE,
      DELETE_ALL,
      SELECT,
   };

   // CONSTRUCTOR
   Screen_SET_LISTBOX();

   // DESTRUCTOR
   virtual ~Screen_SET_LISTBOX();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();
   virtual void process_up_button ();
   virtual void process_down_button ();
   void         process_listbox (int list_position);

   // SET_CALLBACK
   // This method establishes the pointer to the C routine that the Photon
   // subsystem will call when the button is pushed then released.
   void set_callback (void (* callback_routine_ptr) (void*, void*, void*),
                      void* callback_data_ptr);

   void set_the_callbacks (int cbNumber);

   static guistring createAllocationString (int xPos,
                                            bool filterForProcedure,
                                            bool isDeleteListbox);

   #if CPU==SIMNT
   bool isBottomOfList ();
   #endif

}; // END of Screen_SET_LISTBOX Class Declaration

// End of single include define guardian
#endif

/* FORMAT HASH d97151f9f51a85f30e48d7c342e68632 */
