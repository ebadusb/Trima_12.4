/*
 * Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
 *
 *
 */

#ifndef _BITMAP_TEXT_BOX_H_
#define _BITMAP_TEXT_BOX_H_

#include "bitmap.h"
#include "display_text.h"
#include "gui.h"
#include "cfg_ids.h"
#include "oswindow.hpp"

//
//  The button bitmap class is derived from a window so that it can
//  use event handling to process inputs.
//
class BitmapTextBox
   : public OSWindow
{

protected:

   int          _margin ;              // only settable by method call
   Bitmap       _bitmap;
   Display_Text _text ;

   bool         _allocated;

public:



   // CONSTRUCTOR
   // The first parameter is the ID of the button used for auditing; it is required.
   // The second and third parameters are the coordinates of the upper left hand corner
   // of the button. The next parameter is the ID of the text string to label
   // the button. The third and fourth parameters are the up and down bitmap ID's.
   BitmapTextBox(short x = 0,
                 short y = 0,
                 TEXT_STRING_ID txtId = TEXT_NULL,
                 BITMAP_ID bmpId = BITMAP_NULL
                 ) ;


   // DESTRUCTOR
   virtual ~BitmapTextBox () ;


   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   void allocate_resources (OSWindow& parent) ;


   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   void deallocate_resources () ;

   //
   //
   //   You can query my allocation status
   const int allocated () const { return _allocated; }

   // SET_BUTTON_TEXT_COLOR
   // This method will change the color of the text associated with this button
   void setTxtColor (TEXT_COLOR textcolor) ;


   // SET_UP_BITMAP
   // This method will establish the bitmap to be loaded in the up bitmap object
   // when allocation occurs.
   void setBitmapId (BITMAP_ID bitmapId) ;


   // SET_STRING_ID
   // This method has the button object pass its text ID to the text display object and
   // load all appropriate string information.
   void setStringId (TEXT_STRING_ID textid) ;


   // SET_TEXT_DIRECT
   // This method will vector to the display text object's
   // setTxtDirect method - see the Display_Text class
   // declaration for more info.
   void setTxtDirect (const int direct_len, const char* direct_text) ;


   // GET_TEXT_DIRECT
   // This method will vector to the display text object's
   // getTxtDirect method - see the Display_Text class
   // declaration for more info.
   void getTxtDirect (guistring& buffer_ptr) ;


   // SET_BTN_TEXT_MARGIN
   // The method will establish right and left margins on the button face. The
   // text will flow within these margins.
   virtual void setMargin (int text_margin) ;

protected:
   void forceTxtDimensions (void);

} ; // END of BitmapTextBox Class Declaration


#endif /* ifndef __BITMAP_TEXT_BOX_H__ */

/* FORMAT HASH 85f8a14b01e8ac53630e69890f4627f4 */
