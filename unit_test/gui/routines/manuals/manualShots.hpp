/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      manualShots.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the full screenshot routine.
 *
 ******************************************************************************/

#ifndef __MANUAL_SHOTS_HPP__
#define __MANUAL_SHOTS_HPP__

#include "manualShots.hpp"
#include "manualShotsNav.hpp"

#include "screenIdMacros.hpp"

#define US_DIR "us"
#define WW_DIR "ww"
#define JP_DIR "jp"

#define RELEASE_STRING(__RELEASE_CODE__) (__RELEASE_CODE__==JAPAN? JP_DIR : (__RELEASE_CODE__==WORLDWIDE? WW_DIR : US_DIR))

class ScreenshotsForOpsManual: public ScreenshotsForOpsManualNav
{
protected:

	typedef enum
	{
		UNITEDSTATES,
		WORLDWIDE,
		JAPAN
	} RELEASE;

	virtual void populateActionQueue();
	static void getScreens();

	static unsigned int shotOpts();
    static void shot(GUI_SCREEN_ID expectedId, RELEASE release, const char *fmt, ...);
	static void shot(RELEASE release, const char *fmt, ...);

	virtual void kickOffSnap();

	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSDISP);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_DISCONN);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_DONINFO);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_DONVITAL);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_RUNPROC);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSACATT);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_TROUBLE);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_PREDICT);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSCASSETTE);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_AAS_CONNECT_PAS);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_AAS_CONNECT_RAS);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_MSS_END);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_MACHINE);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_LANGUAGE);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PLASMA);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PLATELET);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PROCEDURE);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_RBC);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_DONOR);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PRIORITY);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_VOLUME_LIMITS);
	virtual void SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_METERED_SOLUTIONS);

	virtual void RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUMM1);
	virtual void RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUMM2);
	virtual void RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUM_PLT);

public:
	ScreenshotsForOpsManual();
	virtual ~ScreenshotsForOpsManual();
   
	static void go();
};

#endif // #ifndef __MANUAL_SHOTS_HPP__

