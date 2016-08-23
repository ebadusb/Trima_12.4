/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2008 GAMBRO BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  DynamicListbox

*     File name:   dynamicListbox.cpp

*     Contents:    Source file for dynamic listbox class

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

#include <vxworks.h>

/** include files **/

#include "dynamicListbox.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"
#include "an_alarm.h"
#include "CatalogChecker.h"
#include "cfg_mach.hpp"

#define __MAX_ROWS_DISPLAYED__ 5
#define __BUTTON_SPACING__ 3

/** defines **/
#define  STRING_TOKEN            "~"
#define  __HORIZONTAL_MARGIN__   10
#define  __VERTICAL_MARGIN__     5
#define  BUTTON_SPACING          3

void dynamicListboxBtnCB0 (void* data);
void dynamicListboxBtnCB1 (void* data);
void dynamicListboxBtnCB2 (void* data);
void dynamicListboxBtnCB3 (void* data);
void dynamicListboxBtnCB4 (void* data);
void dynamicListboxUpBtnCB (void* data);
void dynamicListboxDownBtnCB (void* data);

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
DynamicListbox::DynamicListbox(GUI_SCREEN_ID screenId, DynamicListboxVector* listboxVector)
   : Base_Window (BITMAP_NULL),

     _numberOfItems      (0),
     _numberOfButtons    (0),
     _yCursor            (0),
     _topItem            (0),
     _xPos               (0),
     _yPos               (0),
     _itemButtonHeight   (0),
     _scrollButtonHeight (0),
     _listboxWidth       (0),

     _mustScroll (false),

     buttons          (NULL),
     up_button        (NULL),
     up_button_grey   (NULL),
     down_button      (NULL),
     down_button_grey (NULL),

     _listboxItems(listboxVector)
{
   set_screen_id(screenId);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
DynamicListbox::~DynamicListbox()
{
   DynamicListbox::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void DynamicListbox::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;
   if (!_listboxItems) _FATAL_ERROR(__FILE__, __LINE__, "Bad listbox vector pointer");

   // Parse allocation string
   {
      guistring allocstr(allocation_parameter);
      char*     tokstr    = allocstr.make_buffer();
      char*     nextToken = 0;
      _xPos = atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken));
      GUI_POOL_FREE(tokstr);
   }

   OSRegion regionVar;
   _topItem       = 0;

   _numberOfItems = _listboxItems->size();

   GUI_BUTTON_ID button_id_array[] = {GUI_BUTTON_DYNAMIC_LISTBOX_1,
                                      GUI_BUTTON_DYNAMIC_LISTBOX_2,
                                      GUI_BUTTON_DYNAMIC_LISTBOX_3,
                                      GUI_BUTTON_DYNAMIC_LISTBOX_4,
                                      GUI_BUTTON_DYNAMIC_LISTBOX_5 };

   if (_numberOfItems == 0)
   {
      DataLog (log_level_gui_error) << "Attempted to allocate a listbox with zero items." << endmsg;
      removeListBox(-1);
      return;
   }

   if (_numberOfItems > __MAX_ROWS_DISPLAYED__)
   {
      DataLog (log_level_gui_info) << "Number of sets (" << _numberOfItems
                                   << ") exceeds max rows displayed (" << __MAX_ROWS_DISPLAYED__
                                   << "). Scrolling turned on." << endmsg;
      _mustScroll      = true;
      _numberOfButtons = __MAX_ROWS_DISPLAYED__;
   }
   else _numberOfButtons = _numberOfItems;

   buttons  = (Button_Bitmap**)GUI_POOL_MALLOC(_numberOfButtons * sizeof(Button_Bitmap*));

   _yCursor = __VERTICAL_MARGIN__;

   // If necessary, set up the up button.
   if (_mustScroll)
   {
      up_button_grey = new Button_Bitmap(GUI_BUTTON_CASSETTE_LISTBOX_UP_BUTTON,
                                         __HORIZONTAL_MARGIN__,
                                         _yCursor,
                                         textEmpty,
                                         CFG_Look::BUTTON_SCROLL_GRAY_UP,
                                         CFG_Look::BUTTON_SCROLL_GRAY_UP);

      up_button_grey->allocate_resources(*this);
      up_button_grey->get_region(regionVar);
      up_button_grey->deallocate_resources();

      _scrollButtonHeight = regionVar.height;
      _listboxWidth       = regionVar.width;

      _yCursor           += _scrollButtonHeight + BUTTON_SPACING;
   }

   // Set up the other buttons.
   for (int i = 0; i < _numberOfButtons; i++)
   {
      buttons[i] = new Button_Bitmap(button_id_array[i],
                                     __HORIZONTAL_MARGIN__,
                                     _yCursor,
                                     textEmptyFormatSetListboxInit,
                                     CFG_Look::BUTTON_UP_NAME,
                                     CFG_Look::BUTTON_DOWN_NAME);

      set_the_callbacks(i);

      buttons[i]->allocate_resources(*this);
      buttons[i]->get_region(regionVar);
      buttons[i]->deallocate_resources();

      _yCursor += (regionVar.height + BUTTON_SPACING);
      if (_itemButtonHeight < regionVar.height) _itemButtonHeight = regionVar.height;
      if (_listboxWidth < regionVar.width) _listboxWidth = regionVar.width;
   }

   // If necessary, set up the down button.
   if (_mustScroll)
   {
      down_button_grey = new Button_Bitmap (GUI_BUTTON_DYNAMIC_LISTBOX_DOWN_BUTTON,
                                            __HORIZONTAL_MARGIN__,
                                            _yCursor,
                                            textEmpty,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN);


      down_button = new Button_Bitmap (GUI_BUTTON_DYNAMIC_LISTBOX_DOWN_BUTTON,
                                       __HORIZONTAL_MARGIN__,
                                       _yCursor,
                                       textEmpty,
                                       CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                       CFG_Look::BUTTON_SCROLL_DOWN);

      down_button->set_callback (dynamicListboxDownBtnCB, (void*)this);
   }

   updateListboxStrings();

   _yPos            = 275 - (_numberOfButtons * (_itemButtonHeight / 2));

   regionVar.height = (short)( (_numberOfButtons * (_itemButtonHeight + BUTTON_SPACING)) + ( 2 * __VERTICAL_MARGIN__) ) - BUTTON_SPACING;
   regionVar.width  = _listboxWidth + (2 * __HORIZONTAL_MARGIN__);
   regionVar.x      = _xPos;
   regionVar.y      = _yPos;

   if (_mustScroll)
   {
      regionVar.height += (2 * (_scrollButtonHeight + BUTTON_SPACING));
      regionVar.y       = 110;
   }

   set_region(regionVar);

   // Allocate all the buttons and such.
   if (_mustScroll)
   {
      up_button_grey->allocate_resources (*this);
      down_button_grey->allocate_resources (*this);
      down_button->allocate_resources (*this);
   }

   for (int i = 0; i < _numberOfButtons; i++)
      buttons[i]->allocate_resources(*this);

   //
   //   Now allocate the base window.  This is done last in this case so that the
   //   dynamic window sizing above is properly effected.
   //
   Base_Window::allocate_resources (allocation_parameter);

}  // END of DynamicListbox ALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void DynamicListbox::deallocate_resources ()
{
   if (!_allocated) return;

    #define KILL_POINTER(__POINTER__)   \
   if ((__POINTER__) != NULL)      \
   {                               \
      delete (__POINTER__);       \
      (__POINTER__) = NULL;       \
   }

   KILL_POINTER(up_button);
   KILL_POINTER(up_button_grey);
   KILL_POINTER(down_button);
   KILL_POINTER(down_button_grey);

   for (int i = 0; i < _numberOfButtons; i++)
      KILL_POINTER(buttons[i]);

   GUI_POOL_FREE(buttons);
   buttons = NULL;

   Base_Window::deallocate_resources ();

    #undef KILL_POINTER
}   // END of DynamicListbox DEALLOCATE_RESOURCES

//
///////////////////////////////////////////////////////////////////////////////
// callback for up button
void dynamicListboxUpBtnCB (void* data)
{
   ((DynamicListbox*)data)->processUpButton();
}

void DynamicListbox::processUpButton ()
{
   if (_topItem < 1)
   {
      DataLog (log_level_gui_error) << "Error: cannot process up button." << endmsg;
      return;
   }

   _topItem--;

   updateListboxStrings();

   if (down_button == NULL)
   {
      down_button = new Button_Bitmap (GUI_BUTTON_DYNAMIC_LISTBOX_DOWN_BUTTON,
                                       __HORIZONTAL_MARGIN__,
                                       _yCursor,
                                       textEmpty,
                                       CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                       CFG_Look::BUTTON_SCROLL_DOWN);

      down_button->allocate_resources (*this);
      down_button->set_callback (dynamicListboxDownBtnCB, (void*)this);
   }

   if (_topItem == 0)
   {
      delete up_button;
      up_button = NULL;
   }
}  // END of DynamicListbox::PROCESS_UP_BUTTON

//
///////////////////////////////////////////////////////////////////////////////
// callback for down button
void dynamicListboxDownBtnCB (void* data)
{
   ((DynamicListbox*)data)->processDownButton();
}

void DynamicListbox::processDownButton ()
{
   if (_topItem >= _numberOfItems - __MAX_ROWS_DISPLAYED__)
   {
      DataLog (log_level_gui_error) << "Error: cannot process down button." << endmsg;
      return;
   }

   _topItem++;

   updateListboxStrings();

   if (up_button == NULL)
   {
      up_button = new Button_Bitmap (GUI_BUTTON_DYNAMIC_LISTBOX_UP_BUTTON,
                                     __HORIZONTAL_MARGIN__,
                                     __VERTICAL_MARGIN__,
                                     textEmpty,
                                     CFG_Look::BUTTON_UP_SCROLL_NAME,
                                     CFG_Look::BUTTON_SCROLL_DOWN);

      up_button->allocate_resources (*this);
      up_button->set_callback (dynamicListboxUpBtnCB, (void*)this);
   }

   if (_topItem == _numberOfItems - __MAX_ROWS_DISPLAYED__)
   {
      delete down_button;
      down_button = NULL;
   }
} // END of DynamicListbox::PROCESS_DOWN_BUTTON

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void dynamicListboxBtnCB0 (void* data)
{
   ((DynamicListbox*)data)->removeListBox(0);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void dynamicListboxBtnCB1 (void* data)
{
   ((DynamicListbox*)data)->removeListBox(1);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void dynamicListboxBtnCB2 (void* data)
{
   ((DynamicListbox*)data)->removeListBox(2);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void dynamicListboxBtnCB3 (void* data)
{
   ((DynamicListbox*)data)->removeListBox(3);
}

//
///////////////////////////////////////////////////////////////////////////////
// callback for listbox buttons
void dynamicListboxBtnCB4 (void* data)
{
   ((DynamicListbox*)data)->removeListBox(4);
}


//
///////////////////////////////////////////////////////////////////
// set the individual callbacks
void DynamicListbox::set_the_callbacks (int cbNumber)
{
   switch (cbNumber)
   {
      case 0 :
      {
         buttons[cbNumber]->set_callback (dynamicListboxBtnCB0, (void*)this);
         break;
      }
      case 1 :
      {
         buttons[cbNumber]->set_callback (dynamicListboxBtnCB1, (void*)this);
         break;
      }
      case 2 :
      {
         buttons[cbNumber]->set_callback (dynamicListboxBtnCB2, (void*)this);
         break;
      }
      case 3 :
      {
         buttons[cbNumber]->set_callback (dynamicListboxBtnCB3, (void*)this);
         break;
      }
      case 4 :
      {
         buttons[cbNumber]->set_callback (dynamicListboxBtnCB4, (void*)this);
         break;
      }
      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "did not find the callback" << endmsg;
         break;
      }
   }   // end of switch
}  // end of method

void DynamicListbox::createAllocationString (guistring& allocString, int xPos)
{
   allocString.initialize();
   sprintf(allocString, "%d", xPos);
}

void DynamicListbox::removeListBox (int listboxBtnIndex)
{
   DataLog (log_level_gui_info) << "Sending SetListboxItemChosen message" << endmsg;

   if (listboxBtnIndex >= 0)
   {
      Message<DynamicListboxItem*> listboxReturnMsg(MessageBase::SEND_LOCAL);
      listboxReturnMsg.send((*_listboxItems)[listboxBtnIndex + _topItem]);
   }

   // Tell the window manager to remove the most recent child screen.
   RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
   eventmsg.send();
}

void DynamicListbox::updateListboxStrings ()
{
   for (int i = 0; i < _numberOfButtons; i++)
   {
      buttons[i]->set_text_direct((*_listboxItems)[i + _topItem]->getTextItemPtr());
   }
}


// SET_CALLBACK
// This method establishes the pointer to the C routine that the Photon
// subsystem will call when the button is pushed then released.
void DynamicListbox::set_callback (void (* callback_routine_ptr) (void*, void*, void*),
                                   void* callback_data_ptr)
{
   // todo: do something here.
}

/* FORMAT HASH 3350e4758a7983d8be16fb1548a9595d */
