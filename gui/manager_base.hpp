/********************************************************************************
 *
 *  Manager Base:
 *
 *  Common code for our communication managers (Adjust Manager, Predict Manager,
 *  and possibly others not yet conceived).
 *
 *  These functions should probably not be used by anything other than a manager.
 *
 ********************************************************************************/


#ifndef MONITOR_BASE_INCLUDE
#define MONITOR_BASE_INCLUDE

#include <sstream>
#include "trima_datalog_levels.h"

// This macro has proven to be surprisingly useful for making good logging
// output.
#define HERE __LINE__, __FUNCTION__, __FILE__

namespace manager_detail
{

// For managers, this turns the "Optimizaion in progress" on and off (send
// 'true' for on, 'false' for off).
void update_status_line (bool);

// Helper function for unknown cases
void FatalAlarm (stringstream& buf, const char* msg);

inline DataLog_Level& interesting (bool i)
{
   return i ? log_level_gui_info : log_level_gui_debug;
}



} // namespace

#endif

/* FORMAT HASH ded1a1c379af813967b3ba30cd63e0a2 */
