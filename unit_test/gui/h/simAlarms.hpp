/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simAlarms.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This class encapsulates the routines that are used to grab
 *	alarm screenshots.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simAlarms.hpp $
 * Revision 1.1  2009/06/11 15:13:04Z  dslausb
 * Initial revision
 * Revision 1.2  2009/06/11 15:07:42Z  dslausb
 * Update file headers
 *
 ******************************************************************************/

#ifndef __SIM_ALARMS_HPP__
#define __SIM_ALARMS_HPP__

#include <vxworks.h>

#if CPU == SIMNT
#include "gui.h"
#include "string_id.h"
#include "datalog.h"
#include "trimamessages.h"
#include "states.h"
#include "alarms.h"
#include "runinfo.h"

class SimAlarms
{
public:

	struct alarm_state_mapping
	{
	  State_names state;
	  ALARM_VALUES alarms[256];
	};

protected:

    static alarm_state_mapping alarm_state_mappings[];

public:

    static void sendAlarm(ALARM_VALUES a_name, ALARM_TYPES a_type = R1, ALARM_STATES a_state = ACTIVE);
	static void clearAlarm();
	static void walkThroughControlAlarms();
	static void walkThroughSafetyAlarms();
	static void alarmRoutine();
};

#endif /* if CPU==SIMNT */
#endif // #ifndef __SIM_ALARMS_HPP__


