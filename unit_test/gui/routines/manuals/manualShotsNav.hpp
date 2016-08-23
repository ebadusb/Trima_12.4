/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      manualShotsNav.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the full screenshot routine.
 *
 ******************************************************************************/

#ifndef __MANUAL_SHOTS_NAV_HPP__
#define __MANUAL_SHOTS_NAV_HPP__

#include "baseShots.hpp"
#include "screenIdMacros.hpp"

// This macro defines an error message that shows when a function

class ScreenshotsForOpsManualNav: public BaseShots
{
protected:

	virtual void kickOffNav();

	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_SYSDONOR);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_SYSDISP);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_DISCONN);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_DONINFO);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_DONVITAL);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_SYSACATT);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_TROUBLE);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_PREDICT);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_RUNPROC);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_MACHINE);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_LANGUAGE);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PLASMA);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PLATELET);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PROCEDURE);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_RBC);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_DONOR);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PRIORITY);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_VOLUME_LIMITS);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_METERED_SOLUTIONS);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_SYSCASSETTE);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_CONNECT_PAS);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_CONNECT_RAS);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_MSS_END);
	virtual void SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_RUN_SUMMARY);

	virtual void RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUMM1);
	virtual void RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUMM2);
	virtual void RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUM_PLT);
    
public:
	ScreenshotsForOpsManualNav();
	virtual ~ScreenshotsForOpsManualNav();
};

#endif // #ifndef __MANUAL_SHOTS_NAV_HPP__

