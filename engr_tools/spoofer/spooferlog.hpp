/* Copyright 1998 Cobe BCT, Inc.
 *
 * FILENAME: spooferlog.hpp
 * $Header: //depot/main/embedded/Trima/Main/engr_tools/spoofer/spooferlog.hpp#1 $
 * PURPOSE: Logging functions.
 * CHANGELOG:
 *   $Log: $
 */

#ifndef SPOOFERLOG_H
#define SPOOFERLOG_H

#include "datalog_levels.h"
#include "trima_datalog_levels.h"

#define LOG_INFO(__FORMAT__, ...) \
		fprintf(stdout, "%s(%d): ", __FILE__, __LINE__); \
		fprintf(stdout, __FORMAT__, ##__VA_ARGS__); \
		fprintf(stdout, "\n"); \
		datalog_Print(log_handle_spoofer_info, __FILE__, __LINE__, __FORMAT__, ##__VA_ARGS__)
#define LOG_ERROR(__FORMAT__, ...) \
		fprintf(stdout, "ERROR %s(%d): ", __FILE__, __LINE__); \
		fprintf(stdout, __FORMAT__, ##__VA_ARGS__); \
		fprintf(stdout, "\n"); \
		datalog_Print(log_handle_spoofer_error, __FILE__, __LINE__, __FORMAT__, ##__VA_ARGS__)
#define LOG_WARNING(__FORMAT__, ...) \
		fprintf(stdout, "WARNING %s(%d): ", __FILE__, __LINE__); \
		fprintf(stdout, __FORMAT__, ##__VA_ARGS__); \
		fprintf(stdout, "\n"); \
		datalog_Print(log_handle_spoofer_warning, __FILE__, __LINE__, __FORMAT__, ##__VA_ARGS__)
#define LOG_DEBUG(__FORMAT__, ...) datalog_Print(log_handle_spoofer_debug, __FILE__, __LINE__, __FORMAT__, ##__VA_ARGS__)

#define ASSERT(s) if (!(s))  { datalog_Print(log_handle_spoofer_error, __FILE__, __LINE__, "ASSERT FAILED: %s", #s); }

#endif // SPOOFERLOG_H
