/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenCount.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This is a simple class that keeps a count for each screen.
 * 		You can use this count for whatever you want, but generally it
 *		will be used to track how many times you've encountered a string
 *		through the course of a run of the screenshot script.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simScreenCount.hpp $
 * Revision 1.1  2009/07/31 21:22:33Z  dslausb
 * Initial revision
 *
 ******************************************************************************/

#ifndef __SIM_SCREEN_COUNT_HPP__
#define __SIM_SCREEN_COUNT_HPP__

#include <map>
#include "gui.h"

using namespace std;

class SimScreenCount
{
public:
	typedef map<GUI_SCREEN_ID, int> COUNTMAP;

private:
	COUNTMAP screenCounter;

public:
	//
	// count() increments the count for the
	// passed or current screen and returns
	// the new count
	//
	int count();
	int count(GUI_SCREEN_ID screenId);

	//
	// getCount() returns the current count for
	// the passed or current screen without
	// incrementing it.
	//
	int getCount();
	int getCount(GUI_SCREEN_ID screenId);
};

#endif // #ifndef __SIM_SCREEN_COUNT_HPP__


