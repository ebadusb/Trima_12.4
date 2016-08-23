/*
 * Copyright (c) 2002 by Gambro BCT, Inc.  All rights reserved.
 *
 * $Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/button_bitmap.h 1.11 2008/08/22 17:18:53Z dslausb Exp dslausb $
 *
 */

#ifndef _BUTTON_BITMAP_INCLUDE
#define _BUTTON_BITMAP_INCLUDE

#include "bitmap.h"
#include "display_text.h"
#include "gui.h"
#include "cfg_ids.h"
#include "oswindow.hpp"

#if CPU==SIMNT
#include "btnPtrStk.hpp"
#endif // #if CPU==SIMNT

//
//  The button bitmap class is derived from a window so that it can
//  use event handling to process inputs.
//
class Button_Bitmap
   : public OSWindow
{
public:

   // typedef for a callback function with a void pointer input parameter
   typedef void (* CallbackFunc)(void*);

   friend DataLog_Stream& operator << (DataLog_Stream& os, Button_Bitmap& btn);

public:
   // CONSTRUCTOR
   // The first parameter is the ID of the button used for auditing; it is required.
   // The second and third parameters are the coordinates of the upper left hand corner
   // of the button. The next parameter is the ID of the text string to label
   // the button. The third and fourth parameters are the up and down bitmap ID's.
   Button_Bitmap(GUI_BUTTON_ID btn_id,
                 short x = 0,
                 short y = 0,
                 TEXT_STRING_ID text_id = TEXT_NULL,
                 BITMAP_ID btn_up_bitmap_name = BITMAP_NULL,
                 BITMAP_ID btn_dn_bitmap_name = BITMAP_NULL,
                 bool extraLogging = false,
                 bool overrideStylingRecord = true
                 );


   // DESTRUCTOR
   virtual ~Button_Bitmap ();


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   void allocate_resources (OSWindow& parent);


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources ();

   //
   //
   //   You can query my allocation status
   const int allocated () const { return _allocated; }

   //
   //
   // This method removes any callbacks associated with the button.  If you
   // deallocate_resources() the callbacks will be removed automatically.
   void remove_callbacks ();

   //
   //
   //  You may disable or enable callbacks on the button.
   void enable_callbacks ();
   void disable_callbacks ();

   // SET_CALLBACK
   // This method establishes the pointer to the C routine that the UGL
   // subsystem will call when the button is pushed then released.
   void set_callback (CallbackFunc callback_routine_ptr, void* data);


   // SET_BUTTON_TEXT_COLOR
   // This method will change the color of the text associated with this button
   void set_button_text_color (TEXT_COLOR textcolor);


   // SET_DOWN_BITMAP
   // This method will establish the bitmap to be loaded in the down bitmap object
   // when allocation occurs.
   void set_down_bitmap (BITMAP_ID bitmap_down);


   // SET_UP_BITMAP
   // This method will establish the bitmap to be loaded in the up bitmap object
   // when allocation occurs.
   void set_up_bitmap (BITMAP_ID bitmap_up);


   // SET_STRING_ID
   // This method has the button object pass its text ID to the text display object and
   // load all appropriate string information.
   void set_string_id (TEXT_STRING_ID textid);


   // SET_TEXT_DIRECT
   // This method will vector to the display text object's
   // set_text_direct method - see the Display_Text class
   // declaration for more info.
   void set_text_direct (int direct_len, const char* direct_text);
   void set_text_direct (TrimaTextItem* textItem);
   void set_text_direct (const guistring& str) { set_text_direct(str.length(), str); }

   // GET_TEXT_DIRECT
   // This method will vector to the display text object's
   // get_text_direct method - see the Display_Text class
   // declaration for more info.
   void get_text_direct (guistring& buffer_ptr);


   // SET_BTN_TEXT_MARGIN
   // The method will establish right and left margins on the button face. The
   // text will flow within these margins.
   virtual void set_btn_text_margin (int text_margin);


   // GET_BUTTON_ID
   // This method will return the logical ID of the button as defined in the
   // constructor
   GUI_BUTTON_ID   get_button_id ();
   BITMAP_ID const get_up_bitmap_id () { return btn_up_bitmap.get_id(); }
   BITMAP_ID const get_down_bitmap_id () { return btn_dn_bitmap.get_id(); }

   // SET_TEXT_LANGUAGE
   // This method will set the language for display text object.
   // Added for configuration language list box.
   void set_text_language (CONFIG_DATA_LISTBOX_STRINGS config_language);


   //
   void btnAttach (OSWindow* parent);
   void btnDetach ();

   // This callback function is registered for mouse events
   //
   virtual void pointerEvent (const PointerEvent& event);

   void setFont (int newFontIndex,     int newFontSize);

   short getLastClickX () {return last_click_x; }
   short getLastClickY () {return last_click_y; }

protected:
   void force_text_dimensions ();

   void log_button_push ();


   // PROCESS_BUTTON_ARM
   // This method will perform general button processing on the depression of the
   // button object, e.g. generate an audio feedback; hide the button text.
   virtual void process_button_arm ();

   // PROCESS_BUTTON_DISARM
   // This method will perform general button processing on the release of the
   // button object, e.g. realize the button text.
   virtual void process_button_disarm ();

protected:

   GUI_BUTTON_ID btn_logical_id;              // this is used to identify the button when pressed
   int           btn_text_margin;             // only settable by method call
   Bitmap        btn_up_bitmap;
   Bitmap        btn_dn_bitmap;
   Display_Text  btn_text;
   CallbackFunc  callback_routine_ptr;
   void*         callback_data;              // The pointer passed to the callback routine in data param

   int           last_click_x, last_click_y;       // The position of the last button down event

   bool
      _btnEnabled,
      _callbackEnabled,
      _allocated,
      _extraLogging,
      _overrideStylingRecord;


#if CPU == SIMNT
   // Keep track of all the Buttons created, so we can get the
   // coordinates of the center of each button given the button ID
   // Note that if there are multiple copies of a certain button, this
   // array will point to the one which was most recently either allocated
   // or constructed.
protected: static BtnPointerStack btnPointers;
public:    static Button_Bitmap* get_button_from_id (GUI_BUTTON_ID id);
#endif /* if CPU == SIMNT */

} ; // END of Button_Bitmap Class Declaration

DataLog_Stream& operator << (DataLog_Stream& os, Button_Bitmap& btn);

#endif /* ifndef _BUTTON_BITMAP_INCLUDE */

/* FORMAT HASH 864f94d3867ba0905024eeebf9e3be46 */
