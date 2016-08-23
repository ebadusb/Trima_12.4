/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_LISTBOX

*     File name:   listbox.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/listbox.cpp 1.48 2009/04/07 20:40:30Z dslausb Exp dslausb $
$Log: listbox.cpp $
Revision 1.48  2009/04/07 20:40:30Z  dslausb
IT 9101 - Where possible GUI_POOL_MALLOC
Revision 1.47  2008/07/14 20:28:15Z  dslausb
More stringId changes requested by localization
Revision 1.46  2008/07/09 23:50:57Z  dslausb
More string ID standardization as requested by Marita
Revision 1.45  2008/07/07 18:26:22Z  dslausb
More localization string id standardization
Revision 1.44  2008/02/18 20:18:24Z  dslausb
Updates for string.info files
Revision 1.43  2006/12/19 17:17:45Z  dslausb
Revision 1.42  2006/09/11 19:22:52Z  dslausb
Revision 1.41  2006/06/05 22:48:59Z  dslausb
Removed Fixed Volume
Revision 1.40  2003/04/04 17:58:43Z  rm10919
Datalog changes. IT 5818
Revision 1.39  2003/02/26 17:22:50Z  rm10919
IT 5897 - modified behavior to update list with down arrow bitmap correctly.
Revision 1.38  2003/02/06 17:22:24Z  rm10919
Added set_text_language call for cfg_lang listbox for language listing in own spelling.
Revision 1.37  2003/01/06 18:31:15Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.36  2002/11/27 20:24:48  jl11312
- corrected handling of up button
Revision 1.35  2002/11/25 18:38:30  jl11312
- fixed some problems with buttons, display text, and config screens
Revision 1.34  2002/11/11 06:36:20Z  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.32  2002/09/27 17:03:38  sb07663
derived off bwindow; allocation order changes for proper display
Revision 1.31  2002/08/16 16:34:35  sb07663
added <vxworks.h> include
Revision 1.30  2002/08/06 18:58:29  ms10234
Changed the int messages to end with "Msg"
Revision 1.29  2002/07/19 13:58:01  rm70006
Forgot to add #include for an_alarm.
Revision 1.28  2002/07/18 22:05:48Z  rm70006
Ported to use Zinc.
Revision 1.27  2000/06/06 19:35:29Z  ms10234
Messages were moved from the common project to the trima project
Revision 1.26  1999/05/21 17:59:05  SB07663
removed callbacks for grey buttons
Revision 1.25  1998/01/07 01:21:30  tw09453
During the allocation of language buttons, each language is set in
its native format. The wrong class method in disptxt was being
called causing the languages to be mixed up on listbox buttons.
This call (in 3 places) has been  replaced with the appropriate call to
the Set_Configuration_Language method (replacing the
Set_Language method call).
Revision 1.24  1997/12/15 19:55:45  BarryFulkerson
- re-ordered language list to match gui.h's list to clarify printf's during debug.
Revision 1.23  1997/11/24 23:12:36  BarryFulkerson
- added comments
Revision 1.22  1997/06/02 18:11:58  Barry_Fulkerson
- changed button id reference
Revision 1.21  1997/03/14 21:15:07  Barry_Fulkerson
- removed printf's
Revision 1.20  1997/03/13 21:13:40  Barry_Fulkerson
- added () to deallocates. per warning message
Revision 1.19  1997/03/12 18:35:21  Barry_Fulkerson
- added gray'ed out scroll buttons to listbox.
Revision 1.18  1997/03/07 19:42:14  Barry_Fulkerson
- fixed lower scroll button placement
Revision 1.17  1997/03/06 01:55:08  Barry_Fulkerson
- added the ability to display each language button in its own language
Revision 1.16  1997/03/05 23:02:56  Barry_Fulkerson
- aligned the scroll buttons with the button list
Revision 1.15  1997/03/04 00:54:15  Barry_Fulkerson
-  was not specfying the correct down bitmap for the label button
Revision 1.14  1997/03/03 22:30:25  Barry_Fulkerson
- this time I am really adding the previous changes
Revision 1.13  1997/03/03 22:04:21  Barry_Fulkerson
- changed the reference to the new config scroll buttons to include changes
  from Christelle @ Hauser
Revision 1.12  1997/02/26 01:53:50  Barry_Fulkerson
- added dealloc on down button when bottom of list is reached
Revision 1.10  1997/02/21 22:22:14  Barry_Fulkerson
- added clean-up on listbox, so listbox's would not overrun screen.  This is
  temp.
Revision 1.9  1996/10/24 06:14:13  Barry_Fulkerson
Revision 1.8  1996/10/23 22:45:31  Barry_Fulkerson
- added BUTTON_ID's to buttons
Revision 1.7  1996/10/17 02:22:53  Barry_Fulkerson
- increased number of callbacks for listbox
Revision 1.6  1996/10/15 21:28:10  Barry_Fulkerson
- removed keypad callback from cfg_lang
- added callback delete to cfg_proc
- removed printf's
Revision 1.5  1996/10/15 19:03:53  Barry_Fulkerson
-removed a printf
Revision 1.4  1996/10/14 23:28:45  Barry_Fulkerson
- changed listbox from Photon style to an actual list of buttons
- added x,y location capability to listbox
Revision 1.3  1996/10/09 23:35:12  Barry_Fulkerson
- changed listbox to return CONFIG_KEY types instead of chars
Revision 1.2  1996/10/07 19:52:57  Barry_Fulkerson
added File Title Block comment section and MKS keywords


*/
#include <vxworks.h>

/** include files **/

#include "listbox.hpp"
#include "guiglobs.hpp"
#include "trimamessages.h"
#include "cfg_look.h"
#include "an_alarm.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
Screen_LISTBOX::Screen_LISTBOX()
   : Base_Window (BITMAP_NULL),
     save_key_name(LAST_CONFIG_INDEX_KEY),
     no_of_btns(0),
     no_of_items(0),
     index(0),
     y_pos(0),
     listbox_X_pos(0),
     listbox_Y_pos(0),
     up_button(NULL),
     up_button_grey(NULL),
     down_button(NULL),
     down_button_grey(NULL),
     index_array(),
     listname_fetcher(),
     button_array()
{
   Base_Window::set_screen_id(GUI_SCREEN_LISTBOX);
   memset(p_buttons, 0, sizeof(p_buttons));

}  // END of Screen_LISTBOX CONSTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
Screen_LISTBOX::~Screen_LISTBOX()
{
   Screen_LISTBOX::deallocate_resources ();

}  // END of Screen_LISTBOX DESTRUCTOR

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE_RESOURCES
void Screen_LISTBOX::allocate_resources (const char* allocation_parameter)
{
   guistring allocstr(allocation_parameter);
   char*     tokstr    = allocstr.make_buffer();

   char*     nextToken = 0;
   save_key_name = CONFIG_DATA_KEYS (atoi(strtok_r(tokstr, STRING_TOKEN, &nextToken)));
   listbox_X_pos = (short)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
   listbox_Y_pos = (short)atoi(strtok_r(NULL, STRING_TOKEN, &nextToken));
   GUI_POOL_FREE(tokstr);

   set_region(OSRegion(listbox_X_pos, listbox_Y_pos, 1, 1));

   GUI_BUTTON_ID button_id_array[] = {GUI_BUTTON_CONFIG_LISTBOX_1,
                                      GUI_BUTTON_CONFIG_LISTBOX_2,
                                      GUI_BUTTON_CONFIG_LISTBOX_3,
                                      GUI_BUTTON_CONFIG_LISTBOX_4,
                                      GUI_BUTTON_CONFIG_LISTBOX_5,
                                      GUI_BUTTON_CONFIG_LISTBOX_6,
                                      GUI_BUTTON_CONFIG_LISTBOX_7,
                                      GUI_BUTTON_CONFIG_LISTBOX_8,
                                      GUI_BUTTON_CONFIG_LISTBOX_9,
                                      GUI_BUTTON_CONFIG_LISTBOX_10};

   index       = 0;
   y_pos       = Y_CORNER_POS;
   no_of_btns  = 0;
   no_of_items = 0;
   int total_btn_height = 0;

   while ((config_list_table[index].key_name) != LAST_CONFIG_INDEX_KEY)
   {
      if ((config_list_table[index].key_name) == save_key_name)
      {
         index_array[no_of_items++] = index;
      }
      index++;
   }

   OSRegion button_size;
   if (no_of_items - 1 > MAX_BUTTONS_IN_LIST)
   {
      up_button_grey = new Button_Bitmap(GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                         X_CORNER_POS,
                                         5,
                                         textEmpty,
                                         CFG_Look::BUTTON_SCROLL_GRAY_UP,
                                         CFG_Look::BUTTON_SCROLL_GRAY_UP);

      up_button_grey->allocate_resources (*this);
      up_button_grey->get_region(button_size);

      y_pos            = (short)(button_size.y + button_size.height + BUTTON_SPACING);

      down_button_grey = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                            X_CORNER_POS,
                                            y_pos,
                                            textEmpty,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN,
                                            CFG_Look::BUTTON_SCROLL_GRAY_DN);

      down_button_grey->allocate_resources (*this);

      down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                       X_CORNER_POS,
                                       y_pos,
                                       textEmpty,
                                       CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                       CFG_Look::BUTTON_SCROLL_DOWN);

      down_button->allocate_resources (*this);
      down_button->set_callback (cb_down_button, (void*)this);

   }

   index = 0;   // reset the index
   while ((config_list_table[index].key_name) != LAST_CONFIG_INDEX_KEY)
   {
      if ((config_list_table[index].key_name) == save_key_name)
      {
         if (no_of_btns <= MAX_BUTTONS_IN_LIST)
         {
            p_buttons[no_of_btns] = new Button_Bitmap(button_id_array[no_of_btns],
                                                      (short)X_CORNER_POS,
                                                      y_pos,
                                                      textEmptyFormatListboxButton,
                                                      CFG_Look::BUTTON_UP_NAME,
                                                      CFG_Look::BUTTON_DOWN_NAME);

            p_buttons[no_of_btns]->allocate_resources (*this);

            set_the_callbacks();

            if (save_key_name == KEY_LANG)
            {
               p_buttons[no_of_btns]->set_text_language(config_list_table[index].list_name);
            }

            p_buttons[no_of_btns]->set_string_id(config_list_table[index].txtid);
            p_buttons[no_of_btns]->get_region(button_size);
            button_array[no_of_btns] = index;
            y_pos                    = (short)(y_pos + button_size.height + BUTTON_SPACING);
            total_btn_height        += button_size.height;

            no_of_btns++;
         }
      }

      index++;
   } // end of while loop

   OSRegion screen_size;

   get_region(screen_size);
   screen_size.width  = (short)(button_size.width + MIN_SCREEN_WIDTH);
   screen_size.height = (short)(total_btn_height  + SCREEN_EDGE);

   if (no_of_items - 1 > MAX_BUTTONS_IN_LIST)
   {
      screen_size.height = (short)(total_btn_height + button_size.height + SCREEN_EDGE + SCREEN_EDGE);
      down_button->get_region(button_size);
      button_size.y      = y_pos;
      down_button->set_region(button_size);
      down_button_grey->set_region(button_size);
   }

   set_region(screen_size);

   //
   //   Now allocate the base window.  This is done last in this case so that the
   //   dynamic window sizing above is properly effected.
   //
   Base_Window::allocate_resources (allocation_parameter);

}  // END of Screen_LISTBOX ALLOCATE_RESOURCES



///////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES
void Screen_LISTBOX::deallocate_resources ()
{
   for (int y = 0; y < no_of_btns; y++)
   {
      delete p_buttons[y];
      p_buttons[y] = NULL;
   }

   delete up_button;
   up_button = NULL;

   delete up_button_grey;
   up_button_grey = NULL;

   delete down_button;
   down_button = NULL;

   delete down_button_grey;
   down_button_grey = NULL;

   Base_Window::deallocate_resources ();
}   // END of Screen_LISTBOX DEALLOCATE_RESOURCES

//
///////////////////////////////////////////////////////////////////////////////
// callback for up button
void cb_up_button (void* data)
{
   ((Screen_LISTBOX*)data)->process_up_button();
}

void Screen_LISTBOX::process_up_button ()
{
   int y;

   if (button_array[0] != index_array[0])
   {
      for (y = 0; y < no_of_items; y++)
      {
         if (button_array[0] == index_array[y]) break;
      }

      y--;

      for (int x = 0; x < no_of_btns; x++)
      {
         button_array[x] = index_array[y++];

         if (save_key_name == KEY_LANG)
         {
            p_buttons[x]->set_text_language(config_list_table[button_array[x]].list_name);
         }

         p_buttons[x]->set_string_id(config_list_table[button_array[x]].txtid);
      }

      if (down_button == NULL)
      {
         down_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON,
                                          X_CORNER_POS,
                                          y_pos,
                                          textEmpty,
                                          CFG_Look::BUTTON_DOWN_SCROLL_NAME,
                                          CFG_Look::BUTTON_SCROLL_DOWN);

         down_button->allocate_resources (*this);
         down_button->set_callback (cb_down_button, (void*)this);
      }

      if ((button_array[0] == index_array[0]) && (up_button))
      {
         delete up_button;
         up_button = NULL;
      }
   }
}  // END of Screen_LISTBOX::PROCESS_UP_BUTTON

//
///////////////////////////////////////////////////////////////////////////////
// callback for down button
void cb_down_button (void* data)
{
   ((Screen_LISTBOX*)data)->process_down_button();
}

void Screen_LISTBOX::process_down_button ()
{
   int z;

   if (button_array[no_of_btns - 1] != index_array[no_of_items - 1])
   {
      for (z = 0; z < no_of_items; z++)
      {
         if (button_array[0] == index_array[z])
            break;
      }

      z++;

      for (int x = 0; x < no_of_btns; x++)
      {
         button_array[x] = index_array[z++];

         if (save_key_name == KEY_LANG)
         {

            p_buttons[x]->set_text_language(config_list_table[button_array[x]].list_name);
         }

         p_buttons[x]->set_string_id(config_list_table[button_array[x]].txtid);
      }

      if (up_button == NULL)
      {
         up_button = new Button_Bitmap (GUI_BUTTON_CONFIG_LISTBOX_UP_BUTTON,
                                        X_CORNER_POS,
                                        5,
                                        textEmpty,
                                        CFG_Look::BUTTON_UP_SCROLL_NAME,
                                        CFG_Look::BUTTON_SCROLL_DOWN);

         up_button->allocate_resources (*this) ;
         up_button->set_callback (cb_up_button, (void*)this);
      }
   }

   if ((button_array[no_of_btns - 1] == index_array[no_of_items - 1]) && (down_button))
   {
      delete down_button;
      down_button = NULL;
   }
} // END of Screen_LISTBOX::PROCESS_DOWN_BUTTON

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void cb_buttons_0 (void* data)
{
   ((Screen_LISTBOX*)data)->process_listbox(0);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void cb_buttons_1 (void* data)
{
   ((Screen_LISTBOX*)data)->process_listbox(1);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void cb_buttons_2 (void* data)
{
   ((Screen_LISTBOX*)data)->process_listbox(2);
}

//
///////////////////////////////////////////////////////////////////
// callback for listbox buttons
void cb_buttons_3 (void* data)
{
   ((Screen_LISTBOX*)data)->process_listbox(3);
}

///////////////////////////////////////////////////////////////////////
//
void Screen_LISTBOX::process_listbox (int index)
{
   GUI_LISTBOX_STRUCT listbox_structure;
   listbox_structure.key_name  =  save_key_name;
   listbox_structure.key_value = config_list_table[button_array[index]].list_name;

   // allocate the message to display child screen
   Message<GUI_LISTBOX_STRUCT> Msg_LISTBOX_Set(MessageBase::SEND_LOCAL);
   Msg_LISTBOX_Set.send(listbox_structure);

   // Tell the window manager to remove the most recent child screen.
   RemoveMostRecentChildScreenMsg eventmsg(MessageBase::SEND_LOCAL);
   eventmsg.send(0);
}

//
///////////////////////////////////////////////////////////////////
// set the individual callbacks
void Screen_LISTBOX::set_the_callbacks (void)
{
   switch (no_of_btns)
   {
      case 0 :
      {
         p_buttons[no_of_btns]->set_callback (cb_buttons_0, (void*)this);
         break;
      }
      case 1 :
      {
         p_buttons[no_of_btns]->set_callback (cb_buttons_1, (void*)this);
         break;
      }
      case 2 :
      {
         p_buttons[no_of_btns]->set_callback (cb_buttons_2, (void*)this);
         break;
      }
      case 3 :
      {
         p_buttons[no_of_btns]->set_callback (cb_buttons_3, (void*)this);
         break;
      }
      default :
      {
         anAlarmMsg alarm(GUI_UNEXPECTED_CHOICE);
         alarm.setAlarm();

         DataLog(log_level_gui_error) << "did not find the callback! no_of_btns=" << no_of_btns << endmsg;
         break;
      }
   }   // end of switch
}  // end of method

/* FORMAT HASH 1a07a7480e6c1104ad536123ba93bf54 */
