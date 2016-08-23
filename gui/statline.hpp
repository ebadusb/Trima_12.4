/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1995, 1996 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Status_Line

*     File name:    statline.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the status line found on all of the apheresis
         screens.  This class will have one instantiation which will be
         Photon-reparented onto the current apheresis screen prior to that screen's
         realization. When the status line is "unlinked" from the derived
         apheresis screen it is reparented onto the "UR" window.

*************************< FOCUSSED GUI DOMAIN >****************************

$Header: E:/BCT_Development/Trima5.R/Trima/gui/rcs/statline.hpp 1.23 2009/01/06 19:18:25Z dslausb Exp dslausb $

*/


// Single include guardian define
#ifndef STATLINE_HPP
#define STATLINE_HPP


// Focussed #INCLUDES

#include "aphwind.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "osgraphics.h"
#include "timermessage.h"
#include "callback.h"

#define MAX_STATUS_LINE_ARRAY 3

const char* convertStringIdToString (TEXT_STRING_ID string_id);

#define set_current_text(__NEW_TEXT__) set_current_text_real(__NEW_TEXT__, __FILE__, __LINE__)
#define set_current_text_direct(__NEW_TEXT__) set_current_text_direct_real(__NEW_TEXT__, __FILE__, __LINE__)
#define reset_status_line() reset_status_line_real(__FILE__, __LINE__)
#define set_current_status_line_text_by_type(__TYPE__, __NEW_TEXT__) set_current_status_line_text_by_type_real(__TYPE__, __NEW_TEXT__, __FILE__, __LINE__)
#define deactivate_status_line_type(__TYPE__) deactivate_status_line_type_real(__TYPE__, __FILE__, __LINE__)
#define deactivate_status_line_text_type(__NEW_TEXT__, __TYPE__) deactivate_status_line_text_type_real(__NEW_TEXT__, __TYPE__, __FILE__, __LINE__)

class Status_Line
   : public OSWindow
{

private:

   CallbackBase _callbackTxtChange;     // Some peeps may want to know when the text gets changes.

   // The status line enumerated types
   enum STATUS_LINE_LOOKUP
   {
      STATUS_LINE_DEFAULT   = 0,
      STATUS_LINE_ATTENTION = 1,
      STATUS_LINE_END_RUN   = 2,
      STATUS_LINE_ALERT     = 3,
      STATUS_LINE_ALARM     = 4,
      STATUS_LINE_NULL      = 5
   };

   // info for the currently displayed status line
   Bitmap             current_statusline_bitmap ;
   WINDOW_COLOR       current_statusline_color ;
   Display_Text       current_statusline_text ;
   STATUS_LINE_LOOKUP current_status_line_type ;

   guistring          current_custom_text;

   // The ID of the screen owning the status bar
   GUI_SCREEN_ID current_parent_screen ;

   // The status line individual table entries struct
   struct STATUS_LINE_TABLE
   {
      TEXT_STRING_ID type_status_line_text ;
      BITMAP_ID      type_status_line_color ;
      TEXT_COLOR     type_status_line_font_color ;

      STATUS_LINE_TABLE& operator = (const STATUS_LINE_TABLE& source)
      {
         if (&source != this)
         {
            type_status_line_text       = source.type_status_line_text;
            type_status_line_color      = source.type_status_line_color;
            type_status_line_font_color = source.type_status_line_font_color;
         }
         return *this;
      }

   }   ;

   //////////////////////////////////
   // A simple fixed-length stack class - no dynamic memory allocation
   struct Status_Line_Stack
   {
      STATUS_LINE_TABLE myStack[MAX_STATUS_LINE_ARRAY];

      Status_Line_Stack()
      {
         for (int i = 0; i < MAX_STATUS_LINE_ARRAY; i++)
         {
            myStack[i].type_status_line_text = textEmpty;
         }
      }

      void reinit (STATUS_LINE_TABLE& newStatusLine)
      {
         for (int i = 0; i < MAX_STATUS_LINE_ARRAY; i++)
         {
            myStack[i] = newStatusLine;
         }
      }

      void push (TEXT_STRING_ID& newStatusLineText)
      {
         if (myStack[MAX_STATUS_LINE_ARRAY - 1].type_status_line_text != textEmpty)
         {
            DataLog(log_level_gui_statline_debug) << "status line stack pushed one off the end " << myStack[MAX_STATUS_LINE_ARRAY - 1].type_status_line_text << endmsg;
         }

         for (int i = MAX_STATUS_LINE_ARRAY - 1; i > 0; i--)
         {
            myStack[i] = myStack[i - 1];
         }
         myStack[0].type_status_line_text = newStatusLineText;
      }

      STATUS_LINE_TABLE pop ()
      {
         STATUS_LINE_TABLE retval = myStack[0];

         for (int i = 0; i < MAX_STATUS_LINE_ARRAY - 1; i++)
         {
            myStack[i] = myStack[i + 1];
         }
         myStack[MAX_STATUS_LINE_ARRAY - 1].type_status_line_text = textEmpty;

         return retval;
      }

      STATUS_LINE_TABLE& top ()
      {
         return myStack[0];
      }

      void erase (TEXT_STRING_ID& newStatusLineText)
      {
         for (int i = 0; i < MAX_STATUS_LINE_ARRAY; i++)
         {
            int offset = 0;
            while (myStack[i].type_status_line_text == newStatusLineText)
            {
               offset++;
               myStack[i] = myStack[i + offset];
            }

            if (offset)
            {
               for (int x = i + 1; x < MAX_STATUS_LINE_ARRAY; x++)
               {
                  if (x + offset < MAX_STATUS_LINE_ARRAY)
                  {
                     myStack[x] = myStack[x + offset];
                  }
                  else
                  {
                     myStack[x].type_status_line_text = textEmpty;
                  }
               }
            }
         }
      }

      void dump ()
      {
         for (int i = 0; i < MAX_STATUS_LINE_ARRAY; i++)
         {
//            DataLog(log_level_gui_statline_info) << i << " : " << myStack[i].type_status_line_text << endmsg;
            DataLog(log_level_gui_statline_info) << " " << i << ":" << convertStringIdToString(myStack[i].type_status_line_text);
//            DataLog(log_level_gui_statline_info) << ": " << convertStringIdToString(myStack[i].type_status_line_text);
         }
//         DataLog(log_level_gui_statline_info) << endmsg;
      }

   };

   //
   //   Our "dingbat" and timer
   //
   Bitmap       dingbat;
   TimerMessage _timerFlashIcon;

   // An array of simple stacks of status lines
   Status_Line_Stack status_line_type[STATUS_LINE_NULL] ;

   // The default bitmaps and colors for the different status lines
   STATUS_LINE_TABLE status_line_defaults[STATUS_LINE_NULL] ;

   // Temporary range checking variables to prevent against reallocation
   // of the same status line to alieviate flickering.
   TEXT_STRING_ID type_status_line_text_last ;

   // SET_OWNING_SCREEN
   // This method sets the ID of the screen that currently owns the status line
   void set_owning_screen (GUI_SCREEN_ID owning_screen);


   // SET_CURRENT_STATUS_LINE_LOOKUP
   // This method will take user request of status line type and convert it to
   // the local status line lookup enumrated type.
   STATUS_LINE_LOOKUP set_current_status_line_lookup (STATUS_LINE_TYPE user_status_line_request);


   // SET_STATUS_LINE_TYPE_INTERNAL
   // This method will explicitly set the status line to the type defined
   // by the parameter STATUS_LINE_TABLE.
   void set_status_line_type_internal (STATUS_LINE_LOOKUP internal_status_line_request);


   // ACTIVATE_HIGHEST_PRIORITY_STATUS_LINE
   // This method will explicitly reverts the status line to the type defined
   // by the highest active status line table state.
   void activate_highest_priority_status_line ();

   void forceTextDimensions ();

public:

   // CONSTRUCTOR
   Status_Line (WINDOW_COLOR statusline_color = TEAL);


   // DESTRUCTOR
   virtual ~Status_Line ();


   // ALLOCATE RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   void allocate_resources ();


   // DEALLOCATE RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   void deallocate_resources ();


   // GET_OWNING_SCREEN
   // This method returns the ID of the screen that currently owns the status line
   GUI_SCREEN_ID get_owning_screen ();


   // SET_COLOR
   // This method will load the background bitmap corresponding to the color passed
   // as the color parameter.
   void set_color (WINDOW_COLOR statusline_color);


   // GET_COLOR
   // This method will return the color ID currently set in the status line
   //
   WINDOW_COLOR get_color ();


   // SET_CURRENT_TEXT
   // This method will take a text ID and display its corresponding text on the
   // status line.
   void set_current_text_real (TEXT_STRING_ID statusline_text, const char* sourceFileName, int sourceFileLine);


   // SET_CURRENT_TEXT_DIRECT
   // This method will take a const char * and display it on the status line.
   void set_current_text_direct_real (const char* statusline_text, const char* sourceFileName, int sourceFileLine);


   // GET_CURRENT_TEXT
   // This method will return the text ID of the text currently displayed in the
   // status line. If none is currently set in the status line, the textEmpty
   // will be returned
   TEXT_STRING_ID get_current_text ();


   // GET_CURRENT_STATUS_LINE_TEXT_BY_TYPE
   // This method will get the text ID for a specific status line type,
   TEXT_STRING_ID get_current_status_line_text_by_type (STATUS_LINE_TYPE user_status_line_request);

   STATUS_LINE_TYPE get_current_status_line_type ();


   // REPARENT_STATUS_LINE
   // This method will reparent the status line onto the window passed as parent
   // window.
   void reparent_status_line (Base_Window& parent);


   // UNPARENT_STATUS_LINE
   // This method will reparent the status line onto the UR window
   // thereby disconnecting the status bar's fate from the current
   // apheresis window.
   void unparent_status_line (GUI_SCREEN_ID unlinking_request_screen);


   // RESET_STATUS_LINE
   // This method will reset all of the status line text identifier entries to
   // TEXT_NULL, and hence, effectively reinitializes all the status line types
   // to their initial state, and moves the current table pointer to the default
   // state
   void reset_status_line_real (const char* sourceFileName, int sourceFileLine);


   // SET_CURRENT_STATUS_LINE_TEXT_BY_TYPE
   // This method will set the text explicitly for a specific status line type,
   // then proceeds to sequentially check all the status line entries to determine
   // the highest priority to be displayed.
   void set_current_status_line_text_by_type_real (STATUS_LINE_TYPE user_status_line_request,
                                                   TEXT_STRING_ID user_status_line_text_request,
                                                   const char* sourceFileName,
                                                   int sourceFileLine);

   // DEACTIVATE_STATUS_LINE_TYPE
   // This method will remove the current text from the current status line level
   // by changing the text identifier to textEmpty, then proceeds to sequentially
   // check all the status line entries to determine the highest priority to be
   // displayed.
   void deactivate_status_line_type_real (STATUS_LINE_TYPE user_status_line_request, const char* sourceFileName, int sourceFileLine);

   // DEACTIVATE_STATUS_LINE_TEXT_TYPE
   // This method will remove all occurances of the given text from the given status line level
   // backfilling from the end of the stack with textEmpty, then proceeds to sequentially
   // check all the status line entries to determine the highest priority to be
   // displayed.
   void deactivate_status_line_text_type_real (TEXT_STRING_ID user_status_line_text_request,
                                               STATUS_LINE_TYPE user_status_line_request,
                                               const char* sourceFileName, int sourceFileLine);

   void dingbat_timeout ();

   // dump all of the status lines in memory to the dlog
   void dump_stacks ();

public:

   void setTextChangeCallback (const CallbackBase& callback);
   void clearTextChangeCallback ();

   void dump (const char* sourceFile, const int sourceLine, const char* lbl, const va_list args);
   void dump (const char* sourceFile, const int sourceLine, const char* lbl, ...);

}; // END of Status_Line Class Declaration


// End of single include guardian define
#endif

/* FORMAT HASH 4c66ee662dcd7fd9e967b5112c09061b */
