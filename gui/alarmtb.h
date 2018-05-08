/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1996 COBE BCT, Inc. All rights reserved

*     Author:       Michael Cobb

*     Class name:   N/A

*     File name:    alarmtb.h

*     Contents:     See prototype header descriptions below...

*     Description:

         This file contains the prototypes for the alarm table elements
         used in composing the string array in alarmtb.cpp.

*************************< FOCUSSED GUI DOMAIN >*****************************

 NOTE: This file is shared with Vista. Take care when adding new dependencies

**********************************************************************************/

// Single include define guardian
#ifndef ALARMTB_H
#define ALARMTB_H

#ifndef WIN32
#include <vxWorks.h>
#endif


// include all alarm information from the source
#include "alarms.h"


// include all screen information from the source
#include "gui.h"
#include "guiproc.h"

// These are bitmask definitions for various alarm options.  This allows us to have several options that are defined in the table.
enum
{
   ALARM_OPT_NONE                     =0,  // Nothing special about this alarm...
   ALARM_OPT_CAUSES_WBC_FLAG          =1,  // Show the mag glass on the continue button, because this alarm will cause a WBC flag.
   ALARM_OPT_UNLOAD_BTN_ONLY          =2,  // This is for alarms that only display the unload button.
   ALARM_OPT_SHOW_ALARM_NUM           =4,  // Show the alarm number on the screen (A1s and A2s always do, so this setting is for R1s and R2s that want to show it as well.)
   ALARM_OPT_4_BTN_SUITE              =8,  // Show the 4-button suite
   ALARM_OPT_4_BTN_SUITE_PRE_DISP_TEST=16, // Show the 4-button suite if alarm shows prior to disposables test
   ALARM_OPT_UNOBTRUSIVE_ALERT        =32, // Add some extra tweaks to make this alarm unobtrusive
   ALARM_OPT_DISALLOW_END_RUN         =64, // Don't show the 'end run' button on this alarm
   ALARM_OPT_DISALLOW_ADJUST          =128,
   ALARM_OPT_DISALLOW_RINSEBACK       =256 // Don't show rinseback
};


// the following strucure defines all the necessary elements to display all of
// the alarm bitmaps and text as well as all the management of all alarm navigation
//
//
typedef struct
{
   ALARM_VALUES alarm_name;

#if CPU == SIMNT
   const char* alarm_name_literal;
#endif /* if CPU == SIMNT */

   unsigned long  alarm_options;

   TEXT_STRING_ID alarm_body_text;
   TEXT_STRING_ID alarm_status_line_text;

   // If bitmap is BITMAP_NULL, then the no-graphic alarm is displayed.
   BITMAP_ID     alarm_bitmap;

   GUI_SCREEN_ID goto_screen_gui;
   char*         goto_screen_literal;

} ALARM_TABLE;

// The OPERATOR_INFO_PROMPT (OIP) type of alarm will reference a unique screen (not the standard alarm screen).
// This type of screen will "act" like the alarm screen in terms of handshaking with PROC,
// and the method of its invocation as a stateless screen. It will, however, have a different
// layout, button arrangement, and behavior.
//
// Its entry in the alarm table will use the standard structure, ALARM_TABLE, but it will use
//  it differently. The following outlines how each field is used for an OIP alarm:
//
//      alarm_name				-- same as a normal alarm
//      alarm_name_literal		-- same as normal: a text string of alarm_name used for creating screenshots only
//		alarm_options			-- this will always be ALARM_OPT_NONE
//      alarm_body_text			-- will always contain textEmpty
//      alarm_status_line_text	-- will always contain textEmpty
//      alarm_bitmap			-- will always contain BITMAP_NULL
//      goto_screen_gui			-- will contain the screen ID of the OIP screen to invoke (This must be something besides the normal alarm screen or the NULL screen)
//		goto_screen_literal		-- will contain a parameter string to pass to the OIP screen.
//                                  it may not be a NULL ptr; at least an empty string, e.g. ""
//
// Once the OIP screen has been invoked, it will not use the alarm table.  So, what's really important here is
// the alarm name, the screen ID, and the literal.
//

class GuiAlarmTable
{
public:
   GuiAlarmTable(){ index = -1; }
   virtual ~GuiAlarmTable(){}

   const ALARM_TABLE* GetEntry (ALARM_VALUES id);
   const ALARM_TABLE* GetCurrentEntry ();

   // Get the prev/next entry and move the cursor
   const ALARM_TABLE* GetNextEntry ();
   const ALARM_TABLE* GetPreviousEntry ();

   // Get the prev/next entry without moving the cursor
   const ALARM_TABLE* PeekNextEntry ();
   const ALARM_TABLE* PeekPreviousEntry ();


private:
   int index;
};

// End of single include define guardian
#endif

/* FORMAT HASH 7b87705b94ced7b63532e5cac1f86c50 */
