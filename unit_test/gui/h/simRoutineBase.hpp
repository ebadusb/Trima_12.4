/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simRoutineBase.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: Here's a base class for the screenshot routine class.  The idea
 *	here is that if we want to make an alternate routine, we can derive from
 *	this class and have all the message callbacks in place.
 *
 *******************************************************************************/

#ifndef __SIM_ROUTINE_BASE_HPP__
#define __SIM_ROUTINE_BASE_HPP__

#include <vxworks.h>

#if CPU == SIMNT
#include <set>

#include "gui.h"
#include "string_id.h"
#include "datalog.h"
#include "trimamessages.h"
#include "states.h"
#include "simMsgSys.hpp"
#include "guiUnitTestCds.h"
#include "runinfo.h"
#include "runsummary.h"
#include "cfg_ids.h"
#include "simAlarms.hpp"
#include "simUtilities.hpp"
#include "simActionQueue.hpp"

#define RoutineBaseLog DataLog (log_level_gui_info) << "RoutineBaseInfo[" << __FILE__ << "(" << __LINE__ << ")] "

class SimRoutineBase
{
protected:
	static SimRoutineBase* routine;

	ActionQueue _actionQueue;

	// The messages it's waiting for
	SimntGuiScreenChange						*_msgPtrGuiScreenChanged;

	ACPrimeCompletedMsg							*_msgPtrAcPrimeCompleted;
	SystemStateChangeMsg						*_msgPtrSystemStateChange;

	AasAdditionVolumeUpdate						*_msgPtrAasVolUpdate;
	Message<CURRENT_PROCEDURE_STATUS_STRUCT> 	*_msgPtrReceivedRunData;
	Message<RUN_SUMMARY_STRUCT> 				*_msgPtrReceivedSummaryData;
	RunCompleteMsg								*_msgPtrRunCompleteMsg;
	SimntGuiProcessedRunsumStruct				*_msgPtrRunSumProcessed;
	SimntGuiProcessedProcStatus					*_msgPtrProcStatus;

	// Utility Functions
    virtual void init();

public:
	virtual ~SimRoutineBase();
	
	//Utility Functions
	virtual void screenshotRoutine(GUI_SCREEN_ID startScreen=GUI_SCREEN_NULL_SCREEN);

	virtual void kickOff();
    
protected:

    static void startRoutineLoop(SimRoutineBase* routinePtr, GUI_SCREEN_ID startScreenId=GUI_SCREEN_SYSDONOR);
	static int routineLoop();

	virtual void populateActionQueue();

	virtual void dealWithSystemStateChange();
	virtual void dealWithRunComplete();
	virtual void dealWithRunData();
	virtual void dealWithSummaryData();
	virtual void dealWithAasAdditionUpdate();
	virtual void dealWithScreenChange();
	virtual void dealWithAcPrimeComplete();
	virtual void dealWithRunSumProcessed();
	virtual void dealWithProcStatusProcessed();

	virtual void kickOffSnap();
	virtual void kickOffNav();
};

#endif /* if CPU==SIMNT */
#endif // #ifndef __SIM_ROUTINE_BASE_HPP__

