// FILENAME: main.cpp
// $Header: J:/BCT_Development/Trima/engr_tools/std/rcs/main.cpp 1.5 2000/03/09 21:52:26 TD07711 Exp hr10414 $
// PURPOSE: main for std (simple test driver)
// USAGE: std [options]
//   -exedir <pathname> - execution space
// CHANGLOG:
//   $Log: main.cpp $
//   Revision 1.6 2000/06/14 HR10414
//    Changed include file names to lowercase
//
//   Revision 1.5  2000/03/09 21:52:26  TD07711
//   IT4580
//   01/24/2000 - dyes - add -loop option
//   Revision 1.5  2000/01/13 21:22:55  TD07711
//   01/12/2000 - dyes
//   1. remove -nolock option, replace with -clean
//   2. add -skips option to show skipped tests
//   3. add -specs option to show test spec comments
//   4. add -check to just check datafile integrity
//   11/08/99 - dyes - initial version

#include <string.h>
#include <new.h>
#include <stdlib.h>

#include "platform.h"
#include "logger.hpp"
#include "optionparser.hpp"
#include "usage.hpp"
#include "suite.hpp"
#include "version.h"


// globals that user can specify via command line
char* Datafile; // filename containing test suite definition
char* Path_name; // path to suite directory where Datafile is located
bool Debug_flag = false;
bool Nocleanup_flag = false;
int Nesting_levels; // levels of suite/subsuite nesting allowed 
char* Test_name; // limits execution to this one test
bool Force_clean_flag = false; // true to force workspace cleanup
bool Show_specs_flag = false; // true to just show test specs
bool Show_skips_flag = false; // true to just show skipped tests
bool Check_flag = false; // set true to just check datafile integrity
int Loop_count = 1; // how many times to loop on tests


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


int main(int argc, char** argv)
{
    set_new_handler(outOfMemory);

    Usage usage("simple test driver");
    Logger::Init(argc, argv, &usage);

    LOG_INFO("version %s", Version);

    // parse command line
    OptionParser commandline(argc, argv, usage);
    commandline.parse("-check", "datafile integrity check, does not run test", &Check_flag);
    commandline.parse("-clean", "force workspace cleanup, does not run test", &Force_clean_flag);
    commandline.parse("-datafile", "datafile driving test suite execution", &Datafile, "std.dat");
    commandline.parse("-debug", "enables debug messages", &Debug_flag);
    commandline.parse("-loop", "number of times to loop on tests", &Loop_count, 1, 1, 1000000);
    commandline.parse("-noclean", "disables workspace cleanup", &Nocleanup_flag);
    commandline.parse("-nest", "levels of nesting supported", &Nesting_levels, 20, 1, 100);
    commandline.parse("-path", "location of suite to run", &Path_name, ".");
    commandline.parse("-skips", "show skipped tests, does not run test", &Show_skips_flag);
    commandline.parse("-specs", "show test specification, does not run test", &Show_specs_flag);
    commandline.parse("-test", "act only on the specified test", &Test_name, 0);
    commandline.done();

    if(Debug_flag)
        Logger::Enable_debug();

    LOG_DEBUG("std entry");

    // create test suite and set parameters
    Suite suite(Path_name, Nesting_levels);
    if(Test_name) suite.setSingleTest(Test_name);
    if(Nocleanup_flag) suite.setNoCleanup();


    // parse datafile
    suite.parseDataFile(Datafile);
    if(suite.check() != true)
    {
        return 1; // check() supplies error messages
    }

    // exit if we're only checking datafile integrity
    if(Check_flag)
    {
        return 0;
    }

    //
    // do the action on the suite
    //
    if(Force_clean_flag)
    {
        suite.workspaceCleanup();
        return 0;
    }

    if(Show_specs_flag)
    {
        suite.showSpecs();
        return 0;
    }

    if(Show_skips_flag)
    {
        suite.showSkips();
        suite.summary();
        return 0;
    }

    // default is to run tests
    int rval = 0;
    for(int i = 1; i <= Loop_count; i++)
    {
        if(suite.run() == false)
        {
            rval = 1; // set exit code for failure
        }
    }
    suite.summary();
    
    return rval;
}
