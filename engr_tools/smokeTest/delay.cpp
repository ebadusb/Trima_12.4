/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 * TITLE:      delay.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * MODIFIED BY: Robert Watkins
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 ******************************************************************************/

#include "delay.hpp"
#include "guiglobs.hpp"

int Delay::_waitFactor = 1;
int Delay::_ticksPerSecond = sysClkRateGet();
void Delay::ticks(int ticks) { taskDelay(ticks * _waitFactor); }
void Delay::setWaitFactor(int _wait_factor) { _waitFactor = _wait_factor; }

void Delay::logDelay(int n, const char *units) 
{
   DataLog(log_level_spoofer_debug) << "Task \"" << taskName(taskIdSelf()) << "\" waiting for " << n;

   if (_waitFactor != 1) DataLog(log_level_spoofer_debug) << "*" << _waitFactor;

   DataLog(log_level_spoofer_debug) << " " << units << endmsg;
}

// Returns true if there is an error and false otherwise.
bool Delay::waitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations = 20, int waitBetweenIterations = 1)
{
   int waitCount = 0;
   bool waitForScreenError = false;

   // First, loop until we see the correct screen or timeout.
   while (guiglobs::screen_status.get_current_displayed_screen() != expectedScreen && ++waitCount <= maxIterations)
   {
	  DataLog(log_level_spoofer_debug) << "Waiting for screen to get in sync... [curr="
		 << guiglobs::screen_status.get_current_displayed_screen()
		 << "; expected=" << expectedScreen << "; iteration=" << waitCount << "]" << endmsg;

	  seconds(waitBetweenIterations);
   }
   // Did we get to the right place?
   bool correctScreen = (guiglobs::screen_status.get_current_displayed_screen() == expectedScreen);
   // If so, log it and move on.
   if (correctScreen) 
   {
	   DataLog (log_level_spoofer_debug) << "Correct screen " << expectedScreen << " observed on iteration " << waitCount << endmsg;
   }
   else
   {
	  // If not, log it and wait 60 seconds.  Maybe the screenshot master will step in and save the day.
	  DataLog (log_level_spoofer_debug) << "\n\n\n\nERROR: You're on screen " 
		 << guiglobs::screen_status.get_current_displayed_screen()
   		 << " and we're looking for screen " << expectedScreen
		 << ".\nI'll give you 60 seconds to get there if you can.\n\n\n\n" << endmsg;;

      seconds(60);			// give the user a chance to go to the correct screen if possible.

	  // Check one last time to see if we got there.
	  bool correctScreen = (guiglobs::screen_status.get_current_displayed_screen() == expectedScreen);

	  // Log it either way.
	  if (correctScreen) 
	  {
		  DataLog(log_level_spoofer_debug) << "You did it!!! Nice work. Screen " << expectedScreen << " observed." << endmsg;
	  }
	  else 
	  {
		  DataLog(log_level_spoofer_debug) << "Epic fail. Unable to reconcile screens." << endmsg;
	  }
   }

   if (correctScreen == false)
   {
	   waitForScreenError = true;
   }

   return waitForScreenError;
}

void Delay::milliseconds(int milliseconds)
{
   logDelay(milliseconds, "milliseconds");
   ticks(milliseconds * _ticksPerSecond/1000);
}

void Delay::seconds(int seconds)
{
   logDelay(seconds, "seconds");
   ticks(seconds * _ticksPerSecond);
}

// WRAPPER FUNCTION
void Delay::wrapWaitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations = 20, int waitBetweenIterations = 1)
{
	bool waitForScreenError = Delay::waitForScreen(expectedScreen, maxIterations, waitBetweenIterations);

	// If statement here is different than in the other wrapers
	if (waitForScreenError == true) 
	{
		DataLog (log_level_spoofer_debug) << "Error with SimDelay::waitForScreen()" << endmsg;
		exit(0);
	}
}
