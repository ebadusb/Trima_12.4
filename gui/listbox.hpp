/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:      Barry Fulkerson

*     Class name:  Screen_LISTBOX

*     File name:   listbox.hpp

*     Contents:    See prototype header descriptions below...

*     Description:


*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.1x/Trima/gui/rcs/listbox.hpp 1.20 2003/02/06 17:22:53Z rm10919 Exp dslausb $
$Log: listbox.hpp $
Revision 1.20  2003/02/06 17:22:53Z  rm10919
Clean out unused variables.
Revision 1.19  2003/01/06 18:31:16Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.18  2002/11/11 06:37:04  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.17  2002/09/27 17:03:39  sb07663
derived off bwindow; allocation order changes for proper display
Revision 1.16  2002/07/18 22:05:50  rm70006
Ported to use Zinc.
Revision 1.15  2002/05/07 13:58:12Z  sb07663
First cut at VxWorks compatibility
Revision 1.14  1997/03/12 18:35:22  Barry_Fulkerson
- added gray'ed out scroll buttons to listbox.
Revision 1.13  1997/03/07 19:42:18  Barry_Fulkerson
Revision 1.12  1997/03/06 01:55:09  Barry_Fulkerson
- added the ability to display each language button in its own language
Revision 1.11  1997/03/05 23:02:57  Barry_Fulkerson
- aligned the scroll buttons with the button list
Revision 1.10  1997/03/03 22:30:25  Barry_Fulkerson
- this time I am really adding the previous changes
Revision 1.9  1997/03/03 22:04:22  Barry_Fulkerson
- changed the reference to the new config scroll buttons to include changes
  from Christelle @ Hauser
Revision 1.8  1997/02/25 04:28:43  Barry_Fulkerson
- added up/down capability
Revision 1.7  1997/02/21 22:22:15  Barry_Fulkerson
- added clean-up on listbox, so listbox's would not overrun screen.  This is
  temp.
Revision 1.6  1996/10/23 22:45:31  Barry_Fulkerson
- added BUTTON_ID's to buttons
Revision 1.5  1996/10/17 02:22:53  Barry_Fulkerson
- increased number of callbacks for listbox
Revision 1.4  1996/10/14 23:28:45  Barry_Fulkerson
- changed listbox from Photon style to an actual list of buttons
- added x,y location capability to listbox
Revision 1.3  1996/10/09 23:35:13  Barry_Fulkerson
- changed listbox to return CONFIG_KEY types instead of chars
Revision 1.2  1996/10/07 19:52:58  Barry_Fulkerson
added File Title Block comment section and MKS keywords

*/

// Single include define guardian
#ifndef LISTBOX_HPP
#define LISTBOX_HPP

/** include files **/
#include "message.h"
#include "aphwind.hpp"
#include "list_get.hpp"
#include "button_bitmap.h"


/** defines **/

#define  MAX_LIST_SIZE           40
#define  STRING_TOKEN            "~"
#define  MAX_BUTTONS_IN_LIST     3
#define  MAX_SCREEN_WIDTH        80
#define  MIN_SCREEN_WIDTH        20
#define  SCREEN_EDGE             20

#define  X_CORNER_POS            10
#define  Y_CORNER_POS            5
#define  BUTTON_SPACING          3


void cb_buttons_0 (void* data);
void cb_buttons_1 (void* data);
void cb_buttons_2 (void* data);
void cb_buttons_3 (void* data);
void cb_up_button (void* data);
void cb_down_button (void* data);

void listbox_services (OSWindow* wid, void* data, void* info);

//
///////////////////////////////////////////////////////////////////
//
class Screen_LISTBOX
   : public Base_Window
{
protected:
   CONFIG_DATA_KEYS save_key_name;
   int              no_of_btns, no_of_items, index;
   short            y_pos;
   short            listbox_X_pos, listbox_Y_pos;


   Button_Bitmap*           up_button;
   Button_Bitmap*           up_button_grey;

   Button_Bitmap*           down_button;
   Button_Bitmap*           down_button_grey;

   int                      index_array[MAX_LIST_SIZE];

   Get_config_listbox_names listname_fetcher;
   Button_Bitmap*           p_buttons[MAX_LIST_SIZE];
   int                      button_array[MAX_BUTTONS_IN_LIST + 1];

public:

   // CONSTRUCTOR
   Screen_LISTBOX();

   // DESTRUCTOR
   virtual ~Screen_LISTBOX();

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

   void set_the_callbacks (void);

}; // END of Screen_LISTBOX Class Declaration

// End of single include define guardian
#endif


//
///////////////////////////////////////////////////////////////////
// end of listbox.hpp file

/* FORMAT HASH af8bddc9f67dff3ccd8ed8a344570056 */
