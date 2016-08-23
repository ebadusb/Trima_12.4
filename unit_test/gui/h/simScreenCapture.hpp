/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenCapture.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the action of snapping a screenshot.
 *
 *******************************************************************************/

#ifndef __SIM_SCREEN_CAPTURE_HPP__
#define __SIM_SCREEN_CAPTURE_HPP__

#include <vxworks.h>

#if CPU == SIMNT

#include "datalog.h"
#include "states.h"
#include "guiUnitTestCds.h"
#include "gui.h"
#include <map> 

DataLog_Stream & operator << (DataLog_Stream &os, State_names stateName);

class SimScreenCapture
{
public:

	static unsigned int _screenshotNumber;
private:	
	static bool _forceScreenShot;
	static bool _forceNoLabel;
	static char* _category;
	static bool _useCategory;
	static map<GUI_SCREEN_ID, char*> _categoryMap;
	
protected:

	static void takeScreenshot(const char *filename, bool doMapping = true, bool isPruned = true, const char* label="", bool doLabel=true);
	static void getStringName(char* name, const char *fmt, va_list args, bool doNumbering=true);
	static void getLabelString(char* label, unsigned int options);
	static bool shouldDoMapping(unsigned int options);
	static bool shouldDoPruning(unsigned int options);
	static bool shouldDoLabeling(unsigned int options);
	static char* getCategory(GUI_SCREEN_ID id);
	static void setScreenId(GUI_SCREEN_ID id);
	static void initCategoryMap();

public:

	static bool autoNavOn(bool isOn);
	static bool autoNavOn();
    
	static void screenshot(const char *fmt, ...);
	static void screenshot(unsigned int options, const char *fmt, ...);
	static void screenshot(GUI_SCREEN_ID expectedId, const char *fmt, ...);
	static void screenshot(GUI_SCREEN_ID expectedId, unsigned int options, const char *fmt, ...);
	static void screenshot(GUI_SCREEN_ID expectedId, unsigned int options, const char *fmt, va_list args);
	static void unforceScreenShot();
	static void forceNoLabel();
	static void useCategory();
};

#endif /* if CPU==SIMNT */
#endif // #ifndef __SIM_SCREEN_CAPTURE_HPP__


