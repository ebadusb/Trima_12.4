/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 1999 COBE BCT, Inc. All rights reserved

*     Author:       Terry Wahl

*     Class name:   Screen_SETID

*     File name:    setid.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

        This class represents the screen that comes up in the situation where
        the trima hardware is unable to identify the color of the stamp on
        the set.  The operator is given the option of unloading and reloading
        the set (preferred), or continuing and using the stamp information
        associated with the selected REF number.

*************************< FOCUSSED GUI DOMAIN >****************************

Revision 1.10  2006/08/17 15:49:46Z  dslausb
IT 7613 - Overhaul of Set Identification screen.
Revision 1.9  2006/05/30 18:55:48Z  jheiusb
7543 -- change setID to be inline with catalog number entry
Revision 1.8  2003/11/24 19:27:03Z  rm70006
IT 6624.  Fix bug in GUI where alarm screens weren't setting globvar variable for currently displayed alarm.
Revision 1.7  2003/01/06 18:31:22Z  jl11312
- removed Zinc, added Ugl interface layer, modified handling of bitmaps and text
Revision 1.6  2002/11/11 07:22:48  sb07663
replaced char[] constructs with guistring equivalents
Revision 1.5  2002/08/15 22:17:23  sb07663
compiling version for Vx
Revision 1.4  2002/05/07 14:01:55  sb07663
First cut at VxWorks compatibility
Revision 1.3  2001/06/12 22:23:35  sb07663
Latest revs of set ID screen
Revision 1.2  2000/02/28 21:58:43  SB07663
IT4604: changes to include directives for new MKS structure
Revision 1.1  1999/05/27 17:01:02  TW09453
Initial revision
*/

// Single include define guardian
#ifndef SETID_HPP
#define SETID_HPP

// GUI <-> procedure screen include
#include "guiproc.h"
#include "cassette.h"

// CLASS INCLUDE
#include "aphwind.hpp"
#include "btncanc.hpp"
#include "btngobk.hpp"
#include "btncont.hpp"
#include "btnlowl.hpp"
#include "bitmap.h"
#include "display_text.h"
#include "trimamessages.h"

// alarm include
#include "alarms.h"


// CLASS DECLARES

class Screen_SETID
   : public Base_Apheresis_Window
{

private:
   // The set ID text cartouche
   Bitmap _bmpTxtCartouche;

   // The display text object
   Display_Text
      _txtBody,
      _txtRefNum,
      _txtAlrmNum;

   Button_Bitmap _btnUseSet;
   Button_Bitmap _btnUnload;

   // Structure with which to respond to PROC
   Alarm_response_struct _alarmSetStruct;

   int                   _alarmState,        // Current state of the alarm as communicated to GUI by PROC
                         _stateOfBtnLockout; // Button lockout flag

   // Current alarm enum as determined by the parameter value passed at allocation
   ALARM_VALUES _currentAlarm;

   // Screen Event Handler pointers
   AlarmStateChangeMsg _msgAlarmStateChanged;
   void alarmStateChange ();

public:

   // CONSTRUCTOR
   Screen_SETID();

   // DESTRUCTOR
   virtual ~Screen_SETID();

   // ALLOCATE_RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources.
   virtual void allocate_resources (const char* allocation_parameter);

   // DEALLOCATE_RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources.
   virtual void deallocate_resources ();

   // SET_IDENTIFICATION_EXIT
   // This method is called to communicate the type of set selected and to dismiss
   // the alarm screen with handshaking to Proc.
   void setIdExit (cassette::cassetteType setID);

   // PROCESS_LOAD_NEWSET_BUTTON
   // This method is called when the set id load new set button is pressed
   void         processUnloadButton ();
   void         processContinueButton ();
   virtual void process_cancel_button ();

   // FREEZE_SCREEN
   // This method will disable all screen buttons
   void freezeScreen ();

   // THAW_SCREEN
   // This method will enable all screen buttons
   void thawScreen ();

   // RACE_LOCKOUT
   // This method will ensure that if two button presses are made before freezing the
   // screen that only one will be honored.
   int raceLockout ();

   // ALARM_SET_STRUCT_MANAGER
   // This method sets the command codes into a structure and sends it to the Proc
   // handler.
   void alarmSetStructManager ();

   // REFRESH_RESOURCES
   // This routine overrides the aphwindow routine (which must be called too). It will
   // unlock the screen buttons after being overlayed by an "end run" confirmation.
   virtual void refresh_resources ();

   // INITIALIZE_STATUS_LINE
   // This is to initialize the status line one time
   virtual void initialize_status_line ();

};

// End of single include define guardian
#endif

/* FORMAT HASH 4e44e58a4be56772cc13fc060f958076 */
