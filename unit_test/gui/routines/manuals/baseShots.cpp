/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      manualShotsNav.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT

// From common
#include "trima_datalog.h"

// From GUI
#include "gui.h"
#include "guiglobs.hpp"
#include "sealAndDetach.hpp"

// From unit_test
#include "baseShots.hpp"

#include "simScreenCapture.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "scrncomm.hpp"
#include "simActionQueue.hpp"

/*********************************************************************************************
									Constructor
**********************************************************************************************/

BaseShots::BaseShots(): SimRoutineBase() {}

/*********************************************************************************************
									Destructor
**********************************************************************************************/

BaseShots::~BaseShots()
{
	DataLog (log_level_sim_main_routine_error) << "BaseShots destructor.  That's bad." << endmsg;
};

/*********************************************************************************************
									Utility Functions
**********************************************************************************************/

void BaseShots::populateActionQueue()
{
	// Not implemented
}


GUI_SCREEN_ID BaseShots::currentScreen()
{
	GUI_SCREEN_ID returnScreen = guiglobs::screen_status.get_current_displayed_screen();

	if (returnScreen == GUI_SCREEN_KEYPAD ||
		returnScreen == GUI_SCREEN_LISTBOX ||
		returnScreen == GUI_SCREEN_PLT_LISTBOX ||
		returnScreen == GUI_SCREEN_PLAS_LISTBOX ||
		returnScreen == GUI_SCREEN_RBC_LISTBOX ||
		returnScreen == GUI_SCREEN_SET_LISTBOX ||
		returnScreen == GUI_SCREEN_LANG_LISTBOX ||
		returnScreen == GUI_SCREEN_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_AC_CONNECT_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_REBOOT_CONFIRM_BOX ||
		returnScreen == GUI_SCREEN_PREDICT_CONFIRM_BOX)
	{
		DataLog (log_level_sim_main_routine_info) << "Cannot use child screen " << returnScreen
			<< " as current.  Using ";
		returnScreen = guiglobs::screen_status.get_current_screen();
		DataLog (log_level_sim_main_routine_info) << returnScreen << " instead." << endmsg;
	}

	return returnScreen;
}


/*********************************************************************************************
									Message Handlers
**********************************************************************************************/

// This is the switch statement that handles screen changes.  This is the primary method for
// progressing the screenshot routine along.  Whenver a new static screen gets allocated, this
// function gets called, and you can take action depending on the new screen.
// If adding new stuff, it's preferable to try to keem this function very clean so that you can
// easily infer navigation just by glancing at it.  If you're adding more than a couple lines to
// any particular case, consider putting it in a separate function and calling that instead.
// Also, to the extent possible, try to keep them in the order that the routine will visit them.
void BaseShots::dealWithScreenChange()
{
	if ( ! SimUtilities::autoNavOn() ) return; // Don't do anything if we're not taking screenshots.

	// Let's wait a second to make sure all screen allocation stuff takes place.
	SimDelay::seconds(1);

	GUI_SCREEN_ID newScreen = currentScreen();
	GUI_SCREEN_ID oldScreen = guiglobs::screen_status.get_previous_displayed_screen();

	// Increment and return the number of times this particular screen has been displayed.
	int scrnCount = SimUtilities::screenCounter.count(newScreen);

	DataLog (log_level_sim_main_routine_info) << "Screen Change message received.  Transitioned from "
		<< oldScreen << " to " << newScreen << "; scrnCount=" << scrnCount << endmsg;

	switch (newScreen)
	{
	#define __SCREEN_CHANGE_SWITCH__
	#include "screenIdMacros.hpp"
	#undef __SCREEN_CHANGE_SWITCH__

	default:
		DataLog(log_level_sim_main_routine_info) << "Screen display " << newScreen << " not handled" << endmsg;
		break;
	}
}


//
// For the run summary screens, when the screen gets allocated,
// it requests a run summar struct.  The screen display really
// isn't complete until the screen recieves and processes that
// data, even though it's allocate_resources() function has been
// called.  This function deals with the run summary screen after
// it's had a chance to process the info.
// 
void BaseShots::dealWithRunSumProcessed()
{
	if ( ! SimUtilities::autoNavOn() ) return; // Don't do anything if we're not taking screenshots.

	// Let's wait a second to make sure all processing has taken place.
	SimDelay::seconds(1);
	GUI_SCREEN_ID newScreen = currentScreen();

	SimUtilities::runsumResponseCounter.count(newScreen);

	DataLog (log_level_sim_main_routine_info) << "Run summary processed message recieved from screen "
		<< newScreen << endmsg;

	switch (newScreen)
	{
	#define __RUN_SUM_SWITCH__
	#include "screenIdMacros.hpp"
	#undef __RUN_SUM_SWITCH__

	default:
		DataLog(log_level_sim_main_routine_info) << "Run summary stats for " << newScreen << " not handled" << endmsg;
		break;
	}
}

#define __NAV_FUNC_IMPLEMENTATIONS__
#include "screenIdMacros.hpp"
#undef __NAV_FUNC_IMPLEMENTATIONS__

#endif /* CPU == SIMNT */



