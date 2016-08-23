/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      baseShots.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the full screenshot routine.
 *
 ******************************************************************************/

#ifndef __BASE_SHOTS_HPP__
#define __BASE_SHOTS_HPP__

#include "simRoutineBase.hpp"
#include "screenIdMacros.hpp"

// This macro defines an error message that shows when a function

class BaseShots: public SimRoutineBase
{
protected:

	static GUI_SCREEN_ID currentScreen();

    // Callback Functions
    virtual void dealWithScreenChange();
	virtual void dealWithRunSumProcessed();
	virtual void populateActionQueue();

	#define __FUNC_PROTOTYPES__
	#include "screenIdMacros.hpp"
	#undef __FUNC_PROTOTYPES__
    
public:
	BaseShots();
	virtual ~BaseShots();
};

#endif // #ifndef __BASE_SHOTS_HPP__

