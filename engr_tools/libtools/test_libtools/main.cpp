// FILENAME: main.cpp
// $Header: I:/BCT_Development/vxWorks/Trima/engr_tools/libtools/test_libtools/rcs/main.cpp 1.1 2002/05/01 23:11:54 ms10234 Exp td07711 $
// PURPOSE: main for test_libtools
// USAGE: test_libtools [options]
//   -int_arg <n> - range -2 to +5
//   -debug - enable debug messages
// CHANGLOG:
//   $Log: main.cpp $
//   05/07/2002 - dyes - replace main() with test_libtools()
//   Revision 1.1  2002/05/01 18:33:24  td07711
//   Initial revision
//   04/30/2002 - dyes - initial version

#include <string.h>
#include <new.h>
#include <stdlib.h>

#include "platform.h"
#include "logger.hpp"
#include "optionparser.hpp"
#include "usage.hpp"
#include "version.h"


// globals that user can specify via command line
bool Debug_flag = false;
int ArgInt = 999; // how many times to loop on tests


#ifdef BUILD_FOR_NT
int outOfMemory(size_t)
{
    fprintf(stderr, "new failed, out of memory\n");
    _flushall();
    abort();
    return 0;
}
#else
void outOfMemory()
{
    fprintf(stderr, "new failed, out of memory\n");
    abort();
}
#endif

int test_libtools(char* options);

int test_libtools(char* options)
{
    set_new_handler(outOfMemory);

    Usage usage("test for libtools"); // initial usage
    Logger::Init("test_libtools", &usage);

    LOG_INFO("running");

    // parse command line
    OptionParser commandline(options, usage);
    commandline.parse("-debug", "enables debug messages", &Debug_flag);
    commandline.parse("-int_arg", "use to exercise integer argument parsing", &ArgInt, -2, 5, 0);
    commandline.done();

    if(Debug_flag)
        Logger::Enable_debug();

    LOG_DEBUG("test_libtools args parsed");
    LOG_INFO("-arg_int value = %d", ArgInt);

    LOG_INFO("done");
    return 0;
}
