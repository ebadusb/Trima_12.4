/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      fullRoutine.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the full screenshot routine.
 *
 ******************************************************************************/

#ifndef __NEW_ROUTINE_HPP__
#define __NEW_ROUTINE_HPP__

#include <vxworks.h>
#if CPU == SIMNT

#include "simRoutineBase.hpp"

class FullScreenshotRoutine: public SimRoutineBase
{
protected:
    
	// Callback Functions
	virtual void dealWithScreenChange();
	virtual void populateActionQueue();
	virtual void dealWithRunSumProcessed();
	virtual void dealWithProcStatusProcessed();

	static void getRunScreens();
    
public:
	FullScreenshotRoutine();
	virtual ~FullScreenshotRoutine();

	static void initAutoChange();
    static void goFullRoutine();
	static void go();
	static void snapCassetteScreen();
	static void snapAcConnectScreen();
	static void snapDonInfo();
	static void snapDonVitals();
	static void snapPredict();
	static void sysDonorNav();
	static void snapRunScreen(int scrnCount);
	static void configAmapPltOptionPredictEffect();
	static void configCompPlsOptionPredictEffect();
	static void configShowPlasmaPredictEffect();
	static void walkThroughCompPlasmaRunSum();
	
	static void passDonVitals();
	static void passDonorNav();
	static void passDonorInfo();
	static void passPredictAmapPlt();
	static void passPredictCompPlasma();
	static void passPredictShowPlasma();

	static void initConcatStrings();
};

#endif /* if CPU==SIMNT */
#endif // #ifndef __NEW_ROUTINE_HPP__

