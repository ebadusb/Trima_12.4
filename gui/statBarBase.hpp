/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:   StatBarBase

*     File name:    statBarBase.hpp

*     Contents:     See prototype header descriptions below...

*     Description:

         This class represents the status bar found on all of the apheresis
         screens.

*************************< FOCUSSED GUI DOMAIN >****************************

*/

// Single include guardian define
#ifndef __STAT_BAR_BASE_HPP__
#define __STAT_BAR_BASE_HPP__

#include <vxWorks.h> // Needed for CPU compiler directives

#include <map>
#include <string>

// Focussed #INCLUDES

#include "aphwind.hpp"
#include "display_text.h"
#include "osgraphics.h"

#if CPU==SIMNT
#include "states.h"
#endif // #if CPU==SIMNT

#define NUM_PHASES 6

#define __DUMP__(__LOG__) dumpInfo(DataLog(log_level_gui_info), __FILE__, __LINE__, __LOG__);

class StatBarBase
   : public OSWindow
{
protected:

   static map<string, BITMAP_ID> statBarGraphics;

   STATUS_BAR_TYPE               _type;

   // The current status bitmap
   static Bitmap* _bmpStatusPlacard;

   // The phase text labels
   static Display_Text* _txtLabelLoad;
   static Display_Text* _txtLabelInfo;
   static Display_Text* _txtLabelPrepare;
   static Display_Text* _txtLabelDonate;
   static Display_Text* _txtLabelAddition;
   static Display_Text* _txtLabelUnload;

   // The ID of the screen owning the status bar
   static GUI_SCREEN_ID _parentScreen ;

   // The phase status structure
   static STATUS_BAR_PHASE_STATES _currentPhase ;

   // SET_OWNING_SCREEN
   // This method sets the ID of the screen that currently owns the status bar

   static void set_owning_screen (const GUI_SCREEN_ID owning_screen) { _parentScreen = owning_screen; }

   virtual BITMAP_ID getStatusBmp ();
   virtual BITMAP_ID defaultBmp () = 0;

   virtual string getStatusString ();

public:
   // SET_PHASE
   // This method is passed a phase ID and the state to set it to. It then
   // validates both phase and state. If the setting of the phase would change
   // its current value, this routine returns a TRUE indication, otherwise it
   // returns a FALSE indication. This is used by the set_phase_active and
   // set_phase_complete methods.
   virtual bool set_phase (const STATUS_BAR_PHASE sb_phase, const STATUS_BAR_STATE sb_phase_state, bool forceUpdate = false) ;

   // UPDATE_STATUS_GRAPHICS_AND_TEXT
   // This method will inspect the status structure and determine which status
   // graphic should be loaded to represent it. It will also adjust the colors
   // of the text labels to reflect the current status structure. If a status state is
   // detected which has no cooresponding graphic, no changes will be made to
   // the status bar (the current one remains displayed).
   virtual void update_status_graphics_and_text ();

      #if (CPU==SIMNT)
   virtual void             setState (const State_names systemState);
   virtual STATUS_BAR_STATE getSuggestedState (State_names systemState, STATUS_BAR_PHASE phase);

private:

   static STATUS_BAR_STATE _statBarStateTable[METERED_STORAGE_DISCONNECT - STARTUP_TESTS + 1][LAST_STATUS_BAR_PHASE + 1];
    #endif

protected:

   // CONVERT_STATUS_TO_STRCHAR
   // This method will convert a status bar state value into a one character
   // string that will be concatenated into a full state string representation.
   // The returned character pointer will point to a single character string literal.
   // This routine is used by the update_status_graphics_and_text routine exclusively.
   // Conversions:
   //              STATUS_BAR_STATE_RESET    = "C"
   //              STATUS_BAR_STATE_ACTIVE   = "Y"
   //              STATUS_BAR_STATE_COMPLETE = "G"
   virtual char convert_status_to_strchar (const STATUS_BAR_STATE status_state);

   // CONVERT_STATUS_TO_COLOR
   // This method will convert a status bar state value into a text color value
   // which is used to set the color of a particular status text word (e.g. "info").
   // This routine is used by the update_status_graphics_and_text routine exclusively.
   // Conversions:
   //              STATUS_BAR_STATE_RESET    = GREY_LIGHT
   //              STATUS_BAR_STATE_ACTIVE   = YELLOW
   //              STATUS_BAR_STATE_COMPLETE = GREY_DARK
   virtual TEXT_COLOR convert_status_to_color (const STATUS_BAR_STATE status_state) ;




public:

   // CONSTRUCTOR
   StatBarBase (STATUS_BAR_TYPE type,
                TEXT_STRING_ID loadString,
                TEXT_STRING_ID infoString,
                TEXT_STRING_ID prepString,
                TEXT_STRING_ID donateString,
                TEXT_STRING_ID additionString,
                TEXT_STRING_ID unloadString);

   // DESTRUCTOR
   virtual ~StatBarBase ();


   // ALLOCATE RESOURCES
   // This method allocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   virtual void allocate_resources ();


   // DEALLOCATE RESOURCES
   // This method deallocates any bitmap resources or other memory intensive
   // resources and the basic window widget.
   virtual void deallocate_resources ();


   // RESET_ALL_PHASES
   // This method will reset all the phases to the "RESET" state
   virtual void reset_all_phases ();


   // SET_PHASE_ACTIVE
   // This method will set a specific phase (as indicated in the parameter)
   // to the active state.
   virtual void set_phase_active (const STATUS_BAR_PHASE sb_phase);


   // SET_PHASE_COMPLETED
   // This method will set a specific phase (as indicated in the parameter)
   // to the completed state.
   virtual void set_phase_completed (const STATUS_BAR_PHASE sb_phase);


   // REPARENT_STATUS_BAR
   // This method will reparent the status bar onto the window passed as parent
   // window.
   virtual void reparent_status_bar (Base_Window& new_parent);


   // UNPARENT_STATUS_BAR
   // This method will reparent the status bar onto the UR window
   // thereby disconnecting the status bar's fate from the current
   // apheresis window.
   void unparent_status_bar (GUI_SCREEN_ID unlinking_request_screen);

   GUI_SCREEN_ID get_owning_screen () { return (_parentScreen); }

   virtual STATUS_BAR_TYPE getType () const { return _type; }

   virtual void dumpInfo (DataLog_Stream& os, const char* file, int line, const char* log = "");

private:
   StatBarBase();

}; // END of StatBarBase Class Declaration

DataLog_Stream& operator << (DataLog_Stream& os, STATUS_BAR_TYPE type);



// End of single include guardian define
#endif

/* FORMAT HASH 60e08c80bdd26f86358cc10b203e124e */
