/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simActionQueue.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class provides a queue structure for actions that will
 *	be performed throughout a routine.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simActionQueue.hpp $
 * Revision 1.1  2009/06/11 20:57:51Z  dslausb
 * Initial revision
 *
 ******************************************************************************/

#ifndef __SIM_ACTION_QUEUE_HPP__
#define __SIM_ACTION_QUEUE_HPP__

#include <queue>
#include <string>
using namespace std;

class ActionQueue; // Forward declaration

// typedef void (ScreenshotGrabber::*ScreenshotGrabberFunction)();
//#define CALL_MEMBER_FN_BY_POINTER(ptrToObject,ptrToMember)  ((ptrToObject)->*(ptrToMember))
//#define CALL_MEMBER_FN(object, ptrToMember) ((object).*(ptrToMember))
typedef void ( *functPtr )();

class ActionQueueFunctor
{
private:
   string _description;
	ActionQueueFunctor() {};
	functPtr _functionPointer;
	bool _autoAdvance;
	ActionQueue* _ownerQueue;

public:
	ActionQueueFunctor(string description, functPtr functionPointer, ActionQueue* ownerQueue, bool autoAdvance = true);
	void operator ()();
};


class ActionQueue: public queue<ActionQueueFunctor>
{
public:
	void add(string description,
             functPtr functionPointer,
             bool autoAdvance = true);

    void nextRoutine();
};

#endif // #ifndef __SIM_ACTION_QUEUE_HPP__


