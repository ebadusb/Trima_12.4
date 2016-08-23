/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simMsgSys.cpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: See description in corresponding header file.
 *
 *
 ******************************************************************************/

#include "simMsgSys.hpp"
#include "trima_datalog.h"
#include "simUtilities.hpp"

SimMsgSys::SimMsgSys()
{
	if (MessageSystem::exists())
		_msgSystem = NULL;
	else
	{
		_msgSystem = new MessageSystem();
		_msgSystem->initBlocking();
	}
}

SimMsgSys::~SimMsgSys()
{
	delete _msgSystem;
	_msgSystem = NULL;
}


int SimMsgSys::receivedSignal()
{
	if (_msgSystem) return _msgSystem->dispatcher().receivedSignal();
	else
	{
		DataLog (log_level_sim_error) << "NULL POINTER" << endmsg;
		return 0;
	}
}

int SimMsgSys::dispatchMessages()
{
	if (_msgSystem) return _msgSystem->dispatchMessages();
	else
	{
		DataLog (log_level_sim_error) << "NULL POINTER" << endmsg;
		return 0;
	}
}


