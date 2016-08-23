/*
Purpose: Provide some logging functionality needed by our script logging mechanism,
         like timestamps.
*/
#include "test_logger.hpp"
#include "datalog_levels.h"
#include "trima_datalog_levels.h"
#include <time.h>

void TestLogger::Log_Test_Info(char* format,...)
{
    // Parse out args and format string into a new buffer.
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);

    char  timestamp[9];
    time_t currenttime = time( NULL );
    struct tm local_tm;

    localtime_r( &currenttime, &local_tm );
    strftime( timestamp, 10, "%H:%M:%S", &local_tm );
    sprintf(_Log_buf, "<test_data>%.10s - %.1900s</test_data>", timestamp, _Va_buf);
	DataLog( log_level_spoofer_info ) << _Log_buf << endmsg;
    Log_buf();
}

void TestLogger::Log_Verify(char* format,...)
{
    // Parse out args and format string into a new buffer.
    va_list arglist;
    va_start(arglist, format);
    vsprintf(_Va_buf, format, arglist);
    va_end(arglist);

    sprintf(_Log_buf, "<verify>%.1900s</verify>", _Va_buf);
	DataLog( log_level_spoofer_info ) << _Log_buf << endmsg;
    Log_buf();
}
