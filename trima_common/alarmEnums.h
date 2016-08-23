/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      alarmEnums.h
 *
 * ABSTRACT:   This file represents most of the enums and structs needed by
 *		alarms.h. It was moved out of that file to make that file easier to
 *		parse with sed for the alarm table generator tool.
 *
 ******************************************************************************/

/**********************************************************************************
* NOTE: This file is now shared with Vista. Take care when adding new dependencies
**********************************************************************************/

#ifndef _ALARM_ENUMS_H_
#define _ALARM_ENUMS_H_

#define NUM_SAFETY_ALARMS  (NULL_ALARM - FIRST_SAFETY_ALARM - 1)     // be careful of the hack here !

enum ALARM_STATES
{
   LATCHED,
   ACTIVE,
   CLEARED
};

//
// The order of Highest priority alarm type to lowest must be maintained for
//  the alarm code to work...
enum ALARM_TYPES
{
   A2=0,
   A1,
   R2,
   R1,
   OPERATOR_ATTENTION
};

enum ALARM_DISPLAY_TYPES
{
   DISP_A2=0,
   DISP_A1,
   DISP_R2,
   DISP_FLOATING_HIGHPRI,
   DISP_FLOATING,
   DISP_R1,
   DISP_OPERATOR,
   DISP_CFR11,
   DISP_STATUS_LINE,
   DISP_NONE
};

enum ALARM_ACTIONS
{
   STOP_ALL=0,
   SLOW_CENTRIFUGE,
   STOP_PUMPS,
   STOP_PUMPS_CLOSE_RBC,
   STOP_DONOR_ACCESS,
   HALVE_PUMP_SPEEDS,
   STOP_MSS_RBC,
   STOP_MSS_PLT,
   NOTHING
};

enum ALARM_RESPONSES
{
   ABORT,
   ABORT_THRU_RINSEBACK,
   CONTINUE,
   CONFIRM,
   UNLOAD
};

//
// Any internal alarm should be generated in this form.
//
struct Alarm_struct
{
   ALARM_VALUES alarm_name;
   ALARM_STATES alarm_state;
};

//
// GUI response should be of this format...
//
struct Alarm_response_struct
{
   ALARM_VALUES    alarm_name;
   ALARM_RESPONSES alarm_response;
};

//
// Alarm messages to GUI should be of this format.
//
struct GUI_Alarm_struct
{
   ALARM_VALUES alarm_name;
   ALARM_TYPES  alarm_type;
   ALARM_STATES alarm_state;
};

//
// Definition of the format of a alarm sturcture for allowing the
// enumerated values to be matched to a text string.
//
struct the_alarm_struct
{
   ALARM_VALUES alarm_name;
   char*        alarm_text;
};

#ifndef WIN32
// Stream manipulator that prints out alarm names.
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_VALUES id);
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_STATES id);
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_TYPES id);
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_DISPLAY_TYPES id);
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_ACTIONS id);
DataLog_Stream& operator << (DataLog_Stream& os, ALARM_RESPONSES id);
#endif


#endif // #define _ALARM_ENUMS_H_

/* FORMAT HASH d3201b60c7b23397d67d70ced90501cb */
