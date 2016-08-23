/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simDelay.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the functionality of task delays.  You
 *	can set the wait factor to globally increase with times.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simDelay.hpp $
 * Revision 1.2  2009/06/11 15:07:42Z  dslausb
 * Update file headers
 *
 ******************************************************************************/

#ifndef __SIM_DELAY_HPP__
#define __SIM_DELAY_HPP__

#include "gui.h"

class SimDelay
{
private:
	static int _ticksPerSecond;
	static int _waitFactor;  // wait time factor.
	static void ticks(int ticks);
	static void logDelay(int n, const char *units); 

public:
	static void seconds(int seconds);
	static void milliseconds(int milliseconds);
	static void setWaitFactor(int _wait_factor);

	static bool waitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations=20, int waitBetweenIterations=1);
};

#endif // #ifndef __SIM_DELAY_HPP__

