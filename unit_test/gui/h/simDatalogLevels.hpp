/*******************************************************************************
 *
 * Copyright (c) 2009 by CaridianBCT, Inc.  All rights reserved.
 *
 * TITLE:      simDataLogLevels.hpp
 *
 * AUTHOR:     Dan Slauson
 *
 * DESCRIPTION: This file gets includeded from the main trima_datalog_levels if
 *		software is compiled for SIMNT.  It provides several extra datalogs that
 *		can be used in simulator and screenshot software.
 *		Note: do not include this file directly.  It does not make sense out of
 *		a very specific context, so your code will not compile.
 *
 *******************************************************************************
 *
 * REVISION HISTORY:
 * $Log: simDataLogLevels.hpp $
 * Revision 1.3  2009/08/05 21:24:01Z  dslausb
 * Update for spiral 3.5 screenshots
 * Revision 1.2  2009/08/03 17:27:34Z  dslausb
 * Added a file header
 *
 ******************************************************************************/

#ifdef __SIM_LEVEL_DEFS__
// Log Level Defs
LOG_DEF DataLog_Level	log_level_sim_info;
LOG_DEF DataLog_Level	log_level_sim_error;
LOG_DEF DataLog_Level	log_level_sim_debug;
LOG_DEF DataLog_Level	log_level_sim_timing_info;
LOG_DEF DataLog_Level	log_level_sim_timing_error;
LOG_DEF DataLog_Level	log_level_sim_timing_debug;
LOG_DEF DataLog_Level	log_level_sim_screen_touch_info;
LOG_DEF DataLog_Level	log_level_sim_screen_touch_error;
LOG_DEF DataLog_Level	log_level_sim_screen_touch_debug;
LOG_DEF DataLog_Level	log_level_sim_responder_info;
LOG_DEF DataLog_Level	log_level_sim_responder_error;
LOG_DEF DataLog_Level	log_level_sim_responder_debug;
LOG_DEF DataLog_Level	log_level_sim_main_routine_info;
LOG_DEF DataLog_Level	log_level_sim_main_routine_error;
LOG_DEF DataLog_Level	log_level_sim_main_routine_debug;
LOG_DEF DataLog_Level	log_level_sim_utility_info;
LOG_DEF DataLog_Level	log_level_sim_utility_error;
LOG_DEF DataLog_Level	log_level_sim_utility_debug;
LOG_DEF DataLog_Level	log_level_sim_alarm_info;
LOG_DEF DataLog_Level	log_level_sim_alarm_error;
LOG_DEF DataLog_Level	log_level_sim_alarm_debug;
LOG_DEF DataLog_Level	log_level_sim_screenshot_info;
LOG_DEF DataLog_Level	log_level_sim_screenshot_error;
LOG_DEF DataLog_Level	log_level_sim_screenshot_debug;
LOG_DEF DataLog_Level	log_level_sim_mapper_info;
LOG_DEF DataLog_Level	log_level_sim_mapper_error;
LOG_DEF DataLog_Level	log_level_sim_mapper_debug;
LOG_DEF DataLog_Level	log_level_sim_gui_side_info;
LOG_DEF DataLog_Level	log_level_sim_gui_side_error;
LOG_DEF DataLog_Level	log_level_sim_gui_side_debug;

// Log Handle Defs
LOG_DEF DataLog_Handle	log_handle_sim_info;
LOG_DEF DataLog_Handle	log_handle_sim_error;
LOG_DEF DataLog_Handle	log_handle_sim_debug;
LOG_DEF DataLog_Handle	log_handle_sim_timing_info;
LOG_DEF DataLog_Handle	log_handle_sim_timing_error;
LOG_DEF DataLog_Handle	log_handle_sim_timing_debug;
LOG_DEF DataLog_Handle	log_handle_sim_screen_touch_info;
LOG_DEF DataLog_Handle	log_handle_sim_screen_touch_error;
LOG_DEF DataLog_Handle	log_handle_sim_screen_touch_debug;
LOG_DEF DataLog_Handle	log_handle_sim_responder_info;
LOG_DEF DataLog_Handle	log_handle_sim_responder_error;
LOG_DEF DataLog_Handle	log_handle_sim_responder_debug;
LOG_DEF DataLog_Handle	log_handle_sim_main_routine_info;
LOG_DEF DataLog_Handle	log_handle_sim_main_routine_error;
LOG_DEF DataLog_Handle	log_handle_sim_main_routine_debug;
LOG_DEF DataLog_Handle	log_handle_sim_utility_info;
LOG_DEF DataLog_Handle	log_handle_sim_utility_error;
LOG_DEF DataLog_Handle	log_handle_sim_utility_debug;
LOG_DEF DataLog_Handle	log_handle_sim_alarm_info;
LOG_DEF DataLog_Handle	log_handle_sim_alarm_error;
LOG_DEF DataLog_Handle	log_handle_sim_alarm_debug;
LOG_DEF DataLog_Handle	log_handle_sim_screenshot_info;
LOG_DEF DataLog_Handle	log_handle_sim_screenshot_error;
LOG_DEF DataLog_Handle	log_handle_sim_screenshot_debug;
LOG_DEF DataLog_Handle	log_handle_sim_mapper_info;
LOG_DEF DataLog_Handle	log_handle_sim_mapper_error;
LOG_DEF DataLog_Handle	log_handle_sim_mapper_debug;
LOG_DEF DataLog_Handle	log_handle_sim_gui_side_info;
LOG_DEF DataLog_Handle	log_handle_sim_gui_side_error;
LOG_DEF DataLog_Handle	log_handle_sim_gui_side_debug;

#endif // #ifdef __SIM_LEVEL_DEFS__

#ifdef __SIM_LEVEL_TABLE__
, // Comma needed for syntax
// Log table entries
{ &log_level_sim_info,					&log_handle_sim_info,				"sim_info",					true },
{ &log_level_sim_error,					&log_handle_sim_error,				"sim_error",				true },
{ &log_level_sim_debug,					&log_handle_sim_debug,				"sim_debug",				false },
{ &log_level_sim_timing_info,			&log_handle_sim_timing_info,		"sim_timing_info",			true },
{ &log_level_sim_timing_error,			&log_handle_sim_timing_error,		"sim_timing_error",			true },
{ &log_level_sim_timing_debug,			&log_handle_sim_timing_debug,		"sim_timing_debug",			false },
{ &log_level_sim_screen_touch_info,		&log_handle_sim_screen_touch_info,	"sim_screen_touch_info",	true },
{ &log_level_sim_screen_touch_error,	&log_handle_sim_screen_touch_error,	"sim_screen_touch_error",	true },
{ &log_level_sim_screen_touch_debug,	&log_handle_sim_screen_touch_debug, "sim_screen_touch_debug",	false },
{ &log_level_sim_responder_info,		&log_handle_sim_responder_info,		"sim_responder_info",		true },
{ &log_level_sim_responder_error,		&log_handle_sim_responder_error,	"sim_responder_error",		true },
{ &log_level_sim_responder_debug,		&log_handle_sim_responder_debug,	"sim_responder_debug",		false },
{ &log_level_sim_main_routine_info,		&log_handle_sim_main_routine_info,	"sim_main_routine_info",	true },
{ &log_level_sim_main_routine_error,	&log_handle_sim_main_routine_error,	"sim_main_routine_error",	true },
{ &log_level_sim_main_routine_debug,	&log_handle_sim_main_routine_debug,	"sim_main_routine_debug",	false },
{ &log_level_sim_utility_info,			&log_handle_sim_utility_info,		"sim_utility_info",			true },
{ &log_level_sim_utility_error,			&log_handle_sim_utility_error,		"sim_utility_error",		true },
{ &log_level_sim_utility_debug,			&log_handle_sim_utility_debug,		"sim_utility_debug",		false },
{ &log_level_sim_alarm_info,			&log_handle_sim_alarm_info,			"sim_alarm_info",			true },
{ &log_level_sim_alarm_error,			&log_handle_sim_alarm_error,		"sim_alarm_error",			true },
{ &log_level_sim_alarm_debug,			&log_handle_sim_alarm_debug,		"sim_alarm_debug",			false },
{ &log_level_sim_screenshot_info,		&log_handle_sim_screenshot_info,	"sim_screenshot_info",		true },
{ &log_level_sim_screenshot_error,		&log_handle_sim_screenshot_error,	"sim_screenshot_error",		true },
{ &log_level_sim_screenshot_debug,		&log_handle_sim_screenshot_debug,	"sim_screenshot_debug",		false },
{ &log_level_sim_mapper_info,			&log_handle_sim_mapper_info,		"sim_mapper_info",			true },
{ &log_level_sim_mapper_error,			&log_handle_sim_mapper_error,		"sim_mapper_error",			true },
{ &log_level_sim_mapper_debug,			&log_handle_sim_mapper_debug,		"sim_mapper_debug",			false },
{ &log_level_sim_gui_side_info,			&log_handle_sim_gui_side_info,		"sim_gui_side_info",		true },
{ &log_level_sim_gui_side_error,		&log_handle_sim_gui_side_error,		"sim_gui_side_error",		true },
{ &log_level_sim_gui_side_debug,		&log_handle_sim_gui_side_debug,		"sim_gui_side_debug",		false },

#endif // #ifdef __SIM_LEVEL_TABLE__


