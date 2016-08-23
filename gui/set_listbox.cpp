/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2006 Gambro BCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  Screen_SET_LISTBOX

*     File name:   set_listbox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

#include <vxworks.h>

/** include files **/

#include "set_listbox.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"
#include "an_alarm.h"
#include "CatalogChecker.h"
#include "cassette_dat.h"

// Sheesh... some of these values are #defined in all kinds of places, always to
// exactly the same value.  Bad mojo.  Let us guarantee that the value used in
// this file is the correct one.
#ifdef STRING_TOKEN
# undef STRING_TOKEN
#endif

#ifdef BUTTON_SPACING
# undef BUTTON_SPACING
#endif

static const int         MAX_ROWS_DISPLAYED = 5;
static const int         BUTTON_SPACING     = 3;
static const int         HORIZONTAL_MARGIN  = 10;
static const int         VERTICAL_MARGIN    = 5;
static const char* const STRING_TOKEN       = "~";


#if CPU==SIMNT
struct Str_id { TEXT_STRING_ID string_id; const char* str; };
Str_id _cancelItem;
Str_id _removeAllItem;
#endif

///////////////////////////////////////////////////////////////////////////////
// C-style Callback functions to invoke class member functions
//
static void setListUpButtonCB (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_up_button();
}

static void setListDownButtonCB (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_down_button();
}

static void setListButtonCB0 (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_listbox(0);
}

static void setListButtonCB1 (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_listbox(1);
}

static void setListButtonCB2 (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_listbox(2);
}

static void setListButtonCB3 (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_listbox(3);
}

static void setListButtonCB4 (void* data)
{
   ((Screen_SET_LISTBOX*)data)->process_listbox(4);
}


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_SET_LISTBOX::Screen_SET_LISTBOX()
   : Base_Window (BITMAP_NULL),
     _numOfSets(0),
     _numOfButtons(0),
     _yCursor(0),
     _topItem(0),
     _itemButtonHeight(0),
     _scrollButtonHeight(0),
     _listboxWidth(0),
     _xPos(0),
     _yPos(0),
     _mustScroll(false),
     _filterForProcedure(false),
     _isDeleteListbox(false),
     cassettes(),
     buttons(NULL),
     up_button(NULL),
     up_button_grey(NULL),
     down_button(NULL),
     down_button_grey(NULL)
{
   set_screen_id(GUI_SCREEN_SET_LISTBOX);
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_SET_LISTBOX::~Screen_SET_LISTBOX()
{
   Screen_SET_LISTBOX::deallocate_resources ();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_SET_LISTBOX::allocate_resources (const char* allocation_parameter)
{
   if (_allocated) return;

   // Parse allocation string
   {
      guistring allocstr(allocation_parameter);
      char*     tokstr    = allocstr.getBufPtr();
      char*     nextToken = 0;
      _xPos               = (short)atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken));
      _filterForProcedure = (bool)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
      _isDeleteListbox    = (bool)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
   }

   OSRegion regionVar;
   _topItem = 0;

   // If this is a delete box, put in the cancel button.
   if (_isDeleteListbox)
   {
      Display_Text cancelDisplayText(textListboxItemCancel);
      guistring*   cancelString = new guistring();
      cancelDisplayText.get_text_direct(*cancelString);
      cassettes.push_back(cancelString);
#if CPU==SIMNT
      _cancelItem.string_id = textListboxItemCancel;
      _cancelItem.str       = **(cassettes.rbegin());
#endif
   }

   ///////////////////////////////////////////////////////////////////////////////
   for (CASSETTE_VECTOR_ITERATOR it = AdminCassetteDat::begin();
        it != AdminCassetteDat::end();
        ++it)
   {
      const char* refNumStr = (*it)->RefNum();

      if (_filterForProcedure && guiglobs::procedure_selected)
      {
         if ( guiglobs::cdsData.procedures.kitIsCompatibleWithCurrentProcedure(*it) )
         {
            cassettes.push_back(new guistring(refNumStr));
            DataLog (log_level_gui_info) << "Cassette " << refNumStr << " is OK." << endmsg;
         }
      }
      else
      {
         cassettes.push_back(new guistring(refNumStr));
      }

   }

   // If this is a delete box, put in the remove all button.
   if (_isDeleteListbox)
   {
      Display_Text removeAllDisplayText(textListboxItemRemoveAll);
      guistring*   removeAllString = new guistring();
      removeAllDisplayText.get_text_direct(*removeAllString);
      cassettes.push_back(removeAllString);
#if CPU==SIMNT
      _removeAllItem.string_id = textListboxItemRemoveAll;
      _removeAllItem.str       = **(cassettes.rbegin());
#endif
   }

   _numOfSets = cassettes.size();

   GUI_BUTTON_ID button_id_array[] = {GUI_BUTTON_CASSETTE_LISTBOX_1,
                                      GUI_BUTTON_CASSETTE_LISTBOX_2,
                                      GUI_BUTTON_CASSETTE_LISTBOX_3,
                                      GUI_BUTTON_CASSETTE_LISTBOX_4,
                                      GUI_BUTTON_CASSETTE_LISTBOX_5 };

   if (_numOfSets == 0)
   {
      DataLog (log_level_gui_error) << "Attempted to allocate a listbox with zero items." << endmsg;
      removeListBox(EMPTY);
      return;
   }

   if (_numOfSets > MAX_ROWS_DISPLAYED)
   {
      DataLog (log_level_gui_info) << "Number of sets (" << _numOfSets
                                   << ") exceeds max rows displayed (" << MAX_ROWS_DISPLAYED
                                   << "). Scrolling turned on." << endmsg;
      _mustScroll   = true;
      _numOfButtons = MAX_ROWS_DISPLAYED;
   }
   else _numOfButtons = _numOfSets;

   buttons  = (Button_Bitmap**)GUI_POOL_MALLOC(_numOfButtons * sizeof(Button_Bitmap*));

   _yCursor = VERTICAL_MARGIN;

   // If necessary, set up the up button.
   if (_mustScroll)
   {
      up_button_grey = new Button_Bitmap(GUI_BUTTON_CASSETTE_LISTBOX_UP_BUTTON,
                                         HORIZONTAL_MARGIN,
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
   for (int i = 0; i < _numOfButtons; i++)
   {
      buttons[i] = new Button_Bitmap(button_id_array[i],
                                     HORIZONTAL_MARGIN,
                                     _yCursor,
                                     textEmptyFormatSetListboxInit,
                                     CFG_Look::BUTTON_UP_NAME,
                                     CFG_Look::BUTTON_DOWN_NAME);

      setButtonFace(*buttons[i], *cassettes[i]);

#if CPU==SIMNT
      // Use string id here to eliminate missing string ids in simulator
      if ( _cancelItem.str == *cassettes[i])
      {
         buttons[i]->set_string_id(_cancelItem.string_id);
      }
      else if (_removeAllItem.str == *cassettes[i])
      {
         buttons[i]->set_string_id(_removeAllItem.string_id);
      }
#endif

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
      down_button_grey = new Button_Bitmap (GUI_BUTTON_CASSETTE_LISTBOX_DOWN_BUTTON,
                                            HORIZONTAL_MARGIN,
                                            _yCursor,
                                            textEmpty,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN);


      down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       HORIZONTAL_MARGIN,
                                       _yCursor,
                                       textEmpty,
                                       CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                       CFG_Look::BUTTON_SCROLL_DOWN);

      down_button->set_callback (setListDownButtonCB, (void*)this);
   }

   _yPos            = 275 - (_numOfButtons * (_itemButtonHeight / 2));

   regionVar.height = (short)( (_numOfButtons * (_itemButtonHeight + BUTTON_SPACING)) + ( 2 * VERTICAL_MARGIN) ) - BUTTON_SPACING;
   regionVar.width  = _listboxWidth + (2 * HORIZONTAL_MARGIN);
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

   for (int i = 0; i < _numOfButtons; i++)
      buttons[i]->allocate_resources(*this);

   //
   //   Now allocate the base window.  This is done last in this case so that the
   //   dynamic window sizing above is properly effected.
   //
   Base_Window::allocate_resources (allocation_parameter);

}  // END of Screen_SET_LISTBOX ALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_SET_LISTBOX::deallocate_resources ()
{
   if (!_allocated) return;

   delete up_button        ; up_button        = NULL;
   delete up_button_grey   ; up_button_grey   = NULL;
   delete down_button      ; down_button      = NULL;
   delete down_button_grey ; down_button_grey = NULL;

   for (int i = 0; i < _numOfButtons; i++)
   {
      delete buttons[i];
   }

   GUI_POOL_FREE(buttons);
   buttons = NULL;

   for (vector<guistring*>::iterator it = cassettes.begin(); it != cassettes.end(); it++)
   {
      delete *it;
   }
   cassettes.clear();

   Base_Window::deallocate_resources ();

}   // END of Screen_SET_LISTBOX DEALLOCATE_RESOURCES

void Screen_SET_LISTBOX::process_up_button ()
{
   if (_topItem < 1)
   {
      DataLog (log_level_gui_error) << "Error: cannot process up button." << endmsg;
      return;
   }

   _topItem--;

   for (int i = 0; i < _numOfButtons; i++)
   {
      setButtonFace(*buttons[i], *cassettes[i + _topItem]);
   }

   if (down_button == NULL)
   {
      down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       HORIZONTAL_MARGIN,
                                       _yCursor,
                                       textEmpty,
                                       CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                       CFG_Look::BUTTON_SCROLL_DOWN);

      down_button->allocate_resources (*this);
      down_button->set_callback (setListDownButtonCB, (void*)this);
   }

   if (_topItem == 0)
   {
      delete up_button;
      up_button = NULL;
   }
}  // END of Screen_SET_LISTBOX::PROCESS_UP_BUTTON

void Screen_SET_LISTBOX::process_down_button ()
{
   if (_topItem >= _numOfSets - MAX_ROWS_DISPLAYED)
   {
      DataLog (log_level_gui_error) << "Error: cannot process down button." << endmsg;
      return;
   }

   _topItem++;

   for (int i = 0; i < _numOfButtons; i++)
   {
      setButtonFace(*buttons[i], *cassettes[i + _topItem]);

#if CPU==SIMNT
      // Use string id here to eliminate missing string ids in simulator
      if ( _cancelItem.str == *cassettes[i + _topItem])
      {
         buttons[i]->set_string_id(_cancelItem.string_id);
      }
      else if (_removeAllItem.str == *cassettes[i + _topItem])
      {
         buttons[i]->set_string_id(_removeAllItem.string_id);
      }
#endif
   }

   if (up_button == NULL)
   {
      up_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                     HORIZONTAL_MARGIN,
                                     VERTICAL_MARGIN,
                                     textEmpty,
                                     CFG_Look::BUTTON_UP_SCROLL_NAME,
                                     CFG_Look::BUTTON_SCROLL_DOWN);

      up_button->allocate_resources (*this);
      up_button->set_callback (setListUpButtonCB, (void*)this);
   }

   if (_topItem == _numOfSets - MAX_ROWS_DISPLAYED)
   {
      delete down_button;
      down_button = NULL;
   }
} // END of Screen_SET_LISTBOX::PROCESS_DOWN_BUTTON

///////////////////////////////////////////////////////////////////////
//
void Screen_SET_LISTBOX::process_listbox (int index)
{
   int msgVal = SELECT;

   if (_isDeleteListbox)
   {
      if (_topItem == 0 && index == 0)
      {
         msgVal = CANCEL;
      }
      else if (_topItem + index + 1 == _numOfSets)
      {
         msgVal = DELETE_ALL;
      }
      else
      {
         msgVal = DELETE_ONE;
         const char* setToDelete = *cassettes[index + _topItem];
         DataLog (log_level_gui_info) << "This is a DELETE_ONE listbox. (set to delete: " << setToDelete << ")." << endmsg;
         Screen_SET_LISTBOX::setCassetteToDelete(setToDelete);
      }
   }
   else
   {
      const char* newCassette = *cassettes[index + _topItem];
      DataLog (log_level_gui_info) << "Setting new cassette in guiglobs: " << newCassette << endmsg;
      if (!guiglobs::cassette_mgr->LookupCassette(newCassette))
         DataLog (log_level_gui_error) << "Error: Unacceptable listbox item selected." << endmsg;

      DataLog (log_level_gui_info) << "Clearing scanned cassette" << endmsg;
      guiglobs::cassette_mgr->clearScannedCassette();
   }

   removeListBox(msgVal);
}

//
///////////////////////////////////////////////////////////////////
// set the individual callbacks
void Screen_SET_LISTBOX::set_the_callbacks (int cbNumber)
{
   switch (cbNumber)
   {
      case 0 :
      {
         buttons[cbNumber]->set_callback (setListButtonCB0, (void*)this);
         break;
      }
      case 1 :
      {
         buttons[cbNumber]->set_callback (setListButtonCB1, (void*)this);
         break;
      }
      case 2 :
      {
         buttons[cbNumber]->set_callback (setListButtonCB2, (void*)this);
         break;
      }
      case 3 :
      {
         buttons[cbNumber]->set_callback (setListButtonCB3, (void*)this);
         break;
      }
      case 4 :
      {
         buttons[cbNumber]->set_callback (setListButtonCB4, (void*)this);
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

void Screen_SET_LISTBOX::setCassetteToDelete (const char* deleteSet)
{
   guiglobs::cassette_mgr->setCassetteToDelete(deleteSet);
}

guistring Screen_SET_LISTBOX::createAllocationString (int xPos,
                                                      bool filterForProcedure,
                                                      bool isDeleteListbox)
{
   guistring allocationString;

   sprintf(allocationString, "%d~%d~%d",
           xPos,
           (int)filterForProcedure,
           (int)isDeleteListbox);

   return allocationString;
}

void Screen_SET_LISTBOX::removeListBox (int sendValue)
{
   DataLog (log_level_gui_info) << "Sending SetListboxItemChosen message" << endmsg;
   SetListboxItemChosen setListboxItemChosenMsg(MessageBase::SEND_LOCAL);
   setListboxItemChosenMsg.send(sendValue);

   // Tell the window manager to remove the most recent child screen.
   RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
   eventmsg.send();
}


void Screen_SET_LISTBOX::setButtonFace (Button_Bitmap& button, const guistring& cassetteRef)
{
   BITMAP_ID bmid_up, bmid_dn;

   // Look up a potentially reference-coded bitmaps for up and down states
   CFG_Look::lookupListButtonBitMapByCassetteRefNum(cassetteRef, bmid_up, bmid_dn);

   // Apply the up/down bitmap as needed
   if (bmid_dn != button.get_down_bitmap_id())
      button.set_down_bitmap(bmid_dn);
   if (bmid_up != button.get_up_bitmap_id())
      button.set_up_bitmap(bmid_up);

   // Set the button text
   button.set_text_direct(cassetteRef.length(), cassetteRef);
}


#if CPU==SIMNT
bool Screen_SET_LISTBOX::isBottomOfList ()
{
   return (down_button == NULL);
}
#endif

/* FORMAT HASH ec2a521628a59104c79a6f0f882ba753 */
