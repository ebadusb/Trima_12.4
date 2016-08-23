/*
* Copyright (c) 2007 by Gambro BCT, Inc.  All rights reserved.
*
* TITLE:      aasTransitionDummy.cpp
*
* DESCRIPTION: This is just a dummy screen.  It is "allocated" when the system transitions to
* the metered storage state.  The allocation function, however, does not allocate any text or
* graphics, it simply makes the decision where to go from here.  The options are the flagging
* screens for RBC or platelet, and the clamp channel screen.
*
* In other words, though this class is allocated by the screen commutator based on a state
* transition, it has no graphical components, and it's sole purpose is to determine what
* screen is displayed next and transition to it.
*
*/

#include <vxworks.h>


// BASE CLASS INCLUDE

#include "aasTransitionDummy.hpp"
#include "statBarBase.hpp"
#include "statline.hpp"
#include "runsumGlobs.hpp"


// EXTERNAL REFERENCES

#include "guiglobs.hpp"

// FOR OVERRIDE BUTTONS
// #include "screen_config.hpp"

// #define BAG_VOL_BUTT_X 529
// #define BAG_VOL_BUTT_Y 238


////////////////////////////////////////////////////////////////////////////////

aasTransitionDummy::aasTransitionDummy()
   : Base_Apheresis_Window(MAGENTA, GUI_SCREEN_AAS_TRANSITION_DUMMY)
{}


////////////////////////////////////////////////////////////////////////////////
aasTransitionDummy::~aasTransitionDummy()
{
   aasTransitionDummy::deallocate_resources ();
}


////////////////////////////////////////////////////////////////////////////////
void aasTransitionDummy::allocate_resources (const char* allocation_parameter)
{
   // We're now in metered storage, so make sure the status bar and line
   // reflect that.
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_INFO);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_LOAD);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_PREPARE);
   guiglobs::apheresis_status_bar->set_phase_completed (STATUS_BAR_DONATE);
   guiglobs::apheresis_status_bar->set_phase_active    (STATUS_BAR_SOLUTIONS);
   guiglobs::apheresis_status_line->set_color          (MAGENTA);

   if (guiglobs::screen_status.get_previous_screen() != GUI_SCREEN_POWERUP)
   {
      _msgRecieveRunStats.init(Callback<aasTransitionDummy>(this, &aasTransitionDummy::runStatsRecieved),
                               MessageBase::SNDRCV_RECEIVE_ONLY);
      RunsumGlobs::requestRunsumStats();
   }

   // If the last screen was powerup, don't transition here, let aasRunInfo handle the transition
   // when it gets the PFR message.
}

void aasTransitionDummy::runStatsRecieved ()
{
   DataLog (log_level_gui_info) << "Run summary stats recieved." << endmsg;
   RunsumGlobs::gotoNextRunSummaryScreen(_msgRecieveRunStats.getData());
}


////////////////////////////////////////////////////////////////////////////////
// DEALLOCATE_RESOURCES

void aasTransitionDummy::deallocate_resources ()
{
   _msgRecieveRunStats.deregisterMsg();
}

/* FORMAT HASH 28abd9e4ca19cf8a1335ac7d76ead912 */
