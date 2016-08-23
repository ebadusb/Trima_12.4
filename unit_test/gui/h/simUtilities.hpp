/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simUtilities.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This file contains the global functions and general purpose
 *	utility functions for the simulator and screenshot routines.
 *
 *******************************************************************************/


#ifndef __SIM_UTILITIES_HPP__
#define __SIM_UTILITIES_HPP__

#include <vxworks.h>

#if CPU == SIMNT

#include "datalog.h"
#include "states.h"
#include "guiUnitTestCds.h"
#include "gui.h"
#include "simScreenshotCycleNames.hpp"
#include "cfg_ids.h"
#include "config_cds.h"
#include "software_cds.h"
#include "operator_config.h"
#include "simScreenCount.hpp"
#include "runsummary.h"
#include "iguimsgs.h"

#define REPEAT(__NUM_TIMES__) for (int repeatIndex = 0; repeatIndex < __NUM_TIMES__; repeatIndex++)

class FullScreenshotRoutine;

DataLog_Stream & operator << (DataLog_Stream &os, State_names stateName);

class SimUtilities
{
private:

	static bool _cfr11EventsAdded;

	class cds
	{
	private:
		static Config_CDS writeableConfig;

	public:
		static void init();
		static void writeConfig();

		static GuiUnitTest_CDS unitTest;
		static Operator_Config config;
	};

public:

	enum
	{	// Bitfield for procedure type
        PLT = 1,
		PLAS = 2,
		SRBC = 4,
		DRBC = 8,
		RAS = 16,
        PAS = 32
	};

	typedef enum { PRIME, DRAW, RETURN, RINSEBACK } RUNSTATE;

	static void init();

	static const timespec delay;
 
	static CONFIG_DATA_LISTBOX_STRINGS getCurrentLanguage();
    
	static void setSystemState(State_names newSystemState);

	static bool autoNavOn(bool isOn);
	static bool autoNavOn();
	
	static void sendRunStats();

	static void setRunStats(RUN_SUMMARY_STRUCT& runSumStruct);
	static void sendRunsumm1Setup(bool showAasStartTime,
                                  bool showAvgInletFlow,
                                  bool showResidualLoss,
                                  bool plasmaRinsebackOn,
                                  bool salineRinsebackOn);

	static string findAndReplace(const string& source, const string& text, const string& replace);
	static void toggleSplitNotif(bool request);
	static void toggleMasterRas(bool request);
	static void toggleMasterPas(bool request);
	static void toggleDonorVolumeLimit(VOLUME_REMOVAL_TYPE which, bool request);
	static const char *stateIdToStr(State_names state);
	static void setRinseback();
	//static void toggleOption(char * optionName);
	static void changeScreens(GUI_SCREEN_ID newScreen, const char* screenParam="", invocation_method invocMethod=INVOKE_SCREEN);
    static void setAasVolume(int vol);
	static bool checkScreen(GUI_SCREEN_ID correctScreen, bool careAboutFailure=true);
	static void enableLoadSystem();
	static void setRunComplete();

	static void setStatLine(TEXT_STRING_ID txt=TEXT_NULL, STATUS_LINE_TYPE type=ATTENTION_STATUS_LINE);

	static void sendTraverseMsg(TRAVERSE_TYPE type=UNIVERSAL, long param0=0, long param1=0,
								long param2=0, long param3=0, long param4=0, long param5=0,
								long param6=0, long param7=0, long param8=0, long param9=0);

    static bool verifyStartingPoint();
	static int getAasRunType();

	static int receivedSignal();
    static int dispatchMessages();

	static void tbvSafetyReply();
	static void tbvControlReply();

	static void enableLoadBtn();
	static void loadRefCodes();

	static void addCFR11Events();
	static void initCFR11();
	static void walkThroughCFR11Screens();
	static void subWalkThroughCFR11Common();
	static void subWalkThroughCFR11Run();
	static void subWalkThroughCFR11PostRun();
	
    static void walkThroughReplacementFluidAlarm();
	static void walkThroughDrbcSplitScreen();
	static void walkThroughBarcodeScreens();
	static void walkThroughSplashScreen();
	static void walkThroughAbortScreens();
	static void walkThroughPowerDownScreens();
    
	static void cycleThroughScreenTypes(GUI_SCREEN_ID correctScreen, int lastValue, SCREENSHOTINFO screenshotNames[], bool doMappingStuff=true, TRAVERSE_TYPE traverseType = UNIVERSAL, int delaySeconds=1);
	static void cycleThroughConfirmDonInfoDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughDrbcSplitDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughAasRunsumDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughRasConnectDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughPasConnectDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughChannelClampDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughAbortConfirmDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughMssEndDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughRs1DisplayTypes(bool doMappingStuff=true);
	static void cycleThroughRs2DisplayTypes(bool doMappingStuff=true);
	static void cycleThroughPtfLoadTypes(bool doMappingStuff=true);
	static void cycleThroughSysDispTypes(bool doMappingStuff=true);
	static void cycleThroughSysClampDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughPasInitSplitTypes(bool doMappingStuff=true);
    static void cycleThroughJpClampDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughDonConnTypes(bool doMappingStuff=true);
	static void cycleThroughPowerDownDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughSysDonorDisplayTypes(bool doMappingStuff=true);
	static void cycleThroughRunsumLabels(const char * screenshotPrefix);

	static void setScreenshotName(const char* info);
	static void removeScreenshotName();
	static void setScreenshotInfo(const char* name);
	static void removeScreenshotInfo();

	static void showAdjustScreen();

	static void sendProcedureData(RUNSTATE runState, unsigned short runInfo);
	static void sendAdjustData(bool isPltRun);
	
	static void toggleAmapPlatelet();
	static bool isAmapPlateletEnabled();

	class UnitSystem
	{
	public:
		enum UNIT_SYSTEM
		{
			UNITS_ENGLISH,
			UNITS_METRIC
		};

	private:

		static UNIT_SYSTEM _currentUnits;

	public:
		static void init();
		static UNIT_SYSTEM units() { return _currentUnits; };
		static UNIT_SYSTEM units(UNIT_SYSTEM newUnits);
		static void toggle();
	};

	static SimScreenCount screenCounter;
	static SimScreenCount runsumResponseCounter;
	static SimScreenCount procStatusResponseCounter;
};

template<class T>
inline T Max(const T& a, const T& b)
{
	if(a < b)
		return b;
	else
		return a;
}

template<class T>
inline T Min(const T& a, const T& b)
{
	if(a < b)
		return a;
	else
		return b;
}


#endif // #if CPU==SIMNT
#endif // #ifndef __SIM_UTILITIES_HPP__



