/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simActionQueue.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See the corresponding header file for details.
 *
 ******************************************************************************/
 
#include "trima_datalog.h"
#include "simActionQueue.hpp"

ActionQueueFunctor::ActionQueueFunctor(string description,
									   functPtr functionPointer,
									   ActionQueue* ownerQueue,
									   bool autoAdvance = true):
	_functionPointer(functionPointer),
	_autoAdvance(autoAdvance),
	_description(description),
	_ownerQueue(ownerQueue)
	{}

void ActionQueueFunctor::operator ()()
{
   DataLog (log_level_sim_info) << "Calling functor: \"" << _description << "\"" << endmsg;

   _functionPointer();

   DataLog (log_level_sim_info) << "Functor \"" << _description << "\" complete" << endmsg;

   if(_autoAdvance && _ownerQueue) _ownerQueue->nextRoutine();
   else DataLog (log_level_sim_debug) << "Auto advance disabled for this routine." << endmsg;
}

void ActionQueue::nextRoutine()
{

	if(empty()) DataLog (log_level_sim_info) << "The queue is empty." << endmsg;
	else
	{
		DataLog (log_level_sim_info) << "Calling the next routine." << endmsg;
		ActionQueueFunctor next = front();
		pop();
		next();
	}
}

void ActionQueue::add(string description,
                      functPtr functionPointer,
                      bool autoAdvance = true)
{
	push(ActionQueueFunctor(description, functionPointer, this, autoAdvance));
}

