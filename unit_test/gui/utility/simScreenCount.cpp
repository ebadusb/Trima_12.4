/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenCount.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simScreenCount.cpp $
 * Revision 1.1  2009/07/31 21:22:33Z  dslausb
 * Initial revision
 *
 ******************************************************************************/

#include "simScreenCount.hpp"
#include "guiglobs.hpp"

int SimScreenCount::count() { return count(guiglobs::screen_status.get_current_displayed_screen()); }

int SimScreenCount::count(GUI_SCREEN_ID screenId)
{
	return screenCounter[screenId] = getCount(screenId) + 1;
}

int SimScreenCount::getCount() { return getCount(guiglobs::screen_status.get_current_displayed_screen()); }

int SimScreenCount::getCount(GUI_SCREEN_ID screenId)
{
	if ( screenCounter.find(screenId) == screenCounter.end() )
		screenCounter[screenId] = 0;

	return screenCounter[screenId];
}


