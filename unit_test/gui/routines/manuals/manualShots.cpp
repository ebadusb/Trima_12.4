/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      manualShots.cpp
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
#include "statline.hpp"
#include "runsum_product.hpp"

// From unit_test
#include "manualShots.hpp"

#include "simScreenCapture.hpp"
#include "simDelay.hpp"
#include "simScreenTouch.hpp"
#include "scrncomm.hpp"
#include "simActionQueue.hpp"

/*********************************************************************************************
									Constructor
**********************************************************************************************/

ScreenshotsForOpsManual::ScreenshotsForOpsManual(): ScreenshotsForOpsManualNav() {}

/*********************************************************************************************
									Destructor
**********************************************************************************************/

ScreenshotsForOpsManual::~ScreenshotsForOpsManual()
{
	DataLog (log_level_sim_main_routine_error) << "ScreenshotsForOpsManual destructor.  That's bad." << endmsg;
};

void ScreenshotsForOpsManual::go()
{
	startRoutineLoop(new ScreenshotsForOpsManual);
}

void ScreenshotsForOpsManual::populateActionQueue()
{
	_actionQueue.add("Manual Script ",		&getScreens, false							);
}

void ScreenshotsForOpsManual::getScreens()
{
	SimMsgSys msgSys;

	DataLog (log_level_sim_main_routine_info) << "Procedure starting." << endmsg;

	// Turn on the screenshots.
	SimScreenCapture::autoNavOn(true);

	SimUtilities::init();

	if (SimRoutineBase::routine) SimRoutineBase::routine->kickOff();
	else DataLog (log_level_sim_main_routine_info) << "Routine class pointer has not been initialized." << endmsg;
}

unsigned int ScreenshotsForOpsManual::shotOpts()
{
	return SHOT_OPTS_NO_PRUNING | SHOT_OPTS_NO_LABELING;
}

void ScreenshotsForOpsManual::shot(GUI_SCREEN_ID expectedId, RELEASE release, const char *fmt, ...)
{
    char* newFmt = (char*)malloc((strlen(fmt)+strlen(SCREENSHOTPREFIX)+strlen(RELEASE_STRING(release))+3) * sizeof(char));
    // Note: four backslashes in a sprintf translates to two in the string, which will later translate to
	// one in the final sprintf in the screenshot() call.
	sprintf(newFmt, "%s\\\\%s%s", RELEASE_STRING(release), SCREENSHOTPREFIX, fmt);

    va_list args;
	va_start(args, fmt);
	SimScreenCapture::screenshot(expectedId, shotOpts(), newFmt, args);
	va_end(args);

	delete[] newFmt;
}

void ScreenshotsForOpsManual::shot(RELEASE release, const char *fmt, ...)
{
    char* newFmt = (char*)malloc((strlen(fmt)+strlen(SCREENSHOTPREFIX)+strlen(RELEASE_STRING(release))+3) * sizeof(char));
    // Note: four backslashes in a sprintf translates to two in the string, which will later translate to
	// one in the final sprintf in the screenshot() call.
	sprintf(newFmt, "%s\\\\%s%s", RELEASE_STRING(release), SCREENSHOTPREFIX, fmt);

    va_list args;
	va_start(args, fmt);
	SimScreenCapture::screenshot(shotOpts(), newFmt, args);
	va_end(args);

	delete[] newFmt;
}



//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// SCREEN CHANGE FUNCTIONS////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void ScreenshotsForOpsManual::kickOffSnap()
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_SYSDONOR)) return;

	shot(GUI_SCREEN_SYSDONOR, UNITEDSTATES, "TwoButton");
	shot(GUI_SCREEN_SYSDONOR, WORLDWIDE, "TwoButton");
	shot(GUI_SCREEN_SYSDONOR, JAPAN, "TwoButton");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSDISP)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_SYSDISP)) return;

    SimUtilities::sendTraverseMsg(UNIVERSAL, SYSDISP_CASSETTE_EVACUATING);
	shot(GUI_SCREEN_SYSDISP, UNITEDSTATES, "AirEvac");
	shot(GUI_SCREEN_SYSDISP, WORLDWIDE, "AirEvac");
	shot(GUI_SCREEN_SYSDISP, JAPAN, "AirEvac");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_DISCONN)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_DISCONN)) return;

	shot(GUI_SCREEN_DISCONN, UNITEDSTATES, "Disconn");
	shot(GUI_SCREEN_DISCONN, WORLDWIDE, "Disconn");
	shot(GUI_SCREEN_DISCONN, JAPAN, "Disconn");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_DONINFO)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_DONINFO)) return;

	shot(GUI_SCREEN_DONINFO, UNITEDSTATES, "DonInfo");
	shot(GUI_SCREEN_DONINFO, WORLDWIDE, "DonInfo");
	shot(GUI_SCREEN_DONINFO, JAPAN, "DonInfo");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_DONVITAL)
{
    SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_DONVITAL)) return;

	shot(GUI_SCREEN_DONVITAL, UNITEDSTATES, "DonVitals");
	shot(GUI_SCREEN_DONVITAL, WORLDWIDE, "DonVitals");
	shot(GUI_SCREEN_DONVITAL, JAPAN, "DonVitals");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_RUNPROC)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_RUNPROC)) return;

	if (SimUtilities::screenCounter.getCount(GUI_SCREEN_RUNPROC) == 1)
	{
		SimUtilities::sendProcedureData(SimUtilities::DRAW, SimUtilities::PLT | SimUtilities::PLAS );
		shot(GUI_SCREEN_RUNPROC, UNITEDSTATES, "Run");
		shot(GUI_SCREEN_RUNPROC, WORLDWIDE, "Run");
		shot(GUI_SCREEN_RUNPROC, JAPAN, "Run");
	}
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSACATT)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_SYSACATT)) return;

	SimUtilities::sendTraverseMsg(SET_SHOW_AC_ATTACH_CONFIRM, 0);

	shot(GUI_SCREEN_SYSACATT, UNITEDSTATES, "AcConnect");
	shot(GUI_SCREEN_SYSACATT, WORLDWIDE, "AcConnect");

	SimUtilities::sendTraverseMsg(SET_SHOW_AC_ATTACH_CONFIRM, 1);

	shot(GUI_SCREEN_SYSACATT, JAPAN, "AcConnect");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_PREDICT)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_PREDICT)) return;

	SimDelay::seconds(5); // Predict needs a good long time to chew on this.

	REPEAT(2) SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);

	shot(GUI_SCREEN_PREDICT, WORLDWIDE, "Predict");

    REPEAT(3) SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_NEXT, GUI_SCREEN_PREDICT);

	shot(GUI_SCREEN_PREDICT, UNITEDSTATES, "Predict");

	REPEAT(3) SimScreenTouch::touch(GUI_BUTTON_PRED_SELECT_PREVIOUS, GUI_SCREEN_PREDICT);

	SimUtilities::sendTraverseMsg(SET_SHOW_FLOW_ADJUST, true);
	SimUtilities::sendTraverseMsg(SET_SHOW_PLT_VOLUME, true);

	shot(GUI_SCREEN_PREDICT, JAPAN, "Predict");

	SimUtilities::sendTraverseMsg(SET_SHOW_FLOW_ADJUST, false);
	SimUtilities::sendTraverseMsg(SET_SHOW_PLT_VOLUME, false);
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_SYSCASSETTE)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_SYSCASSETTE)) return;

	shot(GUI_SCREEN_SYSCASSETTE, UNITEDSTATES, "RefSelect");
	shot(GUI_SCREEN_SYSCASSETTE, WORLDWIDE, "RefSelect");
	shot(GUI_SCREEN_SYSCASSETTE, JAPAN, "RefSelect");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_AAS_CONNECT_PAS)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_AAS_CONNECT_PAS)) return;

	shot(GUI_SCREEN_AAS_CONNECT_PAS, WORLDWIDE, "ConnectPas");
	shot(GUI_SCREEN_AAS_CONNECT_PAS, JAPAN, "ConnectPas");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_AAS_CONNECT_RAS)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_AAS_CONNECT_RAS)) return;

	shot(GUI_SCREEN_AAS_CONNECT_RAS, UNITEDSTATES, "ConnectRas");
	shot(GUI_SCREEN_AAS_CONNECT_RAS, WORLDWIDE, "ConnectRas");
	shot(GUI_SCREEN_AAS_CONNECT_RAS, JAPAN, "ConnectRas");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_MSS_END)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_MSS_END)) return;

	SimUtilities::sendTraverseMsg(UNIVERSAL,MSS_END_WHITE_BOTH_REMOVE);

	shot(GUI_SCREEN_MSS_END, UNITEDSTATES, "RemoveSet");
	shot(GUI_SCREEN_MSS_END, WORLDWIDE, "RemoveSet");
	shot(GUI_SCREEN_MSS_END, JAPAN, "RemoveSet");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG)
{
	SimScreenTouch::keypadSequence("789852e");

	shot(GUI_SCREEN_CONFIG, UNITEDSTATES,	"CfgMain");
	shot(GUI_SCREEN_CONFIG, WORLDWIDE,		"CfgMain");
	shot(GUI_SCREEN_CONFIG, JAPAN,			"CfgMain");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_MACHINE)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_MACHINE)) return;

	shot(GUI_SCREEN_CONFIG_MACHINE, UNITEDSTATES,	"CfgMachine");
	shot(GUI_SCREEN_CONFIG_MACHINE, WORLDWIDE,		"CfgMachine");
	shot(GUI_SCREEN_CONFIG_MACHINE, JAPAN,			"CfgMachine");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_LANGUAGE)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_LANGUAGE)) return;

	shot(GUI_SCREEN_CONFIG_LANGUAGE, UNITEDSTATES,	"CfgLang");
	shot(GUI_SCREEN_CONFIG_LANGUAGE, WORLDWIDE,		"CfgLang");
	shot(GUI_SCREEN_CONFIG_LANGUAGE, JAPAN,			"CfgLang");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PLASMA)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_PLASMA)) return;

	shot(GUI_SCREEN_CONFIG_PLASMA, UNITEDSTATES,	"CfgPlasma");
	shot(GUI_SCREEN_CONFIG_PLASMA, WORLDWIDE,		"CfgPlasma");
	shot(GUI_SCREEN_CONFIG_PLASMA, JAPAN,			"CfgPlasma");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PLATELET)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_PLATELET)) return;

	SimUtilities::autoNavOn(false); // Turn off the auto nav, because we'll need to switch between this screen and the autoproc screen.
	SimUtilities::changeScreens(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "--Config Platelets to Config Automated Processes--");
	SimUtilities::toggleMasterPas(false);
	SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PLATELET, "--Config Automated Processes to Config Platelets--");

	shot(GUI_SCREEN_CONFIG_PLATELET, WORLDWIDE,		"CfgPlt");

	SimUtilities::changeScreens(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, "--Config Platelets to Config Automated Processes--");
	SimUtilities::toggleMasterPas(true);
	SimUtilities::changeScreens(GUI_SCREEN_CONFIG_PLATELET, "--Config Automated Processes to Config Platelets--");

	REPEAT(3) SimScreenTouch::touch(GUI_BUTTON_CONFIG_LISTBOX_DOWN_BUTTON, GUI_SCREEN_CONFIG_PLATELET);

	shot(GUI_SCREEN_CONFIG_PLATELET, UNITEDSTATES,	"CfgPlt");
    shot(GUI_SCREEN_CONFIG_PLATELET, JAPAN,			"CfgPlt");

	SimUtilities::autoNavOn(true);
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PROCEDURE)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_PROCEDURE)) return;

	shot(GUI_SCREEN_CONFIG_PROCEDURE, UNITEDSTATES,	"CfgProcedure");
	shot(GUI_SCREEN_CONFIG_PROCEDURE, WORLDWIDE,	"CfgProcedure");
	shot(GUI_SCREEN_CONFIG_PROCEDURE, JAPAN,		"CfgProcedure");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_RBC)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_RBC)) return;

	shot(GUI_SCREEN_CONFIG_RBC, UNITEDSTATES,	"CfgRbc");
	shot(GUI_SCREEN_CONFIG_RBC, WORLDWIDE,		"CfgRbc");
	shot(GUI_SCREEN_CONFIG_RBC, JAPAN,			"CfgRbc");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_DONOR)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_DONOR)) return;

	shot(GUI_SCREEN_CONFIG_DONOR, UNITEDSTATES,	"CfgDonor");
	shot(GUI_SCREEN_CONFIG_DONOR, WORLDWIDE,	"CfgDonor");
	shot(GUI_SCREEN_CONFIG_DONOR, JAPAN,		"CfgDonor");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_PRIORITY)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_PRIORITY)) return;

	shot(GUI_SCREEN_CONFIG_PRIORITY, UNITEDSTATES,	"CfgPriority");
	shot(GUI_SCREEN_CONFIG_PRIORITY, WORLDWIDE,		"CfgPriority");
	shot(GUI_SCREEN_CONFIG_PRIORITY, JAPAN,			"CfgPriority");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID)) return;

	shot(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, UNITEDSTATES,	"CfgSaline");
	shot(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, WORLDWIDE,	"CfgSaline");
	shot(GUI_SCREEN_CONFIG_REPLACEMENT_FLUID, JAPAN,		"CfgSaline");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_VOLUME_LIMITS)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_VOLUME_LIMITS)) return;

	SimUtilities::sendTraverseMsg(UNIVERSAL, 0, 0);
	shot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, UNITEDSTATES,	"CfgDonVolLimits");

    SimUtilities::sendTraverseMsg(UNIVERSAL, 0, 1);
	shot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, WORLDWIDE,	"CfgDonVolLimits");

	SimUtilities::sendTraverseMsg(UNIVERSAL, 1, 1);
	shot(GUI_SCREEN_CONFIG_VOLUME_LIMITS, JAPAN,		"CfgDonVolLimits");

	SimUtilities::sendTraverseMsg(UNIVERSAL, 0, 0);
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_CONFIG_METERED_SOLUTIONS)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_CONFIG_METERED_SOLUTIONS)) return;

	SimUtilities::toggleMasterPas(false);

	shot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, UNITEDSTATES,	"CfgAutoProc");

	SimUtilities::toggleMasterPas(true);

	shot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, WORLDWIDE,	"CfgAutoProc");
	shot(GUI_SCREEN_CONFIG_METERED_SOLUTIONS, JAPAN,		"CfgAutoProc");
}

void ScreenshotsForOpsManual::SCREEN_CHANGE_SNAP(GUI_SCREEN_TROUBLE)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_TROUBLE)) return;

	SimUtilities::sendAdjustData(true);
	SimScreenCapture::screenshot(GUI_SCREEN_TROUBLE, "Adjust");
}


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// RUN SUMMARY FUNCTIONS /////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void ScreenshotsForOpsManual::RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUMM1)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_RUNSUMM1)) return;

	SimUtilities::sendRunsumm1Setup(true,	/* AAS Start Time ON		*/
									false,	/* Avg inlet flow OFF		*/
									true,	/* Show residual loss ON	*/
									false,	/* Plasma rinseback OFF		*/
									false);	/* Saline rinseback OFF		*/

	shot(GUI_SCREEN_RUNSUMM1, UNITEDSTATES, "RunSum1");
	shot(GUI_SCREEN_RUNSUMM1, WORLDWIDE, "RunSum1");

	SimUtilities::sendRunsumm1Setup(true,	/* AAS Start Time ON		*/
									true,	/* Avg inlet flow ON		*/
									false,	/* Show residual loss OFF	*/
									false,	/* Plasma rinseback OFF		*/
									false);	/* Saline rinseback OFF		*/

	shot(GUI_SCREEN_RUNSUMM1, JAPAN, "RunSum1");
}

void ScreenshotsForOpsManual::RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUMM2)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_RUNSUMM2)) return;

	shot(GUI_SCREEN_RUNSUMM2, UNITEDSTATES, "Runsum2");
	shot(GUI_SCREEN_RUNSUMM2, WORLDWIDE, "Runsum2");
	shot(GUI_SCREEN_RUNSUMM2, JAPAN, "Runsum2");
}

void ScreenshotsForOpsManual::RUNSUMM_STATS_SNAP(GUI_SCREEN_RUNSUM_PLT)
{
	SimMsgSys msgSys; // In case we want to use this from the command line.
	if (!SimUtilities::checkScreen(GUI_SCREEN_RUNSUM_PLT)) return;

	SimUtilities::sendTraverseMsg(CHANGE_LABELS, DONT_LABEL_PRODUCT);
	SimUtilities::sendRunStats();

	SimScreenTouch::touch(GUI_BUTTON_PRODVERIF_1, GUI_SCREEN_RUNSUM_PLT);

	shot(GUI_SCREEN_RUNSUM_PLT, UNITEDSTATES, "RunSumPlt");
	shot(GUI_SCREEN_RUNSUM_PLT, WORLDWIDE, "RunSumPlt");
	shot(GUI_SCREEN_RUNSUM_PLT, JAPAN, "RunSumPlt");
}

#endif /* CPU == SIMNT */


