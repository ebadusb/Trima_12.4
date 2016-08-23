/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simDelay.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 ******************************************************************************/

#include "SimDelay.hpp"
#include "guiglobs.hpp"

int SimDelay::_waitFactor = 1;
int SimDelay::_ticksPerSecond = sysClkRateGet();
void SimDelay::ticks(int ticks) { taskDelay(ticks * _waitFactor); }
void SimDelay::setWaitFactor(int _wait_factor) { _waitFactor = _wait_factor; }

void SimDelay::logDelay(int n, const char *units) 
{
   DataLog(log_level_sim_timing_debug) << "Task \"" << taskName(taskIdSelf()) << "\" waiting for " << n;

   if (_waitFactor != 1) DataLog(log_level_sim_timing_debug) << "*" << _waitFactor;

   DataLog(log_level_sim_timing_debug) << " " << units << endmsg;
}

void SimDelay::seconds(int seconds)
{
   logDelay(seconds, "seconds");
   ticks(seconds * _ticksPerSecond);
}

void SimDelay::milliseconds(int milliseconds)
{
   logDelay(milliseconds, "milliseconds");
   ticks(milliseconds * _ticksPerSecond/1000);
}

bool SimDelay::waitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations=20, int waitBetweenIterations=1)
{
   int waitCount = 0;

   // First, loop until we see the correct screen or timeout.
   while (guiglobs::screen_status.get_current_displayed_screen() != expectedScreen && ++waitCount <= maxIterations)
   {
	  DataLog(log_level_sim_timing_error) << "Waiting for screen to get in sync... [curr="
		 << guiglobs::screen_status.get_current_displayed_screen()
		 << "; expected=" << expectedScreen << "; iteration=" << waitCount << "]" << endmsg;

	  seconds(waitBetweenIterations);
   }

   // Did we get to the right place?
   bool correctScreen = (guiglobs::screen_status.get_current_displayed_screen() == expectedScreen);

   // If so, log it and move on.
   if (correctScreen) DataLog (log_level_sim_timing_info) << "Correct screen " << expectedScreen << " observed on iteration " << waitCount << endmsg;
   else
   {
	  // If not, log it and wait 60 seconds.  Maybe the screenshot master will step in and save the day.
	  DataLog (log_level_sim_timing_error) << "\n\n\n\nERROR: You're on screen " 
		 << guiglobs::screen_status.get_current_displayed_screen()
   		 << " and we're looking for screen " << expectedScreen
		 << ".\nI'll give you 5 seconds to get there if you can.\n\n\n\n" << endmsg;;

      seconds(5);			// give the user a chance to go to the correct screen if possible.

	  // Check one last time to see if we got there.
	  bool correctScreen = (guiglobs::screen_status.get_current_displayed_screen() == expectedScreen);

	  // Log it either way.
	  if (correctScreen) DataLog(log_level_sim_timing_info) << "You did it!!! Nice work. Screen " << expectedScreen << " observed." << endmsg;
	  else DataLog(log_level_sim_timing_error) << "Epic fail. Unable to reconcile screens." << endmsg;
   }

   // True if we're on the correct screen, false otherwise.
   return correctScreen;
}


