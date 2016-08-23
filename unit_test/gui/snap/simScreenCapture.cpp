/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simScreenCapture.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT
#include "simScreenCapture.hpp"
#include "trima_datalog.h"
#include "config_cds.h"
#include "guiglobs.hpp"
#include "simDelay.hpp"
#include "string_screen_mapper.hpp"
#include "bwindow.hpp"
#include "alarmcontainer.h"
#include "simScreenTouch.hpp"
#include "substatemap.h"
#include "screen_config.hpp"
#include "statline.hpp"
#include "guiUnitTestCds.h"
#include "simUtilities.hpp"

unsigned int SimScreenCapture::_screenshotNumber = 0;
bool SimScreenCapture::_forceScreenShot = true;
bool SimScreenCapture::_forceNoLabel = false;
char* SimScreenCapture::_category = NULL;
bool SimScreenCapture::_useCategory = false;
map<GUI_SCREEN_ID, char*> SimScreenCapture::_categoryMap;

bool SimScreenCapture::autoNavOn(bool isOn)	{ return SimUtilities::autoNavOn(isOn);	}
bool SimScreenCapture::autoNavOn()			{ return SimUtilities::autoNavOn();		}

void SimScreenCapture::takeScreenshot(const char *filename, bool doMapping = true, bool isPruned = true, const char* label="", bool doLabel=true)
{
	DataLog (log_level_gui_info) << "SimScreenCapture::takeScreenshot(name=\"" << filename << "\"; map="
		<< doMapping << "; pruned=" << isPruned << "; lbl=\"" << label << "\"; doLbl=" << doLabel << ")" << endmsg;

    bool showedName = false, showedInfo=false;

	string_screen_mapper::start_screenshot(filename);

	if (!_forceNoLabel && doLabel)
	{
		showedName = true;
		SimUtilities::setScreenshotName(filename);

		if (label)
		{
			showedInfo = true;
			SimUtilities::setScreenshotInfo(label);
		}
		else DataLog (log_level_sim_screenshot_error) << "NULL screenshot label for filename " << filename << "." << endmsg;
	}

    guiglobs::guiMain->getDisplay()->flush();
    

    if(_forceScreenShot || string_screen_mapper::hasNewStrings() || !isPruned)
	{
		guistring fileString(filename);
        Base_Window::screenshot(fileString);

		DataLog (log_level_sim_screenshot_info) << "takeScreenshot(name=\"" << filename << "\"; map=" << doMapping
			<< "; prune=" << isPruned << "; lbl=\"" << label << "\"; doLbl=" << doLabel << ") finished" << endmsg;
        
		string_screen_mapper::stop_screenshot();

		_screenshotNumber++;
	}
	else
	{
		DataLog (log_level_sim_screenshot_info) << "takeScreenshot(\"" << filename << "\") not taken since there are no strings." << endmsg;
        string_screen_mapper::reset();
	
		SimDelay::milliseconds(250);
	}

	if (showedName) SimUtilities::removeScreenshotName();
	if (showedInfo) SimUtilities::removeScreenshotInfo();
}

void SimScreenCapture::getStringName(char* name, const char *fmt, va_list args, bool doNumbering=true)
{
	char filenameFormat[2048];

	if ((!name)||(!fmt))
	{
		DataLog (log_level_sim_screenshot_error) << "ERROR: Unusable screenshot data." << endmsg;
		return;
	}

	if( _category != NULL && _useCategory == true )
		sprintf(filenameFormat, "%s_%s", _category, fmt);
	else if (doNumbering) sprintf(filenameFormat, "N%03i_%s", _screenshotNumber, fmt);
	else sprintf(filenameFormat, "%s", fmt);

	vsprintf(name, filenameFormat, args);

    for (char *ch=name; *ch!='\0'; ++ch)
		if (*ch == ' ') *ch = '_';			// convert spaces to underscores.
}

bool SimScreenCapture::shouldDoMapping(unsigned int options)
{
	return !(options & SHOT_OPTS_NO_MAPPING);
}

bool SimScreenCapture::shouldDoPruning(unsigned int options)
{
	return !(options & SHOT_OPTS_NO_PRUNING);
}

bool SimScreenCapture::shouldDoLabeling(unsigned int options)
{
	return !(options & 	SHOT_OPTS_NO_LABELING);
}

void SimScreenCapture::getLabelString(char* label, unsigned int options)
{
	if (!label)
	{
		DataLog (log_level_sim_screenshot_error) << "ERROR: Unusable screenshot data." << endmsg;
		return;
	}

	bool firstConcatenation=true;

	// Wipe out string data.
	sprintf(label, "");

	// If we said not to label, return an empty string.
	if (!shouldDoLabeling(options))
	{
		DataLog (log_level_gui_info) << "Should not do labeling, apparently." << endmsg;
		return;
	}

	if (options & SHOT_OPTS_LBL_JAPAN_ONLY)
	{
		if (!firstConcatenation) strcat(label, " ");
		else firstConcatenation = false;
		strcat(label, SCREENSHOT_LABEL_JAPAN_ONLY);
	}

	if (options & SHOT_OPTS_LBL_EVERYONE_BUT_JAPAN)
	{
		if (!firstConcatenation) strcat(label, " ");
		else firstConcatenation = false;
		strcat(label, SCREENSHOT_LABEL_EVERYONE_BUT_JAPAN);
	}

	if (options & SHOT_OPTS_LBL_VISTA_ONLY)
	{
		if (!firstConcatenation) strcat(label, " ");
		else firstConcatenation = false;
		strcat(label, SCREENSHOT_LABEL_VISTA_ONLY);
	}

	if (options & SHOT_OPTS_LBL_INCORRECT)
	{
		if (!firstConcatenation) strcat(label, " ");
		else firstConcatenation = false;
		strcat(label, SCREENSHOT_LABEL_INCORRECT);
	}

	if (options & SHOT_OPTS_LBL_AAS_ONLY)
	{
		if (!firstConcatenation) strcat(label, " ");
		else firstConcatenation = false;
		strcat(label, SCREENSHOT_LABEL_AAS_ONLY);
	}
}

void SimScreenCapture::screenshot(const char *fmt, ...)
{
	char filename[2048];

    va_list args;
	va_start(args, fmt);
	getStringName(filename, fmt, args);
	va_end(args);

	takeScreenshot(filename);
	DataLog (log_level_gui_info) << "using screenshot 1: " << filename << endmsg;
}

void SimScreenCapture::screenshot(unsigned int options, const char *fmt, ...)
{
	char lblString[2048];
	char filename[2048];

    va_list args;
	va_start(args, fmt);
	getStringName(filename, fmt, args, shouldDoLabeling(options));
	va_end(args);

	getLabelString(lblString, options);
	takeScreenshot(filename,shouldDoMapping(options),shouldDoPruning(options),lblString,shouldDoLabeling(options));
	DataLog (log_level_gui_info) << "using screenshot 2: " << filename << endmsg;
}

void SimScreenCapture::screenshot(GUI_SCREEN_ID expectedId, const char *fmt, ...)
{
	char filename[2048];

   va_list args;
	va_start(args, fmt);
	setScreenId(expectedId);
	getStringName(filename, fmt, args);
	va_end(args);
		
	SimDelay::waitForScreen(expectedId);
	takeScreenshot(filename);
	DataLog (log_level_gui_info) << "using screenshot 3: " << filename << endmsg;
}

void SimScreenCapture::screenshot(GUI_SCREEN_ID expectedId, unsigned int options, const char *fmt, va_list args)
{
	char lblString[2048];
	char filename[2048];
	
	setScreenId(expectedId);
	getStringName(filename, fmt, args, shouldDoLabeling(options));
	getLabelString(lblString, options);

	SimDelay::waitForScreen(expectedId);
	takeScreenshot(filename,shouldDoMapping(options),shouldDoPruning(options),lblString,shouldDoLabeling(options));
	DataLog (log_level_gui_info) << "using screenshot 4: " << filename << endmsg;
}

void SimScreenCapture::screenshot(GUI_SCREEN_ID expectedId, unsigned int options, const char *fmt, ...)
{
    va_list args;
	va_start(args, fmt);
	setScreenId(expectedId);	screenshot(expectedId, options, fmt, args);
	va_end(args);
	DataLog (log_level_gui_info) << "using screenshot 5: nofilename"  << endmsg;
}

void SimScreenCapture::unforceScreenShot()
{
	_forceScreenShot = false;
}

void SimScreenCapture::forceNoLabel(){
	_forceNoLabel = true;
}

void SimScreenCapture::useCategory()
{
	_useCategory = true;
	initCategoryMap();
}

void SimScreenCapture::initCategoryMap()
{
	_categoryMap[GUI_SCREEN_SYSDONOR] = 	"PROC_SYSDONOR";
	_categoryMap[GUI_SCREEN_SYSDISP] = 		"PROC_SYSDISP";
	_categoryMap[GUI_SCREEN_SYSCLAMP] = 	"PROC_SYSCLAMP";
	_categoryMap[GUI_SCREEN_SYSTEST] = 		"PROC_SYSTEST";
	_categoryMap[GUI_SCREEN_ABORTCNF] = 	"PROC_ABORTCNF";
	_categoryMap[GUI_SCREEN_CLUMP] = 		"PROC_CLUMP";
	
	_categoryMap[GUI_SCREEN_DISCONN] = 		"PROC_DISCONN";
	_categoryMap[GUI_SCREEN_DONCONN] = 		"PROC_DONCONN";
	_categoryMap[GUI_SCREEN_CONFIRM_DON_INFO] = "PROC_DONINFO";
	_categoryMap[GUI_SCREEN_DONINFO] = 		"PROC_DONINFO";
	_categoryMap[GUI_SCREEN_DONVITAL] = 	"PROC_DONVITAL";
	_categoryMap[GUI_SCREEN_POWERUP] = 		"PROC_POWERUP";
	_categoryMap[GUI_SCREEN_RUNPROC] = 		"PROC_RUN";
	_categoryMap[GUI_SCREEN_RUNSUMM1] = 	"PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_RUNSUMM2] = 	"PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_SYSACATT] = 	"PROC_SYSACATT";
	_categoryMap[GUI_SCREEN_TROUBLE] = 		"PROC_ERROR";

	_categoryMap[GUI_SCREEN_PREDICT] = 		"PROC_PREDICT";

	
	_categoryMap[GUI_SCREEN_ALARM] = "ALARM";
	

	_categoryMap[GUI_SCREEN_CONFIG] = "CONFIG_MAIN";
	_categoryMap[GUI_SCREEN_CONFIG_MACHINE] = "CONFIG_MACHINE";
	
	_categoryMap[GUI_SCREEN_CONFIG_LANGUAGE] = "CONFIG_LANGUAGE";
	
	_categoryMap[GUI_SCREEN_CONFIG_PLASMA] = 	"CONFIG_PLS";
	_categoryMap[GUI_SCREEN_PLAS_LISTBOX] = 	"CONFIG_PLS";
	
	_categoryMap[GUI_SCREEN_PLT_LISTBOX] = 		"CONFIG_PLT";
	_categoryMap[GUI_SCREEN_CONFIG_PLATELET] = 	"CONFIG_PLT";
	_categoryMap[GUI_SCREEN_CFG_PLT_AMAP] = 	"CONFIG_PLT";
	
	_categoryMap[GUI_SCREEN_CONFIG_PROCEDURE] = 		"CONFIG_PROCEDURE";
	_categoryMap[GUI_SCREEN_CONFIG_METERED_SOLUTIONS] = "CONFIG_AUTO";
	
	_categoryMap[GUI_SCREEN_CONFIG_RBC] = 				"CONFIG_RBC";
	_categoryMap[GUI_SCREEN_RBC_LISTBOX] = 				"CONFIG_RBC";
	_categoryMap[GUI_SCREEN_CONFIG_DONOR] = "CONFIG_DONOR";
	_categoryMap[GUI_SCREEN_CONFIG_PRIORITY] = "CONFIG_PRIO";
	_categoryMap[GUI_SCREEN_CONFIG_REPLACEMENT_FLUID] = "CONFIG_REPL";
	_categoryMap[GUI_SCREEN_CONFIG_VOLUME_LIMITS] = "CONFIG_VOL";
	
	_categoryMap[GUI_SCREEN_REPFLUID] = "PROC_REPFLUID";
	_categoryMap[GUI_SCREEN_SETID] = "PROC_SETID";

	_categoryMap[GUI_SCREEN_CFR11] = "CFR11";

	_categoryMap[GUI_SCREEN_BARCODE] = "BARCODE";
	
	_categoryMap[GUI_SCREEN_SYSCASSETTE] = "PROC_SYS";
	_categoryMap[GUI_SCREEN_PTF_MOUNT] = "PROC_PTF";
	
	_categoryMap[GUI_SCREEN_PAS_INIT_SPLIT] = "PROC_PAS";
	_categoryMap[GUI_SCREEN_CLAMP_CROSSOVER] = "PROC_CLAMP";
	
	
	_categoryMap[GUI_SCREEN_SET_LISTBOX] = "CONFIG_SET";
	_categoryMap[GUI_SCREEN_LANG_LISTBOX] = "CONFIG_LANGUAGE";
	_categoryMap[GUI_SCREEN_CONFIRM_BOX] = "CONFIG_SET";
	
	_categoryMap[GUI_SCREEN_RAS_CONNECT_CONFIRM_BOX] = "PROC_RAS";
	_categoryMap[GUI_SCREEN_PAS_CONNECT_CONFIRM_BOX] = "PROC_PAS";
	_categoryMap[GUI_SCREEN_SALINE_CONNECT_CONFIRM_BOX] = "PROC_SALINE";
	_categoryMap[GUI_SCREEN_AC_CONNECT_CONFIRM_BOX] = "PROC_AC";
	_categoryMap[GUI_SCREEN_REBOOT_CONFIRM_BOX] = "PROC_REBOOT";
	_categoryMap[GUI_SCREEN_PREDICT_CONFIRM_BOX] = "PROC_PREDICT";
	
	_categoryMap[GUI_SCREEN_RUNSUM_PLT] = 		"PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_RUNSUM_PLASMA] = 	"PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_RUNSUM_RBC] = 		"PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_RUNSUM_RBC_PROD1] = "PROC_RUNSUM";
	_categoryMap[GUI_SCREEN_RUNSUM_RBC_PROD2] = "PROC_RUNSUM";
	
	_categoryMap[GUI_SCREEN_AAS_CLAMP_CHANNEL] = 	"PROC_AAS";
	_categoryMap[GUI_SCREEN_AAS_CHANNEL_TEST] = 	"PROC_AAS";
	_categoryMap[GUI_SCREEN_AAS_CONNECT_PAS] = 		"PROC_AAS";
	_categoryMap[GUI_SCREEN_AAS_CONNECT_RAS] = 		"PROC_AAS";
	_categoryMap[GUI_SCREEN_AAS_PRIME] = 			"PROC_AAS";
	
	_categoryMap[GUI_SCREEN_AAS_PAS_SPLIT] = 		"PROC_AAS";
	_categoryMap[GUI_SCREEN_AAS_ADDITION] = 		"PROC_AAS";
	_categoryMap[GUI_SCREEN_MSS_END] = 				"PROC_MSS";
	_categoryMap[GUI_SCREEN_AAS_RUN_SUMMARY] = 		"PROC_AAS";
	_categoryMap[GUI_SCREEN_DRBC_SPLIT] = 			"PROC_DRBC";
	_categoryMap[GUI_SCREEN_AAS_TRANSITION_DUMMY] = "PROC_AAS";
	_categoryMap[GUI_SCREEN_POWERDOWN] = 			"PROC_POWERDWN";
	_categoryMap[GUI_SCREEN_PAS_EMPTY] = 			"PROC_AAS";

	_categoryMap[GUI_SCREEN_PAS_EMPTY] = 			"PROC_AAS";


	_categoryMap[GUI_SCREEN_RBC_SPLIT_INIT] = 		"PROC_DRBC";
	_categoryMap[GUI_SCREEN_SEAL_AND_DETACH] = 		"PROC_SEAL";
	_categoryMap[GUI_SCREEN_PREDICT_COMP_PLS_CONFIRM_BOX] = "PROC_PREDICT";
	
	
	_categoryMap[GUI_SCREEN_CONFIG_SOFTWARE_OPTIONS] = "CONFIG_SOFT";
	_categoryMap[GUI_SCREEN_KEYBOARD] = "CONFIG_SOFT";
	_categoryMap[GUI_SCREEN_LISTVIEW] = "CONFIG_SOFT";

	_categoryMap[GUI_SCREEN_JP_CLAMP_BLOOD_DIVERSION] = "PROC_CLAMP";
	_categoryMap[GUI_SCREEN_JP_CLAMP_PLASMA_AIR_BAG] = "PROC_CLAMP";

}

char* SimScreenCapture::getCategory(GUI_SCREEN_ID id)
{
	map<GUI_SCREEN_ID, char*>::iterator iter = _categoryMap.find(id);
	if( iter != _categoryMap.end())
		return iter->second;
	return NULL;
}

void SimScreenCapture::setScreenId(GUI_SCREEN_ID id)
{
	if( _useCategory )
	{
		char* tempcategory = getCategory(id);
		_category = tempcategory == NULL ? _category : tempcategory;
	}
}
#endif /* CPU == SIMNT */



