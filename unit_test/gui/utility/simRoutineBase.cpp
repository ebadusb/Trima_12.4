/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simRoutineBase.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 ******************************************************************************/


#include <vxworks.h>
#if CPU == SIMNT

#include "simRoutineBase.hpp"
#include "simScreenCapture.hpp"
#include "guiglobs.hpp"
#include "simDelay.hpp"
#include "statline.hpp"

SimRoutineBase* SimRoutineBase::routine = NULL;

class ScreenshotGrabber;

/*********************************************************************************************
									Destructor
**********************************************************************************************/

SimRoutineBase::~SimRoutineBase()
{
	DataLog (log_level_sim_main_routine_error) << "SimRoutineBase destructor.  That's bad." << endmsg;
};

/*********************************************************************************************
									Utility Functions
**********************************************************************************************/

#define NEW_MSG_OBJ( __MSG_TYPE__, __CALLBACK_FUNCTION__ ) \
	new __MSG_TYPE__ (Callback<SimRoutineBase> ( this, &SimRoutineBase::##__CALLBACK_FUNCTION__	))

void SimRoutineBase::init()
{
	// Intialize message receivers
	_msgPtrGuiScreenChanged =		NEW_MSG_OBJ(SimntGuiScreenChange,						dealWithScreenChange		);
	_msgPtrAcPrimeCompleted =		NEW_MSG_OBJ(ACPrimeCompletedMsg,						dealWithAcPrimeComplete		);
	_msgPtrSystemStateChange = 		NEW_MSG_OBJ(SystemStateChangeMsg,						dealWithSystemStateChange	);
	_msgPtrAasVolUpdate = 			NEW_MSG_OBJ(AasAdditionVolumeUpdate,					dealWithAasAdditionUpdate	);
	_msgPtrReceivedRunData =		NEW_MSG_OBJ(Message<CURRENT_PROCEDURE_STATUS_STRUCT>,	dealWithRunData				);
	_msgPtrReceivedSummaryData =	NEW_MSG_OBJ(Message<RUN_SUMMARY_STRUCT>,				dealWithSummaryData			);
	_msgPtrRunCompleteMsg = 		NEW_MSG_OBJ(RunCompleteMsg,								dealWithRunComplete			);
	_msgPtrRunSumProcessed = 		NEW_MSG_OBJ(SimntGuiProcessedRunsumStruct,				dealWithRunSumProcessed		);
	_msgPtrProcStatus = 			NEW_MSG_OBJ(SimntGuiProcessedProcStatus,				dealWithProcStatusProcessed	);
}


void SimRoutineBase::kickOff()
{
	kickOffSnap();
	kickOffNav();
}

void SimRoutineBase::kickOffSnap()
{
	DataLog (log_level_sim_main_routine_info) << "kickOffSnap() not implemented at this level." << endmsg;
}

void SimRoutineBase::kickOffNav()
{
	DataLog (log_level_sim_main_routine_info) << "kickOffNav() not implemented at this level." << endmsg;
}

void SimRoutineBase::screenshotRoutine(GUI_SCREEN_ID startScreen=GUI_SCREEN_NULL_SCREEN)
{
	if ((startScreen != GUI_SCREEN_NULL_SCREEN) && (!SimUtilities::checkScreen(startScreen)))
	{
		DataLog (log_level_sim_main_routine_error) << "Incorrect start screen." << endmsg;
		return;
	}

	populateActionQueue();
	
	//Start with one unit system.

	DataLog (log_level_sim_main_routine_info) << "Call first routine." << endmsg;
	
	_actionQueue.nextRoutine();

	DataLog (log_level_sim_main_routine_info) << "Leaving screenshotRoutine() after starting chain reaction." << endmsg;
}

void SimRoutineBase::startRoutineLoop(SimRoutineBase* routinePtr, GUI_SCREEN_ID startScreenId=GUI_SCREEN_SYSDONOR)
{
	if (!routinePtr)
	{
		DataLog (log_level_sim_main_routine_error) << "Starting screenshot routine." << endmsg;
		return;
	}

	routine = routinePtr;

	taskSpawn ("screenshotRoutine", 100, 0,
               100000, (FUNCPTR) SimRoutineBase::routineLoop,
               0,0,0,0,0,0,0,0,0,0);

    SimDelay::seconds(2);

	DataLog (log_level_sim_main_routine_debug) << "Starting screenshot routine." << endmsg;
	routine->screenshotRoutine(startScreenId);
    
	DataLog (log_level_sim_main_routine_debug) << "Message responder loop is underway." << endmsg;
}

int SimRoutineBase::routineLoop()
{
	SimMsgSys msgSys;

	// Instantiation must be dynamic so that object doesn't go out of scope right away.
	// This is a leak, but should only be called once.
	if (!routine)
	{
		DataLog (log_level_sim_main_routine_error) << "NULL routine pointer." << endmsg;
		return -1;
	}

	routine->init();

	DataLog (log_level_sim_main_routine_debug) << "Starting message system loop." << endmsg;
    while (true) if (!msgSys.dispatchMessages()) nanosleep(&SimUtilities::delay,0);
	DataLog (log_level_sim_main_routine_error) << "Leaving message system loop. (That's bad.)" << endmsg;

	return 0;
}

// This macro defines an error message that shows when a function
// is called without being defined in the subclass.
#define BASE_ROUTINE(__ROUTINE_NAME__)						\
void SimRoutineBase::##__ROUTINE_NAME__##()					\
{															\
	DataLog (log_level_sim_main_routine_debug)				\
		<< #__ROUTINE_NAME__ <<  "() called from base."		\
		<< endmsg;											\
}

BASE_ROUTINE( dealWithSystemStateChange		);
BASE_ROUTINE( dealWithRunComplete			);
BASE_ROUTINE( dealWithRunData				);
BASE_ROUTINE( dealWithSummaryData			);
BASE_ROUTINE( dealWithAasAdditionUpdate		);
BASE_ROUTINE( dealWithScreenChange			);
BASE_ROUTINE( dealWithAcPrimeComplete		);
BASE_ROUTINE( populateActionQueue			);
BASE_ROUTINE( dealWithRunSumProcessed		);
BASE_ROUTINE( dealWithProcStatusProcessed	);

#undef BASE_ROUTINE

#endif /* CPU == SIMNT */


