/************************> FOCUSSED GUI DOMAIN <****************************

*     Copyright(c) 2009 CaridianBCT, Inc. All rights reserved

*     Author:      Dan Slauson

*     Class name:  StatBarBase

*     File name:   statBarBase.cpp

*     Contents:    Implementations of class methods

*     Description: See descriptions in corresponding .hpp file.

*************************< FOCUSSED GUI DOMAIN >***************************/

#include <vxworks.h>
#include "statBarBase.hpp"
#include "aphwind.hpp"
#include "guiglobs.hpp"
#include "statBarNoSolution.hpp"
#include "statBarWithSolution.hpp"

#if CPU==SIMNT
#include "stringError.h"
#endif

Bitmap*                 StatBarBase::_bmpStatusPlacard = NULL;

Display_Text*           StatBarBase::_txtLabelLoad     = NULL;
Display_Text*           StatBarBase::_txtLabelInfo     = NULL;
Display_Text*           StatBarBase::_txtLabelPrepare  = NULL;
Display_Text*           StatBarBase::_txtLabelDonate   = NULL;
Display_Text*           StatBarBase::_txtLabelAddition = NULL;
Display_Text*           StatBarBase::_txtLabelUnload   = NULL;

GUI_SCREEN_ID           StatBarBase::_parentScreen     = GUI_SCREEN_NULL_SCREEN;

STATUS_BAR_PHASE_STATES StatBarBase::_currentPhase;

map<string, BITMAP_ID>  StatBarBase::statBarGraphics;

DataLog_Stream&         operator << (DataLog_Stream& os, STATUS_BAR_TYPE type)
{
   switch (type)
   {
      case STATUS_BAR_TYPE_NO_ADDITION :   os << "NoAddition"  ; break;
      case STATUS_BAR_TYPE_WITH_ADDITION : os << "WithAddition"; break;
      default :                            os << "Unknown"     ; break;
   }

   return os << "(" << (int)type << ")";
}

////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTOR
//

StatBarBase::StatBarBase (STATUS_BAR_TYPE type,
                          TEXT_STRING_ID loadString,
                          TEXT_STRING_ID infoString,
                          TEXT_STRING_ID prepString,
                          TEXT_STRING_ID donateString,
                          TEXT_STRING_ID additionString,
                          TEXT_STRING_ID unloadString) :

   OSWindow(*GuiMain::getDisplay()),
   _type(type)
{
   if (!_bmpStatusPlacard) _bmpStatusPlacard = new Bitmap(0, 0, BITMAP_NULL);

   if (_txtLabelLoad && _txtLabelLoad->getStringId() != loadString)
   {
      DataLog (log_level_gui_info) << "Deleting load string " << *_txtLabelLoad << endmsg;
      delete _txtLabelLoad;
      _txtLabelLoad = NULL;
   }

   if (_txtLabelInfo && _txtLabelInfo->getStringId() != infoString)
   {
      DataLog (log_level_gui_info) << "Deleting info string " << *_txtLabelInfo << endmsg;
      delete _txtLabelInfo;
      _txtLabelInfo = NULL;
   }

   if (_txtLabelPrepare && _txtLabelPrepare->getStringId() != prepString)
   {
      DataLog (log_level_gui_info) << "Deleting prep string " << *_txtLabelPrepare << endmsg;
      delete _txtLabelPrepare;
      _txtLabelPrepare = NULL;
   }

   if (_txtLabelDonate && _txtLabelDonate->getStringId() != donateString)
   {
      DataLog (log_level_gui_info) << "Deleting donate string " << *_txtLabelDonate << endmsg;
      delete _txtLabelDonate;
      _txtLabelDonate = NULL;
   }

   if (_txtLabelAddition && _txtLabelAddition->getStringId() != additionString)
   {
      DataLog (log_level_gui_info) << "Deleting addition string " << *_txtLabelAddition << endmsg;
      delete _txtLabelAddition;
      _txtLabelAddition = NULL;
   }

   if (_txtLabelUnload && _txtLabelUnload->getStringId() != unloadString)
   {
      DataLog (log_level_gui_info) << "Deleting unload string " << *_txtLabelUnload << endmsg;
      delete _txtLabelUnload;
      _txtLabelUnload = NULL;
   }

   if (!_txtLabelLoad)
   {
      _txtLabelLoad = new Display_Text(loadString);
      DataLog (log_level_gui_info) << "Created new load string from ID \"" << loadString << "\": " << *_txtLabelLoad << endmsg;
   }

   if (!_txtLabelInfo)
   {
      _txtLabelInfo = new Display_Text(infoString);
      DataLog (log_level_gui_info) << "Created new info string from ID \"" << infoString << "\": " << *_txtLabelInfo << endmsg;
   }

   if (!_txtLabelPrepare)
   {
      _txtLabelPrepare = new Display_Text(prepString);
      DataLog (log_level_gui_info) << "Created new prep string from ID \"" << prepString << "\": " << *_txtLabelPrepare << endmsg;
   }

   if (!_txtLabelDonate)
   {
      _txtLabelDonate = new Display_Text(donateString);
      DataLog (log_level_gui_info) << "Created new donate string from ID \"" << donateString << "\": " << *_txtLabelDonate << endmsg;
   }

   if (!_txtLabelAddition)
   {
      _txtLabelAddition = new Display_Text(additionString);
      DataLog (log_level_gui_info) << "Created new addition string from ID \"" << additionString << "\": " << *_txtLabelAddition << endmsg;
   }

   if (!_txtLabelUnload)
   {
      _txtLabelUnload = new Display_Text(unloadString);
      DataLog (log_level_gui_info) << "Created new unload string from ID \"" << unloadString << "\": " << *_txtLabelUnload << endmsg;
   }
}  // END of StatBarBase CONSTRUCTOR


StatBarBase::StatBarBase()
   : OSWindow(*GuiMain::getDisplay()),
     _type(STATUS_BAR_TYPE_NO_ADDITION)
{
   _FATAL_ERROR(__FILE__, __LINE__, "Unimplemented constructor");
}

////////////////////////////////////////////////////////////////////////////////
// DESTRUCTOR
StatBarBase::~StatBarBase ()
{
   unparent_status_bar(get_owning_screen());
   StatBarBase::deallocate_resources();
}

////////////////////////////////////////////////////////////////////////////////
// ALLOCATE RESOURCES

void StatBarBase::allocate_resources ()
{
   // Update the appropriate graphic and text colors
   update_status_graphics_and_text ();

   //
   //  Size window to loaded bitmap size
   //
   OSRegion windowreg;
   if (_bmpStatusPlacard)
      windowreg = _bmpStatusPlacard->get_dimensions();
   windowreg.x = STATBAR_X;
   windowreg.y = STATBAR_Y;
   set_region(windowreg);
} // END of StatBarBase ALLOCATE RESOURCES


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE RESOURCES

void StatBarBase::deallocate_resources ()
{
   // Set parenthood off of the current window
   unparent_status_bar (get_owning_screen ()) ;

   // Deallocate all the constituent objects.
   if (_bmpStatusPlacard) _bmpStatusPlacard->deallocate_resources ();

   if (_txtLabelLoad) _txtLabelLoad->deallocate_resources();
   if (_txtLabelInfo) _txtLabelInfo->deallocate_resources();
   if (_txtLabelPrepare) _txtLabelPrepare->deallocate_resources();
   if (_txtLabelDonate) _txtLabelDonate->deallocate_resources();
   if (_txtLabelAddition) _txtLabelAddition->deallocate_resources();
   if (_txtLabelUnload) _txtLabelUnload->deallocate_resources();
}  // END of StatBarBase DEALLOCATE RESOURCES

////////////////////////////////////////////////////////////////////////////////
// RESET_ALL_PHASES

void StatBarBase::reset_all_phases ()
{
   _currentPhase.status_load      = STATUS_BAR_STATE_RESET ;
   _currentPhase.status_info      = STATUS_BAR_STATE_RESET ;
   _currentPhase.status_prepare   = STATUS_BAR_STATE_RESET ;
   _currentPhase.status_donate    = STATUS_BAR_STATE_RESET ;
   _currentPhase.status_unload    = STATUS_BAR_STATE_RESET ;
   _currentPhase.status_solutions = STATUS_BAR_STATE_RESET ;

   // Update the appropriate graphic and text colors
   update_status_graphics_and_text ();

}  // END of StatBarBase RESET_ALL_PHASES


////////////////////////////////////////////////////////////////////////////////
// SET_PHASE_ACTIVE

void StatBarBase::set_phase_active (const STATUS_BAR_PHASE sb_phase)
{

   if ( set_phase (sb_phase, STATUS_BAR_STATE_ACTIVE) )
   {
      // Update the appropriate graphic and text colors, if
      // something changed
      update_status_graphics_and_text ();
   }

}  // END of StatBarBase SET_PHASE_ACTIVE



////////////////////////////////////////////////////////////////////////////////
// SET_PHASE_COMPLETED

void StatBarBase::set_phase_completed (const STATUS_BAR_PHASE sb_phase)
{
   if ( set_phase (sb_phase, STATUS_BAR_STATE_COMPLETE) )
   {
      // Update the appropriate graphic and text colors, if
      // something changed
      update_status_graphics_and_text ();
   }
}  // END of StatBarBase SET_PHASE_COMPLETED



////////////////////////////////////////////////////////////////////////////////
// SET_PHASE

bool StatBarBase::set_phase (const STATUS_BAR_PHASE sb_phase, const STATUS_BAR_STATE sb_phase_state, bool forceUpdate = false)
{
   // If the setting of a phase CHANGES the status array, this routine
   // returns a TRUE. If it doesn't change the status array, it returns
   // a FALSE.
   if (sb_phase_state != STATUS_BAR_STATE_RESET &&
       sb_phase_state != STATUS_BAR_STATE_ACTIVE &&
       sb_phase_state != STATUS_BAR_STATE_COMPLETE)
   {
      _FATAL_ERROR (__FILE__, __LINE__, "Invalid Status Bar state assignment");
      return false;
   }

   bool returnVal = false;

   switch (sb_phase)
   {
      case STATUS_BAR_LOAD :

         if (_currentPhase.status_load != sb_phase_state)
         {
            _currentPhase.status_load = sb_phase_state ;
            returnVal                 = true;
         }
         break;

      case STATUS_BAR_INFO :

         if (_currentPhase.status_info != sb_phase_state)
         {
            _currentPhase.status_info = sb_phase_state ;
            returnVal                 = true;
         }
         break;

      case STATUS_BAR_PREPARE :

         if (_currentPhase.status_prepare != sb_phase_state)
         {
            _currentPhase.status_prepare = sb_phase_state ;
            returnVal                    = true;
         }
         break;

      case STATUS_BAR_DONATE :

         if (_currentPhase.status_donate != sb_phase_state)
         {
            _currentPhase.status_donate = sb_phase_state ;
            returnVal                   = true;
         }
         break;

      case STATUS_BAR_UNLOAD :

         if (_currentPhase.status_unload != sb_phase_state)
         {
            _currentPhase.status_unload = sb_phase_state ;
            returnVal                   = true;
         }
         break;

      case STATUS_BAR_SOLUTIONS :

         if (_currentPhase.status_solutions != sb_phase_state)
         {
            _currentPhase.status_solutions = sb_phase_state ;
            returnVal = true;
         }
         break;

      default :
         _FATAL_ERROR (__FILE__, __LINE__, "Invalid Status Bar state spec'ed for assignment");
         break;
   }  // End of "switch (sb_phase)...

   if (forceUpdate) update_status_graphics_and_text();

   return returnVal;
}  // END of StatBarBase SET_PHASE



////////////////////////////////////////////////////////////////////////////////
// REPARENT_STATUS_BAR

void StatBarBase::reparent_status_bar (Base_Window& new_parent)
{
   detach();
   attach(&new_parent);

   // Establish the ownership of the status bar
   set_owning_screen (new_parent.get_screen_id()) ;
}  // END of StatBarBase REPARENT_STATUS_BAR


////////////////////////////////////////////////////////////////////////////////
// UNPARENT_STATUS_BAR

void StatBarBase::unparent_status_bar (GUI_SCREEN_ID unlinking_request_screen)
{
   // Only let the owning screen of the status bar "unparent" the status bar
   if (unlinking_request_screen == get_owning_screen ())
   {
      detach();

      // Establish the ownership of the status bar
      set_owning_screen (GUI_SCREEN_NULL_SCREEN);
   }

}   // END of StatBarBase UNPARENT_STATUS_BAR



////////////////////////////////////////////////////////////////////////////////
// CONVERT_STATUS_TO_STRCHAR

char StatBarBase::convert_status_to_strchar (const STATUS_BAR_STATE status_state)
{
   switch (status_state)
   {
      case STATUS_BAR_STATE_ACTIVE :     return 'Y';
      case STATUS_BAR_STATE_COMPLETE :   return 'G';
      default :                          return 'C';
   } // End of switch (status_state)...
}    // END of StatBarBase CONVERT_STATUS_TO_STRCHAR


////////////////////////////////////////////////////////////////////////////////
// CONVERT_STATUS_TO_COLOR

TEXT_COLOR StatBarBase::convert_status_to_color (const STATUS_BAR_STATE status_state)
{
   switch (status_state)
   {
      case STATUS_BAR_STATE_ACTIVE :     return YELLOW;
      case STATUS_BAR_STATE_COMPLETE :   return GREY_DARK;
      default :                          return GREY_LIGHT;
   } // End of switch (status_state)...
}   // END of StatBarBase CONVERT_STATUS_TO_COLOR


string StatBarBase::getStatusString ()
{
   DataLog (log_level_gui_error) << "getStatusString() not implemented at this level" << endmsg;
   return "junk";
}


BITMAP_ID StatBarBase::getStatusBmp ()
{
   string statusString = getStatusString();

   if (statBarGraphics.find(getStatusString() ) == statBarGraphics.end())
   {
      // Debug logging
      // DataLog (log_level_gui_error) << "getStatusBmp() unable to find bitmap associated with \"" << statusString << "\". Returning " << defaultBmp() << " instead." << endmsg;
      return defaultBmp();
   }
   else
   {
      // Debug logging
      // DataLog (log_level_gui_info) << "getStatusBmp() matched status string \"" << statusString << "\" with bitmap " << statBarGraphics[statusString] << "." << endmsg;
      return statBarGraphics[statusString];
   }
}

void StatBarBase::dumpInfo (DataLog_Stream& os, const char* file, int line, const char* log = "")
{
   os << "---- begin stat bar dump (from " << file << "[" << line << "]: " << log << ") ----" << endmsg
      << "type=" << _type << "; parent=" << _parentScreen << endmsg;

   if (_txtLabelLoad) os << "_txtLabelLoad: "      << *_txtLabelLoad       << endmsg;
   if (_txtLabelLoad) os << "_txtLabelInfo: "      << *_txtLabelInfo       << endmsg;
   if (_txtLabelLoad) os << "_txtLabelPrepare: "   << *_txtLabelPrepare    << endmsg;
   if (_txtLabelLoad) os << "_txtLabelDonate: "    << *_txtLabelDonate     << endmsg;
   if (_txtLabelLoad) os << "_txtLabelAddition: "  << *_txtLabelAddition   << endmsg;
   if (_txtLabelLoad) os << "_txtLabelUnload: "    << *_txtLabelUnload     << endmsg;
   if (_bmpStatusPlacard) os << "_bmpStatusPlacard: " << _bmpStatusPlacard->get_id() << endmsg;

   os << "---- end stat bar dump (from " << file << "[" << line << "]) ----" << endmsg;
}


void StatBarBase::update_status_graphics_and_text ()
{
   if (_bmpStatusPlacard)
   {
      _bmpStatusPlacard->set_id(getStatusBmp());
      _bmpStatusPlacard->allocate_resources   (*this);
   }

   if (_txtLabelLoad)
   {
      _txtLabelLoad->set_color(convert_status_to_color(_currentPhase.status_load));
      if (!_txtLabelLoad->allocated()) _txtLabelLoad->allocate_resources(*this);
   }

   if (_txtLabelInfo)
   {
      _txtLabelInfo->set_color(convert_status_to_color(_currentPhase.status_info));
      if (!_txtLabelInfo->allocated()) _txtLabelInfo->allocate_resources(*this);
   }

   if (_txtLabelPrepare)
   {
      _txtLabelPrepare->set_color(convert_status_to_color(_currentPhase.status_prepare));
      if (!_txtLabelPrepare->allocated()) _txtLabelPrepare->allocate_resources(*this);
   }

   if (_txtLabelDonate)
   {
      _txtLabelDonate->set_color(convert_status_to_color(_currentPhase.status_donate));
      if (!_txtLabelDonate->allocated()) _txtLabelDonate->allocate_resources(*this);
   }

   if (_txtLabelAddition)
   {
      _txtLabelAddition->set_color(convert_status_to_color(_currentPhase.status_solutions));
      if (!_txtLabelAddition->allocated()) _txtLabelAddition->allocate_resources(*this);
   }

   if (_txtLabelUnload)
   {
      _txtLabelUnload->set_color(convert_status_to_color(_currentPhase.status_unload));
      if (!_txtLabelUnload->allocated()) _txtLabelUnload->allocate_resources(*this);
   }
}

#if CPU==SIMNT // Simulator-only functions for screenshot routines

STATUS_BAR_STATE StatBarBase::getSuggestedState (State_names systemState, STATUS_BAR_PHASE phase)
{
   if (phase < FIRST_STATUS_BAR_PHASE || phase > LAST_STATUS_BAR_PHASE)
   {
      DataLog (log_level_gui_error) << "Illegal phase " << phase << " passed to getSuggestedState();" << endmsg;
      return STATUS_BAR_STATE_RESET;
   }

   if (systemState < STARTUP_TESTS || systemState > METERED_STORAGE_DISCONNECT)
   {
      DataLog (log_level_gui_error) << "Illegal system state " << systemState << " at getSuggestedState();" << endmsg;
      return STATUS_BAR_STATE_RESET;
   }

   return _statBarStateTable[systemState - STARTUP_TESTS][phase];
}


STATUS_BAR_STATE StatBarBase::_statBarStateTable[METERED_STORAGE_DISCONNECT - STARTUP_TESTS + 1][LAST_STATUS_BAR_PHASE + 1] =
{
   {  // ---STARTUP_TESTS---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---DISPOSABLE_TEST---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---AC_CONNECTED---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---AC_PRIME---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---DONOR_CONNECTED---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---BLOOD_PRIME---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---BLOOD_RUN---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---BLOOD_RINSEBACK---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---DONOR_DISCONNECT---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_RESET
   },

   {  // ---POST_RUN---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_ACTIVE,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_COMPLETE
   },

   {  // ---METERED_STORAGE---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_RESET,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_ACTIVE
   },

   {  // ---METERED_STORAGE_DISCONNECT---
      /* STATUS_BAR_LOAD			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_INFO			*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_PREPARE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_DONATE		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_UNLOAD		*/STATUS_BAR_STATE_COMPLETE,
      /* STATUS_BAR_SOLUTIONS	*/STATUS_BAR_STATE_ACTIVE
   }
};

void StatBarBase::setState (const State_names systemState)
{
   reset_all_phases();

   for ( int phaseIndex = FIRST_STATUS_BAR_PHASE; phaseIndex <= LAST_STATUS_BAR_PHASE; phaseIndex++ )
      set_phase((STATUS_BAR_PHASE)phaseIndex, getSuggestedState(systemState, (STATUS_BAR_PHASE)phaseIndex));

   update_status_graphics_and_text();
}

#endif // #if CPU==SIMNT

/* FORMAT HASH a8bee781ee5e411f4ead596cb029a60b */
