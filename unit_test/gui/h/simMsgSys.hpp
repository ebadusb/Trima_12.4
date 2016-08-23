/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simMsgSys.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This object wraps the message system in such a way that there
 *	is never more than one message system created or initialized at a time.
 *	When this object goes out of scope, the message system is automatically
 *	destroyed.  Create one of these in any situation where you may or may not
 *	already have a message system in place
 *
 *	For example, a function that may be called from the command line or from
 *	the screenshot routine would declare one of these.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simMsgSys.hpp $
 * Revision 1.2  2009/06/11 15:07:44Z  dslausb
 * Update file headers
 *
 ******************************************************************************/

#ifndef __SIM_MSG_SYS_HPP__
#define __SIM_MSG_SYS_HPP__

#include "messagesystem.h"

class SimMsgSys
{
private:
	MessageSystem* _msgSystem;

public:
	SimMsgSys();
	virtual ~SimMsgSys();
	
	virtual int receivedSignal();
	virtual int dispatchMessages();
};

#endif

