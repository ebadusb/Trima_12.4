/************************> TRIMA GUI DOMAIN <****************************

*     Copyright(c) 1998 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Sysdonor_Action

*     File name:    sdaction.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This represents a single action to be executed on the Sysdonor
         screen. The identifying data members are a major "action ID" and a
         subordinate or minor "micro action ID". There may be 1 to many
         "micro action ID's" under each major "action ID". Within the class
         are contained members which are used as parameters to the executing
         function whose vector is also contained within this class.

         This class must be a friend class to Sysdonor since it will
         actually be modifying the GUI elements within the Sysdonor screen.

         This is the non-abstract implementation of the GUI_Act class known
         to the Action_Collection.

*************************< TRIMA GUI DOMAIN >****************************

$Header: I:/BCT_Development/vxWorks/Trima/gui/rcs/sdaction.hpp 1.6 2003/01/31 15:57:07Z rm70006 Exp rm70006 $
$Log: sdaction.hpp $
Revision 1.6  2003/01/31 15:57:07Z  rm70006
Clean up file.
Revision 1.5  2003/01/06 18:31:22Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.4  2002/08/15 21:12:55  sb07663
Vx updates
Revision 1.3  2001/08/02 17:24:04  rm70006
IT 4627, 4766.
Removed unneeded code and transitions for new Vista flow.
Revision 1.2  1998/09/16 00:00:29  tw09453
modify sysdonor state management routines
Revision 1.1  1998/08/08 00:04:46  tw09453
Initial revision

*/



// Single include define guardian
#ifndef SDACTEST_HPP
#define SDACTEST_HPP


#include "sysdonor.hpp" // include target screen class declarations
#include "gui.h"        // include GUI global info.
#include "aphwind.hpp"
#include "aphcalbk.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "button_bitmap.h"
#include "states.h"
#include "guiglobs.hpp"

#include "guiact.hpp"
#include "sysdninc.h"

#include "message.h"

// System #INCLUDES


DataLog_Stream & operator << (DataLog_Stream& os, const SYSDONOR_OBJECT& obj);
DataLog_Stream& operator  << (DataLog_Stream& os, const SYSDONOR_OP& op);

class Sysdonor_Action
   : public GUI_Action
{
private:
   Screen_SYSDONOR* SD_instance;

   SYSDONOR_OBJECT  GUI_object_name;
   SYSDONOR_OP      GUI_object_operation;
   BITMAP_ID        bitmap_ID;
   TEXT_STRING_ID   text_ID;
   TEXT_COLOR       text_color;
   STATUS_BAR_PHASE statusbar_phase; // see gui.h
   int              delay_time;
   GUI_SCREEN_ID    goto_screen;
   WINDOW_COLOR     status_line_color;

   bool             sysdonor_is_visible; // the flag that indicates whether SYSDONOR is currently visible or not


   // EXECUTE_BUTTON_ACTION
   // This method determines which operation to perform on the
   // button and executes it.
   void execute_button_action (Button_Bitmap& sysdonor_button);


   // EXECUTE_STATUSLINE_ACTION
   // This method determines which operation to perform on the
   // status line and executes it.
   void execute_statusline_action ();


   // EXECUTE_STATUSBAR_ACTION
   // This method determines which operation to perform on the
   // status bar and executes it.
   void execute_statusbar_action ();


   // EXECUTE_GENERAL_ACTION
   // This method determines which general operation to perform
   // and executes it.
   void execute_general_action ();


   // CONSTRUCTOR - invalid, except by friend classes, functions
   Sysdonor_Action();


public:
   // CONSTRUCTOR(S)

   // These are the various constructor signatures for different micro-actions

   // For button bitmap changes
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   BITMAP_ID bitmap_ID
                   );

   // For button text string changes
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   TEXT_STRING_ID text_ID
                   );

   // For button text color changes
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   TEXT_COLOR text_color
                   );

   // For Status Bar phase changes
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   STATUS_BAR_PHASE statusbar_phase
                   );

   // For time delays
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   int delay_time
                   );

   // For going-to another screen
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   GUI_SCREEN_ID goto_screen
                   );

   // For status line color changes
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation,
                   WINDOW_COLOR status_line_color
                   );

   // For object/op pairs with no parameters (e.g. visibility, activation, donor info. preset, system load preset, going to DONORDL)
   Sysdonor_Action(int action_id,
                   int micro_action_seq_id,
                   Screen_SYSDONOR* SD_instance,
                   SYSDONOR_OBJECT GUI_object_name,
                   SYSDONOR_OP GUI_object_operation
                   );



   // DESTRUCTOR
   virtual ~Sysdonor_Action();



   // EXECUTE_MICRO_ACTION
   // This method determines what micro-action should take place
   //  and calls the appropriate private member function to
   //  execute that function.
   virtual void execute_micro_action ();


   // COMPARE_ACTION_ID
   // This method compares the action ID and micro action ID to the
   // corresponding parameters and returns the result of that comparison.
   // 0 means equality comparison; -1 means an inequality comparison
   //
   virtual int compare_action_id (int action_id, int micro_action_seq_id);


};  // END of Action_Collection Class Declaration


// End of single include define guardian
#endif

/* FORMAT HASH 7e60b680e536d6d48dcc794094bf1092 */
