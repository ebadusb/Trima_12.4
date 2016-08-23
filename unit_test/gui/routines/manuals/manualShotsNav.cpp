/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      manualShotsNav.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 ******************************************************************************/

#include <vxworks.h>
#if CPU == SIMNT

// From common
#include "trima_datalog.h"

// From GUI
#include "gui.h"
#include "guiglobs.hpp"
#include "sealAndDetach.hpp"

// From unit_test
#include "manualShotsNav.hpp"

#include "simScreenCapture.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "scrncomm.hpp"
#include "simActionQueue.hpp"

/*********************************************************************************************
									Constructor
**********************************************************************************************/

ScreenshotsForOpsManualNav::ScreenshotsForOpsManualNav(): BaseShots() {}

/*********************************************************************************************
									Destructor
**********************************************************************************************/

ScreenshotsForOpsManualNav::~ScreenshotsForOpsManualNav()
{
	DataLog (log_level_sim_main_routine_error) << "ScreenshotsForOpsManualNav destructor.  That's bad." << endmsg;
};

/*********************************************************************************************
									Nav Functions
**********************************************************************************************/

void ScreenshotsForOpsManualNav::kickOffNav()
{
	SimUtilities::changeScreens(GUI_SCREEN_SYSCASSETTE, "--Two-Button to Kit Selection --");
}


void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_SYSDONOR)
{
	switch (SimUtilities::screenCounter.getCount(GUI_SCREEN_SYSDONOR))
	{
	case 1:
		SimUtilities::changeScreens(GUI_SCREEN_DONVITAL, "--Two-Button to DonVitals A --");
		break;
        
	case 2:
		SimScreenTouch::touch(GUI_BUTTON_LOWLEFT, GUI_SCREEN_SYSDONOR);
		break;

	default:
		DataLog (log_level_sim_main_routine_error) << "Unhandled screen count." << endmsg;
		break;
	}
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_SYSDISP)
{ SimUtilities::changeScreens(GUI_SCREEN_SYSACATT, "--SysDisp to AC Attach--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_DISCONN)
{
	SimUtilities::setSystemState(METERED_STORAGE);
	SimUtilities::changeScreens(GUI_SCREEN_AAS_CONNECT_RAS, "--Disconnect to Connect RAS--");
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_DONINFO)
{
	SDonorVitals donVitals;
	guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&donVitals);

	if (donVitals.fDonorHct < 25.0 || donVitals.fDonorHct > 52.0)
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_HEMATOCRIT, GUI_SCREEN_DONINFO);

		if (guiglobs::cdsData.config.LangUnit.Get().key_crit_or_glob == RBC_UNIT_HEMOGLOBIN)
			SimScreenTouch::keypadSequence("150e");
		else SimScreenTouch::keypadSequence("39e");
	}
	else DataLog (log_level_sim_main_routine_info) << "Valid crit of " << donVitals.fDonorHct << " accepted." << endmsg;

	if (donVitals.fDonorPltPrecount <= 200 || donVitals.fDonorPltPrecount > 400)
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_PLATELET_PRECOUNT, GUI_SCREEN_DONINFO);
		SimScreenTouch::keypadSequence("278e");
    }
	else DataLog (log_level_sim_main_routine_info) << "Valid precount of " << donVitals.fDonorPltPrecount << " accepted." << endmsg;

	SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONINFO);
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_DONVITAL)
{
	SDonorVitals donVitals;
	guiglobs::gui_donor_data_repository.get_donor_vitals_struct(&donVitals);

	if (!(donVitals.cDonorSex == DONOR_SEX_IS_MALE ||
		  donVitals.cDonorSex == DONOR_SEX_IS_FEMALE))
		SimScreenTouch::touch(GUI_BUTTON_DONOR_GENDER, GUI_SCREEN_DONVITAL);

	if ( donVitals.cDonorHeightUnits == HEIGHT_UNITS_IN &&
		 (donVitals.fDonorHeight < MIN_ALLOWABLE_HEIGHT_IN ||
		  donVitals.fDonorHeight < MAX_ALLOWABLE_HEIGHT_IN))
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_HEIGHT, GUI_SCREEN_DONVITAL);
		SimScreenTouch::keypadSequence("6e");
	}

	if (donVitals.cDonorHeightUnits == HEIGHT_UNITS_CM &&
		 (donVitals.fDonorHeight < MIN_ALLOWABLE_HEIGHT_CM ||
		  donVitals.fDonorHeight < MAX_ALLOWABLE_HEIGHT_CM))
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_HEIGHT, GUI_SCREEN_DONVITAL);
		SimScreenTouch::keypadSequence("180e");
	}

	if (donVitals.cDonorWeightUnits == WEIGHT_UNITS_LB &&
		 (donVitals.fDonorWeight < MIN_ALLOWABLE_WEIGHT_LB ||
		  donVitals.fDonorWeight < MAX_ALLOWABLE_WEIGHT_LB))
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_WEIGHT, GUI_SCREEN_DONVITAL);
		SimScreenTouch::keypadSequence("250e");
	}

	if (donVitals.cDonorWeightUnits == WEIGHT_UNITS_KG &&
		 (donVitals.fDonorWeight < MIN_ALLOWABLE_WEIGHT_KG ||
		  donVitals.fDonorWeight < MAX_ALLOWABLE_WEIGHT_KG))
	{
		SimScreenTouch::touch(GUI_BUTTON_DONOR_WEIGHT, GUI_SCREEN_DONVITAL);
		SimScreenTouch::keypadSequence("120e");
	}

	SimScreenTouch::touch(GUI_BUTTON_CONTINUE, GUI_SCREEN_DONVITAL);
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_SYSACATT)
{ SimUtilities::setSystemState(AC_PRIME); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_TROUBLE)
{
	RemoveAllStatelessScreensMsg removeAllStatelessMsg(MessageBase::SEND_LOCAL);
	removeAllStatelessMsg.send();
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_PREDICT)
{ SimUtilities::setSystemState(BLOOD_RUN); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_RUNPROC)
{
	switch (SimUtilities::screenCounter.getCount(GUI_SCREEN_RUNPROC))
	{
	case 1:
		SimUtilities::showAdjustScreen();
		break;

	default:
		SimUtilities::setSystemState(DONOR_DISCONNECT);
		break;
	}
}

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_MACHINE, "--Config Main to Config Machine--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_MACHINE)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_LANGUAGE, "--Config Machine to Config Lang--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_LANGUAGE)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PLASMA, "--Lang Config to Config Plasma--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PLASMA)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PROCEDURE, "--Config Plasma to Config Procedure--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PROCEDURE)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_DONOR, "--Config Procedure to Config Donor--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_DONOR)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, "--Config Donor to Config Replacement Fluid--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_VOLUME_LIMITS, "--Config Replacement Fluid to Config Volume Limits--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_VOLUME_LIMITS)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PRIORITY, "--Config Volume Limits to Config Procedure Priority--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PRIORITY)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "--Config Procedure Priority to Config Automated Processes--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_METERED_SOLUTIONS)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_RBC, "--Config Automated Processes to Config RBC--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_RBC)
{ SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PLATELET, "--Config RBC to Config Platelets--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_CONFIG_PLATELET)
{ DataLog (log_level_sim_main_routine_error) << "You're done for now.  Nice work." << endmsg; }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_SYSCASSETTE)
{ SimUtilities::changeScreens(GUI_SCREEN_SYSDISP, "--RBC Setup to SysDisp--"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_CONNECT_PAS)
{ SimUtilities::setSystemState(METERED_STORAGE_DISCONNECT); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_CONNECT_RAS)
{ SimUtilities::changeScreens(GUI_SCREEN_AAS_CONNECT_PAS, "--connectRas to connectPas --"); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_MSS_END)
{ SimUtilities::setSystemState(POST_RUN); }

void ScreenshotsForOpsManualNav::SCREEN_CHANGE_NAV(GUI_SCREEN_AAS_RUN_SUMMARY)
{
	if (guiglobs::screen_status.get_current_system_state() == METERED_STORAGE_DISCONNECT)
		SimUtilities::changeScreens(GUI_SCREEN_MSS_END, "--aasRunSummary to mssSealProducts --");

	if (guiglobs::screen_status.get_current_system_state() == POST_RUN)
		SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM1, "--aasRunSummary to runSumm1 --");
}

void ScreenshotsForOpsManualNav::RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUMM1)
{ SimUtilities::changeScreens(GUI_SCREEN_RUNSUM_PLT, "--runSumm1 to runsumPlt --"); }

void ScreenshotsForOpsManualNav::RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUMM2)
{ SimUtilities::setSystemState(STARTUP_TESTS); }

void ScreenshotsForOpsManualNav::RUNSUMM_STATS_NAV(GUI_SCREEN_RUNSUM_PLT)
{ SimUtilities::changeScreens(GUI_SCREEN_RUNSUMM2, "--runsumRbc2 to runsum2 --");}

#endif /* CPU == SIMNT */


