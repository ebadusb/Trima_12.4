/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * Copyright (c) 2011 CaridianBCT, Inc. All rights reserved.
 *
 * TITLE:      delay.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * MODIFIED BY: Robert Watkins
 *
 * DESCRIPTION: This class encapsulates the functionality of task delays.  You
 *	can set the wait factor to globally increase with times.
 *
 ******************************************************************************/

#ifndef DELAY_HPP__
#define DELAY_HPP__

#include <gui.h>

class Delay
{
private:
	static int _ticksPerSecond;
	static int _waitFactor;  // wait time factor.
	static void ticks(int ticks);
	static void logDelay(int n, const char *units); 

protected:
	/*!
	 * Waits for expectedScreen for maxIterations where each iteration takes 
	 * waitBetweenIterations seconds.
	 *
	 * expectedScreen:
     * The screen that we want to wait for.
	 *
	 * maxIterations:
	 * The number of times that we check to see if the current screen is expectedScreen.
	 *
	 * waitBetweenIterations:
	 * The number of seconds we wait between each iteration.
	 *
	 * Return Value:
	 * If expectedScreen was never gotten to than true is returned (error).
	 * If we got to expectedScreen than false is returned (no error).
     */
    static bool waitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations = 20, int waitBetweenIterations = 1);

public:
	/*!
     * taskDelay() for a specified number of milliseconds.
	 *
	 * milliseconds:
	 * The number of milliseconds we want to taskDelay() for.
     */
	static void milliseconds(int milliseconds);

	/*!
     * taskDelay() for a specified number of seconds.
	 *
	 * seconds:
	 * The number of seconds we want to taskDelay() for.
     */
	static void seconds(int seconds);

	static void setWaitFactor(int _wait_factor);

	/*!
	 * If there is an error with waitForScreen(), then a DataLog error message is printed and
	 * the program is exited.
     */
	static void wrapWaitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations = 20, int waitBetweenIterations = 1);
};

#endif // #ifndef DELAY_HPP__

