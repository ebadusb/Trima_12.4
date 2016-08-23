/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenTouch.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the functionality of simulating screen
 *	touches.  That's pretty much it.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simScreenTouch.hpp $
 * Revision 1.1  2009/06/11 15:30:16Z  dslausb
 * Initial revision
 * Revision 1.8  2009/06/11 15:07:43Z  dslausb
 * Update file headers
 *
 ******************************************************************************/


#ifndef __SIM_SCREEN_TOUCH_HPP__
#define __SIM_SCREEN_TOUCH_HPP__

#include "ugl\uglinput.h"
#include "ugl\ugl.h"
#include "ugl\uglevent.h"
#include "ugl\uglwin.h"
#include "guiglobs.hpp"

class SimScreenTouch
{
private:
	static UGL_DEVICE_ID	_uglDisplay;
    static UGL_INPUT_SERVICE_ID _uglInputServiceId;
	static UGL_INPUT_DEV_ID _pointerDevId;
	static bool _initialized;

public:
	static void init();
	static void keypadSequence(const char *str);
	static void touch(GUI_BUTTON_ID button_id, GUI_SCREEN_ID screen_id);
private:
	static void touch(GUI_BUTTON_ID button_id, short offset_x, short offset_y);
	static bool waitForScreen(GUI_SCREEN_ID expectedScreen, int maxIterations=20, int waitBetweenIterations=1);
	static GUI_BUTTON_ID charToKeypadBtn(char ch);

private:  // the function below should be private.  You should be able to use only
	     // the GUI_BUTTON_ID functions, but that's not always possible yet.
	static void touch(short x, short y);
};

#endif /* ifndef __SIM_SCREEN_TOUCH_HPP__ */

